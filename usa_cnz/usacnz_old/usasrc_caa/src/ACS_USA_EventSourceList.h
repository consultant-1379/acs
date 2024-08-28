//******************************************************************************
//
//  NAME
//     ACS_USA_EventSourceList - List of ACS_USA_EventSource objects
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
// A    060331  UABPEK   Adapted for USA. Based on ACS_ALOG_EventSourceList.h.
//
//******************************************************************************

#ifndef ACS_USA_EVENTSOURCELIST_H
#define ACS_USA_EVENTSOURCELIST_H

#include "ACS_USA_EventSource.h"

class ACS_USA_EventSourceList
{
public:
   ACS_USA_EventSourceList(const _TCHAR* lpszLog);
   ~ACS_USA_EventSourceList();

public:
   ACS_USA_EventSource* EventSource(const _TCHAR* lpszSource);
   void UnloadEventSources();

private:
   void ClearList();

private:
   _TCHAR m_szLog[24];
   ACS_USA_EventSource* m_pRoot;
};

#endif