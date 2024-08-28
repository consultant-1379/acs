
/*! \addtogroup alarm "Alarm Handling"
 *
 *   Raising and Ceasing of Alarms.
 *
 *  @{
 */

//=============================================================================
/**
 *  @file    ACS_SSU_AlarmHandler.cpp
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *
 *  @author 2010-07-12 by XSIRKUM
 *
 *  @documentno 190 89-CAA 109 0868
 *
 */
//=============================================================================

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
   ==================================================================== */
#include <stdio.h>
#include <ace/Synch.h>
//! GCC Util classes
#include <ACS_APGCC_Util.H>
#include "acs_ssu_common.h"
#include "acs_ssu_types.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_alarm_handler.h"

static const int g_nNumOfSeverityClasses = 6;
static const ACE_TCHAR* g_vSeverityClasses[] = {
	ACE_TEXT("Event"),
	ACE_TEXT("O2"),
	ACE_TEXT("O1"),
	ACE_TEXT("A3"),
	ACE_TEXT("A2"),
	ACE_TEXT("A1")
};
#define PARAM_LEN                    2048
#define POPEN_READ 0
#define POPEN_WRITE 1


pid_t ACS_SSU_AlarmHandler::popen_with_pid(std::string command, int *infp, int *outfp)
{
        int p_stdin[2], p_stdout[2];
        pid_t pid = -1;

        if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
                return -1;

        pid = fork();

        if (pid < 0)
                return pid;
        else if (pid == 0)
        {
                close(p_stdin[POPEN_WRITE]);
                dup2(p_stdin[POPEN_READ], POPEN_READ);
                close(p_stdout[POPEN_READ]);
                dup2(p_stdout[POPEN_WRITE], POPEN_WRITE);
                dup2(p_stdout[POPEN_WRITE],2);

                execl("/bin/sh", "sh", "-c", command.c_str(), NULL);
                perror("execl");
                exit(1);
        }

        if (infp == NULL)
                close(p_stdin[POPEN_WRITE]);
        else
                *infp = p_stdin[POPEN_WRITE];

        if (outfp == NULL)
        	close(p_stdout[POPEN_READ]);
        else
        	*outfp = p_stdout[POPEN_READ];

        close(p_stdin[POPEN_READ]);		//HX58640
        close(p_stdout[POPEN_WRITE]);	//HX58640

        return pid;
}//End of popen_with_pid

ACS_SSU_AlarmHandler::ACS_SSU_AlarmHandler()
{
	DEBUG("%s","Entering ACS_SSU_AlarmHandler::ACS_SSU_AlarmHandler()");
	outstandingPID = -1;
	isShutdownTriggered = false;
	//Map which contains folder path and alarm string
	alarmString[SSU_FILEM_CPPRINTOUT] = "cp_printouts";
	alarmString[SSU_FILEM_CPFILES] = "cp_files";
	alarmString[SSU_FILEM_MMLCMDFILES] = "cp_mml";
	alarmString[SSU_FILEM_AUDITLOG] = "audit_logs";
	alarmString[SSU_FILEM_SCRFILES] = "sts_scr";
	alarmString[SSU_FILEM_DATATRANSFER] = "data_transfer";
	alarmString[SSU_FILEM_BUR] = "backup_restore";
	alarmString[SSU_FILEM_SWPKG] = "sw_package";
	alarmString[SSU_FILEM_HEALTHCHK] = "health_check";
	alarmString[SSU_FILEM_LICENSEFILE] = "license_file";
	alarmString[SSU_FILEM_CERTIFICATES] = "certificates";
	alarmString[SSU_FILEM_TOOLS] = "tools";
	alarmString[SSU_FILEM_SUPPORTDATA] = "support_data";
	alarmString[SSU_NONFILEM_FMS] = "cpfs";
        alarmString[SSU_NONFILEM_ACA] = "data_internal";
        alarmString[SSU_NONFILEM_CHB] = "data_internal";
        alarmString[SSU_NONFILEM_MTZ] = "data_internal";
        alarmString[SSU_NONFILEM_RTR] = "data_internal";
        alarmString[SSU_NONFILEM_SSU] = "data_internal";
        alarmString[SSU_NONFILEM_AES] = "data_internal";
        alarmString[SSU_NONFILEM_APZ] = "data_internal";
        alarmString[SSU_NONFILEM_CPS] = "data_internal";
        alarmString[SSU_NONFILEM_MCS] = "data_internal";
        alarmString[SSU_NONFILEM_OCS] = "data_internal";
        alarmString[SSU_NONFILEM_STS] = "data_internal";
	DEBUG("%s","Leaving ACS_SSU_AlarmHandler::ACS_SSU_AlarmHandler()");
}

