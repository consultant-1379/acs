//!@todo Recheck changes

//=============================================================================
/**
 *  @file    ACS_SSU_Common.h
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Utvecklings AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Utvecklings AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been supplied.
 *
 *  @brief Common class having common functionalities is present in this file
 *
 *  @author 2010-07-7 by XSIRKUM
 *
 *  @documentno
 *
 */

#ifndef ACS_SSU_COMMON_H
#define ACS_SSU_COMMON_H

#include <stdio.h>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <mntent.h>
#include <ace/ACE.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ACS_CS_API_NetworkElement_R1.h>
//! Common Class include for Getting Active Node
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_CommonLib.h>
#include <acs_prc_api.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include "acs_ssu_types.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_svc_loader.h"
#include "acs_ssu_service.h"

#define SSU_FILE 1
#define SSU_DIRECTORY 2
#define SSU_OTHERS 3

//! @todo remove this once APGCC implements this
#define SSU_HW_VERSION_APG43 0
//! ACE Trace Macros
#define DEBUG_PREFIX "\n DEBUG%I"
#define INFO_PREFIX "\n INFO%I"
#define NOTICE_PREFIX "\n NOTICE%I"
#define WARNING_PREFIX "\n WARNING%I"
#define ERROR_PREFIX "\n ERROR%I"
#define CRITICAL_PREFIX "\n CRITICAL%I"
#define ALERT_PREFIX "\n ALERT%I"
#define EMERGENCY_PREFIX "\n EMERGENCY%I"

#define ACS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

#ifdef SSUDEBUG
#define DEBUG(FMT, ...) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX FMT ,\
__VA_ARGS__))


#define WARNING(FMT, ...) \
	ACS_SSU_Common::helperTraceWarning(FMT,__VA_ARGS__);
#define TRA_ERROR(FMT, ...) \
	ACS_SSU_Common::helperTraceWarning(FMT,__VA_ARGS__);

#define INFO(FMT, ...) \
ACE_DEBUG(( LM_INFO, \
INFO_PREFIX FMT ,\
__VA_ARGS__))

#define NOTICE(FMT, ...) \
ACE_DEBUG(( LM_NOTICE, \
NOTICE_PREFIX FMT ,\
__VA_ARGS__))

#define ERROR(FMT, ...) \
ACE_DEBUG(( LM_ERROR, \
ERROR_PREFIX FMT ,\
__VA_ARGS__))

#define CRITICAL(FMT, ...) \
ACE_DEBUG(( LM_CRITICAL, \
CRITICAL_PREFIX FMT ,\
__VA_ARGS__))

#define ALERT(FMT, ...) \
ACE_DEBUG(( LM_ALERT, \
ALERT_PREFIX FMT ,\
__VA_ARGS__))

#define EMERGENCY(FMT, ...) \
ACE_DEBUG(( LM_EMERGENCY, \
EMERGENCY_PREFIX FMT ,\
__VA_ARGS__))

#else  // SSUDEBUG

#define INFO(FMT, ...) \
	ACS_SSU_Common::helperTraceInformation(FMT,__VA_ARGS__);

#define DEBUG(FMT, ...) \
	ACS_SSU_Common::helperTraceDebug(FMT,__VA_ARGS__);
#define WARNING(FMT, ...) \
	ACS_SSU_Common::helperTraceWarning(FMT,__VA_ARGS__);

#define ERROR(FMT, ...) \
	ACS_SSU_Common::helperTraceError(FMT,__VA_ARGS__);

#define FATAL(FMT, ...) \
	ACS_SSU_Common::helperTraceFatal(FMT,__VA_ARGS__);

#define NOTICE(FMT, ...) \
	ACS_SSU_Common::helperTraceDebug(FMT,__VA_ARGS__);

#endif

typedef enum _enumNodeState
{
   nsActive = 0,
   nsPassive,
   nsUnknown
} enumNodeState;

/*!
 * @class ACS_SSU_Common
 *
 * @brief It contains all the common functions
 *
 */


class ACS_SSU_Common
{
public:
	static ACS_TRA_trace ACS_SSU_InformationTrace;
	static ACS_TRA_trace ACS_SSU_DebugTrace;
	static ACS_TRA_trace ACS_SSU_WarningTrace;
	static ACS_TRA_trace ACS_SSU_ErrorTrace;
	static ACS_TRA_trace ACS_SSU_FatalTrace;
	static ACS_TRA_Logging ACS_SSU_Logging;
	static bool theIsActiveNode;
	static ACE_Mutex log_mutex ;

	static std::map<std::string,std::string> ssuStringInitialParamsMap;
	static std::map<std::string,int> ssuIntegerInitialParamsMap;
protected:
	/**
	* @brief
	* Protected default constructor
	* Not allowed to instantiate an object of this class
	* @return void
	*
	*/
   ACS_SSU_Common() {  };

public:
   /**
    * @brief
    * static function used to pass the TRA information trace.
    * No arguments
    * @return void
    *
    */

   static void helperTraceInformation(const ACE_TCHAR* messageFormat, ...)
   {
     //if( ACS_SSU_InformationTrace==0 )
     //return;
     const ACE_UINT32 TRACE_BUF_SIZE = 1024;
     ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
     ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
     if ( messageFormat && *messageFormat )
        {
	log_mutex.acquire();
     	   va_list params;
      	   va_start(params, messageFormat);
      	   ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
      	   traceBuffer[TRACE_BUF_SIZE-1]='\0';
      	   //! If Trace if ON .Send trace to TRA
       	   if( ACS_SSU_InformationTrace.ACS_TRA_ON() )
      	   {
              ACS_SSU_InformationTrace.ACS_TRA_event(1,traceBuffer);
      	   }

       	   ACS_SSU_Logging.Write(traceBuffer,LOG_LEVEL_INFO);
       	   va_end(params);
	log_mutex.release();
           }
   }

