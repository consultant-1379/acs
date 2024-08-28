#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_CHB_mtzconv.h"
#include "CUTE_ACS_CHB_filehandler.h"
#include "CUTE_ACS_CHB_Threadhandler.h"


void thisIsATest() {
	ASSERTM("start writing tests", false);	
}

void runSuite()
{
	cute::ide_listener lis;

	CUTE_ACS_CHB_mtzconv::init();

	cute::makeRunner(lis)(CUTE_ACS_CHB_mtzconv::make_suite_CUTE_ACS_CHB_mtzconv(), "Running Suite for ACS_CHB_mtzconv\n\n");

	CUTE_ACS_CHB_mtzconv::destroy();


	CUTE_ACS_CHB_filehandler::init();

	cute::makeRunner(lis)(CUTE_ACS_CHB_filehandler::make_suite_CUTE_ACS_CHB_filehandler(), "Running Suite for ACS_CHB_filehandler\n\n");

	CUTE_ACS_CHB_filehandler::destroy();


	CUTE_ACS_CHB_Threadhandler::init();

	CUTE_ACS_CHB_Threadhandler::destroy();

}

int main(){
    runSuite();
}



