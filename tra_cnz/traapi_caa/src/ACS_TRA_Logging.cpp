/*
 * NAME: ACS_TRA_lib.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Eri#include "ACS_TRA_common.h"csson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This file contains the definitions of all methods
 *  of class ACS_TRA_Logging
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 	2010-05-26 XYV XLUCPET
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	        20100526	XLUCPET		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 * PA2 	        20101104	XGIOPAP		Implementation of comment
 */

#include <sys/stat.h>

#include <ace/ACE.h>

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"

#define ACS_TRA_USE_INTERNAL_CONFIG_WATCH_THREAD

#include "ACS_TRA_ConfigChangeWatchThread.h"
#include "ACS_TRA_Logging.h"
#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"

using namespace log4cplus;
using namespace std;

namespace {
#ifdef ACS_TRA_USE_INTERNAL_CONFIG_WATCH_THREAD
	ACS_TRA_ConfigChangeWatchThread config_watch_thread;
#endif
}

ACE_Recursive_Thread_Mutex ACS_TRA_Logging::number_of_ConfigureAndWatchThread_Mutex;
int ACS_TRA_Logging::number_of_ConfigureAndWatchThread = 0;
ConfigureAndWatchThread* ACS_TRA_Logging::configureThread = 0;

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ACS_TRA_Logging
 * Description: Class constructor
 * Used: This method is used in the following member:
 *       -
 */
