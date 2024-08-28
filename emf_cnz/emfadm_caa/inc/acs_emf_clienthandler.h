/*=================================================================== */
/**
   @file   acs_emf_clienthandler.h

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
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_EMF_CLIENTHANDLER_H
#define ACS_EMF_CLIENTHANDLER_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_emf_defs.h"
//#include "acs_emf_cmd.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief    ACS_EMF_ClientHandler
 */
/*=================================================================== */
class ACS_EMF_ClientHandler
{
	/*=====================================================================
							PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**
		  @brief           ACS_EMF_ClientHandler

		  @param 		   hEndEvent :ACE_Event

		  @param 		   pCriticalSection :ACE_Recursive_Thread_Mutex

		  @exception       none
	 */
	/*=================================================================== */
	ACS_EMF_ClientHandler(ACE_Event* hEndEvent, ACE_Recursive_Thread_Mutex* pCriticalSection);
	/*=====================================================================
   						CLASS DESTRUCTORS
   ==================================================================== */
	/*=================================================================== */
	/**
   	  @brief           ~ACS_EMF_ClientHandler

   	  @exception       none
	 */
	/*=================================================================== */
	~ACS_EMF_ClientHandler();
	/*===================================================================
                             PUBLIC METHOD
      =================================================================== */
	/*=================================================================== */
	/**
   	  @brief           ClientHandler

   	  @exception       none
	 */
	/*=================================================================== */
	void ClientHandler();
	/*=================================================================== */
	/**
     	  @brief           Compose a filename for the log-file

     	  @param 		   Oper: 	ACS_EMF_DVDOperation

     	  @return 		   none

     	  @exception       none
	 */
	/*=================================================================== */
	void GenerateLogFileName(ACS_EMF_DVDOperation& Oper);

	/*=================================================================== */
	/**
     	  @brief           Compose a filename for the log-file

     	  @param 		   Oper: 	ACS_EMF_TapeOperation

     	  @return 		   none

     	  @exception       none
	 */
	/*=================================================================== */
	void GenerateLogFileName(ACS_EMF_TapeOperation& Oper);

	/*=================================================================== */
	/**
     	  @brief           Sending the image file to ACS_EMF_Server on the other node (active or passive) for writing the data to physical disc media.

     	  @param 		   pData: 	ACS_EMF_DATA

          @param 		   hImageFile: 	ACE_HANDLE

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD CopyToDVDRemote(ACS_EMF_DATA* pData, ACE_HANDLE hImageFile);

	/*=================================================================== */
	/**
     	  @brief           InitCopyToDVD

     	  @param 		   pData: 	ACS_EMF_DATA

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD InitCopyToDVD(ACS_EMF_DATA* pData);

	/*=================================================================== */
	/**
     	  @brief           Sending a request to ACS_EMF_Server on the other node (active or passive) to check if the DVD device is assigned to its AP node.

     	  @param 		   pData: 	ACS_EMF_DATA

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD GetDVDOwner(ACS_EMF_DATA* pData);

	/*=================================================================== */
	/**
     	  @brief           Sending a request to ACS_EMF_Server on the other node (active or passive) to check if the DVD device is available to use.

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD GetDVDState();
	/*=================================================================== */
	/**
     	  @brief           Sending a request to ACS_EMF_Server on the other node (active or passive) to remove the DVD device before moving it to this node.

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD RemoveSATADevice();
	/*=================================================================== */
	/**
     	  @brief           Sending a request to ACS_EMF_Server on the other node (active or passive) to check the DVD Board Status.

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD GetDVDBoardStatus();

	/*=================================================================== */
	/**
     	  @brief           Reading operation(s) information either from this node (if active) or send a request to ACS_EMF_Server on the other node through a socket connection.
						    The read/recived information is send back to command emfinfo

     	  @param 		   pData: 	ACS_EMF_DATA

     	  @return 		   DWORD

     	  @exception       none
	 */
	/*=================================================================== */
	DWORD GetLogInfo(ACS_EMF_DATA* pData);

	/*=================================================================== */
	/**
     	  @brief           Create a client socket resources and connect to the server socket on the other node

     	  @return 		   BOOL :true/false

     	  @exception       none
	 */
	/*=================================================================== */
	BOOL ConnectToServerSocket();
	/*=================================================================== */
	/**
       	  @brief           Close the connection

       	  @return 		   BOOL :true/false

       	  @exception       none
	 */
	/*=================================================================== */
	BOOL CloseServerSocket();

