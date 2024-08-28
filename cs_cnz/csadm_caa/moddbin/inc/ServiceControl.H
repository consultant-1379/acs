//******************************************************************************
// 
// .NAME
//    ServiceControl - Provides a set of functions that are generic when it
//                     comes to handing the Service Control Manager.
// .LIBRARY 3C++
// .PAGENAME ServiceControl
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
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
//  This file defines a set of function that are common to all cluster
//  resources of CPTASPBIN where communication should be made with an
//  APZ 212 40.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-05-02 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef SERVICECONTROL_H
#define SERVICECONTROL_H

#include <cassert>
#include <winsock2.h>
#include <windows.h>

namespace
{
   typedef int (*MainFunction)(const int*);
   typedef void (*AlarmCeasingFunction)();
   typedef bool (*ThreadExecutionControlFunction)();
};

class ServiceControl
{
public:
   // Sets the start point for the service.
   static void startPoint(MainFunction theMainFunction);

   // Sets the thread execution suspending function.
   static void threadSuspender(ThreadExecutionControlFunction theThreadSuspender);

   // Sets the alarm ceasing function for the running process.
   static void alarmCeaser(AlarmCeasingFunction theAlarmCeaser);

   // Sets the thread execution resuming function.
   static void threadResumer(ThreadExecutionControlFunction theThreadResumer);

   // Sets the thread's parameter address to supply to the thread's starting
   // point.
   static void threadParameter(void* theParameter);

   // Start point for the application when running as a service.
   static void serviceMain();

   // Start point for the application when not running as a service.
   static void noServiceMain();

   // Installs a service in the Service Control Manager.
   static void installService();

   // Removes a service from the Service Control Manager.
   static void removeService();

   // Begins execution of the service.
   static int commenceExecution(int argc, char* argv[], void (*cleanupFunction)(void) = NULL);

   // The controlHandler responds to any service requests issued by the
   // service control subsystem.
   static void controlHandler(DWORD theCode);

private:
   // Declare the constructor but do not implement it to prohibit its use.
   ServiceControl();

   // Tries to suspend the execution of all threads.
   static bool suspendThreads();

   // Tries to resume the execution of all threads.
   static bool resumeThreads();

   // Handle to the status information structure for the current service.
   static SERVICE_STATUS_HANDLE s_statusHandle;

   // Service specific information.
   static SERVICE_STATUS s_status;
   
   // Handle to the service thread.
   static HANDLE s_threadHandle;

   // The startpoint for the service that ServiceControl should dispatch.
   static MainFunction s_mainFunction;

   // The suspend thread execution function for the running process.
   static ThreadExecutionControlFunction s_suspendThreadExecution;

   // The alarm ceasing function for the running process.
   static AlarmCeasingFunction s_alarmCeaser;

   // The resume thread execution function for the running process.
   static ThreadExecutionControlFunction s_resumeThreadExecution;

   // Address to the parameter that should be supplied to the starting point
   // of the started thread.
   static void* s_threadParameter;

   // Keeps track of whether the application's threads are considered to be
   // running or suspended.
   static bool s_areThreadsRunning;
};

//******************************************************************************
// Description:
//    Sets the start point for the service.
// Parameters:
//    theMainFunction  Pointer to the function to use when dispatching the
//    service.
//******************************************************************************
inline void ServiceControl::startPoint(MainFunction theMainFunction)
{
   assert(s_mainFunction == NULL && "Do not reassign the main function for the service");

   s_mainFunction = theMainFunction;
}

//******************************************************************************
// Description:
//    Sets the thread execution suspending function.
// Parameters:
//    theThreadSuspender  Pointer to the function to use when PRC asks to stop
//                        the execution of the running process.
//******************************************************************************
inline void ServiceControl::threadSuspender(ThreadExecutionControlFunction theThreadSuspender)
{
   assert(s_suspendThreadExecution == NULL && "Do not reassign the thread suspending function for the service");

   s_suspendThreadExecution = theThreadSuspender;
}

//******************************************************************************
// Description:
//    Sets the alarm ceasing function for the running process.
// Parameters:
//    theAlarmCeaser  Pointer to the function to use when PRC asks to stop the
//                    execution of the running process. When this happens, the
//                    running process must cease all raised alarms.
//******************************************************************************
inline void ServiceControl::alarmCeaser(AlarmCeasingFunction theAlarmCeaser)
{
   assert(s_alarmCeaser == NULL && "Do not reassign the alarm ceasing function for the service");

   s_alarmCeaser = theAlarmCeaser;
}

//******************************************************************************
// Description:
//    Sets the thread execution resuming function.
// Parameters:
//    theThreadResumer  Pointer to the function to use when PRC asks to resume
//                      the execution of the running process.
//******************************************************************************
inline void ServiceControl::threadResumer(ThreadExecutionControlFunction theThreadResumer)
{
   assert(s_resumeThreadExecution == NULL && "Do not reassign the thread resuming function for the service");

   s_resumeThreadExecution = theThreadResumer;
}

//******************************************************************************
// Description:
//    Sets the thread's parameter address to supply to the thread's starting
//    point.
// Parameters:
//    theParameter  Address to the parameter that should be supplied to the
//                  thread's starting point.
//******************************************************************************
inline void ServiceControl::threadParameter(void* theParameter)
{
   assert(s_threadParameter == NULL && "Do not reassign the thread parameter address for the service");

   s_threadParameter = theParameter;
}

#endif
