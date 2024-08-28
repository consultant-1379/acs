//********************************************************************************
//
// NAME
// ACS_RTR_Manager.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2007-05-10 by GP/AP/A QPAOELE
//
// DESCRIPTION 
// This class implements a thread that handles a definition.
//
//********************************************************************************
#include "acs_rtr_manager.h"
#include "acs_rtr_acachannel.h"
#include "acs_rtr_outputqueue.h"
#include "acs_rtr_events.h"
#include "acs_rtr_statistics.h"
#include "acs_rtr_cpstatistics_impl.h"
#include "acs_rtr_statistics_impl.h"
#include "acs_rtr_lib.h"


#include "acs_rtr_systemconfig.h"
#include "acs_rtr_gohblockreporter.h"
#include "acs_rtr_gohfilereporter.h"
#include "acs_rtr_server.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include "acs_aca_constants.h"
#include "aes_ohi_extfilehandler2.h"

#include <crypto_status.h>
#include <crypto_api.h>
#include <stdexcept>
#include <sys/eventfd.h>
#include <mqueue.h>


#define ATTACH_RETRIES 86400
#define START_RETIRES 10

namespace
{
	//----------
	// Constants
	//----------
	const ACE_TCHAR * const _RTRMGR_DERR_NO = "No Error.";
	const ACE_TCHAR * const _RTRMGR_DERR_INTERNAL = "Internal Error.";
	const ACE_TCHAR * const _RTRMGR_DERR_TQ_EXISTS = "Cannot validate the transfer queue on GOH.";
	const ACE_TCHAR * const _RTRMGR_DERR_TQ_RESTORED_EXISTS = "Cannot validate the transfer queue on GOH after a restart.";
	const ACE_TCHAR * const _RTRMGR_DERR_WRITE_PARAMETERS = "Cannot write the RTR configuration file on the disk.";
	const ACE_TCHAR * const _RTRMGR_DERR_OUT_QUEUE = "Cannot create the OutputQueue.";
	const ACE_TCHAR * const _RTRMGR_DERR_NEW_GOH_THREAD = "Cannot create Goh Reporter object.";
	const ACE_TCHAR * const _RTRMGR_DERR_START_GOH_THREAD = "Cannot start Goh Reporter thread.";
	const ACE_TCHAR * const _RTRMGR_DERR_GET_CP_FOR_NAME = "Error in 'getCPsForName(...' on the aca interface.";
	const ACE_TCHAR * const _RTRMGR_DERR_STATISTICS = "Cannot create statistics file object";
	const ACE_TCHAR * const _RTRMGR_DERR_NEW_ACA_CHANNEL = "Cannot create ACAChannel thread towards MSD.";
	const ACE_TCHAR * const _RTRMGR_DERR_START_ACA_CHANNEL = "Cannot start the ACAChannel thread towards MSD.";
	const ACE_TCHAR * const _RTRMGR_DERR_MAP_INSERT = "Error inserting element in channel map.";
	const ACE_TCHAR * const _RTRMGR_DERR_GET_CPID = "Cannot retrieve the CPID.";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_Manager);

//----------------------------------------------------------------------------------
//      Constructor
//----------------------------------------------------------------------------------
ACS_RTR_Manager::ACS_RTR_Manager(const char* msname, const RTRMS_Parameters* parameters)
: _msname(msname),
  m_messageStoreRecordSize(0),
  _tq(),
  _rtrParams(NULL),
  _acaInterface(NULL),
  _eventHandler(NULL),
  _msStat(NULL),
  _gohReporter(NULL),
  _outputQueue(NULL),
  _channelMap(),
  m_stopEvent(ACE_INVALID_HANDLE),
  m_ChildStopEvent(eventfd(0,0)),
  m_JobRemoveEvent(eventfd(0,0)),
  m_MsgQueueHandle(ACE_INVALID_HANDLE),
  _stopManager(false),
  _rtrrmIssued(false),
  m_jobBasedDN(),
  m_DataRecordKeys(),
  _gohReporterCriticalSection()
{
	ACS_RTR_TRACE_MESSAGE("IN, MS:<%s>", _msname.c_str());

	if(NULL == msname)
	{
		throw std::invalid_argument("ERROR: Invalid message store name");
	}

	if(NULL == parameters )
	{
		throw std::invalid_argument("ERROR: Invalid RTR parameters");
	}

	//create message store statistics file object
	_msStat = new (std::nothrow) RTR_statistics(_msname);

	if(NULL == _msStat)
	{
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "ERROR: cannot create statistics file object for the message store:<%s>, error:<%d>", _msname.c_str(), errno);
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "<%s>", tracep);
		throw std::runtime_error(tracep);
	}

	_rtrParams = new (std::nothrow) RTRMS_Parameters();

	if (NULL == _rtrParams)
	{
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "ERROR: cannot create RTRMS_Parameters for the message store:<%s>, error:<%d>", _msname.c_str(), errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "<%s>", tracep);
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		throw std::runtime_error("Memory Error: cannot create RTRMS_Parameters");
	}

	memset(_rtrParams, 0, sizeof(RTRMS_Parameters));
	memcpy(_rtrParams, parameters, sizeof(RTRMS_Parameters));

	_tq = _rtrParams->TransferQueue;

	_eventHandler = new (std::nothrow) RTR_Events();
	if (NULL == _eventHandler)
	{
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "ERROR: cannot create event handler object for the message store:<%s>, error:<%d>", _msname.c_str(), errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "<%s>", tracep);
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		throw std::runtime_error("Memory Error: cannot create event handler object");
	}

	_acaInterface = new (std::nothrow) ACS_ACA_MessageStoreInterface();
	if(NULL == _acaInterface)
	{
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "ERROR: cannot create ACS_ACA_MessageStoreInterface for the message store:<%s>, error:<%d>", _msname.c_str(), errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "<%s>", tracep);
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		throw std::runtime_error("Memory Error: cannot create ACS_ACA_MessageStoreInterface");
	}

	memset(m_DataRecordKeys.ipad, '\0', sizeof(m_DataRecordKeys.ipad));
	memset(m_DataRecordKeys.opad, '\0', sizeof(m_DataRecordKeys.opad));
	memset(m_DataRecordKeys.skey, '\0', sizeof(m_DataRecordKeys.skey));

	setLastError(RTRMGR_NO_ERROR, _RTRMGR_DERR_NO);
}

//-------------------
//      Destructor
//-------------------
ACS_RTR_Manager::~ACS_RTR_Manager()
{
	ACS_RTR_TRACE_MESSAGE("IN, Message store:<%s>", _msname.c_str());

	if(_rtrParams) delete _rtrParams;
	if(_eventHandler) delete _eventHandler;
	if(_acaInterface) delete _acaInterface;
	if(_msStat) delete _msStat;

	if( ACE_INVALID_HANDLE != m_ChildStopEvent)
		 ACE_OS::close(m_ChildStopEvent);

	if( ACE_INVALID_HANDLE != m_JobRemoveEvent)
		 ACE_OS::close(m_JobRemoveEvent);

	if( ACE_INVALID_HANDLE != m_MsgQueueHandle)
		::mq_close(m_MsgQueueHandle);

	if(_outputQueue)
	{
		ACS_RTR_TRACE_MESSAGE("Deleting _outputQueue");
		delete _outputQueue;
		_outputQueue = NULL;
	}

}

