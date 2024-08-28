//********************************************************************************
//
// NAME
// ACS_RTR_GOHBlockReporter.cpp
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
// This class implements a thread for the communication with DBO.
// It retrieves blocks from ACS_RTR_ACAChannel thread using the
// ACS_RTR_ACAChannel output queue.
//
//********************************************************************************
#include "acs_rtr_gohblockreporter.h"
#include "acs_rtr_global.h"
#include "acs_rtr_defaultvalues.h"
#include "ace/ACE.h"
#include "acs_rtr_server.h"
#include <ace/Reactor.h>
#include "acs_rtr_macros.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

#include <sys/eventfd.h>

#include <stdexcept>
#include <exception>

extern int SendBlockError;
extern int   AttachBlockTQerror;


namespace {
const char * const _BREP_DERR_NEW_COMMITMAP = "Cannot create the commit file map.";
const char * const _BREP_DERR_CREATE_EVENT = "Cannot create a windows event.";
const char * const _BREP_DERR_ACA_COMMIT = "Commit towards ACA failed!";
}

ACS_RTR_TRACE_DEFINE(ACS_RTR_GOHBlockReporter);

//---------------
// Constructor
//---------------
ACS_RTR_GOHBlockReporter::ACS_RTR_GOHBlockReporter(std::string transferQueue, ACS_RTR_OutputQueue* queue, RTR_Events* eventHandler, RTR_statistics* statptr, ACS_RTR_Manager* rtrManager)
: ACS_RTR_GOHReporter(transferQueue, queue, eventHandler, statptr),
  _mgrCallback(rtrManager),
  commitFileMap(NULL),
  _hDataToSend(ACE_INVALID_HANDLE),
  _tqCriticalSection(),
  _singleCpAttachCounterEnabled(true),
  _isMultipleCPSystem(ACS_RTR_SystemConfig::instance()->isMultipleCPSystem())
{
	ACS_RTR_TRACE_FUNCTION;

	commitFileMap = new (std::nothrow) commitFileMap_t;
	if (!commitFileMap)
	{
		ACS_RTR_TRACE_MESSAGE("Error: Cannot create internal MAP for TQ <%s>", _transferQueue.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot create internal MAP for TQ <%s>", _transferQueue.c_str());
	}

}

//--------------
// Destructor
//--------------
ACS_RTR_GOHBlockReporter::~ACS_RTR_GOHBlockReporter()
{
	ACS_RTR_TRACE_FUNCTION;

	if (commitFileMap)
	{
		for (commitFileMap_t::iterator it = commitFileMap->begin(); it != commitFileMap->end(); ++it)
		{
			ACS_RTR_BlockToCommitStore* commitFile = (*it).second;
			if (commitFile)	delete commitFile;
		}
		commitFileMap->clear();

		delete commitFileMap;
		commitFileMap=0;
	}

}

void ACS_RTR_GOHBlockReporter::fxStop()
{
	ACS_RTR_TRACE_MESSAGE("Stop Requested for TQ <%s>", _transferQueue.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO, "Stop Requested for TQ <%s>", _transferQueue.c_str());
	_gohStopRequested=true;
}

int ACS_RTR_GOHBlockReporter::open(ACE_HANDLE& stopEvent, ACE_HANDLE& rmEvent)
{
	ACS_RTR_TRACE_FUNCTION;

	m_stopEvent = stopEvent;
	m_jobRemoved = rmEvent;
	_hDataToSend = _outputQueue->getHandle();

	int result = activate();

	ACS_RTR_TRACE_MESSAGE("Out, result:<%d>", result);
	return result;
}

//------------
// Operator
//------------
int ACS_RTR_GOHBlockReporter::svc (void )
{
	ACS_RTR_TRACE_FUNCTION;
	setState(ACTIVE);

	int exitCode = 0, numOfFailure = 0;

	ACS_RTR_TRACE_MESSAGE("\n"
			"#####################################################\n"
			"\tStarting thread job [%s]...\n"
			"#####################################################", _transferQueue.c_str());

	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "\n"
			"#####################################################\n"
			"\tStarting thread job [%s]...\n"
			"#####################################################", _transferQueue.c_str());

	do
	{
		exitCode = fxDoWork();
		if (!exitCode) numOfFailure = 0;
		else
		{

			ACE_TCHAR trace[512] = {0};
			ACE_OS::snprintf(trace, sizeof(trace) - 1, "doWork failed for transfer queue %s", _transferQueue.c_str());
			ACS_RTR_TRACE_MESSAGE("Error: %s", trace);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);

			ACE_Time_Value tv(2);
			ACE_OS::sleep(tv);
		}
	} while ((exitCode < 0) && (!_gohStopRequested && numOfFailure++ < FAILURE_RETRIES));

	setState(IDLE);

	ACS_RTR_TRACE_MESSAGE("\n"
			"#####################################################\n"
			"\tTerminated thread job [%s]...\n"
			"#####################################################", _transferQueue.c_str());

	ACS_RTR_LOG(LOG_LEVEL_DEBUG, "\n"
			"#####################################################\n"
			"\tTerminated thread job [%s]...\n"
			"#####################################################", _transferQueue.c_str());

	return exitCode;
}

