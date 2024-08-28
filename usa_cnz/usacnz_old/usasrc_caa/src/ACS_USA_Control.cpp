//******************************************************************************
//
// NAME
//      ACS_USA_Control.cpp
//
// COPYRIGHT
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
//		ACS_USA_Control contorls all activities in USA:
//		configuring, initialisation of parameters, and operation.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//		  19089-CAA 109 0545
//
// AUTHOR 
// 		  1995-06-27 by ETX/TX/T XKKHEIN
//
// REV	DATE	NAME 	 DESCRIPTION
// A	950829	XKKHEIN	 First version.
// B	990915	UABDMT	 Ported to Windows NT4.0.
// C	020723	UABTSO	 Adapted to new AEH API.
// D	030212	UABPEK	 New AP VIRUS alarms.
// E	040429	QVINKAL	 Removal of RougeWave Tools.h++.
// F	041223	UABPEK	 Supervision of three new event logs.
// G	051025	UABPEK	 Suppression of alarms by named PHA parameter table.
// H	071231	EKAMSBA	 General Error filtering.
// I	080208	EKAMSBA	 Auto cease alarm.
// J	080311	EKAMSBA	 PRC alarms handling.
// K	080409	EKAMSBA	 Temp. file versioning.
//
//******************************************************************************

#pragma warning (disable: 4786)

#include "commondll.h"
#include "io.h"
#include <iostream>
#include <stdlib.h>
#include <windows.h>

#include <ACS_PHA_Parameter.h>
#include <ACS_TRA_Trace.h>
#include "ACS_USA_Analyser.h"
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_Common.h"
#include "ACS_USA_Config.h"
#include "ACS_USA_Control.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_Event.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_Initialiser.h"
#include "ACS_USA_ObjectManager.h"
#include "ACS_USA_Regexp.h"
#include "ACS_USA_AutoCeaseManager.h"
#include "ACS_USA_ObjectManagerCollection.h"



//========================================================================================
//	Trace point definitions
//========================================================================================

ACS_TRA_trace traceControl = ACS_TRA_DEF("ACS_USA_Control", "C40");

const char* const ACS_USA_traceLoadingTemp = "Loading Temporary Storage";
const char* const ACS_USA_traceSavingTemp  = "Saving data to temporay storage";
const char* const ACS_USA_traceNoLongTerm  = "Not configured to run long term";

ACS_TRA_trace traceACFLoading = ACS_TRA_DEF("ACS_USA_Control", "C256");
const char* const ACS_USA_traceACFLoading  = "ACF file: %s, has been configured for %s event source(s)";


//******************************************************************************
// local definition to this class
//******************************************************************************
const char ACS_USA_PRCEventSource[]		= "ACS_PRC_EventSource";

const static unsigned char maxPRCEventSource = 33;
struct ACS_USA_PRCEventSourceElement {
  char  source[maxPRCEventSource];
};


// Helper class to manage the dynamically allocated ACS_USA_Config objects
class ACS_USA_ConfigHandler
{
public:
	// Constructor
    ACS_USA_ConfigHandler()
	{
    }

    // Destructor
    ~ACS_USA_ConfigHandler()
    {
		std::vector<ACS_USA_Config *>::iterator iElem = List_.begin();
		for( ; iElem != List_.end(); iElem++)
		{
			try
			{	
				if( NULL != *iElem )
				{
					delete *iElem;
					*iElem = NULL;
				}
			}
			catch(...)
			{
			}
		}

		List_.clear();
    }

	// Return a dynamically allocate ACS_USA_Config
	ACS_USA_Config* getConfig()
	{
		ACS_USA_Config* ptr = new ACS_USA_Config();
		List_.push_back( ptr );
		return ptr;
	}

    
private:
    ACS_USA_ConfigHandler( const ACS_USA_ConfigHandler& );              // copy constructor
    ACS_USA_ConfigHandler& operator=( const ACS_USA_ConfigHandler&);    // copy assignment

    vector<ACS_USA_Config*>  List_; // list of ACF files.
};



//========================================================================================
//	Constructor
//========================================================================================

