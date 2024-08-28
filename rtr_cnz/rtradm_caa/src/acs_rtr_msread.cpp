//******************************************************************************
//
// NAME
// acs_rtr_msread.cpp
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

// DOCUMENT NO

// AUTHOR 

// DESCRIPTION
// This class derives the Message store API (ACS_ACA_MessageStoreAccessPoint)
// and is responsible for retrieving data from the Message store.
//
//  CHANGES
//    RELEASE REVISION HISTORY
//    DATE          NAME       DESCRIPTION
//    2013-02-12    xsamech    Implementation for APG43L
//*************************************************************************
#include "acs_rtr_msread.h"
#include "acs_rtr_macros.h"
#include <stdlib.h>
#include "acs_aca_defs.h"
#include "acs_rtr_global.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_ACAMSread)

//*************************************************************************
// Constructor
//*************************************************************************
MSread::MSread(const char* MSname,unsigned cpSystemId)
: ACS_ACA_MessageStoreClient(MSname, cpSystemId),
  MSconnection(false),
  _msSync()
{
	ACS_RTR_TRACE_MESSAGE("In");
}

//*************************************************************************
// Constructor
//*************************************************************************
MSread::MSread(const char* MSname, const char* CPsite)
: ACS_ACA_MessageStoreClient(MSname, CPsite),
  MSconnection(false),
  _msSync()
{
	ACS_RTR_TRACE_MESSAGE("In");
}

//*************************************************************************
// Destructor
//*************************************************************************
MSread::~MSread() {
	ACS_RTR_TRACE_MESSAGE("In");
}

//*************************************************************************
// getLastCommittedRTRfileNumber
//
// Retrieve the last committed RTR file identity from ACA and extract
// the file number part.
//
//  return value:
//    true : a valid file number returned.
//    false: corrupt file name identity.
//*************************************************************************
bool MSread::getLastCommittedRTRfileNumber(unsigned int& lcf)
{
	ACS_RTR_TRACE_MESSAGE("In (lcf == %u)", lcf);
	bool ret = false;
	unsigned char* tbuf = 0;
	unsigned char fname[128] = {0};
	unsigned char tmp[16] = {0};
	char* fptr = 0;
	int tlength = 0;
	lcf = 0;
	const int FILENUMBERPOS = 14;

	if (getLastTransactionId(tbuf,tlength)) {		
		if (tlength == 0) {
			ret = true; // startup.
		} else if ( (fptr = strstr((char*)tbuf,"RTR")) == 0 ) { // For TR HG35670 added by XCSVAAN
			// When switched from Block Transfer to File Transfer
			ret = true;
		} else if ( (fptr = strstr((char*)tbuf,"RTR")) != 0 ) {
			memcpy(fname,tbuf,tlength);
			memcpy(tmp,&fname[FILENUMBERPOS],4);
			lcf = atoi((char*)tmp);
			if ( (lcf>0) && (lcf<=9999) ) {
				ret = true;
			} else { // Bad information stored in ACA commit file
				lcf = 0;
				ret = true;
			}
		}
	}

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (ret ? "true" : "false"));
	return ret;
}

//*************************************************************************
// getLastBlockNumberFromMS
//
// Retrieve the last committed block number from ACA.
//
//  return value:
//    true : a valid block number returned.
//    false: couln't get the number.
//*************************************************************************
bool MSread::getLastBlockNumberFromMS(unsigned int& bno)
{
	ACS_RTR_TRACE_MESSAGE("In (bno == %u)", bno);
	bool ret = true;
	unsigned char* trData = 0;
	int trLn = 0;
	bno = 0;

	if (!getLastTransactionId(trData,trLn)) {
		ret=false;
	}
	else if ( trLn && (strlen((char*)trData) > 0) ) {
		// For TR HG35670 added by XCSVAAN
		if( (strstr((char*)trData,"RTR")) == 0 )
			bno = (unsigned int)atoi((char*)trData);
	}

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (ret ? "true" : "false"));
	return ret;
}

//*************************************************************************
// getMSbuf
//
// Fetch data from message store, i.e an MS-buffer. 
//
//  return value:
//    true : the reading went well, msgState hold the state of the message.
//    false: reading failed.
//*************************************************************************
bool MSread::getMSbuf(ACS_ACA_MessageStoreMessage::State& msgState, unsigned int& bufln, unsigned char*& buf, bool extraDataReq, int& additionalData)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool ret = true;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_msSync);

	unsigned int bufferInLength = bufln;
	bufln = 0;
	additionalData = 0;
	ACS_ACA_MessageStoreMessage msMessage;

	// Retrieve a message buffer.
	if(extraDataReq)
	{
		int versionNumber = 0;
		ret = getMessageAndRevisionInfo(msMessage, additionalData, versionNumber);
	}
	else
	{
		ret = getMessage(msMessage);
	}

	// Check ACA result
	if(ret)
	{
		msgState = msMessage.getState();

		switch(msgState)
		{
			case ACS_ACA_MessageStoreMessage::Empty:
			{
				//ACS_RTR_TRACE_MESSAGE("[MESSAGE EMPTY] Id: %u", msMessage.getMessageId());
				//ACS_RTR_LOG(LOG_LEVEL_DEBUG, "[MESSAGE EMPTY] Id: %u", msMessage.getMessageId());
			}
				break; // Message store is empty, i.e everything committed.

			case ACS_ACA_MessageStoreMessage::Skipped:
			{
				ACS_RTR_TRACE_MESSAGE("ERROR: [MESSAGE SKIPPED] Id: %llu", msMessage.getMessageId());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[MESSAGE SKIPPED] Id: %llu", msMessage.getMessageId());
			}
			break;

			case ACS_ACA_MessageStoreMessage::Lost:
			{
				ACS_RTR_TRACE_MESSAGE("ERROR: [MESSAGE LOST] Id: %llu", msMessage.getMessageId());
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[MESSAGE LOST] Id: %llu", msMessage.getMessageId());
			}
			break;

			case ACS_ACA_MessageStoreMessage::Filled:
			{
				// Message ready
				bufln = msMessage.getDataLength();

				if (bufln > bufferInLength )
				{
					ACS_RTR_LOG(LOG_LEVEL_ERROR, "ERROR: MTAP Record Length:<%d> bigger than Input RTR Buffer Length:<%d>", bufln, bufferInLength);
				}

				memcpy(buf, msMessage.getData(), (bufferInLength >= bufln ? bufln : bufferInLength));
			}
			break;

			default:
			{
				ACS_RTR_LOG(LOG_LEVEL_ERROR, "[MESSAGE UNKNOW] Id: %llu", msMessage.getMessageId());
				ret = false; // should never happen.
			}
		}
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ACA getMessage() Failed!");
	}
	ACS_RTR_TRACE_MESSAGE("Out (%s)", (ret ? "true" : "false"));
	return ret;
}