//-------------
// fxDoWork
//-------------
ACE_INT32 ACS_RTR_GOHBlockReporter::fxDoWork()
{
	ACS_RTR_TRACE_FUNCTION;
	ACS_RTR_TRACE_MESSAGE("STARTING BLOCK TRANSFER LOOP FOR TQ <%s>", _transferQueue.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO, "STARTING BLOCK TRANSFER LOOP FOR TQ <%s>", _transferQueue.c_str());

	const nfds_t nfds = (elem_pos + 1U);
	struct pollfd fds[nfds];
	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[stop_pos].fd = m_stopEvent;
	fds[stop_pos].events = POLLIN;

	fds[rtrrm_pos].fd = m_jobRemoved;
	fds[rtrrm_pos].events = POLLIN;

	fds[elem_pos].fd = _outputQueue->getHandle();;
	fds[elem_pos].events = POLLIN;

	ACE_Time_Value timeout(0, 500*1000);
	ACE_INT32 pollResult = 0;

	// Run loop
	for ( ; !_gohStopRequested; )
	{
		if(!_outputQueue->queueEmpty())
		{
			ACS_RTR_GohTransmitInfo * cpIdPacket = _outputQueue->getFirst();
			if (!cpIdPacket) continue;
			ACS_RTR_OutputQueue * cpQueue = _mgrCallback->getCPQueue(cpIdPacket->getCpID());
			if (!cpQueue)
			{
				ACS_RTR_TRACE_MESSAGE("Error: Output queue does not exist for CP:<%d>, TQ <%s>", cpIdPacket->getCpID(), _transferQueue.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Output queue does not exist for CP:<%d>, TQ <%s>", cpIdPacket->getCpID(), _transferQueue.c_str());

				return -5;
			}

			if (fxLifeCycle(cpQueue))
			{	//remove the CP transmitting information from
				//round robin scheduling
				_outputQueue->popData();
				if (cpQueue->queueEmpty()) delete cpIdPacket;
				else _outputQueue->pushCpIdInfo(cpIdPacket); //retransmit unsent data
			}
			else
			{ // if something fails, schedule another CP according to the round robin policy
				_outputQueue->popData();
				_outputQueue->pushCpIdInfo(cpIdPacket);
			}
		}

		pollResult= ACE_OS::poll(fds, nfds, &timeout);

		if(pollResult < 0)
		{
			ACS_RTR_TRACE_MESSAGE("Error: poll failed. errno: %d. TQ <%s>", errno, _transferQueue.c_str());
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "poll failed. errno: %d. TQ <%s>", errno, _transferQueue.c_str());

			// poll failed
			if (EINTR == errno)
			{
				continue;
			}
			else
			{
				ACS_RTR_TRACE_MESSAGE("Fatal Error: Terminating main loop for TQ <%s>", _transferQueue.c_str());
				ACS_RTR_LOG(LOG_LEVEL_FATAL, "Terminating main loop for TQ <%s>", _transferQueue.c_str());
				return -4;
			}
		}

		// Check for stop request
		if(fds[stop_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Stop requested. Terminating main loop for TQ <%s>", _transferQueue.c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "Stop requested. Terminating main loop for TQ <%s>", _transferQueue.c_str());
			_gohStopRequested = true;
			break;
		}

		// Check for stop request
		if(fds[rtrrm_pos].revents & POLLIN)
		{
			ACS_RTR_TRACE_MESSAGE("Received Job Remove request from user. Terminating main loop for TQ <%s>", _transferQueue.c_str());
			ACS_RTR_LOG(LOG_LEVEL_INFO, "Received Job Remove request from user. Terminating main loop for TQ <%s>", _transferQueue.c_str());
			_gohStopRequested = true;
			continue;
		}

		// Check error of handle
		for(unsigned int idx = 0; idx < nfds; ++idx)
		{
			// Compare with : Error condition or Hung up or Invalid polling request
			if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				ACS_RTR_TRACE_MESSAGE("Fatal Error: poll on handle pos:<%d> failed error:<%d> revents:<%d> TQ <%s>",
						idx, errno, fds[idx].revents,  _transferQueue.c_str());

				ACS_RTR_LOG(LOG_LEVEL_FATAL, "poll on handle pos:<%d> failed error:<%d> revents:<%d> TQ <%s>",
						idx, errno, fds[idx].revents,  _transferQueue.c_str());
				return -1;
			}
		}
	}

	ACS_RTR_TRACE_MESSAGE("TERMINATING BLOCK TRANSFER LOOP FOR TQ <%s>", _transferQueue.c_str());
	ACS_RTR_LOG(LOG_LEVEL_INFO, "TERMINATING BLOCK TRANSFER LOOP FOR TQ <%s>", _transferQueue.c_str());

	return 0;
}

