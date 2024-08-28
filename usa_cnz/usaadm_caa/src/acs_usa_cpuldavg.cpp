
// Header Template


#include <sstream>
#include <ace/Reactor.h>

#include "acs_usa_global.h"
#include "acs_usa_cpuldavg.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_error.h"

#define ACS_USA_CPU_LOADAVG_SPECIFIC_PROBLEM 8722

ACS_USA_Trace traLD ("ACS_USA_CpuLdAvg		");	
const unsigned int ACS_USA_DefaultMonitorInterval     = 300; // seconds
const unsigned int ACS_USA_DefaultCritical     = 100; 
const unsigned int ACS_USA_DefaultMajor     = 95; 
const unsigned int ACS_USA_DefaultMinor     = 75; 

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

ACS_USA_CpuLdAvg::ACS_USA_CpuLdAvg(){

	USA_TRACE_ENTER2("Constructor");

	cpuLoadAvgMonitorTimeInterval = ACS_USA_DefaultMonitorInterval;
	alarmRaised = false;
	ceaseAlarm = false;
	disableAlarmFilter = false;
	lastAlarmSeverity = NO_ALARM;

	int numOfCores = getCPUCount();
	minorThreshold = static_cast<double>(numOfCores * ACS_USA_DefaultMinor)/100;
	majorThreshold = static_cast<double>(numOfCores * ACS_USA_DefaultMajor)/100;
	criticalThreshold = static_cast<double>(numOfCores * ACS_USA_DefaultCritical)/100;
	m_timerid		= -1;
	m_global_instance	= ACS_USA_Global::instance();	

	USA_TRACE_LEAVE2("Constructor");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_CpuLdAvg::~ACS_USA_CpuLdAvg(){

	USA_TRACE_ENTER2("Destructor");

	USA_TRACE_LEAVE2("Destructor");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_CpuLdAvg::open(){

	USA_TRACE_ENTER();
	int rCode=0;

	// perform any initialization tasks
	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		traLD.traceAndLogFmt(ERROR, "%s() - Failed to start main svc thread.", __func__);
		rCode= -1;
	}

	/*if (rCode != -1){
		// schedule a timer for cpu ldavg montior periodic intervel
		const ACE_Time_Value schedule_time(900); // hardcoded for new. remove it later to shedule configurable interval time
		m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

		if (this->m_timerid < 0){
			traLD.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
			rCode=-1;
		}
	}*/	

	USA_TRACE_LEAVE();
	return rCode;	
}	

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_CpuLdAvg::svc(){

	USA_TRACE_ENTER();

	ACE_Message_Block* mb = NULL;
	bool running = true;

	USA_TRACE_1("ACS_USA_CpuLdAvg:svc() - INFO thread running");
	traLD.traceAndLogFmt(INFO, "ACS_USA_CpuLdAvg:svc() - INFO thread running");
	
	initialize();
	if(disableAlarmFilter) {
		traLD.traceAndLogFmt(INFO, "disableAlarmFilter set Exiting CPU Loadavg Thread.....!");
		return 0;
	}
	this->monitorCpuLdAvg();
	// schedule a timer for cpu ldavg montior periodic intervel
	const ACE_Time_Value schedule_time(cpuLoadAvgMonitorTimeInterval); // hardcoded for new. remove it later to shedule configurable interval time
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);
	if (this->m_timerid < 0){
		traLD.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		return -1;
	}

	while (running){
		
		try {
			if (this->getq(mb) < 0){
				traLD.traceAndLogFmt(ERROR, "ERROR: getq() failed");
				break;
			}

			// Check msg type
			
			switch(mb->msg_type()){
				
				case LDAG_TIMEOUT:
						//traLD.traceAndLogFmt(INFO, "INFO - CPU LdAvg timeout recevied");
						this->monitorCpuLdAvg();
						mb->release();
						break;
				
				case LDAG_CLOSE:
						traLD.traceAndLogFmt(INFO, "INFO - LDAG_CLOSE received");
						mb->release();
						running=false;
						break;
						
				default:
						traLD.traceAndLogFmt(INFO, "WARNING - not handled message received: %i", mb->msg_type());
						mb->release();
						running=false;
						break;			
						

			} // end of switch	
		}// end of try

		catch(...) {
			traLD.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
		}
	}// end of while

	//call 'close(u_long)' at exit of 'svc'
	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_CpuLdAvg::close(u_long) {

	USA_TRACE_ENTER();
	ACE_Time_Value timeout ();

	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered() && (!disableAlarmFilter)) {
		traLD.traceAndLogFmt(ERROR, "%s(u_long) Abnormal shutdown of CPU LdAvg Monitor", __func__);
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

int ACS_USA_CpuLdAvg::close(){

	USA_TRACE_ENTER();

        ACE_Message_Block* mb = 0;
        ACE_NEW_NORETURN( mb, ACE_Message_Block());
	int rCode=0;

        if (0 == mb) {
                traLD.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
                rCode=-1;
        }
	if (rCode != -1){
        	mb->msg_type(LDAG_CLOSE);
        	if (this->putq(mb) < 0){
                	traLD.traceAndLogFmt(ERROR, "%s() Fail to post ZMBE_CLOSE to ourself", __func__);
                	mb->release();
               	 	rCode=-1;
		}	
        }

	USA_TRACE_LEAVE();
        return rCode;
}
int ACS_USA_CpuLdAvg::getCPUCount() {

	cpu_set_t cpuSet;
	CPU_ZERO(&cpuSet);
	sched_getaffinity(0, sizeof(cpuSet), &cpuSet);
	int count = 0;
	for(int i = 0; i < (int)sizeof(cpu_set_t); i++) {
		if(CPU_ISSET(i, &cpuSet))
			count++;
	}
	return count;
}
ACS_USA_ReturnType ACS_USA_CpuLdAvg::setPerameter(String paramName, int value)
{
	USA_TRACE_ENTER();

	int numOfCores = getCPUCount();
	traLD.traceAndLogFmt(INFO, "ACS_USA_CpuLdAvg:%s() - paramName[%s] value = %d", __func__, paramName.data(), value);
	if (paramName == String("CPULoadAvgMonitorInterval")) {
		cpuLoadAvgMonitorTimeInterval = value;
	}else	if(paramName == String("minor")) {
		minorThreshold = static_cast<double>(numOfCores * value)/100;
	} else if(paramName == String("major")) {
		majorThreshold = static_cast<double>(numOfCores * value)/100;
	} else if(paramName == String("critical")) {
		criticalThreshold = static_cast<double>(numOfCores * value)/100;
	} else {
		// Ignore the rest of paramters
		USA_TRACE_1("Ignoring paramName[%s]", paramName.data());
		traLD.traceAndLogFmt(ERROR, "ACS_USA_CpuLdAvg:%s() - Ignoring paramName[%s]", __func__, paramName.data());
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}
int ACS_USA_CpuLdAvg::initialize()
{
	USA_TRACE_ENTER();

	char *className = const_cast<char*>("USACPULoadAvgConfig");
	String char_value;
	unsigned int int_value=0;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	OmHandler immHandle;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		traLD.traceAndLogFmt(ERROR,"ACS_USA_CpuLdAvg:%s() Initialization FAILURE",__func__);
		USA_TRACE_1("Initialization FAILURE");
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_ImmInitFailed);
		reportProblem();
		return ACS_USA_Error;
	}
	result = immHandle.getClassInstances(className, p_dnList);
	int instancesNumber  = p_dnList.size();

	USA_TRACE_1("Instances Found:[%d]",instancesNumber);

	if (instancesNumber == 0){
		USA_TRACE_LEAVE();
		return ACS_USA_Ok; // go with the default paramters
	}
	
	ACS_APGCC_ImmObject object;
	char *nameObject =  const_cast<char*>( p_dnList[0].c_str());
	object.objName = nameObject;
	result = immHandle.getObject(&object);
	for (unsigned int i = 0; i < object.attributes.size(); i++ ){
		for (unsigned int j = 0; j < object.attributes[i].attrValuesNum; j++){
			if ( (object.attributes[i].attrType == 9) || (object.attributes[i].attrType == 6)  ){
				char_value=(char*)(object.attributes[i].attrValues[j]);
				USA_TRACE_1("char_value -[%s]", char_value.data());
				if (object.attributes[i].attrName == String("disableAlarmFilter")){
					if (strcasecmp(char_value.data(), "yes") == 0){
						disableAlarmFilter = true;
					}
				}
				else {
					USA_TRACE_1("Ignoring charValue[%s]", char_value.data());
					traLD.traceAndLogFmt(ERROR, "ACS_USA_CpuLdAvg:%s() - Ignoring char_value[%s]", __func__, char_value.data());
				}
			}else if (object.attributes[i].attrType == 2){
				int_value=*(reinterpret_cast<unsigned int*>(object.attributes[i].attrValues[j]));
				setPerameter(object.attributes[i].attrName, int_value);
			}
		} // end of inner for
	}

	USA_TRACE_LEAVE();	
	return 0;
}
void ACS_USA_CpuLdAvg::reportProblem() const {

	USA_TRACE_ENTER();

	ACS_USA_ErrorType err = error.getError();
	String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
	String probableCause(ACS_USA_INTLFAULT);
	/*if (err == ACS_USA_APeventHandlingError)
	{
	}
	if (	(err == ACS_USA_SyntaxError) ||
		(err == ACS_USA_ParameterHandlingError) ||
		(err == ACS_USA_FileIOError))
	{
		// ACS_USA_ParameterHandlingError and ACS_USA_FileIOError added
		//  for raising Diagnostics fault alarm in these cases as well.
		perceivedSeverity = ACS_USA_PERCEIVED_SEVERITY_A2;
		probableCause = ACS_USA_DIAGFAULT;
	}*/
	if (eventManager.report(err + ACS_USA_FirstErrorCode,
				perceivedSeverity,
				probableCause,
				String(),
				String(error.getErrorText()),
				String(error.getProblemText(error.getError())),
				"",
				0) == ACS_USA_Error)
	{
	}

	USA_TRACE_LEAVE();

	return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ACS_USA_CpuLdAvg::monitorCpuLdAvg(){

	USA_TRACE_ENTER();
	
	ACS_USA_SpecificProblem specificProblem = ACS_USA_CPU_LOADAVG_SPECIFIC_PROBLEM;
	String probableCause(ACS_USA_INTLFAULT);
	String objectOfReference("acs_usad");
	String node;
	String problemText("PROBLEM\nCPU LOAD AVERAGE");
	String problemData("CPU Load Avg Threshold Reached");
	String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
	bool raiseNewAlarm = true;
	
	double load[3];
	if (getloadavg(load, 3) != -1) {
	//	 traLD.traceAndLogFmt(INFO, "%s() 1min load[%f] 5min load[%f] 15min load[%f]", __func__, (float)load[0], load[1], load[2]);
		if(load[2] > criticalThreshold) {
			//perceivedSeverity = String("A3");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "CPU Load Avg Critical Threshold Reached");
			problemData = String(buf);
			if(alarmRaised && (lastAlarmSeverity == CRITICAL_ALARM)) {
				raiseNewAlarm = false;
			}
			else {
				
				if(alarmRaised){
					ceaseAlarm = true;
				}
				else { 
					alarmRaised = true;
				}
				lastAlarmSeverity = CRITICAL_ALARM;
			}
			
		} else if(load[1] > majorThreshold) {
			//perceivedSeverity = String("O1");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "CPU Load Avg Major Threshold Reached");
			problemData = String(buf);
			if(alarmRaised && (lastAlarmSeverity == MAJOR_ALARM)) {
				raiseNewAlarm = false;
			}
			else {
				if(alarmRaised){
					ceaseAlarm = true;
				}
				else { 
					alarmRaised = true;
				}
				lastAlarmSeverity = MAJOR_ALARM;
			}
			
		} else if (load[0] > minorThreshold){
			//perceivedSeverity = String("O2");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "CPU Load Avg Minor Threshold Reached");
			problemData = String(buf);
			if(alarmRaised && (lastAlarmSeverity == MINOR_ALARM)) {
				raiseNewAlarm = false;
			}
			else {
				if(alarmRaised){
					ceaseAlarm = true;
				}
				else { 
					alarmRaised = true;
				}
				lastAlarmSeverity = MINOR_ALARM;
			}
			
		}
		else {
			if(alarmRaised) {
				perceivedSeverity = String(ACS_USA_PERCEIVED_SEVERITY_CEASING);	
				alarmRaised = false;
			}
			else {
				raiseNewAlarm = false;
			}
			
		}	
		if(ceaseAlarm){
			if (eventManager.report(specificProblem, String(ACS_USA_PERCEIVED_SEVERITY_CEASING), probableCause,
						objectOfReference, String("CPU Load Avg Threshold Reached"), problemText, node,
						0, false) == ACS_USA_Error) {
				USA_TRACE_1("Alarm Failed to  probableCause[%s]", probableCause.data());
			}
			else {
				ceaseAlarm = false;
			}

		}
		if(raiseNewAlarm) {
			if (eventManager.report(specificProblem, perceivedSeverity, probableCause,
						objectOfReference, problemData, problemText, node,
						0, false) == ACS_USA_Error) {
				USA_TRACE_1("Alarm Failed to  probableCause[%s]", probableCause.data());
			}
		}
	}
	else {
		 traLD.traceAndLogFmt(ERROR, "%s() Fail to getloadavg", __func__);
	}
	
	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_CpuLdAvg::handle_timeout(const ACE_Time_Value&, const void* ) {

	USA_TRACE_ENTER();

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());

	// Post a new ZMBE_TIMEOUT message to svc.
	if (mb != 0) {
		mb->msg_type(LDAG_TIMEOUT);
		if (this->putq(mb) < 0) {
			mb->release();
		}	
	}	
	// re-schedule the timer
	int rCode=0;
	const ACE_Time_Value schedule_time(cpuLoadAvgMonitorTimeInterval); // hardcoded for new. remove it later to shedule configurable interval time
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

	if (this->m_timerid < 0){
		traLD.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		rCode=-1;
	}

	USA_TRACE_LEAVE();
	return rCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
