//****************************************************************
//
//  NAME 
//    ACAMS_CommitFile.cpp
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
//    This class handles a file which contains information about last
//    transaction commited to a Message store.
//    The file stores the following information:
//     - Message Number of last commited message
//     - Name of file where that message is stored
//     - Application-specific data belonging to the transaction.

//  CHANGES
//    RELEASE REVISION HISTORY
//    DATE          NAME       DESCRIPTION
//    2005-09-21    uabcajn    CNI 399
//    repairFile and validDataFile methods added.
//    2007-01-15    qnicmut    Blade Cluster adaptations
//****************************************************************

#include <unistd.h>
#include <time.h>
#include <inttypes.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <new>
#include <map>
#include <list>

#include "acs_aca_utility.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_ms_commit_file.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMS_CommitFile)


using namespace std;
bool findFirstFile(const string& sStartDir, std::string& fileFound, std::string compareWith);

//****************************************************************
// Constructor. 
// Open the CommitFile given as argument. 
// If 'createFile' is true, it will be created and initialised to 
// its maximum size if it doesn't already exist.
// If 'createFile' is false, the file must exist and is opened for 
// unbuffered write.
// Once the file is open, data from last commit (message number, 
// name of data file, and transaction data) is read from it.
//****************************************************************
ACAMS_CommitFile::ACAMS_CommitFile(const std::string & fullPathOfCommitFileName, bool createFile)
	: hand_commitf(ACE_INVALID_HANDLE) {
	ACS_ACA_TRACE_FUNCTION;

	// Clear the I/O-buffer
	memset(myBuffer, 0, ACAMS_MaxSizeOfCommitData);

	// Open the CommitFile
	if (createFile) {

		// Used by the MSD process. Create it if it doesn't exist.
		if (hand_commitf == ACE_INVALID_HANDLE)
			hand_commitf = ACE_OS::open(fullPathOfCommitFileName.c_str(), O_RDWR | O_SYNC, ACE_DEFAULT_OPEN_PERMS);
		else
			ACS_ACA_TRACE_MESSAGE("Failed in opening file: %s (with handle %d)", fullPathOfCommitFileName.c_str(), hand_commitf);

		if ( hand_commitf == ACE_INVALID_HANDLE ) {
			// the commit file does not exist, so create it.
			if (hand_commitf == ACE_INVALID_HANDLE)
				hand_commitf = ACE_OS::open(fullPathOfCommitFileName.c_str(), O_RDWR | O_CREAT | O_SYNC, ACE_DEFAULT_OPEN_PERMS);
			else
				ACS_ACA_TRACE_MESSAGE("Failed in opening file: %s", fullPathOfCommitFileName.c_str());

			if ( hand_commitf == ACE_INVALID_HANDLE )	{
				// MSD process failed to create commit file.
				ACS_ACA_TRACE_MESSAGE("MSD process failed to create commit file %s", fullPathOfCommitFileName.c_str());
			}
			else {
				// We have just created the file. It should be expanded to its maximum size
				ssize_t size = 0;
				if (hand_commitf != ACE_INVALID_HANDLE)
					size = ACE_OS::write(hand_commitf, (const void *)myBuffer, ACAMS_MaxSizeOfCommitData);
				else
					ACS_ACA_TRACE_MESSAGE("Failed in writing to handle at commit file constructor");

				if ((size == -1) || (size < ACAMS_MaxSizeOfCommitData)) {	// Write failed. That is a disaster.
					if (size < ACAMS_MaxSizeOfCommitData) {
						ACS_ACA_TRACE_MESSAGE("ERROR: Call 'write' failed for commit file '%s', bytes_written = %zd",
								fullPathOfCommitFileName.c_str(), size);
						ACE_OS::ftruncate(hand_commitf, 0);
					}

					if (hand_commitf != ACE_INVALID_HANDLE)	{
						ACE_OS::close(hand_commitf);
						hand_commitf = ACE_INVALID_HANDLE;
						ACS_ACA_TRACE_MESSAGE("Expanding commitFile %s failed.", fullPathOfCommitFileName.c_str());
					}	
				}
				else {	// Write OK, set the file pointer to the beginning of the file
					if ((ACE_OS::lseek(hand_commitf, 0, SEEK_SET)) == -1)
						ACS_ACA_TRACE_MESSAGE("ERROR: Call 'lseek' for handle %d failed, errno == %d", hand_commitf, ACE_OS::last_error());
				}
			} // end of expansion of file
		} // end of creation of file
	} 
	else {
		// Used by the MSA application process, which writes to the file. The
		// file must already exist, and the writing shall be unbuffered.
		if(hand_commitf == ACE_INVALID_HANDLE) {
			ACS_ACA_TRACE_MESSAGE("My commit file handle is invalid, I'm going to open the commit file '%s'",
					fullPathOfCommitFileName.c_str());

			hand_commitf = ACE_OS::open(fullPathOfCommitFileName.c_str(), O_RDWR | O_SYNC, ACE_DEFAULT_OPEN_PERMS);
			int errno_save = ACE_OS::last_error();

			if ( hand_commitf == ACE_INVALID_HANDLE ) {
				ACS_ACA_TRACE_MESSAGE("Call 'open' for file %s failed!, errno is = %d", fullPathOfCommitFileName.c_str(), errno_save);
			}
		}
		else {
			ACS_ACA_TRACE_MESSAGE("ERROR: Failed to open file %s (handle == %d)", fullPathOfCommitFileName.c_str(), hand_commitf);
			throw std::runtime_error("client failed to open commitFile: " + fullPathOfCommitFileName);
		}

		if ( hand_commitf == ACE_INVALID_HANDLE ) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Failed to open file %s", fullPathOfCommitFileName.c_str());
			throw std::runtime_error("client failed to open commitFile: " + fullPathOfCommitFileName);
		}
	}

	// File now exists and the IO buffer should be loaded with its content.
	ssize_t size = 0;

	if ( hand_commitf != ACE_INVALID_HANDLE )
		size = ACE_OS::read(hand_commitf, (void *)myBuffer, ACAMS_MaxSizeOfCommitData);
	else {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'read' for handle %d failed!", hand_commitf);
		throw std::runtime_error("client failed to open commitFile: " + fullPathOfCommitFileName);
	}	

	if ( (size == -1) || (size != ACAMS_MaxSizeOfCommitData) ) {
		if ( hand_commitf != ACE_INVALID_HANDLE ) {
			ACE_OS::close(hand_commitf);
			hand_commitf = ACE_INVALID_HANDLE;
			ACS_ACA_TRACE_MESSAGE("Call 'read' failed, commit file closed, bytes read == %ld", size);
			throw std::runtime_error("client failed to read commitFile: " + fullPathOfCommitFileName);
		}
	}

	// The disk file contained data. Update member data accordingly		
	unsigned char * currentPos = myBuffer;
	// First in the file there is a message number
	myLastMsgNumber = convertToUll(myBuffer); 
	currentPos += sizeof(uint64_t);
	// Then there is the name of the data file where that message is stored
	myDataFileName = (char *) currentPos;
	currentPos += myDataFileName.length() + 1;

	// Size indicator for transaction data
	memcpy(&myTransactionDataLength, currentPos, sizeof(uint32_t));
	currentPos += (sizeof(uint32_t)  );

	// And last the transaction data
	myTransactionDataPtr = currentPos;

	ACS_ACA_TRACE_MESSAGE("trdata = %s", (char *) myTransactionDataPtr);
	// Save full path of commitFile to be used with events
	myName = fullPathOfCommitFileName;

	// Finally verify that the commit file isn't corrupted, i.e check
	// that the file really exists in the Message store, and if not, update
	// the commit file with the first Message number of the next file.
	if (!myDataFileName.empty() && !createFile)	{
		// only the client checks the consistency of the file.
		string chsDir = myName.substr(0, myName.find_last_of("/\\"));
		string dataFileDir = chsDir + '/' + ACAMS_NameOfDataFileDir;
		string dataFile = dataFileDir + '/' + myDataFileName;
		string foundfile;	
		bool foundFile = findFirstFile(dataFileDir,foundfile, myDataFileName);
		char * token = 0;
		unsigned long long int tmpUll = 0;
		char firstMsgNum[128] = {0};
		::strcpy(firstMsgNum, const_cast<char*>(myDataFileName.c_str()));
		token = ::strtok(firstMsgNum, "_");
		tmpUll = strtoull(token, NULL, 10);
		bool repairMsgNum = myLastMsgNumber < tmpUll;
		ACS_ACA_TRACE_MESSAGE("myLastMsgNumber is %llu, myDataFileName is %s", myLastMsgNumber, myDataFileName.c_str());

		if (repairMsgNum)	{
			myLastMsgNumber = tmpUll;
			ACS_ACA_TRACE_MESSAGE("Need to adjust myLastMsgNumber, new value is %llu", myLastMsgNumber);
		}

		if (!foundFile)	{
			// indicated commit file does not exist, i.e file is corrupted.
			if (!repairFile(chsDir, dataFileDir))	{
				if ( hand_commitf != ACE_INVALID_HANDLE ) {
					ACE_OS::close(hand_commitf);
					hand_commitf = ACE_INVALID_HANDLE;
					ACS_ACA_TRACE_MESSAGE("Repair of commit file %s failed!", fullPathOfCommitFileName.c_str());
				}
			}
		}
	}
}