   /**
       * @brief
       * static function used to pass the TRA debug trace.
       * No arguments
       * @return void
       *
       */
   static void helperTraceDebug(const ACE_TCHAR* messageFormat, ...)
      {
       //if( ACS_SSU_DebugTrace==0 )
         //	   return;
       const ACE_UINT32 TRACE_BUF_SIZE = 1024;
       ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
       ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
       if ( messageFormat && *messageFormat )
          {
		log_mutex.acquire();
         	   va_list params;
          	   va_start(params, messageFormat);
          	   ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
          	   traceBuffer[TRACE_BUF_SIZE-1]='\0';
           	   //! If Trace if ON .Send trace to TRA
          	   if( ACS_SSU_DebugTrace.ACS_TRA_ON() )
          	   {
                    ACS_SSU_DebugTrace.ACS_TRA_event(1,traceBuffer);
          	   }

          	   ACS_SSU_Logging.Write(traceBuffer,LOG_LEVEL_DEBUG);
           	   va_end(params);
		log_mutex.release();
           }
      }

   /*===================================================================
     	   ROUTINE: helperTraceWarning
  	=================================================================== */
   /**
       * @brief
       * static function used to pass the TRA warning trace.
       * No arguments
       * @return void
       *
       */
     	static void helperTraceWarning(const ACE_TCHAR* messageFormat, ...)
     	{

          static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
          ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
          ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
          if ( messageFormat && *messageFormat )
             {

		log_mutex.acquire();
        	  va_list params;
        	  va_start(params, messageFormat);

        	  ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
        	  //! If Trace if ON .Send trace to TRA

        	  if( ACS_SSU_WarningTrace.ACS_TRA_ON() )
        	  {
        		  ACS_SSU_WarningTrace.ACS_TRA_event(1,traceBuffer);
        	  }
        	  ACS_SSU_Logging.Write(traceBuffer,LOG_LEVEL_WARN);
        	  va_end(params);
		log_mutex.release();
             }
     	}

    /**
    * @brief
    * static function used to pass the TRA Error trace.
    * No arguments
    * @return void
    *
    */
     	static void helperTraceError(const ACE_TCHAR* messageFormat, ...)
     	{
           static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
           ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
           ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
           if ( messageFormat && *messageFormat )
            {
		log_mutex.acquire();
            	va_list params;
            	va_start(params, messageFormat);

            	ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

            	//! If Trace if ON .Send trace to TRA

            	if( ACS_SSU_ErrorTrace.ACS_TRA_ON() )
            	{
            		ACS_SSU_ErrorTrace.ACS_TRA_event(1,traceBuffer);
            	}
            	 ACS_SSU_Logging.Write(traceBuffer,LOG_LEVEL_ERROR);

            	va_end(params);
		log_mutex.release();
            }
     	}
    /**
    * @brief
    * static function used to pass the TRA FATAL trace.
    * No arguments
    * @return void
    *
    */
     	static void helperTraceFatal(const ACE_TCHAR* messageFormat, ...)
     	   {
     		const ACE_UINT32 TRACE_BUF_SIZE = 1024;
     		ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
     		ACE_OS::memset( traceBuffer, 0, sizeof( TRACE_BUF_SIZE ));
     		if ( messageFormat && *messageFormat )
     		{
		log_mutex.acquire();
     			va_list params;
     			va_start(params, messageFormat);
     			ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
     			traceBuffer[TRACE_BUF_SIZE-1]='\0';

     			//! If Trace if ON .Send trace to TRA
     			if( ACS_SSU_FatalTrace.ACS_TRA_ON() )
     			{
     				ACS_SSU_FatalTrace.ACS_TRA_event(1,traceBuffer);
     			}

     			ACS_SSU_Logging.Write(traceBuffer,LOG_LEVEL_FATAL);
     			va_end(params);
		log_mutex.release();
     		}
     	   }


#if 0
   static _TCHAR* GetActiveNodeName()
   {
      static _TCHAR szCurrentNodeName[64];

      // Use the local machine as default node name (can be handy if we are debugging
      // on a local workstation without  a Cluster)
      DWORD dwLen = 63;
      (void)::GetComputerName(szCurrentNodeName, &dwLen);

      HCLUSTER hCluster = ::OpenCluster(NULL);
      if (!hCluster)
      {
         // No cluster available. We assume this system does not have a cluster or the
         // cluster server is not started
         return szCurrentNodeName;
      }

      HGROUP hGroup = ::OpenClusterGroup(hCluster, L"Cluster Group");
      if (!hGroup)
      {
         // Cluster group "Cluster Group" not found
         (void)::CloseCluster(hCluster);
         *szCurrentNodeName = _T('\0');
         return szCurrentNodeName;
      }

      dwLen = 63;
      WCHAR szActiveNodeName[64];
      CLUSTER_GROUP_STATE enumState = ::GetClusterGroupState(hGroup, szActiveNodeName, &dwLen);
      if (enumState == ClusterGroupStateUnknown)
      {
         // If the cluster state is unknown we have a cluster but we dont know if this is
         // the active or passive node, so we consider this is the passive node
         *szCurrentNodeName = _T('\0');
      }
      else
      {
         (void)::WideCharToMultiByte(CP_ACP,
                                     0,
                                     szActiveNodeName,
                                     (int)dwLen,
                                     szCurrentNodeName,
                                     63,
                                     NULL,
                                     NULL);
      }

      (void)::CloseClusterGroup(hGroup);
      (void)::CloseCluster(hCluster);

      return szCurrentNodeName;
   };

#endif
   /*!
    * @brief GetDataDiskPath This function returns the datapath
    * for the corresponding logical name
    * @param pszLogicalName symbolic name of the disk
    * @param pszPath Path of the Data Disk
    */
   static bool GetDataDiskPath( std::string& pszLogicalName, std::string&  pszPath)
   {
	   ACS_APGCC_CommonLib myAPGCCCommonLib;
	   //char p_LogicalName[1024] ;
	   //char p_Path [1024];
	   //int p_Len =1024;
	   ACS_APGCC_DNFPath_ReturnTypeT  errorCode;
	   char* pszPath1 = new char[1000];
	   int myLen = 100;
//	   INFO("The logical name is = ",pszLogicalName);
	   errorCode = myAPGCCCommonLib.GetDataDiskPath(pszLogicalName.c_str(),pszPath1,myLen) ;
	   bool myResult = false;
	   if ( errorCode == ACS_APGCC_STRING_BUFFER_SMALL )
	   {
		   ERROR("%s","The return code is : ACS_APGCC_STRING_BUFFER_SMALL ");
		   delete []pszPath1;
	   }
	   else if ( errorCode == ACS_APGCC_FAULT_LOGICAL_NAME )
	   {
		   ERROR("%s","The return code is : ACS_APGCC_FAULT_LOGICAL_NAME ");
		   delete []pszPath1;
	   }
	   else if ( errorCode == ACS_APGCC_DNFPATH_SUCCESS )
	   {
		   std::string myStr1(pszPath1);
		   pszPath = myStr1;
		   delete []pszPath1;
		   myResult = true;
	   }
	   else if ( errorCode == ACS_APGCC_DNFPATH_FAILURE )
	   {
		   ERROR("%s","The return code is  ACS_APGCC_DNFPATH_FAILURE ");
		   delete []pszPath1;
	   }
	   return myResult;

   };

