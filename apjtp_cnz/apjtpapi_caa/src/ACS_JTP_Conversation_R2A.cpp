/*=================================================================== */
   /**
   @file ACS_JTP_Conversation_R2A.cpp

   Class method implementation for conversation.

   This module contains the implementation of class declared in
   the ACS_JTP_Conversation_R2A.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       08/04/2010     XTANAGG        APG43 on Linux.
   **/
/*===================================================================*/

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_JTP_Conversation_R2A.h"
#include "JTP_Session_R2A.h"
#include <new>
using namespace std;
/*===================================================================
   ROUTINE: ACS_JTP_Conversation_R2A
=================================================================== */
ACS_JTP_Conversation_R2A::ACS_JTP_Conversation_R2A() : Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R2A(JTP_Session::JTPConversation, 0);

}

/*===================================================================
   ROUTINE: ACS_JTP_Conversation_R2A
=================================================================== */
ACS_JTP_Conversation_R2A::ACS_JTP_Conversation_R2A(char* Service, unsigned short MaxBufLen) : Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R2A(JTP_Session::JTPConversation, Service, MaxBufLen);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Conversation_R2A
=================================================================== */
ACS_JTP_Conversation_R2A::~ACS_JTP_Conversation_R2A()
{
	if( Internal)
	{
		delete Internal;
		Internal = 0;
	}
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
JTP_HANDLE ACS_JTP_Conversation_R2A::getHandle()
{
	JTP_HANDLE handle = ACE_INVALID_HANDLE;
	if (Internal)
	{
		Internal->getHandle(handle);
	}
	return handle;
}

/*===================================================================
   ROUTINE: setNoOfTries
=================================================================== */
void ACS_JTP_Conversation_R2A::setNoOfTries(int Tries)
{
	if (Internal)
	{
		Internal->setNoOfTries(Tries);
	}
}

/*===================================================================
   ROUTINE: setDelayTime
=================================================================== */
void ACS_JTP_Conversation_R2A::setDelayTime(int Sec)
{
	if (Internal)
	{
		Internal->setDelayTime(Sec);
	}
}

/*===================================================================
   ROUTINE: setTimeOut
=================================================================== */
void ACS_JTP_Conversation_R2A::setTimeOut(int Sec)
{
	if (Internal)
	{
		Internal->setTimeOut(Sec);
	}
}


/*===================================================================
   ROUTINE: Side
=================================================================== */
 ACS_JTP_Conversation_R2A::JTP_Side ACS_JTP_Conversation_R2A::Side()
{
	return (JTP_Side)Internal->Side();
}

/*===================================================================
   ROUTINE: State
=================================================================== */
ACS_JTP_Conversation_R2A::JTP_State ACS_JTP_Conversation_R2A::State()
{
	return (JTP_State)Internal->State();

}

/*===================================================================
   ROUTINE: jexinitreq
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexinitreq(unsigned short U1, unsigned short U2)
{
	return Internal ? Internal->jexinitreq(U1, U2) : false;
}

/*===================================================================
   ROUTINE: jexinitind
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexinitind(unsigned short& U1, unsigned short& U2)
{
	return Internal ? Internal->jexinitind(U1, U2) : false;
}

/*===================================================================
   ROUTINE: jexinitrsp
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexinitrsp(unsigned short U1, unsigned short U2, unsigned short R)
{
	return Internal ? Internal->jexinitrsp(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexinitconf
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexinitconf(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jexinitconf(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexdatareq
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexdatareq(unsigned short U1, unsigned short U2, unsigned short BufLen, char* Buf)
{
	return Internal ? Internal->jexdatareq(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jexdiscreq
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexdataind(unsigned short& U1, unsigned short& U2, unsigned short& BufLen, char*& Buf)
{
	return Internal ? Internal->jexdataind(U1, U2, BufLen, Buf) : false;
}

/*===================================================================
   ROUTINE: jexdiscreq
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexdiscreq(unsigned short U1, unsigned short U2, unsigned short R)
{
	return Internal ? Internal->jexdiscreq(U1, U2, R) : false;
}

/*===================================================================
   ROUTINE: jexdiscind
=================================================================== */
bool ACS_JTP_Conversation_R2A::jexdiscind(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return Internal ? Internal->jexdiscind(U1, U2, R) : false;
}