ACS_SSU_AlarmHandler::~ACS_SSU_AlarmHandler()
{
	try
	{
		// Lock the alarm list to gain exclusive access
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_srctMutex);

		// If alarm list is not empty, the alarms must be ceased before shutdown
		if (!m_listAlarms.empty())
		{
			ACE_TCHAR szObjectOfReference[128];
			ACE_TCHAR szData[128];
			ACE_TCHAR szText[1024];

			INFO("%s","In ACS_SSU_AlarmHandler::~ACS_SSU_AlarmHandler()");

			while (!m_listAlarms.empty())
			{
				Alarm* pAlarm = *m_listAlarms.begin();
				LPSSU_PERF_DATA pAlarmData = pAlarm->PerfData();

				if (ACE_OS::strcmp(pAlarmData->Severity, ACE_TEXT("Event")) != 0)
				{
					INFO(" Ceasing SSU alarm. Event Id=%d. Severity:%s",pAlarmData->EventNumber, pAlarmData->Severity);

					// Format the event ObjectOfReference, Data and ProblemText
					FormatPerfText(pAlarmData,
							0.0,
							ACE_TEXT("CEASING"),
							szObjectOfReference,
							szData,
							szText);

					// Cease outstanding alarm
					(void)ACS_SSU_AEH::ReportEvent(pAlarmData->EventNumber,
								       ACE_TEXT("CEASING"),
								       CAUSE_SYSTEM_ANALYSIS,
								       szObjectOfReference,
								       szData,
								       szText);
				}

				(void)m_listAlarms.erase(m_listAlarms.begin());
				delete pAlarm;
			}
		}

		while (!m_listRunFirstTime.empty())
		{
			RunFirstTime* pRunFirstTime = *m_listRunFirstTime.begin();
			(void)m_listRunFirstTime.erase(m_listRunFirstTime.begin());
			delete pRunFirstTime;
		}
		//Erasing the alarm string map
		//		std::map<std::string,std::string>::iterator it = alarmString.begin();
		//	 	for(;it != alarmString.end();++it)
		//	 	{
		//	 		alarmString.erase(it);
		//	 	}
		alarmString.clear();

	}
	catch (...) { }
}

void ACS_SSU_AlarmHandler::CeaseLowerAlarms(LPSSU_PERF_DATA pPerfData)
{
	BOOL bAlarmCeased = false;
	std::list<Alarm*>::iterator iter = m_listAlarms.begin();

	while (iter != m_listAlarms.end())
	{
		LPSSU_PERF_DATA pAlarmData = ((Alarm*)(*iter))->PerfData();

		if (ACE_OS::strcmp(pAlarmData->Object, pPerfData->Object) == 0 &&
				ACE_OS::strcmp(pAlarmData->Counter, pPerfData->Counter) == 0 &&
				ACE_OS::strcmp(pAlarmData->Instance, pPerfData->Instance) == 0)
		{
			// Cease this alarm (if not severity is "Event")
			INFO(" Ceasing \"lower:%s\" alarm for \"higher:%s\" alarm",
					pAlarmData->Severity,pPerfData->Severity);

			SendPerfCease(pAlarmData, 0);
			bAlarmCeased = true;
			iter = m_listAlarms.begin();
		}
		else
			++iter;
	}

	if (bAlarmCeased)
		ACE_OS::sleep(1);
}

