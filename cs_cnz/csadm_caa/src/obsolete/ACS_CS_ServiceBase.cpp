//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%45FFFB8201FD.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%45FFFB8201FD.cm

//## begin module%45FFFB8201FD.cp preserve=no
//## end module%45FFFB8201FD.cp

//## Module: ACS_CS_ServiceBase%45FFFB8201FD; Package body
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Source file: Z:\ntacs\cs_cnz\cssrv_caa\src\ACS_CS_ServiceBase.cpp

//## begin module%45FFFB8201FD.additionalIncludes preserve=no
//## end module%45FFFB8201FD.additionalIncludes

//## begin module%45FFFB8201FD.includes preserve=yes

#include <fstream>
#include <string>
#include <TCHAR.H>
#include "ACS_PRC_Process.H"
#include "ACS_ExceptionHandler.H"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_DHCPHandler.h"

//## end module%45FFFB8201FD.includes

// ACS_CS_ServiceBase
#include "ACS_CS_ServiceBase.h"
//## begin module%45FFFB8201FD.declarations preserve=no
//## end module%45FFFB8201FD.declarations

//## begin module%45FFFB8201FD.additionalDeclarations preserve=yes

static ACS_CS_ServiceBase * theService = 0;
SERVICE_STATUS_HANDLE serviceStatusHandleR1 = 0;

using std::string;
using namespace ACS_CS_Protocol;

#include "CommonDLL.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ServiceBase_TRACE);

//## end module%45FFFB8201FD.additionalDeclarations


// Class ACS_CS_ServiceBase 

ACS_CS_ServiceBase::ACS_CS_ServiceBase (const char* name, const char *displayName, bool autostart, u_int acceptedControls)
  //## begin ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.hasinit preserve=no
      : autoStart_(autostart),
        argc_(0),
        argv_(0),
        name_(0),
        displayName_(0),
        priority_(0),
        debugMode_(false),
        acceptedCtrls_(acceptedControls),
        serverState_(0),
        currCheckPoint_(0),
        loopHandle(0)
  //## end ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.hasinit
  //## begin ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.initialization preserve=yes
  //## end ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.initialization
{
  //## begin ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.body preserve=yes

   loopHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

   this->init_service(name, displayName);

  //## end ACS_CS_ServiceBase::ACS_CS_ServiceBase%45FFFB0B028A.body
}


ACS_CS_ServiceBase::~ACS_CS_ServiceBase()
{
  //## begin ACS_CS_ServiceBase::~ACS_CS_ServiceBase%45FFFAE403D2_dest.body preserve=yes
   
   if (name_)
      delete [] name_;

   if (displayName_)
      delete [] displayName_;

   if (loopHandle)
      (void) CloseHandle(loopHandle);
/*
   if ( this->delete_secattr_at_exit_ ) {
      delete this->secattr_->lpSecurityDescriptor;
      delete this->secattr_;
   }
*/
  //## end ACS_CS_ServiceBase::~ACS_CS_ServiceBase%45FFFAE403D2_dest.body
}



