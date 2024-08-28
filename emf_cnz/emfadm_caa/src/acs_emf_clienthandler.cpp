/*=================================================================== */
/**
   @file   acs_emf_clienthandler.cpp

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
#include <ctime>
#include <iostream>
#include "acs_emf_defs.h"
#include "acs_emf_dvdhandler.h"
#include "acs_emf_clienthandler.h"
#include "acs_emf_tra.h"
using namespace std;
/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */
#define INVALID_SOCKET -1



/*===================================================================
   ROUTINE: ACS_EMF_ClientHandler
=================================================================== */
ACS_EMF_ClientHandler::ACS_EMF_ClientHandler(ACE_Event* hEndEvent,
        ACE_Recursive_Thread_Mutex* pCriticalSection) : m_hEndEvent(hEndEvent),
                                                        m_pCriticalSection(pCriticalSection)
                                                        //m_Network(FALSE)
{
    DEBUG(1,"%s","ACS_EMF_ClientHandler constructor - Entering\n");

   m_ServerSocket = INVALID_SOCKET;
//   m_hServerEvent = NULL;
  // Using default portnumber of EMF block 14020
   m_nPortNo = EMF_DEFAULT_SOCKSERV_PORTNO;

//   struct servent *pServent = getservbyname(EMF_SERVICE_APPNAME, "tcp");
//   if (pServent)
//      m_nPortNo = ntohs(pServent->s_port);
//   else
//   {
//      // Using default portnumber
//      m_nPortNo = EMF_DEFAULT_SOCKSERV_PORTNO;
//   }


//   // Create event object for handling the queue system
//   m_hSaveLogInfoEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//   m_hLocalLoggingEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
//
//   m_ServerSocket = INVALID_SOCKET;
//   m_hServerEvent = NULL;
//   m_nPortNo = 0;
//
//   *m_szLocalLogfilePath = _T('\0');
//   *m_szConnectIpAddress1 = *m_szConnectIpAddress2 = _T('\0');
//
//   m_bLogLocally = FALSE;
//   m_bEventSent = FALSE;
//
//   ::RtlZeroMemory(&m_srctOL, sizeof(OVERLAPPED));
//
//   // Initialize critical section object
//   try
//   {
//      // The following call can throw an exception
//      ::InitializeCriticalSection(&m_srctCS);
//   }
//   catch (...)
//   {
//      exit(EXIT_FAILURE);
//   }
//
//   WSADATA srctWSAData;
//   if (WSAStartup(MAKEWORD(2, 0), &srctWSAData))
//   {
//      DebugPrint(_T("Windows Socket version 2.0 not supported"));
//   }
//
//   PSERVENT pServent = getservbyname(EMF_SERVICE_APPNAME, "tcp");
//   if (pServent)
//      m_nPortNo = ntohs(pServent->s_port);
//   else
//   {
//      // Report error
//      _TCHAR szData[256];
//      _stprintf(szData,
//                _T("Failed to retrieve service information for the service \"%s\".\r\nThe following socket error code was received: %d"),
//                EMF_SERVICE_APPNAME,
//                WSAGetLastError());
//
//      DebugPrint(szData);
//
//      // Send event
//      ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
//                          EMF_EV_EVENTNUM_SOCKSERVERROR,
//                          szData,
//                          EMF_EV_EVENTTEXT_ERROR,
//                          FALSE);
//
//      m_bEventSent = TRUE;
//
//      // Using default portnumber
//      m_nPortNo = EMF_DEFAULT_SOCKSERV_PORTNO;
//   }
   m_bEventSent = false;
   m_bLogLocally = false;
}//End of ACS_EMF_ClientHandler

/*===================================================================
   ROUTINE: ~ACS_EMF_ClientHandler
=================================================================== */
ACS_EMF_ClientHandler::~ACS_EMF_ClientHandler()
{
   if (m_ServerSocket != INVALID_SOCKET)
   {
      close(m_ServerSocket);
      m_ServerSocket = INVALID_SOCKET;
   }

//
//   if (m_hServerEvent)
//   {
//      (void)WSACloseEvent(m_hServerEvent);
//      m_hServerEvent = NULL;
//   }
//
//   if (m_hLocalLoggingEvent)
//      (void)::CloseHandle(m_hLocalLoggingEvent);
//
//   if (m_hSaveLogInfoEvent)
//      (void)::CloseHandle(m_hSaveLogInfoEvent);
//
//   // Initialize critical section object
//   try
//   {
//      // The following call can throw an exception
//      ::DeleteCriticalSection(&m_srctCS);
//   }
//   catch (...)
//   {
//   }
}//End of ~ACS_EMF_ClientHandler

// COMMENTED THESE FUNCTIONS TO AVOID CPPCHECK ERRORS
#if 0
//============================================================================
// AddLogInfoItem
// Add a new ACS_EMF_DATA items to the queue
//============================================================================
void ACS_EMF_ClientHandler::AddLogInfoItem(ACS_EMF_DATA* pData)
{
//   //LockQueue();
//
//   // Adds a ACS_EMF_DATA object to the STL queue object
//   m_LogBuffer.Push(pData);
//
//   // Notify the UpdateLogInfo thread to process the new operation info
//   (void)::SetEvent(m_hSaveLogInfoEvent);
//
//   //UnlockQueue();
}

//============================================================================
// Client
// 
//============================================================================
void ACS_EMF_ClientHandler::ClientLogging()
{
//   HANDLE hHandles[2] = { m_hEndEvent, m_hSaveLogInfoEvent };
//
//   // Notify the ACS_EMF_Server on the active node that we are online
//   if (!ACS_EMF_Common::CheckActiveNode())
//      SentNotifyToActive();
//
//   ACE_UINT32 dwHandleIdx;
//   BOOL bContinue = TRUE;
//   while (bContinue)
//   {
//      // Wait until end is signaled or a new ACS_EMF_DATA structs shall be
//      // logged
//      dwHandleIdx = ::WaitForMultipleObjects(2, hHandles, FALSE, 285000);
//      if (dwHandleIdx == 0)
//         bContinue = FALSE;
//      else if (dwHandleIdx == 1)
//      {
//         (void)::ResetEvent(m_hSaveLogInfoEvent);
//
//         if (m_LogBuffer.get_NumOfItems() > 0)
//         {
//            // Get number of objects to log
//            int nCount = m_LogBuffer.get_NumOfItems();
//
//            // Check if this is the active node
//            BOOL bIsActiveNode = ACS_EMF_Common::CheckActiveNode();
//            BOOL bSuccess = TRUE;
//
//            for (int nIdx = 0; (nIdx < nCount && bSuccess); nIdx++)
//            {
//               LockClient();
//
//               // Get the first ACS_EMF_DATA structure in the queue
//               ACS_EMF_DATA* pData = m_LogBuffer.Front();
//
//               // if this is not the active node, connect to ACS_EMF_Server's listening
//               // socket on the other node and send to. Otherwise save operation into
//               // to log-file on this node
//               if (bIsActiveNode)
//                  bSuccess = (WriteOperationProgress(pData) == EMF_RC_OK);
//               else
//               {
//                  if (!m_bLogLocally)
//                  {
//                     bSuccess = SendToServer(pData, FALSE);
//                     if (!bSuccess)
//                        StoreLogInfoLocally(pData, FALSE);
//                  }
//                  else
//                     StoreLogInfoLocally(pData, FALSE);
//               }
//
//               // Delete the stored buffer
//               m_LogBuffer.Pop();
//               UnlockClient();
//            }
//         }
//      }
//      else if (dwHandleIdx == WAIT_TIMEOUT)
//      {
//         LockClient();
//
//         if (m_ServerSocket != INVALID_SOCKET)
//         {
//            (void)closesocket(m_ServerSocket);
//            m_ServerSocket = INVALID_SOCKET;
//         }
//
//         UnlockClient();
//      }
//      else
//         bContinue = FALSE;
//   }
}

