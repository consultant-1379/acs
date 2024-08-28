
//******************************************************************************//
//  NAME
//     ACS_SSU_CommandClient.cpp
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
//          190 89-CAA 109 0529
//
//  AUTHOR
//     2005-10-26 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO
//     -
//
//******************************************************************************
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <stdio.h>
#include <iostream>
#include "acs_ssu_common.h"
#include "acs_ssu_command_client.h"
#include "acs_ssu_aeh.h"

using namespace std;

extern BOOL g_bInteractive;

//============================================================================
// Constructor
//============================================================================
ACS_SSU_CommandClient::ACS_SSU_CommandClient(ACE_Event* poEvent,
                                             ACS_SSU_FolderQuotaMon* poFolderQuotaMon)
  /* m_hEvents[0] = hEndEvent;
   m_hEvents[1] = NULL;
   m_hPipe = INVALID_HANDLE_VALUE;

   ::RtlZeroMemory(&m_srctOL, sizeof(OVERLAPPED));
   m_FolderQuotaMon = poFolderQuotaMon;
   */
   {
          m_pEvents[0]=poEvent;
          m_pEvents[1]=NULL;
          m_pFolderQuotaMon=poFolderQuotaMon;
          ACE_ASSERT( m_pEvents != 0 );
          ACE_ASSERT( m_pFolderQuotaMon != 0 );
}

//============================================================================
// Desstructor
//============================================================================
ACS_SSU_CommandClient::~ACS_SSU_CommandClient()
{
  /* if (m_hPipe != INVALID_HANDLE_VALUE)
   {
      (void)::FlushFileBuffers(m_hPipe);
      (void)::DisconnectNamedPipe(m_hPipe);
      (void)::CloseHandle(m_hPipe);
   }

   // Closing pipe event
   if (m_hEvents[1])
      (void)::CloseHandle(m_hEvents[1]);*/
}

