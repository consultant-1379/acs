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
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_AlarmHandler.h
 *
 * @brief
 * ACS_XBRM_AlarmHandler Class for System backups Alarm handling
 *
 * @details
 * Implementation of ACS_XBRM_AlarmHandler class to handle the alarms in case of system backups create/export failed
 *
 * @author XHARBAV
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-09-07  XHARBAV  Created First Revision
 *
 ****************************************************************************/


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_XBRM_ALARMHANDLER_H
#define ACS_XBRM_ALARMHANDLER_H

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Recursive_Thread_Mutex.h>
#include "acs_aeh_evreport.h"
#include <string>
#include <map>

namespace alarmInfo
{
	//const char probableCause1[] = "DATA OUTPUT, AP TRANSMISSION FAULT";
	const char probableCause[] = "SCHEDULED BACKUP FAILED";
	const char category[] = "A2";
	const char ceaseText[] = "CEASING";
	const char classReferenceClass[] = "APZ";
	const char objReferenceClass[] = "acs_xbrmd/Alarm";

	//const long specificProblem1 = 7201;
	const char problemData2[] = "BACKUP EXPORT FAILED";
	const char problemText2[] = "BACKUP EXPORT FAILED";

	const long specificProblem = 8901;
	const char problemData1[] = "BACKUP CREATION FAILED";
	const char problemText1[] = "BACKUP CREATION FAILED";

	//const char problemText2part1[] = "CAUSE\nRECORD TRANSFER, DESTINATION\n \nFILE NAME\n-\n \nTRANSFER QUEUE\ntransferQueueId=";
	//const char problemText2part2[] = "DESTINATION SET\n-\n \nDESTINATION\n-\n";
}




class acs_aeh_evreport;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class ACS_XBRM_AlarmHandler
{
 public:

	/**
	   @brief    Constructor for ACS_XBRM_AlarmHandler class.
	   @param    proc
	   @param    suppressAlarm
	*/
	ACS_XBRM_AlarmHandler();

    virtual ~ACS_XBRM_AlarmHandler();

	
	void createAlarmFile();
	bool readAlarmFile();
	void initAlarm( const char* backupType = NULL);
	void writeAlarmFile(std::string data);
	void ceaseAlarm();

 private:

	

	/**
	 * 	@brief	m_mutex
	 *
	 * 	Mutex for internal map access
	*/
	ACE_Recursive_Thread_Mutex m_Mutex;

	/**
	 * 	@brief	m_EventReportObj
	 *
	*/
	acs_aeh_evreport* m_EventReportObj;

	

	/**
	 * 	@brief	m_ProcessName
	*/
	std::string m_ProcessName;

};

#endif
