/*
 * ACS_APGCC_Daemon.cpp
 *
 *  Created on: Apr 3, 2011
 *      Author: echrohi
 */

#include "ACS_APGCC_Daemon.H"
#include "ACS_APGCC_DSD.H"
#include <ace/Reactor.h>
#include "ACS_APGCC_Util.H"
#include "ACS_TRA_Logging.h"


static ACS_APGCC_Daemon* theService = 0;
ACS_TRA_Logging ACS_APGCC_Daemon_logging;

#define REGISTER_PEER_HANDLER(fd,hndl,mask)  service_->reactor()->register_handler(fd,hndl,mask)

//===================================================
//    Class SessionObj and SessionObj_Impl
//    Implements the support for session based commands,
//    i.e. commands that expands over multiple
//    recv/send calls.
//===================================================
class SessionObj_Impl
{
public:
   // Initiation.
   SessionObj_Impl()
   {
   }

   // Termination.
   virtual ~SessionObj_Impl()
   {
      // Close peer stream
      this->peer().close();
   }

   // Receive command object
   bool recv(ACS_APGCC_Command& cmd)
   {
      if ( cmd.recv(this->peer()) < 0 ) {
    	  return false;
      }
      return true;
   }

   // Send command object
   bool send(ACS_APGCC_Command& cmd)
   {
      if ( cmd.send(this->peer()) < 0 ) {
         return false;
      }

      return true;
   }

   // Get underlaying communication stream
   ACS_APGCC_DSD_Stream& peer()
   {
      return peer_;
   }

private:
   ACS_APGCC_DSD_Stream peer_;
};

//
// Class SessionObj
//------------------
//

// Initiation.
ACS_APGCC_Daemon::SessionObj::SessionObj()
{
   impl_ = new SessionObj_Impl;
}


// Termination.
ACS_APGCC_Daemon::SessionObj::~SessionObj()
{
   if (impl_)
      delete impl_;
}


// Send a command object to remote client.
bool ACS_APGCC_Daemon::SessionObj::send(ACS_APGCC_Command& cmd)
{
	return impl_->send(cmd);
	return true;
}


// Receive a command object from the remote client.
bool ACS_APGCC_Daemon::SessionObj::recv(ACS_APGCC_Command& cmd)
{
   return impl_->recv(cmd);
}


// Close session and destroy session object.
void ACS_APGCC_Daemon::SessionObj::destroy()
{
   if (impl_)
      delete impl_;

   impl_ = 0;

   // Commit suicide
   delete this;
}

class ACS_APGCC_CmdServer: public ACE_Task<ACE_SYNCH>
{
public:

   ACS_APGCC_CmdServer(ACS_APGCC_Daemon* service);
   virtual ~ACS_APGCC_CmdServer();

   virtual int open(const char* name, const char* domain);
   virtual int close();

   // user methods
   virtual int handle_input(ACE_HANDLE);
   virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);
//   virtual int handle_signal(int signo, siginfo_t* s=0, ucontext_t* u=0);
   virtual int handle_timeout(const ACE_Time_Value& tv, const void*);

   int accept_session();
   // Accept a session based command from remote peer

protected:

   void start_timer(int seconds);
   void stop_timer();

protected:

   ACS_APGCC_DSD_Acceptor server_;
   // DSD server object

   int numOfFds_;
   // Number of active handles

   int timer_;
   // Holds timer id

   bool sessionBased_;
   // If the service accepts session based streams, instead
   // of the normal command based streams.
   // This controls the type of command object to create

   ACS_APGCC_DSD_Stream stream_;
   // Temporary stream object

   ACS_APGCC_Daemon* service_;
   // Pointer to the service routine

   ACE_HANDLE fds_[10];
   // Stores DSD handles

   string name_;
   // Name of service address

   string domain_;
   // Domain name of service address
};

//===================================================
//   Class ACS_APGCC_CmdServer
//   Implements the server that listen on a
//   DSD socket for commands.
//===================================================