   static bool ExpandEnvironmentStrings( const ACE_TCHAR *pszsrc, ACE_TCHAR * m_szdest )
   {
	   bool bResult = false;

       ACE_TCHAR *pszEnvVar = new ACE_TCHAR[FILE_MAX_PATH];

       //! Strip the beginning and terminating % char and extract Environment var name to pszEnvVar
	   if( bStripChar(pszsrc,pszEnvVar,'%') )
	   {
		   ACE_TCHAR * pzEnv;
		   pzEnv = ACE_OS::getenv(pszEnvVar);

          if( pzEnv )
          {
		     StringTrim(pzEnv);

		     ACE_OS::strcpy( pszEnvVar,pzEnv);

		     //! Check whether pzEnv contains only spaces
		     if( ACE_OS::strcmp(pszEnvVar,"") != 0 )

		     {
			     bResult = true;
		     }
          }

          if( bResult == false )
          {
        	 // ACE_DEBUG((LM_DEBUG,"\nDEBUG pszEnvVar is empty "));
        	  ACE_OS::strcpy(pszEnvVar,"/opt/AP");
          }
	   }

	   //! Copy Remaining string from source string
	   const ACE_TCHAR *pchInstance = ACE_OS::strrchr(pszsrc,'%');

	   ACE_OS::strcpy(&pszEnvVar[ACE_OS::strlen(pszEnvVar)], pchInstance+1);

	   //DEBUG(" pszEnvVar %s",pszEnvVar);

	   ACE_OS::strcpy(m_szdest,pszEnvVar);

	   //DEBUG(" m_szdest %s",pszEnvVar);

	   if( m_szdest )
		   bResult = true;

	   return bResult;
   };

   static void StringTrim( ACE_TCHAR * pszData )
   {
      ACE_TCHAR * temp = pszData;

      //DEBUG( " Before %s : %d",pszData,ACE_OS::strlen(pszData));

      while( *pszData == 0x20 )
    	  pszData++;

      if( *pszData == '\0' )
    	  return;

      temp = pszData+ACE_OS::strlen( pszData )-1;

      while( temp > pszData && *temp==0x20 )
    	  temp--;

      *(temp+1) = '\0';

      //DEBUG( " After %s : %d",pszData,ACE_OS::strlen(pszData));
   }


   static int GetHWVersion()
   {
	  ACE_INT32 nVersion;
	  char myhwVersion[200];
	  int myLen=10;
	  ACS_APGCC_HwVer_ReturnTypeT myReturnCode;
	  ACS_APGCC_CommonLib myAPGCCCommonLib;
	  myReturnCode = myAPGCCCommonLib.GetHWVersion (myhwVersion, myLen );
//	  cout << "hwver = " << myhwVersion << endl;
//	  if ( myReturnCode != ACS_APGCC_HWVER_APG43 )
//	  {
//		  nVersion = -1;
//	  }
//	  else
//	  {
//		  nVersion = SSU_HW_VERSION_APG43;
//	  }
	  if (ACE_OS::strcmp(myhwVersion,"APG43") == 0)
	  {
		  nVersion = SSU_HW_VERSION_APG43;
	  }
	  else if(ACE_OS::strcmp(myhwVersion,"APG43/2") == 0)
	  {
		  nVersion = SSU_HW_VERSION_APG43;
	  }
	  else if(ACE_OS::strcmp(myhwVersion,"APG43/3") == 0)
          {
                  nVersion = SSU_HW_VERSION_APG43;
          }

	  else
	  {
		  nVersion = -1;
	  }
      return nVersion;
   };

