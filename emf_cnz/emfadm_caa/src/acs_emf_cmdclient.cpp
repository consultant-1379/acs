//******************************************************************************
//
//  NAME
//     acs_emf_cmdclient.cpp
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
#include "acs_emf_cmdclient.h"
#include "acs_emf_tra.h"
#include <ace/Time_Value.h>
#include <ace/Event.h>
ACE_HANDLE ACS_EMF_CmdClient::m_stopEventHandle;


/*=================================================================
	ROUTINE: ACS_EMF_CmdClient constructor
=================================================================== */
ACS_EMF_CmdClient::ACS_EMF_CmdClient(ACE_HANDLE stophandle)
:clientConnectEvent(NULL)
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::ACS_EMF_CmdClient() - Entering");
	theEMFCmdClient = new ACS_DSD_Client();
	theEMFCmdSession = new ACS_DSD_Session();
	clientConnectEvent = new ACE_Event();
	m_stopEventHandle = stophandle;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::ACS_EMF_CmdClient() - Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_EMF_CmdClient destructor
=================================================================== */
ACS_EMF_CmdClient::~ACS_EMF_CmdClient()
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::~ACS_EMF_CmdClient() - Entering");
	delete theEMFCmdClient;
	if(theEMFCmdSession != NULL){
		disconnect();
		delete theEMFCmdSession;
		theEMFCmdSession = 0;
	}
	if(clientConnectEvent != NULL){
		clientConnectEvent->remove();
		delete clientConnectEvent;
		clientConnectEvent = 0;
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::~ACS_EMF_CmdClient() - Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: connect
=================================================================== */
bool ACS_EMF_CmdClient::connect(int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState)
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Entering");
	double timeForWait = 0;
	ACE_Time_Value oTimeValue(timeForWait);
	int retry = 5;
	int ret = -1;
	ACS_DSD_Node localNode;
	ACS_DSD_Client *pClient = NULL;

        pClient = new ACS_DSD_Client();
        pClient->get_local_node(localNode);

        DEBUG(1, "ACS_EMF_CmdClient::connect() - Local Node  - nodeId:[%d], systemName:[%s], systemType:[%d], nodeState:[%d], nodeName [%s], nodeSide:[%d]",localNode.system_id, localNode.system_name, localNode.system_type, localNode.node_state, localNode.node_name, localNode.node_side);

	if (NULL != pClient){
		delete pClient;
		pClient = NULL;
	} 

	if( ACS_EMF_Common::GetHWVariant() >= 3)
	{
		iSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
		enmNodeState = acs_dsd::NODE_STATE_UNDEFINED ;
		std::vector<ACS_DSD_Node> reachable_nodes;
		std::vector<ACS_DSD_Node> unreachable_nodes;
		int ret = theEMFCmdClient->query("EMFCMDSERVER", "EMF", acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);
		DEBUG(1, "ACS_EMF_CmdClient::connect() - Query from DSD return [%i]",ret);
		if(!ret)
		{
			std::vector<ACS_DSD_Node>::iterator it;
			it = reachable_nodes.begin();
			while( (it != reachable_nodes.end()) ){
				DEBUG(1, "ACS_EMF_CmdClient::connect() - RemoteNode - nodeId:[%d], systemName:[%s], systemType:[%d], nodeState:[%d], nodeName [%s], nodeSide:[%d]",it->system_id, it->system_name, it->system_type,it->node_state, it->node_name, it->node_side);

				if (reachable_nodes.size() > 1)
				{
					if ((it->node_state) != (localNode.node_state)){
						iSystemId = it->system_id;
						enmNodeState = it->node_state;
						//DEBUG(1, "ACS_EMF_CmdClient::connect() - reachable node id = [%d], node state = [%d]", iSystemId, enmNodeState);
						break;
					}
				}
				else
				{
					DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - query is failed to get reachable and unreachable nodes");
					DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Leaving");
					return false;
				}
				++it;
			}
		}
		else
		{
			DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Client query is failed");
			DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Leaving");
			return(false);
		}
	}

	while( retry--)
	{
		DEBUG(1, "ACS_EMF_CmdClient::connect() - Trying to connect node id = [%d], node state = [%d]", iSystemId, enmNodeState);
                ret = theEMFCmdClient->connect(*theEMFCmdSession, "EMFCMDSERVER", "EMF", iSystemId, enmNodeState);

		if(ret < 0)
		{
			ERROR(1, "ACS_EMF_CmdClient::connect() - Failure occurred in connecting to EMF CMD server. Connect return [%i]",ret);
			DEBUG(1, "%s","ACS_EMF_CmdClient::connect() - Wait for retry...");
			sleep(5);
		}
		else
		{
			INFO(1, "%s", "ACS_EMF_CmdClient::connect() - Connected successfully");
			DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Leaving");
			return true;
		}

	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::connect() - Leaving");
	return(false);
}//end of connect
/*=================================================================
        ROUTINE: connectToNode
=================================================================== */
bool ACS_EMF_CmdClient::connectToNode(int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, acs_dsd::NodeSideConstants *penmNodeSide)
{
        int iRet = EMF_RC_OK;
        ACS_DSD_Node objDsdNode;
        // Initialization
        DEBUG(1, "%s", "ACS_EMF_CmdClient::connectToNode() - Entering");
        int retry = 5;
        while( retry--)
        {
                iRet = theEMFCmdClient->connect(*theEMFCmdSession, "EMFCMDSERVER", "EMF", iSystemId, enmNodeState);
                if(iRet < 0)
                {
                        DEBUG(1, "%s", "ACS_EMF_CmdClient::connectToNode - Failure while connect to EMF server");
                        DEBUG(1, "ACS_EMF_CmdClient::connectToNode - errno=[%d] errorText=[%s]",theEMFCmdClient->last_error(), theEMFCmdClient->last_error_text());
                        DEBUG(1, "%s","ACS_EMF_CmdClient::connectToNode - Wait for retry...");
                        sleep(5);
                }
                else  // Conenct is OK. Get remote side
                {
                        if(penmNodeSide != NULL){
                                *penmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED; //Init
                                theEMFCmdSession->get_remote_node(objDsdNode);// Get remote node
                                *penmNodeSide = objDsdNode.node_side; // Copy node_side to output params
                        }
                        DEBUG(1, "%s", "ACS_EMF_CmdClient::connectToNode - connect successfully");
                        DEBUG(1, "%s", "ACS_EMF_CmdClient::connectToNode - Leaving");
                        return true;
                }
        }
        DEBUG(1, "%s", "ACS_EMF_CmdClient::connectToNode - Leaving");
        return(false);
}//end of connect
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_EMF_CmdClient::send(const ACS_EMF_Cmd& cmd)
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::send() - Entering");
	bool sent = false;
	ACE_TCHAR* buffer = cmd.toBytes();
	ACE_UINT64 bytesToSend = cmd.size();
	ACE_UINT64 bytesSent = 0;
	if( theEMFCmdSession != 0)
	{
		bytesSent = theEMFCmdSession->send(buffer,bytesToSend);
		sent = (bytesToSend == bytesSent);
	}
	if (bytesSent <= 0)
		ERROR(1, "%s", "ACS_EMF_CmdClient::send() - Sending message to EMF CMD Server is failed");
	delete[] buffer;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::send() - Leaving");
	return sent;
}//end of send
/*=================================================================
	ROUTINE: receive
=================================================================== */
bool ACS_EMF_CmdClient::receive(ACS_EMF_Cmd& cmd, unsigned int timeout_ms)
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - Entering");
	ACE_HANDLE data_handle = ACE_INVALID_HANDLE;
	int handle_count = 1;
	bool received = false;
	int ret = theEMFCmdSession->get_handles(&data_handle,handle_count);
        if( ret < 0)
        {
                ERROR(1,"%s","ACS_EMF_CmdClient::receive, get_handles failed");
                return false;
        }
	else
		DEBUG(0, "%s", "ACS_EMF_CmdClient::receive(), get_handles successful.. ");

	struct timeval tv;
	tv.tv_sec = timeout_ms/1000;
	tv.tv_usec = 0;
	fd_set readhandleset;
	FD_ZERO(&readhandleset);
	FD_SET(m_stopEventHandle,&readhandleset);
	FD_SET(data_handle,&readhandleset);
	int max_fd = (m_stopEventHandle > data_handle)?m_stopEventHandle:data_handle;
	ret = select ((int)(max_fd+1), &readhandleset, 0, 0,(timeout_ms == 0)? NULL:&tv);
	if(ret < 0){
		ERROR(1,"%s","ACS_EMF_CmdClient::receive() - select wait failed");
	}
	else if( ret > 0)
	{
		if(FD_ISSET(m_stopEventHandle,&readhandleset))
		{
			DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - stop event signaled ");
			DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - Leaving");
			return received;
		}
		else if(FD_ISSET(data_handle,&readhandleset))
		{
			DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - data handle signaled, read the data.. ");
			ACE_TCHAR myBuffer[EMF_CMD_COPY_BUFF_LEN];
			ACE_UINT64 myBytesToReceive = EMF_CMD_COPY_BUFF_LEN; // Max response length
			ACE_INT64 retcode = 0;
			retcode = theEMFCmdSession->recv(myBuffer,myBytesToReceive);
			DEBUG(1,"ACS_EMF_CmdClient::receive() - call 'theEMFCmdSession->recv()'returns <retcode == %d> ",retcode);
			if (retcode > 0)
				received = cmd.fromBytes(myBuffer);
		}
	}else {
		DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - no data received. Timeout expired! ");
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::receive() - Leaving");
	return received;
}//end of receive
/*=================================================================
	ROUTINE: disconnect
=================================================================== */
bool ACS_EMF_CmdClient::disconnect()
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::disconnect() - Entering");
	clientConnectEvent->pulse();
	if (theEMFCmdSession != 0)
		theEMFCmdSession->close();
	DEBUG(1, "%s", "ACS_EMF_CmdClient::disconnect() - Leaving");
	return true;
}//end of disconnect
/*=================================================================
        ROUTINE: queryApFromDSD
=================================================================== */
bool ACS_EMF_CmdClient::queryApFromDSD(std::vector<ACS_DSD_Node> *pVctSortReachable)
{
	bool bError = true;
	//ACS_DSD_Node node;
	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD() - Entering");
	if(pVctSortReachable != NULL){// Check output pointer
		int iRet = EMF_RC_OK;
		iRet = theEMFCmdClient->query("EMFCMDSERVER", "EMF", acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);// Query from dsd
		if(iRet == acs_dsd::ERR_NO_ERRORS){  // Check for result
			if (reachable_nodes.empty() == false){// Check if reachable_nodes vector is empty
				bError = false;// Set flag to ok
				DEBUG(1, "ACS_EMF_CmdClient::queryApFromDSD() - reachable node size [%i]", reachable_nodes.size());
				sortReachableNode(&reachable_nodes, pVctSortReachable); // Sort ap list
			}else  // Error from dsd query
				DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD - query is failed to get reachable and unreachable nodes");
		}else
			DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD - Client query is failed");
	} // check pointer
	DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD - Leaving");
	return !(bError);
} //end of queryApFromDSD
/*=================================================================
        ROUTINE:sortReachableNode
=================================================================== */
void ACS_EMF_CmdClient::sortReachableNode(std::vector<ACS_DSD_Node> *pReachable, std::vector<ACS_DSD_Node> *pSortReachable)
{
	ACS_DSD_Node objNode;
	bool bOk = true;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::sortReachableNode - Entering");
	if(pSortReachable != NULL){// Check if output obj exist
		theEMFCmdClient->get_local_node(objNode);// get this node
		bOk = fillNodeAndPartner(pReachable, objNode, pSortReachable);// Push in the list this node and the partner (this AP).
		if(bOk == true){ // Check for error
			while((pReachable->size() > 0) && (bOk == true)){ // For all element in pReachable
				getNextNode(*pReachable, &objNode) ;// Get next active node (next AP)
				bOk = fillNodeAndPartner(pReachable, objNode, pSortReachable);// Push in the list this next AP active node and the partner.
			}
		}
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::sortReachableNode - Leaving");
}//end of sortReachableNode
/*=================================================================
        ROUTINE:getNextNode
=================================================================== */
void ACS_EMF_CmdClient::getNextNode(const std::vector<ACS_DSD_Node> &objReachable, ACS_DSD_Node *pNode)
{
	int32_t iSysId = acs_dsd::SYSTEM_ID_UNKNOWN;
	bool bFound = false;
	std::vector<ACS_DSD_Node>::const_iterator it;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::getNextNode - Entering");
	if(pNode != NULL){// Check pointer
		pNode->reset();// Clear node info
		it = objReachable.begin();// Set to begin of list
		if(it != objReachable.end()){// Check if element exist
			iSysId = (*it).system_id;// Set first sysid
			++it;// Next element
		}
		while(it != objReachable.end()){// For all element in the list, search the little
			if((*it).system_id < iSysId){// Check for little
				iSysId = (*it).system_id;// Set new little
			}
			++it;//next element
		} // while
		it = objReachable.begin();// Search active node for iSysId. Set to begin of list
		while((it != objReachable.end()) && (bFound == false)){// For all element in the list, search the little
			if(((*it).system_id == iSysId) && ((*it).node_state) == acs_dsd::NODE_STATE_ACTIVE){ // Check for iSysId-active node
				pNode->set(iSysId, (*it).system_name,(*it).system_type,(*it).node_state,(*it).node_name,(*it).node_side);// Set output node
				bFound = true;//Set flag
			}
			++it;//Next element
		} // while
		it = objReachable.begin();// Search node for iSysId if active node NOT FOUND. Set to begin of list
		while((it != objReachable.end()) && (bFound == false)){// For all element in the list, search the little
			if((*it).system_id == iSysId){ // Check for iSysId-active node
				pNode->set(iSysId, (*it).system_name,(*it).system_type,(*it).node_state,(*it).node_name,(*it).node_side);// Set output node
				bFound = true;// Set flag
				DEBUG(1, "ACS_EMF_CmdClient::fillNodeAndPartner - WARNING: No active node found for system_id [%i]", iSysId);// Trace a no active sc found
			}
			++it;// Next element
		} // while
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::getNextNode - Leaving");
}//end of getNextNode
/*=================================================================
        ROUTINE:fillNodeAndPartner
=================================================================== */
bool ACS_EMF_CmdClient::fillNodeAndPartner(std::vector<ACS_DSD_Node> *pReachable, const ACS_DSD_Node &objDsdNode, std::vector<ACS_DSD_Node> *pSortReachable)
{
	bool bFound = false;
	std::vector<ACS_DSD_Node>::iterator it;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::fillNodeAndPartner - Entering");
	// Check pointer
	if(pSortReachable != NULL){// Check pointer
		it = pReachable->begin();// Set position on the first element
		while((it != pReachable->end()) && bFound == false){// For all element (NOTE: For now the DSD query does't set node_side)
			// search input dsd node
			if(((*it).system_id == objDsdNode.system_id) &&
				 ((*it).node_state == objDsdNode.node_state))   // Node is equal: Set found flag
				bFound = true;
			else // Next element
				++it;
		}
		if(bFound == true){// Check flag
			pSortReachable->push_back(objDsdNode);// Push object in output list
			pReachable->erase(it);// Remove it from input list
			it = pReachable->begin();// Search partner node: Set position on the first element
			bFound = false;// Set found flag to false
			while((it != pReachable->end()) && (bFound == false)){// For all element, search partner node
				// search input dsd node
				if(((*it).system_id == objDsdNode.system_id)){
					bFound = true;// Node is equal: Set found flag
					pSortReachable->push_back((*it));// Push it in output list
					pReachable->erase(it);// Remove it from input list
				}
				++it;// Next element
			} // while((it ...
			// Check if partner node is found
			if(bFound == false){// Not found
				DEBUG(1,"WARNING: Partner node for system_id [%i] not found.", objDsdNode.system_id);
				bFound = true;// This is a warning, set exit flag to TRUE
			}
		}else // No object found
			DEBUG(1,"ACS_EMF_CmdClient::fillNodeAndPartner - ERROR: Input node system_id [%i] not found.", objDsdNode.system_id);
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::fillNodeAndPartner - Leaving");
	// Exit from method
	return (bFound);
}//end of fillNodeAndPartner
/*=================================================================
        ROUTINE:invokeEmfMethod
=================================================================== */
ACE_INT16 ACS_EMF_CmdClient::invokeEmfMethod(int iCommand, int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, acs_dsd::NodeSideConstants *penmNodeSide)
{
	ACE_INT16 iRet;
	// Initialization
	iRet = CHECKDVD_MASK_UNKNOWN;
	DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Entering");
	// Connect to remote node. NOTE: if penmNodeSide==NULL, connectToNode does not call get_remote_node of dsd.
	if(connectToNode(iSystemId,enmNodeState,penmNodeSide))
	{
		DEBUG(1, "ACS_EMF_CmdClient::invokeEmfMethod - Connection to [%i] is successful",iSystemId);
		ACS_EMF_Cmd cmdSend(iCommand);
		if(send(cmdSend))
		{
			DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Send message to server is successful");
			ACS_EMF_Cmd cmdRecv;
			if(receive(cmdRecv, EMF_TIMEOUT_ON_RECEIVE)){
				DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Client received reply from server successfully");

				std::list<std::string> argList = cmdRecv.commandArguments();
				iRet = cmdRecv.commandCode();
				DEBUG(1,"ACS_EMF_CmdClient::invokeEmfMethod - Code received from Server = %d",iRet);
			}else
				DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Client not able to receive reply from server");
		}else
			DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Sending message to server is failed");
		// Close DSD connection
		disconnect();
	}
	else{
		DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Connection with server is failed");
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::invokeEmfMethod - Leaving");
	// Exit from method
	return(iRet);
}//end of invokeEmfMethod
/*=================================================================
        ROUTINE: queryApFromDSD
=================================================================== */
bool ACS_EMF_CmdClient::queryApFromDSD()
{
	DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD() - Entering");
	int i = 0;
	bool bError = true;
	double timeForWait = 0;
	ACE_Time_Value oTimeValue(timeForWait);
	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;
	while(clientConnectEvent->wait(&oTimeValue,0))
	{
		int ret = theEMFCmdClient->query("EMFCMDSERVER", "EMF", acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);
		DEBUG(1, "ACS_EMF_CmdClient::queryApFromDSD() - query return [%i]",ret);
		if(!ret){
			if (reachable_nodes.size() > 1){
				DEBUG(1,"%s","ACS_EMF_CmdClient::queryApFromDSD() - Passive node is reachable");
			}else{
				DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD() - No reachable node found");
				ACE_Time_Value oTimeValueSeconds(5);
				oTimeValue = oTimeValueSeconds;
				i++;
				if(i < 5)
					continue;
				else{
					bError = false;// Set flag to Nok
					break;
				}
				//return false;
			}
		}else{
			DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD() - query is failed");
			bError = false;// Set flag to Nok
		}
		break;
	}
	DEBUG(1, "%s", "ACS_EMF_CmdClient::queryApFromDSD() - Leaving");
	return bError;
} //end of queryApFromDSD

/*=================================================================
        ROUTINE:closeConnection
=================================================================== */
void ACS_EMF_CmdClient::closeConnection()
{

}//end of closeConnection

//=================================================================
//        ROUTINE: send_receive()
//===================================================================
ACE_INT16 ACS_EMF_CmdClient::send_receive(const ACS_EMF_Cmd & cmdSend, int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, ACS_EMF_Cmd & cmdRecv)
{
	ACE_INT16 resultCode = EMF_RC_OK;
	DEBUG(1,"ACS_EMF_CmdClient::send_receive() - EMF is trying to connect to AP == %d state == %d ",iSystemId, enmNodeState);

	if(!connect(iSystemId,enmNodeState)){
		ERROR(1,"ACS_EMF_CmdClient::send_receive() - Connection to service failed ! resultCode == %d",EMF_RC_UNABLETOCONNECTSERV );
		return EMF_RC_UNABLETOCONNECTSERV;
	}
	DEBUG(1, "ACS_EMF_CmdClient::send_receive() - Connection to [%i - nodestate == %d] is successful",iSystemId,enmNodeState);

	if(!send(cmdSend)){
		ERROR(1,"ACS_EMF_CmdClient::send_receive() - Send command [%d] to service failed !", cmdSend.commandCode());
		return EMF_RC_CANNOTSENDCMDTOSERVER;
	}

	DEBUG(1, "%s", "ACS_EMF_CmdClient::send_receive() - Send message to server is successful");
	if(receive(cmdRecv)){
		DEBUG(1, "%s", "ACS_EMF_CmdClient::send_receive() - Client received reply from server successfully");
	}else {
		ERROR(1, "%s", "ACS_EMF_CmdClient::send_receive() - Client not able to receive reply from DSD server");
		//theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Unable to connect to server");
		resultCode = EMF_RC_CANNOTRECEIVEROMSERVER;
	}
	// Close DSD connection
	disconnect();

	DEBUG(1, "%s", "ACS_EMF_CmdClient::send_receive() successful leaving!");
	// Exit from method
	return(resultCode);
}
