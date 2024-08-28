//****************************************************************
//
//  NAME
//    ACAMSD_MsgStore.C
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
//    CAA 109 0312

// AUTHOR
//   EAB/UZ/DG Carl Johannesson

//  DESCRIPTION
//    This class implements the writing part of Message Store.
//    It inherits from ACAMS_MsgStore.
//    Initially, the next msg to store is MsgNumber 1.

//  CHANGES
//    RELEASE REVISION HISTORY
//    DATE          NAME       DESCRIPTION
//    2005-09-27    uabcajn    CNI 399
//****************************************************************
#include <string>
#include <sstream>
#include <new>
#include <stdexcept>

#include "ace/ACE.h"
#include "ace/Event.h"

#include "acs_aeh_evreport.h"

#include "acs_aca_common.h"
#include "acs_aca_defs.h"
#include "acs_aca_msd_mtap_message.h"
#include "acs_aca_msd_mtap_channel_mgr.h"
#include "acs_aca_ms_commit_file.h"
#include "acs_aca_ms_statistics_file.h"
#include "acs_aca_msd_msg_collection.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_utility.h"
#include "acs_aca_logger.h"
#include "acs_aca_system_exception.h"
#include "acs_aca_msd_msg_store.h"

// Message Store Warning Alarm constants
namespace {
	const char * const PROCESS_NAME = "ACS_MSD_Server";
	const int SPECIFIC_PROBLEM = 9200;
	const char * const PROBABLE_CAUSE = "AP MESSAGE STORE WARNING";
	const char * const OBJ_CLASS_OF_REFERENCE = "APZ";
	const char * const PROBLEM_DATA = "The message store has been filled to pre-defined limit.\nProbable cause is that no application reads the messages in the store";
	const char * const PROBLEM_TEXT_FORMAT_SCP = "MESSAGE STORE   LIMIT%%\n%-16s%-d%%";
	const char * const PROBLEM_TEXT_FORMAT_MCP = "MESSAGE STORE   LIMIT%%\n%-16s%-d%%\n\nUse msdls command to find CPs hitting limit";
}

//****************************************************************
// Utility functions
//****************************************************************

static unsigned long long  stringToULL(const std::string & number) {
	unsigned long long result = 0ULL;
	std::istringstream(number) >> result;
	return result;
}

//****************************************************************
// Constructor and initializer
//****************************************************************
ACAMSD_MsgStore::ACAMSD_MsgStore (short index)
	: ACAMS_MsgStore(ACS_MSD_Service::getAddress(index)->name, ACS_MSD_Service::getAddress(index)->site, 0),
	  indextoParam(index), myAlarmStatus(false), myCurrentFile(ACE_INVALID_HANDLE), myCurrentFileName(),myNextMsgNum(1), myMsgColl(0),
	  myDiskSpaceExhausted(false), myNumberOfFiles(0), myFiles(), myStatisticsData(0), version_fptr(ACE_INVALID_HANDLE),
	  failedOldFiles(), _cpSystemId(INVALID_CPSYSTEMID) { }

ACAMSD_MsgStore::ACAMSD_MsgStore (unsigned cpSystemId, short index)
	: ACAMS_MsgStore(ACS_MSD_Service::getAddress(index)->name, cpSystemId, 0),
	  indextoParam(index), myAlarmStatus(false), myCurrentFile(ACE_INVALID_HANDLE), myCurrentFileName(),myNextMsgNum(1), myMsgColl(0),
	  myDiskSpaceExhausted(false), myNumberOfFiles(0), myFiles(), myStatisticsData(0), version_fptr(ACE_INVALID_HANDLE),
	  failedOldFiles(), _cpSystemId(cpSystemId) { }

