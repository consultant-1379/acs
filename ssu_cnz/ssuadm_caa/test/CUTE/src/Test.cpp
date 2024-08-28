/**
 * @file Test.cpp
 * It consists function definition to run the ssu test suites
 */

/* include section */
/* inclusion of ACE library*/
#include <ace/ACE.h>
/* inclusion of Cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
/* inclusion of SSU test header files */
#include "Cute_SSU_Monitor_Service.h"
#include "Cute_SSU_DiskMon.h"
#include "/vobs/cm4ap/ntacs/ssu_cnz/ssuadm_caa/test/CUTE/src/Cute_SSU_Common.h"
#include "Cute_SSU_AlarmHandler.h"
#include "Cute_SSU_FileHandle_Counter.h"
#include "Cute_SSU_Memory_Counter.h"
#include "Cute_SSU_FolderQuota.h"
#include "Cute_SSU_FolderQuotaMon.h"

bool theInteractiveMode = false;
bool ACS_SSU_Common::theIsActiveNode = false;
ACS_SSU_Service *pSSUObj  = 0;
std::string ACS_SSU_Common::dnOfSSURoot = "";
std::map<std::string,std::string> ACS_SSU_Common::ssuStringInitialParamsMap;
std::map<std::string,int> ACS_SSU_Common::ssuIntegerInitialParamsMap;


/**
 * @brief
 * Is a function to call the ssu test suites
 * No arguments
 *@return void
 */
void runSuite(){

	Cute_SSU_Monitor_Service::vCuteSSUInit();

	cute::suite s;
	//TODO add your test here
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
	cute::makeRunner(lis)(Cute_SSU_Monitor_Service::make_suite_Cute_SSU_Monitor_Service(),"Running Service Test Suite\n");
	cute::makeRunner(lis)(Cute_SSU_AlarmHandler::make_suite_Cute_SSU_AlarmHandler(),"Running Alarm handler Test suite\n");
    cute::makeRunner(lis)(Cute_SSU_DiskMon::make_suite_Cute_SSU_DiskMon(),"Running Disk Monitor Test Suite\n\n");
    cute::makeRunner(lis)(Cute_SSU_FileHandle_Counter::make_suite_Cute_SSU_FileHandle_Counter(),"Running FileHandlecounter Test Suite\n");
    cute::makeRunner(lis)(Cute_SSU_Memory_Counter::make_suite_Cute_SSU_Memory_Counter(),"Running memory counter test suite\n\n");
    cute::makeRunner(lis)(Cute_SSU_FolderQuota::make_suite_Cute_SSU_FolderQuota(),"Running FolderQuota test suite \n \n");
    cute::makeRunner(lis)(Cute_SSU_FolderQuotaMon::make_suite_Cute_SSU_FolderQuotaMon(),"Running FolderQuotaMon test suite \n \n");

cute::makeRunner(lis)(Cute_SSU_Common::make_suite_Cute_SSU_Common(),"Running common  test suite \n \n");




	Cute_SSU_DiskMon::vDiskMonCleanup();
	Cute_SSU_FileHandle_Counter::vFileHandleCleanup();
	Cute_SSU_Memory_Counter::vMemoryCounterCleanup();

	Cute_SSU_Monitor_Service::vCuteSSUCleanUp();
}/* end runSuite() */


/**
 * @brief
 * main function to call the runSuite() function
 * @return int
 * returns 0
 */
int main(){
    runSuite();
    return 0;
}
