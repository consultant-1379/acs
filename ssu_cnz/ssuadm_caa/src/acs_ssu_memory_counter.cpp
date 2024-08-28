/*! \addtogroup perfm "Performance Monitoring"
*
*  Memory Counter thread is spawned by Performance Monitor Thread and is responsible
*  for monitoring Memory and Swap
*
*  @{
*/

//=============================================================================
/**
*  @file    ACS_SSU_Memory_Counter.cpp
*
*
*  @version  1.0.0
*
*  @author 2010-08-16 by XSIRKUM
*
*  @documentno
*
*/

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */

#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Log_Msg.h>
#include <boost/lexical_cast.hpp>
//! Common Class include for Getting Active Node
#include <ACS_APGCC_Util.H>
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_memory_counter.h"
#include "acs_ssu_alarm_handler.h"

ACS_SSU_Memory_Counter::ACS_SSU_Memory_Counter( ACE_Event * hEndEvent,
		ACS_SSU_AlarmHandler* pAlarmHandler)
:ACS_SSU_Counter(hEndEvent,pAlarmHandler)
{
}

ACS_SSU_Memory_Counter::~ACS_SSU_Memory_Counter()
{
	//! Cease any outstanding alarm before leaving
	for( std::list<SSU_PERF_DATA>::const_iterator it = m_listPerfMonMonitor.begin(); it != m_listPerfMonMonitor.end() ; ++it )
	{
		//! Compiler throws error if it is passed directly , deference and send address
		m_pAlarmHandler->SendPerfCease( (LPSSU_PERF_DATA)&*it, 0.0);
	}
}

BOOL ACS_SSU_Memory_Counter::bCheckAndRaiseAlarm()
{
	BOOL bResult=FALSE;
	//! Check parsed values and raise alarm accordingly
	for( std::list<SSU_PERF_DATA>::iterator it = m_listPerfMonMonitor.begin(); it != m_listPerfMonMonitor.end() ; ++it )
	{
		double dPercentage = (double) ( (long double)m_usedMem / (long double)m_totalMem);
		DEBUG("percentage is: %lf", dPercentage);

		dPercentage = dPercentage * (double) 100;
		//! Check if threshold limit has been reached
		if ((it->AlertIfOver && dPercentage > it->Value ) ||
				(!it->AlertIfOver && dPercentage < it->Value))
		{
			//! Check whether an alarm shall be issued on this node
			if (!(it->Node) ||
					ACE_OS::strcasecmp(it->Node, ACE_TEXT("both")) == 0 ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("active")) == 0 && ACS_APGCC::is_active_node() ) ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("passive")) == 0 && !ACS_APGCC::is_active_node()))
			{
				//INFO(" MemCounter:vCheckAndRaiseAlarm %6s for %4s : Total : %10ld  Used : %10ld  Percentage: %10f", it->Instance ,it->Severity,u64TotalSize ,u64UsedSize, dPercentage);
				INFO("A1/A2 memory alarm has been sent for ALARM handler on APG with memory % : %lf", dPercentage);
				bResult=m_pAlarmHandler->SendPerfAlarm( &*it, dPercentage);
			}
		}
		else if ((it->AlertIfOver && dPercentage < it->Value ) ||
				(!it->AlertIfOver && dPercentage > it->Value))
		{
			//INFO(" MemCounter:vCheckAndRaiseAlarm %6s for %4s : Total : %10ld  Used : %10ld  Percentage: %10f", it->Instance ,it->Severity,u64TotalSize ,u64UsedSize, dPercentage);
			bResult=m_pAlarmHandler->SendPerfCease(&*it,dPercentage);
		}
	}
	return bResult;
}

bool ACS_SSU_Memory_Counter::bUpdatePerformanceCounters()
{
	//! Run free command
	BOOL bResult=FALSE;
	ACE_TCHAR szData[512];
	ACE_INT32 s32ExitStatus = ACE_OS::system(ACS_SSU_PERF_MEM_CMD);
	//ACE_OS::sleep(1);
	if( s32ExitStatus == -1 )
	{
		//! Failed to run command
		sprintf(szData,
				ACE_TEXT("\n Failed to run command \"%s\" for the SSU Memory Counter due to the following error code:\n%d"),
				ACS_SSU_PERF_MEM_CMD,
				s32ExitStatus);

		ERROR("%s",szData);
		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

		INFO("Application %s exited with status: %d",ACS_SSU_PERF_MEM_CMD,s32ExitStatus);


	}
	else
	{

		//! Invoke Parser
		bResult = bParseCommandOutPut();
	}

	return bResult;
}

//! Thread register Monitor function
ACE_THR_FUNC_RETURN ACS_SSU_Memory_Counter::MonitorThread(void * pvThis)
{
	((ACS_SSU_Memory_Counter*)pvThis)->Monitor();
	return 0;
}

BOOL ACS_SSU_Memory_Counter::bParseCommandOutPut()
{
	BOOL bResult = false;
	m_usedMem = 0;
	m_totalMem = 0;

	ifstream *fPerfMon = new ifstream(ACS_SSU_PERF_MEM_FILE,ios::in);


	if(!fPerfMon)
	{
		//! Failure in creation of input stream
		char szData[200];
		sprintf(szData,
				ACE_TEXT("\n Failed to create file input stream \"%s\" for the SSU MemoryCounter data file"),
				ACS_SSU_PERF_FILE_NR_FILE );

		ERROR("%s",szData);

		//! Report error
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),CAUSE_SYSTEM_ANALYSIS,
				PROCESS_NAME,
				szData,
				ACE_TEXT("FAULT IN SSU SYSTEM SUPERVISOR"));

	} 
	else
	{
		bResult = true;

		std::string tempStr,outputStr;
		ACE_UINT64 freeMem = 0;
		while (std::getline( *fPerfMon, outputStr))
		{
			const char *tmpStr = outputStr.c_str();

			if ( strstr(tmpStr, "MemTotal:"))
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				m_totalMem = m_totalMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());
				continue;
			}


			if( strstr(tmpStr,"MemFree:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;

				freeMem = freeMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());
				continue;

			}

			if( strstr(tmpStr,"Buffers:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;

				freeMem = freeMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());
				continue;

			}

			if( strstr(tmpStr,"Cached:") )
			{

				if(strstr (tmpStr, "SwapCached:"))
					continue;
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				freeMem = freeMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());
				continue;

			}

			if( strstr(tmpStr,"SwapTotal:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				m_totalMem = m_totalMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());

				continue;

			}
			if( strstr(tmpStr,"SwapFree:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				freeMem = freeMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());

				continue;

			}
			if( strstr(tmpStr,"Shmem:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				freeMem = freeMem - boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());

				continue;

			}
			if( strstr(tmpStr,"SReclaimable:") )
			{
				tempStr = outputStr.substr(outputStr.find_first_of(":")+1);
				tempStr.erase(0,tempStr.find_first_not_of(" ")) ;
				freeMem = freeMem + boost::lexical_cast<ACE_UINT64>(tempStr.substr(0,tempStr.find_first_of(" ")).c_str());

				break;

			}

		}

		m_usedMem = m_totalMem - freeMem;
		DEBUG("total mem is = %ld",m_totalMem);	
		DEBUG("total used is = %ld",m_usedMem);
		delete fPerfMon;
		fPerfMon = 0;
	}
	return bResult;
}
/*! @} */