int ACS_RTR_Manager::open(const ACE_HANDLE& stopHandle)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());
	int result = FAILURE;

	if( getDataRecordKeys() && getMessageStoreRecordSize() )
	{
		m_stopEvent = stopHandle;

		if( (FileOutput == _rtrParams->OutputType) && _rtrParams->HmacMd5)
		{
			if( getHMACMD5Password() )
			{
				ACS_RTR_TRACE_MESSAGE(" activate thread Manager for MS:<%s>, FILE JOB WITH HASH KEY", _msname.c_str());
				ACS_RTR_LOG(LOG_LEVEL_INFO, "RTR Manager thread activate for MS:<%s>, FILE JOB", _msname.c_str());
				result = activate();
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("Failed to get MD5 password!");
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "RTR Manager Failed to get MD5 password");
			}
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE(" activate thread Manager for MS:<%s>", _msname.c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "RTR Manager thread activate for MS:<%s>", _msname.c_str());
			result = activate();
		}

		ACS_RTR_TRACE_MESSAGE(" activation result:<%s>", ( (0 == result) ? "OK":"NOT OK") );
	}

	if(FAILURE == result)
	{
		// To try to restart later
		setLastError(RTRMGR_ERR_INTERNAL, _RTRMGR_DERR_INTERNAL);
	}

    return result;
}


//--------------
// Manager worker thread
//--------------
int ACS_RTR_Manager::svc( )
{
	ACS_RTR_TRACE_MESSAGE("Thread Manager for MS:<%s> started", _msname.c_str());

	//validate transfer queue
	for(int retry = 0; ( fxTQExists() == false ) && isRunning(); ++retry)
	{
		ACS_RTR_TRACE_MESSAGE("Warning: [WAITING FOR TQ UP] MSG STORE <%s>. TQ DOWN!!!, ATTEMPTS:<%d>", _msname.c_str(), retry);
		ACS_RTR_LOG(LOG_LEVEL_WARN, "Warning: [WAITING FOR TQ UP] MSG STORE <%s>. TQ DOWN!!!, ATTEMPTS:<%d>", _msname.c_str(), retry);

		// wait before retry or shutdown event
		waitBeforeRetry(RTRMGR_TQ_ERROR_RETRY_TIMEOUT);
	}

	if (_stopManager) return 0;

	// Init message store statistics object
	_msStat->statisticsInfoInit(m_jobBasedDN);

	// create the shared output queue
	if(!fxCreateSharedQueue() ) return 1;

	// create the GOH communication part
	if (!fxCreateGohReporter())	return 1;

	// create the ACA communication part
	if (!fxCreateAcaChannels())	return 1;

	// open message queue
	openMsgQueue();

	//Prepare handle structure for shutdown event and message queue
	ACE_INT32 pollResult;
	const nfds_t nfds = Number_Of_Events; //4U;
	struct pollfd managerFd[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(managerFd, 0, sizeof(managerFd));

	//Set shutdown handle
	managerFd[Shutdown_Event_Index].fd = 		m_stopEvent;
	managerFd[Shutdown_Event_Index].events =	POLLIN;

	// Add the queue handle to the poll structure
	managerFd[Message_Queue_Event_Index].fd =		m_MsgQueueHandle;
	managerFd[Message_Queue_Event_Index].events =	POLLIN;

	// Job Remove Handle
	managerFd[Job_Remove_Event_Index].fd =		m_JobRemoveEvent;
	managerFd[Job_Remove_Event_Index].events =	POLLIN;

	// Children stop signalled by Server in case of rename
	managerFd[Children_Stop_Event_Index].fd =		m_ChildStopEvent;
	managerFd[Children_Stop_Event_Index].events =	POLLIN;
	
	// Poll Timeout
	ACE_Time_Value timeout(RTRMGR_WAIT_TIMEOUT);

	uint8_t safeCounter = 0;

	while(!_stopManager)
	{
		// wait on event
		pollResult = ACE_OS::poll(managerFd, nfds, &timeout);

		//-------------------------------
		// Check for errors
		if(pollResult < 0)
		{
			// Error on poll
			if((EINTR == errno) || (EAGAIN == errno))
			{
				continue;
			}
			ACS_RTR_TRACE_MESSAGE("RTR Manager poll failed, stop event:<%d> error:<%d>", managerFd[0].fd, ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "RTR Manager poll failed, stop event:<%d> error:<%d>", managerFd[0].fd, ACE_OS::last_error());
			_stopManager = true;
			break;
		}

		//-------------------------------
		// Timeout elapsed
		if( 0U == pollResult )
		{
			// previous message queue open failed
			if( ACE_INVALID_HANDLE == m_MsgQueueHandle )
			{
				// check for new channel
				fxLookForNewChannel();
				// try again message queue open
				if( openMsgQueue() )
				{
					// refresh poll handle
					managerFd[Message_Queue_Event_Index].fd = m_MsgQueueHandle;
				}
				else
				{
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager message queue open failed, MS:<%s>", _msname.c_str());
				}
			}
			else
			{
				// extra check for new channel
				if( (++safeCounter % 5) == 0 )
				{
					ACS_RTR_TRACE_MESSAGE("Timeout on RTR Manager of MS:<%s>", _msname.c_str());
					safeCounter = 0U;
					fxLookForNewChannel();
				}
			}

			// Check childs status
			fxCheckChilds();

			continue;
		}

		//-------------------------------
		// Check Shutdown Event
		if(managerFd[Shutdown_Event_Index].revents & POLLIN )
		{
			ACS_RTR_TRACE_MESSAGE("Received a stop request!");
			_stopManager = true;
			break;
		}

		//-------------------------------
		// Check Children Shutdown Event
		if(managerFd[Children_Stop_Event_Index].revents & POLLIN )
		{
			ACS_RTR_LOG(LOG_LEVEL_DEBUG, "CHILDREN SHUTDOWN REQUESTED FOR MS:<%s>", _msname.c_str());
			ACS_RTR_TRACE_MESSAGE("CHILDREN SHUTDOWN REQUESTED FOR MS:<%s>", _msname.c_str());
			_stopManager = true;
			break;
		}

		//-------------------------------
		// Check Job Remove Event
		if(managerFd[Job_Remove_Event_Index].revents & POLLIN )
		{
			ACS_RTR_LOG(LOG_LEVEL_DEBUG, "JOB REMOVE REQUESTED FOR MS:<%s>", _msname.c_str());
			ACS_RTR_TRACE_MESSAGE("JOB REMOVE REQUESTED FOR MS:<%s>", _msname.c_str());
			_stopManager = true;
			break;
		}

		//-------------------------------
		// Check for Parameter change
		if(managerFd[Message_Queue_Event_Index].revents & POLLIN )
		{
			ACS_RTR_LOG(LOG_LEVEL_DEBUG, "PARAMETER CHANGE REQUESTED FOR MS:<%s>", _msname.c_str());
			ACS_RTR_TRACE_MESSAGE("PARAMETER CHANGE REQUESTED FOR MS:<%s>", _msname.c_str());

			char message[aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE + 1] = {0};
			ssize_t bytesRead = ::mq_receive(m_MsgQueueHandle, message, aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE, aca_rtr_communication::MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY);

			if (bytesRead < 0)
			{
				// Receive error
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager message queue receive failed, FD:<%d> error:<%d>", managerFd[1].fd, errno);
			}
			else
			{
				message[bytesRead]= 0;
				ACS_RTR_TRACE_MESSAGE("Message received:<%s>", message);

				// Check for new channel request
				if(0 == ACE_OS::strcmp(aca_rtr_communication::NEW_CP_CONNECTED_MESSAGE, message))
				{
					// New CP connected
					ACS_RTR_TRACE_MESSAGE("New CP connected message");
					ACS_RTR_LOG(LOG_LEVEL_INFO, "Received message: New CP connected to ACA");
					fxLookForNewChannel();
					safeCounter = 0U;
					continue;
				}

				// Check for record size change
				if(0 == ACE_OS::strncmp(aca_rtr_communication::RECORD_SIZE_CHANGE_MESSAGE, message, sizeof(aca_rtr_communication::RECORD_SIZE_CHANGE_MESSAGE)))
				{
					// Message Store record size changed
					// New CP connected
					ACS_RTR_TRACE_MESSAGE("Message Store record size changed");
					ACS_RTR_LOG(LOG_LEVEL_INFO, "Received message: New CP connected to ACA");

					std::string messageStr(message);
					// Get tag split position
					size_t tagSplitPos = messageStr.find(parseSymbol::equal);

					// Check if the tag is present
					if( std::string::npos != tagSplitPos )
					{
						// get new MS name
						std::stringstream  stringValue(messageStr.substr((tagSplitPos+1)));
						stringValue >> m_messageStoreRecordSize;

						maptype::const_iterator acaChannelIterator;

						// update all ACA channels with new value
						for(acaChannelIterator = _channelMap.begin(); acaChannelIterator != _channelMap.end(); ++acaChannelIterator)
						{
							if( (NULL != (*acaChannelIterator).second) )
							{
								(*acaChannelIterator).second->changeMessageStoreRecordSize(m_messageStoreRecordSize);
							}
						}
					}
					else
					{
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager failed to get new record size from message:<%s>, MS:<%s>", message, _msname.c_str());
					}

					continue;
				}
				// Received unknown message from ACA
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager received UNKNOWN message:<%s> from ACA", message);
			}
			continue;
		}

		// Check error on handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(managerFd[idx].revents & (POLLERR | POLLHUP | POLLNVAL) )
			{
				ACS_RTR_TRACE_MESSAGE("poll on handle pos:<%d> failed error:<%d> revents:<%d>",
										idx, errno, managerFd[idx].revents);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: [%s] RTR Manager poll on handle<%d> pos:<%d> failed error:<%d> revents:<%d>",
												_msname.c_str(), managerFd[idx].fd, idx, errno, managerFd[idx].revents);
				_stopManager = true;
				break;
			}
		}
	}

	ACS_RTR_TRACE_MESSAGE("RTR Manager of MS:<%s> thread loop terminated", _msname.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO,"INFO: RTR Manager of MS:<%s> thread loop terminated", _msname.c_str());
	// close the message queue handle
	if( ACE_INVALID_HANDLE != m_MsgQueueHandle)
	{
		::mq_close(m_MsgQueueHandle);
		m_MsgQueueHandle = ACE_INVALID_HANDLE;
	}

	//Calling onStop() here to destroy ACA and GOH threads once Manager thread down
	onStop();
	
	ACS_RTR_TRACE_MESSAGE("RTR Manager for MS:<%s> terminated", _msname.c_str());
	return 0;
}