// *****************************************************************************
// Destructor.	
// *****************************************************************************
ACAMS_CommitFile::~ACAMS_CommitFile() {
	ACS_ACA_TRACE_FUNCTION;
	if ( hand_commitf != ACE_INVALID_HANDLE ) {
		ACE_OS::close(hand_commitf);
		hand_commitf = ACE_INVALID_HANDLE;
	}
}

//**********************************************************************
// Write to the disk-file (commitFile). 
//
// Return values:
//  true: successful update.
//  false : unsuccessful update.
//**********************************************************************
bool ACAMS_CommitFile::commit(ULONGLONG commitMsgNum, const std::string & fileName, const unsigned char * transactionData, const uint32_t transactionDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char * currentPosition = myBuffer;
	bool ret = false;

	// Position at start of the file
	if (ACE_OS::lseek(hand_commitf, 0,SEEK_SET) == -1) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to set the file pointer to begin for file %s.", myName.c_str());
	}
	else {
		// Copy data to the I/O-buffer. First the message number.
		setBufferFromUll(commitMsgNum, currentPosition); //*reinterpret_cast<ULONGLONG *>(currentPosition) = commitMsgNum;
		currentPosition += sizeof(uint64_t);

		// Then the data file name.
		memcpy(myBuffer + sizeof(uint64_t), fileName.c_str(), fileName.length() + 1);
		currentPosition += fileName.length() + 1;

		// And last the transaction data. First a length indicator.
		memcpy(currentPosition, &transactionDataLength, sizeof(uint32_t));
		currentPosition += (sizeof(uint32_t));

		int totalSize = (currentPosition - myBuffer) + transactionDataLength;
	
		// And finally the transaction data itself.
		memcpy(currentPosition, transactionData,  transactionDataLength);

		// Now write the stuff to disk.
		ACE_INT32 nobw = 0;
		if (hand_commitf != ACE_INVALID_HANDLE)
			nobw = ACE_OS::write(hand_commitf, (const void *)myBuffer, totalSize);
		else
			ACS_ACA_TRACE_MESSAGE("ERROR: The handle is invalid!");

		if ((nobw < 0) || (nobw < totalSize)) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'write' failed for file %s, bytes written == %d!", myName.c_str(), nobw);

			if (nobw > 0) {
				ACS_ACA_TRACE_MESSAGE("ERROR: bytes written == %d, bytes_to_write == %d!", nobw, totalSize);
				ACE_OFF_T pos = ACE_OS::lseek(hand_commitf, -nobw, SEEK_END);
				ACE_OS::ftruncate(hand_commitf, pos);
			}
		}
		else {
			// And update member data.
			myLastMsgNumber = commitMsgNum;
			myDataFileName = fileName;
			myTransactionDataPtr = currentPosition;
			myTransactionDataLength = transactionDataLength;
			ret = true;
			ACS_ACA_TRACE_MESSAGE("Commit operation goes OK, returning true");
		}
	}
	
	return ret;
}

