//******************************************************************************
//
//  NAME
//     acs_lm_cmdserver.cpp
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
#include "acs_lm_cmdserver.h"
#include "acs_lm_cmd.h"
#include "acs_lm_tra.h"
#include <ace/Time_Value.h>

bool ACS_LM_CmdServer::theDSDServerStopSignal = false;
ACE_Recursive_Thread_Mutex ACS_LM_CmdServer::theDSDServerMutex;
/*=================================================================
	ROUTINE: ACS_LM_CmdServer constructor
=================================================================== */
ACS_LM_CmdServer::ACS_LM_CmdServer()
:clientConnectEvent(NULL),
 theLMCmdServer(0),theDSDSession(0)
//, forceClose(false)
{
	DEBUG("ACS_LM_CmdServer::ACS_LM_CmdServer() %s","Entering");
	clientConnectEvent = new ACE_Event();
	setDSDServerStopSignal(false);
	DEBUG("ACS_LM_CmdServer::ACS_LM_CmdServer() %s","Entering");
}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_CmdServer destructor
=================================================================== */
ACS_LM_CmdServer::~ACS_LM_CmdServer()
{

	DEBUG("ACS_LM_CmdServer::~ACS_LM_CmdServer() %s","Entering");
	if(clientConnectEvent != NULL)
	{
		clientConnectEvent->remove();
		if( clientConnectEvent != NULL)
		{
		delete clientConnectEvent;
		}
	}
	if(theLMCmdServer != NULL)
	{
		theLMCmdServer->close();
		delete theLMCmdServer;
		theLMCmdServer = NULL;
	}
	DEBUG("ACS_LM_CmdServer::~ACS_LM_CmdServer() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: listen
=================================================================== */
bool ACS_LM_CmdServer::listen()
{
	DEBUG("ACS_LM_CmdServer::listen()  %s","Entering");
	double timeForWait = 0;
	ACE_Time_Value oTimeValue(timeForWait);
	DEBUG("Number of times the cleanup activity has executed since service startup : %d\n",(oTimeValue.sec()));
	//theLMCmdServer = new ACS_DSD_Server(acs_dsd:: SERVICE_MODE_UNIX_SOCKET);
	//***************************************************************************
	//Event terminates when user press ctrl+C
	//Event terminates when the time for debug mode elapses
	//***************************************************************************
	while(clientConnectEvent->wait(&oTimeValue,0))
	{
		theLMCmdServer = new ACS_DSD_Server(acs_dsd:: SERVICE_MODE_UNIX_SOCKET);
		if(theLMCmdServer == NULL)
		{
			DEBUG("%s","ACS_LM_CmdServer::listen() Error occured while allocating memory for theLMCmdServer");
			return -1;
		}
		int ret=theLMCmdServer->open();
		////INFO(1,"Status of open() : %d",ret);
		if (ret < 0)
		{
			DEBUG("ACS_LM_CmdServer::listen()  %s","DSD server open failed");
			if(theLMCmdServer != NULL)
			{
				theLMCmdServer->close();
				delete theLMCmdServer;
				theLMCmdServer = NULL;
			}
			ACE_Time_Value oTimeValueSeconds(10);
			oTimeValue = oTimeValueSeconds;
			continue;
			//return false;
		}
		ret = theLMCmdServer->publish(LM_CMD_DSD_SERVER_NAME, LM_CMD_DSD_SERVER_DOMAIN, acs_dsd::SERVICE_VISIBILITY_LOCAL_SCOPE);
		//INFO(1,"Status of publish() : %d",ret);
		DEBUG("ACS_LM_CmdServer::listen(); The return value of published is :%d", ret);
		if (ret < 0)
		{
			DEBUG("ACS_LM_CmdServer::listen()  %s","DSD server publish failed");
			if(theLMCmdServer != NULL)
			{
				theLMCmdServer->close();
				delete theLMCmdServer;
				theLMCmdServer = NULL;
			}
			ACE_Time_Value oTimeValueSeconds(10);
			oTimeValue = oTimeValueSeconds;
			continue;
		}
		DEBUG("ACS_LM_CmdServer::listen(); %s service is published successfully", LM_CMD_DSD_SERVER_NAME);
		break;
	}
	DEBUG("ACS_LM_CmdServer::listen()  %s","Leaving");
	return(true);
}//end of listen
/*=================================================================
	ROUTINE: accept
=================================================================== */
//ACE_INT32 ACS_LM_CmdServer::accept(ACE_HANDLE stopCmdThreadFds)
ACE_INT32 ACS_LM_CmdServer::accept()
{
	//DEBUG("ACS_LM_CmdServer::accept()  %s","Entering");
	ACE_HANDLE serverHandle = ACE_INVALID_HANDLE;
 	if(theLMCmdServer != NULL)
	{
		int handle_count = 1;
	   theLMCmdServer->get_handles(&serverHandle, handle_count);

		if (serverHandle == 0)
		{
			ERROR("ACS_LM_CmdServer::accept() %s", "serverHandle is zero");
			return 0;
		}



		struct pollfd* myPollfd = new pollfd[serverHandle];

			myPollfd[0].fd = serverHandle;
			myPollfd[0].events = POLLIN | POLLRDHUP | POLLHUP | POLLERR;
			myPollfd[0].revents = 0;

		ACE_Time_Value selectTime;
		selectTime.set(1);
		ACE_INT32 ret;
		ret=ACE_OS::poll (myPollfd, 1, &selectTime);
		//Check for timeout.
		if (ret == 0)
		{
			if(getDSDServerStopSignal() == true)
			{
				DEBUG("ACS_LM_CmdServer::accept() %s", "Stop event signalled");
				DEBUG("ACS_LM_CmdServer::accept() %s", "Leaving ACS_LM_CmdServer::accept()...");
				delete [] myPollfd;
				return -1;
			}
			delete [] myPollfd;
			return 0;
		}
		else if( ret < 0 )
		{
			if( ACE_OS::last_error() == EINTR )
			{
				DEBUG("ACS_LM_CmdServer::accept()  %s","Error in get handles for LM CMD Server, returning.. ");
				if(getDSDServerStopSignal() == true)
				{
					DEBUG("ACS_LM_CmdServer::accept() %s", "Stop event signalled");
					DEBUG("ACS_LM_CmdServer::accept() %s", "Leaving ACS_LM_CmdServer::accept()...");
					delete [] myPollfd;
					return -1;
				}
				//return false;
				delete [] myPollfd;
				return 0;
			}
			else
			{
				ERROR("ACS_LM_CmdServer::accept() %s","Error occurred while waiting for multiple handles ,errno: %d",errno);
				if(getDSDServerStopSignal() == true)
				{
					DEBUG("ACS_LM_CmdServer::accept() %s", "Stop event signalled");
					DEBUG("ACS_LM_CmdServer::accept() %s", "Leaving ACS_LM_CmdServer::accept()...");
					delete [] myPollfd;
					return -1;
				}
				delete [] myPollfd;
				return 0;
			}
		}
		theDSDSession = new ACS_DSD_Session();
		ret = theLMCmdServer->accept(*theDSDSession);
		if (ret < 0)
		{
			DEBUG("ACS_LM_CmdServer::accept()  %s","Error occurred in accepting the LM Client connection");
			delete theDSDSession;
			theDSDSession = 0;
			//return false;
			return 0;
		}
	}
	else
	{
		DEBUG("ACS_LM_CmdServer::accept()  %s","theLMCmdServer = NULL");
		DEBUG("ACS_LM_CmdServer::accept()  %s","Stop request received");
		return -1;
	}

	DEBUG("ACS_LM_CmdServer::accept()  %s","Leaving");
	//return true;
	return 1;

}//end of accept
/*=================================================================
	ROUTINE: close
=================================================================== */
bool ACS_LM_CmdServer::close()
{
	DEBUG("ACS_LM_CmdServer::close()  %s","Entering");
	//forceClose = true;
	clientConnectEvent->pulse();
	//CNI30_4_1740(HO33558)
	if (theDSDSession != 0)
	{
		DEBUG("ACS_LM_CmdServer::close()  %s","Deleting the client session in close()");
		theDSDSession->close();
		delete theDSDSession;
		theDSDSession = 0;
	}
	//END of CNI
	DEBUG("ACS_LM_CmdServer::close()  %s","Leaving");
	return true;
}//end of close
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_LM_CmdServer::send(const ACS_LM_Cmd& cmd)
{
	DEBUG("ACS_LM_CmdServer::send()  %s","Entering");
	bool sent = false;
	const ACE_TCHAR * buffer = cmd.toBytes();
	ACE_UINT64 bytesToSend = cmd.size();
	//	ACE_UINT64 bytesSent = 0;

	if( theDSDSession != 0)
	{
		ACE_UINT64 bytesSent = 0;
		bytesSent = theDSDSession->send(buffer,bytesToSend);
		sent = (bytesToSend == bytesSent);
	}

	delete[] buffer;
	DEBUG("ACS_LM_CmdServer::send()  %s","Leaving");
	return sent;
}//end of send
/*=================================================================
	ROUTINE: receive
=================================================================== */
bool ACS_LM_CmdServer::receive(ACS_LM_Cmd& cmd)
{
	DEBUG("ACS_LM_CmdServer::receive()  %s","Entering");
	bool received = false;
	ACE_TCHAR myBuffer[512];
	ACE_UINT64 myBytesToReceive = 512; // Max response length
	ACE_UINT64 myBytesReceived = 0;
	myBytesReceived = theDSDSession->recv(myBuffer,myBytesToReceive);
	if (myBytesReceived > 0)
	{
		received = cmd.fromBytes(myBuffer);
	}
	DEBUG("ACS_LM_CmdServer::receive()  %s","Leaving");
	return received;
}//end of receive


bool ACS_LM_CmdServer::getDSDServerStopSignal()
{
	ACS_LM_CmdServer::theDSDServerMutex.acquire();
	bool myStatus = theDSDServerStopSignal;
	ACS_LM_CmdServer::theDSDServerMutex.release();
	return myStatus;
}
void ACS_LM_CmdServer::setDSDServerStopSignal(bool mySignal)
{
	DEBUG ("%s","ACS_LM_CmdServer::setDSDServerStopSignal setDSDServerStopSignal is set");
	ACS_LM_CmdServer::theDSDServerMutex.acquire();
	theDSDServerStopSignal = mySignal;
	ACS_LM_CmdServer::theDSDServerMutex.release();

}
