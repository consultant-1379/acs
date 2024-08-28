/**
 * @file Test.cpp
 * It consists function definition to run the nsf test suites
 */

/* include section */
/* inclusion of ACE library*/
#include <ace/ACE.h>
/* inclusion of Cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
/* inclusion of NSF test header files */
#include "CUTE_ACS_NSF_Server.h"
#include "CUTE_ACS_NSF_Common.h"
#include "CUTE_NSF_Biostime_Recovery.h"
#include "CUTE_ACS_NSF_Command_Handler.h"



/**
 * @brief
 * Is a function to call the nsf test suites
 * No arguments
 *@return void
 */
void runSuite(){

	Cute_ACS_NSF_Server::init();
	Cute_ACS_NSF_Common::init();
	Cute_NSF_Biostime_Recovery::init();
	Cute_NSF_Command_Handler::init();
	cute::suite s;
	//TODO add your test here
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");

	cute::makeRunner(lis)(Cute_ACS_NSF_Common::make_suite_Cute_ACS_NSF_Common(),"Running Common Functionalities Test Suite\n");
	cute::makeRunner(lis)(Cute_ACS_NSF_Server::make_suite_Cute_ACS_NSF_Server(),"Running Service Test Suite\n");
	cute::makeRunner(lis)(Cute_NSF_Biostime_Recovery::make_suite_Cute_NSF_Biostime_Recovery(),"Running Biostime Recovery Suite\n");
	cute::makeRunner(lis)(Cute_NSF_Command_Handler::make_suite_Cute_NSF_Command_Handler(),"Running Command Handler Suite\n");
//	Cute_ACS_NSF_Server::vNsfServerCleanup();
//	Cute_ACS_NSF_Common::vNsfCommonCleanup();
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