bool ACAMS_CommitFile::commit(ULONGLONG numOfMessages, ULONGLONG commitMsgNum, const std::string & fileName, const unsigned char * transactionData, const uint32_t transactionDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	ULONGLONG uBoundCommitNumber = myLastMsgNumber + numOfMessages;
	return commit((uBoundCommitNumber < commitMsgNum ? uBoundCommitNumber : commitMsgNum), fileName, transactionData, transactionDataLength);
}

//****************************************************************
// Get Message Number of last commited message.
//****************************************************************
ULONGLONG ACAMS_CommitFile::getMsgNumber() const {
	ACS_ACA_TRACE_FUNCTION;
	return myLastMsgNumber;
}

//****************************************************************
// Get file name of data file where last commited message is stored
//****************************************************************
const std::string ACAMS_CommitFile::getFileName() const {
	ACS_ACA_TRACE_FUNCTION;
	return myDataFileName;
}

//****************************************************************
// Get transaction data from last commit.
//****************************************************************
const unsigned char * ACAMS_CommitFile::getTransactionData() const {
	ACS_ACA_TRACE_FUNCTION;
	return myTransactionDataPtr;
}

//****************************************************************
// Get size of transaction data from last commit.
//****************************************************************
int ACAMS_CommitFile::getTransactionDataLength() const {
	ACS_ACA_TRACE_FUNCTION;
	return myTransactionDataLength;
}

