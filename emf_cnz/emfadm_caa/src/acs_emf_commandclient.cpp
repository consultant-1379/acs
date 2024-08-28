/*=================================================================== */
/**
   @file   acs_emf_aeh.cpp

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "ACS_HWM_API.h"
#include <winsock2.h>
#include <process.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>

//----------------------------CR 149
#include <setupapi.h>
#include <devguid.h>
//----------------------------CR 149

#include "ACS_EMF_Common.h"
#include "ACS_EMF_Defs.h"
#include "ACS_EMF_Debug.h"
#include "ACS_EMF_CommandClient.h"
#include "ACS_EMF_EventHandler.h"
#include "ACS_EMF_ServerHandler.h"

HANDLE hThread;
using namespace std;

/*===================================================================
   ROUTINE: ACS_EMF_CommandClient
=================================================================== */
ACS_EMF_CommandClient::ACS_EMF_CommandClient(HANDLE hEndEvent,
                                             ACS_EMF_DVDHandler* pDVDHandler,
                                             ACS_EMF_TapeHandler* pTapeHandler,
                                             ACS_EMF_ClientHandler* pClientHandler)
{
   m_hEvents[0] = hEndEvent;
   m_hEvents[1] = NULL;
   m_hPipe = INVALID_HANDLE_VALUE;
   m_pDVDHandler = pDVDHandler;
   m_pTapeHandler = pTapeHandler;
   m_pClientHandler = pClientHandler;
   ::RtlZeroMemory(&m_srctOL, sizeof(OVERLAPPED));
}

/*===================================================================
   ROUTINE: ~ACS_EMF_CommandClient
=================================================================== */
ACS_EMF_CommandClient::~ACS_EMF_CommandClient()
{
   if (m_hPipe != INVALID_HANDLE_VALUE)
   {
      (void)::FlushFileBuffers(m_hPipe);
      (void)::DisconnectNamedPipe(m_hPipe);
      (void)::CloseHandle(m_hPipe);
   }
   // Closing pipe event
   if (m_hEvents[1])
      (void)::CloseHandle(m_hEvents[1]);
}

