
#if 0
//******************************************************************************
//
//  NAME
//     ACS_SSU_Disk.cpp
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
//     2004-02-11 by EAB/UKY/GM UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#pragma warning(disable: 4786)

#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <pdhmsg.h>
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_disk.h"

using namespace std;

extern BOOL g_bInteractive;


//============================================================================
// Constructor
//============================================================================
ACS_SSU_Disk::ACS_SSU_Disk(HANDLE hEndEvent,
                           const LPSSU_PERF_DATA pPerfData,
                           ACS_SSU_AlarmHandler* pAlarmHandler)
{
   // Copy/assign input parameters
   m_hEndEvent = hEndEvent;
   RtlCopyMemory(&m_srctPerfData, pPerfData, sizeof(SSU_PERF_DATA));
   m_pAlarmHandler = pAlarmHandler;

   if (g_bInteractive)
   {
      _TCHAR szData[128];
      _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                _T("The following disk supervision has been added:\n\tPartitionletter: %s\n\tAlarm class: %s\n"),
                m_srctPerfData.Instance,
                m_srctPerfData.Severity);

      cout << szData << endl;
   }

   // Ensure that the severity class text is in uppercase
   (void)_tcsupr(m_srctPerfData.Severity);

   DWORD dwLen = 64;
   (void)::GetComputerName(m_szComputerName, &dwLen);

   // Create performance counter monitor event
   m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   if (!m_hEvent)
   {
      // Report error
      _TCHAR szData[256];
      _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                _T("Unable to create event due to the following error:\r\n%s"),
                ACS_SSU_Common::GetWin32Msg());

      if (g_bInteractive)
         cerr << szData << endl;

      // Send event
      (void)ACS_SSU_AEH::ReportEvent(1001,
                                     _T("EVENT"),
                                     CAUSE_AP_INTERNAL_FAULT,
                                     _T(""),
                                     szData,
                                     _T("CREATE EVENT FAILED IN SSU SYSTEM SUPERVISOR"));
   }

   // Start performance counter monitor thread
   m_hThread = (HANDLE)_beginthreadex(NULL,
                                      0,
                                      MonitorThread,
                                      (LPVOID)this,
                                      0,
                                      &m_nThreadId);

   if (!m_hThread)
   {
      // Report error
      _TCHAR szData[256];
      _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                _T("Unable to create thread: MonitorThread due to the following error:\r\n%s"),
                ACS_SSU_Common::GetWin32Msg());

      if (g_bInteractive)
         cerr << szData << endl;

      // Send event
      (void)ACS_SSU_AEH::ReportEvent(1002,
                                     _T("EVENT"),
                                     CAUSE_AP_INTERNAL_FAULT,
                                     _T(""),
                                     szData,
                                     _T("CREATE THREAD FAILED IN SSU SYSTEM SUPERVISOR"));
   }
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_Disk::~ACS_SSU_Disk()
{
   try
   {
      if (m_hEvent)
         (void)::SetEvent(m_hEvent);

      if (m_hThread)
      {
         (void)::WaitForSingleObject(m_hThread, INFINITE);
         (void)::CloseHandle(m_hThread);
         m_hThread = NULL;
      }

      // Cease any outstanding alarm before leaving
      m_pAlarmHandler->SendPerfCease(&m_srctPerfData, 0.0);

      if (m_hEvent)
         (void)::CloseHandle(m_hEvent);
   }
   catch (...) { }
}

//============================================================================
// Returns the available disk space in percentage
//============================================================================
BOOL ACS_SSU_Disk::GetFreeDiskSpacePercentage(const _TCHAR* lpszDrive,
                                              DOUBLE* pFreeSpace)
{
   *pFreeSpace = 0;
   ULARGE_INTEGER lnSize1, lnSize2, lnSize3;
   BOOL bSuccess = ::GetDiskFreeSpaceEx(lpszDrive, &lnSize1, &lnSize2, &lnSize3);
   if (bSuccess && lnSize3.QuadPart > 0)
   {
      DOUBLE nTotal = (DOUBLE)(ULONG)(lnSize2.QuadPart/(ULONGLONG)1024);
      DOUBLE nFree = (DOUBLE)(ULONG)(lnSize3.QuadPart/(ULONGLONG)1024);
      *pFreeSpace = (DOUBLE)((nFree/nTotal)*100);
   }

   return bSuccess;
}

