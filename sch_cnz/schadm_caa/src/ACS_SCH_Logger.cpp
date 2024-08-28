/*
 * ACS_SCH_Logger.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: XMANVEN
 */
#include "ACS_SCH_Logger.h"
#include <ace/OS_NS_dirent.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <ostream>

ACS_TRA_Logging* ACS_SCH_Logger::ACS_SCH_logging = NULL;

ACS_TRA_Logging* ACS_SCH_Logger::getLogInstance ()
{
	if (ACS_SCH_logging == NULL)
	{
		ACS_SCH_logging = new ACS_TRA_Logging();
		ACS_SCH_logging->Open("SCH");
		ACS_SCH_Logger::ACS_SCH_logging->Write(" ACS_SCH - Create Instance for logging !!! ",LOG_LEVEL_DEBUG);

	}

	return ACS_SCH_logging;
}

void ACS_SCH_Logger::closeLogInstance ()
{
	if (ACS_SCH_logging)
	{
		ACS_SCH_logging->Write(" ACS_SCH - Destroy Instance for logging !!! ",LOG_LEVEL_DEBUG);
		ACS_SCH_logging->Close();
		delete(ACS_SCH_logging);
		ACS_SCH_logging=NULL;
	}
}
/******************************************************************************
 *
 * ROUTINE:createTRAObjects()
 *
 *****************************************************************************/

bool ACS_SCH_Logger::createTRAObjects()
{
	ACS_SCH_Logger::ACS_SCH_logging = new ACS_TRA_Logging();
	if ( ACS_SCH_Logger::ACS_SCH_logging == NULL)
	{
		syslog(LOG_INFO,"Failed to create ACS_TRA_Logging");
		return false;
	}
	ACS_TRA_LogResult retVal = ACS_SCH_Logger::ACS_SCH_logging->Open("SCH");
	bool myresult = false;
	switch(retVal)
	{
	case TRA_LOG_OK:
		myresult = true;
		break;
	case TRA_LOG_ERROR:
	case TRA_LOG_ERROR_CONF_FILE:
	case TRA_LOG_ERROR_LOG_LEVEL:
		myresult = false;
		break;
	}
	if(myresult == false)
		syslog(LOG_INFO,"Failed to open the TRA log file");
	return myresult;
}
/******************************************************************************
 *
 * ROUTINE:deleteTRAObjects()
 *
 *****************************************************************************/
void ACS_SCH_Logger::deleteTRAObjects()
{
	std::cout << "Deleting Tra Objects"<<__FUNCTION__ <<"@"<<__LINE__<<std::endl;
	if( ACS_SCH_Logger::ACS_SCH_logging != 0)
	{
		ACS_SCH_Logger::ACS_SCH_logging->Close();
		delete ACS_SCH_Logger::ACS_SCH_logging;
		ACS_SCH_Logger::ACS_SCH_logging = 0 ;
	}
}

void ACS_SCH_Logger::log(std::string anErrMsg, const ACS_SCH_Origin& anOrigin, ACS_TRA_LogLevel aLogLevel)
{
	std::ostringstream myStr;
	myStr << anOrigin << " : " << anErrMsg  << std::ends; // " LWP: " << syscall(SYS_gettid) << std::ends;
	std::cout<<myStr;
	ACS_SCH_Logger::ACS_SCH_logging->Write(myStr.str().c_str(), aLogLevel);
}

bool ACS_SCH_Logger::isTralogInstanceExist()
{
	if(ACS_SCH_Logger::ACS_SCH_logging == NULL )
	{
		return false;
	}
	return true;
}