//--------------------------------------------------------------------------------------------------
// fxLifeCycle
// Perform a transaction using GOH:
//		- create an OHI Block Handler object;
//		- transmit 'noOfBlocksPerTransaction' blocks according to the OHI FSM;
//		- delete the OHI object.
//--------------------------------------------------------------------------------------------------
bool ACS_RTR_GOHBlockReporter::fxLifeCycle(ACS_RTR_OutputQueue * cpQueue)
{
	ACS_RTR_TRACE_FUNCTION;
	bool result = false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_tqCriticalSection);

	ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP

	if (cpQueue->queueEmpty()) return true;
	ACS_RTR_GohTransmitInfo * txPacket = cpQueue->getFirst();

	if (!txPacket)
	{
		if (!cpQueue->queueEmpty()) cpQueue->popData();
		ACS_RTR_TRACE_MESSAGE("Error: Leaving ACS_RTR_GOHBlockReporter::fxLifeCycle because txPacket is null. TQ <%s>", _transferQueue.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Leaving ACS_RTR_GOHBlockReporter::fxLifeCycle because txPacket is null. TQ <%s>", _transferQueue.c_str());
		return false;
	}
	string cpname = txPacket->getCpName();
	short cpid = txPacket->getCpID();

	ACE_INT32 dwConsumedTime = 0;

	//create an OHI object
	RTRblockReporter * pReporter = fxCreateBlockReporter(cpname, cpid);
	if (pReporter == 0)
	{
		ACS_RTR_TRACE_MESSAGE("Error: Leaving - pReporter == 0. TQ <%s>", _transferQueue.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Leaving - pReporter == 0. TQ <%s>", _transferQueue.c_str());
		return false;
	}

	//send a block set
	ACE_Time_Value tv(0,20000);
	for (int retry = 0; (retry < BREP_MAX_RETRIES_ON_SEND) && !_gohStopRequested; ++retry)
	{
		if ( (result = fxSendBlockSet(pReporter, cpQueue, cpname.c_str())) || _gohStopRequested) break;
		else
		{
			ACE_OS::sleep(tv);
		}
	}


	//delete the OHI object
	fxDeleteBlockReporter(pReporter);
	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
	// ACS_RTR_TRACE_MESSAGE("[TRANSACTION] END OF LIFE CYCLE. TQ:<%s>. Consumed time:<%u> ms", _transferQueue.c_str(), dwConsumedTime);
	return result;
}

//----------------------------------------
// fxSendBlockSet
//  fxSendBlockSet send the blocks
//----------------------------------------
bool ACS_RTR_GOHBlockReporter::fxSendBlockSet(RTRblockReporter* pReporter, ACS_RTR_OutputQueue* cpQueue, const char* cpname)
{
	ACS_RTR_TRACE_MESSAGE("IN cp:<%s>, tq:<%s>", cpname, _transferQueue.c_str());
	ACE_INT32 dwBeginTime = GetTickCount();//TIMESTAMP
	ACE_INT32 dwConsumedTime = 0;//TIMESTAMP
	ACE_UINT32  bno = 0;

	//----------------------------------------------------------
	// ATTACH PHASE
	// attach to the transfer queue and fetch the block number
	ACE_INT32 err = fxAttachPhase(pReporter, cpQueue, cpname);
	if ((err != AES_OHI_NOERRORCODE) && (err != AES_OHI_BLOCKNRNOTAVAILABLE))
	{
		ACS_RTR_TRACE_MESSAGE(" Error: attach to GOH failed with error: %d, cp:<%s>, tq:<%s>", err, string(cpname).c_str(), _transferQueue.c_str());

		{ //trace
			ACE_TCHAR tracep[512] = {0};
			if (err == AES_OHI_NODESTINATION) ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: attach Failed. Err: %d. Maybe the transfer queue as been removed.", cpname, _transferQueue.c_str(), err);
			else if (err == AES_OHI_CONNECTERROR) ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: attach Failed. Err: %d. Connection error towards remote host.", cpname, _transferQueue.c_str(), err);
			else ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: fxSendBlockSet: fxAttachPhase returned error: %d", cpname, _transferQueue.c_str(), err);

			ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

			if(m_attachBlockTQerror == 0)
			{
				ACS_RTR_TRACE_MESSAGE("Remote Server Unavailable, attach failed from CP %s to TQ %s", cpname, _transferQueue.c_str());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "Remote Server Unavailable, attach failed from CP %s to TQ %s", cpname, _transferQueue.c_str());
				m_attachBlockTQerror = 1;
			}

		}
		ACE_Time_Value attachFailedWaitTv(0,attachFailedWait*1000);
		ACE_OS::sleep(attachFailedWaitTv);
		_singleCpAttachCounterEnabled = true;	// in single cp system we increment the attach statistics
		// counter at the first attach and on any error

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("Error: Leaving fxSendBlockSet. Consumed Time: %d", dwConsumedTime);
		return false;
	}
	if (m_attachBlockTQerror == 1)
	{
		ACS_RTR_TRACE_MESSAGE("Remote Server Available now. Attach succeeded from CP %s to TQ %s", cpname, _transferQueue.c_str());
		ACS_RTR_LOG(LOG_LEVEL_INFO, "Remote Server Available now. Attach succeeded from CP %s to TQ %s", cpname, _transferQueue.c_str());
		m_attachBlockTQerror = 0;
	}
	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
	ACS_RTR_TRACE_MESSAGE("[TRANSACTION] ATTACH DONE. Consumed Time: %d", dwConsumedTime);

	//----------------------------------------------------------
	// BEGIN PHASE
	// begin the transaction
	dwBeginTime = GetTickCount();
	if (!pReporter->transActionReq(BEGIN, bno, err))
	{
		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] transactionBegin() err:%d bno:%u\n", _transferQueue.c_str(), cpname, err, bno);
		ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

		_singleCpAttachCounterEnabled = true;	// in single cp system we increment the attach statistic
		// counter at the first attach and on any error
		return false;
	}
	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
	ACS_RTR_TRACE_MESSAGE("[TRANSACTION] BEGIN DONE. Consumed Time: %d", dwConsumedTime);

	// data buffer and length
	ACE_TCHAR buf[MAX_BLOCKLENGTH];
	ACE_INT32 bufln = 0;
	RTRblock * pBlock = 0;
	ACE_UINT32 sentItem = 0;
	ACS_RTR_GohTransmitInfo * txPacket = 0;

	//----------------------------------------------------------
	// SEND PHASE
	//send one or more blocks
	while ((sentItem < noOfBlocksPerTransaction) && !_gohStopRequested && !cpQueue->queueEmpty() && (txPacket = cpQueue->getItem(sentItem)))
	{
		pBlock = txPacket->getRTRBlock(); //get block to transmit
		pBlock->getSendData(buf, bufln);  //get the block buffer

		bool sendOk = false;

		{
			//TIMESTAMP
			ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
			ACE_INT32 dwConsumedTime = 0;//TIMESTAMP
			sendOk = pReporter->sendGOH(buf, bufln, bno, err);
			dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
			ACS_RTR_TRACE_MESSAGE("[TRANSACTION] #%d SEND DONE. Consumed Time: %d", (sentItem + 1), dwConsumedTime);
		}

		if (sendOk)
		{
			//send ok
			++sentItem;
			if (txPacket->getStatistics())
			{
				//refresh the statistics
				txPacket->getStatistics()->addReportedData(pBlock->getBSize());
			}

			ACS_RTR_TRACE_MESSAGE("[%s -> %s] bno : %d, pBlock->getNumberOfMessages() : %d", (cpname ? cpname : "-"), _transferQueue.c_str(), bno, pBlock->getNumberOfMessages());

			//store the block Id in toCommit.dat
			pReporter->getStore()->addEntry(bno, pBlock->getNumberOfMessages());

			//bind the sequence number with the sent block
			pBlock->setBlockNumber(pReporter->getCurrentBlockNumber());

			if( m_sendBlockError == 1 )
			{
				_eventHandler->ceaseAlarm(alarmInfo::specificProblem2, _transferQueue);
				m_sendBlockError = 0;
			}
		}
		else
		{
			//Send error
			ACS_RTR_TRACE_MESSAGE("[TRANSACTION] #%d SEND. ERROR: %d. BLOCK Nr: %u ...", (sentItem + 1), err, bno);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] #%d SEND. ERROR: %d. BLOCK Nr: %u ...", (sentItem + 1), err, bno);

			_singleCpAttachCounterEnabled = true;	// in single cp system we increment the attach statistic

			{
				//LOG
				ACE_TCHAR tracep[512] = {0};
				switch (err)
				{
					case AES_OHI_NOTINSYNCH: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH failed, GOH_NOTINSYNC. Bno: %u", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_BUFFERFULL: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:GOH_BUFFERFULL Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_NOSERVERACCESS: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_NOSERVERACCESS Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_CONNECTERROR: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_CONNECTERROR Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_ERRORUNKNOWN: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_ERRORUNKNOWN Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_INCORRECTBLOCKSIZE: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_INCORRECTBLOCKSIZE Bno:%u block size: %d\n", cpname, _transferQueue.c_str(), bno, bufln); break;
					case AES_OHI_INCORRECTCOMMAND: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_INCORRECTCOMMAND Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_INVALIDBLOCKNR: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_INVALIDBLOCKNR Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_NOCDHSERVER: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_NOCDHSERVER Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					case AES_OHI_TRANSACTIONNOTACTIVE: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: sendGOH() err:AES_OHI_TRANSACTIONNOTACTIVE Bno:%u\n", cpname, _transferQueue.c_str(), bno); break;
					default: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "%s/%s: unknown error in sendGOH() err:%d Bno:%u, block size: %d\n", cpname, _transferQueue.c_str(), err, bno, bufln);
				}

				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] %s", tracep);
				ACS_RTR_TRACE_MESSAGE("Error: [TRANSACTION] %s", tracep);

			}

			if( m_sendBlockError == 0 && _eventHandler)
			{
				//_eventHandler->initAlarm(alarmInfo::specificProblem2, _transferQueue);
				m_sendBlockError = 1;
			}

			break;
		}
	}

	if (0 == sentItem)
	{
		//nothing has been sent
		ACS_RTR_TRACE_MESSAGE("[TRANSACTION] [TQ: %s, CP: %s] nothing has been sent", _transferQueue.c_str(), cpname);
		ACS_RTR_LOG(LOG_LEVEL_INFO, "[TRANSACTION] [TQ: %s, CP: %s] nothing has been sent", _transferQueue.c_str(), cpname);

		unsigned int terminateError = pReporter->transactionTerminate();
		if (AES_OHI_NOERRORCODE != terminateError)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "pReporter->transactionTerminate failed - GOH Error: %u", terminateError);
			ACS_RTR_TRACE_MESSAGE("Error: pReporter->transactionTerminate failed - GOH Error: %u", terminateError);
		}

		unsigned int detachError = pReporter->detach();
		if (AES_OHI_NOERRORCODE != detachError)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "pReporter->detach failed - GOH Error: %u", detachError);
			ACS_RTR_TRACE_MESSAGE("Error: pReporter->detach failed - GOH Error: %u", detachError);
		}
		return true;
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_INFO, "[TRANSACTION] [TQ: %s, CP: %s] sent %u blocks", _transferQueue.c_str(), cpname, sentItem);
		if (_gohStopRequested) return fxForcedEndOfTransaction(pReporter, cpname, cpQueue);
		else return fxEndOfTransaction(pReporter, cpname, cpQueue);
	}
	ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", string(cpname).c_str(), _transferQueue.c_str());
}

