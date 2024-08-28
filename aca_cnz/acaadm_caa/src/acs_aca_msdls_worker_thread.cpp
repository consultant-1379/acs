#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>

#include <list>
#include <map>
#include <stdexcept>

#include "ace/ACE.h"
#include "ace/Event.h"

#include "acs_aca_macros.h"
#include "acs_aca_utility.h"
#include "acs_aca_msd_service.h"
#include "acs_aca_ms_statistics_file.h"
#include "acs_aca_ms_const_values.h"
#include "acs_aca_ms_parameters.h"
#include "acs_aca_msgfile.h"
#include "acs_aca_structured_request.h"
#include "acs_aca_structured_response_builder.h"
#include "acs_aca_msg_director.h"
#include "acs_aca_ms_commit_file.h"
#include "acs_aca_logger.h"
#include "acs_aca_configuration_helper.h"
#include "acs_aca_msdls_worker_thread.h"


//----------------------------------------------------------------------------------
//      Constructor
//----------------------------------------------------------------------------------
ACS_ACA_MSDLS_WorkerThread::ACS_ACA_MSDLS_WorkerThread (ACS_DSD_Session * session, unsigned /*theStackSize*/)
	: _state(THREAD_STATE_NOT_STARTED), _tid(0), dfile_hand(ACE_INVALID_HANDLE), MapDataFile(ACE_INVALID_HANDLE),
	  file_view(MAP_FAILED), _file_map_size(0), SizeDataFile(0), _session(session), connectedH(ACE_INVALID_HANDLE), seqNo(0) {

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	int handle_count = 1;
	int ret  = _session->get_handles(&connectedH, handle_count);
	if (ret < 0)
		ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'get_handles' failed, call_result = %d", ret);

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_MSDLS_WorkerThread::~ACS_ACA_MSDLS_WorkerThread () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (_session) _session->close();

	delete _session; _session = 0;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
}

//------------------------------------------------------------------------------
//      stop actions
//------------------------------------------------------------------------------
void ACS_ACA_MSDLS_WorkerThread::stopMSDLSworker () {
	/* add here additional actions */
}
int ACS_ACA_MSDLS_WorkerThread::workerMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "MSDLS Worker Thread, starting to work - TID = %ld", _tid);

	int exitCode = 0;
	char request[ACS_DSD_MAXBUFFER] = {0};
	unsigned timeout = 3 * 1000;	// 3 seconds
	size_t bytes_received;

	if ((bytes_received = _session->recv(request, ACS_DSD_MAXBUFFER, timeout)) <= 0) { //receive failed
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'recv' failed, call_result = %zu error '%s'",
				bytes_received, _session->last_error_text());
		return -1;
	}

	//elaborate data & build response(s)
	if (!processRequest(request /*,bytes_received*/))
		exitCode = -1;

	_session->close();
	delete _session; _session = 0;

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "MSDLS Worker Thread, work done! - TID = %ld", _tid);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return exitCode;
}

