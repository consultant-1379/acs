//******************************************************************************
//
//  NAME
//     acs_emf_dsdserver.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2011. All rights reserved.
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
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//      2011-12-08 by XRAMMAT PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>
#include "acs_emf_dsdserver.h"
#include "acs_emf_tra.h"
#include "acs_emf_param.h"
#include "acs_emf_commandhandler.h"

using namespace std;

ACE_INT32 ACS_EMF_DSDServer::theOperationalStateinPassive = 0;
ACE_UINT16 ACS_EMF_DSDServer::theMediaOwnerinPassive = 0;

const char* mediaOwnerFile="/cluster/emf_unlock_info";
/*=================================================================
	ROUTINE: ACS_EMF_DSDServer constructor
=================================================================== */
ACS_EMF_DSDServer::ACS_EMF_DSDServer(ACE_HANDLE endEvent)
:isStop(false),
 // serviceStopEvent(NULL),
 stopThreadFds(endEvent)
{
	DEBUG(1,"%s","ACS_EMF_DSDServer::ACS_EMF_DSDServer - Entering");
	//serviceStopEvent = new ACE_Event();
	isDSDPublished = false;
        theClienthandlerPtr = NULL;
	m_pCmdHandler = NULL;
	DEBUG(1,"%s","ACS_EMF_DSDServer::ACS_EMF_DSDServer - Leaving");
}//end of constructor