// *****************************************************************************
// validDataFile
// Checks whether a file should be considered to be a data file or not.
// A file in the dataFiles directory which name contains an '_' is treated
// as a data file.
// Return values:
// true : message number part of the file name.
// false: no data file.
// *****************************************************************************
bool ACAMS_CommitFile::validDataFile(string FileName, ULONGLONG & msgNumber) {
	ACS_ACA_TRACE_FUNCTION;
	size_t sepIndex;
	
	if ((sepIndex = FileName.find('_')) != string::npos) {
		std::istringstream(FileName.substr(0, sepIndex)) >> msgNumber;
		ACS_ACA_TRACE_MESSAGE("The file is valid, returning %llu", msgNumber);
		return true;
	}

	return false;
}

// *****************************************************************************
// repairFile
// If the commit file was found to be corrupted, i.e the file points to a
// a non-existent data file, the file must be updated to point to the next
// data file (the file with the lowest message number, which is higher than
// the message number of the non-existent file).
// Return values:
// true : repair done.
// false: repair failed.
// *****************************************************************************
bool ACAMS_CommitFile::repairFile(string chsDir, string dataDir) {
	ACS_ACA_TRACE_FUNCTION;
	bool logFile = true;
	bool ret = true;
	ofstream of;

	// create log file name: <timestamp>.corruptlog
	time_t tt = ACE_OS::time(&tt);
	string fname = ACE_OS::ctime(&tt);

	for (std::string::iterator si = fname.begin(); si != fname.end(); ) {
		if (*si == ':')
			fname.erase(si++);
		else {
			if (*si == ' ')
				*si = '_';
			++si;
		}
	}
	fname.erase(fname.size() - 1, 5); // remove \n
	fname.append(".corruptlog");

	string absFname = chsDir + "/" + fname;

	of.open(absFname.c_str());
	if (!of.is_open()) {
		of.close();
		logFile = false;
	}

	unsigned char * trdata = new (std::nothrow) unsigned char[myTransactionDataLength + 1];
	if (trdata != 0) {
		strncpy((char *)trdata, (char *)myTransactionDataPtr, myTransactionDataLength);
		trdata[myTransactionDataLength] = '\0';
	}

	if (logFile) {
		of << "Commit file was found corrupted." << endl << endl;
		of << "BEFORE repair :" << endl;
		of << "---------------" << endl;
		of << "commit file contains:" << endl;
		of << "file :" << myDataFileName.data() << endl;
		of << "Msg no:" << myLastMsgNumber << endl;
		of << "transaction data:" << trdata << endl;
		of << "transaction data length:" << myTransactionDataLength << endl;
		of << endl << "dataFiles directory contains:" << endl;
	}

	dataDir.append("/");
	std::list<string> lstFiles;
	list_all_files( dataDir, lstFiles, true);
	std::list<ULONGLONG> msgNoList;
	map<ULONGLONG, string> m;
	string filename;
	if (lstFiles.size() > 0) {
		do
		{
			ULONGLONG msgNo = 0;
			if(validDataFile(lstFiles.front().c_str(), msgNo)) {
				if (logFile)
					of << lstFiles.front().c_str() << endl;
				m[msgNo] = lstFiles.front().c_str();
				msgNoList.push_back(msgNo);
			}
			lstFiles.pop_front();
		} while(lstFiles.size() > 0);
	}
	else {
		of << "no data files were found, there should be at least 1" << endl;
		ret = false;
	}

	// get the message number string for the non-existent data file.
	ULONGLONG mnoNonExistent = 0ULL;
	validDataFile(myDataFileName.data(), mnoNonExistent);
	ULONGLONG ref = mnoNonExistent;
	ULONGLONG newno = 0ULL;
	ULONGLONG tmp = 0ULL;
	ULONGLONG mnoStr = 0ULL;

	for (std::list<ULONGLONG>::iterator ii = msgNoList.begin(); ii != msgNoList.end(); ++ii) {
		tmp = *ii;
		if (newno > 0) {
			if ((tmp < newno) && (tmp > ref)) {
				newno = tmp;
				mnoStr = *ii;
			}
		}
		else if (tmp > ref) {
			newno = tmp;
			mnoStr = *ii;
		}
	}
	
	if (logFile) {
		of << endl << endl << "AFTER repair:" << endl;
		of << "-------------" << endl;
		of << "Commit file updated :" << endl;
		of << "file :" << m[mnoStr] << endl;
		of << "Msg no:" << mnoStr << endl;
		of << "transaction data:" << trdata << endl;		
		of << "transaction data length:" << myTransactionDataLength << endl;
	}

	string nFile = m[mnoStr];
	commit(newno, nFile, myTransactionDataPtr, myTransactionDataLength);

	delete[] trdata;
	trdata = 0;

	if (logFile)
		of.close();

	return ret;
}

