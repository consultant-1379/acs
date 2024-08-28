//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%45FFFB800190.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%45FFFB800190.cm

//## begin module%45FFFB800190.cp preserve=no
//## end module%45FFFB800190.cp

//## Module: ACS_CS_ServiceBase%45FFFB800190; Package specification
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Source file: T:\ntacs\cs_cnz\cssrv_caa\src\ACS_CS_ServiceBase.h

#ifndef ACS_CS_ServiceBase_h
#define ACS_CS_ServiceBase_h 1

//## begin module%45FFFB800190.additionalIncludes preserve=no
//## end module%45FFFB800190.additionalIncludes

//## begin module%45FFFB800190.includes preserve=yes

#include <winsock2.h>
#include <windows.h>

//## end module%45FFFB800190.includes

//## begin module%45FFFB800190.declarations preserve=no
//## end module%45FFFB800190.declarations

//## begin module%45FFFB800190.additionalDeclarations preserve=yes

typedef unsigned int u_int;

//## end module%45FFFB800190.additionalDeclarations


//## begin ACS_CS_ServiceBase%45FFFAE403D2.preface preserve=yes
//## end ACS_CS_ServiceBase%45FFFAE403D2.preface

//## Class: ACS_CS_ServiceBase%45FFFAE403D2
//	This class implements an interface between a Windows
//	service and Windows service manager. An application that
//	should run as a service should inherit from this class
//	and implement the virtual functions.
//## Category: cssrv%458BCEF400CD
//## Subsystem: ACS_CS::cssrv_caa::src%459B6F040389
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class ACS_CS_ServiceBase 
{
  //## begin ACS_CS_ServiceBase%45FFFAE403D2.initialDeclarations preserve=yes
  //## end ACS_CS_ServiceBase%45FFFAE403D2.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: ACS_CS_ServiceBase%45FFFB0B028A
      ACS_CS_ServiceBase (const char* name, const char *displayName = 0, bool autostart = true, u_int acceptedControls = SERVICE_ACCEPT_STOP);

    //## Destructor (generated)
      virtual ~ACS_CS_ServiceBase();


    //## Other Operations (specified)
      //## Operation: run_service%45FFFB0B02A9
      int run_service (int argc, char** argv);

      //## Operation: open%45FFFB0B02B9
      virtual int open (int argc, char** argv) = 0;

      //## Operation: close%45FFFB0B02D8
      //	-----------------------------------------
      //	 Override the below functions when needed
      //	-----------------------------------------
      virtual int close (); // TR HL33186

      //## Operation: handle_install%45FFFB0B0345
      //	---------------------------------------------------------
      //	 Override the handle_install() and handle_remove()
      //	 when needed. Note that these are only called on Windows.
      //	---------------------------------------------------------
      bool handle_install ();

      //## Operation: handle_remove%45FFFB0B0355
      //	 Description:
      //	    Called when the service is installed. Override this
      //	    method to handle installations specific to Windows
      //	    that must be done at installation. Note that by
      //	returning
      //	    false, the installation of the service will be
      //	aborted.
      //	    Method handle_remove() will not be called in that
      //	case.
      //	 Parameters:
      //	    -
      //	 Return value:
      //	    -
      //	 Additional information:
      //	    Does not start the service.
      //	    Default: Does nothing, returns true.
      void handle_remove ();

      //## Operation: handle_shutdown%45FFFB0B0374
      //	 Description:
      //	    Called when the service is removed (de-installed).
      //	Override
      //	    this method to reverse the actions done when handle_
      //	install()
      //	    was called.
      //	 Parameters:
      //	    -
      //	 Return value:
      //	    -
      //	 Additional information:
      //	    Stops the service if it's running.
      //	    Default: Does nothing.
      //	    Option : remove.
      int handle_shutdown ();

      //## Operation: setState%45FFFB0B0394
      //	 Description:
      //	    Called when the system is being shutdown. This method
      //	    should normally not be implemented.
      //	 Parameters:
      //	    -
      //	 Return value:
      //	    -
      //	 Additional information:
      //	    Note that close() will NOT be called.
      //	    Default: Does nothing, returns 0.
      void setState (u_int state, u_int checkPoint = 0, u_int waitHint = 3000);

      //## Operation: getName%45FFFB0B03B3
      const char* getName ();

      //## Operation: instance%45FFFB0C0009
      ACS_CS_ServiceBase * instance ();

    // Additional Public Declarations
      //## begin ACS_CS_ServiceBase%45FFFAE403D2.public preserve=yes
      //## end ACS_CS_ServiceBase%45FFFAE403D2.public

  protected:

    //## Other Operations (specified)
      //## Operation: run_event_loop%45FFFB0C0086
      int run_event_loop ();

      //## Operation: end_event_loop%45FFFB0C00C5
      void end_event_loop ();

      //## Operation: InstallService%45FFFB0C0103
      int InstallService (bool autoStart, const char* pgmPath);

      //## Operation: RemoveService%45FFFB0C0132
      int RemoveService ();

      //## Operation: DebugService%45FFFB0C0151
      bool DebugService (int argc, char** argv);

      //## Operation: StartService%45FFFB0C0180
      int StartService ();

      //## Operation: findOption%45FFFB0C01AF
      int findOption (const char* str, char** argv, int startPos = 1);

    // Additional Protected Declarations
      //## begin ACS_CS_ServiceBase%45FFFAE403D2.protected preserve=yes
      //## end ACS_CS_ServiceBase%45FFFAE403D2.protected

  private:
    //## Constructors (generated)
      ACS_CS_ServiceBase(const ACS_CS_ServiceBase &right);

    //## Assignment Operation (generated)
      ACS_CS_ServiceBase & operator=(const ACS_CS_ServiceBase &right);


    //## Other Operations (specified)
      //## Operation: init_service%4600E1C301BC
      void init_service (const char *name, const char *displayName);

      //## Operation: ServiceMain%45FFFB0C025B
      static void WINAPI ServiceMain (DWORD argc, LPTSTR* argv);

      //## Operation: ServiceCtrlHandler%45FFFB0C028A
      static void WINAPI ServiceCtrlHandler (DWORD code);

      //## Operation: ConsoleHandler%45FFFB0C02B9
      static BOOL WINAPI ConsoleHandler (DWORD controlCode);

      //## Operation: checkDataDisk%4755649A00DE
      static bool checkDataDisk ();

    // Additional Private Declarations
      //## begin ACS_CS_ServiceBase%45FFFAE403D2.private preserve=yes
      //## end ACS_CS_ServiceBase%45FFFAE403D2.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: autoStart_%45FFFAF70113
      //	 Internal variables
      //## begin ACS_CS_ServiceBase::autoStart_%45FFFAF70113.attr preserve=no  private:  bool {U} autostart
       bool autoStart_;
      //## end ACS_CS_ServiceBase::autoStart_%45FFFAF70113.attr

      //## Attribute: argc_%45FFFAF70161
      //## begin ACS_CS_ServiceBase::argc_%45FFFAF70161.attr preserve=no  private:  int {U} 0
       int argc_;
      //## end ACS_CS_ServiceBase::argc_%45FFFAF70161.attr

      //## Attribute: argv_%45FFFAF70171
      //## begin ACS_CS_ServiceBase::argv_%45FFFAF70171.attr preserve=no  private:  char* * {U} 0
       char* *argv_;
      //## end ACS_CS_ServiceBase::argv_%45FFFAF70171.attr

      //## Attribute: name_%45FFFAF70190
      //## begin ACS_CS_ServiceBase::name_%45FFFAF70190.attr preserve=no  private:  char* {U} 0
       char* name_;
      //## end ACS_CS_ServiceBase::name_%45FFFAF70190.attr

      //## Attribute: displayName_%45FFFAF701BF
      //## begin ACS_CS_ServiceBase::displayName_%45FFFAF701BF.attr preserve=no  private:  char* {U} 0
       char* displayName_;
      //## end ACS_CS_ServiceBase::displayName_%45FFFAF701BF.attr

      //## Attribute: priority_%45FFFAF701DE
      //## begin ACS_CS_ServiceBase::priority_%45FFFAF701DE.attr preserve=no  private:  char* {U} 0
       char* priority_;
      //## end ACS_CS_ServiceBase::priority_%45FFFAF701DE.attr

      //## Attribute: debugMode_%45FFFAF701FD
      //## begin ACS_CS_ServiceBase::debugMode_%45FFFAF701FD.attr preserve=no  private:  bool {U} false
       bool debugMode_;
      //## end ACS_CS_ServiceBase::debugMode_%45FFFAF701FD.attr

      //## Attribute: acceptedCtrls_%4600DA200353
      //## begin ACS_CS_ServiceBase::acceptedCtrls_%4600DA200353.attr preserve=no  private:  u_int {U} acceptedControls
       u_int acceptedCtrls_;
      //## end ACS_CS_ServiceBase::acceptedCtrls_%4600DA200353.attr

      //## Attribute: serverState_%4600DA2C019D
      //## begin ACS_CS_ServiceBase::serverState_%4600DA2C019D.attr preserve=no  private:  u_int {U} 0
       u_int serverState_;
      //## end ACS_CS_ServiceBase::serverState_%4600DA2C019D.attr

      //## Attribute: currCheckPoint_%4600DA3502E5
      //## begin ACS_CS_ServiceBase::currCheckPoint_%4600DA3502E5.attr preserve=no  private:  u_int {U} 0
       u_int currCheckPoint_;
      //## end ACS_CS_ServiceBase::currCheckPoint_%4600DA3502E5.attr

      //## Attribute: loopHandle%4600FC88020A
      //## begin ACS_CS_ServiceBase::loopHandle%4600FC88020A.attr preserve=no  private:  HANDLE {U} 0
       HANDLE loopHandle;
      //## end ACS_CS_ServiceBase::loopHandle%4600FC88020A.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_ServiceBase%45FFFAE403D2.implementation preserve=yes
       char traStr_[150];
      //## end ACS_CS_ServiceBase%45FFFAE403D2.implementation
};

//## begin ACS_CS_ServiceBase%45FFFAE403D2.postscript preserve=yes
//## end ACS_CS_ServiceBase%45FFFAE403D2.postscript

// Class ACS_CS_ServiceBase 

//## begin module%45FFFB800190.epilog preserve=yes
//## end module%45FFFB800190.epilog


#endif