//------------------------------------------------------------------------------
//      Process Request
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::processRequest (const char * request /*, unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool retCode = false;
	
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Request to process %hu", req->optmask);

	switch (req->optmask) {
	///// FORMAT 1
	case (0): //"mslds"
	case (1): //"msdls -ap apx"
		retCode = process_0(/*request, reqSize*/);
		break;
	///// FORMAT 2
	case (3): //"mslds -cp cpname -ap apx"
	case (2): //"mslds -cp cpname"
		retCode = process_2(request /*, reqSize*/);
		break;
	case (5): //"mslds -m mstore -ap apx"
	case (4): //"mslds -m mstore"
		retCode = process_4(request /*, reqSize*/);
		break;
	//case (256): //"mslds -s cp_src"
	//	retCode = process_256(request /*, reqSize*/);
	//	break;
	///// FORMAT 3
	case (7): //"mslds -m mstore -cp cpname -ap apx"
	case (6): //"mslds -m mstore -cp cpname"
		retCode = process_6(request /*, reqSize*/);
		break;
	case (15): //"mslds -m mstore -cp cpname -a -ap apx"
	case (14): //"mslds -m mstore -cp cpname -a"
		retCode = process_14(request /*, reqSize*/);
		break;
	case (260): //"mslds -m mstore -s cp_src"
	case (261): //"mslds -m mstore -s cp_src -ap apx"
		retCode = process_260(request /*, reqSize*/);
		break;
	case (268): //"mslds -m mstore -s cp_src -a"
	case (269): //"mslds -m mstore -s cp_src -a -ap apx"
		retCode = process_260(request /*, reqSize*/, true);
		break;
	///// FORMAT 4
	case (22): //"mslds -m mstore -cp cpname -d file"
		retCode = process_22(request /*, reqSize*/ , false, false);
		break;
	case (54): //"mslds -m mstore -cp cpname -d file -t"
		retCode = process_22(request /*, reqSize*/ , false, true);
		break;
	case (86): //"mslds -m mstore -cp cpname -d file -n"
		retCode = process_22(request /*, reqSize*/, true, false);
		break;
	case (276): //"mslds -m mstore -s cp_src -d file"
		retCode = process_276(request /*, reqSize*/ , false, false);
		break;
	case (340): //"mslds -m mstore -s cp_src -d file -n"
		retCode = process_276(request /*, reqSize*/ , true, false);
		break;
	case (308): //"mslds -m mstore -s cp_src -d file -t"
		retCode = process_276(request /*, reqSize*/, false, true);
		break;
	///// FORMAT 5
	case (135): //"mslds -m mstore -cp cpname -S -ap apx"
	case (134): //"mslds -m mstore -cp cpname -S"
		retCode = process_134(request /*, reqSize*/);
		break;
	case (388): //"mslds -m mstore -s cp_src -S"
		retCode = process_388(request /*, reqSize*/);
		break;
	default:
		sendInvalidError();
		break;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Process "msdls"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_0 (/*const char * request , unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool retCode = false;
	//aca_msdls_req_t * req = (aca_msdls_req_t *)request;

	bool isBC =  false;

	try {
		isBC = isMultipleCPSystem();
	} catch(ACS_ACA_CS_API::CS_API_Result_ACA & result) { //CSERROR
		if (result == ACS_ACA_CS_API::Result_NoEntry) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' threw an exception, exiting");
			return sendCSError_56();
		}
		else
			return sendCSError_55();
	}
	
	vector<ACAMSD_MS_Manager *> mgrs = ACS_MSD_Service::getAllMSManagers(); //get all MSManagers
	vector<ACAMSD_MS_Manager *>::iterator mgrIt;
	
	for (mgrIt = mgrs.begin(); mgrIt != mgrs.end(); ++mgrIt) {
		ACAMSD_MS_Manager * mgr = *mgrIt;

		//get message store name
		char msname[16] = {0};
		::strcpy(msname, mgr->msName());

		if (isBC) { //Multiple CP
			//get all cp sys id
			std::list<unsigned short> cplist = mgr->getCPList();
			std::list<unsigned short>::iterator idIt;

			for (idIt = cplist.begin(); idIt != cplist.end(); ++idIt) {
				unsigned short cpID = *idIt; //Get CPID
				retCode = sendResponse(msname, cpID); //send CPID
				if (retCode == false) break;
			}
		} else { //Single CP
			char msg[512] = {0};
			ACE_OS::snprintf(msg, sizeof(msg) - 1, "%s", msname);//Format String
			retCode = sendResponse(msg); //send string
			if (retCode == false) break;
		}
	}
	//Simulate a response for testing purpose
	//end of responses
	retCode = sendEnd();
	
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Process "mslds -cp cpname"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_2 (const char * request /*, unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool retCode = true;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);
	unsigned short cpid = req->cpID;

	vector<ACAMSD_MS_Manager *> mgrs = ACS_MSD_Service::getAllMSManagers();
	vector<ACAMSD_MS_Manager *>::iterator mgrIt;
	for (mgrIt = mgrs.begin(); mgrIt != mgrs.end(); ++mgrIt) {
		ACAMSD_MS_Manager * mgr = 0;
		mgr = *mgrIt;
		if (!mgr)	continue;

		std::list<unsigned short> cplist = mgr->getCPList();
		std::list<unsigned short>::iterator idIt;
		for (idIt = cplist.begin(); idIt != cplist.end(); ++idIt) {
			unsigned short t_id = *idIt;

			if (t_id == cpid) { //get message store name
				char msname[16] = {0};
				::strcpy(msname, mgr->msName());
				retCode = sendResponse(msname, cpid);//send MS e cpID
				if (retCode == false) break;
			}	
		}
	}

	//end of responses
	retCode = sendEnd();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Process "mslds -m mstore"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_4 (const char * request /*, unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool retCode = false;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);
	ACAMSD_MS_Manager * mgr = ACS_MSD_Service::getMSManagerByMsgStore(req->msname);

	if (mgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The ACAMSD_MS_Manager pointer is NULL, sending error");
		retCode = sendError(INVALID, req->msname);
	} else {
		std::list<unsigned short> cplist = mgr->getCPList();
		std::list<unsigned short>::iterator idIt;

		for (idIt = cplist.begin(); idIt != cplist.end(); ++idIt) {
			unsigned short cpID = *idIt;
			retCode = sendResponse(req->msname, cpID);//send MS e CPID
			if (retCode == false) break;
		}
		retCode = sendEnd();
	}
	
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}
/*===================================================================
   ROUTINE: findFirstFile
=================================================================== */
int ACS_ACA_MSDLS_WorkerThread::findFirstFile (string receiveDir, vector<MsgFile>& dataFiles) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	std::string dotfile = receiveDir;
	DIR * dir = 0;
	struct dirent entry;
	struct dirent *result = 0;

	int return_code;

	if ((dir = ::opendir(dotfile.c_str())) == NULL) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the directory, leaving");
		return 1;
	} else {
		string tmpFile("");
		string tmpString("");
		for (return_code = ::readdir_r(dir, &entry, &result);
			         result != NULL && return_code == 0;
			         return_code = ::readdir_r(dir, &entry, &result)){
			if (::strcmp(entry.d_name, ".") && ::strcmp(entry.d_name, "..")) {

				tmpFile.clear();
				tmpString.clear();
				tmpFile = entry.d_name;
				tmpString = dotfile + tmpFile;

				if ((tmpString[tmpString.length() - 1]) != '.') {
					dataFiles.push_back(MsgFile(tmpString, receiveDir));
				}

				struct stat stat_buff;
				::stat(tmpString.c_str(), &stat_buff);

				if ((S_ISDIR(stat_buff.st_mode)) && ((tmpString[tmpString.length() - 1]) != '.')) {
					tmpString = tmpString + "/";
					findFirstFile(tmpString,dataFiles);
				}
			}
		}
		::closedir(dir);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}//End of findFirstFile
