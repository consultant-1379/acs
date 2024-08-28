//******************************************************************************
//
// NAME
//  	  ACS_USA_Control - Controls all activities in USA.
//
//	COPYRIGHT
//      Ericsson AB 2005 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
//		  ACS_USA_Control contorls all activities in USA:
//		  configuring, initialisation of parameters, and operation.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//		  19089-CAA 109 0545
//
// AUTHOR 
//	 	  1995-06-19 by ETX/TX/T XKKHEIN
//
// REV  DATE 	NAME	 DESCRIPTION
// A    950829  XKKHEIN  First version.
// B    990915  UABDMT   Ported to Windows NT4.0.
// C    020723  UABTSO   Adapted to new AEH API.
// D    030212  UABPEK   New AP VIRUS alarms.
// E    040429  QVINKAL  Removal of RougeWave Tools.h++.
// F    041223  UABPEK   Supervision of three new event logs.
// G    051010  UABPEK   Suppression of alarms by named PHA parameter table.
// H    071231  EKAMSBA  General Error filtering.
// I	080208	EKAMSBA	 Auto cease alarm.
// J	080311	EKAMSBA	 PRC alarms handling.
//
//******************************************************************************

#ifndef ACS_USA_CONTROL_H 
#define ACS_USA_CONTROL_H

#include <string>
#include <sys/types.h>
#include <vector>
#include <windows.h>

#include "ACS_USA_Event.h"
#include "ACS_USA_Global.h"

using namespace std;
typedef string String;

//========================================================================================
// Constants used in this class
//========================================================================================
const size_t fileNameLen 	= 256; 

const char ACS_USA_ACS_LOGS[] = "ACS_LOGS";			// For testing only
const char ACS_USA_ACS_HOME[] = "ACS_HOME";			// For testing only


const char AP_Registry_Home[] = "SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\common";

const char AP_Registry_Log_Dir[] = "acsnslogs";		// The Registry value in the 
													// common key containing the path
													// to the ACS standard log
													// directory is called "acsnslogs"

const char ACS_USA_ParamTable[]    = "ACS/CXC1371081";  
const char ACS_USA_PRCParamTable[] = "ACS/CXC1371059";  

const char ACS_USA_SuppressedAlarmsTableName[] = "ACS_USABIN_SuppressedAlarms";

const char ACS_USA_acfName[]    = "\\etc\\ACS_USA_config.acf";
const char ACS_USA_prcAcfName[] = "\\etc\\ACS_USA_prc.acf";

const char ACS_USA_tempStorageName[] = "\\USA\\usa.tmp";  
													// Relative to ACS_USA_logsDefault

const char ACS_USA_ceaseStorageName[] = "\\USA\\cease.tmp";  
													// Relative to ACS_USA_logsDefault

static char ACS_USA_homeDefault[256];				// Read in part from the environment  
													// variable AP_HOME.
													// Not "const", since 
													// variable is changed in a 
													// member function of the class

static char ACS_USA_logsDefault[256];				// Read from Registry
													// using AP_Registry_Home and 
													// AP_Registry_Log_Dir
													// Not "const", since variable is 
													// changed in a member function of 
													// the class.

const char ACS_Alarm_SourcePath[] = "SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\AEH";
													// The AEH Registry 
													// structure contains the source name			
													// of the AP alarms in the Event Log.

const char AP_Alarm_SourceName[] = "eventlogSource";
													// Registry value for the AP Alarm Source 

static String ACS_USA_alarmSourceNameDefault;	// Read from Registry

const char ACS_USA_logBufferSize[]	 = "ACS_USABIN_LogBufferSize";
const char ACS_USA_recordSize[]		   = "ACS_USABIN_RecordBufSize";
const char ACS_USA_startOffset[]	   = "ACS_USABIN_StartOffset";
const char ACS_USA_specificProblem[] = "ACS_USABIN_SpecificProblem";
const char ACS_USA_LongInterval[]	   = "ACS_USABIN_LongInterval";
const char ACS_USA_ShortInterval[]	 = "ACS_USABIN_ShortInterval";

