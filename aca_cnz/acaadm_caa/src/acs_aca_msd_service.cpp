//******************************************************************************
//
//  NAME
//     acs_aca_msd_service.cpp
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
//     2012-10-08 by XLANSRI
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <stdexcept>

#include "ace/ACE.h"
#include "ace/Handle_Set.h"
#include "ace/Guard_T.h"

#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_AmfTypes.h"
#include "ACS_APGCC_ApplicationManager.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "ACS_APGCC_CommonLib.h"

#include "acs_aca_macros.h"
#include "acs_aca_common.h"
#include "acs_aca_msdls_server.h"
#include "acs_aca_utility.h"
#include "acs_aca_cptable_observer.h"
#include "acs_aca_logger.h"
#include "acs_aca_root_objectImpl.h"
#include "acs_aca_configuration_helper.h"
#include "acs_aca_msd_service.h"

//#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace alarmSeverity;

/////////////////////////////////
// Define declaration section
/////////////////////////////////
#define LOAD_MS_PARAMETERS_RETRIES 400
#define LOAD_MS_PARAMETERS_TIMEOUT 1000 * 10 //ten seconds
#define CHECK_MULTIPLE_CP_RETRIES 400
#define CHECK_MULTIPLE_CP_TIMEOUT 1000 * 10 //ten seconds
#define GET_CP_IDS_RETRIES 400
#define GET_CP_IDS_TIMEOUT 1000 * 10 //ten seconds
#define CHECK_DATA_DISK_RETRIES 3
#define CHECK_DATA_DISK_TIMEOUT_MS 400 //milliseconds
#define CS_API_SUCCESS 1
#define CS_API_FAILURE 0 


/////////////////////////////////
//Static members initializations
/////////////////////////////////
std::string ACS_MSD_Service::parentObjDNofACA = "";
std::string ACS_MSD_Service::dnObject = "";
ACE_RW_Thread_Mutex ACS_MSD_Service::_ms_parameters_access_sync;
ACAMS_Parameters ** ACS_MSD_Service::gParams = 0;
bool ACS_MSD_Service::_serviceMode = true;
int ACS_MSD_Service::_messageStoreNum = 8;
ACE_Event * ACS_MSD_Service::_stopEvent = 0;
bool ACS_MSD_Service::_stopRequested = false;
ACAMSD_MS_Manager ** ACS_MSD_Service::_msManagers = 0;
std::vector<unsigned> ACS_MSD_Service::_cpIds;
ACE_Recursive_Thread_Mutex ACS_MSD_Service::_cpIds_sync;
bool ACS_MSD_Service::_isMultipleCP = false;
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
bool ACS_MSD_Service::_isOSCachingEnabled = false;
#endif

ACS_MSD_Service::ACS_MSD_Service ()
: _root_worker_thread_id(0), _IMM_thread_id(0), m_poMsgStoreCommandHandler(0) {
	_stopRequested = false;
}

ACE_THR_FUNC_RETURN threadStartupMSDLS (void * param) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_MSDLS_Server * msdls = reinterpret_cast<ACS_ACA_MSDLS_Server*>(param);
	msdls->_state = THREAD_STATE_RUNNING;
	msdls->workerMain();
	msdls->_state = THREAD_STATE_STOPPED;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}


//****************************************************************
// serviceMain
//****************************************************************
//This is the start of the service.
void ACS_MSD_Service::serviceMain () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Root Worker Thread, starting to work!");

	ACS_MSD_Service::_stopEvent = new (std::nothrow) ACE_Event(true, true, USYNC_THREAD, NULL);
	if (!ACS_MSD_Service::_stopEvent) { //ERROR: event not created!
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for _stopEvent object, leaving");
		return;
	}

	if(checkDataDisk(CHECK_DATA_DISK_RETRIES, CHECK_DATA_DISK_TIMEOUT_MS))
	{
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'checkDataDisk' failed, leaving");
		StopServer();
		return;
	}

	const int call_result = load_ms_parameters_from_IMM();
	if ( call_result ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'load_ms_parameters_from_IMM' failed with error code %d, leaving", call_result);
		StopServer();
		return;
	}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	const int return_code = checkDiskReintegrationOperationOngoing();
	if (return_code != 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to retrieve the information about if the disk reintegration operation "
				"is ongoing, assuming that it's not running, return_code == %d!", return_code);
	}
#endif

	if (StartOIThreadsOnActiveNode() == ACS_CC_FAILURE ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while creating threads handling OI callbacks, leaving");
		StopServer();
		return;
	}

	if (fxCheckMultipleCP(CHECK_MULTIPLE_CP_RETRIES, CHECK_MULTIPLE_CP_TIMEOUT)) {
		delete _stopEvent; _stopEvent = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxCheckMultipleCP' failed, leaving");
		return;
	}

	if (fxGetCpIds(GET_CP_IDS_RETRIES, GET_CP_IDS_TIMEOUT)) {
		delete _stopEvent; _stopEvent = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxGetCpIds' failed, leaving");
		return;
	}

	//Start MSDLS Server thread
	ACS_ACA_MSDLS_Server * msdlsServer = new (std::nothrow) ACS_ACA_MSDLS_Server();
	if (!msdlsServer) {
		delete _stopEvent; _stopEvent = 0;
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for msdlsServer object, leaving");
		return;
	}

	ACE_thread_t msdls_server_tid = 0;
	int msdls_thread_creation_res = ACE_Thread_Manager::instance()->spawn(
			threadStartupMSDLS,
			msdlsServer,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&msdls_server_tid,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (msdls_thread_creation_res == -1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the MSDLS Server Thread, errno = %d", ACE_OS::last_error());
		delete _stopEvent; _stopEvent = 0;
		return;
	}
	ACS_ACA_LOG(LOG_LEVEL_INFO, "MSDLS Server Thread correctly created!");

	// Start the CP Table change observer (only in Multi-CP configuration), in order to be notified when some CP is added/removed
	ACS_ACA_CPTable_Observer cp_table_observer;
	if (_isMultipleCP) {
		const int return_code = cp_table_observer.start();
		if (return_code != ACS_ACA_CPTable_Observer::ERROR_NO_ERROR) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to start the ACS_ACA_CPTable_Observer object, return code is %d", return_code);
		}
	}

	fxCreateMSManagers();
	fxStartMSManagers();

	ACE_Time_Value oTimeValue(3);

	while (!_stopRequested && !(_stopEvent->wait(&oTimeValue, 0))) {
		ACE_OS::sleep(1);
		fxCheckMSManagers();
		fxCreateMSManagers();
		fxStartMSManagers();
	}

	//Propagate the stop signal to all MS manager children created.
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Stop signal received, stopping all threads");

	fxStopMSManagers();
	fxDeleteMSManagers();

	// Stop the CP Table change observer, if has been previously started
	if (_isMultipleCP) {
		const int return_code = cp_table_observer.stop();
		if (return_code != ACS_ACA_CPTable_Observer::ERROR_NO_ERROR) {
			ACS_ACA_LOG(LOG_LEVEL_WARN, "Failed to stop the ACS_ACA_CPTable_Observer object, return code is %d", return_code);
		}
	}

	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Signaling stop to MSDLS Server Thread");
	msdlsServer->stopMSDLS();

	if (msdls_server_tid) {
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Waiting the MSDLS Server Thread termination...");
		ACE_Thread_Manager::instance()->join(msdls_server_tid);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "MSDLS Server Thread terminated!");
	}
	delete msdlsServer; msdlsServer = 0;

	stopIMMThread();
	fxFreeMessageStoreParameters();
	
	//Delete the stop event object.
	delete _stopEvent; _stopEvent = 0;

	// Delete the stored configuration
	ACS_ACA_ConfigurationHelper::force_configuration_reload();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// getMSManagerByMsgStore