//------------------------------------------------------------------------------------
// fxCreateBlockReporter
// create the OHI Block Interface and create/set the 'toCommit.dat' file
// to store the GOH-block-no/num-Of-Aca-Messages-Per-Block
//------------------------------------------------------------------------------------
RTRblockReporter* ACS_RTR_GOHBlockReporter::fxCreateBlockReporter(string cpname, short cpid) {
	ACS_RTR_TRACE_FUNCTION;

	RTRblockReporter* bRep = 0;
	string streamId = "";

	if (strcmp(cpname.c_str(), DEFAULT_CPNAME))	streamId = cpname;

	ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
	ACE_INT32 dwConsumedTime = 0;//TIMESTAMP

	bRep = new (std::nothrow)RTRblockReporter(OHI_USERSPACE::SUBSYS,
			OHI_USERSPACE::APPNAME,
			_transferQueue.c_str(),
			OHI_USERSPACE::EVENTBLOCKTXT,
			streamId.c_str());

	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP

	if (!bRep)
	{
		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[TQ: %s, CP: %s] Cannot create AES_OHI_BlockHandler2", _transferQueue.c_str(), cpname.c_str());
		ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		return 0;
	}

	//get commit file
	ACS_RTR_BlockToCommitStore* commitFile = fxGetCommitFile(cpname, cpid);
	bRep->setStore(commitFile);

	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP


	return bRep;
}

//----------------------
// fxGetCommitFile
//----------------------
ACS_RTR_BlockToCommitStore* ACS_RTR_GOHBlockReporter::fxGetCommitFile(string cpname, unsigned cpid) {
	ACS_RTR_TRACE_FUNCTION;
	bool found = false;
	ACS_RTR_BlockToCommitStore* retValue = 0;
	commitFileMap_t::iterator it;
	it = commitFileMap->find(cpname);
	if (it != commitFileMap->end())
	{
		ACS_RTR_BlockToCommitStore* commitFile = (*it).second;
		if (commitFile)
		{
			retValue = commitFile;
			found = true;
		}
	}

	//create commit file and add it to map
	if (!found)
	{
		ACS_RTR_TRACE_MESSAGE("Commit File creation for CP: %s", cpname.c_str() );
		ACS_RTR_BlockToCommitStore* store = 0;

		if (_isMultipleCPSystem)
			store = new ACS_RTR_BlockToCommitStore(_mgrCallback->getMessageStore().c_str(), 0, cpid);
		else
			store = new ACS_RTR_BlockToCommitStore(_mgrCallback->getMessageStore().c_str(), RTR::SITENAME);

		pair<commitFileMap_t::iterator,bool> ret = commitFileMap->insert(commitFileMap_t::value_type(cpname, store));
		if (ret.second) retValue = store;
		else delete store;
	}

	return retValue;
}

//----------------------------
// fxAttachPhase
// handle the attach phase
//----------------------------
ACE_UINT32 ACS_RTR_GOHBlockReporter::fxAttachPhase(RTRblockReporter* pReporter, ACS_RTR_OutputQueue * cpQueue, const ACE_TCHAR * cpname)
{
	ACS_RTR_TRACE_MESSAGE("IN cp:<%s>, tq:<%s>", string(cpname).c_str(), _transferQueue.c_str());
	ACE_INT32 err = AES_OHI_NOERRORCODE;
	ACE_UINT32 lastCommBlock = 0;

	if (_statptr && (_singleCpAttachCounterEnabled || _isMultipleCPSystem))
	{
		_statptr->incAttachGOHattempts();
		_singleCpAttachCounterEnabled = false;
	}

	{//TIMESTAMP
		ACE_TCHAR traceTime[256];//TIMESTAMP 
		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP

		ACS_RTR_TRACE_MESSAGE("\n"
				"####################################################################################\n"
				"#        [TRANSACTION] ATTACH PHASE\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				cpname, _transferQueue.c_str());

		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
						"####################################################################################\n"
						"#        [TRANSACTION] ATTACH PHASE\n"
						"#                                   CP: %s\n"
						"#                                   TQ: %s\n"
						"####################################################################################\n",
						cpname, _transferQueue.c_str());

		//attach to the transfer queue
		if (!pReporter->attachGOH(err))
		{
			dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
			ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "[TRANSACTION] ATTACH PHASE. Error %u, ConsumedTime: %u ms !!!", err, dwConsumedTime);//TIMESTAMP
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", traceTime);
			ACS_RTR_TRACE_MESSAGE("%s", traceTime);
			return err;
		}

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "[TRANSACTION] ATTACH PHASE. ConsumedTime: %u ms", dwConsumedTime);//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("%s", traceTime);
	}//TIMESTAMP

	{//TIMESTAMP
		ACE_TCHAR traceTime[256];//TIMESTAMP
		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP

		//fetch the block number
		if(!pReporter->getLastBlockNumber(lastCommBlock,err))
		{
			dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
			ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "getLastBlockNumber() reported error %u, ConsumedTime: %u ms", err, dwConsumedTime);//TIMESTAMP
			ACS_RTR_TRACE_MESSAGE("\n"
					"####################################################################################\n"
					"#        [TRANSACTION] ATTACH PHASE - GET LAST BLOCK NUMBER - ERROR!\n"
					"#                             GOH Error: %u\n"
					"#                                   CP: %s\n"
					"#                                   TQ: %s\n"
					"####################################################################################\n",
					err, cpname, _transferQueue.c_str());

			ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
					"####################################################################################\n"
					"#        [TRANSACTION] ATTACH PHASE - GET LAST BLOCK NUMBER - ERROR!\n"
					"#                            GOH Error: %u\n"
					"#                                   CP: %s\n"
					"#                                   TQ: %s\n"
					"####################################################################################\n",
					err, cpname, _transferQueue.c_str());

			ACS_RTR_TRACE_MESSAGE("%s", traceTime);
			return err;
		}

		ACS_RTR_TRACE_MESSAGE("\n"
				"####################################################################################\n"
				"#        [TRANSACTION] ATTACH PHASE - GET LAST BLOCK NUMBER\n"
				"#              last Committed Block Nr: %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				lastCommBlock, cpname, _transferQueue.c_str());

		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"####################################################################################\n"
				"#        [TRANSACTION] ATTACH PHASE - GET LAST BLOCK NUMBER\n"
				"#              last Committed Block Nr: %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				lastCommBlock, cpname, _transferQueue.c_str());

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "[TRANSACTION] ATTACH PHASE: getLastBlockNumber(): %d - ConsumedTime: %u ms", lastCommBlock, dwConsumedTime);//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("%s", traceTime);
	}//TIMESTAMP

	//if happened an error in a previous 'send' remove the already sent blocks
	ULONGLONG nMessages = cpQueue->deleteItemsByBno(lastCommBlock);

	{//TIMESTAMP
		ACE_TCHAR traceTime[256];//TIMESTAMP
		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP
		//commit towards aca
		if (nMessages > 0)
		{
			ACS_RTR_LOG(LOG_LEVEL_WARN, "[ATTACH PHASE] [CP: %s, TQ: %s] Found %llu ACA messages to be committed. Last BlockNo from BGW is %u", cpname, _transferQueue.c_str(), nMessages, lastCommBlock);
			ACS_RTR_TRACE_MESSAGE("[ATTACH PHASE] [CP: %s, TQ: %s] Found %llu ACA messages to be committed. Last BlockNo from BGW is %u", cpname, _transferQueue.c_str(), nMessages, lastCommBlock);

			while ((_mgrCallback->pfxCommitAcaMessages(nMessages, cpname)) == false)
			{
				ACE_TCHAR trace[512] = {0};
				ACE_OS::snprintf(trace, sizeof(trace) - 1,"%s/%s: ACA COMMIT FAILED. cannot commit %llu aca messages", cpname, _transferQueue.c_str(), nMessages);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", trace);
				ACS_RTR_TRACE_MESSAGE("%s", trace);

				ACE_Time_Value BREP_WAIT_ON_COMMIT_ERROR_TV(0,BREP_WAIT_ON_COMMIT_ERROR*1000);
				ACE_OS::sleep(BREP_WAIT_ON_COMMIT_ERROR_TV);
			}
			ACS_RTR_BlockToCommitStore* store = pReporter->getStore();
			if (store) store->deleteAllEntries();
			ACS_RTR_LOG(LOG_LEVEL_WARN, "[CP: %s, TQ: %s] Deleted all entries from internal toCommit.dat file", cpname, _transferQueue.c_str());
		}

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "[ATTACH PHASE] [pfxCommitAcaMessages()]ConsumedTime: %u ms", dwConsumedTime);//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("%s", traceTime);
	}//TIMESTAMP

	ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", string(cpname).c_str(), _transferQueue.c_str());
	return err;
}

