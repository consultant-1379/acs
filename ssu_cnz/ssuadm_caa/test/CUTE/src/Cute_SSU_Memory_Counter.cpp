/**
 * @file Cute_SSU_Memory_Counter.cpp
 * Test functions for ACS_SSU_Memory_Counter are defined in this file
 */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_Memory_Counter.h"
#include "Cute_SSU_Monitor_Service.h"

#include "acs_ssu_memory_counter.h"
#include "acs_ssu_svc_loader.h"

/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:Cute_SSU_Memory_Counter()
 *========================================================*/
Cute_SSU_Memory_Counter::Cute_SSU_Memory_Counter(){
}


ACS_SSU_Memory_Counter * Cute_SSU_Memory_Counter::m_pOMemoryCounter=0;

/*========================================================
 *  ROUTINE:vTestParseCommandOutput()
 *========================================================*/
void Cute_SSU_Memory_Counter::vTestParseCommandOutput(){

	BOOL bResult=getPoMemoryCounter()->bParseCommandOutPut();

    ASSERTM("Unable to Parse the command output",bResult==TRUE);

	 }
/*========================================================
 *  ROUTINE:vTestCheckAndRaiseMemoryAlarm()
 *========================================================*/
void Cute_SSU_Memory_Counter::vTestCheckAndRaiseMemoryAlarm(){

	BOOL bResult=false;

	SSU_PERF_DATA oPerfData;

	ACE_OS::strcpy( oPerfData.Object,"Memory");

	ACE_OS::strcpy( oPerfData.Counter, "Percentage");

	ACE_OS::strcpy( oPerfData.Instance,"Mem");

	oPerfData.AlertIfOver =TRUE;

	ACE_OS::strcpy( oPerfData.Node,"Both");

	oPerfData.EventNumber = 6101;

	ACE_OS::strcpy( oPerfData.Severity,"A2");

   oPerfData.Value = 80;

	oPerfData.CeaseValue =75;

	getPoMemoryCounter()->bAddAlarm(oPerfData);

	bResult=getPoMemoryCounter()->bCheckAndRaiseAlarm();

	ASSERTM("Unable to raise Memory alarm",bResult==TRUE);

	 }
/*========================================================
 *  ROUTINE:vTestCheckAndCeaseMemoryAlarm()
 *========================================================*/
void Cute_SSU_Memory_Counter::vTestCheckAndCeaseMemoryAlarm(){

	BOOL bResult=false;

	SSU_PERF_DATA oPerfData;

	ACE_OS::strcpy( oPerfData.Object,"Memory");

	ACE_OS::strcpy( oPerfData.Counter, "Percentage");

	ACE_OS::strcpy( oPerfData.Instance,"Mem");

	oPerfData.AlertIfOver =TRUE;

	ACE_OS::strcpy( oPerfData.Node,"Both");

	oPerfData.EventNumber = 6101;

	ACE_OS::strcpy( oPerfData.Severity,"A2");

	oPerfData.Value = 90;

	oPerfData.CeaseValue =80;

	getPoMemoryCounter()->bAddAlarm(oPerfData);


	bResult=getPoMemoryCounter()->bCheckAndRaiseAlarm();

	ASSERTM("Unable to cease Memory alarm",bResult==TRUE);

	 }

void Cute_SSU_Memory_Counter::bTestUpdatePerformanceCounters(){

	bool bResult=getPoMemoryCounter()->bUpdatePerformanceCounters();

	ASSERTM("Unable to update File Handle alarm",bResult==true);
}
/*========================================================
 *  ROUTINE:vMemoryCounterCleanup()
 *========================================================*/
void Cute_SSU_Memory_Counter::vMemoryCounterCleanup(){

	delete m_pOMemoryCounter;

	 }

/*========================================================
 *  ROUTINE:getPoMemoryCounter()
 *========================================================*/
ACS_SSU_Memory_Counter *Cute_SSU_Memory_Counter::getPoMemoryCounter(){


	return m_pOMemoryCounter;

}
/*========================================================
 *  ROUTINE:make_suite_Cute_SSU_Memory_Counter()
 *========================================================*/
cute::suite Cute_SSU_Memory_Counter::make_suite_Cute_SSU_Memory_Counter(){

	m_pOMemoryCounter= new ACS_SSU_Memory_Counter(Cute_SSU_Monitor_Service::getPlhEventHandles(),Cute_SSU_Monitor_Service::getPAlarmHandler());
	cute::suite s;

	s.push_back(CUTE(Cute_SSU_Memory_Counter::vTestParseCommandOutput));

	s.push_back(CUTE(Cute_SSU_Memory_Counter::vTestCheckAndRaiseMemoryAlarm));

	s.push_back(CUTE(Cute_SSU_Memory_Counter::vTestCheckAndCeaseMemoryAlarm));

	s.push_back(CUTE(Cute_SSU_Memory_Counter::bTestUpdatePerformanceCounters));

	return s;
}



