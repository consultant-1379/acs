/**
 * @file Cute_SSU_AlarmHandler.cpp
 * Test functions ACS_SSU_AlarmHandler are defined in this file
 */
/* INCLUDE SECTION */
/* inclusion of cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

/* inclusion of SSU Cute test header files */
#include "Cute_SSU_AlarmHandler.h"
#include "Cute_SSU_DiskMon.h"
#include "Cute_SSU_Monitor_Service.h"

/* inclusion of SSU source header files */
#include "acs_ssu_alarm_handler.h"
#include "acs_ssu_disk_mon.h"
//#include "ACS_PHA_Parameter.h"
//#include "ACS_PHA_Tables.h"
/* Inclusion of ACE library files */
#include <ace/ACE.h>

/*===========================================
 * Declaring Static variables
 * ===========================================*/
//ACS_SSU_DiskMon * Cute_SSU_AlarmHandler::m_cpoDiskMon=0;

/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:Cute_SSU_AlarmHandler()
 *========================================================*/
Cute_SSU_AlarmHandler::Cute_SSU_AlarmHandler(){

}

/*========================================================
 *  ROUTINE:vTestSendAlarm()
 *========================================================*/

void Cute_SSU_AlarmHandler::vTestSendAlarm(){
	LPSSU_PERF_DATA pPerfData=new SSU_PERF_DATA() ;
	double nValue;
	strcpy(pPerfData->Node,"both");
	strcpy(pPerfData->Severity,"A2");
    nValue=1234567;
    bool bResult=Cute_SSU_Monitor_Service::getPAlarmHandler()->SendPerfAlarm(pPerfData,nValue);
    ASSERTM("Unable to raise A2 alarm",bResult==true);

}

/*========================================================
 *  ROUTINE:vTestCeaseAlarm()
 *========================================================*/
void Cute_SSU_AlarmHandler::vTestCeaseAlarm(){
	LPSSU_PERF_DATA pPerfData=new SSU_PERF_DATA();
		double nValue;
		strcpy(pPerfData->Node,"both");
		strcpy(pPerfData->Severity,"A2");
	    nValue=1234567;
	    BOOL bResult=Cute_SSU_Monitor_Service::getPAlarmHandler()->SendPerfCease(pPerfData,nValue);
		ASSERTM("Unable to cease A2 alarm",bResult==true);

}
void Cute_SSU_AlarmHandler::vTestSendFolderQuotaAlarm(){
	const long nEventNumber=6105;
	const ACE_TCHAR* lpszSeverity="A2";
	const ACE_TCHAR* lpszPath="APZ";
	const ACE_UINT32 nLimit=8;
	const double nValue=12345678;
 bool bResult=Cute_SSU_Monitor_Service::getPAlarmHandler()->SendFolderQuotaAlarm(nEventNumber,lpszSeverity,lpszPath,nLimit,nValue);\
 ASSERTM("Unable to send folder quota alarm",bResult==true);
}

void Cute_SSU_AlarmHandler::vTestSendFolderQuotaCease(){
	const long nEventNumber=6105;
	const ACE_TCHAR* lpszPath="APZ";
	const ACE_UINT32 nLimit=8;
		const double nValue=123456;
		 bool bResult=Cute_SSU_Monitor_Service::getPAlarmHandler()->SendFolderQuotaCease(nEventNumber,lpszPath,nLimit,nValue);
		 ASSERTM("Unable to cease folder quota alarm",bResult==true);
}




/*========================================================
 * ROUTINE:make_suite_Cute_SSU_AlarmHandler()
 *========================================================*/
cute::suite Cute_SSU_AlarmHandler::make_suite_Cute_SSU_AlarmHandler(){
	cute::suite s;

	s.push_back(CUTE(vTestSendAlarm));
	s.push_back(CUTE(vTestCeaseAlarm));
	s.push_back(CUTE(vTestSendFolderQuotaAlarm));
	s.push_back(CUTE(vTestSendFolderQuotaCease));

	return s;
}