//## Other Operations (implementation)
 int ACS_CS_ServiceBase::run_service (int argc, char** argv)
{
  //## begin ACS_CS_ServiceBase::run_service%45FFFB0B02A9.body preserve=yes

   int res = 0;
   char pgmPath[1024];
   int optionPos = 0;

   argc_ = argc;
   argv_ = argv;

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::run_service()\n"
         "Creating service %s\n", this->getName()));

   if (argc_ > 1) {

      if ( findOption("install",argv_) > 0 ) {

         (void) ::GetModuleFileName(NULL, pgmPath, sizeof(pgmPath)-1 );

         // Make sure that the path is enclosed
         // with qutation marks to allow spaces
         // in the path name. Only valid for Windows (?)
         string tmpPath;
         tmpPath = "\"";
         tmpPath += pgmPath;
         tmpPath += "\"";

         // Install the service
         if ( this->InstallService(autoStart_,tmpPath.c_str()) < 0 )
            return -1;

         // Call user's install method
         if ( !this->handle_install() ) {
            (void) this->RemoveService();
            return -1;
         }

         return 0;

      }
      else if ( findOption("remove",argv_) > 0 ) {

         res = this->RemoveService();

         this->handle_remove();

         return res;

      }
      else if ( (optionPos=findOption("debug",argv_)) > 0 ) {

         debugMode_ = true;
      }
   }
   else {
      
      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::run_service()\n"
         "Error: Unknown option\n"));

   }

   if ( debugMode_ ) {

      char ** argv_temp = new char*[argc_];
      
      // Create a new argv vector; 
      string tmp;
      int oi = 0;
      while (oi < argc_) {
         if (oi != optionPos) {
            if ( oi == 0 ) {
               tmp = "\"";
               tmp+= argv_[oi];
               tmp+= "\"";

               size_t length = tmp.length();
               char * currentOption = 0;
               
               if (length > 0)
                  currentOption = new char[length + 1];

               if (currentOption)
               {
                  strncpy(currentOption, tmp.c_str(), length);
                  currentOption[oi] = 0;
               }
                  
               argv_temp[oi] = currentOption;
            }
            else
            {
               tmp = argv_[oi];
               size_t length = tmp.length();
               char * currentOption = 0;
               
               if (length > 0)
                  currentOption = new char[length + 1];

               if (currentOption)
               {
                  strncpy(currentOption, tmp.c_str(), length);
                  currentOption[oi] = 0;
               }

               argv_temp[oi] = currentOption;
            }
         }
         else
            argc--;

         oi++;
      }

      res = this->DebugService(argc,argv_temp);

      for (int i = 0; i < argc_; i++)
      {
         if (argv_temp[i] != 0)
            delete [] argv_temp[i];
      }

      delete [] argv_temp;

      return res;
      /*NOTREACHED*/

   }

   // Register the service to the SCM

   return this->StartService();

  //## end ACS_CS_ServiceBase::run_service%45FFFB0B02A9.body
}


 int ACS_CS_ServiceBase::close ()
{
  //## begin ACS_CS_ServiceBase::close%45FFFB0B02D8.body preserve=yes

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		"(%t) ACS_CS_ServiceBase::close()\n"
      "%s: close(); Not implemented by user\n", this->getName()));
  
   return 0;

  //## end ACS_CS_ServiceBase::close%45FFFB0B02D8.body
}


 bool ACS_CS_ServiceBase::handle_install ()
{
  //## begin ACS_CS_ServiceBase::handle_install%45FFFB0B0345.body preserve=yes

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		"(%t) ACS_CS_ServiceBase::handle_install()\n"
      "%s: handle_install(); Not implemented by user\n", this->getName()));

   return true;

  //## end ACS_CS_ServiceBase::handle_install%45FFFB0B0345.body
}

 void ACS_CS_ServiceBase::handle_remove ()
{
  //## begin ACS_CS_ServiceBase::handle_remove%45FFFB0B0355.body preserve=yes

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		"(%t) ACS_CS_ServiceBase::handle_remove()\n"
      "%s: handle_remove(); Not implemented by user\n", this->getName()));

  //## end ACS_CS_ServiceBase::handle_remove%45FFFB0B0355.body
}

 int ACS_CS_ServiceBase::handle_shutdown ()
{
  //## begin ACS_CS_ServiceBase::handle_shutdown%45FFFB0B0374.body preserve=yes

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		"(%t) ACS_CS_ServiceBase::handle_shutdown()\n"
      "%s: handle_shutdown(); Not implemented by user\n", this->getName()));

   return 0;

  //## end ACS_CS_ServiceBase::handle_shutdown%45FFFB0B0374.body
}

 void ACS_CS_ServiceBase::setState (u_int state, u_int checkPoint, u_int waitHint)
{
  //## begin ACS_CS_ServiceBase::setState%45FFFB0B0394.body preserve=yes

   SERVICE_STATUS serviceStatus;

   // Fill in all of the SERVICE_STATUS fields

   serviceStatus.dwCurrentState        = (DWORD) state;
   serviceStatus.dwWaitHint            = (DWORD) waitHint;
   serviceStatus.dwWin32ExitCode       = NO_ERROR;
   serviceStatus.dwServiceType            = SERVICE_WIN32_OWN_PROCESS;
   serviceStatus.dwServiceSpecificExitCode = 0;

   // If in the process of something, then accept
   // no control events, else accept stop and shutdown
   if ( state == SERVICE_START_PENDING )
      serviceStatus.dwControlsAccepted = 0;
   else
      serviceStatus.dwControlsAccepted = acceptedCtrls_;


   // Define the check point value

   if ( checkPoint == 0 ) {

      if ( state == SERVICE_START_PENDING || state == SERVICE_STOP_PENDING )
         currCheckPoint_++;
      else
         currCheckPoint_ = 0;

   }
   else
      currCheckPoint_ = checkPoint;

   serviceStatus.dwCheckPoint = (DWORD) currCheckPoint_;


   // Pass the status record to the SCM
   BOOL success = TRUE;
   
   if (serviceStatusHandleR1)
      success = SetServiceStatus (serviceStatusHandleR1, &serviceStatus);

   if ( !success ) {

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::setState()\n"
         "ERROR: setState() failed with %i\n", GetLastError()));

      // @@ Should this be done?
      this->end_event_loop();
   }

  //## end ACS_CS_ServiceBase::setState%45FFFB0B0394.body
}

