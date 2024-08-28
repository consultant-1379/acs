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

#include "acs_rtr_acachannel.h"
#include "acs_rtr_outputqueue.h"
#include "acs_rtr_events.h"
#include "acs_rtr_statistics.h"
#include "acs_rtr_msread.h"
#include "acs_rtr_filebuild.h"
#include "acs_rtr_blockbuild.h"
#include "acs_rtr_systemconfig.h"

#include "acs_rtr_global.h"
#include "acs_rtr_filenumbermgr.h"
#include "acs_rtr_defaultvalues.h"
#include "acs_rtr_blocktocommitstore.h"
#include "acs_rtr_blockreport.h"
#include "acs_rtr_filereport.h"

#include "acs_rtr_gohblockreporter.h"
#include "acs_rtr_manager.h"
#include "acs_rtr_macros.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <stdexcept>

#include <boost/filesystem.hpp>

namespace{
 	 const char* const FIRST_REF_FILE = "/REF-RTR-0000-0000.0000";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_ACAChannel)

//----------------
// Constructor
//----------------
ACS_RTR_ACAChannel::ACS_RTR_ACAChannel(ACS_RTR_Manager *parent,
									const std::string msName,
									const short cpID,
									RTRMS_Parameters* rtrParams,
									HMACMD5_keys* rtrKeys,
									ACS_RTR_OutputQueue* q,
									RTR_statistics* statptr,
									uint32_t msRecordSize,
									RTR_statistics* msstat)
:_msSleeping(true),
 _cpID(cpID),
 _cpName(),
 _msName(msName),
 _msRoot(),
 _destDir(),
 _repDir(),
 _REFfile(),
 _isMultipleCpSystem(false),
 _recsize(msRecordSize),
 m_newRecordSize(msRecordSize),
 m_recordSizeChanged(false),
 _timeoutExpired(false),
 _fsmStatus(IDLE),
 _cpQueue(NULL),
 _messageStoreClient(NULL),
 _acaClientHandle(ACE_INVALID_HANDLE),
 m_rtrRMEvent(ACE_INVALID_HANDLE),
 m_rtrFEEvent(eventfd(0,0)),
 m_stopEvent(ACE_INVALID_HANDLE),
 m_TimerEvent(ACE_INVALID_HANDLE),
  _fobj(NULL),
 _bobj(NULL),
 _rtrParams(NULL),
 _rtrchIssued(false),
 _outputQueue(q),
 _statptr(statptr),
 _msstat(msstat),
 m_MSManager(parent),
 _safeBadMsgCounter(0),
 m_acaMutex(),
 _stopRequested(false),
 m_jobRemoved(false)
 {
	ACS_RTR_TRACE_MESSAGE("In");

	// create the timer file descriptor
	m_TimerEvent = timerfd_create(CLOCK_MONOTONIC, 0);

	if(m_TimerEvent < 0 )
	{
		ACS_RTR_TRACE_MESSAGE("failed to create timer MS:<%s> CP:<%d>, error:<%d>", _msName.c_str(), _cpID, errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "failed to create timer MS:<%s> CP:<%d>, error:<%d>", _msName.c_str(), _cpID, errno);
	}

	if(NULL == rtrParams)
	{
		ACS_RTR_TRACE_MESSAGE("Received RTRParams is NULL");
	}
	else 
	{
		_rtrParams = new (std::nothrow) RTRMS_Parameters();
		if (NULL == _rtrParams)
		{
			ACS_RTR_TRACE_MESSAGE("Memory ERROR: failed to create RTRMS_Parameters. MS:<%s> CP:<%d>, error:<%d>", _msName.c_str(), _cpID, errno);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "failed to create RTRMS_Parameters. MS:<%s> CP:<%d>, error:<%d>", _msName.c_str(), _cpID, errno);
		}
		else
		{
			memset(_rtrParams, 0, sizeof(RTRMS_Parameters));
			memcpy(_rtrParams, rtrParams, sizeof(RTRMS_Parameters));
		}
	}

	if(_rtrParams->OutputType == BlockOutput) 
	{
		_cpQueue = new (std::nothrow) ACS_RTR_OutputQueue(false);
		if(NULL == _cpQueue)
		{
			ACS_RTR_TRACE_MESSAGE("Memory ERROR: cannot create the local output queue. Errno: %d", errno);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "cannot create the local output queue. Errno: %d", errno);
		}
	}

	memset(&_changedParams, 0, sizeof(RTRMS_Parameters));


	_isMultipleCpSystem = ACS_RTR_SystemConfig::instance()->isMultipleCPSystem();


	if(_isMultipleCpSystem)
	{
		ACS_RTR_FileNumberMgr::instance()->addNode(_msName.c_str(), _cpID);
		_cpName = ACS_RTR_SystemConfig::instance()->cs_getCPName(_cpID);
	}
	else
	{
		ACS_RTR_FileNumberMgr::instance()->addNode(_msName.c_str());
		_cpName = DEFAULT_CPNAME;
	}

	// Init MD5 keys structure
	memcpy(&_RTRkeys, rtrKeys, sizeof(HMACMD5_keys));
	memcpy(&m_changedKeys, rtrKeys, sizeof(HMACMD5_keys));

	ACS_RTR_TRACE_MESSAGE("Out - Channel created for CP: %s/%d and Message store: %s", _cpName.c_str(), _cpID, _msName.c_str());
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_RTR_ACAChannel::~ACS_RTR_ACAChannel(void)
{
	ACS_RTR_TRACE_MESSAGE("In");

	{//HL21578
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_acaMutex);
		// Message store connection.
		if (_messageStoreClient)
		{
			if (!_messageStoreClient->disconnectOnShutdown())
			{

			}

			if(_messageStoreClient)
			{
				delete _messageStoreClient;
				_messageStoreClient = 0;
			}
		}
	}

	if( ACE_INVALID_HANDLE != m_rtrFEEvent)
		 ACE_OS::close(m_rtrFEEvent);

	if( ACE_INVALID_HANDLE != m_TimerEvent)
	{
		cancelTimer();
		ACE_OS::close(m_TimerEvent);
	}

	//HL21578

	if (_isMultipleCpSystem)  //it's using statistics at cp level
	{
		if(_statptr && m_jobRemoved)
		{
			char trace[512] = {0};
			snprintf(trace, sizeof(trace) - 1, "(%s/%s) ACS_RTR_ACAChannel destroying statistics", _msName.c_str(), _cpName.c_str());
			ACS_RTR_TRACE_MESSAGE("%s", trace);
			ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", trace);
			_statptr->delStatFile();
		}
		delete _statptr;
		_statptr = 0;
	}

	delete _cpQueue;
	_cpQueue = 0;


	if(_fobj)
	{
		delete _fobj;
		_fobj = 0;
	}
	if (_bobj) 
	{
		delete _bobj;
		_bobj = 0;
	}
	if(_rtrParams)
	{
		delete _rtrParams;
		_rtrParams =  0;
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}


//------------------------
// setRtrfeEvent
// rtrfe command issued
//------------------------
bool ACS_RTR_ACAChannel::setRtrfeEvent() 
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	bool result = true;

	eventfd_t setEvent = 1U;
	if(eventfd_write(m_rtrFEEvent, setEvent) != 0)
	{
		ACS_RTR_TRACE_MESSAGE("set File End Event failed, error<%d>", errno);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: [%s@%s],  set File End Event failed,FD:<%d> error<%d>", _msName.c_str(), _cpName.c_str(), m_rtrFEEvent, errno);
		result = false;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}


int ACS_RTR_ACAChannel::open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	int result = FAILURE;
	// check if timer FD is ok
	if(m_TimerEvent > 0)
	{
		m_stopEvent = stopEvent;
		m_rtrRMEvent = rmEvent;
		// Start a joinable thread
		result = activate();
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%d>", result);
	return result;
}

//--------------
// svc()
//--------------
int ACS_RTR_ACAChannel::svc()
{
	bool initialized = false;
	// Sleep on error of 30 sec
	const int WAIT_BEFORE_RETRY = 30U;

	ACS_RTR_TRACE_MESSAGE("\n"
			"          -----------------------------------------------\n"
			"              ACA CHANNEL\n"
			"                          Msg Store: %s\n"
			"                          CP Name:   %s\n"
			"\n", _msName.c_str(), _cpName.c_str());

	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "\n"
			"          -----------------------------------------------\n"
			"              ACA CHANNEL\n"
			"                          Msg Store: %s\n"
			"                          CP Name:   %s\n"
			"\n", _msName.c_str(), _cpName.c_str());


	//Check if CP data is present with ACA else retry forever.
	int retry = 0;
	while (isRunning() && ((initialized = init()) == false))
	{
		ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%s] initialization failed. Attempts:<%d>", _msName.c_str(), _cpName.c_str(), (++retry));
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%s] initialization failed. Attempts:<%d>", _msName.c_str(), _cpName.c_str(), retry);
		waitBeforeRetry(WAIT_BEFORE_RETRY);
	}

	if(initialized)
	{
		ACS_RTR_TRACE_MESSAGE("initialization done.");

		//RESTART CHECK
		if (isRunning())
			restartCheck();

		ACS_RTR_TRACE_MESSAGE("start building loop");

		if (isRunning())
			building();

		ACS_RTR_TRACE_MESSAGE("building loop terminated");
		end();
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("initialization failed!!");
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
	return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool ACS_RTR_ACAChannel::init()
{
	ACS_RTR_TRACE_MESSAGE("In");
	setFSMState(INIT);

	if(_rtrParams->OutputType == FileOutput)
	{
		// specific file transfer initializations
		//creates directory structure
		if(!loadDirectoryStructure())
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s@%s] DIRECTORY ERROR: cannot create the directory structure", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			return false;
		}
	}

	/* both file transfer and block transfer initializations */
	if(_stopRequested) return true;
	
	if (!connectToMessageStore(RTRACA_MAX_RETRIES_ON_CONNECT_ERROR))
	{
		//Create message store client
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1, "[%s@%s] MSD ERROR: cannot connect to MSD service", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);

		disconnectFromMessageStore();

		return false;
	}
	if (_stopRequested) return true;

	if (!makeHandles())
	{	//build handles
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1, "[%s@%s] Synchronization ERROR: cannot create handles for ACA Channel", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		return false;
	}

	if ((_rtrParams->OutputType == BlockOutput) && (_recsize > _rtrParams->BlockLength))
	{
		// ERROR CASE
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1,
				"[%s/%s][TQ:%s] channel cannot be started: Record size in ACA (%u) is greater than the Block length in RTR (%u)",
				_msName.c_str(), _cpName.c_str(), _rtrParams->TransferQueue, _recsize, _rtrParams->BlockLength);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		return false;
	}

	return true;
}