const char ACS_USA_alarmFilterInterval[]	= "ACS_USABIN_alarmFilterInterval";
const char ACS_USA_longTermAllowed[]		= "ACS_USABIN_longTermAllowed";
const char ACS_USA_recordsPrioThreshold[]	= "ACS_USABIN_recordsPrioThreshold";
const char ACS_USA_secondsPrioThreshold[]	= "ACS_USABIN_secondsPrioThreshold";
const char ACS_USA_GeneralErrorFilter[]		= "ACS_USABIN_GeneralErrorFilteringFlag";

const static unsigned char maxObjectOfRefSuffixLen = 31;
struct ACS_USA_SuppressedAlarmsElement {
	unsigned short  specificProblem;
  char            objectOfRefSuffix[maxObjectOfRefSuffixLen];
};

const static unsigned char maxGeneralErrorFilterLen = 4;
struct ACS_USA_GeneralErrorFilterElement {
  char  flag[maxGeneralErrorFilterLen];
};


//========================================================================================
//	Forward declaration(s)
//========================================================================================
class ACS_USA_ObjectManagerCollection;


//========================================================================================
//	Member functions, constructors, destructors, operators
//========================================================================================

class ACS_USA_Control
{
public:
	
	ACS_USA_Control();
	// Description:
	//		Constructor.
	// Parameters: 
	//		None 
	
	virtual ~ACS_USA_Control();	
	// Description:
	// 		Destructor.
	
	ACS_USA_StatusType initParameters();
	// Description:
	//		Initialises parameters. Retrieves them from PHA 
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_error 	An error occurred during analysis
	//		ACS_USA_ok		Analysis OK
	
	ACS_USA_StatusType initPathConstants();
	// Description:
	// 		Initialises directory path constants.
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_error 	An enviroment variable, registry or 
	//						string error occurred.
	//		ACS_USA_ok		OK
	
	ACS_USA_StatusType initAlarmSourceConstant();
	// Description:
	//		Initialises the Alarm source constant (default APEventLog)
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_error 	A Registry or string error occurred.
	//		ACS_USA_ok		OK
	
	ACS_USA_StatusType createLogDirectory(const String& fileName) const;
	// Description:
	//		Create a USA log directory if it doesn't exist
	// Parameters: 
	//		fileName		The file for which a directory will be created.
	// Return value: 
	//		ACS_USA_error 	An error occurred
	//		ACS_USA_ok		All is fine

	unsigned getLongInterval() const;
	// Description:
	//		Returns long term interval. 
	// Parameters: 
	//		None 
	// Return value: 
	//		unsigned		Long term analysis interval
	
	unsigned getShortInterval() const;
	// Description:
	//		Returns short term interval.  
	// Parameters: 
	//		None 
	// Return value: 
	//		unsigned		Short term analysis interval
	
	void setStartOffset(const int newOffset); 
	// Description:
	//		Changes the event number to start analysis at.
	//		Used in the 'notify change in event log functionality'.  
	// Parameters: 
	//		newOffset		The no of the event to start analysis from				 
	// Return value: 
	//		void
	
	unsigned getStartOffset() const;  
	// Description:
	//		Returns the event number to start analysis at..
	//		Used in the 'notify change in event log functionality'.  
	// Parameters: 
	//		None
	// Return value: 
	//		unsigned		Start offset record number
	
	BOOL getRaisedAlarmOnEventFrequency() const;
	// Description:
	//		Returns the boolean indicating if an alarm had been 
	//		set on the 'too high event frequency'-case.
	// Parameters: 
	//		None
	// Return value: 
	//		TRUE			An alarm has been raised on the 
	//						'too high event frequency'-case.
	//		FALSE			No alarm has been raised on the
	//						'too high event frequency'-case.
	