BOOL ACS_SSU_AlarmHandler::CheckIfSent(LPSSU_PERF_DATA pPerfData)
{
	//! Check if list is empty
	if (m_listAlarms.empty())
	{
		//	  DEBUG("%s","m_listAlarms size is zero");
		return false;
	}

	//! Check whether the alarm already hes been inserted in the alarm list
	BOOL bFound = false;
	std::list<Alarm*>::iterator iter = m_listAlarms.begin();
	while (!bFound && iter != m_listAlarms.end())
	{
		LPSSU_PERF_DATA pAlarmData = ((Alarm*)(*iter))->PerfData();

		if (ACE_OS::strcmp(pAlarmData->Object, pPerfData->Object) == 0 &&
				ACE_OS::strcmp(pAlarmData->Counter, pPerfData->Counter) == 0 &&
				ACE_OS::strcmp(pAlarmData->Instance, pPerfData->Instance) == 0 &&
				ACE_OS::strcmp(pAlarmData->Severity, pPerfData->Severity) == 0)
		{
			//! Found an exact match
			bFound = true;
		}
		else
			++iter;
	}

	return bFound;
}

BOOL ACS_SSU_AlarmHandler::CheckIfHigher(LPSSU_PERF_DATA pPerfData,
		LPSSU_PERF_DATA pAlarmData)
{
	int nPerfDataIdx = 0;
	int nAlarmDataIdx = 0;

	for (int nIdx = 0; nIdx < g_nNumOfSeverityClasses; nIdx++)
	{
		if (ACE_OS::strcmp(pPerfData->Severity, g_vSeverityClasses[nIdx]) == 0)
			nPerfDataIdx = nIdx+1;

		if (ACE_OS::strcmp(pAlarmData->Severity, g_vSeverityClasses[nIdx]) == 0)
			nAlarmDataIdx = nIdx+1;
	}

	return (nAlarmDataIdx > nPerfDataIdx);
}

/*!
 * @brief Check if there exists any alarm for performance counter pPerfData with
 *        higher severity
 * @param pPerfData
 * @return
 */
BOOL ACS_SSU_AlarmHandler::CheckIfHigherSent(LPSSU_PERF_DATA pPerfData)
{
	if (m_listAlarms.size() == 0)
		return false;

	BOOL bFound = false;
	std::list<Alarm*>::iterator iter = m_listAlarms.begin();
	while (!bFound && iter != m_listAlarms.end())
	{
		LPSSU_PERF_DATA pAlarmData = (*iter)->PerfData();
		if (ACE_OS::strcmp(pAlarmData->Object, pPerfData->Object) == 0 &&
				ACE_OS::strcmp(pAlarmData->Counter, pPerfData->Counter) == 0 &&
				ACE_OS::strcmp(pAlarmData->Instance, pPerfData->Instance) == 0)
		{
			// Checks whether the alarm in the alarm list has higher severity
			if (CheckIfHigher(pPerfData, pAlarmData))
				bFound = true;
			else
				++iter;
		}
		else
			++iter;
	}

	return bFound;
}

BOOL ACS_SSU_AlarmHandler::CheckIfRunApplication(LPSSU_PERF_DATA pPerfData)
{
	if(isShutdownTriggered)
	{
		DEBUG("%s","Shutdown triggered, CheckIfRunApplication is false");
		return false;
	}
	if (!(*pPerfData->Application))
	return false;

	// Check whether the application shall be executed every time
	if (!pPerfData->RunFirstTime)
	return true;

	if (m_listRunFirstTime.size() > 0)
	{
		// Check whether the application already has been executed on another
		// alarm occation
		RunFirstTime* pRunFirstTime = 0;
		BOOL bFound = false;

		std::list<RunFirstTime*>::iterator iter = m_listRunFirstTime.begin();
		while (!bFound && iter != m_listRunFirstTime.end())
		{
			pRunFirstTime = *iter;
			if (pRunFirstTime->Match(pPerfData))
				bFound = true;
			else
				++iter;
		}

		if (bFound)
		return false;
	}

	RunFirstTime* pRunFirstTime = new RunFirstTime(pPerfData);

	if (pRunFirstTime)
		m_listRunFirstTime.push_back(pRunFirstTime);

	return true;
}

/*! @brief Formats ObjectOfReference, ProblemData and ProblemText for a performance
 *  counter alarm/event
 * @param lpszPath Folder path
 * @param nLimit Folder quota limit
 * @param nValue Current quota value
 * @param lpszSeverity Alarm severity
 * @param lpszObjectOfReference Shows in which instance problem was detected
 * @param lpszData Free text description to be logged
 * @param lpszText Free text description to be printed
 */
