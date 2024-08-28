/*=================================================================== */
/**
@file CUTE_ACS_JTP_Job_R3A.cpp

This file contains the implementation of CUTE_ACS_JTP_Job_R3A class.

@version 1.0.0

@documentno CAA 109 0870

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS		DESCRIPTION
-----------------------------------------------------------
N/A       09/27/2010   XTANAGG		APG43 on Linux.
**/
/*=================================================================== */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_JTP_Job_R3A.h"

ACS_JTP_Job_R3A* CUTE_ACS_JTP_Job_R3A::J1;

void CUTE_ACS_JTP_Job_R3A::testTimeout()
{
	//Set a timeout value.
	J1->setTimeOut(3);
}

void CUTE_ACS_JTP_Job_R3A::testRetries()
{
	//Specify the number of retries.
	J1->setNoOfTries(2);
}

void CUTE_ACS_JTP_Job_R3A::testDelay()
{
	//Specify the delay between the different retries.
	J1->setDelayTime(1);
}

void CUTE_ACS_JTP_Job_R3A::testSide()
{
	//Get the side of the ACS_JTP_Job_R3A object.
	ASSERTM("Side failed\n", (J1->Side() == ACS_JTP_Job_R3A::JTPClient));
}

void CUTE_ACS_JTP_Job_R3A::testState()
{
	//Get the state of the ACS_JTP_Job_R3A object.
	ASSERTM("State failed\n", (J1->State() == ACS_JTP_Job_R3A::StateJobRunning));
}

void CUTE_ACS_JTP_Job_R3A::testJinitreq()
{
	unsigned short U1=5, U2=6;

	//Allocate memory for buffer.
	char* pMsg = new char[sizeof("message")+1];

	//Initialise message buffer to zero.
	ACE_OS::memset(pMsg,0, sizeof(pMsg));

	if(pMsg != 0)
	{
		//Copy the contents in the buffer.
		ACE_OS::strcpy(pMsg,"message");
	}

	//Calculate user buffer length.
	unsigned short bufferLength = ACE_OS::strlen(pMsg);

	//Invoke jinitreq on job object.
	ASSERTM("jinitreq failed\n", (J1->jinitreq(U1, U2, bufferLength, pMsg) == true));

	//If message is not allocated, delete the message.
	if( pMsg != 0)
	{
		delete[] pMsg;
		pMsg = 0;
	}

}

void CUTE_ACS_JTP_Job_R3A::testJinitreqNode()
{
	unsigned short U1=5, U2=6;

	//Allocate memory for buffer.
	char* pMsg = new char[sizeof("message")+1];

	//Initialise message buffer to zero.
	ACE_OS::memset(pMsg,0, sizeof(pMsg));

	//Copy the message into the buffer.
	if(pMsg != 0)
	{
		//Copy the contents in the buffer.
		ACE_OS::strcpy(pMsg,"message");
	}

	unsigned short bufferLength = ACE_OS::strlen(pMsg);

	//Create a JTP Node object.
	ACS_JTP_Job_R3A::JTP_Node* pNode = 0;

	//Allocate memory for JTP Node
	pNode = new ACS_JTP_Job_R3A::JTP_Node();

	pNode->system_id = ACS_JTP_Job_R3A::SYSTEM_ID_THIS_NODE;
	pNode->node_state= ACS_JTP_Job_R3A::NODE_STATE_ACTIVE;
	ACE_OS::strcpy(pNode->node_name, "AP1A");

	//Invoke jinitreq method on job object.
	if (pNode != 0)
	{
		ASSERTM("jinitreq failed\n", (J1->jinitreq(pNode, U1, U2, bufferLength, pMsg) == true));

		delete pNode;
		pNode = 0;
	}

	//If message buffer is not null, delete it.
	if( pMsg != 0)
	{
		delete[] pMsg;
		pMsg = 0;
	}
}

void CUTE_ACS_JTP_Job_R3A::testJinitconf()
{
	unsigned short U1=5, U2=6, R=0;

	ASSERTM("jinitconf failed\n",(J1->jinitconf(U1, U2, R)== true));
}

void CUTE_ACS_JTP_Job_R3A::testJfault()
{
	unsigned short U1=0, U2=0, R=0;

	ASSERTM("jfault failed\n", (J1->jfault(U1, U2, R) == true));
}

void CUTE_ACS_JTP_Job_R3A::testJresultInd()
{
	unsigned short RU1=0, RU2=0,R=0;
	unsigned short RbufferLength = 0;
	char* RBuf = 0;

	//Fetch the jresultind for the incoming jresultreq.
	ASSERTM("jresultind failed\n",(J1->jresultind(RU1, RU2, R, RbufferLength, RBuf) == true));
}

void CUTE_ACS_JTP_Job_R3A::testJTPHandle()
{
	//get the handle of the session to listen for incoming jresultreq.
	ASSERTM("getHandle failed\n", (J1->getHandle() != ACE_INVALID_HANDLE));
}

void CUTE_ACS_JTP_Job_R3A::testQuery()
{
	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Job_R3A::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Job_R3A::JTP_Node> notReachNodes;

	//Find out the set on reachable and unreachable nodes.
	ASSERTM("query failed\n", (J1->query((ACS_JTP_Job_R3A::SYSTEM_TYPE_AP), nodes, notReachNodes) == true));
}

void CUTE_ACS_JTP_Job_R3A::testQueryWithTimeout(){
	//Create vector for reachable nodes.
	std::vector<ACS_JTP_Job_R3A::JTP_Node> nodes;

	//Create vector for unreachable nodes.
	std::vector<ACS_JTP_Job_R3A::JTP_Node> notReachNodes;

	//Find out the set on reachable and unreachable nodes.
	ASSERTM("Query with timeout failed\n", (J1->query((ACS_JTP_Job_R3A::SYSTEM_TYPE_AP), nodes, notReachNodes, 1000) == true));
}

void CUTE_ACS_JTP_Job_R3A::init()
{
	char Service[32];
	ACE_OS::memset(Service,0, sizeof(Service));
	ACE_OS::strcpy(Service, "Service1");

	//Create a ACS_JTP_Job_R3A object J1.
	J1 = new ACS_JTP_Job_R3A(Service, 100);

}

void CUTE_ACS_JTP_Job_R3A::destroy()
{
	//Destroy the job object.
	delete J1;
	J1 = 0;

}

cute::suite CUTE_ACS_JTP_Job_R3A::makeACSJTPJobR3ASuite()
{

	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testRetries));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testDelay));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testQuery));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testQueryWithTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testJinitreqNode));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testJinitconf));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testSide));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testState));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testJTPHandle));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testJresultInd));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R3A::testJfault));

	return s;
}