//------------------------------------------------------------------------------
//      Process "mslds -m mstore -cp cpname"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_6 (const char * request, /*unsigned reqSize,*/ bool all) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool retCode = false;
	int MaxNoOfRecParameter = 0;
	int MaxRecSizeParameter = 0;
	int MaxNoOfFilesParameter = 0;
	int msize = 65000;
	char message[65000] = {0};
	char tmp_message[65000] = {0};
	struct stat buf;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);

	int lastMessIncorrect = 0;
	ULONGLONG firstMessNo = 0;
	ULONGLONG lastMessNo = 0;
	char msname[16] = {0};
	unsigned short cpid = req->cpID;
	::strcpy(msname, req->msname);
	char mstore[25] = {0};
	ACE_OS::snprintf(mstore, sizeof(mstore) - 1, "%s %s", msname,req->cpsource);
	vector<MsgFile> allDataFiles;
	
	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msname);
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendError(INVALID, mstore); //MSTORE Error
	}

	const ACAMSD_MTAP_ChannelManager * chMgr = msMgr->getMTAPChannelMgrByCP(cpid);
	if (chMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MTAP_ChannelManager pointer is NULL");
		return sendError(INVALID, mstore);//MSTORE Error		
	}

	ACAMSD_MsgStore * msgStore = chMgr->getMsgStore();
	if (msgStore == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MsgStore pointer is NULL");
		return sendError(INVALID, mstore);
	}
    //NO PROBLEM IN MESSAGE STORE
	MaxNoOfRecParameter = msMgr->messageStoreParameters()->noOfRecords;
	MaxRecSizeParameter = msMgr->messageStoreParameters()->recordSize;
	MaxNoOfFilesParameter = msMgr->messageStoreParameters()->noOfFiles;
	//get DataFileDirectoyName and CommitFilename
	string dataDirectoryName = msgStore->getDataFileDirectoryName();
	string commitFileName = msgStore->getCommitFileName();
	ACAMS_CommitFile * commitFile = 0;

	//access to the directory related to mstore and cpname
	ACE_HANDLE dir = ACE_OS::open(dataDirectoryName.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);

	if (dir == ACE_INVALID_HANDLE) { //check dataDirectoryName
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read directory %s, leaving", dataDirectoryName.c_str());
		return sendError(GENERAL_FAULT); //return false;
	}

	// See if there really exists a commit file. If so, create
	// such an object.
	//HANDLE temphand = CreateFile(commitFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	ACE_HANDLE temphand = ACE_OS::open(commitFileName.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
	if (temphand != ACE_INVALID_HANDLE) { //check commit file
		ACE_OS::close(temphand);

		try {
			commitFile = new (std::nothrow) ACAMS_CommitFile(commitFileName);

			if (!commitFile) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for ACAMS_CommitFile object for file '%s'!", commitFileName.c_str());
				ACE_OS::close(dir);
				return sendError(COMMIT_ERROR, commitFileName.c_str());
			}
		}
		catch (std::runtime_error & ex) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a runtime exception for file '%s': error_message == '%s'!",
					commitFileName.c_str(), ex.what());
			delete commitFile; commitFile = 0;
			ACE_OS::close(dir);
			return sendError(COMMIT_ERROR, commitFileName.c_str());
		}
		catch (...) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a generic exception for file '%s'!", commitFileName.c_str());
			delete commitFile; commitFile = 0;
			ACE_OS::close(dir);
			return sendError(COMMIT_ERROR, commitFileName.c_str());
		}
	} else {
		ACE_OS::close(dir);
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read the commit file %s, leaving", commitFileName.c_str());
		return sendError(COMMIT_ERROR, commitFileName.c_str());
	}
	
	// Scan through the directory.
	//WIN32_FIND_DATA currentEntry;
	string dotfile = dataDirectoryName + "/";
	int search_hand = findFirstFile(dotfile.c_str(),allDataFiles);//,&currentEntry);
	if (search_hand == 1) {
		ACE_OS::close(dir);
		delete commitFile; commitFile = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read the directory %s, leaving", dataDirectoryName.c_str());
		sendError(GENERAL_FAULT);
		return false;
	}


	std::sort(allDataFiles.begin(), allDataFiles.end(), less<MsgFile>());
	//FindClose(search_hand);
	ACE_OS::close(dir);
    
	//write informations into buffer
	int noOfDataFiles = static_cast<int>(allDataFiles.size());
	if (noOfDataFiles > 0) {
		//get first msg num
		firstMessNo = allDataFiles[0].firstMsgNum;

		// The sequence searches for the last non-empty file. 
		int lastFileIndex = noOfDataFiles;
		while (lastFileIndex > 0) {
			lastFileIndex--;
			ACE_HANDLE file_hand = ACE_OS::open(allDataFiles[lastFileIndex].name.c_str(),
					GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
			if (file_hand != ACE_INVALID_HANDLE) {
				   // TR HR57598 start
				ACE_OS::fstat(file_hand,&buf);
				if ( buf.st_size > 0) {
					// Check the sanity of the file
					ACE_OS::close(file_hand);
					break;
				}
				// TR HR57598  End	
			}
		} // End of while

		//get last msg num
		if (allDataFiles[lastFileIndex].readLastMsg() > 0)
			lastMessNo = allDataFiles[lastFileIndex].lastMsgNum;
		else
			lastMessIncorrect = 1;
	} else {
		delete commitFile; commitFile = 0;
		if (sendResponse("\nCommon part:\nNo data files available\n"))
			return sendEnd(); //no info in Common part
		else
			return false;
	}

	//info in Common part,format message to send
	ACE_OS::snprintf(message, sizeof(message) - 1, "\nCommon part:\nfirstMessNo: %.20llu\n", firstMessNo);//firstMesNo

	if (lastMessIncorrect == 0) {
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "lastMessNo:  %.20llu\n", lastMessNo); //lastMessage ok
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	} else {
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "lastMessNo:  { Not available }\n"); //last Messagen not ok
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	}

	ULONGLONG lastAck = 0;

	if (commitFile) {
		lastAck = commitFile->getMsgNumber();
		if (lastAck > 0) { //lastAckMess ok
			char row[512] = {0};
			ACE_OS::snprintf(row, sizeof(row) - 1, "lastAckMess: %.20llu\ninfile:      %s/%s\n", lastAck,
					dataDirectoryName.c_str(), commitFile->getFileName().c_str());
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
		} else { //lastAckMess not ok
			char row[512] = {0};
			ACE_OS::snprintf(row, sizeof(row) - 1, "lastAckMess: No message acknowledged yet\n");
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
		}
	}

	char row[512] = {0};

	ACE_OS::snprintf(row, sizeof(row) - 1, "firstFile:   %05u, lastFile:    %05u, noOfFiles:    %05u\n",
			allDataFiles[0].fileNum, allDataFiles[noOfDataFiles-1].fileNum, noOfDataFiles);
	::strcpy(tmp_message, message);
	ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);

	ACE_OS::snprintf(row, sizeof(row) - 1, "MaxNoOfMess: %05u, MaxMessSize: %05u, MaxNoOfFiles: %05u\n",
			MaxNoOfRecParameter, MaxRecSizeParameter, MaxNoOfFilesParameter);
	::strcpy(tmp_message, message);
	ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);

	retCode = sendResponse(message); //sending Message Common Part
	if (retCode) {
		::memset(message, 0, msize); //reset message
	} else {
		delete commitFile; commitFile = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
		return retCode;
	}

	if (commitFile && !all && (lastAck == lastMessNo)) { //No info in File Part:
		ACE_OS::snprintf(message, sizeof(message) - 1, "\nFile part:\nMessage store empty\n");

		//send
		retCode = sendResponse(message);
		if (retCode)
			::memset(message, 0, msize);
		else {
		  delete commitFile; commitFile = 0;
		  ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
		  return retCode; 
		}
	} else { //info in File part
		ACE_OS::snprintf(message, sizeof(message) - 1, "\nFile part:\n");
		//////////////////////////////////
		for (int i = 0; i < noOfDataFiles; i++) {
			ULONGLONG firstInFile = allDataFiles[i].firstMsgNum;
			if ((all == false) && (lastAck > firstInFile)) {
				continue;
			}
			
			if (allDataFiles[i].readLastMsg() == -1) { // The file is read
				char denied[512] = {0};
				ACE_OS::snprintf(denied, sizeof(denied) - 1, " permission denied\n");	// It could not be read
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, denied);

				//send error message
				retCode = sendResponse(message);
				if (retCode) {
					::memset(message, 0, msize);
				} else {
					delete commitFile; commitFile = 0;
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
					return retCode;
				}

				continue;
			}
			
			char one[512] = {0};
			ACE_OS::snprintf(one, sizeof(one) - 1, "%05u: filename: %s\n", i, allDataFiles[i].name.c_str());
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, one); //filename

			int eof_found = allDataFiles[i].eofFound;
			if (eof_found || allDataFiles[i].eomFound) {
				char two[512] = {0};
				ACE_OS::snprintf(two, sizeof(two) - 1, "       firstMessInFile: %020llu, noOfMess: %05u\n",
						firstInFile, allDataFiles[i].noOfMsgs);
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, two); //firstMessInFile
			} else {	// File might not be complete.
				char two[512] = {0};
				ACE_OS::snprintf(two, sizeof(two) - 1, "       firstMessInFile: %.20llu, noOfMess: Unknown. File %s.\n",
						firstInFile, eof_found ? "corrupt" : "not complete");
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, two);
			}
			
			char three[512] = {0};
			ACE_OS::snprintf(three, sizeof(three) - 1, "       unlinked: %s, MTAPFileNo: %03u\n", eof_found? "Yes" : " No",
					allDataFiles[i].fileNum);
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, three); //unlinked

			//sending File part message
			retCode = sendResponse(message);
			if (retCode)
				::memset(message, 0, msize);
			else {
				delete commitFile; commitFile = 0;
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
				return retCode;
			}
		}
	}
	
	retCode = sendEnd();
	delete commitFile; commitFile = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Process "mslds -m mstore -cp cpname -a"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_14 (const char * request /*, unsigned reqSize*/) {
	return process_6(request, /*reqSize,*/ true);
}