//============================================================================
// ClientLoggingProc
// Starting function ClientLogging in a new thread
//============================================================================
//unsigned __stdcall ACS_EMF_ClientHandler::ClientLoggingProc(LPVOID lpvThis)
//{
//   ((ACS_EMF_ClientHandler*)lpvThis)->ClientLogging();
//   _endthreadex(0);
//   return 0;
//}
#endif

/*===================================================================
   ROUTINE: CloseServerSocket
=================================================================== */
BOOL ACS_EMF_ClientHandler::CloseServerSocket()
{
	close(m_ServerSocket);
	m_ServerSocket = INVALID_SOCKET;
	return true;
}//End of CloseServerSocket

/*===================================================================
   ROUTINE: SendDataToServerSocket
=================================================================== */
BOOL ACS_EMF_ClientHandler::SendDataToServerSocket(ACS_EMF_DATA* pData)
{
   char send_data[1024];
   ACE_OS::strcpy(send_data,"Hello");
   send(m_ServerSocket,send_data,strlen(send_data),0);
   return true;
}//End of SendDataToServerSocket

/*===================================================================
   ROUTINE: ConnectToServerSocket
=================================================================== */
BOOL ACS_EMF_ClientHandler::ConnectToServerSocket()
{
   DEBUG(1,"%s","ACS_EMF_ClientHandler::ConnectToServerSocket() - Entering\n");
#if 0
   char hostname[8];
   char partner_hostname [8];
   struct hostent *host;
   struct sockaddr_in srctSockAddr;


   // Create a socket
   m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

   if (m_ServerSocket == INVALID_SOCKET)
   {
	   DEBUG(1,"%s","ACS_EMF_ClientHandler::ConnectToServerSocket - Failed to create client socket\n");

      return FALSE;
   }

   gethostname(hostname, sizeof(hostname) );

   if ( ACE_OS::strcmp(hostname,"SC-2-1") == 0 )
   {
       strcpy(partner_hostname, "SC-2-2") ;
   }
   else
   {
	   strcpy(partner_hostname, "SC-2-1") ;
   }

   DEBUG(1,"ACS_EMF_ClientHandler::ConnectToServerSocket - PARTNER NODE = %s\n",partner_hostname);
   host = gethostbyname( partner_hostname );

   bzero((char *) &srctSockAddr, sizeof(srctSockAddr));

   // Specify IP address and port of the server to be connected to

   srctSockAddr.sin_family = AF_INET;

   bcopy((char *)host->h_addr,
         (char *)&srctSockAddr.sin_addr.s_addr,
          host->h_length);

   m_nPortNo = EMF_DEFAULT_SOCKSERV_PORTNO;

   srctSockAddr.sin_port = htons(m_nPortNo);

   // Connect to the server socket on the other node
   if (connect(m_ServerSocket, (const struct sockaddr *)&srctSockAddr, sizeof(srctSockAddr)) < 0 )
   {
	   DEBUG(1,"%s","ACS_EMF_ClientHandler::ConnectToServerSocket - Failed to connect to listening socket on the other node\n");

      close(m_ServerSocket);
      m_ServerSocket = INVALID_SOCKET;

      return FALSE;
   }
#endif
   DEBUG(1,"%s","ACS_EMF_ClientHandler::ConnectToServerSocket - Connected to ACS_EMF_Server on the other node\n");

   return true;



//   DebugPrint(_T("ConnectToServerSocket"));
//
//   if (m_nPortNo == 0)
//   {
//      PSERVENT pServent = getservbyname(EMF_SERVICE_APPNAME, "tcp");
//      if (pServent)
//         m_nPortNo = ntohs(pServent->s_port);
//      else
//      {
//         // Using default portnumber
//         m_nPortNo = EMF_DEFAULT_SOCKSERV_PORTNO;
//      }
//   }
//
//   Lock();
//
//   if (!m_Network.GetInternalAddresses())
//   {
//      DebugPrint(_T("Failed to read the internal heartbeat network IP addresses for the other node"));
//
//      Unlock();
//      return FALSE;
//   }
//   else
//   {
//      const _TCHAR* lpszOtherIp = m_Network.get_OtherFirstIP();
//      if (*lpszOtherIp && (_tcscmp(lpszOtherIp, m_szConnectIpAddress1) != 0))
//         _tcscpy(m_szConnectIpAddress1, lpszOtherIp);
//
//      lpszOtherIp = m_Network.get_OtherSecondIP();
//      if (*lpszOtherIp && (_tcscmp(lpszOtherIp, m_szConnectIpAddress2) != 0))
//         _tcscpy(m_szConnectIpAddress2, lpszOtherIp);
//
//      DebugPrint(_T("Connect IP address 1: %s\nConnect IP address 2: %s\n"),
//                 m_szConnectIpAddress1,
//                 m_szConnectIpAddress2);
//   }
//
//   Unlock();
//
//   // Create client socket event object
//   if (!m_hServerEvent)
//   {
//      m_hServerEvent = WSACreateEvent();
//      if (!m_hServerEvent)
//      {
//         DebugPrint(_T("Failed to create client socket event object"));
//
//         return FALSE;
//      }
//   }
//
//   // Create a socket
//   m_ServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
//   if (m_ServerSocket == INVALID_SOCKET)
//   {
//      DebugPrint(_T("Failed to create client socket"));
//
//      return FALSE;
//   }
//
//   // Specify IP address and port of the server to be connected to
//   SOCKADDR_IN srctSockAddr;
//   srctSockAddr.sin_family = AF_INET;
//   srctSockAddr.sin_addr.s_addr = inet_addr(m_szConnectIpAddress1);
//   srctSockAddr.sin_port = htons(m_nPortNo);
//
//   // Connect to the server socket on the other node
//   if (WSAConnect(m_ServerSocket, (SOCKADDR*)&srctSockAddr, sizeof(srctSockAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
//   {
//      DebugPrint(_T("Failed to connect to listening socket on the other node"));
//
//      (void)closesocket(m_ServerSocket);
//      m_ServerSocket = INVALID_SOCKET;
//
//      return FALSE;
//   }
//
//   DebugPrint(_T("\nConnected to ACS_EMF_Server on the other node at IP address \"%s\""),
//              m_szConnectIpAddress1);
//
//   return TRUE;
}//End of ConnectToServerSocket