//============================================================================
// Connect
// Create a named pipe to communicate between the commands and the server and
// wait for a command to connect to it
//============================================================================
/*ACE_UINT32 ACS_SSU_CommandClient::Connect(PSECURITY_DESCRIPTOR pSD)
{
   SECURITY_ATTRIBUTES srctSA;
        srctSA.nLength = sizeof(SECURITY_ATTRIBUTES);
   srctSA.bInheritHandle = FALSE;
   srctSA.lpSecurityDescriptor = pSD;

   // Create a named pipe for receiving command requests
   m_hPipe = ::CreateNamedPipe(SSU_CMD_PIPENAME,
                               PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                               PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                               5,
                               0,
                               0,
                               1000*5, // 5 seconds
                               &srctSA);

   if (m_hPipe == INVALID_HANDLE_VALUE)
   {
      if (::GetLastError() != ERROR_PIPE_CONNECTED && ::GetLastError() != ERROR_PIPE_BUSY)
      {
         // Report error
         _TCHAR szData[256];
         _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                   _T("Unable to create named pipe due to the following error:\r\n%s"),
                   ACS_SSU_Common::GetWin32Msg());

         // Report error
         (void)ACS_SSU_AEH::ReportEvent(1003,
                                        _T("EVENT"),
                                        CAUSE_AP_INTERNAL_FAULT,
                                        _T(""),
                                        szData,
                                        _T("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

         return 1; // Error
      }
      else
      {
         ::Sleep(50);
         return 3; // Try again
      }
   }

   // Create event to monitor the pipe
   m_hEvents[1] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   if (!m_hEvents[1])
   {
      // Report error
      _TCHAR szData[256];
      _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                _T("Unable to create event due to the following error:\r\n%s"),
                ACS_SSU_Common::GetWin32Msg());

      // Report error
      (void)ACS_SSU_AEH::ReportEvent(1003,
                                     _T("EVENT"),
                                     CAUSE_AP_INTERNAL_FAULT,
                                     _T(""),
                                     szData,
                                     _T("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

      return 1;
   }

   m_srctOL.Internal = 0;
   m_srctOL.InternalHigh = 0;
   m_srctOL.Offset = 0;
   m_srctOL.OffsetHigh = 0;
   m_srctOL.hEvent = m_hEvents[1];

   // Enable a client to connect to the pipe (non blocking)
   (void)::ConnectNamedPipe(m_hPipe, &m_srctOL);

   DWORD dwLastError = ::GetLastError();
   if (dwLastError != ERROR_IO_PENDING && dwLastError != ERROR_PIPE_LISTENING)
   {
      if (dwLastError != ERROR_PIPE_CONNECTED)
      {
         // Report error
         _TCHAR szData[256];
         _sntprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                   _T("Unable to connect named pipe due to the following error:\r\n%s"),
                   ACS_SSU_Common::GetWin32Msg());

         // Report error
         (void)ACS_SSU_AEH::ReportEvent(1003,
                                        _T("EVENT"),
                                        CAUSE_AP_INTERNAL_FAULT,
                                        _T(""),
                                        szData,
                                        _T("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));

         return 1; // Error
      }
      else
      {
         ::Sleep(50);
         return 3; // Try again. Pipe not ready
      }
   }

   // Wait for a client to connect to the pipe
   DWORD dwObjectIdx = ::WaitForMultipleObjects(2, m_hEvents, FALSE, INFINITE);

   // Wait for a client to connect to the pipe
   switch (dwObjectIdx - WAIT_OBJECT_0)
   {
   case 0:
      // End event was signaled
      return 2;
      //break;

   case 1:
      // A command has connected, reset the event to not signaled
      (void)::ResetEvent(m_hEvents[1]);
      break;

   default:
      // Error
      return 1;
      //break;
   }

   return 0; // = OK
}

//============================================================================
// ExecuteCommand
// Read and parse pipe data and execute the command
//============================================================================
void ACS_SSU_CommandClient::ExecuteCommand()
{
   DWORD dwStatus = 0;
   ACS_SSU_Data srctData;

   // Reset event
   BOOL bContinue = TRUE;
   while (bContinue)
   {
      (void)::ResetEvent(m_hEvents[1]);

      // Read request from the command
      if ((dwStatus = Receive((LPBYTE)&srctData, sizeof(ACS_SSU_Data))) != 0)
      {
         // Normal behaviour, the command had been executed and the client has left
         bContinue = FALSE;
      }
      else
      {
         DWORD dwResult;

         switch (srctData.Code)
         {
         case SSU_CMD_CONFIG_SET:
            dwResult = m_FolderQuotaMon->SetConfigType(srctData);
            break;

         case SSU_CMD_CONFIG_LIST:
            dwResult = m_FolderQuotaMon->ListFolderQuotaConfig(srctData);
            break;

         default:
            dwResult = SSU_RC_UNKNOWNCMDTYPE;
            break;
         }

         srctData.Code = dwResult;

         // Send back result code (+ data) to the command
         dwStatus = SendRespond((LPBYTE)&srctData, sizeof(ACS_SSU_Data));
         if (dwStatus != 0)
            bContinue = FALSE;
      }
   }
}
*/

//============================================================================
// ExecuteCommandProc
// ExecuteCommand thread function
//============================================================================
ACE_THR_FUNC_RETURN ACS_SSU_CommandClient::ExecuteCommandProc(void* pvThis)
{
  // ((ACS_SSU_CommandClient*)lpvThis)->ExecuteCommand();
  // delete (ACS_SSU_CommandClient*)pvThis;
   //_endthread();
        return 0;
}