//------------------------------------------------------------------------------
//      Process "mslds -m mstore -cp cpname -S"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_134 (const char * request /*, unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool retCode = false;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);

	char msname[16] = {0};
	unsigned short cpid = req->cpID;
	::strcpy(msname, req->msname);
	char mstore[25] = {0};
	ACE_OS::snprintf(mstore, sizeof(mstore) - 1, "%s  %s\n", req->msname, req->cpsource);

	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msname);
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendError(INVALID, mstore);//MSTORE PROBLEM
	}

	const ACAMSD_MTAP_ChannelManager * chMgr = msMgr->getMTAPChannelMgrByCP(cpid);
	if (chMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MTAP_ChannelManager pointer is NULL");
		return sendError(INVALID, mstore); //MSTORE PROBLEM
	}
	
	ACAMSD_MsgStore * msgStore = chMgr->getMsgStore();
	if (msgStore == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MsgStore pointer is NULL");
		return sendError(INVALID, mstore); //MSTORE PROBLEM
	}
	//NO problem in MStore

	char message[512] = {0};
	ACAMS_StatisticsFile * stats = msgStore->getStatisticsData();
	if (stats == 0) {
		string stFile = msgStore->getDataFileDirectoryName() + ACAMS_NameOfStatisticsFile;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMS_StatisticsFile pointer for file %s is NULL", stFile.c_str());
		return sendError(STATISTICS_ERROR, stFile.c_str());
	} //STAT. FILE PROBLEM
	
	//SYSTEMTIME
	struct tm cTime = stats->getCreationTime();
	int lostMsg = stats->getNumberOfLostMsg(); //lostMsg
	int skippedMsg = stats->getNumberOfSkippedMsg();//skippedMSG
	ULONGLONG RecMsg = stats->getNumberOfMsg();//RecMsg
	ULONGLONG RecBytes = stats->getNumberOfBytes(); //recBytes
	ACE_OS::snprintf (message, sizeof(message) - 1,
			"Created: %s %s %.2d %.2d:%.2d:%.2d %d\nlostMsg:\t%.5d\nskippedMsg:\t%.5d\nRecMsg:\t\t%.20llu\nRecBytes:\t%.20llu\n",
			getDay(cTime.tm_wday).c_str(), getMonth(cTime.tm_mon).c_str(),
			cTime.tm_mday, cTime.tm_hour, cTime.tm_min, cTime.tm_sec, cTime.tm_year,
			lostMsg, skippedMsg, RecMsg, RecBytes);
    //sending Message
	retCode = sendResponse(message);

	if (retCode) retCode = sendEnd();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Process "mslds -m mstore -s site -S"  unused format
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_388 (const char * request /*, unsigned reqSize*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool retCode = false;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);

	char msname[16] = {0};
	char cpsite[16] = {0};

	::strcpy(msname, req->msname);
	::strcpy(cpsite, req->cpsource);
	char mstore[25] = {0};
	ACE_OS::snprintf(mstore, sizeof(mstore) - 1, "%s %s", msname, cpsite);

	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msname);
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendError(INVALID, mstore); //MSTORE ERROR
	}

	::strcpy(cpsite, msMgr->messageStoreParameters()->site);
	char message[512] = {0};
	string statisticsFile = getSingleCPStatisticsFile(msname, cpsite);

	ACAMS_StatisticsFile stats(statisticsFile);

	struct tm  cTime = stats.getCreationTime();
	int lostMsg = stats.getNumberOfLostMsg();
	int skippedMsg = stats.getNumberOfSkippedMsg();
	ULONGLONG RecMsg = stats.getNumberOfMsg();
	ULONGLONG RecBytes = stats.getNumberOfBytes();

	ACE_OS::snprintf(message, sizeof(message) - 1,
			"Created: %s %s %.2d %.2d:%.2d:%.2d %d\nlostMsg:\t%.5d\nskippedMsg:\t%.5d\nRecMsg:\t\t%020llu\nRecBytes:\t%020llu\n",
			getDay(cTime.tm_wday).c_str(), getMonth(cTime.tm_mon).c_str(),
			cTime.tm_mday, cTime.tm_hour, cTime.tm_min, cTime.tm_sec, cTime.tm_year,
			lostMsg, skippedMsg, RecMsg, RecBytes);

	retCode = sendResponse(message);

	if (retCode) retCode = sendEnd();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//------------------------------------------------------------------------------
//      Utility methods
//------------------------------------------------------------------------------
string ACS_ACA_MSDLS_WorkerThread::getDay (ACE_UINT16 wDayOfWeek) {
	// Determine day of the week.
	string day;
	switch (wDayOfWeek) {
	case 0:	day = "Sun"; break;
	case 1:	day = "Mon"; break;
	case 2:	day = "Tue"; break;
	case 3:	day = "Wed"; break;
	case 4:	day = "Thu"; break;
	case 5:	day = "Fri"; break;
	case 6:	day = "Sat"; break;
	}
	
	return day;
}

string ACS_ACA_MSDLS_WorkerThread::getMonth (ACE_UINT16 wMonth) {
	// Determine day of the week.
	string month;
	switch (wMonth) {
	case 1:	month  = "January";		break;
	case 2:	month  = "February"; 	break;
	case 3:	month  = "March"; 		break;
	case 4:	month  = "April"; 		break;
	case 5:	month  = "May"; 			break;
	case 6:	month  = "June"; 			break;
	case 7:	month  = "July"; 			break;
	case 8:	month  = "August"; 		break;
	case 9:	month  = "September";	break;
	case 10: month = "October"; 	break;
	case 11: month = "November"; 	break;
	case 12: month = "December"; 	break;
	}
	
	return month;
}

//------------------------------------------------------------------------------
//      Send methods
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::sendEnd () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	ACS_ACA_StructuredResponseBuilder builder;
	response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, END, 0, 0);
	unsigned timeout = 3 * 1000;	// 3 seconds

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
				_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

bool ACS_ACA_MSDLS_WorkerThread::sendError (unsigned int code, const char * reason) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	ACS_ACA_StructuredResponseBuilder builder;
	unsigned timeout = 3 * 1000;	// 3 seconds
	
	if (reason == 0)
		response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, code, 0, 0);
	else
		response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, code, ::strlen(reason), reason);

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
				_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

bool ACS_ACA_MSDLS_WorkerThread::sendInvalidError (const char * reason) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	ACS_ACA_StructuredResponseBuilder builder;
	unsigned timeout = 3 * 1000;	// 3 seconds

	if (reason == 0)
		response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, INVALID, 0, 0);
	else
		response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, INVALID, ::strlen(reason), reason);

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
					_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

bool ACS_ACA_MSDLS_WorkerThread::sendCSError_55 () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	ACS_ACA_StructuredResponseBuilder builder;
	response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, CS_ERROR55, 0, 0);
	unsigned timeout = 3 * 1000;	// 3 seconds

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
					_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

bool ACS_ACA_MSDLS_WorkerThread::sendCSError_56 () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	ACS_ACA_StructuredResponseBuilder builder;
	response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, CS_ERROR56, 0, 0);
	unsigned timeout = 3 * 1000;	// 3 seconds

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
				_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

bool ACS_ACA_MSDLS_WorkerThread::sendResponse (const char * data, unsigned short cpID) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	void * response = 0;
	unsigned int respSize = 0;
	unsigned int data_length = ::strlen(data) + 1;
	ACS_ACA_StructuredResponseBuilder builder;
	unsigned timeout = 3 * 1000;	// 3 seconds

	if (data_length == 0) return true;

	response = ACS_ACA_MSGDirector::buildStructuredResponse(&builder, respSize, ++seqNo, OK, data_length, data, cpID);

	//send response
	if ((_session->sendf(response, respSize, timeout, MSG_NOSIGNAL)) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendf' failed, error_code = %d, error '%s', returning false",
				_session->last_error(), _session->last_error_text());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// msdls -m message_store -cp cpname -d file [-t | -n]  unused format // Retained for future purposes
//////////////////////////////////////////////////////////////////////////////////
bool ACS_ACA_MSDLS_WorkerThread::process_22 (const char * request, /* unsigned reqSize,*/ bool num, bool trunc) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	bool retCode = true;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);

	string inputFileName = req->data;
	string msName = req->msname;
	unsigned short cpId = req->cpID;
	char mstore[25] = {0};
	ACE_OS::snprintf(mstore, sizeof(mstore) - 1, "%s %s", msName.c_str(), req->cpsource);

	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msName.c_str());
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendError(INVALID, mstore);
	}

	const ACAMSD_MTAP_ChannelManager * chMgr = msMgr->getMTAPChannelMgrByCP(cpId);
	if (chMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MTAP_ChannelManager pointer is NULL");
		return sendError(INVALID, mstore);
	}

	ACAMSD_MsgStore * msgStore = chMgr->getMsgStore();
	if (msgStore == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MsgStore pointer is NULL");
		return sendError(INVALID, mstore);
	}
	//Get dataDirectoryName
	string dataDirectoryName = msgStore->getDataFileDirectoryName();
    //Get fileName
	string fileName = dataDirectoryName + inputFileName;

	if (loadDataFile(fileName)) { //fornat ans send message
		if (displayData(fileName, num, trunc) < 0)
			retCode = false;
	}
	closeDataFile();

	sendEnd();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

