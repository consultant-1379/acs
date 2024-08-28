//********************************************************
//
//	COPYRIGHT Ericsson AB 2014.
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson AB.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson AB or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//********************************************************

#include "acs_rtr_server.h"
#include "acs_rtr_systemconfig.h"
#include "acs_rtr_global.h"
#include "acs_rtr_immhandler.h"
#include "acs_rtr_cpstatistics_impl.h"
#include "acs_rtr_statistics_impl.h"
#include "acs_rtr_manager.h"
#include "acs_rtr_events.h"
#include "acs_rtr_statistics.h"
#include "acs_rtr_acachannel.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"
#include "acs_rtr_defaultvalues.h"

#include "aes_ohi_extfilehandler2.h"
#include "aes_ohi_blockhandler2.h"

#include "ACS_APGCC_CommonLib.h"
#include "acs_aca_constants.h"

#include <crypto_status.h>
#include <crypto_api.h>

#include <iostream>
#include <sys/eventfd.h>
#include <mqueue.h>
#include<string.h>

#define MESSAGE_STORE_INFO "messageStoreInfoId"
#define classNameAttr "AxeDataRecordMessageStore"


namespace RTR_DSD
{
	const std::string serviceDomain("ACS");
	const std::string serviceName("ACS_RTR_Server");
	const size_t ACS_DSD_MAXBUFFER = 65000U;
}

namespace {
		//----------
		// Constants
		//----------
		const char * const _RTRSRV_DERR_NEW_DSD = "Memory Error, cannot create ACS_DSD_Server object";
		const char * const _RTRSRV_DERR_CREATE_STOP_EVENT = "Failing to create internal stop event.";
		const char * const _RTRSRV_DERR_OPEN_DSD = "Error occurred while opening the DSD server!";
		const char * const _RTRSRV_DERR_PUBLISH_DSD = "Error occurred while publishing on the DSD server!";
		const char * const _RTRSRV_DERR_GETHANDLES_DSD = "Error occurred while getting handles from DSD!";
		const char * const _RTRSRV_DERR_NEW_SYS_CONFIGURATION = "Error creating the system configuration table.";
		const char * const _RTRSRV_DERR_SYS_CONFIGURATION = "Error retrieving the system configuration.";
		const char * const _RTRSRV_DERR_CREATE_EVENT_HANDLER = "Failing to create global event handler.";
		const char * const _RTRSRV_DERR_NEW_DSD_SESSION = "Memory Error, cannot create ACS_DSD_Session object.";
		const char * const _RTRSRV_DERR_DSD_ACCEPT = "Error in ACS_DSD_Server::accept.";
		const char * const _RTRSRV_DERR_DSD_RECV = "Error in ACS_DSD_Session::recv.";
		const char * const _RTRSRV_DERR_WAIT_FAILED = "'WaitForMultipleObject(...' failed!";
		const char * const _RTRSRV_DERR_NEW_MGR = "Cannot create ACS_RTR_Manager object.";
		const char * const _RTRSRV_DERR_START_MGR = "Cannot start ACS_RTR_Manager object.";
		const char * const _RTRSRV_DERR_SEND_DSD = "ACS_DSD_Session::send() failed sending response";
		const char * const _RTRSRV_DERR_NEW_STAT = "Memory Error: cannot create statistics file object for RTR service.";
		const char * const _RTRSRV_DERR_INIT_STAT = "Cannot initialize the statistics file for RTR service.";
		const char * const _RTRSRV_DERR_LOAD_DEF_FILE = "Cannot load the definition file 'rtr.def'";
		const char * const _RTRSRV_DERR_CHECK_DATADISK = "Data disk isn't available";
		const char * const _RTRSRV_DERR_COMMONDLL = "Cannot retrieve data from CommonDLL";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_Server);


RTR_statistics* ACS_RTR_Server::_rtrStat = 0;

ACS_RTR_Server::ACS_RTR_Server()
:m_stopEvent(eventfd(0, 0)),
 m_DSDServer(NULL),
 m_dsdHandles(),
 _stopRequested(false),
 _managerList(),
 m_ImmHandler(NULL),
 m_MsgQueueHandle(ACE_INVALID_HANDLE)
{
	ACS_RTR_TRACE_FUNCTION;
}

//-----------
// Destructor
//-----------
ACS_RTR_Server::~ACS_RTR_Server()
{
	ACS_RTR_TRACE_FUNCTION;

	delete m_ImmHandler;
	ACE_OS::close(m_stopEvent);

	if( ACE_INVALID_HANDLE != m_MsgQueueHandle)
		::mq_close(m_MsgQueueHandle);
}

//-----
// stop
//-----
bool ACS_RTR_Server::stop()
{
	ACS_RTR_TRACE_MESSAGE("IN");
	_stopRequested = true;

	eventfd_t dataReady = 1U;
	if(eventfd_write(m_stopEvent, dataReady) != 0)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to signal m_stopEvent Event error:<%d>", errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to signal Stop Event:<%d>, error:<%d>", m_stopEvent, errno);
	}

	ACS_RTR_TRACE_MESSAGE("OUT");
	return _stopRequested;
}

int ACS_RTR_Server::open(void *args)
{
	ACS_RTR_TRACE_MESSAGE("In, start RTR Server Worker Thread");
	UNUSED(args);

	int result = activate();

	ACS_RTR_TRACE_MESSAGE(" activation result:<%s>", ( (0 == result) ? "OK":"NOT OK") );

	return result;
}