//****************************************************************
ACAMSD_MS_Manager * ACS_MSD_Service::getMSManagerByMsgStore (const char * msname) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (!_msManagers || !msname) {
		return 0;
	}

	for (int i = 0; i < _messageStoreNum; ++i) {
		if (_msManagers[i]){
			if (!::strcmp(_msManagers[i]->msName(), msname)) {
				return _msManagers[i];
			}
		}
	}
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// getAllMSManagers
//****************************************************************
std::vector<ACAMSD_MS_Manager *> ACS_MSD_Service::getAllMSManagers () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	std::vector<ACAMSD_MS_Manager *> result;
	
	if (_msManagers) {
		for (int i = 0; i < _messageStoreNum; ++i) { //for (int i = 0; i < 1; ++i)
			if (_msManagers[i]) {
				if(_msManagers[i]->_state != THREAD_STATE_STOPPED) {
					result.push_back(_msManagers[i]);
				}
			}
		}
	}
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return result;
}

int ACS_MSD_Service::fetchDnOfRootObjFromIMM () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	static int infoAlreadyLoad = 0;
	static char dnOfRootObj[512] = {0};
	int returnCode = 0;
	OmHandler omhandler;

	if (omhandler.Init() == ACS_CC_FAILURE) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'OmHandler.Init' failed, returning -1");
		return -1;
	}

	if (infoAlreadyLoad) {
		parentObjDNofACA = std::string(dnOfRootObj);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "The parent DN has been already loaded, exiting");
		omhandler.Finalize();
		return 0;
	}

	std::vector<std::string> dnList;
	
	if (omhandler.getClassInstances(ACS_CONFIG_IMM_ACA_CLASS_NAME, dnList) != ACS_CC_SUCCESS) { //ERROR
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed, error = '%s'", omhandler.getInternalLastErrorText());
		returnCode = -1;
	} else {
		//OK: Checking how many dn items were found in IMM

		if (dnList.size() ^ 1) {
			//WARNING: 0 (zero) or more than one node found
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Found %lu DNs instead of 1", dnList.size());
			returnCode = -1;
		} else {
			//OK: Only one root node
			::strncpy(dnOfRootObj, dnList[0].c_str(), ACS_ACA_ARRAY_SIZE(dnOfRootObj));
			dnOfRootObj[ACS_ACA_ARRAY_SIZE(dnOfRootObj) - 1] = 0;
			infoAlreadyLoad = 1;
			parentObjDNofACA = std::string(dnOfRootObj);
		}
	}

	omhandler.Finalize();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return returnCode;
}

ACAMS_Parameters * ACS_MSD_Service::getAddress (short indextoParam) {
	ACE_Read_Guard<ACE_RW_Thread_Mutex> read_guard(_ms_parameters_access_sync);
	return gParams[indextoParam];
}