//------------------------------------------
// fxEndOfTransaction
// handle the end of transaction phase
//------------------------------------------
bool ACS_RTR_GOHBlockReporter::fxEndOfTransaction(RTRblockReporter* pReporter, string cpname, ACS_RTR_OutputQueue* cpQueue)
{
	ACS_RTR_TRACE_MESSAGE("IN cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
	if (pReporter->blockState == detached)
	{
		ACS_RTR_TRACE_MESSAGE("pReporter->blockState == detached in fxEndOfTransaction");
		return true;
	}

	ACE_UINT32 lastCommBlock = 0;
	ACE_INT32 err = 0;
	ACE_UINT32 cBno = pReporter->getCurrentBlockNumber();
	{//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP

		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP

		ACE_Time_Value initialDelay(0, TRANSACTION_END_DELAY*1000);
		ACS_RTR_TRACE_MESSAGE("Sleeping before TRANSACTION END");
		ACE_OS::sleep(initialDelay); // Needed for transmission-per-block time estimation

		//end the transaction
		if (!pReporter->transActionReq(END,lastCommBlock,err))
		{
			ACE_TCHAR tracep[512] = {0};
			ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] transactionEnd failed. GOH Error:%d. Last block nr:%u\n", _transferQueue.c_str(), cpname.c_str(), err, lastCommBlock);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
			return false;
		}

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("\n"
				"####################################################################################\n"
				"#        [TRANSACTION] END DONE.    Consumed Time: %d ms\n"
				"#                                   latest tx bno: %u\n"
				"#                                   latest committed bno : %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				dwConsumedTime, cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());

		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"####################################################################################\n"
				"#        [TRANSACTION] END DONE.    Consumed Time: %d ms\n"
				"#                                   latest tx bno: %u\n"
				"#                                   latest committed bno : %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				dwConsumedTime, cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());


		if ((cBno != lastCommBlock)) //We need to wait some more time
		{
			ACE_Time_Value partialCommitDelay(0, BREP_NOT_ALIGNED_DELAY*1000);
			ACE_UINT32 totalCommitDelay = adaptiveCommitDelayEstimation(cBno, lastCommBlock, dwConsumedTime);
			int numOfTicks = static_cast<int>( ((totalCommitDelay > 10000) ? 10000 : totalCommitDelay) / BREP_NOT_ALIGNED_DELAY);
			ACS_RTR_TRACE_MESSAGE("cBno:<%u>, lastCommBlock:<%u>, totalCommitDelay:<%u ms>, numOfTicks:<%i>", cBno, lastCommBlock, totalCommitDelay, numOfTicks);

			for(int i = 0; ( (i < numOfTicks) && !_gohStopRequested) ; ++i)
			{
				ACE_OS::sleep(partialCommitDelay);
			}
		}
	}//TIMESTAMP


	{//TIMESTAMP
		ACE_TCHAR traceTime[256];//TIMESTAMP
		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP
		if (!pReporter->transActionReq(COMMIT,lastCommBlock,err))
		{
			ACE_TCHAR tracep[512] = {0};
			ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] transactionCommit failed. GOH error:%d. Last Block Nr:%u\n", _transferQueue.c_str(), cpname.c_str(), err, lastCommBlock);
			ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

			_singleCpAttachCounterEnabled = true;	// in single cp system we increment the attach statistic
			// counter at the first attach and on any error
			return false;
		}
		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "[transActionReq(COMMIT)]ConsumedTime: %u ms", dwConsumedTime);//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("\n"
				"####################################################################################\n"
				"#        [TRANSACTION] COMMIT DONE. Consumed Time: %d ms\n"
				"#                                   latest tx bno: %u\n"
				"#                                   latest committed bno : %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				dwConsumedTime, cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());

		ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
				"####################################################################################\n"
				"#        [TRANSACTION] COMMIT DONE. Consumed Time: %d ms\n"
				"#                                   latest tx bno: %u\n"
				"#                                   latest committed bno : %u\n"
				"#                                   CP: %s\n"
				"#                                   TQ: %s\n"
				"####################################################################################\n",
				dwConsumedTime, cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());

	}//TIMESTAMP

	//calculate 'delta': the number of transmitted blocks
	ACE_UINT32 delta = 0;
	ACE_UINT32 firstBlockNumber = pReporter->getFirstBlockNumber();
	if (lastCommBlock < firstBlockNumber)
	{
		ACE_UINT32 allrange = MAX_BLOCK_NUMBER - MIN_BLOCK_NUMBER;
		ACE_UINT32 diff = firstBlockNumber - cBno;
		delta = allrange - diff;
	}
	else delta = lastCommBlock - firstBlockNumber;

	//remove the committed blocks from cpQueue and commit towards ACA
	if (delta != 0)
	{
		if(delta > noOfBlocksPerTransaction)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] [TQ: %s, CP: %s] <%u> Blocks transmitted", _transferQueue.c_str(), cpname.c_str(), delta);
		}

		ULONGLONG nMessages = fxDeleteItemsFromCpQueue(cpQueue, delta);
		//commit towards aca
		if (nMessages > 0)
		{
			ACS_RTR_TRACE_MESSAGE("[TRANSACTION] [TQ: %s, CP: %s] %u Blocks have been transmitted. %llu ACA messages can be committed", _transferQueue.c_str(), cpname.c_str(), delta, nMessages);
			ACS_RTR_LOG(LOG_LEVEL_INFO, "[TRANSACTION] [TQ: %s, CP: %s] %u Blocks have been transmitted. %llu ACA messages can be committed", _transferQueue.c_str(), cpname.c_str(), delta, nMessages);

			bool acaCommitDone = false;
			int commitFailedCounter = 0;
			do
			{
				acaCommitDone = _mgrCallback->pfxCommitAcaMessages(nMessages, cpname);
				if (!acaCommitDone)
				{
					char trace[512] = {0};
					snprintf(trace, sizeof(trace) - 1,
							"[TQ: %s, CP: %s] %d ACA COMMIT FAILED. cannot commit %llu aca messages", _transferQueue.c_str(), cpname.c_str(), (++commitFailedCounter), nMessages);

					ACS_RTR_TRACE_MESSAGE("ERROR: [TRANSACTION] %s", trace);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] %s", trace);

					if (!_gohStopRequested)
					{
						ACE_Time_Value commitTv(0,BREP_WAIT_ON_COMMIT_ERROR*1000);
						ACE_OS::sleep(commitTv);
					}
				}

			} while (!acaCommitDone && !_gohStopRequested);

			if (!acaCommitDone && _gohStopRequested)
			{
				ACE_TCHAR tracep[512] = {0};
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1,
								"[TQ: %s, CP: %s] [TRANSACTION] ACA COMMIT FAILED. SHUTDOWN REQUESTED. Skip goh detach and file update.",
								_transferQueue.c_str(), cpname.c_str());

						ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
						ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
						return true;
			}
		}
	}

	{
		ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
		ACE_INT32 dwConsumedTime = 0;//TIMESTAMP

		err = pReporter->detach();
		if (AES_OHI_NOERRORCODE != err)
		{
			char tracep[512] = {0};
			ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[TQ: %s, CP: %s] [TRANSACTION] DETACH ERROR! return value: %d", _transferQueue.c_str(), cpname.c_str(), err);
			ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		}

		dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
		ACS_RTR_TRACE_MESSAGE("[TRANSACTION] DETACH INFO. Consumed Time: %d ms, CP: %s, TQ: %s", dwConsumedTime, cpname.c_str(), _transferQueue.c_str());

	}

	pReporter->blockState = detached;

	//is the transmission failed?
	if (cBno != lastCommBlock)
	{

		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1,
				"[TQ: %s, CP: %s] [TRANSACTION] ERROR: GOH BLOCK REPORTER IS NOT ALIGNED AFTER COMMIT. "
				"Last transmitted BlockNr by RTR:%u; Last received BlockNr by GOH:%u",
				_transferQueue.c_str(), cpname.c_str(), cBno, lastCommBlock);

		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

		if (pReporter->getStore())
		{
			pReporter->getStore()->deleteEntries(lastCommBlock); //refresh 'toCommit.dat'
		}

		return false;
	}
	else
	{
		if (pReporter->getStore())
		{
			pReporter->getStore()->deleteAllEntries(); //refresh 'toCommit.dat'
		}

		return true;
	}
	ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
	return true;
}


