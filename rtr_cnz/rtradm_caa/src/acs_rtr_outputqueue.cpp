//********************************************************************************
//
// NAME
// ACS_RTR_OutputQueue.cpp
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
// This class is responsible for the communication among the ACS_RTR_ACAChannel threads
// and the ACS_RTR_GOHReporter one.
//
//********************************************************************************

#include "acs_rtr_outputqueue.h"
#include "acs_rtr_global.h"
#include <sys/eventfd.h>

#include "acs_rtr_macros.h"
#include "acs_rtr_tracer.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_OutputQueue);

//----------------
// Constructor
//----------------
ACS_RTR_OutputQueue::ACS_RTR_OutputQueue(bool /*createEvent*/)
: _queueHandle(eventfd(0, 0)),
   maxSize(noOfBlocksPerTransaction * 2),
  _queueMutex(),
  _qInfo()
{

}

//---------------
// Destructor
//---------------
ACS_RTR_OutputQueue::~ACS_RTR_OutputQueue()
{
	{ // exclusive access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);
		while (!_qInfo.empty())
		{
			delete _qInfo.front();
			_qInfo.pop_front();
		}
	}

	if(ACE_INVALID_HANDLE  != _queueHandle)
	{
		ACE_OS::close(_queueHandle);
	}
}

//-----------
// getItem
//-----------
ACS_RTR_GohTransmitInfo* ACS_RTR_OutputQueue::getItem(int index)
{
	ACS_RTR_TRACE_MESSAGE(" at index:<%d>", index);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	std::list<ACS_RTR_GohTransmitInfo*>::const_iterator it = _qInfo.begin();
	while (index-- && (it != _qInfo.end())) ++it;

	return (it == _qInfo.end() ? reinterpret_cast<ACS_RTR_GohTransmitInfo *>(0) : *it);
}

//-----------------------------------------------------------------------------------------
// deleteItems
// delete numOfItems packets from the queue and return the number of
// aca messages stored in the blocks.
//-----------------------------------------------------------------------------------------
unsigned long long ACS_RTR_OutputQueue::deleteItems(ACE_UINT32 numOfItems)
{
	ACS_RTR_TRACE_MESSAGE(" remove:<%d> items", numOfItems);

	unsigned long long nAcaMessages = 0;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	int delCounter = 0;

	while((_qInfo.empty() == false) && (delCounter < numOfItems))
	{
			ACS_RTR_GohTransmitInfo* pkt = _qInfo.front();
			if(NULL == pkt) continue;

			RTRblock* block = pkt->getRTRBlock();
			if(NULL == block)	continue;

			if(block->getBlockNumber() == 0) break; //block not yet sent

			nAcaMessages += block->getNumberOfMessages();
			_qInfo.pop_front(); // remove packet from the queue
			delete pkt; //delete the packet
			delCounter++;
	}
	
	ACS_RTR_TRACE_MESSAGE(" nAcaMessages:<%lld> ", nAcaMessages);

	return nAcaMessages;
}

//--------------------------------------------
// deleteItemsByBno
// delete all the packets until lastBno
//--------------------------------------------
unsigned long long ACS_RTR_OutputQueue::deleteItemsByBno(unsigned lastBno)
{
	ACS_RTR_TRACE_MESSAGE("lastBno:<%d>", lastBno);

	unsigned long long nAcaMessages = 0;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	while(_qInfo.empty() == false)
	{
		ACS_RTR_GohTransmitInfo* pkt = _qInfo.front();
		if(NULL == pkt) continue;

		RTRblock* block = pkt->getRTRBlock();
		if(NULL == block) continue;

		if (block->getBlockNumber() == 0)
		{
			//block not yet sent
			break;
		}
		else if ((block->getBlockNumber() > lastBno) && ((block->getBlockNumber() - lastBno) <= noOfBlocksPerTransaction))
		{
			break;
		}

		nAcaMessages += block->getNumberOfMessages();
		//delete the packet and remove it from the queue
		_qInfo.pop_front();
		delete pkt;
	}

	ACS_RTR_TRACE_MESSAGE(" nAcaMessages:<%lld> ", nAcaMessages);

	return nAcaMessages;
}
//---------------------------------------------------------------------------
// PushData
//---------------------------------------------------------------------------
bool ACS_RTR_OutputQueue::pushData(ACS_RTR_GohTransmitInfo* info)
{
	ACS_RTR_TRACE_MESSAGE("IN");
	bool result = false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	if( (NULL != info) && (info->getRTRBlock() || info->getRTRFile()) )
	{
		if (_qInfo.size() <= maxSize)
		{
			_qInfo.push_back(info);
			if((_qInfo.size() == 1) && (ACE_INVALID_HANDLE != _queueHandle) )
			{
				eventfd_t dataReady = 1U;
				if(eventfd_write(_queueHandle, dataReady) != 0)
				{
					ACS_RTR_TRACE_MESSAGE("Failed to signal _queueHandle Event error:<%d>", errno);
				}
			}
			result = true;
		}
	}

	ACS_RTR_TRACE_MESSAGE("OUT, result:<%s>", (result ? "TRUE" : "FALSE"));
	return result;
}

