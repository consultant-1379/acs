//******************************************************************************
//
// NAME
//      ServiceControl
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2005.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  Sleeps an infinite amount of time. The only reason for the existance of
//  this binary is to maintain the same set of processes in CPTASP that are
//  set up as cluster resources.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-08 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

//  SEE ALSO 
//  N/A.
//
//******************************************************************************

#ifdef _MSC_VER
// Suppress the following warnings when compiling with MSVC++:
// 4274  #ident ignored; see documentation for #pragma comment(exestr, 'string')
#pragma warning ( disable : 4274 )
#endif // _MSC_VER

#include "ServiceControl.H"
#include "EventReporter.H"
#include "Log.H"
#include "ACS_PRC_Process.H"
#include "ACS_ExceptionHandler.H"
#include "ACS_AEH_signalHandler.H"
#include <iostream>
#include <cassert>

extern const char* MAS_Common_ProcessIdentifier;

// Static member allocations
SERVICE_STATUS_HANDLE ServiceControl::s_statusHandle;
SERVICE_STATUS ServiceControl::s_status;
HANDLE ServiceControl::s_threadHandle;
MainFunction ServiceControl::s_mainFunction = NULL;
ThreadExecutionControlFunction ServiceControl::s_suspendThreadExecution = NULL;
AlarmCeasingFunction ServiceControl::s_alarmCeaser = NULL;
ThreadExecutionControlFunction ServiceControl::s_resumeThreadExecution = NULL;
void* ServiceControl::s_threadParameter = NULL;
bool ServiceControl::s_areThreadsRunning = false;

using namespace std;

