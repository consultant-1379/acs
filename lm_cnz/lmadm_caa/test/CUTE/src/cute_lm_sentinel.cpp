#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "acs_lm_defs.h"
#include "cute_lm_sentinel.h"
#include <string>
using namespace std;
ACS_LM_Sentinel* CUTE_LM_Sentinel::theLMSentinel =0;
OmHandler*  theOMHandlerPtr;
//ACS_LM_ClientHandler* theClienthandler;
//ACS_LM_Persistent * theLMPersistent;

bool CUTE_LM_Sentinel::ExecuteCommand(string cmd)
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
bool CUTE_LM_Sentinel::initBasicTest()
{
	bool myResult = false;
	bool myInitialize;
    myResult = theLMSentinel->init(myInitialize,myInitialize,false,0);
	ASSERTM("##initBasicTest init() Failed##",myResult);		
}
bool CUTE_LM_Sentinel::fetchSidBasicTest()
{
	bool myResult = false;
	std::string mySID;
    myResult = theLMSentinel->fetchSid(mySID);
	ASSERTM("##fetchSidBasicTest fetchSid Failed##",myResult);		
}
bool CUTE_LM_Sentinel::generateFingerPrintBasicTest()
{
	bool myResult = false;
	myResult = theLMSentinel->generateFingerPrint();
	ASSERTM("##generateFingerPrintBasicTest  Failed##",myResult);		
}

