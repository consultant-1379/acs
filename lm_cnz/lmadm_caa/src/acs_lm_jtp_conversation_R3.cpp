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
#include "acs_lm_jtp_conversation_R3.h"
#include "acs_lm_tra.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>
#include <ace/Task.h>
#include <ace/Event_Handler.h>
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>
#include <ace/Process.h>
#define ACS_LM_SEND_THREAD_TIMEOUT 10*1000*1000
#define ACS_LM_SEND_THREAD_TERMINATION_TIMEOUT 10*1000*1000

ACE_Recursive_Thread_Mutex ACS_LM_JTP_Conversation_R3::theStopMutex;

#define cleanUpProtoCol 1 // Protocol

bool lminstCmnd ;
/*=================================================================
	ROUTINE: ACS_LM_JTP_Conversation_R3 constructor
=================================================================== */
ACS_LM_JTP_Conversation_R3::ACS_LM_JTP_Conversation_R3()
:activeNodes(NULL),
 noOfActiveNodes(0),
 pThrMgr(NULL),
 stopEvent(NULL),
 cpService(""),
 threadData(NULL)
{
	INFO("Entering %s","In ACS_LM_JTP_Conversation_R3()");
	pThrMgr = new(std::nothrow) ACE_Thread_Manager();
	theStopSignal = false;
	INFO("Leaving %s","In ACS_LM_JTP_Conversation_R3()");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_JTP_Conversation_R3 destructor
=================================================================== */
ACS_LM_JTP_Conversation_R3::~ACS_LM_JTP_Conversation_R3()
{
	INFO("Entering %s","In ACS_LM_JTP_Conversation_R3()");
	disconnect();
	INFO("Leaving %s","In ACS_LM_JTP_Conversation_R3()");
}//end of constructor
/*=================================================================
	ROUTINE: connect
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::connect(const std::string& cpService,
							JtpNode* node)
{
        DEBUG("ACS_LM_JTP_Conversation_R3::connect() %s", "Entering");
	bool connected = false;
	if(updateActiveNodes(node, cpService))
	{
		if(noOfActiveNodes)
		{
			this->cpService = cpService;
			connected = true;
		}
	}
	// Raise Event when unable to connect to JTP
	if(!connected)
	{
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
	}
	return connected;
	DEBUG("ACS_LM_JTP_Conversation_R3::connect() %s", "Leaving");
}//end of connect

/*===============================================================================
        ROUTINE: getProtocolFromCP
	Description: This method fetch the protocol value from CP and returns it. 
=============================================================================== */
unsigned short ACS_LM_JTP_Conversation_R3::getProtocolFromCP(const std::string& cpService)
{
	unsigned short protocolVer_CP = 1;
       	DEBUG("ACS_LM_JTP_Conversation_R3::getProtocolFromCP() %s","Entering");

	acs_lm_cpdata myCPData;
	myCPData.setService(cpService);
	/* noOfActiveNodes = number of BC's + number of CP's; BC and CP properties are stored into activeNodes array. 
	   So to set CP node, considering the last element in the array */	
	myCPData.setJTPNode(activeNodes[noOfActiveNodes-1]);
	acs_lm_senddata *mySendDataPtr = new acs_lm_senddata(myCPData,pThrMgr);
	protocolVer_CP = mySendDataPtr->getProtocolFromCP();
	delete mySendDataPtr;

	DEBUG("ACS_LM_JTP_Conversation_R3::getProtocolFromCP() %s, protocol from CP : %d", "Leaving",protocolVer_CP);
return protocolVer_CP;
}//end of getProtocolFromCP

/*=================================================================
	ROUTINE: disconnect
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::disconnect()
{
	INFO("Entering %s","ACS_LM_JTP_Conversation_R3::disconnect ");

	if(stopEvent != NULL)
	{
		stopEvent->signal();
		delete stopEvent;
		stopEvent = 0;
	}

	if( threadData != NULL )
	{
	    delete[] threadData;
	    threadData = 0;
	}

	if( pThrMgr != NULL )
	{
		delete pThrMgr;
		pThrMgr = 0;
	}
	if( activeNodes != NULL)
	{
		delete[] activeNodes;
		activeNodes = NULL;
	}

	noOfActiveNodes = 0;

	INFO("Leaving %s","ACS_LM_JTP_Conversation_R3::disconnect ");
	return true;
}//end of disconnect

/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::send(	const std::list<LkData*>& lkDataList,
					int lmMode,
					const std::list<LkData*>& discLklist,
					const std::list<LkData*>& connLklist)
{
	bool sent = false;

	if(noOfActiveNodes > 0)
	{
		//stopEvent = new ACE_Event();
		buffer32List = encode(lkDataList, lmMode);
		if(theLMInstCommandFlag)//LMcleanup Start
		{
			DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send...theLMInstCommandFlag is true");
			if ((int)discLklist.size()!=0)
			{
				DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send...Building disconnection list");
				discbuffer32List = encode(discLklist,lmMode,true);
			}

			if ((int)connLklist.size()!=0)
			{
				DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send...Building connection list");
				connbuffer32List = encode(connLklist,lmMode,true);
			}
		}//LMcleanup End

		for(int i=0; i < noOfActiveNodes; i++)
		{
			DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send...int i=0; i<noOfActiveNodes; i++");
			acs_lm_cpdata myCPData;
			myCPData.setBufferList(buffer32List);
			myCPData.setDiscbufferList(discbuffer32List);
			myCPData.setConnbufferList(connbuffer32List);
			myCPData.setRetries(3);
			myCPData.setService(cpService);
			myCPData.setJTPNode(activeNodes[i]);
			acs_lm_senddata *mySendDataPtr = new acs_lm_senddata(myCPData,pThrMgr);
			theCPSendDataList.push_back(mySendDataPtr);
			if(theLMInstCommandFlag == true)
			{
				DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send() Setting LMINST Command  flag to true");
				mySendDataPtr->setLMInstCommandOption(true);
			}
			else
			{
				DEBUG ("%s","ACS_LM_JTP_Conversation_R3::send() Setting LMINST Command flag to false");
				mySendDataPtr->setLMInstCommandOption(false);
			}
			mySendDataPtr->activate();
			//theLMInstCommandFlag = false;
		}

		bool myContinue = true;
		while(myContinue)
		{
			ACE_Time_Value tv( 1 );
			int ret =  pThrMgr->wait( &tv , 0, 0);
			if( ret == 0 )
			{
				DEBUG("ACS_LM_JTP_Conversation_R3::send() %s","All CP threads work completed");
				sent = true;
				myContinue = false;
				clearCPSendDataList();
				//clearbuffers(buffer32List,discbuffer32List,connbuffer32List); // Commented as buffers are cleared else where
			}
			else
			{
				if(getStopSignal() == true)
				{
					myContinue = false;
					sendStopSignalCPSendDataList();
					DEBUG("ACS_LM_JTP_Conversation_R3::send() %s","Before wait for all CP threads ");
					pThrMgr->wait();
					DEBUG("ACS_LM_JTP_Conversation_R3::send() %s","After wait for all CP threads ");
					clearCPSendDataList();
				}
			}
		}
		DEBUG("ACS_LM_JTP_Conversation_R3::send() %s","clearbuffers");
		clearbuffers(buffer32List,discbuffer32List,connbuffer32List); // Commented as buffers are cleared else where
		disconnect();

	}
	return sent;
}//end of send
void ACS_LM_JTP_Conversation_R3::clearCPSendDataList()
{
	DEBUG("Entering %s","ACS_LM_JTP_Conversation_R3::clearCPSendDataList() ");
	std::list<acs_lm_senddata*>::iterator myIter = theCPSendDataList.begin();
	std::list<acs_lm_senddata*>::iterator myIterEnd = theCPSendDataList.end();

	for(;myIter != myIterEnd;)
	{
		acs_lm_senddata* myPtr = (*myIter);
		delete myPtr;
		theCPSendDataList.erase(myIter++);
	}
	DEBUG("Leaving %s","ACS_LM_JTP_Conversation_R3::clearCPSendDataList() ");
}
void ACS_LM_JTP_Conversation_R3::sendStopSignalCPSendDataList()
{
	DEBUG("Entering %s","ACS_LM_JTP_Conversation_R3::sendStopSignalCPSendDataList() ");
	std::list<acs_lm_senddata*>::iterator myIter = theCPSendDataList.begin();
	std::list<acs_lm_senddata*>::iterator myIterEnd = theCPSendDataList.end();
	acs_lm_senddata* myPtr = 0;
	for(;myIter != myIterEnd;++myIter)
	{
		myPtr = *(myIter);
		myPtr->setStopSignal(true);
	}
	DEBUG("Leaving %s","ACS_LM_JTP_Conversation_R3::sendStopSignalCPSendDataList() ");
}
/*=================================================================
	ROUTINE: updateActiveNodes
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::updateActiveNodes(JtpNode* node, const std::string& cpService)
{
//	DEBUG("In ACS_LM_JTP_Conversation_R3::updateActiveNodes():node = %d, cpService = %s",node->id,cpService.c_str());
	bool bcUpdated = false;
	bool cpUpdated = false;
	bool updated = true;
	int aptType = ACS_LM_Common::getApt();
	DEBUG("APT Type is: %d ",aptType);

	if(node == NULL)
	{
		//Reset active nodes array
		noOfActiveNodes = 0;
		if( activeNodes != NULL )
		{
			delete[] activeNodes;
			activeNodes = NULL;
		}	

		//Query BC blades
		ACS_JTP_Conversation_R3A* bcJtpConv = new ACS_JTP_Conversation_R3A((char*)cpService.c_str(), 1024);

		ACS_JTP_Conversation_R3A::JTP_Node* tempBCReachableNodes = NULL;
		std::vector<ACS_JTP_Conversation_R3A::JTP_Node> bcReachableNodes;
		std::vector<ACS_JTP_Conversation_R3A::JTP_Node> bcUnreachableNodes;

		for(int bcAttempt = 0; bcAttempt < 3; bcAttempt++)
		{
			if(bcJtpConv->query(ACS_JTP_Conversation_R3A::SYSTEM_TYPE_BC,
					bcReachableNodes,
					bcUnreachableNodes,
					5000))
			{
				bcUpdated = true;
				break;
			}
		}


		if(!bcUpdated)
		{
			DEBUG("%s","updateActiveNodes(): bcJtpConv->query() failed");
			updated = false;
		}
		else
		{
			DEBUG("Entering else of bcReachableNodes: %d ",bcReachableNodes.size());

			//Count only the active nodes for Blades
			tempBCReachableNodes = new ACS_JTP_Conversation_R3A::JTP_Node[bcReachableNodes.size()];
			for(ACE_UINT32 i=0; i<bcReachableNodes.size(); i++)
			{
				if(bcReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE)
				{
					tempBCReachableNodes[i].system_id = bcReachableNodes[i].system_id;
					tempBCReachableNodes[i].node_state   = bcReachableNodes[i].node_state;
					tempBCReachableNodes[i].system_type  = bcReachableNodes[i].system_type;
					DEBUG("BCReachableNodes[%d].system_id = %d,", i,tempBCReachableNodes[i].system_id);
					DEBUG("BCReachableNodes[%d].node_state = %d", i,tempBCReachableNodes[i].node_state);
					DEBUG("BCReachableNodes[%d].system_type = %d", i,tempBCReachableNodes[i].system_type);
					noOfActiveNodes++;
				}
			}
			DEBUG("%s","Leaving else of bcReachableNodes ");
		}

		delete bcJtpConv;

		//Query CP blades
		ACS_JTP_Conversation_R3A* cpJtpConv = new ACS_JTP_Conversation_R3A((char*)cpService.c_str(), 1024);
		std::vector<ACS_JTP_Conversation_R3A::JTP_Node> cpReachableNodes;
		std::vector<ACS_JTP_Conversation_R3A::JTP_Node> cpUnreachableNodes;

		for(int cpAttempt=0; cpAttempt<3; cpAttempt++)
		{
			if(cpJtpConv->query(ACS_JTP_Conversation_R3A::SYSTEM_TYPE_CP,
					cpReachableNodes,
					cpUnreachableNodes))
			{
				cpUpdated = true;
				break;
			}
		}



		if(!cpUpdated)
		{
			DEBUG("%s","updateActiveNodes() cpJtpConv->query() failed");
			updated = false;
		}
		else
		{
			//Count only the active nodes SPXs
			DEBUG("Entering else of cpReachableNodes: %d ",cpReachableNodes.size());
			for(int i=0; i<(int)cpReachableNodes.size(); i++)
			{
				if(cpReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE)
				{
					DEBUG("%s","updateActiveNodes() cpJtpConv->query() CP State is Executive");
					DEBUG("CPReachableNodes[%d].system_id = %d,", i,cpReachableNodes[i].system_id);
					DEBUG("CPReachableNodes[%d].node_state = %d", i,cpReachableNodes[i].node_state);
					DEBUG("CPReachableNodes[%d].system_type = %d", i,cpReachableNodes[i].system_type);
					noOfActiveNodes++;

				} //Enable License Distribution In StandBy CP during LM Server Restart
				else if((aptType == 2) && (cpReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_PASSIVE))
				{
					DEBUG("%s","updateActiveNodes() cpJtpConv->query() CP State is Standby");
					DEBUG("CPReachableNodes[%d].system_id = %d,", i,cpReachableNodes[i].system_id);
					DEBUG("CPReachableNodes[%d].node_state = %d", i,cpReachableNodes[i].node_state);
					DEBUG("CPReachableNodes[%d].system_type = %d", i,cpReachableNodes[i].system_type);
					noOfActiveNodes++;

				}

			}
			DEBUG("%s","Leaving else of cpReachableNodes ");


		}

		if(noOfActiveNodes > 0)
		{
			DEBUG(" ACS_LM_JTP_Conversation_R3::updateActiveNodes() noOfActiveNodes =%d ", noOfActiveNodes);

			//Create new array of nodes for avtive blades & cps
			activeNodes =  new ACS_JTP_Conversation_R3A::JTP_Node[noOfActiveNodes];

			// Copy all the active nodes to activeNodes array
			int anIndex = 0;
			DEBUG("ACS_LM_JTP_Conversation_R3::updateActiveNodes() %s","updating bcReachableNodes");
			for(ACE_UINT32 i=0; i<bcReachableNodes.size(); i++)
			{
				if((tempBCReachableNodes) && (tempBCReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE))
				{
					activeNodes[anIndex].system_id = tempBCReachableNodes[i].system_id;
					activeNodes[anIndex].node_state   = tempBCReachableNodes[i].node_state;
					activeNodes[anIndex].system_type  = tempBCReachableNodes[i].system_type;
					anIndex++;
				}
			}
			DEBUG("ACS_LM_JTP_Conversation_R3::updateActiveNodes() %s","updating cpReachableNodes");
			for(ACE_UINT32 j=0; j<cpReachableNodes.size(); j++)
			{
				if((cpReachableNodes[j].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE) || ((aptType == 2) && (cpReachableNodes[j].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_PASSIVE)))
				{
					activeNodes[anIndex].system_id = cpReachableNodes[j].system_id;
					activeNodes[anIndex].node_state = cpReachableNodes[j].node_state;
					activeNodes[anIndex].system_type = cpReachableNodes[j].system_type;
					anIndex++;
				}//Populate the object
			}
		}

		delete cpJtpConv;
		delete [] tempBCReachableNodes;
	}
	else
	{
		DEBUG("In ACS_LM_JTP_Conversation_R3::updateActiveNodes():node = %d, cpService = %s",node->id,cpService.c_str());
		int bnIndex = 0;
		if ((aptType == 2) && ((ACS_JTP_Conversation_R3A::JTP_SystemType)node->type == ACS_LM_JTP_NODE_TYPE_CP)) //Enable License Distribution In StandBy CP Restart
		{
			DEBUG("%s","Query CP for Reachable Nodes");
			ACS_JTP_Conversation_R3A* cpJtpConv = new ACS_JTP_Conversation_R3A((char*)cpService.c_str(), 1024);
			std::vector<ACS_JTP_Conversation_R3A::JTP_Node> CPReachableNodes;
			std::vector<ACS_JTP_Conversation_R3A::JTP_Node> CPUnreachableNodes;
			for(int cpAttempt=0; cpAttempt<3; cpAttempt++)
			{
				if(cpJtpConv->query(ACS_JTP_Conversation_R3A::SYSTEM_TYPE_CP,        //Query is req as DSD provides cp type as default as unknown
						CPReachableNodes,
						CPUnreachableNodes))
				{
					cpUpdated = true;
					DEBUG("%s","Break the Loop");
					break;
				}
			}
			if(!cpUpdated)
			{
				DEBUG("%s","updateActiveNodes() cpJtpConv->query() failed");
				updated = false;
			}
			for(int i=0; i<(int)CPReachableNodes.size(); i++)
			{
				if((CPReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE) || (CPReachableNodes[i].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_PASSIVE))
				{
					noOfActiveNodes++;

				}

			}
			if(noOfActiveNodes > 0)
			{
				DEBUG("ACS_LM_JTP_Conversation_R3::updateActiveNodes() noOfActiveNodes =%d ", noOfActiveNodes);
				activeNodes =  new ACS_JTP_Conversation_R3A::JTP_Node[noOfActiveNodes];
				for(ACE_UINT32 j=0; j<CPReachableNodes.size(); j++)
				{
					DEBUG("%s","Start Loop");
					if((CPReachableNodes[j].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE) || (CPReachableNodes[j].node_state == ACS_JTP_Conversation_R3A::NODE_STATE_PASSIVE))
					{
						DEBUG("%s","Populate the Reachble node structure");
						activeNodes[bnIndex].system_id = CPReachableNodes[j].system_id;
						activeNodes[bnIndex].node_state = CPReachableNodes[j].node_state;
						activeNodes[bnIndex].system_type = CPReachableNodes[j].system_type;
						bnIndex++;
					}
					DEBUG("%s","End Loop");
				}
			}
			delete cpJtpConv;
			cpJtpConv = 0;
		}
		else
		{
			activeNodes = new ACS_JTP_Conversation_R3A::JTP_Node[1];
			activeNodes[bnIndex].system_id = node->id;
			activeNodes[bnIndex].node_state = ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE;
			activeNodes[bnIndex].system_type = (ACS_JTP_Conversation_R3A::JTP_SystemType)node->type;
			noOfActiveNodes = 1;

		}

	}
	DEBUG("%s","Leaving Function updateActiveNodes");
	return updated;
}//end of updateActiveNodes

/*=================================================================
	ROUTINE: clearbuffers
=================================================================== */
void ACS_LM_JTP_Conversation_R3::clearbuffers(std::list<Buffer32*>& normalList,std::list<Buffer32*>& disckeyList,std::list<Buffer32*>& connkeyList )
{
	INFO("%s","Entering ACS_LM_JTP_Conversation_R3::clearbuffers");
	for(std::list<Buffer32*>::iterator it = disckeyList.begin();
			it != disckeyList.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	disckeyList.clear();

	for(std::list<Buffer32*>::iterator it = connkeyList.begin();
			it != connkeyList.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	connkeyList.clear();

	for(std::list<Buffer32*>::iterator it = normalList.begin();
			it != normalList.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	normalList.clear();
	INFO("%s","Leaving ACS_LM_JTP_Conversation_R3::clearbuffers");
	return ;
}//end of clearbuffers


/*=================================================================
	ROUTINE: connect
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::connect(ACS_JTP_Conversation_R3A* jtpConv,
					 ACS_JTP_Conversation_R3A::JTP_Node* node,
					 unsigned short &protoCol)
{
	DEBUG("%s","In ACS_LM_JTP_Conversation_R3::connect()");
	bool connected = false;

	if(node != NULL)
	{
		unsigned short ud1 =protoCol;//lmcleanup
		unsigned short ud2 = 0;
		DEBUG("%s","ACS_LM_JTP_Conversation_R3::Connect : jtpConv.jexinitreq(node, ud1, ud2)");
		if(jtpConv->jexinitreq(node, ud1, ud2))
		{
			unsigned short ret = 0;
			unsigned short rud1 = 0;
			unsigned short rud2 = 0;
			if(jtpConv->jexinitconf(rud1, rud2, ret))
			{
				connected = true;
			}
			else
			{
				//Dont use this one at present, until the problem is solved by DSD.
				//jtpConv.jexdiscind(rud1, rud2, ret);
			}
			DEBUG ("ACS_LM_JTP_Conversation_R3::connect() rud1 = %d",rud1);
			protoCol = rud1 ; //LMcleanup
		}
	}

	if(!connected)
	{

		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
	}
	DEBUG("%s","ACS_LM_JTP_Conversation_R3::connect():Leaving");
	return connected;
}//end of connect
/*=================================================================
	ROUTINE: disconnect
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::disconnect(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node)
{

	INFO("%s","ACS_LM_JTP_Conversation_R3::disconnect()ENTERING");
	node = NULL;
	bool disconnected = true;
	ACE_UINT16 ret = 0;
	ACE_UINT16 rud1 = 0;
	ACE_UINT16 rud2 = 0;

	jtpConv->jexdiscreq(rud1, rud2, ret);
	disconnected = true;
	INFO("%s","ACS_LM_JTP_Conversation_R3::disconnect()LEAVING")
	return disconnected;
}//end of disconnect
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::send(	ACS_JTP_Conversation_R3A* jtpConv,
					ACS_JTP_Conversation_R3A::JTP_Node* node,
					const std::list<Buffer32*>& bufferList,
					functionCode fnCode)//lmcleanup
{
	bool sent = true;

	ACE_UINT16 ud1 = fnCode;      //LMcleanup
	ACE_UINT16 ud2 = 0;

	//Send all the 32K buffers
	for(std::list<Buffer32*>::const_iterator it = bufferList.begin();
			it != bufferList.end(); ++it)
	{
		Buffer32* buffer32 = (*it);

		if(jtpConv->jexdatareq(ud1, ud2, buffer32->size,(char*)buffer32->buffer))
		{
			ACE_UINT16 rud1 = 0;
			ACE_UINT16 rud2 = 0;
			ACE_TCHAR *msg = NULL;
			ACE_UINT16 size = 0;
			DEBUG("%s","connect() jtpConv.jexdataind(rud1, rud2, size, msg)");
			bool ret = jtpConv->jexdataind(rud1, rud2, size, msg);
			if( ret == false )
			{
				sent = false;
			}
		}
		else
		{
			sent = false ;
		}
		DEBUG ("buffer size is :%d", buffer32->size);
	}
	DEBUG("ACS_LM_JTP_Conversation_R3::send():node->system_id :%d",node->system_id);

	if(!sent)
	{
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
	}
	return sent;
}//end of send
/*=================================================================
	ROUTINE: send
=================================================================== */
bool ACS_LM_JTP_Conversation_R3::send(ACS_JTP_Conversation_R3A* jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node,const std::list<LkData*>& lkDataList, int lmMode)
{
	node = NULL;
	bool sent = true;
	ACE_UINT16 ud1 = 0;
	ACE_UINT16 ud2 = 0;

	buffer32List_Job = encode(lkDataList, lmMode);

	//Send all the 32K buffers
	for(std::list<Buffer32*>::const_iterator it = buffer32List_Job.begin();
			it != buffer32List_Job.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		if(jtpConv->jexdatareq(ud1, ud2, buffer32->size,(char*)buffer32->buffer))
		{
			ACE_UINT16 rud1 = 0;
			ACE_UINT16 rud2 = 0;
			ACE_TCHAR * msg = NULL;
			ACE_UINT16 size = 0;
			if(!jtpConv->jexdataind(rud1, rud2, size, msg))
			{
				sent = false;
			}
		}
	}

	//report event about JTP connection problem
	if(!sent)
	{
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
	}
	return sent;
}//end of send

void ACS_LM_JTP_Conversation_R3::setStopSignal(bool aStopFlag)
{
	ACS_LM_JTP_Conversation_R3::theStopMutex.acquire();
	theStopSignal = aStopFlag;
	ACS_LM_JTP_Conversation_R3::theStopMutex.release();
}

bool ACS_LM_JTP_Conversation_R3::getStopSignal()
{
	bool myFlag = false;
	ACS_LM_JTP_Conversation_R3::theStopMutex.acquire();
	myFlag = theStopSignal;
	ACS_LM_JTP_Conversation_R3::theStopMutex.release();
	return myFlag;

}