//------------------------------------------------------
// fxForcedEndOfTransaction
// handle the end of transaction phase at the shutdown
//------------------------------------------------------
bool ACS_RTR_GOHBlockReporter::fxForcedEndOfTransaction(RTRblockReporter* pReporter, string cpname, ACS_RTR_OutputQueue* cpQueue)
{
	ACS_RTR_TRACE_MESSAGE("IN cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
	if (pReporter->blockState == detached)
	{
		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] pReporter->blockState == detached. Returning...", _transferQueue.c_str(), cpname.c_str());
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		return true;
	}

	ACE_UINT32 lastCommBlock = 0;
	ACE_INT32 err = 0;
	ACE_UINT32 cBno = pReporter->getCurrentBlockNumber();

	//----------------------
	//end transaction
	if (!pReporter->transActionReq(END, lastCommBlock, err))
	{
		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] transactionEnd failed. GOH Error:%d. Last block nr:%u\n", _transferQueue.c_str(), cpname.c_str(), err, lastCommBlock);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("\n"
			"####################################################################################\n"
			"#        [TRANSACTION] END DONE.    \n"
			"#                                   latest tx bno: %u\n"
			"#                                   latest committed bno : %u\n"
			"#                                   CP: %s\n"
			"#                                   TQ: %s\n"
			"####################################################################################\n",
			cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());

	ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
			"####################################################################################\n"
			"#        [TRANSACTION] END DONE.    \n"
			"#                                   latest tx bno: %u\n"
			"#                                   latest committed bno : %u\n"
			"#                                   CP: %s\n"
			"#                                   TQ: %s\n"
			"####################################################################################\n",
			cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());


	//----------------------
	//commit transaction
	if (!pReporter->transActionReq(COMMIT,lastCommBlock,err))
	{
		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep)-1, "[TQ: %s, CP: %s] transactionCommit failed. GOH error: %d. Last Block Nr:%u\n",
				_transferQueue.c_str(), cpname.c_str(), err, lastCommBlock);
		ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

		_singleCpAttachCounterEnabled = true;	// in single cp system we increment the attach statistic
		// counter at the first attach and on any error
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("\n"
			"####################################################################################\n"
			"#        [TRANSACTION] COMMIT DONE. \n"
			"#                                   latest tx bno: %u\n"
			"#                                   latest committed bno : %u\n"
			"#                                   CP: %s\n"
			"#                                   TQ: %s\n"
			"####################################################################################\n",
			cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());

	ACS_RTR_LOG(LOG_LEVEL_INFO, "\n"
			"####################################################################################\n"
			"#        [TRANSACTION] COMMIT DONE. \n"
			"#                                   latest tx bno: %u\n"
			"#                                   latest committed bno : %u\n"
			"#                                   CP: %s\n"
			"#                                   TQ: %s\n"
			"####################################################################################\n",
			cBno, lastCommBlock, cpname.c_str(), _transferQueue.c_str());



	//calculate 'delta': the amount of transmitted blocks
	ACE_UINT32 delta = 0;
	ACE_UINT32 firstBlockNumber = pReporter->getFirstBlockNumber();
	if (lastCommBlock < firstBlockNumber)
	{
		ACE_UINT32 allrange = MAX_BLOCK_NUMBER - MIN_BLOCK_NUMBER;
		ACE_UINT32 diff = firstBlockNumber - cBno;
		delta = allrange - diff;
	}
	else delta = lastCommBlock - firstBlockNumber;

	//remove the committed blocks from cpQueue and commit towards ACA
	if (delta != 0)
	{
		if(delta > noOfBlocksPerTransaction)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] [TQ: %s, CP: %s] <%u> Blocks transmitted", _transferQueue.c_str(), cpname.c_str(), delta);
		}

		ULONGLONG nMessages = fxDeleteItemsFromCpQueue(cpQueue, delta);
		//commit towards aca
		if (nMessages > 0)
		{
			ACS_RTR_TRACE_MESSAGE("[TRANSACTION] [TQ: %s, CP: %s] %u Blocks have been transmitted. %llu ACA messages can be committed",
					_transferQueue.c_str(), cpname.c_str(), delta, nMessages);
			ACS_RTR_LOG(LOG_LEVEL_INFO, "[TRANSACTION] [TQ: %s, CP: %s] %u Blocks have been transmitted. %llu ACA messages can be committed",
					_transferQueue.c_str(), cpname.c_str(), delta, nMessages);

			bool acaCommitDone = false;
			int commitFailedCounter = 0;
			do
			{
				acaCommitDone = _mgrCallback->pfxCommitAcaMessages(nMessages, cpname);
				if (!acaCommitDone)
				{
					char trace[512] = {0};
					snprintf(trace, sizeof(trace) - 1,
							"[TQ: %s, CP: %s] %d ACA COMMIT FAILED. cannot commit %llu aca messages",
							_transferQueue.c_str(), cpname.c_str(), (++commitFailedCounter), nMessages);

					ACS_RTR_TRACE_MESSAGE("ERROR: [TRANSACTION] %s", trace);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "[TRANSACTION] %s", trace);

					if (!_gohStopRequested)
					{
						ACE_Time_Value commitTv(0,BREP_WAIT_ON_COMMIT_ERROR*1000);
						ACE_OS::sleep(commitTv);
					}
				}

			} while (!acaCommitDone && !_gohStopRequested);

			if (!acaCommitDone && _gohStopRequested)
			{
				ACE_TCHAR tracep[512] = {0};
				ACE_OS::snprintf(tracep, sizeof(tracep) - 1,
						"[TQ: %s, CP: %s] [TRANSACTION] ACA COMMIT FAILED. SHUTDOWN REQUESTED. Skip goh detach and file update.",
						_transferQueue.c_str(), cpname.c_str());

				ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
				return true;
			}
		}
	}

	//----------------------
	//detach transaction
	err = pReporter->detach();
	if (AES_OHI_NOERRORCODE != err)
	{
		char tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[TQ: %s, CP: %s] [TRANSACTION] DETACH ERROR! return value: %d", _transferQueue.c_str(), cpname.c_str(), err);
		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("[TQ: %s, CP: %s] [TRANSACTION] DETACH DONE", _transferQueue.c_str(), cpname.c_str());
	}
	pReporter->blockState = detached;

	//is the transmission failed?
	if (cBno != lastCommBlock)
	{

		if (pReporter->getStore())
		{
			pReporter->getStore()->deleteEntries(lastCommBlock); //refresh 'toCommit.dat'
		}

		ACE_TCHAR tracep[512] = {0};
		ACE_OS::snprintf(tracep, sizeof(tracep) - 1,
				"[TQ: %s, CP: %s] [TRANSACTION] ERROR: GOH BLOCK REPORTER IS NOT ALIGNED AFTER COMMIT. "
				"Last transmitted BlockNr by RTR: %u; Last received BlockNr by GOH: %u",
				_transferQueue.c_str(), cpname.c_str(), cBno, lastCommBlock);

		ACS_RTR_TRACE_MESSAGE("ERROR: %s", tracep);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);

		return false;
	}
	else
	{
		if (pReporter->getStore())
		{
			pReporter->getStore()->deleteAllEntries(); //refresh 'toCommit.dat'
		}

		ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
		return true;
	}

	ACS_RTR_TRACE_MESSAGE("OUT cp:<%s>, tq:<%s>", cpname.c_str(), _transferQueue.c_str());
	return true;
}

