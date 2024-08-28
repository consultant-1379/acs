//******************************************************************************
//
// NAME
//      acs_usa_control.cpp
//
// COPYRIGHT
//      Ericsson AB 2012 - All rights reserved
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
//******************************************************************************

#include <iostream>
#include <stdlib.h>

#include "acs_usa_analyser.h"
#include "acs_usa_analysisObject.h"
#include "acs_usa_common.h"
#include "acs_usa_config.h"
#include "acs_usa_control.h"
#include "acs_usa_criterion.h"
#include "acs_usa_event.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_initialiser.h"
#include "acs_usa_regexp.h"
#include "acs_usa_autoCeaseManager.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_error.h"
#include "acs_usa_logtrace.h"


//========================================================================================
//	Trace point definitions
//========================================================================================

ACS_USA_Trace traCntl("ACS_USA_Control 	") ;

//========================================================================================
//	Constant  definitions
//========================================================================================

static char ACS_USA_logsDefault[256];
const unsigned int ACS_USA_DefaultsyslogRecordsMax      = 20000;
const unsigned int ACS_USA_DefaultshortTermInterval     = 1200; // seconds
const unsigned int ACS_USA_DefaultrecordBufferSize      = 2048;
const unsigned int ACS_USA_DefaultalarmFilterInterval   = 60;   //seconds
const unsigned int ACS_USA_DefaultrecordsPrioThreshold  = 100;  //seconds
const unsigned int ACS_USA_DefaultsecondsPrioThreshold  = 60;   //seconds
const unsigned int ACS_USA_DefaultspecificProblem       = 8977L;
String ACS_USA_DefaultsyslogTimeStampFormat("YYYY-MM-DD hh:mm:ss");

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
			for( ; iElem != List_.end(); iElem++) {
				try {	
					if( NULL != *iElem ) {
						delete *iElem;
						*iElem = NULL;
					}
				}
				catch(...) {
				
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
	shortTermInterval(0),
	recordBufferSize(0),
	syslogRecordsMax(0),
	alarmFilterInterval(0),
	recordsPrioThreshold(0),
	secondsPrioThreshold(0),
	syslogTimeStampFormat(),
	specificProblem(0),
	m_configObject(0),
	latestRecordTimeStamp(0)
{	
	USA_TRACE_ENTER();
	USA_TRACE_LEAVE();	
}


//========================================================================================
//	Destructor
//========================================================================================

ACS_USA_Control::~ACS_USA_Control()
{
	USA_TRACE_ENTER();
}

//========================================================================================
//      initParameters()
//========================================================================================
ACS_USA_ReturnType ACS_USA_Control::initParameters()  {

	USA_TRACE_ENTER();

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

	// Assign the default values
	this->shortTermInterval 	= ACS_USA_DefaultshortTermInterval;
	this->recordBufferSize 		= ACS_USA_DefaultrecordBufferSize;	
	this->syslogRecordsMax 		= ACS_USA_DefaultsyslogRecordsMax;	
	this->alarmFilterInterval 	= ACS_USA_DefaultalarmFilterInterval;	
	this->recordsPrioThreshold 	= ACS_USA_DefaultrecordsPrioThreshold;	
	this->secondsPrioThreshold 	= ACS_USA_DefaultsecondsPrioThreshold;	
	this->specificProblem 		= ACS_USA_DefaultspecificProblem;	
	this->syslogTimeStampFormat 	= ACS_USA_DefaultsyslogTimeStampFormat;	

	USA_TRACE_LEAVE2("tmpName[%s] , ceaseName[%s]", tmpName.c_str(), ceaseName.c_str());
	return ACS_USA_Ok;
}

//========================================================================================
//	createLogDirectory()
//========================================================================================
ACS_USA_ReturnType ACS_USA_Control::createLogDirectory(const String& fileName) const {

	USA_TRACE_ENTER();

	String usaLogPath = fileName;
	
	size_t lastForwardSlash = usaLogPath.find_last_of('/');
	if (lastForwardSlash != String::npos) 
	{			
		usaLogPath.erase(lastForwardSlash);			// Strip away the "usa.tmp"

		// part of the path.		
		if ((access(usaLogPath.data(), F_OK)) == -1) 
		{ 
			// Create the directory
			if(mkdir(usaLogPath.data(), 0777) < 0) {		
				String createFailure = "Failure creating directory: ";
				createFailure.append(usaLogPath.data());
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();
				return ACS_USA_Error;
			}
		}
		else 
		{ 
			// The directory already exists, do nothing
			USA_TRACE_1("Direcotory [%s] Already exists", usaLogPath.data());
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
		return ACS_USA_Error;
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}
ACS_USA_ReturnType ACS_USA_Control::setPerameter(String paramName, int value)
{
	USA_TRACE_ENTER();

	if (paramName == String("retrieveSyslogRecordsMax")) {
		syslogRecordsMax=value;
	}else	if(paramName == String("shortTermInterval")) {
		shortTermInterval=value;
	} else if(paramName == String("recordBufferSize")) {
		recordBufferSize=value;
	} else if(paramName == String("alarmFilterInterval")) {
		alarmFilterInterval=value;
	} else if(paramName == String("recordsPrioThreshold")) {
		recordsPrioThreshold=value;
	} else if(paramName == String("secondsPrioThreshold")) {
		secondsPrioThreshold=value;
	} else if(paramName == String("defaultSpecificProblem")) {
		specificProblem=value;
	} else {
		// Ignore the rest of paramters
		USA_TRACE_1("Ignoring paramName[%s]", paramName.data());
		traCntl.traceAndLogFmt(ERROR, "ACS_USA_Control:%s() - Ignoring paramName[%s]", __func__, paramName.data());
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}
ACS_USA_ReturnType  ACS_USA_Control::loadConfig() {

	USA_TRACE_ENTER();

	char *className = const_cast<char*>("USAConfig");
	String char_value;
	unsigned int int_value=0;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	OmHandler immHandle;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		traCntl.traceAndLogFmt(ERROR,"ACS_USA_Control:%s() Initialization FAILURE",__func__);
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
				if (object.attributes[i].attrName == String("timeStampFormat")){
					this->syslogTimeStampFormat=char_value;
				}else {
					USA_TRACE_1("Ignoring charValue[%s]", char_value.data());
					traCntl.traceAndLogFmt(ERROR, "ACS_USA_Control:%s() - Ignoring char_value[%s]", __func__, char_value.data());
				}
			}else if (object.attributes[i].attrType == 2){
				int_value=*(reinterpret_cast<unsigned int*>(object.attributes[i].attrValues[j]));
				setPerameter(object.attributes[i].attrName, int_value);
			}
		} // end of inner for
	}

	USA_TRACE_LEAVE();	
	return ACS_USA_Ok;
}

//========================================================================================
//	configure()
//========================================================================================
ACS_USA_ReturnType ACS_USA_Control::configure() {

	USA_TRACE_ENTER();

	// Each ACS_USA_Config object should stay in memory as long as 
	// the built ACS_USA_ObjectManager object exists.
	ACS_USA_Initialiser ini;
	ACS_USA_ConfigHandler	cfgStore;
	
	//// Create USA's log directory to hold  'usa.tmp' file if it doesn't exist
	if( createLogDirectory(tmpName) == ACS_USA_Error ) {
		// createLogDirectory() already reported the error
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	// Use one ACS_USA_Config parser per ACF file.
	ACS_USA_Config* cfg = cfgStore.getConfig();
	if( NULL == cfg )
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Control::configure.");
		reportProblem();  
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	ACE_NEW_NORETURN(this->m_configObject, ACS_USA_AnalysisObject());
	if (0 == this->m_configObject){
		error.setError(ACS_USA_Memory);
		error.setErrorText("ACS_USA_AnalysisObject() Memory Creation Failed.");
		USA_TRACE_LEAVE();
		return ACS_USA_Error;		
	}

	// Load the ACF file and parse it.
	if ( cfg->loadIMM(this->m_configObject) == ACS_USA_Error) 
	{
		reportProblem();
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	// Get information about previous analysis results (update)
	// and write the data to the disk

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

			int retCode = rename(tmpName.data(), oldName.data());

			if ( retCode < 0 )
			{
				String createFailure = "Failed to rename file " + tmpName;
				error.setError(ACS_USA_TmpFileIOError);
				error.setErrorText(createFailure.data());
				reportProblem();  
				return ACS_USA_Error;
			}
		}
		else
		{
			// An error occurred
			reportProblem();  
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
	}

	// Check the USA.TMP file
	// This is the first time we use it.
	// It is Ok to delete, a new will be generated next time USA is started.
	if (ini.update(*m_configObject, tmpName.data()) == ACS_USA_Error ||
		ini.flush(*m_configObject, tmpName.data()) == ACS_USA_Error) 
	{		
		// If exist we rename and send a event
		String oldName = tmpName + ".old";
		
		int retCode = rename(tmpName.data(), oldName.data());

		if ( retCode < 0 )
		{
			String createFailure = "Failed to rename file " + tmpName;
			error.setError(ACS_USA_TmpFileIOError);
			error.setErrorText(createFailure.data());
			reportProblem();  
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
		
		String probableCause(ACS_USA_DIAGFAULT);
		String perceivedSeverity(ACS_USA_PERCEIVED_SEVERITY_EVENT);
		error.setError(ACS_USA_TmpFileIOError);
		
		if (eventManager.report(
						ACS_USA_TmpFileIOError + ACS_USA_FirstErrorCode,
						perceivedSeverity,
						probableCause,
						String(),
						String(error.getErrorText()),
						String(error.getProblemText(error.getError())),
						"",								// Node
						0
						) == ACS_USA_Error)
		{
		}
						
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}
    
	//// Create USA's log directory to hold  'cease.tmp' file if it doesn't exist
	if( createLogDirectory(ceaseName) == ACS_USA_Error ){
		// createLogDirectory() already reported the error
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	
	if( (ACS_USA_AutoCeaseManager::getInstance()).loadList(ceaseName) == ACS_USA_Error ){
		// loadList already reported the error
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	USA_TRACE_LEAVE();
    	return ACS_USA_Ok;
}


//========================================================================================
//	initPathConstants()
//========================================================================================

ACS_USA_ReturnType  ACS_USA_Control::initPathConstants() {

	USA_TRACE_ENTER();
	
	strcpy(ACS_USA_logsDefault, "/opt/ap/acs/bin/");

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}

//========================================================================================
//	operate()
//========================================================================================

ACS_USA_ReturnType ACS_USA_Control::operate(	ACS_USA_AnalysisType analysisType, 
						ACS_USA_AnalysisObject *analysisObj, 
						ACS_USA_RecordBuffer *record) {

	USA_TRACE_ENTER();

	(void)analysisType;
	ACS_USA_Initialiser   ini;
	ACS_USA_ReturnType  ret = ACS_USA_Ok;
	
	/*ACS_USA_AnalysisObject analysisObj;
	if (ini.loadTempStorage(&analysisObj, tmpName.data()) == ACS_USA_Error)
	{
		reportProblem();
		return ACS_USA_Error;
	}*/
	 // This dumps the contents of all analysis object and criterions.
	//analysisObj.dump();
	

	ACS_USA_Analyser analyser;
	if(analysisType == ACS_USA_Instant_Analysis) {
		if(NULL != strstr(record, "SecurityEvent"))
		{
			traCntl.traceAndLogFmt(ERROR, "ACS_USA_Control:%s() - record[%s]", __func__, (const char *)record);
			USA_TRACE_LEAVE();
			return ret;
		}

		ACS_USA_MatchType retCode;
		retCode = analyser.analyseRecord(	analysisObj, 
							alarmFilterInterval, 
							suppressedAlarms, 
							analysisType, 
							record);
		
		if(retCode == Criterion_match) {
			if (ini.flush(*analysisObj, tmpName.data()) == ACS_USA_Error)
			{
				reportProblem();
				return ACS_USA_Error;
			}
		}

	}
	else
	{
		ret = analyser.analyseObject(	analysisObj, 
						latestRecordTimeStamp, 
						alarmFilterInterval, 
				     		suppressedAlarms, 
						recordBufferSize,
						this->syslogRecordsMax,
						analysisType);
		if (ret == ACS_USA_Error) {
			reportProblem();
		}
		/*if (analysisType == Activation_startUp) {
			// Do not save to temp storage, since start-up analysis
			// only means finding the latest boot event, no record
			// matching a'la short or long term analysis
			return ret;
		}*/
		if (analyser.getAlarmSetOnMissedBootEvent() == TRUE) {
			 // If an alarm on 'too high event frequency'
			 // has been raised in first short term,
			 // don't raise it again in long term.

			 //raisedAlarmOnEventFrequency = TRUE;
		}
		// Save into temp storage even if error occurred during analysis
		if (ini.flush(*analysisObj, tmpName.data()) == ACS_USA_Error)
		{
			reportProblem();
			return ACS_USA_Error;
		}
	}

	// Return the analysis result. If 'flush error' return is made already.
	USA_TRACE_LEAVE();

	return ret;
}

//========================================================================================
//	reportProblem()
//========================================================================================
void ACS_USA_Control::reportProblem() const {

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

//========================================================================================
//	dumpAll()
//========================================================================================

void ACS_USA_Control::dumpAll(ACS_USA_AnalysisObject& analysisObject) {
	(void)analysisObject;	
	USA_TRACE_ENTER();
	
	analysisObject.dump();

	USA_TRACE_LEAVE();
	return;
}

//----------------------------------------------------------------------------------------
String ACS_USA_Control::getUSATmpFileName() {
	return tmpName;
}	


//----------------------------------------------------------------------------------------
String ACS_USA_Control::getCeaseFileName() {
	return ceaseName;
}	
//----------------------------------------------------------------------------------------