const char * ACS_CS_ServiceBase::getName ()
{
  //## begin ACS_CS_ServiceBase::getName%45FFFB0B03B3.body preserve=yes

   return name_;

  //## end ACS_CS_ServiceBase::getName%45FFFB0B03B3.body
}

 ACS_CS_ServiceBase * ACS_CS_ServiceBase::instance ()
{
  //## begin ACS_CS_ServiceBase::instance%45FFFB0C0009.body preserve=yes

   return theService;

  //## end ACS_CS_ServiceBase::instance%45FFFB0C0009.body
}

 int ACS_CS_ServiceBase::run_event_loop ()
{
  //## begin ACS_CS_ServiceBase::run_event_loop%45FFFB0C0086.body preserve=yes

   (void) WaitForSingleObject(loopHandle, INFINITE);

   return 0;

  //## end ACS_CS_ServiceBase::run_event_loop%45FFFB0C0086.body
}

 void ACS_CS_ServiceBase::end_event_loop ()
{
  //## begin ACS_CS_ServiceBase::end_event_loop%45FFFB0C00C5.body preserve=yes

   if (SetEvent(loopHandle) == 0)
   {
      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::end_event_loop()\n"
         "ERROR: SetEvent(loopHandle) failed with %i\n", GetLastError()));
   }

  //## end ACS_CS_ServiceBase::end_event_loop%45FFFB0C00C5.body
}

 int ACS_CS_ServiceBase::InstallService (bool autoStart, const char* pgmPath)
{
  //## begin ACS_CS_ServiceBase::InstallService%45FFFB0C0103.body preserve=yes

   SC_HANDLE hService;
   SC_HANDLE hScm;

   // connect to the SCM
   hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
   if (!hScm)
      return -1;

   DWORD startMode = SERVICE_DEMAND_START;
   if (autoStart)
      startMode = SERVICE_AUTO_START;

   // install our service
   hService = ::CreateService(hScm,
                              name_,                     // service name
                              displayName_,              // display name
                              SERVICE_ALL_ACCESS,        // desired access
                              SERVICE_WIN32_OWN_PROCESS, // type of service
                              startMode,                 // when to start service
                              SERVICE_ERROR_IGNORE,      // severity if service fails to start
                              pgmPath,                   // path to executable file
                              NULL,                      // load ordering group
                              NULL,                      // tag identifier
                              NULL,                      // array of dependency names
                              NULL,                      // account name
                              NULL );                    // account password

   if (!hService) {
      (void) ::CloseServiceHandle(hScm);

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::InstallService()\n"
         "%s: InstallService(); ERROR: CreateService() failed with err=%i\n", this->getName(), GetLastError()));

      return -1;
      /*NOTREACHED*/
   }


   // Service has been installed

   (void) ::CloseServiceHandle(hService);
   (void) ::CloseServiceHandle(hScm);

   return 0;

  //## end ACS_CS_ServiceBase::InstallService%45FFFB0C0103.body
}

 int ACS_CS_ServiceBase::RemoveService ()
{
  //## begin ACS_CS_ServiceBase::RemoveService%45FFFB0C0132.body preserve=yes

   bool removed = true;

   SC_HANDLE hScm;
   SC_HANDLE hService;

   // Connect to the SCM
   hScm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
   if (!hScm)
      return -1;


   // Connect to the service
   hService = ::OpenService(hScm, this->name_, SERVICE_ALL_ACCESS | DELETE);
   if (!hService) {
      (void) ::CloseServiceHandle(hScm);

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::RemoveService()\n"
         "%s: RemoveService(); ERROR: OpenService() failed with err=%i\n", this->getName(), GetLastError()));

      return -1;
   }

   // Stop the service if not already stopped

   SERVICE_STATUS status;
   BOOL success;

   // Query the service status
   success = ::QueryServiceStatus(hService, &status);

   if ( success && status.dwCurrentState != SERVICE_STOPPED ) {

      success = ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
      if ( success ) {

         ::Sleep(1000);

         // Wait for the service to shutdown

         while ( ::QueryServiceStatus(hService, &status) ) {

            if ( status.dwCurrentState == SERVICE_STOP_PENDING )
               ::Sleep(1000);
            else
               break;
         }

         if ( status.dwCurrentState != SERVICE_STOPPED ) {
            (void) ::CloseServiceHandle(hScm);
            (void) ::CloseServiceHandle(hService);

            ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			      "(%t) ACS_CS_ServiceBase::RemoveService()\n"
               "%s: RemoveService(); ERROR: Failed to stop service\n", this->getName()));

            return -1;
            /*NOTREACHED*/
         }
      }
   }

   // Now delete the service

   if ( DeleteService(hService) )
      removed = true;
   else
      removed = false;

   (void) ::CloseServiceHandle(hService);
   (void) ::CloseServiceHandle(hScm);

   if (removed)
      return 0;
   else
      return -1;

  //## end ACS_CS_ServiceBase::RemoveService%45FFFB0C0132.body
}

 bool ACS_CS_ServiceBase::DebugService (int argc, char** argv)
{
  //## begin ACS_CS_ServiceBase::DebugService%45FFFB0C0151.body preserve=yes

   // Make sure to catch Ctrl-C
   (void) SetConsoleCtrlHandler(ConsoleHandler, TRUE);

   // Initiate user's service
   //-------------------------

   if ( this->open(argc,argv) < 0 ) {
      
      goto ErrorExit;
      //NOTREACHED
   }


   // Run global Reactor event loop
   //-------------------------------

   (void) this->run_event_loop();

   //*** Does not come here until end_event_loop() is called ***

   // Set service state
   this->setState( SERVICE_STOPPED );

   return true;


ErrorExit:

   return false;

  //## end ACS_CS_ServiceBase::DebugService%45FFFB0C0151.body
}

 int ACS_CS_ServiceBase::StartService ()
{
  //## begin ACS_CS_ServiceBase::StartService%45FFFB0C0180.body preserve=yes

   SERVICE_TABLE_ENTRY serviceTable[] = 
   { 
      { name_, (LPSERVICE_MAIN_FUNCTION) ACS_CS_ServiceBase::ServiceMain},
      { NULL, NULL }
   };


   // Register with the SCM
   // Does not return until Stopped!

   if ( StartServiceCtrlDispatcher(serviceTable) )
      return 0;
   else {

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::StartService()\n"
         "%s StartService(); ERROR: StartServiceCtrlDispatcher() failed with %i\n", this->getName(), GetLastError()));

      return -1;
   }
   
  //## end ACS_CS_ServiceBase::StartService%45FFFB0C0180.body
}

 int ACS_CS_ServiceBase::findOption (const char* str, char** argv, int startPos)
{
  //## begin ACS_CS_ServiceBase::findOption%45FFFB0C01AF.body preserve=yes

   string optionString = str;
   int pos = 0;
   
   for (int i=startPos; i<argc_; i++) {
      string currentOption = argv[i];
      if ( optionString == currentOption ) {
         pos = i;
         break;
      }
   }
   
   return pos;

  //## end ACS_CS_ServiceBase::findOption%45FFFB0C01AF.body
}

 void ACS_CS_ServiceBase::init_service (const char *name, const char *displayName)
{
  //## begin ACS_CS_ServiceBase::init_service%4600E1C301BC.body preserve=yes

   // Set the global pointer
   theService = this;
   char defaultName[] = "ACS_CS_ServiceBase";

   if ( name )
   {
      size_t length = strlen(name);

      if (length > 0)
         name_ = new char[length + 1];

      if (name_)
      {
         strncpy(name_, name, length);
         name_[length] = 0;
      }
   }
   else
   {
      size_t length = strlen(defaultName);

      if (length > 0)
         name_ = new char[length + 1];

      if (name_)
      {
         strncpy(name_, defaultName, length);
         name_[length] = 0;
      }
   }

   if ( displayName )
   {
      size_t length = strlen(displayName);

      if (length > 0)
         displayName_ = new char[length + 1];

      if (displayName_)
      {
         strncpy(displayName_, displayName, length);
         displayName_[length] = 0;
      }
   }
   else
   {
      if (name_)
      {
         size_t length = strlen(name_);

         if (length > 0)
            displayName_ = new char[length + 1];

         if (displayName_)
         {
            strncpy(displayName_, name_, length);
            displayName_[length] = 0;
         }
      }
   }

  //## end ACS_CS_ServiceBase::init_service%4600E1C301BC.body
}

 void WINAPI ACS_CS_ServiceBase::ServiceMain (DWORD argc, LPTSTR* argv)
{
  //## begin ACS_CS_ServiceBase::ServiceMain%45FFFB0C025B.body preserve=yes

   // Immediately call Registration function
   serviceStatusHandleR1 = ::RegisterServiceCtrlHandler(theService->name_,
                                        (LPHANDLER_FUNCTION) ServiceCtrlHandler);
   if (!serviceStatusHandleR1) {

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::ServiceMain()\n"
         "ServiceMain(); ERROR: RegisterServiceCtrlHandler() failed with %i\n", GetLastError()));

      //goto ServiceStop;
			theService->setState( SERVICE_STOPPED );  // TR HL37682   
			return;
      /*NOTREACHED*/
   }


   // Notify SCM of progress; increasing step count
   theService->setState( SERVICE_START_PENDING );

   // Set service priority
   AP_InitProcess(theService->name_,AP_SERVICE);

   // Set default cleanup (crash) routine
   AP_SetCleanupAndCrashRoutine(theService->name_,NULL);

   // Notify SCM of progress; increasing step count
   theService->setState( SERVICE_START_PENDING );

   if (CS_ProtocolChecker::getServiceType() == ACS_CS_Protocol::ServiceType_MASTER) {
	   int retry = 0;
	   for (retry = 0; !checkDataDisk() ; retry++) { //Wait for the data disk to become available
		   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			   "(%t) ACS_CS_ServiceBase::ServiceMain()\n"
			   "ServiceMain(); WARNING: checkDataDisk() failed\n"));
		   if (retry%120 == 0) { //report event every minute
			   ACS_CS_EVENT(Event_StartupFailure, "Check resources", "data disk is not available", "");
			   ACS_CS_EventReporter::reset();
		   }
		   Sleep(500);
	   }
   }
   
   // Call user's start routine
   //---------------------------

   if ( theService->open(argc,argv) < 0 ) {

      ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::ServiceMain()\n"
         "ServiceMain(); ERROR: User service->open() failed; Shutting down\n"));
      
      //goto ServiceStop;
      theService->setState( SERVICE_STOPPED );  // TR HL37682
			return;
      //NOTREACHED
   }


   // The service is now running. 
   // Notify SCM of progress
   theService->setState( SERVICE_RUNNING );


   // Run the event loop
   (void) theService->run_event_loop();

   /*
   N.B. Will not come here as long as the
        service is alive and running.
   */

   ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		"(%t) ACS_CS_ServiceBase::ServiceMain()\n"
      "ServiceMain(); Closing service...\n"));

   // Set service state
   theService->setState( SERVICE_STOPPED );

   return;