/*===================================================================
   ROUTINE: Connect
=================================================================== */
DWORD ACS_EMF_CommandClient::Connect(PSECURITY_DESCRIPTOR pSD)
{
   SECURITY_ATTRIBUTES srctSA;
	srctSA.nLength = sizeof(SECURITY_ATTRIBUTES);
   srctSA.bInheritHandle = FALSE;
   srctSA.lpSecurityDescriptor = pSD;
   // Create a named pipe for receiving command requests
   m_hPipe = ::CreateNamedPipe(EMF_CMD_PIPENAME,
                               PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                               PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                               5,
                               0,
                               0,
                               1000*5, // 5 seconds
                               &srctSA);
   if (m_hPipe == INVALID_HANDLE_VALUE)
   {
      if (::GetLastError() != ERROR_PIPE_CONNECTED)
      {
         // Report error
         _TCHAR szData[256];
         _stprintf(szData,
                   _T("Unable to create named pipe due to the following error:\r\n%s"),
                   ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
         // Send event
         ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
                             EMF_EV_EVENTNUM_INTERNALERROR,
                             szData,
                             EMF_EV_EVENTTEXT_INTERNALERROR);
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
      _stprintf(szData,
                _T("Unable to create event due to the following error:\r\n%s"),
                ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
      // Send event
      ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
                          EMF_EV_EVENTNUM_INTERNALERROR,
                          szData,
                          EMF_EV_EVENTTEXT_INTERNALERROR);
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
         _stprintf(szData,
                   _T("Unable to connect named pipe due to the following error:\r\n%s"),
                   ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
         // Send event
         ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
                             EMF_EV_EVENTNUM_INTERNALERROR,
                             szData,
                             EMF_EV_EVENTTEXT_INTERNALERROR);
         return 1; // Error
      }
      else
      {
         ::Sleep(50);
         return 3; // Try again
      }
   }
   // Wait for a client to connect to the pipe
   DWORD dwObjectIdx = ::WaitForMultipleObjects(2, m_hEvents, FALSE, INFINITE);
   // Wait for a client to connect to the pipe
   switch (dwObjectIdx - WAIT_OBJECT_0)
   {
   case 0:// End event was signaled
      return 2;
   case 1:// A command has connected, reset the event to not signaled
      (void)::ResetEvent(m_hEvents[1]);
      break;
   default:// Error
      return 1;
   }
   return 0; // = OK
}//End of Connect
/*===================================================================
   ROUTINE: ExecuteCommand
=================================================================== */
void ACS_EMF_CommandClient::ExecuteCommand()
{
   DWORD dwStatus = 0;
   ACS_EMF_Data srctData;
   // Reset event
   BOOL bContinue = TRUE;
   while (bContinue)
   {
      (void)::ResetEvent(m_hEvents[1]);
      // Read request from the command
      if ((dwStatus = Receive((LPBYTE)&srctData, sizeof(ACS_EMF_Data))) != 0)
      {
         // Normal behaviour, the command had been executed and the client has left
         bContinue = FALSE;
      }
      else
      {
         DWORD dwResult;
		 DWORD ret = EMF_RC_OK ;
         int DVDBoardStatus = GetBoardStatus(DVD);
		 DebugPrint(_T(" DVD Board Status: %d\n"), DVDBoardStatus);
	//-------------------------------CR 149
	if ( DVDBoardStatus != 0 )
		{
		HDEVINFO hDevInfo = NULL;
		hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_CDROM, 0, 0, DIGCF_PRESENT );
		if(hDevInfo != INVALID_HANDLE_VALUE)
			{
			SP_DEVINFO_DATA DevInfoData = {0};
			DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
			if ( SetupDiEnumDeviceInfo(hDevInfo, 0, &DevInfoData) != 0 )
				DVDBoardStatus = 0;
			/* Clean up the device info set */ 
			SetupDiDestroyDeviceInfoList( hDevInfo );
			}
		DebugPrint(_T(" DVD Board Status (APZ 212 60): %d\n"), DVDBoardStatus);
		}
	//-------------------------------CR 149
		 if( DVDBoardStatus != EMF_RC_OK )// Check DVD Board Status on another node
			 ret = m_pClientHandler->GetDVDBoardStatus();
		 if ( ( srctData.Code == EMF_CMD_TYPE_GETMEDIAINFO ||
			    srctData.Code == EMF_CMD_TYPE_GETDVDOWNER  ||
			    srctData.Code == EMF_CMD_TYPE_COPYTOMEDIA    ||
			    srctData.Code == EMF_CMD_TYPE_GETDVDOWNERSHIP)
             && ret != EMF_RC_OK) //External media board isn't working OK. 
			dwResult = EMF_RC_DVDDEVICENOTFOUND ;
		 else if (srctData.Code == EMF_CMD_TYPE_COPYTOMEDIA)
            		dwResult = m_pDVDHandler->DVDOperation(&srctData, m_hPipe);
         	else if (srctData.Code == EMF_CMD_TYPE_COPYFROMTAPE ||
            		srctData.Code == EMF_CMD_TYPE_COPYTOTAPE)
         		   dwResult = m_pTapeHandler->TapeOperation(&srctData, m_hPipe);
         else if (srctData.Code == EMF_CMD_TYPE_TAPELIST)
            dwResult = m_pTapeHandler->TapeOperation(&srctData, m_hPipe, (LPVOID)this);
         else if (srctData.Code == EMF_CMD_TYPE_GETLOGINFO)
            dwResult = m_pClientHandler->GetLogInfo(&srctData);
         else if (srctData.Code == EMF_CMD_TYPE_GETMEDIAINFO)
            dwResult = m_pDVDHandler->DVDOperation(&srctData, m_hPipe);
         else if (srctData.Code == EMF_CMD_TYPE_GETDVDOWNER)
            dwResult = m_pDVDHandler->DVDOperation(&srctData, m_hPipe);
         else if (srctData.Code == EMF_CMD_TYPE_GETDVDOWNERSHIP)
            dwResult = m_pDVDHandler->DVDOperation(&srctData, m_hPipe);
         else
            dwResult = EMF_RC_UNKNOWNCMDTYPE;
         srctData.Code = dwResult;
         // Send back result code (+ data) to the command
         dwStatus = SendRespond((LPBYTE)&srctData, sizeof(ACS_EMF_Data));
         if (dwStatus != 0)
            bContinue = FALSE;
      }
   }
}//End of ExecuteCommand
/*===================================================================
   ROUTINE: ExecuteCommandProc
=================================================================== */
unsigned __stdcall ACS_EMF_CommandClient::ExecuteCommandProc(LPVOID lpvThis)
{
   ((ACS_EMF_CommandClient*)lpvThis)->ExecuteCommand();
   delete (ACS_EMF_CommandClient*)lpvThis;
   _endthreadex(0);

   if (hThread != INVALID_HANDLE_VALUE)
		(void)::CloseHandle(hThread);
   return 0;
}//End of ExecuteCommandProc
/*===================================================================
   ROUTINE: Run
=================================================================== */
BOOL ACS_EMF_CommandClient::Run()
{
   // Start threads
   //HANDLE hThread = (HANDLE)_beginthread(ExecuteCommandProc, 0, this);
  hThread = (HANDLE)_beginthreadex(NULL,0,ExecuteCommandProc, this,  0, NULL);
   if (!hThread)
   {
      // Report error
      _TCHAR szData[256];
      _stprintf(szData,
                _T("Unable to create thread: \"ExecuteCommandProc\" due to the following error:\r\n%s"),
                ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));

      DebugPrint(szData);
      // Send event
      ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
                          EMF_EV_EVENTNUM_INTERNALERROR,
                          szData,
                          EMF_EV_EVENTTEXT_INTERNALERROR);
      return FALSE;
   }
   return TRUE;
}//End of Run
/*===================================================================
   ROUTINE: Receive
=================================================================== */
DWORD ACS_EMF_CommandClient::Receive(LPBYTE lpBuffer, DWORD dwSize)
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
         DebugPrint(_T("WaitForMultipleObjects returned: %u\nError code: %u"), dwObjectIdx, GetLastError());
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
   if (dwBytesRead != dwSize)// Unexpected size of buffer was received
      return 1; // Error
   (void)::ResetEvent(m_hEvents[1]);// All is well. Reset pipe event and continue
   return 0; // OK
}//End of Receive
/*===================================================================
   ROUTINE: SendRespond
=================================================================== */
DWORD ACS_EMF_CommandClient::SendRespond(LPBYTE lpBuffer, DWORD dwSize)
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
   else // Check if client end of pipe has beeing closed
   {
      if (::GetLastError() != ERROR_NO_DATA)
         return 1;
   }
   return 0;
}//End of SendRespond
/*===================================================================
   ROUTINE: SendToCommand
=================================================================== */
DWORD ACS_EMF_CommandClient::SendToCommand(LPBYTE lpBuffer, DWORD dwSize)
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
   else // Check if client end of pipe has beeing closed
   {
      if (::GetLastError() != ERROR_NO_DATA)
         return 1;
   }
   return 0;
}//End of SendToCommand