int ACS_MSD_Service::load_ms_parameters_from_IMM () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACAMS_Parameters * pParameters = 0;
	std::vector<std::string> msList;
	OmHandler theOmHandler;

	if (theOmHandler.Init() == ACS_CC_FAILURE) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'OmHandler.Init' failed, returning -1");
		return -1;
	}

	if (theOmHandler.getClassInstances(ACS_CONFIG_IMM_ACA_MS_CLASS_NAME, msList) != ACS_CC_SUCCESS) {   //ERROR
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed, error = '%s', returning -1",
				theOmHandler.getInternalLastErrorText());

		theOmHandler.Finalize();
		return -1;
	}

	std::sort(msList.begin(), msList.end());

	if (!(gParams = new (std::nothrow) ACAMS_Parameters * [_messageStoreNum])) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for ACAMS_Parameters object, returning -1");
		theOmHandler.Finalize();
		return -1;
	}

	for (int i = 0; i < _messageStoreNum; gParams[i++] = 0) ;

	ACS_APGCC_ImmAttribute alarmSeverity;
	ACS_APGCC_ImmAttribute alarmLevel;
	ACS_APGCC_ImmAttribute recordSize;
	ACS_APGCC_ImmAttribute numberOfRecords;
	ACS_APGCC_ImmAttribute numberOfFiles;
	ACS_APGCC_ImmAttribute messageStoreName;

	alarmSeverity.attrName 		= ACA_ALARM_SEVERITY;
	alarmLevel.attrName 			= ACA_ALARM_LEVEL;
	recordSize.attrName 			= ACA_RECORD_SIZE;
	numberOfRecords.attrName 	= ACA_NUMBER_OF_RECORDS;
	numberOfFiles.attrName 		= ACA_NUMBER_OF_FILES;
	messageStoreName.attrName = ACA_MESSAGESTORE_NAME;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	attributes.push_back(&alarmSeverity);
	attributes.push_back(&alarmLevel);
	attributes.push_back(&recordSize);
	attributes.push_back(&numberOfRecords);
	attributes.push_back(&numberOfFiles);
	attributes.push_back(&messageStoreName);

	for (unsigned int m = 0; m < msList.size(); m++) {
		pParameters = 0;
		if (!(pParameters = new (std::nothrow) ACAMS_Parameters)) {
			fxFreeMessageStoreParameters();
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for pParameters object, returning -1");
			theOmHandler.Finalize();
			return -1;
		}

		if (theOmHandler.getAttribute(msList[m].c_str(),attributes) != ACS_CC_SUCCESS) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getAttribute' for object '%s' failed, error = '%s', "
					"continuing with next message store", msList[m].c_str(), theOmHandler.getInternalLastErrorText());
			delete pParameters;
			theOmHandler.Finalize();
			continue;
		}

		// Load parameters with default values
		::strcpy(pParameters->site, "cp0ex");
		pParameters->winSize     = 32;
		pParameters->noOfResends = 48;
		pParameters->resendTime  = 240;
		pParameters->reconnTime  = 10;
		pParameters->FAV         = 0; 		//HU52490: FAV changed to 0 so that MTAP will not send unlink primitive in case of FAV timeout
		pParameters->noOfConn    = 8;
		pParameters->echoValue   = 1;
		pParameters->nameChanged = false;

		// For the other parameters, load the value from IMM
		for (int i = 0; i < static_cast<int>(attributes.size()); i++) {
			if (attributes[i]->attrName == ACA_ALARM_SEVERITY) {
				pParameters->alarmSeverity = *(reinterpret_cast<unsigned short *>(attributes[i]->attrValues[0]));
				const unsigned short alarm_severity = pParameters->alarmSeverity;

				if ( alarm_severity == ALARM_HIGH )
					strcpy(pParameters->ACFilledLevel, "A1");
				if ( alarm_severity == ALARM_MEDIUM )
					strcpy(pParameters->ACFilledLevel, "A2");
				if ( alarm_severity == ALARM_LOW )
					strcpy(pParameters->ACFilledLevel, "A3");
				if ( alarm_severity == ALERT_HIGH )
					strcpy(pParameters->ACFilledLevel, "O1");
				if ( alarm_severity == ALERT_LOW )
					strcpy(pParameters->ACFilledLevel, "O2");
			}
			else if(attributes[i]->attrName == ACA_ALARM_LEVEL) {
				pParameters->alarmLevel = *(reinterpret_cast<unsigned short *>(attributes[i]->attrValues[0]));
			}
			else if (attributes[i]->attrName == ACA_RECORD_SIZE) {
				pParameters->recordSize = *(reinterpret_cast<unsigned short *>(attributes[i]->attrValues[0]));
			}
			else if (attributes[i]->attrName == ACA_NUMBER_OF_RECORDS) {
				pParameters->noOfRecords = *(reinterpret_cast<unsigned short *>(attributes[i]->attrValues[0]));
			}
			else if (attributes[i]->attrName == ACA_NUMBER_OF_FILES) {
				pParameters->noOfFiles = *(reinterpret_cast<unsigned short *>(attributes[i]->attrValues[0]));
			}
			else if (attributes[i]->attrName == ACA_MESSAGESTORE_NAME) {
				::strcpy(pParameters->name, reinterpret_cast<char *>(attributes[i]->attrValues[0]));
			}
		}

		ACE_Write_Guard<ACE_RW_Thread_Mutex> write_guard(_ms_parameters_access_sync);
		gParams[m] = pParameters;
		write_guard.release();

		ACS_ACA_LOG(LOG_LEVEL_INFO, "Parameters correctly read from IMM for MS having index %u", m);
		}

	theOmHandler.Finalize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

////////////
// return: -1 on error, 0 on success
int ACS_MSD_Service::checkDataDisk(const unsigned long maxRetries, const ACE_INT32 timeoutMS)
{
	int result = 0;
	for(int counter = 0; (counter < maxRetries) && ((result = checkDataDisk()) != 0); ++counter)
	{
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'checkDataDisk()' failed, retrying");
		ACE_INT32 usec = timeoutMS * 1000;//microseconds
		ACE_Time_Value safeSleep(0, usec);
		ACE_OS::sleep(safeSleep);
	}

	if(0 == result)
	{
		ACS_ACA_LOG(LOG_LEVEL_INFO, "data disk is in a good shape");
	}
	else
	{
		ACS_ACA_LOG(LOG_LEVEL_FATAL, "### data disk is not in a good shape");
	}
	return result;
}

int ACS_MSD_Service::checkDataDisk()
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int returnValue = -1;
	std::string logicalName("ACS_ACA_DATA");
	std::string pathString;
	if (ACS_ACA_Common::GetDataDiskPath(logicalName, pathString))
	{
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Verifying {$%s, path: %s}", logicalName.c_str(), pathString.c_str());

		boost::system::error_code ec;
		boost::filesystem::path aca_home_folder(pathString.c_str());

		boost::filesystem::file_status s = boost::filesystem::status(aca_home_folder, ec);
		if (ec == 0)
		{
			//Success case
			if (boost::filesystem::exists(s))
			{
				ACS_ACA_LOG(LOG_LEVEL_INFO, "The folder '%s' is accessible", pathString.c_str());

				returnValue = 0;

				//Double check data files. No empty files shall be found.
				std::string file_seach_key_s("_");
				boost::filesystem::recursive_directory_iterator end_itr;
				for( boost::filesystem::recursive_directory_iterator itr(aca_home_folder); itr != end_itr; ++itr )
				{
					if ( boost::filesystem::is_directory(itr->status()) == false)
					{
						std::string path_found(itr->path().filename().string());
						printf("analyzing:\t%s\n", itr->path().string().c_str());

						if (itr->path().filename().string().find(file_seach_key_s) != std::string::npos)
						{
							boost::uintmax_t current_file_size = boost::filesystem::file_size(itr->path());
							printf("found:\t** %s ** size: %lu\n", path_found.c_str(), current_file_size);

							// if some data file is empty at startup something weird is happening
							if(current_file_size == 0)
							{
								ACS_ACA_LOG(LOG_LEVEL_ERROR,
											"\n"
											"############################################################\n"
											"## Error: Found data file empty at startup!\n"
											"## Path == '%s'\n"
											"## Size == '%lu' bytes\n"
											"## This could happen on power failures.\n"
											"## Data Disk synchronization is maybe still ongoing.\n"
											"############################################################\n",
											itr->path().string().c_str(), current_file_size);

								printf("\n"
										"############################################################\n"
										"## Error: Found data file empty at startup!\n"
										"## Path == '%s'\n"
										"## Size == '%lu' bytes\n"
										"## This could happen on power failures.\n"
										"## Data Disk synchronization is maybe still ongoing.\n"
										"############################################################\n",
										itr->path().string().c_str(), current_file_size);

								returnValue = -1;
								break;
							}
						}
					}
				}
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "File search terminated");
				printf("File search terminated\n");
			}
			else
			{
				ACS_ACA_LOG(LOG_LEVEL_ERROR,
							"Error: Call 'boost::filesystem::exists()' failed. Error Type == '%d'. Path == '%s'",
							s.type(), pathString.c_str());
				returnValue = -1;
			}
		}
		else
		{
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'boost::filesystem::status()' failed. Cannot get path status: %s", ec.message().c_str());
			returnValue = -1;
		}
	}
	else
	{
		ACS_ACA_LOG(LOG_LEVEL_ERROR,
					"Failed to get data disk path. Call 'ACS_ACA_Common::GetDataDiskPath()' failed. Logical Name used: %s",
					logicalName.c_str());
		returnValue = -1;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving <%d>", returnValue);
	return returnValue;
}