/*===================================================================
   ROUTINE: addNewManager
=================================================================== */
bool ACS_RTR_Server::addNewManager(const char* msName,
					   const RTRMS_Parameters* parameters,
					   const std::string& jobBaseDN
					  )
{
	ACS_RTR_TRACE_MESSAGE("IN, add manager for MS:<%s>", msName);
	bool result = false;
	ACS_RTR_Manager* manager = NULL;

	try
	{
		manager = new (std::nothrow) ACS_RTR_Manager(msName, parameters);

		if((NULL != manager) )
		{
			result = true;
			manager->setJobBaseDN(jobBaseDN.c_str());

			if( manager->open(m_stopEvent) == FAILURE)
			{
				//trace error
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: NEW MANAGER STARTUP failed for the MessageStore:<%s>, Last error:<%d>, retry later...", msName, errno);

				ACS_RTR_TRACE_MESSAGE("%s", trace);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
			}

			// Add the new created manager object to the list,
			// also when open fails, since it will be retried again...
			_managerList.push_back(manager);
		}
		else
		{
			//trace error
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: NEW MANAGER creation failed for the MessageStore:<%s>, Last error:<%d>", msName, errno);

			ACS_RTR_TRACE_MESSAGE("%s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		}
	}
	catch(std::exception& ex)
	{
		//trace error
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: NEW MANAGER creation threw exception for the MessageStore:<%s>", msName);

		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
	}

	ACS_RTR_TRACE_MESSAGE("OUT, result:<%s>", (result ? "TRUE":"FALSE"));
	return result;
}

/*===================================================================
   ROUTINE: removeManager
=================================================================== */
bool ACS_RTR_Server::removeManager(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_managerCriticalSection);
	bool result = true;

	ACS_RTR_Manager* manager = getRTRManager(messageStoreName);

	// Check if Message Store is defined
	if( NULL != manager)
	{
		if(manager->rtrrmIssue())
		{
			ACS_RTR_TRACE_MESSAGE("Manager can be removed");
			ACS_RTR_LOG(LOG_LEVEL_INFO, "Found the manager MS:<%s>, waiting and deleting it",messageStoreName.c_str());
			manager->wait();
			delete manager;
			//  element from the list
			_managerList.remove(manager);
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("Manager can NOT be removed");
			result = false;
		}
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Manger not found for MS:<%s>", messageStoreName.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*===================================================================
   ROUTINE: isJobDefined
=================================================================== */
bool ACS_RTR_Server::isJobDefined(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	bool found = false;
	std::list<ACS_RTR_Manager*>::const_iterator manager;
	for( manager = _managerList.begin(); manager != _managerList.end(); ++manager)
	{
		if (messageStoreName.compare((*manager)->getMessageStore()) == 0 )
		{
			found = true;
			break;
		}
	}
	ACS_RTR_TRACE_MESSAGE("Out, found<%s>", ( found ? "TRUE" : "FALSE") );
	return found;
}

bool ACS_RTR_Server::isJobDefined(const std::string& messageStoreName,const std::string& transferQueue)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	bool found = false;
	std::list<ACS_RTR_Manager*>::const_iterator manager;

	for( manager = _managerList.begin(); manager != _managerList.end(); ++manager)
	{
		if (messageStoreName.compare((*manager)->getMessageStore()) == 0 || transferQueue.compare((*manager)->getTransferQueue()) == 0)
		{
			found = true;
			break;
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, found<%s>", ( found ? "TRUE" : "FALSE") );
	return found;
}

/*===================================================================
   ROUTINE: getManagerParameters
=================================================================== */
bool ACS_RTR_Server::getManagerParameters(const std::string& messageStoreName, RTRMS_Parameters* currentSet)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	bool found = false;

	memset(currentSet, 0, sizeof(RTRMS_Parameters));
	std::list<ACS_RTR_Manager*>::const_iterator manager;

	for( manager = _managerList.begin(); manager != _managerList.end(); ++manager)
	{
		if (messageStoreName.compare((*manager)->getMessageStore()) == 0 )
		{
			// Copy current setting
			memcpy(currentSet, (*manager)->getRTRParameters(), sizeof(RTRMS_Parameters));
			found = true;
			break;
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, found<%s>", ( found ? "TRUE" : "FALSE") );
	return found;
}

/*===================================================================
   ROUTINE: setManagerParameters
=================================================================== */
void ACS_RTR_Server::setManagerParameters(const std::string& messageStoreName, RTRMS_Parameters* currentSet)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	std::list<ACS_RTR_Manager*>::const_iterator manager;

	for( manager = _managerList.begin(); manager != _managerList.end(); ++manager)
	{
		if (messageStoreName.compare((*manager)->getMessageStore()) == 0 )
		{
			(*manager)->rtrchIssue(currentSet);
			break;
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: forceFileEnd
=================================================================== */
void ACS_RTR_Server::forceFileEnd(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	ACS_RTR_Manager* manager = getRTRManager(messageStoreName);

	if(NULL != manager)
	{
		ACS_RTR_TRACE_MESSAGE("Manager found");
		manager->rtrfeIssue();
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

/*===================================================================
   ROUTINE: getCpStatistics
=================================================================== */
bool ACS_RTR_Server::getCpStatistics(const std::string& messageStoreName, CpStatisticsInfo* cpStatisticsInfo, short cpId)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	bool result = false;

	ACS_RTR_Manager* manager = getRTRManager(messageStoreName);

	if(NULL != manager)
	{
		ACS_RTR_TRACE_MESSAGE("Manager found");
		result = manager->getCpStatistics(cpStatisticsInfo, cpId);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}

/*===================================================================
   ROUTINE: getMessageStoreStatistics
=================================================================== */
bool ACS_RTR_Server::getMessageStoreStatistics(const std::string& messageStoreName, StatisticsInfo* msStatisticsInfo)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());
	bool result = false;

	ACS_RTR_Manager* manager = getRTRManager(messageStoreName);

	if(NULL != manager)
	{
		ACS_RTR_TRACE_MESSAGE("Manager found");
		result = manager->getMessageStoreStatistics(msStatisticsInfo);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", ( result ? "TRUE" : "FALSE") );
	return result;
}


//-----------------
// fxInitialize
//-----------------
bool ACS_RTR_Server::fxInitialize(ACE_INT32 waitTimeout)
{
	ACS_RTR_TRACE_FUNCTION;

	// check if the message queue is already opened
	if(ACE_INVALID_HANDLE == m_MsgQueueHandle)
	{
		// open message queue in order to handle message store name change
		struct mq_attr msgQueueAttributes;
		msgQueueAttributes.mq_flags = aca_rtr_communication::MESSAGE_QUEUE_DEFAULT_FLAGS;
		msgQueueAttributes.mq_maxmsg = aca_rtr_communication::MESSAGE_QUEUE_MAX_NUM_OF_MESS;
		msgQueueAttributes.mq_msgsize = aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE;
		msgQueueAttributes.mq_curmsgs = aca_rtr_communication::MESSAGE_QUEUE_CURRENT_NUM_OF_MESS;

		char messageQueueName[128]={0};
		snprintf(messageQueueName, sizeof(messageQueueName)-1, "%s%s", aca_rtr_communication::MESSAGE_QUEUE_PREFIX, aca_rtr_communication::MS_NAME_CHANGE_MESS_QUEUE_NAME);

		m_MsgQueueHandle = ::mq_open(messageQueueName, O_CREAT | O_RDONLY, aca_rtr_communication::MESSAGE_QUEUE_DEFAULT_PERM, &msgQueueAttributes);


		if(ACE_INVALID_HANDLE == m_MsgQueueHandle)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Server message queue:<%s> open failed,  error:<%d>", messageQueueName, errno);
			return false;
		}
		else
		{
			ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: RTR Server message queue:<%s> opened", messageQueueName);
		}
	}

	// TODO rework need
	if (fxCreateRTRStatistics((_rtrStat ? internalRestart : normalStart))) 
	{
		char buf[512] = {0};
		ACE_OS::snprintf(buf, sizeof(buf) - 1, "Error description: %s Error Code: %d", lastErrorDescription(), lastError());
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", buf);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", buf);
		return false;
	}

	if (!ACS_RTR_SystemConfig::instance())
	{
		char buf[512] = {0};
		ACE_OS::snprintf(buf, sizeof(buf) - 1, "%s Error Description: %s Error Code: %d", _RTRSRV_DERR_NEW_SYS_CONFIGURATION, ACS_RTR_SystemConfig::lastErrorDescription(), ACS_RTR_SystemConfig::lastError());

		if (!setLastError(RTRSRV_ERR_SYS_CONFIGURATION, buf, true))
			setLastError(RTRSRV_ERR_SYS_CONFIGURATION, _RTRSRV_DERR_SYS_CONFIGURATION);

		ACS_RTR_TRACE_MESSAGE("%s", buf);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", buf);
		return false;
	}

	bool success = true;
	for ( ; (!(success = ACS_RTR_SystemConfig::instance()->init())) && (!_stopRequested); )
	{
		char buf[512] = {0};
		ACE_OS::snprintf(buf, sizeof(buf) - 1, "%s Error Description: %s Error Code: %d", _RTRSRV_DERR_SYS_CONFIGURATION, ACS_RTR_SystemConfig::lastErrorDescription(), ACS_RTR_SystemConfig::lastError());
		if (!setLastError(RTRSRV_ERR_SYS_CONFIGURATION, buf, true)) setLastError(RTRSRV_ERR_SYS_CONFIGURATION, _RTRSRV_DERR_SYS_CONFIGURATION);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", buf);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", buf);
		ACE_Time_Value tv(0,waitTimeout*1000);
		ACE_OS::sleep(tv);
	}

	return true;
}

//----------------------
// fxStartProtocol
//----------------------
bool ACS_RTR_Server::fxStartProtocol(ACE_INT32 waitTimeout)
{
	ACS_RTR_TRACE_MESSAGE("IN, waitTimeout:<%d>", waitTimeout);

	bool result = false;
	int dsdResult;

	if( NULL != m_DSDServer)
	{
		ACS_RTR_TRACE_MESSAGE("DSD server already up");
		result = true;
	}

	while(!result && !_stopRequested)
	{
		m_DSDServer = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET);
		if (NULL == m_DSDServer)
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: DSD server allocation error");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "DSD server allocation error");
			setLastError(RTRSRV_ERR_NEW_DSD, _RTRSRV_DERR_NEW_DSD);

			waitBeforeRetry(waitTimeout);

			continue;
		}
		// Initialize DSD resources
		dsdResult = m_DSDServer->open();
		// Check the operation result
		if(acs_dsd::ERR_NO_ERRORS != dsdResult)
		{
			ACS_RTR_TRACE_MESSAGE("DSD server open error:<%d>", dsdResult);
			delete m_DSDServer;
			m_DSDServer = NULL;
			waitBeforeRetry(waitTimeout);
			continue; 
		}
		// Publish the RTR service
		dsdResult = m_DSDServer->publish(RTR_DSD::serviceName, RTR_DSD::serviceDomain);

		// Check the operation result
		if( acs_dsd::ERR_NO_ERRORS != dsdResult)
		{
			ACS_RTR_TRACE_MESSAGE("DSD server publish error:<%d>", dsdResult);
			 // Close and delete DSD server object
			m_DSDServer->unregister();
			m_DSDServer->close();
			delete m_DSDServer;
			m_DSDServer = NULL;
			waitBeforeRetry(waitTimeout);
			continue;
		}

		int dsdHandleCount = 0;
		// To get the number of handle
		dsdResult = m_DSDServer->get_handles(NULL, dsdHandleCount);

		if(acs_dsd::ERR_NOT_ENOUGH_SPACE != dsdResult)
		{
			ACS_RTR_TRACE_MESSAGE("DSD server get_handles error:<%d>", dsdResult);
			 // Close and delete DSD server object
			m_DSDServer->unregister();
			m_DSDServer->close();
			delete m_DSDServer;
			m_DSDServer = NULL;
			waitBeforeRetry(waitTimeout);
			continue;
		}

		// Now dsdHandleCount has the correct number of handles to retrieve
		acs_dsd::HANDLE dsdHandles[dsdHandleCount];

		dsdResult = m_DSDServer->get_handles(dsdHandles, dsdHandleCount);

		if( acs_dsd::ERR_NO_ERRORS == dsdResult )
		{
			result = true;
			for(int idx=0; idx < dsdHandleCount; ++idx)
			{
				m_dsdHandles.push_back(dsdHandles[idx]);
			}
		}
		else
		{
			// Close and delete DSD server object
			ACS_RTR_TRACE_MESSAGE("DSD server get_handles error:<%d>", dsdResult);
			m_DSDServer->unregister();
			m_DSDServer->close();
			delete m_DSDServer;
			m_DSDServer = NULL;
			waitBeforeRetry(waitTimeout);
			continue;
		}

	}
	ACS_RTR_TRACE_MESSAGE("OUT, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

//---------------------
// fxStopProtocol
//---------------------
bool ACS_RTR_Server::fxStopProtocol()
{
	ACS_RTR_TRACE_MESSAGE("In");

	if(NULL != m_DSDServer)
	{
		ACS_RTR_TRACE_MESSAGE("Close and delete DSD server object");
		m_DSDServer->unregister();
		m_DSDServer->close();
		delete m_DSDServer;
		m_DSDServer = NULL;
	}
	ACS_RTR_TRACE_MESSAGE("Out");
	return true;
}

//-------------------------------------------------------------------------------------
// fxRecovery
// f a restart happened, this function restore the state of the system
//-------------------------------------------------------------------------------------
bool ACS_RTR_Server::fxRecovery()
{
	ACS_RTR_TRACE_MESSAGE("In");

	bool result = false;

	OmHandler objManager;

	if( ACS_CC_SUCCESS == objManager.Init() )
	{
		if( loadFileJobsFromIMM(&objManager) && loadBlockJobsFromIMM(&objManager) )
		{
			ACS_RTR_TRACE_MESSAGE("All defined manager are created");
			result = true;
			// start all manager thread
			std::list<ACS_RTR_Manager*>::const_iterator managerIterator;
			for( managerIterator = _managerList.begin(); managerIterator != _managerList.end(); ++managerIterator)
			{
				if( (*managerIterator)->open(m_stopEvent) == FAILURE)
				{
					//trace error
					char trace[512] = {0};
					ACE_OS::snprintf(trace, sizeof(trace) - 1, "MANAGER STARTUP failed for the MessageStore:<%s>", (*managerIterator)->getMessageStore().c_str());

					if (!setLastError(RTRSRV_ERR_START_MGR, trace, true))
						setLastError(RTRSRV_ERR_START_MGR, _RTRSRV_DERR_START_MGR);

					ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
				}
			}
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("Job definition load failed!");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Job definition load failed!");
		}

		objManager.Finalize();
	}
	else
	{
		// IMM handler init failed
		ACS_RTR_TRACE_MESSAGE("Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" :"FALSE"));
	return result;
}

bool ACS_RTR_Server::loadFileJobsFromIMM(OmHandler* objManager)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = true;
	ACS_CC_ReturnType getResult;

	std::vector<std::string> fileJobDNList;
	// Get all IMM objects of the class FileJob
	getResult = objManager->getClassInstances(rtr_imm::FileBaseJobClassName, fileJobDNList);

	if(ACS_CC_SUCCESS == getResult)
	{
		ACS_RTR_TRACE_MESSAGE("There are <%zu> fileJod defined", fileJobDNList.size());
		std::vector<std::string>::const_iterator fileJobElement;

		for(fileJobElement = fileJobDNList.begin(); fileJobElement != fileJobDNList.end(); ++fileJobElement)
		{
			std::string fileJobDN(*fileJobElement);
			ACS_RTR_TRACE_MESSAGE("Loading fileJob DN:<%s>", fileJobDN.c_str());

			// get the MS DN
			size_t tagCommaPos = fileJobDN.find_first_of(parseSymbol::comma);

			if( std::string::npos != tagCommaPos)
			{
				// Message store DN
				std::string messageStoreDN(fileJobDN.substr(tagCommaPos+1));

				// get the MS name attribute
				ACS_CC_ImmParameter messageStoreNameAttribute;
				messageStoreNameAttribute.attrName = rtr_imm::MessageStoreNameAttribute;

				getResult = objManager->getAttribute(messageStoreDN.c_str(), &messageStoreNameAttribute );

				// check for message store name attributes
				if( (ACS_CC_FAILURE != getResult) && (0 != messageStoreNameAttribute.attrValuesNum) )
				{
					std::string messageStoreName(reinterpret_cast<char *>(messageStoreNameAttribute.attrValues[0]));

					ACS_RTR_TRACE_MESSAGE("Create a Manager for the MS:<%s>", messageStoreName.c_str());
					ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: Create a File JOB Manager for the MS:<%s>", messageStoreName.c_str());
					// check if already started
					if(isJobDefined(messageStoreName))
					{
						// manager of this job already load
						ACS_RTR_TRACE_MESSAGE("Manager already running for this MS");
						continue;
					}

					// Get fileJob parameters, list of attributes to get
					std::vector<ACS_APGCC_ImmAttribute*> attributeList;

					// to get the transfer queue name
					ACS_APGCC_ImmAttribute transferQueueAttribute;
					transferQueueAttribute.attrName = rtr_imm::TransferQueueAttribute;
					attributeList.push_back(&transferQueueAttribute);

					// to get the CDR counter flag
					ACS_APGCC_ImmAttribute cdrCounterFlagAttribute;
					cdrCounterFlagAttribute.attrName = rtr_imm::CdrCounterFlagAttribute;
					attributeList.push_back(&cdrCounterFlagAttribute);

					// to get the Hold Time
					ACS_APGCC_ImmAttribute holdTimeAttribute;
					holdTimeAttribute.attrName = rtr_imm::HoldTimeAttrribute;
					attributeList.push_back(&holdTimeAttribute);

					// to get the paddingChar
					ACS_APGCC_ImmAttribute paddingCharAttribute;
					paddingCharAttribute.attrName = rtr_imm::PaddingCharAttribute;
					attributeList.push_back(&paddingCharAttribute);

					// to get the file record length
					ACS_APGCC_ImmAttribute recordlengthAttribute;
					recordlengthAttribute.attrName = rtr_imm::FileRecordlengthAttribute;
					attributeList.push_back(&recordlengthAttribute);

					// to get the file size
					ACS_APGCC_ImmAttribute fileSizeAttribute;
					fileSizeAttribute.attrName = rtr_imm::FileSizeAttribute;
					attributeList.push_back(&fileSizeAttribute);

					// to get the fixed file records flag
					ACS_APGCC_ImmAttribute fixedFileRecordsFlagAttribute;
					fixedFileRecordsFlagAttribute.attrName = rtr_imm::FixedFileRecordsFlagAttribute;
					attributeList.push_back(&fixedFileRecordsFlagAttribute);

					// to get the minimum file size
					ACS_APGCC_ImmAttribute minFileSizeAttribute;
					minFileSizeAttribute.attrName = rtr_imm::MinFileSizeAttribute;
					attributeList.push_back(&minFileSizeAttribute);

					// to get the hash key
					ACS_APGCC_ImmAttribute hashKeyAttribute;
					hashKeyAttribute.attrName = rtr_imm::HashKeyAttribute;
					attributeList.push_back(&hashKeyAttribute);

					getResult = objManager->getAttribute(fileJobDN.c_str(), attributeList );
					// check for mandatory attributes
					if( ACS_CC_FAILURE != getResult)
					{
						RTRMS_Parameters parameters;
						memset(&parameters, 0, sizeof(RTRMS_Parameters));
						parameters.OutputType = FileOutput;

						strncpy(parameters.TransferQueue, reinterpret_cast<char *>(transferQueueAttribute.attrValues[0]), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]));
						ACS_RTR_TRACE_MESSAGE("transferQueue:<%s>", parameters.TransferQueue);

						int crdCounterValue = *reinterpret_cast<int*>(cdrCounterFlagAttribute.attrValues[0]);
						parameters.FiCdrFormat = (crdCounterValue == ENABLE);

						parameters.FiFileHoldTime = *reinterpret_cast<unsigned int*>(holdTimeAttribute.attrValues[0]);
						parameters.FiPaddingChar = *reinterpret_cast<int*>(paddingCharAttribute.attrValues[0]);
						parameters.FileRecordLength = *reinterpret_cast<int*>(recordlengthAttribute.attrValues[0]);
						parameters.FileSize = *reinterpret_cast<unsigned int*>(fileSizeAttribute.attrValues[0]);
						parameters.FixedFileRecords = *reinterpret_cast<int*>(fixedFileRecordsFlagAttribute.attrValues[0]);
						parameters.MinFileSize = *reinterpret_cast<unsigned int*>(minFileSizeAttribute.attrValues[0]);

						parameters.HmacMd5 = (0 != hashKeyAttribute.attrValuesNum);

						ACS_RTR_Manager* manager = NULL;
						try
						{
							manager = new (std::nothrow) ACS_RTR_Manager(messageStoreName.c_str(), &parameters );
						}
						catch( const std::exception& ex)
						{
							//trace error
							char trace[512] = {0};
							ACE_OS::snprintf(trace, sizeof(trace) - 1, "RESTART MANAGER FAILED: create ACS_RTR_Manager (MsgStore=%s) thread failed. Error Message: %s", messageStoreName.c_str(), ex.what());
							if (!setLastError(RTRSRV_ERR_NEW_MGR, trace, true)) setLastError(RTRSRV_ERR_NEW_MGR, _RTRSRV_DERR_NEW_MGR);
							ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
							result = false;
							continue;
						}

						//store the file Job DN
						manager->setJobBaseDN(fileJobDN.c_str());

						// add to the new created manager
						_managerList.push_back(manager);

						ACS_RTR_TRACE_MESSAGE("File Manager created for Message Store:<%s>", messageStoreDN.c_str());
						ACS_RTR_LOG(LOG_LEVEL_INFO, "File Manager created for Message Store:<%s>", messageStoreDN.c_str());
					}
					else
					{
						// getAttribute failed
						result = false;
						ACS_RTR_TRACE_MESSAGE("Failed to get attributes of fileJob object DN:<%s>", fileJobDN.c_str());
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get attributes of fileJob object DN:<%s>", fileJobDN.c_str());
					}
				}
				else
				{
					// get message store name failed
					result = false;
					ACS_RTR_TRACE_MESSAGE("Failed to get name attributes of MS object DN:<%s>", messageStoreDN.c_str());
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get name attributes of MS object DN:<%s>", messageStoreDN.c_str());
				}
			}
			else
			{
				// Parse failed
				result = false;
				ACS_RTR_TRACE_MESSAGE("Failed to get parent DN of fileJob object DN:<%s>", fileJobDN.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get parent DN of fileJob object DN:<%s>", fileJobDN.c_str());
			}
		} // for loop on each fileJob defined
	}
	else
	{
		// Get Instance failed
		if (objManager->getInternalLastError() != APGCC_NOT_FOUND)
		{
			ACS_RTR_TRACE_MESSAGE("<0x%X>(%d) != <0x%X>(%d)", objManager->getInternalLastError(), objManager->getInternalLastError(), APGCC_NOT_FOUND, APGCC_NOT_FOUND);
			ACS_RTR_LOG(LOG_LEVEL_WARN, "<0x%X>(%d) != <0x%X>(%d)", objManager->getInternalLastError(), objManager->getInternalLastError(), APGCC_NOT_FOUND, APGCC_NOT_FOUND);
			result = false;
		}

		ACS_RTR_TRACE_MESSAGE("Failed to getClassInstances of <%s>, error:<%d>", rtr_imm::FileBaseJobClassName, objManager->getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to getClassInstances of <%s>, error:<%d>", rtr_imm::FileBaseJobClassName, objManager->getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" :"FALSE"));
	return result;
}

bool ACS_RTR_Server::loadBlockJobsFromIMM(OmHandler* objManager)
{
	bool result = true;
	ACS_CC_ReturnType getResult;

	std::vector<std::string> blockJobDNList;
	// Get all IMM objects of the class FileJob
	getResult = objManager->getClassInstances(rtr_imm::BlockBaseJobClassName, blockJobDNList);

	if(ACS_CC_SUCCESS == getResult)
	{
		ACS_RTR_TRACE_MESSAGE("There are <%zu> Block Job defined", blockJobDNList.size());
		std::vector<std::string>::const_iterator blockJobElement;

		for(blockJobElement = blockJobDNList.begin(); blockJobElement != blockJobDNList.end(); ++blockJobElement)
		{
			std::string blockJobDN(*blockJobElement);
			ACS_RTR_TRACE_MESSAGE("Loading Block Job DN:<%s>", blockJobDN.c_str());

			// get the MS DN
			size_t tagCommaPos = blockJobDN.find_first_of(parseSymbol::comma);

			if( std::string::npos != tagCommaPos)
			{
				// Message store DN
				std::string messageStoreDN(blockJobDN.substr(tagCommaPos+1));

				// get the MS name attribute
				ACS_CC_ImmParameter messageStoreNameAttribute;
				messageStoreNameAttribute.attrName = rtr_imm::MessageStoreNameAttribute;

				getResult = objManager->getAttribute(messageStoreDN.c_str(), &messageStoreNameAttribute );

				// check for message store name attributes
				if( (ACS_CC_FAILURE != getResult) && (0 != messageStoreNameAttribute.attrValuesNum) )
				{
					std::string messageStoreName(reinterpret_cast<char *>(messageStoreNameAttribute.attrValues[0]));

					ACS_RTR_TRACE_MESSAGE("Create a Manager for the MS:<%s>", messageStoreName.c_str());
					ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: Create a BLOCK JOB Manager for the MS:<%s>", messageStoreName.c_str());

					// check if already started
					if(isJobDefined(messageStoreName))
					{
						// manager of this job already load
						ACS_RTR_TRACE_MESSAGE("Manager already running for this MS");
						continue;
					}

					// Get fileJob parameters, list of attributes to get
					std::vector<ACS_APGCC_ImmAttribute*> attributeList;

					// to get the transfer queue name
					ACS_APGCC_ImmAttribute transferQueueAttribute;
					transferQueueAttribute.attrName = rtr_imm::TransferQueueAttribute;
					attributeList.push_back(&transferQueueAttribute);

					// to get the block length
					ACS_APGCC_ImmAttribute blockLengthAttribute;
					blockLengthAttribute.attrName = rtr_imm::BlockLengthAttribute;
					attributeList.push_back(&blockLengthAttribute);

					// to get the block length type
					ACS_APGCC_ImmAttribute blockLengthTypeAttribute;
					blockLengthTypeAttribute.attrName = rtr_imm::BlockLengthTypeAttribute;
					attributeList.push_back(&blockLengthTypeAttribute);

					// to get the Hold Time
					ACS_APGCC_ImmAttribute holdTimeAttribute;
					holdTimeAttribute.attrName = rtr_imm::HoldTimeAttrribute;
					attributeList.push_back(&holdTimeAttribute);

					// to get the paddingChar
					ACS_APGCC_ImmAttribute paddingCharAttribute;
					paddingCharAttribute.attrName = rtr_imm::PaddingCharAttribute;
					attributeList.push_back(&paddingCharAttribute);

					getResult = objManager->getAttribute(blockJobDN.c_str(), attributeList );

					// check for mandatory attributes
					if( ACS_CC_FAILURE != getResult)
					{
						RTRMS_Parameters parameters;
						memset(&parameters, 0, sizeof(RTRMS_Parameters));
						parameters.OutputType = BlockOutput;

						strncpy(parameters.TransferQueue, reinterpret_cast<char *>(transferQueueAttribute.attrValues[0]), sizeof(parameters.TransferQueue)/sizeof(parameters.TransferQueue[0]));
						ACS_RTR_TRACE_MESSAGE("transferQueue:<%s>", parameters.TransferQueue);

						parameters.BlockLength = *reinterpret_cast<int*>(blockLengthAttribute.attrValues[0]);
						parameters.BlockLengthType = *reinterpret_cast<int*>(blockLengthTypeAttribute.attrValues[0]);
						parameters.BlockHoldTime = *reinterpret_cast<unsigned int*>(holdTimeAttribute.attrValues[0]);
						parameters.BlPaddingChar = *reinterpret_cast<int*>(paddingCharAttribute.attrValues[0]);

						ACS_RTR_Manager* manager = 0;
						try
						{
							manager = new (std::nothrow) ACS_RTR_Manager(messageStoreName.c_str(), &parameters);
						}
						catch( const std::exception& ex)
						{
							//trace error
							char trace[512] = {0};
							ACE_OS::snprintf(trace, sizeof(trace) - 1, "RESTART MANAGER FAILED: create ACS_RTR_Manager (MsgStore=%s) thread failed. Error Message: %s", messageStoreName.c_str(), ex.what());
							if (!setLastError(RTRSRV_ERR_NEW_MGR, trace, true)) setLastError(RTRSRV_ERR_NEW_MGR, _RTRSRV_DERR_NEW_MGR);
							ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
							result = false;
							continue;
						}

						//store block Job DN
						manager->setJobBaseDN(blockJobDN.c_str());

						// add the new created manager
						_managerList.push_back(manager);
						ACS_RTR_TRACE_MESSAGE("Block Manager created for Message Store:<%s>", messageStoreDN.c_str());
						ACS_RTR_LOG(LOG_LEVEL_INFO, "Block Manager created for Message Store:<%s>", messageStoreDN.c_str());
					}
					else
					{
						// getAttribute failed
						result = false;
						ACS_RTR_TRACE_MESSAGE("Failed to get attributes of blockJob object DN:<%s>", blockJobDN.c_str());
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get attributes of blockJob object DN:<%s>", blockJobDN.c_str());
					}
				}
				else
				{
					// get message store name failed
					result = false;
					ACS_RTR_TRACE_MESSAGE("Failed to get name attributes of MS object DN:<%s>", messageStoreDN.c_str());
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get name attributes of MS object DN:<%s>", messageStoreDN.c_str());
				}
			}
			else
			{
				// Parse failed
				result = false;
				ACS_RTR_TRACE_MESSAGE("Failed to get parent DN of blockJob object DN:<%s>", blockJobDN.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get parent DN of blockJob object DN:<%s>", blockJobDN.c_str());
			}
		} // for loop on each fileJob defined
	}
	else
	{
		// Get Instance failed
		if (objManager->getInternalLastError() != APGCC_NOT_FOUND)
		{
			ACS_RTR_TRACE_MESSAGE("<0x%X>(%d) != <0x%X>(%d)", objManager->getInternalLastError(), objManager->getInternalLastError(), APGCC_NOT_FOUND, APGCC_NOT_FOUND);
			ACS_RTR_LOG(LOG_LEVEL_WARN, "<0x%X>(%d) != <0x%X>(%d)", objManager->getInternalLastError(), objManager->getInternalLastError(), APGCC_NOT_FOUND, APGCC_NOT_FOUND);
			result = false;
		}

		ACS_RTR_TRACE_MESSAGE("Failed to getClassInstances of <%s>, error:<%d>", rtr_imm::BlockBaseJobClassName, objManager->getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to getClassInstances of <%s>, error:<%d>", rtr_imm::BlockBaseJobClassName, objManager->getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" :"FALSE"));
	return result;

}

int ACS_RTR_Server::svc()
{
	ACS_RTR_TRACE_MESSAGE("RTR Worker Thread started");
	ACS_RTR_LOG(LOG_LEVEL_INFO,
			"\n"
			"\t\t----------------------------------------------\n"
			"\t\t--------- RTR SERVICE THREAD STARTED ---------\n"
			"\t\t----------------------------------------------\n");

	bool recovered = false;
	bool setUpImmCallBacks=false;

	ACE_OS::sleep(5); //Wait for ACA becoming up & running

	while (!_stopRequested)
	{
		if(fxInitialize(RTRSRV_GET_CONFIGURATION_TIMEOUT) && fxStartProtocol(RTRSRV_START_PROTOCOL_TIMEOUT))
		{
			if(!setUpImmCallBacks)
			{
				ACS_RTR_TRACE_MESSAGE("Starting IMM Handler");
				setUpImmCallBacks = startImmHandler();
				if(!setUpImmCallBacks)
				{
					ACS_RTR_TRACE_MESSAGE("Failed to start IMM handler thread");
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to start IMM handler thread");
					continue;
				}
				ACS_RTR_TRACE_MESSAGE("IMM handler thread started");
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "IMM handler thread started");
			}

			if(!recovered)
			{
				ACS_RTR_TRACE_MESSAGE("Check for Recovery");
				recovered = fxRecovery();
				if(!recovered)
				{
					ACS_RTR_TRACE_MESSAGE("Failed to restore previous RTR state");
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to restore previous RTR state");

					//Avoid 100% CPU usage on IMM failures
					sleep(1);

					continue;
				}
				ACS_RTR_TRACE_MESSAGE("RTR state restored");
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "RTR state restored");
			}

			ACS_RTR_TRACE_MESSAGE("Start Server eternal loop");

			fxWorking(RTRSRV_WAIT_DSDEVENT_TIMEOUT, RTRSRV_MAX_RETRIES_ON_ERROR);

			ACS_RTR_TRACE_MESSAGE("Server eternal loop terminated!");

			fxStopProtocol();
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("RTR Server initialize failed!");
			ACE_Time_Value tv(0,100*1000);
			ACE_OS::sleep(tv);
		}
	}

	if( NULL != m_ImmHandler)
	{
		ACS_RTR_TRACE_MESSAGE("Stopping IMM Handler");
		// Stop the IMM handler and wait for it termination
		m_ImmHandler->stopImmHandler();
	}

	fxStopManagers();

	ACS_RTR_LOG(LOG_LEVEL_INFO,
				"\n"
				"\t\t----------------------------------------------\n"
				"\t\t------- RTR SERVICE THREAD TERMINATED --------\n"
				"\t\t----------------------------------------------\n");
	ACS_RTR_TRACE_MESSAGE("RTR Worker Thread terminated");
	return 0;
}

//----------------
// fxWorking
//----------------
bool ACS_RTR_Server::fxWorking(int waitTimeoutSec, unsigned maxRetries)
{
	ACS_RTR_TRACE_MESSAGE("IN, RTR server main loop started");

	bool result = false;

	const unsigned int dsdHanldesOffset = 2U;
	const nfds_t nfds = m_dsdHandles.size() + dsdHanldesOffset;
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	// Set Shutdown event
	fds[0].fd = m_stopEvent;
	fds[0].events = POLLIN;

	// Set message queue handle
	fds[1].fd = m_MsgQueueHandle;
	fds[1].events = POLLIN;

	// Set DSD handle
	for(unsigned int idx = dsdHanldesOffset; idx < nfds; ++idx)
	{
		fds[idx].fd = m_dsdHandles[idx-dsdHanldesOffset];
		fds[idx].events = POLLIN;
	}

	ACE_Time_Value timeOut(waitTimeoutSec);
	ACE_INT32 pollResult;
	unsigned retryCounter = 0;

	while( !_stopRequested && (retryCounter < maxRetries) )
	{
		pollResult = ACE_OS::poll(fds, nfds, &timeOut);

		// Error on poll
		if( 0 > pollResult )
		{
			// Error on poll
			if(errno == EINTR)
			{
				continue;
			}
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Server poll error:<%d>", errno);
			retryCounter++;
			continue;
		}

		retryCounter = 0;

		// Timeout elapsed
		if( 0 == pollResult )
		{
			// Check RTR manager status
			fxGarbageCollector();
			continue;
		}

		// Stop event signaled
		if(fds[0].revents & POLLIN)
		{
			// Received signal of Server thread termination
			ACS_RTR_TRACE_MESSAGE("termination signal received");
			_stopRequested = true;
			break;
		}

		// Message queue signaled
		if(fds[1].revents & POLLIN)
		{
			// Received a new message on the queue
			ACS_RTR_TRACE_MESSAGE("Received a new message on the queue");
			char message[aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE + 1] = {0};
			ssize_t bytesRead = ::mq_receive(m_MsgQueueHandle, message, aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE, aca_rtr_communication::MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY);

			if (bytesRead < 0)
			{
				// Receive error
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Server message queue receive failed, FD:<%d> error:<%d>", fds[1].fd, errno);
			}
			else
			{
				message[bytesRead]= 0;
				ACS_RTR_TRACE_MESSAGE("Message received:<%s>", message);

				// Received message from ACA
				ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: RTR Manager received message:<%s> from ACA", message);
				std::string messageStr(message);

				// Get tag split position
				size_t tagSplitPos = messageStr.find(aca_rtr_communication::MS_NAME_CHANGE_SEPARATOR);

				// Check if the tag is present
				if( std::string::npos != tagSplitPos )
				{
					// get old MS name
					std::string oldMessageStoreName(messageStr.substr(0, tagSplitPos));
					// get new MS name
					std::string newMessageStoreName(messageStr.substr((tagSplitPos+1)));

					ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: MS name:<%s> changed to <%s>", oldMessageStoreName.c_str(), newMessageStoreName.c_str());
					if(!fxRestartManager(oldMessageStoreName, newMessageStoreName))
					{
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: fatal error on RTR Manager restart!");
						// Terminate the Server AMF will restart it
						break;
					}
				}
				else
				{
					// Parse error
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Server failed to parse received message:<%s>", message);
					// Terminate the Server AMF will restart it
					break;
				}
			}
		}

		// DSD request from rtr commands
		for(unsigned int idx = dsdHanldesOffset; idx < nfds; ++idx)
		{
			if(fds[idx].revents & POLLIN)
			{
				ACS_RTR_TRACE_MESSAGE("DSD event received");
				if(!fxDsdProcessing())
				{
					++retryCounter;
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Server failed to handle CMD request");
				}
				// exit for the for loop
				break;
			}
		}
	}

	ACS_RTR_TRACE_MESSAGE("OUT, RTR server main loop terminated!, lastError::<%d>", lastError());
	return result;
}

//------------------------
// fxDsdProcessing
// process dsd events
//-------------------------
bool ACS_RTR_Server::fxDsdProcessing()
{
	ACS_RTR_TRACE_FUNCTION;

	ACS_DSD_Session* session = new (std::nothrow) ACS_DSD_Session();
	if(NULL == session)
	{
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s", _RTRSRV_DERR_NEW_DSD_SESSION);
		if (!setLastError(RTRSRV_ERR_NEW_DSD_SESSION, trace, true)) setLastError(RTRSRV_ERR_NEW_DSD_SESSION, _RTRSRV_DERR_NEW_DSD_SESSION);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		return false;
	}

	//accept the new session and pass it to worker thread
	int res = m_DSDServer->accept(*session);

	if(res<0)
	{	
		// error in accept
		int errCode = m_DSDServer->last_error();
		const char * errText = m_DSDServer->last_error_text();

		//trace error
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s. Error code: %d - Error text: %s", _RTRSRV_DERR_DSD_ACCEPT, errCode, errText);
		delete session;
		return true;
	}
	else 
	{
		char request[RTR_DSD::ACS_DSD_MAXBUFFER] = {0};
		unsigned int reqSize = RTR_DSD::ACS_DSD_MAXBUFFER;
		//TR HR82127
		//BEGIN return value of DSD_Session must be interger.

		ssize_t  received = session->recv(request, reqSize);

		if ((received > 0) && (reqSize != 0))
		{
			// session will be deleted by method/class final user
			fxProcessRequest(session, request);
		}
		else
		{
			//trace error
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s. Error code: %d - Error text: %s", _RTRSRV_DERR_DSD_RECV, session->last_error(),  session->last_error_text());
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

			session->close();
			delete session;
			return false;
		}
	}

	return true;
}

//-------------------------------------
// processRequest
// process an incoming request
//-------------------------------------
void ACS_RTR_Server::fxProcessRequest(ACS_DSD_Session* session,const char* request)
{
	ACS_RTR_TRACE_FUNCTION;

	RTRCommandType cmd = (RTRCommandType)request[0];
	switch(cmd)
	{
	case rtrStartService:
		fxProcessRTRStartService(session,request);
		break;
	case rtrListStatistics:
		fxProcessRTRListStatistics(session, request);
		break;
	case rtrChangeService:
		fxProcessRTRChangeService(session, request);
		break;
	case rtrForcedFileEnd:
		fxProcessRTRForcedFileEnd(session, request);
		break;
	case rtrStopService:
		fxProcessRTRStopService(session, request);
		break;
	default:
		fxProcessUnknown(session,  request);
	}

}

//-----------------------------------------------------
// fxGarbageCollector
// delete stopped threads from manager list
//-----------------------------------------------------
void ACS_RTR_Server::fxGarbageCollector()
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_managerCriticalSection);

	std::list<ACS_RTR_Manager*>::iterator managerIterator;
	managerIterator = _managerList.begin();

	while( (managerIterator != _managerList.end()) )
	{
		if(!(*managerIterator)->isAlive() && !(*managerIterator)->isRunning())
		{
			ACS_RTR_TRACE_MESSAGE("WARN: Found RTR Manager of MS name:<%s> stopped", (*managerIterator)->getMessageStore().c_str());
			ACS_RTR_LOG(LOG_LEVEL_WARN, "WARN: Found RTR Manager of MS name:<%s> stopped", (*managerIterator)->getMessageStore().c_str());

			if((*managerIterator)->restartNeed())
			{
				ACS_RTR_LOG(LOG_LEVEL_WARN, "WARN: RTR Manager of MS name:<%s> need to be restarted", (*managerIterator)->getMessageStore().c_str());

				// create an new RTR Manager object
				try
				{
					ACS_RTR_Manager* manager = new (std::nothrow) ACS_RTR_Manager((*managerIterator)->getMessageStore().c_str(), (*managerIterator)->getRTRParameters());

					manager->setJobBaseDN((*managerIterator)->getJobBaseDN().c_str());

					// Start the RTR manager worker thread
					if (manager->open(m_stopEvent) == FAILURE)
					{
						//trace error
						char trace[512] = {0};
						ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR ON RESTART MANAGER: start of a new RTR Manager thread for MS:<%s> failed. Last error:<%d>", (*managerIterator)->getMessageStore().c_str(), errno);

						ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", trace);
						ACS_RTR_TRACE_MESSAGE("%s", trace);
					}

					// Store in any case the new RTR manager object
					_managerList.push_back(manager);
				}
				catch(const std::exception& ex)
				{
					//trace error
					char trace[512] = {0};
					ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR ON RESTART MANAGER: creation of a new RTR Manager for MS:<%s> failed. Error Message:<%s>", (*managerIterator)->getMessageStore().c_str(), ex.what());
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
					ACS_RTR_TRACE_MESSAGE("%s", trace);
					continue;
				}
			}

			// reclaim memory allocation
			(*managerIterator)->wait();
			delete (*managerIterator);

			//  element from the list
			_managerList.erase(managerIterator++);
		}
		else
			++managerIterator;
	}

}