ACS_APGCC_CmdServer::ACS_APGCC_CmdServer(ACS_APGCC_Daemon* service)
{
	ACS_APGCC_Daemon_logging.Open("APGCC");
	service_=service;
	timer_=-1;
	numOfFds_=0;
	for (int i=0; i<10; i++)
      fds_[i] = ACE_INVALID_HANDLE;
}


ACS_APGCC_CmdServer::~ACS_APGCC_CmdServer()
{
   //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::~ACS_APGCC_CmdServer(); dtor called\n"));
	ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::~ACS_APGCC_CmdServer(); dtor called",LOG_LEVEL_DEBUG);
	ACS_APGCC_Daemon_logging.Close();
}

// Define DSD error and error string
const int ERR_DSD_protErrCode           = 9803;
const char* const ERR_DSD_appAlreadyReg = "Application already registered";

int ACS_APGCC_CmdServer::open(const char* name, const char* domain)
{
	//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::open() called...\n"));
	ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::open() called...",LOG_LEVEL_DEBUG);
//   if (ACS_TRA_ON(traCS)) {
//      sprintf(traStr,"open(); name=%s, domain=%s",(name?name:""),(domain?domain:""));
//      ACS_TRA_event(&traCS, traStr);
//   }

   bool started;

   // Note that we can be come here for two reasons:
   //   1, the service is starting up
   //   2, re-connection attempts are being made
   //
   // We must therefore check the string values, as
   // in the second case, these will be NULL.

   if (name)
      name_ = name;

   if (domain)
      domain_ = domain;
   // Publish and register our DSD command server
   started = ( server_.open( name_.c_str(), domain_.c_str() ) < 0 ? false : true );
   if ( !started ) {
      // Failed to start DSD server
      // Make a new attempt after closing server object
      server_.close();
      started = ( server_.open( name_.c_str(), domain_.c_str() ) < 0 ? false : true );
      // Check if DSD returns Already published
      if ( !started ) {
         // Failed to start server
         // We check if we can get the handles and if so,
         // assume that everything is working Ok...
         if ( server_.get_handle(0) != ACE_INVALID_HANDLE )
         {
            // Ha, it seems that we're Ok after all...
            started = true;
         }
         else
         {
            // No handles available
         }
      }// if !started 2:nd
   }// if !started 1:st
   int res = -1;
   if ( started )
   {
	  res = 0;
	  numOfFds_ = 0;
      int hIndex = 0;
      char str_to_log[2048] = {0};
      while ( true )
      {
    	  //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::open(); Command Server handle[%i]=%i\n",hIndex,server_.get_handle(hIndex)));
    	 snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::open(); Command Server handle[%i]=%i",hIndex,server_.get_handle(hIndex));
    	  ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);
         fds_[hIndex] = server_.get_handle(hIndex);
         if ( fds_[hIndex] == ACE_INVALID_HANDLE )
         {
            break;
         }
         // Increase counter for DSD handles
         numOfFds_++;
         //if (service_->reactor()->register_handler(fds_[0],this,ACCEPT_MASK) < 0) {
//         if ( REGISTER_PEER_HANDLER(fds_[hIndex],this,ACCEPT_MASK_MASK) < 0 )
         if ( REGISTER_PEER_HANDLER(fds_[hIndex],this,ACCEPT_MASK) < 0 )
         {
        	 //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG, "(%t) ACS_APGCC_CmdServer::open(); ERROR: register_handler failed for handle[%i]=%i\n",hIndex,(int)fds_[hIndex]));
        	 snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::open(); ERROR: register_handler failed for handle[%i]=%i",hIndex,(int)fds_[hIndex]);
        	 ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);
         }
         else
         {
            res++;
         }

         // Get next handle
         hIndex++;
      } // for all
      // Check if succeeded
      if ( numOfFds_ > 0 && res > 0 /*res == numOfFds_*/ )
      {
    	  //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::open(); Command Server successfully started; NumOfFds=%i\n",numOfFds_));
    	  snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::open(); Command Server successfully started; NumOfFds=%i",numOfFds_);
    	  ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);
      }
      else
      {
    	  //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::open(); ERROR: Failed to start Command Server\n"));
    	  ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::open(); ERROR: Failed to start Command Server",LOG_LEVEL_DEBUG);
         // There is something wrong with the Reactor. Cannot continue...
         // @@ Should we initiate re-connection attempts ??
         server_.close();

         return -1;
      }
   }
   else
   {
      // Schedule a timer so we can try to re-open
      // the command server
	   this->start_timer(5);

	   //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::open(); Command Server failed to started; New attempts scheduled...\n"));
	   ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::open(); Command Server failed to started; New attempts scheduled...",LOG_LEVEL_DEBUG);
   }
   return 0;
}

