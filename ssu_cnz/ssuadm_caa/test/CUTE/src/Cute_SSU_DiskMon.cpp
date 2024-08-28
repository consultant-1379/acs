/**
 * @file Cute_SSU_DiskMon.cpp
 * @brief
 *Test functions ACS_SSU_DiskMon are defined in this file
 */

/* INCLUDE SECTION */
/* Inclusion of Cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_DiskMon.h"

/*Inclusion of ssu block header files */
#include "acs_ssu_disk_mon.h"
#include "acs_ssu_alarm_handler.h"
/* Inclusion of ssu cute test header file */
#include "Cute_SSU_Monitor_Service.h"
/* Inclusion of system memory library files */
#include <mntent.h>
#include <sys/statvfs.h>

//Static variable declaration
ACS_SSU_DiskMon * Cute_SSU_DiskMon::m_poDiskMon=0;

/*======================================================
 * CONSTRUCTOR
 *====================================================== */
Cute_SSU_DiskMon::Cute_SSU_DiskMon()
{

}

/*======================================================
 * ROUTINE:vTestAlarmHandler()
 *======================================================*/
//void Cute_SSU_DiskMon::vTestAlarmHandler(){
//	BOOL bResult=getPoDiskMon()->bTestAlarmHandler();
//	ASSERTM("Unable to raise dummy alarm for disk partitions",bResult==true);
//}
/*======================================================
 * ROUTINE:vTestGetDiskMonitorSettings()
 *======================================================*/

void Cute_SSU_DiskMon::vTestGetDiskMonitorSettings(){

	ASSERTM("Unable to get disk monitoring settings from PHA tables",getPoDiskMon()->bGetDiskMonitorSettings()==true);
} /* end vTestGetDiskMonitorSettings()*/

/*=========================================================
 * ROUTINE:vTestGetDiskData()
 *=========================================================*/
void Cute_SSU_DiskMon::vTestGetDiskData(){
	std::map<string,struct statvfs> oPartitionsInfo;

	getPoDiskMon()->bGetDiskData(oPartitionsInfo);

	ASSERTM("Unable to get the disk data",oPartitionsInfo.empty() == false);
}/* end vTestGetDiskData()*/

/*===========================================================
 * ROUTINE:vTestRemoveSmallPartitions()
 *===========================================================*/
void Cute_SSU_DiskMon::vTestRemoveSmallPartitions(){
	 std::map<string,struct statvfs> oPartitionsInfo;
       BOOL bResult = getPoDiskMon()->bGetDiskData(oPartitionsInfo);

      bool brResult = getPoDiskMon()->bRemoveSmallPartitions(oPartitionsInfo);

	ASSERTM("Unable to remove smaller disk partitions",brResult==true);

} /*end vTestRemoveSmallPartitions()*/

/*===============================================================
 * ROUTINE:vGetSpecificPartitionSettings()
 *===============================================================*/
void Cute_SSU_DiskMon::vGetSpecificPartitionSettings(){

	BOOL bResult;

    list<SSU_PERF_DATA> olsAlarmsForPartition;
    olsAlarmsForPartition.clear();

    bResult= getPoDiskMon()->bGetSpecificPartitionSettings("/boot", olsAlarmsForPartition );

    ASSERTM("\n Specific for partition /boot are not present",olsAlarmsForPartition.empty() == false);
}/*end vGetSpecificPartitionSettings()*/

/*=================================================================
 * ROUTINE:vGetDefaultParitionSettings()
 *=================================================================*/
void Cute_SSU_DiskMon::vGetDefaultParitionSettings(){
	BOOL bResult=false;
	list<SSU_PERF_DATA> olsAlarmsForPartition;

	struct statvfs refoFileSysInfo;
	refoFileSysInfo.f_blocks = 10;
	refoFileSysInfo.f_bsize = MEGABYTE;

    bResult= getPoDiskMon()->bGetDefaultPartitionSettings( refoFileSysInfo, olsAlarmsForPartition);

    ASSERTM("\n Default disk partition settings are not present ", olsAlarmsForPartition.empty() == false);

}/*end vGetSpecificPartitionSettings()*/

