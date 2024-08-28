/*=================================================================== */
   /**
   @file ACS_JTP_Job_R2A.cpp

   Class method implementation for job.

   This module contains the implementation of class declared in
   the ACS_JTP_Job_R2A.h module

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
#include "ACS_JTP_Job_R2A.h"
#include "JTP_Session_R2A.h"
#include <new>
using namespace std;

/*===================================================================
   ROUTINE: ACS_JTP_Job_R2A
=================================================================== */
ACS_JTP_Job_R2A::ACS_JTP_Job_R2A(char* Service, unsigned short MaxBufLen) : Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R2A(JTP_Session::JTPJob, Service, MaxBufLen);
}

/*===================================================================
   ROUTINE: ACS_JTP_Job_R2A
=================================================================== */
ACS_JTP_Job_R2A::ACS_JTP_Job_R2A() : Internal(0)
{
	Internal = new (std::nothrow) JTP_Session_R2A(JTP_Session::JTPJob, 0);
}

/*===================================================================
   ROUTINE: ~ACS_JTP_Job_R2A
=================================================================== */
ACS_JTP_Job_R2A::~ACS_JTP_Job_R2A()
{
	if(Internal)
	{
		delete Internal;
		Internal = 0;
	}
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
JTP_HANDLE ACS_JTP_Job_R2A::getHandle()
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
void ACS_JTP_Job_R2A::setNoOfTries(int Tries)
{
	if (Internal)
	{
		Internal->setNoOfTries(Tries);
	}
}

/*===================================================================
   ROUTINE: setDelayTime
=================================================================== */
void ACS_JTP_Job_R2A::setDelayTime(int Sec)
{
	if (Internal)
	{
		Internal->setDelayTime(Sec);
	}
}

/*===================================================================
   ROUTINE: setTimeOut
=================================================================== */
void ACS_JTP_Job_R2A::setTimeOut(int Sec)
{
	if (Internal)
	{
		Internal->setTimeOut(Sec);
	}
}

/*===================================================================
   ROUTINE: Side
=================================================================== */
 ACS_JTP_Job_R2A::JTP_Side ACS_JTP_Job_R2A::Side()
{
	return ACS_JTP_Job_R2A::JTPClient;
}

/*===================================================================
   ROUTINE: State
=================================================================== */
 ACS_JTP_Job_R2A::JTP_State ACS_JTP_Job_R2A::State()
{
	return ((JTP_State)Internal->State());
}

/*===================================================================
   ROUTINE: jinitreq
=================================================================== */
bool ACS_JTP_Job_R2A::jinitreq(unsigned short U1, unsigned short U2, unsigned short BufLen, char* Buf)
{
	return (Internal ? Internal->jinitreq(U1, U2, BufLen, Buf) : false);
}

/*===================================================================
   ROUTINE: jinitind
=================================================================== */
bool ACS_JTP_Job_R2A::jinitind(unsigned short& U1, unsigned short& U2, unsigned short& BufLen, char*& Buf)
{
	return (Internal ? Internal->jinitind(U1, U2, BufLen, Buf) : false);
}

/*===================================================================
   ROUTINE: jinitrsp
=================================================================== */
bool ACS_JTP_Job_R2A::jinitrsp(unsigned short U1, unsigned short U2, unsigned short R)
{
	return (Internal ? Internal->jinitrsp(U1, U2, R) : false);
}

/*===================================================================
   ROUTINE: jinitconf
=================================================================== */
bool ACS_JTP_Job_R2A::jinitconf(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return (Internal ? Internal->jinitconf(U1, U2, R) : false);
}

/*===================================================================
   ROUTINE: jresultreq
=================================================================== */
bool ACS_JTP_Job_R2A::jresultreq(unsigned short U1, unsigned short U2, unsigned short R, unsigned short BufLen, char* Buf)
{
	return (Internal ? Internal->jresultreq(U1, U2, R, BufLen, Buf) : false);
}

/*===================================================================
   ROUTINE: jresultind
=================================================================== */
bool ACS_JTP_Job_R2A::jresultind(unsigned short& U1, unsigned short& U2, unsigned short& R, unsigned short& BufLen, char*& Buf)
{
	return (Internal ? Internal->jresultind(U1, U2, R, BufLen, Buf) : false);
}

/*===================================================================
   ROUTINE: jtermreq
=================================================================== */
bool ACS_JTP_Job_R2A::jtermreq()
{
	return (Internal ? Internal->jtermreq() : false);
}

/*===================================================================
   ROUTINE: jfault
=================================================================== */
bool ACS_JTP_Job_R2A::jfault(unsigned short& U1, unsigned short& U2, unsigned short& R)
{
	return (Internal ? Internal->jfault(U1, U2, R) : false);
}
