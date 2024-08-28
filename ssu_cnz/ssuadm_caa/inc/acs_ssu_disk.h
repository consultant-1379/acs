//******************************************************************************
//
//  NAME
//     ACS_SSU_Disk.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2003. All rights reserved.
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
//	    190 89-CAA 109 0529
//
//  AUTHOR 
//     2003-04-07 by EAB/UKY/GM UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef ACS_SSU_DISK_H
#define ACS_SSU_DISK_H

#include "acs_ssu_types.h"
#include "acs_ssu_alarm_handler.h"

class ACS_SSU_Disk
{
public:
   ACS_SSU_Disk(HANDLE hEndEvent,
                const LPSSU_PERF_DATA pPerfData,
                ACS_SSU_AlarmHandler* pAlarmHandler);
   ~ACS_SSU_Disk();

public:
   void Close();
   BOOL IsThreadTerminated();

private:
   BOOL GetFreeDiskSpacePercentage(const _TCHAR* lpszDrive, DOUBLE* pFreeSpace);
   void Monitor();

public:
   //static void MonitorThread(LPVOID lpvThis);
   static UINT WINAPI MonitorThread(LPVOID lpvThis);

public:
   // Properties
   const LPSSU_PERF_DATA PerfData() const;

private:
   // Private class members
   ACS_SSU_AlarmHandler*  m_pAlarmHandler;
   SSU_PERF_DATA          m_srctPerfData;
   HANDLE                 m_hEndEvent;
   HANDLE                 m_hEvent;
   HANDLE                 m_hThread;
   _TCHAR                 m_szComputerName[64+1];
   UINT                   m_nThreadId;
};

#endif
