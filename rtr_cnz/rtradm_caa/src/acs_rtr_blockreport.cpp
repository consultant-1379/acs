//******************************************************************************
//
// NAME
// acs_rtr_blockReport.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2012.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// DOCUMENT NO
//
// AUTHOR 
// 2012-12-12 by XSAMECH
// CHANGE HISTORY
// DESCRIPTION
// This class encapsulates the block transfer API.
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
// 2012-12-12	XSAMECH			First Release
//******************************************************************************

#include "acs_rtr_blockreport.h"
#include "acs_rtr_global.h"
#include <ace/OS_NS_poll.h>
#include "ACS_TRA_trace.h"
#include <cstdio>
#include <iostream>

#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

using namespace std;

ACS_RTR_TRACE_DEFINE(ACS_RTR_BLOCK_REP)

//------------
// Constructor
//------------
RTRblockReporter::RTRblockReporter(const char * subSys, const char * appName,
		const char * blockTQ, const char * evtext, const char * streamId)
: blockState(detached), lastCommittedBlock(0), currentBlock(0),
  firstBlockNumber(0),  maxNumOfBlocksInTransaction(MAX_BLOCK_TRANSACTION),
  _store(0),_blockHandler(0)
{
	ACS_RTR_TRACE_FUNCTION;

	_blockHandler = new AES_OHI_BlockHandler2(subSys, appName, blockTQ, evtext, streamId);
}

//-----------
// Destructor
//-----------
RTRblockReporter::~RTRblockReporter()
{
	ACS_RTR_TRACE_FUNCTION;
	delete _blockHandler;
}

//-----------------
// isEndTransaction
//-----------------
bool RTRblockReporter::isEndTransaction()
{
	ACS_RTR_TRACE_FUNCTION;
	if (currentBlock == firstBlockNumber)
		return false;
	else if (currentBlock > firstBlockNumber)
	{
		if ((currentBlock - firstBlockNumber) >= maxNumOfBlocksInTransaction)
			return true;
		else
			return false;
	}
	else 
	{
		unsigned int allrange = MAX_BLOCK_NUMBER - MIN_BLOCK_NUMBER;
		unsigned int delta = firstBlockNumber - currentBlock;
		if ((allrange - delta) >= maxNumOfBlocksInTransaction)
			return true;
		else
			return false;
	}
}

//------------------------------------------------------------
// attachGOH

// Attach to the block destination.

//  return value:
//    true : ok.
//    false: something went wrong, err contains a fault value.
//------------------------------------------------------------
bool RTRblockReporter::attachGOH(ACE_INT32& err)
{
	ACS_RTR_TRACE_FUNCTION;
	//if already attached return true
	if (blockState != detached) 
	{
		return true;
	}

	err = _blockHandler->attach();
	if ((err == AES_OHI_NOERRORCODE) || (err == AES_OHI_INCORRECTCOMMAND ))
	{
		ACS_RTR_TRACE_MESSAGE(" Attach GOH successfull");
		blockState = attached;
		return true;
	}
	else
	{
		ACS_RTR_TRACE_MESSAGE("Error: ATTACH failed with error: %d", err);
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ATTACH failed with error: %d", err);

		unsigned int detachError = _blockHandler->detach();
		if ((AES_OHI_NOERRORCODE == detachError) || (AES_OHI_INCORRECTCOMMAND == detachError))
		{
			ACS_RTR_TRACE_MESSAGE("DETACH failed with error: %d", detachError);
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "DETACH failed with error: %d", detachError);
		}

		return false;
	}
}

//--------------------------------------------------------------
// getLastBlockNumber

// Obtain the last committed block number identity from GOH/BGW.

