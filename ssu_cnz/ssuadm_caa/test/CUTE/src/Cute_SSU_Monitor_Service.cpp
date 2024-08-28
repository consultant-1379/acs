
/**@file Cute_SSU_Monitor_Service.cpp
 * Test functions to test ACS_SSU_SVC_Loader are defined in this file
 * @version 1.1.1
 */
/* INCLUDE SECTION */
/* inclusion of ACE library */
#include <ace/ACE.h>
/* Inclusion of  Cute libraries*/
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_SSU_Monitor_Service.h"


/* Inclusion of required ssu header files */
#include "acs_ssu_svc_loader.h"
#include "acs_ssu_alarm_handler.h"
#include "acs_ssu_common.h"

/*static variables declaration*/

SSU_Service_Loader* Cute_SSU_Monitor_Service::m_poServiceLoader;
ACS_SSU_AlarmHandler *Cute_SSU_Monitor_Service::m_poAlarmHandler = NULL;
//ACE_Event *Cute_SSU_Monitor_Service::m_plhEventHandles[NO_OF_HANDLES]={0};

/* ROUTINE:dummyTestThreadFuntion()
 */
ACE_THR_FUNC_RETURN dummyTestThreadFuntion(void *){
	return 0;

}/*end dummyTestThreadFuntion()*/


/* ROUTINE:vCuteSSUInit()
 */
void Cute_SSU_Monitor_Service::vCuteSSUInit()
{
   m_plhEventHandles[0] = new ACE_Event(
		   false,
		   false,
                                         USYNC_THREAD,
                                         "EV_ACS_SSU_TERMINATE_EVENT"
			                           );

   m_poAlarmHandler = new ACS_SSU_AlarmHandler();

}/* end vCuteSSUInit()*/

/* ROUTINE:vCuteSSUCleanUp()
 */
void Cute_SSU_Monitor_Service::vCuteSSUCleanUp()
{
   delete m_plhEventHandles[0];

   delete m_poAlarmHandler;
}/*end vCuteSSUCleanUp()*/

/* ROUTINE:CuteSSUMonitorService()
 */
Cute_SSU_Monitor_Service::Cute_SSU_Monitor_Service()
{
  m_poServiceLoader = new SSU_Service_Loader();

}/* end CuteSSUMonitorService()*/

/* ROUTINE:vTestParseArgs()
 */
void Cute_SSU_Monitor_Service::vTestParseArgs(){

	ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

	ACE_DEBUG((LM_INFO,"\n vTestArgs"));

	//ACE_TCHAR ptr = '\0';
	ACE_TCHAR* argv[4];

	argv[0] = new char [30];
	argv[1] = new char [10];
	argv[2] = new char [10];
	argv[3] = new char [10];
	//ACE_TCHAR* argv[4]= {"./SSUMONITORSEVICE_CXC"," ","-V","\0"};
	ACE_OS::strcpy(argv[0],"./SSUMONITORSEVICE_CXC");
	ACE_OS::strcpy(argv[1]," ");
	ACE_OS::strcpy(argv[2],"-V");
	ACE_OS::strcpy(argv[3],"\0");


    ACE_INT32 s32Result= getPoServiceLoader()->parseArgs(4,argv);
    ACE_DEBUG((LM_INFO,ACE_TEXT("return value of parsing():%d"),s32Result));

	ASSERTM("\n Unable to parseArgs:",s32Result>0);

	delete argv[0];
	delete argv[1];
	delete argv[2];
	delete argv[3];
}/*end vTestParseArgs()*/

/* ROUTINE:vTestGetProcessLock()
 */
void Cute_SSU_Monitor_Service::vTestGetProcessLock(){
	ACE_HANDLE  fd;
	const char *path;
	fd = ACE_OS::open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	bool bResult=getPoServiceLoader()->getProcessLock(fd,path);
	ASSERTM("\n Unable to get the process lock ",bResult==true);

}/*end vTestGetProcessLock()*/

/*ROUTINE:vSetTimeoutForInteractiveMode()
 */
/*void CuteSSUMonitorService::vSetTimeoutForInteractiveMode(){
	ACE_INT32 s32Result=getPoServiceLoader()->setTimeoutForInteractiveMode(10);
	ASSERTM("\n Unable to set the Timer for interactive mode ",s32Result!=-1);

}*/ /*end vSetTimeoutForInteractiveMode() */

/*ROUTINE:vSetupFilesAndDirectories()
 */
/*void Cute_SSU_Monitor_Service::vSetupFilesAndDirectories(){

    ASSERTM("Setting Up Files and DIrectories failed",m_poServiceLoader->SetupFileAndDirectories()== true);
}end vSetupFilesAndDirectories()*/

/*ROUTINE:vTestStartThreadFunction()
 */
void Cute_SSU_Monitor_Service::vTestStartThreadFunction(){

	ACE_INT32 s32Result =  ACS_SSU_Common::StartThread( dummyTestThreadFuntion,
	                           0,
	                          "vTestStartThreadFunction");

	ASSERTM("Spawning Thread Failed", s32Result != -1);

}/* end vTestStartThreadFunction() */


/*ROUTINE:make_suite_Cute_SSU_Monitor_Service()
 */
cute::suite Cute_SSU_Monitor_Service::make_suite_Cute_SSU_Monitor_Service(){

	cute::suite s;
	//s.push_back(CUTE(Cute_SSU_Monitor_Service::vSetupFilesAndDirectories));
	s.push_back(CUTE(Cute_SSU_Monitor_Service::vTestStartThreadFunction));
 	s.push_back(CUTE(Cute_SSU_Monitor_Service::vTestParseArgs));
 	//s.push_back(CUTE(Cute_SSU_Monitor_Service::vTestGetProcessLock));
 	//s.push_back(CUTE(CuteSSUMonitorService::vSetTimeoutForInteractiveMode));
	return s;
}/*end make_suite_Cute_SSU_Monitor_Service()*/
