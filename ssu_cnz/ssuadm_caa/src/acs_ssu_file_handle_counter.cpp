/*=================================================================== */
/**
   @file acs_ssu_file_handle_counter.cpp

   This module contains the implementation for File Handle Counter thread is spawned by
   Performance Monitor Thread and is responsible for monitoring File Handle Count.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Log_Msg.h>
//! Common Class include for Getting Active Node
#include <ACS_APGCC_Util.H>
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_file_handle_counter.h"
#include "acs_ssu_alarm_handler.h"

//! Constructor
ACS_SSU_FileHandle_Counter::ACS_SSU_FileHandle_Counter( ACE_Event * hEndEvent,
		ACS_SSU_AlarmHandler* pAlarmHandler): ACS_SSU_Counter(hEndEvent,pAlarmHandler)
{
	DEBUG("%s","Entering ACS_SSU_FileHandle_Counter::ACS_SSU_FileHandle_Counter()");
        for(int i=0;i< ACS_SSU_FILE_NR_MAX_FIELDS;i++)
         m_lsu64FileHandleCount[i]=0;
	DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::ACS_SSU_FileHandle_Counter()");
}

ACS_SSU_FileHandle_Counter::~ACS_SSU_FileHandle_Counter()
{
	// Cease any outstanding alarm before leaving
	for( std::list<SSU_PERF_DATA>::const_iterator it = m_listPerfMonMonitor.begin(); it != m_listPerfMonMonitor.end() ; ++it )
	{
		m_pAlarmHandler->SendPerfCease( (LPSSU_PERF_DATA)&*it, 0.0);
	}
}

//! Returns a reference to the SSU_PERF_DATA struct
const std::list<SSU_PERF_DATA>& ACS_SSU_FileHandle_Counter::PerfData() const
{
	return m_listPerfMonMonitor;
}


BOOL ACS_SSU_FileHandle_Counter::bCheckAndRaiseAlarm()
{
	DEBUG("%s","Entering ACS_SSU_FileHandle_Counter::bCheckAndRaiseAlarm()");
	BOOL bResult=FALSE;

	DEBUG("%s","Entering ACS_SSU_FileHandle_Counter::bCheckAndRaiseAlarm()");
	for( std::list<SSU_PERF_DATA>::iterator it = m_listPerfMonMonitor.begin(); it != m_listPerfMonMonitor.end() ; ++it )
	{
		if ((it->AlertIfOver && m_lsu64FileHandleCount[FILE_NR_ALLOCATED] > it->Value ) ||
				(!it->AlertIfOver && m_lsu64FileHandleCount[FILE_NR_ALLOCATED] < it->Value))
		{
			// Check whether an alarm shall be issued on this node
			if (!(it->Node) ||
					ACE_OS::strcasecmp(it->Node, ACE_TEXT("both")) == 0 ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("active")) == 0 && ACS_APGCC::is_active_node() ) ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("passive")) == 0 && !ACS_APGCC::is_active_node()))
			{
				INFO("Current FileHandle count %d A2 Alarm Limit %d",m_lsu64FileHandleCount[FILE_NR_ALLOCATED],it->Value);
				bResult = m_pAlarmHandler->SendPerfAlarm( &*it, m_lsu64FileHandleCount[FILE_NR_ALLOCATED]);
			}
		}
		else if ((it->AlertIfOver && m_lsu64FileHandleCount[FILE_NR_ALLOCATED] < it->Value ) ||
				(!it->AlertIfOver && m_lsu64FileHandleCount[FILE_NR_ALLOCATED] > it->Value))
		{
			bResult = m_pAlarmHandler->SendPerfCease(&*it,m_lsu64FileHandleCount[FILE_NR_ALLOCATED]);
		}
	}

	DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::bCheckAndRaiseAlarm()");
	return bResult;
}

bool ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()
{
	DEBUG("%s","Entering ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()");
	ACE_UINT32 u8Index = 0;
	ACE_TCHAR szData[512];
	FILE *fp;
	char path[1000];
	fp = popen(ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED, "r");
	if (fp == NULL)
	{
		//! Failed to run command
		sprintf(szData,
				ACE_TEXT("\n Failed to run command \"%s\" \n"),
				ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED);

		ERROR("%s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

		DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()");
		return false;
	}
	while (fgets(path, 1000, fp) != NULL)
	{
		m_lsu64FileHandleCount[u8Index++] = ACE_OS::atoi(path);
	}
	pclose(fp);

	fp = popen(ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED_BUT_UNUSED, "r");
	if (fp == NULL)
	{
		//! Failed to run command
		sprintf(szData,
				ACE_TEXT("\n Failed to run command \"%s\" \n"),
				ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_ALLOCATED_BUT_UNUSED);

		ERROR("%s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

		DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()");
		return false;
	}
	while (fgets(path, 1000, fp) != NULL)
	{
		m_lsu64FileHandleCount[u8Index++] = ACE_OS::atoi(path);
	}
	pclose(fp);


	fp = popen(ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_TOTAL, "r");
	if (fp == NULL)
	{
		//! Failed to run command
		sprintf(szData,
				ACE_TEXT("\n Failed to run command \"%s\" \n"),
				ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD_TOTAL);

		ERROR("%s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

		DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()");
		return false;
	}
	while (fgets(path, 1000, fp) != NULL)
	{
		m_lsu64FileHandleCount[u8Index++] = ACE_OS::atoi(path);
	}
	pclose(fp);
	DEBUG("%s","Exiting ACS_SSU_FileHandle_Counter::bUpdatePerformanceCounters()");
	return true;

	/*ACE_TCHAR szData[512];
	ACE_INT32 s32ExitStatus = ACE_OS::system(ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD);
	bool bResult = FALSE;

	if( s32ExitStatus ==-1 )
	{
       //! Failed to run command
	   sprintf(szData,
		       ACE_TEXT("\n Failed to run command \"%s\" for the SSU FileHandle Counter due to the following error code:\n%d"),
			   ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD,
			   s32ExitStatus);

	   ERROR("%s",szData);

	   //! Report error
	   (void)ACS_SSU_AEH::ReportEvent(1003,
		                      ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				              PROCESS_NAME,
				              szData,
				              ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

		INFO("Application %s exited with status: %d",ACS_SSU_PERF_FILE_HANDLE_COUNT_CMD,s32ExitStatus);
	}
	else
	{
	   //! Invoke Parser
		bResult = bParseCommandOutPut();
   }
   return bResult;*/
}