/*=================================================================
	ROUTINE: ACS_EMF_DSDServer destructor
=================================================================== */
ACS_EMF_DSDServer::~ACS_EMF_DSDServer()
{
	DEBUG(1,"%s","ACS_EMF_DSDServer::~ACS_EMF_DSDServer - Entering");
	DEBUG(1,"%s","ACS_EMF_DSDServer::~ACS_EMF_DSDServer - Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: stop
=================================================================== */
void ACS_EMF_DSDServer::stop()
{
	DEBUG(1,"ACS_EMF_DSDServer::stop - %s","Entering");
#if 0
	if(serviceStopEvent != NULL)
	{
		serviceStopEvent->signal();
		serviceStopEvent->remove();
		serviceStopEvent = NULL;
	}
#endif
	m_cmdServer.close();
	DEBUG(1,"ACS_EMF_DSDServer::stop - %s","Leaving");
}//end of stop
/*=================================================================
	ROUTINE: start
=================================================================== */
void ACS_EMF_DSDServer::start(void)
{
	DEBUG(0,"%s","ACS_EMF_DSDServer::start - Entering");
	activate();
	DEBUG(0,"%s","ACS_EMF_DSDServer::start - Leaving");
}//end of start
/*=================================================================
	ROUTINE: setClienthandler
=================================================================== */

void ACS_EMF_DSDServer::setClienthandler(ACS_EMF_MEDIAHandler* aClienthandler)
{
	DEBUG(1,"ACS_EMF_DSDServer::setClienthandler - %s","Entering");
	theClienthandlerPtr = aClienthandler;
	DEBUG(1,"ACS_EMF_DSDServer::setClienthandler - %s","Leaving");
}//end of setClienthandler

/*=================================================================
	ROUTINE: setCommandhandler
=================================================================== */

void ACS_EMF_DSDServer::setCommandhandler(acs_emf_commandhandler* aCmdhandler)
{
	DEBUG(1,"ACS_EMF_DSDServer::setCommandhandler - %s","Entering");
	m_pCmdHandler = aCmdhandler;
	DEBUG(1,"ACS_EMF_DSDServer::setCommandhandler - %s","Leaving");
}//end of setClienthandler


void ACS_EMF_DSDServer::setOperationalStateinPassive(int operationStateinPassive)
{
	DEBUG(1,"%s", " ACS_EMF_DSDServer::theOperationalStateinPassive - Entering");
	theOperationalStateinPassive=operationStateinPassive;
	DEBUG(1,"ACS_EMF_DSDServer::theOperationalStateinPassive - theOperationalStateinPassive = %d",theOperationalStateinPassive);
	DEBUG(1,"%s", "ACS_EMF_DSDServer::theOperationalStateinPassive - Leaving");
}
int ACS_EMF_DSDServer::getOperationalStateinPassive()
{
	return theOperationalStateinPassive;
}
int ACS_EMF_DSDServer::svc(void)
{
	int iRet;
	// Initialization
	iRet = EMF_RC_ERROR;
	// Debug message
	DEBUG(1,"%s","ACS_EMF_DSDServer::svc - Entering");
	// Listen
	isDSDPublished = m_cmdServer.listen();
	if(isDSDPublished == true){
		// Listen OK. Print info message
		DEBUG(1,"%s","ACS_EMF_DSDServer::svc - listen success");
		// Accept (return 0 if OK)
		iRet = serverAccept(m_cmdServer);
		// Check for error
		if(iRet != 0){
			DEBUG(1,"%s","ACS_EMF_DSDServer::svc - serverAccept() return with error [%d]", iRet);
		}
	}else{
		// Error from listen
		DEBUG(1,"%s","ACS_EMF_DSDServer::svc - Listening DSD server is Failed");
	}
	// Exit debug message
	DEBUG(1,"ACS_EMF_DSDServer::svc - %s","Leaving" );
	// Exit from method
	return (iRet);
}

// First of my solution (for back)
//int ACS_EMF_DSDServer::serverAccept(ACS_EMF_CmdServer &cmdServer)
//{	
//	bool bExit;
//	int bCheck;
//	int iCmdCode;
//	int iFlag;
//	ACE_INT16 iRetVal;
//	ACE_INT32 iAccepted;
//	int iRet;
//	ACS_DSD_Node remoteNodeInfo;
//	ACS_EMF_Cmd cmdReceived;
//	std::list<string> lstStrArgsOut;
//	// Initialization
//	bExit = false;
//	bCheck = false;
//	iCmdCode = -1;
//	iFlag = -1;
//	iRetVal = 1;
//	iAccepted = -1;
//	iRet = -1;
//	lstStrArgsOut.clear();
//	remoteNodeInfo.reset();
//	// Accept loop
//	while(bExit == false){
//		// Accept connection
//		iAccepted = cmdServer.accept(stopThreadFds);
//		// Check error
//		if(iAccepted == 1){
//			// ok. Get remote node info
//			iFlag = cmdServer.get_remote_node(remoteNodeInfo);
//			// Check for error
//			if(iFlag > -1){
//				// Print debug string
//				INFO(1,"Accepted connection request from SysId = %d, SystemName =%s, NodeSide=%d", remoteNodeInfo.system_id, remoteNodeInfo.system_name, remoteNodeInfo.node_side);
//				// Parsing received command
//
//				cmdServer.receive(cmdReceived);
//				// Decode command
//				iCmdCode = cmdReceived.commandCode();
//				// Execute command
//				iRetVal = executeCommand(iCmdCode, cmdReceived.commandArguments(), remoteNodeInfo, lstStrArgsOut);
//			}// if(iFlag > 0)
//			// Send response to client
//			ACS_EMF_Cmd cmdToSend(iRetVal);
//			// Check output args
//			if (lstStrArgsOut.size() > 0){
//				// Append out arguments list
//				cmdToSend.addArguments(lstStrArgsOut);
//			}
//			// TODO: controllare l'errore sulla send
//			cmdServer.send(cmdToSend);
//			// Debug message
//			DEBUG(1,"Send to client [%i]", iRetVal);
//			// Close
//			cmdServer.close();
//			// Reset flags
//			iRetVal = 1;
//			lstStrArgsOut.clear();
//		}else{
//			// Exit debug string
//			INFO(1,"%s","ACS_EMF_DSDServer::svc(): Service stop requested");
//			// Error from accept. Set exit flag
//			bExit = true;
//		}
//	} // while
//	// Exit from method
//	return(iRet);
//}

// After
int ACS_EMF_DSDServer::serverAccept(ACS_EMF_CmdServer &cmdServer)
{	
	bool bExit;
	int bCheck;
	int iCmdCode;
	int iFlag;
	ACE_INT16 iRetVal;
	ACE_INT32 iAccepted;
	int iRet;
	ACS_DSD_Node remoteNodeInfo;
	ACS_EMF_Cmd cmdReceived;
	std::list<string> lstStrArgsOut;
	// Initialization
	bExit = false;
	bCheck = false;
	iCmdCode = -1;
	iFlag = CMDSRV_ERROR;
	iAccepted = CMDSRV_ERROR;
	iRet = CMDSRV_ERROR;
	iRetVal = CMD_ERROR;
	lstStrArgsOut.clear();
	remoteNodeInfo.reset();

	DEBUG(1,"%s","ACS_EMF_DSDServer::serverAccept - Entering");

	// Accept loop
	while(bExit == false){
		// Accept connection
		iAccepted = cmdServer.accept(stopThreadFds);
		// Check error
		if(iAccepted == CMDSRV_OK){
			// ok. Get remote node info
			iFlag = cmdServer.get_remote_node(remoteNodeInfo);
			// Check for error
			if(iFlag >= 0){
				// Print debug string
				DEBUG(1,"ACS_EMF_DSDServer::serverAccept - Accepted connection request from SysId = %d, SystemName =%s, NodeSide=%d, NodeState=%d", remoteNodeInfo.system_id, remoteNodeInfo.system_name, remoteNodeInfo.node_side, remoteNodeInfo.node_state);
				// Parsing received command
				if(cmdServer.receive(cmdReceived)){
					// Decode command
					iCmdCode = cmdReceived.commandCode();
					// Execute command
					iRetVal = executeCommand(iCmdCode, cmdReceived.commandArguments(), remoteNodeInfo, lstStrArgsOut);
				}
				else
					DEBUG(1, "%s","ACS_EMF_DSDServer::serverAccept - Error in receiving data!!");

			}// if(iFlag >= 0)
			// Send response to client
			ACS_EMF_Cmd cmdToSend(iRetVal);
			// Check output args
			if (lstStrArgsOut.size() > 0){
				// Append out arguments list
				cmdToSend.addArguments(lstStrArgsOut);
			}
			// TODO: controllare l'errore sulla send
			if(cmdServer.send(cmdToSend) == true){
				// Debug message
				DEBUG(1,"ACS_EMF_DSDServer::serverAccept - Success send to client [%i]", iRetVal);
			}else{
				DEBUG(1,"ACS_EMF_DSDServer::serverAccept - Fail send to client [%i]", iRetVal);
			}
			// Close
			cmdServer.close();
			// Reset flags
			iRetVal = CMD_ERROR;
			lstStrArgsOut.clear();
		}else if(iAccepted == CMDSRV_STOPSIGNAL){
			// Stop signal received into accept.
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverAccept - Stop signal received!");
			bExit=true;
		}else{
			// Unknow error
			DEBUG(1,"ACS_EMF_DSDServer::serverAccept() - WARNING: accept fail with retcode[%i].", iAccepted);
			//bExit = true; // Continue
		}
	} // while
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverAccept - Leaving");
	// Exit from method
	return(iRet);
}

bool ACS_EMF_DSDServer::parserRequest(const ACS_DSD_Node &remoteNodeInfo)
{
	bool bRet;
	bool bDvdLocked;
	int32_t iApgOwner;
	// Initialization
	bRet = false;
	bDvdLocked = true;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;	
	ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
	// Check if global client info is valid
//		if(m_strctServerSession.remoteClientInfo.system_id != acs_dsd::SYSTEM_ID_UNKNOWN){
//		// A previous client has requested the session
//		if(remoteNodeInfo.system_id == m_strctServerSession.remoteClientInfo.system_id){
//			// Client is the same. Accept the connection
//			bRet = true;
//		}
//	}else{
	// Test dvd status
	pClienthandler->getDvdState(&iApgOwner, &bDvdLocked);
	if(bDvdLocked == false){
		// No client connected. Set client info
		//m_strctServerSession.remoteClientInfo.set(remoteNodeInfo.system_id, remoteNodeInfo.system_name, remoteNodeInfo.system_type, remoteNodeInfo.node_state, remoteNodeInfo.node_name, remoteNodeInfo.node_side);
		// Set flag to ok (accept connection)
		bRet = true;
	}else{
		// DVD is locked. Verify if the client is the owner
		if(remoteNodeInfo.system_id == iApgOwner){
			// Set return flag to true
			bRet = true;
		}
	}
	//}
	// exit from method
	return(bRet);
}

ACE_INT16 ACS_EMF_DSDServer::executeCommand(int iCmdCode, const std::list<string> &lstStrArgs, const ACS_DSD_Node &remoteNodeInfo, std::list<string> &lstStrArgsOut)
{
	ACE_INT16 iRet;
	// Initialization
	iRet = EMF_RC_ERROR;
	// DEBUG
	DEBUG(1,"%s", "ACS_EMF_DSDServer::executeCommand - Entering");
	// Switch command code
	switch(iCmdCode){
	case acs_emf_cmd_ns::EMF_CMD_CheckForDVDPresence:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_CheckForDVDPresence");
		// Check DVD hardware presence
		iRet = serverCheckForDVDPresence(remoteNodeInfo);
		break;
	case acs_emf_cmd_ns::EMF_CMD_CheckForDVDMediaPresence:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_CheckForDVDMediaPresence");
		// Check if media is inserted in DVD
		iRet = serverCheckForDVDMediaPresence(lstStrArgsOut);
		break;
	case acs_emf_cmd_ns::EMF_CMD_CopyToMedia:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_CopyToMedia");
			// Copy to DVD
		iRet = serverCopyToDVD(remoteNodeInfo, lstStrArgs, lstStrArgsOut, false);
		break;
	case acs_emf_cmd_ns::EMF_CMD_EraseAndCopyToMedia:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s","EMF_CMD_EraseAndCopyToMedia");
		// Copy to DVD
		iRet = serverCopyToDVD(remoteNodeInfo, lstStrArgs, lstStrArgsOut,true);
		break;
//	case acs_emf_cmd_ns::EMF_CMD_EraseAndCopyToMedia:
//		// Debug message
//		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_EraseAndCopyToDVD");
//		// Erase and copy to DVD
//		iRet = serverEraseAndCopyToDVD(remoteNodeInfo, lstStrArgs,  lstStrArgsOut);
//		break;
	case acs_emf_cmd_ns::EMF_CMD_FetchMediaInfo:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_FetchDVDMediaInfo");
		// Fetch media info
		iRet = serverFetchDVDMediaInfo(remoteNodeInfo, lstStrArgsOut);
		break;
	case acs_emf_cmd_ns::EMF_CMD_MountMedia:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_MountMedia");
		// Mount DVD (from unlockMedia)
		iRet = serverMountMediaOnDVDOwner(remoteNodeInfo,lstStrArgs);
		break;
	case acs_emf_cmd_ns::EMF_CMD_CheckAndMountMedia:
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_CheckAndMountMedia");
		iRet = serverMountMediaOnAccessibleNode(remoteNodeInfo,lstStrArgs);
		break;
	case acs_emf_cmd_ns::EMF_CMD_UnmountMedia:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_UnmountMedia");
		// Unmount DVD (from lockMedia)
		iRet = serverUnmountMediaOnDVDOwner(lstStrArgs);
		break;
	case acs_emf_cmd_ns::EMF_CMD_UnmountActiveMedia:
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_UnmountActiveMedia");
		iRet = serverUnmountActiveMediaOnDVDOwner();
		break;
	case acs_emf_cmd_ns::EMF_CMD_SyncMedia:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_SyncMedia");
		//Sync both the USB's
		iRet = serverSyncMedia();
		break;
	case acs_emf_cmd_ns::EMF_CMD_RemovePassiveShare:
		// Debug message
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_RemovePassiveShare");
		// remove the passive share
		iRet = serverRemovePassiveShare(lstStrArgs);
		break;
	case acs_emf_cmd_ns::EMF_CMD_PassiveShare:
                // Debug message
                DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_PassiveShare");
                // share passive /media path
                iRet = serverPassiveShare();
                break;
        case acs_emf_cmd_ns::EMF_CMD_SyncMediaOnPassive:
                DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_SyncMediaOnPassive");
                // share active media path
                iRet = serverSyncMediaOnPassive();
                break;
	case acs_emf_cmd_ns::EMF_CMD_FormatMedia:
	                // Debug message
	                DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Command received from client is cmdCode == %s", "EMF_CMD_FormatMedia");
	                // share passive /media path
	                iRet = serverFormatToMedia();
	                break;

	default:
		// Unknow command
		DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Unknow command received from client (code [%i]).", iCmdCode);
	}
	// DEBUG
	DEBUG(1,"ACS_EMF_DSDServer::executeCommand - Leaving (retcode [%i])", iRet);
	// Exit from method
	return(iRet);
}

// This method check if dvd is present on this node and then SET INTERNAL DVD DEVICE NAME (eg. /dev/sr0)
ACE_INT16 ACS_EMF_DSDServer::serverCheckForDVDPresence(const ACS_DSD_Node &remoteNodeInfo)
{
	bool bDvdLocked;
	int32_t iApgOwner;
	ACE_INT16 iRet;
	std::string	strDeveName;
	// Initialization
	bDvdLocked = true;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;	
	ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
	iRet = 0;
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDPresence - Entering");
	// Call check dvd
	iRet = ACS_EMF_Common::checkForDVDStatus(strDeveName);
	// CHECK FOR DVD Drive Availability
	if ((iRet & CHECKDVD_MASK_PRESENT) != EMF_RC_OK){
		// Set device name (eg. "/dev/sr0")
		ACS_EMF_Common::setDeviceName(strDeveName);
		// Get DVD status
		pClienthandler->getDvdState(&iApgOwner, &bDvdLocked);
		DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDPresence - getDvdState return iApgOwner[%i], bDvdLocked[%i]",iApgOwner, bDvdLocked);
		// Check for dvd locked
		if (bDvdLocked == true){
			// Check if the required unlockMedia is the same APG that has a last connection
			if(remoteNodeInfo.system_id == iApgOwner){
				// Force mask to unmounted (client receive DVD is unmounted and interpret it as "unlocked")
				iRet &= ~CHECKDVD_MASK_BUSY;
				DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDPresence - allowed for APG[%i] force mount", iApgOwner);
			}else{
				// unlocking the driver denied. Force mask to mounted (flag bDvdLocked == true)
				iRet |= CHECKDVD_MASK_BUSY;
				DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDPresence - unlockMedia denied for APG[%i] force Mount mask", remoteNodeInfo.system_id);
			}
		}
		// Debug message
		DEBUG(1, "ACS_EMF_Common::checkForDVDStatus - found DVD Drive on local node! The device file name is %s", ACS_EMF_Common::getDeviceName());
	}else{
		// DVD not found on this node.
		ERROR(1,"%s", "ACS_EMF_Common::checkForDVDStatus - did not find any DVD Drive on local node");
	}
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDPresence - Leaving");
	// Exit from method
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverCheckForDVDMediaPresence(std::list<string> &lstStrArgsOut)
{
	ACE_INT32 iRet;
	ACE_INT32 iFlag;

	// Initialization
	iRet = 1;	// Error
	iFlag = -1;

	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Entering");

	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Entering");
	ACE_INT32 mediumPresence = ACS_EMF_Common::checkForMediumPresence();
	if( ACS_EMF_Common::GetHWVariant() < 3){
		// Check medium presence
		if (mediumPresence == acs_emf_common::DVDMEDIA_NOT_PRESENT){
			// Message
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - No Media in DVD Drive");
			 iRet = EMFSERVER_ERROR;
		}else{
			// Media is in dvd
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Media is present in DVD Drive");
			//int previousUnmountStatus = ACS_EMF_Common::isMediaAlreadyMountedOnPassive();
			// TR HR44823 - BEGIN
			ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
			if (ACS_EMF_Common::getMediumType(mediumType) == -1){
				// Medium is unknow
				strcpy(mediumType,"UNKNOWN");
			}
			// Debug string
			DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Medium Type is [%s]",mediumType);
			// Insert media type in output args
			lstStrArgsOut.push_back(mediumType);
			// Verify if the DVD is empty: Here the media is present in the DVD
			iFlag = ACS_EMF_Common::isDVDEmpty();
			// Check if DVD is empty
			if(iFlag == acs_emf_common::DVDMEDIA_STATE_EMPTY){
				// DVD is empty
				lstStrArgsOut.push_back("EMPTY");
				// Set ret flag
				iRet = EMFSERVER_OK;
				DEBUG(1,"%s", "ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - DVD media state is EMPTY");
			}else if(iFlag == acs_emf_common::DVDMEDIA_STATE_NOT_EMPTY){
				// DVD is NOT empty
				lstStrArgsOut.push_back("NOTEMPTY");
				// Set ret flag
				iRet = EMFSERVER_OK;
				DEBUG(1,"%s", "ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - DVD media state is NOTEMPTY");
			}else{
				// Error. The other return code [ERROR == -1 and other] are considered errors. If increase the return code
				// enumerative, handle it.
				lstStrArgsOut.push_back("NOTEMPTY");
				DEBUG(1,"%s", "ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - isDVDEmpty fails. Force DVD media state to NOTEMPTY");
			}
		}
	}else{
		DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Entering for checking of nanoUSB on Passive Node:%d",theMediaOwnerinPassive);
		if(ACS_EMF_Common::checkForUSBStatus() == EMF_RC_OK )
		{
			theMediaOwnerinPassive = MEDIA_ON_PARTNER_NODE;
			iRet = EMFSERVER_OK;
			DEBUG(1,"ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - nanoUSB is present on Passive Node :%d",theMediaOwnerinPassive);
		}
		else
		{
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - No nanoUSB is present on Passive Node");
			iRet = EMFSERVER_ERROR;
		}
	}	
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCheckForDVDMediaPresence - Leaving");
	// Exit from method
	return(iRet);
}
ACE_INT16 ACS_EMF_DSDServer::serverCopyToDVD(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs, std::list<string> &lstStrArgsOut, bool format)
{
	int i;
	int iFlag;
	bool bDvdLocked;
	int32_t iApgOwner;
	ACE_INT32 iRet;
	ACS_EMF_ParamList paramLst;
	ACS_EMF_DATA srctData;
	std::list<string>::const_iterator itr;
	// Initialization
	i = EMF_RC_OK;
	iFlag = EMF_RC_ERROR;
	bDvdLocked = false;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	iRet = EMFSERVER_ERROR;	// Error
	itr = lstStrArgs.begin();
	paramLst.Clear();
	memset(&srctData, 0, sizeof(ACS_EMF_DATA));
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - Entering");

	if( ACS_EMF_Common::GetHWVariant() < 3){
		ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
		// Check the status of DVD. Verify that DVD is still present on this node
		// This check is necessary because a failover could be occurred after an unlockMedia

		std::string deviceFileName;
		ACE_INT16 dvd_status= ACS_EMF_Common::checkForDVDStatus(deviceFileName);
		if( (dvd_status & CHECKDVD_MASK_PRESENT) == 0){
			ERROR(1,"ACS_EMF_DSDServer::serverCopyToDVD - ExportToMedia failed! DVD is no longer present or no longer configured on this node (dvd_status == %d)!!",dvd_status);
			return ((dvd_status & CHECKDVD_MASK_NOTCONFIG) != 0)? EMFSERVER_MEDIANOTCONFIGURED: EMFSERVER_MEDIANOTPRESENT;
		}
		// Verify dvd owner
		pClienthandler->getDvdState(&iApgOwner, &bDvdLocked);
		if(bDvdLocked == true){
			if (iApgOwner == remoteNodeInfo.system_id){

			// Construct the command

			paramLst.CmdCode = (format == false)? EMF_CMD_TYPE_COPYTOMEDIA: EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA;
			// Append arguments list
			while(itr != lstStrArgs.end()){
				std::string arg = (*itr);
				paramLst.Data[i] = arg.c_str();
				++i;
				++itr;
			}
			// Set code
			srctData.Code = paramLst.CmdCode;
			paramLst.Encode();
			paramLst.getEncodedBuffer(srctData.Data);
			int result = pClienthandler->ExportFiles(remoteNodeInfo.system_id, remoteNodeInfo.node_side, &srctData);
			// Check error
			if(result == EMF_RC_OK){
				// Set ok flag
				if (ACS_EMF_Common::removeFilesOnPassive() != EMF_RC_OK){
					ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - Removing files on passive node failed!!");
				}
				iRet = EMFSERVER_OK;
			}else{
				// Error
				ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - Execution of ExportFiles() on passive node is failed");
			}	
			//Unmount fileM folder from passive node
			if(ACS_EMF_Common::unmountFileM() != EMF_RC_OK){
				ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - unmount of fileM failed on passive node");
			}
			// set result of ExportFiles
			lstStrArgsOut.push_back(pClienthandler->historyResult_text());
			lstStrArgsOut.push_back(pClienthandler->lastOperation_error_text());
			}else{ // QUI
				// INFO
				ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - DVD is locked (another apg use DVD)");
//				lstStrArgsOut.push_back(FAILED);
//				lstStrArgsOut.push_back("Media is busy");
				iRet = EMFSERVER_MEDIABUSY;
			}
		}else{
			ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - ExportToMedia failed! DVD is no longer locked !");
//			lstStrArgsOut.push_back(FAILED);
//			lstStrArgsOut.push_back("Media is no longer enabled");
			iRet = EMFSERVER_MEDIANOTMOUNTED;
		}
	} // (if HWVariant != 3)
	else{
		// Export To USB operation
		// Construct the command
		paramLst.CmdCode = (format == false)? EMF_CMD_TYPE_COPYTOMEDIA: EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA;
		itr = lstStrArgs.begin();
		int i = EMF_RC_OK;
		while(itr != lstStrArgs.end())
		{
			std::string arg = (*itr);
			paramLst.Data[i] = arg.c_str();
			++i;
			++itr;
		}
		srctData.Code = paramLst.CmdCode;
		paramLst.Encode();
		(void)paramLst.getEncodedBuffer(srctData.Data);

		//	DEBUG(1,"theOperationalStateinPassive = %d",theOperationalStateinPassive);
		int result;
		result = ACS_EMF_Common::checkForUSBStatus();
		if(result != EMF_RC_OK)
		{
			//	ACS_EMF_Cmd cmdToSend(result);
			//	cmdServer.send(cmdToSend);
			//TODO:  verify how to handle this case!!!
			//need to check whether it works or not
			iRet = EMFSERVER_MEDIANOTPRESENT;

		}
		DEBUG(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - Calling ExportFiles to perform TOUSB on passive");
		result = theClienthandlerPtr->ExportFiles(remoteNodeInfo.system_id, remoteNodeInfo.node_side, &srctData);
		if (result != EMF_RC_OK)
		{
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - ExportFiles() is Failed on other node");
			//Unmount fileM folder from passive node
			if(ACS_EMF_Common::unmountFileM() != EMF_RC_OK){
				ERROR(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - unmount of fileM failed on passive node");
			}
		}else{
			// its patch for ok result
			iRet = EMFSERVER_OK;
		}
		DEBUG(1,"%s","Media Operation execution is completed");
		
	}
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverCopyToDVD - Leaving");
	// Exit from method
	return(iRet);
}
ACE_INT16 ACS_EMF_DSDServer::serverEraseAndCopyToDVD(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs, std::list<string> &lstStrArgsOut)
{
	int i;
	bool bDvdLocked;
	int32_t iApgOwner;
	ACE_INT32 iRet;
	ACS_EMF_ParamList paramLst;
	ACS_EMF_DATA srctData;
	std::list<string>::const_iterator itr;
	// Initialization
	i=0;
	bDvdLocked = false;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	iRet = EMFSERVER_ERROR;	// Error
	itr = lstStrArgs.begin();
	paramLst.Clear();
	memset(&srctData, 0, sizeof(ACS_EMF_DATA));
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - Entering");
	if( ACS_EMF_Common::GetHWVariant() < 3){
		// Verify dvd owner
		ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
		pClienthandler->getDvdState(&iApgOwner, &bDvdLocked);
		if((bDvdLocked == true) && (iApgOwner == remoteNodeInfo.system_id)){	
			// Construct the command
			paramLst.CmdCode = EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA;
			// Append arguments list
			while(itr != lstStrArgs.end()){
				std::string arg = (*itr);
				paramLst.Data[i] = arg.c_str();
				++i;
				++itr;
			}
			// Set code
			srctData.Code = paramLst.CmdCode;
			paramLst.Encode();
			paramLst.getEncodedBuffer(srctData.Data);
			int result = pClienthandler->ExportFiles(remoteNodeInfo.system_id, remoteNodeInfo.node_side, &srctData);
			// Check error
			if(result == EMF_RC_OK){
				// Set ok flag
				iRet = EMFSERVER_OK;
			}else{
				// Error
					if (ACS_EMF_Common::removeFilesOnPassive() != EMF_RC_OK){
						ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - Removing files on passive node failed!!");
					}
				ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - Execution of ExportFiles() on passive node is failed");
			}
			//Unmount fileM folder from passive node
			if(ACS_EMF_Common::unmountFileM() != EMF_RC_OK){
				ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - unmount of fileM failed on passive node");
			}
			// set result of ExportFiles
			lstStrArgsOut.push_back(pClienthandler->historyResult_text());
			lstStrArgsOut.push_back(pClienthandler->lastOperation_error_text());
		}else{
			// INFO
			ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - DVD is locked (another apg use DVD)");
		}
	}else { // if(ACS_EMF_Common::GetHWVariant() != 3)
		// Erase and Export To DVD operation
		// Construct the command
		paramLst.CmdCode = EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA;
		itr = lstStrArgs.begin();
		int i = EMF_RC_OK;
		while(itr != lstStrArgs.end())
		{
			std::string arg = (*itr);
			paramLst.Data[i] = arg.c_str();
			++i;
			++itr;
		}
		srctData.Code = paramLst.CmdCode;
		paramLst.Encode();
		(void)paramLst.getEncodedBuffer(srctData.Data);

		// Check For USB Presence on Passive Node else raise an Alarm
		int retCode = EMF_RC_OK;
		retCode = ACS_EMF_Common::checkForUSBStatus();
		if(retCode != EMF_RC_OK)
		{
			//TODO: ????
			iRet = EMFSERVER_MEDIANOTPRESENT;
		}
		int result = theClienthandlerPtr->ExportFiles(remoteNodeInfo.system_id, remoteNodeInfo.node_side,&srctData);

		if (result != EMF_RC_OK)
		{
			iRet = result;
			ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - Execution of ExportFiles() is Failed on passive node");
		}else{
			// inserted from ITS patch (NO ERROR)
			iRet = EMFSERVER_OK;
		}
	/*	if (ACS_EMF_Common::removeFilesOnPassive() != 0)
		{
			ERROR(1,"%s","Removing files on passive node failed!!");
		}*/
		//Unmount fileM folder from passive node
		if(ACS_EMF_Common::unmountFileM() != EMF_RC_OK)
		{
			ERROR(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - unmount of fileM failed on passive node");
		}
		// COMMENTED BY ITS. The retcode is returned at exit from this method (return from "iRet")
		//ACS_EMF_Cmd cmdToSend(retCode);
		//cmdServer.send(cmdToSend);
	}
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverEraseAndCopyToDVD - Leaving");
	// Exit from method
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverFetchDVDMediaInfo(const ACS_DSD_Node &remoteNodeInfo, std::list<string> &lstStrArgsOut)
{
	ACE_INT32 iRet;
	bool bDvdLocked;
	int32_t iApgOwner;
	std::string mediaType;
	std::string freeSpace;
	std::string usedSpace;
	std::string totalSpace;
	// Initialization
	iRet = EMFSERVER_ERROR;	// Error
	bDvdLocked = false;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	lstStrArgsOut.clear();
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverFetchDVDMediaInfo - Entering");
	// Verify dvd owner

	int result = EMF_RC_OK;

	if( ACS_EMF_Common::GetHWVariant() < 3){
		ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
		pClienthandler->getDvdState(&iApgOwner, &bDvdLocked);
		if((bDvdLocked == true) && (iApgOwner == remoteNodeInfo.system_id)){
			// ITS PATCH: In gep5 version result is not initialized!
			// Get media data info
			result = pClienthandler->GetMediaInfoOnPassive(mediaType,freeSpace,usedSpace,totalSpace);
			if(result == EMF_RC_OK){

				// Add return arguments to list
				lstStrArgsOut.push_back(mediaType);
				lstStrArgsOut.push_back(freeSpace);
				lstStrArgsOut.push_back(usedSpace);
				lstStrArgsOut.push_back(totalSpace);
				// Set return flag
				iRet = EMFSERVER_OK;
			}
			else{
				// Error to execute GetMediaInfoOnPassive
				ERROR(1,"%s","ACS_EMF_DSDServer::serverFetchDVDMediaInfo - Execution of GetMediaInfoOnPassive() is Failed on passive node");
			}

		}else{
			// INFO
			ERROR(1,"%s","ACS_EMF_DSDServer::serverFetchDVDMediaInfo - DVD is locked (another apg use DVD)");
		}

	}else{
		//		DEBUG(1,"%s","push_back of myList Entering");
		//		myList.push_back((const char*)iMediaType);
		//		myList.push_back((const char*)iFreeSpace);
		//		myList.push_back((const char*)iUsedSpace);
		//		myList.push_back((const char*)iTotalSpace);
		//		DEBUG(1,"%s","push_back of myList Exiting");
	}

	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverFetchDVDMediaInfo - Leaving");
	// Exit from method
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverMountMediaOnDVDOwner(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs)
{
	ACE_INT32 iRet = EMF_RC_OK;
	//bool bCheck = false;
	//ACE_INT32 iFlag = EMF_RC_ERROR;
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Entering");
	if( ACS_EMF_Common::GetHWVariant() < 3){
		// Check if request is valid
		if(parserRequest(remoteNodeInfo) == false){
			// Refuse operation
			INFO(1,"ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Refuse unlockMedia from SysId = [%d], SystemName =[%s], NodeSide=[%d]. DVD is locked from another APG", remoteNodeInfo.system_id, remoteNodeInfo.system_name, remoteNodeInfo.node_side);
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
			return EMFSERVER_ERROR;
		}
		int isMediaMounted = ACS_EMF_Common::isDvdCorrectlyMounted(false);

		if (isMediaMounted == EMF_MEDIA_MOUNTEDBAD){
			INFO(1,"ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Refuse unlockMedia from SysId = [%d], SystemName =[%s], NodeSide=[%d]. DVD has been already mounted from another user", remoteNodeInfo.system_id, remoteNodeInfo.system_name, remoteNodeInfo.node_side);
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
			return EMFSERVER_MEDIAMOUNTEDONOTHERDIR;
		}
		if(isMediaMounted == EMF_MEDIA_UMOUNTREQ){
			// Require to umount from /data/opt/ap/internal_root/media/
			if( ACS_EMF_Common::unmountPassiveDVDOnActive() == EMF_RC_OK ){
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - unmounted the DVD which is mounted to FileM=media");
			}else{
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Failed to unmount the DVD which is mounted to FileM=media");
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
				return EMFSERVER_ERROR;
			}
		}	
		// IF Media is already mounted skip mount operation
		bool mountMode = false;
		int mount_status = -1;

		if(lstStrArgs.size() == 1){
			// Get mount mode
			mountMode = (*(lstStrArgs.begin())).compare("LOGICAL") == 0;
			mount_status = 0;
		}
		if (isMediaMounted != EMF_MEDIA_MOUNTED &&
						ACS_EMF_Common::isDVDEmpty() == acs_emf_common::DVDMEDIA_STATE_NOT_EMPTY)
		{
			int maxNoofRetrys = 5;
			while (mount_status != 0 && maxNoofRetrys > 0 /*&& !m_stopFlag*/ )
			{
				mount_status = ACS_EMF_Common::mountDVDOnPassive();

				if(mount_status != 0){
					ERROR(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - mountDVDOnPassive() failed! ");
					DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner -retry mount after 20 sec");

					ACE_OS::sleep(20);
					maxNoofRetrys--;
				}
			}
			if (maxNoofRetrys <= 0 && mount_status != 0)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::resetMedia, tried to mountDVDOnPassive for maximum number of times on passive node, but failed");
				ERROR(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - mountDVDOnPassive() failed! ");
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
				return EMFSERVER_ERROR;
			}
		}
		if( !mountMode && ACS_EMF_Common::setupNFSOnPassive() != 0) {
				ERROR(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Setup of NFS share for DVD data on passive node failed");
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
				return EMFSERVER_ERROR;
		}

		// No error
		ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);
		pClienthandler->setDvdState(remoteNodeInfo.system_id, true);

		ofstream emfUnlockfile(mediaOwnerFile);
		if(emfUnlockfile.ios::fail() != true) {
			ACE_INT32 nodeState = ACS_EMF_Common::getNodeState();
			emfUnlockfile << remoteNodeInfo.system_id <<" "<< nodeState << endl;
			emfUnlockfile.close();
		}
		else{
			ERROR(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - mount info file creation failed");
		}
	}else{ 
		//if( ACS_EMF_Common::GetHWVariant() != 3)
		int retCode = EMF_RC_OK;
		// Check For USB Presence on Passive Node else raise an Alarm
		retCode = ACS_EMF_Common::checkForUSBStatus();
		if(retCode != EMF_RC_OK)
		{

			//			ACS_EMF_Cmd cmdToSend(retCode);
			//			cmdServer.send(cmdToSend);

			DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
			return EMFSERVER_MEDIANOTPRESENT;
		}
		bool formatEnabled = false;
		if(lstStrArgs.size() == 1){
			// Get format mode
			formatEnabled = ((*(lstStrArgs.begin())).compare("yes") == 0);
		}
		DEBUG(1,"acs_emf_commandhandler::serverMountMediaOnDVDOwner -formatEbabled :%d ",formatEnabled);
		if(formatEnabled){
			DEBUG(1,"%s","acs_emf_commandhandler::serverMountMediaOnDVDOwner - Formation option is enabled in unlonckMedia");
			if (ACS_EMF_Common::isMountd() == TRUE){
				if(ACS_EMF_Common::umountUSBData() != EMF_RC_OK){
					DEBUG(1,"%s","ACS_EMF_Common::serverMountMediaOnDVDOwner - Failed to unmount media");
					return EMFSERVER_MEDIAUNMOUNTERROR;
				}
			}
			if(ACS_EMF_Common::formatMedium() == EMF_RC_OK){
				DEBUG(1,"%s","ACS_EMF_Common::serverMountMediaOnDVDOwner - formatMedium is successful !! ");
			}
			else{
				DEBUG(1,"%s","ACS_EMF_Common::serverMountMediaOnDVDOwner - Formating thumb drive failed");
				return EMFSERVER_MEDIAFORMATERROR;
			}
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		}
		//Setup NFS share for USB data on passive node
		retCode = ACS_EMF_Common::mountThumbDrive();
		if(retCode  != EMF_RC_OK)
		{
			DEBUG(1,"ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - %s","ACS_EMF_Common::mountThumbDrive failed");
			iRet = EMFSERVER_MEDIAMOUNTERROR;
		}
		else
		{
			DEBUG(1,"ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - %s","ACS_EMF_Common::mountThumbDrive Success");
		}
		DEBUG(1,"ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - theMediaOwnerinPassive in unlock == %d",theMediaOwnerinPassive);
		if(theMediaOwnerinPassive == MEDIA_ON_PARTNER_NODE)
		{
			if(retCode == EMF_RC_OK)
			{
				if((retCode = ACS_EMF_Common::setupNFSOnPassive()) != EMF_RC_OK)
				{
					ERROR(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Setup of NFS share for DVD data on passive node failed");
					iRet = EMFSERVER_ERROR;
				}else{ // ITS patch for return value. Server send "iRet" value.
					// OK
					//iRet = 0;
				}
			}
		}
	}
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnDVDOwner - Leaving");
	// Exit from method
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs)
{
	ACE_INT32 iRet = EMFSERVER_ERROR;
	ACE_INT32 result = EMF_RC_NOK;

	DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode - Entering");
	if(NULL != m_pCmdHandler){
		ACS_DSD_Node remoteNode = remoteNodeInfo;
		if((ACS_EMF_Common::GetHWVariant() < 3) && (lstStrArgs.size() == 1)) {
			//get_remote_node is not fetching remote node state, The node state is being passed from the remote node and the same will be updated here
			if((*(lstStrArgs.begin())).compare(DSD_ACTIVE_NODE) == 0) {
				remoteNode.node_state = acs_dsd::NODE_STATE_ACTIVE;
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode - Media-owner node has ACTIVE assignment");
			}
			else if((*(lstStrArgs.begin())).compare(DSD_PASSIVE_NODE) == 0) {
				remoteNode.node_state = acs_dsd::NODE_STATE_PASSIVE;
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode - Media-owner node has PASSIVE assignment");
			}
		}

		result = m_pCmdHandler->checkAndMountMediaOnAccessibleNode(remoteNode);
		if(result == EMF_RC_OK)
			iRet = EMFSERVER_OK;
		else if(result == EMF_RC_MOUNTINGFAILED)
			iRet = EMFSERVER_MEDIAMOUNTERROR;
		else if(result == EMF_RC_MEDIAACCESSALREADYENABLED)
			iRet = EMFSERVER_MEDIAALREADYENABLED;
		else if(result == EMF_RC_MEDIANOTMOUNTED)
			iRet = EMFSERVER_MEDIANOTMOUNTED ;
	}
	else{
		DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode - m_pCmdHandler is NULL");
	}
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverMountMediaOnAccessibleNode - Leaving");
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner(const std::list<string> &lstStrArgs)
{
	bool bUnlockMedia = false;
	ACE_INT32 iRet = EMFSERVER_ERROR;
	ACE_INT32 iFlag = EMF_RC_ERROR;
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Entering");
	if( ACS_EMF_Common::GetHWVariant() < 3)
	{
		ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(theClienthandlerPtr);

		ACE_INT32 argsSize = lstStrArgs.size();
		if(argsSize == 0){
			ERROR(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - invalid number of the arguments");
			return EMFSERVER_ERROR;
		}
		if( (*(lstStrArgs.begin())).compare(LOGICALMOUNT_MODE) == 0){
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Unmount mode is [L+]: Umount and remove AP owner of the DVD.");
			// Release DVD
			pClienthandler->setDvdState(acs_dsd::SYSTEM_ID_UNKNOWN, false);
			return EMFSERVER_OK;
		}
		// Get lockMedia type
		bUnlockMedia = (*(lstStrArgs.begin())).compare(DSDSERVER_LOCK_MEDIA) == 0;
		// umount DVD
		iFlag = ACS_EMF_Common::unmountPassiveDVDData(false);
		// Check for error
		if(iFlag != EMF_RC_OK){
			// Failed to unmount
			ERROR(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Remove Setup of NFS share for DVD data on passive node failed");
		}else{
			// Check lockMedia flag
			if(bUnlockMedia == true){
				// Reset server session. In this mode unlockMedia is possible only from APG that preview unlocked this server.
				// This behavior is required because for write on DVD is required umont the DVD. When DVD is umout for use
				// it from same APG, lockMedia don't destroy a session.
				//m_strctServerSession.remoteClientInfo.reset();
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Unmount mode is [L+]: Umount and remove AP owner of the DVD.");
				// Release DVD
				pClienthandler->setDvdState(acs_dsd::SYSTEM_ID_UNKNOWN, false);

				if(argsSize == 2){
					if((lstStrArgs.back()).compare(DSDSERVER_REMOVE_MEDIA_OWN_FILE) == 0){
						//To remove emf temporary mount information file present in cluster path
						if (ACE_OS::access(mediaOwnerFile, F_OK) == 0){
							ACE_OS::unlink(mediaOwnerFile);
						}
					}
				}
			}else{
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Unmount mode is [L]: Umount only. NOT remove AP owner of the DVD.");
			}
			// No error. Set exit flag
			iRet = EMFSERVER_OK;
		}

	}else{	//if( ACS_EMF_Common::GetHWVariant() != 3)
		int retCode = EMF_RC_OK;
		// Check For USB Presence on Passive Node else raise an Alarm
		retCode = ACS_EMF_Common::checkForUSBStatus();
		if(retCode != EMF_RC_OK)
		{
			//	ACS_EMF_Cmd cmdToSend(retCode);
			//	cmdServer.send(cmdToSend);
			//TODO:  verify how to handle this case!!!
			return EMFSERVER_MEDIANOTPRESENT;
		}
		//Setup NFS share for DVD data on passive node
		if(theMediaOwnerinPassive == MEDIA_ON_PARTNER_NODE){
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - media owner is MEDIA_ON_PARTNER_NODE");
				if(ACS_EMF_Common::unmountPassiveDVDData(false) != EMF_RC_OK){
					ERROR(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Remove Setup of NFS share for DVD data on passive node failed");
				}else{	// ITS Patch: server return "iRet"
					// OK
					iRet = EMFSERVER_OK;
					DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - unmountPassiveDVDData ok");
				}
		}
		//Setup NFS share for DVD data on passive node
			if((retCode = ACS_EMF_Common::umountUSBData()) != 0)
			{
				DEBUG(1,"ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - %s","umountUSBData failed");
			}else{	// ITS Patch: server return "iRet"
				// OK
				iRet = EMFSERVER_OK;
				DEBUG(1,"ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - %s","umountUSBData ok");
			}
	}
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountMediaOnDVDOwner - Exiting");
	// Exit from method
	return(iRet);
}

ACE_INT16 ACS_EMF_DSDServer::serverUnmountActiveMediaOnDVDOwner()
{
	int result = EMF_RC_ERROR;
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountActiveMediaOnDVDOwner - Entering");

	if( ACS_EMF_Common::GetHWVariant() < 3)
	{
		if(NULL != m_pCmdHandler){
			result = m_pCmdHandler->unmountDVDOnActiveNodeAndDisable(DSDSERVER_LOCK_MEDIA);
			if( result != EMF_RC_OK ) {
				DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountActiveMediaOnDVDOwner - unmount DVD On Active Node Failed");
			}
		}
	}
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverUnmountActiveMediaOnDVDOwner - Exiting");
	return(result);
}


ACE_INT16 ACS_EMF_DSDServer::serverSyncMedia()
{
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMedia - Entering");
	ACE_INT32 retCode = EMF_RC_OK;
	ACE_INT32 iRet = EMFSERVER_ERROR;
	retCode = ACS_EMF_Common::checkForUSBStatus();
	if(retCode != EMF_RC_OK){
		iRet = retCode;
	}
	retCode = theClienthandlerPtr->syncMediaOnPassive();
	if(retCode == EMF_RC_OK)
        {
               iRet = retCode; 
                DEBUG(1, "%s", "ACS_EMF_DSDServer::serverSyncMedia - is succsess ");
        }
	else
	{
                DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMedia - ACS_EMF_DSDServer::checksumForPassive on passive node failed");
       /*         if (ACS_EMF_Common::removeFilesOnPassive() != 0)
                {
                        DEBUG(1,"%s","Removing files on passive node failed!!");
                }*/
	}

	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMedia - Leaving");
	// Exit from method
	return(iRet);

}
ACE_INT16 ACS_EMF_DSDServer::serverRemovePassiveShare(const std::list<string> &lstStrArgs)
{
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverRemovePassiveShare - Entering");
	int retCode = EMF_RC_OK;
	bool unmontFlag=true;
	ACE_INT32 iRet=EMFSERVER_ERROR;
	ACE_INT32 argsSize = lstStrArgs.size();

	if(argsSize == 1) {
		if( (*(lstStrArgs.begin())).compare(SYNC_NFS_REMOVE) == 0) {
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverRemovePassiveShare - SYNC_OP");
			unmontFlag=false;
		}
	}

	if(unmontFlag == true) {
		retCode = ACS_EMF_Common::unmountPassiveDVDData(false);
		if(retCode != EMF_RC_OK) {
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverRemovePassiveShare - Removing passive share of usb failed!!");
			iRet = retCode;
		}
	}
	else {
		retCode = ACS_EMF_Common::unmountPassiveDVDData(false, false);
		if(retCode != EMF_RC_OK) {
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverRemovePassiveShare - Removing passive share of usb failed!!");
			iRet = retCode;
		}
	}

	// ITS patch: Server send "iRet" to client
	if(retCode == EMF_RC_OK)
		iRet = EMFSERVER_OK;
	// DEBUG MESSAGE
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverRemovePassiveShare - Leaving");
	// Exit from method
	return(iRet);
}
ACE_INT16 ACS_EMF_DSDServer::serverPassiveShare()
{
        DEBUG(1,"%s","ACS_EMF_DSDServer::serverPassiveShare - Entering");
        int retCode = EMF_RC_OK;
        ACE_INT32 iRet= EMFSERVER_ERROR;
        retCode = ACS_EMF_Common::setupNFSOnPassive();
        if(retCode != EMF_RC_OK)
        {
                DEBUG(1,"%s","ACS_EMF_DSDServer::serverPassiveShare - sharing passive media path of usb failed!!");
                iRet = retCode;
        }
        if(retCode == EMF_RC_OK)
                iRet = EMFSERVER_OK;
        // DEBUG MESSAGE
        DEBUG(1,"%s","ACS_EMF_DSDServer::serverPassiveShare - Leaving");
        // Exit from method
        return(iRet);
}
ACE_INT16 ACS_EMF_DSDServer::serverSyncMediaOnPassive()
{
        DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMediaOnPassive - Entering");
        int retCode = EMF_RC_OK;
        ACE_INT32 iRet= EMFSERVER_ERROR;
        if(ACS_EMF_Common::checkForUSBStatus() == 0 )
	{
		retCode = theClienthandlerPtr->syncMediaOnActive();
		if(retCode == EMF_RC_OK) {
			iRet = EMFSERVER_OK;
			ACS_EMF_Common::setMediaFlag(Sync_Pos,Sync);
		}
		else {
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMediaOnPassive - Syncing of media content on passive node failed!!");
			iRet = retCode;
		}
	}
        DEBUG(1,"%s","ACS_EMF_DSDServer::serverSyncMediaOnPassive - Leaving");
        return(iRet);
}
ACE_INT16 ACS_EMF_DSDServer::serverFormatToMedia()
{
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverFormatToMedia - Entering");
	int retCode = EMF_RC_OK;
	ACE_INT32 iRet= EMFSERVER_ERROR;
	if(ACS_EMF_Common::checkForUSBStatus() == 0 )
	{
		retCode = ACS_EMF_Common::formatMedium();
		if(retCode != EMF_RC_OK)
		{
			DEBUG(1,"%s","ACS_EMF_DSDServer::serverFormatToMedia - Formatting failed");
			iRet = retCode;
		}
		if(retCode == EMF_RC_OK)
			iRet = EMFSERVER_OK;
	}
	DEBUG(1,"%s","ACS_EMF_DSDServer::serverFormatToMedia - Leaving");
	return(iRet);
}