int ACAMSD_MsgStore::initialize_post_ctor (ACAMSD_MsgCollection * & msgCollection) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	// this is the vector holding old files that couldn't be removed because of the virus scanner.
	failedOldFiles.clear();
	msgCollection = 0;

	// Set up environment for Message Store files
	string path;
	if (!setup(path)) { // Failed to set up directory structure
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'setup(path)' failed: failed to setup the directory data structure: "
				"path == '%s'", path.c_str());
		return -1;
	}

	// Get file name of oldest file to keep in MsgStore
	ACAMS_CommitFile * commitFile = 0;

	try {
		commitFile = new (std::nothrow) ACAMS_CommitFile(this->getCommitFileName(), true);

		if (!commitFile) { //BAD ALLOCATION
			if (version_fptr != ACE_INVALID_HANDLE) ACE_OS::close(version_fptr);
			version_fptr = ACE_INVALID_HANDLE;

			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to build a ACAMS_CommitFile object instance for '%s'",
					getCommitFileName().c_str());
			return -2;
		}
	}
	catch (std::runtime_error & ex) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' for file %s threw an exception!", this->getCommitFileName().c_str());

		if (version_fptr != ACE_INVALID_HANDLE)
			ACE_OS::close(version_fptr);

		version_fptr = ACE_INVALID_HANDLE;
		delete commitFile; commitFile = 0;

		return -3;
	}
	catch (...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' for file %s threw an exception!", this->getCommitFileName().c_str());

		if (version_fptr != ACE_INVALID_HANDLE)
			ACE_OS::close(version_fptr);

		version_fptr = ACE_INVALID_HANDLE;
		delete commitFile; commitFile = 0;

		return -4;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Opened the commit file '%s'", this->getCommitFileName().c_str());
	string firstFileToKeep = commitFile->getFileName();

	// Build an array of file names currently in the message store (sorted in increasing order).
	// Also clean up by removing any files that should already have been deleted.
	string fileName = "";
	string previousFile = "";
	bool finished = false;

	while (!finished) {
		bool foundDir;
		fileName = findNextFile(previousFile, &foundDir);

		if (!foundDir) {
			if (version_fptr != ACE_INVALID_HANDLE)
				ACE_OS::close(version_fptr);

			version_fptr = ACE_INVALID_HANDLE;

			ACS_ACA_LOG(LOG_LEVEL_TRACE, "Deleting the commit file '%s' - No data files were found!", this->getCommitFileName().c_str());
			delete commitFile; commitFile = 0;
		}

		if (!fileName.empty()) {
			// Found a file. See if it isn't too old
			if (stringToULL(fileName) < stringToULL(firstFileToKeep)) {
				// Too old. Delete it
				string completeName = getDataFileDirectoryName() + "/" + fileName;

				if (ACE_OS::unlink(completeName.c_str()) != 0) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'unlink' failed for file '%s', error == %d",
							completeName.c_str(), ACE_OS::last_error());
					failedOldFiles.push_back(completeName);
				}
				else ACS_ACA_LOG(LOG_LEVEL_DEBUG, "File '%s' successfully unlinked!", completeName.c_str());
			}
			else myFiles[myNumberOfFiles++] = fileName; // Keep this file

			previousFile = fileName;
		}
		else
			finished = true;
	}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	_open_flags = (ACS_MSD_Service::isOsCachingEnabled()) ? O_RDWR : (O_RDWR | O_SYNC);
	ACS_ACA_LOG(LOG_LEVEL_INFO, "OS caching is %s, _open_flags == %#010x", (ACS_MSD_Service::isOsCachingEnabled()) ? "ENABLED" : "DISABLED", _open_flags);
#endif

	// Get the alarm status up to date. First clear any outstanding alarm left
	// by a crashing process, then see if it's time to raise a new one.
	checkAlarmStatus();

	// Open the most recently created file and find out where to continue writing.
	if (myNumberOfFiles > 0) {
		string completeName = path + "/" + ACAMS_NameOfDataFileDir + "/" + myFiles[myNumberOfFiles - 1];

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
		myCurrentFile = ACE_OS::open(completeName.c_str(), _open_flags, ACE_DEFAULT_OPEN_PERMS);
#else
		myCurrentFile = ACE_OS::open(completeName.c_str(), O_RDWR | O_SYNC , ACE_DEFAULT_OPEN_PERMS);
#endif
		if (myCurrentFile == ACE_INVALID_HANDLE) { // Failed to open file. We cannot continue.
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the newest data file '%s'", completeName.c_str());

			myCurrentFileName = "";

			if (version_fptr != ACE_INVALID_HANDLE)
				ACE_OS::close(version_fptr);

			version_fptr = ACE_INVALID_HANDLE;
			delete commitFile; commitFile = 0;
			failedOldFiles.clear();

			return -5;
		}
		else {
			myCurrentFileName = completeName;

			// File now open. Locate the end.
			unsigned long long firstInFile = getFirstMsgNumber(myFiles[myNumberOfFiles - 1]);
			myNextMsgNum = firstInFile;   // Just in case the current file is empty
			unsigned long long nextMTAP_MsgNumber = 0ULL;
			bool fileFull = false;

			if (!(gotoEnd(myCurrentFile, completeName, myNextMsgNum, fileFull))) {
				ACE_OS::close(myCurrentFile);
				myCurrentFile = ACE_INVALID_HANDLE;
				myCurrentFileName = "";

				if (version_fptr != ACE_INVALID_HANDLE)
					ACE_OS::close(version_fptr);

				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while accessing to file '%s', file corrupt or permission lost", completeName.c_str());

				version_fptr = ACE_INVALID_HANDLE;
				delete commitFile; commitFile = 0;
				failedOldFiles.clear();

				return -6;
			}

			if (fileFull) {
				// The file was complete (with End-of-file mark at the end).
				ACE_OS::close(myCurrentFile);
				myCurrentFile = ACE_INVALID_HANDLE;
				myCurrentFileName = "";
				nextMTAP_MsgNumber = 0ULL;
			}
			else {
				// We will continue writing to this file.
				nextMTAP_MsgNumber = myNextMsgNum - firstInFile;
				myMsgColl = new (std::nothrow) ACAMSD_MsgCollection(this, static_cast<unsigned long>(nextMTAP_MsgNumber),	ACS_MSD_Service::getAddress(indextoParam)->winSize);
				msgCollection = myMsgColl;
			}
		}
	}

	delete commitFile; commitFile = 0;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// Destructor
