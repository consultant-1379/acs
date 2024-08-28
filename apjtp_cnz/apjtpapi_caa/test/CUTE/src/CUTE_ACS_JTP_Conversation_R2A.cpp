/*=================================================================== */
/**
@file CUTE_ACS_JTP_Conversation_R2A.cpp

This file contains the implementation of CUTE_ACS_JTP_Conversation_R2A class.

@version 1.0.0

@documentno CAA 109 0870

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       10/18/2010     XTANAGG        APG43 on Linux.
**/
/*=================================================================== */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_JTP_Conversation_R2A.h"

ACS_JTP_Conversation_R2A* CUTE_ACS_JTP_Conversation_R2A::C1;

void CUTE_ACS_JTP_Conversation_R2A::testTimeout()
{
	//Set a timeout value.
	C1->setTimeOut(3);
}

void CUTE_ACS_JTP_Conversation_R2A::testRetries()
{
	//Specify the number of retries.
	C1->setNoOfTries(2);
}

void CUTE_ACS_JTP_Conversation_R2A::testDelay()
{
	//Specify the delay between the different retries.
	C1->setDelayTime(1);
}

void CUTE_ACS_JTP_Conversation_R2A::testSide()
{
	ASSERTM("Side: \n", (C1->Side() == ACS_JTP_Conversation_R2A::JTPClient));
}

void CUTE_ACS_JTP_Conversation_R2A::testState()
{
	ASSERTM("State: \n", (C1->State() == ACS_JTP_Conversation_R2A::StateJobRunning));
}

void CUTE_ACS_JTP_Conversation_R2A::testJexinitreq()
{
	unsigned short U1=5, U2=6;

	ASSERTM("jexinitreq failed: \n", (C1->jexinitreq(U1, U2) == true));
}

void CUTE_ACS_JTP_Conversation_R2A::testJexinitconf()
{
	unsigned short U1=5, U2=6, R=0;

	ASSERTM("jexinitconf failed: \n",(C1->jexinitconf(U1, U2, R)== true));
}

void CUTE_ACS_JTP_Conversation_R2A::testJexdatareq()
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



void CUTE_ACS_JTP_Conversation_R2A::testJexdiscreq()
{
	unsigned short U1=10, U2=11, R=0;

	ASSERTM("jexdiscreq failed: \n",(C1->jexdiscreq(U1, U2, R) == true));
}

void CUTE_ACS_JTP_Conversation_R2A::testJTPHandle()
{
	ASSERTM("getHandle failed: \n", (C1->getHandle() != ACE_INVALID_HANDLE));
}

void CUTE_ACS_JTP_Conversation_R2A::init()
{
	//Define a service to which the client will connect.
	char Service[32];
	ACE_OS::memset(Service,0, sizeof(Service));
	ACE_OS::strcpy(Service,"Service1");

	//Create a ACS_JTP_Conversation_R2A object C1.
	C1 = new ACS_JTP_Conversation_R2A(Service, 100);
}

void CUTE_ACS_JTP_Conversation_R2A::destroy(){

	//Destruct object C1.
	delete C1;
	C1 = 0;

}

cute::suite CUTE_ACS_JTP_Conversation_R2A::makeACSJTPConversationR2ASuite(){

	//Create the test suite
	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testRetries));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testDelay));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testJexinitreq));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testJexinitconf));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testSide));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testState));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testJTPHandle));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testJexdatareq));
	s.push_back(CUTE(CUTE_ACS_JTP_Conversation_R2A::testJexdiscreq));

	//Return the test suite.
	return s;
}





