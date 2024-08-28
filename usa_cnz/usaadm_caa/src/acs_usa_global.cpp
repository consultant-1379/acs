#include <ace/Guard_T.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <syslog.h>

#include <ACS_APGCC_Util.H>

#include "acs_usa_tratrace.h"
#include "acs_usa_global.h"
#include "acs_usa_zombiemonitor.h"
#include "acs_usa_cpuldavg.h"
#include "acs_usa_sysloganalyser.h"
#include "acs_usa_aprebootmonitor.h"
#include "acs_usa_ntpSrvM.h"
#include "acs_usa_ntfNotificationThread.h"
#include "acs_usa_adhIntUserObserver.h"
// Define trace point
ACS_USA_Trace traGlob 	("ACS_USA_Global	");

// Global mutex for trace
static ACE_Recursive_Thread_Mutex g_GlobalTraceMutex;


static time_t g_StartTime; // Holds start time of service

ACE_Recursive_Thread_Mutex Global::varLock_;



//-----------------------------------------------------------------------------
Global::Global() :
   m_ignrSshdEvents(false),
   m_shutdownOrdered(false),
   m_isVirtual(false),
   m_implementerSet(false),
   m_adhImplementerSet(false),
   m_messageId(0),
   m_reactorImpl(0),
   m_reactor(0),
   m_zMonObj(0),
   m_ldAvgObj(0),
   m_slaObj(0),
   m_aprObj(0),
   m_ntpSrvObj(0),
   m_ntfObj(0),
   m_adhIntObj(0)
{
   ACE_NEW_NORETURN(m_reactorImpl, ACE_TP_Reactor());
   if (0 == m_reactorImpl) {
	syslog(LOG_ERR, "usa-global-class: Failed to create TP Reactor");
	return;
   }

   ACE_NEW_NORETURN(m_reactor, ACE_Reactor(m_reactorImpl));
   if (0 == m_reactor) {
	syslog(LOG_ERR, "usa-global-class: Failed to create Reactor");
	return;
   }

   m_reactor->open(1);
}


//-----------------------------------------------------------------------------
Global::~Global()
{
	this->deactivate();
}


//-----------------------------------------------------------------------------
void Global::deactivate()
{
	delete this->m_zMonObj;
	this->m_zMonObj=0;

	delete this->m_ldAvgObj;
	this->m_ldAvgObj=0;

	delete this->m_slaObj;
	this->m_slaObj=0;

	delete this->m_aprObj;
	this->m_aprObj=0;

	if(this->m_ntpSrvObj != 0){
           delete this->m_ntpSrvObj;
           this->m_ntpSrvObj=0;
        }

        if(this->m_ntfObj != 0) {
           delete this->m_ntfObj;
           this->m_ntfObj=0;
        }

	if(this->m_adhIntObj != 0) {
           delete this->m_adhIntObj;
           this->m_adhIntObj=0;
        }

}


//-----------------------------------------------------------------------------
bool Global::shutdown_ordered()
{
	return this->m_shutdownOrdered;
}


//-----------------------------------------------------------------------------
void Global::shutdown_ordered(bool ordered)
{
	this->m_shutdownOrdered = ordered;
}
void Global::implementer_set(bool flag)
{
	this->m_implementerSet = flag;
}
bool Global::implementer_set()
{
	return this->m_implementerSet;
}

void Global::adh_implementer_set(bool flag)
{
        this->m_adhImplementerSet = flag;
}
bool Global::adh_implementer_set()
{
        return this->m_adhImplementerSet;
}

bool Global::isVirtual()
{
	return this->m_isVirtual;
}
void Global::getEnvironment()
{
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;
	if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success)
	{
		if(nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)
			this->m_isVirtual = true;
	}
}

// Returns handle to global shutdown event
//-----------------------------------------------------------------------------
ACE_event_t Global::shutdown_event()
{
	return m_eventShutdown.handle();
}


// Returns the start time for the service
//-----------------------------------------------------------------------------
time_t* Global::get_service_start_time()
{
	return &g_StartTime;
}