//---------------------------------------------------------------------------------
// Push CP ID of the ACAChannel containing the block to transmit
//---------------------------------------------------------------------------------
void ACS_RTR_OutputQueue::pushCpIdInfo(ACS_RTR_GohTransmitInfo* info)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	short cpId = info->getCpID();
	ACS_RTR_TRACE_MESSAGE(" add CPid:<%d>", cpId);

	for(std::list<ACS_RTR_GohTransmitInfo*>::const_iterator it = _qInfo.begin(); it != _qInfo.end(); ++it)
	{
		ACS_RTR_GohTransmitInfo* pkt = (*it);
		if(pkt)
		{
			if (pkt->getCpID() == cpId)
			{
				ACS_RTR_TRACE_MESSAGE(" CPid:<%d> already present", cpId);
				//info already exists, delete it and return
				delete info;
				return;
			}
		}
	}

	_qInfo.push_back(info);

	if( (_qInfo.size() == 1) && (ACE_INVALID_HANDLE != _queueHandle) )
	{
		ACS_RTR_TRACE_MESSAGE(" First CPid:<%d> added signal _queueHandle", cpId);
		eventfd_t cpInfoReady = 1U;
		if(eventfd_write(_queueHandle, cpInfoReady) != 0)
		{
			ACS_RTR_TRACE_MESSAGE("Failed to signal _queueHandle Event error:<%d>", errno);
		}
	}
}

//----------------------
// removeCpIdInfo
//----------------------
void ACS_RTR_OutputQueue::removeCpIdInfo(short id)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	std::list<ACS_RTR_GohTransmitInfo*>::iterator it;
	ACS_RTR_TRACE_MESSAGE(" remove CPid:<%d>", id);

	bool found = false;

	for (it = _qInfo.begin(); it != _qInfo.end(); ++it)
	{
		ACS_RTR_GohTransmitInfo* pkt = (*it);
		if (pkt)
		{
			if (pkt->getCpID() == id)
			{
				delete pkt;
				found = true;
				break;
			}
		}
	}

	if (found)
	{
		_qInfo.erase(it);
	}

	if( (_qInfo.size() == 0) && (_queueHandle != ACE_INVALID_HANDLE) )
	{
		//read the handle in order to reset it
		eventfd_t value;
		eventfd_read(_queueHandle, &value);
	}
}

//--------------------------------------------
// getFirst
// Get the first element in the queue
//--------------------------------------------
ACS_RTR_GohTransmitInfo* ACS_RTR_OutputQueue::getFirst()
{
	ACS_RTR_GohTransmitInfo * getFirst = NULL;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	getFirst = (_qInfo.empty()) ? reinterpret_cast<ACS_RTR_GohTransmitInfo *>(0) : _qInfo.front();

	return getFirst;
}

//-----------
// popData
//-----------
void ACS_RTR_OutputQueue::popData()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);

	if (!_qInfo.empty())
	{
		_qInfo.pop_front();

		if( (_qInfo.size() == 0) && (_queueHandle != ACE_INVALID_HANDLE) )
		{
			//read the handle in order to reset it
			eventfd_t value;
			eventfd_read(_queueHandle, &value);
		}
	}
}

//----------------
// queueEmpty
//----------------
bool ACS_RTR_OutputQueue::queueEmpty()
{
	bool Info=false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);
	Info =_qInfo.empty();
	return Info;
}

//---------------
// queueSize
//---------------
size_t ACS_RTR_OutputQueue::queueSize()
{
	size_t  sizeInfo;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_queueMutex);
	sizeInfo =_qInfo.size();
	return sizeInfo;
}