//////////////////////////////////////////////////////////////////////////////////
// msdls -m message_store -s site -d file [-t | -n]
//////////////////////////////////////////////////////////////////////////////////
bool ACS_ACA_MSDLS_WorkerThread::process_276 (const char * request, /* unsigned reqSize,*/ bool num, bool trunc) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool retCode = true;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t *>(request);
	char msname[16] = {0};
	char cpsite[16] = {0};
	string inputFileName = req->data;
    //get msname and cpsource
	::strcpy(msname, req->msname);
	::strcpy(cpsite, req->cpsource);
	char mstore[50] = {0};
	ACE_OS::snprintf(mstore, sizeof(mstore) - 1, "%s %s",msname,cpsite);//format mstore

	//get MSManager
	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msname);
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendError(INVALID, mstore);
	}

	::strcpy(cpsite, msMgr->messageStoreParameters()->site);
	//make directory by msname and cpsite
	string dataDirectoryName = getSingleCPDataDirectory(msname, cpsite);
	//get filename
	string fileName = dataDirectoryName + "/" + inputFileName;

	//load file
	if (loadDataFile(fileName)) {
		if (displayData(fileName, num, trunc) < 0)
			retCode = false;
	}
	closeDataFile();
   
	sendEnd();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

bool ACS_ACA_MSDLS_WorkerThread::loadDataFile (const string & fileName) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	// Open the file.

	//struct __stat64 buf;
	struct stat buf;
	int result = ::stat(fileName.c_str(), &buf);
	
	if( result != 0 ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'stat' failed for file %s, errno = %d", fileName.c_str(), ACE_OS::last_error());
		sendError(DATA_FILE_ERROR, fileName.c_str());
		return false;
	} else {
		SizeDataFile = _file_map_size = buf.st_size;
	}

	//open the file
	ACE_HANDLE dfile_hand = ACE_OS::open(fileName.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
	if (dfile_hand == ACE_INVALID_HANDLE) { //filename not ok
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the file %s, errno = %d", fileName.c_str(), ACE_OS::last_error());
		sendError(DATA_FILE_ERROR, fileName.c_str());
		return false;
	}

	// map a view of the file
	if ((file_view = ::mmap(0, _file_map_size, PROT_READ, MAP_SHARED, (int)dfile_hand, 0)) == MAP_FAILED) {
		// failed to make a view of the data file in memory.
		sendError(DATA_FILE_ERROR, fileName.c_str());
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mmap' failed for file %s, errno = %d", fileName.c_str(), ACE_OS::last_error());
		return false;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

void ACS_ACA_MSDLS_WorkerThread::closeDataFile () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (dfile_hand != ACE_INVALID_HANDLE) {
		if ((file_view != MAP_FAILED) && ::munmap(file_view, _file_map_size)) { // ERROR: unmapping the file
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'munmap' failed, errno = %d", ACE_OS::last_error());
			sendError(DATA_FILE_ERROR);
		}
		MapDataFile = ACE_INVALID_HANDLE;
	
		if (!ACE_OS::close(dfile_hand)) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'close' failed, errno = %d", ACE_OS::last_error());
			sendError(GENERAL_FAULT);
		}
	
		dfile_hand = ACE_INVALID_HANDLE;
	}
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

int ACS_ACA_MSDLS_WorkerThread::displayData (const string & fileName, bool num, bool trunc) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	enum State  { Empty, Filled, Lost, Skipped };
	static const char * stateText[4] = {"Empty","Filled","Lost","Skipped" };
	const int MessageTail = 8;
	unsigned char * dataPtr = 0;
	unsigned char * msgData = 0;
	ULONGLONG	msgNumber = 0;
	unsigned long msgSize = 0;
	ULONGLONG	thisNumber = 0;
	ULONGLONG	previousNumber = 0;
	//ULONGLONG	one = 1;
	State state = Empty;
	int msgInFile = 0;
	unsigned long	bytesDone = 0;
	bool zeroMessage = false;
	unsigned char tempv[4];
	int additionalData = 0;
	int versionNumber = 0;
	char tmp[ACS_DSD_MAXBUFFER] = {0};
	int msize = ACS_DSD_MAXBUFFER;
	char message[ACS_DSD_MAXBUFFER] = {0};

	do {
		// Must check if we have enough data for message number and size.
		if ((SizeDataFile - bytesDone) < static_cast<unsigned int>(ACAMS_MsgHeaderSize)) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Unexpected EOF in data file %s, leaving", fileName.c_str());
			return sendError(EOF_FILE, fileName.c_str());
		}          
	
		// Move to current message.
		if (dataPtr == 0) {
			// First time.
			dataPtr = reinterpret_cast<unsigned char *>(file_view);
		} else {
			dataPtr += ACAMS_FileOverhead+msgSize;
		}

		// Get message number and size and position pointer into message.
		msgNumber = convertToUll(dataPtr);//_atoi64((char*)dataPtr);//memcpy((void*)&msgNumber,(const void*)dataPtr,8);
		::memcpy(&msgSize, (dataPtr + 8), sizeof(unsigned long)/2);

//	msgSize=convertToUll(dataPtr);
		msgSize -= MessageTail;
		msgData = dataPtr + ACAMS_MsgHeaderSize;
		bytesDone += ACAMS_MsgHeaderSize;

		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "File: %s, msgSize: %lu, fileSize: %llu, bytesDone: %lu",
				fileName.c_str(), msgSize, SizeDataFile, bytesDone);

		// See if all message is in file.
		if ((SizeDataFile - bytesDone) < msgSize) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Truncated data in data file %s, leaving", fileName.c_str());
			return sendError(DATA_FILE_ERROR, fileName.c_str());
		}

		::memcpy(tempv, (dataPtr + 12 +msgSize), 4);

		additionalData = tempv[0]; //additional data
		versionNumber = tempv[1]; //version number
		versionNumber = versionNumber >> 1;

		if (versionNumber == 0) { //if (tempv[3]==0)
			//version 2
			additionalData = 0;
		}
		bytesDone += msgSize + MessageTail;

		// Swap message numbers.

		if (previousNumber == 0) {
			size_t pos = fileName.find_last_of("/\\");
			const char * ptr = fileName.c_str() + pos + 1;
	    		
			// First time. Must fake a previous number.
			thisNumber = ::atoi(ptr);
			
			//==> 2^32 + 2^0 = 4294967297
			if (!thisNumber) previousNumber = thisNumber;
			else thisNumber--;
		}

		if (thisNumber > 0) { //if ((halves[0] > 0) || (halves[1] > 0))
			previousNumber = thisNumber;
		}

		thisNumber = msgNumber;//new ACS_ACA_MessageNumber(msgNumber.octets );  

		zeroMessage = !previousNumber || !thisNumber;


		// Find out state of current message.
		while (!zeroMessage && ((thisNumber - 1) > previousNumber)) {
			// There is a hole in the sequence! Messages are lost!
			// These lost messages are not stored at all.
			// Must fake and print them!
			++previousNumber;
			// Print basic info about lost message:
            
			char row[512] = {0};
			ACE_OS::snprintf(row, sizeof(row) - 1, "%05u: MessNo: %.20llu, MessSize: 0000000000, messState: Lost\n",
					msgInFile++, previousNumber);
			ACE_OS::strcpy(tmp,message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, row);
		}

		state = msgSize ? Filled : Skipped;

    	    
		// Print basic info about message:

		char one[512] = {0};
		
		::snprintf(one, sizeof(one) - 1, "%05u: MessNo: %020llu, MessSize: %010lu, messState: %s\n ",
				msgInFile++, msgNumber, msgSize, stateText[state]);
		::strcpy(tmp, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, one);

		char two[512] = {0};
		ACE_OS::snprintf(two, sizeof(two) - 1, " Version:%d, AdditionalData:%d\n",
				versionNumber, additionalData);
		ACE_OS::strcpy(tmp, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, two);
           
		// Print some of its contents.    
		if (!num) { // if opt != -n
			for (unsigned int j = 0; j < msgSize; j += 16) {
				char three[512] = {0};
				unsigned long k = j;

				ACE_OS::snprintf(three, sizeof(three) - 1, "       %05X: ", j);

				ACE_OS::strcpy(tmp,message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, three);

				for (k = j; (k < msgSize) && ((k - j) < 16); k++) {
					char four[512] = {0};
					ACE_OS::snprintf(four, sizeof(four) - 1, "%02X ", *(msgData + k));
					ACE_OS::strcpy(tmp,message);
					ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, four);
				}

				ACE_OS::strcpy(tmp, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, "   ");

				for (k = j; (k < msgSize) && ((k - j) < 16); k++) {
					char six[512] = {0};
					ACE_OS::snprintf(six, sizeof(six) - 1, "%c", (::isprint(*(msgData + k)) ? *(msgData + k) : '.'));
					ACE_OS::strcpy(tmp, message);
					ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, six);
				}

				//printf("\n");
				ACE_OS::strcpy(tmp, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, "\n");

				if ((j > (1024 * 1024)) || ((msgSize > 16) && trunc)) { // -t
					char seven[512] = {0};
					ACE_OS::snprintf(seven, sizeof(seven) - 1, "       Printout truncated\n" );
					ACE_OS::strcpy(tmp, message);
					ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp, seven);
					break;
				}

				if (!sendResponse(message)) return (-1);
				::memset(message, 0, msize);
			}
		}

		if (!sendResponse(message)) return (-1);
		::memset(message, 0, msize);
	} while((bytesDone + ACAMS_Eof_Mark_Length) < SizeDataFile);

	//Delete the Map after it is used!
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
} // End of displayData