//============================================================================
/*===================================================================
   ROUTINE: CopyToDVDRemote
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::CopyToDVDRemote(ACS_EMF_DATA* pData,
                                             ACE_HANDLE hImageFile)
{
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("Copying CD/DVD image file to other node/AP request received.\n"));
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(pData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      pData->Code = EMF_CMD_TYPE_COPYFILEDATA;
//
//      ACE_UINT32 dwBytes = 0;
//      LARGE_INTEGER nFileDataSent;
//      nFileDataSent.QuadPart = 0;
//
//      BOOL bContinue = TRUE, bSuccess;
//      while (bContinue)
//      {
//         bSuccess = ::ReadFile(hImageFile,
//                               pData->Data,
//                               ACS_EMF_DATA_LEN,
//                               &dwBytes,
//                               NULL);
//
//         if (!bSuccess)
//         {
//            dwResult = EMF_RC_IMAGEFILEFILEERROR;
//            bContinue = FALSE;
//         }
//         else if (dwBytes == 0)
//         {
//            bContinue = FALSE;
//         }
//         else
//         {
//            if (!SendToServer(pData, FALSE))
//            {
//               dwResult = EMF_RC_SERVERNOTRESPONDING;
//               bContinue = FALSE;
//            }
//         }
//      }
//
//      if (dwResult == EMF_RC_OK)
//      {
//         pData->Code = EMF_CMD_TYPE_ENDCOPYFILEDATA;
//
//         if (!SendToServer(pData, TRUE))
//            dwResult = EMF_RC_SERVERNOTRESPONDING;
//      }
//
//      if (dwResult == EMF_RC_OK)
//         DebugPrint(_T("Copying CD/DVD image file to other node/AP done.\n"));
//      else
//         DebugPrint(_T("Copying CD/DVD image file to other node/AP failed.\n"));
//   }
//
//   UnlockClient();
//
//   return dwResult;
}//End of CopyToDVDRemote

/*===================================================================
   ROUTINE: GenerateLogFileName
=================================================================== */
void ACS_EMF_ClientHandler::GenerateLogFileName(ACS_EMF_DVDOperation& Oper)
{
//   // Set operation start time
//   if (Oper.nStartTime == 0)
//   {
//      Oper.nStartTime = (long)time(NULL);
//
//      ACE_UINT32 dwLen = 64;
//      (void)::GetComputerName(Oper.szLogfile, &dwLen);
//      _stprintf(Oper.szLogfile+dwLen, _T(".%d"), Oper.nStartTime);
//   }
}//End of GenerateLogFileName


/*===================================================================
   ROUTINE: GenerateLogFileName
=================================================================== */
void ACS_EMF_ClientHandler::GenerateLogFileName(ACS_EMF_TapeOperation& Oper)
{
//   // Set operation start time
//   if (Oper.nStartTime == 0)
//   {
//      Oper.nStartTime = (long)time(NULL);
//
//      ACE_UINT32 dwLen = 64;
//      (void)::GetComputerName(Oper.szLogfile, &dwLen);
//      _stprintf(Oper.szLogfile+dwLen, _T(".%d"), Oper.nStartTime);
//   }
}//End of GenerateLogFileName

/*===================================================================
   ROUTINE: GetDVDOwner
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::GetDVDOwner(ACS_EMF_DATA* pData)
{
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("Get-DVD-owner command received.\n"));
//   DebugPrint(_T("Sending Get-DVD-owner command to the other node for execution.\n"));
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(pData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      DebugPrint(_T("Get-DVD-owner command execution done.\n"));
//      dwResult = pData->Code;
//   }
//
//   UnlockClient();
//
//   return dwResult;
	return EMF_RC_OK;
}//End of GetDVDOwner

/*===================================================================
   ROUTINE: GetDVDState
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::GetDVDState()
{
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("Get-DVD-state command received.\n"));
//   DebugPrint(_T("Sending Get-DVD-state command to the other node for execution.\n"));
//
//   ACS_EMF_DATA srctData;
//   srctData.Code = EMF_CMD_TYPE_GETDVDSTATE;
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(&srctData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      DebugPrint(_T("Get-DVD-state command execution done.\n"));
//      dwResult = srctData.Code;
//   }
//
//   UnlockClient();
//
//   return dwResult;
	return EMF_RC_OK;
}//End of GetDVDState

/*===================================================================
   ROUTINE: RemoveSATADevice
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::RemoveSATADevice()
{
   
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("RemoveSATADevice command received.\n"));
//   DebugPrint(_T("Sending RemoveSATADevice command to the other node for execution.\n"));
//
//   ACS_EMF_DATA srctData;
//   srctData.Code = EMF_CMD_TYPE_REMOVEDVD;
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(&srctData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      DebugPrint(_T("RemoveSATADevice command execution done.\n"));
//      dwResult = srctData.Code;
//   }
//
//   UnlockClient();
//
//   return dwResult;
	return EMF_RC_OK;
}//End of RemoveSATADevice

/*===================================================================
   ROUTINE: GetDVDBoardStatus
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::GetDVDBoardStatus()
{
   
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("GetDVDBoardStatus command received.\n"));
//   DebugPrint(_T("Sending GetDVDBoardStatus command to the other node for execution.\n"));
//
//   ACS_EMF_DATA srctData;
//   srctData.Code = EMF_CMD_TYPE_CHECKDVDBOARDSTATUS;
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(&srctData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      DebugPrint(_T("GetDVDBoardStatus command execution done.\n"));
//      dwResult = srctData.Code;
//   }
//
//   UnlockClient();
//
//   return dwResult;
	return EMF_RC_OK;
}//End of GetDVDBoardStatus

/*===================================================================
   ROUTINE: GetLogInfo
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::GetLogInfo(ACS_EMF_DATA* pData)
{
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   LockClient();
//
//   DebugPrint(_T("Get-Log-info command received.\n"));
//
//   if (ACS_EMF_Common::CheckActiveNode())
//      dwResult = ::GetLogInfo(pData, NULL, NULL);
//   else
//   {
//      BOOL bGetLogLocally = m_bLogLocally;
//      if (!bGetLogLocally)
//      {
//         DebugPrint(_T("Sending Get-Log-info command to other (active) node for execution.\n"));
//
//         // if this is not the active node, connect to ACS_EMF_Server's listening
//         // socket on the other node and send to
//         if (SendToServer(pData, TRUE))
//            dwResult = EMF_RC_OK;
//         else
//            bGetLogLocally = TRUE;
//      }
//
//      if (bGetLogLocally)
//      {
//         DebugPrint(_T("The ACS_EMF_Server on the active node not responding."));
//
//         if (*m_szLocalLogfilePath == _T('\0'))
//            ACS_EMF_Common::GetEMFLocalLogFilePath(m_szLocalLogfilePath);
//
//         // Check if data is stored on the local system disk
//         LONG nNumOfFiles;
//         dwResult = ::GetLogInfo(pData, m_szLocalLogfilePath, &nNumOfFiles);
//         if (dwResult == EMF_RC_OK)
//            dwResult = EMF_RC_SERVERONACTIVENOTRESPONDING;
//
//         if (nNumOfFiles > 0)
//            DebugPrint(_T("Copy information found and read from the local system disk.\n"));
//         else
//            DebugPrint(_T("No copy information found on the local system disk.\n"));
//      }
//   }
//
//   DebugPrint(_T("Get-Log-info command execution done.\n"));
//
//   UnlockClient();
//
//   return dwResult;
	return EMF_RC_OK;
}//End of GetLogInfo

/*===================================================================
   ROUTINE: InitCopyToDVD
=================================================================== */
ACE_UINT32 ACS_EMF_ClientHandler::InitCopyToDVD(ACS_EMF_DATA* pData)
{
//   ACE_UINT32 dwResult = EMF_RC_OK;
//
//   DebugPrint(_T("Init Copy-to-DVD remotely request received.\n"));
//
//   // Connect to ACS_EMF_Server's listening socket on the other node and send
//   // the request
//   if (!SendToServer(pData, TRUE))
//      dwResult = EMF_RC_SERVERNOTRESPONDING;
//   else
//   {
//      // Copy the result code
//      dwResult = pData->Code;
//
//      DebugPrint(_T("Init Copy-to-DVD remotely request execution done.\n"));
//   }
//
//   return dwResult;
	return EMF_RC_OK;
}//End of InitCopyToDVD

