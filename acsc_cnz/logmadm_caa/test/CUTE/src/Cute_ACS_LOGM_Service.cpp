
/**@file Cute_ACS_LOGM_Service.cpp
 * Test functions to test ACS_LOGM_SVC_Loader are defined in this file
 * @version 1.1.1
 */
/* INCLUDE SECTION */
/* inclusion of ACE library */
#include <ace/ACE.h>
/* Inclusion of  Cute libraries*/
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_ACS_LOGM_Service.h"


/* Inclusion of required LOGM header files */
#include "acs_logm_svc_loader.h"
#include "acs_logm_common.h"


/*static variables declaration*/

LOGM_Service_Loader* Cute_ACS_LOGM_Service::m_poServiceLoader;

ACE_Event *Cute_ACS_LOGM_Service::m_plhEventHandles[NO_OF_HANDLES]={0};

/* ROUTINE:dummyTestThreadFuntion()
 */
ACE_THR_FUNC_RETURN dummyTestThreadFuntion(void *){

}/*end dummyTestThreadFuntion()*/


/* ROUTINE:vCuteLOGMInit()
 */
/*void Cute_ACS_LOGM_Service::vCuteLOGMInit()
{
   m_plhEventHandles[0] = new ACE_Event(
			                               FALSE,
                                         FALSE,
                                         USYNC_THREAD,
                                         "EV_ACS_LOGM_TERMINATE_EVENT"
			                           );



}*/
/* end vCuteLOGMInit()*/

/* ROUTINE:vCuteLOGMCleanUp()
 */
/*void Cute_ACS_LOGM_Service::vCuteLOGMCleanUp()
{
   delete m_plhEventHandles[0];


}/*end vCuteLOGMCleanUp()*/


/* ROUTINE:CuteLOGMService()
 */
Cute_ACS_LOGM_Service::Cute_ACS_LOGM_Service()
{
  m_poServiceLoader = new LOGM_Service_Loader();

}/* end CuteLOGMService()*/

/* ROUTINE:vTestParseArgs()
 */
void Cute_ACS_LOGM_Service::vTestParseArgs(){

	ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::OSTREAM);

	ACE_DEBUG((LM_INFO,"\n vTestArgs"));

	//ACE_TCHAR ptr = '\0';


	ACE_TCHAR* argv[4]= {"./ACS_ACSC_Logmaint"," ","-V","\0"};


    ACE_INT32 s32Result= getPoServiceLoader()->parseArgs(4,argv);
    ACE_DEBUG((LM_INFO,ACE_TEXT("return value of parsing():%d"),s32Result));

	ASSERTM("\n Unable to parseArgs:",s32Result>0);
}/*end vTestParseArgs()*/

/* ROUTINE:vTestGetProcessLock()
 */
void Cute_ACS_LOGM_Service::vTestGetProcessLock(){

	ACE_HANDLE  fd;
	const char *path;

	fd = ACE_OS::open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	bool bResult=getPoServiceLoader()->getProcessLock(fd,path);
	ASSERTM("\n Unable to get the process lock ",bResult==true);

}/*end vTestGetProcessLock()*/

/*ROUTINE:vSetTimeoutForInteractiveMode()
 */
/*void CuteLOGMService::vSetTimeoutForInteractiveMode(){
	ACE_INT32 s32Result=getPoServiceLoader()->setTimeoutForInteractiveMode(10);
	ASSERTM("\n Unable to set the Timer for interactive mode ",s32Result!=-1);

}*/ /*end vSetTimeoutForInteractiveMode() */



/*ROUTINE:vTestStartThreadFunction()
 */
void Cute_ACS_LOGM_Service::vTestStartThreadFunction(){

	ACE_INT32 s32Result =  ACS_LOGM_Common::StartThread( dummyTestThreadFuntion,
	                           0,
	                          "vTestStartThreadFunction");

	ASSERTM("Spawning Thread Failed", s32Result != -1);

}/* end vTestStartThreadFunction() */


/*ROUTINE:make_suite_Cute_ACS_LOGM_Service()
 */
cute::suite Cute_ACS_LOGM_Service::make_suite_Cute_ACS_LOGM_Service(){

	cute::suite s;

//	s.push_back(CUTE(Cute_ACS_LOGM_Service::vTestStartThreadFunction));
 	s.push_back(CUTE(Cute_ACS_LOGM_Service::vTestParseArgs));
 	s.push_back(CUTE(Cute_ACS_LOGM_Service::vTestGetProcessLock));
 	//s.push_back(CUTE(CuteLOGMService::vSetTimeoutForInteractiveMode));
	return s;
}/*end make_suite_Cute_ACS_LOGM_Service()*/