//****************************************************************
// fxFreeMessageStoreParameters
//****************************************************************
bool ACS_MSD_Service::fxFreeMessageStoreParameters () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	fxFreeMessageStoreParameters(gParams, _messageStoreNum);
	delete[] gParams;
	gParams = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

//****************************************************************
// fxFreeMessageStoreParameters
//****************************************************************
bool ACS_MSD_Service::fxFreeMessageStoreParameters (ACAMS_Parameters ** msParameters, int size) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (!msParameters) return true;

	for (int i = 0; i < size; delete msParameters[i++]) ;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return true;
}

//****************************************************************
// fxCreateMSManagers
//****************************************************************
void ACS_MSD_Service::fxCreateMSManagers () {
	if (!_msManagers) {
		if (!(_msManagers = new (std::nothrow) ACAMSD_MS_Manager * [_messageStoreNum])) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for _msManagers object, returning");
			return;
		}
		for (int i = 0; i < _messageStoreNum; i++) _msManagers[i] = 0;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_cpIds_sync);
		for (int i = 0; i < _messageStoreNum; i++) {
			if (!_msManagers[i]) {
				if (_isMultipleCP) /* Multiple CP */
					_msManagers[i] = new (std::nothrow) ACAMSD_MS_Manager(_cpIds, static_cast<short>(i));
				else /*Single CP*/
					_msManagers[i] = new (std::nothrow) ACAMSD_MS_Manager(static_cast<short>(i));

				if ( !_msManagers[i] ) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for _msManagers[i] object, returning");
					fxDeleteMSManagers();
					return;
				}
			}
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();
}

ACE_THR_FUNC_RETURN threadStartupMS (void * param) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACAMSD_MS_Manager * msMgr = reinterpret_cast<ACAMSD_MS_Manager *>(param);
	msMgr->_state = THREAD_STATE_RUNNING;
	msMgr->workerMain();
	msMgr->_state = THREAD_STATE_STOPPED;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// fxStartMSManagers
//****************************************************************
void ACS_MSD_Service::fxStartMSManagers () {
	if (_msManagers) {
		for (int i = 0; i < _messageStoreNum; ++i) {
			if (_msManagers[i]) {
				if (_msManagers[i]->_state == THREAD_STATE_NOT_STARTED) {
					ACS_ACA_LOG(LOG_LEVEL_INFO, "Creating thread for the message store having index %d", i);
					const int call_result = ACE_Thread_Manager::instance()->spawn(
							threadStartupMS,
							_msManagers[i],
							THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
							&(_msManagers[i]->_tid),
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							-1,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE);

					if (call_result == -1) {
							ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the Message Store Manager Thread having index %d, errno = %d", i, ACE_OS::last_error());
							delete _msManagers[i]; _msManagers[i] = 0;
							continue;
					}
					ACS_ACA_LOG(LOG_LEVEL_INFO, "Message Store Manager Thread having index %d correctly created!", i);
				}
			} else {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "MS manager having index %d not yet created - _state == %d!", i, _msManagers[i]->_state);
			}
		}
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "_msManagers array is NULL!");
	}
}

//****************************************************************
// fxDeleteMSManagers
//****************************************************************
void ACS_MSD_Service::fxDeleteMSManagers () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (_msManagers) {
		for (int i = 0; i < _messageStoreNum; ++i) {
			delete _msManagers[i]; _msManagers[i] = 0;
		}
	} else {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "_msManagers array is NULL, nothing to destroy!");
	}

	delete _msManagers; _msManagers = 0;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// fxCheckMSManagers
//****************************************************************
void ACS_MSD_Service::fxCheckMSManagers () {
	if (_msManagers) {
		for (int i = 0; i < _messageStoreNum; i++) {
			if (_msManagers[i]) {
				if (_msManagers[i]->_state == THREAD_STATE_STOPPED) {
					ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the Message Store Manager Thread having index %d...", i);
					ACE_Thread_Manager::instance()->join(_msManagers[i]->_tid);
					ACS_ACA_LOG(LOG_LEVEL_INFO, "Message Store Manager Thread having index %d terminated!", i);
					delete _msManagers[i]; _msManagers[i] = 0;
				}
			}
		}
	} else {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "_msManagers array is NULL!");
	}
}

//****************************************************************
// fxStopMSManagers
//****************************************************************
void ACS_MSD_Service::fxStopMSManagers (unsigned long /*retryNum*/, ACE_INT32 /*milliseconds*/) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (_msManagers) {
		for (int i = 0; i < _messageStoreNum; ++i) {
			if (_msManagers[i]) {
				_msManagers[i]->stopMS();
			}
		}

		for (int i = 0; i < _messageStoreNum; i++) {
			if (_msManagers[i]) {
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Joining the Message Store Manager Thread having index %d...", i);
				ACE_Thread_Manager::instance()->join(_msManagers[i]->_tid);
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Message Store Manager Thread having index %d terminated!", i);
			}
			else
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Message Store Manager Thread having index %d is already stopped...", i);
		}
	}
	else
		ACS_ACA_LOG(LOG_LEVEL_WARN, "_msManagers array is NULL!");

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

