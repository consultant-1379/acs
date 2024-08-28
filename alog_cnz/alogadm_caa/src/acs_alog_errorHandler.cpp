/*=================================================================== */
   /**
   @file acs_alog_errorHandler.cpp

   Class method implementationn for acs_alog_errorHandler type module.

   This module contains the implementation of class declared in
   the acs_alog_errorHandler.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       02/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
  =================================================================== */

#include "acs_alog_errorHandler.h"

acs_alog_errorHandler::acs_alog_errorHandler()
{
	//instance for TRA Logging
//	Log = new ACS_TRA_Logging();
	Log.Open("ALOG");

	//instance for TRA EVENT
	traceEvent = new ACS_TRA_trace("ALOG","C512");

	//instance for AEH
	myAlarm = new acs_aeh_evreport;
	errorText = "";
}


int acs_alog_errorHandler::InternalLogging(string log, ACS_TRA_LogLevel Log_Level)
{
	ACS_TRA_LogResult result;

	result = Log.Write(log.c_str(),Log_Level);

	if (result != TRA_LOG_OK)  return -1;
	return 0;
}


void acs_alog_errorHandler::TRAEventString(string message)
{

	if (traceEvent->ACS_TRA_ON())
	{
		traceEvent->ACS_TRA_event(1, message.c_str());
	}
}


int acs_alog_errorHandler::AEHEventMessage(const char * processName,long int specificProblem, const char * Severity, const char * probCause, const char * classReference, const char * objOfReference,const char * problemData, const char * problemText )
{
	ACS_AEH_ReturnType ret;
	ret=myAlarm->sendEventMessage(processName,specificProblem,Severity,probCause,classReference,objOfReference,problemData,problemText);

	if ( ret != ACS_AEH_ok ){
		switch ( myAlarm->getError() ){
		case ACS_AEH_syntaxError:
			errorText =  myAlarm->getErrorText();
			break;
		case ACS_AEH_eventDeliveryFailure:
			errorText =  myAlarm->getErrorText();
			break;
		case ACS_AEH_genericError:
			errorText =  myAlarm->getErrorText();
			break;
		case ACS_AEH_noErrorType:
			break;
		}
		return -1;
	}

	return 0;

}


acs_alog_errorHandler::~acs_alog_errorHandler()
{
//	delete Log;
	Log.Close();	//Fix for TR HV80013
	delete traceEvent;
	delete myAlarm;

}