//------------------------------------------------------------------------------
//      query for multiple/single cp system
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::isMultipleCPSystem () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	// Check if multiple CP system
	bool isMultipleCPSystem = false;
	ACS_ACA_CS_API::CS_API_Result_ACA result =
			static_cast<ACS_ACA_CS_API::CS_API_Result_ACA>(ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem));
	
	if (result == ACS_ACA_CS_API::Result_Success) {
		ACS_ACA_LOG(LOG_LEVEL_INFO, "The system configuration is %s, leaving",
				(isMultipleCPSystem) ? "MCP" : "SCP");
		return isMultipleCPSystem;

	} else {
		switch (result) {
		case ACS_ACA_CS_API::Result_NoEntry:
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' returned Result_NoEntry, throwing ax exception");
			throw ACS_ACA_CS_API::Result_NoEntry;
			break;

		case ACS_ACA_CS_API::Result_NoValue:
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' returned Result_NoValue, throwing ax exception");
			throw ACS_ACA_CS_API::Result_NoValue;
			break;

		case ACS_ACA_CS_API::Result_NoAccess:
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' returned Result_NoAccess, throwing ax exception");
			throw ACS_ACA_CS_API::Result_NoAccess;
			break;

		case ACS_ACA_CS_API::Result_Failure:
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'isMultipleCPSystem' returned Result_Failure, throwing ax exception");
			throw ACS_ACA_CS_API::Result_Failure;
			break;

		default:
			break;
		}
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return false;
	}
}