//******************************************************************************
// Description:
//    The controlHandler responds to any service requests issued by the
//    service control subsystem.
// Parameters:
//    theCode  The reason why the service control subsystem called upon us.
//******************************************************************************
void ServiceControl::controlHandler(DWORD theCode)
{
   static bool isSuspendThreadFailureLogged = false;
   static bool isResumeThreadFailureLogged = false;
   static bool isUnexpectedCodeLogged = false;
   static bool isSetServiceStatusErrorLogged = false;

   DBGLOG("%s: controlHandler called with code %d\n", MAS_Common_ProcessIdentifier, theCode);

   switch(theCode)
   {
   case SERVICE_CONTROL_PAUSE:
      DBGLOG("Taking action SERVICE_CONTROL_PAUSE\n");
      
      // Suspend the execution of all application specific threads.
      if(suspendThreads() == true)
      {
         // Suspend the execution of the registered application thread.
         if(SuspendThread(s_threadHandle) == 0)
         {
            static_cast<void>(EVENT(EventReporter::NonCritical,
                  PROBLEM_DATA("All thread's exectution have entered a suspended state"),
                  PROBLEM_TEXT("")));

            // Inform the service control handler that all threads have
            // entered a suspended state.
            s_status.dwCurrentState = SERVICE_PAUSED;
            if(!SetServiceStatus(s_statusHandle, &s_status))
            {
               if(!isSetServiceStatusErrorLogged)
               {
                  if(EVENT(EventReporter::SubSystemError,
                           PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                        s_status.dwCurrentState,
                                        GetLastError()),
                           PROBLEM_TEXT("")) == EventReporter::OK)
                  {
                     isSetServiceStatusErrorLogged = true;
                  }
               }
            }
            else
            {
               static_cast<void>(EVENT(EventReporter::NonCritical,
                     PROBLEM_DATA("The execution of all threads has been suspended by SERVICE_CONTROL_PAUSE"),
                     PROBLEM_TEXT("")));
            }
         }
         else
         {
            if(!isSuspendThreadFailureLogged)
            {
               if(EVENT(EventReporter::SubSystemError,
                        PROBLEM_DATA("SuspendThread failed, GetLastError() reports %d",
                                     GetLastError()),
                        PROBLEM_TEXT("")) == EventReporter::OK)
               {
                  isSuspendThreadFailureLogged = true;
               }
            }

            // Tell the service control manager of the last known state since we
            // were unable to suspend the thread, moving to a paused state.
            if(!SetServiceStatus(s_statusHandle, &s_status))
            {
               if(!isSetServiceStatusErrorLogged)
               {
                  if(EVENT(EventReporter::SubSystemError,
                           PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                        s_status.dwCurrentState,
                                        GetLastError()),
                           PROBLEM_TEXT("")) == EventReporter::OK)
                  {
                     isSetServiceStatusErrorLogged = true;
                  }
               }
            }
         }
      }
      else
      {
         static_cast<void>(EVENT(EventReporter::ApplicationError,
               PROBLEM_DATA("Failed to suspend the execution of all threads"),
               PROBLEM_TEXT("")));
      }
      break;
      
   case SERVICE_CONTROL_CONTINUE:
      DBGLOG("Taking action SERVICE_CONTROL_CONTINUE\n");

      // Resume the execution of all application specific threads.
      if(resumeThreads() == true)
      {
         // Resume the execution of the registered application thread.
         if(ResumeThread(s_threadHandle) == 1)
         {
            static_cast<void>(EVENT(EventReporter::NonCritical,
                  PROBLEM_DATA("The application is continuing its execution"),
                  PROBLEM_TEXT("")));

            // Signal the service control manager or the change in state or
            // to tell it that we remain in the same state as before since the
            // thread's suspend count has still not reached 0.
            s_status.dwCurrentState = SERVICE_RUNNING;
            if(!SetServiceStatus(s_statusHandle, &s_status))
            {
               if(!isSetServiceStatusErrorLogged)
               {
                  if(EVENT(EventReporter::SubSystemError,
                           PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                        s_status.dwCurrentState,
                                        GetLastError()),
                           PROBLEM_TEXT("")) == EventReporter::OK)
                  {
                     isSetServiceStatusErrorLogged = true;
                  }
               }
            }
            else
            {
               static_cast<void>(EVENT(EventReporter::NonCritical,
                     PROBLEM_DATA("The execution of all threads has been resumed by SERVICE_CONTROL_CONTINUE"),
                     PROBLEM_TEXT("")));
            }
         }
         else
         {
            if(!isResumeThreadFailureLogged)
            {
               if(EVENT(EventReporter::SubSystemError,
                        PROBLEM_DATA("ResumeThread failed, GetLastError() reports %d",
                                     GetLastError()),
                        PROBLEM_TEXT("")) == EventReporter::OK)
               {
                  isResumeThreadFailureLogged = true;
               }
            }

            // Tell the service control manager of the last known state since
            // we were unable to resume the thread's execution, moving to a
            // running state.
            if(!SetServiceStatus(s_statusHandle, &s_status))
            {
               if(!isSetServiceStatusErrorLogged)
               {
                  if(EVENT(EventReporter::SubSystemError,
                           PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                        s_status.dwCurrentState,
                                        GetLastError()),
                           PROBLEM_TEXT("")) == EventReporter::OK)
                  {
                     isSetServiceStatusErrorLogged = true;
                  }
               }
            }
         }
      }
      else
      {
         static_cast<void>(EVENT(EventReporter::ApplicationError,
               PROBLEM_DATA("Failed to resume the execution of all threads"),
               PROBLEM_TEXT("")));
      }
      break;
      
   case SERVICE_CONTROL_STOP:
   case SERVICE_CONTROL_SHUTDOWN:
      DBGLOG("Taking action %s\n", (theCode == SERVICE_CONTROL_STOP ? "SERVICE_CONTROL_STOP" : "SERVICE_CONTROL_SHUTDOWN"));

      // Suspend the execution of all application specific threads.
      if(suspendThreads() == true)
      {
         // Ask the process for which all threads have been suspended to cease
         // all its alarms.
         DBGLOG("Calling the process' registered alarm ceasing method\n");
         s_alarmCeaser();

         s_status.dwCurrentState = SERVICE_STOPPED;
         s_status.dwWin32ExitCode = 0;
         s_status.dwCheckPoint = 0;
         s_status.dwWaitHint = 0;

         // Tell the service control subsystem that the service is stopped and
         // terminate, ignore any failure to signal the stopped service since we
         // are about to terminate anyhow.
         if(!SetServiceStatus(s_statusHandle, &s_status))
         {
            if(!isSetServiceStatusErrorLogged)
            {
               if(EVENT(EventReporter::SubSystemError,
                        PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                     s_status.dwCurrentState,
                                     GetLastError()),
                        PROBLEM_TEXT("")) == EventReporter::OK)
               {
                  isSetServiceStatusErrorLogged = true;
               }
            }
         }
         else
         {
            static_cast<void>(EVENT(EventReporter::NonCritical,
                  PROBLEM_DATA("The execution of all threads has been stopped by %s",
                               (theCode == SERVICE_CONTROL_STOP ? "SERVICE_CONTROL_STOP" : "SERVICE_CONTROL_SHUTDOWN")),
                  PROBLEM_TEXT("")));
         }
      }
      else
      {
         static_cast<void>(EVENT(EventReporter::ApplicationError,
               PROBLEM_DATA("Failed to suspend the execution of all threads"),
               PROBLEM_TEXT("")));

         // Ask the running proess to cease alarms if the process has
         // registered an alarm ceasing function.
         DBGLOG("Calling the process' registered alarm ceasing method\n");
         s_alarmCeaser();
      }
      return;
      
   case SERVICE_CONTROL_INTERROGATE:
      // Tell the service control manager of the current state.
      if(!SetServiceStatus(s_statusHandle, &s_status))
      {
         if(!isSetServiceStatusErrorLogged)
         {
            if(EVENT(EventReporter::SubSystemError,
                     PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                  s_status.dwCurrentState,
                                  GetLastError()),
                     PROBLEM_TEXT("")) == EventReporter::OK)
            {
               isSetServiceStatusErrorLogged = true;
            }
         }
      }
      break;
      
   default:
      if(!isUnexpectedCodeLogged)
      {
         if(EVENT(EventReporter::ProgrammingError,
                  PROBLEM_DATA("Code is missing to handle the requested operation (%d)",
                               theCode),
                  PROBLEM_TEXT("")) == EventReporter::OK)
         {
            isUnexpectedCodeLogged = true;
         }
      }
   }
}

