/*===================================================================
 *
 *	@file  acs_usa_sysloganalyser.cpp 
 * 
 * 	@brief
 *
 *
 * 	@version 1.0.0
 *
 *
 *	HISTORY
 *
 *
 *
 *
 *	PR           DATE      INITIALS    DESCRIPTION
 *--------------------------------------------------------------------
 * 	N/A       DD/MM/YYYY     NS       Initial Release
 *==================================================================== */

/*====================================================================
 *		DIRECTIVE DECLARATION SECTION
 *===================================================================*/

#include <sstream>
#include <sys/time.h>
#include <ace/Reactor.h>

#include "acs_usa_global.h"
#include "acs_usa_sysloganalyser.h"
#include "acs_usa_hamanager.h"
#include "acs_usa_control.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_error.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_autoCeaseManager.h"
#include "acs_usa_analysisObject.h"
#include "acs_usa_initialiser.h"
#include "acs_usa_file.h"
#include "acs_usa_time.h"
#include "acs_usa_imm.h"
#include <sys/eventfd.h>


ACS_USA_Trace traSLA ("ACS_USA_SLA         ");

////////////////////////////////////////////////////////////////////
// Global objects. These are accessible anywhere after including
// appropriate inlude file
////////////////////////////////////////////////////////////////////

ACS_USA_EventManager eventManager;
acs_usa_error        error;

const int ignrUsaEvntsTimeout = 90;
const long ignrMsgId = 1000;


