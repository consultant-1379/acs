/*=================================================================== */
/**
   @file CUTE_ACS_JTP_Service_R3A.cpp

   This file contains the implementation of CUTE_ACS_JTP_Service_R3A class.

   @version 1.0.0

   @documentno CAA 109 0870

   @copyright Ericsson AB, Sweden 2010. All rights reserved.

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/10/2010     XTANAGG		APG43 on Linux.
 **/
/*=================================================================== */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_JTP_Service_R3A.h"
#include "CUTE_ACS_JTP_Job_R3A.h"
#include "CUTE_ACS_JTP_Conversation_R3A.h"


ACS_JTP_Service_R3A* CUTE_ACS_JTP_Service_R3A::S1;
ACS_JTP_Conversation_R3A* CUTE_ACS_JTP_Service_R3A::C1;
ACS_JTP_Job_R3A*	CUTE_ACS_JTP_Service_R3A::J1;


void CUTE_ACS_JTP_Service_R3A::testJidrepreq()
{
	ASSERTM("jidreqreq failed\n", S1->jidrepreq() == true);
}

void CUTE_ACS_JTP_Service_R3A::testGetHandles()
{
	ACE_HANDLE *phandles = new ACE_HANDLE[2];
	int noOfHandles = 2;

	S1->getHandles(noOfHandles, phandles );

	ASSERTM("No of Handles is zero\n",noOfHandles > 0);

	delete phandles;
	phandles = 0;
}

void CUTE_ACS_JTP_Service_R3A::testAccept()
{
	//Call accept
	ASSERTM("accept failed\n",S1->accept(J1, C1) == true);
}

void CUTE_ACS_JTP_Service_R3A::testJinitind()
{
	unsigned short U1, U2, Len;
	char* Msg = 0;
	cout<< J1->State();
	ASSERTM("jinitind failed\n", J1->jinitind(U1, U2, Len, Msg) == true);
}

void CUTE_ACS_JTP_Service_R3A::testJinitrsp()
{
	unsigned short U1=10, U2=11, R=0;
	ASSERTM("jinitrsp failed\n", J1->jinitrsp(U1, U2, R) == true);
}

void CUTE_ACS_JTP_Service_R3A::testJresultreq()
{
	unsigned short U1=10, U2=11, R=0, Len = 14;
	char* Msg = new char[123];
	ACE_OS::memset(Msg,0, sizeof(Msg));
	ACE_OS::strcpy(Msg, "test Message");
	//Send jresultreq.
	ASSERTM("jresultreq failed\n", J1->jresultreq(U1, U2, R, Len, Msg) == true);

}

void CUTE_ACS_JTP_Service_R3A::testJexinitind()
{
	unsigned short U1, U2;
	ASSERTM("jexinitind failed\n", C1->jexinitind(U1, U2) == true);
}

void CUTE_ACS_JTP_Service_R3A::testJexinitrsp()
{
	unsigned short U1, U2, R=0;
	ASSERTM("jexinitrsp failed\n", C1->jexinitrsp(U1, U2, R) == true);
}

void CUTE_ACS_JTP_Service_R3A::testJexdatareq()
{
	unsigned short U1, U2, Len;
	char* Msg = 0;
	Msg = new char[50];
	ACE_OS::memset(Msg,0, sizeof(Msg));
	ACE_OS::strcpy(Msg,"test message");
	Len = ACE_OS::strlen(Msg);
	//Send jexdatareq.
	ASSERTM("jexdatareq failed\n", C1->jexdatareq(U1, U2, Len, Msg) == true);

	if( Msg != 0)
	{
		delete[] Msg;
		Msg = 0;
	}
}

void CUTE_ACS_JTP_Service_R3A::testJexdataind()
{
	unsigned short RU1=0, RU2=0;
	unsigned short RbufferLength = 0;
	char* RBuf = 0;

	ASSERTM("jexdataind failed: \n",(C1->jexdataind(RU1, RU2, RbufferLength, RBuf) == true));
}

void CUTE_ACS_JTP_Service_R3A::testJexdiscind()
{
	unsigned short U1, U2, R = 0;
	ASSERTM("jexdiscind failed\n", C1->jexdiscind(U1, U2, R) == true);
}

void CUTE_ACS_JTP_Service_R3A::init(){

	char ServiceName[32];
	ACE_OS::memset(ServiceName,0,sizeof(ServiceName));

	ACE_OS::strcpy(ServiceName, "Service1" );

	S1 = (ACS_JTP_Service_R3A*)new ACS_JTP_Service_R3A(ServiceName);

	J1 = (ACS_JTP_Job_R3A*)new ACS_JTP_Job_R3A();

	C1 = (ACS_JTP_Conversation_R3A*) new ACS_JTP_Conversation_R3A();
}

void CUTE_ACS_JTP_Service_R3A::destroy(){

   delete S1;
   S1 = 0;

   delete J1;
   J1 = 0;

   delete C1;
   C1 = 0;
}

cute::suite CUTE_ACS_JTP_Service_R3A::makeACSJTPServiceR3AJobSuite()
{

	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJidrepreq));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testGetHandles));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testAccept));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJinitind));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJinitrsp));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJresultreq));
	return s;
}

cute::suite CUTE_ACS_JTP_Service_R3A::makeACSJTPServiceR3AConvSuite()
{

	cute::suite s;

	//Add your test here
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJidrepreq));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testGetHandles));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testAccept));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJexinitind));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJexinitrsp));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJexdataind));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJexdatareq));
	s.push_back(CUTE(CUTE_ACS_JTP_Service_R3A::testJexdiscind));
	return s;
}


