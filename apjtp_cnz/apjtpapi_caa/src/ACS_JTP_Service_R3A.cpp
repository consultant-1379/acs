/*=================================================================== */
   /**
   @file ACS_JTP_Service_R3A.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the ACS_JTP_Service_R3A.h module

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_JTP_Service_R3A.h"
#include "JTP_Service_R3A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: ACS_JTP_Service_R3A
=================================================================== */
ACS_JTP_Service_R3A::ACS_JTP_Service_R3A(char* Service) : Internal(0)
{
	Internal = new (std::nothrow) JTP_Service_R3A(Service);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Service_R3A
=================================================================== */
ACS_JTP_Service_R3A::~ACS_JTP_Service_R3A()
{
	if( Internal )
	{
		delete Internal;
		Internal = 0;
	}
}

/*===================================================================
   ROUTINE: jidrepreq
=================================================================== */
bool ACS_JTP_Service_R3A::jidrepreq()
{
	return Internal ? Internal->jidrepreq() : false;
}

/*===================================================================
   ROUTINE: accept
=================================================================== */
bool ACS_JTP_Service_R3A::accept(ACS_JTP_Job_R3A* j, ACS_JTP_Conversation_R3A* c)
{
	return Internal ? Internal->accept_new(j, c) : false;
}

/*===================================================================
   ROUTINE: getHandles
=================================================================== */
void ACS_JTP_Service_R3A::getHandles(int& noOfHandles, JTP_HANDLE*& handles)
{
	if (Internal)
	{
		Internal->getHandles(noOfHandles, handles);
	}
}
