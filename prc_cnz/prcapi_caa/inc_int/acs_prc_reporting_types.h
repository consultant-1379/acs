#ifndef ACS_PRC_H
#define ACS_PRC_H

#include "acs_prc_reporting.h"
#include "acs_aeh_signalhandler.h"

// Arguments
const char* const	installation="install";
const char* const	removal="remove";

// service name
const char* const	ACS_PRC_ClusCtrlService="ACS_PRC_ClusterControl";

// Parameters
const char* const ACS_PRC_mancxcName = "ACS/CXC1371327";
const char* const ACS_PRC_supservTableName = "ACS_PRCMAN_SupServiceTable";
const char* const ACS_PRC_PRCEventSource   = "ACS_PRC_EventSource";

// environment variable
const char* const	ap_home="AP_HOME";

// directory
const char* const	sys="\\ACS\\sys\\";

// ISP logdirectory and files
const char* const prclog="\\ACS\\logs\\PRC\\";
const char* const ispLogFileName="ISP_log.dat";
const char* const ispTimeFileName="ISP_time.dat";
const char* const ispStartEntry="ISP_start";

// key to AP logs/data path
const char* const	AP="SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\common";
// key to PRC registry
const char* const	PRCBIN="SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\PRC";
// key to Force registry
const char* const	FORCE="HKEY_LOCAL_MACHINE\\SOFTWARE\\FORCE Computers\\LBB\\MOVEGROUP";

// filenames and entrynames
const char* const	rebootCauseFile="ACS_PRC_RebootCause";
const char* const	rebootStampFile="ACS_PRC_SystemBootTime";
const char* const	endRebootEntry="ACS_PRC_Reboot_End";
const char* const	startRebootEntry="ACS_PRC_Reboot_Start";
const char* const	rebootCountEntry="ACS_PRC_Reboot_Count";
const char* const	acsaplogs="acsaplogs";

// reboot causes
const char* const	failover="failover";
const char* const	fault="fault";
const char* const	fch="fch";
const char* const	sfc="sfc";
const char* const	user="user";
const char* const	event="event";
const char* const	unknown="";
const char* const	up="up";
const char* const	down="down";
const char* const	manual="manualreboot";

const int	pathlen=512;	// path lenght
const int	linelen=256;	// line length
const int	nodelen=32;		// node name length
const int	rglen=64;		// resource group name length
const int	reslen=128;		// resource name lenght
const int	prclen=32;		// Lengt used for misc information

// number af move group attempts
const int	moveAttempts=10;

// allowed time (s) for passive node to be gone
const int	nodeDownAllowedTime = 900; // 15 min

// Strings boundaries
const int		maxStringLen = 1024;
const int		maxPathLen = 512;

// String constants
const char* const	pidSeparator = ":";
const char* const	separator = ":";
const char* const	pidConversion = "%ld";
const char* const	intConversion = "%d";
const char* const	slashString = "/";
const char* const	nullString = "";
const char* const	spaceString = " ";
const char* const	tabString = "\t";
const char* const	zeroString = "0";
const char* const	equalString = "=";

// Char constants
const char		pathSeparator = '/';
const char		optSeparator = ' ';

// Alarms

const char* const	ACS_PRC_apNotRedundant="AP NOT REDUNDANT";
const char* const	ACS_PRC_FCHipCause="AP FUNCTION CHANGE IN PROGRESS";
const char* const	ACS_PRC_FCHfailCause="AP FUNCTION CHANGE FAILED";
const char* const	ACS_PRC_apReboot="AP REBOOT";
const char* const	ACS_PRC_apBackup="AP BACKUP NOT CREATED";
const char* const	ACS_PRC_apgBackup="APG BACKUP NOT CREATED";
const char* const	ACS_PRC_apFuncNotAvailable = "AP FUNCTION NOT AVAILABLE";

// Alarm severities
const char* const	ACS_PRC_a1Severity="A1";
const char* const	ACS_PRC_a2Severity="A2";
const char* const	ACS_PRC_a3Severity="A3";
const char* const	ACS_PRC_o1Severity="O1";
const char* const	ACS_PRC_ceaseSeverity="CEASING";

// Alarm causes
const char* const	ACS_PRC_function="Function Change";
const char* const	ACS_PRC_up="Node is up";
const char* const	ACS_PRC_down="Node is down";
const char* const	ACS_PRC_degraded="Node is degraded";
const char* const	ACS_PRC_evstop = "Stopped by event";
const char* const	ACS_PRC_user="Command initiated";
const char* const	ACS_PRC_fault="Fault initiated";

// Alarm texts
const char* const	processFormat = "%-25s%-25s\n%-25s%-25s";
const char* const	resourcegroup="RESOURCE GROUP";
const char* const	process="PROCESS";
const char* const	nodeFormat = "%-25s\n%-25s";
const char* const	notavailable="NODE NOT AVAILABLE";


// Error and Event texts, data and numbers: Range 6000 - 6099
const int			ACS_PRC_RebootedWarningO1=6012;
const int			ACS_PRC_RebootedWarningA2=6013;
const char* const	ACS_PRC_RebootedWarningText="The system node has been rebooted";

const int			ACS_PRC_NotRedundantWarning=6035;
const char* const	ACS_PRC_NotRedundantWarningText="The passive node is down or degraded.";

const int 			ACS_PRC_FCHstartedSP = 8802;	// FCH start alarm code
const char* const 	ACS_PRC_FCHstartedSPText = "AP Function Change initiated.";
const int 			ACS_PRC_FCHfailedSP	= 8803;	// FCH failed alarm code
const char* const 	ACS_PRC_FCHfailedSPText = "AP Function Change has failed.";
const int 			ACS_PRC_FCHmissingBackup	= 8822;	// FCH failed alarm code
const char* const 	ACS_PRC_FCHmissingBackupText = "Backup not created after latest Software Update";
const int 			ACS_PRC_APG_Backup_Missing	= 8823;	// FCH failed alarm code
const char* const 	ACS_PRC_APG_Backup_MissingText = "Backup not created after latest Model Change";
const int 			ACS_PRC_APFuncNotAvailable = 8723; // Web Server stopped alarm code
const char * const	ACS_PRC_APFuncNotAvailableText = "Web Server is down. Manually stopped.";

#endif