void ACAMS_CommitFile::list_all_files(const string& sStartDir, std::list<string>& lstFound, bool bRecursive) {
	ACS_ACA_TRACE_FUNCTION;

	DIR* pDir = ACE_OS::opendir ( sStartDir.c_str());

	if (!pDir) return;

 	dirent* pEntry;
 	pEntry = ACE_OS::readdir (pDir);

	while (pEntry != 0) {
		if ( DT_DIR & pEntry->d_type && strcmp ( pEntry->d_name, ".") && strcmp ( pEntry->d_name, "..") && bRecursive) {
			string sSubDir = sStartDir + string ( "/") + string ( pEntry->d_name);
			list_all_files( sSubDir, lstFound,true);
		}

		string sFound = string ( pEntry->d_name);
		lstFound.push_back ( sFound);
		pEntry = ACE_OS::readdir (pDir);
	}

	ACE_OS::closedir(pDir);
}

bool ACAMS_CommitFile::findFirstFile(const string& sStartDir, std::string& fileFound, std::string compareWith = "") {
	ACS_ACA_TRACE_FUNCTION;

	DIR* pDir = ACE_OS::opendir ( sStartDir.c_str());

	if (!pDir)	return false;

	std::string search = compareWith;
	dirent* pEntry;
	pEntry = ACE_OS::readdir (pDir);

	while (pEntry != 0) {
		string sFound = string (pEntry->d_name);

		if(sFound.find(search) != string::npos) {
			ACS_ACA_TRACE_MESSAGE("Found file in %s with file %s", sStartDir.c_str(), sFound.c_str());
			fileFound = sFound;
			ACE_OS::closedir(pDir);
			return true;
		}

		pEntry = ACE_OS::readdir (pDir);
	}
	ACE_OS::closedir(pDir);

	return false;
}
