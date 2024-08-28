//======================================================================
//
// NAME
//      HealthCheckServer.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================

#include <iostream>
#include "acs_hcs_healthcheckserver.h"
#include "acs_hcs_healthcheckservice.h"

using namespace std;
using namespace AcsHcs;

#define HCS_DOMAIN_LENGTH 255
#define AP1_NET1_NODEA  "192.168.169.1"
#define AP1_NET1_NODEB  "192.168.169.2"
#define AP1_NET2_NODEA  "192.168.170.1"
#define AP1_NET2_NODEB  "192.168.170.2"
#define AP2_NET1_NODEA  "192.168.169.3"  
#define AP2_NET1_NODEB  "192.168.169.4"
#define AP2_NET2_NODEA  "192.168.170.3"
#define AP2_NET2_NODEB  "192.168.170.4"

//SERVICE_STATUS			HCSServerStatus;
//SERVICE_STATUS_HANDLE	HCSServerStatusHandle;
ACE_HANDLE					HCSStopEvent = NULL;

// the HCS Service static instance.
HealthCheckService HealthCheckServer::theService_;

// the HCS Service name.
char* HealthCheckServiceName = "ACS_HCS_Service";

int main(int argc, char* argv[])
{
	DEBUG("%s"," Entering main() in healthcheck server");		
	// Description:
	// The main-program. Used to install, uninstall, test and start an HCS service.
	
	// the server hosting HCS service
	HealthCheckServer theServer;

	// set the managed service name
	theServer.SetServiceName(HealthCheckServiceName);
	// set the managed service display name
	theServer.SetServiceDisplayName(HealthCheckServiceName);

	if (argc > 1)
	{
		if (stricmp(argv[1], "install") == 0)
		{
			cout << "Install the service : " << theServer.GetServiceName() << endl;
			theServer.Install();
			ExitProcess(0);
		}
		if (stricmp(argv[1], "remove") == 0)
		{
			theServer.Remove();
			ExitProcess(0);
		}
		if (stricmp(argv[1], "debug") == 0)
		{
			theServer.DebugService();
			ExitProcess(0);
		}
		 cout << "Usage : " << argv[0] << " {install | remove | debug }" << endl;
	}
	else
	{
	/*	SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{ TEXT("ACS_HCS_Service"), (LPSERVICE_MAIN_FUNCTION) HealthCheckServer::StartService },
			{ NULL, NULL }
		}; 

		if (!StartServiceCtrlDispatcher(DispatchTable))
		{
			ACE_UINT32 er =  ACE_OS::last_error ();
			cout << "StartServiceCtrlDispatcher failed" << endl;
			cerr << "Error:  ACE_OS::last_error () = " << er << endl;
            return -1;
        }*/
    }
 DEBUG("%s","Leaving main in healthcheckserver ");
 return -1;
} // End of main

HealthCheckServer::HealthCheckServer()
{
}


HealthCheckServer::~HealthCheckServer()
{
}

void /*WINAPI*/ HealthCheckServer::StartService()
{
	DEBUG("%s","Entering  HealthCheckServer::StartService() ");	
	// Description:
	//	The main-program called from SCM or from console(debug mode)

	// Register control handler
	HCSServerStatusHandle = RegisterServiceCtrlHandler(HealthCheckServiceName,(LPHANDLER_FUNCTION) ServerCtrlHandler);
	if (!HCSServerStatusHandle)
	{
		cout << "ServiceMain failed to register control handler" << endl;
		DEBUG("%s","Leaving  HealthCheckServer::StartService() ");
		return;
	}

	SendStatus(SERVICE_START_PENDING);
	// Create stop event
	HCSStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (HCSStopEvent == NULL)
	{
		cout << "ServiceMain failed to create stop event. System Error=" <<  ACE_OS::last_error () << endl;
		DEBUG("%s","Leaving  HealthCheckServer::StartService() ");
		return;
	}
	SendStatus(SERVICE_RUNNING);

	// start HCS service now
	theService_.start(HCSStopEvent);

	SendStatus(SERVICE_STOPPED);

	ExitProcess(0);
	DEBUG("%s","Leaving  HealthCheckServer::StartService() ");
	return;
} // End of HCSServerMain

