//****************************************************************
//
//  NAME
//    ACAMS_StatisticsFile.C
//
//  COPYRIGHT
//    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein 
//    is the property of Ericsson Utvecklings AB, Sweden.
//    The program(s) may be used and/or copied only with 
//    the written permission from Ericsson Utvecklings AB or in 
//    accordance with the terms and conditions stipulated in the 
//    agreement/contract under which the program(s) have been 
//    supplied.

//  DOCUMENT NO
//    CAA 109 0313

//  AUTHOR 
//    EAB/UZ/DG Carl Johannesson

//  DESCRIPTION
//    This class handles a file which contains statistical information
//    about a Message Store.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO     DATE        NAME       DESCRIPTION
//    A          2000-05-10  uabcajn    product release
//    B          2004-03-12  uabcajn    rogue wave replaced by STL.
//    C          2007-01-15  qnicmut    Blade Cluster adaptations
//****************************************************************

#include <time.h>

#include "acs_aca_utility.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_ms_statistics_file.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMS_StatisticsFile);


//****************************************************************
// Constants
//****************************************************************
const int StatisticsFileSize = 128;
const int SizeOfStatFileMap = 28;

//****************************************************************
// Constructor
//****************************************************************

ACAMS_StatisticsFile::ACAMS_StatisticsFile(const std::string & fullPathOfFileName, bool writePermission)
	: stat_hand(ACE_INVALID_HANDLE), myView(0) {
	ACS_ACA_TRACE_FUNCTION;

	ACE_INT32 nobr = 0;
	ACE_INT32 nobw;

	// This memory area is used for initializing the disk-file and
	//also for storage of file data when the memory-mapping fails.
	memset(dummyBuffer, 0, StatisticsFileSize);

	// Open the disk-file
	if (writePermission) // Used by the MSD process. Create if it doesn't exist
		stat_hand = ACE_OS::open(fullPathOfFileName.c_str(), /*O_WRONLY |*/ O_RDWR|O_CREAT,ACE_DEFAULT_OPEN_PERMS);
	else	// Used by msdls, file should already exist.
		stat_hand = ACE_OS::open(fullPathOfFileName.c_str(), O_RDWR,ACE_DEFAULT_OPEN_PERMS);

	if ((stat_hand == ACE_INVALID_HANDLE) || (stat_hand < 3)) {
		// We failed to open the disk-file
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to open stat file %s", fullPathOfFileName.c_str());
		this->initMembers(writePermission);
		return;
	}

	// Open OK. Try to read the file
	nobr = ACE_OS::read(stat_hand, dummyBuffer, StatisticsFileSize); 

	if (nobr < 0) {
		//We failed to read
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read stat file %s", fullPathOfFileName.c_str());
		ACE_OS::close(stat_hand);
		this->initMembers(writePermission);
		return;
	}
	else {
		if ((nobr == 0) || (nobr == 28)) {
			//size==28, old format
			// The file is empty (we just created it), it should be initialized
			// Or old format of file, extend its size.
			ACE_OS::lseek(stat_hand, 0,SEEK_END);
			memset(dummyBuffer, 0, StatisticsFileSize);
			nobw = ACE_OS::write(stat_hand, dummyBuffer, StatisticsFileSize - nobr);

			if ((nobw < 0) || (nobw < (StatisticsFileSize - nobr))) {
				// Write failed
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to write stat file %s", fullPathOfFileName.c_str());
				ACE_OS::close(stat_hand);
				this->initMembers(writePermission);
				return;
			}
		}
	}

	// Position at start of file
	if (ACE_OS::lseek(stat_hand, 0, SEEK_SET) == -1) {
		// Write failed
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to set stat file pointer for file %s", fullPathOfFileName.c_str());
		ACE_OS::close(stat_hand);
		this->initMembers(writePermission);
		return;
	}

	// Now map the file in memory
	if (writePermission)
		myView = ACE_OS::mmap(0,StatisticsFileSize,/*PROT_READ|*/PROT_WRITE,MAP_SHARED, stat_hand, 0);
	else
		myView = ACE_OS::mmap(0,StatisticsFileSize,PROT_READ,MAP_SHARED,stat_hand, 0);

	if (! myView) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to create view to stat file %s", fullPathOfFileName.c_str());
	}

	// Everything OK. Now update member data (pointers into the file map)
	if (nobr)
		this->initMembers(writePermission, false);
	else
		this->initMembers(writePermission, true);
}

