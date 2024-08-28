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
//******************************************************************************

#ifndef ACS_USA_CONTROL_H 
#define ACS_USA_CONTROL_H

#include <string>
#include <sys/types.h>
#include <vector>

#include "acs_usa_event.h"
#include "acs_usa_types.h"
#include "acs_apgcc_omhandler.h"

using namespace std;
//typedef string String;

//========================================================================================
// Constants used in this class
//========================================================================================
const char ACS_USA_syslogFile[]    	= "/tmp/usaEventList.log";
const char ACS_USA_tempStorageName[]	= "usa.tmp";  
const char ACS_USA_ceaseStorageName[] 	= "cease.tmp";  
const char ACS_USA_ACS_LOGS[] = "USA_LOGS"; 

const static unsigned char maxObjectOfRefSuffixLen = 31;
struct ACS_USA_SuppressedAlarmsElement {
	unsigned int  specificProblem;
  	char	objectOfRefSuffix[maxObjectOfRefSuffixLen];
};

//========================================================================================
//	Forward declaration(s)
//========================================================================================
class ACS_USA_AnalysisObject;

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
	
	ACS_USA_ReturnType initParameters();
	// Description:
	//		Initialises parameters. Retrieves them from PHA 
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_Error 	An error occurred during analysis
	//		ACS_USA_Ok		Analysis OK
	
	ACS_USA_ReturnType initPathConstants();
	// Description:
	// 		Initialises directory path constants.
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_Error 	An enviroment variable, registry or 
	//						string error occurred.
	//		ACS_USA_Ok		OK
	
	ACS_USA_ReturnType createLogDirectory(const String& fileName) const;
	// Description:
	//		Create a USA log directory if it doesn't exist
	// Parameters: 
	//		fileName		The file for which a directory will be created.
	// Return value: 
	//		ACS_USA_Error 	An error occurred
	//		ACS_USA_Ok		All is fine

	unsigned int getShortInterval() const;
	// Description:
	//		Returns short term interval.  
	// Parameters: 
	//		None 
	// Return value: 
	//		unsigned		Short term analysis interval
	
	unsigned int getRecordBufferSize() const;
	// Description:
	//		Returns Record buffer size
	// Parameters: 
	//		None 
	// Return value: 
	//		unsigned int
	//
	ACS_USA_ReturnType configure();
	// Description:
	//		Initialises parameters. Retrieves them from ACF. Updates 
	//		parameters by retrieving them from Temporary storage.
	// Parameters: 
	//		None 
	// Return value: 
	//		ACS_USA_Error 	An error occurred during analysis
	//		ACS_USA_Ok		Analysis OK
	
	
	ACS_USA_ReturnType operate(ACS_USA_AnalysisType, ACS_USA_AnalysisObject *analysisObj, ACS_USA_RecordBuffer *record=NULL);
	// Description:
	//		Does the analysis. It is supposed to be called with 
	//		certain time intervals and supplied with Analysis type
	// Parameters: 
	//		analysisType	Long or short term analysis 
	// Return value: 
	//		ACS_USA_Error 	An error occurred during analysis
	//		ACS_USA_Ok		Analysis OK
	
	void dumpAll(ACS_USA_AnalysisObject& analysisObject);
	// Description:
	//		Dumps all Analysis objects to log. Can be used when compiled
	//		with -DDEBUG option. See ACS_USA_Debug.h.
	// Parameters: 
	//		managers			List of object managers
	// Return value: 
	//		None
	
	ACS_USA_ReturnType  loadConfig();
	// Description:
	//		Loads all the constant paramters from IMM.
	// Parameters:
	// 		none
	//
	// Return Value:
	// 		ACS_USA_Error 	An error occurred during imm load.
	// 		ACS_USA_Ok	IMM fetch OK.
	String getUSATmpFileName();
	// Description:
	// Parameters:
	// 		none
	// Return Value:
	// 		tmpName
	String getCeaseFileName();
	void reportProblem() const;
	// Description:
	//		Reports the problem via the global event reporting object.
	//		If it fails the the event is reported to the console.
	// Parameters: 
	//		None
	// Return value: 
	//		None

private:
	
	ACS_USA_ReturnType setPerameter(String paramName, int value);	

	unsigned int shortTermInterval;		// Short term analysisInterval
	unsigned int recordBufferSize;		// Size of record buffer
	unsigned int syslogRecordsMax; 		// Max latest syslog records to retrieve
	unsigned int alarmFilterInterval;	// Min time between two alarms on the same event type.
	unsigned int recordsPrioThreshold;	// Number of records allowed to be
	unsigned int secondsPrioThreshold;	// High load time allowed before low prio.
	String 	     syslogTimeStampFormat;
	ACS_USA_SpecificProblem specificProblem; // Default specific problem
	ACS_USA_AnalysisObject* m_configObject;
	//String latestBootTimeStamp;	// Time of the latest boot
	ACS_USA_TimeType  latestRecordTimeStamp;	// Time of the latest Analysed Record 
	String tmpName;			// File name of temp storage
	String ceaseName;		// File name of cease temp storage

	vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms;
};


//========================================================================================
//	Returns short term interval.  
//========================================================================================

inline unsigned int ACS_USA_Control::getShortInterval() const
{
	return this->shortTermInterval;
}

inline unsigned int ACS_USA_Control::getRecordBufferSize() const
{
	return this->recordBufferSize;
}


#endif
