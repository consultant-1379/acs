/*=================================================================== */
/**
   @file   acs_alog_errorHandler.h

   @brief Header file for acs_alog_errorHandler type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       28/11/2011     xgencol               Final Release
==================================================================== */

#ifndef ACS_ALOG_ERRORHANDLER_H_
#define ACS_ALOG_ERRORHANDLER_H_


#include <iostream>
#include <fstream>
#include <string.h>

#include "ACS_TRA_Logging.h"
#include "ACS_TRA_trace.h"
#include "acs_aeh_error.h"
#include "acs_aeh_evreport.h"

using namespace std;

class acs_alog_errorHandler {
private:
	ACS_TRA_Logging  Log;
	ACS_TRA_trace* traceEvent;
	acs_aeh_evreport* myAlarm;
	string errorText;
public:

	/**@brief
		 *  default constructor for the inizialization
		 *
		 */
	acs_alog_errorHandler();

	/**@brief
	 *  This method give an interface to the log4C class throw the TRA lib
	 *
	 *
	 *	@param[in]    string				log message
	 *	@param[in]    ACS_TRA_LogLevel		Log Level
	 *
	 *
	 *  @return int  0 for OK, -1 for KO
	 *
	 *
	 */
	int InternalLogging(string log, ACS_TRA_LogLevel Log_Level);

	/**@brief
		 *  This method give an interface to the TRA Event handler
		 *  sending a string message to the TRA EVENT catcher
		 *
		 *
		 *	@param[in]    string				message
		 *
		 *
		 *  @return int 0 for OK, -1 for KO
		 *
		 */
	void TRAEventString(string message);

	/**@brief
		 *  This method call the sendEventMessage from AEH CLass
		 *
		 *
		 *	@param[in]
		 *
		 *
		 *  @return int 0 for OK, -1 for KO
		 *
		 */
	int AEHEventMessage(const char * processName,long int specificProblem, const char * Severity, const char * probCause, const char * classReference, const char * objOfReference,const char * problemData, const char * problemText);

	/**@brief
		 *  This method return the error message for AEH sendEvent Method
		 *
		 *
		 *
		 *  @return string
		 *
		 */
	inline string getAEHErrorMEssage()	{ return errorText;};

	~acs_alog_errorHandler();
};


#endif /* ACS_ALOG_ERRORHANDLER_H_ */
