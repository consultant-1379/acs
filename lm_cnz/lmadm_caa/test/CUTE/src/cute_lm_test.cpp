/**
 * @file cute_lm_test.cpp
 * It consists function definition to run the LM Basic test
 */

/* include section */
/* inclusion of ACE library*/
#include <ace/ACE.h>
#include "acs_lm_tra.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "cute_lm_common.h"
#include "cute_lm_testmode.h"

/**
 * @brief
 * Is a function to call the LM test suites
 * No arguments
 *@return void
 */
void run_ACS_LM_COMMON_Suite(){
	cout<<endl<<"---------------------------"<<endl;
	cout<<endl<<"acs_lm_common.cpp basic test"<<endl;
	cute::ide_listener lis;
	cute::makeRunner(lis)(cute_lm_common::make_suite_cute_lm_common(),"Running LM Basic Test Suite\n");
	cout<<endl<<"---------------------------"<<endl;
}


/**
 * @brief
 * main function to call the runSuite() function
 * @return int
 * returns 0
 */
int main()
{
	ACS_LM_Logging.Open("LM");
	run_ACS_LM_COMMON_Suite();

    return 0;
}
