
// Header Template


#include <sstream>
#include <utmp.h>
#include <ace/Reactor.h>

#include "acs_usa_global.h"
#include "acs_usa_adm.h"
#include "acs_usa_aprebootmonitor.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_error.h"


#define ACS_USA_RUNLEVEL_6 48 
#define ACS_USA_RUNLEVEL_0 54

ACS_USA_Trace traRM ("ACS_USA_APRebootMon		");	
const unsigned int ACS_USA_DefaultMonitorInterval     = 1; // seconds

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

ACS_USA_APRebootMon::ACS_USA_APRebootMon(){

	USA_TRACE_ENTER();

	m_admObj 		  = 0;
	rebootMonitorTimeInterval = ACS_USA_DefaultMonitorInterval;
	m_timerid		  = -1;
	m_global_instance	  = ACS_USA_Global::instance();	

	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_APRebootMon::~ACS_USA_APRebootMon(){

	USA_TRACE_ENTER();

	USA_TRACE_LEAVE();

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_APRebootMon::open(ACS_USA_Adm* p_admObj){

	USA_TRACE_ENTER();

	m_admObj=p_admObj;	
	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		traRM.traceAndLogFmt(ERROR, "%s() - Failed to start main svc thread.", __func__);
		USA_TRACE_LEAVE();
	       	return -1;
	}

	USA_TRACE_LEAVE();
	return 0;	
}	

//-----------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_APRebootMon::svc(){

	USA_TRACE_ENTER();

	ACE_Message_Block* mb = NULL;
	bool running = true;

	USA_TRACE_1("ACS_USA_APRebootMon:svc() - thread running");

	// schedule a timer for zombie montior periodic intervel
        const ACE_Time_Value schedule_time(rebootMonitorTimeInterval); // hardcoded for new. remove it later to shedule configurable interval time
        m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

        if (this->m_timerid < 0){
                traRM.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
                USA_TRACE_LEAVE();
                return -1;
        }


	while (running){
		
		try {
			if (this->getq(mb) < 0){
				traRM.traceAndLogFmt(ERROR, "ERROR: getq() failed");
				USA_TRACE_1("ERROR: getq() failed");
				break;
			}

			// Check msg type
			
			switch(mb->msg_type()){
				
				case APRB_TIMEOUT:
						//traRM.traceAndLogFmt(INFO, "INFO - APReboot timeout recevied");
						//USA_TRACE_1("INFO - APReboot timeout recevied");
						this->queryRunLevel();
						mb->release();
						break;
				
				case APRB_CLOSE:
						traRM.traceAndLogFmt(INFO, "INFO - Shutdown received");
						USA_TRACE_1("INFO - Shutdown received");
						mb->release();
						running=false;
						break;
						
				default:
						traRM.traceAndLogFmt(INFO, "WARNING - not handled message received: %i", mb->msg_type());
						USA_LOG_WA("WARNING - not handled message received: %i",mb->msg_type());
						mb->release();
						running=false;
						break;			

			} // end of switch	
		}// end of try

		catch(...) {
			traRM.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
			USA_TRACE_1("ERROR: EXCEPTION!");
		}
	}// end of while

	//call 'close(u_long)' at exit of 'svc'
	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_APRebootMon::close(u_long) {

	USA_TRACE_ENTER();

	ACE_Time_Value timeout ();

	traRM.traceAndLogFmt(INFO, "close(u_long) - INFO: Closing down.");
	USA_TRACE_1("close(u_long) - INFO: Closing down.");

	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered() ) {
		traRM.traceAndLogFmt(ERROR, "%s(u_long) Abnormal shutdown of Zombie Monitor", __func__);
		USA_TRACE_1("(u_long) Abnormal shutdown of Zombie Monitor");
		exit(EXIT_FAILURE);
	}

	// remove our timer
	if (m_timerid != -1) {
		ACS_USA_Global::instance()->reactor()->cancel_timer(m_timerid);
	}

	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_APRebootMon::close(){

	USA_TRACE_ENTER();
	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());

	int rCode=0;
	syslog(LOG_INFO, "ACS_USA_APRebootMon::%s() invoked", __func__);

	if (0 == mb) {
		traRM.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
		USA_TRACE_1("Failed to create mb object");
		rCode= -1;
	}
	if (rCode != -1){
		mb->msg_type(APRB_CLOSE);
		if (this->putq(mb) < 0){
			traRM.traceAndLogFmt(ERROR, "%s() Fail to post ZMBE_CLOSE to ourself", __func__);
			USA_TRACE_1("Fail to post ZMBE_CLOSE to ourself");
			mb->release();
			rCode=-1;
		}
	}
	USA_TRACE_LEAVE();
	return rCode;		
}

void ACS_USA_APRebootMon::queryRunLevel() {

	struct utmp *ut;
	int runlevel;

	setutent();
	while ((ut = getutent()) != NULL) {
		if (ut->ut_type != RUN_LVL) {
			continue;	
		}	

		runlevel=ut->ut_pid%256;
		if (runlevel == ACS_USA_RUNLEVEL_6 || runlevel == ACS_USA_RUNLEVEL_0){
			traRM.traceAndLogFmt(INFO, "%s() - Reboot Initiated Stopping USA", __func__);
			m_admObj->stop();
			break;			
		}
  	}
  	endutent();
}

//------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_APRebootMon::handle_timeout(const ACE_Time_Value&, const void* ) {

//	USA_TRACE_ENTER();

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());
	int rCode=0;


	// Post a new ZMBE_TIMEOUT message to svc.
	if (mb != 0) {
		mb->msg_type(APRB_TIMEOUT);
		if (this->putq(mb) < 0) {
			mb->release();
		}	
	}	

	// re-schedule the timer
	const ACE_Time_Value schedule_time(rebootMonitorTimeInterval);
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

	if (this->m_timerid < 0){
		traRM.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		rCode=-1;
	}

//	USA_TRACE_LEAVE();
	return rCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