/*=========================================================
 * ROUTINE:vCheckAndRaiseAlarmForPartition()
 *=========================================================*/
void Cute_SSU_DiskMon::vCheckAndRaiseAlarmForPartition(){

	BOOL bResult=false;

	list<SSU_PERF_DATA> olsAlarmsForPartition;
	struct statvfs oFileSysData;

	oFileSysData.f_blocks = 100;
	oFileSysData.f_bfree = 5;  // Free size is less than 5
	oFileSysData.f_bsize = 1;

	SSU_PERF_DATA oPerfData;
	ACE_OS::strcpy(oPerfData.Object,"LogicalDisk");
	ACE_OS::strcpy(oPerfData.Instance,"/temp");
	ACE_OS::strcpy(oPerfData.Severity,"A2");
	ACE_OS::strcpy(oPerfData.Node,"both");

	ACE_OS::strcpy(oPerfData.Application,"/opt/ap/acs/bin/acs_ssu_procls.sh");


	oPerfData.AlertIfOver = false;
	oPerfData.Value = 6; //Alarm Raise Value is 6
	oPerfData.CeaseValue=8;


    olsAlarmsForPartition.push_back(oPerfData);

    bResult=getPoDiskMon()->bCheckAndRaiseAlarmForPartition("/temp",oFileSysData,olsAlarmsForPartition);

    ASSERTM("Unable to check and raise the alarms for Disk partition /temp",bResult==true);
}/*end vCheckAndRaiseAlarmForPartition()*/

/*======================================================
 * ROUTINE:vCheckCeaseAlarmForPartition()
 *======================================================*/
void Cute_SSU_DiskMon::vCheckCeaseAlarmForPartition(){
	BOOL bResult=false;

	list<SSU_PERF_DATA> olsAlarmsForPartition;
	struct statvfs oFileSysData;

	oFileSysData.f_blocks = 100;
	oFileSysData.f_bfree = 10;  // Free size is less than 5
	oFileSysData.f_bsize = 1;

	SSU_PERF_DATA oPerfData;
	ACE_OS::strcpy(oPerfData.Instance,"/temp");
	ACE_OS::strcpy(oPerfData.Severity,"A2");
	oPerfData.AlertIfOver = false;
	oPerfData.Value = 6; //Alarm Raise Value is 10
	oPerfData.CeaseValue=8;
    olsAlarmsForPartition.push_back(oPerfData);

    bResult= getPoDiskMon()->bCheckAndRaiseAlarmForPartition("/temp",oFileSysData,olsAlarmsForPartition);

    ASSERTM("Unable to check and cease the alarms for Disk partition /temp",bResult== true);

} /* end vCheckCeaseAlarmForPartition()*/


/*==========================================================
 * ROUTINE:make_suite_Cute_SSU_DiskMon()
 *==========================================================*/
cute::suite Cute_SSU_DiskMon::make_suite_Cute_SSU_DiskMon(){

	Cute_SSU_DiskMon::m_poDiskMon = new ACS_SSU_DiskMon( Cute_SSU_Monitor_Service::getPlhEventHandles(),Cute_SSU_Monitor_Service::getPAlarmHandler() );
	cute::suite s;
	//s.push_back(CUTE(vTestAlarmHandler));
	s.push_back(CUTE(vTestGetDiskMonitorSettings));
	s.push_back(CUTE(vTestGetDiskData));
	s.push_back(CUTE(vTestRemoveSmallPartitions));
    s.push_back(CUTE(vGetSpecificPartitionSettings));
	s.push_back(CUTE(vGetDefaultParitionSettings));
	s.push_back(CUTE(vCheckAndRaiseAlarmForPartition));
	s.push_back(CUTE(vCheckCeaseAlarmForPartition));

	return s;
}/* end make_suite_Cute_SSU_DiskMon()*/

 void Cute_SSU_DiskMon:: vDiskMonCleanup()
	{
		delete m_poDiskMon;
	}