//--------------------------
// loadDirectoryStructure
//--------------------------
bool ACS_RTR_ACAChannel::loadDirectoryStructure() 
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_INT32 dwlen = 530;
	char szPath[530] = {0};

	// TODO rework
	ACS_APGCC_CommonLib oComLib;
	ACS_APGCC_DNFPath_ReturnTypeT retCode = oComLib.GetFileMPath( RTR_NBI::FILEM_ATTRIBUTE, szPath, dwlen );

	if (retCode != ACS_APGCC_DNFPATH_SUCCESS)
	{
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] GetFileMPath on:<%s> failed with error:<%d>", _msName.c_str(), _cpName.c_str(), RTR_NBI::FILEM_ATTRIBUTE, retCode);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("fileM path:<%s>", szPath);

	_msRoot.assign(szPath);
	_msRoot.append(RTR_NBI::FILEBUILDING_FOLDER);

	if( ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
	{
		_msRoot.append(_cpName);
		_msRoot.push_back(DirDelim);
		_msRoot.append(_msName);
	}
	else
	{
		_msRoot.append(_msName);
	}

	ACS_RTR_TRACE_MESSAGE("Message store:<%s> building path:<%s>", _msName.c_str(), _msRoot.c_str());

	_destDir = _msRoot ;

	// according to necessities of OHI_FileHandler, we'll use an unique Reported directory
	_repDir.assign(szPath);
	_repDir.append(RTR_NBI::FILETRANSFER_FOLDER);
	_repDir.append(_msName);
	_repDir.push_back(DirDelim);
	_repDir.append(_rtrParams->TransferQueue);

	ACS_RTR_TRACE_MESSAGE("##############################################################\n"
			"\t(%s/%s) Folders:\n"
			"\t        Destination: %s\n"
			"\t        Reported:    %s\n", _cpName.c_str(), _msName.c_str(), _destDir.c_str(), _repDir.c_str());

	ACS_RTR_LOG(LOG_LEVEL_INFO, "##############################################################\n"
				"\t(%s/%s) Folders:\n"
				"\t        Destination: %s\n"
				"\t        Reported:    %s\n", _cpName.c_str(), _msName.c_str(), _destDir.c_str(), _repDir.c_str());

	//dataFiles directory
	int err = 0;
	if (createDir(_msRoot.c_str(), err) == false)
		return false;

	if(!isREFFileInDestDir())
	{
		//it doesn't exist any REF file
		//create REF file
		_REFfile.append(_msRoot);
		_REFfile.append(FIRST_REF_FILE);
		
		ACS_RTR_TRACE_MESSAGE("Creating the first file:<%s>", _REFfile.c_str());
		
		ACE_HANDLE fileHandle = ACE_OS::open(_REFfile.c_str(), O_CREAT, ACE_DEFAULT_OPEN_PERMS);

		if(ACE_INVALID_HANDLE == fileHandle)
		{
			// failed to open the disk-file
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Create REF File ERROR: cannot create REF File", _msName.c_str(), _cpName.c_str());
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			return false;
		}

		ACE_OS::close(fileHandle);
	}

	//Reported directory
	err = 0;
	if (createDir(_repDir.c_str(), err) == false)
	{
		ACS_RTR_TRACE_MESSAGE("Out (false)");
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("Out (true)");
	return true;
}

//---------------------------
// connectToMessageStore
//---------------------------
bool ACS_RTR_ACAChannel::connectToMessageStore(unsigned maxRetries)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool result = false;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_acaMutex);
	unsigned retryCounter = 0;
	//Create message store client
	if (!_messageStoreClient)
	{
		try
		{
			if (_isMultipleCpSystem)
			{
				_messageStoreClient = new (std::nothrow) MSread(_msName.c_str(), _cpID); //throw exception
			}
			else
			{
				_messageStoreClient = new (std::nothrow) MSread(_msName.c_str(), RTR::SITENAME); //throw exception
			}
		}
		catch(std::runtime_error& re)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "ACA Connect ERROR: cannot create MessageStoreClient for the message store:<%s> CP:<%s>, Reason: <%s>", _msName.c_str(), _cpName.c_str(), re.what());

			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			_messageStoreClient = 0;
		}
		catch(...)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "ACA Connect ERROR: cannot create MessageStoreClient for the message store:<%s> CP:<%s>, Last Error:<%d>", _msName.c_str(), _cpName.c_str(), errno);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

			_messageStoreClient = 0;
		}
	}

	if(_messageStoreClient)
	{
		result = _messageStoreClient->connectedToMSD();

		// connect to MSD
		if( !result )
		{
			retryCounter = 0;
			for( ; isRunning() && (retryCounter < maxRetries); )
			{
				_statptr->incConnMSattempts();
				if (!_messageStoreClient->MSconnect())
				{ // connect failed
					_acaClientHandle = ACE_INVALID_HANDLE;
					retryCounter++;
					ACE_Time_Value safeSleep(0,100000);
					ACE_OS::sleep(safeSleep);

					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Cannot connect to ACA: <%s>", _msName.c_str(), _cpName.c_str(), _messageStoreClient->getLastErrorName().c_str());
					ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				}
				else
				{
					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] connected to MSD", _msName.c_str(), _cpName.c_str());
					_acaClientHandle = _messageStoreClient->getHandle();
					result = true;
					ACS_RTR_TRACE_MESSAGE("%s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", tracep);
					break; // exit from the for loop
				}
			}// for
		}
	}
	ACS_RTR_TRACE_MESSAGE("Out, ACA Handle:<%d>, result:<%s>", _acaClientHandle, (result ? "TRUE" : "FALSE"));
	return result;
}
//---------------
// makeHandles
//---------------
bool ACS_RTR_ACAChannel::makeHandles()
{
	ACS_RTR_TRACE_MESSAGE("In");
	//auto reset & nonsignaled event objects
	//they're signalled by RTR_Manager thread receiving commands

	//ACA notification handle is notified as soon as new message stores are ready
	if ((_messageStoreClient == 0) || (_messageStoreClient->connectedToMSD() == false))
	{
		if (connectToMessageStore() == false)
		{
			ACS_RTR_TRACE_MESSAGE("Out (false)");
			return false;
		}
	}

	_acaClientHandle = ACE_INVALID_HANDLE;
	if (NULL != _messageStoreClient)
		_acaClientHandle = _messageStoreClient->getHandle();

	ACS_RTR_TRACE_MESSAGE("Out (true)");
	return true;
}

//-----------------
// restartCheck
//-----------------
void ACS_RTR_ACAChannel::restartCheck()
{
	ACS_RTR_TRACE_MESSAGE("IN, OutputType: %d", _rtrParams->OutputType);
	setFSMState(RESTART_CHECK);

	if(FileOutput == _rtrParams->OutputType)
	{
		//recovery file definitions
		ACS_RTR_TRACE_MESSAGE("recovery file definitions");
		recovery();
	}
	else
	{
		//recovery block definitions
		ACS_RTR_TRACE_MESSAGE("recovery block definitions");

		//check for not committed ACA messages
		unsigned int nAcaMessages = 0;

		while (isRunning() && (m_MSManager->checkForNotCommittedAcaMessages(_cpName, nAcaMessages, _cpID) == false))
		{
			ACS_RTR_TRACE_MESSAGE("[%s@%s] RESTART CHECK - WAITING FOR BGWRPC CONNECTION", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s@%s] RESTART CHECK - WAITING FOR BGWRPC CONNECTION", _msName.c_str(), _cpName.c_str());

			ACE_Time_Value checkFailedPause(0,500*1000); // 500 ms
			ACE_OS::sleep(checkFailedPause);
		}

		ACS_RTR_TRACE_MESSAGE("[%s@%s] %u ACA Messages must be committed", _msName.c_str(), _cpName.c_str(), nAcaMessages);
		ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s@%s] %u ACA Messages must be committed", _msName.c_str(), _cpName.c_str(), nAcaMessages);

		if (isRunning()) recovery(nAcaMessages); //align with ACA
	}
}

