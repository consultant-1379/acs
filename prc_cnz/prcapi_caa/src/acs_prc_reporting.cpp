#include "acs_prc_reporting.h"
#include <string.h>
#include <sstream>
#include <errno.h>
#include <stdio.h>

extern char *program_invocation_name;
extern char *program_invocation_short_name;

using namespace std;

acs_prc_report::acs_prc_report():error(ACS_AEH_noErrorType){
}

acs_prc_report::~acs_prc_report(){
}

int acs_prc_report::alarm(const acs_aeh_specificProblem specificProblem,
					  const char* const alarm,
					  const acs_aeh_percSeverity percSeverity,
					  const acs_aeh_problemData problemData,
					  const acs_aeh_problemText problemText,
					  const char* const objOfRef,
					  const char* const processInfo,
					  bool manualcease)
{
	string			processName = program_invocation_short_name;
	char			dateOfAlarm[maxStringLen] = {0};
	char			timeOfAlarm[maxStringLen] = {0};
	char			formattedProblemText[maxCauseLen] = {0};
	char			formattedAlarmText[maxAlarmLen] = {0};

	time_t ftime = time(NULL);
	struct tm * timeinfo = localtime ( &ftime );

	int year = timeinfo->tm_year % 100;

	snprintf ( dateOfAlarm,maxStringLen,dateConversion,year,timeinfo->tm_mon+1, timeinfo->tm_mday);
	snprintf ( timeOfAlarm, maxStringLen,timeConversion,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

	snprintf(formattedAlarmText, maxStringLen,alarmFormat, causeHeader, dateHeader, timeHeader, problemText, dateOfAlarm, timeOfAlarm);

	if (strcmp(processInfo,"")!=0){
		sprintf(formattedProblemText,causeFormat,processInfo,formattedAlarmText);
	}
	else {
		strcpy(formattedProblemText,formattedAlarmText);
	}

	if (eventReport.sendEventMessage(processName.data(),
				 specificProblem,
				 percSeverity,
				 alarm,
				 ACS_PRC_objClassOfReference,
				 objOfRef,
				 problemData,
				 formattedProblemText,manualcease) != ACS_AEH_ok ) {

		errorText = eventReport.getErrorText();
		error = eventReport.getError();

		return ACS_PRC_Report_NotOK;
	}

	return ACS_PRC_Report_OK;
}

int acs_prc_report::alarmAPNotRedundant(const acs_aeh_percSeverity percSeverity,
										const acs_aeh_problemText problemText,
										const char* nodeName)
{
	char	nodeInfo[maxStringLen] = {0};

	snprintf(nodeInfo,maxStringLen,nodeFormat,notavailable,nodeName);

	return alarm(ACS_PRC_NotRedundantWarning,ACS_PRC_apNotRedundant,
				 percSeverity,ACS_PRC_NotRedundantWarningText,
				 problemText,ACS_PRC_objectOfReference,nodeInfo);
}

int acs_prc_report::event (	const acs_aeh_specificProblem specificProblem, const acs_aeh_problemData problemData, const acs_aeh_problemText problemText ){

	string			processName;

	// Appends the pid of the process
	//getCurrentProcName(processName);

	// Sends the event to AEH
	if ((eventReport.sendEventMessage(processName.data(),
		specificProblem,ACS_PRC_eventSeverity,
		ACS_PRC_probableCause,ACS_PRC_objClassOfReference,
		ACS_PRC_objectOfReference,problemData,
		problemText)) != ACS_AEH_ok)
	{
		errorText = eventReport.getErrorText();
		error = eventReport.getError();
		return ACS_PRC_Report_NotOK;
	}

	// The event has successfully sent.
	return ACS_PRC_Report_OK;
} // End of event

ACS_AEH_ErrorType acs_prc_report::getError(){
	return error;
}

string acs_prc_report::getErrorText(){

	return errorText;
}

int acs_prc_report::alarmFCHipCause(const acs_aeh_percSeverity percSeverity,
										const acs_aeh_problemText problemText,
										const char* nodeName)
{
	char nodeInfo[maxStringLen] = {0};

	snprintf(nodeInfo,maxStringLen,nodeFormat,notavailable,nodeName);

	return alarm(ACS_PRC_FCHstartedSP,ACS_PRC_FCHipCause,
				 percSeverity,ACS_PRC_FCHstartedSPText,
				 problemText,ACS_PRC_objectOfReference,nodeInfo);
}

int acs_prc_report::alarmFCHfailCause(const acs_aeh_percSeverity percSeverity,
										const acs_aeh_problemText problemText,
										const char* nodeName)
{
	char nodeInfo[maxStringLen] = {0};

	snprintf(nodeInfo,maxStringLen,nodeFormat,notavailable,nodeName);

	return alarm(ACS_PRC_FCHfailedSP,ACS_PRC_FCHfailCause,
				 percSeverity,ACS_PRC_FCHfailedSPText,
				 problemText,ACS_PRC_objectOfReference,nodeInfo);
}

int	acs_prc_report::alarmAPReboot(const acs_aeh_problemText problemText, const acs_aeh_percSeverity percSeverity)
{
	int APReboot_Specific_Problem = ACS_PRC_RebootedWarningO1;

	if ( strncmp ( percSeverity, ACS_PRC_a2Severity, sizeof(ACS_PRC_a2Severity)) == 0 ){
		APReboot_Specific_Problem = ACS_PRC_RebootedWarningA2;
	}

	return alarm(APReboot_Specific_Problem,ACS_PRC_apReboot,
				 percSeverity,ACS_PRC_RebootedWarningText,
				 problemText,ACS_PRC_objectOfReference,unknown,true);
}

int	acs_prc_report::alarmAPBackup(const acs_aeh_problemText problemText, const acs_aeh_percSeverity percSeverity)
{
	return alarm_APBackup(ACS_PRC_FCHmissingBackup,ACS_PRC_apBackup,
						  percSeverity,ACS_PRC_FCHmissingBackupText,
						  problemText,ACS_PRC_objectOfReference,unknown,false);
}

int acs_prc_report::alarm_APBackup(const acs_aeh_specificProblem specificProblem,
								   const char* const alarm,
								   const acs_aeh_percSeverity percSeverity,
								   const acs_aeh_problemData problemData,
								   const acs_aeh_problemText problemText,
								   const char* const objOfRef,
								   const char* const /*processInfo*/,
								   bool manualcease) {

	if (eventReport.sendEventMessage(program_invocation_short_name,
				 specificProblem,
				 percSeverity,
				 alarm,
				 ACS_PRC_objClassOfReference,
				 objOfRef,
				 problemData,
				 problemText,manualcease) != ACS_AEH_ok ) {

		errorText = eventReport.getErrorText();
		error = eventReport.getError();

		return ACS_PRC_Report_NotOK;
	}

	return ACS_PRC_Report_OK;
}

int	acs_prc_report::alarmAPFuncNotAvailable(const acs_aeh_problemText problemText, const acs_aeh_percSeverity percSeverity) {

	if (eventReport.sendEventMessage(program_invocation_short_name, ACS_PRC_APFuncNotAvailable, percSeverity, ACS_PRC_apFuncNotAvailable,
				 ACS_PRC_objClassOfReference, ACS_PRC_WebServer_objOfRef, ACS_PRC_APFuncNotAvailableText, problemText, false) != ACS_AEH_ok) {

		errorText = eventReport.getErrorText();
		error = eventReport.getError();
		return ACS_PRC_Report_NotOK;
	}

	return ACS_PRC_Report_OK;
}

int	acs_prc_report::alarmAPGBackup(const acs_aeh_problemText problemText, const acs_aeh_percSeverity percSeverity){

	if (eventReport.sendEventMessage(program_invocation_short_name, ACS_PRC_APG_Backup_Missing, percSeverity, ACS_PRC_apgBackup,
				 ACS_PRC_objClassOfReference, ACS_PRC_objectOfReference, ACS_PRC_APG_Backup_MissingText, problemText, false) != ACS_AEH_ok) {

		errorText = eventReport.getErrorText();
		error = eventReport.getError();
		return ACS_PRC_Report_NotOK;
	}

	return ACS_PRC_Report_OK;
}
