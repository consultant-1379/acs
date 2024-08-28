/*=================================================================== */
/**
@file CUTE_ACS_JTP_Conversation_R3A.cpp

This file contains the implementation of CUTE_ACS_JTP_Conversation_R3A class.

@version 1.0.0

@documentno CAA 109 0870

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       10/18/2010     XTANAGG   APG43 on Linux.
**/
/*=================================================================== */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_JTP_Conversation_R3A.h"

ACS_JTP_Conversation_R3A* CUTE_ACS_JTP_Conversation_R3A::C1;

void CUTE_ACS_JTP_Conversation_R3A::testTimeout()
{
	//Set a timeout value.
	C1->setTimeOut(3);
}

void CUTE_ACS_JTP_Conversation_R3A::testRetries()
{
	//Specify the number of retries.
	C1->setNoOfTries(2);
}

void CUTE_ACS_JTP_Conversation_R3A::testDelay()
{
	//Specify the delay between the different retries.
	C1->setDelayTime(1);
}

void CUTE_ACS_JTP_Conversation_R3A::testSide()
{
	//Get the side of the ACS_JTP_Conversation_R3A object.
	ASSERTM("Side failed\n", (C1->Side() == ACS_JTP_Conversation_R3A::JTPClient));
}

void CUTE_ACS_JTP_Conversation_R3A::testState()
{
	//Get the state of the ACS_JTP_Conversation_R3A object.
	ASSERTM("State failed\n", (C1->State() == ACS_JTP_Conversation_R3A::StateJobRunning));
}

void CUTE_ACS_JTP_Conversation_R3A::testJexinitreq()
{
	unsigned short U1=5, U2=6;

	//Invoke jexinitreq on conversation object.
	ASSERTM("jexinitreq failed\n", (C1->jexinitreq(U1, U2) == true));

}

void CUTE_ACS_JTP_Conversation_R3A::testJexinitreqNode()
{
	unsigned short U1=5, U2=6;

	//Create a JTP Node object.
	ACS_JTP_Conversation_R3A::JTP_Node* pNode = 0;

	//Allocate memory for JTP Node
	pNode = new ACS_JTP_Conversation_R3A::JTP_Node();

	pNode->node_state = ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE;
	pNode->system_id = ACS_JTP_Conversation_R3A::SYSTEM_ID_THIS_NODE;
	ACE_OS::strcpy(pNode->node_name,"AP1A");

	//Invoke jexinitreq method on conversation object.
	if (pNode != 0)
	{
		ASSERTM("jexinitreq failed\n", (C1->jexinitreq(pNode, U1, U2 ) == true));

		delete pNode;
		pNode = 0;
	}

}

void CUTE_ACS_JTP_Conversation_R3A::testJexdatareq()
{
	unsigned short U1, U2, Len;
	char* Msg = 0;
	Msg = (char*)new char[50];
	ACE_OS::memset(Msg,0,sizeof(Msg));
	ACE_OS::strcpy(Msg ,"test message");
	Len = ACE_OS::strlen(Msg);
	//Send jexdatareq.
	ASSERTM("jexdatareq failed\n", C1->jexdatareq(U1, U2, Len, Msg) == true);
	if( Msg != 0)
	{
		delete[] Msg;
		Msg = 0;
	}
}
void CUTE_ACS_JTP_Conversation_R3A::testJexinitconf()
{
	unsigned short U1=5, U2=6, R=0;

	ASSERTM("jexinitconf failed\n",(C1->jexinitconf(U1, U2, R)== true));
}

void CUTE_ACS_JTP_Conversation_R3A::testJexdiscreq()
{
	unsigned short U1=5, U2=6, R=0;

	ASSERTM("jexdiscreq failed\n", (C1->jexdiscreq(U1, U2, R) == true));
}

void CUTE_ACS_JTP_Conversation_R3A::testJexdataind()
{
	unsigned short RU1=0, RU2=0;
	unsigned short RbufferLength = 0;
	char* RBuf = 0;

	//Fetch the jexdataind for the incoming jexdatareq.
	ASSERTM("jexdataind failed\n",(C1->jexdataind(RU1, RU2, RbufferLength, RBuf) == true));
}

void CUTE_ACS_JTP_Conversation_R3A::testJTPHandle()
{
	//get the handle of the session to listen for incoming jresultreq.
	ASSERTM("getHandle failed\n", (C1->getHandle() != ACE_INVALID_HANDLE));
}

void CUTE_ACS_JTP_Conversation_R3A::testQuery()
{
	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Conversation_R3A::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Conversation_R3A::JTP_Node> notReachNodes;

	//Find out the set on reachable and unreachable nodes.
	ASSERTM("query failed\n", (C1->query(ACS_JTP_Conversation_R3A::SYSTEM_TYPE_AP, nodes, notReachNodes) == true));
}

void CUTE_ACS_JTP_Conversation_R3A::testQueryWithTimeout(){
	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Conversation_R3A::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Conversation_R3A::JTP_Node> notReachNodes;

	//Find out the set on reachable and unreachable nodes.
	ASSERTM("Query with timeout failed\n", (C1->query(ACS_JTP_Conversation_R3A::SYSTEM_TYPE_AP, nodes, notReachNodes, 1000) == true));
}

void CUTE_ACS_JTP_Conversation_R3A::init()
{
	ACE_TCHAR Service[32];
	ACE_OS::memset(Service,0, sizeof(Service));
	ACE_OS::strcpy(Service,"Service1");

	//Create a ACS_JTP_Conversation_R3A object C1.
	C1 = new ACS_JTP_Conversation_R3A(Service, 100);
}

void CUTE_ACS_JTP_Conversation_R3A::destroy()
{
	//Destroy the conversation object.
	delete C1;
	C1 = 0;

}

cute::suite CUTE_ACS_JTP_Conversation_R3A::makeACSJTPConversationR3ASuite()
{

	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testRetries));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testDelay));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testQuery));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testQueryWithTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJexinitreqNode));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJexinitconf));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testSide));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testState));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJTPHandle));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJexdatareq));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJexdataind));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R3A::testJexdiscreq));

	return s;
}