//-----------------------------------------
// recovery
// restore block or file definitions
//-----------------------------------------
void ACS_RTR_ACAChannel::recovery(unsigned int nAcaMessages) 
{
	ACS_RTR_TRACE_MESSAGE("In");

	setFSMState(RECOVERY);
	if ( (_rtrParams->OutputType == FileOutput) && (nAcaMessages == 0) )
	{
		recoveryFileDef();
	}
	else if ( (_rtrParams->OutputType == BlockOutput) && (nAcaMessages > 0) )
	{
		recoveryBlockDef(nAcaMessages);
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//-------------------------
// recoveryBlockDef
//-------------------------
void ACS_RTR_ACAChannel::recoveryBlockDef(unsigned int nAcaMessages)
{
	ACS_RTR_TRACE_MESSAGE("In");
	unsigned int acaMsgToRecv = nAcaMessages;

	const nfds_t nfds = 2U;

	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[0].fd = m_stopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = _acaClientHandle;
	fds[1].events = POLLIN;

	// timeout of 1 sec
	ACE_Time_Value timeout(1);

	ACE_INT32 pollResult;

	while(isRunning())
	{
		pollResult = ACE_OS::poll(fds, nfds, &timeout);

		if(pollResult < 0)
		{
			ACS_RTR_TRACE_MESSAGE(" poll failed with error %d", errno);
			if(errno == EINTR)
			{
				// try again
				continue;
			}
			break;
		}

		if( (pollResult == 0) || (fds[1].revents & POLLIN) )
		{
			// IF (ACA has something to read OR timeout expired)
			brep_processACAEvent(acaMsgToRecv, true);
			if(0 == acaMsgToRecv)
			{
				ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[Cp Name: %s, Msg Store: %s] Committing ACA Messages...", _cpName.c_str(), _msName.c_str());
				pfxCommitAcaMessages(nAcaMessages);
				break;
			}
			continue;
		}

		// Check error of handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				ACS_RTR_TRACE_MESSAGE("poll on handle pos:<%d> failed error:<%d> revents:<%d>",
										idx, errno, fds[idx].revents);
				break;
			}
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//----------------------
// recoveryFileDef
//----------------------
void ACS_RTR_ACAChannel::recoveryFileDef()
{
	ACS_RTR_TRACE_MESSAGE("In");
	unsigned int refNum = 0; // last transmitted file number
	unsigned int lcf = 0;  // last committed file number identity
	bool isRtrFileInData = false;
	removeFileInDestDir("BLD"); // BLD file are not necessaries to restore any definitions

	if ((_messageStoreClient == 0) || (_messageStoreClient->connectedToMSD() == false))
	{
		disconnectFromMessageStore();
		if (!connectToMessageStore(1))
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: MSIPERROR - connect to message store failed in recoveryFileDef()");
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "MSIPERROR - connect to message store failed in recoveryFileDef()");
			return;
		}
	}

	//get last committed file number
	for ( unsigned safeCc = 0; (safeCc < 3) && _messageStoreClient && (!_messageStoreClient->getLastCommittedRTRfileNumber(lcf)) && isRunning(); safeCc++)
	{
		char tracep[512] = {0};
		if (_messageStoreClient->connectedToMSD()) snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] getLastCommittedRTRfileNumber failed",  _msName.c_str(), _cpName.c_str());
		else snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Not connected to MSD. Waiting for getLastCommittedRTRfileNumber success. Nr: %u", _msName.c_str(), _cpName.c_str(), lcf);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", tracep);

		if (!_messageStoreClient->connectedToMSD())
		{
			disconnectFromMessageStore();
			if (!connectToMessageStore(1))
			{
				ACS_RTR_TRACE_MESSAGE("[%s/%s] MSIPERROR - connect to message store failed in 'recoveryFileDef()' while retrieving the last committed file number", _msName.c_str(), _cpName.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%s] MSIPERROR - connect to message store failed in 'recoveryFileDef()' while retrieving the last committed file number", _msName.c_str(), _cpName.c_str());
				return;
			}
		}
	}//for

	if (_messageStoreClient == 0) return;

	//get REF file number: the number of the last reported file to the GOH
	std::string fileFound;
	bool found = find_file(_destDir, "REF", fileFound);

	if(!found)
	{
		ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] REF file not found! first RTR startup", _msName.c_str(), _cpName.c_str());

		_REFfile = _destDir + "/REF-RTR-0000-0000.0000";
		
		ACS_RTR_TRACE_MESSAGE("Create REF file:<%s>", _REFfile.c_str());
		
		ACE_HANDLE fileHandle = ACE_OS::open(_REFfile.c_str(), O_CREAT, ACE_DEFAULT_OPEN_PERMS);


		if(ACE_INVALID_HANDLE != fileHandle)
		{
			ACE_OS::close(fileHandle);
		}
	}

	ACS_RTR_TRACE_MESSAGE("fileFound : %s",fileFound.c_str());
	if(found && strcmp(fileFound.c_str(), "REF-RTR-0000-0000.0000") != 0)
	{
		size_t pos = fileFound.find_first_of(".");
		fileFound = fileFound.substr((pos+1), 4);
		// number of the last reported file to the GOH
		refNum = atoi(fileFound.c_str());
		ACS_RTR_TRACE_MESSAGE("refNum : %d",refNum);
		isRtrFileInData = isRTRFileInDestDir();
	}

	unsigned int latestFileNr = (refNum > lcf) ? refNum : lcf;

	ACS_RTR_TRACE_MESSAGE("[%s/%s] latestFileNr : %d", _msName.c_str(), _cpName.c_str(), latestFileNr);
	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s/%s] latestFileNr : %d", _msName.c_str(), _cpName.c_str(), latestFileNr);
	ACS_RTR_TRACE_MESSAGE("[%s/%s] refNum  : %d", _msName.c_str(), _cpName.c_str(), refNum);
	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s/%s] refNum  : %d", _msName.c_str(), _cpName.c_str(), refNum);
	ACS_RTR_TRACE_MESSAGE("[%s/%s] lcf : %d", _msName.c_str(), _cpName.c_str(), lcf);
	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s/%s] lcf : %d", _msName.c_str(), _cpName.c_str(), lcf);

	// initialize the file counter
	if (_isMultipleCpSystem)
		ACS_RTR_FileNumberMgr::instance()->setLCF(latestFileNr, _msName.c_str(), _cpID);
	else
		ACS_RTR_FileNumberMgr::instance()->setLCF(latestFileNr, _msName.c_str());

	if ((lcf == 0) && (refNum == 0)) return;

	if ((lcf == refNum) && (isRtrFileInData == false)) 
	{
		ACS_RTR_TRACE_MESSAGE("[%s/%s] [case 'A' OR case 'B'] Smooth File Transfer restart :-)", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"   #################################################################################################################################\n"
				"   [%s/%s] [case 'A' OR case 'B'] Smooth File Transfer restart :-)\n"
				"   #################################################################################################################################\n",
				_msName.c_str(), _cpName.c_str());

		/* case 'A', description:
		 * RTR restarted while it was building a file */

		/* case 'B', description:
		 * RTR restarted after it closed the build file, but when it's not yet renamed it in RTR* */

	}
	else if ((lcf == refNum) && isRtrFileInData)
	{

		ACS_RTR_TRACE_MESSAGE("[%s/%s] [case 'C'] Before the RTR restart the file was renamed, but the commit towards ACA didn't work", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"   #################################################################################################################################\n"
				"   [%s/%s] [case 'C'] Before the RTR restart the file was renamed, but the commit towards ACA didn't work\n"
				"                      Recovery action: remove the RTR* file in the RTR building folder.\n"
				"   #################################################################################################################################\n",
				_msName.c_str(), _cpName.c_str());

		/* case 'C', description:
		 * the BLD* file has been renamed in RTR* but it hasn't been committed
		 * to ACA neither moved into Reported directory. */
		removeFileInDestDir("RTR");
		return;
	}
	else if ((lcf != refNum) && isRtrFileInData)
	{
		ACS_RTR_TRACE_MESSAGE("[%s/%s] [case 'D'] At startup there are files ready to be sent.", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"   #################################################################################################################################\n"
				"   [%s/%s] [case 'D'] At startup there are files ready to be sent\n"
				"   #################################################################################################################################\n",
				_msName.c_str(), _cpName.c_str());

		/* case 'D', description:
		 * the RTR restart happened after commit towards ACA */
		retransmitUnreportedFile(lcf);
	}
	else if ((lcf > refNum) && (isRtrFileInData == false))
	{
		ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%s] [case 'E'] ", _msName.c_str(), _cpName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "\n"
				"   #################################################################################################################################\n"
				"   [%s/%s] [case 'E'] Probable error while sending the latest file or while renaming the latest REF file.\n"
				"   #################################################################################################################################\n",
				_msName.c_str(), _cpName.c_str());

		/* case 'E', description:
		 * before the restart, first RTR committed to ACA, then it moved the file to the Reported
		 * directory, but RTR didn't send the file. In fact, refNum is not equal to the ACA-lcf
		 * because GOHFileReporter thread doesn't refresh yet the REF* file name when the sendGOH api fails. */

		//retransmitLastReportedFile(lcf);
	}

}

//--------------------------------------------------------------------------------------------------------------
// retransmitUnreportedFile
// this method retransmits all the files committed towards ACA but not reported to GOH.
//--------------------------------------------------------------------------------------------------------------
void ACS_RTR_ACAChannel::retransmitUnreportedFile(unsigned int lcf)
{
	ACS_RTR_TRACE_MESSAGE("In (lcf = %u)", lcf);
	(void)lcf;
	char searchDir[MAX_PATH] = {0};
	std::list<string> lstFiles;

	strncpy(searchDir, _destDir.c_str(), sizeof(searchDir) - 1);
	search_for_all_files(_destDir,lstFiles, "RTR");
	if(lstFiles.size() > 0)
	{
		do
		{
			if (_isMultipleCpSystem)
				_fobj = new (std::nothrow) RTRfile(lstFiles.front().c_str(), _destDir, _repDir, _REFfile, _cpID);
			else
				_fobj = new (std::nothrow) RTRfile(lstFiles.front().c_str(), _destDir, _repDir, _REFfile);

			ACS_RTR_TRACE_MESSAGE("[%s/%s] [lcf = <%u>] Transmit file <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] [lcf = <%u>] Transmit file <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());

			ACS_RTR_GohTransmitInfo* packet = new (std::nothrow) ACS_RTR_GohTransmitInfo();
			packet->setRTRFile(_fobj);
			if (_msstat)
				packet->setStatistics(_msstat);
			else
				packet->setStatistics(_statptr);

			bool pushOk = false;
			while (!pushOk) 
			{
				pushOk = _outputQueue->pushData(packet);
				if (pushOk == false) 
				{
					ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%s] [lcf = <%u>] Cannot push file to GOH Sender: <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%s] [lcf = <%u>] Cannot push file to GOH Sender: <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());

					ACE_OS::sleep(1);
					if(!isRunning())
					{
						//delete _fobj;			deleted in class ACS_RTR_GohTransmitInfo 
						if(packet)
						{
							delete packet;
							packet = 0;
						}
						_fobj = 0;
						return;
					}
				}
				else
				{
					ACS_RTR_TRACE_MESSAGE("[%s/%s] [lcf = <%u>] File pushed to GOH Sender: <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());
					ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] [lcf = <%u>] File pushed to GOH Sender: <%s>", _msName.c_str(), _cpName.c_str(), lcf, _fobj->getFileName().c_str());
				}
			}
			if (_msstat)
			{
				_msstat->addReadMSdata(_fobj->getFSize());
			}
			else
			{
				if (_statptr)
				{
					_statptr->addReadMSdata(_fobj->getFSize());
				}
			}
			_fobj = 0;
			lstFiles.pop_front();
		}
		while(lstFiles.size() > 0);
	}
	ACS_RTR_TRACE_MESSAGE("Out");
}
//----------------------------------------------------------------------------
// verifyNumOfFile
// returns true if the REF file is greater than last committed file
//----------------------------------------------------------------------------
bool ACS_RTR_ACAChannel::verifyNumOfFile(const char* rtrFile, unsigned int lcf)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool retValue = false;
	string sRtrFile = rtrFile;
	int idx = sRtrFile.find_last_of(".");
	sRtrFile = sRtrFile.substr((idx+1), 4);
	unsigned int num = atoi(sRtrFile.c_str());
	if (num <= lcf) retValue = true;
	ACS_RTR_TRACE_MESSAGE("Out (%s)", (retValue ? "true" : "false"));
	return retValue;
}

//----------------------------------------------------------
// removeFileInDestDir
// remove 'searchkey*' files into destination directory.
// searchkey = {"BLD*" | "REF*" | "RTR*"}
//----------------------------------------------------------
bool ACS_RTR_ACAChannel::removeFileInDestDir(const char* searchkey) 
{
	ACS_RTR_TRACE_MESSAGE("In");

	bool atLeastOne = false;
	std::list<std::string> lstFiles;
	search_for_all_files(_destDir, lstFiles, searchkey);
	int numOfFiles=lstFiles.size();
	if( numOfFiles  > 0)
	{
		do
		{
			std::string fullFilePath = _destDir+ "/" + lstFiles.front();

			if (remove(fullFilePath.c_str()))
			{
				//Error
				ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%s] Cannot remove file: %s, errno: %d", _msName.c_str(), _cpName.c_str(), fullFilePath.c_str(), errno);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%s] Cannot remove file: %s, errno: %d", _msName.c_str(), _cpName.c_str(), fullFilePath.c_str(), errno);
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("[%s/%s] Removed file: %s", _msName.c_str(), _cpName.c_str(), fullFilePath.c_str());
				ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] Removed file: %s", _msName.c_str(), _cpName.c_str(), fullFilePath.c_str());
			}
			lstFiles.pop_front();
		}
		while(lstFiles.size() > 0);			
	}

	ACS_RTR_TRACE_MESSAGE("Out (at least one file = %s)", (atLeastOne ? "true" : "false"));
	return atLeastOne;
}