int ACS_APGCC_CmdServer::close()
{
	//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG, "(%t) ACS_APGCC_CmdServer::close() called...\n"));
	ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::close() called...",LOG_LEVEL_DEBUG);
   // Stop any running timer
   this->stop_timer();
   // Close our command server
   for (int i=0; i<this->numOfFds_; i++) {
      if ( fds_[i] != ACE_INVALID_HANDLE ) {
         service_->reactor()->remove_handler(fds_[i],ACCEPT_MASK | DONT_CALL);
      }
   }
   // Close our DSD server
   server_.close();
   return 0;
}


int ACS_APGCC_CmdServer::handle_input(ACE_HANDLE fd)
{
	char str_to_log[2048] = {0};
	//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG, "(%t) ACS_APGCC_CmdServer::handle_input(fd=%i); Command received on DSD Server\n",(int)fd));
	snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::handle_input(fd=%i); Command received on DSD Server",(int)fd);
	ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);

   return this->accept_session();
}

int ACS_APGCC_CmdServer::accept_session()
{
   // Create a session object
   ACS_APGCC_Daemon::SessionObj* sess = new ACS_APGCC_Daemon::SessionObj;
   if ( server_.accept(sess->impl_->peer()) < 0 ) {
	   sess->destroy();
	   //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_ERROR, "(%t) ACS_APGCC_CmdServer::accept_session(); failed to accept Session!\n"));
	   ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::accept_session(); failed to accept Session!",LOG_LEVEL_ERROR);
      // This should never happen, but if it does
      // we most likely need to re-register our service
      // Close our server
       this->close();
      // Schedule a timer so we can try to re-open
      // the command server
      this->start_timer(5);
      return 0;
   }
   // Call the user
   if ( ACS_APGCC_Daemon::instance()->handle_session(sess) < 0 ) {
      sess->destroy();
      //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_ERROR, "(%t) ACS_APGCC_CmdServer::accept_session(); User's handle_session() failed for Session Command!\n"));
	   ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::accept_session(); User's handle_session() failed for Session Command!",LOG_LEVEL_ERROR);
   }
   return 0;
}

int ACS_APGCC_CmdServer::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask mask)
{
	//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG, "(%t) ACS_APGCC_CmdServer::handle_close(handle=%i); mask=0x%x\n",(int)fd,mask));
	char str_to_log[2048] = {0};
	snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::handle_close(handle=%i); mask=%lu",(int)fd,mask);
	ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);

   // Remove our timer
   this->stop_timer();
   // Decrease the number of registred handles
   // It has something to do with the two registered DSD handles.
   this->numOfFds_--;
   if ( this->numOfFds_ <= 0 ) {
      // Close down our server
      server_.close();
      //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG, "(%t) ACS_APGCC_CmdServer::handle_close(handle=%i); Close server and die\n",(int)fd));
      snprintf(str_to_log,2048,"ACS_APGCC_CmdServer::handle_close(handle=%i); Close server and die",(int)fd);
      ACS_APGCC_Daemon_logging.Write(str_to_log,LOG_LEVEL_DEBUG);

      delete this;
   }
   return 0;
}