//****************************************************************
ACAMSD_MsgStore::~ACAMSD_MsgStore () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	// Cease any outstanding alarm
	checkAlarmStatus(true);

	delete myMsgColl; myMsgColl = 0;

	if (version_fptr != ACE_INVALID_HANDLE)
		ACE_OS::close(version_fptr);

	if (myCurrentFile != ACE_INVALID_HANDLE) {
		ACE_OS::close(myCurrentFile);
		myCurrentFileName = "";
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
int ACAMSD_MsgStore::setCachingStrategyForDataFiles(bool cache_on)
{
	int retcode = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if((myCurrentFile != ACE_INVALID_HANDLE) && ACE_OS::close(myCurrentFile) && (errno != EBADF)) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to close current data file for the ACAMSD_MsgStore object identified by the couple (%u, %d) ! errno == %d", _cpSystemId, indextoParam, errno);
		retcode = -1;
	}

	if(retcode != -1) {
		_open_flags = cache_on ? O_RDWR : (O_RDWR | O_SYNC);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "OS caching for the ACAMSD_MsgStore object identified by the couple (%u, %d) is %s, _open_flags == %#010x", _cpSystemId, indextoParam, (cache_on ? "ENABLED" : "DISABLED"), _open_flags);
	}

	if((retcode != -1) && (myCurrentFile != ACE_INVALID_HANDLE)) {
		// reopen the current file
		myCurrentFile = ACE_OS::open(myCurrentFileName.c_str(), _open_flags, ACE_DEFAULT_OPEN_PERMS);
		if(myCurrentFile == ACE_INVALID_HANDLE) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Unable to re-open current data file '%s' for the ACAMSD_MsgStore object identified by the couple (%u, %d)", myCurrentFileName.c_str(), _cpSystemId, indextoParam);
			myCurrentFileName = "";
			retcode = -1;
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retcode;
}
#endif

//****************************************************************
// getNextMsgNumber
//****************************************************************
unsigned long long ACAMSD_MsgStore::getNextMsgNumber () const {
	return myNextMsgNum;
}