//-----------------------------------------------------------
// isRTRFileInDestDir
// look for "RTR*" files in destination directory
//-----------------------------------------------------------
bool ACS_RTR_ACAChannel::isRTRFileInDestDir(char* rtrFile) 
{
	ACS_RTR_TRACE_MESSAGE("In");

	bool atLeastOne = false;
	char searchDir[MAX_PATH] = {0};
	std::list<std::string> lstFiles;
	snprintf(searchDir, sizeof(searchDir) - 1, _destDir.c_str());
	ACS_RTR_TRACE_MESSAGE("Searching RTR files in dir: %s", _destDir.c_str());
	search_for_all_files(_destDir, lstFiles, "RTR");
	if(lstFiles.size() > 0)
	{
		do
		{
			if (rtrFile)
			{
				ACS_RTR_TRACE_MESSAGE("Found RTR file dataFiles: %s", lstFiles.front().c_str());
				strcpy(rtrFile, lstFiles.front().c_str());
			}
			atLeastOne = true;
			break;
			lstFiles.pop_front();
		}
		while(lstFiles.size() > 0);
	}

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (atLeastOne ? "true" : "false"));
	return atLeastOne;
}

//--------------------------
// isREFFileInDestDir
//--------------------------
bool ACS_RTR_ACAChannel::isREFFileInDestDir()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, REF files in directory:<%s>", _msName.c_str(), _destDir.c_str());

	std::string refFile;
	bool found = find_file(_destDir, "REF", refFile);

	ACS_RTR_TRACE_MESSAGE("Out, Found:<%s>", (found ? "TRUE" : "FALSE"));
	return found;
}

//-----------
// building
//-----------
void ACS_RTR_ACAChannel::building()
{
	ACS_RTR_TRACE_MESSAGE("In");
	setFSMState(BUILDING);
	if(_rtrParams->OutputType == FileOutput)
	{
		fileBuilding();
	}
	else
	{
		blockBuilding();
	}
	ACS_RTR_TRACE_MESSAGE("Out");
}

//----------------
// fileBuilding
//----------------
void ACS_RTR_ACAChannel::fileBuilding()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO [%s@%s] STARTING File Building", _msName.c_str(), _cpName.c_str());

	ACE_INT32 pollResult = 0;
	ACE_INT32 nextTimeout = 0;

	const nfds_t nfds = (rtrfe_pos + 1U);
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[stop_pos].fd = m_stopEvent;
	fds[stop_pos].events = POLLIN;

	fds[rtrrm_pos].fd = m_rtrRMEvent;
	fds[rtrrm_pos].events = POLLIN;

	fds[holdTime_pos].fd = m_TimerEvent;
	fds[holdTime_pos].events = POLLIN;

	fds[rtrfe_pos].fd = m_rtrFEEvent;
	fds[rtrfe_pos].events = POLLIN;


	{
		// Read from ACA regardless of the handle.
		//      The ACA handle is signalled only in case of an MTAP message comes from CP.
		//      This happens in case CP is not transmitting, but there are unread ACA files.
		 if(_rtrchIssued && !_fobj)
		 {
			 memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
			 memcpy(&_RTRkeys, &m_changedKeys, sizeof(HMACMD5_keys));
			 _rtrchIssued = false;
		 }

		int processResult = frep_processACAEvent();

		ACS_RTR_TRACE_MESSAGE("First frep_processACAEvent done. Result: %s. Return code: %d.",
				(( RTRACA_NO_ERROR == processResult || RTRACA_FILE_BUILDING_OK == processResult) ? "SUCCESS" : "ERROR"),
				processResult);
	}

	nextTimeout = RTRACA_INITIAL_BUILDING_SLEEP;

	while(isRunning())
	{
		ACE_Time_Value timeoutInSec(nextTimeout);

		// re-int data ready handle since it can be change after a reconnection
		fds[msclient_pos].fd = _acaClientHandle;
		fds[msclient_pos].events = POLLIN;
		fds[msclient_pos].revents = 0;

		pollResult = ACE_OS::poll(fds, nfds, timeoutInSec);

		// Check for error on poll
		if(pollResult < 0)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] poll FAILED. last error = %u. TERMINATING THREAD...", _msName.c_str(), _cpName.c_str(), ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			continue;
		}

		// Check for Timeout or data ready
		if( (0 == pollResult) || (fds[msclient_pos].revents & POLLIN ))
		{
			if(0 == pollResult)
			{
				ACS_RTR_TRACE_MESSAGE("Timeout during fileBuilding");
			}

			if (_rtrchIssued)
			{
				//Check for new parameters just in case any file is under construction. See HQ35177
				if (0 == _fobj)
				{
					memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
					memcpy(&_RTRkeys, &m_changedKeys, sizeof(HMACMD5_keys));
					_rtrchIssued = false;
				}
				else
				{
					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1,
							"\n"
							"         ################################"
							"         [%s/%s] Parameter change (rtrch) not possibile now. File under construction.\n"
							"         Use rtrfe to force the change.\n", _msName.c_str(), _cpName.c_str());
					ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", tracep);
					ACS_RTR_TRACE_MESSAGE("%s", tracep);
				}
			}


			int processResult = frep_processACAEvent();
			nextTimeout = adaptiveDelayOnError(processResult);

			switch(processResult)
			{
				case RTRACA_NO_ERROR:
					{
						msSleeping();  // no activity
					}
					break;

				case RTRACA_FILE_BUILDING_OK:
					{
						msWorking(); // rtr is building files
					}
					break;

				case RTRACA_MSD_CONNECT_ERROR:
					{
						ACS_RTR_TRACE_MESSAGE("failed with error:<%d>", processResult);
					}
					break;

				case RTRACA_CREATE_FILE_IF_ERROR:
				case RTRACA_ERR_DISK_FULL:
					{
						ACS_RTR_TRACE_MESSAGE("failed with error:<%d>", processResult);
					}
					break;
			}
			continue;
		}

		// Check for stop request
		if(fds[stop_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a stop request!");
			_stopRequested = true;
			break;
		}

		// Check for job remove request
		if(fds[rtrrm_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a remove request!");
			m_jobRemoved = true;
			break;
		}

		// Check for file end action/command request
		if(fds[rtrfe_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a File End request!");

			// Reset the event
			eventfd_t reset;
			if(eventfd_read(fds[rtrfe_pos].fd, &reset) < 0)
			{
				ACS_RTR_TRACE_MESSAGE("failed to reset file end event, error<%d>", errno);
			}

			if( (reportFile(true) < 0) && (!waitOnProcessingError(RTRACA_BUILDING_ERROR_SLEEP)) )
			{
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1,
						"\n"
						"         ################################"
						"         [%s/%s] File Switch failed. Error at reporting file.\n", _msName.c_str(), _cpName.c_str());

				ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", tracep);
				ACS_RTR_TRACE_MESSAGE("%s", tracep);
				break;
			}
			else
			{
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1,
						"\n"
						"         ################################"
						"         [%s/%s] File Switch done", _msName.c_str(), _cpName.c_str());

				ACS_RTR_LOG(LOG_LEVEL_INFO, "%s", tracep);
				ACS_RTR_TRACE_MESSAGE("%s", tracep);
			}

			if(!isRunning()) break;
			cancelTimer();
			createRTRFileInterface();
			ACS_RTR_TRACE_MESSAGE("File End request handled!");
			continue;
		}

		if(fds[holdTime_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a Timer elapsed request!");

			if( (frep_processTimeout() != RTRACA_NO_ERROR) &&
				(!waitOnProcessingError(RTRACA_BUILDING_ERROR_SLEEP) ) )
			{
				ACS_RTR_TRACE_MESSAGE("Error on timer handling");
				break;
			}
			ACS_RTR_TRACE_MESSAGE("Timer elapsed request handled!");
			continue;
		}

		// Check error of handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1, "ERROR: [%s@%s] ACA Channel poll on handle:<%d> pos:<%d> failed error:<%d> revents:<%d>",
												_msName.c_str(), _cpName.c_str(), fds[idx].fd, idx, errno, fds[idx].revents);
				ACS_RTR_TRACE_MESSAGE("%s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if(idx != msclient_pos)
				{
					// Fatal error
					return;
				}
				else
				{
					// if ACA Msg handle try to get a new one
					disconnectFromMessageStore();
				}
			}
		}

		ACS_RTR_TRACE_MESSAGE("IF ALL OK NEVER YOU SEE THIS TRACE!");
	}

	ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO [%s@%s] END File Building", _msName.c_str(), _cpName.c_str());
	ACS_RTR_TRACE_MESSAGE("OUT, [%s/%s] end of file building", _msName.c_str(), _cpName.c_str());
}

//----------------------------------------------------------------------------
// reportFile
// Report files to GOH reporter thread through _outputQueue
//----------------------------------------------------------------------------
int ACS_RTR_ACAChannel::reportFile(bool forcedEnd)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, forceEnd:<%s>", _msName.c_str(), (forcedEnd ? "TRUE" : "FALSE"));
	int retValue = RTRACA_REPORTING_OK;
	char errstr[1024] = {0};
	bool diskProblem = false;

	setFSMState(REPORTING);

	if (_fobj == 0)
		return retValue;

	if(_fobj->fileClose(_messageStoreClient, errstr, sizeof(errstr) - 1, forcedEnd, diskProblem))
	{
		//flush-rename-commit
		ACS_RTR_GohTransmitInfo* packet = new (std::nothrow) ACS_RTR_GohTransmitInfo();
		packet->setRTRFile(_fobj);

		if (_msstat)
			packet->setStatistics(_msstat);
		else
			packet->setStatistics(_statptr);

		bool pushOk = false;

		unsigned traceCounter = 0;
		ACE_Time_Value sleepOnPushError(0,300000);
		while (!pushOk)
		{
			pushOk = _outputQueue->pushData(packet);
			if (!pushOk)
			{
				ACS_RTR_TRACE_MESSAGE("loop on pushData");
				ACE_OS::sleep(sleepOnPushError);
				traceCounter++;

				if(!isRunning())
				{
					//stop requested
					if (_statptr)
					{
						// statistics data, not yet stored
						_statptr->clearBLD();
					}

					// _fobj will be deleted in class ACS_RTR_GohTransmitInfo
					_fobj->deleteFile();

					if(packet)
					{
						delete packet;
						packet = 0;
					}
					_fobj = 0;

					if(_messageStoreClient)
						disconnectFromMessageStore();

					return retValue;
				}
			}
		}
		//push ok
		if(_msstat)
			_msstat->addReadMSdata(_fobj->getFSize());
		else if (_statptr)
			_statptr->addReadMSdata(_fobj->getFSize());

		//delete _fobj;
		_fobj = 0;
	}
	else
	{
		// file closure failed
		char tracep[1024] = {0};
		snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] ERROR: File reporting failed because of %s. %s", _msName.c_str(), _cpName.c_str(), (diskProblem ? "a disk problem" : "an ACA problem"), errstr);

		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

		if (diskProblem)
		{
			retValue = RTRACA_ERR_DISK_FULL;
			if (_fobj != 0)
			{
				if (_statptr)
					_statptr->clearBLD();  // statistics data, not yet stored

				_fobj->deleteFile(); // delete the BLD file.
				if(_fobj)
				{
					delete _fobj;
					_fobj = 0;
				}
			}
			if (_messageStoreClient && _messageStoreClient->connectedToMSD())
				_messageStoreClient->rollback();
		}
		else
		{
			retValue = RTRACA_MSD_CONNECT_ERROR;
			disconnectFromMessageStore(); // implicit rollback
		}
	}

	if (_rtrchIssued)
	{
		memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
		memcpy(&_RTRkeys, &m_changedKeys, sizeof(HMACMD5_keys));
		_rtrchIssued = false;
	}

	setFSMState(BUILDING);
	ACS_RTR_TRACE_MESSAGE("Out");
	return retValue;
}