void ACS_SSU_AlarmHandler::FormatFolderQuotaText(const ACE_TCHAR* lpszPath,
		const ACE_UINT32 nLimit,
		double nValue,
		const ACE_TCHAR* lpszSeverity,
		ACE_TCHAR* lpszObjectOfReference,
		ACE_TCHAR* lpszData,
		ACE_TCHAR* lpszText)
{
	//! Fill ObjectOfReference*/
	sprintf(lpszObjectOfReference,ACE_TEXT("Data Disk %s, %% Free Space"), alarmString.at(lpszPath).c_str());

	ACE_TCHAR szValue[24];
	ACE_TCHAR szLimit[24];

	/*! Check if the current value does contain decimals. We dont want no
	 *  values in the decimal part of the values, so we format the values
	 *  without decimals, otherwise with 2 decimals
	 */
	sprintf(szLimit, ACE_TEXT("%d"), nLimit);

	//!edanric
	if (nValue + 0.01 > nLimit)
		nValue = nValue - 0.01;
	//!end edanric

	if (((nValue+1) / (ACE_UINT32)(nValue+1)) == 1)
		sprintf(szValue, ACE_TEXT("%0.0f"), nValue);
	else
		sprintf(szValue, ACE_TEXT("%0.2f"), nValue);

	//! Data shall be empty
	*lpszData = ACE_TEXT('\0');

	//! Fill ProblemText
	if (nValue == 0 && ACE_OS::strcmp(lpszSeverity, ACE_TEXT("CEASING")) == 0)
		sprintf(lpszText,
				ACE_TEXT("\r\n%-12s %-20s %-24s %-12s %s\r\n%-12s %-20s %-24s %s%-11s %s"),
				ACE_TEXT("OBJECT"),
				ACE_TEXT("COUNTER"),
				ACE_TEXT("INSTANCE"),
				ACE_TEXT("LIMIT"),
				ACE_TEXT("VALUE"),
				ACE_TEXT("Data Disk"),
				ACE_TEXT("% Free Space"),
				alarmString.at(lpszPath).c_str(),
				ACE_TEXT("<"),
				szLimit,
				szValue);
	else
		sprintf(lpszText,
				ACE_TEXT("\r\n%-12s %-20s %-24s %-12s %s\r\n%-12s %-20s %-24s %s%-11s %s"),
				ACE_TEXT("OBJECT"),
				ACE_TEXT("COUNTER"),
				ACE_TEXT("INSTANCE"),
				ACE_TEXT("LIMIT"),
				ACE_TEXT("VALUE"),
				ACE_TEXT("Data Disk"),
				ACE_TEXT("% Free Space"),
				alarmString.at(lpszPath).c_str(),
				ACE_TEXT("<"),
				szLimit,
				szValue);
}

