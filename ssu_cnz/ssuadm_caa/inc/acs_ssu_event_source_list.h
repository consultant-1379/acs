//******************************************************************************
//
//  NAME
//     ACS_SSU_EventSourceList.h
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

#ifndef ACS_SSU_EVENTSOURCELIST_H
#define ACS_SSU_EVENTSOURCELIST_H

#include "acs_ssu_event_source.h"


class ACS_SSU_EventSourceList
{
public:
   ACS_SSU_EventSourceList(const TCHAR* lpszLog);
   ~ACS_SSU_EventSourceList();

public:
   ACS_SSU_EventSource* EventSource(const TCHAR* lpszSource);
   void UnloadEventSources();

private:
   void ClearList();

private:
   TCHAR m_szLog[24];
   ACS_SSU_EventSource* m_pRoot;
};

#endif