   static BOOL CheckIfActiveNode()
   {
	   ACS_PRC_API prcObj;
	   BOOL isActive=false;

	   	if((prcObj.askForNodeState())==1)
	   	{
	   		isActive=true;
	   	}
	   return(isActive);

   };

   /*!
    * @brief Checks if a severity is a valid SSU severity class
    * @param lpszSeverity Severity clas need to be checked
    * @return TRUE Valid
    * @return false Invalid
    */
   static BOOL bCheckValidSeverityClass(const ACE_TCHAR* lpszSeverity)
   {
      const ACE_INT32 nNumOfSeverityClasses = 6;

      const ACE_TCHAR* vSeverityClasses[] = {
         ACE_TEXT("Event"),
         ACE_TEXT("O2"),
         ACE_TEXT("O1"),
         ACE_TEXT("A3"),
         ACE_TEXT("A2"),
         ACE_TEXT("A1")
      };

      BOOL bIsValid = false;

      for (ACE_INT32 nIdx = 0; nIdx < nNumOfSeverityClasses; nIdx++)
      {
         if (ACE_OS::strcmp(lpszSeverity, vSeverityClasses[nIdx]) == 0)
            bIsValid = true;
       }


      return bIsValid;
   };

#if 0
   // Gets the state of the current node (active, passive or unknown)
   static  enumNodeState GetNodeState()
   {
      HCLUSTER hCluster = ::OpenCluster(NULL);
      if (!hCluster)
         return nsUnknown;

      HGROUP hGroup = ::OpenClusterGroup(hCluster, L"Cluster Group");
      if (!hGroup)
      {
         (void)::CloseCluster(hCluster);
         return nsUnknown;
      }

      // Use the local machine as default node name (can be handy if we are debugging
      // on a local workstation without a Cluster)
      DWORD dwLen = 64;
      WCHAR szCurrentNodeName[64+1];
      (void)::GetComputerNameW(szCurrentNodeName, &dwLen);

      // Get the name of the node which owns the active cluster group
      dwLen = 64;
      WCHAR szActiveNodeName[64+1];
      CLUSTER_GROUP_STATE enumState = GetClusterGroupState(hGroup, szActiveNodeName, &dwLen);

      (void)::CloseClusterGroup(hGroup);
      (void)::CloseCluster(hCluster);

      if (enumState == ClusterGroupStateUnknown)
         return nsUnknown;

      return (wcscmp(szCurrentNodeName, szActiveNodeName) == 0 ? nsActive : nsPassive);
   }

   static  _TCHAR* GetWin32Msg(DWORD dwCode = (DWORD)-1)
   {
      static _TCHAR szMsg[128];
      DWORD dwMessageId = (dwCode == (DWORD)-1 ? ::GetLastError() : dwCode);

      DWORD dwLen = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                    NULL,
                                    dwMessageId,
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    szMsg,
                                    127,
                                    NULL);

      if (dwLen > 1)
         szMsg[dwLen-2] = _T('\0');
      else
         _sntprintf(szMsg,sizeof(szMsg)/sizeof(*szMsg) - 1,
		           _T("Error code: %d"), dwMessageId);

      return szMsg;
   };


   static BOOL RunChildProcess(const _TCHAR* lpszCommand,
                               _TCHAR* lpszOutput,
                               const DWORD dwOutputBufferLen)
   {
      SECURITY_ATTRIBUTES srctSA;
      srctSA.nLength = sizeof(SECURITY_ATTRIBUTES);
      srctSA.bInheritHandle = TRUE;
      srctSA.lpSecurityDescriptor = NULL;

      HANDLE hReadPipe, hWritePipe;

      // Create a named pipe to redirect standard output to so we can parse the printout
      if (!::CreatePipe(&hReadPipe, &hWritePipe, &srctSA, 0))
         return false;

      // Set up members of the STARTUPINFO structure.
      STARTUPINFO srctStartInfo;
      ::RtlZeroMemory(&srctStartInfo, sizeof(STARTUPINFO));
      srctStartInfo.cb = sizeof(STARTUPINFO);
      srctStartInfo.dwFlags = STARTF_USESTDHANDLES;
      srctStartInfo.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
      srctStartInfo.hStdOutput = hWritePipe;
      srctStartInfo.hStdError = hWritePipe;

      // Set up members of the PROCESS_INFORMATION structure.
      PROCESS_INFORMATION srctProcInfo;
      ::RtlZeroMemory(&srctProcInfo, sizeof(PROCESS_INFORMATION));

      // Create the child process.  
      BOOL bSuccess = ::CreateProcess(NULL, 
                                      (LPTSTR)lpszCommand, // command line
                                      NULL,                // process security attributes
                                      NULL,                // primary thread security attributes
                                      TRUE,                // handles are inherited
                                      0,                   // creation flags
                                      NULL,                // use parent's environment
                                      NULL,                // use parent's current directory
                                      &srctStartInfo,      // STARTUPINFO pointer
                                      &srctProcInfo);      // receives PROCESS_INFORMATION

      if (bSuccess)
      {
         // Wait until the process has finished execute
         (void)::WaitForInputIdle(srctProcInfo.hProcess, 5000);
         (void)::WaitForSingleObject(srctProcInfo.hProcess, INFINITE);
         (void)::CloseHandle(srctProcInfo.hThread);
         (void)::CloseHandle(srctProcInfo.hProcess);

         if (lpszOutput)
         {
            *lpszOutput = _T('\0');

            DWORD dwBytesRead;
            bSuccess = ::ReadFile(hReadPipe,
                                  lpszOutput,
                                  (dwOutputBufferLen*sizeof(_TCHAR)),
                                  &dwBytesRead,
                                  NULL);

            if (bSuccess)
               lpszOutput[dwBytesRead/sizeof(_TCHAR)] = _T('\0');
         }
      }

      (void)::CloseHandle(hReadPipe);
      (void)::CloseHandle(hWritePipe);

      return bSuccess;
   };
