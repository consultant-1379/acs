#include <ace/Guard_T.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <syslog.h>

#include <ACS_APGCC_Util.H>
#include "acs_asec_global.h"
#include "acs_asec_ObjectImplementer.h"
#include "acs_asec_dsdServer.h"
#include "acs_asec_immhandler.h"

#include <ace/Monotonic_Time_Policy.h>

// Global mutex for trace
static ACE_Recursive_Thread_Mutex g_GlobalTraceMutex;

ACE_Recursive_Thread_Mutex Global::varLock_;

//-----------------------------------------------------------------------------
Global::Global() :
   m_shutdownOrdered(false),
   m_reactorImpl(0),
   m_reactor(0),
   m_ObjImpl(0),
   m_immHandler(0),
   m_dsdObj(0)
{
   ACE_NEW_NORETURN(m_reactorImpl, ACE_TP_Reactor());
   if (0 == m_reactorImpl) {
	syslog(LOG_ERR, "asec-global-class: Failed to create TP Reactor");
	return;
   }

   ACE_NEW_NORETURN(m_reactor, ACE_Reactor(m_reactorImpl));
   if (0 == m_reactor) {
	syslog(LOG_ERR, "asec-global-class: Failed to create Reactor");
	return;
   }

   m_reactor->open(1);
}


//-----------------------------------------------------------------------------
Global::~Global()
{
	//this->deactivate();
}


//-----------------------------------------------------------------------------
void Global::deactivate() {

	syslog(LOG_INFO, "asec-global-class:deactivate()");
	if (this->m_ObjImpl){
		syslog(LOG_INFO, "asec-global-class:m_ObjImpl()");
		delete this->m_ObjImpl;
		this->m_ObjImpl=0;
	}	
	if (this->m_immHandler){
		syslog(LOG_INFO, "asec-global-class:m_immHandler()");
		delete this->m_immHandler;
		this->m_immHandler=0;
	}


	if (this->m_dsdObj){
		syslog(LOG_INFO, "asec-global-class:m_dsdObj()");
		delete this->m_dsdObj;
		this->m_dsdObj=0;
	}
}


//-----------------------------------------------------------------------------
bool Global::shutdown_ordered() {

	return this->m_shutdownOrdered;
}


//-----------------------------------------------------------------------------
void Global::shutdown_ordered(bool ordered) {

	this->m_shutdownOrdered = ordered;
}

// Returns handle to global shutdown event
//-----------------------------------------------------------------------------
ACE_event_t Global::shutdown_event() {

	return m_eventShutdown.handle();
}


//-----------------------------------------------------------------------------
bool Global::shutdown_received() {

   // New linux solution
   //ACE_Time_Value wait_timeout(0);
   ACE_Time_Value_T<ACE_Monotonic_Time_Policy> wait_timeout;
   AceTask_AsecTask myTask;
   wait_timeout = myTask.gettimeofday ();
   wait_timeout += ACE_Time_Value (1,0);

   //wait_timeout.sec(RELATIVETIME(1));

   if (this->m_eventShutdown.wait(&wait_timeout) == 0)
   {
      // "shutdown" event has occurred
      return true;
   }
   else
   {
      // Timeout has occurred
      return false;
   }
}

//-----------------------------------------------------------------------------
void Global::notifyGlobalShutdown() {

	syslog(LOG_INFO, "asec-global-class: notifyGlobalShutdown() returning...");
}


//----------------------------------------------------------------------------
ACE_Reactor* Global::reactor() {
   return ACE_Reactor::instance();
}

//----------------------------------------------------------------------------
ACS_ASEC_ObjImpl* Global::asecImpl(){

	if ( this->m_ObjImpl == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_ObjImpl == 0) {
			ACE_NEW_NORETURN(this->m_ObjImpl, ACS_ASEC_ObjImpl());
			if (0 == this->m_ObjImpl){
				syslog(LOG_ERR, "asec-global-class:%s() Memory Allocation Failed", __func__);
			}	
		}
	}
	return this->m_ObjImpl;
}


ACS_ASEC_ImmHandler* Global::immHandler(){

	if ( this->m_immHandler == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_immHandler == 0) {
			ACE_NEW_NORETURN(this->m_immHandler, ACS_ASEC_ImmHandler());
			if (0 == this->m_immHandler){
				syslog(LOG_ERR, "asec-global-class:%s() Memory Allocation Failed", __func__);
			}
		}
	}
	return this->m_immHandler;
}


//----------------------------------------------------------------------------
ACS_ASEC_DSDSrv* Global::asecDsd(){

	if ( this->m_dsdObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_dsdObj == 0) {
			ACE_NEW_NORETURN(this->m_dsdObj, ACS_ASEC_DSDSrv());
			if (0 == this->m_dsdObj){
				syslog(LOG_ERR, "asec-global-class:%s() Memory Allocation Failed", __func__);
			}	
		}
	}
	return this->m_dsdObj;
}
//----------------------------------------------------------------------------