//****************************************************************
// fxGetCpIds
//****************************************************************
int ACS_MSD_Service::fxGetCpIds (unsigned retries, unsigned timeout) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	int errorLevel = 0;
	double timeForWait = timeout/1000;
	ACE_Time_Value oTimeValue(timeForWait);

	errorLevel = fxGetCpIds();
	if (errorLevel) {
		unsigned tryStep = 0;
		while (!_stopRequested && (tryStep++ < retries) && !(ACS_MSD_Service::_stopEvent->wait(&oTimeValue, 0)) &&
					 (errorLevel = fxGetCpIds())) ;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACS_MSD_Service::fxGetCpIds () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (!_isMultipleCP) return 0;

	ACS_CS_API_CP * cp = 0;

	try {
		if (!(cp = ACS_CS_API::createCPInstance())) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'createCPInstance' failed, returning 5");
			return 5;
		}
	} catch (...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'createCPInstance' threw an exception, returning 5");
		return 5;
	}

	ACS_CS_API_IdList cpList;
	ACS_ACA_CS_API::CS_API_Result_ACA result;

	try {
		result = (ACS_ACA_CS_API::CS_API_Result_ACA) cp->getCPList(cpList);
	}
	catch (...) {
		ACS_CS_API::deleteCPInstance(cp);
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getCPList' threw an exception, returning 3");
		return 3;
	}

	if (result != ACS_ACA_CS_API::Result_Success) {
		ACS_CS_API::deleteCPInstance(cp);
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getCPList' failed, error_code = %d, returning 3", result);
		return 3;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_cpIds_sync);
		_cpIds.clear();
		for (unsigned i = 0; i < cpList.size(); _cpIds.push_back(cpList[i++])) ;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_CS_API::deleteCPInstance(cp);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

//****************************************************************
// fxCheckMultipleCP
//****************************************************************
int ACS_MSD_Service::fxCheckMultipleCP (unsigned retries, unsigned timeout) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	int errorLevel = 0;
	double timeForWait = timeout/1000;
	ACE_Time_Value oTimeValue(timeForWait);

	errorLevel = fxCheckMultipleCP();
	if (errorLevel) {
		unsigned tryStep = 0;
		while (!_stopRequested && (tryStep++ < retries) && !(ACS_MSD_Service::_stopEvent->wait(&oTimeValue, 0)) &&
					 (errorLevel = fxCheckMultipleCP())) ;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return errorLevel;
}

int ACS_MSD_Service::fxCheckMultipleCP () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_ACA_CS_API::CS_API_Result_ACA result;
	try {
		result = static_cast<ACS_ACA_CS_API::CS_API_Result_ACA>(ACS_CS_API_NetworkElement::isMultipleCPSystem(_isMultipleCP));
	}
	catch (...) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxCheckMultipleCP' threw an exception, returning 3");
		return 3;
	}

	if (result != ACS_ACA_CS_API::Result_Success) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'fxCheckMultipleCP' failed, error_code = %d, returning 3", result);
		return 3;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}
//****************************************************************
// fxGetDefaultCPName
//****************************************************************
int ACS_MSD_Service::fxGetDefaultCPName (short id, std::string & dname) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	std::string CPName;
	ACE_INT32 result = getDefaultCPName_aca(id, CPName);
	if (result == CS_API_SUCCESS) {
		dname = CPName;
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return 0;
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getDefaultCPName_aca' failed, returning -1");
		return -1;
	}
}
//*******************CR639********************

int ACS_MSD_Service::getDefaultCPName_aca (unsigned short cpId, std::string & defName) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (cpId < (ACS_ACA_CS_API::SysType_BC + 1000)) { // Single sided CP (blade)
		stringstream defNameStringStream;
		defNameStringStream << "BC" << cpId;            // For example "BC0"
		defName = defNameStringStream.str().c_str();

		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return CS_API_SUCCESS;
	} else if ((cpId >= ACS_ACA_CS_API::SysType_CP) && (cpId < (ACS_ACA_CS_API::SysType_CP + 1000))) { // Double sided CP (SPX)
		stringstream defNameStringStream;
		defNameStringStream << "CP" << (cpId - 1000);   // For example "CP0"
		defName = defNameStringStream.str().c_str();
		
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return CS_API_SUCCESS;
	} else {
		// Failure if this is reached
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Error while trying to get the default CP name, returning error");
		return CS_API_FAILURE;
	}
}
//****************CR639******************

/*=====================================================================
   ROUTINE: stop
 =====================================================================*/
void ACS_MSD_Service::stop () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	StopServer();

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Waiting the Root Worker Thread termination...");
	ACE_Thread_Manager::instance()->join(_root_worker_thread_id);
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Root Worker Thread terminated!");
	_root_worker_thread_id = 0;

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

/*=====================================================================
        ROUTINE: StopServer
=====================================================================*/
void ACS_MSD_Service::StopServer () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	if (_stopEvent) { //set _stopEvent
		_stopEvent->signal();
	}
	_stopRequested = true;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

/*=====================================================================
        ROUTINE: StartServer
=====================================================================*/
void ACS_MSD_Service::StartServer () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Starting the application acs_acad: pid = %d", ACE_OS::getpid());
	serviceMain();
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACS_MSD_Service::stopIMMThread () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if ( m_poMsgStoreCommandHandler ) {
		m_poMsgStoreCommandHandler->shutdown();
		delete m_poMsgStoreCommandHandler; m_poMsgStoreCommandHandler = 0;
	}

	// Wait the thread termination
	if (_IMM_thread_id) {
		ACS_ACA_LOG(LOG_LEVEL_INFO, "Waiting the IMM Callbacks Thread termination...");
		ACE_Thread_Manager::instance()->join(_IMM_thread_id);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "IMM Callbacks Thread terminated!");
		_IMM_thread_id = 0;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

/*=====================================================================
        ROUTINE: ACAFunctionalMethod
=====================================================================*/
ACE_THR_FUNC_RETURN ACS_MSD_Service::ACAFunctionalMethod (void * aACAPtr) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_MSD_Service * myACAPtr = reinterpret_cast<ACS_MSD_Service *>(aACAPtr);

	if (myACAPtr) {
		ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Starting ACA server...");
		myACAPtr->StartServer();
	} else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACS_MSD_Service pointer is NULL, exiting");
		return 0;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