#if 0
//============================================================================
// LocalLogging
// 
//============================================================================
void ACS_EMF_ClientHandler::LocalLogging()
{
//   HANDLE hHandles[2] = { m_hEndEvent, m_hLocalLoggingEvent };
//
//   if (*m_szLocalLogfilePath == _T('\0'))
//      ACS_EMF_Common::GetEMFLocalLogFilePath(m_szLocalLogfilePath);
//
//   ACE_UINT32 dwHandleIdx;
//   ACE_UINT32 dwWait = 1000*15;
//   ACS_EMF_DATA* pData;
//   ACS_EMF_ParamList Param;
//
//   BOOL bContinue = TRUE;
//   while (bContinue)
//   {
//      dwHandleIdx = ::WaitForMultipleObjects(2, hHandles, FALSE, dwWait);
//      if (dwHandleIdx == 0)
//         bContinue = FALSE;
//      else if (dwHandleIdx == 1)
//      {
//         (void)::ResetEvent(m_hLocalLoggingEvent);
//         dwWait = 1000*15;
//      }
//      else if (dwHandleIdx == WAIT_TIMEOUT)
//      {
//         if (::GetLogInfo(Param, m_szLocalLogfilePath) == EMF_RC_OK && Param.NumOfData() > 0)
//         {
//            DebugPrint(_T("Number of files stored locally: %d\nLocal directory: %s"),
//                       (int)(Param.NumOfData() / 2),
//                       m_szLocalLogfilePath);
//
//            // Check if this is the active node
//            BOOL bIsActiveNode = ACS_EMF_Common::CheckActiveNode();
//
//            LockClient();
//
//            if (bIsActiveNode)
//            {
//               DebugPrint(_T("Move log-files to the data disk"));
//
//               // Try to move the files to the data disk
//               (void)MoveLogfiles(Param, m_szLocalLogfilePath);
//               m_bLogLocally = FALSE;
//               dwWait = INFINITE;
//            }
//            else
//            {
//               // Encode the data
//               Param.Encode();
//
//               // Copy buffer to ACS_EMF_DATA data member
//               pData = (ACS_EMF_DATA*)::HeapAlloc(::GetProcessHeap(),
//                                                  HEAP_ZERO_MEMORY,
//                                                  sizeof(ACS_EMF_DATA));
//
//               if (pData)
//               {
//                  pData->Code = EMF_CMD_TYPE_UPDATEPROGRESS;
//                  (void)Param.getEncodedBuffer(pData->Data);
//
//                  if (SendToServer(pData, FALSE))
//                  {
//                     DebugPrint(_T("Log-files successfully sent to active node.\nDelete locally stored log-files.\n"));
//
//                     // Delete the files from the local system
//                     (void)DeleteLogfiles(Param, m_szLocalLogfilePath);
//                     m_bLogLocally = FALSE;
//                     dwWait = INFINITE;
//                  }
//
//                  (void)::HeapFree(::GetProcessHeap(), 0, pData);
//               }
//            }
//
//            UnlockClient();
//         }
//         else
//         {
//            if (Param.NumOfData() == 0)
//               dwWait = INFINITE;
//         }
//
//         Param.Clear();
//      }
//      else
//         bContinue = FALSE;
//   }
}

//============================================================================
// LocalLoggingProc
// Starting function LocalLogging in a new thread
//============================================================================
//unsigned __stdcall ACS_EMF_ClientHandler::LocalLoggingProc(LPVOID lpvThis)
//{
//   ((ACS_EMF_ClientHandler*)lpvThis)->LocalLogging();
//   _endthreadex(0);
//   return 0;
//}

//============================================================================
// Lock
// Request ownership to the critical section object
//============================================================================
void ACS_EMF_ClientHandler::Lock()
{
//   try
//   {
//      // The following call can throw an exception
//      if (m_pCriticalSection)
//         ::EnterCriticalSection(m_pCriticalSection);
//   }
//   catch (...)
//   {
//      exit(EXIT_FAILURE);
//   }
}

//============================================================================
// LockClient
// Request ownership to the critical section object
//============================================================================
void ACS_EMF_ClientHandler::LockClient()
{
//   try
//   {
//      // The following call can throw an exception
//      ::EnterCriticalSection(&m_srctCS);
//   }
//   catch (...)
//   {
//      exit(EXIT_FAILURE);
//   }
}
#endif

