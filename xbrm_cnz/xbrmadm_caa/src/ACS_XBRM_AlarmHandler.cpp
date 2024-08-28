/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/
/**
*
* @file ACS_XBRM_AlarmHandler.cpp
*
* @brief
* ACS_XBRM_AlarmHandler Class for System backups Alarm handling
*
* @details
* Implementation of ACS_XBRM_AlarmHandler class to handle the alarms in case of system backups create/export failed
*
* @author XHARBAV
*
-------------------------------------------------------------------------*/
/*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-09-07  XHARBAV  Created First Revision
 *
 ****************************************************************************/

#include "ACS_XBRM_AlarmHandler.h"
#include "acs_aeh_evreport.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_Utilities.h"
#include <ace/ACE.h>
#include <sstream>
#include <fstream>

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_AlarmHandler);
//------------
// Constructor
//------------
ACS_XBRM_AlarmHandler::ACS_XBRM_AlarmHandler()
	: m_Mutex(),
	  m_EventReportObj(new(std::nothrow) acs_aeh_evreport())
	  
{
	ACS_XBRM_TRACE_FUNCTION;
	createAlarmFile();
	// assemble process name with pid
	pid_t processPid = getpid();
	std::stringstream procName;
	procName << ACS_XBRM_UTILITY::APPNAME << ":" << processPid << std::ends;
	m_ProcessName = procName.str();
}

//-----------
// Destructor
//-----------
ACS_XBRM_AlarmHandler::~ACS_XBRM_AlarmHandler()
{
	ACS_XBRM_TRACE_FUNCTION;
	delete m_EventReportObj;
}

void ACS_XBRM_AlarmHandler::createAlarmFile()
{
	ACS_XBRM_TRACE_FUNCTION;
	
	string fileToCreate = ACS_XBRM_UTILITY::alarmDir + ACS_XBRM_UTILITY::alarmFile;
	ACE_stat statBuff;
	
	if (ACE_OS::stat(fileToCreate.c_str(), &statBuff) != 0)
	{
		ACS_XBRM_LOG(LOG_LEVEL_INFO,"Alarm file not exist, create it");
		ACS_XBRM_TRACE_MESSAGE("Alarm file not exist, create it");
		ACE_OS::mkdir(ACS_XBRM_UTILITY::alarmDir.c_str());
	
		ofstream file(fileToCreate.c_str());
		file<<"";
	}
	else
	{
		ACS_XBRM_LOG(LOG_LEVEL_INFO,"Alarm file already exist");
		ACS_XBRM_TRACE_MESSAGE("Alarm file already exist");
	}
}
bool ACS_XBRM_AlarmHandler::readAlarmFile()
{
	ACS_XBRM_TRACE_FUNCTION;
	
	string fileToRead = ACS_XBRM_UTILITY::alarmDir + ACS_XBRM_UTILITY::alarmFile;
	string myline;
	ifstream inFile;
	bool isAlarm = false;
	inFile.open(fileToRead.c_str());
	
	if (inFile.is_open())
	{
		while (getline(inFile, myline))
		{
			
			if(!myline.empty())
			{
				isAlarm  = true;
				break;
			}
		}
		inFile.close();
	}
	else{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR,"Unable to read alrmFile");
	}
	return isAlarm;
}

void ACS_XBRM_AlarmHandler::writeAlarmFile(string data)
{
	ACS_XBRM_TRACE_FUNCTION;
	string fileToWrite = ACS_XBRM_UTILITY::alarmDir + ACS_XBRM_UTILITY::alarmFile;
	ACE_stat statBuff;
	
	
	if (ACE_OS::stat(fileToWrite.c_str(), &statBuff) == 0)
	{
		ACS_XBRM_LOG(LOG_LEVEL_INFO,"Alarm raised/ceased. Write content to file");
		ACS_XBRM_TRACE_MESSAGE("Alarm raised/ceased. Write content to file");	
		ofstream file(fileToWrite.c_str());
		file<<data.c_str();
	}
	else
	{
		ACS_XBRM_LOG(LOG_LEVEL_ERROR,"Unable to update alarmfile, not exist");
	}
	
}


//--------------------------------------------------------------------------------------
// initAlarm
//
//--------------------------------------------------------------------------------------
void ACS_XBRM_AlarmHandler::initAlarm(const char *backupType)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	ACS_XBRM_TRACE_FUNCTION;
	ACS_AEH_ReturnType aehResult = ACS_AEH_error;
	std::stringstream alarmText;
	if (backupType == NULL)
	{
		if(!readAlarmFile()){
			alarmText << "\nCAUSE\n"
					  << alarmInfo::problemText2 << std::ends;
		}else{
			string fileToRead = ACS_XBRM_UTILITY::alarmDir + ACS_XBRM_UTILITY::alarmFile;
			string myline;
			ifstream inFile;
			inFile.open(fileToRead.c_str());
			
			if (inFile.is_open())
			{
				while (getline(inFile, myline))
				{
					if(0 == strcmp(myline.c_str(), "CAUSE"))
						alarmText << "\n" << myline << "\n";
					else
						alarmText << myline;
				}
				inFile.close();
			}
			else{
				ACS_XBRM_LOG(LOG_LEVEL_ERROR,"Unable to read alrmFile");
			}
		}
		
	}
	else
	{

		alarmText << "\nCAUSE\n"
				  << backupType << " " << alarmInfo::problemText1 << std::ends;
		
	}

	aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
												   alarmInfo::specificProblem,
												   alarmInfo::category,
												   alarmInfo::probableCause,
												   alarmInfo::classReferenceClass,
												   alarmInfo::objReferenceClass,
												   // alarmInfo::problemData2,
												   "",
												   alarmText.str().c_str(),
												   true); // Manual cease

	if (ACS_AEH_ok != aehResult)
	{

		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to raise alarm , error %d, Message %s ", m_EventReportObj->getError(), m_EventReportObj->getErrorText());
	}
	writeAlarmFile(alarmText.str());
}

//******************************************************************************
//
void ACS_XBRM_AlarmHandler::ceaseAlarm()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	ACS_XBRM_TRACE_FUNCTION;
	ACS_AEH_ReturnType aehResult = ACS_AEH_error;

	aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
												   alarmInfo::specificProblem,
												   alarmInfo::ceaseText,
												   alarmInfo::probableCause,
												   alarmInfo::classReferenceClass,
												   alarmInfo::objReferenceClass,
												   "",
												   "");

	if (ACS_AEH_ok != aehResult)
	{

		ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to cease alarm, error %d, Message %s ", m_EventReportObj->getError(), m_EventReportObj->getErrorText());
	}
	writeAlarmFile("");
}