//******************************************************************************
// Description:
//    Start point for the application when running as a service.
//******************************************************************************
void ServiceControl::serviceMain()
{
   static bool isSetServiceStatusErrorLogged = false;

   DBGLOG("%s: serviceMain()\n", MAS_Common_ProcessIdentifier);

   // Tell the service subsystem the function that should handle service
   // control requests.
   s_statusHandle = RegisterServiceCtrlHandler(MAS_Common_ProcessIdentifier,
                                               (LPHANDLER_FUNCTION) controlHandler);

   if(s_statusHandle && s_mainFunction)
   {
      s_status.dwServiceType = SERVICE_WIN32;
      s_status.dwCurrentState = SERVICE_START_PENDING;
      s_status.dwControlsAccepted = 0;
      s_status.dwWin32ExitCode = 0;
      s_status.dwServiceSpecificExitCode = 0;
      s_status.dwCheckPoint = 0;
      s_status.dwWaitHint = 0;
      if(!SetServiceStatus(s_statusHandle, &s_status))
      {
         if(!isSetServiceStatusErrorLogged)
         {
            if(EVENT(EventReporter::SubSystemError,
                     PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                  s_status.dwCurrentState,
                                  GetLastError()),
                     PROBLEM_TEXT("")) == EventReporter::OK)
            {
               isSetServiceStatusErrorLogged = true;
            }
         }
      }

      // Create the thread that starts the execution of the provided main function.
      s_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) s_mainFunction, 
                                    s_threadParameter, 0, NULL);

      s_status.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                    SERVICE_ACCEPT_PAUSE_CONTINUE |
                                    SERVICE_ACCEPT_SHUTDOWN;
      s_status.dwCurrentState = SERVICE_RUNNING;
      if(!SetServiceStatus(s_statusHandle, &s_status))
      {
         if(!isSetServiceStatusErrorLogged)
         {
            if(EVENT(EventReporter::SubSystemError,
                     PROBLEM_DATA("SetServiceStatus(%d) failed, GetLastError() reports %d",
                                  s_status.dwCurrentState,
                                  GetLastError()),
                     PROBLEM_TEXT("")) == EventReporter::OK)
            {
               isSetServiceStatusErrorLogged = true;
            }
         }
      }

      Sleep(INFINITE);
   }
   else
   {
      if(!s_statusHandle)
      {
         DWORD lastError = GetLastError();

         static_cast<void>(EVENT(EventReporter::SubSystemError,
               PROBLEM_DATA("RegisterServiceCtrlHandler(%s, ...) failed, GetLastError() reports %s (%d)\n",
                            MAS_Common_ProcessIdentifier,
                            lastError == ERROR_INVALID_NAME ? "Invalid name" :
                              lastError == ERROR_SERVICE_DOES_NOT_EXIST ? "Service does not exist" : "Unknown",
                              lastError),
               PROBLEM_TEXT("")));
      }
      else
      {
         assert(!"A call to ServiceControl::startPoint(...) must be made prior to the call to ServiceControl::serviceMain()");
         static_cast<void>(EVENT(EventReporter::ProgrammingError,
               PROBLEM_DATA("A call to ServiceControl::startPoint(...) must be made prior to the call to ServiceControl::serviceMain()"),
               PROBLEM_TEXT("")));
      }
   }
}