//****************************************************************
// createDataFile()
//
// Create a new DataFile in the MsgStore
//  - Create a Collection for temporary storage of messages
//  - Assign file name for new file
//  - Open new file
// If there is already the maximum number of files allowed in the 
// message store, MessageStoreFull is returned.
//****************************************************************
ACAMSD_MsgStore::MsgEcode ACAMSD_MsgStore::createDataFile (unsigned char mtapFileNum, ACAMSD_MsgCollection * & msgCollection) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	MsgEcode retCode = NoError;

	if (myNumberOfFiles >= ACS_MSD_Service::getAddress(indextoParam)->noOfFiles) {
		msgCollection = 0;
		return MessageStoreFull; // The Message Store is full
	}

	myMsgColl = new (std::nothrow) ACAMSD_MsgCollection(this, 0, ACS_MSD_Service::getAddress(indextoParam)->winSize);
	msgCollection = myMsgColl;

	// Create and open a new file for unbuffered writing
	string fileName = assignFileName(myNextMsgNum, mtapFileNum);
	string completeName = getDataFileDirectoryName() + "/" + fileName;

	myCurrentFile = ACE_OS::open(completeName.c_str(), _open_flags, ACE_DEFAULT_OPEN_PERMS);
	if (myCurrentFile == ACE_INVALID_HANDLE)
		myCurrentFile = ACE_OS::open(completeName.c_str(), _open_flags | O_CREAT, ACE_DEFAULT_OPEN_PERMS);
    
	if (myCurrentFile == ACE_INVALID_HANDLE) {
		// Failed to open file. We cannot continue.
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the new data file '%s', errno = %d",
				completeName.c_str(), ACE_OS::last_error());
		myCurrentFileName = "";
		retCode = DiskSpaceExhaust;
	}
	else {
		myCurrentFileName = completeName;

		// If the file existed before (it shouldn't), an Eof-mark at the end of the file must be removed.

		// First get the size of the file.
		ACE_LOFF_T fsize = ACE_OS::filesize(myCurrentFile);

		if (fsize == -1)
			retCode = AccessFileDenied; // Access denied for some reason
		else if (fsize >= ACAMS_Eof_Mark_Length) { // Is file large enough ?
			// Back the file pointer off one Eof-mark size
			ACE_OS::lseek(myCurrentFile, -ACAMS_Eof_Mark_Length, SEEK_END);

			// Read last part of file
			unsigned char buffer[ACAMS_Eof_Mark_Length];
			ACE_INT32 nobr = ACE_OS::read(myCurrentFile, buffer, ACAMS_Eof_Mark_Length, 0);

			if ((nobr < 0) || (nobr != ACAMS_Eof_Mark_Length)) {
				ACE_OS::close(myCurrentFile);
				myCurrentFile = ACE_INVALID_HANDLE;
				myCurrentFileName = "";
				retCode = AccessFileDenied; // Access denied for some reason
			}
			else if (::memcmp(buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0) {
				// Cut the tail off the file
				ACE_OFF_T pos = ACE_OS::lseek(myCurrentFile, -ACAMS_Eof_Mark_Length, SEEK_END);
				ACE_OS::ftruncate(myCurrentFile, pos);
			}
			else {
				ACE_OS::close(myCurrentFile);
				myCurrentFile = ACE_INVALID_HANDLE;
				myCurrentFileName = "";
				retCode = AccessFileDenied; // Access denied for some reason
			}
		} else {}
	}

	if (retCode == NoError) {
		myFiles[myNumberOfFiles++] = fileName;
		checkAlarmStatus(); // Get the alarm status up to date
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;			 // Successful turnout
}

//****************************************************************
// closeDataFile()
//
// Close the current disk-file. First write any remaining messages 
// in the MsgCollection to disk (there shouldn't be any). Then 
// write the 'End-Of-File' mark to the disk-file and close it. 
// Finally, delete the MsgCollection.
//****************************************************************
bool ACAMSD_MsgStore::closeDataFile () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool ret = true;

	if ((myCurrentFile != ACE_INVALID_HANDLE) && myMsgColl) {
		unsigned char * outData = 0;
		unsigned int outSize = 0;
		vector<ACAMSD_MTAP_Message *> flushedMsg;
		int numberOfLostMsg;
		int numberOfSkippedMsg;
		int numberOfFlushedMsg;

		int numberOfMsgLeft = myMsgColl->flush(outData, outSize, false, flushedMsg, numberOfLostMsg, numberOfSkippedMsg, numberOfFlushedMsg);

		myMsgColl->flushCommit(numberOfFlushedMsg);

		if (numberOfMsgLeft > 0) {
			// There are still Messages left in the MsgCollection. Write them to disk now.
			ACE_OFF_T lastPosition = ACE_OS::lseek(myCurrentFile, 0, SEEK_END);
			ssize_t bytesWritten = ACE_OS::write(myCurrentFile, outData, outSize);

			if ((bytesWritten < 0) || (bytesWritten < outSize)) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Write file failed, moving the file pointer to its previous position, "
						"bytesWritten == %zd, bytes_to_write == %d, error == %d", bytesWritten, outSize, ACE_OS::last_error());
				
				if (bytesWritten > 0) {	// Move the file pointer backwards to its previous position
					ACS_ACA_LOG(LOG_LEVEL_WARN, "Not all the bytes have been written, truncating the file!");

					for (int i = 0; (i < 50) && (ACE_OS::lseek(myCurrentFile, lastPosition, SEEK_SET) < 0); i++) {
						::usleep(20 * 1000);
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'lseek' failed (iteration %d), last_error = %d",	i, ACE_OS::last_error());
					}

					for (int c = 0; (c < 50) && (ACE_OS::ftruncate(myCurrentFile, lastPosition) != 0); c++) { //truncate the file
						::usleep(20 * 1000);
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ftruncate' failed (iteration %d), last_error = %d", c, ACE_OS::last_error());
					}
					ACS_ACA_LOG(LOG_LEVEL_WARN, "File correctly truncated!");
				}
				ret = false;
			}
			else {
				myNextMsgNum += numberOfMsgLeft;
				while (flushedMsg.size()) {
					delete flushedMsg.back();
					flushedMsg.pop_back();
				}
			}		 
		} else {}

		delete[] outData;	outData = 0;

		// If the file to be closed is empty, it must be removed
		ACE_INT32 fsize = ACE_OS::filesize(myCurrentFile);

		if (fsize == -1) {
			ret = false;
		}
		else if (fsize == 0) {
			ACE_OS::close(myCurrentFile);
			myCurrentFile = ACE_INVALID_HANDLE;
			myCurrentFileName.clear();

			string fullPath = getMS_Name() + "//dataFiles//" + myFiles[myNumberOfFiles - 1];

			if (!ACE_OS::unlink(fullPath.c_str())) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'unlink' failed for file '%s', error == %d",	fullPath.c_str(), ACE_OS::last_error());
				ret = false;
			}
		}
		else {
			// Verify that the last thing is an End-Of-Message mark, and if not, return false.
			if (!endOfMsg(myCurrentFile)) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "File %s corrupt at file closure, error == %d",
						myFiles[myNumberOfFiles - 1].c_str(), ACE_OS::last_error());

				ret = false;
			}
			else {
				// Write End-Of-File mark to disk-file and close it
				ret = writeEndOfFileMarker();
			}
		}

		if (ret) {
			if (myCurrentFile != ACE_INVALID_HANDLE)
				ACE_OS::close(myCurrentFile);

			myCurrentFile = ACE_INVALID_HANDLE;
			myCurrentFileName = "";
			delete myMsgColl; myMsgColl = 0;
		}
	} else {}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ret;
}


