
//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_JtpMgr
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************
#include <sstream>
#include <errno.h>
#include <iostream>

#include "ace/Handle_Set.h"

#include "ACS_APSESH_hexdump.h"
#include "ACS_APSESH_JtpMgr.h"

#include "ace/OS_NS_poll.h"

namespace APSESH_JTP
{
	char ACS_APSESH_SRV_NAME[] = "ACS_APSESH";
	const int  TIMEOUT_WAIT = 9;
}

ACS_APSESH_JtpMgr::JtpMgrState::~JtpMgrState(){}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::JtpMgrState::getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::JtpMgrState::getData Entered");

   ACS_APSESH_JtpMgr::JtpMsg msg;

   ACS_APSESH_JtpReturnType returnVal = context.getMsgFromJtp(msg);

   if (returnVal.returnCode == ACS_APSESH_JtpReturnType::JTP_OK)
   {
      bufLen = msg.content.jexDataInd.bufLen;
      buffer = msg.content.jexDataInd.buffer;
   }

   return returnVal;
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::JtpMgrState::sendStatusCs (short status, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::JtpMgrState::sendStatusCs Entered");

   // Prepare the message.
   ACS_APSESH_JtpMgr::JtpMsg msg;
   msg.type = JEXDATAREQ;
   msg.content.jexDataReq.data1 = status;
   msg.content.jexDataReq.data2 = 0;
   msg.content.jexDataReq.bufLen = 0;
   msg.content.jexDataReq.buffer = NULL;

   return context.sendMsgToJtp(msg);
}

ACS_APSESH_JtpMgr::StateNoConn::~StateNoConn(){}


 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::StateNoConn::getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::StateNoConn::getData Entered");

   ACS_APSESH_JtpReturnType returnVal = context.connect();

   if (returnVal.returnCode != ACS_APSESH_JtpReturnType::JTP_OK)
   {
      // If no connection return here
      return returnVal;
   }

//   while(context.m_currentState == &context.m_stateNoConn){
//	   cout << "Waiting for JTP connection" << endl;
//	   sleep(1);
//   }

  // JTP is connected so move to wait state
   context.nextState(context.m_stateWaitData);

   returnVal = JtpMgrState::getData(buffer, bufLen, context);

   return returnVal;
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::StateNoConn::sendStatusCs (short status, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::StateNoConn::sendStatusCs Entered");

   ACS_APSESH_JtpReturnType returnVal = context.connect();

   if (returnVal.returnCode != ACS_APSESH_JtpReturnType::JTP_OK)
   {
      // If no connection return here
      return returnVal;
   }

   // JTP is connected so move to wait state
   context.nextState(context.m_stateWaitData);

   returnVal = JtpMgrState::sendStatusCs(status, context);

   return returnVal;
}

ACS_APSESH_JtpMgr::StateWaitData::~StateWaitData(){}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::StateWaitData::getData (char*& buffer, int& bufLen, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::StateWaitData::getData Entered");

   ACS_APSESH_JtpReturnType returnVal = JtpMgrState::getData(buffer, bufLen, context);

   if (returnVal.returnCode != ACS_APSESH_JtpReturnType::JTP_OK &&
      returnVal.returnCode != ACS_APSESH_JtpReturnType::JTP_NO_DATA	)
   {
      // getData returned some error. Move the state to NoConn
      context.nextState(context.m_stateNoConn);
   }

   return returnVal;
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::StateWaitData::sendStatusCs (short status, ACS_APSESH_JtpMgr& context)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "ACS_APSESH_JtpMgr::StateWaitData::sendStatusCs Entered");

   ACS_APSESH_JtpReturnType returnVal = JtpMgrState::sendStatusCs(status, context);

   if (returnVal.returnCode != ACS_APSESH_JtpReturnType::JTP_OK)
   {
      // sendStatusCs returned some error. Move the state to NoConn
      context.nextState(context.m_stateNoConn);
   }

   return returnVal;
}


// Class ACS_APSESH_JtpMgr 

//const int  ACS_APSESH_JtpMgr::TIMEOUT_WAIT = 10000;

const short  ACS_APSESH_JtpMgr::DEF_TRACE_MSG_SIZE = 512;

ACS_APSESH_JtpMgr::StateWaitData ACS_APSESH_JtpMgr::m_stateWaitData;
ACS_APSESH_JtpMgr::StateNoConn ACS_APSESH_JtpMgr::m_stateNoConn;

ACS_APSESH_JtpMgr::ACS_APSESH_JtpMgr ()
: m_published(false),
  m_jtpConv(),
  m_jtpSvc(0),
  m_jtpWaitTimeout(APSESH_JTP::TIMEOUT_WAIT),
  m_currentState(&m_stateNoConn)
{
}