/*=================================================================
        ROUTINE: setupACAThread
=================================================================== */
ACS_CC_ReturnType ACS_MSD_Service::setupACAThread (ACS_MSD_Service * aACAPtr) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	const int call_result = ACE_Thread_Manager::instance()->spawn(
			&ACAFunctionalMethod,
			aACAPtr,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&(aACAPtr->_root_worker_thread_id),
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (call_result == -1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the Root Worker Thread, errno = %d", ACE_OS::last_error());
		aACAPtr->_root_worker_thread_id = 0;
		return ACS_CC_FAILURE;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ACS_CC_SUCCESS;
}

void ACS_MSD_Service::setupIMMCallBacks () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if ( !m_poMsgStoreCommandHandler ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for m_poMsgStoreCommandHandler object, returning");
		return;
	}

	//Start the reactor.
	if ( (m_poMsgStoreCommandHandler->svc()) ==  -1 ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'm_poMsgStoreCommandHandler->svc' failed, returning");
		return;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}//End of setupIMMCallBacks

ACS_CC_ReturnType ACS_MSD_Service::StartOIThreadsOnActiveNode () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	std::vector<string> classes_name;
	classes_name.clear();
	classes_name.push_back(ACS_CONFIG_IMM_ACA_MS_CLASS_NAME);

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	classes_name.push_back(ACS_CONFIG_IMM_ACA_CLASS_NAME);
#endif

	m_poMsgStoreCommandHandler = new (std::nothrow) ACS_ACA_ObjectImpl(classes_name, ACA_MSG_STORE_IMPL_NAME, ACS_APGCC_ONE);
	if ( !m_poMsgStoreCommandHandler ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to allocate memory for m_poMsgStoreCommandHandler object, returning");
		return ACS_CC_FAILURE;
	}

	const int call_result = ACE_Thread_Manager::instance()->spawn(
			&setupIMMCallBacksThreadFunc,
			this,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&_IMM_thread_id,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if (call_result == -1) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to create the IMM Callbacks Thread, errno = %d", ACE_OS::last_error());
		_IMM_thread_id = 0;
		return ACS_CC_FAILURE;
	}
	ACS_ACA_LOG(LOG_LEVEL_INFO, "IMM Callbacks Thread correctly created!");

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ACS_CC_SUCCESS;
}

ACE_THR_FUNC_RETURN ACS_MSD_Service::setupIMMCallBacksThreadFunc (void * aPtr) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	ACS_MSD_Service * myMsdserverptr = reinterpret_cast<ACS_MSD_Service *>(aPtr);

	if ( !myMsdserverptr ) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACS_MSD_Service pointer is NULL, exiting");
		return 0;
	}

	myMsdserverptr->setupIMMCallBacks();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACS_MSD_Service::modify_ms_parameter (int ms_index, const char * param_to_modify, void * new_value) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACE_Write_Guard<ACE_RW_Thread_Mutex> write_guard(_ms_parameters_access_sync);

	if ( ::strcmp(param_to_modify, ACA_ALARM_SEVERITY) == 0 ) {
		const unsigned short alarm_severity = *(reinterpret_cast<unsigned short *> (new_value));

		if ( alarm_severity == ALARM_HIGH )
			::strcpy(gParams[ms_index]->ACFilledLevel, "A1");
		else if ( alarm_severity == ALARM_MEDIUM )
			::strcpy(gParams[ms_index]->ACFilledLevel, "A2");
		else if ( alarm_severity == ALARM_LOW )
			::strcpy(gParams[ms_index]->ACFilledLevel, "A3");
		else if ( alarm_severity == ALERT_HIGH )
			::strcpy(gParams[ms_index]->ACFilledLevel, "O1");
		else if ( alarm_severity == ALERT_LOW )
				::strcpy(gParams[ms_index]->ACFilledLevel, "O2");
		else {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Value '%hu' is not valid for 'alarmSeverity' parameter!", alarm_severity);
			return -1;
		}

		gParams[ms_index]->alarmSeverity = alarm_severity;
	}
	else if ( ::strcmp(param_to_modify, ACA_ALARM_LEVEL) == 0 ) {
		gParams[ms_index]->alarmLevel = *(reinterpret_cast<unsigned short *> (new_value));
	}
	else if ( ::strcmp(param_to_modify, ACA_RECORD_SIZE) == 0 ) {
		gParams[ms_index]->recordSize = *(reinterpret_cast<unsigned short *> (new_value));
	}
	else if ( ::strcmp(param_to_modify, ACA_NUMBER_OF_RECORDS) == 0 ) {
		gParams[ms_index]->noOfRecords = *(reinterpret_cast<unsigned short *> (new_value));
	}
	else if ( ::strcmp(param_to_modify, ACA_NUMBER_OF_FILES) == 0 ) {
		gParams[ms_index]->noOfFiles = *(reinterpret_cast<unsigned short *> (new_value));
	}
	else if ( ::strcmp(param_to_modify, ACA_MESSAGESTORE_NAME) == 0 ) {
		::strncpy(gParams[ms_index]->name, reinterpret_cast<const char *> (new_value), ACS_ACA_ARRAY_SIZE(gParams[ms_index]->name));
		gParams[ms_index]->nameChanged = true;
	}
	else {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Received a request to modify an unrecognized parameter "
				"for the message store having index %d, exiting.", ms_index);
		return -2;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Parameter '%s' correctly modified for the message store having index %d!",
			param_to_modify, ms_index);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

void ACS_MSD_Service::reset_name_changed_attribute(int ms_index) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACE_Write_Guard<ACE_RW_Thread_Mutex> write_guard(_ms_parameters_access_sync);
	gParams[ms_index]->nameChanged = false;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