//****************************************************************
// deleteDataFile()
//
// Delete DataFile (and any DataFile before it). Return number of 
// files deleted.
//****************************************************************
int ACAMSD_MsgStore::deleteDataFile (const string & fileName) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int numberOfFilesToDelete = 0;

	// check if there are old files that should be removed. only one at a time.
	if (failedOldFiles.size() > 0) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "There are %lu old files to delete, deleting them!", failedOldFiles.size());
		if (ACE_OS::unlink(failedOldFiles.back().c_str()) == 0)
			failedOldFiles.pop_back();
	}

	for (int i = 0; i < myNumberOfFiles - 1; ++i) {
		if (myFiles[i] == fileName) { // File name found. This file and any file before it shall be deleted.
			numberOfFilesToDelete = i + 1;

			for (int ii = 0; ii < numberOfFilesToDelete; ++ii) {
				string completeName = getDataFileDirectoryName() + "/" + myFiles[ii];

				if (ACE_OS::unlink(completeName.c_str()) != 0) {
					// Failed to delete file. Not considered disastrous.
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to unlink file '%s', error = %d", completeName.c_str(), ACE_OS::last_error());
					failedOldFiles.push_back(completeName);
				}
				else {
					myDiskSpaceExhausted = false;
				}
			}

			myNumberOfFiles -= numberOfFilesToDelete;

			// Now the remaining items in the array must be moved backward
			for (int j = 0; j < myNumberOfFiles; ++j) myFiles[j] = myFiles[j + numberOfFilesToDelete];

			// Get the alarm status up to date
			checkAlarmStatus();

			break;
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");

	return numberOfFilesToDelete;
}

//****************************************************************
// store()
//
// Store the Messages in the MsgCollection to disk and acknowledge 
// them through owner. Return number of Msg stored.
//****************************************************************
bool ACAMSD_MsgStore::store (ACAMSD_MTAP_ChannelManager * owner, int & numberOfMsg) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool ret = true;
	numberOfMsg = 0;

	if (myMsgColl) {
		unsigned char * outData = 0;
		unsigned int outSize = 0;
		vector<ACAMSD_MTAP_Message *> flushedMsg;
		int numberOfLostMsg;
		int numberOfSkippedMsg;
		int numberOfFlushedMsg;   

		// Get message data from the Collection
		numberOfMsg = myMsgColl->flush(outData, outSize, true, flushedMsg, numberOfLostMsg, numberOfSkippedMsg, numberOfFlushedMsg);

		if (numberOfMsg < 1) { // No messages flushed ?
			// We haven't even opened any file yet, so there is nothing to store
			numberOfMsg = 0;
			delete[] outData;	outData = 0;
			return true;
		}

		ACE_OFF_T lastPosition = ACE_OS::lseek(myCurrentFile, 0, SEEK_END);
		ssize_t bytesWritten = ACE_OS::write(myCurrentFile, outData, outSize);

		if ((bytesWritten < 0) || ( bytesWritten < outSize)) { // Write failed. We cannot continue
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'write' failed, no space on the disk, "
					"bytesWritten == %zd, bytes_to_write == %d, error == %d",	bytesWritten, outSize, ACE_OS::last_error());
			myDiskSpaceExhausted = true;

			for (unsigned int i = 0; i < flushedMsg.size(); ++i) {
				owner->ackToCP(flushedMsg[i], ACAMSD_MTAP_Protocol::DiskSpaceExhaust);
			}

			// Move the file pointer backwards to its previous position
			if (bytesWritten > 0) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Not all the bytes have been written, truncating the file!");

				for (int c = 0; (c < 50) && (ACE_OS::lseek(myCurrentFile, lastPosition, SEEK_SET) < 0); c++) {
					::usleep(20 * 1000);
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'lseek' failed (iteration %d), last_error = %d", c, ACE_OS::last_error());
				}

				for (int c = 0; (c < 50) && (ACE_OS::ftruncate(myCurrentFile, lastPosition) != 0); c++) { //truncate the file
					::usleep(20 * 1000);
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Cannot truncate the file (iteration %d), last_error = %d", c, ACE_OS::last_error());
				}

				ACS_ACA_LOG(LOG_LEVEL_WARN, "File correctly truncated!");
				ret = false;
			}
		}
		else {
			myMsgColl->flushCommit(numberOfFlushedMsg);
			myNextMsgNum += numberOfMsg;

			// Make sure the messages are stored on disk before sending acknowledgements to the CP.
			// Now acknowledge all messages, successfully written to disk.
			for (unsigned int i = 0;  i < flushedMsg.size(); ++i) {
				if (flushedMsg[i]) {
					owner->ackToCP(flushedMsg[i]);
				}
			}

			// delete the stored and confirmed MTAP messages.
			while (flushedMsg.size()) {
				delete flushedMsg.back();
				flushedMsg.pop_back();
			}		
		}

		// And clean up.
		delete[] outData;	outData = 0;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ret;
}

ACAMSD_MsgStore::MsgEcode ACAMSD_MsgStore::storeExpRecord (unsigned char * outData, unsigned int outSize) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	MsgEcode ret = NoError;

	// Write to file
	 ssize_t bytesWritten = ACE_OS::write(myCurrentFile, outData, outSize);
	if ((bytesWritten < outSize)) { // Move the file pointer backwards to its previous position
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to write to disk for file '%s', error = %d", myFiles[myNumberOfFiles - 1].c_str(), ACE_OS::last_error());

		if (bytesWritten > 0) {
			int lastposition = ACE_OS::lseek(myCurrentFile, -bytesWritten, SEEK_END);

			for (int c = 0; (c < 50) && (ACE_OS::ftruncate(myCurrentFile, lastposition) != 0); c++) { //truncate the file
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Write to disk successful, but file truncation failed for file '%s', error == %d",
						myFiles[myNumberOfFiles - 1].c_str(), ACE_OS::last_error());
			}
		}
		ret = DiskSpaceExhaust;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ret;
}

