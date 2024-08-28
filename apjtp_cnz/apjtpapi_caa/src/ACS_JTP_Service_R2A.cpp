/*=================================================================== */
   /**
   @file ACS_JTP_Service_R2A.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the ACS_JTP_Service_R2A.h module

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
#include "ACS_JTP_Service_R2A.h"
#include "JTP_Service_R2A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: ACS_JTP_Service_R2A
=================================================================== */
ACS_JTP_Service_R2A::ACS_JTP_Service_R2A(char* Service)
{
	Internal = new (std::nothrow) JTP_Service_R2A(Service);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Service_R2A
=================================================================== */
ACS_JTP_Service_R2A::~ACS_JTP_Service_R2A()
{
	if( Internal)
	{
		delete Internal;
		Internal = 0;
	}
}

/*===================================================================
   ROUTINE: jidrepreq
=================================================================== */
bool ACS_JTP_Service_R2A::jidrepreq()
{
	return (Internal ? Internal->jidrepreq() : false);
}

/*===================================================================
   ROUTINE: accept
=================================================================== */
bool ACS_JTP_Service_R2A::accept(ACS_JTP_Job_R2A* j, ACS_JTP_Conversation_R2A* c)
{
	return (Internal ? Internal->accept_new(j, c) : false);
}

/*===================================================================
   ROUTINE: getHandles
=================================================================== */
void ACS_JTP_Service_R2A::getHandles(int& noOfHandles, JTP_HANDLE*& handles)
{
	if (Internal)
	{
		Internal->getHandles(noOfHandles, handles);
	}
}
