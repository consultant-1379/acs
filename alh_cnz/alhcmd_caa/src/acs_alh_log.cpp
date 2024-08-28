/*
 * acs_alh_log.cpp
 *
 *  Created on: Nov 04, 2011
 *      Author: xgiopap
 */
#include <cstdarg>
#include <stdio.h>
#include "acs_alh_log.h"

#define ACS_ALH_LOG_MESSAGE_SIZE_MAX 4096
#define ACS_ALH_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_log::acs_alh_log() : log_obj_()
{
	log_obj_.Open("ALH");
}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_log::~acs_alh_log()
{
}


//========================================================================================
//  trace method
//========================================================================================
int acs_alh_log::write(ACS_TRA_LogLevel Log_Level, const char* Log_Message, ...)
{
	char log_msg[ACS_ALH_LOG_MESSAGE_SIZE_MAX];
	va_list argp;
	va_start(argp, Log_Message);
	vsnprintf(log_msg, ACS_ALH_ARRAY_SIZE(log_msg), Log_Message, argp);
	log_msg[ACS_ALH_ARRAY_SIZE(log_msg) - 1] = 0;
	va_end(argp);

	log_obj_.Write(log_msg, Log_Level);

	return 0;
}
