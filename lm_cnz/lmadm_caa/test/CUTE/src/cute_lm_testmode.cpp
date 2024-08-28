#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "acs_lm_persistent.h"
#include "acs_lm_testlkf.h"
#include "cute_lm_testmode.h"
#include <string>
using namespace std;

bool cute_lm_testmode::ExecuteCommand(string cmd)
{
        FILE *fp1,*fp2;
        char * data = new char[512];
        int status = -1;
        int ret = -1;
        bool bstatus = false;

        fp1 = popen(cmd.c_str(),"r");

        if (fp1 == NULL)
        {
                bstatus = false;
        }

        status = pclose(fp1);

        if (WIFEXITED(status))
        {
                ret=WEXITSTATUS(status);
        }

        if (ret == 0)
        {
                bstatus = true;
        }
        else
        {
                bstatus = false;
        }
        return bstatus;
}

bool cute_lm_testmode::getReturnStatus(bool retn)
{
	if( retn == 0)
	{
		retn = true;
	}	
	else
	{
		retn = false;
	}

	return retn;
}

void cute_lm_testmode::testmodeBasicTest()
{
	ExecuteCommand("rm -f /data/acs/data/lm/l*");
	ExecuteCommand("rm -f /cluster/etc/ap/acs/lm/l*");

	bool returnStatus;
        ACS_LM_Persistent* persistFile =ACS_LM_Persistent::load("/data/acs/data/lm/lmdata", "/cluster/etc/ap/acs/lm/lmdata","/cluster/etc/ap/acs/lm/lmdata", true); 
        ACS_LM_TestLkf* testlkf = new ACS_LM_TestLkf(persistFile);
	LkData lmData, lmData1;
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_NOK;
	cout << "Testcase1: To test the empty list first time for lkkey" << endl;
	std::list<LkData*> argList;
	returnStatus = testlkf->getTestLks(argList);
	ASSERTM("##checkTestKeysDuringStartup - getTestLks Test case Failed##",getReturnStatus(returnStatus));
	int totalLks = (int)argList.size();
	ASSERTM("##checkTestKeysDuringStartup Test case Failed - License keys not zero ##",totalLks==0);

	cout << "Testcase2: Adding 1 lkkey and checking list" << endl;
	lmData.lkId.assign("CXC001");
	lmData.paramName.assign("param");
	lmData.setName.assign("set");
	lmData.status = 1;
	lmData.value = 30;
	returnStatus = testlkf->addTestLk(lmData);
	ASSERTM("##addKeysBasicTest - addKeysBasic test case failed",getReturnStatus(returnStatus));
	returnStatus = testlkf->getTestLks(argList);
	ASSERTM("##checkTestKeysDuringStartup - getTestLks Test case Failed##",getReturnStatus(returnStatus));
	totalLks = (int)argList.size();
	ASSERTM("##getTestLksBasicTest - list size should be 1 test case failed",totalLks==1);

	cout << "Testcase3: Adding 2nd lkkey and checking list" << endl;
        lmData.lkId.assign("CXC002");
        lmData.paramName.assign("param1");
        lmData.setName.assign("set1");
        lmData.status = 1;
        lmData.value = 30;
        returnStatus = testlkf->addTestLk(lmData);
        ASSERTM("##addKeysBasicTest - addKeysBasic test case failed",getReturnStatus(returnStatus));

	argList.clear();
	cout << "ArgList size::" << argList.size() << endl;
        returnStatus = testlkf->getTestLks(argList);
        ASSERTM("##checkTestKeysDuringStartup - getTestLks Test case Failed##",getReturnStatus(returnStatus));
        totalLks = (int)argList.size();
        ASSERTM("##getTestLksBasicTest - list size should be 2 test case failed",totalLks==2);
	
}



cute::suite cute_lm_testmode::make_suite_cute_lm_testmode()
{
	cute::suite s;

	s.push_back(CUTE(testmodeBasicTest));

	return s;
}