void ACS_SSU_AlarmHandler::FormatPerfText(const LPSSU_PERF_DATA pPerfData,
		const double nValue,
		const ACE_TCHAR* lpszSeverity,
		ACE_TCHAR* lpszObjectOfReference,
		ACE_TCHAR* lpszData,
		ACE_TCHAR* lpszText)
{
	//! Fill ObjectOfReference
	if (ACE_OS::strcmp(pPerfData->Object, ACE_TEXT("logicaldisk")) == 0)
		sprintf(lpszObjectOfReference, ACE_TEXT("%s %s, %s"), pPerfData->Object, pPerfData->Instance, pPerfData->Counter);
	else
		sprintf(lpszObjectOfReference, ACE_TEXT("%s, %s"), pPerfData->Object, pPerfData->Counter);

	ACE_TCHAR szValue[24];
	ACE_TCHAR szLimit[24];

	/*! Check if the counter value and the limit value does contain decimals
	 *  No values in the decimal part of the values, so we format the values
	 *  without decimals, otherwise with 2 decimals
	 */

	if (((nValue+1) / (long)(nValue+1)) == 1)
		sprintf(szValue, ACE_TEXT("%0.0f"), nValue);
	else
		sprintf(szValue, ACE_TEXT("%0.2f"), nValue);

	if (((pPerfData->Value+1) / (long)(pPerfData->Value+1)) == 1)
		//      sprintf(szLimit, ACE_TEXT("%0.0f"), pPerfData->Value);
		sprintf(szLimit, ACE_TEXT("%0.0f"), (double)pPerfData->Value);
	else
		sprintf(szLimit, ACE_TEXT	("%0.2f"), (double)pPerfData->Value);


	//! Data shall be empty
	*lpszData = ACE_TEXT('\0');
	string tempInstance(pPerfData->Instance);
	if(strcmp(pPerfData->Object,ACE_TEXT("System Disk"))== 0)
	{
		DEBUG("Object  %s",pPerfData->Object);

		tempInstance = "-";
	}
	//! Fill ProblemText
	if (nValue == 0 && ACE_OS::strcmp(lpszSeverity, ACE_TEXT("CEASING")) == 0)
	{
		sprintf(lpszText,
				ACE_TEXT("\n%-12s %-20s %-15s %-12s %s\n%-12s %-20s %-15s %s%-11s"),
				ACE_TEXT("OBJECT"),
				ACE_TEXT("COUNTER"),
				ACE_TEXT("INSTANCE"),
				ACE_TEXT("LIMIT"),
				ACE_TEXT("VALUE"),
				pPerfData->Object,
				pPerfData->Counter,
				tempInstance.c_str(),
				(pPerfData->AlertIfOver ? ACE_TEXT(">") : ACE_TEXT("<")),
				szLimit);
	}
	else
	{
		sprintf(lpszText,
				ACE_TEXT("\n%-12s %-20s %-15s %-12s %s\n%-12s %-20s %-15s %s%-11s %s"),
				ACE_TEXT("OBJECT"),
				ACE_TEXT("COUNTER"),
				ACE_TEXT("INSTANCE"),
				ACE_TEXT("LIMIT"),
				ACE_TEXT("VALUE"),
				pPerfData->Object,
				pPerfData->Counter,
				tempInstance.c_str(),
				(pPerfData->AlertIfOver ? ACE_TEXT(">") : ACE_TEXT("<")),
				szLimit,
				szValue);
	}
}