/*===================================================================
   ROUTINE: ClientHandler
=================================================================== */
void ACS_EMF_ClientHandler::ClientHandler()
{
//	ACE_INT32 s32Wait = 1;
//	ACE_Time_Value oTimeValue(s32Wait);
//
//	while (m_hEndEvent->wait(&oTimeValue,0))
//	{
//
//	}
//	cout << "Exit from client Handler thread" << endl;



//   HANDLE hHandles[3];
//
//   // Save reference to end event
//   hHandles[0] = m_hEndEvent;
//
//   //hHandles[1] = (HANDLE)_beginthread(ClientLoggingProc, 0, this);
//   hHandles[1] = (HANDLE)_beginthreadex(NULL,0, ClientLoggingProc, this, 0, NULL);
//   if (!hHandles[1])
//   {
//      // Report error
//      _TCHAR szData[256];
//      _stprintf(szData,
//                _T("Unable to create thread: \"ClienLoggingProc\" due to the following error:\r\n%s"),
//                ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
//
//      DebugPrint(szData);
//
//      // Send event
//      ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
//                          EMF_EV_EVENTNUM_INTERNALERROR,
//                          szData,
//                          EMF_EV_EVENTTEXT_INTERNALERROR);
//
//      (void)::WaitForSingleObject(hHandles[0], INFINITE);
//      return;
//   }
//
//   //hHandles[2] = (HANDLE)_beginthread(LocalLoggingProc, 0, this);
//   hHandles[2] = (HANDLE)_beginthreadex(NULL,0,LocalLoggingProc, this, 0, NULL);
//   if (!hHandles[2])
//   {
//      // Report error
//      _TCHAR szData[256];
//      _stprintf(szData,
//                _T("Unable to create thread: \"LocalLoggingProc\" due to the following error:\r\n%s"),
//                ACS_EMF_Common::GetWindowsErrorText(::GetLastError()));
//
//      DebugPrint(szData);
//
//      // Send event
//      ACS_EMF_ReportEvent(EMF_EV_SEVERITY_EVENT,
//                          EMF_EV_EVENTNUM_INTERNALERROR,
//                          szData,
//                          EMF_EV_EVENTTEXT_INTERNALERROR);
//
//      (void)::SetEvent(hHandles[0]);
//      (void)::WaitForSingleObject(hHandles[1], INFINITE);
//      return;
//   }
//
//   ACE_UINT32 dwHandleIdx = ::WaitForMultipleObjects(3, hHandles, FALSE, INFINITE);
//   switch (dwHandleIdx - WAIT_OBJECT_0)
//   {
//   case 0:
//      (void)::WaitForMultipleObjects(3, hHandles, TRUE, INFINITE);
//      break;
//
//   case 1:
//      (void)::SetEvent(hHandles[0]);
//      (void)::WaitForMultipleObjects(3, hHandles, TRUE, INFINITE);
//      break;
//
//   case 2:
//      (void)::SetEvent(hHandles[0]);
//      (void)::WaitForMultipleObjects(3, hHandles, TRUE, INFINITE);
//      break;
//
//   default:
//      (void)::SetEvent(hHandles[0]);
//      (void)::WaitForMultipleObjects(3, hHandles, TRUE, INFINITE);
//      break;
//   }
//
//   (void)::CloseHandle(hHandles[0]);
//   (void)::CloseHandle(hHandles[1]);
//   (void)::CloseHandle(hHandles[2]);
//
//   ::Sleep(200);
}//End of ClientHandler

//============================================================================
// ReceiveFromServerSocket
// Read data from server blocking socket
//============================================================================
/*
ACE_UINT32 ACS_EMF_ClientHandler::ReceiveFromServerSocket(LPWSABUF lpBuffer)
{
//   fd_set srctFdSet;
//   FD_ZERO(&srctFdSet);
//   FD_SET(m_ServerSocket, &srctFdSet);
//   TIMEVAL srctTV;
//
//   // 3 min default timeout
//   srctTV.tv_sec  = 30;
//   srctTV.tv_usec = 0;
//
//   // Put client socket in read mode
//   int nReturn = select(0, &srctFdSet, NULL, NULL, &srctTV);
//   if (nReturn <= 0)
//   {
//      DebugPrint(_T("select() returned: %d"), nReturn);
//
//      return (nReturn == 0 ? 3 : 1);
//   }
//
//   int nRecv = 0;
//   while (nRecv < (int)lpBuffer->len)
//   {
//      nReturn = recv(m_ServerSocket, lpBuffer->buf+nRecv, lpBuffer->len-nRecv, 0);
//      if (nReturn <= 0)
//      {
//         DebugPrint(_T("recv() returned: %d. Socket error code: %u"), nReturn, WSAGetLastError());
//
//         // Connection closed on the other side
//         if (nReturn == 0 ||
//            WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAECONNRESET)
//            return 2;
//         else
//            return 1;
//      }
//      else
//         nRecv += nReturn;
//   }
//
//   return 0;
}
*/
//============================================================================
// SendToServerSocket
// Send data to server blocking socket
//============================================================================
/*
ACE_UINT32 ACS_EMF_ClientHandler::SendToServerSocket(LPWSABUF lpBuffer)
{

//   fd_set srctFdSet;
//   FD_ZERO(&srctFdSet);
//   FD_SET(m_ServerSocket, &srctFdSet);
//   TIMEVAL srctTV;
//
//   // 30 sec default timeout
//   srctTV.tv_sec  = 60;//Modified the value from 30 sec to 60 sec (CNI1012)
//   srctTV.tv_usec = 0;
//
//   // For blocking socket
//   int nReturn = select(0, NULL, &srctFdSet, NULL, &srctTV);
//   if (nReturn == 0 || nReturn == SOCKET_ERROR)
//   {
//      DebugPrint(_T("select() returned: %d"), nReturn);
//      return 1;
//   }
//
//   // Send string buffer to client
//   int nBytesSent = 0;
//   do
//   {
//      nReturn = send(m_ServerSocket, lpBuffer->buf+nBytesSent, lpBuffer->len-nBytesSent, 0);
//      if (nReturn == SOCKET_ERROR)
//      {
//         // If the client hasn't received the previous buffer yet. Try again
//         if (WSAGetLastError() == WSAEWOULDBLOCK)
//            ::Sleep(10);
//         else
//         {
//            DebugPrint(_T("send() returned: %d. Socket error code: %u"), nReturn, WSAGetLastError());
//
//            if (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAECONNABORTED)
//               return 2; // Connection aborted
//            else
//               return 1; // Other error
//         }
//      }
//      else
//         nBytesSent += nReturn;
//   }
//   while (nBytesSent < (int)lpBuffer->len);
//
//   return 0;
}
*/

/*===================================================================
   ROUTINE: SentNotifyToActive
=================================================================== */
void ACS_EMF_ClientHandler::SentNotifyToActive()
{
//   _TCHAR szNodename[64];
//   ACE_UINT32 dwLen = 63;
//   (void)::GetComputerName(szNodename, &dwLen);
//
//   // Save computername name.
//   ACS_EMF_ParamList param;
//   param.Data[0] = (const _TCHAR*)szNodename;
//
//   // Encode the data
//   param.Encode();
//
//   // Copy data to ACS_EMF_DATA structure
//   ACS_EMF_DATA srctData;
//   srctData.Code = EMF_CMD_TYPE_SERVERONPASSIVEONLINE;
//   (void)param.getEncodedBuffer(srctData.Data);
//
//   LockClient();
//
//   // Send data to server
//   BOOL bSuccess = SendToServer(&srctData, FALSE);
//
//   if (bSuccess)
//      DebugPrint(_T("Notify successfully sent to ACS_EMF_Server on the active node.\n"));
//   else
//      DebugPrint(_T("Failed to send notification to ACS_EMF_Server on the active node.\n"));
//
//   UnlockClient();
}//End of SentNotifyToActive