//--------------------------------------------
//      restart an RTR Manager object
//--------------------------------------------
bool ACS_RTR_Server::fxRestartManager(const std::string& oldMSName, const std::string& newMSName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_managerCriticalSection);

	ACS_RTR_TRACE_MESSAGE("IN, old MS Name:<%s>, new MS name:<%s>", oldMSName.c_str(), newMSName.c_str());

	bool result = true;
	bool found = false;

	std::list<ACS_RTR_Manager*>::iterator managerIterator;
	for( managerIterator = _managerList.begin(); managerIterator != _managerList.end(); ++managerIterator)
	{
		if( oldMSName.compare((*managerIterator)->getMessageStore()) == 0 )
		{
			found = true;
			ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: Stop RTR Manager after MS rename");
			ACS_RTR_TRACE_MESSAGE("INFO: Stop RTR Manager after MS rename");

			(*managerIterator)->fxStopOnRename();
			(*managerIterator)->wait();

			ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: Start RTR Manager after MS rename");
			ACS_RTR_TRACE_MESSAGE("INFO: Start RTR Manager after MS rename");

			// create an new RTR Manager object
			ACS_RTR_Manager* manager = NULL;
			try
			{
				manager = new (std::nothrow) ACS_RTR_Manager(newMSName.c_str(), (*managerIterator)->getRTRParameters());
			}
			catch(const std::exception& ex)
			{
				result = false;
				//trace error
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR ON RESTART MANAGER: creation of a new RTR Manager for MS:<%s> failed. Error Message:<%s>", newMSName.c_str(), ex.what());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
				ACS_RTR_TRACE_MESSAGE("%s", trace);
				break;
			}

			manager->setJobBaseDN((*managerIterator)->getJobBaseDN().c_str());

			// Start the RTR manager worker thread
			if (manager->open(m_stopEvent) == FAILURE)
			{
				//trace error
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR ON RESTART MANAGER: start of a new RTR Manager thread for MS:<%s> failed. Last error:<%d>", newMSName.c_str(), errno);

				ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", trace);
				ACS_RTR_TRACE_MESSAGE("%s", trace);
			}

			// Store in any case the new RTR manager object
			_managerList.push_back(manager);

			// Erase and delete the previous RTR manager object
			_managerList.erase(managerIterator);
			delete (*managerIterator);

			break;
		}
	}

	if(!found)
	{
		//trace error
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: RTR Manager not found for MS:<%s>", oldMSName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
	}

	ACS_RTR_TRACE_MESSAGE("OUT");
	return result;
}