/*============================================================================
	ROUTINE: getDataRecordKeys
 ============================================================================ */
bool ACS_RTR_Manager::getDataRecordKeys()
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;
	OmHandler objManager;
	// Init OmHandler
	ACS_CC_ReturnType getResult = objManager.Init();

	if(ACS_CC_SUCCESS == getResult)
	{
		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the iPadding attribute
		ACS_APGCC_ImmAttribute iPaddingAttribute;
		iPaddingAttribute.attrName =  rtr_imm::IPaddingAttribute;

		attributeList.push_back(&iPaddingAttribute);

		// to get the iPadding attribute
		ACS_APGCC_ImmAttribute oPaddingAttribute;
		oPaddingAttribute.attrName =  rtr_imm::OPaddingAttribute;

		attributeList.push_back(&oPaddingAttribute);

		ACS_RTR_TRACE_MESSAGE("get opadding and ipadding attribute");
		getResult = objManager.getAttribute(rtr_imm::AxeDataRecordDN, attributeList );

		// check for mandatory attributes
		if( (ACS_CC_FAILURE != getResult) &&
				(0 != iPaddingAttribute.attrValuesNum) &&
									(0 != oPaddingAttribute.attrValuesNum) )
		{
			result = true;
			memcpy(m_DataRecordKeys.ipad, reinterpret_cast<char*>(iPaddingAttribute.attrValues[0]), sizeof(m_DataRecordKeys.ipad) );
			memcpy(m_DataRecordKeys.opad, reinterpret_cast<char*>(oPaddingAttribute.attrValues[0]), sizeof(m_DataRecordKeys.opad) );
		}
		else
		{
			// Error on get password attribute
			ACS_RTR_TRACE_MESSAGE("Failed to get i/oPadding on <%s>, error:<%d>", rtr_imm::AxeDataRecordDN, objManager.getInternalLastError() );
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager failed to get i/oPadding on <%s>, error:<%d>", rtr_imm::AxeDataRecordDN, objManager.getInternalLastError() );
		}

		// Release all resources
		objManager.Finalize();
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager OmHandler init FAILED:<%d>", objManager.getInternalLastError());
		ACS_RTR_TRACE_MESSAGE("OmHandler init FAILURE");
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: getHMACMD5Password
 ============================================================================ */
bool ACS_RTR_Manager::getHMACMD5Password()
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;

	OmHandler objManager;
	// Init OmHandler
	ACS_CC_ReturnType getResult = objManager.Init();

	if(ACS_CC_SUCCESS == getResult)
	{
		// get password from ECIM password structure
		// password structure DN
		char passwordDN[256]={0};
		snprintf(passwordDN, sizeof(passwordDN), "%s,%s", rtr_imm::PasswordRDN, m_jobBasedDN.c_str() );

		// Password attribute
		ACS_CC_ImmParameter passwordAttribute;
		passwordAttribute.attrName = rtr_imm::PasswordAttribute;

		ACS_RTR_TRACE_MESSAGE("get password attribute");
		getResult = objManager.getAttribute( passwordDN, &passwordAttribute);

		if( (ACS_CC_FAILURE != getResult) &&
				(0 != passwordAttribute.attrValuesNum) )
		{
			std::string passwordValue = reinterpret_cast<char*>(passwordAttribute.attrValues[0]);

			result = true;
			char* passwordDecrypt = NULL;
			SecCryptoStatus decryptResult = sec_crypto_decrypt_ecimpassword(&passwordDecrypt, passwordValue.c_str());

			if( SEC_CRYPTO_OK != decryptResult )
			{
				memcpy(m_DataRecordKeys.skey, passwordValue.c_str(), sizeof(m_DataRecordKeys.skey) );
				
			}
			else
			{	// Convert 32 Hex to 16 Byte ASCII
				char keyBuffer[17]={0};
				char passBuffer[hashKeyLength+1]={0};
				memcpy(passBuffer,passwordDecrypt,hashKeyLength);
				packHexString(hashKeyLength,passBuffer,keyBuffer);
				memcpy(m_DataRecordKeys.skey,keyBuffer,sizeof(m_DataRecordKeys.skey));
				ACS_RTR_LOG(LOG_LEVEL_INFO, "Fetched hash Key  Attribute of <%s>, with length :<%lu>", passwordDN,sizeof(m_DataRecordKeys.skey));
			}

			if(NULL != passwordDecrypt )
				free(passwordDecrypt);
		}
		else
		{
			// Error on get password attribute
			ACS_RTR_TRACE_MESSAGE("Failed to get Password Attribute on <%s>, error:<%d>", passwordDN, objManager.getInternalLastError() );
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager Failed to get Password Attribute on <%s>, error:<%d>", passwordDN, objManager.getInternalLastError() );
		}

		// Release all resources
		objManager.Finalize();
    }
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager OmHandler init FAILED:<%d>", objManager.getInternalLastError());
		ACS_RTR_TRACE_MESSAGE("OmHandler init FAILURE");
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: openMsgQueue
 ============================================================================ */
bool ACS_RTR_Manager::openMsgQueue()
{
	ACS_RTR_TRACE_MESSAGE("In, JobDN:<%s>", m_jobBasedDN.c_str());
	bool result = false;

	if( ACE_INVALID_HANDLE != m_MsgQueueHandle )
	{
		ACS_RTR_TRACE_MESSAGE("Message Queue already open, FD:<%d>", m_MsgQueueHandle);
		::mq_close(m_MsgQueueHandle);
		m_MsgQueueHandle = ACE_INVALID_HANDLE;
	}

	std::string messageStoreName;
	if( getMessageStoreDefaultName(messageStoreName) )
	{
		std::string msgQueueName(aca_rtr_communication::MESSAGE_QUEUE_PREFIX);
		msgQueueName.append(messageStoreName);

		struct mq_attr msgQueueAttributes;
		msgQueueAttributes.mq_flags = aca_rtr_communication::MESSAGE_QUEUE_DEFAULT_FLAGS;
		msgQueueAttributes.mq_maxmsg = aca_rtr_communication::MESSAGE_QUEUE_MAX_NUM_OF_MESS;
		msgQueueAttributes.mq_msgsize = aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE;
		msgQueueAttributes.mq_curmsgs = aca_rtr_communication::MESSAGE_QUEUE_CURRENT_NUM_OF_MESS;

		ACS_RTR_TRACE_MESSAGE("Message Queue Name:<%s>", msgQueueName.c_str());

		m_MsgQueueHandle = ::mq_open(msgQueueName.c_str(), O_CREAT | O_RDONLY, aca_rtr_communication::MESSAGE_QUEUE_DEFAULT_PERM, &msgQueueAttributes);

		result = (ACE_INVALID_HANDLE != m_MsgQueueHandle);

		if(!result)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager message queue:<%s> open failed,  error:<%d>", msgQueueName.c_str(), errno);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*============================================================================
	ROUTINE: getMessageStoreDefaultName
 ============================================================================ */
bool ACS_RTR_Manager::getMessageStoreDefaultName(std::string& messageStoreName)
{
	ACS_RTR_TRACE_MESSAGE("In, JobDN:<%s>", m_jobBasedDN.c_str());

	bool result = false;
	messageStoreName.clear();
	// Get start position of the message store DN
	size_t tagStartPos = m_jobBasedDN.find(rtr_imm::MessageStoreId);

	// Check if the tag is present
	if( std::string::npos != tagStartPos )
	{
		// get the DN
		std::string messageStoreDN(m_jobBasedDN.substr(tagStartPos));

		ACS_RTR_TRACE_MESSAGE("Message Store DN:<%s>", messageStoreDN.c_str());
		// Split the field in RDN and Value
		size_t equalPos = messageStoreDN.find_first_of(parseSymbol::equal);
		size_t commaPos = messageStoreDN.find_first_of(parseSymbol::comma);

		// Check if some error happens
		if( (std::string::npos != equalPos) )
		{
			result = true;
			// check for a single field case
			if( std::string::npos == commaPos )
				messageStoreName.assign(messageStoreDN.substr(equalPos + 1));
			else
				messageStoreName.assign(messageStoreDN.substr(equalPos + 1, (commaPos - equalPos - 1) ));

			ACS_RTR_TRACE_MESSAGE("Message Store Default Name:<%s>", messageStoreName.c_str());
		}
		else
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager failed parse of DN:<%s>, search tag:<%c>", messageStoreDN.c_str(), parseSymbol::equal);
		}
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager failed parse of DN:<%s>, search tag:<%s>", m_jobBasedDN.c_str(), rtr_imm::MessageStoreId);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}
/*============================================================================
	ROUTINE: getMessageStoreRecordSize
 ============================================================================ */
bool ACS_RTR_Manager::getMessageStoreRecordSize()
{
	ACS_RTR_TRACE_MESSAGE("In, JobDN:<%s>", m_jobBasedDN.c_str());
	bool result = false;

	// Get start position of the message store DN
	size_t tagStartPos = m_jobBasedDN.find(rtr_imm::MessageStoreId);

	// Check if the tag is present
	if( std::string::npos != tagStartPos )
	{
		// get the DN
		std::string messageStoreDN(m_jobBasedDN.substr(tagStartPos));

		ACS_RTR_TRACE_MESSAGE("Message Store DN:<%s>", messageStoreDN.c_str());

		OmHandler objManager;
		// Init OmHandler
		ACS_CC_ReturnType getResult = objManager.Init();

		if(ACS_CC_SUCCESS == getResult)
		{
			// to get record Size
			ACS_CC_ImmParameter recordSizeAttribute;
			recordSizeAttribute.attrName = rtr_imm::RecordSizeAttibute;

			getResult = objManager.getAttribute(messageStoreDN.c_str(), &recordSizeAttribute );

			// check for mandatory attributes
			if( (ACS_CC_FAILURE != getResult) && (0 != recordSizeAttribute.attrValuesNum) )
			{
				result = true;
				m_messageStoreRecordSize = *reinterpret_cast<uint32_t*>(recordSizeAttribute.attrValues[0]);
				ACS_RTR_TRACE_MESSAGE(" MS:<%s>, recordSize:<%d>", messageStoreDN.c_str(), m_messageStoreRecordSize);
				ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO: RTR Manager get recordSize:<%d> for MS:<%s>", m_messageStoreRecordSize, messageStoreDN.c_str());
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("get attribute:<%s> of MS:<%s> failed", rtr_imm::MessageStoreNameAttribute, messageStoreDN.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager get attribute:<%s> of MS:<%s> failed, error:<%d>", rtr_imm::MessageStoreNameAttribute, messageStoreDN.c_str(), objManager.getInternalLastError());
			}

			// Release all resources
			objManager.Finalize();
		}
		else
		{
			// Init error
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager OmHandler init FAILED:<%d>", objManager.getInternalLastError());
			ACS_RTR_TRACE_MESSAGE("OmHandler init FAILURE");
		}
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager failed parse of DN:<%s>, search tag:<%s>", m_jobBasedDN.c_str(), rtr_imm::MessageStoreId);
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

//------------
//fxStop
//------------
void ACS_RTR_Manager::fxStop()
{
	ACS_RTR_TRACE_FUNCTION;
	_stopManager=true;
}

//----------------
//fxStopOnRename
//----------------
void ACS_RTR_Manager::fxStopOnRename()
{
	ACS_RTR_TRACE_FUNCTION;
	_stopManager=true;

	//Signal shutdown to children threads
	eventfd_t stopEvent = 1U;
	eventfd_write(m_ChildStopEvent, stopEvent);
}


//----------
// onStop
//----------
void ACS_RTR_Manager::onStop()
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());

	if (_rtrrmIssued && (NULL != _msStat))
	{
		// delete statistic file
		_msStat->delStatFile();
	}

	//Stop GOH REPORTER
	if(NULL != _gohReporter)
	{
		_gohReporter->fxStop();
	}

	//Stop ACA CHANNELS
	for(maptype::const_iterator it = _channelMap.begin(); it != _channelMap.end(); ++it)
	{
		ACS_RTR_ACAChannel* channel = (*it).second;
		if(NULL != channel)
		{
			channel->fxStop();
		}
	}

	//Signal shutdown to children threads
	eventfd_t stopEvent = 1U;
	eventfd_write(m_ChildStopEvent, stopEvent);

	fxDestroyGOHReporter(true);
	fxDestroyACAChannels(true);

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msname.c_str());
}

/*===================================================================
   ROUTINE: getCpStatistics
=================================================================== */
bool ACS_RTR_Manager::getCpStatistics(CpStatisticsInfo* cpStatisticsInfo, short cpId)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;
	maptype::const_iterator channelElement = _channelMap.find(cpId);

	if( (_channelMap.end() !=  channelElement) &&  (NULL != channelElement->second) )
	{
		ACS_RTR_TRACE_MESSAGE("found valid ACA channel");
		RTR_statistics* cpStatistics = channelElement->second->getStatistics();
		if(NULL != cpStatistics)
		{
			ACS_RTR_TRACE_MESSAGE("get CP statistics info");
			result = true;
			cpStatistics->getCpStatisticsInfo(cpStatisticsInfo);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

/*===================================================================
   ROUTINE: getMessageStoreStatistics
=================================================================== */
bool ACS_RTR_Manager::getMessageStoreStatistics(StatisticsInfo* msStatisticsInfo)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;

	if(NULL != _msStat)
	{
		ACS_RTR_TRACE_MESSAGE("get statistics info");
		_msStat->getMessageStoreStatistics(msStatisticsInfo);
		result = true;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}
//----------------
// fxTQExists
//----------------
bool ACS_RTR_Manager::fxTQExists()
{
	ACS_RTR_TRACE_FUNCTION;
	bool retValue = true;
	ACE_INT32 gohErr = 0;
	eventType rtrEvt = GOH_TQ_NOT_DEFINED;
	if (_rtrParams->OutputType == FileOutput)
	{
		AES_OHI_ExtFileHandler2* mySenderFile = new (std::nothrow) AES_OHI_ExtFileHandler2( OHI_USERSPACE::SUBSYS , OHI_USERSPACE::APPNAME);
		if (mySenderFile == NULL)
		{
			ACE_TCHAR trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "Cannot create AES_OHI_ExtFileHandler2 obj for TQ %s", _tq.c_str());

			ACS_RTR_TRACE_MESSAGE("Error: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

			return false;
		}
		gohErr = mySenderFile->attach();
		if (gohErr != AES_OHI_NOERRORCODE)
		{
			rtrEvt = GOH_CONNECT_ERROR;
			ACE_Time_Value safeSleep(0,500000);
			for (int retry = 0; (retry < 10) && ( mySenderFile->detach() != AES_OHI_NOERRORCODE ); retry++) ACE_OS::sleep(safeSleep);
			delete mySenderFile;
			ACS_RTR_TRACE_MESSAGE("Error: attach failed to TQ \"%s\" with error %d", _tq.c_str(), gohErr);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "attach failed to TQ \"%s\" with error %d", _tq.c_str(), gohErr);
			retValue = false;
		}

		if (retValue)
		{
			//check that transfer queue is defined
			gohErr = mySenderFile->fileTransferQueueDefined(_tq.c_str());
			if (gohErr != AES_OHI_NOERRORCODE)  retValue = false;
			ACE_Time_Value safeSleepAgain(0,500000);
			for (int retry = 0; (retry < 10) && ( mySenderFile->detach() != AES_OHI_NOERRORCODE ); retry++) ACE_OS::sleep(safeSleepAgain);
			delete mySenderFile;
		}
	}
	else
	{
		gohErr = AES_OHI_BlockHandler2::blockTransferQueueDefined(_tq.c_str());
		if (gohErr != AES_OHI_NOERRORCODE)
		{
			ACE_TCHAR trace[512] = {0};
			if (gohErr == AES_OHI_TQNOTFOUND) ACE_OS::snprintf(trace, sizeof(trace) - 1, "verify block transfer queue %s returned the error %u, maybe the transfer queue as been removed.", _tq.c_str(), gohErr);
			else ACE_OS::snprintf(trace, sizeof(trace) - 1, "verify block transfer queue %s returned an error: %u", _tq.c_str(), gohErr);
			ACS_RTR_TRACE_MESSAGE("Error: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

			retValue = false;
		}
	}

	if (!retValue)
	{
		ACE_TCHAR trace[512] = {0};
		// ACS_RTR_Manager started after reboot/rtr-restart
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s TQ: %s. Type: %s. Error code from GOH: %u", _RTRMGR_DERR_TQ_RESTORED_EXISTS, _tq.c_str(), ((_rtrParams->OutputType == FileOutput) ? "File Output" : "Block Output"), gohErr);
		ACS_RTR_TRACE_MESSAGE("Error: %s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
	}
	return retValue;
}


//-------------------------------
// fxCreateSharedQueue
//-------------------------------
bool ACS_RTR_Manager::fxCreateSharedQueue()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());

	bool retValue = true;

	if(NULL == _outputQueue)
	{
		_outputQueue = new (std::nothrow) ACS_RTR_OutputQueue();
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("INFO: Output queue already created for MS:<%s>", _msname.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "INFO: Output queue already created for MS:<%s>", _msname.c_str());
	}

	if(NULL == _outputQueue)
	{
		retValue = false;
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR:  %s MS:%s. memory allocation error", _RTRMGR_DERR_OUT_QUEUE, _msname.c_str() );
		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
	}

	ACS_RTR_TRACE_MESSAGE("OUT, result:<%s>", (retValue ? "TRUE" :"FALSE") );
	return retValue;
}

//-----------------------------
// fxCreateGohReporter
//-----------------------------
bool ACS_RTR_Manager::fxCreateGohReporter()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_gohReporterCriticalSection);

	bool retValue = true;
	eventType problemNo = BREP_NOT_STARTED;

	if(FileOutput == _rtrParams->OutputType )
	{
		problemNo = FREP_NOT_STARTED;
	}

	try
	{
		ACS_RTR_TRACE_MESSAGE("create GOH reporter, TQ:<%s>", _tq.c_str());

		if(_rtrParams->OutputType == FileOutput)
			_gohReporter = new (std::nothrow) ACS_RTR_GOHFileReporter(_tq.c_str(), _outputQueue, _eventHandler, _msStat, _msname, RTR::SITENAME);
		else
			_gohReporter = new (std::nothrow) ACS_RTR_GOHBlockReporter(_tq.c_str(), _outputQueue, _eventHandler, _msStat, this);
	}
	catch(std::exception& ex)
	{
		retValue = false;
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s: %s error text = %s", __func__, _RTRMGR_DERR_NEW_GOH_THREAD, ex.what());
		if (!setLastError(RTRMGR_ERR_NEW_GOH_THREAD, trace, true))
			setLastError(RTRMGR_ERR_NEW_GOH_THREAD, _RTRMGR_DERR_NEW_GOH_THREAD);

		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s",trace);
		

		if(_gohReporter)
			_gohReporter = 0;
	}

	if(_gohReporter)
	{
		ACS_RTR_TRACE_MESSAGE("Starting thread for _gohReporter");

		if( _gohReporter->open(m_ChildStopEvent, m_JobRemoveEvent) == 0)
		{
			//----------------------------------
			// Wait for the thread activation
			for(int i = 0; i < START_RETIRES && !_stopManager && !_gohReporter->isRunning(); ++i)
			{
				ACS_RTR_TRACE_MESSAGE("Wait for _gohReporter goes RUNNING for MS: <%s>", _msname.c_str());
				ACE_OS::sleep(1);
			}

			if (!_gohReporter->isRunning())
			{
				ACS_RTR_TRACE_MESSAGE("_gohReporter is not running yet");
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("_gohReporter is RUNNING");
			}

			//----------------------------------
			// Wait for thread becoming ACTIVE
			int i = 0;
			for(; i < ATTACH_RETRIES && !_stopManager && _gohReporter->isRunning(); ++i)
			{
				ACS_RTR_TRACE_MESSAGE(" wait for _gohReporter state ACTIVE for MS:<%s>", _msname.c_str());
				//ACS_RTR_Manager waits for 15 minutes that the reporting part is working fine.
				if ((_gohReporter->getState() == ACS_RTR_GOHReporter::ACTIVE) )
				{
					ACS_RTR_TRACE_MESSAGE("_gohReporter state ACTIVE");
					break;
				}
				else
				{
					ACE_OS::sleep(2);
				}
			}

			//--------------------------------------------------------------------------------------------
			if( !_gohReporter->isRunning() || (_gohReporter->getState() != ACS_RTR_GOHReporter::ACTIVE) )
			{
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1,
						"[%s@%s] %s. GOH Reporter Thread is not active after %d seconds. Error text = %s",
						_msname.c_str(), _tq.c_str(), _RTRMGR_DERR_START_GOH_THREAD, i, _gohReporter->lastErrorDescription());

				if (!setLastError(RTRMGR_ERR_START_GOH_THREAD, trace, true))
					setLastError(RTRMGR_ERR_START_GOH_THREAD, _RTRMGR_DERR_START_GOH_THREAD);

				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
				retValue = false;

				_gohReporter->fxStop();

				_gohReporter->wait();
				delete _gohReporter;
				_gohReporter = NULL;


				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s@%s] GOH THREAD STOPPED BECAUSE IT IS IMPOSSIBLE TO INITIALIZE IT", _msname.c_str(), _tq.c_str());
				ACS_RTR_TRACE_MESSAGE("ERROR: [%s@%s] GOH THREAD STOPPED BECAUSE IT IS IMPOSSIBLE TO INITIALIZE IT", _msname.c_str(), _tq.c_str());

			}
		}
		else
		{
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "[%s@%s] failed to start GOH reported thread", _msname.c_str(), _tq.c_str());
			if (!setLastError(RTRMGR_ERR_START_GOH_THREAD, trace, true)) setLastError(RTRMGR_ERR_START_GOH_THREAD, _RTRMGR_DERR_START_GOH_THREAD);

			retValue= false;
			delete _gohReporter;
			_gohReporter = NULL;

			ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		}
	}
	else
		retValue= false;

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%s>", _msname.c_str(), (retValue ? "TRUE":"FALSE") );
	return retValue;
}

