#if 0

//******************************************************************************
//
//  NAME
//     ACS_SSU_EventSourceList.cpp
//
//  COPYRIGHT
//     Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0xxx
//
//  AUTHOR 
//     2002-03-26 by UAB/UKB/GM UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#include <windows.h>
#include <tchar.h>
#include "acs_ssu_event_source_list.h"


//============================================================================
// Constructor
//============================================================================
ACS_SSU_EventSourceList::ACS_SSU_EventSourceList(const TCHAR* lpszLog)
{
   (void)_tcsncpy(m_szLog, lpszLog, 23);
   m_pRoot = NULL;
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_EventSourceList::~ACS_SSU_EventSourceList()
{
   try
   {
      ClearList();
   }
   catch (...) {}
}

//============================================================================
// Removes all items in the linked list and free allocated memory
//============================================================================
void ACS_SSU_EventSourceList::ClearList()
{
   ACS_SSU_EventSource* pCurrent = m_pRoot;

   while (pCurrent)
   {
      pCurrent = pCurrent->Next;
      delete m_pRoot;

      m_pRoot = pCurrent;
   }

   m_pRoot = NULL;
}

//============================================================================
// Returns a reference to a ACS_SSU_EventSource object in the list with
// source name as search condition. If the items was not found in the list,
// a new object is created.
//============================================================================
ACS_SSU_EventSource* ACS_SSU_EventSourceList::EventSource(const TCHAR* lpszSource)
{
   ACS_SSU_EventSource* pCurrent = m_pRoot;
   ACS_SSU_EventSource* pPrev = pCurrent;

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

   ACS_SSU_EventSource* pNewItem = new ACS_SSU_EventSource(m_szLog, lpszSource);
   if (!pNewItem)
      return NULL;

   pNewItem->Next = pCurrent;
   if (pPrev == m_pRoot)
      m_pRoot = pNewItem;
   else
      pPrev->Next = pNewItem;

   return pNewItem;
}

//============================================================================
// Unload all event sources dll's
//============================================================================
void ACS_SSU_EventSourceList::UnloadEventSources()
{
   ACS_SSU_EventSource* pCurrent = m_pRoot;

   while (pCurrent)
   {
      pCurrent->UnloadSourceModules();
      pCurrent = pCurrent->Next;
   }
}

#endif