	ACS_USA_StatusType configure();
	// Description:
	//		Initialises parameters. Retrieves them from ACF. Updates 
	//		parameters by retrieving them from Temporary storage.
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_error 	An error occurred during analysis
	//		ACS_USA_ok		Analysis OK
	
	
	ACS_USA_StatusType operate(ACS_USA_ActivationType analysisType,
							   String triggedLogName,
							   const DWORD latestRecordNumbers[6]);
	// Description:
	//		Does the analysis. It is supposed to be called with 
	//		certain time intervals and supplied with Analysis type
	// Parameters: 
	//		analysisType	Long or short term analysis 
	// Return value: 
	//		ACS_USA_error 	An error occurred during analysis
	//		ACS_USA_ok		Analysis OK
	
	void dumpAll(ACS_USA_ObjectManagerCollection& managers);
	// Description:
	//		Dumps all Analysis objects to log. Can be used when compiled
	//		with -DDEBUG option. See ACS_USA_Debug.h.
	// Parameters: 
	//		managers			List of object managers
	// Return value: 
	//		None
	
private:
	
	void reportProblem() const;
	// Description:
	//		Reports the problem via the global event reporting object.
	//		If it fails the the event is reported to the console.
	// Parameters: 
	//		None
	// Return value: 
	//		None

	// Use to map ACF file and the corresponding event source.
	struct EventSourceAcfFilePair
	{
		EventSourceAcfFilePair(	const String& eventSource, const String& acfName )
			: eventSource(eventSource), acfName(acfName)	         
		{
		}

		String eventSource;		// the event source.
		String acfName;			// the corresponding acf file.
	};


	unsigned shortInterval;			              // Short term analysisInterval
	unsigned longInterval;			              // Long term analysis interval
	
	size_t logBufferSize;			                // Size of log buffer
	size_t recordBufferSize;		              // Size of record buffer
	off_t startOffset;   			                // Where start short term analysis
	ACS_USA_SpecificProblem specificProblem;  // Default specific problem
	DWORD alarmFilterInterval;		            // Min time between two alarms  
									                          // on the same event type
	DWORD latestBootTimeStamp;		            // Time of the latest boot
	String tmpName;				                // File name of temp storage
	String ceaseName;			                // File name of cease temp storage

	vector<EventSourceAcfFilePair> acfFileList; // Map: ACF files <-to-> event source.

	String bootEventRegExp;		            // Regexp for finding the 
								                          // latest boot event.
	String RestartEventRegExp;	            // Regexp for checking if the
									                          // Security log has been cleared.
	
	DWORD longTermAllowed;			              // Switch for running long term .
	DWORD recordsPrioThreshold;		            // Number of records allowed to be 
									                          // read before low prio.
	DWORD secondsPrioThreshold;		            // High load time allowed 
									                          // before low prio.
	BOOL raisedAlarmOnEventFrequency;

	String generalErrorFilter;          // General Error filter, 
                                            // ON : Enabled.
                                            // OFF: Disabled.

	vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms;
};


//========================================================================================
//	Returns long term interval. 
//========================================================================================

inline
unsigned
ACS_USA_Control::getLongInterval() const
{
    return longInterval;
}


//========================================================================================
//	Returns short term interval.  
//========================================================================================

inline
unsigned
ACS_USA_Control::getShortInterval() const
{
    return shortInterval;
}


//========================================================================================
//	Changes the event number to start analysis at. Used in the 'notify change in 
//	event log functionality'.  
//========================================================================================

inline
void
ACS_USA_Control::setStartOffset(const int newOffset) 
{
	// Used for the 'notify change in event log' functionality.
    startOffset = newOffset;
}


//========================================================================================
//	Returns the event number to start analysis at.	Used in the 'notify change in 
//	event log functionality'.  
//========================================================================================

inline
unsigned
ACS_USA_Control::getStartOffset() const
{	
	// Used for the 'notify change in event log' functionality.
    return startOffset;
}


//========================================================================================
//	Returns the boolean indicating if an alarm had been set on the 'too high event 
//	frequency'-case.
//========================================================================================

inline
BOOL 
ACS_USA_Control::getRaisedAlarmOnEventFrequency() const
{
    return raisedAlarmOnEventFrequency;
}

#endif