//  return value:
//    true : ok.
//    false: something went wrong, err contains a fault value.
//--------------------------------------------------------------
bool RTRblockReporter::getLastBlockNumber(unsigned int& lastCommBlock, ACE_INT32& err)
{
	ACS_RTR_TRACE_FUNCTION;
	err = _blockHandler->getLastCommitedBlockNo(lastCommBlock);
	switch(err)
	{

	case AES_OHI_NOERRORCODE:

		lastCommittedBlock = lastCommBlock;
		firstBlockNumber = lastCommBlock;
		return true;

	case AES_OHI_BLOCKNRNOTAVAILABLE:

		lastCommBlock = 0;
		lastCommittedBlock = lastCommBlock;
		firstBlockNumber = lastCommBlock;
		return true;

	case AES_OHI_TRANSACTIONACTIVE:
	{
		unsigned int terminateError = 0;
		for (int retry = 0; (retry < maxRetriesOnError) && ( (terminateError = _blockHandler->transactionTerminate())!=AES_OHI_NOERRORCODE ); retry++)
		{
			ACE_Time_Value terminateFailedWaitTv(0,terminateFailedWait*1000);
			ACE_OS::sleep(terminateFailedWaitTv);
		}//for
	}
	case AES_OHI_INCORRECTCOMMAND:
	case AES_OHI_CONNECTERROR:
	case AES_OHI_NOCDHSERVER:
	case AES_OHI_NOSERVERACCESS:
	default:
	{
		unsigned int detachError = 0;
		for (int retry = 0; (retry < maxRetriesOnError) && ( (detachError = _blockHandler->detach())!=AES_OHI_NOERRORCODE ); retry++)
		{
			if(detachError == AES_OHI_INCORRECTCOMMAND)
			{
				break;
			}
			ACE_Time_Value detachFailedWaitTv(0,detachFailedWait*1000);
			ACE_OS::sleep(detachFailedWaitTv);
		}
	}
	}
	return false;
}

//-------------------------------------------------------------------------
// transActionReq

// Handles the 'transactionBegin', 'transactionEnd' and 'transactionCommit'
// API function calls.

//  return value:
//    true : ok.
//    false: something went wrong, err contains a fault value.
//-------------------------------------------------------------------------
bool RTRblockReporter::transActionReq(trReq ttype, unsigned int& blNo, ACE_INT32& err)
{
	ACS_RTR_TRACE_FUNCTION;
	blNo = 0;
	if (ttype == BEGIN)
	{
		if (blockState == attached)
		{
			err = _blockHandler->transactionBegin();
			switch(err)
			{
			case AES_OHI_NOERRORCODE:
				currentBlock = lastCommittedBlock;
				blNo = currentBlock;
				blockState = sending;
				return true;
			case AES_OHI_TRANSACTIONACTIVE:
			case AES_OHI_COMMITNOTPERFORMED:
			case AES_OHI_INCORRECTCOMMAND:
			case AES_OHI_NOCDHSERVER:
			case AES_OHI_NOSERVERACCESS:
			default:
			{//terminate
				unsigned int terminateError = 0;
				for (int retry = 0; (retry < maxRetriesOnError) && ( (terminateError = _blockHandler->transactionTerminate())!=AES_OHI_NOERRORCODE ); retry++) {
					ACE_Time_Value terminateFailedWaitTv(0,terminateFailedWait*1000);
					ACE_OS::sleep(terminateFailedWaitTv);
				}//for
			}
			{//detach
				unsigned int detachError = 0;
				for (int retry = 0; (retry < maxRetriesOnError) && ( (detachError = _blockHandler->detach())!=AES_OHI_NOERRORCODE ) ; retry++) {
					if(detachError == AES_OHI_INCORRECTCOMMAND)
					{
						break;
					}
					ACE_Time_Value detachFailedWaitTv(0,detachFailedWait*1000);
					ACE_OS::sleep(detachFailedWaitTv);
				}
			}
			return false;
			}
		}
		else if (blockState == sending) 
		{
			blNo = currentBlock;
			return true;
		}
		else
			return false;
	}
	else if (ttype == END)
	{
		if (blockState != sending) return false;
		err = _blockHandler->transactionEnd(blNo);
		switch(err) {
		case AES_OHI_NOERRORCODE:
		{
			ACS_RTR_TRACE_MESSAGE(" transactionEnd() returns block num:<%u>", blNo);
		}
		return true;
		case AES_OHI_CONNECTERROR:
		case AES_OHI_NOCDHSERVER:
		case AES_OHI_NOSERVERACCESS:
		case AES_OHI_TRANSACTIONNOTACTIVE:
		default:
		{//terminate
			unsigned int terminateError = 0;
			for (int retry = 0; (retry < maxRetriesOnError) && ( (terminateError = _blockHandler->transactionTerminate())!=AES_OHI_NOERRORCODE ); retry++)
			{
				ACE_Time_Value  terminateFailedWaitTv(0,terminateFailedWait*1000);
				ACE_OS::sleep(terminateFailedWaitTv);
			}//for
		}
		{//detach
			unsigned int detachError = 0;
			for (int retry = 0; (retry < maxRetriesOnError) && ( (detachError = _blockHandler->detach())!=AES_OHI_NOERRORCODE ); retry++)
			{
				if(detachError == AES_OHI_INCORRECTCOMMAND)
				{
					break;
				}
				ACE_Time_Value detachFailedWaitTv(0,detachFailedWait*1000);
				ACE_OS::sleep(detachFailedWaitTv);
			}
		}
		return false;
		}
	}
	else if (ttype == COMMIT)
	{
		err = _blockHandler->transactionCommit(blNo);
		switch(err)
		{
		case AES_OHI_NOERRORCODE:
			ACS_RTR_TRACE_MESSAGE(" transactionCommit() returns block num:<%u>", blNo);
			blockState = attached;
			currentBlock = lastCommittedBlock = blNo;
			return true;
		case AES_OHI_BLOCKNRNOTAVAILABLE:
		case AES_OHI_CONNECTERROR:
		case AES_OHI_NOCDHSERVER:
		case AES_OHI_NOSERVERACCESS:
		case AES_OHI_NOTRANSACTIONPERFORMED:
		default:
			unsigned int detachError = 0;
			for (int retry = 0; (retry < maxRetriesOnError) && ( (detachError = _blockHandler->detach())!=AES_OHI_NOERRORCODE ); retry++)
			{
				if(detachError == AES_OHI_INCORRECTCOMMAND)
				{
					break;
				}
				ACE_Time_Value detachFailedWaitTv(0,detachFailedWait*1000);
				ACE_OS::sleep(detachFailedWaitTv);
			}
		}
		return false;
	} else {
		return false;
	}
}