#endif
   //============================================================================
   // Check a file or directory for existence and permissions
   // Also checks if it is a file (1) or directory (2)
   //============================================================================
   static BOOL getIsMultCPSysValue()
   {
	   bool multipleCPSystem ;
	   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement_R1::isMultipleCPSystem(multipleCPSystem);
	   if(result != ACS_CS_API_NS::Result_Success)
	   {
		   DEBUG("%s","Failure in getting the CP type");
	   }
	   DEBUG("getIsMultCPSysValue, (0-single ,1-multi) :%d",multipleCPSystem);
	   return multipleCPSystem;
   }

   static ACE_INT32 CheckFile(const ACE_TCHAR* lpszFile)
   {

	   ACE_stat fileStat;
	   ACE_INT32 s32Type = 0 ;

	   if ( ACE_OS::stat(lpszFile,&fileStat) != 0 )
	   {
	       s32Type = -1;
	   }

      if( s32Type !=-1 )
      {
         if ( S_ISDIR(fileStat.st_mode) )
            s32Type = SSU_DIRECTORY;
         else if( S_ISREG(fileStat.st_mode) )
            s32Type = SSU_FILE;
         else
        	s32Type = SSU_OTHERS;
      }

      return s32Type;
   };


   static BOOL CheckFileExists(const ACE_TCHAR* lpszFileName)
   {
      bool bFileExists = false;

	  if( CheckFile( lpszFileName ) != -1 )
		  bFileExists = true;

      return bFileExists;
   };

   static ACE_INT32 s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
   		                         ACE_TCHAR* lpszThreadName,ACE_thread_t *threadId)
   {
   	ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
   													Arglist,
   													THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
   													threadId,
   													0,
   													ACE_DEFAULT_THREAD_PRIORITY,
   													-1,
   													0,
   													ACE_DEFAULT_THREAD_STACKSIZE,
   													&lpszThreadName);


   	return s32Result;
   }

   static ACE_INT32 StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
                              void* Arglist,
                              const ACE_TCHAR* lpszThreadName,
                              ACE_thread_t *threadId)
   {

   	ACE_INT32 s32Result =  s32Spawn( ThreadFunc,
   		                             Arglist,
   		                             lpszThreadName,
   		                              threadId);

      if ( s32Result == -1 )
      {
         // Report error
         ACE_TCHAR szData[256];

         snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                   ACE_TEXT("Unable to create thread: %s due to the following error:\n %m"),
                   lpszThreadName);

         ACE_DEBUG((LM_INFO, ACE_TEXT(szData)));

         //! Send event
         (void)ACS_SSU_AEH::ReportEvent(1002,
                                        ACE_TEXT("EVENT"),
                                        CAUSE_AP_INTERNAL_FAULT,
                                        ACE_TEXT(""),
                                        szData,
                                        ACE_TEXT("CREATE THREAD FAILED IN SSU SYSTEM SUPERVISOR"));
      }

      return s32Result;

   }

   static ACE_INT32 s32SpawnForPerf(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
      		                         ACE_TCHAR* lpszThreadName,ACE_INT32 grpId)
      {
      	ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
      													Arglist,
      													THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
      													0,
      													0,
      													ACE_DEFAULT_THREAD_PRIORITY,
      													grpId,
      													0,
      													ACE_DEFAULT_THREAD_STACKSIZE,
      													&lpszThreadName);


      	return s32Result;
      }

      static ACE_INT32 StartThreadForPerf( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
                                 void* Arglist,
                                 const ACE_TCHAR* lpszThreadName,
                                 ACE_INT32 grpId)
      {

      	ACE_INT32 s32Result =  s32SpawnForPerf( ThreadFunc,
      		                             Arglist,
      		                             lpszThreadName,
      		                           grpId);

         if ( s32Result == -1 )
         {
            // Report error
            ACE_TCHAR szData[256];

            snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
                      ACE_TEXT("Unable to create thread: %s due to the following error:\n %m"),
                      lpszThreadName);

            ACE_DEBUG((LM_INFO, ACE_TEXT(szData)));

            //! Send event
            (void)ACS_SSU_AEH::ReportEvent(1002,
                                           ACE_TEXT("EVENT"),
                                           CAUSE_AP_INTERNAL_FAULT,
                                           ACE_TEXT(""),
                                           szData,
                                           ACE_TEXT("CREATE THREAD FAILED IN SSU SYSTEM SUPERVISOR"));
         }

         return s32Result;

      }
   static bool retrievePHAParameter(const std::string& aParameterName,ACE_UINT32& aRetrievedValue)
   {
	   std::map<std::string,int>::iterator it;
	   it = ACS_SSU_Common::ssuIntegerInitialParamsMap.find(aParameterName);
	   if (it != ACS_SSU_Common::ssuIntegerInitialParamsMap.end())
	   {
		   aRetrievedValue = (*it).second;
		   DEBUG("%s value received from MAP is %d",aParameterName.c_str(),aRetrievedValue);
		   return true;
	   }
	   else
	   {
		   DEBUG("Unable to fetch %s value from MAP",aParameterName.c_str());
		   return false;
	   }

   }


   static bool retrievePHAParameter(const std::string& aParameterName,ACE_TCHAR *aRetrievedValue)
   {
	   std::map<std::string,std::string>::iterator it;
	   it = ACS_SSU_Common::ssuStringInitialParamsMap.find(aParameterName);
	   if (it != ACS_SSU_Common::ssuStringInitialParamsMap.end())
	   {
		   ACE_OS::strcpy(aRetrievedValue,((*it).second).c_str());
		   DEBUG("%s value received from MAP is %s",aParameterName.c_str(),aRetrievedValue);
		   return true;
	   }
	   else
	   {
		   DEBUG("Unable to fetch %s value from MAP",aParameterName.c_str());
		   return false;
	   }
   }

   static bool retrievePHAParameter(const std::string& aParameterName,bool& aRetrievedValue)
   {
	   std::map<std::string,int>::iterator it;
	   it = ACS_SSU_Common::ssuIntegerInitialParamsMap.find(aParameterName);
	   if (it != ACS_SSU_Common::ssuIntegerInitialParamsMap.end())
	   {
		   aRetrievedValue = (*it).second;
		   DEBUG("%s value received from MAP is %d",aParameterName.c_str(),aRetrievedValue);
		   return true;
	   }
	   else
	   {
		   DEBUG("Unable to fetch %s value from MAP",aParameterName.c_str());
		   return false;
	   }

   }

   static bool GetAllPhaParameters(OmHandler *om)
   {
	   DEBUG("%s", "Entered ACS_SSU_Common::GetAllPhaParameters()");
	   (void) om;
#if 0
	   // Fetch initial values from IMM
	   ACS_APGCC_ImmAttribute attrDiskMonObject;
	   ACS_APGCC_ImmAttribute attrDiskMonCounter;
	   ACS_APGCC_ImmAttribute attrDiskMonInstance;
	   ACS_APGCC_ImmAttribute attrA1SpecificPartitionName;
	   ACS_APGCC_ImmAttribute attrA2AlarmLevelLargePartition;

	   ACS_APGCC_ImmAttribute attrA1CeaseLevelSmallPartition;
	   ACS_APGCC_ImmAttribute attrA1AlarmLevelSmallPartition;
	   ACS_APGCC_ImmAttribute attrA1CeaseLevelLargePartition;
	   ACS_APGCC_ImmAttribute attrA1AlarmLevelLargePartition;
	   ACS_APGCC_ImmAttribute attrDiskMonEventNumber;

	   ACS_APGCC_ImmAttribute attrDiskMonPollingInterval;
	   ACS_APGCC_ImmAttribute attrDiskMonAlertIfOver;
	   ACS_APGCC_ImmAttribute attrA1SpecificAlarmLevel;
	   ACS_APGCC_ImmAttribute attrA1SpecificCeaseLevel;
	   ACS_APGCC_ImmAttribute attrMaxSizeOfSmallPartition;

	   ACS_APGCC_ImmAttribute attrA2CeaseLevelSmallPartition;
	   ACS_APGCC_ImmAttribute attrA2AlarmLevelSmallPartition;
	   ACS_APGCC_ImmAttribute attrA2CeaseLevelLargePartition;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryA2AlarmLimit;
	   ACS_APGCC_ImmAttribute attrPerfMonAlertIfOver;

	   ACS_APGCC_ImmAttribute attrPerfMonMemoryInstance;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryCounter;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryObject;
	   ACS_APGCC_ImmAttribute attrA2SpecificCeaseLevel;
	   ACS_APGCC_ImmAttribute attrA2SpecificPartitionName;

	   ACS_APGCC_ImmAttribute attrA2SpecificAlarmLevel;
	   ACS_APGCC_ImmAttribute attrPerfMonMemA2AehEventNumber;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryA1CeaseLimit;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryA1AlarmLimit;
	   ACS_APGCC_ImmAttribute attrPerfMonMemA1AehEventNumber;

	   ACS_APGCC_ImmAttribute attrPerfMonMemPollingInterval;
	   ACS_APGCC_ImmAttribute attrPerfMonMemoryA2CeaseLimit;
	   ACS_APGCC_ImmAttribute attrA1Severity;
	   ACS_APGCC_ImmAttribute attrPerfMonFileHandleCeaseLimit;

	   ACS_APGCC_ImmAttribute attrPerfMonFileHandleAlarmLimit;
	   ACS_APGCC_ImmAttribute attrPerfMonFHandleAehEventNum;
	   ACS_APGCC_ImmAttribute attrPerfMonFHandlePollInterval;
	   ACS_APGCC_ImmAttribute attrPerfMonFileHandleInstance;
	   ACS_APGCC_ImmAttribute attrPerfMonFileHandleCounter;

	   ACS_APGCC_ImmAttribute attrPerfMonFileHandleObject;
	   ACS_APGCC_ImmAttribute attrApplicationToRun;
	   ACS_APGCC_ImmAttribute attrNode;
	   ACS_APGCC_ImmAttribute attrA2Severity;

	   ACS_APGCC_ImmAttribute attrExecuteAppFirstTime;

	   attrDiskMonObject.attrName = SSU_DISK_MON_OBJECT;
	   attrDiskMonCounter.attrName = SSU_DISK_MON_COUNTER;
	   attrDiskMonInstance.attrName = SSU_DISK_MON_INSTANCE;
	   attrA1SpecificPartitionName.attrName = SSU_A1_SPECIFIC_PARTITION_NAME;
	   attrA2AlarmLevelLargePartition.attrName = SSU_A2_ALARM_LEVEL_LARGE_PARTITION;
	   attrA1CeaseLevelSmallPartition.attrName = SSU_A1_CEASE_LEVEL_SMALL_PARTITION;
	   attrA1AlarmLevelSmallPartition.attrName = SSU_A1_ALARM_LEVEL_SMALL_PARTITION;
	   attrA1CeaseLevelLargePartition.attrName = SSU_A1_CEASE_LEVEL_LARGE_PARTTION;
	   attrA1AlarmLevelLargePartition.attrName = SSU_A1_ALARM_LEVEL_LARGE_PARTITION;
	   attrDiskMonEventNumber.attrName = SSU_DISK_MON_EVENT_NUM;
	   attrDiskMonPollingInterval.attrName = SSU_DISK_MON_POLLING_INTERVAL;
	   attrDiskMonAlertIfOver.attrName = SSU_DISK_MON_ALERT_IF_OVER;
	   attrA1SpecificAlarmLevel.attrName = SSU_A1_SPECIFIC_ALARM_LEVEL;
	   attrA1SpecificCeaseLevel.attrName = SSU_A1_SPECIFIC_CEASE_LEVEL;
	   attrMaxSizeOfSmallPartition.attrName = SSU_MAX_SIZE_OF_SMALL_PARTITION;
	   attrA2CeaseLevelSmallPartition.attrName = SSU_A2_CEASE_LEVEL_SMALL_PARTTION;
	   attrA2AlarmLevelSmallPartition.attrName = SSU_A2_ALARM_LEVEL_SMALL_PARTTION;
	   attrA2CeaseLevelLargePartition.attrName = SSU_A2_CEASE_LEVEL_LARGE_PARTION;
	   attrPerfMonMemoryA2AlarmLimit.attrName = SSU_PERF_MON_MEM_A2_ALARM_LIMIT;
	   attrPerfMonAlertIfOver.attrName = SSU_PERF_MON_ALERT_IF_OVER;
	   attrPerfMonMemoryInstance.attrName = SSU_PERF_MON_MEM_INSTANCE;
	   attrPerfMonMemoryCounter.attrName = SSU_PERF_MON_MEM_COUNTER;
	   attrPerfMonMemoryObject.attrName = SSU_PERF_MON_MEM_OBJECT;
	   attrA2SpecificCeaseLevel.attrName = SSU_A2_SPECIFIC_CEASE_LEVEL;
	   attrA2SpecificPartitionName.attrName = SSU_A2_SPECIFIC_PARTITION_NAME;
	   attrA2SpecificAlarmLevel.attrName = SSU_A2_SPECIFIC_ALARM_LEVEL;
	   attrPerfMonMemA2AehEventNumber.attrName = SSU_PERF_MON_MEM_A2_AEH_EVENT_NUM;
	   attrPerfMonMemoryA1CeaseLimit.attrName = SSU_PERF_MON_MEM_A1_CEASE_LIMIT;
	   attrPerfMonMemoryA1AlarmLimit.attrName = SSU_PERF_MON_MEM_A1_ALARM_LIMIT;
	   attrPerfMonMemA1AehEventNumber.attrName = SSU_PERF_MON_MEM_A1_AEH_EVENT_NUM;
	   attrPerfMonMemPollingInterval.attrName = SSU_PERF_MON_MEM_POLLING_INTERVAL;
	   attrPerfMonMemoryA2CeaseLimit.attrName = SSU_PERF_MON_MEMORY_A2_CEASE_LIMIT;
	   attrA1Severity.attrName = SSU_A1_SEVERITY;
	   attrPerfMonFileHandleCeaseLimit.attrName = SSU_PERF_MON_FILE_HANDLE_CEASE_LIMIT;
	   attrPerfMonFileHandleAlarmLimit.attrName = SSU_PERF_MON_FILE_HANDLE_ALARM_LIMIT;
	   attrPerfMonFHandleAehEventNum.attrName = SSU_PERF_MON_FILE_HANDLE_AEH_EVENT_NUM;
	   attrPerfMonFHandlePollInterval.attrName = SSU_PERF_MON_HANDLE_POLL_INTERVAL;
	   attrPerfMonFileHandleInstance.attrName = SSU_PERF_MON_HANDLE_INSTANCE;
	   attrPerfMonFileHandleCounter.attrName = SSU_PERF_MON_FILE_HANDLE_COUNTER;
	   attrPerfMonFileHandleObject.attrName = SSU_PERF_MON_FILE_HANDLE_OBJECT;
	   attrApplicationToRun.attrName = SSU_PERF_MON_APPLICATION_TO_RUN;
	   attrNode.attrName = SSU_NODE;
	   attrA2Severity.attrName = SSU_A2_SEVERITY;
	   attrExecuteAppFirstTime.attrName = SSU_EXECUTE_APP_FIRST_TIME;


	   std::vector<ACS_APGCC_ImmAttribute *> attributes;

	   attributes.push_back(&attrDiskMonObject);
	   attributes.push_back(&attrDiskMonCounter);
	   attributes.push_back(&attrDiskMonInstance);
	   attributes.push_back(&attrA1SpecificPartitionName);
	   attributes.push_back(&attrA2AlarmLevelLargePartition);
	   attributes.push_back(&attrA1CeaseLevelSmallPartition);
	   attributes.push_back(&attrA1AlarmLevelSmallPartition);
	   attributes.push_back(&attrA1CeaseLevelLargePartition);
	   attributes.push_back(&attrA1AlarmLevelLargePartition);
	   attributes.push_back(&attrDiskMonEventNumber);
	   attributes.push_back(&attrDiskMonPollingInterval);
	   attributes.push_back(&attrDiskMonAlertIfOver);
	   attributes.push_back(&attrA1SpecificAlarmLevel);
	   attributes.push_back(&attrA1SpecificCeaseLevel);
	   attributes.push_back(&attrMaxSizeOfSmallPartition);
	   attributes.push_back(&attrA2CeaseLevelSmallPartition);
	   attributes.push_back(&attrA2AlarmLevelSmallPartition);
	   attributes.push_back(&attrA2CeaseLevelLargePartition);
	   attributes.push_back(&attrPerfMonMemoryA2AlarmLimit);
	   attributes.push_back(&attrPerfMonAlertIfOver);
	   attributes.push_back(&attrPerfMonMemoryInstance);
	   attributes.push_back(&attrPerfMonMemoryCounter);
	   attributes.push_back(&attrPerfMonMemoryObject);
	   attributes.push_back(&attrA2SpecificCeaseLevel);
	   attributes.push_back(&attrA2SpecificPartitionName);
	   attributes.push_back(&attrA2SpecificAlarmLevel);

	   attributes.push_back(&attrPerfMonMemA2AehEventNumber);
	   attributes.push_back(&attrPerfMonMemoryA1CeaseLimit);
	   attributes.push_back(&attrPerfMonMemoryA1AlarmLimit);
	   attributes.push_back(&attrPerfMonMemA1AehEventNumber);
	   attributes.push_back(&attrPerfMonMemPollingInterval);


	   attributes.push_back(&attrPerfMonMemoryA2CeaseLimit);
	   attributes.push_back(&attrA1Severity);
	   attributes.push_back(&attrPerfMonFileHandleCeaseLimit);
	   attributes.push_back(&attrPerfMonFileHandleAlarmLimit);

	   attributes.push_back(&attrPerfMonFHandleAehEventNum);
	   attributes.push_back(&attrPerfMonFHandlePollInterval);
	   attributes.push_back(&attrPerfMonFileHandleInstance);
	   attributes.push_back(&attrPerfMonFileHandleCounter);
	   attributes.push_back(&attrPerfMonFileHandleObject);
	   attributes.push_back(&attrApplicationToRun);
	   attributes.push_back(&attrNode);
	   attributes.push_back(&attrA2Severity);
	   attributes.push_back(&attrExecuteAppFirstTime);

	   ACS_CC_ReturnType enResult = ACS_CC_SUCCESS;

	   if((enResult = om->getAttribute(ACS_SSU_Common::dnOfSSURoot.c_str(),attributes)) == ACS_CC_SUCCESS)
	   {
		   for(int i=0; i<(int)attributes.size(); i++)
		   {
			   if (attributes[i]->attrType == 2) // For unsigned int
			   {
				   ssuIntegerInitialParamsMap.insert(std::pair<std::string,int>(attributes[i]->attrName,*(reinterpret_cast<unsigned int*>(attributes[i]->attrValues[0]))));
			   }
			   else if (attributes[i]->attrType == 9) // For string
			   {
				   ssuStringInitialParamsMap.insert(std::pair<std::string,std::string>(attributes[i]->attrName,(char *) attributes[i]->attrValues[0]));
			   }
		   }
	   }
	   else
	   {
		   ERROR("unable to fetch attribute values from %s", ACS_SSU_Common::dnOfSSURoot.c_str());
		   return false;
	   }
#endif
	   DEBUG("%s", "Exited ACS_SSU_Common::GetAllPhaParameters()");
	   return true;
   }

   //============================================================================
   // fetchDnOfRootObjFromIMM
   // Fetches the dn of root class from IMM
   // returns 0 on success and dnOfSSURoot contains the Parent DN
   // -1 on failure
   //============================================================================
   static int fetchDnOfRootObjFromIMM (OmHandler *om)
   {
	   static int infoAlreadyLoad = 0;
	   static char dnOfRootClass[512] = {0};

	   if (infoAlreadyLoad) { dnOfSSURoot = std::string(dnOfRootClass); return 0; }

	   std::vector<std::string> dnList;
	   int returnCode = 0;

	   if (om->getClassInstances(SSU_ROOT_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	   {
		   //ERROR
		   returnCode = -1;
	   }
	   else
	   {
		   //OK: Checking how much dn items was found in IMM
		   if (dnList.size() ^ 1)
		   {
			   //WARNING: 0 (zero) or more than one node found
			   returnCode = -1;
		   }
		   else
		   {
			   //OK: Only one root node
			   strncpy(dnOfRootClass, dnList[0].c_str(), ACS_ARRAY_SIZE(dnOfRootClass));
			   dnOfRootClass[ACS_ARRAY_SIZE(dnOfRootClass) - 1] = 0;
			   infoAlreadyLoad = 1;
			   dnOfSSURoot = std::string(dnOfRootClass);
		   }
	   }
	   return returnCode;
   }
   // End of fetchDnOfRootObjFromIMM
   static std::string dnOfSSURoot;


private:

  static bool bStripChar(const ACE_TCHAR * pszSrc, ACE_TCHAR *pszEnvVar,ACE_UINT32 u32Char)
  {
        bool bResult = true;
       const ACE_TCHAR *pchInstance2;
       //! Copy the Source String
       ACE_TCHAR * szTemp = pszEnvVar;

       const ACE_TCHAR *pchInstance1 = ACE_OS::strchr(pszSrc,u32Char);

	    if( pchInstance1 )
	    {
	    	pchInstance1++;

	       pchInstance2 = ACE_OS::strrchr(pchInstance1,u32Char);

	       if( !pchInstance2  )
	       {
	    	  bResult = false;
	       }
       }

        while( bResult && pchInstance1 < pchInstance2)
        {
        	*szTemp = *pchInstance1;
        	szTemp++;
        	pchInstance1++;
        }

	    *szTemp = '\0';

	    DEBUG( " bStripChar %s %s %s" , pszEnvVar, pchInstance1, pchInstance2);

	    return bResult;
   }




};

#endif