	/*=================================================================== */
	/**
       	  @brief           Send Data to Sever

       	  @param		   pData : ACS_EMF_DATA

       	  @return 		   BOOL :true/false

       	  @exception       none
	 */
	/*=================================================================== */
	BOOL SendDataToServerSocket(ACS_EMF_DATA* pData);
	/*=================================================================== */
	/**
       	  @brief           Send a notify to the ACS_EMF_Server on the active node that we are online

       	  @return 		   none

       	  @exception       none
	 */
	/*=================================================================== */
	void SentNotifyToActive();
	/*=================================================================== */
	/**
       	  @brief           Send a ACS_EMF_DATA structure buffer to server socket

       	  @param		   pData :ACS_EMF_DATA

       	  @param 		   bReceiveResponse : const BOOL

       	  @return 		   none

       	  @exception       none
	 */
	/*=================================================================== */
	BOOL SendToServer(ACS_EMF_DATA* pData, const BOOL bReceiveResponse);
private:
	// COMMENTED TO AVOID CPPCHECK ERRORS
#if 0
	void AddLogInfoItem(ACS_EMF_DATA* pData);
	void StoreLogInfoDirectly(ACS_EMF_DATA* pData);
	void StoreLogInfoLocally(ACS_EMF_DATA* pData, BOOL bEndEventSignaled);

	// Supervises server and client logging threads
	void ClientLogging();
	void LocalLogging();


	void SentNotifyToActive();
	BOOL SendToServer(ACS_EMF_DATA* pData, const BOOL bReceiveResponse);

	//   DWORD ReceiveFromServerSocket(LPWSABUF lpBuffer);
	//   DWORD SendToServerSocket(LPWSABUF lpBuffer);

	void Lock();
	void Unlock();

	void LockClient();
	void UnlockClient();
#endif
	/*===================================================================
                      PRIVATE DECLARATION SECTION
   =================================================================== */
private:
	/*===================================================================
                      PRIVATE ATTRIBUTE
   =================================================================== */

	/*=================================================================== */
	/**
	@brief   m_hEndEvent
	 */
	/*=================================================================== */
	ACE_Event*               m_hEndEvent;
	/*=================================================================== */
	/**
	@brief   m_hSaveLogInfoEvent
	 */
	/*=================================================================== */
	ACE_HANDLE               m_hSaveLogInfoEvent;
	/*=================================================================== */
	/**
	@brief   m_hLocalLoggingEvent
	 */
	/*=================================================================== */
	ACE_HANDLE               m_hLocalLoggingEvent;
	/*=================================================================== */
	/**
        @brief   m_szLocalLogfilePath
	 */
	/*=================================================================== */
	ACE_TCHAR               m_szLocalLogfilePath[128];
	/*=================================================================== */
	/**
        @brief   m_szConnectIpAddress1
	 */
	/*=================================================================== */
	ACE_TCHAR               m_szConnectIpAddress1[24];
	/*=================================================================== */
	/**
        @brief   m_szConnectIpAddress2
	 */
	/*=================================================================== */
	ACE_TCHAR               m_szConnectIpAddress2[24];
	/*=================================================================== */
	/**
        @brief   m_nPortNo
	 */
	/*=================================================================== */
	ACE_INT32              m_nPortNo;
	/*=================================================================== */
	/**
        @brief   m_ServerSocket
	 */
	/*=================================================================== */
	ACE_HANDLE               m_ServerSocket;
	//   ACE_HANDLE               m_hServerEvent;
	/*=================================================================== */
	/**
        @brief   m_bEventSent
	 */
	/*=================================================================== */
	BOOL                 m_bEventSent;
	/*=================================================================== */
	/**
        @brief   m_bLogLocally
	 */
	/*=================================================================== */
	BOOL                 m_bLogLocally;
	/*=================================================================== */
	/**
        @brief   m_srctCS
	 */
	/*=================================================================== */
	ACE_Recursive_Thread_Mutex     m_srctCS;
	/*=================================================================== */
	/**
        @brief   m_pCriticalSection
	 */
	/*=================================================================== */
	ACE_Recursive_Thread_Mutex*   m_pCriticalSection;
};

#endif