void ACS_MSD_Service::notify_cptable_change(unsigned cp_id, int op_type) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	if (!_msManagers) {		// This is possible into initial phases
		ACS_ACA_LOG(LOG_LEVEL_WARN, "No MS managers array found, nothing to notify!");
		return;
	}

	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_cpIds_sync);
		if (op_type == ACS_ACA_CPTable_Observer::BLADE_ADD) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Adding the CP with cp_id == %u into the internal data structure!", cp_id);
			_cpIds.push_back(cp_id);
		}
		else {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Removing the CP with cp_id == %u from the internal data structure!", cp_id);
			std::vector<unsigned>::iterator it = std::find(_cpIds.begin(), _cpIds.end(), cp_id);

			if (it == _cpIds.end()) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "No element for cp_id == %u was found into the internal data structure!", cp_id);
				return;
			}

			_cpIds.erase(it);
		}

		for (int i = 0; i < _messageStoreNum; i++) {
			if (_msManagers[i]) {
				ACS_ACA_LOG(LOG_LEVEL_INFO, "Notifying the MS manager having index %d...", i);
				_msManagers[i]->notify_cptable_change(cp_id, op_type);
			}
			else
				ACS_ACA_LOG(LOG_LEVEL_WARN, "No MS manager found for index %d!", i);
		}
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACS_MSD_Service::ParameterChangeErrorCode ACS_MSD_Service::is_change_applicable (int ms_index) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	char ms_name[16] = {0};
	::snprintf(ms_name, sizeof(ms_name), "%s", ACS_MSD_Service::getAddress(ms_index)->name);

	// In Single-CP configuration, it's needed to check if the change is applicable only for the unique CP
	if (!_isMultipleCP)	return ACS_MSD_Service::check_change_applicable_for_cp(ACAMSD_MS_Manager::SINGLE_CP_ID, ms_name);

	// In Multi-CP configuration, it's needed to check if the change is applicable for each CP/BC
	std::vector<unsigned> my_cpids_vector;
	ACS_ACA_CRITICAL_SECTION_GUARD_BEGIN(_cpIds_sync);
		my_cpids_vector = _cpIds;
	ACS_ACA_CRITICAL_SECTION_GUARD_END();

	ACS_MSD_Service::ParameterChangeErrorCode return_code = NO_ERROR;
	for (size_t i = 0; i < my_cpids_vector.size(); i++) {
		 if ((return_code = ACS_MSD_Service::check_change_applicable_for_cp(my_cpids_vector[i], ms_name)) != NO_ERROR)
			 break;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "The change is %s!", (return_code == NO_ERROR) ? "APPLICABLE" : "NOT APPLICABLE");
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return return_code;
}

ACS_MSD_Service::ParameterChangeErrorCode ACS_MSD_Service::check_change_applicable_for_cp(unsigned cp_id, char * ms_name) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "Checking if the change is applicable for the couple (%s, %d)", ms_name, cp_id);

	// Get the MS Manager object handling the given message store
	ACAMSD_MS_Manager * ms_manager = ACS_MSD_Service::getMSManagerByMsgStore(ms_name);
	if (!ms_manager) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MS_Manager pointer is NULL");
		return ERROR_TRY_AGAIN;
	}

	// Get the MTAP Channel Manager handling the couple (message_store, cp)
	const ACAMSD_MTAP_ChannelManager * mtap_channel = ms_manager->getMTAPChannelMgrByCP(cp_id);
	if (!mtap_channel) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MTAP_ChannelManager pointer is NULL");
		return ERROR_TRY_AGAIN;
	}

	// Get the MsgStore object in order to retrieve all the info needed
	ACAMSD_MsgStore * msg_store = mtap_channel->getMsgStore();
	if (!msg_store) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "ACAMSD_MsgStore pointer is NULL");
		return ERROR_TRY_AGAIN;
	}

	// Retrieve info about last message received
	unsigned long long lastMess = (msg_store->getNextMsgNumber() - 1);

	// Retrieve info about last message acknowledged
	std::string data_files_dir_name = msg_store->getDataFileDirectoryName();
	std::string commit_file_name = msg_store->getCommitFileName();

	ACAMS_CommitFile * commit_file;
	try {
		commit_file = new (std::nothrow) ACAMS_CommitFile(commit_file_name);

		if (!commit_file) {	// ERROR: Memory allocation failed
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for ACAMS_CommitFile object for file '%s'!", commit_file_name.c_str());
			return ERROR_COMMIT_FILE_OPEN_FAILED;
		}
	}
	catch (std::runtime_error & ex) {	// ERROR: Failed to open the commit file
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a runtime exception for file '%s': error_message == '%s'!",
				commit_file_name.c_str(), ex.what());

		delete commit_file; commit_file = 0;
		return ERROR_COMMIT_FILE_OPEN_FAILED;
	}
	catch (...) {	// ERROR: Failed to open the commit file
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'ACAMS_CommitFile' threw a generic exception for file '%s'!", commit_file_name.c_str());
		delete commit_file; commit_file = 0;
		return ERROR_COMMIT_FILE_OPEN_FAILED;
	}

	unsigned long long lastAckMess = commit_file->getMsgNumber();
	delete commit_file; commit_file = 0;

	// If the two values are't equal, the change is not applicable
	if (lastAckMess < lastMess) {
		ACS_ACA_LOG(LOG_LEVEL_WARN, "The change is not applicable for the couple (%s, %u): "
				"the value of lastAckMess is %llu, but the value of lastMess is %llu!",
				ms_name, cp_id, lastAckMess, lastMess);
		return ERROR_NOT_ACK_MSG;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "The change is applicable for the couple (%s, %d)!", ms_name, cp_id);
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return NO_ERROR;
}

