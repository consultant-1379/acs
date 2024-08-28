//******************************************************************************
//
//  NAME
//     acs_aca_msgfile.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-10-15 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include "acs_aca_utility.h"
#include "acs_aca_logger.h"
#include "acs_aca_ms_const_values.h"
#include "acs_aca_msgfile.h"

//****************************************************************
// Try to read the last message in the file.
// An unlinked file has an EndOfFile mark at the very end.
// A non-corrupted still in use file has an EndOfMessage mark at 
// the end.
// Returns: -1 if file no longer exists; 
//           0 if corrupted; 
//           1 if OK.
//****************************************************************
int MsgFile::readLastMsg () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	unsigned char * dataPtr = 0;
	ULONGLONG SizeDataFile = 0;
	struct stat buf;
	void * file_view = 0;
	size_t file_map_size = 0;
	int result = 0;

	unsigned char buffer[ACAMS_Eof_Mark_Length];
	ACE_UINT32 offset = 0;
	ACE_HANDLE file_hand = ACE_INVALID_HANDLE;

	file_hand = ACE_OS::open(name.c_str(), O_RDONLY, ACE_DEFAULT_OPEN_PERMS);
	if (file_hand == ACE_INVALID_HANDLE) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the message file %s, errno = %d, leaving",
				name.c_str(), ACE_OS::last_error());
		return -1;
	}

	//For testing purpose remove later
	int po = ACE_OS::lseek(file_hand, 0, SEEK_END);
	po = ACE_OS::lseek(file_hand, -ACAMS_Eof_Mark_Length, SEEK_END);

	ACE_INT32 nobr = 0;
	nobr = ACE_OS::read(file_hand, buffer, ACAMS_Eof_Mark_Length);
	if ((nobr < 0)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read the message file %s, errno = %d, leaving",
				name.c_str(), ACE_OS::last_error());
		ACE_OS::close(file_hand);
		return 0;
	}

	if (::memcmp(buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0) {
		// It was a good EOF.
		eofFound = true;

		// Position file pointer to offset to start of last message.
		po = ACE_OS::lseek(file_hand, -(ACAMS_Eof_Mark_Length + 8), SEEK_CUR);
	} else if (::memcmp((buffer + 8), ACAMS_Msg_End_Mark, 4) == 0) {
		// It was an end-of-message mark.
		eomFound = true;

		// Position file pointer to offset to start of last message.
		ACE_OS::lseek(file_hand, -8, SEEK_CUR);
	} else {
		// Can't recognize were we are in the file. Possibly corrupted.
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to understand the position in the file %s,"
				" probably it's corrupted, leaving",	name.c_str());
		ACE_OS::close(file_hand);
		return 0; // Failed.
	}

	result = ::stat(name.c_str(), &buf);

	if (result != 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'stat' failed for the file %s, errno = %d, leaving",	name.c_str(), ACE_OS::last_error());
		ACE_OS::close(file_hand);
		return false;
	} else {
		SizeDataFile = file_map_size = buf.st_size;
	}

	ACE_HANDLE dfile_hand = ACE_OS::open(name.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
	if (dfile_hand == ACE_INVALID_HANDLE) { //filename not ok
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the message file %s, errno = %d, leaving",
						name.c_str(), ACE_OS::last_error());
		ACE_OS::close(file_hand);
		return false;
	}

	if ((file_view = ::mmap(0, buf.st_size, PROT_READ, MAP_SHARED, dfile_hand, 0)) == MAP_FAILED) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to map in memory the data file %s, errno = %d, leaving",
						name.c_str(), ACE_OS::last_error());
		ACE_OS::close(dfile_hand);
		ACE_OS::close(file_hand);
		return false;
	}

	// Read offset to beginning of last message and position to that point.
	nobr = ACE_OS::read(file_hand, &offset, 4);
	if ((nobr < 0)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read the offset to beginning of last message "
				"in the data file %s, errno = %d, leaving",	name.c_str(), ACE_OS::last_error());

		::munmap(file_view, file_map_size);
		ACE_OS::close(dfile_hand);
		ACE_OS::close(file_hand);
		return 0;
	}

	if (offset > 0xFFFF) { //version 3
		int leastsigbyte = (offset & 0xFF000000) >> 24;
		int middlebyte = (offset & 0x00FF0000) >> 8;
		offset = leastsigbyte + middlebyte;
	}

	dataPtr = reinterpret_cast<unsigned char *>(file_view);
	ULONGLONG lastm = 0;

	if (!eomFound) {
	 lastm = convertToUll((dataPtr + (SizeDataFile - (12 + 8 + offset))));
	} else
		lastm = convertToUll((dataPtr + (SizeDataFile - (8 + offset))));
	
	lastMsgNum = lastm;
	noOfMsgs = (static_cast<int>(lastMsgNum - firstMsgNum)) + 1;

	::munmap(file_view, file_map_size);
	ACE_OS::close(dfile_hand);
	ACE_OS::close(file_hand);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 1;
}
