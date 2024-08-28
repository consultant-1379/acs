
#include "test_swupdate.h"
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
	//s.push_back(CUTE(thisIsATest));
	s.push_back(CUTE(TestSuite::testSoftwareCompatibility));
	s.push_back(CUTE(TestSuite::testCampaignVerification));
	s.push_back(CUTE(TestSuite::SDPCheckSumFileCreation));
	s.push_back(CUTE(TestSuite::testPrintDownloadPath));
	//s.push_back(CUTE(TestSuite::testCleanDownloadFolder));
	s.push_back(CUTE(TestSuite::testGet_SrcSdpList));
	s.push_back(CUTE(TestSuite::testCpTo_APGRepository));
	//s.push_back(CUTE(TestSuite::testCoreMW_Cleanup));
	//s.push_back(CUTE(TestSuite::testCoreMW_Import));
	s.push_back(CUTE(TestSuite::testcheckIntegrityCompatibility));
	s.push_back(CUTE(TestSuite::testfcscPrepareWoOption));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
	suite.destroy();
}

int main(){
    runSuite();
}





