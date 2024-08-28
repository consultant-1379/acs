/*
 * Demo.cpp
 *
 *  Created on: Mar 4, 2011
 *      Author: xgiopap
 */

#include "Demo.h"
#include <string>
#include "/vobs/cm4ap/ntacs/tra_cnz/traapi_caa/inc_ext/ACS_TRA_Logging.h"

ACS_TRA_Logging Log;

//FILE *			m_file;


Demo::Demo(std::string Subsystem)
{
	Log.Open(Subsystem.c_str());
	//m_file = fopen ("/var/log/acs/tra/logging/printf.log", "w");
}

int Demo::svc ( void ){

	int i = 0;
	char* chrTemp = 0;
	while (i >= 0)
	{
		Log.Write(chrTemp, LOG_LEVEL_INFO);

		Log.Write("LOG_LEVEL_TRACE - 1", LOG_LEVEL_TRACE);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_TRACE - 1\n");
		Log.Write("LOG_LEVEL_DEBUG - 2", LOG_LEVEL_DEBUG);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_DEBUG - 2\n");
		Log.Write("LOG_LEVEL_INFO - 3", LOG_LEVEL_INFO);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_INFO - 3\n");
		Log.Write("LOG_LEVEL_WARN - 4", LOG_LEVEL_WARN);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_WARN - 4\n");
		Log.Write("LOG_LEVEL_ERROR - 5", LOG_LEVEL_ERROR);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_ERROR - 5\n");
		Log.Write("LOG_LEVEL_FATAL - 6", LOG_LEVEL_FATAL);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_FATAL - 6\n");
		Log.Write("LOG_LEVEL_TRACE - 7", LOG_LEVEL_TRACE);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_TRACE - 7\n");
		Log.Write("LOG_LEVEL_DEBUG - 8", LOG_LEVEL_DEBUG);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_DEBUG - 8\n");
		Log.Write("LOG_LEVEL_INFO - 9", LOG_LEVEL_INFO);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_INFO - 9\n");
		Log.Write("LOG_LEVEL_WARN - 10", LOG_LEVEL_WARN);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_WARN - 10\n");
		Log.Write("LOG_LEVEL_ERROR - 11", LOG_LEVEL_ERROR);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_ERROR - 11\n");
		Log.Write("LOG_LEVEL_FATAL - 12", LOG_LEVEL_FATAL);
		//		fprintf(m_file, "072008 23:56:13 10107 LOG_LEVEL_FATAL - 12\n");
		usleep(10000000);
		i++;
	}

	return 0;
}


Demo::~Demo()
{
	// TODO Auto-generated destructor stub
}
