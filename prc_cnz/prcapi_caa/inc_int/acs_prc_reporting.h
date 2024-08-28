#ifndef REPORTING_H
#define REPORTING_H

#include	"acs_aeh_evreport.h"
#include	"acs_aeh_error.h"
#include	"acs_prc_reporting_types.h"
#include	<string>

// Return values
const int	ACS_PRC_Report_OK = 1;
const int	ACS_PRC_Report_NotOK = 0;

// Event probableCause, objClassOfReference and objectOfReference for
// event reporting
const char* const	ACS_PRC_probableCause = "AP INTERNAL FAULT";
const char* const	ACS_PRC_objClassOfReference = "APZ";
const char* const	ACS_PRC_objectOfReference = " ";
const char* const	ACS_PRC_WebServer_objOfRef = "Web Server";

// Event severity
const char* const       ACS_PRC_eventSeverity = "EVENT";

// Probable cause formatting
const char* const	causeHeader = "CAUSE";
const char* const	dateHeader = "DATE";
const char* const	timeHeader = "TIME";

const int		maxAlarmLen = 1024;
const int		maxCauseLen = 1024;
const char* const	dateConversion = "%02d%02d%02d";
const char* const	timeConversion = "%02d%02d%02d";
//const char* const	alarmFormat = "%-25s%-9s%-9s\r\n%-25s%-9s%-9s";
const char* const	alarmFormat = "%-25s%-9s%-9s\n%-25s%-9s%-9s";
const char* const	alarmFormat_APBackup = "%-16s%-6s\n%-16s%-6s";
//const char* const	causeFormat = "%-100s\r\n%-100s";
const char* const	causeFormat = "%-100s\n%-100s";
const char* const	causeFormat_APBackup = "%-512s\n%-512s";
const char* const	unknownDate = "";
const char* const	unknownTime = "";

// Reporting event message under debug mode
const char* const	reportingEvent = "REPORTING EVENT: ";

// Log constants
const std::string dateLine = __DATE__;
const std::string timeLine = __TIME__;
const char* const nodeLogs = "acsnslogs";
const char* const logFile = "ACS_PRC_error";
const char* const oldLogFile = "ACS_PRC_error.old";
const int maxLogFileSize = 1000000;

class acs_prc_report
{
public:
	acs_prc_report();
	// Description:
	//      Contructor
	// Parameters:
	//      None
	// Return value:
	//      None
	// Additional information:
	//      None

	~acs_prc_report();
	// Description:
	//      Destructor
	// Parameters:
	//      None
	// Return value:
	//      None
	// Additional information:
	//      None

	int	event( const acs_aeh_specificProblem specificProblem,
		const acs_aeh_problemData problemData,
		const acs_aeh_problemText problemText);
	// Description:
	//	Report an event by means of the ACS_AEH_EvReport API.
	// Parameters:
	//	specificProblem	The error code number of the problem;
	//	problemData	Free text description to be logged;
	//	problemText	Free text description to be printed;
	// Return value:
	//	ACS_PRC_Errors_ok
	//			The event has successfully reported to AEH
	// 	ACS_PRC_Errors_notOk
	//			The event is not reported to AEH
	// Additional information:
	//	None

	int	alarm(  const acs_aeh_specificProblem specificProblem,
		const char* const alarm,
		const acs_aeh_percSeverity percSeverity,
		const acs_aeh_problemData problemData,
		const acs_aeh_problemText problemText,
		const char* const objOfRef,
		const char* const processInfo,
		bool manualcease = false);
	// Description:
	//	Sends an AXE alarm by means of the ACS_AEH_EvReport API.
	// Parameters:
	//	specificProblem	The error code number of the problem; in.
	//	percSeverity	The severity level of the problem; in.
	//	problemData	Free text description to be logged; in.
	//	problemText	Free text description to be printed; in.
	//	objOfRef	Free text description to be printed; in.
	//  processInfo Free text description to be printed; in.
	// Return value:
	//	ACS_PRC_Errors_ok
	//			The alarm has successfully reported to AEH
	// 	ACS_PRC_Errors_notOk
	//			The alarm has not reported to AEH but
	//			written on standard output
	// Additional information:
	//	None

	int	alarm_APBackup(  const acs_aeh_specificProblem specificProblem,
		const char* const alarm,
		const acs_aeh_percSeverity percSeverity,
		const acs_aeh_problemData problemData,
		const acs_aeh_problemText problemText,
		const char* const objOfRef,
		const char* const processInfo,
		bool manualcease = false);

	int alarmAPNotRedundant(const acs_aeh_percSeverity percSeverity,
							const acs_aeh_problemText problemText,
							const char* nodeName);

	int alarmFCHipCause(const acs_aeh_percSeverity percSeverity,
						const acs_aeh_problemText problemText,
						const char* nodeName);

	int alarmFCHfailCause(const acs_aeh_percSeverity percSeverity,
						  const acs_aeh_problemText problemText,
						  const char* nodeName);

	int	alarmAPReboot(const acs_aeh_problemText problemText,
					  const acs_aeh_percSeverity percSeverity = ACS_PRC_o1Severity);

	int	alarmAPBackup(const acs_aeh_problemText problemText,
					  const acs_aeh_percSeverity percSeverity = ACS_PRC_o1Severity);

	int	alarmAPFuncNotAvailable(const acs_aeh_problemText problemText,
					  const acs_aeh_percSeverity percSeverity = ACS_PRC_o1Severity);

	int	alarmAPGBackup(const acs_aeh_problemText problemText,
					  const acs_aeh_percSeverity percSeverity = ACS_PRC_o1Severity);

	std::string getErrorText();
	ACS_AEH_ErrorType getError();

private:

	std::string errorText;
	ACS_AEH_ErrorType error;
	acs_aeh_evreport eventReport;
};
#endif // REPORTING_H
