//******************************************************************************
//
//
//******************************************************************************

#include <memory>
#include "acs_usa_analyser.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_analysisObject.h"
#include "acs_usa_error.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_logtrace.h"
#include "acs_usa_file.h"
#include "acs_usa_sysloganalyser.h"

#define eventTypeStringLength		30
#define logonTypeStringLength		28
#define eventRecordLength		8192

/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//	Trace point definitions
//******************************************************************************
ACS_USA_Trace traAR("ACS_USA_Analyser	");

// USA only analyses the events from the latest boot, to avoid failing over 
// several times between the nodes upon several fail over reqeusts.
//

//******************************************************************************
//	ACS_USA_Analyser()
//******************************************************************************
ACS_USA_Analyser::ACS_USA_Analyser() :
	recordBufferSize(0),
	logIndex(0),
	recordBuffer(0),
	sleepAwhile(ACS_USA_False),
	clearSecLogPerformed(ACS_USA_False),
	bootTimeWithinReach(ACS_USA_False),
	alarmSetOnMissedBootEvent(ACS_USA_False),
	oldestRecordNoInAnalysis(0),
	currentRecordNoInAnalysis(0),
	oldestRecordTimeInAnalysis(0),
	currentRecordTimeInAnalysis(0),
	m_nNumOfParams(0),
	m_syslogRecordsMax(0)
{
	USA_TRACE_ENTER2("Constructor");
 
	USA_TRACE_LEAVE2("Constructor");
}

//******************************************************************************
//	~ACS_USA_Analyser()
//******************************************************************************
ACS_USA_Analyser::~ACS_USA_Analyser()
{
	USA_TRACE_ENTER2("Destructor");
 
	USA_TRACE_LEAVE2("Destructor");
}