//------------------------------------------------------------------------------
//      Process "mslds -m mstore -s site"
//------------------------------------------------------------------------------
bool ACS_ACA_MSDLS_WorkerThread::process_260 (const char * request, /* unsigned reqSize,*/ bool all) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	bool retCode = false;
	int MaxNoOfRecParameter = 0;
	int MaxRecSizeParameter = 0;
	int MaxNoOfFilesParameter = 0;
	int msize = 65000;
	char message[65000] = {0};
	char tmp_message[65000] = {0};
	struct stat buf;
	const aca_msdls_req_t * req = reinterpret_cast<const aca_msdls_req_t*>(request);
	int lastMessIncorrect = 0;
	ULONGLONG firstMessNo = 0;
	ULONGLONG lastMessNo = 0;
	char msname[16] = {0};
	char cpsite[16] = {0};
	vector<MsgFile> allDataFiles;

	::strcpy(msname, req->msname);
	::strcpy(cpsite, req->cpsource);

	ACAMSD_MS_Manager * msMgr = ACS_MSD_Service::getMSManagerByMsgStore(msname);
	if (msMgr == 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return sendInvalidError();
	}
	::strcpy(cpsite, msMgr->messageStoreParameters()->site);


	MaxNoOfRecParameter = msMgr->messageStoreParameters()->noOfRecords;
	MaxRecSizeParameter = msMgr->messageStoreParameters()->recordSize;
	MaxNoOfFilesParameter = msMgr->messageStoreParameters()->noOfFiles;
	
	std::string dataDirectoryName = getSingleCPDataDirectory(msname, cpsite);
	std::string commitFileName = getSingleCPCommitFileName(msname, cpsite);

	ACAMS_CommitFile * commitFile = 0;
	ACE_HANDLE dir = ACE_OS::open(dataDirectoryName.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
	
	if (dir == ACE_INVALID_HANDLE) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the directory %s, errno = %d",
				dataDirectoryName.c_str(), ACE_OS::last_error());
		sendInvalidError();
		return false;
	}
	
	ACE_HANDLE temphand = ACE_OS::open(commitFileName.c_str(), GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
	if (temphand != ACE_INVALID_HANDLE) {
		ACE_OS::close(temphand);
		ACE_OS::close(dir);

		try {
			commitFile = new (std::nothrow) ACAMS_CommitFile(commitFileName);

			if (!commitFile) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for ACAMS_CommitFile object for file '%s'!", commitFileName.c_str());
				sendInvalidError();
				return false;
			}
		}
		catch (std::runtime_error & ex) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a runtime exception for file '%s': error_message == '%s'!",
					commitFileName.c_str(), ex.what());
			delete commitFile; commitFile = 0;
                        sendError(COMMIT_ERROR, commitFileName.c_str());
			return false;
		}
		catch (...) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a generic exception for file '%s'!", commitFileName.c_str());
			delete commitFile; commitFile = 0;
                        sendError(COMMIT_ERROR, commitFileName.c_str());
			return false;
		}
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Commit file %s correctly created", commitFileName.c_str());
	} else {
		ACE_OS::close(dir);
		sendInvalidError();
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to open the commit file %s, errno = %d, leaving",
				commitFileName.c_str(), ACE_OS::last_error());
		return false;
	}

	// Scan through the directory.
	std::string dotfile = dataDirectoryName;
	int search_hand = findFirstFile(dotfile.c_str(),allDataFiles);
	if (search_hand == 1) {
		delete commitFile; commitFile = 0;
		ACE_OS::close(dir);
		sendInvalidError();
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to read the directory %s, leaving", dataDirectoryName.c_str());
   	return false;
	}

	std::sort(allDataFiles.begin(), allDataFiles.end(), less<MsgFile>());
	ACE_OS::close(dir);
    
	//write informations into buffer
	int noOfDataFiles = allDataFiles.size();
	
	if (noOfDataFiles > 0) {
		//get first msg num
		firstMessNo = allDataFiles[0].firstMsgNum;

		// The sequence searches for the last non-empty file. 
		int lastFileIndex = noOfDataFiles;
		while (lastFileIndex > 0) {
			lastFileIndex--;
			ACE_HANDLE file_hand = ACE_OS::open(allDataFiles[lastFileIndex].name.c_str(),
					GENERIC_READ|OPEN_EXISTING, ACE_DEFAULT_OPEN_PERMS);
			if (file_hand != ACE_INVALID_HANDLE) {
				// TR HR57598 start
				ACE_OS::fstat(file_hand,&buf);
				if ( buf.st_size > 0) {
					// Check the sanity of the file
					ACE_OS::close(file_hand);
					break;
				}
				// TR HR57598 End
			}
		} // End of while

		//get last msg num
		if (allDataFiles[lastFileIndex].readLastMsg() > 0) {
			lastMessNo = allDataFiles[lastFileIndex].lastMsgNum;
		} else {
			lastMessIncorrect = 1;
		}
	} else {
		if (sendResponse("\nCommon part:\nNo data files available\n")) {
			delete commitFile; commitFile = 0;
			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Response sent, leaving");
			return sendEnd();
		} else {
			delete commitFile; commitFile = 0;
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
			return false;
		}
	}

	ACE_OS::snprintf(message, sizeof(message) - 1, "\nCommon part:\nfirstMessNo: %020llu\n", firstMessNo);

	if (lastMessIncorrect == 0) {
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "lastMessNo:  %020llu\n", lastMessNo);
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	} else {
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "lastMessNo:  { Not available }\n");
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	}

	ULONGLONG lastAck = 0;

	if (commitFile) {
		lastAck = commitFile->getMsgNumber();
		if (lastAck > 0) {
			char row[512] = {0};
			ACE_OS::snprintf(row, sizeof(row) - 1, "lastAckMess: %020llu\ninfile:      %s%s\n", lastAck,
					dataDirectoryName.c_str(), commitFile->getFileName().c_str());
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
		} else {
			char row[512] = {0};
			ACE_OS::snprintf(row, sizeof(row) - 1, "lastAckMess: No message acknowledged yet\n");
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message , row);
		}
	}

	{
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "firstFile:   %05u, lastFile:    %05u, noOfFiles:    %05u\n",
				allDataFiles[0].fileNum, allDataFiles[noOfDataFiles - 1].fileNum, noOfDataFiles);
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	}

	{
		char row[512] = {0};
		ACE_OS::snprintf(row, sizeof(row) - 1, "MaxNoOfMess: %05u, MaxMessSize: %05u, MaxNoOfFiles: %05u\n",
				MaxNoOfRecParameter, MaxRecSizeParameter, MaxNoOfFilesParameter);
		::strcpy(tmp_message, message);
		ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, row);
	}

	retCode = sendResponse(message);
	if (retCode) {
		::memset(message, 0, msize);
	} else {
		delete commitFile; commitFile = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
		return retCode;
	}

	if (commitFile && (!all) && (lastAck == lastMessNo)) {
		ACE_OS::snprintf(message, sizeof(message) - 1, "\nFile part:\nMessage store empty\n" );

		//send
		retCode = sendResponse(message);
		if (retCode) {
			::memset(message, 0, msize);
		} else {
			delete commitFile; commitFile = 0;
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
			return retCode;
		}
	} else {
		ACE_OS::snprintf(message, sizeof(message) - 1, "\nFile part:\n");

		for (int i = 0; i < noOfDataFiles; i++) {
			ULONGLONG firstInFile = allDataFiles[i].firstMsgNum;
			if ((all == false) && (lastAck > firstInFile)) {
				continue;
			}

			if (allDataFiles[i].readLastMsg() == -1) { // The file is read
				char denied[512] = {0};
				ACE_OS::snprintf(denied, sizeof(denied) - 1, "       permission denied\n" );	// It could not be read
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s",tmp_message , denied);

				//send
				retCode = sendResponse(message);
				if (retCode) {
					::memset(message, 0, msize);
				} else {
					delete commitFile; commitFile = 0;
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
					return retCode;
				}
				
				continue;
			}
			
			char one[512] = {0};
			ACE_OS::snprintf(one, sizeof(one) - 1, "%05u: filename: %s\n", i, allDataFiles[i].name.c_str());
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, one);

			int eof_found = allDataFiles[i].eofFound;
			if (eof_found || allDataFiles[i].eomFound) {
				char two[512] = {0};
				ACE_OS::snprintf(two, sizeof(two) - 1, "       firstMessInFile: %020llu, noOfMess: %05u\n",
						firstInFile, allDataFiles[i].noOfMsgs );
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, two);
			} else {	// File might not be complete.
				char two[512] = {0};
				ACE_OS::snprintf(two, sizeof(two) - 1, "       firstMessInFile: %020llu, noOfMess: Unknown. File %s.\n",
						firstInFile, eof_found ? "corrupt" : "not complete");
				::strcpy(tmp_message, message);
				ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, two);
			}
			
			char three[512] = {0};
			ACE_OS::snprintf(three, sizeof(three) - 1, "       unlinked: %s, MTAPFileNo: %03u\n",
				eof_found ? "Yes" : " No", allDataFiles[i].fileNum);
			::strcpy(tmp_message, message);
			ACE_OS::snprintf(message, sizeof(message) - 1, "%s%s", tmp_message, three);

			//send
			retCode = sendResponse(message);
			if (retCode) {
				::memset(message, 0, msize);
			} else {
				delete commitFile; commitFile = 0;
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'sendResponse' failed, leaving");
				return retCode;
			}
		}
	}

	retCode = sendEnd();

	delete commitFile; commitFile = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return retCode;
}