//-----------------------------
// fxCreateAcaChannels
//-----------------------------
bool ACS_RTR_Manager::fxCreateAcaChannels(ACE_INT32 maxRetries)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;

	if( ACS_RTR_SystemConfig::instance()->isMultipleCPSystem() )
	{
		// Multi-CP System
		std::vector<unsigned> cpIDs;

		// Get CPid of connected CP
		for(int retryCounter = 0; !_stopManager && (retryCounter < maxRetries); ++retryCounter )
		{
			// Get the list of connected CPs
			result = _acaInterface->getCPsForName(_msname, cpIDs);

			if(result)
			{
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: ON MCP System found <%zu> CP connected for MS:<%s>", cpIDs.size(), _msname.c_str());
				ACS_RTR_TRACE_MESSAGE("%s", trace);
				ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
				break;
			}
			else
			{
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: call of getCPsForName() on ACA interface failed for MS:<%s>, Error Code:<%d>, Error description:<%s>", _msname.c_str(), _acaInterface->getLastError(), _acaInterface->getLastErrorDescription().c_str());

				ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

				// wait before retry 150 ms
				waitBeforeRetry(0, 150);
			}
		}

		if(result)
		{

			// check the list
			if(cpIDs.empty())
			{
				ACS_RTR_TRACE_MESSAGE("WARNING: There is no CP connected to the message store:<%s>", _msname.c_str());
				ACS_RTR_LOG(LOG_LEVEL_WARN, "There is no CP connected to the message store:<%s>", _msname.c_str());
			}
			else
			{
				// Add for each CP a new ACA channel
				std::vector<unsigned>::const_iterator cpIdIterator;

				for(cpIdIterator = cpIDs.begin(); cpIDs.end() != cpIdIterator; ++cpIdIterator)
				{
					short id_ = static_cast<short>(*cpIdIterator);
					bool singleAddResult = false;
					int retryCounter = 0;

					do
					{
						if(fxCreateChannel(id_))
						{
							singleAddResult = true;
							char trace[512] = {0};
							ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: ON MCP System ACA channel for CPid:<%d> created for MS:<%s>", id_, _msname.c_str());
							ACS_RTR_TRACE_MESSAGE("%s", trace);
							ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
							// stop the inner loop
							break;
						}
						else
						{
							char trace[512] = {0};
							ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: ON MCP System failed to create ACA channel for CPid:<%d> for MS:<%s>, ATTEMPTS:<%d>", id_, _msname.c_str(), retryCounter);

							ACS_RTR_TRACE_MESSAGE("%s", trace);
							ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

							++retryCounter;
							// wait before retry 100 ms
							waitBeforeRetry(0, 100);
						}

					}while( (retryCounter < maxRetries) && !_stopManager);

					// update the result with the last add result
					result &= singleAddResult;
				}
			}
		}
	}
	else
	{
		// Single-CP System
		int retryCounter = 0;
		do
		{
			if(fxCreateChannel(DEFAULT_CPID))
			{
				result = true;
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: ON SCP System ACA channel created for MS:<%s>", _msname.c_str());
				ACS_RTR_TRACE_MESSAGE("%s", trace);
				ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
				break;
			}
			else
			{
				char trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: ON SCP System failed to create ACA channel for MS:<%s>, ATTEMPTS:<%d>", _msname.c_str(), retryCounter);

				ACS_RTR_TRACE_MESSAGE("%s", trace);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

				++retryCounter;
				// wait before retry 100 ms
				waitBeforeRetry(0, 100);
			}

		}while( (retryCounter < maxRetries) && !_stopManager);
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%s>", _msname.c_str(), (result ? "TRUE":"FALSE") );
	return result;
}