//****************************************************************
//                   Private help-functions
//****************************************************************

//****************************************************************
// Build a string that defines file name for the data-file storing
// messages from given MsgNumber.
//****************************************************************
string ACAMSD_MsgStore::assignFileName (unsigned long long firstMsgNum, const unsigned char mtapFileNum) const {
	std::ostringstream outString;
	outString << firstMsgNum << '_' << static_cast<unsigned>(mtapFileNum);
	return outString.str();
}

//****************************************************************
// Update alarm status, i.e. issue or cease alarm 'Message Store 
// Warning' as the number of data files in the message store 
// passes the threshold 'myAlarmLevel'. If 'forcedCease' is true, 
// the alarm should be ceased unconditionally.
//****************************************************************
void ACAMSD_MsgStore::checkAlarmStatus (bool forcedCease) {
	if (myAlarmStatus) {
		int threshold = ACS_MSD_Service::getAddress(indextoParam)->noOfFiles * ACS_MSD_Service::getAddress(indextoParam)->alarmLevel / 100;

		if (forcedCease || (myNumberOfFiles < threshold)) { // cease alarm
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);

			const int call_result = cease_msg_full_alarm();
			if (!call_result)
				myAlarmStatus = false;
			else
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Failed to cease the alarm for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);
		}
	} else {
		int threshold = ACS_MSD_Service::getAddress(indextoParam)->noOfFiles * ACS_MSD_Service::getAddress(indextoParam)->alarmLevel / 100;

		if (!forcedCease && (myNumberOfFiles >= threshold)) { // raise alarm
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Raising alarm for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);

			const int call_result = raise_msg_full_alarm();
			if (!call_result)
				myAlarmStatus = true;
			else
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Failed to raise the alarm for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);
		}
	}
}


//****************************************************************
// gotoEnd()
//
// Locate last message stored in this file, and return next 
// MsgNumber to be stored. Will also truncate the file after last 
// correctly written message if there is rubbish at the end.
// If the file is empty or doesn't contain any complete message, 
// the value of 'msgNum' will be kept.
//****************************************************************
bool ACAMSD_MsgStore::gotoEnd (ACE_HANDLE fhand, const string & fileName, unsigned long long & msgNum, bool & fileFull) {
	unsigned char buffer[ACAMS_MsgHeaderSize];
	unsigned long long tmpMsgNo = 0;
	unsigned char bufferMsgSize[4];
	ACE_OFF_T currentOffset = 0;
	fileFull = false;
	bool atLeastOneMsgFound = false;
	ACE_INT32 lastOkFilePtr = 0;
	unsigned long long lastOkMsgNum = 0;
	bool ret = false;

	// First get the size of the file
	ACE_OFF_T fsizeLow = ACE_OS::filesize(fhand);

	if (fsizeLow == -1) { // -1 <--> 0xFFFFFFFF
		// We couldn't get the size. Must abort.
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to get file size for file '%s'", fileName.c_str());
		return ret;
	}
   
	// Now read one message at the time until we are at the end of the file.
	ACE_INT32 nobr = 0;
	bool continueFileRead = true;
	while (continueFileRead) {
		// First read the message header: 12 bytes.
		nobr = ACE_OS::read(fhand, buffer, ACAMS_MsgHeaderSize);

		if (nobr < 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read file header for file '%s'", fileName.c_str());
			continueFileRead = false;
		}
		else if (nobr == ACAMS_MsgHeaderSize) {
			// Message header read OK, i.e record number + record size
			if (::memcmp(buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0) {
				// We found the End-of-file mark. Verify that E-o-F is preceeded by End-of-Message mark (E-o-M).
				continueFileRead = false;
				ACE_OS::lseek(fhand, -ACAMS_Eof_Mark_Length, SEEK_CUR);

				if (!endOfMsg(fhand)) {
					// there was no E-o-M preceeding E-o-F. Truncate the file to the last ok place.
					ACE_OFF_T lastposition = ACE_OS::lseek(fhand, lastOkFilePtr, SEEK_SET); // SEEK_BEGIN);

					// now check that the file ends with E-o-M.
					if (!endOfMsg(fhand)) {
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "File '%s' corrupted!", fileName.c_str());
					}
					else if (ACE_OS::ftruncate(fhand,lastposition) != 0) {
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Cannot truncate file '%s'!", fileName.c_str());
					}
					else if (lastOkFilePtr != 0) {
						msgNum = lastOkMsgNum;
						ret = true;
					}
				} else {
					// We found the End-of-Message mark. The file is complete. Position the file pointer to the end of file.
					ret = true;
					fileFull = true;
				}
			}
			else {
				// Now get the message size.
				::memcpy(bufferMsgSize, &buffer[8], 4);
				unsigned int msgSiz = *reinterpret_cast<unsigned int *>(bufferMsgSize);

				if (((msgSiz + ACAMS_MsgHeaderSize + currentOffset) > static_cast<unsigned>(fsizeLow)) || (msgSiz == 0)) {

					// Next message is not complete. Truncate the file here.			
					ACE_OFF_T lastposition = ACE_OS::lseek(fhand, currentOffset, SEEK_SET);

					// now check that the file ends with E-o-M.
					if (!endOfMsg(fhand))	{
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "File '%s' corrupted!", fileName.c_str());
					}
					else if (ACE_OS::ftruncate(fhand,lastposition) != 0) {
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Cannot truncate file '%s'!", fileName.c_str());
						fsizeLow = ACE_OS::filesize(fhand);
					}
					else if (lastOkFilePtr != 0) {
						msgNum = lastOkMsgNum;
						ret = true;
					}

					continueFileRead = false;
				}
				else {
					tmpMsgNo = convertToUll(buffer);
					if (tmpMsgNo != 0ULL) { // Zero means 'expedited data'.
						atLeastOneMsgFound = true;  // The message is OK.
						msgNum = tmpMsgNo;
					}

					lastOkFilePtr = currentOffset;
					lastOkMsgNum = msgNum;
					currentOffset = ACE_OS::lseek(fhand, msgSiz, SEEK_CUR);
				}
			}
		} else { // nobr < ACAMS_MsgHeaderSize
			// We are at the end. If any bytes were read, there is a non-complete
			// message header which must be truncated.
			ACE_OFF_T lastposition;
			if (nobr > 0) {
				lastposition = ACE_OS::lseek(fhand, -nobr, SEEK_CUR);
			}

			// Verify that there is an E-o-M before the last read.
			ret = true;
			if (fsizeLow > 0) {
				if (!endOfMsg(fhand)) { // file corrupt
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "File '%s' corrupted!", fileName.c_str());
					ret = false;
				}
			}

			ACS_ACA_LOG(LOG_LEVEL_ERROR, "File '%s' is being cleaned at offset: 0x%X (%u)", fileName.c_str(), currentOffset, currentOffset);

			ACE_OS::ftruncate(fhand,currentOffset);
			continueFileRead = false;
		}
	} // End while
 
	if (atLeastOneMsgFound) ++msgNum; // step to the next msg number

	return ret;
}

