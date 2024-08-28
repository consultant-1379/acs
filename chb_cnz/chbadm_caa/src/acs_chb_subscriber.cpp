/*=================================================================== */
   /**
   @file acs_chb_subscriber.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_subscriber.h module

   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/01/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_chb_subscriber.h"
#include <ace/OS_NS_sys_socket.h>

//#include "acs_chb_common.h"

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief INVALID_SOCKET

 */
/*=================================================================== */
const int INVALID_SOCKET = 0;
/*=================================================================== */
/**
   @brief SOCKET_ERROR

 */
/*=================================================================== */
const int SOCKET_ERROR = -1;
/*=================================================================== */
/**
   @brief PORT

 */
/*=================================================================== */
const int PORT = 1234;

/*===================================================================
   ROUTINE: ACS_CHB_Subscriber
=================================================================== */
ACS_CHB_Subscriber::ACS_CHB_Subscriber()
{
  requestPending = 0;
  sForReportToClient = INVALID_SOCKET;
  headEventList = new ACS_CHB_EventList(); // List head
  requestBuffer[0] = 0;
  Next = 0;
  Pre = 0;
} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_Subscriber
=================================================================== */
ACS_CHB_Subscriber::~ACS_CHB_Subscriber()
{
  // Remove links to next and prev element
  if (Next != 0)
  {
    Next->Pre = Pre;
  }
  if (Pre != 0)
  {
    Pre->Next = Next;
  }
  // Delete evetlist
  ACS_CHB_EventList* ptr = headEventList->get_next();
  while (ptr)
  {
    delete ptr;
    ptr = headEventList->get_next();
  } // End of while
  delete headEventList;
  // Close fd
  if (sForReportToClient != INVALID_SOCKET)
    ACE_OS::closesocket(sForReportToClient);
} // End of destructor

/*===================================================================
   ROUTINE: get_requestPending
=================================================================== */
int ACS_CHB_Subscriber::get_requestPending() const
{
  return requestPending;
} // End of get_requestPending

/*===================================================================
   ROUTINE: set_requestPending
=================================================================== */
void ACS_CHB_Subscriber::set_requestPending(int p)
{
  requestPending = p;
} // End of set_requestPending

/*===================================================================
   ROUTINE: get_sForReportToClient
=================================================================== */
ACE_HANDLE ACS_CHB_Subscriber::get_sForReportToClient()
{
  return sForReportToClient;
} // End of get_sForReportToClient

/*===================================================================
   ROUTINE: set_sForReportToClient
=================================================================== */
void ACS_CHB_Subscriber::set_sForReportToClient(ACE_HANDLE fd)
{
  sForReportToClient = fd;
} // End of set_sForReportToClient

/*===================================================================
   ROUTINE: get_next
=================================================================== */
ACS_CHB_Subscriber* ACS_CHB_Subscriber::get_next()
{
  return Next;
} // End of get_next

/*===================================================================
   ROUTINE: get_last
=================================================================== */
ACS_CHB_Subscriber* ACS_CHB_Subscriber::get_last()
{
  ACS_CHB_Subscriber* tmp = this;
  while (tmp->Next) tmp = tmp->Next;
  return tmp;
} // End of get_last

/*===================================================================
   ROUTINE: appendToList
=================================================================== */
void ACS_CHB_Subscriber::appendToList(ACS_CHB_Subscriber* newElm)
{
  ACS_CHB_Subscriber* tmp = get_last();
  tmp->Next = newElm;
  newElm->Pre = tmp;
} // End of appendToList

/*===================================================================
   ROUTINE: get_requestBufferPtr
=================================================================== */
char* ACS_CHB_Subscriber::get_requestBufferPtr()
{
  return requestBuffer;
} // End of get_requestBufferPtr

/*===================================================================
   ROUTINE: get_headEventList
=================================================================== */
ACS_CHB_EventList* ACS_CHB_Subscriber::get_headEventList()
{
  return headEventList;
} // End of get_headEventList


