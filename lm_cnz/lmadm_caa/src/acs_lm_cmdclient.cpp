//******************************************************************************
//
//  NAME
//     acs_lm_cmdclient.cpp
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_cmdclient.h"
#include "acs_lm_tra.h"
#include <ace/Time_Value.h>
#include <ace/Event.h>
/*=================================================================
	ROUTINE: ACS_LM_CmdClient constructor
=================================================================== */
ACS_LM_CmdClient::ACS_LM_CmdClient()
{
	DEBUG("ACS_LM_CmdClient::ACS_LM_CmdClient()  %s","Entering");
	theLMCmdClient = new ACS_DSD_Client();
	theLMCmdSession = new ACS_DSD_Session();
	DEBUG("ACS_LM_CmdClient::ACS_LM_CmdClient()  %s","Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_CmdClient destructor
=================================================================== */
ACS_LM_CmdClient::~ACS_LM_CmdClient()
{
	DEBUG("ACS_LM_CmdClient::~ACS_LM_CmdClient()  %s","Entering");
	delete theLMCmdClient;
	delete theLMCmdSession;
	DEBUG("ACS_LM_CmdClient::~ACS_LM_CmdClient()  %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: connect
=================================================================== */
bool ACS_LM_CmdClient::connect()
{
	DEBUG("ACS_LM_CmdClient::connect()  %s","connect");

	int retry = 5;
	while( retry--)
	{
		int ret = theLMCmdClient->connect(*theLMCmdSession, LM_CMD_DSD_SERVER_NAME, LM_CMD_DSD_SERVER_DOMAIN, 5000);
		if(ret < 0)
		{
			ERROR("ACS_LM_CmdClient::connect()  %s","Failure in connecting to  LM CMD server");
		}
		else
		{
			return true;
		}

	}
	DEBUG("ACS_LM_CmdClient::connect()  %s","Leaving");
	return(false);
}//end of connect
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_LM_CmdClient::send(const ACS_LM_Cmd& cmd)
{
	DEBUG("ACS_LM_CmdClient::send()  %s","Entering");
	bool sent = false;
	ACE_TCHAR* buffer = cmd.toBytes();
	ACE_UINT64 bytesToSend = cmd.size();
	ACE_UINT64 bytesSent = 0;

	if( theLMCmdSession != 0)
	{

		bytesSent = theLMCmdSession->send(buffer,bytesToSend);
		sent = (bytesToSend == bytesSent);

	}
	if (bytesSent <= 0)
	{
		DEBUG("ACS_LM_CmdClient::send()  %s","message not sent to LM CMD Server");
	}
	delete[] buffer;
	DEBUG("ACS_LM_CmdClient::send()  %s","Leaving");
	return sent;


}//end of send
/*=================================================================
	ROUTINE: receive
=================================================================== */
bool ACS_LM_CmdClient::receive(ACS_LM_Cmd& cmd)
{

	DEBUG("ACS_LM_CmdClient::receive()  %s","Entering");
	bool received = false;
	ACE_TCHAR myBuffer[512];
	ACE_UINT64 myBytesToReceive = 512; // Max response length
	ACE_UINT64 myBytesReceived = 0;
	myBytesReceived = theLMCmdSession->recv(myBuffer,myBytesToReceive);
	if (myBytesReceived > 0)
	{
		received = cmd.fromBytes(myBuffer);
	}
	DEBUG("ACS_LM_CmdClient::receive()  %s","Leaving");
	return received;
}//end of receive
/*=================================================================
	ROUTINE: disconnect
=================================================================== */
bool ACS_LM_CmdClient::disconnect()
{

	DEBUG("ACS_LM_CmdClient::disconnect()  %s","Entering");
	theLMCmdSession->close();
	DEBUG("ACS_LM_CmdClient::disconnect()  %s","Leaving");
	return true;
}//end of disconnect
