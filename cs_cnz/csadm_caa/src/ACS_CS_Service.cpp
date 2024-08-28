

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_EventReporter.h"

#include "ACS_PRC_Process.H"


// ACS_CS_ServiceHandler
#include "ACS_CS_ServiceHandler.h"
// ACS_CS_Service
#include "ACS_CS_Service.h"




#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_Service_TRACE);

#include <process.h>

using namespace ACS_CS_NS;

static const int DISKEEPER_INITIAL_WAIT = 5 * 60 * 1000;
static const int DISKEEPER_LOOP_WAIT    = 60 * 1000;


namespace {
	int fxStartService(char * serviceName) {
		int result = 0;
			
		if (SC_HANDLE scmHandle = OpenSCManager(0, 0, SC_MANAGER_CONNECT)) { //OK
			if (SC_HANDLE serviceHandle = OpenService(scmHandle, serviceName, SERVICE_START)) { //OK
				const char * args[] = {serviceName};
				unsigned sysLastError = 0;

				if (StartService(serviceHandle, 1, args) == FALSE) { //ERROR
					sysLastError = GetLastError();
					if (sysLastError == ERROR_SERVICE_ALREADY_RUNNING) result = 0; //NOT ERROR
					else {
						result = __LINE__;
						char tracep[128] = {0};
						_snprintf(tracep, sizeof(tracep) - 1, "Cannot start the Windows Service %s. System last error = %u", serviceName, sysLastError);

						ACS_CS_EVENT(Event_StartupFailure,
								"Start Windows Service",
								tracep,
								"");
					}
				}
				
				CloseServiceHandle(serviceHandle);
			} else {
				result = __LINE__;
				char tracep[128] = {0};
				_snprintf(tracep, sizeof(tracep) - 1, "'OpenService(...' failed on service %s. System last error = %u\n", serviceName, GetLastError());

				ACS_CS_EVENT(Event_StartupFailure,
						"Open Windows Service",
						tracep,
						"");
			}
				
			CloseServiceHandle(scmHandle);
		} else {
			result = __LINE__;
			char tracep[128] = {0};
			_snprintf(tracep, sizeof(tracep) - 1, "'OpenSCManager(...' failed. System last error = %u\n", GetLastError());

			ACS_CS_EVENT(Event_StartupFailure,
					"Open Windows SCManager",
					tracep,
					"");
		}

		return result;
	}

	int prepareDiskeeperOnOpen() {
		int diskeeperCode = 0;
		
		if (fxStartService("Diskeeper")) return -2;
		
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::open()::prepareDiskeeperOnOpen()\n"
			"  Entering function to prepare DISKEEPER (command = 'diskeeper I: /s /jt=z /ds')\n"));
		char traceMessage[1024] = {0};
		
		FILE * pipe = _popen("diskeeper I: /s /jt=z /ds", "rt");

		if (pipe) { //OK
			char pipeBuffer[512];

			while (!feof(pipe)) if (fgets(pipeBuffer, 511, pipe)) {
				_snprintf(traceMessage, sizeof(traceMessage) - 1,
					"(%%t) ACS_CS_Service::open()::prepareDiskeeperOnOpen()\n"
					"  COMMAND OUTPUT: '%s'\n",
					pipeBuffer);
				ACS_CS_TRACE((ACS_CS_Service_TRACE, traceMessage));
			}

			diskeeperCode = _pclose(pipe);

			//Closing command pipe
			_snprintf(traceMessage, sizeof(traceMessage) - 1,
				"(%%t) ACS_CS_Service::open()::prepareDiskeeperOnOpen()\n"
				"  COMMAND ENDED: Return code = %d\n",
				diskeeperCode);
			ACS_CS_TRACE((ACS_CS_Service_TRACE, traceMessage));
		} else { //Command open filed
			diskeeperCode = -1;
			_snprintf(traceMessage, sizeof(traceMessage) - 1,
				"(%%t) ACS_CS_Service::open()::prepareDiskeeperOnOpen()\n"
				"  '_popen(\"diskeeper I: /s /jt=z /ds\", \"rt\")' failed. ERROR CODE = %d\n", errno);
			ACS_CS_TRACE((ACS_CS_Service_TRACE, traceMessage));
		}

		return diskeeperCode;
	}

	unsigned __stdcall fxDiskeeperThreadProcedure(void * args) {
		if (ACS_CS_Protocol::CS_ProtocolChecker::checkIfActiveNode())
      {
         HANDLE shutdownEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);

         if(NULL == shutdownEvent)
         {
            ACS_CS_TRACE((ACS_CS_Service_TRACE,
                     "(%t) fxDiskeeperThreadProcedure()\n"
                     "Error: Could not get handle to shutdown event.\n"));
            return 0;
         }

         if(WaitForSingleObject(shutdownEvent, DISKEEPER_INITIAL_WAIT) == WAIT_OBJECT_0)
         {
            ACS_CS_TRACE((ACS_CS_Service_TRACE,
                     "(%t) fxDiskeeperThreadProcedure()\n"
                     "Info: shutdown signaled.\n"));
            return 0;
         }

         while(prepareDiskeeperOnOpen() == -2)
         {
            ACS_CS_TRACE((ACS_CS_Service_TRACE,
                     "(%t) fxDiskeeperThreadProcedure()\n"
                     "Error: prepareDiskeeperOnPollInterval() failed!\n"));

            if(WaitForSingleObject(shutdownEvent, DISKEEPER_LOOP_WAIT) == WAIT_OBJECT_0)
            {
               ACS_CS_TRACE((ACS_CS_Service_TRACE,
                        "(%t) fxDiskeeperThreadProcedure()\n"
                        "Info: shutdown signaled.\n"));
               return 0;
            }
         }
      }

      return 0;
	}	 

	void fxCreateThreadForDiskeeper() {
		unsigned threadID = 0;
		CloseHandle(reinterpret_cast<HANDLE>(_beginthreadex(0, 0, fxDiskeeperThreadProcedure, 0, 0, &threadID)));
	}
}