bool CUTE_LM_Sentinel::installLkfAndfinishInstallationBasicTest()
{
	bool myResult = false;
	bool myInitialize = false;
	cout<<endl<<" Please make sure that two files are available in /cluster directory";
	cout<<endl<<" (1) lservrc_valid.txt - A valid LKF File (2)lservrc_invalid.txt - A invalid LKF File ";
	myResult = ExecuteCommand("ls /cluster | grep lservrc_valid.txt");
	myResult = ExecuteCommand("cp /cluster/lservrc_valid.txt /cluster/lservrc_valid1.txt ");
	myResult = ExecuteCommand("cp /cluster/lservrc_valid.txt /cluster/lservrc_valid2.txt ");
	myResult = ExecuteCommand("cp /cluster/lservrc_valid.txt /cluster/lservrc_valid3.txt ");
	myResult = ExecuteCommand("cp /cluster/lservrc_valid.txt /cluster/lservrc_valid4.txt ");
	myResult = ExecuteCommand("cp /cluster/lservrc_valid.txt /cluster/lservrc_valid5.txt ");
	
	ACS_LM_AppExitCode myRes = theLMSentinel->installLkf("/cluster/lservrc_test.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_OK);
	ASSERTM("##installLkfAndfinishInstallationBasicTest validLKF file  Failed##",myResult);	
	theLMSentinel->finishInstallation(true,myInitialize,myInitialize);
	myResult = ExecuteCommand("ls /data/acs/data/lm | grep lservrc.tmp");
	myResult = ( myResult == false);
	ASSERTM("##installLkfAndfinishInstallationBasicTest NO tmp file Failed##",myResult);
	
	myResult = false;
	myResult = ExecuteCommand("ls /cluster | grep lservrc_invalid.txt");
	ASSERTM("##installLkfAndfinishInstallationBasicTest Valid lservrc_invalid.txt File not available -  Failed##",myResult);		
	myResult = false;
	myResult = theLMSentinel->installLkf("/cluster/lservrc_invalid.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_INVALIDLKF);
	ASSERTM("##installLkfAndfinishInstallationBasicTest InvalidLKF file Failed##",myResult);	
	theLMSentinel->finishInstallation(false,myInitialize,myInitialize);
	myResult = ExecuteCommand("ls /data/acs/data/lm | grep lservrc.tmp");
	myResult = ( myResult == false);
	ASSERTM("##installLkfAndfinishInstallationBasicTest NO tmp file Failed##",myResult);

	myRes = theLMSentinel->installLkf("/cluster/lservrc_noexist.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_PHYFILEERROR);
	ASSERTM("##installLkfAndfinishInstallationBasicTest No exists file Failed##",myResult);	

}
bool CUTE_LM_Sentinel::isTestModeAllowedBasicTest()
{
	bool myResult = false;
	bool myInitialize;
	myResult = ExecuteCommand("ls /cluster | grep lservrc_valid2.txt");
	ASSERTM("##isTestModeAllowedBasicTest Valid lservrc_valid2.txt File not available -  Failed##",myResult);		
	ACS_LM_AppExitCode myRes = theLMSentinel->installLkf("/cluster/lservrc_valid2.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_OK);
	ASSERTM("##isTestModeAllowedBasicTest installLkf  Failed##",myResult);	

	myResult = theLMSentinel->isTestModeAllowed();
	ASSERTM("##isTestModeAllowedBasicTest isTestModeAllowed  Failed##",myResult);	
}
bool CUTE_LM_Sentinel::isEmergencyKeyValidBasicTest()
{
	bool myResult = false;
	bool myInitialize;
	myResult = ExecuteCommand("ls /cluster | grep lservrc_valid3.txt");
	ASSERTM("##isEmergencyKeyValidBasicTest Valid lservrc_valid3.txt File not available -  Failed##",myResult);		
	ACS_LM_AppExitCode myRes = theLMSentinel->installLkf("/cluster/lservrc_valid3.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_OK);
	ASSERTM("##isEmergencyKeyValidBasicTest installLkf  Failed##",myResult);	
	myResult = false;
	myResult = theLMSentinel->isEmergencyModeAllowed();
	ASSERTM("##isEmergencyKeyValidBasicTest isEmergencyModeAllowed  Failed##",myResult);	
	myResult = false;
	myResult = theLMSentinel->isEmergencyKeyValid();
	ASSERTM("##isEmergencyKeyValidBasicTest isEmergencyKeyValid  Failed##",myResult);	
}
bool CUTE_LM_Sentinel::getKeysFromLservrcBasicTest()
{
	bool myResult = false;
	bool myInitialize;
	myResult = ExecuteCommand("ls /cluster | grep lservrc_valid4.txt");
	ASSERTM("##isEmergencyKeyValidBasicTest Valid lservrc_valid4.txt File not available -  Failed##",myResult);		
	ACS_LM_AppExitCode myRes = theLMSentinel->installLkf("/cluster/lservrc_valid4.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_OK);
	ASSERTM("##getKeysFromLservrcBasicTest installLkf Failed##",myResult);	
	myResult = false;
	std::vector<ACS_LM_Sentinel::Key> lkeys;
	theLMSentinel->getKeysFromLservrc("/data/acs/data/lm/lservrc",lkeys);
	int myCount = 0;
	std::vector<ACS_LM_Sentinel::Key>::iterator myKeyIter = lkeys.begin();
	std::vector<ACS_LM_Sentinel::Key>::iterator myKeyIterEnd = lkeys.end();
	for(;myKeyIter != myKeyIterEnd;myKeyIter++)
	{
		myCount++;
	}
	myResult = (myCount > 0);
	ASSERTM("##getKeysFromLservrcBasicTest  Failed##",myResult);	

	myResult = false;
	theLMSentinel->getKeysFromLservrc("/data/acs/data/lm/",lkeys);
	myCount = 0;
	myKeyIter = lkeys.begin();
	myKeyIterEnd = lkeys.end();
	for(;myKeyIter != myKeyIterEnd;myKeyIter++)
	{
		myCount++;
	}
	myResult = (myCount == 0);
	ASSERTM("##getKeysFromLservrcBasicTest  Failed##",myResult);	
	
}
bool CUTE_LM_Sentinel::getAllLksBasicTest()
{
	bool myResult = false;
	bool myInitialize;
	myResult = ExecuteCommand("ls /cluster | grep lservrc_valid5.txt");
	ASSERTM("##getAllLksBasicTest Valid lservrc_valid5.txt File not available -  Failed##",myResult);		
	ACS_LM_AppExitCode myRes = theLMSentinel->installLkf("/cluster/lservrc_valid5.txt",myInitialize,myInitialize);
	myResult = (myRes == ACS_LM_RC_OK);
	ASSERTM("##getAllLksBasicTest installLkf Failed##",myResult);	
	myResult = false;
	int daysToExpire = 0;
	//std::vector<ACS_LM_Sentinel::Key> lkeys;
	std::list<LkData*> mylklist;
	theLMSentinel->getAllLks(mylklist,daysToExpire);
	int myCount = 0;
	//std::vector<ACS_LM_Sentinel::Key>::iterator myKeyIter = mylklist.begin();
	//std::vector<ACS_LM_Sentinel::Key>::iterator myKeyIterEnd = mylklist.end();
	std::list<LkData*>::iterator myKeyIter = mylklist.begin();
	std::list<LkData*>::iterator myKeyIterEnd = mylklist.end();

	for(;myKeyIter != myKeyIterEnd;myKeyIter++)
	{
		myCount++;
	}
	myResult = (myCount > 0);
	ASSERTM("##getAllLksBasicTest  Failed##",myResult);		
}
//bool CUTE_LM_Sentinel::fetchParameterIMM(std::string aRDN,std::string aParameterName,std:string & aParameterValue)
//{
//	aParameterValue = "";
//	theOmHandlerPtr = new OmHandler();
//	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
//	{
//		ASSERTM("##theOmHandlerPtr->Init() Failed##",false);
//	}
//	ACS_CC_ImmParameter paramToFind;
//
//	paramToFind.attrName = (char*)aParameterName.c_str();
//	if(theOmHandlerPtr->getAttribute( aRDN.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
//	{
//		char* pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
//		if(ACE_OS::strcmp(pszAttrValue , "") != 0)
//		{
//			std::string myStr(pszAttrValue);
//			aParameterValue= myStr;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	else
//	{//ERROR :Not able to read the parameter from the IMM
//			return false;
//	}
//	if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
//	{
//		ASSERTM("##theOmHandlerPtr->Finalize() Failed##",false);
//	}
//	delete theOmHandlerPtr;
//	return true;
//}


cute::suite CUTE_LM_Sentinel::make_suite_cute_lm_sentinel()
{
	theLMSentinel = new ACS_LM_Sentinel("/data/acs/data/lm/lservrc","/cluster/etc/ap/acs/lm/lservrc","/cluster/etc/ap/acs/lm/lservrc");
	theOMHandlerPtr =  new OmHandler();
	theLMSentinel->setInternalOMhandler(theOMHandlerPtr);
	
	cute::suite s;


	//s.push_back(CUTE(initandFinalizeLMRunTimeOwnerBasicTest));
	s.push_back(CUTE(initBasicTest));
	s.push_back(CUTE(fetchSidBasicTest));
	s.push_back(CUTE(generateFingerPrintBasicTest));
	s.push_back(CUTE(installLkfAndfinishInstallationBasicTest));
	s.push_back(CUTE(isTestModeAllowedBasicTest));
	s.push_back(CUTE(isEmergencyKeyValidBasicTest));
	s.push_back(CUTE(getKeysFromLservrcBasicTest));
	s.push_back(CUTE(getAllLksBasicTest));
	
	return s;
}


