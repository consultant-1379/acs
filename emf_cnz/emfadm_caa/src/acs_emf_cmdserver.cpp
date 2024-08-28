//******************************************************************************
//
//  NAME
//     acs_emf_cmdserver.cpp
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
//     2011-12-08 by XRAMMAT PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_emf_cmdserver.h"
#include "acs_emf_server.h"
#include "acs_emf_cmd.h"
#include "acs_emf_tra.h"
#include <ace/ACE.h>
#include <ace/Time_Value.h>
/*=================================================================
	ROUTINE: ACS_EMF_CmdServer constructor
=================================================================== */
ACS_EMF_CmdServer::ACS_EMF_CmdServer()
:clientConnectEvent(NULL),
 theEMFCmdServer(0),theDSDSession(0)
{
	DEBUG(0, "%s", "ACS_EMF_CmdServer::ACS_EMF_CmdServer() - Entering");
	clientConnectEvent = new ACE_Event();
        forceClose = false;
	DEBUG(0, "%s", "ACS_EMF_CmdServer::ACS_EMF_CmdServer() - Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_EMF_CmdServer destructor
=================================================================== */
ACS_EMF_CmdServer::~ACS_EMF_CmdServer()
{
	DEBUG(0, "%s", "Entering ACS_EMF_CmdServer::~ACS_EMF_CmdServer()");
	if(clientConnectEvent != NULL){
		clientConnectEvent->remove();
		delete clientConnectEvent;
		clientConnectEvent = NULL;
	}
	if(theEMFCmdServer != NULL)
	{
		//theEMFCmdServer->unregister();
		//DEBUG(1,"%s","Unregister successful");
		theEMFCmdServer->close();
		delete theEMFCmdServer;
		theEMFCmdServer = NULL;
	}
	DEBUG(0, "%s", "Leaving ACS_EMF_CmdServer::~ACS_EMF_CmdServer()");
}//end of destructor
/*=================================================================
	ROUTINE: listen
=================================================================== */
bool ACS_EMF_CmdServer::listen()
{
	DEBUG(1, "%s", "ACS_EMF_CmdServer::listen() - Entering");
	double timeForWait = 0;
	ACE_Time_Value oTimeValue(timeForWait);

	theEMFCmdServer = new ACS_DSD_Server(acs_dsd:: SERVICE_MODE_INET_SOCKET);
	if(theEMFCmdServer == NULL)
	{
		ERROR(1, "%s", "ACS_EMF_CmdServer::listen() - Error occurred while allocating memory for theEMFCmdServer");
		DEBUG(1, "%s", "ACS_EMF_CmdServer::listen() - Leaving");
		return EMF_RC_ERROR;
	}
	bool  publish_done = false;
	while(clientConnectEvent->wait(&oTimeValue,0))
	{
		int open_result = theEMFCmdServer->open();
		if (open_result < 0)
		{
			ERROR(1, "%s", "ACS_EMF_CmdServer::listen() - EMF DSD server OPEN failed!!");
			ACE_Time_Value oTimeValueSeconds(10);
			oTimeValue = oTimeValueSeconds;
			continue;
		}
		int ret = theEMFCmdServer->publish("EMFCMDSERVER", "EMF", acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);
		if (ret < 0)
		{
			ERROR(1, "%s", "ACS_EMF_CmdServer::listen() - EMF DSD server publish failed!!");
			theEMFCmdServer->close();

			ACE_Time_Value oTimeValueSeconds(10);
			oTimeValue = oTimeValueSeconds;
			continue;
		}
		publish_done = true;
		INFO(1, "%s", "ACS_EMF_CmdServer::listen() - EMF service published to DSD successfully ");
		break;
	}
	if( publish_done == false && theEMFCmdServer != NULL)
	{
		INFO(1, "%s", "ACS_EMF_CmdServer::listen() - EMF service NOT published to DSD !");
		theEMFCmdServer->close();
		delete theEMFCmdServer;
		theEMFCmdServer = NULL;
	}

	DEBUG(1, "%s", "ACS_EMF_CmdServer::listen() - Leaving");
	return publish_done;
}//end of listen
/*=================================================================
	ROUTINE: listen
=================================================================== */
ACE_INT32 ACS_EMF_CmdServer::accept(ACE_HANDLE stopCmdThreadFds)
{
	DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Entering");
	ACE_HANDLE serverHandle[3] = {ACE_INVALID_HANDLE, ACE_INVALID_HANDLE, ACE_INVALID_HANDLE};
	if(theEMFCmdServer != NULL)
	{
    	int handle_count = 3;
		int ret  = theEMFCmdServer->get_handles(serverHandle, handle_count);
		if( ret < 0)
		{
			ERROR(1, "ACS_EMF_CmdServer::accept - get handles failed for EMF CMD Server, returning error=%d, errortext=%s ",theEMFCmdServer->last_error(), theEMFCmdServer->last_error_text());
			DEBUG(0, "%s", "ACS_EMF_CmdServer::accept - Leaving");
			return CMDSRV_ERROR;
		}
		else
		{
			//Now, we have to wait on the handle for some data from the child.
			//If some data arrives, we will recieve a signal on the handle and then we can receive the data.
			//Create a FD set.
			INFO(1, "ACS_EMF_CmdServer::accept - No of handles received = %d", handle_count);
			ACE_Handle_Set readHandleSet;
			int maxfd = stopCmdThreadFds;
			ACE_Time_Value tv(5, 0);
			readHandleSet.set_bit(stopCmdThreadFds);
			for( int i = 0 ;  i < handle_count; i++)
			{
				readHandleSet.set_bit(serverHandle[i]);
				if( maxfd < serverHandle[i] )
				{
					maxfd = serverHandle[i];
				}
			}
			INFO(1,"%s", "ACS_EMF_CmdServer::accept - Wait until one of the object becomes signaled");
			int ret1 = ACE::select(maxfd+1, &readHandleSet, 0, 0);
			//Check for timeout.
			if (ret1 > 0)
			{
				if (readHandleSet.is_set(stopCmdThreadFds))
				{
					DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Stop Event signaled from the service");
					theEMFCmdServer->unregister();
					// Clear pipe
					int iReaded;
					uint8_t pBuffer[INT_PIPE_BUFFERSIZE];
					// Initialization
					iReaded = INT_PIPE_BUFFERSIZE;
					DEBUG(1,"%s","ACS_EMF_CmdServer::accept - Prepare to clear pipe...");
					if(stopCmdThreadFds != ACE_INVALID_HANDLE){
						// Read all byte in the pipe (clear pipe)
						while(iReaded == INT_PIPE_BUFFERSIZE){
							// Read a block
							iReaded = ACE_OS::read(stopCmdThreadFds, pBuffer, INT_PIPE_BUFFERSIZE);
							// Message
							DEBUG(1,"ACS_EMF_CmdServer::accept - Clear pipe bytes [%i]...", iReaded);
						}
					}else{ 
						// stopThreadFds is null
						DEBUG(1,"%s","ACS_EMF_CmdServer::accept - WARNING: stopThreadFds is null. No clear pipe.")
					}
					DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Unregister EMF with DSD successful...Leaving ACS_EMF_CmdServer::accept()");
					return CMDSRV_STOPSIGNAL;
				}
				for(int i=1;i<=handle_count;i++)
				{
					if(readHandleSet.is_set(serverHandle[i]))
					{
						//one of the handle signaled
						DEBUG(1, "ACS_EMF_CmdServer::accept - One of the handle[%d] signaled",i);
						break;
					}
				}
			}
			else if( ret1 == EMF_RC_OK )
			{
				ERROR(1,"%s","ACS_EMF_CmdServer::accept - Timeout occurred while waiting for request from EMF CMD Client");
				DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving ACS_EMF_CmdServer::accept()");
				return CMDSRV_ERROR;
			}
			else
			{
				ERROR(1, "%s", "ACS_EMF_CmdServer::accept - Error occurred in select system call in request handler");
				DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving ACS_EMF_CmdServer::accept()");
				return CMDSRV_ERROR;
			}
		}
		INFO(1, "%s","ACS_EMF_CmdServer::accept - One of the handle is signaled and establishing session");
		theDSDSession = new ACS_DSD_Session();
		ret = theEMFCmdServer->accept(*theDSDSession);
//    ACS_DSD_Node objNode;
//    theDSDSession->get_remote_node (objNode);
//    DEBUG(1,"Client has request connection: sysID[%d], side[%d], state[%d]", objNode.system_id, objNode.node_side, objNode.node_state);
		if (ret < 0)
		{
			ERROR(1, "%s", "ACS_EMF_CmdServer::accept - Error occurred in accepting the EMF Client connection");
			delete theDSDSession;
			theDSDSession = 0;
			DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving");
			return CMDSRV_ERROR;
		}
	}
	else
	{
		ERROR(1, "%s", "ACS_EMF_CmdServer::accept - theEMFCmdServer = NULL");
		DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving");
		return CMDSRV_ERROR;
	}
	DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving");
	return CMDSRV_OK;
}//end of accept
/*=================================================================
	ROUTINE: close
=================================================================== */
bool ACS_EMF_CmdServer::close()
{
	DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Entering");
	clientConnectEvent->signal();
	if (theDSDSession != 0)
	{
		INFO(1, "%s", "ACS_EMF_CmdServer::accept - Deleting the established client session");
		theDSDSession->close();
		delete theDSDSession;
		theDSDSession = 0;
	}
	DEBUG(1, "%s", "ACS_EMF_CmdServer::accept - Leaving ACS_EMF_CmdServer::close()");
	return true;
}//end of close
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_EMF_CmdServer::send(const ACS_EMF_Cmd& cmd)
{
	DEBUG(1, "%s", "ACS_EMF_CmdServer::send - Entering");
	bool sent = false;
	const ACE_TCHAR * buffer = cmd.toBytes();
	ACE_UINT64 bytesToSend = cmd.size();
	if( theDSDSession != 0)
	{
		ACE_UINT64 bytesSent = 0;
		bytesSent = theDSDSession->send(buffer,bytesToSend);
		sent = (bytesToSend == bytesSent);
	}else{
		DEBUG(1, "%s", "ACS_EMF_CmdServer::send - WARNING: theDSDSession is NULL");
	}
	delete[] buffer;
	DEBUG(1, "%s", "ACS_EMF_CmdServer::send - Leaving");
	return sent;
}//end of send
/*=================================================================
	ROUTINE: receive
=================================================================== */
bool ACS_EMF_CmdServer::receive(ACS_EMF_Cmd& cmd)
{
	DEBUG(1, "%s", "ACS_EMF_CmdServer::receive() - Entering");
	bool received = false;
	ACE_TCHAR myBuffer[EMF_CMD_COPY_BUFF_LEN];
	ACE_UINT64 myBytesToReceive = EMF_CMD_COPY_BUFF_LEN; // Max response length
	ACE_UINT64 myBytesReceived = 0;
	myBytesReceived = theDSDSession->recv(myBuffer,myBytesToReceive);
	if (myBytesReceived > 0)
		received = cmd.fromBytes(myBuffer);
	else
		INFO(1, "%s", "ACS_EMF_CmdServer::receive() - no data received!");
	DEBUG(1, "%s", "ACS_EMF_CmdServer::receive() - Leaving");
	return received;
}//end of receive
/*=================================================================
        ROUTINE: get_remote_node 
=================================================================== */
int ACS_EMF_CmdServer::get_remote_node (ACS_DSD_Node & nodeInfo) const {
	DEBUG(1, "%s", "ACS_EMF_CmdServer::get_remote_node() - Entering");
	int op_res = theDSDSession->get_remote_node(nodeInfo);
	if(op_res < 0 ) {
		ERROR(1, "ACS_EMF_CmdServer::get_remote_node() failed - ! error_code == < %d >", op_res);
	}
	else {
		DEBUG(1, "ACS_EMF_CmdServer::get_remote_node() - Remote Node Info: SysId = %d, SystemName =%s, NodeSide=%d", nodeInfo.system_id, nodeInfo.system_name, nodeInfo.node_side);
	}
	DEBUG(1, "%s", "ACS_EMF_CmdServer::get_remote_node() - Leaving");
	return op_res;
}//end of get_remote_node