void HealthCheckServer::DebugService()
{	
	DEBUG("%s","Entering  HealthCheckServer::DebugService() ");
	cout << "Debug mode started " << endl;

   // Make sure to catch Ctrl-C
   (void) SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	cout << " SetConsoleCtrlHandler called " << endl;

	// Create stop event
	HCSStopEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (HCSStopEvent == NULL)
	{
		DEBUG("%s","Leaving  HealthCheckServer::DebugService() ");
		cout << "ServiceMain failed to create stop event. System Error=" <<  ACE_OS::last_error () << endl;
		return;
	}

	// Initiate user's service
	theService_.start(HCSStopEvent);

	cout << " HCS ending... " << endl;
	DEBUG("%s","Calling  SendStatus( SERVICE_STOPPED ); ");
   // Set service state
   SendStatus( SERVICE_STOPPED );
	DEBUG("%s","Leaving  HealthCheckServer::DebugService() ");

}

bool WINAPI HealthCheckServer::ConsoleHandler (ACE_UINT32 controlCode)
{
	DEBUG("%s","Entering WINAPI HealthCheckServer::ConsoleHandler ( ");
   switch (controlCode)
   {
   case CTRL_C_EVENT:
   case CTRL_BREAK_EVENT:

    // SendStatus(SERVICE_STOP_PENDING);
	SetEvent(HCSStopEvent);
	DEBUG("%s","Leaving WINAPI HealthCheckServer::ConsoleHandler ( ");
    return TRUE;

   default:
      break;
   }
	DEBUG("%s","Leaving WINAPI HealthCheckServer::ConsoleHandler ( ");
   return FALSE;

}

void HealthCheckServer::Install()
{
	DEBUG("%s","Entering HealthCheckServer::Install() ");
	// Description:
	//	This routine is used to install an HCS service

	SC_HANDLE SCManager;
	SC_HANDLE Service;

	char buffer[HCS_MAX_BUFSIZE];
	char filepath[HCS_MAX_BUFSIZE];

	if (GetModuleFileName(NULL, buffer, HCS_MAX_BUFSIZE) == 0)
	{	
		DEBUG("%s","Leaving HealthCheckServer::Install() ");
		cout << "Error. failed to install Server. R=" <<  ACE_OS::last_error () << endl;
		return;
	}
	strcpy(filepath, "\"");
	strcat(filepath, buffer);
	strcat(filepath, "\"");

	cout << " I got the module file name, filepath is " << filepath << endl;

	SCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (SCManager)
	{
		Service = CreateService(SCManager,serviceName_,serviceDisplayName_,
								SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
								SERVICE_DEMAND_START,SERVICE_ERROR_IGNORE,filepath,
								NULL,NULL,NULL,NULL,NULL);
		if (Service)
		{
			CloseServiceHandle(Service);
		}
		else
		{
			ACE_UINT32 err =  ACE_OS::last_error ();
			if (err != ERROR_SERVICE_EXISTS)
			{
				cerr << "HealthCheckServer. Failed to OpenSCManager. System error=" <<  ACE_OS::last_error () << endl;
			}
		}
		CloseServiceHandle(SCManager);
		cout << "Close Service Control Manager handle " << endl;
	}
	else
	{
		cout << "Failed to open Service Control Manager. " <<  ACE_OS::last_error () << endl;
	}
	DEBUG("%s","Leaving HealthCheckServer::Install() ");
} // End of HCSInstall