ACS_APSESH_JtpMgr::~ACS_APSESH_JtpMgr()
{

}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::init ()
{
   //trace
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

   ACS_APSESH_JtpReturnType returnVal;

   m_jtpSvc = new (std::nothrow) ACS_JTP_Service(APSESH_JTP::ACS_APSESH_SRV_NAME);

   if(0 != m_jtpSvc)
   {
	   // publish APSESH server
	   m_published = m_jtpSvc->jidrepreq();
   }

   // Registers the application
   if(m_published)
   {
      returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;  // JTP registration has succeeded.
      APSESH_Tracer::instance()->trace(__FUNCTION__, "JTP server published.");
   }
   else
   {
      returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;  // JTP registration has failed, and connection cannot proceed.  tbd: add tracing here.
      returnVal.returnMsg = "JtpMgr::init: Failure to register application as a JTP service (jidrepreq).";
   }
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Leave");
   return returnVal;  
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::getData (char*& buffer, int& bufLen)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

   return m_currentState->getData(buffer, bufLen, *this);
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::sendStatusCs (short status)
{
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

   return m_currentState->sendStatusCs(status, *this);
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::connect()
 {

	APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

	ACS_APSESH_JtpReturnType returnVal;

	//int pollResult;
	int jtpHandleCount = 0;
	JTP_HANDLE* jtpHandles = NULL;

	APSESH_Tracer::instance()->trace(__FUNCTION__," get the number of jtp handles");

	// To get the number of handles
	m_jtpSvc->getHandles(jtpHandleCount, jtpHandles);

	char logMsg[DEF_TRACE_MSG_SIZE]={0};
	snprintf(logMsg, DEF_TRACE_MSG_SIZE-1, "retrieved <%i> jtp handles", jtpHandleCount);
	APSESH_Tracer::instance()->trace(__FUNCTION__, logMsg);

	// Now jtpHandleCount has the correct number of handles to retrieve
	jtpHandles = new JTP_HANDLE[jtpHandleCount];

	// get jtp handles
	m_jtpSvc->getHandles(jtpHandleCount, jtpHandles);

	// Create a fd to wait for request from JTP client
	const nfds_t nfds = jtpHandleCount;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	// Set JTP handles
	for(int handleIdx = 0; handleIdx < jtpHandleCount; ++handleIdx)
	{
		fds[handleIdx].fd = jtpHandles[handleIdx];
		fds[handleIdx].events = POLLIN;
	}

	// timeout for SESH connection
	ACE_Time_Value timeout;

	__time_t secs = APSESH_JTP::TIMEOUT_WAIT;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

	APSESH_Tracer::instance()->trace(__FUNCTION__, "start waiting for SESH connection");

	int ret = ACE_OS::poll(fds, nfds, &timeout);

	if (ret == 0)
	{
		// Timeout Expired

		{// trace
		  char msg[DEF_TRACE_MSG_SIZE] = {0};
		  snprintf(msg, sizeof(msg) - 1, "Timeout expired while waiting for SESH connection.");
		  APSESH_Tracer::instance()->trace(__FUNCTION__, msg);
		}

		returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_NO_CONNECTION;
		returnVal.returnMsg = "JtpMgr::connect: SESH is not connected.";
	}
	else if (ret > 0)
	{
		bool handleSignaled = false;
		// checks for jtp handle signal
		for(int handleIdx = 0; handleIdx < jtpHandleCount; ++handleIdx)
		{
			if(fds[handleIdx].revents & POLLIN)
			{
				handleSignaled = true;
				APSESH_Tracer::instance()->trace(__FUNCTION__,"SESH connection received");
				// Handling client connection
				// One of the JTP Service handles is signaled, which means that a client is trying to connect.
				if (m_jtpSvc->accept(NULL, &m_jtpConv)) // Attempt to accept a connection to a JTP conversation.
				{
					ushort u1, u2;
					ACS_JTP_Conversation::JTP_Node node;

					if (m_jtpConv.jexinitind(node, u1, u2) == false)
					{
						returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;
						returnVal.returnMsg = "JtpMgr::connect: jexinitind failed.";
					}
					else if (m_jtpConv.jexinitrsp(u1, u2, 0) == false)
					{
						returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;
						returnVal.returnMsg = "JtpMgr::connect: jexinitrsp failed.";
					}
					else
					{
						returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;
						returnVal.returnMsg = "JtpMgr::connect: Data received";
					}
				}
				else
				{
					returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;
					stringstream ss;
					ss << errno;
					returnVal.returnMsg = "JtpMgr::connect: Failure to accept JTP conversation, error code = " + ss.str();
				}
				break;

			}
		}

		// Check if a signaled handle has been found
		if(!handleSignaled)
		{
			returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED;
			returnVal.returnMsg = "JtpMgr::connect: Unexpected return value from synchronization operation.";
		}
	}
	else if (ret < 0)
	{
		// Poll error
		returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED;
		returnVal.returnMsg = "JtpMgr::connect: Unexpected return value from synchronization operation.";
	}

	APSESH_Tracer::instance()->trace(__FUNCTION__, returnVal.returnMsg.c_str());

	delete[] jtpHandles;

	return returnVal;
}

 ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::getMsgFromJtp(ACS_APSESH_JtpMgr::JtpMsg& msg)
 {
	//trace
	APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");
	ACS_APSESH_JtpReturnType returnVal;

	int jtpHandleCount = 1;
	JTP_HANDLE hJtpConv = m_jtpConv.getHandle();

	// Make sure our handles are still valid.
	if ( (ACE_INVALID_HANDLE == hJtpConv ) || (0 == hJtpConv ) )
	{
		returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED;
		returnVal.returnMsg = "JtpMgr::getMsgFromJtp: JTP conversation handle is NULL.";
		return returnVal;
	}

	// Create a fd to wait for request from JTP client
	const nfds_t nfds = jtpHandleCount;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0, sizeof(fds));

	fds[0].fd = hJtpConv;
	fds[0].events = POLLIN;

	// timeout on message receive
	ACE_Time_Value timeout;

	__time_t secs = m_jtpWaitTimeout;
	__suseconds_t usecs = 0;
	timeout.set(secs, usecs);

	APSESH_Tracer::instance()->trace(__FUNCTION__, "start waiting for SESH MESSAGE");

	int ret = ACE_OS::poll(fds, nfds, &timeout);

	if(ret < 0)
	{
		// poll failure
		char errorText[256] = {0};
		std::string errorDetail(strerror_r(errno, errorText, 255));
		returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;
		returnVal.returnMsg = "JtpMgr::getMsgFromJtp: I/O Failure in poll(). Error: " + errorDetail;
	}
	else if(ret > 0)
	{
		// jtp data ready
		APSESH_Tracer::instance()->trace(__FUNCTION__, "Select signaled: JTP Conversation ready...");

		unsigned short u1 = 0;
		unsigned short u2 = 0;
		unsigned short reason = 0;
		unsigned short bufLen = 0;
		char* buffer = NULL;

		if (m_jtpConv.jexdataind(u1, u2, bufLen, buffer) == true)
		{
			{// trace
				// Make this string large enough to contain the hexdump of the buffer sent from SESH.
				const int messageSize = 4096;
				char msg[messageSize] = { 0 };
				std::string bufferContent = "No Data";
				char* strDump = NULL; // No need to allocate memory; hexdump will do that, but this pointer should be initialized to NULL, to make sure that memory is allocated for it in hexdump().
				if (hexdump(strDump, buffer, bufLen)) // Get a hex dump of the data buffer for tracing.
				{
					bufferContent = strDump;
					free(strDump); // If hexdump() returns true, it means it has allocated a char array, which must now be freed.
				}
				snprintf(
						msg,
						sizeof(msg) - 1,
						"JEXDATAIND at %s %s\nu1 = %d\nu2 = %d\nbufLen = %d\nbuffer address = %p\nbuffer hexdump:\n%s\n",
						__DATE__, __TIME__, u1, u2, bufLen, buffer,
						bufferContent.c_str());
				APSESH_Tracer::instance()->trace(__FUNCTION__, msg);
			}

			if (bufLen != 0 && buffer != NULL)
			{
				msg.type = JEXDATAIND;
				msg.content.jexDataInd.data1 = u1;
				msg.content.jexDataInd.data2 = u2;
				msg.content.jexDataInd.bufLen = bufLen;
				msg.content.jexDataInd.buffer = buffer;

				returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;
			}
			else
			{
				returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;
				returnVal.returnMsg = "JtpMgr::getMsgFromJtp: No data returned from JEXDATAIND().";
			}
		}
		else if (m_jtpConv.jexdiscind(u1, u2, reason) == true)
		{
			char msg[128] = { 0 };
			snprintf( msg, 127,"! JTP disconnected with u1 (%d), u2 (%d), reason code (%d)", u1, u2, reason);

			APSESH_Tracer::instance()->trace(__FUNCTION__, msg);

			if (u1 == 1 && u2 == 0 && reason == 0)
			{
				returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_QUORUM_DISSOLVE_DISCONNECT;
				returnVal.returnMsg = "JtpMgr::getMsgFromJtp: JTP conversation has been disconnected due to Quorum dissolve in CP.";
				APSESH_Tracer::instance()->trace(__FUNCTION__, "Unexpected JTP operation", true, LOG_LEVEL_INFO);
			}
			else
			{
				returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_DISCONNECT;
				returnVal.returnMsg = "JtpMgr::getMsgFromJtp: JTP conversation has been disconnected.";
			}
		}
		else
		{
			returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED; // No other operations are valid on the APSESH side, so something has gone wrong.
			returnVal.returnMsg = "JtpMgr::getMsgFromJtp: Unexpected JTP operation.";
			APSESH_Tracer::instance()->trace(__FUNCTION__, "Unexpected JTP operation", true, LOG_LEVEL_ERROR);
		}
	}
	else
	{
		// Timeout expired
		APSESH_Tracer::instance()->trace(__FUNCTION__, "Select time out.");
		returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_NO_DATA;
		returnVal.returnMsg = "JtpMgr::getMsgFromJtp: No data from SESH.";
	}


	APSESH_Tracer::instance()->trace(__FUNCTION__, returnVal.returnMsg.c_str());
	return returnVal;
}

ACS_APSESH_JtpReturnType ACS_APSESH_JtpMgr::sendMsgToJtp (ACS_APSESH_JtpMgr::JtpMsg msg)
{
   //trace
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

   ACE_HANDLE hJtpConv = m_jtpConv.getHandle();
   ACS_APSESH_JtpReturnType returnVal;

   // Make sure our handle is still valid.

   if (hJtpConv == ACE_INVALID_HANDLE || hJtpConv == NULL)
   {
	  APSESH_Tracer::instance()->trace(__FUNCTION__, "JTP conversation handle is invalid.", true, LOG_LEVEL_ERROR);
	  returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED;
      returnVal.returnMsg = "JtpMgr::sendMsgToJtp: JTP conversation handle is NULL.";
   }

   if (msg.type == JEXDATAREQ)  // This happens when APSESH notifies SESH of its success or failure with sending data to CS.
   {
	   //msgToSend is needed for two reasons:
	   // 1) msg.content.jexDataReq.bufffer refers to JTP internal buffer
	   // 2) jexdatareq manipulates the input buffer: it copies the input buffer in its internal buffer
	   char msgToSend[msg.content.jexDataReq.bufLen];
	   memcpy(msgToSend, msg.content.jexDataReq.buffer, msg.content.jexDataReq.bufLen);

	   if (m_jtpConv.jexdatareq(
               msg.content.jexDataReq.data1, 
               msg.content.jexDataReq.data2, 
               msg.content.jexDataReq.bufLen, 
               msgToSend))
       {
		   APSESH_Tracer::instance()->trace(__FUNCTION__, "Successfully sent jexdatareq");
    	   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;
       }
       else
       {
    	   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL;  // The message could not be sent.
    	   stringstream ss;
           ss << errno;
           APSESH_Tracer::instance()->trace(__FUNCTION__, "Failed to send jexdatareq", true, LOG_LEVEL_ERROR);
           returnVal.returnMsg = "JtpMgr::sendMsgToJtp: Failure to send JTP message, error code = " + ss.str();
       }
   }
   else 
   {
	   APSESH_Tracer::instance()->trace(__FUNCTION__, "Unexpected JTP message type.");
	   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_FAIL_UNEXPECTED;  // No other operations are valid on the APSESH side, so something has gone wrong.
       returnVal.returnMsg = "JtpMgr::sendMsgToJtp: Unexpected JTP message type.";
   }

   return returnVal;
}

void ACS_APSESH_JtpMgr::nextState (ACS_APSESH_JtpMgr::JtpMgrState& state)
{
   //trace
   APSESH_Tracer::instance()->trace(__FUNCTION__, "Entered");

   m_currentState = &state;
}

void ACS_APSESH_JtpMgr::setJtpWaitTimeout (const unsigned int& timeout)
{
   m_jtpWaitTimeout = timeout;
}

void ACS_APSESH_JtpMgr::resetJtpWaitTimeout ()
{
   m_jtpWaitTimeout = APSESH_JTP::TIMEOUT_WAIT;
}

void ACS_APSESH_JtpMgr::unregister()
{
	if(0 != m_jtpSvc)
	{
		delete m_jtpSvc;
		m_jtpSvc = 0;
		m_published = false;
		APSESH_Tracer::instance()->trace(__FUNCTION__, "JTP server unregistered.");
	}
}