//-----------------------
// fxCreateChannel
//-----------------------
bool ACS_RTR_Manager::fxCreateChannel(short id_)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("IN, MS:<%s>, CPid:<%d>", _msname.c_str(), id_);

	if(_channelMap.find(id_) != _channelMap.end())
	{
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: ACA Channel of CP:<%d> for MS:<%s> already added", id_, _msname.c_str());
		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
		return true;
	}

	//Create statistic object
	RTR_statistics* acaStat = 0;

	if(DEFAULT_CPID != id_ )
	{
		// Multi-CP configuration
		//build statistics at the message store/cp level
		// Create and initialize statistics
		acaStat = new (std::nothrow) RTR_statistics(id_, _msname);

		if( NULL == acaStat)
		{
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: %s Message Store: %s CPid: %d", _RTRMGR_DERR_STATISTICS, _msname.c_str(), id_);
			ACS_RTR_TRACE_MESSAGE("%s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
			return false;
		}

		// init Cp statistic object
		acaStat->cpStatisticsInfoInit(m_jobBasedDN);
	}
	else
	{
		// Single CP configuration
		acaStat = _msStat;
	}

	// create ACA channel
	ACS_RTR_ACAChannel* acaChannel = 0;
	try
	{
		if(DEFAULT_CPID != id_)
		{
			ACS_RTR_TRACE_MESSAGE("Create ACA channel in MCP, cpID:<%d>", id_);
			acaChannel = new (std::nothrow) ACS_RTR_ACAChannel(this, _msname, id_, _rtrParams, &m_DataRecordKeys, _outputQueue , acaStat, m_messageStoreRecordSize, _msStat);
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("Create ACA channel in SCP");
			acaChannel = new (std::nothrow) ACS_RTR_ACAChannel(this, _msname, id_, _rtrParams, &m_DataRecordKeys, _outputQueue , acaStat, m_messageStoreRecordSize);
		}

		if(NULL == acaChannel)
		{
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: failed to create ACA channel object for MS:<%s>", _msname.c_str());
			ACS_RTR_TRACE_MESSAGE("%s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
			return false;
		}
	}
	catch(const std::exception& ex)
	{
		// only on MCP it is needed
		if(id_ != DEFAULT_CPID)
			delete acaStat;

		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: creation of ACA channel object failed for Message Store:<%s> CPid:<%d>. Error description:<%s>", _msname.c_str(), id_, ex.what());

		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		return false;
	}

	// Insert the new ACA channel and check the insert result
	if(!_channelMap.insert( std::make_pair(id_, acaChannel) ).second)
	{
		// delele the ACA channel object
		delete acaChannel;
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: failed to insert new ACA channel into Manager map, Message Store:<%s> CPid:<%d>", _msname.c_str(), id_);

		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("start the ACA channel thread");

	if(acaChannel->open(m_ChildStopEvent, m_JobRemoveEvent) < 0 )
	{
		_channelMap.erase(id_);
		delete acaChannel;
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: failed to start ACA channel worker thread, Message Store:<%s> CPid:<%d>", _msname.c_str(), id_);
		ACS_RTR_TRACE_MESSAGE("%s", trace);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

		return false;
	}

	return true;
}

//------------------------------
// fxLookForNewChannel
//------------------------------
bool ACS_RTR_Manager::fxLookForNewChannel()
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;
	if (ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
	{
		// Multi-CP System
		std::vector<unsigned> cpIDs;

		// Get the list of connected CPs
		result = _acaInterface->getCPsForName(_msname, cpIDs);

		if(result)
		{
			// Add for each CP a new ACA channel
			std::vector<unsigned>::const_iterator cpIdIterator;

			for(cpIdIterator = cpIDs.begin(); cpIDs.end() != cpIdIterator; ++cpIdIterator)
			{
				short id_ = static_cast<short>(*cpIdIterator);

				if( (_channelMap.end() == _channelMap.find(id_)) && fxCreateChannel(id_) )
				{
					char trace[512] = {0};
					ACE_OS::snprintf(trace, sizeof(trace) - 1, "INFO: ON MCP System ACA channel for CPid:<%d> created for MS:<%s>", id_, _msname.c_str());
					ACS_RTR_TRACE_MESSAGE("%s", trace);
					ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
				}
			}
		}
		else
		{
			char trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "ERROR: call of getCPsForName() on ACA interface failed for MS:<%s>, Error Code:<%d>, Error description:<%s>", _msname.c_str(), _acaInterface->getLastError(), _acaInterface->getLastErrorDescription().c_str());

			ACS_RTR_TRACE_MESSAGE("ERROR: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
		}
	}
	else
	{
		// SCP system, check if the ACA channel has been created
		if(_channelMap.find(DEFAULT_CPID) == _channelMap.end() )
		{
			result = fxCreateAcaChannels();
		}
	}
	return result;
}

//--------------------
// fxCheckChilds
//--------------------
bool ACS_RTR_Manager::fxCheckChilds()
{
	ACS_RTR_TRACE_FUNCTION;
	fxCheckGohReporter();
	fxCheckACAChannels();
	return true;
}

//----------------------------
// fxCheckGohReporter
//----------------------------
void ACS_RTR_Manager::fxCheckGohReporter(ACE_INT32 maxRetries)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_gohReporterCriticalSection);

	if ( _gohReporter && !_gohReporter->isRunning())
	{
		ACS_RTR_TRACE_MESSAGE("Error: GOH Reporter thread is not running for Message Store %s!!", _msname.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "GOH Reporter thread is not running for Message Store %s!!", _msname.c_str());

		// reclaim system resources
		_gohReporter->wait();
		delete _gohReporter;
		_gohReporter = 0;
	}

	for (ACE_INT32 retry = 0; (!_gohReporter) && !_stopManager && (retry < maxRetries); retry++)
	{
		fxCreateGohReporter();
		if (!_gohReporter)
		{
			ACE_Time_Value safeSleep(0,100000);
			ACE_OS::sleep(safeSleep);
		}
	}
}

