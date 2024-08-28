/*
 * ACS_APGCC_Daemon.h
 *
 *  Created on: Apr 3, 2011
 *      Author: echrohi
 */

#ifndef ACS_APGCC_DAEMON_H_
#define ACS_APGCC_DAEMON_H_

#ifndef ACS_APGCC_COMMAND_H_
#  include "ACS_APGCC_Command.H"
#endif

#include <ace/Task.h>

class ACS_APGCC_CmdServer;
class SessionObj_Impl;

class ACS_APGCC_Daemon : public ACE_Task<ACE_SYNCH>
{
public:
   ACS_APGCC_Daemon(const char* name);
   ACS_APGCC_Daemon(){};
   virtual ~ACS_APGCC_Daemon();


   // Session object used in handle_session().
   // This object is used for session based commands,
   // i.e. commands that extends over multiple recv/send calls.
   // The user must first call recv() to receive the initial
   // command object.
   // The session is ended and the session object released
   // by calling destroy().
   //-----------------------------------------------
   class  SessionObj
   {
      public:
         friend class ACS_APGCC_CmdServer;

         SessionObj();
         // Initiation.

         virtual ~SessionObj();
         // Termination.

         bool send(ACS_APGCC_Command& cmd);
           // Send a command object to remote client.
         // Returns true if successful.

         bool recv(ACS_APGCC_Command& cmd);
         // Receive a command object from the remote client.
         // Returns true if successful.

         void destroy();
         // Close session and destroy (free) session object.
         // Warning:
         // After this method has been called, the object no longer
         // exists!

      private:
         SessionObj_Impl* impl_;
      };

   virtual int handle_session(SessionObj* cmd);
     // Description:
     //    Called when a new session has been started.
     //    Implements the support for session based commands,
     //    i.e. commands that expands over multiple recv/send calls.
     //    See also the class SessionObj above.
     // Parameters:
     //    -
     // Return value:
     //    0        The session was accepted by the user, and is
     //             not further handled by the MCS_Service.
     //             It is then up to the user to destroy the session
     //             object by calling method destroy().
     //
     //    -1       The session was NOT accepted. The session object will be
     //             destroyed by the MCS_Service.
     //
     // Additional information:
     //    The flag useSessionObj must be set to true in the constructor
     //    of this object for this method to be called.
     //    Important:
     //    This method is used instead of the method handle_command().
     //    They are mutally exclusion, i.e. they cannot be used
     //    together.

   const char* name();
     // Description:
     //    Returns the name of this service.
     // Parameters:
     //    -
     // Return value:
     //    The name given in the constructor.
     // Additional information:
     //    -

     const char* domain();
     // Description:
     //    Returns the domain name used for this service.
     // Parameters:
     //    -
     // Return value:
     //    The domain given in the constructor, or the default "MAS".
     // Additional information:
     //    -

     virtual int open(int argc, char* argv[]) = 0;
     // Description:
     //    Called when the service is being started.
     //    The service should register own event handlers to the
     //    Reactor, or start own threads here.
     // Parameters:
     //    -
     // Return value:
     //    -
     // Additional information:
     //    Called prior to run_event_loop().
     //    MUST be overridden.


   virtual int run_service(int argc, char* argv[]);

   static ACS_APGCC_Daemon* instance();

   int  StartService();

   ACE_Reactor* reactor();
      // Description:
      //    Access routine to access the used Reactor.
      //    By default returns the global OSF_Reactor.
      // Parameters:
      //    -
      // Return value:
      //    Current used reactor.
      // Additional information:
      //    It is also possible to access the global OSF_Reactor directly
      //    with OSF_Reactor::instance().

   int argc_;
   char** argv_;
   char* name_;
   char* domain_;
   ACE_Reactor* reactor_;
   ACS_APGCC_CmdServer* cmdServer_;
};

#endif /* ACS_APGCC_DAEMON_H_ */