ACS_MSD_Service::ParameterChangeErrorCode ACS_MSD_Service::is_record_size_valid (int ms_index, unsigned short new_record_size, bool & is_job_present) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	is_job_present = false;	// By default, no job is present for the given message store

	OmHandler om_handler;
	if (om_handler.Init()) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'om_handler.Init()' failed! - Error message is '%s'", om_handler.getInternalLastErrorText());
		return ERROR_OM_INIT_FAILED;
	}

	char ms_to_update[128];
	(ms_index == 0)
				?		(::snprintf(ms_to_update, sizeof(ms_to_update), "CHS"))
				: 	(::snprintf(ms_to_update, sizeof(ms_to_update), "CHS%d", ms_index));

	char file_job_dn[512];
	::snprintf(file_job_dn, sizeof(file_job_dn), "%s=1,%s=%s,%s=1",
			ACA_FILE_BASED_JOB_ID, ACA_MSG_STORE_ID, ms_to_update, ACA_DATA_RECORD_M_ID);

	ACS_APGCC_ImmAttribute fixed_file_records_flag;
	ACS_APGCC_ImmAttribute record_length;
	fixed_file_records_flag.attrName = ACA_FIXED_RECORDS_FLAG;
	record_length.attrName = ACA_RECORD_LENGTH;

	std::vector<ACS_APGCC_ImmAttribute *> file_job_attributes;
	file_job_attributes.push_back(&fixed_file_records_flag);
	file_job_attributes.push_back(&record_length);

	ACS_CC_ReturnType return_code = om_handler.getAttribute(file_job_dn, file_job_attributes);

	if ((return_code != ACS_CC_SUCCESS) && (om_handler.getInternalLastError() != -12)) {	// ERROR while fetching the object
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getAttribute()' failed for the object '%s', error is: %s",
				file_job_dn, om_handler.getInternalLastErrorText());
		om_handler.Finalize();
		return ERROR_GET_ATTRBUTE_FAILED;
	}
	else if (return_code == ACS_CC_SUCCESS) {
		is_job_present = true;
		int fixed_file_records_flag_val = *(reinterpret_cast<int *>(fixed_file_records_flag.attrValues[0]));
		int record_length_val = *(reinterpret_cast<int *>(record_length.attrValues[0]));

		if (fixed_file_records_flag_val != ACA_FIXED_RECORDS_FLAG_FIXED) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "The value of fixedFileRecordsFlag is %d, the change can be accepted!", fixed_file_records_flag_val);
			om_handler.Finalize();
			return NO_ERROR;
		}

		bool is_valid = (record_length_val >= new_record_size);
		ACS_ACA_LOG(LOG_LEVEL_INFO, "ASSERTION[record_length_val >= new_record_size] is %s!", (is_valid) ? "TRUE" : "FALSE");
		om_handler.Finalize();
		return (is_valid) ? NO_ERROR : ERROR_NOT_VALID_FOR_FILE_JOB;
	}

	// If no FileBased object has been found, try to search for a BlockBased object
	char block_job_dn[512];
	::snprintf(block_job_dn, sizeof(block_job_dn), "%s=1,%s=%s,%s=1",
			ACA_BLOCK_BASED_JOB_ID, ACA_MSG_STORE_ID, ms_to_update, ACA_DATA_RECORD_M_ID);

	ACS_APGCC_ImmAttribute block_length;
	block_length.attrName = ACA_BLOCK_LENGTH;

	std::vector<ACS_APGCC_ImmAttribute *> block_job_attributes;
	block_job_attributes.push_back(&block_length);

	return_code = om_handler.getAttribute(block_job_dn, block_job_attributes);

	if ((return_code != ACS_CC_SUCCESS) && (om_handler.getInternalLastError() != -12)) {	// ERROR while fetching the object
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getAttribute()' failed for the object '%s', error is: %s",
				block_job_dn, om_handler.getInternalLastErrorText());
		om_handler.Finalize();
		return ERROR_GET_ATTRBUTE_FAILED;
	}
	else if (return_code == ACS_CC_SUCCESS) {
		is_job_present = true;
		int block_length_val = *(reinterpret_cast<int*>(block_length.attrValues[0]));
		bool is_valid = (block_length_val >= new_record_size);

		ACS_ACA_LOG(LOG_LEVEL_INFO, "ASSERTION[block_length_val >= new_record_size] is %s!", (is_valid) ? "TRUE" : "FALSE");
		om_handler.Finalize();
		return (is_valid) ? NO_ERROR : ERROR_NOT_VALID_FOR_BLOCK_JOB;
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "No FileBasedJob and BlockBasedJob objects found, the change can be accepted!");
	om_handler.Finalize();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return NO_ERROR;
}

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
int ACS_MSD_Service::checkDiskReintegrationOperationOngoing () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	// Create an initialize the OM Handler object
	OmHandler om_handler;
	if (om_handler.Init()) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'om_handler.Init()' failed! - Error message is '%s'", om_handler.getInternalLastErrorText());
		return -1;
	}

	// Create the DN string of AxeDataRecordDataRecordM class
	char dataRecordM_dn[1024] = {0};
	::snprintf(dataRecordM_dn, ACS_ACA_ARRAY_SIZE(dataRecordM_dn), "%s=1", ACA_DATA_RECORD_M_ID);

	// Select the attribute to retrieve
	ACS_CC_ImmParameter os_caching_enabled_attr;
	os_caching_enabled_attr.attrName = ACA_OS_CACHING_ENABLED;

	// Retrieve the selected attribute
	if (om_handler.getAttribute(dataRecordM_dn, &os_caching_enabled_attr) != ACS_CC_SUCCESS) {	// ERROR while fetching the object
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'getAttribute()' failed for the object '%s', "
				"error_code = %d, error_message = '%s'", dataRecordM_dn,
				om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());

		om_handler.Finalize();
		return -2;
	}

	// Check if at least one value has been retrieved
	if (os_caching_enabled_attr.attrValuesNum <= 0) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "The field 'os_caching_enabled_attr.attrValuesNum' is negative!!!");
		om_handler.Finalize();
		return -3;
	}

	// Get the retrieved value
	int retrieved_value = *(reinterpret_cast<int *> (os_caching_enabled_attr.attrValues[0]));
	_isOSCachingEnabled = (retrieved_value) ? true : false;
	om_handler.Finalize();

	ACS_ACA_LOG(LOG_LEVEL_INFO, "The value retrieved from '%s' object is '%d', _isOSCachingEnabled == %s",
			dataRecordM_dn, retrieved_value, (_isOSCachingEnabled) ? "TRUE" : "FALSE");
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

int ACS_MSD_Service::notify_os_caching_status(bool caching_status) {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	int n_notify_failures = 0;

	if (!_msManagers) {		// This is possible into initial phases
		ACS_ACA_LOG(LOG_LEVEL_WARN, "No MS managers array found, nothing to notify!");
		return 0;
	}

	for (int i = 0; i < _messageStoreNum; i++) {
		if (_msManagers[i]) {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Notifying OS Caching status '%s' to the MS manager having index %d...",
					(caching_status ? "ENABLED" : "DISABLED"), i);
			if(_msManagers[i]->notify_os_caching_status(caching_status))
				 ++n_notify_failures;
		}
		else
			ACS_ACA_LOG(LOG_LEVEL_WARN, "No MS manager found for index %d!", i);
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");

	return -n_notify_failures;
}


int ACS_MSD_Service::set_os_caching_status(bool status) {
	return (status != _isOSCachingEnabled) ?  ACS_MSD_Service::notify_os_caching_status(_isOSCachingEnabled = status) : 0;
}

#endif

