
// Header Template


#include <sstream>
#include <ace/Reactor.h>

#include "acs_usa_global.h"
#include "acs_usa_zombiemonitor.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_error.h"

ACS_USA_Trace traZM ("ACS_USA_ZombieMon		");	

#define ACS_USA_ZOMBIE_SPECIFIC_PROBLEM 8721
const unsigned int ACS_USA_DefaultMonitorInterval     = 300; // seconds
const unsigned int ACS_USA_DefaultCritical     = 10; 
const unsigned int ACS_USA_DefaultMajor     = 15; 
const unsigned int ACS_USA_DefaultMinor     = 20; 

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

ACS_USA_ZombieMon::ACS_USA_ZombieMon(){

	USA_TRACE_ENTER();

	
	lastAlarmTime = 0;
	alarmRaised = false;
	ceaseAlarm = false;
	disableAlarmFilter = false;
	lastAlarmSeverity = NO_ALARM;
	zombieMonitorTimeInterval = ACS_USA_DefaultMonitorInterval;
	maxZombiesMinor = ACS_USA_DefaultMinor;
	maxZombiesMajor = ACS_USA_DefaultMajor;
	maxZombiesCritical = ACS_USA_DefaultCritical;
	m_timerid		= -1;
	m_global_instance	= ACS_USA_Global::instance();	

	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_ZombieMon::~ACS_USA_ZombieMon(){

	USA_TRACE_ENTER();

	USA_TRACE_LEAVE();

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_ZombieMon::open(){

	USA_TRACE_ENTER();

	//syslog(USA_TRACE_1FO, "ACS_USA_ZombieMon:%s() invoked", __func__);
	// perform any initialization tasks
	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		traZM.traceAndLogFmt(ERROR, "%s() - Failed to start main svc thread.", __func__);
		USA_TRACE_LEAVE();
	       	return -1;
	}

	// schedule a timer for zombie montior periodic intervel
	/*const ACE_Time_Value schedule_time(900); // hardcoded for new. remove it later to shedule configurable interval time
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

	if (this->m_timerid < 0){
		traZM.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		USA_TRACE_LEAVE();
		return -1;
	}*/
	USA_TRACE_LEAVE();
	return 0;	
}	

ACS_USA_ReturnType ACS_USA_ZombieMon::setPerameter(String paramName, int value)
{
	USA_TRACE_ENTER();

	traZM.traceAndLogFmt(INFO, "ACS_USA_ZombieMon:%s() - paramName[%s] value = %d", __func__, paramName.data(), value);
	if (paramName == String("zombieMonitorInterval")) {
		zombieMonitorTimeInterval = value;
	}else	if(paramName == String("minor")) {
		maxZombiesMinor = value;
	} else if(paramName == String("major")) {
		maxZombiesMajor = value;
	} else if(paramName == String("critical")) {
		maxZombiesCritical = value;
	} else {
		// Ignore the rest of paramters
		USA_TRACE_1("Ignoring paramName[%s]", paramName.data());
		traZM.traceAndLogFmt(ERROR, "ACS_USA_ZombieMon:%s() - Ignoring paramName[%s]", __func__, paramName.data());
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}
int ACS_USA_ZombieMon::initialize()
{
	USA_TRACE_ENTER();

	char *className = const_cast<char*>("USAZombieConfig");
	String char_value;
	unsigned int int_value=0;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	OmHandler immHandle;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		traZM.traceAndLogFmt(ERROR,"ACS_USA_Control:%s() Initialization FAILURE",__func__);
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
					traZM.traceAndLogFmt(ERROR, "ACS_USA_ZombieMon:%s() - Ignoring char_value[%s]", __func__, char_value.data());
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
void ACS_USA_ZombieMon::reportProblem() const {

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

int ACS_USA_ZombieMon::svc(){

	USA_TRACE_ENTER();

	ACE_Message_Block* mb = NULL;
	bool running = true;

	USA_TRACE_1("ACS_USA_ZombieMon:svc() - thread running");

	initialize();
	if(disableAlarmFilter) {
		traZM.traceAndLogFmt(INFO, "disableAlarmFilter set Exiting CPU Loadavg Thread.....!");
		return 0;
	}

	this->monitorZombie();
	// schedule a timer for zombie montior periodic intervel
        const ACE_Time_Value schedule_time(zombieMonitorTimeInterval); // hardcoded for new. remove it later to shedule configurable interval time
        m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

        if (this->m_timerid < 0){
                traZM.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
                USA_TRACE_LEAVE();
                return -1;
        }


	while (running){
		
		try {
			if (this->getq(mb) < 0){
				traZM.traceAndLogFmt(ERROR, "ERROR: getq() failed");
				USA_TRACE_1("ERROR: getq() failed");
				break;
			}

			// Check msg type
			
			switch(mb->msg_type()){
				
				case ZMBE_TIMEOUT:
						//traZM.traceAndLogFmt(INFO, "INFO - Zomibie timeout recevied");
						USA_TRACE_1("INFO - Zomibie timeout recevied");
						this->monitorZombie();
						mb->release();
						break;
				
				case ZMBE_CLOSE:
						traZM.traceAndLogFmt(INFO, "INFO - Shutdown received");
						USA_TRACE_1("INFO - Shutdown received");
						mb->release();
						running=false;
						break;
						
				default:
						traZM.traceAndLogFmt(INFO, "WARNING - not handled message received: %i", mb->msg_type());
						USA_LOG_WA("WARNING - not handled message received: %i",mb->msg_type());
						mb->release();
						running=false;
						break;			

			} // end of switch	
		}// end of try

		catch(...) {
			traZM.traceAndLogFmt(ERROR, "ERROR: EXCEPTION!");
			USA_TRACE_1("ERROR: EXCEPTION!");
		}
	}// end of while

	//call 'close(u_long)' at exit of 'svc'
	USA_TRACE_LEAVE();
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_ZombieMon::close(u_long) {

	USA_TRACE_ENTER();

	ACE_Time_Value timeout ();

	traZM.traceAndLogFmt(INFO, "close(u_long) - INFO: Closing down.");
	USA_TRACE_1("close(u_long) - INFO: Closing down.");

	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered() && (!disableAlarmFilter)) {
		traZM.traceAndLogFmt(ERROR, "%s(u_long) Abnormal shutdown of Zombie Monitor", __func__);
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

int ACS_USA_ZombieMon::close(){

	USA_TRACE_ENTER();
	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());

	int rCode=0;
	syslog(LOG_INFO, "ACS_USA_ZombieMon::%s() invoked", __func__);

	if (0 == mb) {
		traZM.traceAndLogFmt(INFO, "%s() Failed to create mb object", __func__);
		USA_TRACE_1("Failed to create mb object");
		rCode= -1;
	}
	if (rCode != -1){
		mb->msg_type(ZMBE_CLOSE);
		if (this->putq(mb) < 0){
			traZM.traceAndLogFmt(ERROR, "%s() Fail to post ZMBE_CLOSE to ourself", __func__);
			USA_TRACE_1("Fail to post ZMBE_CLOSE to ourself");
			mb->release();
			rCode=-1;
		}
	}
	USA_TRACE_LEAVE();
	return rCode;		
}
int ACS_USA_ZombieMon::getZombies(unsigned int &zombies, vector<String>& zombieList) {
	USA_TRACE_ENTER();	
	struct dirent *ent;
	DIR *proc;
	char tbuf[32];
	char *cp;
	char *cpp;
	int fd;
	char c;

	zombies = 0;
	if((proc=opendir("/proc"))==NULL) {
		perror("opendir");
		return -1;
	} 

	while(( ent=readdir(proc) )) {
		if (!isdigit(ent->d_name[0])) continue;
		sprintf(tbuf, "/proc/%s/stat", ent->d_name);

		fd = ::open(tbuf, O_RDONLY, 0);
		if (fd == -1) continue;
		memset(tbuf, '\0', sizeof tbuf); // didn't feel like checking read()
		::read(fd, tbuf, sizeof tbuf - 1); // need 32 byte buffer at most
		::close(fd);

		cp = strrchr(tbuf, ')');
		if(!cp) continue;
		c = cp[2];

		if (c=='Z') {
			zombies++;
			cpp = strchr(tbuf, '(');
			if(!cpp){
				continue;
			}
			else
			{
				cpp++;
				*(cp) = '\0';
				bool processFound = false;
				for(unsigned int count=0; count < zombieList.size(); count++) {
					if(zombieList[count] == String(cpp))
					{
						processFound = true;
						break;
					}
				}
				if(!processFound)
					zombieList.push_back(String(cpp));
			}
			continue;
		}
	}
	closedir(proc);
	USA_TRACE_ENTER();	
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ACS_USA_ZombieMon::monitorZombie(){

	USA_TRACE_ENTER();	
	// do rquired stuff
	unsigned int zombiesCount = 0;
	vector<String> zombieNameList;
	getZombies(zombiesCount, zombieNameList);
	ACS_USA_SpecificProblem specificProblem = ACS_USA_ZOMBIE_SPECIFIC_PROBLEM;
	String probableCause(ACS_USA_INTLFAULT);
	String objectOfReference("acs_usad");
	String node;
	String problemText("PROBLEM\nZOMBIE PROCESS");
	String problemData("Zombie process Threshold Reached");
	String perceivedSeverity("A1");
	bool raiseNewAlarm = true;
	//traZM.traceAndLogFmt(INFO, "%s(): zombiesCount = %u", __func__, zombiesCount);
	if(zombiesCount >= maxZombiesMinor) {
		traZM.traceAndLogFmt(INFO, "%s(): zombiesCount = %u", __func__, zombiesCount);
		traZM.traceAndLogFmt(INFO, "%s(): zombieNameList.size = %u", __func__, zombieNameList.size());
		/*problemData.append("Process List[ ");
		if(zombieNameList.size() > 0)
		{
			for(unsigned int count=0; count < zombieNameList.size(); count++) {
				problemData.append(zombieNameList[count]);
				problemData.append(",");
			}
			problemData.append("]");
		}*/

		perceivedSeverity = String(ACS_USA_PERCEIVED_SEVERITY_EVENT);
		if((zombiesCount >= maxZombiesMinor) && (zombiesCount < maxZombiesMajor)) {
	//		perceivedSeverity = String("O2");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "Zombie Process Minor Threshold Reached");
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

		} else if ((zombiesCount >= maxZombiesMajor) && (zombiesCount < maxZombiesCritical)){
	//		perceivedSeverity = String("O1");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			//sprintf(buf, "Zombie Process Major[%u] Threshold Reached", zombiesCount);
			sprintf(buf, "Zombie Process Major Threshold Reached");
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
		}
		else if(zombiesCount>= maxZombiesCritical){
	//		perceivedSeverity = String("A3");
			char buf[64];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "Zombie Process Critical Threshold Reached");
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
		}

		if(ceaseAlarm){
			if (eventManager.report(specificProblem, String(ACS_USA_PERCEIVED_SEVERITY_CEASING), probableCause,
						objectOfReference, String("Zombie Process Threshold Reached"), 
						problemText, node,
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

		if(alarmRaised) {
			perceivedSeverity = String(ACS_USA_PERCEIVED_SEVERITY_CEASING);
			if (eventManager.report(specificProblem, perceivedSeverity, probableCause,
					objectOfReference, problemData, problemText, node,
					0, false) == ACS_USA_Error) {
				USA_TRACE_1("Alarm Failed to  probableCause[%s]", probableCause.data());
			}
			alarmRaised = false;
			ceaseAlarm = false;
			lastAlarmSeverity = NO_ALARM;
		}
	}
	
	USA_TRACE_LEAVE();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ACS_USA_ZombieMon::handle_timeout(const ACE_Time_Value&, const void* ) {

	USA_TRACE_ENTER();

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN(mb, ACE_Message_Block());
	int rCode=0;

	//traZM.traceAndLogFmt(INFO, "%s(): ZombieMon: handle_timeout", __func__);

	// Post a new ZMBE_TIMEOUT message to svc.
	if (mb != 0) {
		mb->msg_type(ZMBE_TIMEOUT);
		if (this->putq(mb) < 0) {
			mb->release();
		}	
	}	

	// re-schedule the timer
	const ACE_Time_Value schedule_time(zombieMonitorTimeInterval);
	m_timerid = ACS_USA_Global::instance()->reactor()->schedule_timer(this, 0, schedule_time);

	if (this->m_timerid < 0){
		traZM.traceAndLogFmt(ERROR, "%s() - Unable to schedule timer.", __func__);
		rCode=-1;
	}

	USA_TRACE_LEAVE();
	return rCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
