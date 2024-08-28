/*=================================================================== */
/**
   @file acs_ssu_disk_mon.cpp

   This module contains the implementation for All mounted partitions are monitored using statvfs
   system call. This monitors not only local file systems but also remote file systems which are
   mounted.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <algorithm>
//! For Monitoring System memory
#include <mntent.h>
#include <sys/statvfs.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ACS_APGCC_Util.H>
#include "acs_ssu_disk_mon.h"
#include "acs_ssu_types.h"
#include "acs_ssu_common.h"
#include "acs_ssu_aeh.h"
#include "acs_ssu_pha.h"
#include "acs_ssu_svc_loader.h"
#include "acs_ssu_alarm_handler.h"

//! Minimum size of a partition to monitor (4 Mbyte)
#define MIN_DRIVESIZE_TO_MONITOR (ACE_INT32)4194304

SSU_DISK_ALARM_LEVEL srctDiskAlarmLevelLow;
SSU_DISK_ALARM_LEVEL srctDiskAlarmLevelHigh;

//============================================================================
// Constructor
//============================================================================
ACS_SSU_DiskMon::ACS_SSU_DiskMon(ACE_Event* poEvent,ACS_SSU_AlarmHandler* poAlarmHandler):
		m_phEndEvent(poEvent),
		m_poAlarmHandler(poAlarmHandler)
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::ACS_SSU_DiskMon()");

	// Initialize class variables m_srctDefaultDiskDataLow and m_srctDefaultDiskDataHigh
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Object,ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Counter,ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Instance,ACE_TEXT(""));
	m_srctDefaultDiskDataLow.AlertIfOver = false;
	m_srctDefaultDiskDataLow.Value = 0;
	m_srctDefaultDiskDataLow.Interval = 0;
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Node,ACE_TEXT(""));
	m_srctDefaultDiskDataLow.EventNumber= 0;
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Severity,ACE_TEXT(""));
	m_srctDefaultDiskDataLow.CeaseValue = 0;
	m_srctDefaultDiskDataLow.RunFirstTime = false;
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Application,ACE_TEXT(""));

	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Object,ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Counter,ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Instance,ACE_TEXT(""));
	m_srctDefaultDiskDataHigh.AlertIfOver = false;
	m_srctDefaultDiskDataHigh.Value = 0;
	m_srctDefaultDiskDataHigh.Interval = 0;
	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Node,ACE_TEXT(""));
	m_srctDefaultDiskDataHigh.EventNumber= 0;
	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Severity,ACE_TEXT(""));
	m_srctDefaultDiskDataHigh.CeaseValue = 0;
	m_srctDefaultDiskDataHigh.RunFirstTime = false;
	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Application,ACE_TEXT(""));

	ACE_OS::strcpy(m_srctDiskAlarmLevelLow.Severity, ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDiskAlarmLevelLow.SmallDiskSizeLimit, ACE_TEXT(""));
	m_srctDiskAlarmLevelLow.AlarmLevelSmallDisk = 0 ;
	m_srctDiskAlarmLevelLow.CeaseLevelSmallDisk = 0;
	m_srctDiskAlarmLevelLow.AlarmLevelLargeDisk = 0;
	m_srctDiskAlarmLevelLow.CeaseLevelLargeDisk = 0;

	ACE_OS::strcpy(m_srctDiskAlarmLevelHigh.Severity, ACE_TEXT(""));
	ACE_OS::strcpy(m_srctDiskAlarmLevelHigh.SmallDiskSizeLimit, ACE_TEXT(""));
	m_srctDiskAlarmLevelHigh.AlarmLevelSmallDisk = 0 ;
	m_srctDiskAlarmLevelHigh.CeaseLevelSmallDisk = 0;
	m_srctDiskAlarmLevelHigh.AlarmLevelLargeDisk = 0;
	m_srctDiskAlarmLevelHigh.CeaseLevelLargeDisk = 0;

	ACE_ASSERT( m_phEndEvent != 0 );
	ACE_ASSERT( m_poAlarmHandler != 0 );
	DEBUG("%s","Exiting ACS_SSU_DiskMon::ACS_SSU_DiskMon()");
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_DiskMon::~ACS_SSU_DiskMon()
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::~ACS_SSU_DiskMon()");
	DEBUG("%s","Exiting ACS_SSU_DiskMon::~ACS_SSU_DiskMon()");
}

void ACS_SSU_DiskMon::Execute()
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::Execute()");
	if(!bGetDiskMonitorSettings())
	{
		ERROR("%s","  Fetching Disk Monitoring Parameters has failed");
		DEBUG("%s","Exiting ACS_SSU_DiskMon::Execute()");
		return;
	}

	ACE_Time_Value oTimeValue(m_srctDefaultDiskDataLow.Interval);

	while( m_phEndEvent->wait(&oTimeValue,0))
	{
		std::map<string,struct statvfs> oPartitionsInfo;

		BOOL bResult = bGetDiskData(oPartitionsInfo);
		if (bResult)
		{
			bResult = bRemoveSmallPartitions(oPartitionsInfo);

		}

		if (bResult)
		{
			vPrintPartitions(oPartitionsInfo);
			vCheckPartitions(oPartitionsInfo);
		}

		INFO(" ACS_SSU_DiskMon Running after %d sec", oTimeValue.msec()/1000);
	}

	DEBUG("%s","Exiting ACS_SSU_DiskMon::Execute()");
}

//============================================================================
// Get the PHA parameters for disk partition monitoring
//============================================================================
BOOL ACS_SSU_DiskMon::bGetDiskMonitorSettings()
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::bGetDiskMonitorSettings()");

	m_srctDefaultDiskDataLow.RunFirstTime = SSU_ENABLE_EXECUTE_APPL_FIRST_TIME;
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Application,ACE_TEXT(SSU_APPLICATION_TO_RUN));
	ACE_OS::strcpy(srctDiskAlarmLevelLow.Severity,SSU_A2_SEVERITY);
	ACE_OS::strcpy(srctDiskAlarmLevelLow.SmallDiskSizeLimit,SSU_MAX_SIZEOF_SMALL_PARTITION);

	ACE_OS::strcpy(srctDiskAlarmLevelHigh.Severity,SSU_A1_SEVERITY);
	ACE_OS::strcpy(srctDiskAlarmLevelHigh.SmallDiskSizeLimit,SSU_MAX_SIZEOF_SMALL_PARTITION);

	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Object,SSU_DISK_MON_OBJECT_NAME);
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Counter,SSU_DISK_MON_COUNTER);
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Instance,SSU_DISK_MON_INSTANCE_VALUE);
	m_srctDefaultDiskDataLow.AlertIfOver = SSU_DISABLE_ALERT_IF_OVER;
	ACE_OS::strcpy(m_srctDefaultDiskDataLow.Severity,SSU_A2_SEVERITY);
	m_srctDefaultDiskDataLow.EventNumber = SSU_DISK_MON_EVENT_NUM;

	if(!readAllPHAValuesForLogicalDisk())
	{
		DEBUG("%s","Exiting ACS_SSU_DiskMon::bGetDiskMonitorSettings()");
		return false;
	}
	else
	{
		ACE_OS::memcpy(&m_srctDiskAlarmLevelLow, &srctDiskAlarmLevelLow, sizeof(SSU_DISK_ALARM_LEVEL));
		ACE_OS::memcpy(&m_srctDiskAlarmLevelHigh, &srctDiskAlarmLevelHigh, sizeof(SSU_DISK_ALARM_LEVEL));
		ACE_OS::memcpy(&m_srctDefaultDiskDataHigh, &m_srctDefaultDiskDataLow, sizeof(SSU_PERF_DATA));
	}


	ACE_OS::strcpy(m_srctDefaultDiskDataHigh.Severity,SSU_A1_SEVERITY);

	//! Validate the severity class value
	if(!ACS_SSU_Common::bCheckValidSeverityClass(m_srctDiskAlarmLevelLow.Severity))
	{
		ACE_TCHAR szText[128];

		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT(" ( %d )Invalid severity class (%s) in PHA parameter \"%s\""),
				__LINE__,
				m_srctDiskAlarmLevelLow.Severity,
				SSU_PHA_DEFDISKMONLOW);

		// PHA error. send event
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

		DEBUG("%s","Entering ACS_SSU_DiskMon::bGetDiskMonitorSettings()");
		return FALSE;
	}

	//! Validate the severity class value
	if (!ACS_SSU_Common::bCheckValidSeverityClass(m_srctDiskAlarmLevelHigh.Severity))
	{
		ACE_TCHAR szText[128];
		snprintf(szText,sizeof(szText)/sizeof(*szText)-1,
				ACE_TEXT("Invalid severity class (%s) in PHA parameter \"%s\""),
				m_srctDiskAlarmLevelHigh.Severity,
				SSU_PHA_DEFDISKMONHIGH);

		//! PHA error. send event
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

		ERROR("%s",szText);
		DEBUG("%s","Entering ACS_SSU_DiskMon::bGetDiskMonitorSettings()");
		return FALSE;
	}


#if 0
	// COMMENTED AS SSU IS NOT SUPPORTING SPECIFIC PARTITIONS
	//! Get user specific alarm levels
	//Read all performance counters in the PHA database

	SSU_DISK_MONITOR srctDiskMonitor1;
	SSU_DISK_MONITOR srctDiskMonitor2;

	LPSSU_DISK_MONITOR pDiskMonitor;
	ACE_UINT32 u32Result = 0;

	INFO("%s","Fetching Specific partition Parameters");


	ACE_OS::memset(&srctDiskMonitor1,'\0',sizeof(SSU_DISK_MONITOR));
	ACE_OS::memset(&srctDiskMonitor2,'\0',sizeof(SSU_DISK_MONITOR));

	if(!(ACS_SSU_Common::retrievePHAParameter("prvA1SpecificAlarmLevel",srctDiskMonitor1.AlarmLevel)&&
			ACS_SSU_Common::retrievePHAParameter("prvA1SpecificCeaseLevel",srctDiskMonitor1.CeaseLevel)&&
			ACS_SSU_Common::retrievePHAParameter("prvA1SpecificPartitionName",srctDiskMonitor1.Partition)&&
			ACS_SSU_Common::retrievePHAParameter("prvA1Severity",srctDiskMonitor1.Severity)&&
			ACS_SSU_Common::retrievePHAParameter("prvA2SpecificAlarmLevel",srctDiskMonitor2.AlarmLevel)&&
			ACS_SSU_Common::retrievePHAParameter("prvA2SpecificCeaseLevel",srctDiskMonitor2.CeaseLevel)&&
			ACS_SSU_Common::retrievePHAParameter("prvA2SpecificPartitionName",srctDiskMonitor2.Partition)&&
			ACS_SSU_Common::retrievePHAParameter("prvA2Severity",srctDiskMonitor2.Severity)))
	{
		return false;
	}

	// Validate the severity class value
	else if (!(ACS_SSU_Common::bCheckValidSeverityClass(srctDiskMonitor1.Severity)&&ACS_SSU_Common::bCheckValidSeverityClass(srctDiskMonitor2.Severity)))
	{
		ACE_TCHAR szText[128];
		snprintf(szText,(sizeof(szText)/sizeof(*szText))-1,
				ACE_TEXT("Invalid severity class in PHA parameter table \"%s\""),
				SSU_PHA_DISKMONITOR);

		// PHA error. send event
		(void)ACS_SSU_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

		u32Result = 4;

	}
	else
	{
		pDiskMonitor = new SSU_DISK_MONITOR;

		if (!pDiskMonitor)
		{
			//! PHA error. send event
			(void)ACS_SSU_AEH::ReportEvent(1003,
					ACE_TEXT("EVENT"),
					CAUSE_AP_INTERNAL_FAULT,
					ACE_TEXT(""),
					ACE_TEXT("Failed to allocate heap memory."),
					SSU_AEH_TEXT_INT_SUPERVISOR_ERROR);

			u32Result = 3;

		}
		else
		{
			//! Add specific alarm level to list
			ACE_OS::memcpy(pDiskMonitor, &srctDiskMonitor1, sizeof(SSU_DISK_MONITOR));
			INFO("Add Specific Disk Alarm to List:%s",pDiskMonitor->Partition);
			m_listDiskMonitor.push_back(pDiskMonitor);

			INFO("Added %s With Severity %s AlarmRaise:%d Alarm Cease:%d To Monitoring",
					pDiskMonitor->Partition,pDiskMonitor->Severity,
					pDiskMonitor->AlarmLevel,pDiskMonitor->CeaseLevel);

			ACE_OS::memcpy(pDiskMonitor, &srctDiskMonitor2, sizeof(SSU_DISK_MONITOR));
			INFO("Add Specific Disk Alarm to List:%s",pDiskMonitor->Partition);
			m_listDiskMonitor.push_back(pDiskMonitor);

			INFO("Added %s With Severity %s AlarmRaise:%d Alarm Cease:%d To Monitoring",
					pDiskMonitor->Partition,pDiskMonitor->Severity,
					pDiskMonitor->AlarmLevel,pDiskMonitor->CeaseLevel);


		}
	}

	INFO(" GetDiskSettings() Result: %d",u32Result);
	return (u32Result == 0);
#endif
	DEBUG("%s","Exiting ACS_SSU_DiskMon::bGetDiskMonitorSettings()");
	return true;
}

BOOL ACS_SSU_DiskMon::bGetDiskData( std::map<string,struct statvfs> &refoPartitionsInfo )
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::bGetDiskData()");

	FILE *fpmnt = setmntent("/etc/mtab","r");
	struct mntent *poMntEntryPoint;
	BOOL bResult = FALSE;
	
	if (fpmnt != NULL)
	{
		while( (poMntEntryPoint=getmntent(fpmnt))!= 0 )
		{
			struct statvfs oPartitionInfo;

			//! Below check evalutes to -1 on INVALID Partition
			if( statvfs(poMntEntryPoint->mnt_dir, &oPartitionInfo) != -1 )
			{
				refoPartitionsInfo[poMntEntryPoint->mnt_dir] = oPartitionInfo ;
			}
		bResult = true;
		}
	}

	if( fpmnt != 0 )
	{
		endmntent(fpmnt);
	}
	DEBUG("%s","Exiting ACS_SSU_DiskMon::bGetDiskData()");
	return bResult;
}

BOOL ACS_SSU_DiskMon::bRemoveSmallPartitions( std::map<string,struct statvfs> & refoPartitionsInfo)
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::bRemoveSmallPartitions()");
	std::map<string,struct statvfs>::iterator it = refoPartitionsInfo.begin();

	for(; it != refoPartitionsInfo.end() ; )
	{
		if( ( it->second.f_blocks * it->second.f_bsize ) <= MIN_DRIVESIZE_TO_MONITOR)
		{
			refoPartitionsInfo.erase(it++);
		}
		else if ((strcmp(it->first.c_str(),CLUSTER_PARTITION) != 0 ) && (strcmp(it->first.c_str(),VAR_LOG_PARTITION) != 0) && (strcmp(it->first.c_str(),DATA_PARTITION) != 0 ))
		{
			// SSU Monitors /cluster , /var/log and /data partions only.
			refoPartitionsInfo.erase(it++);
		}
		else
		{
			++it;
		}
	}
	DEBUG("%s","Exiting ACS_SSU_DiskMon::bRemoveSmallPartitions()");
	return true;
}

void ACS_SSU_DiskMon::vPrintPartitions( std::map<string,struct statvfs> & refoPartitionsInfo )
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::vPrintPartitions()");
	DEBUG("%s","Partitions that are going to be monitored is ...")
	std::map<string,struct statvfs>::iterator it = refoPartitionsInfo.begin();

	for(; it != refoPartitionsInfo.end() ; ++it)
	{
		DEBUG(" %s : %ld ",it->first.c_str(), (it->second.f_bfree * it->second.f_bsize)/MEGABYTE);
	}
	DEBUG("%s","Exiting ACS_SSU_DiskMon::vPrintPartitions()");
}

void ACS_SSU_DiskMon::vCheckPartitions(std::map<string,struct statvfs> & refoPartitionsInfo )
{
	std::map<string,struct statvfs>::iterator it = refoPartitionsInfo.begin();
	std::list<SSU_PERF_DATA> olsAlarmsForPartition;

	for(; it != refoPartitionsInfo.end() ; ++it)
	{
		olsAlarmsForPartition.clear();
#if 0
		if( bGetSpecificPartitionSettings(it->first, olsAlarmsForPartition ) )
		{
			DEBUG(" Found Specific Alarm Settings For Partition : %s",it->first.c_str());
		}
		else
#endif
			if( bGetDefaultPartitionSettings( it->second,olsAlarmsForPartition ) )
			{
				//DEBUG(" Found Default Alarm Settings For Partition : %s",it->first.c_str());
			}

		bCheckAndRaiseAlarmForPartition(it->first,it->second,olsAlarmsForPartition);
	}
}

BOOL ACS_SSU_DiskMon::bCheckAndRaiseAlarmForPartition(string partition,
		struct statvfs & oFileSysData,
		std::list<SSU_PERF_DATA> &olsAlarmsForPartition)
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::bCheckAndRaiseAlarmForPartition()");
        (void) partition;
	BOOL bResult = FALSE;
	ACE_UINT64 u64FreeSize = oFileSysData.f_bfree * oFileSysData.f_bsize;
	ACE_UINT64 u64TotalSize= oFileSysData.f_blocks * oFileSysData.f_bsize;

	double dPercentageFreeSpace = (double)( (long double)u64FreeSize / (long double)u64TotalSize );
	dPercentageFreeSpace = dPercentageFreeSpace*(double)100;

	std::list<SSU_PERF_DATA>::iterator it =  olsAlarmsForPartition.begin();

	for( ;it != olsAlarmsForPartition.end(); ++it)
	{
		//ACE_OS::strcpy( (char*) it->Instance, "-");

		ACE_OS::strcpy( (char*) it->Instance, partition.c_str());
		if( strcmp(partition.c_str(),DATA_PARTITION)==0 )
		{
			ACE_OS::strcpy( (char*) it->Instance, SSU_DATA_DISK_MON_INSTANCE_NAME);
			ACE_OS::strcpy( (char*) it->Object , SSU_DATA_DISK_MON_OBJECT_NAME);
		}
		//! Check if threshold limit has been reached
		if (( it->AlertIfOver && dPercentageFreeSpace > it->Value) ||
				(!it->AlertIfOver && dPercentageFreeSpace  < it->Value))
		{
			//!@todo delete
			DEBUG("%s %s",it->Application,it->Instance);

			//! Check whether an alarm shall be issued on this node
			if (!(it->Node) ||
					ACE_OS::strcasecmp(it->Node, ACE_TEXT("both")) == 0 ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("active")) == 0 && ACS_APGCC::is_active_node()) ||
					(ACE_OS::strcasecmp(it->Node, ACE_TEXT("passive")) == 0 && !ACS_APGCC::is_active_node()))
			{
				//! Send alarm
				//DEBUG("%s","Sending Raise Alarm request");
				//  INFO(" Free:%10ld Total:%10ld percentage of Free space for %s :%10f",u64FreeSize, u64TotalSize,partition.c_str(),dPercentageFreeSpace);
				bResult  = m_poAlarmHandler->SendPerfAlarm(&*it,dPercentageFreeSpace);
			}
		}
		else
		{
			//! Send a cease only if collected value is below/over the cease level
			if (( it->AlertIfOver && dPercentageFreeSpace < it->CeaseValue) ||
					( !it->AlertIfOver && dPercentageFreeSpace > it->CeaseValue ))
			{
				//! Send cease
				//DEBUG("%s","Sending Cease Alarm request");
				// INFO(" Free:%10ld Total:%10ld percentage of Free space for %s :%10f",u64FreeSize, u64TotalSize,partition.c_str(),dPercentageFreeSpace);
				bResult = m_poAlarmHandler->SendPerfCease(&*it, dPercentageFreeSpace);
			}
		}
	}

	DEBUG("%s","Exiting ACS_SSU_DiskMon::bCheckAndRaiseAlarmForPartition()");
	return bResult;
}

BOOL ACS_SSU_DiskMon::bGetSpecificPartitionSettings(const string &pszpartition,  std::list< SSU_PERF_DATA > &refoDiskData )
{
	BOOL bResult = FALSE;
	SSU_PERF_DATA oPermAlarmData = m_srctDefaultDiskDataLow;
	std::list<LPSSU_DISK_MONITOR>::iterator it = m_listDiskMonitor.begin();

	for(; it != m_listDiskMonitor.end(); ++it )
	{
		LPSSU_DISK_MONITOR poDiskAlarmData = *it;

       if( pszpartition == poDiskAlarmData->Partition )
       {
    	   oPermAlarmData.Value = poDiskAlarmData->AlarmLevel;
    	   oPermAlarmData.CeaseValue = poDiskAlarmData->CeaseLevel;
    	   ACE_OS::strcpy( oPermAlarmData.Severity,poDiskAlarmData->Severity);
    	   refoDiskData.push_back(oPermAlarmData);
    	   bResult = true;
       }
	}

    if( bResult == true )
    	refoDiskData.sort(AlarmSortPredicate<SSU_PERF_DATA>());

    return bResult;
}

BOOL ACS_SSU_DiskMon::bGetDefaultPartitionSettings( struct statvfs & refoFileSysInfo,  std::list< SSU_PERF_DATA > &refoDiskData )
{
	//DEBUG("%s","Entering ACS_SSU_DiskMon::bGetDefaultPartitionSettings()");
	ACE_INT64 u64FileSysSize = refoFileSysInfo.f_bfree * refoFileSysInfo.f_bsize;

	//! Intialize default settings for High Alarm( "A1" )
	ACE_OS::strcpy( m_srctDefaultDiskDataLow.Severity , m_srctDiskAlarmLevelHigh.Severity );

	//! @todo: Check size of int is 64 on LOTC
	if ( u64FileSysSize <  ACE_OS::atoi( m_srctDiskAlarmLevelHigh.SmallDiskSizeLimit ) )
	{
		m_srctDefaultDiskDataLow.Value = m_srctDiskAlarmLevelHigh.AlarmLevelSmallDisk;
		m_srctDefaultDiskDataLow.CeaseValue = m_srctDiskAlarmLevelHigh.CeaseLevelSmallDisk;
	}
	else
	{
		m_srctDefaultDiskDataLow.Value = m_srctDiskAlarmLevelHigh.AlarmLevelLargeDisk;
		m_srctDefaultDiskDataLow.CeaseValue = m_srctDiskAlarmLevelHigh.CeaseLevelLargeDisk;
	}

	refoDiskData.push_back(m_srctDefaultDiskDataLow);

	//! Intialize default settings for Low Alarm( "A2" )
	ACE_OS::strcpy( m_srctDefaultDiskDataLow.Severity , m_srctDiskAlarmLevelLow.Severity );

	if ( u64FileSysSize <  ACE_OS::atoi(m_srctDiskAlarmLevelLow.SmallDiskSizeLimit) )
	{
		m_srctDefaultDiskDataLow.Value = m_srctDiskAlarmLevelLow.AlarmLevelSmallDisk;
		m_srctDefaultDiskDataLow.CeaseValue = m_srctDiskAlarmLevelLow.CeaseLevelSmallDisk;
	}
	else
	{
		m_srctDefaultDiskDataLow.Value = m_srctDiskAlarmLevelLow.AlarmLevelLargeDisk;
		m_srctDefaultDiskDataLow.CeaseValue = m_srctDiskAlarmLevelLow.CeaseLevelLargeDisk;
	}

	refoDiskData.push_back(m_srctDefaultDiskDataLow);
	//DEBUG("%s","Exiting ACS_SSU_DiskMon::bGetDefaultPartitionSettings()");
	return true;

}

bool ACS_SSU_DiskMon::readAllPHAValuesForLogicalDisk()
{
	DEBUG("%s","Entering ACS_SSU_DiskMon::readAllPHAValuesForLogicalDisk()");
	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS )
	{
		DEBUG("%s","OmHandler Init is FAILED in readAllPHAValuesForLogicalDisk()");
		DEBUG("%s","Exiting ACS_SSU_DiskMon::readAllPHAValuesForLogicalDisk()");
		return false;
	}

	std::string dn(SSU_LOGICAL_DISK_RDN);
	dn.append(",");
	dn.append(ACS_SSU_Common::dnOfSSURoot);
	ACS_APGCC_ImmObject object;
	object.objName = dn.c_str();
	result = immHandle.getObject(&object);

	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler getObject is FAILED in readAllPHAValuesForLogicalDisk()");
		result = immHandle.Finalize();
		DEBUG("%s","Exiting ACS_SSU_DiskMon::readAllPHAValuesForLogicalDisk()");
		return false;
	}
	vector<ACS_APGCC_ImmAttribute> &objAttributeList = object.attributes;

	for ( unsigned int i = 0; i < objAttributeList.size(); i++ )
	{
		for (unsigned int j = 0; j < objAttributeList[i].attrValuesNum; j++)
		{
			if ( (objAttributeList[i].attrType == 9) || (objAttributeList[i].attrType == 6) )
			{
				//cout << "     " << (char*)(objAttributeList[i].attrValues[j]);
			}
			else if (objAttributeList[i].attrType == 2)
			{
				if(ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_SMALLDISK_A1_ALARM) == 0)
				{
					srctDiskAlarmLevelHigh.AlarmLevelSmallDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_SMALLDISK_A1_CEASE) == 0)
				{
					srctDiskAlarmLevelHigh.CeaseLevelSmallDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_LARGEDISK_A1_ALARM) == 0)
				{
					srctDiskAlarmLevelHigh.AlarmLevelLargeDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_LARGEDISK_A1_CEASE) == 0)
				{
					srctDiskAlarmLevelHigh.CeaseLevelLargeDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_SMALLDISK_A2_ALARM) == 0)
				{
					srctDiskAlarmLevelLow.AlarmLevelSmallDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_SMALLDISK_A2_CEASE) == 0)
				{
					srctDiskAlarmLevelLow.CeaseLevelSmallDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_LARGEDISK_A2_ALARM) == 0)
				{
					srctDiskAlarmLevelLow.AlarmLevelLargeDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_LARGEDISK_A2_CEASE) == 0)
				{
					srctDiskAlarmLevelLow.CeaseLevelLargeDisk = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}
				else if (objAttributeList[i].attrName.compare(SSU_LOGICAL_DISK_POLL_INTERVAL) == 0)
				{
					m_srctDefaultDiskDataLow.Interval = *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
				}

				DEBUG("Attribute Name = %s and its Value %d",objAttributeList[i].attrName.c_str(),*(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j])))
			}else if (objAttributeList[i].attrType == 1)
			{
				if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_DISK_MONITORING_TYPE) == 0)
				{
					switch (*(reinterpret_cast<int*>(objAttributeList[i].attrValues[j])))
					{
					case 1:
						ACE_OS::strcpy(m_srctDefaultDiskDataLow.Node, "both");
						break;
					case 2:
						ACE_OS::strcpy(m_srctDefaultDiskDataLow.Node, "active");
						break;
					case 3:
						ACE_OS::strcpy(m_srctDefaultDiskDataLow.Node, "passive");
						break;
					default:
						ACE_OS::strcpy(m_srctDefaultDiskDataLow.Node, (char *)"both");
						break;
					}
				}
				else if (ACE_OS::strcmp(objAttributeList[i].attrName.c_str(),SSU_LOGICAL_DISK_DUMP_FREQ) == 0)
				{
					m_srctDefaultDiskDataLow.RunFirstTime = *(reinterpret_cast<int*>(objAttributeList[i].attrValues[j]));
				}

				DEBUG("Attribute Name = %s and its Value %d",objAttributeList[i].attrName.c_str(),*(reinterpret_cast<int*>(objAttributeList[i].attrValues[j])))

				// cout << "     " << *(reinterpret_cast<int*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 3)
			{
				// cout << "     " << *(reinterpret_cast<long long*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 4)
			{
				// cout << "     " << *(reinterpret_cast<unsigned long long*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 7)
			{
				// cout << "     " << *(reinterpret_cast<float*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 8)
			{
				// cout << "     " << *(reinterpret_cast<double*>(objAttributeList[i].attrValues[j]));
			}
		}
	}

	result = immHandle.Finalize();
	if (result != ACS_CC_SUCCESS)
	{
		DEBUG("%s","OmHandler Finalize is FAILED in readAllPHAValuesForLogicalDisk()");
	}

	DEBUG("%s","Exiting ACS_SSU_DiskMon::readAllPHAValuesForLogicalDisk()");
	return true;
}
/*! @} */