//----------------------
// fxStopManagers
//----------------------
bool ACS_RTR_Server::fxStopManagers()
{
	ACS_RTR_TRACE_FUNCTION;

	std::list<ACS_RTR_Manager*>::const_iterator rtrManager;

	for(rtrManager = _managerList.begin(); rtrManager != _managerList.end(); ++rtrManager)
	{
		if(NULL != (*rtrManager) )
		{
			ACS_RTR_TRACE_MESSAGE("Manager found, stop it");
			// All managers have already received the stop event!
			// just to be sure that the manager thread will terminate
			(*rtrManager)->fxStop();

			(*rtrManager)->wait();

			delete (*rtrManager);
		}

	}
	_managerList.clear();

	return true;
}

//-------------------------
// fxGetRTRManager
//-------------------------
ACS_RTR_Manager* ACS_RTR_Server::fxGetRTRManager(const char* msname)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", msname);

	ACS_RTR_Manager* manager = 0;
	std::list<ACS_RTR_Manager*>::const_iterator managerIterator;

	for( managerIterator = _managerList.begin(); managerIterator != _managerList.end(); ++managerIterator)
	{
		if((*managerIterator)->getMessageStore().compare(msname) == 0 )
		{
			ACS_RTR_TRACE_MESSAGE("Manager found!");
			manager = *managerIterator;
			break;
		}
	}

	return manager;
}

//-------------------------
// getRTRManager
//-------------------------
ACS_RTR_Manager* ACS_RTR_Server::getRTRManager(const std::string& messageStoreName)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", messageStoreName.c_str());

	ACS_RTR_Manager* manager = NULL;
	std::list<ACS_RTR_Manager*>::const_iterator managerIterator;

	for( managerIterator = _managerList.begin(); managerIterator != _managerList.end(); ++managerIterator)
	{
		if( messageStoreName.compare((*managerIterator)->getMessageStore()) == 0 )
		{
			ACS_RTR_TRACE_MESSAGE("Manager found!");
			manager = *managerIterator;
			break;
		}
	}

	return manager;
}