//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_SLA::ACS_USA_SLA(){

	USA_TRACE_ENTER();
	
	m_timerid               = -1;
	m_ignrEvntsTimerid      = -1;
	m_global_instance       = ACS_USA_Global::instance();
	m_haObj			= 0;
	m_cntlObj		= 0;
	m_analysisObj		= 0;
	_handle			= -1;
	m_haMode		= false;
	m_loganalyser_file	= false;
	userInfo = new LocalUserInfo();
	loadConfig();
	usaAdhObj = NULL;
	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_SLA::~ACS_USA_SLA(){

	USA_TRACE_ENTER();

	if (m_cntlObj){
		delete m_cntlObj;
	}	
	
	if (usaAdhObj)
	{
		delete usaAdhObj;
	}

	if(m_loganalyser_file){
		unlink_loganalyser_file();
	}		
	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::p_open(){

	USA_TRACE_ENTER();

	traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA:%s(): Entering.", __func__);
	int fd;

	fd = ::open(USA_PIPE, O_RDWR);
	if( fd == -1 ){
		syslog(LOG_ERR, "acs_usad: Error Creating: [%s]", USA_PIPE);
		traSLA.traceAndLogFmt(ERROR, "%s() - Failed to to create pipe[ %s ]", __func__, USA_PIPE);
		return -1;
	}
	setHandle((ACE_HANDLE)fd); 
	USA_TRACE_LEAVE();

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_SLA::p_close(){

	USA_TRACE_ENTER();

	traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA:%s(): Entering.", __func__);
	int fd;

	fd = ::close(_handle);
	if( fd == -1 ){
		syslog(LOG_ERR, "acs_usad: Error in close:[%s]", USA_PIPE);
		traSLA.traceAndLogFmt(ERROR, "%s() - Error in close: [ %s ]", __func__, USA_PIPE);
		return -1;
	}

	setHandle(ACE_INVALID_HANDLE); 

	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ACS_USA_SLA::setHandle(ACE_HANDLE p_handle){

	USA_TRACE_ENTER();

	this->_handle = p_handle;

	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACE_HANDLE ACS_USA_SLA::get_handle(void) const {

	USA_TRACE_ENTER();
	USA_TRACE_1("get_handle: %d", this->_handle);
	USA_TRACE_LEAVE();
	return this->_handle;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::_initialize(){
	
	USA_TRACE_ENTER();
	
	int rCode=0;
	// open fifo pipe
	if ( this->p_open() < 0){
		traSLA.traceAndLogFmt(ERROR, "%s() - PIPE Open Failed", __func__);
		USA_TRACE_1("PIPE Open Failed");
		rCode=-1;
	}	
	if (rCode != -1){
		ACE_NEW_NORETURN(this->m_cntlObj, ACS_USA_Control());
		if (0 == this->m_cntlObj){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s(): Memory Allocation Failed", __func__);
			USA_TRACE_1("Memory Allocation Failed");
			rCode=-1;
		}
	}

	if (rCode != -1){
		m_cntlObj->initPathConstants(); // expected to always return ACS_USA_Ok
		m_cntlObj->initParameters(); // expected to always return ACS_USA_Ok
				// Load PHA Parameters from IMM
		if (m_cntlObj->loadConfig() == ACS_USA_Error) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() Error in m_cntlObj->loadConfig", __func__);
			USA_TRACE_1("Error in m_cntlObj->loadConfig");
			rCode=-1;	
		}
	}	

	if(rCode != -1) {
		ACE_NEW_NORETURN(this->m_analysisObj, ACS_USA_AnalysisObject());
		if (0 == this->m_analysisObj){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s(): Memory Allocation Failed", __func__);
			USA_TRACE_1("Memory Allocation Failed");
			rCode=-1;
		}
	}

	if(rCode != -1) {
		String tmpName = m_cntlObj->getUSATmpFileName();
		ACS_USA_Initialiser ini;
		if((access(tmpName.data(), F_OK) != -1) && ini.integrityCheck(tmpName.data())) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() ini.integrityCheck is OK!", __func__);
		}
		else {
			traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA:%s() usa.tmp integrityCheck Failed! Loading Objects From IMM!", __func__);
			// Congfigure USA from IMM
			if (m_cntlObj->configure() == ACS_USA_Error){
				traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() Error in m_cntlObj->configure", __func__);
				USA_TRACE_1("ACS_USA_SLA: Error in m_cntlObj->configure");
				rCode=-1;	
			}	
		}	

		if(rCode != -1) {
			if(ini.loadTempStorage(m_analysisObj, tmpName.data()) == ACS_USA_Error) {
				traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() Error in ini.loadTempStorage", __func__);
				m_cntlObj->reportProblem();
				rCode=-1;
			}
		}
		
		String ceaseName = m_cntlObj->getCeaseFileName();
		if( (rCode != -1) && (ACS_USA_AutoCeaseManager::getInstance()).loadList(ceaseName) == ACS_USA_Error ){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() Error in ACS_USA_AutoCeaseManager.loadList()", __func__);
			rCode=-1;
		}	
	}	

	// Start the autoceising activities.
	if (rCode != -1){	
		if ((ACS_USA_AutoCeaseManager::getInstance()).startAutoCeasingTask() == -1) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - Error ACS_USA_AutoCeaseManager.startAutoCeasingTask()", __func__);
			USA_TRACE_1("ACS_USA_SLA: Error ACS_USA_AutoCeaseManager.startAutoCeasingTask()");
			rCode=-1;
		}
	}	

	// Load user information.
	if (rCode != -1){	
		if ((ACS_USA_AutoCeaseManager::getInstance()).startAutoCeasingTask() == -1) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - Error ACS_USA_AutoCeaseManager.startAutoCeasingTask()", __func__);
			USA_TRACE_1("ACS_USA_SLA: Error ACS_USA_AutoCeaseManager.startAutoCeasingTask()");
			rCode=-1;
		}
	}
		
	if (rCode == -1) {
		if (m_haObj){
			// Report to AMF that we want to restart of ourselves
			ACS_APGCC_ReturnType result = m_haObj->componentReportError(ACS_APGCC_COMPONENT_RESTART);
			if (result == ACS_APGCC_SUCCESS) {
				return -1;
			}	
		}
		exit (EXIT_FAILURE);
	}	

	USA_TRACE_LEAVE();
	return rCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::open(usaHAClass *haObj){

	USA_TRACE_ENTER();
	m_haObj=haObj;
	int rCode=0;
	
	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		traSLA.traceAndLogFmt(ERROR, "%s() - Failed to start main svc thread.", __func__);
		rCode=-1;
	}	
	USA_TRACE_LEAVE();
	return rCode;
}
	
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::svc(){

	ACE_Message_Block* mb = NULL;
	bool running = true;
	bool errorDetected=false;

	USA_TRACE_ENTER();
	loadConfig();
	// initialize usa
	// perform startup analysis
	// schedule timer for shortterm analysis

	if (!errorDetected){	
		if (this->_initialize() < 0){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - _initialize FAILED", __func__);
			errorDetected=true;
		}	
	 }

	// perform startup analysis
	if (!errorDetected){
		if (this->_startupAnalysis() < 0){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - _startupAnalysis FAILED", __func__);
			errorDetected=true;
		}		
		else{ 
			traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA: - _startupAnalysis completetd");
		}
	}	

	// schedule tmer for short term analysis
	if (!errorDetected){
		if (this->_setTimer() < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - _setTimer FAILED", __func__);
			errorDetected=true;
		}	
	}	

	// no faults found so-far. register for SEGV handler to capture USA stop time in case crash happens
	if (!errorDetected){
		int status = this->sig_shutdown_.register_handler(SIGSEGV, this);
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - register_handler(SIGSEV, this) Failed", __func__);
			errorDetected=true;
		}	
	}	

	// no faults found so-far. register for SIG_IGNORE_EVENTS handler to ignore the evnets in case of events generated during csadm
	if (!errorDetected){
		int status = this->sig_shutdown_.register_handler(SIG_IGNORE_EVENTS, this);
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - register_handler(SIG_IGNORE_EVENTS, this) Failed", __func__);
			errorDetected=true;
		}	
	}	

	// no faults found so-far. register for SIG_IGNORE_EVENTS handler to ignore the evnets in case of events generated during csadm
	if (!errorDetected){
		int status = this->sig_shutdown_.register_handler(SIG_RESUME_EVENTS, this);
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - register_handler(SIG_RESUME_EVENTS, this) Failed", __func__);
			errorDetected=true;
		}	
	}	
	
	// register pipe handle for reading instant records
	if (!errorDetected) {
		int status = ACS_USA_Global::instance()->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - register_handler(this, ACE_Event_Handler::READ_MASK) Failed", __func__);
			errorDetected=true;
		}	
	} 

	// create log analyser running file so that when cluster reload happens, 
	// USA receives SIG_RESUME_EVENTS, SIG_IGNORE_EVENTS events.
	// For more information, visit '/opt/ap/apos/bin/clusterconf/clu_parse'.
	if (!errorDetected) {
		int status = this->createLogAnalyserFile();
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - createLogAnalyserFile() Failed", __func__);
			errorDetected=true;
		}
	}	

	// before starting the thread, check if we have seen any big faults
	if (errorDetected) {
		if (m_haObj){
			// Report to AMF that we want to restart of ourselves
			ACS_APGCC_ReturnType result = m_haObj->componentReportError(ACS_APGCC_COMPONENT_RESTART);
			if (result == ACS_APGCC_SUCCESS) {
				return -1;
			}	
		}
		exit (EXIT_FAILURE);
	}	


	USA_TRACE_1("ACS_USA_SLA:- Thread running");

	while (running){
		try {
			if (this->getq(mb) < 0){
				traSLA.traceAndLogFmt(ERROR, "ERROR: getq() failed");
				USA_TRACE_1("ERROR: getq() failed");
				break;
			}

			// Check msg type
			switch(mb->msg_type()){

				case SHRT_TIMEOUT:
						USA_TRACE_1("ACS_USA_SLA: shortTermAnalysis timeout");
						this->_shortTermAnalysis();
						mb->release();
						break;
					
				case SHRT_CLOSE:
						USA_TRACE_1("ACS_USA_SLA: SHRT_CLOSE Received");
						mb->release();
						running=false;
						break;

				default:
						traSLA.traceAndLogFmt(INFO, "WARNING:ACS_USA_SLA - not handled message received: %i", mb->msg_type());
						USA_LOG_WA("ACS_USA_SLA: not handled message received:%i", mb->msg_type());
						mb->release();
						running=false;
						break;
			}// end of swith
		}//end of try

		catch(...){
			traSLA.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
			USA_TRACE_1("ERROR: EXCEPTION!");
		}
		
	}// end of while	

	//call 'close(u_long)' at exit of 'svc'
	USA_TRACE_LEAVE();
	return 0;
}