//------------------------------------------
// fxDeleteBlockReporter
// delete an RTRblockReporter pointer
//------------------------------------------
void ACS_RTR_GOHBlockReporter::fxDeleteBlockReporter(RTRblockReporter* bRep)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_TCHAR traceTime[256];//TIMESTAMP
	ACE_INT32 const dwBeginTime = GetTickCount();//TIMESTAMP
	ACE_INT32 dwConsumedTime = 0;//TIMESTAMP
	if (bRep)
	{
		if (bRep->blockState != detached)
		{
			ACE_UINT32 detachError = 0;
			for (ACE_INT32 retry = 0; (retry < BREP_MAX_RETRIES_ON_ERROR) && !_gohStopRequested && ( (detachError = bRep->detach())!=AES_OHI_NOERRORCODE ) ; retry++)
			{
				if(detachError == AES_OHI_INCORRECTCOMMAND)
				{
					break;
				}
				ACE_TCHAR tracep[512] = {0};
				switch(detachError)
				{
				case AES_OHI_ERRORUNKNOWN: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: AES_OHI_ERRORUNKNOWN", _transferQueue.c_str()); break;
				case AES_OHI_INCORRECTCOMMAND: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: AES_OHI_INCORRECTCOMMAND", _transferQueue.c_str()); break;
				case AES_OHI_NOPROCORDER: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: AES_OHI_NOPROCORDER", _transferQueue.c_str()); break;
				case AES_OHI_NOSERVERACCESS: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: AES_OHI_NOSERVERACCESS", _transferQueue.c_str()); break;
				case AES_OHI_TRANSACTIONACTIVE: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: AES_OHI_TRANSACTIONACTIVE", _transferQueue.c_str()); break;
				default: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s] detach return value: UNKNOWN!", _transferQueue.c_str());
				}

				ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				ACE_Time_Value detachTv(0,BREP_WAIT_ON_DETACH_ERROR*1000);
				ACE_OS::sleep(detachTv);
			}
		}
		delete bRep;
	}

	dwConsumedTime = GetTickCount() - dwBeginTime;//TIMESTAMP
	ACE_OS::snprintf(traceTime, sizeof(traceTime) - 1, "ConsumedTime: %u ms", dwConsumedTime);//TIMESTAMP

	ACS_RTR_TRACE_MESSAGE("Out - ConsumedTime: %u ms", dwConsumedTime);
}

//---------------------------
// setRtrrmEvent
// rtrrm command issued
//---------------------------
bool ACS_RTR_GOHBlockReporter::setRtrrmEvent() {
	ACS_RTR_TRACE_FUNCTION;
	bool result = true;
	return result;
}

//----------------------------------------------------------------------------------------------
//  fxDeleteItemsFromCpQueue
//  It deletes numOfItem packets from the CP QUEUE related to the ACA Channel.
//  This method returns the number of ACA Messages to commit.
//----------------------------------------------------------------------------------------------
//ULONGLONG ACS_RTR_GOHBlockReporter::fxDeleteItemsFromCpQueue(ACS_RTR_OutputQueue* cpQueue, int numOfItems) {
ULONGLONG ACS_RTR_GOHBlockReporter::fxDeleteItemsFromCpQueue(ACS_RTR_OutputQueue* cpQueue, ACE_UINT32 numOfItems) {
	ACS_RTR_TRACE_FUNCTION;

	ULONGLONG noOfMessages = 0;
	if (cpQueue != 0) noOfMessages = cpQueue->deleteItems(numOfItems);

	ACS_RTR_TRACE_MESSAGE("Out (num of items = %u, num of ACA Messages = %llu)", numOfItems, noOfMessages);
	return noOfMessages;
}