bool ACS_RTR_Server::getParentName(char *parentName, const char* msName)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;
	returnCode=immHandler.Init();
	if(returnCode!=ACS_CC_SUCCESS){
	}
	std::vector<std::string> pd_dnList;

	char *classNamepd =const_cast<char*>(classNameAttr);
	returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);

	sort (pd_dnList.begin(), pd_dnList.end());
	for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end();it2++)
	{

		char msgStoreN[16] = {0};

		ACS_CC_ImmParameter paramToFind;
		paramToFind.attrName = rtr_imm::MessageStoreNameAttribute;
		if (immHandler.getAttribute((*it2).c_str(), &paramToFind ) == ACS_CC_FAILURE)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed in retreiving messageStore name");
			ACS_RTR_TRACE_MESSAGE("Error in retreiving messageStore name");
		}
		else
		{
			strcpy(msgStoreN , reinterpret_cast<char*>(*(paramToFind.attrValues)));
		}


		if ((std::strcmp(msgStoreN,msName)== 0 ))
		{
			strcpy(parentName,(*it2).c_str());
			return true;
		}
	}
	return false;
}

bool ACS_RTR_Server::createRTRJob(const char *parentName, const int outputType, const char *transferQueue,std::string& errorText)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;
	OmHandler objManager;

	if( ACS_CC_SUCCESS == objManager.Init() )
	{
		char immClassName[128] = {0};
		// attribute list of the new object
		std::vector<ACS_CC_ValuesDefinitionType> jobAttributeList;

		// Prepare the object RDN attribute

		ACS_CC_ValuesDefinitionType attributeRDN;

		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;
		char tmpRDN[64] = {0};

		if(FileOutput == outputType )
		{
			attributeRDN.attrName = rtr_imm::FileBaseJobId;
			ACE_OS::strncpy(immClassName, rtr_imm::FileBaseJobClassName, sizeof(immClassName)-1);
		}
		else
		{
			attributeRDN.attrName = rtr_imm::BlockBaseJobId;
			ACE_OS::strncpy(immClassName, rtr_imm::BlockBaseJobClassName, sizeof(immClassName)-1);
		}

		ACE_OS::snprintf(tmpRDN, sizeof(tmpRDN)-1, "%s=1", attributeRDN.attrName);
		void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN)};
		attributeRDN.attrValues = tmpValueRDN;

		jobAttributeList.push_back(attributeRDN);

		// Prepare transfer queue attribute
		ACS_CC_ValuesDefinitionType attributeTransferQueue;
		attributeTransferQueue.attrName = rtr_imm::TransferQueueAttribute;
		attributeTransferQueue.attrType = ATTR_STRINGT;
		attributeTransferQueue.attrValuesNum = 1;
		char tmpTQName[64]={0};
		ACE_OS::strncpy(tmpTQName, transferQueue, sizeof(tmpTQName)-1);

		void* tmpTQValue[1] = { reinterpret_cast<void*>(tmpTQName)};
		attributeTransferQueue.attrValues = tmpTQValue;

		jobAttributeList.push_back(attributeTransferQueue);

		ACS_RTR_TRACE_MESSAGE("Create a new job DN:<%s> for MS DN:<%s>, TQ name:<%s>", tmpRDN, parentName, tmpTQName);

		// create the Imm Object
		if( objManager.createObject(immClassName, parentName, jobAttributeList) == ACS_CC_SUCCESS)
		{
			ACS_RTR_TRACE_MESSAGE("Job successfully created");
			result = true;
		}
		else
		{
			int errorId;
			objManager.getExitCode(errorId,errorText);
		}

		// free resources
		objManager.Finalize();
	}
	else
	{
		// IMM handler init failed
		ACS_RTR_TRACE_MESSAGE("Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" :"FALSE"));
	return result;
}


//------------------------------------
// fxProcessRTRStartService
// process rtrdef command
//------------------------------------
void ACS_RTR_Server::fxProcessRTRStartService(ACS_DSD_Session* session, const char* request)
{
	ACS_RTR_TRACE_FUNCTION;

	const rtr_def_req_t* rtrdef = reinterpret_cast<const rtr_def_req_t*>(request);

	if(fxIsRTRManager(rtrdef->msname, rtrdef->TransferQueue))
	{
		sendResponse(session, CmdPrintouts::TqDefError);
		ACS_RTR_TRACE_MESSAGE("A Job is already defined for the MS:<%s>", rtrdef->msname);
	}
	else
	{
		char parentName[256]={0};

		std::string errorText;
		if(getParentName(parentName, rtrdef->msname))
		{
			if(createRTRJob(parentName, rtrdef->OutputType, rtrdef->TransferQueue,errorText))
			{
				sendResponse(session, CmdPrintouts::Executed);
			}
			else
			{
				sendResponse(session, errorText.c_str());
				ACS_RTR_TRACE_MESSAGE("Failed to create the new job");
			}
		}
		else
		{
			sendResponse(session, CmdPrintouts::MsNotExist);
			ACS_RTR_TRACE_MESSAGE("MS:<%s> not defined", rtrdef->msname);
		}
	}
	session->close();
	delete session;
}

//-------------------------------------
// fxProcessRTRListStatistics
// process rtrls command
//-------------------------------------
void ACS_RTR_Server::fxProcessRTRListStatistics(ACS_DSD_Session* session, const char* request)
{
	ACS_RTR_TRACE_FUNCTION;

	char ms[16] = {0};
	char site[16] = {0};
	char ap[16] = {0};
	RTRMS_Parameters defPar;
	RTRMS_Parameters par;
	bool suppAl = false;
	const rtr_ls_req_t* rtrls = reinterpret_cast<const rtr_ls_req_t*> (request);

	strncpy(ms, rtrls->msname, sizeof(ms) - 1);
	bool stat = rtrls->StatOpt;
	std::list<ACS_RTR_Manager *>::iterator it;
	char msg[(RTR_DSD::ACS_DSD_MAXBUFFER - sizeof(respCode))] = {0};
	int msgLen = 0;

	if(!strcmp(ms,"")) //(rtrls,rtrls -d(Local AP),rtrls -ap AP -d (Remote AP)
	{
		if(rtrls->DefOpt) //default option
		{//rtrls -d
			//reading ok,getting and sendig Default parameters

			defParPrintingFormat(msg,msgLen,defPar);
			cout<<"rtrls -d DATA "<<msg<<endl;
			sendMessage(session,READ_DATA,msgLen,msg);
			sendMessage(session,END_MESSAGE,0,"");
		}
		else
		{
			//rtrls [-s]i
			//check CPSystem
			if(!ACS_RTR_SystemConfig::instance()->isMultipleCPSystem() && !rtrls->mAPOpt )
			{
				//single cp system
				if(!_managerList.empty())
				{//managerList not empty
					for (it = _managerList.begin(); it != _managerList.end(); it++)
					{
						//get messagestore
						string MSname = (*it)->getMessageStore();
						string TQ = (*it)->getTransferQueue();
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "Found the manager MS:<%s>,TQ<%s>",MSname.c_str(),TQ.c_str());
						//format string
						msgLen = ACE_OS::snprintf(msg, sizeof(msg) - 1, "%-16s %s\n",MSname.c_str(),TQ.c_str());
						sendMessage(session,READ_DATA,msgLen,msg);
					}
					//send message
					sendMessage(session,END_MESSAGE,0,"");
				}
				else
				{//managerList empty
					sendMessage(session,NODATA,0,"");
					session->close();
					delete session;
					return;
				}
			}
			else
			{//multipleCPSystem (ALL AP)
				if(!_managerList.empty())
				{//managerList not empty
					for (it = _managerList.begin(); it != _managerList.end(); it++)
					{
						//get messagestore and TQ
						string MSname = (*it)->getMessageStore();
						string TQ = (*it)->getTransferQueue();
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "Found the manager MS:<%s>,TQ<%s>",MSname.c_str(),TQ.c_str());
						//format string
						msgLen = ACE_OS::snprintf(msg, sizeof(msg) - 1, "%-16s %s\n",MSname.c_str(),TQ.c_str());
						//send message
						sendMessage(session,READ_DATA,msgLen,msg);
					}
					sendMessage(session,END_MESSAGE,0,"");
				}
				else
				{//managerList empty
					sendMessage(session,NODATA,0,"");
				}
			}
		}
	}//ms = ""
	else
	{//ms  != ""
		//chek if Messagestore is defined
		char pSite[32] = {0};
		if (!getACASite(ms, pSite, sizeof(pSite)))
		{
			sendResponse(session,CmdPrintouts::MsNotExist);
			session->close();
			delete session;
			return;
		}

		if(!fxIsRTRManager(ms))
		{//error ms doesn't exist
			sendResponse(session,CmdPrintouts::NoMSDef);
			session->close();
			delete session;
			return;
		}
		//ms ok
		if (!ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
		{
			//single cp system,getsite
			//reading parameters
			strcpy(ap,rtrls->apIdentity);
			char errstr[512] = {0};
			if(!getRTRparameters(ms,&par,suppAl,errstr, sizeof(errstr), site))
			{
				sendResponse(session,CmdPrintouts::NoMSDef);
				session->close();
				delete session;
				return;
			}
			//reading ok format message
			parPrintingFormat(msg,msgLen,&par,ms,stat,ap);
			sendMessage(session,READ_DATA,msgLen,msg);
			sendMessage(session,END_MESSAGE,0,"");
		}
		else
		{//multiple cp
			strcpy(ap,rtrls->apIdentity);
			//reading parameters
			char errstr[512] = {0};
			if(!getRTRparameters(ms,&par,suppAl, errstr, sizeof(errstr)))
			{
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errstr);
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", errstr);
				sendResponse(session,CmdPrintouts::NoMSDef);
				session->close();
				delete session;
				return;
			}

			//reading ok, format parameters
			BC_parPrintingFormat(msg,msgLen,&par,ms,stat,ap);
			sendMessage(session,READ_DATA,msgLen,msg);
			sendMessage(session,END_MESSAGE,0,"");
		}
	}//ms != ""
	session->close();
	delete session;
}

//---------------------------------------
// fxProcessRTRChangeService
// process rtrch command
//---------------------------------------
void ACS_RTR_Server::fxProcessRTRChangeService(ACS_DSD_Session* session, const char* request )
{
	ACS_RTR_TRACE_FUNCTION;
	int options[128];
	memset(options,0,128*sizeof(int));
	//	RTRMS_Parameters defpar;
	RTRMS_Parameters FileConfPar;
	char ms[16] = {0};
	bool twoFileSizePar = false;
	bool notused;
	//	char Site[16] = {0};
	unsigned short recSize = 0;
	const rtr_ch_req_t* rtrch = reinterpret_cast<const rtr_ch_req_t*> (request);

	//get options array and messagestore sent from client
	//	rtrch->opt[c_key]=1;
	memcpy(options,rtrch->opt,128*sizeof(int));
	strcpy(ms,rtrch->msname);

	char parentName[256]={0};
	bool ret = getParentName(parentName,ms);
	if(ret==false)
	{
		sendResponse(session,CmdPrintouts::MsNotExist);
		session->close();
		delete session;
		return;
	}

	//check if user set both the Maxfilesize and MinFilezsize
	if((options[h_fileSize] == 1 ) && (options[p_minFileSize] == 1))
		twoFileSizePar = true;


	if( fxIsRTRManager(rtrch->msname,0) == false )//chek if Messagestore is defined
	{//error ms doesn't exist
		sendResponse(session,CmdPrintouts::NoMSDef);
		session->close();
		delete session;
		return;
	}
	if (!ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
	{
		//reading file
		char errstr[512] = {0};
		if (!getRTRparameters(rtrch->msname,&FileConfPar,notused, errstr, sizeof(errstr)))
		{//reafding rtr parameters failed
			sendResponse(session,CmdPrintouts::NoMSDef);
			session->close();
			delete session;
			return;
		}
		//get recsize
		if (!getACAparameters(rtrch->msname, 0, recSize))
		{
			sendResponse(session,CmdPrintouts::MsNotExist);
			session->close();
			delete session;
			return;
		}
	}
	else
	{//multiple
		//reading file
		// MULTICP case
		char errstr[512] = {0};
		if (!getRTRparameters(ms,&FileConfPar,notused, errstr, sizeof(errstr)))
		{//reafding rtr parameters failed
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", errstr);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", errstr);
			sendResponse(session,CmdPrintouts::NoMSDef);
			session->close();
			delete session;
			return;
		}
		//get recsize
		if (!getACAparameters(rtrch->msname,0,recSize))
		{
			sendResponse(session,CmdPrintouts::MsNotExist);
			session->close();
			delete session;
			return;
		}
	}
	//check if user try to chacnge not correct parameter
	if ((rtrch->Blockpar && FileConfPar.OutputType==FileOutput) ||
			(rtrch->Filepar && FileConfPar.OutputType==BlockOutput))
	{
		sendResponse(session,CmdPrintouts::IllegalParam);
		session->close();
		delete session;
		return;
	}
	ACS_RTR_TRACE_MESSAGE("FIXEDfilerecords = %d ", rtrch->parameters.FixedFileRecords );
	ACS_RTR_TRACE_MESSAGE("rec size = %d ", recSize );

	std::string errorText;
	if(modifyRTRJob(parentName, options, rtrch, FileConfPar.OutputType,errorText))
	{
		sendResponse(session, CmdPrintouts::Executed);
		session->close();
		delete session;
		return;
	}
	else
	{
		sendResponse(session, errorText.c_str());
		session->close();
		delete session;
		return;
	}

}

