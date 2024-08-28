#include <ace/Reactor.h>

#include "acs_usa_adm.h"
#include "acs_usa_hamanager.h"
#include "acs_usa_global.h"
#include "acs_usa_reactorrunner.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_cpuldavg.h"
#include "acs_usa_zombiemonitor.h"
#include "acs_usa_sysloganalyser.h"
#include "acs_usa_aprebootmonitor.h"
#include "acs_usa_ntfNotificationThread.h"
#include "acs_usa_adhIntUserObserver.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_ntpSrvM.h"
#include "acs_prc_api.h"
#include "acs_usa_common.h"

//Defined trace points
//--------------------
ACS_USA_Trace traAdm ("ACS_USA_Adm            ");
const unsigned int implementerRetryTimeout = 1;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_Adm::ACS_USA_Adm():
	m_haObj(0),
	m_reactorRunner(0),
	m_zMonObj(0),
	m_ldAvgObj(0),
	m_slaObj(0),
	ShutdownDone(false),
	m_retryCount(0),
	m_timerid(-1)
{
	USA_TRACE_ENTER2("Constructor");
	USA_TRACE_LEAVE2("Constructor");

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_Adm::~ACS_USA_Adm(){
	// empty
	USA_TRACE_ENTER2("Destructor");
	USA_TRACE_LEAVE2("Destructor");
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
int ACS_USA_Adm::start(usaHAClass* haObj ){

	USA_TRACE_ENTER();

	if (0 == haObj) {
		traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() NULL haObj Found", __func__);
		return -1;
	}
	m_haObj=haObj;
	traAdm.traceAndLogFmt(INFO, "usa-adm-class: %s() active invoked", __func__);

	USA_TRACE_LEAVE();
	return this->start(0,0);
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
int ACS_USA_Adm::start(int argc, char* argv[]) {

	(void)argc;
	(void)argv;
	USA_TRACE_ENTER();

	int status = this->sig_shutdown_.register_handler(SIGINT, this);
	if (status < 0) {
		traAdm.traceAndLogFmt(ERROR,"usa-adm-class: %s() register_handler(SIGINT,this) failed..",__func__);
		return -1;
	}
	status = this->sig_shutdown_.register_handler(SIGTERM,this);
	if (status < 0) {
		traAdm.traceAndLogFmt(ERROR, "usa-adm-class: %s() register_handler(SIGTERM,this) failed.",__func__);
		return -1;
	}	

	ACE_NEW_NORETURN(m_reactorRunner, ACS_USA_ReactorRunner(ACS_USA_Global::instance()->reactor(), "USA main reactor"));
	if (0 == m_reactorRunner) {
		traAdm.traceAndLogFmt(ERROR, "usa-adm-class: %s() Failed to create ACS_USA_ReactorRunner", __func__);
		return -1;
	}

	int res = m_reactorRunner->open();
	if (res < 0) {
		traAdm.traceAndLogFmt(ERROR, "usa-adm-class: Failed to start ACS_USA_ReactorRunner");
		return -1;
	}	
	USA_TRACE_LEAVE();
	return this->activate( THR_JOINABLE | THR_NEW_LWP );
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
void ACS_USA_Adm::stop() {

	USA_TRACE_ENTER();
	traAdm.traceAndLogFmt(INFO, "usa-adm-class: stop invoked");

	if (this->ShutdownDone){
		traAdm.traceAndLogFmt(INFO, "usa-adm-class: Shutdown Already Done");
		return ;	
	}
	
	// Shutdown message
	ACE_Message_Block* mb=0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());
	if (mb == 0){
		traAdm.traceAndLogFmt(INFO, "usa-adm-class:Failed create message USA_SHUTDOWN");
	} else {
		mb->msg_type(USA_SHUTDOWN);
		if (this->putq(mb) < 0){
			mb->release();
			mb=0;
			traAdm.traceAndLogFmt(ERROR, "usa-adm-class:Failed to send msg USA_SHUTDOWN");
		}else{
			traAdm.traceAndLogFmt(INFO, "usa-adm-class:USA_SHUTDOWN Ordered Internally");
		}	
	}

	this->ShutdownDone=true;
	USA_TRACE_LEAVE();
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
int ACS_USA_Adm::handle_close(ACE_HANDLE, ACE_Reactor_Mask /*mask*/) {
	
	USA_TRACE_ENTER();
	// Remove this handler from the reactor.
	ACS_USA_Global::instance()->reactor()->remove_handler(this, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
	if (m_timerid != -1) {
		ACS_USA_Global::instance()->reactor()->cancel_timer(m_timerid);
	}
	USA_TRACE_LEAVE();

	return 0;
}	
	
//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------
int ACS_USA_Adm::handle_signal(int signum, siginfo_t*, ucontext_t *) {

	USA_TRACE_ENTER();

	switch (signum) {

		case SIGTERM:
			traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() - signal SIGTERM caught...", __func__);
			break;
		case SIGINT:
			traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() - signal SIGINT caught...", __func__);
			break;
		default:
			traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() - other sinal caught..[%d]", __func__, signum);
			break;
	}
	this->stop();
	USA_TRACE_LEAVE();
	return 0;	
}

int ACS_USA_Adm::close(u_long /* flags */){

	USA_TRACE_ENTER();
	int status = this->sig_shutdown_.remove_handler(SIGINT);
	if (status < 0) {
		traAdm.traceAndLogFmt(ERROR, "usa-adm-class:%s() - remove_handler(SIGINT) failed.", __func__);
	}

	status = this->sig_shutdown_.remove_handler(SIGTERM);
	if (status < 0) {
		traAdm.traceAndLogFmt(ERROR, "usa-adm-class:%s() - remove_handler(SIGTERM) failed.", __func__);
	}
	// Removing the NTP implementer
	finalizeNtp();

	// Set flag
	ACS_USA_Global::instance()->shutdown_ordered(true);

	// Closing System Log Analyser 
	(void)ACS_USA_Global::instance()->Analyser()->close();
	traAdm.traceAndLogFmt(INFO, "%s() waiting for ACS_USA_SLA to close", __func__);
	(void)ACS_USA_Global::instance()->Analyser()->wait();

	// Closing Zombie monitor
	(void)ACS_USA_Global::instance()->ZombieMon()->close();
	traAdm.traceAndLogFmt(INFO, "%s() waiting for the ZombieMon to close", __func__);
	(void)ACS_USA_Global::instance()->ZombieMon()->wait();

	//Closing CPU ldavg monitor
	(void)ACS_USA_Global::instance()->CpuLdAvg()->close();
	traAdm.traceAndLogFmt(INFO,"%s() waiting for the CpuLdAvg to close", __func__);
	(void)ACS_USA_Global::instance()->CpuLdAvg()->wait();

	//Closing Reboot monitor thread
	(void)ACS_USA_Global::instance()->APRebootMon()->close();
	traAdm.traceAndLogFmt(INFO,"%s() waiting for the APRebootMon to close", __func__);
	(void)ACS_USA_Global::instance()->APRebootMon()->wait();

	//Closing Ntf subscription thread
	finalizeNtf();

	//Closing adh user monitor thread
	if(ACS_USA_Global::instance()->adh_implementer_set())
	{
		ACS_USA_Global::instance()->AdhIntThread()->shutdown();
		traAdm.traceAndLogFmt(ERROR,"%s() waiting for the ADHObserver to close", __func__);
		ACS_USA_Global::instance()->AdhIntThread()->wait();
	}
	
	// Stop reactor
	if (m_reactorRunner != 0) {
		m_reactorRunner->stop();
		m_reactorRunner->wait();
		delete m_reactorRunner;
		m_reactorRunner = 0;
	}
	USA_TRACE_LEAVE();
	return 0;
}

//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------

int ACS_USA_Adm::svc() {
	
	USA_TRACE_ENTER();

	bool errorDetected=false;
	bool done=false;
	int res=0;
	ACS_USA_Global::instance()->getEnvironment();

	// Create main USA instance
	if (!errorDetected){
		// Create System Log Analyser (SLA) Instance
		if (ACS_USA_Global::instance()->Analyser()->open(m_haObj) < 0){
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to create System Log Analyser (SLA) Instance", __func__);
			errorDetected=true;
		}
	}
	if (!errorDetected){
		if (initializeNtp() < 0) {
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to InitializeNtp", __func__);
			errorDetected=true;
		}		
	}
	if (!errorDetected){
		// Create CPU LD Avg Instance
		if (ACS_USA_Global::instance()->CpuLdAvg()->open() < 0){
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to create CPU LdAvg montior Instance", __func__);
			errorDetected=true;			
		}	
	}

	if (!errorDetected){
		// Create Zombie monitor Instance
		if (ACS_USA_Global::instance()->ZombieMon()->open() < 0){
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to create Zombie monitor Instance", __func__);
			errorDetected=true;
		}
	}	

	if (!errorDetected){
		// Create Zombie monitor Instance
		if (ACS_USA_Global::instance()->APRebootMon()->open(this) < 0){
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to create Zombie monitor Instance", __func__);
			errorDetected=true;
		}
	}
	if (!errorDetected){
		// Create Ntf subscription Instance
		if (initializeNtf() < 0) {
			traAdm.traceAndLogFmt(ERROR, "%s(): Failed to InitializeNtp", __func__);
			errorDetected=true;
		}
	}
	
	if(!errorDetected){
		//Activate adhUser Observer
		if(!initializeAdhUserObserver())
		{
			traAdm.traceAndLogFmt(ERROR,"%s()--Failed to initailize ADH->USA Observer !!", __func__);
			errorDetected=true;
		}
	}

	if (errorDetected) {
		if (m_haObj){
			// Report to AMf that we want to restart of ourselves
			ACS_APGCC_ReturnType result = m_haObj->componentReportError(ACS_APGCC_COMPONENT_RESTART);
			if (result == ACS_APGCC_SUCCESS) {
				 return -1;
			}		 
		}
		exit (EXIT_FAILURE);
	}

	traAdm.traceAndLogFmt(INFO, "%s(): Thread is running now..", __func__);

	ACE_Message_Block* mb=0;
	while (!done){
		res = this->getq(mb);
		if (res < 0)
			break;
		
		//Checked received message
		switch( mb->msg_type() ){

			case USA_SHUTDOWN: {
				traAdm.traceAndLogFmt(INFO, "usa-adm-class: received USA_SHUTDOWN");
				mb->release();					   
				mb=0;
				done=true;
				break;
			}

			default: {
				traAdm.traceAndLogFmt(ERROR, "usa-adm-class:[%d] Unknown message received:", mb->msg_type());
		      		mb->release();
       				mb=0;
				break;
			}	
		}		
	}// end of while

	USA_TRACE_LEAVE();
	return ACS_APGCC_SUCCESS;
}

int ACS_USA_Adm::handle_timeout(const ACE_Time_Value&, const void* )
{

	USA_TRACE_ENTER();
	traAdm.traceAndLogFmt(INFO, "INFO - IMM_TIMEOUT received");
	if (ACS_USA_Global::instance()->isVirtual()) {
		int nodeState=NODE_STATE_UNDEFINED;
		ACS_PRC_API prc;
		nodeState=prc.askForNodeState();
		traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() - node state=%d", __func__,nodeState);
		if(nodeState == NODE_STATE_ACTIVE)
		{
			ACS_USA_Global::instance()->NtpSrvM()->monitorNtpServer();
			if(!ACS_USA_Global::instance()->implementer_set())
			{
				ACS_USA_Global::instance()->NtpSrvM()->registerNtpObserver();
			}
			m_retryCount=0;
			return 0;
		}
		else if(nodeState == NODE_STATE_PASSIVE)
		{
			m_retryCount=0;
			return 0;
		}
	}
	// re-schedule the timer
	int rCode=0;
	if(m_retryCount< 10)
	{
		const ACE_Time_Value schedule_time(implementerRetryTimeout);
		m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
		if (this->m_timerid < 0){
			traAdm.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
			rCode=-1;
		}
		m_retryCount++;
	}

	USA_TRACE_LEAVE();
	return rCode;
}

int ACS_USA_Adm::initializeNtp() {

	if (ACS_USA_Global::instance()->isVirtual()) {
		int nodeState=NODE_STATE_UNDEFINED;
		ACS_PRC_API prc;
		nodeState=prc.askForNodeState();
		traAdm.traceAndLogFmt(INFO, "usa-adm-class:%s() - node state=%d", __func__,nodeState);
		if(nodeState == NODE_STATE_ACTIVE)
		{
			ACS_USA_Global::instance()->NtpSrvM()->monitorNtpServer();
			if(!ACS_USA_Global::instance()->implementer_set())
			{
				traAdm.traceAndLogFmt(ERROR, "usa-adm-class::%s() ---- implementer set", __func__);
				ACS_USA_Global::instance()->NtpSrvM()->registerNtpObserver();
			}
		}
		else if(nodeState != NODE_STATE_PASSIVE)
		{
			traAdm.traceAndLogFmt(ERROR, "ACS_USA_class:%s() - Going for Retry.", __func__);
			const ACE_Time_Value schedule_time(implementerRetryTimeout);
			m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
			if (this->m_timerid < 0){
				traAdm.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
				return -1;
			}
		}
	}
	return 0;
}
int ACS_USA_Adm::initializeNtf() {
	if (ACS_USA_Global::instance()->NtfThread()->open() < 0){
		traAdm.traceAndLogFmt(ERROR, "%s(): Failed to create NtfThread subscription Instance", __func__);
		return -1;
	}
	return 0;
}
void ACS_USA_Adm::finalizeNtp() {
	if (ACS_USA_Global::instance()->isVirtual()) {
		if(ACS_USA_Global::instance()->implementer_set())
		{
			traAdm.traceAndLogFmt(INFO, "%s(): Removing Ntp Observer", __func__);
			(void)ACS_USA_Global::instance()->NtpSrvM()->removeNtpObserver();
		}
	}
}
void ACS_USA_Adm::finalizeNtf() {
	(void)ACS_USA_Global::instance()->NtfThread()->close();
	traAdm.traceAndLogFmt(INFO,"%s() waiting for the NtfThread to close", __func__);
	(void)ACS_USA_Global::instance()->NtfThread()->wait();
	traAdm.traceAndLogFmt(INFO,"%s()--NtfThread is closed", __func__);
}

bool ACS_USA_Adm::initializeAdhUserObserver()
{
	int nodeState=NODE_STATE_UNDEFINED;
	ACS_PRC_API prc;
	nodeState=prc.askForNodeState();
	traAdm.traceAndLogFmt(ERROR, "usa-adm-class:%s() - node state=%d", __func__,nodeState);
	if(nodeState == NODE_STATE_ACTIVE)
	{
		if (-1 == ACS_USA_Global::instance()->AdhIntThread()->activate()) //activation of thread failed 
		{
			traAdm.traceAndLogFmt(ERROR,"%s()--ADH->USA Observer Thread Activation failed !!", __func__);
			return false;
		}
		traAdm.traceAndLogFmt(ERROR,"%s()--ADH->USA Observer Thread Activation successful !!", __func__);
	}
	return true;
}
