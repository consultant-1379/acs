/**
 * @file Cute_SSU_FileHandle_Counter.cpp
 * Test functions for ACS_SSU_FileHandle_Counter are defined in this file
 */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_FileHandle_Counter.h"
#include "Cute_SSU_Monitor_Service.h"

#include "acs_ssu_file_handle_counter.h"
#include "acs_ssu_svc_loader.h"
#include "acs_ssu_counter.h"


/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:Cute_SSU_FileHandle_Counter()
 *========================================================*/

Cute_SSU_FileHandle_Counter::Cute_SSU_FileHandle_Counter(){
}

ACS_SSU_FileHandle_Counter * Cute_SSU_FileHandle_Counter::m_pOFileHandleCounter=0;
/*========================================================
 *  ROUTINE:vTestParseCommandOutput()
 *========================================================*/
void Cute_SSU_FileHandle_Counter::vTestParseCommandOutput(){

	BOOL bResult=getPoFileHandleCounter()->bParseCommandOutPut();

    ASSERTM("Unable to Parse the command output",bResult== true);

	 }
/*========================================================
 *  ROUTINE:vTestCheckAndRaiseAlarm()
 *========================================================*/
void Cute_SSU_FileHandle_Counter::vTestCheckAndRaiseAlarm(){
	BOOL bResult=false;

	//list<SSU_PERF_DATA> m_listPerfMonMonitor;

	SSU_PERF_DATA oPerfData;

	ACE_OS::strcpy( oPerfData.Object,"Process");

	ACE_OS::strcpy( oPerfData.Counter, "Number");

	ACE_OS::strcpy( oPerfData.Instance,"FILEHANDLECOUNT");

	oPerfData.AlertIfOver = true;
	ACE_OS::strcpy( oPerfData.Node,"both");

	oPerfData.EventNumber = 6105;

	ACE_OS::strcpy( oPerfData.Severity,"A2");

	oPerfData.Value = 210000;

	oPerfData.CeaseValue = 195000;
	oPerfData.RunFirstTime = 1;

	ACE_OS::strcpy(oPerfData.Application,"/opt/ap/acs/bin/acs_ssu_procls.sh");

	BOOL bStatus=getPoFileHandleCounter()->bAddAlarm(oPerfData);
	ACE_DEBUG((LM_INFO,ACE_TEXT("\n oPerfData added to performance object list: %d"),bStatus));

	bResult=getPoFileHandleCounter()->bCheckAndRaiseAlarm();

	ACE_DEBUG((LM_INFO,ACE_TEXT("\n raising alarm: %d"),bResult));

	ASSERTM("Unable to raise File Handle alarm",bResult==true);

	 }
/*========================================================
 *  ROUTINE:vTestCheckAndCeaseAlarm()
 *========================================================*/
void Cute_SSU_FileHandle_Counter::vTestCheckAndCeaseAlarm(){

	BOOL bResult=false;

	SSU_PERF_DATA oPerfData;

	ACE_OS::strcpy( oPerfData.Object,"Process");

	ACE_OS::strcpy( oPerfData.Counter, "Number");

	ACE_OS::strcpy( oPerfData.Instance,"FILEHANDLECOUNT");

	oPerfData.AlertIfOver = true;

	ACE_OS::strcpy( oPerfData.Node,"both");

	oPerfData.EventNumber = 6105;

	ACE_OS::strcpy( oPerfData.Severity,"A2");

	oPerfData.Value = 200000;

	oPerfData.CeaseValue = 190000;

	//oPerfData.RunFirstTime = 0;

	ACE_OS::strcpy(oPerfData.Application,"/opt/ap/acs/bin/acs_ssu_procls.sh");

	getPoFileHandleCounter()->bAddAlarm(oPerfData);

	bResult=getPoFileHandleCounter()->bCheckAndRaiseAlarm();
	ACE_DEBUG((LM_INFO,ACE_TEXT("\n ceasing alarm: %d"),bResult));

	ASSERTM("Unable to cease File Handle alarm",bResult==true);

	 }


void Cute_SSU_FileHandle_Counter::vTestbUpdatePerformanceCounters(){
	bool bResult=getPoFileHandleCounter()->bUpdatePerformanceCounters();

	ASSERTM("Unable to update File Handle alarm",bResult==true);
}

/*========================================================
 *  ROUTINE:vFileHandleCleanup()
 *========================================================*/
void Cute_SSU_FileHandle_Counter::vFileHandleCleanup(){

	delete m_pOFileHandleCounter;

	 }
/*========================================================
 *  ROUTINE:getPoFileHandleCounter()
 *========================================================*/
ACS_SSU_FileHandle_Counter *Cute_SSU_FileHandle_Counter::getPoFileHandleCounter(){


	return m_pOFileHandleCounter;
}
/*========================================================
 *  ROUTINE:make_suite_Cute_SSU_FileHandle_Counter()
 *========================================================*/
cute::suite Cute_SSU_FileHandle_Counter::make_suite_Cute_SSU_FileHandle_Counter(){

	m_pOFileHandleCounter= new ACS_SSU_FileHandle_Counter(Cute_SSU_Monitor_Service::getPlhEventHandles(),Cute_SSU_Monitor_Service::getPAlarmHandler());

	cute::suite s;

	s.push_back(CUTE(Cute_SSU_FileHandle_Counter::vTestParseCommandOutput));

	s.push_back(CUTE(Cute_SSU_FileHandle_Counter::vTestCheckAndRaiseAlarm));

	s.push_back(CUTE(Cute_SSU_FileHandle_Counter::vTestCheckAndCeaseAlarm));
	s.push_back(CUTE(Cute_SSU_FileHandle_Counter::vTestbUpdatePerformanceCounters));
	return s;
}