bool ACS_RTR_Server::modifyRTRJob(char *parentName, int* options, const rtr_ch_req_t* rtrch, int OutputType,std::string &errorText)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;
	bool issueChangeRequest=false;

	OmHandler objManager;

	if( ACS_CC_SUCCESS == objManager.Init() )
	{
		std::vector<std::string> jobDNList;
		if( ACS_CC_SUCCESS == objManager.getChildren(parentName, ACS_APGCC_SUBLEVEL, &jobDNList) )
		{
			std::string jobDN;
			std::vector<std::string>::const_iterator jobIterator;
			// ToDo change when messageStoreInfoId class will be removed
			for(jobIterator = jobDNList.begin();jobIterator != jobDNList.end(); ++jobIterator)
			{
				if( (*jobIterator).find("messageStoreInfoId") == std::string::npos )
				{
					jobDN.assign(*jobIterator);
					break;
				}
			}
			if( !jobDN.empty() )
			{
				ACS_RTR_TRACE_MESSAGE("change attribute of job DN<%s>", jobDN.c_str());
				// prepare an transaction id
				std::string transActionName("RTRPARCHANGE");
				ACS_CC_ReturnType changeResult = ACS_CC_SUCCESS;

				if( OutputType == FileOutput)
				{
					// File job parameter change
					//reading default files  ok
					if((ACS_CC_SUCCESS == changeResult) && options[h_fileSize] ) //Only Maxfilesez set
					{
						// Converted the Filesize to bytes to check with minimum file size
						// which is in terms of bytes for TR HH71761
						ACS_CC_ImmParameter attributeFileSize;
						int intattributeFileSize = rtrch->parameters.FileSize;
						attributeFileSize.attrName = rtr_imm::FileSizeAttribute;
						attributeFileSize.attrType = ATTR_UINT32T;
						attributeFileSize.attrValuesNum = 1;
						void* valueInt1[1]={ reinterpret_cast<void*>(&intattributeFileSize)};
						attributeFileSize.attrValues = valueInt1;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeFileSize, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[j_fileHoldTime])
					{
						ACS_CC_ImmParameter attributeFileHoldTime;
						int fileHoldTime = rtrch->parameters.FiFileHoldTime;
						attributeFileHoldTime.attrName= rtr_imm::HoldTimeAttrribute;
						attributeFileHoldTime.attrType=ATTR_UINT32T;
						attributeFileHoldTime.attrValuesNum = 1;
						void* valueInt2[1] = {reinterpret_cast<void*>(&fileHoldTime)};
						attributeFileHoldTime.attrValues = valueInt2;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeFileHoldTime, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[m_fixedFileRecords])
					{
						ACS_CC_ImmParameter attributeFixedFileRecordsFlag;
						ACE_INT32 val = rtrch->parameters.FixedFileRecords;
						attributeFixedFileRecordsFlag.attrName = rtr_imm::FixedFileRecordsFlagAttribute;
						attributeFixedFileRecordsFlag.attrType=ATTR_INT32T;
						attributeFixedFileRecordsFlag.attrValuesNum = 1;
						void* valueInt2[1] = {reinterpret_cast<void*>(&val)};
						attributeFixedFileRecordsFlag.attrValues = valueInt2;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeFixedFileRecordsFlag, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[n_fileRecordLength])
					{
						ACS_CC_ImmParameter attributeFileRecordLength;
						attributeFileRecordLength.attrName = rtr_imm::FileRecordlengthAttribute;
						attributeFileRecordLength.attrType = ATTR_INT32T;
						attributeFileRecordLength.attrValuesNum = 1;
						int intattributeFRecLen = rtrch->parameters.FileRecordLength;
						void* valueInt4[1]={ reinterpret_cast<void*>(&intattributeFRecLen)};
						attributeFileRecordLength.attrValues = valueInt4;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeFileRecordLength, transActionName);
					}

					if( (ACS_CC_SUCCESS == changeResult) && options[o_filePaddingChar])
					{
						ACS_CC_ImmParameter attributeFilePaddingChar;
						attributeFilePaddingChar.attrName = rtr_imm::PaddingCharAttribute;
						attributeFilePaddingChar.attrType = ATTR_INT32T;
						attributeFilePaddingChar.attrValuesNum = 1;
						int intattributePaddingChar = rtrch->parameters.FiPaddingChar;
						void* valueInt5[1]={ reinterpret_cast<void*>(&intattributePaddingChar)};
						attributeFilePaddingChar.attrValues = valueInt5;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeFilePaddingChar, transActionName);
					}

					if( (ACS_CC_SUCCESS == changeResult) && options[b_cdrFileFormat])
					{
						ACS_CC_ImmParameter attributeCdrCounterFlag;
						attributeCdrCounterFlag.attrName = rtr_imm::CdrCounterFlagAttribute;
						attributeCdrCounterFlag.attrType = ATTR_INT32T;
						attributeCdrCounterFlag.attrValuesNum = 1;
						int intCdrCounterFlag = rtrch->parameters.FiCdrFormat;
						void* valueIntFlag[1]={ reinterpret_cast<void*>(&intCdrCounterFlag)};
						attributeCdrCounterFlag.attrValues = valueIntFlag;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeCdrCounterFlag, transActionName);
					}

					if( (ACS_CC_SUCCESS == changeResult) && options[p_minFileSize])
					{
						ACS_CC_ImmParameter attributeMinFileSize;
						attributeMinFileSize.attrName = rtr_imm::MinFileSizeAttribute;
						attributeMinFileSize.attrType = ATTR_UINT32T;
						attributeMinFileSize.attrValuesNum = 1;
						int intattributeMinFileSize = rtrch->parameters.MinFileSize;
						void* valueInt6[1]={ reinterpret_cast<void*>(&intattributeMinFileSize)};
						attributeMinFileSize.attrValues = valueInt6;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeMinFileSize, transActionName);
					}


					if( (ACS_CC_SUCCESS == changeResult) && options[c_key])
					{
						ACS_CC_ImmParameter attributeHashKey;
						attributeHashKey.attrName = rtr_imm::HashKeyAttribute;
						attributeHashKey.attrType = ATTR_NAMET;
						attributeHashKey.attrValuesNum = 1;
						char *passwordEncrypt = NULL;


						if (strcasecmp(rtrch->parameters.FiKey,DefHashKey)==0)
						{

							ACS_RTR_LOG(LOG_LEVEL_INFO, "Undefine hashKey for <%s> ", jobDN.c_str());

							ACS_CC_ImmParameter attributePassword;
							attributePassword.attrName = rtr_imm::HashKeyAttribute;
							attributePassword.attrType = ATTR_NAMET;
							attributePassword.attrValuesNum = 0;
							void* tmpPwdValue[1] ={0};
							attributePassword.attrValues = tmpPwdValue;
							changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributePassword, transActionName);
							if (ACS_CC_SUCCESS == changeResult)
							{
								char hashKeyDn[128] = {0};
								ACE_OS::snprintf(hashKeyDn, sizeof(hashKeyDn)-1, "%s,%s",rtr_imm::PasswordRDN,jobDN.c_str() );			
								changeResult  = objManager.deleteObject(hashKeyDn,transActionName);		
								if ((ACS_CC_FAILURE == changeResult )&& (APGCC_NOT_EXISTS == objManager.getInternalLastError()))
								{
									ACS_RTR_LOG(LOG_LEVEL_INFO, "The haskey for <%s> ,is already undefined ! ",jobDN.c_str());
									changeResult =ACS_CC_SUCCESS;			
								}
							}
							else
							{
								ACS_RTR_LOG(LOG_LEVEL_INFO, "Failed to modify hashKey value for <%s>, with  error <%d>",jobDN.c_str(),objManager.getInternalLastError());
							}

						}
						else
						{

							SecCryptoStatus encryptResult = sec_crypto_encrypt_ecimpassword(&passwordEncrypt,rtrch->parameters.FiKey);
							if( SEC_CRYPTO_OK != encryptResult )
							{
								changeResult = ACS_CC_FAILURE;
								ACS_RTR_TRACE_MESSAGE("%s","Failed in encrypting hashkey value");
							}
							else
							{
								ACS_RTR_TRACE_MESSAGE("%s","Success in encrypting hashkey value");

								//Verify if already hashKey object existing or not
								ACS_CC_ReturnType returnCode;
								std::vector<std::string> rdnList;
								returnCode = objManager.getChildren(jobDN.c_str(),ACS_APGCC_SUBLEVEL,&rdnList);
								string hashKeyDN;
								if(ACS_CC_SUCCESS == returnCode)
								{

									std::vector<std::string>::const_iterator jobChild; 
									for( jobChild = rdnList.begin(); jobChild!=rdnList.end(); ++jobChild)
									{
										if( std::string::npos != (*jobChild).find(rtr_imm::HashKeyAttribute) )
										{
											hashKeyDN = (*jobChild);
											break;
										}
									}


								}

								if(!hashKeyDN.empty())
								{
									ACS_CC_ImmParameter attributePassword;
									attributePassword.attrName = rtr_imm::PasswordAttribute;
									attributePassword.attrType = ATTR_STRINGT;
									attributePassword.attrValuesNum = 1;
									char tmpPwdName[256]={0};

									ACE_OS::strncpy(tmpPwdName, passwordEncrypt, sizeof(tmpPwdName)-1);

									void* tmpPwdValue[1] = { reinterpret_cast<void*>(tmpPwdName)};
									attributePassword.attrValues = tmpPwdValue;
									changeResult = objManager.modifyAttribute(hashKeyDN.c_str(), &attributePassword, transActionName);

									issueChangeRequest=true;
								}
								else
								{
									//Create new hashkey object with encrypted password value
									// attribute list of the new object
									std::vector<ACS_CC_ValuesDefinitionType> jobAttributeList;

									// Prepare the object RDN attribute

									ACS_CC_ValuesDefinitionType attributeRDN;

									attributeRDN.attrType = ATTR_STRINGT;
									attributeRDN.attrValuesNum = 1;
									char tmpRDN[64] = {0};


									attributeRDN.attrName = rtr_imm::HashKeyId;


									ACE_OS::snprintf(tmpRDN, sizeof(tmpRDN)-1, "%s",rtr_imm::PasswordRDN );
									void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN)};
									attributeRDN.attrValues = tmpValueRDN;

									jobAttributeList.push_back(attributeRDN);

									// Prepare transfer queue attribute
									ACS_CC_ValuesDefinitionType attributePassword;
									attributePassword.attrName = rtr_imm::PasswordAttribute;
									attributePassword.attrType = ATTR_STRINGT;
									attributePassword.attrValuesNum = 1;
									char tmpPwdName[255]={0};
									ACE_OS::strncpy(tmpPwdName, passwordEncrypt, sizeof(tmpPwdName)-1);

									void* tmpPwdValue[1] = {reinterpret_cast<void*>(tmpPwdName)};
									attributePassword.attrValues = tmpPwdValue;

									jobAttributeList.push_back(attributePassword);

									// create the Imm Object
									changeResult = objManager.createObject(rtr_imm::EcimPasswordClassName, jobDN.c_str(), jobAttributeList);
									if( changeResult == ACS_CC_SUCCESS)
									{
										ACS_RTR_TRACE_MESSAGE("%s","Success in creating hashkey object");
										//Fetch the hashKey DN to assign it to hashKey attribute	
										char hashKeyDn[128] = {0};
										ACE_OS::snprintf(hashKeyDn, sizeof(hashKeyDn)-1, "%s,%s",rtr_imm::PasswordRDN,jobDN.c_str() );
										void* valueStr[1]={ reinterpret_cast<void*>(hashKeyDn) };
										attributeHashKey.attrValues = valueStr;

										changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeHashKey, transActionName);
										if(changeResult == ACS_CC_SUCCESS)
											ACS_RTR_TRACE_MESSAGE("%s","Success in modifying hashKey attribute value");
										else
											ACS_RTR_TRACE_MESSAGE("%s","Failure in modifying hashKey attribute value");
									}
									else
									{
										ACS_RTR_TRACE_MESSAGE("%s","Failed in creating hashkey object");
									}
								}
							}
							if(NULL != passwordEncrypt)
								free(passwordEncrypt);
						}		

					}
				}
				else
				{
					// Block job parameter change
					if((ACS_CC_SUCCESS == changeResult) && options[a_blockLength])
					{
						ACS_CC_ImmParameter attributeblockLength;
						attributeblockLength.attrName = rtr_imm::BlockLengthAttribute;
						attributeblockLength.attrType = ATTR_INT32T;
						attributeblockLength.attrValuesNum=1;
						int blkLength = rtrch->parameters.BlockLength;
						void* valueintFlag[1]={ reinterpret_cast<void*>(&blkLength)};
						attributeblockLength.attrValues = valueintFlag;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeblockLength, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[d_blockLengthType])
					{
						ACS_CC_ImmParameter attributeblockLengthType;
						attributeblockLengthType.attrName = rtr_imm::BlockLengthTypeAttribute;
						attributeblockLengthType.attrType = ATTR_INT32T;
						attributeblockLengthType.attrValuesNum=1;
						int blkLengthType = rtrch->parameters.BlockLengthType;
						void* valueFlag[1]={ reinterpret_cast<void*>(&blkLengthType)};
						attributeblockLengthType.attrValues = valueFlag;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeblockLengthType, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[e_blockHoldTime])
					{
						ACS_CC_ImmParameter attributeblockHoldTime;
						attributeblockHoldTime.attrName = rtr_imm::HoldTimeAttrribute;
						attributeblockHoldTime.attrType = ATTR_UINT32T;
						attributeblockHoldTime.attrValuesNum = 1;
						int intattributeHoldTime = rtrch->parameters.BlockHoldTime;
						void* valueInt7[1]={ reinterpret_cast<void*>(&intattributeHoldTime)};
						attributeblockHoldTime.attrValues = valueInt7;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeblockHoldTime, transActionName);
					}

					if((ACS_CC_SUCCESS == changeResult) && options[g_blockPaddingChar])
					{
						ACS_CC_ImmParameter attributeBlkPaddingChar;
						attributeBlkPaddingChar.attrName = rtr_imm::PaddingCharAttribute;
						attributeBlkPaddingChar.attrType = ATTR_INT32T;
						attributeBlkPaddingChar.attrValuesNum = 1;
						int intattributePaddingChar = rtrch->parameters.BlPaddingChar;
						void* valueInt6[1]={ reinterpret_cast<void*>(&intattributePaddingChar)};
						attributeBlkPaddingChar.attrValues = valueInt6;

						changeResult = objManager.modifyAttribute(jobDN.c_str(), &attributeBlkPaddingChar, transActionName);
					}
				}

				if( ACS_CC_SUCCESS == changeResult )
				{
					//apply transaction changes
					result = (ACS_CC_SUCCESS == objManager.applyRequest(transActionName));
					if(issueChangeRequest)
					{
						std::string messageStoreName(rtrch->msname);
						bool getResult=false;
						RTRMS_Parameters parameters;
						
						getResult=getManagerParameters(messageStoreName,&parameters);
						if(getResult)
						{	
						parameters.HmacMd5=true;
						setManagerParameters(messageStoreName,&parameters);
						}
						else
						{

							ACS_RTR_TRACE_MESSAGE("Failed getManagerParameters of <%s>", messageStoreName.c_str());
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed getManagerParameters of <%s>", messageStoreName.c_str());

						}	
					}

				}
				else
				{
					//undo of transaction changes
					int errorId;
					objManager.getExitCode(transActionName,errorId,errorText);
					objManager.resetRequest(transActionName);	
				}
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("Failed to get job DN of <%s>", parentName);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to get job DN of <%s>", parentName);
			}
		}
		else
		{
			// Get getChildren failed
			ACS_RTR_TRACE_MESSAGE("Failed to getChildren of <%s>, error:<%d>", parentName, objManager.getInternalLastError());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to getChildren of <%s>, error:<%d>", parentName, objManager.getInternalLastError());
		}

		// free resources
		objManager.Finalize();
	}
	else
	{
		// IMM handler init failed
		ACS_RTR_TRACE_MESSAGE("Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to initialize OM handler, error:<%d>", objManager.getInternalLastError());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" :"FALSE"));
	return result;
}

