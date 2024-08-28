//	© Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Service_h
#define ACS_CS_Service_h 1


// ACS_CS_ServiceBase
#include "ACS_CS_ServiceBase.h"

class ACS_CS_ServiceHandler;



class ACS_CS_Service : public ACS_CS_ServiceBase
{

  public:
      ACS_CS_Service();

      virtual ~ACS_CS_Service();


      int open (int arg1, char **arg2);

      int close ();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_Service(const ACS_CS_Service &right);

      ACS_CS_Service & operator=(const ACS_CS_Service &right);

    // Additional Private Declarations

    // Data Members for Class Attributes

      ACS_CS_ServiceHandler* serviceHandler;

       HANDLE syncReplicationEvent;

       HANDLE hwcReplicationEvent;

       HANDLE cpIdReplicationEvent;

       HANDLE cpGroupReplicationEvent;

       HANDLE fdReplicationEvent;

       HANDLE neReplicationEvent;

       HANDLE vlanReplicationEvent;

       HANDLE shutdownAllEvent;

       HANDLE shutdownHandlerEvent;

    // Additional Implementation Declarations

};





#endif