string ACS_ACA_MSDLS_WorkerThread::getSingleCPDataDirectory (const char * msname, const char * cpsite) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	std::string dataDirectoryName;
	char msdHome[512] = {0};
	int path_len = ACS_ACA_ARRAY_SIZE(msdHome);

	if (ACS_ACA_ConfigurationHelper::get_data_disk_path(msdHome, path_len) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'GetDataDiskPath' failed, unable to read data disk path");
		return dataDirectoryName;
	}

	dataDirectoryName = msdHome;
	dataDirectoryName += "/";
	dataDirectoryName += msname;
	dataDirectoryName += "/";
	dataDirectoryName += cpsite;
	dataDirectoryName += "/";
	dataDirectoryName += ACAMS_NameOfDataFileDir;
	dataDirectoryName += "/";

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return dataDirectoryName;
}

std::string ACS_ACA_MSDLS_WorkerThread::getSingleCPCommitFileName (const char * msname, const char * cpsite) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	std::string commitFileName;
	char msdHome[512] = {0};
	int path_len = ACS_ACA_ARRAY_SIZE(msdHome);

	if (ACS_ACA_ConfigurationHelper::get_data_disk_path(msdHome, path_len) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'GetDataDiskPath' failed, unable to read data disk path");
		return commitFileName;
	}

	commitFileName = msdHome;
	commitFileName += "/";
	commitFileName += msname;
	commitFileName += "/";
	commitFileName += cpsite;
	commitFileName += "/";
	commitFileName += ACAMS_NameOfCommitFile;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return commitFileName;
}

std::string ACS_ACA_MSDLS_WorkerThread::getSingleCPStatisticsFile (const char * msname, const char * cpsite) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	std::string statisticsFile;
	char msdHome[512] = {0};
	int path_len = ACS_ACA_ARRAY_SIZE(msdHome);

	if (ACS_ACA_ConfigurationHelper::get_data_disk_path(msdHome, path_len) < 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'GetDataDiskPath' failed, unable to read data disk path");
		return statisticsFile;
	}

	statisticsFile = msdHome;
	statisticsFile += "/";
	statisticsFile += msname;
	statisticsFile += "/";
	statisticsFile += cpsite;
	statisticsFile += "/";
	statisticsFile += ACAMS_NameOfStatisticsFile;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return statisticsFile;
}
