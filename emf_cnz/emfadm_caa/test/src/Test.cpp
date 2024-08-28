#include "cute_acs_emf_nanousbhandler.h"
#include "cute_acs_emf_common.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include<iostream>
using namespace std;


void thisIsATest() {
	ASSERTM("start writing tests", false);
}

void runSuite()
{

	TestSuite suite;
	suite.init();

	cute::suite s;
	//TODO add your test here
	//s.push_back(CUTE(TestSuite::serviceStartupCheck));
	//s.push_back(CUTE(TestSuite::formatAndMountnanoUSB));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForToUSB));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForEraseAndCopyToUSB));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForFromUSB));
	//s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo));
	//s.push_back(CUTE(TestSuite::umountUSBData));

                 s.push_back(CUTE(TestSuite::mediaOperationCheckForToUSB));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForMCToUSB));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForMCToUSBINT));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForToUSBINT));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForFromUSB));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForFromUSBINT));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForEraseAndCopyToUSBINT));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForEraseAndCopyToUSB));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo));
		 s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfocmd));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfoINT));
                 //s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo1));
                 s.push_back(CUTE(TestSuite::formatAndMountnanoUSB));
                 s.push_back(CUTE(TestSuite::umountUSBData));
                 s.push_back(CUTE(TestSuite::serviceStartupCheck));
                 s.push_back(CUTE(TestSuite::mediaOperationCheckForMediaInfo));
		 s.push_back(CUTE(TestSuite::checksumForActive));


    // Checking the acs_emf_common class method
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
	//cute::makeRunner(lis)(cute_acs_emf_common::make_suite_Cute_EMF_Common(),"Running Common methods");
	suite.destroy();
}

int main(){
    runSuite();
}