/*===================================================================
   ROUTINE: SendToServer
=================================================================== */
BOOL ACS_EMF_ClientHandler::SendToServer(ACS_EMF_DATA* pData,
                                         const BOOL bReceiveResponse)
{
//   BOOL bNewConnection = FALSE;
//
//   if (m_ServerSocket == INVALID_SOCKET)
//   {
//      if (!ConnectToServerSocket())
//         return FALSE;
//      else
//         bNewConnection = TRUE;
//   }
//
//   WSABUF srctBuffer;
//   srctBuffer.len = sizeof(ACS_EMF_DATA);
//   srctBuffer.buf = (char*)pData;
//
//   ACE_UINT32 dwResult = SendToServerSocket(&srctBuffer);
//   if (dwResult == 0)
//   {
//      if (bReceiveResponse)
//         dwResult = ReceiveFromServerSocket(&srctBuffer);
//   }
//
//   if (dwResult != 0)
//   {
//      DebugPrint(_T("Reception of data failed. Errorcode: %u.\n"), dwResult);
//   }
//
//   // Server side has closed the socket
//   if (dwResult == 2)
//   {
//      if (m_ServerSocket != INVALID_SOCKET)
//      {
//         (void)closesocket(m_ServerSocket);
//         m_ServerSocket = INVALID_SOCKET;
//      }
//
//      if (!bNewConnection)
//      {
//         if (!ConnectToServerSocket())
//            return FALSE;
//
//         if ((dwResult = SendToServerSocket(&srctBuffer)) == 0)
//         {
//            if (bReceiveResponse)
//               dwResult = ReceiveFromServerSocket(&srctBuffer);
//         }
//
//         if (dwResult != 0)
//         {
//            DebugPrint(_T("Reception of data failed (2:nd time). Errorcode: %u.\n"), dwResult);
//         }
//
//         if (dwResult == 2)
//         {
//            if (m_ServerSocket != INVALID_SOCKET)
//            {
//               (void)closesocket(m_ServerSocket);
//               m_ServerSocket = INVALID_SOCKET;
//            }
//         }
//      }
//   }
//
//   return (dwResult != 1);
	return true;
}//End of SendToServer

// COMMENTED TO AVOID CPPCHECK ERRORS
#if 0
//============================================================================
// StoreLogInfoDirectly
// Store a ACS_EMF_DATA structure directly
//============================================================================
void ACS_EMF_ClientHandler::StoreLogInfoDirectly(ACS_EMF_DATA* pData)
{
//   // Check if this is the active node
//   BOOL bIsActiveNode = ACS_EMF_Common::CheckActiveNode();
//
//   LockClient();
//
//   // if this is not the active node, connect to ACS_EMF_Server's listening
//   // socket on the other node and send to. Otherwise save operation into
//   // to log-file on this node
//   if (bIsActiveNode)
//      (void)WriteOperationProgress(pData);
//   else
//   {
//      if (!m_bLogLocally)
//      {
//         if (!SendToServer(pData, FALSE))
//            StoreLogInfoLocally(pData, TRUE);
//      }
//      else
//         StoreLogInfoLocally(pData, TRUE);
//   }
//
//   // Delete the stored buffer
//   (void)::HeapFree(GetProcessHeap(), 0, pData);
//
//   UnlockClient();
}

//============================================================================
// StoreLogInfoLocally
// Store a ACS_EMF_DATA structure locally on the system disk
//============================================================================
void ACS_EMF_ClientHandler::StoreLogInfoLocally(ACS_EMF_DATA* pData,
                                                   BOOL bEndEventSignaled)
{
//   if (*m_szLocalLogfilePath == _T('\0'))
//      ACS_EMF_Common::GetEMFLocalLogFilePath(m_szLocalLogfilePath);
//
//   (void)WriteOperationProgress(pData, m_szLocalLogfilePath);
//
//   if (!bEndEventSignaled)
//   {
//      if (!m_bLogLocally)
//         m_bLogLocally = TRUE;
//
//      // Tell the 'LocalLogging' thread to wake up
//      (void)::SetEvent(m_hLocalLoggingEvent);
//   }
}

//============================================================================
// Unlock
// Release ownership of the critical section object
//============================================================================
void ACS_EMF_ClientHandler::Unlock()
{
//   try
//   {
//      // The following call can throw an exception
//      if (m_pCriticalSection)
//         ::LeaveCriticalSection(m_pCriticalSection);
//   }
//   catch (...)
//   {
//      exit(EXIT_FAILURE);
//   }
}