//--------
// sendGOH
//--------
bool RTRblockReporter::sendGOH(const char* buf, const int bufln, unsigned int& blockNo, ACE_INT32& err)
{
	bool retValue = true;
	ACS_RTR_TRACE_FUNCTION;
	if ((++currentBlock) > MAX_BLOCK_NUMBER) currentBlock = MIN_BLOCK_NUMBER;

	blockNo = currentBlock;
	ACE_Time_Value tv(BUFFER_FULL_WAIT);
	for (int c = 0; (c < maxRetriesOnBufferFull) && ((err = _blockHandler->send(buf, bufln, blockNo)) == AES_OHI_BUFFERFULL) ; c++) ACE_OS::sleep(tv);

	if (AES_OHI_NOERRORCODE == err)
	{
		currentBlock = blockNo;
		retValue = true;
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "_blockHandler->send failed - GOH Error: %u", err);
		ACS_RTR_TRACE_MESSAGE("Error: _blockHandler->send failed - GOH Error: %u", err);
		retValue = false;
	}

	return retValue;
}

//*************************************************************************
// HandleGOHevent

// Function for receiving events from GOH.
//  return value:
//    true : ok.
//    false: something went wrong.
//*************************************************************************
void RTRblockReporter::HandleGOHevent(void)
{
	ACS_RTR_TRACE_FUNCTION;
	// events should not be handled.
	// 2005-05-11
	return;
}

//*************************************************************************
// handleTransferFailed

// handle a transfer failed event
//  return value:
//    true : event handled
//    false: event not handled
//*************************************************************************
bool RTRblockReporter::handleTransferFailed()
{
	ACS_RTR_TRACE_FUNCTION;
	bool ret = false;
	AES_OHI_Eventcodes eventCode;
	ACE_Semaphore * h = NULL;
	if(!h)
	{
		struct pollfd* blockH;
		 memset(blockH,0,sizeof(struct pollfd));
		blockH->fd=*(ACE_HANDLE *)h;
		blockH->events = POLLIN | POLLRDHUP | POLLHUP | POLLERR;
		blockH->revents = 0;

		ACE_INT32 wRes = ACE_OS::poll(blockH,1, 0);

		if (wRes == 1 )
		{
			if (blockH->revents == POLLIN)
			{
				unsigned int res = _blockHandler->getEvent(eventCode);
				if (res != AES_OHI_NOERRORCODE)
				{
					if (eventCode == AES_OHI_EVEBLOCKTRANSFERFAILED)
					{
						_blockHandler->transactionTerminate();
						_blockHandler->detach();
						blockState = detached;
						ret = true;
					}
				}
			}
		}
	}
	return ret;
}

//-------
// detach
//-------
unsigned int RTRblockReporter::detach()
{
	ACS_RTR_TRACE_FUNCTION;
	if (_blockHandler) 
	{
		unsigned int test = _blockHandler->detach();
		return test;
	}
	else 
	{
		return AES_OHI_ERRORUNKNOWN;
	}
}

//---------------------
// transactionTerminate
//---------------------
unsigned int RTRblockReporter::transactionTerminate()
{
	ACS_RTR_TRACE_FUNCTION;
	if (_blockHandler)
	{
		return _blockHandler->transactionTerminate();
	}
	else return AES_OHI_ERRORUNKNOWN;
}