//*************************************************************************
// alignMessageStore
//
// Move the MS-read pointer the indicated number of filled messages
// forward. The only purpose is to move the pointer, the actual data
// content is of no interest. Then commit message store with the
// indicated block number.
//
//  return value:
//    true : ok
//    false: move failed.
//*************************************************************************
bool MSread::alignMessageStore(unsigned int blockNo,
		unsigned int noOfMess,
		unsigned int& alignedSize)
{
	ACS_RTR_TRACE_MESSAGE("In (blockNo == %u, noOfMess == %u, alignedSize == %u)", blockNo, noOfMess, alignedSize);
	ACS_ACA_MessageStoreMessage msMessage;
	unsigned int dummyLn=0;
	unsigned char dummyBuf[64000] = {0};
	bool ret = false;
	if (noOfMess > 0)
	{
		while (noOfMess)
		{
			// Retrieve a message buffer.
			if (!getMessage(msMessage))
			{
				break;
			}

			if (msMessage.getState()==ACS_ACA_MessageStoreMessage::Filled)
			{
				noOfMess--;
				dummyLn = msMessage.getDataLength();
				alignedSize = alignedSize+dummyLn;
				memcpy(dummyBuf, msMessage.getData(), ((dummyLn > sizeof (dummyBuf)) ? sizeof (dummyBuf) : dummyLn));
			}
		}

		if (noOfMess <= 0) ret = commitBlockTransfer(blockNo);
	}

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (ret ? "true" : "false"));
	return ret;
}


//*************************************************************************
// commitBlockTransfer
//
// Block transfer mode of RTR only. The block number should be stored
// in ACA commit file.
//
//  return value:
//    true : ok
//    false: move failed.
//*************************************************************************
bool MSread::commitBlockTransfer(unsigned int bno)
{
	ACS_RTR_TRACE_MESSAGE("In (bno == %u)", bno);
	bool ret = false;
	unsigned char trData[64] = {0};
	sprintf(( char *)trData,"%d",bno);

	if (commit(trData,strlen((char*)trData)+1))
	{
		ret = true;
	}
	ACS_RTR_TRACE_MESSAGE("Out (%s)", (ret ? "true" : "false"));
	return ret;
}


//*************************************************************************
// MSconnect
//
// Connect to Message store. 
//
//  return value:
//    true : RTR is connected to MS.
//    false: connection not granted.
//*************************************************************************
bool MSread::MSconnect(void)
{
	ACS_RTR_TRACE_MESSAGE("In");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_msSync);
	bool success = false;
	try
	{
		success = this->connect();

		//log error
		if (!success) ACS_RTR_LOG(LOG_LEVEL_ERROR, "CONNECT ERROR: %s", getLastErrorName().c_str());
	}
	catch(...)
	{
		success = false;
	}
	if (success)
	{
		MSconnection=true;
	}
	else MSdisconnect();

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (MSconnection ? "true" : "false"));
	return MSconnection;
}

//*************************************************************************
// MSdisconnect
//
// Disconnect from Message store. 
//
//  return value:
//   -
//*************************************************************************
void MSread::MSdisconnect(void)
{
	ACS_RTR_TRACE_MESSAGE("In");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_msSync);

	disconnect();
	MSconnection = false;

	ACS_RTR_TRACE_MESSAGE("Out");
}

bool MSread::disconnectOnShutdown()
{
	ACS_RTR_TRACE_MESSAGE("In");

	bool success = true;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_msSync);
	ACE_Time_Value safeSleep(0,100000);
	if (MSconnection) {
		for (int retry = 0; (retry < 5) && !(success = this->disconnect()); retry++) ACE_OS::sleep(safeSleep);
		if (success) 
		{
			MSconnection = false;
		}
		else MSconnection = true;
	}
	ACS_RTR_TRACE_MESSAGE("Out (%s)", (success ? "true" : "false"));
	return success;
}

//*************************************************************************
// connectedToMSD
//
// Is RTR connected to MS or not?
//
//  return value:
//    true : RTR is connected to MS.
//    false: RTR is NOT connected to MS..
//*************************************************************************
bool MSread::connectedToMSD(void)
{
	ACS_RTR_TRACE_MESSAGE("In");
	bool connected = false;
	if (getState() == ACS_ACA_MessageStoreAccessPoint::Connected) connected = true;
	if (MSconnection != connected) MSdisconnect();

	ACS_RTR_TRACE_MESSAGE("Out (%s)", (MSconnection ? "true" : "false"));
	return MSconnection;
}