//-------------------------------------------------------------
// fxCheckACAChannels
// find and restart the aca channel thread stopped
//-------------------------------------------------------------
bool ACS_RTR_Manager::fxCheckACAChannels()
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());

	bool channelFound = false;
	short CpId = DEFAULT_CPID;
	maptype::iterator mapElement;

	for(mapElement = _channelMap.begin(); (mapElement != _channelMap.end()) && (!channelFound); ++mapElement)
	{
		//Null check the channel TR HR70638
		// Check if the work thread is running
		if( (NULL != mapElement->second) && ( mapElement->second->thr_count() == 0) )
		{
			// get the Cp Id
			CpId = mapElement->first;
			channelFound = true;
			// Work thread is terminate
			// join with it in order to release system resources
			mapElement->second->wait();
			delete (mapElement->second);
			mapElement->second = NULL;
			ACS_RTR_TRACE_MESSAGE("found ACA Channel stopped, CPid:<%d>", CpId);
			break;
		}
	}

	if(channelFound)
	{
		_channelMap.erase(CpId);
		ACS_RTR_TRACE_MESSAGE("restart ACA Channel, CPid:<%d>", CpId);
		bool restarted = fxCreateChannel(CpId);

		ACS_RTR_TRACE_MESSAGE("ACA channel restarted:<%s>", ( restarted ? "TRUE" : "FALSE") );
	}

	return channelFound;
}