// Returns the uptime for the service in minutes
//-----------------------------------------------------------------------------
ACE_UINT32 Global::get_service_uptime()
{
	time_t now;

	(void) ::time(&now);

	return static_cast<DWORD>( (::difftime( now, g_StartTime ) + 30.0 ) / 60.0);
}



//-----------------------------------------------------------------------------
int Global::get_next_msg_id()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> Guard(varLock_);

	m_messageId = ( m_messageId + 1 ) % MAX_MESSAGE_IDS;

	return m_messageId;
}

bool Global::shutdown_received()
{
   // New linux solution
   ACE_Time_Value wait_timeout(0);
   wait_timeout.sec(RELATIVETIME(1));

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
void Global::notifyGlobalShutdown()
{
	syslog(LOG_INFO, "usa-global-class: notifyGlobalShutdown() returning...");
}


//----------------------------------------------------------------------------
ACE_Reactor* Global::reactor() {
   return ACE_Reactor::instance();
}

//----------------------------------------------------------------------------
ACS_USA_ZombieMon* Global::ZombieMon(){

	if ( this->m_zMonObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_zMonObj == 0) {
			ACE_NEW_NORETURN(this->m_zMonObj, ACS_USA_ZombieMon());
			if (0 == this->m_zMonObj){
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);						
			}	
		}
	}
	return this->m_zMonObj;
}


//----------------------------------------------------------------------------
ACS_USA_CpuLdAvg* Global::CpuLdAvg(){

	if ( this->m_ldAvgObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_ldAvgObj == 0) {
			ACE_NEW_NORETURN(this->m_ldAvgObj, ACS_USA_CpuLdAvg());
			if (0 == this->m_ldAvgObj){
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);						
			}	
		}
	}
	return this->m_ldAvgObj;
}

//----------------------------------------------------------------------------
ACS_USA_SLA* Global::Analyser(){

	if ( this->m_slaObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_slaObj == 0) {
			ACE_NEW_NORETURN(this->m_slaObj, ACS_USA_SLA());
			if (0 == this->m_slaObj){
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);						
			}	
		}
	}
	return this->m_slaObj;
}
//----------------------------------------------------------------------------
ACS_USA_APRebootMon* Global::APRebootMon(){

	if ( this->m_aprObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_aprObj == 0) {
			ACE_NEW_NORETURN(this->m_aprObj, ACS_USA_APRebootMon());
			if (0 == this->m_aprObj){
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);						
			}	
		}
	}
	return this->m_aprObj;
}
//----------------------------------------------------------------------------
ACS_USA_NtpSrvM* Global::NtpSrvM(){

        if ( this->m_ntpSrvObj == 0) {
                // Serialize access
                ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
                if (this->m_ntpSrvObj == 0) {
                        ACE_NEW_NORETURN(this->m_ntpSrvObj, ACS_USA_NtpSrvM());
                        if (0 == this->m_ntpSrvObj){
                                traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);                                   
                        }
                }
        }
        return this->m_ntpSrvObj;
}
//----------------------------------------------------------------------------
ACS_USA_NtfNotificationThread* Global::NtfThread(){

	if ( this->m_ntfObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_ntfObj == 0) {
			ACE_NEW_NORETURN(this->m_ntfObj, ACS_USA_NtfNotificationThread());
			if (0 == this->m_ntfObj){
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);
			}
		}
	}
	return this->m_ntfObj;
}

ACS_USA_AdhIntUserObserver* Global::AdhIntThread() 
{
	if ( this->m_adhIntObj == 0) {
		// Serialize access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->thread_lock_);
		if (this->m_adhIntObj == 0)
		{
			ACE_NEW_NORETURN(this->m_adhIntObj, ACS_USA_AdhIntUserObserver("AxeAdditionalInfo","@UsaAdhUserObserver",ACS_APGCC_ONE));
			if (0 == this->m_adhIntObj)
			{
				traGlob.traceAndLogFmt(ERROR, "%s() Memory Allocation Failed", __func__);
			}
		}
	}
	return this->m_adhIntObj;
}