//-------------------------------------------------------
// restore the connection:
// 1) rollback on the aca message store client
// 2) remove last RTR file
//-------------------------------------------------------
void ACS_RTR_ACAChannel::restoreACAConnection()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	cancelTimer();
	_statptr->clearBLD(); // statistics data not yet stored

	if(_rtrParams->OutputType == FileOutput)
	{
		ACS_RTR_TRACE_MESSAGE("File Based Job");
		if (_fobj)
		{
			_fobj->deleteFile(); // clean BLD file and decrease the file counter
			delete _fobj;
			_fobj = 0;
		}
		removeFileInDestDir("BLD");
	}

	if( (_rtrParams->OutputType == BlockOutput) && _bobj)
	{
		ACS_RTR_TRACE_MESSAGE("Block Based Job");
		delete _bobj;
		_bobj = 0;
	}

	disconnectFromMessageStore();

	if(!connectToMessageStore())
	{
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1, "[%s@%s] ERROR: connection to message store failed!", _cpName.c_str(), _msName.c_str());
		ACS_RTR_TRACE_MESSAGE("%s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//--------------------------------
// createRTRFileInterface
//--------------------------------
void ACS_RTR_ACAChannel::createRTRFileInterface()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	if(NULL == _fobj)
	{
		ACS_RTR_TRACE_MESSAGE("create RTR File object, _recsize:<%d>, _destDir:<%s>, _repDir:<%s>, _REFfile:<%s>",
								_recsize, _destDir.c_str(), _repDir.c_str(), _REFfile.c_str() );

		//create RTRFile object
		_fobj = new (std::nothrow) RTRfile(_msName, _recsize, _rtrParams, &_RTRkeys, _destDir, _repDir, _REFfile, _cpID);

		if( (NULL == _fobj) || (!_fobj->fileOpened()) )
		{
			// error cases
			char tracep[512] = {0};

			if(_fobj)
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] fileOpened() failed. Last ERROR:<%d>", _msName.c_str(), _cpName.c_str(), errno);
			else
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Memory ERROR: can't create RTRfile object", _msName.c_str(), _cpName.c_str());

			ACS_RTR_TRACE_MESSAGE("%s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

			delete _fobj;
			_fobj = NULL;
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("RTR File object created");
			startTimer();
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//-----------------
// cancelTimer
//-----------------
bool ACS_RTR_ACAChannel::cancelTimer()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	bool result = false;

	_timeoutExpired = false;

	struct itimerspec resetValue;
	resetValue.it_value.tv_sec = resetValue.it_value.tv_nsec = 0U;
	resetValue.it_interval.tv_sec = resetValue.it_interval.tv_nsec = 0U;

	result = (timerfd_settime(m_TimerEvent, 0, &resetValue, NULL) >= 0);
	if(!result)
	{
		ACS_RTR_TRACE_MESSAGE("MS:[<%s>/<%u>] timer cancel failed, error<%d>",  _msName.c_str(), _cpID, errno);
	}

	ACS_RTR_TRACE_MESSAGE("Out, timer canceled:<%s>", (result ? "YES": "NO"));
	return result;
}

bool ACS_RTR_ACAChannel::resetTimer()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	bool result = false;

	struct itimerspec resetValue;
	resetValue.it_value.tv_sec = resetValue.it_value.tv_nsec = 0U;
	resetValue.it_interval.tv_sec = resetValue.it_interval.tv_nsec = 0U;

	result = (timerfd_settime(m_TimerEvent, 0, &resetValue, NULL) >= 0);
	if(!result)
	{
		ACS_RTR_TRACE_MESSAGE("MS:[<%s>/<%u>] timer cancel failed, error<%d>",  _msName.c_str(), _cpID, errno);
	}

	ACS_RTR_TRACE_MESSAGE("Out, timer canceled:<%s>", (result ? "YES": "NO"));
	return result;
}

//---------------
// startTimer
//---------------
bool ACS_RTR_ACAChannel::startTimer()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	bool result = false;
	int expirationTimeInSec = 0;
	_timeoutExpired = false;

	// check job type
	if(NULL != _fobj )
	{
		expirationTimeInSec = _fobj->getFileHoldTime();
	}
	else if(NULL != _bobj)
	{
		expirationTimeInSec = _bobj->getBlockHoldTime();
	}

	ACS_RTR_TRACE_MESSAGE(" timer expiration time:<%d>", expirationTimeInSec);

	if( expirationTimeInSec > 0 )
	{
		 struct itimerspec timeout;
		 timeout.it_value.tv_sec = expirationTimeInSec;
		 timeout.it_value.tv_nsec = 0U;
		 //TR HS24694
		 timeout.it_interval.tv_sec = 0U;
		 timeout.it_interval.tv_nsec = 0U;

		 // set the timer
		 result = (timerfd_settime(m_TimerEvent, 0, &timeout, NULL) >= 0);

		 if(!result)
		 {
			 // error on set timer report the event
			 char tracep[512] = {0};
			 snprintf(tracep, sizeof(tracep) - 1, "[%s/%u] ERROR: cannot set timer for ACA channel. Timer FD:<%d>, timeout:<%d>, Error Code:<%d>",
					 _msName.c_str(), _cpID, m_TimerEvent, expirationTimeInSec, errno);

			 ACS_RTR_TRACE_MESSAGE("%s", tracep);
			 ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		 }
	}

	ACS_RTR_TRACE_MESSAGE("Out, timer started:<%s>", (result ? "YES": "NO"));
	return result;
}


//------------------------------------------------------------------
// meetFileSkipConditions
// check two skip conditions:
// 1) timeout expired AND minimum file size reached;
// 2) OR maximumsize reached;
//------------------------------------------------------------------
bool ACS_RTR_ACAChannel::metFileSkipConditions(unsigned int bufLen)
{
	ACS_RTR_TRACE_MESSAGE("IN, bufLen<%d>", bufLen);

	//no one condition met
	bool ret = false;

	if(NULL != _fobj)
	{
		if( _timeoutExpired && (_fobj->getFSize() >= _rtrParams->MinFileSize))
		{
			ret = true;
		}
		else if(bufLen > 0)
		{
			//this check is necessary because of FixedFileRecords option considered in enoughSpace method
			ret = !_fobj->enoughSpace(bufLen);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (ret ? "TRUE" : "FALSE"));
	return ret;
}

//-------------------------------
// frep_processACAEvent
//-------------------------------
int ACS_RTR_ACAChannel::frep_processACAEvent()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	int ret = RTRACA_NO_ERROR;
	char tracep[512] = {0};

	// State of a message (Filled,Lost..)
	ACS_ACA_MessageStoreMessage::State msgState = ACS_ACA_MessageStoreMessage::Empty;

	if( (NULL == _messageStoreClient) || (!_messageStoreClient->connectedToMSD()) )
	{
		if(!connectToMessageStore())
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: [%s@%s] CANNOT CONNECT TO ACA SERVER", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s@%s] CANNOT CONNECT TO ACA SERVER", _msName.c_str(), _cpName.c_str());
			return RTRACA_MSD_CONNECT_ERROR;
		}
	}

	// check for message store record size change
	if(m_recordSizeChanged)
	{
		_recsize = m_newRecordSize;
		m_recordSizeChanged = false;
		ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] Record Size changed to:<%d>", _msName.c_str(), _cpName.c_str(), _recsize);
	}

	size_t bufLen = (_recsize >= _rtrParams->FileRecordLength) ? _recsize : _rtrParams->FileRecordLength;

	unsigned char* buf = new (std::nothrow) unsigned char[bufLen];

	do
	{
		unsigned int bufln = bufLen;
		int cdrCnt = 0; //additional data

		if( (NULL != _messageStoreClient) && _messageStoreClient->getMSbuf(msgState, bufln, buf, _rtrParams->FiCdrFormat, cdrCnt))
		{
			if (msgState==ACS_ACA_MessageStoreMessage::Filled)
			{
				ret = RTRACA_FILE_BUILDING_OK;
				_safeBadMsgCounter = 0;

				if (metFileSkipConditions(bufln))
				{
					// does BLD file meet conditions?
					ret = reportFile();
					if( !isRunning() ) break;

					if (_timeoutExpired)
					{
						_timeoutExpired = false; //Timer already canceled
					}
					else
					{
						cancelTimer();
					}
					if(ret != RTRACA_REPORTING_OK)
					{
						delete [] buf;
						return ret;
					}
					else
						ret = RTRACA_FILE_BUILDING_OK; // continue
				}

				createRTRFileInterface(); //if doesn't exist create a new fobj
				if (!_fobj)
				{
					ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%u] Cannot create File Object", _msName.c_str(), _cpID);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%u] Cannot create File Object", _msName.c_str(), _cpID);
					delete[] buf;
					return RTRACA_CREATE_FILE_IF_ERROR;
				}

				memset(tracep, 0 , sizeof(tracep));
				if (!_fobj->store(bufln, buf, cdrCnt, tracep, sizeof(tracep)))
				{
					// cannot write to file
					// delete the file and make a rollback, then try again.
					char tracee[512] = {0};
					snprintf(tracee, sizeof(tracee) - 1, "[%s/%s] [CANNOT WRITE BUILDING FILE] %s", _msName.c_str(), _cpName.c_str(), tracep);

					ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracee);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracee);

					resetTimer();
					// statistics data, not yet stored
					_statptr->clearBLD();
					_fobj->deleteFile();
					if(_fobj)
					{
						delete _fobj;
						_fobj = NULL;
					}

					if(_messageStoreClient)
						disconnectFromMessageStore();

					ret = RTRACA_ERR_DISK_FULL;
					break;
				}

				// update statistics.
				_statptr->incFilled();

				//changes for HV18307 start
				if (metFileSkipConditions(bufln)) { // does BLD file meet conditions?
				    ret = reportFile();
				    if (!isRunning()) break;
				    if (_timeoutExpired) _timeoutExpired = false; //WaitableTimer already canceled
				    else cancelTimer();
				    if (ret != RTRACA_REPORTING_OK) { delete [] buf; return ret; }
				    else ret = RTRACA_FILE_BUILDING_OK; // continue
				    }
			   createRTRFileInterface();
			   //changes for HV18307 End
			}
			else if(msgState==ACS_ACA_MessageStoreMessage::Skipped)
			{
				// SKIPPED
				if(_fobj != 0 && _statptr)
				{
					_statptr->incSkipped();
					std::string tmpS(_fobj->getFileName());
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE SKIPPED] file: %s\n", _msName.c_str(), _cpName.c_str(), tmpS.c_str());

					ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				}

				if ((++_safeBadMsgCounter) == 100)
				{
					ACE_Time_Value tv(0, 50000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}
			}
			else if (msgState==ACS_ACA_MessageStoreMessage::Lost)
			{
				// LOST
				if (_fobj != 0 && _statptr)
				{
					_statptr->incLost();
					std::string tmpS(_fobj->getFileName());
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE LOST] file: %s\n", _msName.c_str(), _cpName.c_str(), tmpS.c_str());

					ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				}

				if ((++_safeBadMsgCounter) == 100)
				{
					ACE_Time_Value tv(0, 50000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}
			}
			else if (msgState==ACS_ACA_MessageStoreMessage::Empty)
			{
				// EMPTY
				if ((++_safeBadMsgCounter) == 100)
				{
					memset(tracep, 0 , sizeof(tracep));
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE EMPTY] READING LOOP...", _msName.c_str(), _cpName.c_str());
					ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

					//Avoid 100% usage CPU
					ACE_Time_Value tv(0, 10000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}

				//according to the apg43 rtr
				if (_messageStoreClient != 0)  _acaClientHandle = _messageStoreClient->getHandle();
				else _acaClientHandle = ACE_INVALID_HANDLE;

				createRTRFileInterface();
				if (!_fobj)
				{
					delete[] buf;
					return RTRACA_CREATE_FILE_IF_ERROR;
				}
			}
		}
		else
		{
			if (isRunning())
			{
				//get Message from ACA fails during the shutdown
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Error getting message. Msg Store API error code: %d", _msName.c_str(), _cpName.c_str(), _messageStoreClient->getLastError());
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if( ACS_ACA_MessageStoreClient::ConnectionLost == _messageStoreClient->getLastError() )
				{
					ACS_RTR_LOG(LOG_LEVEL_WARN, "get Message failed with error-disconnectFromMessageStore");
					// only close the connection
					disconnectFromMessageStore();
				}
				else
				{
					restoreACAConnection();
				}
			}
			ret = RTRACA_MSD_CONNECT_ERROR;
			break;
		}

	} while( (msgState != ACS_ACA_MessageStoreMessage::Empty) && isRunning());

	delete[] buf;

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%d>", _msName.c_str(), ret);
	return ret;
}

