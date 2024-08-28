//******************************************************************************
//
//  NAME
//     ACS_USA_EventSourceList.cpp
//
// COPYRIGHT
//      Ericsson AB 2006 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
// 	    ACS_USA_EventSourceList handles a list of all ACS_USA_EventSource
//      objects for one event log.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    2006-03-31 by EAB/AZA/DH UABPEK
//
// REV  DATE    NAME     DESCRIPTION
// A    060331  UABPEK   Adapted for USA. Based on ACS_ALOG_EventSourceList.cpp.
//
//******************************************************************************

#include <windows.h>

#include "ACS_USA_EventSourceList.h"


//============================================================================
// Constructor
//============================================================================
ACS_USA_EventSourceList::ACS_USA_EventSourceList(const _TCHAR* lpszLog)
{
   _tcscpy(m_szLog, lpszLog);
   m_pRoot = NULL;
}

//============================================================================
// Destructor
//============================================================================
ACS_USA_EventSourceList::~ACS_USA_EventSourceList()
{
   ClearList();
}

//============================================================================
// Removes all items in the linked list and free allocated memory
//============================================================================
void ACS_USA_EventSourceList::ClearList()
{
   ACS_USA_EventSource* pCurrent = m_pRoot;

   while (pCurrent)
   {
      pCurrent = pCurrent->Next;
      delete m_pRoot;

      m_pRoot = pCurrent;
   }

   m_pRoot = NULL;
}

//============================================================================
// Returns a reference to an ACS_USA_EventSource object in the list with
// source name as search condition. If the items was not found in the list,
// a new object is created.
//============================================================================
ACS_USA_EventSource* ACS_USA_EventSourceList::EventSource(const _TCHAR* lpszSource)
{
   ACS_USA_EventSource* pCurrent = m_pRoot;
   ACS_USA_EventSource* pPrev = pCurrent;

   int  nRes = -1;
   BOOL bContinue = TRUE;
   while (pCurrent && bContinue)
   {
      if ((nRes = _tcscmp(pCurrent->m_szSource, lpszSource)) >= 0)
         bContinue = FALSE;
      else
      {
         pPrev = pCurrent;
         pCurrent = pCurrent->Next;
      }
   }

   // Object found in the linked list
   if (nRes == 0)
      return pCurrent;

   ACS_USA_EventSource* pNewItem = new ACS_USA_EventSource(m_szLog, lpszSource);
   if (!pNewItem)
      return NULL;

   pNewItem->Next = NULL; //pCurrent;
   // TR HI60745 
   if (NULL == m_pRoot ) // First element
      m_pRoot = pNewItem;
   else {
	   if(pPrev != pCurrent) { // Insert new element between two element of list
	      pPrev->Next = pNewItem;
		  pNewItem->Next = pCurrent;
	   }
	   else { // Insert new element in top of list
		  pNewItem->Next = pPrev;
		  m_pRoot = pNewItem;	
	   }
	   /*if(nRes < 0 ) // Insert new element in end of list
		   pPrev->Next = pNewItem;
	   else{ 
			if(pPrev != pCurrent) { // Insert new element between two element of list
				pPrev->Next = pNewItem;
				pNewItem->Next = pCurrent;
			}
			else { // Insert new element in top of list
				pNewItem->Next = pPrev;
				m_pRoot = pNewItem;	
			}
		}*/
   }
   return pNewItem;
}

//============================================================================
// Unload all event sources dll's
//============================================================================
void ACS_USA_EventSourceList::UnloadEventSources()
{
   ACS_USA_EventSource* pCurrent = m_pRoot;

   while (pCurrent)
   {
      pCurrent->UnloadSourceModules();
      pCurrent = pCurrent->Next;
   }
}