//--------------------------------------
// fxProcessRTRForcedFileEnd
// process rtrfe command
//--------------------------------------
void ACS_RTR_Server::fxProcessRTRForcedFileEnd(ACS_DSD_Session* session, const char* request)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_Manager* mgr = 0;
	bool success = false;
	const rtr_fe_req_t* rtrfe = reinterpret_cast<const rtr_fe_req_t*>(request);

	char pSite[32] = {0};
	if (!getACASite(rtrfe->msname, pSite, sizeof(pSite)))
	{
		sendResponse(session,CmdPrintouts::MsNotExist);
		session->close();
		delete session;
		return;
	}

	//check if Messagestore is defined
	if (fxIsRTRManager(rtrfe->msname, 0) == false) {
		sendResponse(session, CmdPrintouts::NoMSDef);
		session->close();
		delete session;
		return ;
	}

	//get manager
	mgr = fxGetRTRManager(rtrfe->msname);
	success = mgr->rtrfeIssue();
	if (success)
	{//file output
		sendResponse(session, CmdPrintouts::Executed);
		session->close();
		delete session;
		return ;
	}
	else
	{//block output
		sendResponse(session,CmdPrintouts::NotAllowBlk);
		session->close();
		delete session;
		return ;
	}

	sendResponse(session, CmdPrintouts::Executed);
	session->close();
	delete session;
	return ;
}

//------------------------------------
// fxProcessRTRStopService
// process rtrrm command
//------------------------------------
void ACS_RTR_Server::fxProcessRTRStopService(ACS_DSD_Session* session, const char* request)
{
	ACS_RTR_TRACE_FUNCTION;
	// TODO REWORK NEEDED!!!
	const rtr_rm_req_t* rtrrm = reinterpret_cast<const rtr_rm_req_t*>(request);

	char pSite[32] = {0};
	if (getACASite(rtrrm->msname, pSite, sizeof(pSite)) == false) 
	{
		ACS_RTR_TRACE_MESSAGE("Error: Message Store does not exist");
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Message Store does not exist");
		sendResponse(session, CmdPrintouts::MsNotExist);
		session->close();
		delete session;
		return;
	}

	ACS_RTR_Manager* mgr = 0;

	if (fxIsRTRManager(rtrrm->msname, 0) == false)
	{
		sendResponse(session, CmdPrintouts::NoMSDef);
		session->close();
		delete session;

		ACS_RTR_TRACE_MESSAGE("Error: Message Store not defined");
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Message Store not defined");
		return ;
	}

	//get manager
	mgr = fxGetRTRManager(rtrrm->msname);
	if (mgr == 0) {
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Cannot fulfill rtrrm: cannot find RTR Manager obj for message strore: %s", rtrrm->msname);

		sendResponse(session, CmdPrintouts::NoMSDef);
		session->close();
		delete session;

		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		return ;
	}
	OmHandler omHandler;
	if( omHandler.Init() == ACS_CC_FAILURE )
	{
		ACS_RTR_TRACE_MESSAGE("OmHandler initialize failed.");
	}
	char parentName[256]={0};
	ACS_CC_ReturnType returnCode;
	getParentName(parentName,rtrrm->msname);
	std::vector<std::string> rdnList;
	ACS_RTR_TRACE_MESSAGE("got parent name : %s",parentName);

	returnCode = omHandler.getChildren(parentName,ACS_APGCC_SUBLEVEL,&rdnList);
	string dnName = "";

	if(returnCode == ACS_CC_SUCCESS)
	{

		for(vector<string>::iterator it2=rdnList.begin();it2!=rdnList.end();it2++)
		{
			size_t posMsName=0;
			if(rdnList.size() != 0)
			{
				for(vector<string>::iterator it2=rdnList.begin();it2!=rdnList.end();it2++)
				{
					posMsName=0;
					string objname = (*it2);
					if((posMsName = objname.find("messageStoreInfoId",posMsName))==std::string::npos)
					{
						dnName = objname;
					}
				}
			}

		}
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Error: Failed in getting children for messageStore.");
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed in getting children for messageStore.");
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "Cannot fulfill rtrrm: cannot find RTR Manager obj for message strore: %s", rtrrm->msname);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		sendResponse(session, CmdPrintouts::NoMSDef);
		session->close();
		delete session;

	}
	ACS_RTR_TRACE_MESSAGE("OBJ DELETED = %s",  dnName.c_str()) ;
	returnCode = omHandler.deleteObject(dnName.c_str(), ACS_APGCC_SUBTREE);
	if(returnCode== ACS_CC_SUCCESS)
	{
		ACS_RTR_TRACE_MESSAGE("Return success to remove job");
		sendResponse(session, CmdPrintouts::Executed);
	}
	else
	{
		std::string errorText;
		int errorId;
		omHandler.getExitCode(errorId,errorText);
		ACS_RTR_TRACE_MESSAGE("Return failure to remove job");
		sendResponse(session,errorText.c_str());
	}

	session->close();
	delete session;
	omHandler.Finalize();
}


//------------------------------------
// fxProcessUnknown
// process an unknown request
//------------------------------------
void ACS_RTR_Server::fxProcessUnknown(ACS_DSD_Session* session,const char* request)
{
	ACS_RTR_TRACE_FUNCTION;
	RTRCommandType cmdType = (RTRCommandType)request[0];

	char trace[512] = {0};
	ACE_OS::snprintf(trace, sizeof(trace) - 1, "Received unknown message with command code: %d", cmdType);

	ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
	ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

	sendResponse(session,CmdPrintouts::IntProgFault);
	session->close();
	delete session;
}


//---------------------------------------------------
// sendResponse
// send a response through the dsd session
//---------------------------------------------------
bool ACS_RTR_Server::sendResponse(ACS_DSD_Session* session, const  char* code)
{
	ACS_RTR_TRACE_FUNCTION;
	bool retCode = true;
	void* response = 0;
	unsigned int respSize = strlen(code);

	response = malloc(respSize);
	memcpy(response, code, respSize);

	// XCASALE: Fix to avoid getting SIGPIPE signal.
	if ( session->sendf(response, respSize, MSG_NOSIGNAL) < 0 ) {
		retCode = false;
	}
	free(response);
	return retCode;
}

//--------------------------------------------------------------------------------
// isRTRManager
// check if a manager related to the message store already exists
//--------------------------------------------------------------------------------
bool ACS_RTR_Server::fxIsRTRManager(const char* msname, const char* tq)
{
	ACS_RTR_TRACE_FUNCTION;
	bool found = false;
	for (std::list<ACS_RTR_Manager*>::iterator it = _managerList.begin(); it != _managerList.end(); it++)
	{
		ACS_RTR_Manager* rtrMgr = (*it);
		if (tq == 0)
		{
			if ( strcmp(rtrMgr->getMessageStore().c_str(),msname) == 0 )
			{
				found = true;
				break;
			}
		}
		else
		{
			if ( (strcmp(rtrMgr->getMessageStore().c_str(),msname) == 0) ||
					(strcmp(rtrMgr->getTransferQueue().c_str(),tq) == 0))
			{
				found = true;
				break;
			}
		}
	}
	return found;
}

//-------------------------------
// fxCreateRTRStatistics
//-------------------------------
int ACS_RTR_Server::fxCreateRTRStatistics(int /*startMode*/)
{
	return 0;
}

//------------------------------
//  defParPrintingFormat
//------------------------------
void ACS_RTR_Server::defParPrintingFormat(char* str,int& len,const RTRMS_Parameters& defPar)
{
	ACS_RTR_TRACE_FUNCTION;
	char str2[10000] = {0};

	(void) defPar;
	const int BlockLength = 4096;
	const char *BlockLengthType = "FIXED"; 
	const int BlockHoldTime = 60;
	const int BlPaddingChar = 0;
	const int FileSize = 1024;
	const int MinFileSize = 0;
	const int FiFileHoldTime = 300;
	const char *FixedFileRecords = "NO";
	const int FileRecordLength = 2048;
	const int FiPaddingChar = 0;

	sprintf(str,"%s\n","DEFAULT PARAMETER VALUES");
	strcat(str,"\n");
	sprintf(str2,"%s %8s %d\n","Block length","",BlockLength);
	strcat(str,str2);

	sprintf(str2,"%s %3s %s\n","Block length type","",BlockLengthType);
	strcat(str,str2);

	sprintf(str2,"%s %2s %d\n","Block holding time","",BlockHoldTime);
	strcat(str,str2);

	sprintf(str2,"%s %2s %d\n","Block padding char","",BlPaddingChar);
	strcat(str,str2);

	sprintf(str2,"%s %11s %d\n","File size","",FileSize);
	strcat(str,str2);

	if (((!ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())&&(MinFileSize != 0))
			|| (ACS_RTR_SystemConfig::instance()->isMultipleCPSystem()))
	{
		sprintf(str2,"%s %3s %d\n","Minimum File Size","",MinFileSize);
		strcat(str,str2);

	}
	sprintf(str2,"%s %3s %d\n","File holding time","",FiFileHoldTime);
	strcat(str,str2);

	sprintf(str2,"%s %2s %s\n","Fixed file records","",FixedFileRecords);
	strcat(str,str2);

	sprintf(str2,"%s %2s %d\n","File record length","",FileRecordLength);
	strcat(str,str2);

	sprintf(str2,"%s %3s %d\n","File padding char","",FiPaddingChar);
	strcat(str,str2);
	len = (int)strlen(str);
}

