#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_acs_chb_cpobject.h"


void thisIsATest() {
	ASSERTM("start writing tests", false);	
}

void runSuite()
{
	cute::ide_listener lis;

	CUTE_ACS_CHB_CP_object::init();

	cute::makeRunner(lis)(CUTE_ACS_CHB_CP_object::make_suite_CUTE_acs_chb_cp_object(), "Running Suite for ACS_CHB_CP_object\n\n");

	CUTE_ACS_CHB_CP_object::destroy();

}

int main(){
    runSuite();
}