ACS_USA_Control::ACS_USA_Control() :
shortInterval(0),
longInterval(0),
logBufferSize(0),
recordBufferSize(0),
startOffset(0),
latestBootTimeStamp(0),
bootEventRegExp(""),
RestartEventRegExp(""),
alarmFilterInterval(0),
longTermAllowed(1),			// Default is allowing long term analysis
raisedAlarmOnEventFrequency(FALSE),
specificProblem(0),
generalErrorFilter(Key_GEFilterON)
{	
}


//========================================================================================
//	Destructor
//========================================================================================

ACS_USA_Control::~ACS_USA_Control()
{
}


//========================================================================================
//	initParameters()
//========================================================================================

ACS_USA_StatusType
ACS_USA_Control::initParameters() 
{
	// Retrieve parameters from AP param handling
	ACS_USA_StatusType ret = ACS_USA_ok;

	ACS_PHA_Tables usaParmTable((char*)ACS_USA_ParamTable);
	ACS_PHA_Parameter<size_t>	logParam(ACS_USA_logBufferSize);
	ACS_PHA_Parameter<size_t>	recParam(ACS_USA_recordSize);
	ACS_PHA_Parameter<off_t> offParam(ACS_USA_startOffset);
	ACS_PHA_Parameter<unsigned>	shtParam(ACS_USA_ShortInterval);
	ACS_PHA_Parameter<unsigned>	lngParam(ACS_USA_LongInterval);
	ACS_PHA_Parameter<ACS_USA_SpecificProblem> sppParam(ACS_USA_specificProblem);
	ACS_PHA_Parameter<unsigned>	filterParam(ACS_USA_alarmFilterInterval);
	ACS_PHA_Parameter<unsigned>	longTermAllowedParam(ACS_USA_longTermAllowed);
	ACS_PHA_Parameter<unsigned>	recordsPrioThresholdParam(ACS_USA_recordsPrioThreshold);
	ACS_PHA_Parameter<unsigned>	secondsPrioThresholdParam(ACS_USA_secondsPrioThreshold);
	ACS_PHA_Parameter<ACS_USA_GeneralErrorFilterElement> generalErrorParam(ACS_USA_GeneralErrorFilter);

	// Get PRC event source
	ACS_PHA_Tables prcParamTable((char*)ACS_USA_PRCParamTable);
	ACS_PHA_Parameter<ACS_USA_PRCEventSourceElement> prcEventSourceParam(ACS_USA_PRCEventSource);

	if (logParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(logParam.getErrorText());
		ret = ACS_USA_error;
	}
	else if (recParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(recParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (offParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(offParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (sppParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(sppParam.getErrorText());
		ret = ACS_USA_error;
	}
	else if (shtParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(shtParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (lngParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(lngParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (filterParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(filterParam.getErrorText());
		ret = ACS_USA_error;
	}
	else if (longTermAllowedParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(longTermAllowedParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (recordsPrioThresholdParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(recordsPrioThresholdParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (secondsPrioThresholdParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) {
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(secondsPrioThresholdParam.getErrorText());
		ret = ACS_USA_error;
	} 
	else if (generalErrorParam.get(usaParmTable) == ACS_PHA_PARAM_RC_ERROR) 
	{
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(generalErrorParam.getErrorText());
		ret = ACS_USA_error;
	}
	else if(prcEventSourceParam.get(prcParamTable) == ACS_PHA_PARAM_RC_ERROR) 
	{		
		error.setError(ACS_USA_ParameterHandlingError);
		error.setErrorText(prcEventSourceParam.getErrorText());
		ret = ACS_USA_error;
	}

	if (ret == ACS_USA_error) {
		reportProblem();
		return ACS_USA_error;
	}

	// Check the range of the GE param.
	if( _stricmp(generalErrorParam.data().flag, Key_GEFilterON) != 0 && 
		_stricmp(generalErrorParam.data().flag, Key_GEFilterOFF) != 0 )
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_IllegalValue);
		reportProblem();
		return ACS_USA_error;
	}


	// Read USA suppressed alarms named table.
	ACS_PHA_NamedTable suppressedAlarmsNamedTable(
		(ACS_PHA_CXCName)ACS_USA_ParamTable,
		(ACS_PHA_TableName)ACS_USA_SuppressedAlarmsTableName);
	ACS_PHA_Parameter<ACS_USA_SuppressedAlarmsElement> suppressedAlarmsNamedTableElement;

	// Scans rows of USA suppressed alarms named table.
	while(suppressedAlarmsNamedTableElement.get(suppressedAlarmsNamedTable)) {
		// Valid parameter found. Creates space for an element to store
		// the contents of the parameter and to be copied in the
		// internal image of the suppressed alarms named table.
		ACS_USA_SuppressedAlarmsElement* suppressedAlarmsElement = new ACS_USA_SuppressedAlarmsElement;
		if (suppressedAlarmsElement == NULL) {
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Control::initParameters().");
			reportProblem();
			return ACS_USA_error;	
		}
		suppressedAlarmsElement->specificProblem = suppressedAlarmsNamedTableElement.data().specificProblem;
		strncpy(suppressedAlarmsElement->objectOfRefSuffix, suppressedAlarmsNamedTableElement.data().objectOfRefSuffix, maxObjectOfRefSuffixLen);
		suppressedAlarms.push_back(suppressedAlarmsElement);
	}

	// Checks the errors returned by the PHA get() method
	switch(suppressedAlarmsNamedTableElement.getError()) {
	  case ACS_PHA_PARAM_DB_NOT_FOUND :
		  // Named parameter table not found.
		  // Normal situation. No alarms suppressed.
		  break;
	  case ACS_PHA_PARAM_WRONG_SIZE : {
		  // Size mismatch between the parameter format and its internal image.
		  error.setError(ACS_USA_ParameterHandlingError);
		  error.setErrorText(suppressedAlarmsNamedTableElement.getErrorText());
		  reportProblem();
		  return ACS_USA_error;
									  }
	  default : {
		  // Normal situation. Parameter fetch successfull.
		  break;
				}
	}


	// Get home path.
	char* envHome = getenv(ACS_USA_ACS_HOME);
	String homePath;

	if (envHome == 0 || strlen(envHome) == 0) 
	{
		homePath    = ACS_USA_homeDefault;
	} 
	else 
	{
		homePath    = envHome;
	} 


	// Initialise ACF file map
	acfFileList.clear();		

	// 1. The main acf file should be the first. 
	acfFileList.push_back( EventSourceAcfFilePair( "ALL", 
												   (homePath + ACS_USA_acfName) ) 
						  );

	// 2. Then add any other acf file ...
	acfFileList.push_back( EventSourceAcfFilePair( prcEventSourceParam.data().source, 
												   (homePath + ACS_USA_prcAcfName) ) 
					      );


	// Get log path.
	char* envLogs = getenv(ACS_USA_ACS_LOGS);
	String logPath;
	if (envLogs == 0 || strlen(envLogs) == 0) 
	{
		logPath   = ACS_USA_logsDefault;
	}
	else 
	{
		logPath   = envLogs; 
	} 

	// Initialise temporary storage name
	tmpName   = logPath + ACS_USA_tempStorageName;
	ceaseName = logPath + ACS_USA_ceaseStorageName;


	// Assign retrieved values to local variables.
	logBufferSize    = logParam.data();
	recordBufferSize = recParam.data();
	startOffset      = offParam.data();
	specificProblem  = sppParam.data();
	shortInterval    = shtParam.data();
	longInterval     = lngParam.data();

	alarmFilterInterval  = filterParam.data();
	longTermAllowed      = longTermAllowedParam.data();
	recordsPrioThreshold = recordsPrioThresholdParam.data();
	secondsPrioThreshold = secondsPrioThresholdParam.data();

	generalErrorFilter = generalErrorParam.data().flag;
	toUpper(generalErrorFilter);

	eventManager.setDefaultSpecificProblem(specificProblem);
	return ACS_USA_ok;
}

//========================================================================================
//	createLogDirectory()
//========================================================================================
ACS_USA_StatusType
ACS_USA_Control::createLogDirectory(const String& fileName) const
{   
	// Create USA's log directory that will hold the fileName, if it doesn't exist
	String usaLogPath = fileName;
	
	size_t lastBackSlash = usaLogPath.find_last_of('\\');
	if (lastBackSlash != String::npos) 
	{			
		usaLogPath.erase(lastBackSlash);			// Strip away the "usa.tmp"
		// part of the path.		
		if ((_access(usaLogPath.data(), 00)) == -1) 
		{ 
			// Create the directory
			if (!CreateDirectory(usaLogPath.data(), NULL))
			{
				String createFailure = "Failure creating directory: ";
				createFailure.append(usaLogPath.data());
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				return ACS_USA_error;
			}
			
			// Set file access permissions
			//
			LPSTR errorText;
			int err2 =  SetFilePerm(
							(LPTSTR)usaLogPath.data(), 
							(LPTSTR)"ACSADMG", 
							GENERIC_ALL, 
							(LPTSTR)"set", 
							SUB_CONTAINERS_AND_OBJECTS_INHERIT, 
							&errorText
							);
			if (err2 != 0)
			{
				String createFailure = 
					"Failure setting security permissions for \'ACSADMG\' on directory: ";
				createFailure.append(usaLogPath.data());
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				LocalFree(errorText);
				return ACS_USA_error;
			}
			
			err2 = SetFilePerm(
						(LPTSTR)usaLogPath.data(), 
						(LPTSTR)"Administrators", 
						GENERIC_ALL, 
						(LPTSTR)"set", 
						SUB_CONTAINERS_AND_OBJECTS_INHERIT, 
						&errorText
						);
			if (err2 != 0)
			{
				String createFailure = 
					"Failure setting security permissions for \'Administrators\' on directory: ";
				createFailure.append(usaLogPath.data());
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				LocalFree(errorText);
				return ACS_USA_error;
			}
			
			err2 = SetFilePerm(
						(LPTSTR)usaLogPath.data(), 
						(LPTSTR)"Everyone", 
						GENERIC_ALL, 
						(LPTSTR)"revoke",  
						NO_INHERITANCE, 
						&errorText
						);
			if (err2 != 0)
			{
				String createFailure = 
					"Failure revoking security permissions for \'Everyone\' on directory: ";
				createFailure.append(usaLogPath.data());
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				LocalFree(errorText);
				return ACS_USA_error;
			}
		}
		else 
		{ 
			// The directory already exists, do nothing
		}		
	}
	else
	{
		// Something wrong with the path. Just log and die.
		String createFailure = 
			"Failure creating USA's log directory. The path is erroneous: ";
		createFailure.append(usaLogPath.data());
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(createFailure.data());
		reportProblem();  
		return ACS_USA_error;
	}
    
    return ACS_USA_ok;
}

//========================================================================================
//	configure()
//========================================================================================
ACS_USA_StatusType
ACS_USA_Control::configure()
{
	ACS_USA_ObjectManagerCollection managers;
	ACS_USA_Initialiser		ini;
	
	// Each ACS_USA_Config object should stay in memory as long as 
	// the built ACS_USA_ObjectManager object exists.
	ACS_USA_ConfigHandler	cfgStore;

	// List of event sources and the corresponding acf file.
	if( acfFileList.empty() )
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText("No ACF file provided in ACS_USA_Control::configure.");
		reportProblem();
		return ACS_USA_error;
	}


	//// Create USA's log directory to hold  'usa.tmp' file if it doesn't exist
	if( createLogDirectory(tmpName) == ACS_USA_error )
	{
		// createLogDirectory() already reported the error
		return ACS_USA_error;
	}

	for(int idx=0; idx < acfFileList.size(); idx++ )
	{		
		// Use one ACS_USA_ObjectManager per ACF file.
		ACS_USA_ObjectManager*	mgr = managers.addManager( acfFileList[idx].eventSource.data() );
		if(NULL == mgr)
		{			
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Control::configure.");
			reportProblem();  
			return ACS_USA_error;
		}

		// Use one ACS_USA_Config parser per ACF file.
		ACS_USA_Config* cfg = cfgStore.getConfig();
		if( NULL == cfg )
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Control::configure.");
			reportProblem();  
			return ACS_USA_error;
		}


		String tempBootEventRegExp; 
		String tempRestartEventRegExp;

		// Load the ACF file and parse it.
		if ( cfg->loadACF(*mgr, 
			              acfFileList[idx].acfName.data(), 
						  tempBootEventRegExp, 
						  tempRestartEventRegExp ) == ACS_USA_error) 
		{
			reportProblem();
			return ACS_USA_error;
		}
		else
		{
			// Init those parameters from the main ACF file ONLY.
			// The main ACF file is the FIRST in the lisrt
			if( 0 == idx )
			{
				bootEventRegExp = tempBootEventRegExp;
				RestartEventRegExp = tempRestartEventRegExp; 
			}
		}

		// Criteria that don't match the generalErrorFilter are removed from the 
		// criteria list
		if (ini.removeGeneralError(*mgr, generalErrorFilter) == ACS_USA_error) 
		{
			reportProblem();
			return ACS_USA_error;
		}

		if (ACS_TRA_ON(traceACFLoading)) 
		{ 
            const unsigned short buffSize = 255;
            char msg[buffSize+1] = {0}; // +1 for the null char

            // 'ACF file: %s, has been configured for %s event source(s)'
            _snprintf( msg, buffSize, ACS_USA_traceACFLoading 
                        , acfFileList[idx].acfName.data()
						, acfFileList[idx].eventSource.data() );

            ACS_TRA_event(&traceACFLoading, msg);  
		}
	}

    //
	// Get information about previous analysis results (update)
	// and write the data to the disk
    //

	// Check USA.TMP version
	if( ini.integrityCheck(tmpName.data()) == ACS_USA_False )
	{
		// Bad version or old file
		if ( error.getError() == ACS_USA_BadState )
		{
			// Clear error
			error.setError(ACS_USA_NoError);
			error.setErrorText(noErrorText);
			

			// Rename it, and continue, new one will be created
			String oldName = tmpName + ".old";

			BOOL ok = MoveFileEx(
									tmpName.data(), 
									oldName.data(),
									MOVEFILE_REPLACE_EXISTING
								);

			if ( 0 == ok )
			{
				String createFailure = "Failed to rename file " + tmpName;
				error.setError(ACS_USA_TmpFileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				return ACS_USA_error;
			}
		}
		else
		{
			// An error occurred
			reportProblem();  
			return ACS_USA_error;
		}
	}

	// Check the USA.TMP file
	// This is the first time we use it.
	// It is Ok to delete, a new will be generated next time USA is started.
	if (ini.update(managers, tmpName.data()) == ACS_USA_error ||
		ini.flush(managers, tmpName.data()) == ACS_USA_error) 
	{		
		// If exist we rename and send a event
		String oldName = tmpName + ".old";
		
		BOOL ok = MoveFileEx(
						tmpName.data(), 
						oldName.data(),
						MOVEFILE_REPLACE_EXISTING
						);
		
		if (!ok)
		{
			String createFailure = "Failed to rename file " + tmpName;
			error.setError(ACS_USA_TmpFileIOError);
			error.setErrorText(createFailure.data());
			reportProblem();  
			return ACS_USA_error;
		}
		
		String probableCause(ACS_USA_DiagnosticFault);
		String perceivedSeverity(Key_perceivedSeverity_EVENT);
		error.setError(ACS_USA_TmpFileIOError);
		
		eventManager.report(
						ACS_USA_TmpFileIOError + ACS_USA_FirstErrorCode,
						perceivedSeverity,
						probableCause,
						String(),
						String(error.getErrorText()),
						String(error.getProblemText(error.getError())),
						"",								// Node
						0
						);
		return ACS_USA_error;
	}
    

	//// Create USA's log directory to hold  'cease.tmp' file if it doesn't exist
	if( createLogDirectory(ceaseName) == ACS_USA_error )
	{
		// createLogDirectory() already reported the error
		return ACS_USA_error;
	}

	//// Load cease temporary file if it exists
	if( (ACS_USA_AutoCeaseManager::getInstance()).loadList(ceaseName) == ACS_USA_error )
	{
		// loadList already reported the error
		return ACS_USA_error;
	}


    return ACS_USA_ok;
}


//========================================================================================
//	initPathConstants()
//========================================================================================

ACS_USA_StatusType 
ACS_USA_Control::initPathConstants()
{
    ACS_USA_StatusType ret = ACS_USA_ok;   
	
	// Set ACS_USA_homeDefault 
	//			
	char apHomePath [256] ;
	strcpy(apHomePath,"") ;
	if (getenv("AP_HOME") == NULL)
	{ 
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_EnvironmentVariableReadError);
		reportProblem();  		
		// Might as well die - can't read
		// AP_HOME environment variable.		
		ret = ACS_USA_error;
		return ret;
    }	
	else
	{	// AP_HOME is "C:\Program Files\Adjunct Processor"	
		strcpy(apHomePath, getenv("AP_HOME"));
		strcat(apHomePath, "\\ACS");
		strcpy(ACS_USA_homeDefault, apHomePath);
	}	 
	
	// Read registry for ACS_USA_logsDefault
	HKEY handleToRegistryKey;
	if (RegOpenKeyEx(
			HKEY_LOCAL_MACHINE, 
			AP_Registry_Home, 
			0, 
			KEY_ALL_ACCESS, 
			&handleToRegistryKey
			) != ERROR_SUCCESS) 
	{    
		error.setError(ACS_USA_SyntaxError);
		
		char tmpPathText[256] = "";
		if (sprintf(
				tmpPathText, 
				"%s%s%s", 
				ACS_USA_RegistryOpenError,
				": ", 
				AP_Registry_Home
				) == 0)
		{
			error.setErrorText(ACS_USA_RegistryOpenError);
		}
		else
		{
			error.setErrorText(tmpPathText);
		}
		reportProblem();  		
		// Might as well die - can't open registry.		
		ret = ACS_USA_error;
		return ret;
	}
	else								// Open ok
	{		
		DWORD valueType, valueSize;		
		// Find out how long the registry entry is (stored in valueSize)
		if (RegQueryValueEx(
					handleToRegistryKey, 
					AP_Registry_Log_Dir, 
					NULL, 
					&valueType, 
					NULL, 
					&valueSize
					) != ERROR_SUCCESS)
		{
			error.setError(ACS_USA_SyntaxError);
			
			char tmpPathText[256] = "";
			if (sprintf(
					tmpPathText, 
					"%s%s%s", 
					ACS_USA_RegistryReadError,
					": ", 
					AP_Registry_Log_Dir
					) == 0)
			{
				error.setErrorText(ACS_USA_RegistryReadError);
			}
			else
			{
				error.setErrorText(tmpPathText);
			}
			reportProblem();  		
			// Might as well die - can't query registry.
			ret = ACS_USA_error;
			return ret;
		}

		// Read the value of the registry entry
		unsigned char* valueString = new unsigned char[valueSize];		
		if (valueString == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Control::initPathConstants().");
			reportProblem();  			
		}
		
		if (RegQueryValueEx(
					handleToRegistryKey, 
					AP_Registry_Log_Dir,
					NULL, 
					&valueType, 
					valueString, 
					&valueSize
					) != ERROR_SUCCESS)
		{   
			error.setError(ACS_USA_SyntaxError);
			
			char tmpPathText[256] = "";
			if (sprintf(
					tmpPathText, 
					"%s%s%s", 
					ACS_USA_RegistryReadError,
					": ", 
					AP_Registry_Log_Dir
					) == 0)
			{
				error.setErrorText(ACS_USA_RegistryReadError);
			}
			else
			{
				error.setErrorText(tmpPathText);
			}
			reportProblem();  		
			// Might as well die - can't query registry.
			delete [] valueString;
			ret = ACS_USA_error;
			return ret;
		}
		else							// Registry value read ok
		{
			if (valueString != NULL)
			{							
				// Assemble path to "logs" directory
				strcpy(ACS_USA_logsDefault, (char*)valueString); 
				delete [] valueString;
			}
		}
	}
	try
	{
		RegCloseKey(handleToRegistryKey);
	}
	catch (...)
	{
		// Ignore if handle is invalid, just to make Purify happy.
	}
	
	ret = initAlarmSourceConstant();
	return ret;
}


//========================================================================================
//	initAlarmSourceConstant()
//========================================================================================

ACS_USA_StatusType 
ACS_USA_Control::initAlarmSourceConstant()
{
    ACS_USA_StatusType 	  ret = ACS_USA_ok;   
	
	// Set ACS_USA_alarmSourceNameDefault 
	HKEY handleToRegistryKey;
	if (RegOpenKeyEx(
				HKEY_LOCAL_MACHINE, 
				ACS_Alarm_SourcePath, 
				0, 
				KEY_ALL_ACCESS, 
				&handleToRegistryKey
				) != ERROR_SUCCESS) 
	{   
		// If ever in here, the alarm probably won't turn up in the Eventlog
		// since the reason for the fault is AEH Registry problems.
		error.setError(ACS_USA_SyntaxError);
		
		char tmpPathText[256] = "";
		if (sprintf(
				tmpPathText, 
				"%s%s%s", 
				ACS_USA_RegistryOpenError,
				": ", 
				ACS_Alarm_SourcePath
				) == 0)
		{
			error.setErrorText(ACS_USA_RegistryOpenError);
		}
		else
		{
			error.setErrorText(tmpPathText);
		}
		reportProblem();  		
		// Might as well die - can't open registry.		
		ret = ACS_USA_error;
		return ret;
	}
	else								// Open ok
	{		
		DWORD valueType, valueSize;		
		// Find out how long the registry entry is (stored in valueSize)
		if (RegQueryValueEx(
					handleToRegistryKey, 
					AP_Alarm_SourceName, 
					NULL, 
					&valueType, 
					NULL, 
					&valueSize
					) != ERROR_SUCCESS)
		{
			// If ever in here, the alarm probably won't turn up in the Eventlog
			// since the reason for the fault is AEH Registry problems.
			error.setError(ACS_USA_SyntaxError);
			
			char tmpPathText[256] = "";
			if (sprintf(
					tmpPathText, 
					"%s%s%s", 
					ACS_USA_RegistryReadError,
					": ", 
					AP_Alarm_SourceName
					) == 0)
			{
				error.setErrorText(ACS_USA_RegistryReadError);
			}
			else
			{
				error.setErrorText(tmpPathText);
			}
			reportProblem();  		
			// Might as well die - can't query registry.
			ret = ACS_USA_error;
			return ret;
		} 

		// Read the value of the registry entry
		unsigned char* valueString = new unsigned char[valueSize];		
		if (valueString == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Control::initAlarmSourceConstant().");
			reportProblem();  			
		}
		
		if (RegQueryValueEx(
					handleToRegistryKey, 
					AP_Alarm_SourceName,
					NULL, 
					&valueType, 
					valueString, 
					&valueSize
					) != ERROR_SUCCESS)
		{   
			// If ever in here, the alarm probably won't turn up in the Eventlog
			// since the reason for the fault is AEH Registry problems.
			error.setError(ACS_USA_SyntaxError);
			
			char tmpPathText[256] = "";
			if (sprintf(
					tmpPathText, 
					"%s%s%s", 
					ACS_USA_RegistryReadError,
					": ", 
					AP_Alarm_SourceName
					) == 0)
			{
				error.setErrorText(ACS_USA_RegistryReadError);
			}
			else
			{
				error.setErrorText(tmpPathText);
			}
			reportProblem();  		
			// Might as well die - can't query registry.
			delete [] valueString;
			ret = ACS_USA_error;
			return ret;
		}
		else							// Registry value read ok
		{
			if (valueString != NULL)
			{							// Set ACS_USA_alarmSourceNameDefault
				ACS_USA_alarmSourceNameDefault.append((char*)valueString);
				delete [] valueString;
			}
		}
	}
	try
	{
		RegCloseKey(handleToRegistryKey);
	}
	catch (...)
	{
		// Ignore if handle is invalid, just to make Purify happy.
	}
	
	return ret;
}


//========================================================================================
//	operate()
//========================================================================================

ACS_USA_StatusType
ACS_USA_Control::operate(
  ACS_USA_ActivationType analysisType,
  String triggedLogName,
	const DWORD latestRecordNumbers[6])						 						
{
    if (longTermAllowed == ACS_USA_False) 
    {
        if (ACS_TRA_ON(traceControl)) 
        {
            ACS_TRA_event(&traceControl, ACS_USA_traceNoLongTerm);
        }

        if ((analysisType == Activation_longTerm) || 
            (analysisType == Activation_FirstLongTerm)) 
        {
                return ACS_USA_ok_conditional; // If longterm analysis isn't allowed, just return.
        }
    }

    ACS_USA_ObjectManagerCollection managers;

    ACS_USA_Initialiser   ini;
    ACS_USA_StatusType  ret; 

    // Load temp storage.
    if (ACS_TRA_ON(traceControl)) 
    { 
        ACS_TRA_event(&traceControl, ACS_USA_traceLoadingTemp);
    }

    if (ini.loadTempStorage(managers, tmpName.data()) == ACS_USA_error) 
    {
        reportProblem();
        return ACS_USA_error;
    }


    // This dumps the contents of all analysis objects and criterions.  
    DEBUG_DumpObjects( managers );

    // Analyse all objects
    ACS_USA_Analyser analyser;
    ret = analyser.analyse(
                            managers,
                            ACS_USA_alarmSourceNameDefault,
                            triggedLogName, 
                            latestBootTimeStamp,
                            bootEventRegExp,
                            RestartEventRegExp,
                            alarmFilterInterval,
                            suppressedAlarms,
                            latestRecordNumbers,
                            recordsPrioThreshold,
                            secondsPrioThreshold,
                            logBufferSize,
                            recordBufferSize,
                            analysisType,
                            startOffset,
                            &tmpName);

    if (ret == ACS_USA_error) {
        USA_DEBUG(logMsg("Analyse error: %s\n", error.getErrorText()));
        reportProblem();
    }
    if (analysisType == Activation_startUp) {
        // Do not save to temp storage, since start-up analysis
        // only means finding the latest boot event, no record 
        // matching a'la short or long term analysis
        return ret;
    }
    if (analyser.getAlarmSetOnMissedBootEvent() == TRUE) {
        // If an alarm on 'too high event frequency'
        // has been raised in first short term, 
        // don't raise it again in long term.
        raisedAlarmOnEventFrequency = TRUE;
    }

    // Save into temp storage even if error occurred during analysis
    if (ACS_TRA_ON(traceControl)) 
	{
        ACS_TRA_event(&traceControl, ACS_USA_traceSavingTemp);
    }

    if (ini.flush(managers, tmpName.data()) == ACS_USA_error) 
    {
        reportProblem();
        return ACS_USA_error;
    }

    // Return the analysis result. If 'flush error' return is made already.
    return ret;
}

//========================================================================================
//	reportProblem()
//========================================================================================
void
ACS_USA_Control::reportProblem() const
{
	ACS_USA_ErrorType err = error.getError();
	String perceivedSeverity(Key_perceivedSeverity_EVENT);
	String probableCause(ACS_USA_FaultCause);
	if (err == ACS_USA_APeventHandlingError) 
	{
	}
	if ((err == ACS_USA_SyntaxError) ||
		  (err == ACS_USA_ParameterHandlingError) ||
		  (err == ACS_USA_FileIOError))		
	{
		// ACS_USA_ParameterHandlingError and ACS_USA_FileIOError added 
		// for raising Diagnostics fault alarm in these cases as well.
		perceivedSeverity = Key_perceivedSeverity_A2;
		probableCause = ACS_USA_DiagnosticFault;
	}
	if (eventManager.report(
				err + ACS_USA_FirstErrorCode,
				perceivedSeverity,
				probableCause,
				String(),
				String(error.getErrorText()),
				String(error.getProblemText(error.getError())),
				"",								// Node
				0) == ACS_USA_error) 
	{
	}
}

//========================================================================================
//	dumpAll()
//========================================================================================

void
ACS_USA_Control::dumpAll(ACS_USA_ObjectManagerCollection& managers)
{ 
	vector<ACS_USA_ObjectManager*> aManagers = managers.getManagers();
	vector<ACS_USA_ObjectManager*>::iterator iElem = aManagers.begin();

	for( ; iElem != aManagers.end(); iElem++)
	{
		ACS_USA_ObjectManager* manager = *iElem;

		// Should never happen
		if( NULL == manager )
		{		
			USA_DEBUG(logMsg("NULL Object Manager\n"));
			continue;
		}

		int no = manager->getObjectCount();
		ACS_USA_AnalysisObject *o = manager->getFirst();
		for (int i = 0; i < no; i++) 
		{ 
			o->dump();
			o = manager->getNext();
		}
	}
}