//----------------------------------------------------------------------------------------------
// getNotCommittedAcaMessages
// get the amount of not committed yet ACA messages related to the latest committed data block
//----------------------------------------------------------------------------------------------
bool ACS_RTR_GOHBlockReporter::getNotCommittedAcaMessages(std::string &cpname, unsigned int & nAcaMessages, short cpId)
{
	ACS_RTR_TRACE_FUNCTION;

	bool returnValue = false;

	int32_t err = AES_OHI_ERRORUNKNOWN;
	ACE_UINT32 lastCommittedBlock = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_tqCriticalSection);

	//create an OHI object
	RTRblockReporter * pReporter = fxCreateBlockReporter(cpname, cpId);
	if (pReporter)
	{
		ACS_RTR_BlockToCommitStore* commitFile = pReporter->getStore();
		if (commitFile && commitFile->dataExists())
		{
			//ATTACH
			if (pReporter->attachGOH(err))
			{
				//Get Last Committed Block Nr on Remote Destination
				if (pReporter->getLastBlockNumber(lastCommittedBlock, err))
				{
					// SUCCESS CASE: GET LAST SUCCEEDED
					returnValue = true;

					// GET ACA MESSAGES
					unsigned int blocksInTheMap = 0;
					nAcaMessages = commitFile->getAcaMessages(lastCommittedBlock, blocksInTheMap);
					ACS_RTR_LOG(LOG_LEVEL_INFO,
							"******************************************\n"
							"        [RESTART CHECK PHASE]\n"
							"        [TQ: %s, CP: %s]\n"
							"        Num of ACA Messages to be committed: %u\n"
							"        Last seqNo on BGW: %u\n"
							"        Number of blocks in the Map: %u\n"
							"*******************************************",
							_transferQueue.c_str(), cpname.c_str(), nAcaMessages, lastCommittedBlock, blocksInTheMap);

					commitFile->deleteAllEntries(); //clean up the file since the stored information is not needed anymore
					ACS_RTR_LOG(LOG_LEVEL_WARN, "[RESTART CHECK PHASE] [TQ: %s, CP: %s] Deleted all entries from internal toCommit.dat file", _transferQueue.c_str(), cpname.c_str());
				}
				else
				{
					// ERROR CASE: GET LAST FAILED
					returnValue = false;

					char tracep[512] = {0};
					snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] ERROR: getLastBlockNumber failed. Error code: %d", cpname.c_str(), _transferQueue.c_str(), err);
					ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
				}

				//Detach anyway
				{
					ACE_UINT32 detachError = 0;
					for (int retry = 0; (retry < 10) &&  !_gohStopRequested && ((detachError = pReporter->detach())!=AES_OHI_NOERRORCODE ) ; retry++)
					{
						if(detachError == AES_OHI_INCORRECTCOMMAND)
						{
							break;
						}

						ACE_TCHAR tracep[512] = {0};
						switch(detachError)
						{
							case AES_OHI_ERRORUNKNOWN: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: AES_OHI_ERRORUNKNOWN", cpname.c_str(), _transferQueue.c_str()); break;
							case AES_OHI_INCORRECTCOMMAND: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: AES_OHI_INCORRECTCOMMAND", cpname.c_str(), _transferQueue.c_str()); break;
							case AES_OHI_NOPROCORDER: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: AES_OHI_NOPROCORDER", cpname.c_str(), _transferQueue.c_str()); break;
							case AES_OHI_NOSERVERACCESS: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: AES_OHI_NOSERVERACCESS", cpname.c_str(), _transferQueue.c_str()); break;
							case AES_OHI_TRANSACTIONACTIVE: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: AES_OHI_TRANSACTIONACTIVE", cpname.c_str(), _transferQueue.c_str()); break;
							default: ACE_OS::snprintf(tracep, sizeof(tracep) - 1, "[%s/%s] detach return value: UNKNOWN!", cpname.c_str(), _transferQueue.c_str());
						}

						ACS_RTR_TRACE_MESSAGE("Error: %s", tracep);
						ACS_RTR_LOG(LOG_LEVEL_ERROR, "%s", tracep);
						ACE_Time_Value detachTv(0,BREP_WAIT_ON_DETACH_ERROR*1000);
						ACE_OS::sleep(detachTv);
					}
				}
			}
			else
			{
				// ERROR CASE: ATTACH FAILED
				returnValue = false;

				ACS_RTR_LOG(LOG_LEVEL_WARN, "[RESTART CHECK PHASE] [TQ: %s, CP: %s] ATTACH FAILED - TQ Error: %d", _transferQueue.c_str(), cpname.c_str(), err);
				ACS_RTR_TRACE_MESSAGE("[RESTART CHECK PHASE] [TQ: %s, CP: %s] ATTACH FAILED - TQ Error: %d", _transferQueue.c_str(), cpname.c_str(), err);
			}
		}
		else
		{
			// SUCCESS CASE: NO ACTIONS SINCE toCommit.DAT IS EMPTY
			returnValue = true;

			ACS_RTR_LOG(LOG_LEVEL_INFO, "[RESTART CHECK PHASE] [TQ: %s, CP: %s] NO DATA TO BE COMMITTED - toCommit.dat is empty", _transferQueue.c_str(), cpname.c_str());
			ACS_RTR_TRACE_MESSAGE("[RESTART CHECK PHASE] [TQ: %s, CP: %s] NO DATA TO BE COMMITTED - toCommit.dat is empty", _transferQueue.c_str(), cpname.c_str());
		}

		// delete the OHI object
		fxDeleteBlockReporter(pReporter);
	}

	return returnValue;
}

unsigned ACS_RTR_GOHBlockReporter::GetTickCount()
{
	// Get start time
	struct timespec time_;
	clock_gettime(CLOCK_MONOTONIC, &time_);
	long int timeMs = (time_.tv_sec * 1000) + (static_cast<long int>(time_.tv_nsec / 1000000));
	return (unsigned)timeMs;
}

ACE_UINT32 ACS_RTR_GOHBlockReporter::adaptiveCommitDelayEstimation(ACE_UINT32 lastSentBlock, ACE_UINT32 lastCommittedBlock, ACE_UINT32 timeDifference)
{
	ACS_RTR_TRACE_MESSAGE("IN");

	//Figure out the amount of block that BGW didn't receive yet
	ACE_UINT32 numBlocksToBeTransmitted = lastSentBlock - lastCommittedBlock;

	//If CDH returns an unreasonable great lastCommittedBlock value...
	if(numBlocksToBeTransmitted > noOfBlocksPerTransaction)
	{
		//...RTR will commit towards ACA all 100 blocks, so let's wait
		//enough time so that CDH sends all blocks to the Billing Gateway
		return 10000;
	}

	//Figure out the amount of block already received by BGW
	ACE_UINT32 numAckedBlocks = noOfBlocksPerTransaction - numBlocksToBeTransmitted;

	//Safe check to avoid division by '0'
	if( numAckedBlocks == 0 )
	{
		numAckedBlocks = 1;
	}

	// Transmission-per-block time estimation to round up
	// 		E.g 'timeDifference'/'numAckedBlocks' is round up according to this formula:
	//			 q = ('timeDifference' + 'numAckedBlocks' - 1)/'numAckedBlocks'

	// Figure out the time spent for the transmission of a single block
	ACE_UINT32 blockTransmissionTime = (timeDifference + numAckedBlocks - 1) / numAckedBlocks;

	//Return value in milliseconds
	ACS_RTR_TRACE_MESSAGE("OUT");
	return (blockTransmissionTime*numBlocksToBeTransmitted);
}