//--------------------------------------------------------------------------------
int ACS_USA_SLA::createLogAnalyserFile(){
	USA_TRACE_ENTER();
	int rCode=0;
	m_loganalyser_file=true;
	traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA::%s() creating [%s]", __func__, USA_LOGANALYSER_RUNNING);
	
	ACE_HANDLE file_handle = ACE_OS::open (USA_LOGANALYSER_RUNNING,
						O_RDWR | O_CREAT | O_TRUNC,
						ACE_DEFAULT_FILE_PERMS);

	if (file_handle  == ACE_INVALID_HANDLE) {
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - createLogAnalyserFile file creation failed", __func__);
		USA_LOG_ER("ERROR: ACS_USA_SLA: - createLogAnalyserFile file creation failed");
		m_loganalyser_file=false;
		rCode=-1;
	} 

	if (rCode != -1)
		ACE_OS::close(file_handle);

	USA_TRACE_LEAVE();
	return rCode;
}

//--------------------------------------------------------------------------------
void ACS_USA_SLA::unlink_loganalyser_file(){
	USA_TRACE_ENTER();
	traSLA.traceAndLogFmt(INFO,"ACS_USA_SLA::%s() removing [%s]", __func__, USA_LOGANALYSER_RUNNING);
		
	if (ACE_OS::unlink (USA_LOGANALYSER_RUNNING) != 0){
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - unlink(%s) failed.", __func__, USA_LOGANALYSER_RUNNING);
		USA_LOG_ER("ERROR: ACS_USA_SLA: - unlink failed.");
	} else { m_loganalyser_file=false; }

	USA_TRACE_LEAVE();
}	