//----------------------------------------------------------------------------------------------------------------
// waitOnProcessingError
// wait for rtrrm event when either MSD problems or DISK FULL problems happen
// return values:
//		- true if the timeout is expired
//		- false if the rtrrm is issued or if an error happens into WaitForSingleObject
//----------------------------------------------------------------------------------------------------------------
bool ACS_RTR_ACAChannel::waitOnProcessingError(ACE_INT32 waitTimeout)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, timeout:<%d>", _msName.c_str(), waitTimeout);

	bool result = false;

	const nfds_t nfds = (rtrrm_pos + 1U);
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[stop_pos].fd = m_stopEvent;
	fds[stop_pos].events = POLLIN;

	fds[rtrrm_pos].fd = m_rtrRMEvent;
	fds[rtrrm_pos].events = POLLIN;

	ACE_Time_Value timeout(waitTimeout);

	ACE_INT32 pollResult = ACE_OS::poll(fds, nfds, timeout);

	if(pollResult < 0)
	{
		// Error on poll
		char tracep[512] = {0};
		snprintf(tracep, sizeof(tracep) - 1,"[%s/%s] POOL FAILED. Last ERROR: %u", _msName.c_str(), _cpName.c_str(), ACE_OS::last_error());
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
	}
	else if(0 == pollResult)
	{
		// Timeout elapsed
		result = true;
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%s>", _msName.c_str(), (result ? "TRUE" : "FALSE") );
	return result;
}

//-----------------------------------------------------------
// frep_processTimeout
// process a timeout event in file transfer mode
//-----------------------------------------------------------
int ACS_RTR_ACAChannel::frep_processTimeout()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	_timeoutExpired = true;
	int retValue = RTRACA_NO_ERROR;

	if(metFileSkipConditions())
	{
		ACS_RTR_TRACE_MESSAGE(" File Skip Conditions is true");
		if(reportFile() < 0)
		{
			retValue = RTRACA_ERROR;
		}
		msSleeping();

		if(!isRunning())
		{
			return retValue;
		}

		if(!cancelTimer())
		{
			retValue = RTRACA_ERROR;
		}

		createRTRFileInterface();
	}
	else
	{
		if(!resetTimer()) retValue = RTRACA_ERROR;
	}

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%d> ", _msName.c_str(), retValue);
	return retValue;
}

//------------------
// blockBuilding
//------------------
void ACS_RTR_ACAChannel::blockBuilding()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO [%s@%s] STARTING Block Building", _msName.c_str(), _cpName.c_str());

	ACE_INT32 pollResult = 0;
	ACE_INT32 nextTimeout = 0;

	// close FileEnd event since we don't use it in a block Job
	if( ACE_INVALID_HANDLE != m_rtrFEEvent)
	{
		 ACE_OS::close(m_rtrFEEvent);
		 m_rtrFEEvent = ACE_INVALID_HANDLE;
	}

	const nfds_t nfds = rtrfe_pos;
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[stop_pos].fd = m_stopEvent;
	fds[stop_pos].events = POLLIN;

	fds[rtrrm_pos].fd = m_rtrRMEvent;
	fds[rtrrm_pos].events = POLLIN;

	fds[holdTime_pos].fd = m_TimerEvent;
	fds[holdTime_pos].events = POLLIN;


	{
		// Read from ACA regardless of the handle.
		// 	The ACA handle is signalled only in case of an MTAP message comes from CP.
		// 	This happens in case CP is not transmitting, but there are unread ACA files.
		 if(_rtrchIssued && !_bobj)
                {
                        memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
                        _rtrchIssued = false;
                }

		unsigned int none = 0;
		int processResult = brep_processACAEvent(none);

		ACS_RTR_TRACE_MESSAGE("First brep_processACAEvent done. Result: %s. Return code: %d.",
				(( RTRACA_NO_ERROR == processResult || RTRACA_BLOCK_BUILDING_OK == processResult) ? "SUCCESS" : "ERROR"),
				processResult);
	}

	nextTimeout = RTRACA_INITIAL_BUILDING_SLEEP;

	while(!_stopRequested)
	{
		ACE_Time_Value timeoutInSec(nextTimeout);

		// re-int data ready handle since it can be change after a reconnection
		fds[msclient_pos].fd = _acaClientHandle;
		fds[msclient_pos].events = POLLIN;
		fds[msclient_pos].revents = 0;

		pollResult = ACE_OS::poll(fds, nfds, timeoutInSec);

		if(pollResult < 0)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] pool FAILED. last error = %u. TERMINATING THREAD...", _msName.c_str(), _cpName.c_str(), ACE_OS::last_error());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			continue;
		}

		// Check for Timeout or data ready
		if( (0 == pollResult) || (fds[msclient_pos].revents & POLLIN ))
		{
			if(0 == pollResult)
			{
				ACS_RTR_TRACE_MESSAGE("Timeout during blockBuilding");
			}

			unsigned int none = 0;
			
			if (_rtrchIssued)
			{
				//Check for new parameters just in case any block is under construction. See HQ35177
				if (0 == _bobj)
				{
					memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
					_rtrchIssued = false;
				}
				else
				{
					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1,
							"\n"
							"         ################################"
							"         [%s/%s] Parameter change (rtrch) not possibile now. Block burst under construction.\n"
							"         Wait for the transmission in order to use new settings.\n", _msName.c_str(), _cpName.c_str());
					ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", tracep);
					ACS_RTR_TRACE_MESSAGE("%s", tracep);
				}
			}
			
			ACS_RTR_TRACE_MESSAGE("Read loop is starting for %s/%s", _cpName.c_str(), _msName.c_str());
			int processResult = brep_processACAEvent(none);
			nextTimeout = adaptiveDelayOnError(processResult);

			ACS_RTR_TRACE_MESSAGE("Read loop is finished for %s/%s", _cpName.c_str(), _msName.c_str());

			switch(processResult)
			{
				case RTRACA_NO_ERROR:
				{
					msSleeping();
				}
				break;

				case RTRACA_BLOCK_BUILDING_OK:
				{
					msWorking();
				}
				break;

				case RTRACA_MSD_CONNECT_ERROR:
				{
					ACS_RTR_TRACE_MESSAGE("brep_processACAEvent return error<%d>", processResult);
				}
				break;

				case RTRACA_CREATE_BLOCK_IF_ERROR:
				{
					ACS_RTR_TRACE_MESSAGE("brep_processACAEvent return error<%d>", processResult);
				}
			}
			continue;
		}

		// Check for stop request
		if(fds[stop_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a stop request!");
			_stopRequested = true;
			break;
		}

		// Check for job remove request
		if(fds[rtrrm_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a remove request!");
			m_jobRemoved = true;
			break;
		}

		if(fds[holdTime_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received a Timer elapsed request!");
			brep_processTimeout();
			continue;
		}

		// Check error of handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1, "ERROR: [%s@%s] ACA Channel poll on handle:<%d> pos:<%d> failed error:<%d> revents:<%d>",
																_msName.c_str(), _cpName.c_str(), fds[idx].fd, idx, errno, fds[idx].revents);
				ACS_RTR_TRACE_MESSAGE("%s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if(idx != msclient_pos)
				{
					// Fatal Error
					return;
				}
				else
				{
					// if ACA Msg handle try to get a new one
					disconnectFromMessageStore();
				}
			}
		}

		ACS_RTR_TRACE_MESSAGE("IF ALL OK NEVER YOU SEE THIS TRACE!");
	}
	ACS_RTR_LOG(LOG_LEVEL_INFO, "INFO [%s@%s] END Block Building", _msName.c_str(), _cpName.c_str());
	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//----------------------------------------------
