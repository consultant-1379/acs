/*=================================================================== */
/**
   @file acs_ssu_perf_mon.cpp

   This module contains the implementation for Memory Counter thread is spawned by
   Performance Monitor Thread and is responsible for monitoring Memory and Swap

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
#include <ace/OS_NS_strings.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>

#include "acs_ssu_common.h"
#include "acs_ssu_pha.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_perf_mon.h"
#include "acs_ssu_memory_counter.h"
#include "acs_ssu_file_handle_counter.h"

using namespace std;

SSU_PERF_DATA srctPerfData[3];

ACS_SSU_PerfMon::ACS_SSU_PerfMon(ACE_Event* poEvent, ACS_SSU_AlarmHandler* pAlarmHandler): m_pAlarmHandler( pAlarmHandler ),
		m_phEndEvent( poEvent ),
		m_s32GrpId(-1)
{
	DEBUG("%s","Entering ACS_SSU_PerfMon::ACS_SSU_PerfMon()");
	DEBUG("%s","Exiting ACS_SSU_PerfMon::ACS_SSU_PerfMon()");
}

ACS_SSU_PerfMon::~ACS_SSU_PerfMon()
{
	try
	{
		while (!m_mapPerfCounters.empty())
		{
			ACS_SSU_Counter* pCounter = ( m_mapPerfCounters.begin()->second );
			(void)m_mapPerfCounters.erase(m_mapPerfCounters.begin());
			delete pCounter;
			pCounter = NULL;
		}
	}
	catch (...) { }
}

void ACS_SSU_PerfMon::Execute()
{
	DEBUG("%s","Entering acs_ssu_PerfMon::Execute()");

	//! Fetch the performance counters from the registry
	if (!GetPerfCounterSettings())
	{
		DEBUG("%s","acs_ssu_PerfMon::Execute Failed to fetch Perf Par ");
		DEBUG("%s","Exiting acs_ssu_PerfMon::Execute()");
		return;
	}

	ACE_UINT32 dwState = m_phEndEvent->wait();

	if ( dwState != 0 )
	{
		//! Nothing to do
	}
	else
	{
		if( m_s32GrpId != -1 )
		{

			ACE_Thread_Manager::instance()->cancel_grp(m_s32GrpId);
			ACE_Thread_Manager::instance()->wait_grp( m_s32GrpId );
		}
	}
	DEBUG("%s","Exiting acs_ssu_PerfMon::Execute()");
}


//! Reads the performance counters from PHA into a vector and create corresponding counter objects
BOOL ACS_SSU_PerfMon::GetPerfCounterSettings()
{
	DEBUG("%s","Entering acs_ssu_PerfMon::GetPerfCounterSettings()");
	// Read all performance counters in the PHA database

	ACS_SSU_Counter* pCounter;
	ACE_UINT32 dwResult = 0;

	// Initializing srctPerfData structures
	ACE_OS::memset(&srctPerfData[0],0,sizeof(SSU_PERF_DATA));
	ACE_OS::memset(&srctPerfData[1],0,sizeof(SSU_PERF_DATA));
	ACE_OS::memset(&srctPerfData[2],0,sizeof(SSU_PERF_DATA));


	// Initialize Structure1 - MEM A1
	ACE_OS::strcpy(srctPerfData[0].Application,ACE_TEXT(SSU_APPLICATION_TO_RUN));
	ACE_OS::strcpy(srctPerfData[0].Counter,SSU_PERF_MON_MEM_COUNTER);
	ACE_OS::strcpy(srctPerfData[0].Instance,SSU_PERF_MON_MEM_INSTANCE_VALUE);
	ACE_OS::strcpy(srctPerfData[0].Object,SSU_PERF_MON_MEM_OBJECT_NAME);
	srctPerfData[0].EventNumber = SSU_PERF_MON_MEM_A1_EVENT_NUM;
	ACE_OS::strcpy(srctPerfData[0].Severity,SSU_A1_SEVERITY);
	srctPerfData[0].AlertIfOver = SSU_ENABLE_ALERT_IF_OVER;


	// Initialize Structure2 - MEM A2
	ACE_OS::strcpy(srctPerfData[1].Application,ACE_TEXT(SSU_APPLICATION_TO_RUN));
	ACE_OS::strcpy(srctPerfData[1].Counter,SSU_PERF_MON_MEM_COUNTER);
	ACE_OS::strcpy(srctPerfData[1].Instance,SSU_PERF_MON_MEM_INSTANCE_VALUE);
	ACE_OS::strcpy(srctPerfData[1].Object,SSU_PERF_MON_MEM_OBJECT_NAME);
	srctPerfData[1].EventNumber = SSU_PERF_MON_MEM_A2_EVENT_NUM;
	ACE_OS::strcpy(srctPerfData[1].Severity,SSU_A2_SEVERITY);
	srctPerfData[1].AlertIfOver = SSU_ENABLE_ALERT_IF_OVER;


	if(!readAllPHAValuesForMemory())
	{
		DEBUG("%s","Exiting acs_ssu_PerfMon::GetPerfCounterSettings()");
		return false;
	}


	// Initialize Structure5 - FILE HANDLE A2
	ACE_OS::strcpy(srctPerfData[2].Application,ACE_TEXT(SSU_APPLICATION_TO_RUN));
	ACE_OS::strcpy(srctPerfData[2].Counter,SSU_PERF_MON_FILEHANDLE_COUNTER);
	ACE_OS::strcpy(srctPerfData[2].Instance,SSU_PERF_MON_FILEHANDLE_INSTANCE_VALUE);
	ACE_OS::strcpy(srctPerfData[2].Object,SSU_PERF_MON_FILEHANDLE_OBJECT_NAME);
	srctPerfData[2].EventNumber = SSU_PERF_MON_FILEHANDLE_EVENT_NUM;
	ACE_OS::strcpy(srctPerfData[2].Severity,SSU_A2_SEVERITY);
	srctPerfData[2].AlertIfOver = SSU_ENABLE_ALERT_IF_OVER;


	if(!readAllPHAValuesForFileHandle())
	{
		DEBUG("%s","Exiting acs_ssu_PerfMon::GetPerfCounterSettings()");
		return false;
	}


	for(int i=0;i<3;i++)
	{
		//! Validate the severity class value
		if (!ACS_SSU_Common::bCheckValidSeverityClass(srctPerfData[i].Severity))
		{
			ACE_TCHAR szText[128];
			ACE_OS::snprintf(szText,
					(sizeof(szText)/sizeof(*szText))-1,
					ACE_TEXT("Invalid severity class (%s) in IMM parameter \"%s\""),
					srctPerfData[i].Severity,
					SSU_PHA_PERFMONDATAITEM);

			//! PHA error. send event
			(void)ACS_SSU_AEH::ReportEvent(1003,
					ACE_TEXT("EVENT"),
					CAUSE_AP_INTERNAL_FAULT,
					ACE_TEXT(""),
					szText,
					SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

			dwResult = 1;

		}
		else
		{
			DEBUG(" Alarm Settings for %-15s %-4s Raise:%-8d Cease:%-8d ", srctPerfData[i].Instance,srctPerfData[i].Severity,
					(ACE_UINT32)srctPerfData[i].Value,(ACE_UINT32)srctPerfData[i].CeaseValue);

			//! Add alarm to the counter if it exists or create new counter object
			if( m_mapPerfCounters.find( srctPerfData[i].Object ) != m_mapPerfCounters.end() )
			{
				m_mapPerfCounters[srctPerfData[i].Object]->bAddAlarm(srctPerfData[i]);
			}
			else
			{
				DEBUG(" ACS_SSU_PerfMon::Execute Starting %s Counter Thread",srctPerfData[i].Object);

				pCounter = poGetPerformanceMonitorObject(srctPerfData[i].Object);

				if (!pCounter)
				{
					//! PHA error. send event
					(void)ACS_SSU_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							ACE_TEXT("Failed to allocate heap memory."),
							SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

					dwResult = 2;

				}
				else
				{
					pCounter->bAddAlarm(srctPerfData[i]);
					//! Add performance counter object
					m_mapPerfCounters[srctPerfData[i].Object]  = pCounter;

					//! Start performance counter monitor thread
                    ACE_INT32 s8Result = ACS_SSU_Common::StartThreadForPerf(pCounter->MonitorThread, (void*)pCounter, ACE_TEXT("PerfMonThread"),m_s32GrpId);

					if ( s8Result == -1 )
					{
						// Report error
						ACE_TCHAR szData[256];
						ACE_OS::snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
								ACE_TEXT("Unable to create thread: Counter Thread due to the following error: \n%d"),
								errno);

						ERROR("\n%s",szData);

						//! Send event
						(void)ACS_SSU_AEH::ReportEvent(1002,
								ACE_TEXT("EVENT"),
								CAUSE_AP_INTERNAL_FAULT,
								ACE_TEXT(""),
								szData,
								ACE_TEXT("CREATE THREAD FAILED IN SSU SYSTEM SUPERVISOR"));
						dwResult  = 3;
					}
					else
					{
						m_s32GrpId = s8Result;
					}
				}
			}
		}
	}
	DEBUG("%s","Exiting acs_ssu_PerfMon::GetPerfCounterSettings()");
	return (dwResult == 0);
}

ACS_SSU_Counter* ACS_SSU_PerfMon::poGetPerformanceMonitorObject( const ACE_TCHAR* pszPerfObjectString )
{
	DEBUG("%s","Entering ACS_SSU_PerfMon::poGetPerformanceMonitorObject()");
	ACS_SSU_Counter *poPerfCounter = 0;

	if( ACE_OS::strcasecmp( pszPerfObjectString, SSU_PERF_MON_OBJ_STRING_MEM ) == 0 )
	{
		poPerfCounter = new ACS_SSU_Memory_Counter(m_phEndEvent, m_pAlarmHandler);
	}
	else if( ACE_OS::strcasecmp( pszPerfObjectString, SSU_PERF_MON_OBJ_STRING_FILE_HANDLE_COUNT ) == 0 )
	{
		poPerfCounter = new ACS_SSU_FileHandle_Counter(m_phEndEvent, m_pAlarmHandler);
	}

	DEBUG("%s","Exiting ACS_SSU_PerfMon::poGetPerformanceMonitorObject()");
	return poPerfCounter;
}

bool ACS_SSU_PerfMon::readAllPHAValuesForMemory()
{
	DEBUG("%s","Entering ACS_SSU_PerfMon::readAllPHAValuesForMemory()");
	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		DEBUG("%s","OmHandler Init is FAILED in readAllPHAValuesForMemory()");
		DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForMemory()");
		return false;
	}

	std::string dn(SSU_PERF_MON_MEM_RDN);
	dn.append(",");
	dn.append(ACS_SSU_Common::dnOfSSURoot);
	ACS_APGCC_ImmObject object;
	object.objName = dn.c_str();
	result = immHandle.getObject(&object);

	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler getObject is FAILED in readAllPHAValuesForMemory()");
		result = immHandle.Finalize();
		DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForMemory()");
		return false;
	}
	vector<ACS_APGCC_ImmAttribute> &objAttributeList = object.attributes;

	for ( unsigned int i = 0; i < objAttributeList.size(); i++ )
	{
		for (unsigned int j = 0; j < objAttributeList[i].attrValuesNum; j++)
		{
			if (objAttributeList[i].attrType == 2)
			{
				if(ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A1_ALARM) == 0)
				{
					srctPerfData[0].Value = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A1_CEASE) == 0)
				{
					srctPerfData[0].CeaseValue = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_ALARM) == 0)
				{
					srctPerfData[1].Value = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_CEASE) == 0)
				{
					srctPerfData[1].CeaseValue = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_POLL_INTERVAL) == 0)
				{
					srctPerfData[0].Interval = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
					srctPerfData[1].Interval = srctPerfData[0].Interval;
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A1_DUMP_FREQ) == 0)
				{
					srctPerfData[0].RunFirstTime = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_DUMP_FREQ) == 0)
				{
					srctPerfData[1].RunFirstTime = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}

			}
			else if (objAttributeList[i].attrType == 1)
			{
				if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_DISK_MONITORING_TYPE) == 0)
				{

					switch (*(reinterpret_cast< int*>(objAttributeList[i].attrValues[j])))
					{
					case 1:
						ACE_OS::strcpy(srctPerfData[0].Node, "both");
						ACE_OS::strcpy(srctPerfData[1].Node, "both");
						break;
					case 2:
						ACE_OS::strcpy(srctPerfData[0].Node, "active");
						ACE_OS::strcpy(srctPerfData[1].Node, "active");
						break;
					case 3:
						ACE_OS::strcpy(srctPerfData[0].Node, "passive");
						ACE_OS::strcpy(srctPerfData[1].Node, "passive");
						break;
					default:
						ACE_OS::strcpy(srctPerfData[0].Node, "both");
						ACE_OS::strcpy(srctPerfData[1].Node, "both");
						break;
					}
				}
				DEBUG("Attribute Name = %s and its Value %d",objAttributeList[i].attrName.c_str(),*(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j])))
			}
		}
	}

	result = immHandle.Finalize();
	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler Finalize is FAILED in readAllPHAValuesForMemory()");
	}

	DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForMemory()");
	return true;
}

bool ACS_SSU_PerfMon::readAllPHAValuesForFileHandle()
{
	DEBUG("%s","Entering ACS_SSU_PerfMon::readAllPHAValuesForFileHandle()");
	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		DEBUG("%s","OmHandler Init is FAILED in readAllPHAValuesForFileHandle()");
		DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForFileHandle()");
		return false;
	}

	std::string dn(SSU_PERF_MON_FILEHANDLE_RDN);
	dn.append(",");
	dn.append(ACS_SSU_Common::dnOfSSURoot);
	ACS_APGCC_ImmObject object;
	object.objName = dn.c_str();
	result = immHandle.getObject(&object);

	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler getObject is FAILED in readAllPHAValuesForFileHandle()");
		result = immHandle.Finalize();
		DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForFileHandle()");
		return false;
	}
	vector<ACS_APGCC_ImmAttribute> &objAttributeList = object.attributes;

	for ( unsigned int i = 0; i < objAttributeList.size(); i++ )
	{
		for (unsigned int j = 0; j < objAttributeList[i].attrValuesNum; j++)
		{
			if (objAttributeList[i].attrType == 2)
			{
				if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_ALARM) == 0)
				{
					srctPerfData[2].Value = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_CEASE) == 0)
				{
					srctPerfData[2].CeaseValue = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_POLL_INTERVAL) == 0)
				{
					srctPerfData[2].Interval = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}

				DEBUG("Attribute Name = %s and its Value %d",objAttributeList[i].attrName.c_str(),*(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j])))
			}
			else if (objAttributeList[i].attrType == 1)
			{
				if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_PERF_MON_A2_DUMP_FREQ) == 0)
				{
					srctPerfData[2].RunFirstTime = *(reinterpret_cast<int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_DISK_MONITORING_TYPE) == 0)
				{
					switch (*(reinterpret_cast<int*>(objAttributeList[i].attrValues[j])))
					{
					case 1:
						ACE_OS::strcpy(srctPerfData[2].Node, "both");
						break;
					case 2:
						ACE_OS::strcpy(srctPerfData[2].Node, "active");
						break;
					case 3:
						ACE_OS::strcpy(srctPerfData[2].Node, "passive");
						break;
					default:
						ACE_OS::strcpy(srctPerfData[2].Node, "both");
						break;
					}
				}

				DEBUG("Attribute Name = %s and its Value %d",objAttributeList[i].attrName.c_str(),*(reinterpret_cast<int*>(objAttributeList[i].attrValues[j])))
			}
		}
	}

	result = immHandle.Finalize();
	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler Finalize is FAILED in readAllPHAValuesForFileHandle()");
	}

	DEBUG("%s","Exiting ACS_SSU_PerfMon::readAllPHAValuesForFileHandle()");
	return true;
}
/*! @} */