//------------------
// sendMessage
//------------------
bool ACS_RTR_Server::sendMessage(ACS_DSD_Session* ses,respCode code,unsigned int len,
		const char* data)
{	
	ACS_RTR_TRACE_FUNCTION;
	char buf[RTR_DSD::ACS_DSD_MAXBUFFER] = {0};
	unsigned int msgSize = sizeof(respCode) + len + 1;
	//copy data
	memcpy(buf,&code,sizeof(respCode));
	buf[sizeof(respCode) + 1]='\0';

	strcpy((buf + sizeof(respCode)),data);

	// XCASALE: Fix to avoid getting SIGPIPE signal.
	if( ses->sendf(buf, msgSize, MSG_NOSIGNAL) < 0 )
	{//sending failed
		return false;
	}
	return true;
}

//------------------------
// parPrintingFormat
//------------------------
void ACS_RTR_Server::parPrintingFormat(char* str,int& len,const RTRMS_Parameters* par, const char* msname,bool stat,const char* ap)
{
	ACS_RTR_TRACE_FUNCTION;
	char str2[10000] = {0};
	int type = 1;
	if(!strcmp(str,""))
		sprintf(str,"%s\n","RTR TABLE");
	strcat(str,"\n");
	if (par->OutputType == BlockOutput)
	{//BlockOutput
		if(strcmp(ap,""))
		{
			sprintf(str2,"%-5s %-16s %s\n","NODE","MESSAGE STORE","BLOCK TRANSFER QUEUE");
			strcat(str,str2);
			char ms_site[34]={0};						//HK54866
			ACE_OS::snprintf(ms_site,33,"%s ",msname);
			ACE_OS::snprintf(str2,66,"%-5s %-16s %s\n",ap,ms_site,par->TransferQueue);
			strcat(str,str2);
		}
		else
		{
			sprintf(str2,"%-22s %s\n","MESSAGE STORE","BLOCK TRANSFER QUEUE");
			strcat(str,str2);
			char ms_site[34]={0};                                           //HK54866
			ACE_OS::snprintf(ms_site,33,"%s ",msname);
			ACE_OS::snprintf(str2,66,"%-22s %s\n",ms_site,par->TransferQueue);
			strcat(str,str2);
		}

		sprintf(str2,"%s %9s %d\n","Block length","",par->BlockLength);
		strcat(str,str2);

		const char* blkLengthType = "FIXED";
		if (par->BlockLengthType == FIXED)
			blkLengthType = "FIXED";
		else if (par->BlockLengthType == VARIABLE)
			blkLengthType = "VARIABLE";
		else if (par->BlockLengthType == EVEN)
			blkLengthType = "EVEN";
		sprintf(str2,"%s %4s %s\n","Block length type","", blkLengthType);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","Block holding time","",par->BlockHoldTime);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","Block padding char","",par->BlPaddingChar);
		strcat(str,str2);

	}
	else
	{//file output
		type = 0;
		if(strcmp(ap,""))
		{
			sprintf(str2,"%-5s %-16s %s\n","NODE","MESSAGE STORE","FILE TRANSFER QUEUE");
			strcat(str,str2);
			char ms_site[34]={0};                                           //HK54866
			ACE_OS::snprintf(ms_site,33,"%s ",msname);
			ACE_OS::snprintf(str2,66,"%-5s %-16s %s\n",ap,ms_site,par->TransferQueue);
			strcat(str,str2);
		}
		else
		{
			sprintf(str2,"%-22s %s\n","MESSAGE STORE","FILE TRANSFER QUEUE");
			strcat(str,str2);
			char ms_site[34]={0};                                           //HK54866
			ACE_OS::snprintf(ms_site,33,"%s ",msname);
			ACE_OS::snprintf(str2,66,"%-22s %s\n",ms_site,par->TransferQueue);
			strcat(str,str2);
		}

		sprintf(str2,"%s %12s %d\n","File size","",par->FileSize);
		strcat(str,str2);

		if(par->MinFileSize != 0)
		{
			sprintf(str2,"%s %4s %d\n","Minimum File Size","",par->MinFileSize);
			strcat(str,str2);

		}
		sprintf(str2,"%s %4s %d\n","File holding time","",par->FiFileHoldTime);
		strcat(str,str2);
		const char* fixedFileRec = "NO";
		if (par->FixedFileRecords == FIXED)
			fixedFileRec = "YES";
		else if (par->FixedFileRecords == VARIABLE)
			fixedFileRec = "NO";
		sprintf(str2,"%s %3s %s\n","Fixed file records","", fixedFileRec);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","File record length","",par->FileRecordLength);
		strcat(str,str2);

		sprintf(str2,"%s %4s %d\n","File padding char","",par->FiPaddingChar);
		strcat(str,str2);

	}

	if (par->OutputType == FileOutput && par->HmacMd5)
	{
		sprintf(str2,"%s\n","HMACMD5 Key DEFINED");
		strcat(str,str2);

	}
	if (par->OutputType == FileOutput && par->FiCdrFormat) {
		sprintf(str2,"%s\n","CDR Counting ENABLED"); strcat(str,str2);
	}
	if(stat) {
		char statString[10000] = {0};
		//get RTR manager
		ACS_RTR_Manager* mng = fxGetRTRManager(msname);
		//get statistics
		RTR_statistics* pstat = mng->getStatistics();
		//format sattistics info
		if (pstat) pstat->getFormattedBuffer(statString, sizeof(statString), type); strcat(str,"\n"); strcat(str,statString);
	}
	len = (int)strlen(str);
}

//------------------------------
// BC_parPrintingFormat
//------------------------------
void ACS_RTR_Server::BC_parPrintingFormat(char* str,int& len,const RTRMS_Parameters* par, const char* msname,bool stat,const char* ap) {
	ACS_RTR_TRACE_FUNCTION;
	char str2[10000] = {0};
	int type = 1;
	if(!strcmp(str,""))
		sprintf(str,"%s\n","RTR TABLE");
	strcat(str,"\n");
	if (par->OutputType == BlockOutput)
	{//BlockOutput
		sprintf(str2,"%-5s %-16s %s\n","NODE","MESSAGE STORE","BLOCK TRANSFER QUEUE");
		strcat(str,str2);

		sprintf(str2,"%-5s %-16s %s\n",ap,msname,par->TransferQueue);
		strcat(str,str2);

		sprintf(str2,"%s %9s %d\n","Block length","",par->BlockLength);
		strcat(str,str2);

		const char* blkLengthType = "FIXED";
		if (par->BlockLengthType == FIXED)
			blkLengthType = "FIXED";
		else if (par->BlockLengthType == VARIABLE)
			blkLengthType = "VARIABLE";
		else if (par->BlockLengthType == EVEN)
			blkLengthType = "EVEN";
		sprintf(str2,"%s %4s %s\n","Block length type","", blkLengthType);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","Block holding time","",par->BlockHoldTime);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","Block padding char","",par->BlPaddingChar);
		strcat(str,str2);

	}
	else
	{//file output
		type = 0;
		sprintf(str2,"%-5s %-16s %s\n","NODE","MESSAGE STORE","FILE TRANSFER QUEUE");
		strcat(str,str2);

		sprintf(str2,"%-5s %-16s %s\n",ap,msname,par->TransferQueue);
		strcat(str,str2);

		sprintf(str2,"%s %12s %d\n","File size","",par->FileSize);
		strcat(str,str2);
		//in blade cluster the minimum file size is always printed
		sprintf(str2,"%s %4s %d\n","Minimum File Size","",par->MinFileSize);
		strcat(str,str2);

		sprintf(str2,"%s %4s %d\n","File holding time","",par->FiFileHoldTime);
		strcat(str,str2);

		const char* fixedFileRec = "NO";
		if (par->FixedFileRecords == FIXED)
			fixedFileRec = "YES";
		else if (par->FixedFileRecords == VARIABLE)
			fixedFileRec = "NO";
		sprintf(str2,"%s %3s %s\n","Fixed file records","", fixedFileRec);
		strcat(str,str2);

		sprintf(str2,"%s %3s %d\n","File record length","",par->FileRecordLength);
		strcat(str,str2);

		sprintf(str2,"%s %4s %d\n","File padding char","",par->FiPaddingChar);
		strcat(str,str2);

	}
	sprintf(str2,"%s %6s %s\n","Generation time","","00:00"); // par->GenerationTime);
	strcat(str,str2);

	sprintf(str2,"%s %11s %s\n","Reset flag","","NO"); // par->ResetFlag);
	strcat(str,str2);

	if (par->OutputType == FileOutput && par->HmacMd5)
	{
		sprintf(str2,"%s\n","HMACMD5 Key DEFINED");
		strcat(str,str2);

	}
	if (par->OutputType == FileOutput && par->FiCdrFormat)
	{
		sprintf(str2,"%s\n","CDR Counting ENABLED");
		strcat(str,str2);

	}
	if(stat)
	{
		char statString[10000] = {0};
		//		int statLen = 0;
		std::list<ACS_RTR_ACAChannel*> lch;
		std::list<ACS_RTR_ACAChannel*>::iterator itch;
		string cpname = "";
		//COMMON part
		ACS_RTR_Manager* mng = fxGetRTRManager(msname);
		//get statistics
		RTR_statistics* pstat = mng->getStatistics();
		if (pstat) pstat->statParFormat(statString, sizeof(statString), type);// statLen);
		strcat(str,"\n");
		strcat(str,statString);
		//CP part,get all channel for this messagestore
		lch = mng->getChannelList();
		for(itch = lch.begin(); itch != lch.end(); itch++) {
			pstat = (*itch)->getStatistics();
			short cpid = (*itch)->getCpID();
			cpname = ACS_RTR_SystemConfig::instance()->cs_getCPName(cpid);
			//format data
			strcat(str,"\n");
			if (pstat) pstat->cpStatFormat(statString, sizeof(statString), cpname);
			strcat(str,statString);
		}
	}
	len = (int)strlen(str);
}

//----------------
// checkDatadisk
//----------------
bool ACS_RTR_Server::checkDatadisk()
{
	ACS_RTR_TRACE_FUNCTION;
	bool datadisk = false;
	char szPath[FILENAME_MAX] = {0};
	ACE_INT32 dwLen = FILENAME_MAX;
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_DNFPath_ReturnTypeT  retCode;

	retCode = myAPGCCCommonLib.GetFileMPath( RTR_NBI::FILEM_ATTRIBUTE, szPath, dwLen );
	if (retCode != ACS_APGCC_DNFPATH_SUCCESS)
	{
		datadisk = false;
		char msg[128] = {0};
		ACE_OS::snprintf(msg, sizeof(msg) - 1, "Failed to get data disk path  %s [ERROR CODE = %d] ", szPath, retCode );

	}
	else
	{
		DIR* pDir = opendir ( szPath);
		if ( !pDir )
		{
			//create directory
			if(ACE_OS::mkdir(szPath, ACE_DEFAULT_FILE_PERMS))
			{	
				datadisk = true;
			}
		}
		else
		{
			datadisk = true;
		}
		if (!datadisk)
		{
			char buf[512] = {0};
			ACE_OS::sprintf(buf, "Error accessing to data disk. WinErr: %d", ACE_OS::last_error());
			if (!setLastError(RTRSRV_ERR_CHECK_DATADISK, buf, true)) setLastError(RTRSRV_ERR_CHECK_DATADISK, _RTRSRV_DERR_CHECK_DATADISK);
		}
		closedir(pDir);
	}
	return datadisk;
}

bool ACS_RTR_Server::startImmHandler()
{
	ACS_RTR_TRACE_MESSAGE("In");

	bool result = false;

	// create the IMM handler object
	if( NULL == m_ImmHandler)
		m_ImmHandler = new (std::nothrow) ACS_RTR_ImmHandler(this);

	if( NULL != m_ImmHandler)
	{
		// Configure the OI and start the handler thread
		if( SUCCESS == m_ImmHandler->open() )
		{
			ACS_RTR_TRACE_MESSAGE("IMM Handler thread started!");
			result = true;
		}
		else
		{
			// error on IMM handler
			ACS_RTR_TRACE_MESSAGE("IMM Handler failed to start");
			delete m_ImmHandler;
			m_ImmHandler = NULL;
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE") );

	return result;
}

void ACS_RTR_Server::waitBeforeRetry(int waitTimeOutSec)
{
	ACS_RTR_TRACE_MESSAGE("IN, waitTimeOut:<%d> sec", waitTimeOutSec);

	const nfds_t nfds = 1U;
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_stopEvent;
	fds[0].events = POLLIN;

	ACE_Time_Value timeout(waitTimeOutSec);

	ACE_INT32 pollResult;
	while(true)
	{
		// Waits for 5s before re-try, exit in case of a stop service request
		pollResult = ACE_OS::poll(fds, nfds, &timeout);

		// Error on poll
		if( -1 == pollResult )
		{
			// Error on poll
			if(errno == EINTR)
			{
				continue;
			}
			ACS_RTR_TRACE_MESSAGE("exit after error=<%d>", errno);
			break;
		}

		/* Time out happened */
		if( 0 == pollResult )
		{
			ACS_RTR_TRACE_MESSAGE("time-out expired");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received signal of Server thread termination
			ACS_RTR_TRACE_MESSAGE("termination signal received");
			break;
		}
	}
	ACS_RTR_TRACE_MESSAGE("OUT");
}