void HealthCheckServer::Remove()
{
	DEBUG("%s"," Entering  HealthCheckServer::Remove(");
	// Description:
	//	This routine is used to uninstall an HCS service.

	SC_HANDLE	SCManager;
	SC_HANDLE	Service;
	
	SCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (SCManager)
	{
		Service = OpenService(SCManager,serviceName_,SERVICE_ALL_ACCESS);
		if (Service)
		{
			if (ControlService(Service, SERVICE_CONTROL_STOP, &HCSServerStatus))
			{
				// cout << "Stopping ACS_HCS_Server" << endl;
				Sleep(1000);
				
				while (QueryServiceStatus(Service, &HCSServerStatus))
				{
					if (HCSServerStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						// cout << "." << endl;
						Sleep(1000);
					}
					else
					{
						break;
					}
				} // End of while
				if (HCSServerStatus.dwCurrentState == SERVICE_STOPPED)
				{
					cout << "ACS_HCS_Server stopped" << endl;
					DEBUG("%s"," ACS_HCS_Server stopped in HealthCheckServer::Remove(");
				}
				else
				{
					cout << "Failed to stop ACS_HCS_Server" << endl;
					DEBUG("%s"," Failed to stop ACS_HCS_Server in HealthCheckServer::Remove(")
				}
			}
			if ( DeleteService(Service) )
			{
				cout << "ACS_HCS_Server removed" << endl;
				DEBUG("%s"," ACS_HCS_Server removed in HealthCheckServer::Remove(");
			}
			else
			{
				cout << "Failed to remove ACS_HCS_Server" << endl;
				DEBUG("%s","Failed to ACS_HCS_Server remove in HealthCheckServer::Remove(");
			}
			
			CloseServiceHandle(Service);
		}
		else
		{
			DEBUG("%s"," ACS_HCS_Server Failed to OpenService.. in HealthCheckServer::Remove(");
			cout << "ACS_HCS_Server. Failed to OpenService. System error=" <<  ACE_OS::last_error () << endl;
			CloseServiceHandle(SCManager);
		}
	}
	else
	{
		cout << "Open ServiceControlManager failed" << endl;
	}
	DEBUG("%s"," Leaving  HealthCheckServer::Remove(");
} // End of HCSRemove

void HealthCheckServer::SendStatus(ACE_UINT32 currstate)
{
	DEBUG("%s","Entering  HealthCheckServer::SendStatus( ");
	// Description
	//	This routine reports service status. Used during startup and shutdown.

	HCSServerStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	HCSServerStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
		SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
	HCSServerStatus.dwCurrentState = currstate;

	HCSServerStatus.dwWin32ExitCode = 0;
	HCSServerStatus.dwServiceSpecificExitCode = 0;
	HCSServerStatus.dwCheckPoint = 0;
	HCSServerStatus.dwWaitHint = 1000;

	// Send status update to SCM
	SetServiceStatus(HCSServerStatusHandle, &HCSServerStatus);
	DEBUG("%s","Leaving  HealthCheckServer::SendStatus( ");
	return;
} // End of SendStatus

void WINAPI HealthCheckServer::ServerCtrlHandler(ACE_UINT32 opcode)
{
	DEBUG("%s","Entering  WINAPI HealthCheckServer::ServerCtrlHandler( ");
	// Description:
	//	This routine is called from the ServiceControlManager

	switch (opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_STOP:		// Continue to ShutDown
	case SERVICE_CONTROL_SHUTDOWN:
		SendStatus(SERVICE_STOP_PENDING);
		SetEvent(HCSStopEvent);
		DEBUG("%s","Leaving  WINAPI HealthCheckServer::ServerCtrlHandler( ");
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	} // End of switch
	DEBUG("%s","Leaving  WINAPI HealthCheckServer::ServerCtrlHandler( ");
	return;
} // End of HCSServerCtrlHandler

// get Service Name
char* HealthCheckServer::GetServiceName()
{
	DEBUG("%s","Leaving  HealthCheckServer::GetServiceName() ");
	return serviceName_;
}

// set Service Name
void HealthCheckServer::SetServiceName(char* serviceName)
{
	DEBUG("%s","Entering HealthCheckServer::SetServiceName( ");
	serviceName_ = serviceName;
	DEBUG("%s","Leaving HealthCheckServer::SetServiceName( ");
}

// get Service Display Name
char* HealthCheckServer::GetServiceDisplayName()
{
	DEBUG("%s","In  HealthCheckServer::GetServiceDisplayName() ");
	return serviceDisplayName_;
}

// set Service Display Name
void HealthCheckServer::SetServiceDisplayName(char* serviceDisplayName)
{
	DEBUG("%s","Entering HealthCheckServer::SetServiceDisplayName( ");
	serviceDisplayName_ = serviceDisplayName;
	DEBUG("%s","Leaving HealthCheckServer::SetServiceDisplayName( ");
}

// get execution mode
bool HealthCheckServer::GetDebugMode()
{
	DEBUG("%s","In HealthCheckServer::GetDebugMode() ");
	return debugMode_;
}

// set execution mode
void HealthCheckServer::SetDebugMode(bool debugMode)
{
	DEBUG("%s","Entering HealthCheckServer::SetDebugMode( ");
	debugMode_ = debugMode;
	DEBUG("%s","Leaving HealthCheckServer::SetDebugMode( ");
}
