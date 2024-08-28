//******************************************************************************
//
//  NAME
//     ACS_SSU_CommandClient.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0529
//
//  AUTHOR 
//     2005-10-26 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef ACS_SSU_COMMANDCLIENT_H
#define ACS_SSU_COMMANDCLIENT_H
#include <ace/ACE.h>
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include "acs_ssu_folder_quota_mon.h"

class ACS_SSU_CommandClient
{
public:
   ACS_SSU_CommandClient(ACE_Event* poEvent, ACS_SSU_FolderQuotaMon* pFolderQuotaMon);
   ~ACS_SSU_CommandClient();

public:
   // Thread function
   static ACE_THR_FUNC_RETURN ExecuteCommandProc(void* pvThis);

public:
  // ACE Connect(PSECURITY_DESCRIPTOR pSD);
   bool run();

private:
  // void ExecuteCommand();
   //DWORD Receive(LPBYTE lpBuffer, DWORD dwSize);
  // DWORD SendRespond(LPBYTE lpBuffer, DWORD dwSize);

private:
    ACE_Event*                m_pEvents[2];
  // HANDLE                   m_hPipe;
   //OVERLAPPED               m_srctOL;
    ACE_INT32                 m_s32GrpId;

   ACS_SSU_FolderQuotaMon*  m_pFolderQuotaMon;
};

#endif