//ServiceStop:     // TR HL37682

   // Stop service
   //theService->setState( SERVICE_STOPPED );

  //## end ACS_CS_ServiceBase::ServiceMain%45FFFB0C025B.body
}

 void WINAPI ACS_CS_ServiceBase::ServiceCtrlHandler (DWORD code)
{
  //## begin ACS_CS_ServiceBase::ServiceCtrlHandler%45FFFB0C028A.body preserve=yes

   DWORD currentState = SERVICE_RUNNING;

   switch(code)
   {
      // There is no START option because
      // ServiceMain gets called on at start

      case SERVICE_CONTROL_STOP:

         ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		      "(%t) ACS_CS_ServiceBase::ServiceCtrlHandler()\n"
            "ServiceCtrlHandler(); Received SERVICE_CONTROL_STOP\n"));

         // Tell the SCM what's happening
         theService->setState(SERVICE_STOP_PENDING);

         // Call user's close routine
         (void) theService->close();

         // Stop the service
         theService->end_event_loop();

         return;
         /*NOTREACHED*/


      case SERVICE_CONTROL_SHUTDOWN:
         // Do nothing in a shutdown. Could do cleanup
         // here but it must be very quick.

         ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
		      "(%t) ACS_CS_ServiceBase::ServiceCtrlHandler()\n"
            "ServiceCtrlHandler(); Received SERVICE_CONTROL_SHUTDOWN\n"));

         // Tell the SCM what's happening
         theService->setState(SERVICE_STOP_PENDING); // Needed??

         // Call user's close routine
         (void) theService->handle_shutdown();

         // Stop the service
         (void) theService->end_event_loop();

         return;
         /*NOTREACHED*/


      case SERVICE_CONTROL_INTERROGATE:
         // Update current status
         // it will fall to bottom and send status
         break;


      case SERVICE_CONTROL_PAUSE:
      case SERVICE_CONTROL_CONTINUE:
      default:
         break;
   }

   // Report current state

   theService->setState(currentState);

  //## end ACS_CS_ServiceBase::ServiceCtrlHandler%45FFFB0C028A.body
}

 BOOL WINAPI ACS_CS_ServiceBase::ConsoleHandler (DWORD controlCode)
{
  //## begin ACS_CS_ServiceBase::ConsoleHandler%45FFFB0C02B9.body preserve=yes

   switch (controlCode)
   {
   case CTRL_C_EVENT:
   case CTRL_BREAK_EVENT:

      
      theService->setState(SERVICE_STOP_PENDING);
      (void) theService->close();
      theService->end_event_loop();

      return TRUE;
      /*NOTREACHED*/

   default:
      break;
   }

   return FALSE;

  //## end ACS_CS_ServiceBase::ConsoleHandler%45FFFB0C02B9.body
}

 bool ACS_CS_ServiceBase::checkDataDisk ()
{
  //## begin ACS_CS_ServiceBase::checkDataDisk%4755649A00DE.body preserve=yes
	TCHAR buf[100] = {0};
	bool driveReady = false;
	char dataDiskDrive[MAX_PATH] = {0};
	DWORD dwlen = MAX_PATH;
	char szPath[MAX_PATH] = {0};

	//Get data disk drive by commondll
	char * szLogName = "ACS_DATA$";
	int path = GetDataDiskPath(szLogName,szPath,&dwlen);
	if (path < 0) {
		ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::ServiceMain()\n"
			"ServiceMain(); ERROR: GetDataDiskPath() failed\n"));
		return false;
	}
	
	std::string tmpPath = szPath;
	size_t idx = tmpPath.find_first_of("\\");
	if (idx == ~0U) {
		ACS_CS_TRACE((ACS_CS_ServiceBase_TRACE,
			"(%t) ACS_CS_ServiceBase::ServiceMain()\n"
			"ServiceMain(); ERROR: cannot read data disk drive\n"));
		return false;
	}
	
	size_t driveLen = ( (idx + 1) < sizeof(dataDiskDrive) ) ? (idx + 1) : sizeof(dataDiskDrive);
	_snprintf(dataDiskDrive, driveLen, "%s", szPath);
	
	//Look for data disk drive
	if (GetLogicalDriveStrings(sizeof(buf)/sizeof(TCHAR), buf) != 0) {
		for (TCHAR * s = buf; *s; s += _tcslen(s)+1) {
			LPCTSTR sDrivePath = s;
			if (_tcscmp(sDrivePath, dataDiskDrive) == 0) {
				driveReady = true;
				break;
			}
		}
	}

	return driveReady;
  //## end ACS_CS_ServiceBase::checkDataDisk%4755649A00DE.body
}

// Additional Declarations
  //## begin ACS_CS_ServiceBase%45FFFAE403D2.declarations preserve=yes
  //## end ACS_CS_ServiceBase%45FFFAE403D2.declarations

//## begin module%45FFFB8201FD.epilog preserve=yes
//## end module%45FFFB8201FD.epilog