ACS_USA_ReturnType  ACS_USA_Analyser::getRecordTimeinSeconds(const char *record, ACS_USA_TimeType& recordTime)
{

	USA_TRACE_ENTER();

	regex_t regex;
	int reti;
	char msgbuf[100];
	char buf[100];
	regmatch_t pmatch;


	reti = regcomp(&regex, "([0-9]{4}\\-[0-9]{2}\\-[0-9]{2}\\ [0-9]{2}:[0-9]{2}:[0-9]{2})", REG_EXTENDED);
	if( reti ){ 
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() failed regcomp - Leaving", __func__);
		return ACS_USA_Error; 
	}
	/* Execute regular expression */
	reti = regexec(&regex, record, 1, &pmatch, 0);
	if( !reti ){
		memset(buf, 0, sizeof(buf));
		strncpy(buf, record+pmatch.rm_so, pmatch.rm_eo - pmatch.rm_so); 

		struct tm when;
		int yy = 0;
		int mm = 0;
		int dd = 0;
		int hh = 0;
		int m = 0;
		int ss = 0;
		if(sscanf(buf, "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &m, &ss) == 6)
		{

		when.tm_year = yy - 1900;
		when.tm_mon = mm - 1;
		when.tm_mday = dd;
		when.tm_sec = ss;
		when.tm_min = m;
		when.tm_hour = hh;
		when.tm_isdst = -1;// Force calculation of TZ+DST
	
		recordTime = mktime(&when);
		}
		else {
			traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - sscanf failed", __func__);
		}
		
	}
	else if( reti == REG_NOMATCH ){
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - failed regexec REG_NOMATCH", __func__);
		return ACS_USA_Error;
	}
	else{
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - failed regexec ", __func__);
		regerror(reti, &regex, msgbuf, sizeof(msgbuf));
		return ACS_USA_Error;
	}
	/* Free compiled regular expression if you want to use the regex_t again */
	regfree(&regex);

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;

}

//******************************************************************************
//	analyseObject()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Analyser::analyseObject(
		ACS_USA_AnalysisObject* object,
		ACS_USA_TimeType& latestBootTimeStamp,
		ACS_USA_TimeType alarmFilterInterval,
		const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
		const size_t recordSize,
		unsigned int syslogRecordsMax,
		ACS_USA_AnalysisType activation)

{
	USA_TRACE_ENTER();

	m_syslogRecordsMax=syslogRecordsMax;
	ACS_USA_ReturnType ret = ACS_USA_Ok;
	if(recordSize < 1024 || recordSize > 4096)
	{
		recordBufferSize = 2048;
	}
	else {
		recordBufferSize = recordSize;
	}
	
	bool isRebootLatest = false;
    
	if(activation == ACS_USA_Startup_Analysis)
	{
		findBootTime(latestBootTimeStamp, isRebootLatest);
	}

	if(!isRebootLatest)
	{
		// recordBuffer will hold one formatted record string at a time for analysis.
		try {
			recordBuffer = new ACS_USA_RecordBuffer[recordBufferSize];
		}
		catch(...) { // Catches "bad_alloc" exception 
			// (generated if stl headers are used)
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
			USA_TRACE_LEAVE2("new() failed in ACS_USA_Analyser::analyseObject().");
			return ACS_USA_Error;
		}
		if (recordBuffer == NULL) {	// If catching "bad_alloc" wasn't enough.
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Analyser::analyseObject().");
			USA_TRACE_LEAVE2("new() failed in ACS_USA_Analyser::analyseObject().");
			return ACS_USA_Error;	
		}

		ret = createLogFile(latestBootTimeStamp, ACS_USA_syslogFile, activation);
		if(ret == ACS_USA_Error)
		{
			USA_TRACE_LEAVE2("createLogFile Failed");
			delete [] recordBuffer;
			return ACS_USA_Error;
		}
		object->reset();

		//-------- Get records from log file and perform analysis --------
		//ACS_USA_TimeType stamp = ACS_USA_Time_error;
		ACS_USA_TimeType stamp = 0;
		ACS_USA_TimeType lastStamp = initialTime;
		ACS_USA_Flag notFinish = ACS_USA_True;
		ACS_USA_Flag swUpdate = ACS_USA_False;
		ACS_USA_ReturnType retCode = ACS_USA_Ok;

		// In this loop we read one event at the time and try
		// to match all the event against the criterion object.

		String logFileName(ACS_USA_syslogFile);
		FILE *logFp = fopen(logFileName.c_str(), "r");
		if(logFp == NULL)
		{
			USA_TRACE_LEAVE2("logFp Null found");
			delete [] recordBuffer;
			return ACS_USA_Error;
		}
		while(notFinish) 
		{

			switch( getRecord(logFp) )
			{
				case recordEOF: // End of file
					notFinish = ACS_USA_False;
					ret = ACS_USA_Ok;
					break;
				case recordOK: // Another record fetched.
					// printf("%s \n", recordBuffer);
					//traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - recordBuffer[ %s ]", __func__, recordBuffer);
					USA_TRACE_1("recordBuffer[ %s ]", recordBuffer);
					if(NULL != strstr(recordBuffer, "RPM activation")){
						swUpdate = ACS_USA_True;
						notFinish = ACS_USA_False; 
						traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - RPM activation found : %s", __func__,recordBuffer); 
					}
					else {
						retCode = getRecordTimeinSeconds(recordBuffer, stamp);

						if(retCode == ACS_USA_Error)
						{
							error.setError(ACS_USA_SyntaxError);
							traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - getRecordTimeinSeconds failed - %s ", __func__,recordBuffer); 
							notFinish = ACS_USA_False; 
						}
						if (notFinish == ACS_USA_True) 
						{
							latestBootTimeStamp = stamp;
							if (stamp != ACS_USA_Time_error) 
							{
								lastStamp = stamp;
								traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - lastSTamp value is %d", __func__,lastStamp);
							}
							// Use the default AnalysisObject
							//
							if( (NULL != strstr(recordBuffer, "SecurityEvent")) && (NULL != strstr(recordBuffer, "sshd")) )
							{
								//traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - sshd event received in SecurityEvent ", __func__); 
							}
							else if (object->match(recordBuffer, stamp, activation) == ACS_USA_Error) 
							{
								// Error handling.
								traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - object->match failed ", __func__);
								notFinish = ACS_USA_False; 
							}
						}
					}
					break;
				default: // Error occured.
					notFinish = ACS_USA_False; 
					break;
			} // switch.
		} // while.


		delete [] recordBuffer;

		fclose(logFp);


		if(swUpdate == ACS_USA_True) {
			object->reset();
		}
		else {
			// Indicates which params will be used when logging an new APEvent
			// -> TRUE : use the default i.e. from the ACF file.
			// or
			// -> FALSE: use the ones retrieved from the record info.
			bool bDefaultLogging = true;
			// We are under the Main object manager (main ACF file)

			// Check for alarm conditions in criteria.
			if (object->checkAlarms(alarmFilterInterval, suppressedAlarms, bDefaultLogging) == ACS_USA_Error) {
				// Handle error.
				USA_TRACE_LEAVE();
				return ACS_USA_Error;
			}
		}

		object->clearInstAlarmsQue(latestBootTimeStamp);

		if (sleepAwhile) 
		{
			// Is some application writing events to the logs at a 
			// crazy pace? If so, sleep a short while, to give the 
			// application a chance to cool down.
			usacc_msec_sleep(5000);
			sleepAwhile = FALSE;
		}
	}

	USA_TRACE_LEAVE();
	return ret;
}

ACS_USA_MatchType
ACS_USA_Analyser::analyseRecord( ACS_USA_AnalysisObject* object,
				ACS_USA_TimeType alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				ACS_USA_AnalysisType activation,
				ACS_USA_RecordBuffer *record)
{

	USA_TRACE_ENTER();

	ACS_USA_MatchType ret = ACS_USA_Error;
	ACS_USA_Time interpreter;
    
	//-------- Get records from log file and perform analysis --------
	//ACS_USA_TimeType stamp = ACS_USA_Time_error;
	ACS_USA_TimeType stamp = 0;
	ACS_USA_Flag notFinish = ACS_USA_True;
	ACS_USA_ReturnType retCode = ACS_USA_Ok;

	// In this loop we read one event at the time and try
	// to match all the event against the criterion object.

	retCode = getRecordTimeinSeconds(record, stamp);

	if(retCode == ACS_USA_Error)
	{
		error.setError(ACS_USA_SyntaxError);
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - getRecordTimeinSeconds failed ", __func__);
		notFinish = ACS_USA_False; 
	}
	if (notFinish == ACS_USA_True) 
	{

		// Use the default AnalysisObject
		if ((ret = object->matchRecord(record, stamp, activation)) == Criterion_matchError) 
		{
			// Error handling.
			traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - object->match failed ", __func__);
		}
	}
	
	if(ret == Criterion_match) {	
		bool bDefaultLogging = true;
		if (object->checkAlarms(alarmFilterInterval, suppressedAlarms, bDefaultLogging) == ACS_USA_Error) 
		{
			// Handle error.
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
	}

	USA_TRACE_LEAVE();

	return ret;

}

//******************************************************************************
//	getRecord()
//******************************************************************************


ACS_USA_RecordStatus
ACS_USA_Analyser::getRecord(FILE *fp ) {

	USA_TRACE_ENTER();

	char *retVal;
	ACS_USA_RecordStatus ret;

	retVal = fgets(recordBuffer, recordBufferSize, fp);
	if(retVal == NULL)
	{
		if(feof(fp)) {
			ret = recordEOF;
		}
		else{
			ret = recordError;
		}
	}
	else
	{
		ret = recordOK;
	}

	USA_TRACE_ENTER();
	return ret;
}

ACS_USA_ReturnType ACS_USA_Analyser::getSysBootTime(ACS_USA_TimeType& bootTime)
{
	ACS_USA_ReturnType rCode = ACS_USA_Ok;
	USA_TRACE_ENTER();

        FILE *fp;
        char buf[100];
        time_t bTime = 0;
        fp = fopen("/proc/stat", "r");

        if(fp != NULL)
        {
                while(fgets(buf, sizeof(buf), fp))
                {
                        if(sscanf(buf, "btime %lu", &bTime) == 1)
                                break;
                }
        	fclose(fp);
        }

        if(!bTime)
        {
		traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - bTime(%d)", __func__, bTime);
		rCode = ACS_USA_Error;		
        }
        else
        {
		bootTime = bTime;
        }
	USA_TRACE_LEAVE();
        return rCode;
}

ACS_USA_ReturnType ACS_USA_Analyser::createLogFile(ACS_USA_TimeType timeSec, const char *tempLogFile, ACS_USA_AnalysisType activation)
{
	USA_TRACE_ENTER(); 
	ACS_USA_ReturnType rCode = ACS_USA_Error;

	USA_TRACE_1("ACS_USA_Analyser:%s() - timeSec[%lu] ", __func__, timeSec);
	struct tm *pstTime; 
	struct tm stTime; 
	time_t ltime = timeSec; 
	pstTime = localtime_r(&ltime, &stTime); 

	if(pstTime != NULL) { 
		for (int precission = 0; precission <= 2; precission++)
		{
			rCode = findTimeFilter(pstTime, tempLogFile, precission, activation);
			if(rCode == ACS_USA_Ok ){
				break;
			}
		}
	}

	USA_TRACE_LEAVE();
	return rCode;
}

ACS_USA_ReturnType ACS_USA_Analyser::createTempLogFile(char *timeFilter , ACS_USA_AnalysisType activation)
{
	USA_TRACE_ENTER2("ACS_USA_Analyser:%s() - Filter[%s]", __func__, timeFilter);

	ACE_UINT32 CMD_LEN=1024;
	ACE_TCHAR command_string[CMD_LEN];
	int rCode;
	int analysisType = 2;
	if(activation == ACS_USA_Startup_Analysis) {
		analysisType = 1;
	}

	ACE_OS::memset(command_string ,0 , CMD_LEN);
	ACE_OS::snprintf(command_string, CMD_LEN, GET_EVNT_LIST_CMD "--records %u --time-stamp %s --analysis-type %d", m_syslogRecordsMax, timeFilter, analysisType);
	//ACE_OS::snprintf(command_string, CMD_LEN, GET_EVNT_LIST_CMD "-n %lu -t %s ", m_syslogRecordsMax, timeFilter);
	rCode = ACE_OS::system(command_string);
	if(rCode == -1) {
		traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - Failed to launch [%s]", __func__, command_string);
	}

	if (rCode != -1){
		rCode=WEXITSTATUS(rCode);
		if((rCode != 0) && (rCode != 1)) {
			traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - Failed to execute [%s]", __func__, command_string);
			rCode=-1;
		}
	}

	USA_TRACE_LEAVE();	
	if (rCode == -1)
		return ACS_USA_Error;
	else	
		return ACS_USA_Ok;
}
//------------------------------------------------------------------------------------------------------------------------

ACS_USA_ReturnType ACS_USA_Analyser::getTimeFilter(ACS_USA_TimeType bootTime, char *timeFilter, int Precessin)
{
	struct tm *pstTime;
	struct tm stTime;
	time_t ltime = bootTime;
	pstTime = localtime_r(&ltime, &stTime);

	//USA_TRACE_ENTER2("ACS_USA_Analyser:%s() - timeSec[%lu] timeFilter[%s] pstTime[%p]", __func__, bootTime, timeFilter, pstTime);
	USA_TRACE_ENTER();

	if((pstTime != NULL) && (timeFilter != NULL)) {
		if(Precessin == 0) {
			sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%02d", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday, 
				pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
		}
		else if (Precessin == 1) {
			sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%d[0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday, 
				pstTime->tm_hour, pstTime->tm_min, (int)pstTime->tm_sec/10);
		}
		else if(Precessin == 2){
			sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:[0-9][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday, 
				pstTime->tm_hour, pstTime->tm_min);
		}
		else {
			sprintf(timeFilter, "%04d-%02d-%02d_%02d:%d[0-9]:[0-9][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday, 
				pstTime->tm_hour, (int)pstTime->tm_min/10);
		}
		USA_TRACE_1("ACS_USA_Analyser:%s() - timeSec[%lu] timeFilter[%s] pstTime[%p]", __func__, bootTime, timeFilter, pstTime);
		USA_TRACE_LEAVE();
		return ACS_USA_Ok;
	}

	USA_TRACE_LEAVE();

	return ACS_USA_Error;
}

//******************************************************************************
//	findBootTime()
//******************************************************************************

// We want to handle the following situations:
// 1. The node was rebooted and many new events has been generated
//    before USA started this analysis.
//    ACTION: Use BootEvent as starting point for analysis. 
//    Delete the tmp file.
// 2. The USA service has been restarted (taken on-line)
//    and the boot event is not found among the
//    analysed events.
//    ACTION: Use ReStartedEvent as starting point.
// 3. Eventlog was cleared (both the above events were missing.)
//    ACTION: If previously stored time value in registry is available
//    we use that. Otherwise we use the current time as starting point.


ACS_USA_ReturnType 
ACS_USA_Analyser::findBootTime( ACS_USA_TimeType& latestBootTimeStamp, bool& isRebootLatest)
{
	USA_TRACE_ENTER();

	ACS_USA_ReturnType rCode = ACS_USA_Ok;
	ACS_USA_TimeType LastUSAstopTimeStamp = 0;
	ACS_USA_TimeType lastBootTimeStamp = 0;
	latestBootTimeStamp = 0;

	File file(USA_TIMESTAMP_FILE);
	if (file.isValid() == false) {
		traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - Error Opening USA Latest STOP Event file", __func__);
		USA_TRACE_LEAVE2("ACS_USA_Analyser:%s() - Error Opening USA Latest STOP Event file", __func__);
		return ACS_USA_Error;
	}
		
	if (!file.Read(LastUSAstopTimeStamp))
	{
		traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - Error in getting USA Latest STOP Event", __func__);
	}
	traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - USA Last STOP TimeStamp %d", __func__,LastUSAstopTimeStamp);

	if (LastUSAstopTimeStamp > 0){
		latestBootTimeStamp = LastUSAstopTimeStamp;
	}
	
	rCode = getSysBootTime(lastBootTimeStamp);	
	traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:%s() - bootTime %d", __func__,lastBootTimeStamp);
	if(rCode == ACS_USA_Error)
	{
		USA_TRACE_1("getSysBootTime: returned error" );
		traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:findBootTime() - ERROR ");
	}
	if( (latestBootTimeStamp != 0 ) && (lastBootTimeStamp > latestBootTimeStamp)){
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:findBootTime() Reboot is Latest");
		time((long*)&latestBootTimeStamp);
		isRebootLatest = true;
	}	
	if (latestBootTimeStamp == 0) {
		time((long*)&latestBootTimeStamp);
		isRebootLatest = true;
		traAR.traceAndLogFmt(INFO, "ACS_USA_Analyser:findBootTime() - considering USA Start Time:%d",latestBootTimeStamp);
	}
	
	// Make sure that we detect events logged the same sec as time stamp
	//latestBootTimeStamp--;
	

	USA_TRACE_LEAVE();	
	return ACS_USA_Ok;
}


bool ACS_USA_Analyser::isTimeFilterMatched(char *timeBuf, const char *tempLogFile, ACS_USA_AnalysisType activation)
{
	ACS_USA_ReturnType rCode = ACS_USA_Error;
	int retValue = -1;
	USA_TRACE_1("ACS_USA_Analyser:%s() - timeFilter[%s] ", __func__, timeBuf);
	if(NULL != timeBuf) {
		rCode = createTempLogFile(timeBuf, activation);
		if(rCode == ACS_USA_Error) {
			traAR.traceAndLogFmt(ERROR, "ACS_USA_Analyser:%s() - isTimeFilterMatched() Error timeBuf[%s]", __func__,timeBuf);
			return false;
		}

		struct stat fileStat;
		retValue = stat(tempLogFile, &fileStat);
		if((retValue == 0) &&(fileStat.st_size != 0)) {
			return true;
		}
	}
	return false;
}


ACS_USA_ReturnType ACS_USA_Analyser::findTimeFilter(struct tm *pstTime, const char *tempLogFile, const int Precessin, ACS_USA_AnalysisType activation)
{
        bool timeFilterMatched = false;
        ACS_USA_ReturnType rCode = ACS_USA_Error;

	USA_TRACE_1("ACS_USA_Analyser - tempLogFile[%s] Precessin[%d]",tempLogFile,Precessin);
        if(Precessin == 0) {
                timeFilterMatched = checkSecsTimeFilterMatched(SECONDS2, pstTime, tempLogFile, activation);
                if(!timeFilterMatched) {
                        timeFilterMatched = checkSecsTimeFilterMatched(SECONDS1, pstTime, tempLogFile, activation);
                }
        }
        else if(Precessin == 1) {
                timeFilterMatched = checkMinsTimeFilterMatched(MINUTS2, pstTime, tempLogFile, activation);
                if(!timeFilterMatched) {
                        timeFilterMatched = checkMinsTimeFilterMatched(MINUTS1, pstTime, tempLogFile, activation);
                }
        }
        else if(Precessin == 2) {
                timeFilterMatched = checkHoursTimeFilterMatched(HOURS2, pstTime, tempLogFile, activation);
                if(!timeFilterMatched) {
                        timeFilterMatched = checkHoursTimeFilterMatched(HOURS1, pstTime, tempLogFile, activation);
                }
        }

        if(timeFilterMatched){
                rCode = ACS_USA_Ok;
	}
	USA_TRACE_1("ACS_USA_Analyser -  Precessin[%d] rCode : %d ",Precessin, rCode);

        return rCode;
}

bool ACS_USA_Analyser::checkSecsTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation)
{
        char timeFilter[50] = {'\0'};
        bool secTimeFilterMatched = false;

        switch(type)
        {
                case SECONDS2:
                        {
                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%02d", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        secTimeFilterMatched = true;
                                }
                                else {
                                        int tempSec1 = pstTime->tm_sec/10;
                                        int tempSec2 = pstTime->tm_sec%10;

                                        while(tempSec2 > 0) {
                                                tempSec2 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%d%d", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                pstTime->tm_hour, pstTime->tm_min, tempSec1, tempSec2);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        secTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                }
                                break;
                        }
                case SECONDS1:
                        {

                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%d[0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec/10);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        secTimeFilterMatched = true;
                                }
                                else {
                                        int tempSec1 = pstTime->tm_sec/10;

                                        while(tempSec1 > 0) {
                                                tempSec1 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%d[0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                pstTime->tm_hour, pstTime->tm_min, tempSec1);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        secTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                        if(!secTimeFilterMatched) {
                                                int tempSec1 = pstTime->tm_sec/10;
                                                while(tempSec1 < 5) {
                                                        tempSec1 += 1;
                                                        sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:%d[0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                        pstTime->tm_hour, pstTime->tm_min, tempSec1);
                                                        if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                                secTimeFilterMatched = true;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                break;
                        }
                default:
                        break;
        }
        return secTimeFilterMatched;
}

bool ACS_USA_Analyser::checkMinsTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation)
{
        bool minTimeFilterMatched = false;
        char timeFilter[50] = {'\0'};

        switch(type)
        {
                case MINUTS2:
                        {
                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%02d:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour, pstTime->tm_min);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        minTimeFilterMatched = true;
                                }
                                else {
                                        int tempMin1 = pstTime->tm_min/10;
                                        int tempMin2 = pstTime->tm_min%10;

                                        while(tempMin2 > 0) {
                                                tempMin2 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%d%d:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                pstTime->tm_hour, tempMin1, tempMin2);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        minTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                }
                                break;
                        }
                case MINUTS1:
                        {
                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%d[0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour, pstTime->tm_min/10);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        minTimeFilterMatched = true;
                                }
                                else {
                                        int tempMin1 = pstTime->tm_min/10;

                                        while(tempMin1 > 0) {
                                                tempMin1 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:%d[0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                pstTime->tm_hour, tempMin1);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        minTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                        if(!minTimeFilterMatched) {
                                                int tempMin1 = pstTime->tm_min/10;
                                                while(tempMin1 < 5) {
                                                        tempMin1 += 1;
                                                        sprintf(timeFilter, "%04d-%02d-%02d_%02d:%d[0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                        pstTime->tm_hour, tempMin1 );
                                                        if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                                minTimeFilterMatched = true;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                break;
                        }
                default:
                        break;
        }
        return minTimeFilterMatched;
}

bool ACS_USA_Analyser::checkHoursTimeFilterMatched(timePrecs_t type, struct tm *pstTime, const char *tempLogFile, ACS_USA_AnalysisType activation)
{
        bool hrsTimeFilterMatched = false;
        char timeFilter[50] = {'\0'};

        switch(type)
        {
                case HOURS2:
                        {
                                sprintf(timeFilter, "%04d-%02d-%02d_%02d:[0-5][0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        hrsTimeFilterMatched = true;
                                }
                                else {
                                        int tempHrs1 = pstTime->tm_hour/10;
                                        int tempHrs2 = pstTime->tm_hour%10;

                                        while(tempHrs2 > 0) {
                                                tempHrs2 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%d%d:[0-5][0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                tempHrs1, tempHrs2);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        hrsTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                }
                                break;
                        }
                case HOURS1:
                        {
                                sprintf(timeFilter, "%04d-%02d-%02d_%d[0-3]:[0-5][0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                pstTime->tm_hour/10);
                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                        hrsTimeFilterMatched = true;
                                }
                                else {
                                        int tempHrs1 = pstTime->tm_hour/10;

                                        while(tempHrs1 > 0) {
                                                tempHrs1 -= 1;
                                                sprintf(timeFilter, "%04d-%02d-%02d_%d[0-3]:[0-5][0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                tempHrs1);
                                                if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                        hrsTimeFilterMatched = true;
                                                        break;
                                                }
                                        }
                                        if(!hrsTimeFilterMatched) {
                                                int tempHrs1 = pstTime->tm_hour/10;
                                                while(tempHrs1 < 3) {
                                                        tempHrs1 += 1;
                                                        sprintf(timeFilter, "%04d-%02d-%02d_%d[0-3]:[0-5][0-9]:[0-5][0-9]", pstTime->tm_year +1900, pstTime->tm_mon +1, pstTime->tm_mday,
                                                                        tempHrs1 );
                                                        if(isTimeFilterMatched(timeFilter, tempLogFile, activation)) {
                                                                hrsTimeFilterMatched = true;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                break;
                        }
                default:
                        break;

        }
        return hrsTimeFilterMatched;
}