//============================================================================
// Checks if the Monitor thread has terminated
//============================================================================
BOOL ACS_SSU_Disk::IsThreadTerminated()
{
   return (::WaitForSingleObject(m_hThread, 0L) != WAIT_TIMEOUT);
}

//============================================================================
// Signals the event to tell the Monitor thread to terminate
//============================================================================
void ACS_SSU_Disk::Close()
{
   if (m_hEvent)
      (void)::SetEvent(m_hEvent);
}

//============================================================================
// Returns the current LPSSU_PERF_DATA data structure
//============================================================================
const LPSSU_PERF_DATA ACS_SSU_Disk::PerfData() const
{
   return (const LPSSU_PERF_DATA)&m_srctPerfData;
}

//============================================================================
// Disk partition Monitor thread
//============================================================================
void ACS_SSU_Disk::Monitor()
{
   HANDLE hObjects[2] = { m_hEndEvent, m_hEvent };
   DWORD dwSampleInterval = (m_srctPerfData.Interval * 1000);
   DWORD dwState;

   DOUBLE nFreeSpace = 0;
   _TCHAR* lpszActiveNode = NULL;
   BOOL bContinue = TRUE;
   while (bContinue)
   {
      dwState = ::WaitForMultipleObjects(2, hObjects, FALSE, dwSampleInterval);
      if (dwState != WAIT_TIMEOUT)
         bContinue = FALSE;
      else
      {
         // Check free disk space for this partition and compare
         if (!GetFreeDiskSpacePercentage(m_srctPerfData.Instance, &nFreeSpace))
            bContinue = FALSE;
         else
         {
            // Check if threashold limit has been reached
            if ((m_srctPerfData.AlertIfOver && nFreeSpace > m_srctPerfData.Value) ||
               (!m_srctPerfData.AlertIfOver && nFreeSpace < m_srctPerfData.Value))
            {
               // Get the active AP node name from the cluster
               lpszActiveNode = ACS_SSU_Common::GetActiveNodeName();
                  
               // Check whether an alarm shall be issued on this node
               if (!(*m_srctPerfData.Node) ||
                  _tcsicmp(m_srctPerfData.Node, _T("both")) == 0 ||
                  (_tcsicmp(m_srctPerfData.Node, _T("active")) == 0 && _tcsicmp(m_szComputerName, lpszActiveNode) == 0) ||
                  (_tcsicmp(m_srctPerfData.Node, _T("passive")) == 0 && _tcsicmp(m_szComputerName, lpszActiveNode) != 0))
               {
                  // Send alarm
                  m_pAlarmHandler->SendPerfAlarm(&m_srctPerfData, nFreeSpace);
               }
            }
            else
            {
               // Send a cease only if collected value is below/over the cease level
               if ((m_srctPerfData.AlertIfOver && nFreeSpace < m_srctPerfData.CeaseValue) ||
                  (!m_srctPerfData.AlertIfOver && nFreeSpace > m_srctPerfData.CeaseValue))
               {
                  // Send cease
                  m_pAlarmHandler->SendPerfCease(&m_srctPerfData, nFreeSpace);
               }
            }
         }
      }
   }

   ::Sleep(200);
}

//============================================================================
// Thread register function
//============================================================================
UINT WINAPI ACS_SSU_Disk::MonitorThread(LPVOID lpvThis)
{
   ((ACS_SSU_Disk*)lpvThis)->Monitor();
   _endthreadex(0);
   return 0;
}

#endif
