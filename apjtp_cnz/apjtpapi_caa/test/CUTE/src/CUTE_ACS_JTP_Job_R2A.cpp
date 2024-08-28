/*=================================================================== */
/**
@file CUTE_ACS_JTP_Job_R2A.cpp

This file contains the implementation of CUTE_ACS_JTP_Job_R2A class.

@version 1.0.0

@documentno CAA 109 0870

@copyright Ericsson AB, Sweden 2010. All rights reserved.

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       09/20/2010   XTANAGG		APG43 on Linux.
**/
/*=================================================================== */

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_JTP_Job_R2A.h"

ACS_JTP_Job_R2A* CUTE_ACS_JTP_Job_R2A::J1;

void CUTE_ACS_JTP_Job_R2A::testTimeout()
{
	//Set a timeout value.
	J1->setTimeOut(3);
}

void CUTE_ACS_JTP_Job_R2A::testRetries()
{
	//Specify the number of retries.
	J1->setNoOfTries(2);
}

void CUTE_ACS_JTP_Job_R2A::testDelay()
{
	//Specify the delay between the different retries.
	J1->setDelayTime(1);
}

void CUTE_ACS_JTP_Job_R2A::testSide()
{
	ASSERTM("Side: \n", (J1->Side() == ACS_JTP_Job_R2A::JTPClient));
}

void CUTE_ACS_JTP_Job_R2A::testState()
{
	ASSERTM("State: \n", (J1->State() == ACS_JTP_Job_R2A::StateJobRunning));
}

void CUTE_ACS_JTP_Job_R2A::testJinitreq()
{

	unsigned short U1=5, U2=6;

	//Allocate memory for message buffer.
	char* pMsg = (char*)new char[sizeof("message")+1];

	//if memory is successfully allocated, copy message into the buffer.
	if(pMsg != 0)
	{
		//Copy the contents in the buffer.
		ACE_OS::strcpy(pMsg,"message");
	}

	unsigned short bufferLength = ACE_OS::strlen(pMsg);

	ASSERTM("jinitreq: \n", (J1->jinitreq(U1,U2, bufferLength, pMsg) == true));

	//If message buffer is not zero, free the allocated memory.
	if( pMsg != 0)
	{
		delete[] pMsg;
		pMsg = 0;
	}

}

void CUTE_ACS_JTP_Job_R2A::testJinitconf()
{
	unsigned short U1=5, U2=6, R=0;

	ASSERTM("jinitconf: \n",(J1->jinitconf(U1, U2, R)== true));
}

void CUTE_ACS_JTP_Job_R2A::testJfault()
{
	unsigned short U1=0, U2=0, R=0;

	ASSERTM("jfault: \n", (J1->jfault(U1,U2,R) == true));
}

void CUTE_ACS_JTP_Job_R2A::testJresultInd()
{
	unsigned short RU1=0, RU2=0, R=0;
	unsigned short RbufferLength = 0;
	char* RBuf = 0;

	ASSERTM("jresultind: \n",(J1->jresultind(RU1, RU2, R, RbufferLength, RBuf) == true));
}

void CUTE_ACS_JTP_Job_R2A::testJTPHandle()
{
	ASSERTM("getHandle: \n", (J1->getHandle() != ACE_INVALID_HANDLE));
}

void CUTE_ACS_JTP_Job_R2A::init()
{
	//Define a service to which the client will connect.
	char Service[32];
	ACE_OS::memset(Service,0, sizeof(Service));
	ACE_OS::strcpy(Service,"Service1");

	//Create a ACS_JTP_Job_R2A object J1.
	J1 = new ACS_JTP_Job_R2A(Service, 100);
}

void CUTE_ACS_JTP_Job_R2A::destroy(){

	//Destruct object J1.
	delete J1;
	J1 = 0;

}

cute::suite CUTE_ACS_JTP_Job_R2A::makeACSJTPJobR2ASuite(){

	//Create the test suite

	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testTimeout));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testRetries));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testDelay));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testJinitreq));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testJinitconf));
	s.push_back(CUTE( CUTE_ACS_JTP_Job_R2A::testSide));
	s.push_back(CUTE( CUTE_ACS_JTP_Job_R2A::testState));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testJTPHandle));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testJresultInd));
	s.push_back(CUTE(CUTE_ACS_JTP_Job_R2A::testJfault));

	//Return the test suite.
	return s;
}