//! Thread Monitor function
ACE_THR_FUNC_RETURN ACS_SSU_FileHandle_Counter::MonitorThread(void * pvThis)
{
	((ACS_SSU_FileHandle_Counter*)pvThis)->Monitor();
	return 0;
}

BOOL ACS_SSU_FileHandle_Counter::bParseCommandOutPut()
{

	//! Open command output file for processing
	ifstream *fPerfMon = new ifstream(ACS_SSU_PERF_FILE_NR_FILE,ios::in);

	//!Line to be processed is stored in this buffer
	ACE_TCHAR pszCharBuff[200] = {0};
	std::string pszReadLine ;
	//! Individual tokens are copied in this buffer
	ACE_TCHAR* pszToken;
	ACE_TCHAR szData[512];
	bool bResult = FALSE;


	if(!fPerfMon)
	{
		//! Failure in creation of file
		sprintf(szData,
				ACE_TEXT("\n Failed to create file input stream \"%s\" for the SSU FileHandleCounter data file"),
				ACS_SSU_PERF_FILE_NR_FILE);

		ERROR("%s",szData);

		//Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));
	}
	//! Read Lines from command output and process it accordingly
	else if( std::getline( *fPerfMon, pszReadLine))
	{
		ACE_INT8 u8Index = 0;
		bResult = true;

		ACE_OS::strcpy( pszCharBuff , pszReadLine.c_str());
		pszToken = ACE_OS::strtok( pszCharBuff, " ");
		m_lsu64FileHandleCount[u8Index++] = ACE_OS::strtoll(pszToken,0,0);

		while( ( pszToken = strtok(0," ")) != 0 )
		{
			m_lsu64FileHandleCount[u8Index++] = ACE_OS::strtoll(pszToken,0,0);
		}

		delete fPerfMon;
	}

	DEBUG(" File Counter:bParseCommandOutPut() HandleCount : %d ",m_lsu64FileHandleCount[FILE_NR_ALLOCATED]);

	return bResult;
}