//****************************************************************
// Destructor
//****************************************************************
ACAMS_StatisticsFile::~ACAMS_StatisticsFile()
{
	ACS_ACA_TRACE_FUNCTION;
	if (myView)
		ACE_OS::munmap(myView,StatisticsFileSize);

	if (stat_hand != ACE_INVALID_HANDLE)
		ACE_OS::close(stat_hand);
}

//****************************************************************
// Get file creation time
//****************************************************************
struct tm  ACAMS_StatisticsFile::getCreationTime() const {
	ACS_ACA_TRACE_FUNCTION;
	return *fileCreationTime;
}

//****************************************************************
// Get total number of messages stored in the message store since 
// creation time.
//****************************************************************
ULONGLONG ACAMS_StatisticsFile::getNumberOfMsg() const {
	ACS_ACA_TRACE_FUNCTION;
	return convertToUll(reinterpret_cast<unsigned char *>(myNumberOfMsg));
}

//****************************************************************
// Get total number of bytes stored in the message store since 
// creation time.
//****************************************************************
ULONGLONG ACAMS_StatisticsFile::getNumberOfBytes() const {
	ACS_ACA_TRACE_FUNCTION;
	return convertToUll(reinterpret_cast<unsigned char *>(myNumberOfBytes));
}

//****************************************************************
// Get number of messages lost since creation time
//****************************************************************
int ACAMS_StatisticsFile::getNumberOfLostMsg() const {
	ACS_ACA_TRACE_FUNCTION;
	return *myNumberOfLostMsg;
}

//****************************************************************
// Get number of messages skipped since creation time
//****************************************************************
int ACAMS_StatisticsFile::getNumberOfSkippedMsg() const {
	ACS_ACA_TRACE_FUNCTION;
	return *myNumberOfSkippedMsg;
}

//****************************************************************
// Update all counters
//****************************************************************
void ACAMS_StatisticsFile::incrementCounters(int numberOfMsg, int numberOfBytes, int numberOfLostMsg, int numberOfSkippedMsg) {
	ACS_ACA_TRACE_FUNCTION;
	ULONGLONG tmp = convertToUll(reinterpret_cast<unsigned char *>(myNumberOfMsg));
	tmp += numberOfMsg;
	setBufferFromUll(tmp, reinterpret_cast<unsigned char *>(myNumberOfMsg));
	
	tmp = convertToUll(reinterpret_cast<unsigned char *>(myNumberOfBytes));
	tmp += numberOfBytes;
	setBufferFromUll(tmp, reinterpret_cast<unsigned char *>(myNumberOfBytes));

	*myNumberOfLostMsg += numberOfLostMsg;
	*myNumberOfSkippedMsg += numberOfSkippedMsg;
}

//****************************************************************
// Store current counter values to disk file. Don't wait for the 
// write to complete.
//****************************************************************
bool ACAMS_StatisticsFile::syncToDisk() {
	ACS_ACA_TRACE_FUNCTION;
	if (myView ) {
		if (!msync(myView,StatisticsFileSize,MS_SYNC))
			return false;
	}
	return true;
}

//****************************************************************
//                   Private help-functions
//****************************************************************

//****************************************************************
// Initialise the pointers into the memory mapped region, and set 
// 'myFileMap' to the start of it.
//****************************************************************
void ACAMS_StatisticsFile::initMembers(bool writingAllowed, bool fileCreated) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char * currentPos;
	time_t currentTime;
	::localtime(&currentTime);
	time_t now = time(NULL);
	struct tm * loTime = ::localtime(&now);
	loTime->tm_year=loTime->tm_year + 1900;
	loTime->tm_mon =loTime->tm_mon+1;
	 
	if (myView) { // filemap
		currentPos = (unsigned char *)myView;	
		if (writingAllowed && fileCreated)
			memcpy(currentPos, loTime, sizeof(tm));
	}
	else { // buffer
		currentPos = (unsigned char *)dummyBuffer;
		if (writingAllowed)
			memcpy(dummyBuffer, loTime, sizeof(tm));
	}

	// First there is creation time
	fileCreationTime = (tm *)currentPos;
	currentPos += sizeof(tm);

	// Number of messages (LongLong)
	myNumberOfMsg = reinterpret_cast<ULONGLONG *>(currentPos);
	currentPos += sizeof(ULONGLONG);

	// Number of bytes (LongLong)
	myNumberOfBytes = reinterpret_cast<ULONGLONG *>(currentPos);
	currentPos += sizeof(ULONGLONG);

	// Number of lost messages (int)
	myNumberOfLostMsg = (int *)currentPos;
	currentPos += sizeof(int);

	// Number of skipped messages (int)
	myNumberOfSkippedMsg = (int *)currentPos;
}