//****************************************************************
// Verifies an End-of-Message pattern and returns true if found, 
// else false.
//****************************************************************
bool ACAMSD_MsgStore::endOfMsg (ACE_HANDLE fhand) {
	bool ret = false;
	char eomBuf[ACAMS_Msg_End_Mark_Length];
	ACE_INT32 dataLn;

	if (ACE_OS::lseek(fhand, -ACAMS_Msg_End_Mark_Length, SEEK_CUR) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'lseek' failed, errno == %d", ACE_OS::last_error());
	}

	if ((dataLn = ACE_OS::read(fhand, eomBuf, ACAMS_Msg_End_Mark_Length)) && (dataLn == ACAMS_Msg_End_Mark_Length)) {
		ret = (::memcmp(eomBuf, ACAMS_Msg_End_Mark, ACAMS_Msg_End_Mark_Length) == 0);
		if (!ret) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "File corrupted: End of Message Buf: %x %x %x %x",
					static_cast<unsigned>(eomBuf[0]), static_cast<unsigned>(eomBuf[1]),
					static_cast<unsigned>(eomBuf[2]), static_cast<unsigned>(eomBuf[3]));
		} else {
			ACS_ACA_LOG(LOG_LEVEL_TRACE, "File OK: End of Message Buf: %x %x %x %x",
					static_cast<unsigned>(eomBuf[0]), static_cast<unsigned>(eomBuf[1]),
					static_cast<unsigned>(eomBuf[2]), static_cast<unsigned>(eomBuf[3]));
		}
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'read' failed, call_result == %d, errno == %d", dataLn, ACE_OS::last_error());
	}
	
	return ret;
}

//****************************************************************
// setup()
//
// Create message store unless it already exists, and change curr-
// ent directory. If anything fails, return false (and path points 
// out the path of the directory that failed).
//****************************************************************
bool ACAMSD_MsgStore::setup (string & path) {
	// First create 'name' directory, if not already done
	path = getMSD_Home() + "/" + ACS_MSD_Service::getAddress(indextoParam)->name;

	if (ACE_OS::mkdir(path.c_str(), ACE_DEFAULT_FILE_PERMS) < 0) {
		int error = ACE_OS::last_error();
		if (error != EEXIST) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mkdir' failed for path '%s', errno == %d", path.c_str(), error);
			return false;
		}
	}
	 	
	path = getMS_Name();
	if (ACE_OS::mkdir(path.c_str(), ACE_DEFAULT_DIR_PERMS) < 0) {
		if (ACE_OS::last_error() != EEXIST) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create '%s' directory, error == %d", path.c_str(), ACE_OS::last_error());
			return false; // Failed to create 'site' directory
		}
	}

	// the 'dataFiles' directory
	path = getDataFileDirectoryName();
	if (ACE_OS::mkdir(path.c_str(), ACE_DEFAULT_DIR_PERMS) < 0) {
		if (ACE_OS::last_error()!= EEXIST) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create '%s' directory, error == %d", path.c_str(), ACE_OS::last_error());
			return false; // Failed to create 'dataFiles' directory
		}
	}

	// Change current directory.
	path = getMS_Name();
	string vfile = path + "/versionFile"; 
	version_fptr = ACE_OS::open(vfile.c_str(), _open_flags | O_CREAT, ACE_DEFAULT_OPEN_PERMS);
	return true;
}