//----------------------------
// fxDestroyGOHReporter
//----------------------------
bool ACS_RTR_Manager::fxDestroyGOHReporter(bool onstop)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());
	bool result = true;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_gohReporterCriticalSection);
	(void)onstop;
	if(NULL != _gohReporter)
	{
		//Signal to shutdown
		eventfd_t stopEvent = 1U;
		eventfd_write(m_ChildStopEvent, stopEvent);
		// Just to be sure that the thread loop ends
		_gohReporter->fxStop();
		// wait svc termination
		_gohReporter->wait();

		delete _gohReporter;
		_gohReporter = 0;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

//---------------------------
// fxDestroyACAChannels
//---------------------------
bool ACS_RTR_Manager::fxDestroyACAChannels(bool /*onstop*/)
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("IN, MS:<%s>", _msname.c_str());
	bool result = true;
	if(!_channelMap.empty())
	{
		// Check for stop case or remove
		if(!_rtrrmIssued)
		{
			//Signal to all Channel to shutdown
			eventfd_t stopEvent = 1U;
			eventfd_write(m_ChildStopEvent, stopEvent);
		}

		maptype::const_iterator it;
		ACS_RTR_ACAChannel* channel;

		for(it = _channelMap.begin(); it != _channelMap.end(); ++it)
		{
			channel = (*it).second;
			if(NULL != channel)
			{
				// Just to be sure that the thread loop ends
				channel->fxStop();
				// wait svc termination
				channel->wait();
				// memory clean
				delete channel;
			}
		}
		// remove all elements from the map
		_channelMap.clear();
	}

	ACS_RTR_TRACE_MESSAGE("OUT, MS:<%s>", _msname.c_str());
	return result;
}


//----------------------------------------------------------------------------------------
// rtrrmIssue
// check if remove operation is allowed at this moment and issue the rtrrm event
//----------------------------------------------------------------------------------------
bool ACS_RTR_Manager::rtrrmIssue()
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());

	bool canStop = true;
	//can stop event be issued?
	maptype::const_iterator it;
	ACS_RTR_ACAChannel* channel;
	for(it = _channelMap.begin(); it != _channelMap.end(); ++it)
	{
		channel = (*it).second;
		if( (NULL != channel) && !channel->isMsSleeping() )
		{
			ACS_RTR_TRACE_MESSAGE("ACA channel not stoppable");
			canStop = false;
			break;
		}
	}

	if(canStop && (NULL != _outputQueue))
		canStop = _outputQueue->queueEmpty();

	//issue stop event
	if(canStop)
	{
		ACS_RTR_TRACE_MESSAGE("set Remove Event");

		_rtrrmIssued = true;
		_stopManager = true;

		// Set the Remove Event to all ACA channels
		eventfd_t setEvent = 1U;
		if(eventfd_write(m_JobRemoveEvent, setEvent) != 0)
		{
			ACS_RTR_TRACE_MESSAGE("set Remove Event failed, error<%d>", errno);
		}

	}
	ACS_RTR_TRACE_MESSAGE("Out, can stop:<%s>", (canStop ? "TRUE" : "FALSE") );
	return canStop;
}