//----------------------------------------------------------------------------------
int ACS_USA_SLA::handle_signal(int signum, siginfo_t*, ucontext_t *) {
	
	USA_TRACE_ENTER();
	int rCode=0;
	int status;
#if 0	
	switch (signum) {
		case SIGSEGV :
			{
				collectUsaStopTime();
				status = this->sig_shutdown_.remove_handler(SIGSEGV);
				if (status < 0) {
					traSLA.traceAndLogFmt(ERROR, "usa-sla-class:%s()- remove_handler(SIGSEGV) failed.", __func__);
					USA_TRACE_LEAVE2("remove_handler(SIGSEGV) failed");
					rCode=-1;
				}	
				break;
			}
		default:
				traSLA.traceAndLogFmt(ERROR, "usa-sla-class:%s()- Other signal caught.[%d]", __func__, signum);
				break;
	}
#endif

	if(SIGSEGV == signum) {

		unlink_loganalyser_file();

		collectUsaStopTime();

		status = this->sig_shutdown_.remove_handler(SIGSEGV);
		if (status < 0) {
			traSLA.traceAndLogFmt(ERROR, "usa-sla-class:%s()- remove_handler(SIGSEGV) failed.", __func__);
			USA_TRACE_LEAVE2("remove_handler(SIGSEGV) failed");
			rCode=-1;
		}	
	}
	else if(SIG_IGNORE_EVENTS == signum) {
		m_global_instance->m_ignrSshdEvents = true;
		traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA: - Received SIG_IGNORE_EVENTS signal");
		const ACE_Time_Value timeoutVal(ignrUsaEvntsTimeout);
		m_ignrEvntsTimerid = m_global_instance->reactor()->schedule_timer(this, (const void *)ignrMsgId , timeoutVal);
		if (this->m_ignrEvntsTimerid < 0){
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - Unable to schedule timer for ignore events", __func__);
			rCode=-1;
		}

	}
	else if(SIG_RESUME_EVENTS == signum) {
		m_global_instance->m_ignrSshdEvents = false;
		traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA: - Received SIG_RESUME_EVENTS signal");
		// remove our timer
		if (this->m_ignrEvntsTimerid != -1) {
			USA_TRACE_1("Cancelling the Outstanding ignore events - Timer");
			if(1 != m_global_instance->reactor()->cancel_timer(this->m_ignrEvntsTimerid)) {
				traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - Cancelling ignore events timer failed", __func__);
			}
		}

	}
	else {
		traSLA.traceAndLogFmt(ERROR, "usa-sla-class:%s()- Other signal caught.[%d]", __func__, signum);
	}

	USA_TRACE_LEAVE();	
	return rCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::close(u_long) {

	USA_TRACE_ENTER();

	ACE_Time_Value timeout ();

	unlink_loganalyser_file();

	int status = this->sig_shutdown_.remove_handler(SIGSEGV);
	if (status < 0) {
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - remove_handler(SIGSEGV) failed.", __func__);
	}

	int igStatus = this->sig_shutdown_.remove_handler(SIG_IGNORE_EVENTS);
	if (igStatus < 0) {
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - remove_handler(SIG_IGNORE_EVENTS) failed.", __func__);
	}

	int reStatus = this->sig_shutdown_.remove_handler(SIG_RESUME_EVENTS);
	if (reStatus < 0) {
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - remove_handler(SIG_RESUME_EVENTS) failed.", __func__);
	}

	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered()) {
		traSLA.traceAndLogFmt(ERROR, "%s(u_long) Abnormal shutdown of ACS_USA_SLA", __func__);
		USA_TRACE_1("(u_long) Abnormal shutdown of ACS_USA_SLA");
		exit(EXIT_FAILURE);
	}

	// remove our timer
	if (this->m_timerid != -1) {
		USA_TRACE_1("Cancelling the Outstanding Timer");
		ACS_USA_Global::instance()->reactor()->cancel_timer(this->m_timerid);
	}

	// remove our timer
	if (this->m_ignrEvntsTimerid != -1) {
		USA_TRACE_1("Cancelling the Outstanding Timer(ignore sshd events)");
		ACS_USA_Global::instance()->reactor()->cancel_timer(this->m_ignrEvntsTimerid);
	}


	// close the pipe fd
	this->p_close();

	traSLA.traceAndLogFmt(INFO, "%s() ACS_USA_SLA stopping AutoCeaseManager", __func__);
	(ACS_USA_AutoCeaseManager::getInstance()).stopAutoCeasingTask();

	USA_TRACE_LEAVE();
	return 0;
}	

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::close(){
	
	USA_TRACE_ENTER();

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());

	int rCode=0;
	if (0 == mb) {
		traSLA.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
		USA_TRACE_1("Failed to create mb object");
		rCode=-1;
	}

	if (rCode != -1){
		mb->msg_type(SHRT_CLOSE);
		if (this->putq(mb) < 0){
			traSLA.traceAndLogFmt(ERROR, "%s() Fail to post SHRT_CLOSE to ourself", __func__);
			USA_TRACE_1("Fail to post SHRT_CLOSE to ourself");
			mb->release();
			rCode=-1;
		}
	}

	// collect USA Stop time now.
	// should not comment the following syslog msg, It is required for startup analysis.
	syslog(LOG_INFO, "USA is shutting down");
	collectUsaStopTime();
		
	USA_TRACE_LEAVE();
	return rCode;
}	

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::_shortTermAnalysis(){

	USA_TRACE_ENTER();
	traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA::%s() Short term initiated", __func__);
	
	m_cntlObj->operate(ACS_USA_ShortTerm_Analysis, m_analysisObj);
	//Added to get the shorterm analysis time filter 
	// should not comment the following syslog msg, It is required for short term analysis.
	syslog(LOG_INFO, "shortTermAnalysis: usa-short-term-end");

	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::handle_timeout(const ACE_Time_Value&, const void* arg ) {

	USA_TRACE_ENTER();

	long msgId = long(arg);

	if (msgId == ignrMsgId ) {
		m_global_instance->m_ignrSshdEvents = false;
		traSLA.traceAndLogFmt(INFO, "ACS_USA_SLA: - Timeout Received for Ignore events");
		return 0;
	}


	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());

	// Post a new SHRT_TIMEOUT message to svc.
	if (mb != 0) {
		mb->msg_type(SHRT_TIMEOUT);
		if (this->putq(mb) < 0) {
			mb->release();
		}
	}
	// re-schedule the timer
	const ACE_Time_Value schedule_time(m_cntlObj->getShortInterval());
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
	if (this->m_timerid < 0){
		traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - Unable to schedule timer.", __func__);
		return -1;
	}
	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::handle_input(ACE_HANDLE fd){

	ACE_UNUSED_ARG(fd);
	//ACE_UNUSED_ARG(arg);
	USA_TRACE_ENTER();

	this->dispatch();

	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ACS_USA_SLA::dispatch(){

	USA_TRACE_ENTER();

	// Get size of the buffer
	int recrdBufSize=m_cntlObj->getRecordBufferSize();
	ACS_USA_RecordBuffer buffer[recrdBufSize];
	int bytesRead=0;
	while (true) {
		bytesRead = read(this->_handle, buffer, recrdBufSize-1);
		if (bytesRead == -1){
			if (errno == EINTR)
				continue;
			traSLA.traceAndLogFmt(ERROR, "ACS_USA_SLA:%s() - read error ", __func__);
		}
		break;
	}	
	if(bytesRead > 0)
	{
		buffer[bytesRead]=0;
		USA_TRACE_1("Msg:[%s]", buffer);
		std::string str(buffer);
		if(str.find("Authentication failure") != std::string::npos)
		{
			std::string search_string = "user";
			if(str.find(search_string) != std::string::npos)
			{
				
				// Expected message
				// "2018-06-05 09:35:05 SecurityEvent su SC-2-1 notice pam_sss(su:auth): received for user MYS1: 7 (Authentication failure)"
				// Parsing implemented according to the above message. If the msg changed, parsing has to be modified.
				int search_string_index = str.rfind(search_string);
				int pos_of_delimiter = str.find_last_of(":");
				int starting_index = search_string_index + search_string.length() + 1;
				int user_length = pos_of_delimiter - starting_index;
				str = str.substr (starting_index, user_length); 

				std::string userName = this->userDetails()->getUserName();
				int status = this->userDetails()->getMonitoingStatus();
				traSLA.traceAndLogFmt(DEBUG,"UserName Received: #%s# compare status = %d", str.c_str(), str.compare(userName));
				USA_TRACE_1("UserName Received: #%s# compare status = %d", str.c_str(), str.compare(userName));
				traSLA.traceAndLogFmt(DEBUG,"\n===========================\n\nUserName to Monitored: %s\nMontoring Status = %d   [0 - DISABLE and 1 - ENABLE]\n===========================\n", userName.c_str(), status);
				USA_TRACE_1("\n===========================\n\nUserName to Monitored: %s\nMontoring Status = %d   [0 - DISABLE and 1 - ENABLE]\n===========================\n", userName.c_str(), status);
				if (status == 1 && !userName.empty() && (str.compare(userName) == 0))
				{
					traSLA.traceAndLogFmt(INFO,"\n===========================\n\nUserName to Monitored: %s\nMontoring Status = %d   [0 - DISABLE and 1 - ENABLE]\n===========================\n", userName.c_str(), status);
					if(usaAdhObj != NULL)
					{
						delete usaAdhObj;
					}
					usaAdhObj = new ACS_USA_AUT_THREAD();
					usaAdhObj->start();

				}
			}
		}
		else
		{

			// Send this record for instant analysis
			m_cntlObj->operate(
					ACS_USA_Instant_Analysis,
					m_analysisObj,
					buffer); 
		}    
	}

	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::_startupAnalysis(){
	USA_TRACE_ENTER();
	
	m_cntlObj->operate(ACS_USA_Startup_Analysis, m_analysisObj);

	// do stuff here
	USA_TRACE_LEAVE();
	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ACS_USA_SLA::_setTimer(){

	// schedule a timer for short term analysis
	// This timer is is used for short term analysis
	USA_TRACE_ENTER();

	USA_TRACE_1("Setting shortTermInterval to [%d]", m_cntlObj->getShortInterval());
	const ACE_Time_Value schedule_time(m_cntlObj->getShortInterval()); 
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

	if (this->m_timerid < 0){
		traSLA.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		USA_TRACE_1("Unable to schedule timer.");
		return -1;
	}

	USA_TRACE_LEAVE();

	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ACS_USA_SLA::collectUsaStopTime() {
		
	USA_TRACE_ENTER();

	ACS_USA_Time clock;
	clock.reset();

	ACS_USA_TimeType currentTime = clock.seconds();
	File file(USA_TIMESTAMP_FILE);

	if (file.isValid() == false) {
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() - Error Opening USA Latest STOP Event file", __func__);
		USA_TRACE_LEAVE2("usa-sla-class:%s() - Error Opening USA Latest STOP Event file", __func__);

	}else if (file.Write(currentTime) == false){
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() - Error in writing the timestamp to [%s]", __func__, USA_TIMESTAMP_FILE);
		
		// report the error
		//String probableCause(ACS_USA_DIAGFAULT);
		String probableCause(ACS_USA_INTLFAULT);
		String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
		error.setError(ACS_USA_TmpFileIOError);
		if (eventManager.report(
					ACS_USA_TmpFileIOError + ACS_USA_FirstErrorCode,
					perceivedSeverity,
					probableCause,
					String(),
					String(error.getErrorText()),
					String(error.getProblemText(error.getError())),
					"",                                          	// Node
					0
					) == ACS_USA_Error)
		{
		}

	}	

	USA_TRACE_LEAVE();
	return;
}

ACS_USA_ReturnType  ACS_USA_SLA::loadConfig()
{
	USA_TRACE_ENTER();
	String char_value;
	std::vector<std::string> p_dnList;
	OmHandler immHandle;
	ACS_CC_ReturnType result = immHandle.Init();
	if( result != ACS_CC_SUCCESS )
	{
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() Initialization FAILURE",__func__);
		USA_TRACE_1("Initialization FAILURE");
	}
	result = immHandle.getClassInstances(imm::AxeAdditionalInfo::className.c_str(), p_dnList);
	int instancesNumber  = p_dnList.size();

	USA_TRACE_1("Instances Found:[%d]",instancesNumber);
	traSLA.traceAndLogFmt(INFO,"usa-sla-class:%s() Instances Found:[%d]",__func__,instancesNumber);
	if (instancesNumber == 0)
	{
		USA_TRACE_LEAVE();
		return ACS_USA_Ok; // go with the default paramters
	}

	ACS_APGCC_ImmObject object;
	char *nameObject =  const_cast<char*>( p_dnList[0].c_str());
	object.objName = nameObject;
	result = immHandle.getObject(&object);
	for (unsigned int i = 0; i < object.attributes.size(); i++ )
	{
		for (unsigned int j = 0; j < object.attributes[i].attrValuesNum; j++)
		{
			
			if((imm::AxeAdditionalInfo::userName.compare(object.attributes[i].attrName) == 0)  && (object.attributes[i].attrValuesNum != 0))
			{
				this->userInfo->setUserName(reinterpret_cast<char*>( object.attributes[i].attrValues[j]));
			}

			if((imm::AxeAdditionalInfo::status.compare(object.attributes[i].attrName) == 0) && (object.attributes[i].attrValuesNum != 0))
			{
				this->userInfo->setMonitoingStatus( (*reinterpret_cast<int*>(object.attributes[i].attrValues[j])));
			}	
		} // end of inner for
	}
	
	result = immHandle.Finalize();
	if( result != ACS_CC_SUCCESS )
	{
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() Finalization FAILURE",__func__);
		USA_TRACE_1("Finalization FAILURE");
	}
	USA_TRACE_LEAVE();	
	return ACS_USA_Ok;
}
/*===================================================================
        ROUTINE: getCurrentTime
=================================================================== */
string ACS_USA_AUT_THREAD::getCurrentTime()
{	
	char            fmt[64], buf[64];
	struct timeval  tv;
	struct tm       *tm;

	::gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	strftime(fmt, sizeof fmt, "%Y-%m-%d_%H:%M:%S.%%06u", tm);
	snprintf(buf, sizeof buf, fmt, tv.tv_usec);
	return buf;
}

int ACS_USA_AUT_THREAD::createAdhInterfaceObject()
{
	USA_TRACE_ENTER();
	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	returnCode = immHandler.Init();
	if( returnCode != ACS_CC_SUCCESS )
	{
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() Initialization FAILURE",__func__);
		USA_TRACE_1("Initialization FAILURE");
		return returnCode;
	}
	vector<ACS_CC_ValuesDefinitionType> adhInterfaceAttrList;
	ACS_CC_ValuesDefinitionType attributeRDN;
	char *className = const_cast<char*>(imm::AxeAdditionalInterface::className.c_str());
	
	//Fill the rdn attribute name
	char tmpAttrName[64] = {0};
	imm::AxeAdditionalInterface::rDN.copy(tmpAttrName,sizeof(tmpAttrName));
	attributeRDN.attrName = tmpAttrName;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;
	
	//RDN value
	char tmpRdnValue[256] = {0};
	snprintf(tmpRdnValue, sizeof(tmpRdnValue) - 1, "%s=%s", imm::AxeAdditionalInterface::rDN.c_str(), getCurrentTime().c_str());
	//snprintf(tmpRdnValue, sizeof(tmpRdnValue) - 1, "%s", getCurrentTime().c_str());
	traSLA.traceAndLogFmt(DEBUG,"usa-sla-class:%s() - RDN of USA_>ADH Interface Object = %s", __func__, tmpRdnValue);
	void* valueRDN[1]={reinterpret_cast<void*>(tmpRdnValue)};
	attributeRDN.attrValues = valueRDN;

	adhInterfaceAttrList.push_back(attributeRDN);


	returnCode = immHandler.createObject(className, NULL, adhInterfaceAttrList);
	if(returnCode == ACS_CC_FAILURE)
	{
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() - Error in creating USA->ADH Interface object %s", __func__,immHandler.getInternalLastErrorText());
		result = ACS_CC_FAILURE;
	}

	returnCode = immHandler.Finalize();
	if( returnCode != ACS_CC_SUCCESS )
	{
		traSLA.traceAndLogFmt(ERROR,"usa-sla-class:%s() Finalization FAILURE",__func__);
		USA_TRACE_1("Finalization FAILURE");
		result = ACS_CC_FAILURE;
	}
	USA_TRACE_LEAVE();
	return result;
}
ACS_USA_AUT_THREAD::ACS_USA_AUT_THREAD()
{

}

ACS_USA_AUT_THREAD::~ACS_USA_AUT_THREAD()
{
	
}
void ACS_USA_AUT_THREAD::start()
{
	traSLA.traceAndLogFmt(DEBUG,"%s", "ACS_USA_SLA_MML START");
	activate();
}
int ACS_USA_AUT_THREAD::svc()
{
	traSLA.traceAndLogFmt(DEBUG,"%s", "ACS_USA_SLA_MML svc()");
	triggerObjCreation();
	return 0;
}
void ACS_USA_AUT_THREAD::triggerObjCreation()
{
	traSLA.traceAndLogFmt(DEBUG,"\n\n%s\n%s\n%s\n\n","================================================","          AUTHENTICATION FAILURE RECEIVED       ","================================================");
	traSLA.traceAndLogFmt(DEBUG,"\n\n%s\n%s\n%s\n","===================================================================","    USA CREATING ADH INTERFACE OBJECT","===================================================================");
	int command_result = createAdhInterfaceObject();
	traSLA.traceAndLogFmt(WARNING,"AUTHENTICATION FAILURE RECEIVED FOR LOCAL USER, OBJECT CREATION STATUS = [%d]", command_result);
	traSLA.traceAndLogFmt(DEBUG,"ADH IMM Object Creation Status: %d", command_result);
	USA_TRACE_1("ADH IMM Object Creation Status: %d", command_result);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//******************************************************************************
//
//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

