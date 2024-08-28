/* inclusion of Cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "Cute_acs_logm_time.h"
#include "Cute_acs_logm_filetime.h"
#include "Cute_acs_logm_base.h"
#include "Cute_acs_logm_apglogmfactory.h"
#include "Cute_acs_logm_logfile.h"
#include "Cute_acs_logm_directory.h"

void runSuite()
{

	cute::ide_listener lis;
	cute::makeRunner(lis)(cute_acs_logm_time::make_suite(),"Running LOGM Time unit test");
	cute::makeRunner(lis)(cute_acs_logm_filetime::make_suite(),"Running LOGM File Time unit test");
	cute::makeRunner(lis)(cute_acs_logm_base::make_suite(),"Running LOGM Base unit test");
	cute::makeRunner(lis)(cute_acs_logm_apglogmfactory::make_suite(),"Running APG LOGM FACTORY unit test");
	cute::makeRunner(lis)(cute_acs_logm_logfile::make_suite(),"Running APG LOG FILE unit test");
	cute::makeRunner(lis)(cute_acs_logm_directory::make_suite(),"Running APG Directory unit test");

}/* end runSuite() */

int main()
{
    runSuite();
    return 0;
}