bool ACS_SSU_AlarmHandler::SendPerfAlarm(const LPSSU_PERF_DATA pPerfData,
		const double nValue)
{
	//! Lock the alarm list to gain exclusive access
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_srctMutex);
	BOOL bSendAlarm = false;

	//! Check first if this alarm already has been raised
	if (!CheckIfSent(pPerfData))
	{
		//! Check if an alarm with a higher alarm severity has been raised
		if (!CheckIfHigherSent(pPerfData))
		{
			/*!
			 * Check if an alarm already has been raised with a lower severity
			 * in that case that alarm shall be ceased before we raise a new
			 */
			CeaseLowerAlarms(pPerfData);

			bSendAlarm = true;

			if (int(pPerfData->Value) == 80){
				//A2 alarm has been raised on APG
				INFO("A2 memory alarm has been received and executing the script : %d", int(nValue));
				ExecuteRsyslogCheck();
			}

		}
	}

	if (bSendAlarm)
	{
		ACE_TCHAR szObjectOfReference[128];
		ACE_TCHAR szData[512];
		ACE_TCHAR szText[1024];
		INFO(" SendPerfAlarm : %s %s", pPerfData->Severity,pPerfData->Instance);

		//! Format the event ObjectOfReference, Data and ProblemText
		FormatPerfText(pPerfData,
				nValue,
				pPerfData->Severity,
				szObjectOfReference,
				szData,
				szText);

		DEBUG("ACS_SSU_AlarmHandler::SendPerfAlarm() - %s  %s  ",
				pPerfData->Severity,szText);

		//! Send alarm
		(void)ACS_SSU_AEH::ReportEvent(pPerfData->EventNumber,
					       pPerfData->Severity,
					       CAUSE_SYSTEM_ANALYSIS,
					       szObjectOfReference,
					       szData,
					       szText);


		if (CheckIfRunApplication(pPerfData))
		{
			DEBUG("%s","ACS_SSU_AlarmHandler::CheckIfRunApplication - YES");

			ACE_INT32 s32ExitStatus = -1;
			//! Run program in separate process
			//! Execute application

			int infp, outfp;	
			int status = -1;
			if ((outstandingPID = popen_with_pid(pPerfData->Application, &infp, &outfp)) <= 0)
			{
				DEBUG("Error in executing the %s",pPerfData->Application);
			}
			else 
			{ 

				DEBUG("PID of application process %s is %d",pPerfData->Application,outstandingPID);
				waitpid(outstandingPID,&status,0);

				close(infp);	//HX58640
				close(outfp);	//HX58640

				if (WIFEXITED(status))
				{
					s32ExitStatus = WEXITSTATUS(status);
					outstandingPID = -1;
					DEBUG("%s execution status = %d",pPerfData->Application,s32ExitStatus);
				}
			}

			if( s32ExitStatus == -1 )
			{
				//! Failed to run command
				sprintf(szData,
						ACE_TEXT(" \n Failed to run command \"%s\" for the SSU Performance Counter: \"%s\" \"%s\"  due to the following error code:%d"),
						pPerfData->Application,
						pPerfData->Object,
						pPerfData->Counter,
					//	pPerfData->Instance,
						s32ExitStatus);

				ERROR("%s",szData);

				//! Report error
				(void)ACS_SSU_AEH::ReportEvent(9994,
							       ACE_TEXT("EVENT"),
							       CAUSE_SYSTEM_ANALYSIS,
							       PROCESS_NAME,
							       szData,
							       ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

				INFO(" Application %s exited with status: %d",pPerfData->Application,s32ExitStatus);

			}

		}

		//! Create a new Alarm object and append it last in the alarm list
		Alarm* pAlarm = new Alarm(pPerfData, nValue);
		if (pAlarm)
		{
			if ( ACE_OS::strcmp(pPerfData->Severity, ACE_TEXT("Event")) == 0 )
			{
				DEBUG("%s","Adding a new Event");
			}
			else
			{
				DEBUG("%s","Adding a new Alarm");
			}
			m_listAlarms.push_back(pAlarm);
		}
	}
	return bSendAlarm;
}

bool ACS_SSU_AlarmHandler::SendPerfCease(const LPSSU_PERF_DATA pPerfData,
		double nValue)
{
	//! Lock the alarm list to gain exclusive access
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_srctMutex);

	bool bAlarmCeaseSent = false;

	//! Check if alarm has been raised to be ceased
	if (CheckIfSent(pPerfData))
	{
		//! Check if alarm was not an event (we can't cease an event)
		if (ACE_OS::strcmp(pPerfData->Severity, ACE_TEXT("Event")) != 0)
		{
			INFO("Ceasing SSU alarm. Event Id=%d", pPerfData->EventNumber);
			INFO("SendPerfCease : %s %s", pPerfData->Severity,pPerfData->Instance);

			ACE_TCHAR szObjectOfReference[128];
			ACE_TCHAR szData[128];
			ACE_TCHAR szText[1024];

			//! Format the event ObjectOfReference, Data and ProblemText
			FormatPerfText(pPerfData,
					nValue,
					ACE_TEXT("CEASING"),
					szObjectOfReference,
					szData,
					szText);

			DEBUG("ACS_SSU_AlarmHandler::SendPerfCease() -  %s %s ",
					pPerfData->Severity,szText);

			//! Cease outstanding alarm
			bAlarmCeaseSent = ACS_SSU_AEH::ReportEvent(pPerfData->EventNumber,
					ACE_TEXT("CEASING"),
					CAUSE_SYSTEM_ANALYSIS,
					szObjectOfReference,
					szData,
					szText);
		}
	}

	//! Look up the insertion point of the Alarm object in the alarm list
	BOOL bFound = false;

	std::list<Alarm*>::iterator iter = m_listAlarms.begin();
	while (!bFound && iter != m_listAlarms.end())
	{
		LPSSU_PERF_DATA pAlarmData = (*iter)->PerfData();
		if ( ACE_OS::strcmp(pAlarmData->Object, pPerfData->Object) == 0 &&
				ACE_OS::strcmp(pAlarmData->Counter, pPerfData->Counter) == 0 &&
				ACE_OS::strcmp(pAlarmData->Instance, pPerfData->Instance) == 0 &&
				ACE_OS::strcmp(pAlarmData->Severity, pPerfData->Severity) == 0)
		{
			bFound = true;
		}
		else
			++iter;
	}

	//! We found the insertion point, so we remove the Alarm object
	if (bFound && *iter)
	{
		INFO("%s","Cease alarm.");

		Alarm* pAlarm = *iter;
		(void)m_listAlarms.erase(iter);
		delete pAlarm;
	}

	return bAlarmCeaseSent;
}