//============================================================================
// Run
// Executes the command by start the ExecuteCommand function in a new thread
//============================================================================
bool ACS_SSU_CommandClient::run()
{
   // Start threads
   //HANDLE hThread = (HANDLE)_beginthread(ExecuteCommandProc, 0, this);
   ACE_INT32 s8Result = ACS_SSU_Common::StartThread(this->ExecuteCommandProc,(void*)this, ACE_TEXT("CommandHandler Thread"),m_s32GrpId);
   if (!s8Result)
   {
      // Report error
      ACE_TCHAR szData[256];
      ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                ACE_TEXT("Unable to create thread: \"ExecuteCommandProc\" due to the following error:\r\n%s"),
                errno);
      ACE_DEBUG((LM_ERROR,"\n%s",szData));

      // Report error
      (void)ACS_SSU_AEH::ReportEvent(1003,
                                     ACE_TEXT("EVENT"),
                                     CAUSE_AP_INTERNAL_FAULT,
                                     ACE_TEXT(""),
                                     szData,
                                     ACE_TEXT("INTERNAL SSU SYSTEM SUPERVISOR ERROR"));


      return FALSE;
   }

   return TRUE;
}
/*
//============================================================================
// Receive
// Read data from the pipe sent by a command
//============================================================================
DWORD ACS_SSU_CommandClient::Receive(LPBYTE lpBuffer, DWORD dwSize)
{
   m_srctOL.Offset = 0;
   m_srctOL.OffsetHigh = 0;

   DWORD dwBytesRead = 0;
   DWORD dwLastError;

   // Read the first struct with information of the log data
   BOOL bSuccess = ::ReadFile(m_hPipe,
                              (LPVOID)lpBuffer,
                              dwSize,
                              &dwBytesRead,
                              &m_srctOL);

   if (bSuccess && dwBytesRead == dwSize)
      return 0; // OK

   // IO overlapped operation in progress
   if ((dwLastError = ::GetLastError()) == ERROR_IO_PENDING)
   {
      // Wait for the pending overlapped operation to be completed
      DWORD dwObjectIdx = ::WaitForMultipleObjects(2,
                                                   m_hEvents,
                                                   FALSE,
                                                   INFINITE);

      if ((dwObjectIdx - WAIT_OBJECT_0) == 0)
         return 2; // End event was signaled
      else if ((dwObjectIdx - WAIT_OBJECT_0) == 1)
      {
         // Check status of overlapped operation
         bSuccess = ::GetOverlappedResult(m_hPipe,
                                          &m_srctOL,
                                          &dwBytesRead,
                                          FALSE);

         if (!bSuccess)
         {
            // The client end of the pipe has been closed
            if (::GetLastError() == ERROR_BROKEN_PIPE)
               return 3;
            else
               return 1; // Error
         }
      }
      else
      {
         if (g_bInteractive)
            cerr << "WaitForMultipleObjects returned: " << dwObjectIdx << "\nError code: " << GetLastError() << endl;
         return 1; // Error
      }
   }
   else
   {
      if (dwLastError == ERROR_BROKEN_PIPE)
         return 3;
      else
         return 1;
   }

   // Unexpected size of buffer was received
   if (dwBytesRead != dwSize)
      return 1; // Error

   // All is well. Reset pipe event and continue
   (void)::ResetEvent(m_hEvents[1]);

   return 0; // OK
}

//============================================================================
// Respond
// Send a respond through the pipe to the command
//============================================================================
DWORD ACS_SSU_CommandClient::SendRespond(LPBYTE lpBuffer, DWORD dwSize)
{
   DWORD dwBytesWritten;
   BOOL bSuccess = ::WriteFile(m_hPipe,
                               (LPVOID)lpBuffer,
                               dwSize,
                               &dwBytesWritten,
                               NULL);

   if (bSuccess && dwBytesWritten == dwSize)
   {
      (void)::FlushFileBuffers(m_hPipe);
      return 0; // OK
   }
   else
   {
      // Check if client end of pipe has beeing closed
      if (::GetLastError() != ERROR_NO_DATA)
         return 1;
   }

   return 0;
}*/