//****************************************************************
// Stores the MTAP protocol version on the disc (needed in case 
// the process restarts.
// Return values:
//     false  : storage of version failed
//     true   : storage succeded
//****************************************************************
bool ACAMSD_MsgStore::putVersion (unsigned char v) {

	if (version_fptr == ACE_INVALID_HANDLE) {
		return false;
	}

	ACE_OS::lseek(version_fptr, NULL, SEEK_SET);
	long bytesWritten = ACE_OS::write(version_fptr, &v, sizeof(unsigned char));

	if (bytesWritten < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Write failed!, errno == %d", ACE_OS::last_error());
		return false;
	} else {
		return true;
	}
}

//****************************************************************
// Read the MTAP protocol version from the disc (needed in case 
// the process restarts.
//****************************************************************
unsigned char ACAMSD_MsgStore::getVersion () {

	ACE_INT32 nobr = 0;
	unsigned char v = 0;

	ACE_OS::lseek(version_fptr, NULL, SEEK_SET);//SEEK_BEGIN);
	nobr = ACE_OS::read(version_fptr, &v, sizeof(unsigned char));
	if (nobr < 0) {
		ACE_INT32 err = ACE_OS::last_error();
		if (err > 10000) err -= 10000;
		if (err < 4) err += 100;

		ACE_OS::close(version_fptr);

		return static_cast<unsigned char>(err);
	} else {
		return v;
	}
}

int ACAMSD_MsgStore::raise_msg_full_alarm() {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	acs_aeh_evreport ev_report;
	char problem_text[512] = {0};

	if (_cpSystemId != INVALID_CPSYSTEMID)	// MCP Format: MessageStore, Limit and message
		::snprintf(problem_text, sizeof(problem_text) - 1, PROBLEM_TEXT_FORMAT_MCP,
				ACS_MSD_Service::getAddress(indextoParam)->name, ACS_MSD_Service::getAddress(indextoParam)->alarmLevel);
	else	// SCP Format: MessageStore and Limit
		::snprintf(problem_text, sizeof(problem_text) - 1, PROBLEM_TEXT_FORMAT_SCP,
				ACS_MSD_Service::getAddress(indextoParam)->name, ACS_MSD_Service::getAddress(indextoParam)->alarmLevel);

	ACS_AEH_ReturnType return_code = ev_report.sendEventMessage(
			PROCESS_NAME,
			SPECIFIC_PROBLEM,
			ACS_MSD_Service::getAddress(indextoParam)->ACFilledLevel,
			PROBABLE_CAUSE,
			OBJ_CLASS_OF_REFERENCE,
			ACS_MSD_Service::getAddress(indextoParam)->name,
			PROBLEM_DATA,
			problem_text);

	if (return_code != ACS_AEH_ok) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendEventMessage' failed, error text: %s", ev_report.getErrorText());
		return -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Alarm successfully raised for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACAMSD_MsgStore::cease_msg_full_alarm() {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	acs_aeh_evreport ev_report;

	ACS_AEH_ReturnType return_code = ev_report.sendEventMessage(
			PROCESS_NAME,
			SPECIFIC_PROBLEM,
			"CEASING",
			PROBABLE_CAUSE,
			OBJ_CLASS_OF_REFERENCE,
			ACS_MSD_Service::getAddress(indextoParam)->name,
			"",
			"");

	if (return_code != ACS_AEH_ok) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendEventMessage' failed, error text: %s", ev_report.getErrorText());
		return -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Alarm successfully ceased for the message store %s", ACS_MSD_Service::getAddress(indextoParam)->name);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// writeEndOfFileMarker()
//
// Write End-Of-File mark to the current open disk-file (myCurrentFile).
// Return value false indicates success.
//****************************************************************
bool ACAMSD_MsgStore::writeEndOfFileMarker()
{
	bool success = true;

	// Write End-Of-File mark to disk-file
	ssize_t bytesWritten = ACE_OS::write(myCurrentFile, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length);

	if ((bytesWritten < 0) || (bytesWritten < ACAMS_Eof_Mark_Length))
	{  // Write failed. We cannot continue
		ACS_ACA_LOG(
				LOG_LEVEL_ERROR,
				"Call 'write' failed, bytes_to_write = %d, bytes_written = %zd, error = %d",
				ACAMS_Eof_Mark_Length, bytesWritten, ACE_OS::last_error());
		success = false;
	}

	return success;
}