// meetBlockSkipConditions
// check two skip conditions:
// 1) timeout expired
// 2) OR maximum block size reached;
//----------------------------------------------
bool ACS_RTR_ACAChannel::metBlockSkipConditions(unsigned int bufLen)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, bufLen:<%d>", _msName.c_str(), bufLen);
	bool ret = false;
	if(NULL != _bobj)
	{
		if( _timeoutExpired && (_bobj->getBlockLength() > 0))
		{
			ret = true;
		}
		else if(bufLen > 0)
		{
			ret = !_bobj->enoughSpace(bufLen);
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (ret ? "TRUE" : "FALSE"));
	return ret;
}

//-------------------------------
// brep_processACAEvent
//-------------------------------
int ACS_RTR_ACAChannel::brep_processACAEvent(unsigned int& acaMsgToRecv, bool recoveryMode)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	int ret = RTRACA_NO_ERROR;

	// State of a message (Filled,Lost..)
	ACS_ACA_MessageStoreMessage::State msgState = ACS_ACA_MessageStoreMessage::Empty;

	if( (NULL == _messageStoreClient) || !_messageStoreClient->connectedToMSD())
	{
		if(!connectToMessageStore())
		{
			ACS_RTR_TRACE_MESSAGE("ERROR: [%s@%s] CANNOT CONNECT TO ACA SERVER", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s@%s] CANNOT CONNECT TO ACA SERVER", _msName.c_str(), _cpName.c_str());
			return RTRACA_MSD_CONNECT_ERROR;
		}
	}

	// check for message store record size change
	if(m_recordSizeChanged)
	{
		_recsize = m_newRecordSize;
		m_recordSizeChanged = false;
		ACS_RTR_LOG(LOG_LEVEL_INFO, "[%s/%s] Record Size changed to:<%d>", _msName.c_str(), _cpName.c_str(), _recsize);
	}

	size_t bufLen = (_recsize >= _rtrParams->BlockLength) ? _recsize : _rtrParams->BlockLength;

	unsigned char* buf = new (std::nothrow) unsigned char[bufLen];

	do
	{
		unsigned int bufln = bufLen;
		//additional data
		int cdrCnt = 0;

		if( (_messageStoreClient != NULL) && _messageStoreClient->getMSbuf(msgState, bufln, buf, false, cdrCnt))
		{
			if((msgState == ACS_ACA_MessageStoreMessage::Filled) && (bufln > 0) && !recoveryMode )
			{
				ret = RTRACA_BLOCK_BUILDING_OK;
				_statptr->incFilled(); // update statistics

				if(metBlockSkipConditions(bufln))
				{
					_bobj->store(bufln,buf);
					reportBlock();

					if(_timeoutExpired)
					{
						_timeoutExpired = false; //Timer already canceled
					}
					else
					{
						cancelTimer();
					}
					continue;
				}

				if( NULL == _bobj )
				{
					//if doesn't exist create a new bobj
					createRTRBlockInterface(false);
					if(NULL == _bobj)
					{
						ACS_RTR_TRACE_MESSAGE("ERROR: [%s/%s] Cannot create Block Interface", _msName.c_str(), _cpName.c_str());
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "[%s/%s] Cannot create Block Interface", _msName.c_str(), _cpName.c_str());
						ret = RTRACA_CREATE_BLOCK_IF_ERROR;
						break;
					}

					if(!_bobj->enoughSpace(bufln))
					{
						//_bobj can contain just one message store
						_bobj->store(bufln,buf);
						reportBlock();
						continue;
					}
					else
					{
						startTimer();
					}
				}

				_bobj->store(bufln,buf);

				//Mapping TR HQ33520 changes from APG43
				if (metBlockSkipConditions(bufln)) {
				    reportBlock();
					if (_timeoutExpired)
					   _timeoutExpired = false;
					else
					   cancelTimer();
					   continue;
				}
				//End of mapping TR HQ33520 chnages from APG43

			}
			else if((msgState == ACS_ACA_MessageStoreMessage::Filled) && recoveryMode )
			{
				ret = RTRACA_BLOCK_BUILDING_OK;
				if(acaMsgToRecv) acaMsgToRecv--;
				if(acaMsgToRecv == 0) break;
			}
			else if(msgState == ACS_ACA_MessageStoreMessage::Skipped)
			{
				_statptr->incSkipped();
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE SKIPPED]", _msName.c_str(), _cpName.c_str());
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if ((++_safeBadMsgCounter) == 100)
				{
					ACS_RTR_TRACE_MESSAGE("msgState = Skipped");
					ACE_Time_Value tv(0, 50000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}
			}
			else if(msgState == ACS_ACA_MessageStoreMessage::Lost)
			{
				_statptr->incLost();

				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE LOST]", _msName.c_str(), _cpName.c_str());
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if ((++_safeBadMsgCounter) == 100)
				{
					ACS_RTR_TRACE_MESSAGE("msgState = Lost");
					ACE_Time_Value tv(0, 50000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}
			}
			else if(msgState == ACS_ACA_MessageStoreMessage::Empty)
			{
				if ((++_safeBadMsgCounter) == 100)
				{
					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] [MESSAGE EMPTY] READING LOOP...", _msName.c_str(), _cpName.c_str());
					ACS_RTR_TRACE_MESSAGE("Warning: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_WARN, "%s", tracep);

					//Avoid 100% usage CPU
					ACE_Time_Value tv(0, 10000);
					ACE_OS::sleep(tv);
					_safeBadMsgCounter = 0;
				}

				// TODO review handle
				//according to the apg43 rtr
				if(_messageStoreClient != 0)
					_acaClientHandle = _messageStoreClient->getHandle();
				else
				{
					ACS_RTR_TRACE_MESSAGE("failed to get ms client handle");
					_acaClientHandle = ACE_INVALID_HANDLE;
				}
			}
		}
		else
		{
			if (isRunning())
			{
				//get Message from ACA fails during the shutdown
				char tracep[512] = {0};
				snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Error getting message. Msg Store API error code: %d", _msName.c_str(), _cpName.c_str(), _messageStoreClient->getLastError());
				ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

				if( ACS_ACA_MessageStoreClient::ConnectionLost == _messageStoreClient->getLastError() )
				{
					// only close the connection
					disconnectFromMessageStore();
				}
				else
				{
					restoreACAConnection();
				}
			}

			ret = RTRACA_MSD_CONNECT_ERROR;
			break;
		}	

	} while((msgState != ACS_ACA_MessageStoreMessage::Empty) && !_stopRequested );

	delete[] buf;

	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>, result:<%d>", _msName.c_str(), ret);
	return ret;
}

//----------------------------
// brep_processTimeout
//----------------------------
void ACS_RTR_ACAChannel::brep_processTimeout()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	_timeoutExpired = true;

	if(metBlockSkipConditions())
	{
		ACS_RTR_TRACE_MESSAGE("Block Skip Conditions is true");
		cancelTimer();
		reportBlock();
		msSleeping();
	}
	else
	{
		resetTimer();
	}
	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//----------------
// reportBlock
//----------------
void ACS_RTR_ACAChannel::reportBlock()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	setFSMState(REPORTING);
	if (_bobj == 0) return;

	//store in the local queue the packet that GohRep thread will transmit
	ACS_RTR_GohTransmitInfo* lclpkt = new (std::nothrow) ACS_RTR_GohTransmitInfo();
	lclpkt->setRTRBlock(_bobj);
	if (_msstat) lclpkt->setStatistics(_msstat);
	else lclpkt->setStatistics(_statptr);
	lclpkt->setCpName(_cpName);
	lclpkt->setCpId(_cpID);

	bool pushOk = false;

	unsigned traceCounter = 0;
	while (!pushOk)
	{
		pushOk = _cpQueue->pushData(lclpkt);
		if (pushOk == false)
		{
			ACE_Time_Value tv(0, 300000);
			ACE_OS::sleep(tv);
			traceCounter++;
			if(_stopRequested)
			{ //stop requested
				//delete _bobj;    deleted in class ACS_RTR_GohTransmitInfo
				if(lclpkt)
				{
					delete lclpkt;
					lclpkt = 0;
				}
				_bobj = 0;
				return;
			}
		}
	}

	//notify to goh reporter thread the CP producing blocks
	//if (_cpQueue->queueSize() >= (noOfBlocksPerTransaction / 10)) { //A first black delivery for HJ37640
	ACS_RTR_GohTransmitInfo* packet = new (std::nothrow) ACS_RTR_GohTransmitInfo();
	packet->setCpId(_cpID);
	_outputQueue->pushCpIdInfo(packet);
	//}

	//refresh statistics
	if(_msstat)
	{
		_msstat->addReadMSdata(_bobj->getBlockLength());
	}
	else if (_statptr)
	{
		_statptr->addReadMSdata(_bobj->getBlockLength());
	}

	_bobj = 0;

	if (_rtrchIssued)
	{
		//Do Not copy changed HMACMD5 keys since they are not required for blockbased
		memcpy(_rtrParams, &_changedParams, sizeof(RTRMS_Parameters));
		_rtrchIssued = false;
	}
	setFSMState(BUILDING);
	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//----------------------------------
// createRTRBlockInterface
//----------------------------------
void ACS_RTR_ACAChannel::createRTRBlockInterface(bool startT)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	if(NULL == _bobj)
	{
		_bobj = new (std::nothrow) RTRblock(_recsize, _rtrParams);

		if( NULL == _bobj )
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] Memory ERROR: can't create RTRblock object", _msName.c_str(), _cpName.c_str());

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		}
		else
		{
			ACS_RTR_TRACE_MESSAGE("RTRblock object created");
			if(startT)
			{
				startTimer();
				ACS_RTR_TRACE_MESSAGE("start a timer");
			}
		}
	}
	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}