//******************************************************************************
// Description:
//    Start point for the application when not running as a service.
//******************************************************************************
void ServiceControl::noServiceMain()
{
   DBGLOG("%s: noServiceMain()\n", MAS_Common_ProcessIdentifier);

   // Create the thread that starts the execution of the provided main function.
   s_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) s_mainFunction, 
                                 s_threadParameter, 0, NULL);

   if(s_threadHandle == NULL)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("CreateThread(..., s_mainFunction) failed, GetLastError() reports %d\n",
                           GetLastError()),
            PROBLEM_TEXT("")));
   }

   DBGLOG("%s: noServiceMain() goes to sleep\n", 
          MAS_Common_ProcessIdentifier);
   Sleep(INFINITE);
}

//******************************************************************************
// Description:
//    This function installs the service in the Service Control Manager.
//******************************************************************************
void ServiceControl::installService()
{
   DBGLOG("installService\n");

   SC_HANDLE serviceControlManager;
   SC_HANDLE service;
   const int MaxFilePathLength = 512;
   TCHAR filePath[MaxFilePathLength];

   if(0 == GetModuleFileName(NULL, filePath, MaxFilePathLength))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("GetModuleFileName(NULL, %s, %d) failed, GetLastError() reports %d",
                         filePath ? filePath : "NULL",
                         MaxFilePathLength,
                         GetLastError()),
            PROBLEM_TEXT("")));
   }

   serviceControlManager = OpenSCManager(NULL, // Connect to the Service control manager on the local machine
                                         NULL, // Use SERVICES_ACTIVE_DATABASE
                                         SC_MANAGER_CREATE_SERVICE);

   service = CreateService(serviceControlManager,        // Service Control Manager handle
                           MAS_Common_ProcessIdentifier, // Service name
                           MAS_Common_ProcessIdentifier, // Display name
                           SC_MANAGER_CREATE_SERVICE,    // Create the service
                           SERVICE_WIN32_OWN_PROCESS,    // The service should run in a process of its own
                           SERVICE_AUTO_START,           // The service should start automatically by the Service Control Manager during startup
                           SERVICE_ERROR_IGNORE,         // Log startup errors but continue the startup operation
                           filePath,                     // Location of the binary
                           NULL,                         // No group
                           NULL,                         // No change of an existing tag
                           NULL,                         // No dependencies
                           NULL,                         // Use the local system account when starting the service
                           "");                          // No password required for the local system account

   if(0 == service)
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("CreateService(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }
   else
   {
      if(0 == CloseServiceHandle(service))
      {
         static_cast<void>(EVENT(EventReporter::SubSystemError,
               PROBLEM_DATA("CloseServiceHandle(...) failed, GetLastError() reports %d",
                            GetLastError()),
               PROBLEM_TEXT("")));
      }
   }
   if(0 == CloseServiceHandle(serviceControlManager))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("CloseServiceHandle(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }
}

//******************************************************************************
// Description:
//    This function removes the service from the Service Control Manager.
//******************************************************************************
void ServiceControl::removeService()
{
   DBGLOG("%s: removeService\n", MAS_Common_ProcessIdentifier);

   SC_HANDLE serviceControlManager;
   SC_HANDLE service;
   
   serviceControlManager = OpenSCManager(NULL,
                                         NULL,
                                         SC_MANAGER_ALL_ACCESS);

   service = OpenService(serviceControlManager,
                         MAS_Common_ProcessIdentifier,
                         SERVICE_ALL_ACCESS);

   if(0 == ControlService(service, SERVICE_CONTROL_STOP, &s_status))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("ControlService(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }

   while(QueryServiceStatus(service, &s_status) &&
         s_status.dwCurrentState != SERVICE_STOPPED)
   {
      if(s_status.dwCurrentState == SERVICE_STOP_PENDING)
      {
         Sleep(1000);
      }
      else
      {
         DBGLOG("Unable to remove service\n");
      }
   };

   if(0 == DeleteService(service))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("DeleteService(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }

   if(0 == CloseServiceHandle(service))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("CloseServiceHandle(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }

   if(0 == CloseServiceHandle(serviceControlManager))
   {
      static_cast<void>(EVENT(EventReporter::SubSystemError,
            PROBLEM_DATA("CloseServiceHandle(...) failed, GetLastError() reports %d",
                         GetLastError()),
            PROBLEM_TEXT("")));
   }
}

//******************************************************************************
// Description:
//    Tries to suspend the execution of all threads if the threads are not
//    already considered to be suspended.
// Returns:
//    true if all threads were suspended successfully.
//******************************************************************************
bool ServiceControl::suspendThreads()
{
   DBGLOG("IN suspendThreads() s_areThreadsRunning = %s\n", (s_areThreadsRunning ? "true" : "false"));

   if(s_areThreadsRunning)
   {
      DBGLOG("Calling the process' registered suspend thread execution method\n");

      // Try to suspend the application's running threads.
      bool isThreadSuspended = s_suspendThreadExecution();

      // Was it successful?
      s_areThreadsRunning = (isThreadSuspended ? false : true);

      DBGLOG("OUT suspendThreads() s_areThreadsRunning = %s, isThreadSuspended = %s\n", (s_areThreadsRunning ? "true" : "false"), (isThreadSuspended ? "true" : "false"));

      return isThreadSuspended;
   }

   DBGLOG("OUT suspendThreads() s_areThreadsRunning = %s\n", (s_areThreadsRunning ? "true" : "false"));

   return true;
}

//******************************************************************************
// Description:
//    Tries to resume the execution of all threads if the threads are already
//    considered to be suspended.
// Returns:
//    true if all threads' execution were resumed successfully.
//******************************************************************************
bool ServiceControl::resumeThreads()
{
   DBGLOG("IN resumeThreads() s_areThreadsRunning = %s\n", (s_areThreadsRunning ? "true" : "false"));

   if(!s_areThreadsRunning)
   {
      DBGLOG("Calling the process' registered resume thread execution method\n");

      // Try to resume the execution of the application's suspended threads.
      bool isThreadResumed = s_resumeThreadExecution();

      // Was it successful?
      s_areThreadsRunning = isThreadResumed;

      DBGLOG("OUT suspendThreads() s_areThreadsRunning = %s, isThreadResumed = %s\n", (s_areThreadsRunning ? "true" : "false"), (isThreadResumed ? "true" : "false"));

      return isThreadResumed;
   }

   DBGLOG("OUT resumeThreads() s_areThreadsRunning = %s\n", (s_areThreadsRunning ? "true" : "false"));

   return true;
}

//******************************************************************************
// Description:
//    Begins execution of the service.
// Parameters:
//    argc          The number of arguments passed to the process when
//                  starting.
//    argv          The arguments passed to the process when starting.
//    crashRoutine  A function specified by the user. This function will get
//                  called in the event that the process is about to crash.
// Returns:
//    0 indicates termination without errors.
//******************************************************************************
int ServiceControl::commenceExecution(int argc, char* argv[],
                                      void (*crashRoutine)(void))
{
   if(!(s_suspendThreadExecution &&
        s_alarmCeaser &&
        s_resumeThreadExecution))
   {
      assert(!"You must register methods to suspend and resume thread execution as well as ceasing alarms before calling the commenceExecution method");
      return 1;
   }

   if(argc == 1)
   {
      DWORD lastError = 0;

      AP_InitProcess(MAS_Common_ProcessIdentifier, AP_SERVICE);
      AP_SetCleanupAndCrashRoutine(MAS_Common_ProcessIdentifier, crashRoutine);
      ACS_AEH_setSignalExceptionHandler(MAS_Common_ProcessIdentifier, PRC);

      {
         SERVICE_TABLE_ENTRY dispatchTable[] =
         {
            {TEXT("ServiceMain"), (LPSERVICE_MAIN_FUNCTION) ServiceControl::serviceMain},
            {NULL, NULL }
         };

         // Consider the threads to be running unless an error ocurrs.
         s_areThreadsRunning = true;

         BOOL result = StartServiceCtrlDispatcher(dispatchTable);

         if(!result)
         {
            // It was not possible to start the application's threads.
            s_areThreadsRunning = false;

            lastError = GetLastError();

            static_cast<void>(EVENT(EventReporter::SubSystemError,
                  PROBLEM_DATA("StartServiceCtrlDispatcher(...) for %s failed, GetLastError() reports %s (%d)",
                               MAS_Common_ProcessIdentifier,
                               lastError == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ? "ERROR_FAILED_SERVICE_CONTROLLER_CONNECT" :
                                 lastError == ERROR_INVALID_DATA ? "ERROR_INVALID_DATA" : "Unknown",
                                 lastError),
                  PROBLEM_TEXT("")));
         }
      }
      return lastError;
   }
   else
   {
      if(!stricmp(argv[1], "install"))
      {
         ServiceControl::installService();
      }
      else if(!stricmp(argv[1], "remove"))
      {
         ServiceControl::removeService();
      }
      else if(!stricmp(argv[1], "noservice"))
      {
         ServiceControl::noServiceMain();
      }

      return 0;
   }
}