//============================================================================
// UnlockClient
// Release ownership of the critical section object
//============================================================================
void ACS_EMF_ClientHandler::UnlockClient()
{
//   try
//   {
//      // The following call can throw an exception
//      ::LeaveCriticalSection(&m_srctCS);
//   }
//   catch (...)
//   {
//      exit(EXIT_FAILURE);
//   }
}
#endif
//============================================================================
// UpdateOperationProgress
// Create a ACS_EMF_DATA structure with the current DVD operation status and
// post ot to a STL queue object which will be processed by LogServerHandler
// thread.
//============================================================================
/*
void ACS_EMF_ClientHandler::UpdateOperationProgress(ACS_EMF_DVDOperation& Oper,
                                                    DVDOperationUpdateType UpdateType)
{
//   if (UpdateType == dutOperationInit)
//   {
//      // Generate a log-file name
//      GenerateLogFileName(Oper);
//
//      Oper.nEndTime = 0;
//      Oper.nCurrentFile = -1;
//   }
//
//   _TCHAR szBuffer[4096];
//   ACS_EMF_ParamList param;
//
//   // Save logfile name. Used by the logging thread when creating the log-file
//   param.Data[0] = (const _TCHAR*)Oper.szLogfile;
//
//   // Hostname
//   SIZE_T nLen = _stprintf(szBuffer, _T("Nodename:                         %s\r\n"), Oper.szHost);
//
//   // Tape device state: "IDLE" or "PROCESSING"
//   if (UpdateType == dutOperationEnd ||
//      UpdateType == dutEraseMediaError ||
//      UpdateType == dutWriteTrackError ||
//      UpdateType == dutDiscOrDeviceInitError ||
//      UpdateType == dutOperationCanceled)
//      _tcscpy(Oper.szState, _T("IDLE"));
//   else
//      _tcscpy(Oper.szState, _T("PROCESSING"));
//
//   nLen += _stprintf(szBuffer+nLen, _T("State:                            %s\r\n\r\n"), Oper.szState);
//
//   //Operation: "Copying To Tape" or "Copying From Tape"
//   if (Oper.dwOperation == EMF_CMD_TYPE_COPYTODVD || Oper.dwOperation == EMF_CMD_TYPE_COPYIMAGEFILETODVD)
//      _tcscpy(Oper.szOperation, _T("Copying To DVD"));
//   else
//      _tcscpy(Oper.szOperation, _T("Unknown"));
//
//   nLen += _stprintf(szBuffer+nLen, _T("Current/last operation:           %s\r\n"), Oper.szOperation);
//   nLen += _stprintf(szBuffer+nLen, _T("Result:                           %s\r\n"), Oper.szResult);
//
//   // Username: domain\accodutname
//   nLen += _stprintf(szBuffer+nLen, _T("User:                             %s\r\n"), Oper.szUser);
//
//   // Operation start time
//   nLen += _stprintf(szBuffer+nLen,
//                     _T("Operation start time:             %s\r\n"),
//                     ACS_EMF_Common::GetDateTimeString(Oper.nStartTime));
//
//   // Operation stop time
//   if (UpdateType == dutOperationEnd ||
//      UpdateType == dutDiscOrDeviceInitError ||
//      UpdateType == dutEraseMediaError ||
//      UpdateType == dutWriteTrackError ||
//      UpdateType == dutOperationCanceled)
//   {
//      // Update the end time
//      if (Oper.nEndTime == 0)
//         Oper.nEndTime = (long)time(NULL);
//
//      nLen += _stprintf(szBuffer+nLen,
//                        _T("Operation end time:               %s\r\n"),
//                        ACS_EMF_Common::GetDateTimeString(Oper.nEndTime));
//   }
//   else
//      nLen += _stprintf(szBuffer+nLen, _T("Operation end time:               \r\n"));
//
//   // File(s) and copy status: "Done", "n%", "Queued", "Writing", "Verifying", "Failed" or "Cancelled"
//   _TCHAR* lpszFile;
//   for (int nIdx = 0; nIdx < Oper.nNumOfFiles; nIdx++)
//   {
//      if (lpszFile = _tcsrchr(Oper.pFiles[nIdx].Filename, _T('\\')))
//         lpszFile++;
//      else
//         lpszFile = Oper.pFiles[nIdx].Filename;
//
//      if (UpdateType == dutOperationInit)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Queued\r\n"), lpszFile);
//      else if (UpdateType == dutDiscOrDeviceInitError ||
//         UpdateType == dutOperationCanceled ||
//         UpdateType == dutEraseMediaError)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Cancelled\r\n"), lpszFile);
//      else if (UpdateType == dutStartEraseMedia)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Erasing/formatting\r\n"), lpszFile);
//      else if (UpdateType == dutUpdateEraseMediaProgress)
//      {
//         if (Oper.dwPercentErased > 0)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% erased/formatted\r\n"), lpszFile, Oper.dwPercentErased);
//         else
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Erasing/formatting\r\n"), lpszFile);
//      }
//      else if (UpdateType == dutAddToTrack)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Copying to track\r\n"), lpszFile);
//      else if (UpdateType == dutWriteTrackError)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Failed\r\n"), lpszFile);
//      else if (UpdateType == dutOperationEnd)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Done\r\n"), lpszFile);
//      else if (UpdateType == dutStartWriteTrack)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Writing\r\n"), lpszFile);
//      else if (UpdateType == dutUpdateWriteTrackProgress)
//      {
//         if (Oper.dwPercentWritten > 0)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% written\r\n"), lpszFile, Oper.dwPercentWritten);
//         else
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Writing\r\n"), lpszFile);
//      }
//      else if (UpdateType == dutStartWriteImage)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Writing physical image file\r\n"), lpszFile);
//      else if (UpdateType == dutUpdateWriteTrackProgress)
//      {
//         if (Oper.dwPercentWritten > 0)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% written to physical image file\r\n"), lpszFile, Oper.dwPercentWritten);
//         else
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Writing physical image file\r\n"), lpszFile);
//      }
//      else if (UpdateType == dutCopyImageFile)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Copying image file\r\n"), lpszFile);
//      else if (UpdateType == dutStartSessionFixation)
//         nLen += _stprintf(szBuffer+nLen, _T("  %-31s Fixating\r\n"), lpszFile);
//      else if (UpdateType == dutUpdateSessionFixation)
//      {
//         if (Oper.dwPercentFixated > 0)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% fixated\r\n"), lpszFile, Oper.dwPercentFixated);
//         else
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Fixating\r\n"), lpszFile);
//      }
//    else if (UpdateType == dutUpdateVerifyDataProgress)
//      {
//    if (Oper.dwPercentVerified > 0)
//        nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% verified\r\n"), lpszFile, Oper.dwPercentVerified);
//         else
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Verifying\r\n"), lpszFile);
//      }
//
//   }
//
//   // Add the printodut buffer
//   param.Data[1] = (const _TCHAR*)szBuffer;
//
//   // Encode the data
//   param.Encode();
//
//   ACS_EMF_DATA* pData = (ACS_EMF_DATA*)::HeapAlloc(::GetProcessHeap(),
//                                                    HEAP_ZERO_MEMORY,
//                                                    sizeof(ACS_EMF_DATA));
//
//   if (!pData)
//      return;
//
//   // Copy buffer to ACS_EMF_DATA data member
//   pData->Code = EMF_CMD_TYPE_UPDATEPROGRESS;
//   (void)param.getEncodedBuffer(pData->Data);
//
//   if (UpdateType == dutOperationCanceled)
//   {
//      // Go ahead and log the operation information directly since the service is
//      // abodut to go offline
//      StoreLogInfoDirectly(pData);
//   }
//   else
//   {
//      // Add the new ACS_EMF_DATA structure to the queue
//      AddLogInfoItem(pData);
//   }
}
*/
//============================================================================
// UpdateOperationProgress
// Create a ACS_EMF_DATA structure with the current tape operation status and
// post ot to a STL queue object which will be processed by LogServerHandler
// thread.
//============================================================================
/*
void ACS_EMF_ClientHandler::UpdateOperationProgress(ACS_EMF_TapeOperation& Oper,
                                                    TapeOperationUpdateType UpdateType)
{
//   if (UpdateType == tutOperationInit)
//   {
//      // Generate a log-file name
//      GenerateLogFileName(Oper);
//
//      Oper.nEndTime = 0;
//      Oper.nCurrentFile = -1;
//   }
//
//   _TCHAR szBuffer[4096];
//   ACS_EMF_ParamList param;
//
//   // Save logfile name. Used by the logging thread when creating the log-file
//   param.Data[0] = (const _TCHAR*)Oper.szLogfile;
//
//   // Hostname
//   SIZE_T nLen = _stprintf(szBuffer, _T("Nodename:                         %s\r\n"), Oper.szHost);
//
//   // Tape device state: "IDLE" or "PROCESSING"
//   if (UpdateType == tutOperationEnd ||
//      UpdateType == tutFileCopyError ||
//      UpdateType == tutTapeOrDeviceInitError ||
//      UpdateType == tutCryptHashError ||
//      UpdateType == tutOperationCanceled)
//      _tcscpy(Oper.szState, _T("IDLE"));
//   else
//      _tcscpy(Oper.szState, _T("PROCESSING"));
//
//   nLen += _stprintf(szBuffer+nLen, _T("State:                            %s\r\n\r\n"), Oper.szState);
//
//   //Operation: "Copying To Tape" or "Copying From Tape"
//   if (Oper.dwOperation == EMF_CMD_TYPE_COPYTOTAPE)
//      _tcscpy(Oper.szOperation, _T("Copying To Tape"));
//   else
//      _tcscpy(Oper.szOperation, _T("Copying From Tape"));
//
//   nLen += _stprintf(szBuffer+nLen, _T("Current/last operation:           %s\r\n"), Oper.szOperation);
//   nLen += _stprintf(szBuffer+nLen, _T("Result:                           %s\r\n"), Oper.szResult);
//
//   // Username: domain\accotutname
//   nLen += _stprintf(szBuffer+nLen, _T("User:                             %s\r\n"), Oper.szUser);
//
//   // Operation start time
//   nLen += _stprintf(szBuffer+nLen,
//                     _T("Operation start time:             %s\r\n"),
//                     ACS_EMF_Common::GetDateTimeString(Oper.nStartTime));
//
//   // Operation stop time
//   if (UpdateType == tutOperationEnd || UpdateType == tutFileCopyError ||
//      UpdateType == tutTapeOrDeviceInitError || UpdateType == tutOperationCanceled)
//   {
//      // Update the end time
//      if (Oper.nEndTime == 0)
//         Oper.nEndTime = (long)time(NULL);
//
//      nLen += _stprintf(szBuffer+nLen,
//                        _T("Operation end time:               %s\r\n"),
//                        ACS_EMF_Common::GetDateTimeString(Oper.nEndTime));
//   }
//   else
//      nLen += _stprintf(szBuffer+nLen, _T("Operation end time:               \r\n"));
//
//   // File(s) and copy status: "Done", "n%", "Queued", "Failed" or "Cancelled"
//   _TCHAR* lpszFile;
//   for (int nIdx = 0; nIdx < Oper.nNumOfFiles; nIdx++)
//   {
//      if (!(Oper.dwOperation == EMF_CMD_TYPE_COPYFROMTAPE && (nIdx % 2) != 0))
//      {
//         if (lpszFile = _tcsrchr(Oper.pFiles[nIdx].Filename, _T('\\')))
//            lpszFile++;
//         else
//            lpszFile = Oper.pFiles[nIdx].Filename;
//
//         if (UpdateType == tutOperationInit)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Queued\r\n"), lpszFile);
//         else if (UpdateType == tutTapeOrDeviceInitError)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Cancelled\r\n"), lpszFile);
//         else if (UpdateType == tutOperationEnd)
//            nLen += _stprintf(szBuffer+nLen, _T("  %-31s Done\r\n"), lpszFile);
//         else
//         {
//            if (UpdateType == tutStartFileCopy ||
//               UpdateType == tutUpdateFileCopyProgress ||
//               UpdateType == tutStartVerify ||
//               UpdateType == tutUpdateFileVerifyProgress)
//            {
//               if (nIdx < Oper.nCurrentFile)
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Done\r\n"), lpszFile);
//               else if (nIdx == Oper.nCurrentFile)
//               {
//                  if (UpdateType == tutStartVerify)
//                     nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% verified\r\n"), lpszFile, Oper.dwPercentVerified);
//                  else if (UpdateType == tutUpdateFileVerifyProgress)
//                  {
//                     if (Oper.dwPercentVerified > 0)
//                        nLen += _stprintf(szBuffer+nLen, _T("  %-31s %lu%% verified\r\n"), lpszFile, Oper.dwPercentVerified);
//                     else
//                        nLen += _stprintf(szBuffer+nLen, _T("  %-31s Verifying\r\n"), lpszFile);
//                  }
//                  else
//                  {
//                     if (Oper.dwPercentCopied > 0)
//                     {
//                        if (!Oper.bCompression && Oper.dwOperation == EMF_CMD_TYPE_COPYTOTAPE)
//                           nLen += _stprintf(szBuffer+nLen,
//                                             _T("  %-31s %lu%% copied (uncompressed)\r\n"),
//                                             lpszFile,
//                                             Oper.dwPercentCopied);
//                        else
//                           nLen += _stprintf(szBuffer+nLen,
//                                             _T("  %-31s %lu%% copied\r\n"),
//                                             lpszFile,
//                                             Oper.dwPercentCopied);
//                     }
//                     else
//                     {
//                        if (!Oper.bCompression && Oper.dwOperation == EMF_CMD_TYPE_COPYTOTAPE)
//                           nLen += _stprintf(szBuffer+nLen, _T("  %-31s Copying (uncompressed)\r\n"), lpszFile);
//                        else
//                           nLen += _stprintf(szBuffer+nLen, _T("  %-31s Copying\r\n"), lpszFile);
//                     }
//                  }
//               }
//               else
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Queued\r\n"), lpszFile);
//            }
//            else if (UpdateType == tutEndFileCopy)
//            {
//               if (nIdx <= Oper.nCurrentFile)
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Done\r\n"), lpszFile);
//               else
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Queued\r\n"), lpszFile);
//            }
//            else if (UpdateType == tutFileCopyError || UpdateType == tutOperationCanceled)
//            {
//               if (nIdx < Oper.nCurrentFile)
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Done\r\n"), lpszFile);
//               else if (nIdx == Oper.nCurrentFile)
//               {
//                  if (UpdateType == tutOperationCanceled)
//                  {
//                     if (Oper.bVerify && Oper.dwPercentCopied == 100)
//                        nLen += _stprintf(szBuffer+nLen, _T("  %-31s Verification interrupted\r\n"), lpszFile);
//                     else
//                        nLen += _stprintf(szBuffer+nLen, _T("  %-31s Cancelled\r\n"), lpszFile);
//                  }
//                  else
//                     nLen += _stprintf(szBuffer+nLen, _T("  %-31s Failed\r\n"), lpszFile);
//               }
//               else
//                  nLen += _stprintf(szBuffer+nLen, _T("  %-31s Cancelled\r\n"), lpszFile);
//            }
//         }
//      }
//   }
//
//   // Add the printotut buffer
//   param.Data[1] = (const _TCHAR*)szBuffer;
//
//   // Encode the data
//   param.Encode();
//
//   ACS_EMF_DATA* pData = (ACS_EMF_DATA*)::HeapAlloc(::GetProcessHeap(),
//                                                    HEAP_ZERO_MEMORY,
//                                                    sizeof(ACS_EMF_DATA));
//
//   if (!pData)
//      return;
//
//   // Copy buffer to ACS_EMF_DATA data member
//   pData->Code = EMF_CMD_TYPE_UPDATEPROGRESS;
//   (void)param.getEncodedBuffer(pData->Data);
//
//   if (UpdateType == tutOperationCanceled)
//   {
//      // Go ahead and log the operation information directly since the service is
//      // abotut to go offline
//      StoreLogInfoDirectly(pData);
//   }
//   else
//   {
//      // Add the new ACS_EMF_DATA structure to the queue
//      AddLogInfoItem(pData);
//   }
}
*/