//-----
// end
//-----
void ACS_RTR_ACAChannel::end() 
{
	ACS_RTR_TRACE_MESSAGE("In");
	setFSMState(END);

	if(_isMultipleCpSystem && _statptr)
	{
		delete _statptr;
		_statptr = 0;
	}

	if(_messageStoreClient && _messageStoreClient->connectedToMSD() )
			_messageStoreClient->MSdisconnect();

	if( FileOutput == _rtrParams->OutputType )
	{
		if(_fobj)
		{
			_fobj->deleteFile();
			delete _fobj;
		}
		_fobj = 0;

		removeFileInDestDir("BLD");
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//------------------------------------------------------------
// disconnectFromMessageStore
// disconnect and delete the message store client
//------------------------------------------------------------
void ACS_RTR_ACAChannel::disconnectFromMessageStore()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_acaMutex);
	if(_messageStoreClient)
	{
		if (_rtrParams->OutputType == FileOutput)
		{
			if(_statptr)
				_statptr->clearBLD();

			if (_fobj != 0)
			{
				cancelTimer();
				_fobj->deleteFile(); // delete the BLD file.
				delete _fobj;
				_fobj = 0;
			}
		}
		else
		{
			if (_bobj != 0)
			{
				cancelTimer();
				delete _bobj;
				_bobj = 0;
			}
		}

		if(_messageStoreClient)
		{
			_messageStoreClient->MSdisconnect();

			delete _messageStoreClient; //implicit rollback
			_messageStoreClient = NULL;
		}
	}

	_acaClientHandle = ACE_INVALID_HANDLE;
	ACS_RTR_TRACE_MESSAGE("Out, MS:<%s>", _msName.c_str());
}


//--------------------------------
// pfxCommitAcaMessages
// commit
//--------------------------------
bool ACS_RTR_ACAChannel::pfxCommitAcaMessages()
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_acaMutex);
	bool result = false;

	if( (!_messageStoreClient) || (!_messageStoreClient->connectedToMSD()))
	{
		if (connectToMessageStore() == false)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] CANNOT CONNECT TO ACA!!", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		}
	}

	if(_messageStoreClient && _messageStoreClient->connectedToMSD())
	{

		if(_messageStoreClient->commit())
		{
			result = true;
		}
		else
		{
			char tracep[512] = {0};

			// Understand the Result Code
			switch (_messageStoreClient->getLastError())
			{
				case ACS_ACA_MessageStoreClient::NoMessageToCommit:
				case ACS_ACA_MessageStoreClient::SendPrimitiveFailed:
					result = true;
					break;
				default:
					result = false;
			}

			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] ACA commit failed with error: %s. %s",
					_msName.c_str(), _cpName.c_str(), _messageStoreClient->getLastErrorName().c_str(),
					(result ? "Not a blocking error. Continue..." : "This is a blocking error."));

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);

		}
	}

	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, result:<%s>", _msName.c_str(), (result ? "TRUE" : "FALSE"));
	return result;
}

//--------------------------------
// pfxCommitAcaMessages
// commit
//--------------------------------
bool ACS_RTR_ACAChannel::pfxCommitAcaMessages(ULONGLONG numOfMessages)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_acaMutex);
	bool result = false;

	if( (!_messageStoreClient) || (!_messageStoreClient->connectedToMSD()))
	{
		if (connectToMessageStore() == false)
		{
			char tracep[512] = {0};
			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] CANNOT CONNECT TO ACA!!", _msName.c_str(), _cpName.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		}
	}

	if(_messageStoreClient && _messageStoreClient->connectedToMSD())
	{
		// msd connected
		unsigned char transactionIdData[64] = {0};
		ACE_OS::itoa(numOfMessages, (char*)transactionIdData, 10);
		int transactionIdDataLength = strlen((char*)transactionIdData) + 1;

		if(_messageStoreClient->commit(numOfMessages, transactionIdData, transactionIdDataLength))
		{
			ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Commit done for %zu ACA Messages", _msName.c_str(), _cpName.c_str(), numOfMessages);
			result = true;
		}
		else
		{
			char tracep[512] = {0};

			// Understand the Result Code
			switch (_messageStoreClient->getLastError())
			{
			case ACS_ACA_MessageStoreClient::NoMessageToCommit:
			case ACS_ACA_MessageStoreClient::SendPrimitiveFailed:
				result = true;
				break;
			default:
				result = false;
			}

			snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] ACA commit failed with error: %s. %s",
					_msName.c_str(), _cpName.c_str(), _messageStoreClient->getLastErrorName().c_str(),
					(result ? "Not a blocking error. Continue..." : "This is a blocking error."));

			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);

		}
	}

	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>, result:<%s>", _msName.c_str(), (result ? "TRUE" : "FALSE"));
	return result;
}

//-----------------------------
// changeRTRParameters
//-----------------------------
void ACS_RTR_ACAChannel::changeRTRParameters(RTRMS_Parameters* parameters, HMACMD5_keys* newKey)
{
	ACS_RTR_TRACE_MESSAGE("In, MS:<%s>", _msName.c_str());

	memcpy(&_changedParams, parameters, sizeof(RTRMS_Parameters));
	memcpy(&m_changedKeys, newKey, sizeof(HMACMD5_keys));

	_rtrchIssued = true;
	ACS_RTR_TRACE_MESSAGE("Out");
}

void ACS_RTR_ACAChannel::getRecSize(char * jobBasedDn, unsigned short& recSize)
{
	// Function to get the record size from the Message Store.

	ACS_CC_ReturnType returnCode;
	ACS_APGCC_ImmAttribute attribute_1;
	OmHandler immHandler;
	returnCode=immHandler.Init();

	if(returnCode == ACS_CC_SUCCESS){

		//Extract the Message store DN from File/Block DN
		std::string fileBasedObjname(jobBasedDn);
		size_t posMsName=0;
		posMsName = fileBasedObjname.find(",");
		std::string msgStoreName=fileBasedObjname.substr(posMsName+1);


		//Fetch the recordSize value now!
		char attrname[]="recordSize";
		attribute_1.attrName = attrname;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		attributes.push_back(&attribute_1);
		returnCode = immHandler.getAttribute(msgStoreName.c_str(), attributes);
		recSize=*(reinterpret_cast<unsigned short *>(attribute_1.attrValues[0]));

		//finalize
		returnCode=immHandler.Finalize();
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Imm Failure in getRecSize!");
	}

}

void ACS_RTR_ACAChannel::waitBeforeRetry(int waitTimeOutSec)
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
		// Waits for waitTimeOutSec sec before re-try, exit in case of a stop service request
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

ACE_UINT32 ACS_RTR_ACAChannel::adaptiveDelayOnError(int errorCode)
{
	ACS_RTR_TRACE_MESSAGE("IN");
	ACE_UINT32 nextTimeout = 0;

	switch(errorCode)
	{
		case RTRACA_NO_ERROR:
		case RTRACA_BLOCK_BUILDING_OK:
		case RTRACA_FILE_BUILDING_OK:
			{
				nextTimeout = RTRACA_BUILDING_SLEEP;
			}
			break;
		case RTRACA_MSD_CONNECT_ERROR:
			{
				nextTimeout = RTRACA_ACA_CONNECTION_ERROR_SLEEP;
			}
			break;

		case RTRACA_CREATE_BLOCK_IF_ERROR:
		case RTRACA_CREATE_FILE_IF_ERROR:
			{
				nextTimeout = RTRACA_BUILDING_ERROR_SLEEP;
				break;
			}

		case RTRACA_ERR_DISK_FULL:
		default:
			{
				nextTimeout = RTRACA_DISK_ERROR_SLEEP;
				break;
			}
	}

	//Return value in milliseconds
	ACS_RTR_TRACE_MESSAGE("OUT <%d>", nextTimeout);

	return nextTimeout;
}

bool ACS_RTR_ACAChannel::find_file(const std::string& input_folder, const char* file_seach_key, std::string& path_found)
{
	ACS_RTR_TRACE_MESSAGE("In, input_folder:<%s>, search:<%s>", input_folder.c_str(), file_seach_key );

	bool result = false;
	std::string file_seach_key_s(file_seach_key);

	boost::filesystem::path dir_path(input_folder);
	try
	{
		if( boost::filesystem::exists(dir_path) )
		{
			boost::filesystem::directory_iterator end_itr;
			for ( boost::filesystem::directory_iterator itr( dir_path ); (itr != end_itr) && !result; ++itr )
			{
				if ( boost::filesystem::is_directory(itr->status()) == false)
				{
					if (itr->path().filename().string().find(file_seach_key_s) != std::string::npos)
					{
						path_found = itr->path().filename().string();
						result = true;
						ACS_RTR_TRACE_MESSAGE("found file: <%s>", path_found.c_str());
					}
				}
			}
		}
	}
	catch(const boost::filesystem::filesystem_error& ex)
	{
		ACS_RTR_TRACE_MESSAGE("ERROR: cannot search in folder:<%s>, file: <%s>", input_folder.c_str(), file_seach_key_s.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "cannot search in folder:<%s>, file: <%s>", input_folder.c_str(), file_seach_key_s.c_str());
		result = false;
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE" : "FALSE") );

	return result;
}

void ACS_RTR_ACAChannel::search_for_all_files(const std::string& input_folder, std::list<std::string>& lstFiles, const char* file_seach_key)
{
	ACS_RTR_TRACE_MESSAGE("In, input_folder:<%s>, search:<%s>", input_folder.c_str(), file_seach_key );

	bool result = false;
	std::string file_seach_key_s(file_seach_key);

	boost::filesystem::path dir_path(input_folder);
	try
	{
		if( boost::filesystem::exists(dir_path) )
		{
			boost::filesystem::directory_iterator end_itr;
			for ( boost::filesystem::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
			{
				std::string file_found;
				if ( boost::filesystem::is_directory(itr->status()) == false)
				{
					// Check that the filename starts with the searched tag
					std::string partialFilename( itr->path().filename().string().substr(0, file_seach_key_s.length()) );

					//if( itr->path().filename().find(file_seach_key_s.c_str(), 0, file_seach_key_s.length()) != std::string::npos)
					if(partialFilename.compare(file_seach_key_s) == 0)
					{
						// get only the filename without its path
						file_found = itr->path().filename().string();
						lstFiles.push_back(file_found);
						ACS_RTR_TRACE_MESSAGE("found file: <%s>", file_found.c_str());
					}
				}
			}
		}
	}
	catch(const boost::filesystem::filesystem_error& ex)
	{
		ACS_RTR_TRACE_MESSAGE("ERROR: cannot search in folder:<%s>, file: <%s>", input_folder.c_str(), file_seach_key_s.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "cannot search in folder:<%s>, file: <%s>", input_folder.c_str(), file_seach_key_s.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out, result:<%s>", (result ? "TRUE":"FALSE"));

}
/*============================================================================
	ROUTINE: changeMessageStoreRecordSize
 ============================================================================ */
void ACS_RTR_ACAChannel::changeMessageStoreRecordSize(const uint32_t& recordSize)
{
	ACS_RTR_TRACE_MESSAGE("In, MS record Size changed from <%d> to <%d> bytes, CP:<%s>", _recsize, recordSize, _cpName.c_str());

	m_newRecordSize = recordSize;
	m_recordSizeChanged = true;

	ACS_RTR_TRACE_MESSAGE("Out");
}