/*!
 * @brief Sends an alarm/event regarding folder quotas threshold is reached
 * @param nEventNumber
 * @param lpszSeverity
 * @param lpszPath
 * @param nLimit
 * @param nValue
 */
bool ACS_SSU_AlarmHandler::SendFolderQuotaAlarm(const long nEventNumber,
		const ACE_TCHAR* lpszSeverity,
		const ACE_TCHAR* lpszPath,
		const ACE_UINT32 nLimit,
		const double nValue)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_srctMutex);
	bool bAlarmSent = false;

	INFO("Sending SSU Folder Quota alarm. Event Id=%d",nEventNumber);

	ACE_TCHAR szObjectOfReference[512] ;
	ACE_TCHAR szData[128];
	ACE_TCHAR szText[1024];

	//! Format the event ObjectOfReference, Data and ProblemText
	FormatFolderQuotaText(lpszPath,
			nLimit,
			nValue,
			lpszSeverity,
			szObjectOfReference,
			szData,
			szText);
	//! Send alarm
	bAlarmSent = ACS_SSU_AEH::ReportEvent(nEventNumber,
			lpszSeverity,
			CAUSE_SYSTEM_ANALYSIS,
			szObjectOfReference,
			szData,
			szText);

	return bAlarmSent;
}

/*!
 * @brief Sends an alarm/event for Folder quota alarm cease
 * @param nEventNumber Event number
 * @param lpszPath Folder path
 * @param nLimit Quota limit
 * @param nValue Current Quota value
 */
bool ACS_SSU_AlarmHandler::SendFolderQuotaCease(const ACE_UINT32 nEventNumber,
		const ACE_TCHAR* lpszPath,
		const ACE_UINT32 nLimit,
		const double nValue)
{

	//! Lock the alarm list to gain exclusive access
	//   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(this->m_srctMutex);

	bool bAlarmCeaseSent = false;

	INFO("Sending SSU Folder Quota alarm cease. Event Id=%d",nEventNumber);

	ACE_TCHAR szObjectOfReference[512];
	ACE_TCHAR szData[128];
	ACE_TCHAR szText[1024];

	//! Format the event ObjectOfReference, Data and ProblemText
	FormatFolderQuotaText(lpszPath,
			nLimit,
			nValue,
			ACE_TEXT("CEASING"),
			szObjectOfReference,
			szData,
			szText);

	//! Sent alarm
	bAlarmCeaseSent = ACS_SSU_AEH::ReportEvent(nEventNumber,
			ACE_TEXT("CEASING"),
			CAUSE_SYSTEM_ANALYSIS,
			szObjectOfReference,
			szData,
			szText);

	return bAlarmCeaseSent;
}

void ACS_SSU_AlarmHandler::ExecuteRsyslogCheck()  {

	if(ACS_SSU_Common::CheckFileExists(ACS_SSU_RSYSLOG_MEMORY_CHECK_CMD_SCRIPT)){

		int status;
		string cmd = ACS_SSU_RSYSLOG_MEMORY_CHECK_CMD_SCRIPT + string(" &>/dev/null");
		pid_t pid = fork();
		if (pid == 0) {
			if(execlp("sh","sh", "-c", cmd.c_str(), (char *) NULL) == -1){
				INFO("Error: execlp: RSYSLOG SCRIPT",errno);
			}
		}
		else if (pid < 0)
			INFO("Fatal error fork() failed. for ssu_memory_recovery.sh %d ",errno);

		waitpid(pid, &status, 0);
	}

}


/*! @} */