int ACS_APGCC_CmdServer::handle_timeout(const ACE_Time_Value& tv, const void*)
{
	ACE_UNUSED_ARG(tv);
	//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,"(%t) ACS_APGCC_CmdServer::handle_timeout() called at %T\n"));
	ACS_APGCC_Daemon_logging.Write("ACS_APGCC_CmdServer::handle_timeout()",LOG_LEVEL_DEBUG);
   // Clear timer id
   timer_ = -1;
   // Try to re-establish our Command Server.
   // The open() will start a new timer if it fails.
   this->open(0,0);
   return 0;
}


void ACS_APGCC_CmdServer::start_timer(int seconds)
{
   ACE_Time_Value tv1;
   tv1.set(seconds);
   ACE_Time_Value tv2;
   tv2.set(0);
   if ( timer_ < 0 ) {
//      timer_ = service_->reactor()->schedule_timer(this,0,seconds,0);
      timer_ = service_->reactor()->schedule_timer(this,0,tv1,tv2);
   }
}

void ACS_APGCC_CmdServer::stop_timer()
{
   // Remove our timer
   if ( timer_ >= 0 ) {
      service_->reactor()->cancel_timer(this);
   }
   timer_ = -1;
}


/**********************************ACS_APGCC_Daemon***********/

ACS_APGCC_Daemon::ACS_APGCC_Daemon(const char* name)
:name_(0)
{
   // TODO Auto-generated constructor stub
   // Remember the Reactor
	ACS_APGCC_Daemon_logging.Open("APGCC");
  reactor_ = ACE_Reactor::instance();
  // Set the global pointer
  theService = this;
  // Set name of service and name of domain
   string dom  = ACS_APGCC::after((string)name,":");
   string appl = ACS_APGCC::before((string)name,":");
   if ( dom.length() > 0 )
   {
      // Domain name was present
      domain_ = ::strdup( dom.c_str() );
   }
   else
   {
      // Set default domain name
      domain_ = ::strdup( "APGCC" );
   }

   // Set service name
   name_ = ::strdup( appl.c_str() );
   theService->cmdServer_ = new ACS_APGCC_CmdServer( theService);
   if ( (!theService->cmdServer_) || theService->cmdServer_->open(theService->name(),theService->domain()) < 0 )
   {
	   //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_ERROR, "(%t) ACS_APGCC_Daemon::ServiceMain(); ERROR: failed to open Cmd Server\n"));
	   ACS_APGCC_Daemon_logging.Write("ACS_APGCC_Daemon::ServiceMain(); ERROR: failed to open Cmd Server",LOG_LEVEL_ERROR);
   }
}

ACS_APGCC_Daemon::~ACS_APGCC_Daemon()
{
   // TODO Auto-generated destructor stub
	ACS_APGCC_Daemon_logging.Close();
	free ( name_ );
	free ( domain_ );

}

ACE_Reactor* ACS_APGCC_Daemon::reactor()
{
   return reactor_;
}

ACS_APGCC_Daemon* ACS_APGCC_Daemon::instance()
{
   return theService;
}

int ACS_APGCC_Daemon::run_service(int argc, char* argv[])
{
   argc_ = argc;
   argv_ = argv;

   return this->StartService();
}

int  ACS_APGCC_Daemon::StartService()
{
   theService->open(argc_,argv_);

   if ( theService->reactor() != 0 )
	   theService->reactor()->owner(ACE_Thread::self());

   if ( theService->reactor() != 0 )
	   theService->reactor()->run_event_loop();

   if ( theService->reactor() != 0 )
	   theService->reactor()->close();

   return 0;
}

const char* ACS_APGCC_Daemon::name()
{
   return name_;
}

const char* ACS_APGCC_Daemon::domain()
{
   return domain_;
}

int ACS_APGCC_Daemon::handle_session(SessionObj* cmd)
{
	ACE_UNUSED_ARG(cmd);
	//   if (ACS_TRA_ON(traSvc)) {
//      ::sprintf(traStr_,"%s: handle_session(); Not implemented by user",this->name());
//      ACS_TRA_event(&traSvc,traStr_);
//   }
   return -1;
}