//-----------------------------------------------------------------------------------------------
// rtrfeIssue
// check if end of file operation is allowed in this output type and issue the rtrfe event
//-----------------------------------------------------------------------------------------------
bool ACS_RTR_Manager::rtrfeIssue()
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());
	bool result = false;
	if( FileOutput == _rtrParams->OutputType )
	{
		maptype::const_iterator it;
		for(it = _channelMap.begin(); it != _channelMap.end(); ++it)
		{
			if( NULL != (*it).second)
				(*it).second->setRtrfeEvent();
		}
		result = true;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}

//---------------------------------------------------------------------
// rtrchIssue
// rtrchIssue issues the changed parameter to the ACA channels
//---------------------------------------------------------------------
bool ACS_RTR_Manager::rtrchIssue(RTRMS_Parameters* changedParams)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;

	if(changedParams)
	{
		// check job type
		if(FileOutput == changedParams->OutputType)
		{
			if( changedParams->HmacMd5 )
			{
				ACS_RTR_TRACE_MESSAGE("get HMACMD5 keys");
				getHMACMD5Password();
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("reset HMACMD5 keys");
				memset(m_DataRecordKeys.skey, '\0', sizeof(m_DataRecordKeys.skey));
			}
		}

		//copy new rtr parameters
		memcpy(_rtrParams, changedParams, sizeof(RTRMS_Parameters));
		maptype::const_iterator acaChannel;

		// Notify all ACA channel
		for(acaChannel = _channelMap.begin(); acaChannel != _channelMap.end(); ++acaChannel)
		{
			if(NULL != (*acaChannel).second)
			{
				(*acaChannel).second->changeRTRParameters(_rtrParams, &m_DataRecordKeys);
			}
		}

		result = true;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}

//---------------------------------------------------------------
// restartNeed
//  manage error situations
//---------------------------------------------------------------
bool ACS_RTR_Manager::restartNeed()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msname.c_str());

	bool toRestart = !_rtrrmIssued;

	if(toRestart)
	{
		char trace[512] = {0};
		ACE_OS::snprintf(trace, sizeof(trace) - 1, "WARNING: RTR Manager of MS:<%s> need to be restarted", _msname.c_str());
		ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", trace);
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, need to restart:<%s>", _msname.c_str(), (toRestart ? "TRUE" : "FALSE") );
	return toRestart;
}

//-----------------------------------------------------------------------
// getChannelList
// getChannelList is used for retrieving the statistics at CP level
//-----------------------------------------------------------------------
std::list<ACS_RTR_ACAChannel*> ACS_RTR_Manager::getChannelList()
{
	ACS_RTR_TRACE_FUNCTION;
	std::list<ACS_RTR_ACAChannel*> channelList;
	for (maptype::iterator it = _channelMap.begin(); it != _channelMap.end(); it++) {
		ACS_RTR_ACAChannel * channel = (*it).second;
		if (channel) channelList.push_back(channel);
	}
	return channelList;
}

//---------------------------------------------------------------------------------------------------------------
// getCPQueue
// in the block transfer goh reporter thread gets the block to transmit by the aca channel local queue
//---------------------------------------------------------------------------------------------------------------
ACS_RTR_OutputQueue* ACS_RTR_Manager::getCPQueue(short cpId) {
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_OutputQueue * queue = 0;
	maptype::iterator it = _channelMap.find(cpId);
	if (it != _channelMap.end()) {
		ACS_RTR_ACAChannel* channel = (*it).second;
		if (channel) queue = channel->getLocalBlockQueue();
	}
	return queue;
}

//--------------------------------------
// pfxCommitAcaMessages
// used by GohBlockReporter thread
//--------------------------------------
bool ACS_RTR_Manager::pfxCommitAcaMessages(ULONGLONG num, string cpname)
{
	ACS_RTR_TRACE_FUNCTION;
	short id = DEFAULT_CPID;
	bool success = true;
	if (ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
	{
		id = ACS_RTR_SystemConfig::instance()->cs_getCPID(cpname.c_str());
		if (!ACS_RTR_SystemConfig::instance()->isValidCPId(id))
		{
			ACE_TCHAR trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "%s CP name not found: %s", _RTRMGR_DERR_GET_CPID, cpname.c_str());
			ACS_RTR_TRACE_MESSAGE("Warning: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", trace);
			return false;
		}
	}
	// commit
	maptype::iterator it = _channelMap.find(id);
	if (it != _channelMap.end())
	{
		ACS_RTR_ACAChannel* channel = (*it).second;
		if (num > 0) success = channel->pfxCommitAcaMessages(num);
		else success = channel->pfxCommitAcaMessages();
	}

	return success;
}

//------------------------------------
// checkForNotCommittedAcaMessages
//
//------------------------------------
bool ACS_RTR_Manager::checkForNotCommittedAcaMessages(std::string &_cpName, unsigned int & nAcaMessages, short cpId)
{
	ACS_RTR_TRACE_FUNCTION;
	bool returnValue = false;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_gohReporterCriticalSection);
	ACS_RTR_GOHBlockReporter * gohBlockReporter = dynamic_cast<ACS_RTR_GOHBlockReporter *>(_gohReporter);
	if(gohBlockReporter)
	{
		returnValue = gohBlockReporter->getNotCommittedAcaMessages(_cpName, nAcaMessages, cpId);
	}
	else
	{
		//Trace internal error
		ACS_RTR_TRACE_MESSAGE("Error: GOH Reporter thread is not created during the restart check procedure.");
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Error: GOH Reporter thread is not created during the restart check procedure.");
	}

	return returnValue;
}


void ACS_RTR_Manager::waitBeforeRetry(int waitTimeOutSec, int waitTimeOutMs)
{
	ACS_RTR_TRACE_MESSAGE("IN, waitTimeOut:<%d> sec and <%d> ms", waitTimeOutSec, waitTimeOutMs);

	const nfds_t nfds = 2U;
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_stopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = m_JobRemoveEvent;
	fds[1].events = POLLIN;

	ACE_Time_Value timeout(waitTimeOutSec, waitTimeOutMs*1000);

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
			// Received signal of thread termination
			ACS_RTR_TRACE_MESSAGE("termination signal received");
			_stopManager = true;
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			// Received signal of thread termination
			ACS_RTR_TRACE_MESSAGE("remove job signal received");
			break;
		}

		// Check error on handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL) )
			{
				ACS_RTR_TRACE_MESSAGE("poll on handle pos:<%d> failed error:<%d> revents:<%d>",
										idx, errno, fds[idx].revents);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: RTR Manager poll on handle pos:<%d> failed error:<%d> revents:<%d>",
													idx, errno, fds[idx].revents);
				break;
			}
		}
	}
	ACS_RTR_TRACE_MESSAGE("OUT");
}



void ACS_RTR_Manager::packHexString(int ln,char* hexStr,char* pHexStr)
{
	int ind=0;
	for (int i=0;i<ln;i++)
	{
		ind=i/2;
		if (isdigit(hexStr[i])) hexStr[i]-='0';
		else if (hexStr[i]>='a' && hexStr[i]<='f') hexStr[i]=hexStr[i]-'a' +10;
		else hexStr[i]=hexStr[i]-'A' +10;
		if (!(i%2)) pHexStr[ind]=hexStr[i] << 4;
		else pHexStr[ind]|=hexStr[i];
	}

}