// Class ACS_CS_Service 

ACS_CS_Service::ACS_CS_Service()
      : serviceHandler(0),
        syncReplicationEvent(0),
        hwcReplicationEvent(0),
        cpIdReplicationEvent(0),
        cpGroupReplicationEvent(0),
        fdReplicationEvent(0),
        neReplicationEvent(0),
        vlanReplicationEvent(0),
        shutdownAllEvent(0),
        shutdownHandlerEvent(0)
  , ACS_CS_ServiceBase(ACS_CS_PROCESS_NAME)
{

   ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Creating service\n"));

   // Events are created here for use in different parts of the service

   // Event that is signaled to shutdown the service handler
   shutdownHandlerEvent = CreateEvent(NULL, TRUE, FALSE, ACS_CS_NS::EVENT_NAME_SHUTDOWN_SERVICE_HANDLER);

	if (shutdownHandlerEvent == 0)
	{
      ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create shutdown event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
				"Create Windows Event",
				"Cannot create Windows event",
				"");
	}

   // Event that is signaled to shutdown the entire service
   shutdownAllEvent = CreateEvent(NULL, TRUE, FALSE, ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	if (shutdownAllEvent == 0)
	{
      ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create shutdown event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
				"Create Windows Event",
				"Cannot create Windows event",
				"");
	}

   // Event that is signaled when replication data is received by a slave
   syncReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_SYNC);

	if (syncReplicationEvent == 0)
	{
      ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication sync event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
				"Create Windows Event",
				"Cannot create Windows event",
				"");
	}

   // Create event to be used to wait for replication of the HWC table
   hwcReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_HWC);

	if (hwcReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   // Create event to be used to wait for replication of the CP table
   cpIdReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_CPID);

	if (cpIdReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   // Create event to be used to wait for replication of the CP group table
   cpGroupReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_CPGROUP);

	if (cpGroupReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   // Create event to be used to wait for replication of the function distribution table
   fdReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_FD);

	if (fdReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   // Create event to be used to wait for replication of the network element table
   neReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_NE);

	if (neReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   // Create event to be used to wait for replication of the VLAN table
   vlanReplicationEvent = CreateEvent(NULL, FALSE, FALSE, ACS_CS_NS::EVENT_NAME_REPLICATION_VLAN);

	if (vlanReplicationEvent == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::ACS_CS_Service()\n"
			"Error: Cannot create replication event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
					"Create Windows Event",
					"Cannot create Windows event",
					"");
	}

   serviceHandler = new ACS_CS_ServiceHandler();

}


ACS_CS_Service::~ACS_CS_Service()
{

   if (shutdownHandlerEvent)
      (void) CloseHandle(shutdownHandlerEvent);

   if (shutdownAllEvent)
      (void) CloseHandle(shutdownAllEvent);

   if (hwcReplicationEvent)
      (void) CloseHandle(hwcReplicationEvent);

   if (cpIdReplicationEvent)
      (void) CloseHandle(cpIdReplicationEvent);

   if (cpGroupReplicationEvent)
      (void) CloseHandle(cpGroupReplicationEvent);

   if (fdReplicationEvent)
      (void) CloseHandle(fdReplicationEvent);

   if (neReplicationEvent)
      (void) CloseHandle(neReplicationEvent);

   if (vlanReplicationEvent)
      (void) CloseHandle(vlanReplicationEvent);

   if (syncReplicationEvent)
      (void) CloseHandle(syncReplicationEvent);

   if (serviceHandler)
      delete serviceHandler;

}



 int ACS_CS_Service::open (int arg1, char **arg2)
{

   ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::open()\n"
			"Entering function\n"));

   fxCreateThreadForDiskeeper();
   
   //prepareDiskeeperOnOpen();

   if (serviceHandler)
      (void) serviceHandler->activate();

   return 0;

}

 int ACS_CS_Service::close ()
{

   ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::close()\n"
			"Entering function\n"));
   
   int retries = 0;
   
   //try to set Event for 5000 ms
   while (SetEvent(shutdownAllEvent) == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::close()\n"
			"Error: Cannot signal shutdown event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
				"Create Windows Event",
				"Cannot signal Windows event",
				"");
		
		if( ++retries > 10) break;
		
		Sleep(500);
	}

	//restart the counter
    retries = 0;

   while (SetEvent(shutdownHandlerEvent) == 0)
	{
		ACS_CS_TRACE((ACS_CS_Service_TRACE,
			"(%t) ACS_CS_Service::close()\n"
			"Error: Cannot signal shutdown event, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
				"Create Windows Event",
				"Cannot signal Windows event",
				"");

		if( ++retries > 10) break;
			
		Sleep(500);

	}

	if (serviceHandler)
	{
		serviceHandler->deActivate();
		bool noTimeout = serviceHandler->wait(Amplify_Timeout * (Mutex_Timeout + Connection_Timeout));  // TR HL33186
		if (!noTimeout)
		{
			ACS_CS_TRACE((ACS_CS_Service_TRACE,
				"(%t) ACS_CS_Service_TRACE::close()()\n"
				"Wait for the ACS_CS_ServiceHandler thread to finish: Timeout expired, the thread is still running\n"));

			serviceHandler->terminate();

			ACS_CS_EVENT(Event_NonCritical,
				"Create Windows Event",
				"Wait for the ACS_CS_ServiceHandler thread to finish: Timeout expired",
				"");
		}
		serviceHandler->close();
	}

   return 0;

}

// Additional Declarations