ACS_TRA_Logging::ACS_TRA_Logging() : LoggerName(const_cast<char *>("UNKNOW.log")), Is_Open(false){
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ACS_TRA_Log_Open
 * Description: Open the log file
 * Used: This method is used in the following member:
 *       -
 */
ACS_TRA_LogResult ACS_TRA_Logging::ACS_TRA_Log_Open(const char* Subsystem)
{
	string newSubsystem;

	// duplicate and allocate the Subsystem variable
	newSubsystem = Subsystem;

	// Concatenate the .log extension with the Subsystem
	newSubsystem.append(".log");

	/*
	 * If the subsystem not exists in the
	 * configuration file, it automatically
	 * logging in unknow.log file
	 */
	if (!(Logger::exists(newSubsystem)))
	{
		PropertyConfigurator::doConfigure(LOG4CPLUSPATH);

		if (!(Logger::exists(newSubsystem)))
			newSubsystem = "UNKNOW.log";
	}

	/*
	 * Check if exists the configuration file
	 * of log4cplus, if not exist return a
	 * proper error code (TRA_LOG_ERROR_CONF_FILE)
	 */
	if (this->CheckPath(LOG4CPLUSPATH))
	{
		//PropertyConfigurator::doConfigure(LOG4CPLUSPATH);
	}
	else
	{
		this->Is_Open = false;
		setError (TRA_LOG_ERROR_CONF_FILE, "Error in Log4CPlus configuration file");
		return TRA_LOG_ERROR_CONF_FILE;
	}

	// Set the Subsystem
	m_Subsystem = newSubsystem;

	/*
	 * Create an instance of Logger class,
     * if already exist return the point
     * of the object
     */
	Logger logger = Logger::getInstance(newSubsystem);

	/*
	 * Start the thread to watch and reload
	 * the configuration file if any changes
	 * occurs
	 */
	if (this->CheckPath(LOG4CPLUSPATH)) {
		ACE_Guard<ACE_Recursive_Thread_Mutex> The_Guard(ACS_TRA_Logging::number_of_ConfigureAndWatchThread_Mutex);
		if ( !number_of_ConfigureAndWatchThread ) {
#ifdef ACS_TRA_USE_INTERNAL_CONFIG_WATCH_THREAD
			config_watch_thread.start(LOG4CPLUSPATH);
#else
			configureThread = new ConfigureAndWatchThread( LOG4CPLUSPATH, NUMBER_OF_SECOND * 1000 );
#endif
		}

		number_of_ConfigureAndWatchThread++;
		this->Is_Open = true;

		return TRA_LOG_OK;
	}
	else
	{
		this->Is_Open = false;
		setError (TRA_LOG_ERROR_CONF_FILE, "Error in Log4CPlus configuration file");
		return TRA_LOG_ERROR_CONF_FILE;
	}
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: Open
 * Description: Open the log file
 * Used: This method is used in the following member:
 *       -
 */
ACS_TRA_LogResult ACS_TRA_Logging::Open(const char* Subsystem)
{
	string newSubsystem;

	// duplicate and allocate the Subsystem variable
	newSubsystem = Subsystem;

	// Concatenate the .log extension with the Subsystem
	newSubsystem.append(".log");

	/*
	 * If the subsystem not exists in the
	 * configuration file, it automatically
	 * logging in unknow.log file
	 */
	if (!(Logger::exists(newSubsystem)))
	{
		PropertyConfigurator::doConfigure(LOG4CPLUSPATH);

		if (!(Logger::exists(newSubsystem)))
			newSubsystem = "UNKNOW.log";
	}

	/*
	 * Check if exists the configuration file
	 * of log4cplus, if not exist return a
	 * proper error code (TRA_LOG_ERROR_CONF_FILE)
	 */
	if (this->CheckPath(LOG4CPLUSPATH))
	{
		//PropertyConfigurator::doConfigure(LOG4CPLUSPATH);
	}
	else 
	{
		this->Is_Open = false;
		setError (TRA_LOG_ERROR_CONF_FILE, "Error in Log4CPlus configuration file");
		return TRA_LOG_ERROR_CONF_FILE;
	}

	// Set the Subsystem
	m_Subsystem = newSubsystem;

	 /*
	  * Create an instance of Logger class,
      * if already exist return the point
     * of the object
     */
	Logger logger = Logger::getInstance(newSubsystem);

	/*
	 * Start the thread to watch and reload
	 * the configuration file if any changes
	 * occurs
	 */
	if (this->CheckPath(LOG4CPLUSPATH))
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> The_Guard(ACS_TRA_Logging::number_of_ConfigureAndWatchThread_Mutex);
		if ( !number_of_ConfigureAndWatchThread ) {
#ifdef ACS_TRA_USE_INTERNAL_CONFIG_WATCH_THREAD
			config_watch_thread.start(LOG4CPLUSPATH);
#else
			configureThread = new ConfigureAndWatchThread( LOG4CPLUSPATH, NUMBER_OF_SECOND * 1000 );
#endif
		}

		number_of_ConfigureAndWatchThread++;

		this->Is_Open = true;
		return TRA_LOG_OK;
	}
	else 
	{
		this->Is_Open = false;
		setError (TRA_LOG_ERROR_CONF_FILE, "Error in Log4CPlus configuration file");
		return TRA_LOG_ERROR_CONF_FILE;
	}
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ACS_TRA_Log
 * Description: API to permit each application to
 *  			log its information on a file
 * Used: This method is used in each application
 * 		 who want trace on a file
 */
ACS_TRA_LogResult ACS_TRA_Logging::ACS_TRA_Log(const char* Log_Message,  ACS_TRA_LogLevel Log_Level)
{
	return Write(Log_Message, Log_Level);
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: Write
 * Description: API to permit each application to
 *  			log its information on a file
 * Used: This method is used in each application
 * 		 who want trace on a file
 */
ACS_TRA_LogResult ACS_TRA_Logging::Write(const char * Log_Message,  ACS_TRA_LogLevel Log_Level)
{
	if (!(Log_Message && *Log_Message)) {
		setError (TRA_LOG_ERROR_LOG_LEVEL, "Invalid Log Message");
		return TRA_LOG_ERROR_LOG_LEVEL;
	}

	const size_t tmp_LogMessageSize = ::strlen(Log_Message) + 1;

	char tmp_LogMessage[tmp_LogMessageSize];

//	char* tmp_LogMessage = new char [tmp_LogMessageSize];
//	::memset(tmp_LogMessage, 0, tmp_LogMessageSize);
	*tmp_LogMessage = '\0';

	::strncpy(tmp_LogMessage, Log_Message, tmp_LogMessageSize);
	tmp_LogMessage[tmp_LogMessageSize - 1] = '\0';

	if (ACS_TRA_Log_IsOpen() && !m_Subsystem.empty())
	{
		switch (Log_Level)
		{
			case LOG_LEVEL_TRACE:
				LOG4CPLUS_TRACE(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			case LOG_LEVEL_DEBUG: 
				LOG4CPLUS_DEBUG(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			case LOG_LEVEL_INFO:
				LOG4CPLUS_INFO(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			case LOG_LEVEL_WARN: 
				LOG4CPLUS_WARN(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			case LOG_LEVEL_ERROR: 
				LOG4CPLUS_ERROR(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			case LOG_LEVEL_FATAL: 
				LOG4CPLUS_FATAL(Logger::getInstance(m_Subsystem.c_str()), LOG4CPLUS_TEXT( tmp_LogMessage ));
				break;
			default:
				setError (TRA_LOG_ERROR_LOG_LEVEL, "Unknow Log Level");
//				delete tmp_LogMessage;
				return TRA_LOG_ERROR_LOG_LEVEL;
				break;
		}

//		delete tmp_LogMessage;
		return TRA_LOG_OK;
	}
	else 
	{
		setError (TRA_LOG_ERROR, "Illegal use of the API");
//		delete tmp_LogMessage;
		return TRA_LOG_ERROR;
	}
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ACS_TRA_Log_Close
 * Description: Calling this method will safely close
 * 				and remove all appenders in all the
 * 				loggers including root contained in the
 * 				default hierachy
 * Used: This method is used in the following member:
 * 		 ACS_TRA_Logging::~ACS_TRA_Logging in
 * 		 ACS_TRA_Logging.cpp file
 */
void ACS_TRA_Logging::ACS_TRA_Log_Close(){
	//Logger::getInstance(m_Subsystem.c_str()).shutdown();

	Close();
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: Close
 * Description: Calling this method will safely close
 * 				and remove all appenders in all the
 * 				loggers including root contained in the
 * 				default hierachy
 * Used: This method is used in the following member:
 * 		 ACS_TRA_Logging::~ACS_TRA_Logging in
 * 		 ACS_TRA_Logging.cpp file
 */
void ACS_TRA_Logging::Close(){
	//Logger::getInstance(m_Subsystem.c_str()).shutdown();
	Is_Open = false;

	ACE_Guard<ACE_Recursive_Thread_Mutex> The_Guard(ACS_TRA_Logging::number_of_ConfigureAndWatchThread_Mutex);

	number_of_ConfigureAndWatchThread--;

	if ( number_of_ConfigureAndWatchThread == 0) {
#ifdef ACS_TRA_USE_INTERNAL_CONFIG_WATCH_THREAD
		config_watch_thread.stop();
#else
		delete configureThread;
#endif
	}
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ACS_TRA_Log_IsOpen
 * Description: Check if the log4cplus configuration
 * 				has been done in correct way
 * Used: This method is used in the following member:
 * 		 ACS_TRA_Logging::ACS_TRA_Log in
 * 		 ACS_TRA_Logging.cpp file
 */
bool ACS_TRA_Logging::ACS_TRA_Log_IsOpen() 
{
	return Is_Open;
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: ~ACS_TRA_Logging
 * Description: Class destructor
 * Used: This method is not used.
 */
ACS_TRA_Logging::~ACS_TRA_Logging() {
	if (this->ACS_TRA_Log_IsOpen())
		this->Close();
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: CheckPath
 * Description: Check if the specified path exists
 * Used: This method is used in the following member:
 * 		 ACS_TRA_Logging::ACS_TRA_Log_Open in
 * 		 ACS_TRA_Logging.cpp file
 */
bool ACS_TRA_Logging::CheckPath(const char* userPath)
{
   struct stat statBuf;

   if(stat(userPath, &statBuf))
         return false;

   return true;
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: setError
 * Description: Set the error message
 * Used: This method is used in the following member:
 * 		 ACS_TRA_Logging::ACS_TRA_Log_Open and
 *		 ACS_TRA_Logging::ACS_TRA_Log in
 * 		 ACS_TRA_Logging.cpp file
 */
void ACS_TRA_Logging::setError(ACS_TRA_LogResult err, const char* errText)
{
	error = err;
	errorText = errText;
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: getLastError
 * Description: Return the las error message
 * Used: This method is not used
 */
ACS_TRA_LogResult ACS_TRA_Logging::getLastError(void) const
{
	return error;
}

/*
 * Class Name: ACS_TRA_Logging
 * Method Name: getLastErrorText
 * Description: Return the las error text message
 * Used: This method is not used
 */
const char* ACS_TRA_Logging::getLastErrorText(void) const
{
	return errorText.c_str();
}
