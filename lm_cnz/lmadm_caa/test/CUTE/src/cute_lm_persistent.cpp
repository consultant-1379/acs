#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "cute_lm_persistent.h"
#include"acs_lm_persistent.h"
#include <string>
#include <stdio.h>
using namespace std;

ACS_LM_Persistent* CUTE_LM_Persistent::theLMPersistent = 0;
bool CUTE_LM_Persistent::ExecuteCommand(string cmd)
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
	else
		{
			bstatus = true;
		}

	status = pclose(fp1);

//	if (WIFEXITED(status))
//	{
//		ret=WEXITSTATUS(status);
//	}
//	cout<<"The output of WEXITSTATUS function is :"<<ret<<endl;
//	if (ret == 0)
//	{
//
//		bstatus = true;
//	}
//	else
//	{
//		bstatus = false;
//	}
	return bstatus;
}
bool CUTE_LM_Persistent::setLmModeBasicTest()
{
	bool myResult = false;
	getLMPersistent()->setLmMode(ACS_LM_MODE_GRACE,true);
	if(getLMPersistent()->getLmMode() == ACS_LM_MODE_GRACE)
	{
		myResult= true;
	}
	ASSERTM("##setLmModeBasicTest Setting GRACE MODE Failed##",myResult);
	if(getLMPersistent()->getLkfChecksum() == 0)
	{
		myResult= true;
	}
	ASSERTM("##setLmModeBasicTest LKFSum in Gracemode must be Zero Failed##",myResult);
	

	getLMPersistent()->setLmMode(ACS_LM_MODE_EMERGENCY,true);
	if(getLMPersistent()->getLmMode() == ACS_LM_MODE_EMERGENCY)
	{
		myResult= true;
	}
	ASSERTM("##setLmModeBasicTest Setting Emergency MODE Failed##",myResult);
}

bool CUTE_LM_Persistent::loadPersistentFileBasicTest()
{
	bool myResult = false;
	ACS_LM_Persistent* myPtr = getLMPersistent()->load("","","",false);
	cout<<"The return value for myPtr is : "<<myPtr<<endl;
	myResult == (myPtr == 0);
	cout<<"The return code for load is : "<<myResult<<endl;
	ASSERTM("##loadPersistentFileBasicTest empty lmdata file given Failed##",myResult == false);
	myResult = false;
	ExecuteCommand("rm /data/acs/data/lm/lmdata");
	ExecuteCommand("rm /cluster/etc/ap/acs/lm/lmdata");
	myPtr = getLMPersistent()->load("/data/acs/data/lm/lmdata","/cluster/etc/ap/acs/lm/lmdata","/cluster/etc/ap/acs/lm/lmdata",true);
	myResult == (myPtr != 0);
	theLMPersistent =myPtr;
	cout<<"The return value for myPtr is : "<<myPtr<<endl;
	cout<<"The return code for load is : "<<myResult<<endl;
	ASSERTM("##loadPersistentFileBasicTest load during virgin mode Failed##",myResult ==0);
	myResult = false;
	//getLMPersistent()->setLmMode(ACS_LM_MODE_GRACE,true);
	int myMode= getLMPersistent()->getLmMode();
	printf ("The output of the getLmMode is :%d",myMode);
	if(getLMPersistent()->getLmMode() == 0)
	{
		cout<<"Inside getLmMode "<<endl;
		myResult= true;
	}
	cout<<"The return code for load is : "<<myResult<<endl;
	ASSERTM("##loadPersistentFileBasicTest force load leads to virgin mode Failed##",myResult);

}
bool CUTE_LM_Persistent::commitFileBasicTest()
{
	bool myResult = false;
	loadPersistentFileBasicTest();

	myResult = getLMPersistent()->commit();
	cout<<"The return code for the commit() is :"<<myResult<<endl;
	ASSERTM("##loadPersistentFileBasicTest commit() Failed##",myResult);
	myResult = false;
	myResult = ExecuteCommand("ls  /data/acs/data/lm|grep -i lmdata");
	cout<<"The return code for the ExecuteCommand() is :"<<myResult<<endl;;
	ASSERTM("##commitFileBasicTest lmdata saving to disk Failed##",myResult);
	myResult = false;
	ExecuteCommand("rm /cluster/etc/ap/acs/lm/lmdata");
	myResult = getLMPersistent()->backup();
	ASSERTM("##backupFileBasicTest backup() Failed##",myResult);
	myResult = false;
	myResult = ExecuteCommand("ls  /cluster/etc/ap/acs/lm | grep -i lmdata");
	ASSERTM("##backupFileBasicTest lmdata saving to /cluster/etc/ap/acs/lm Failed##",myResult);
	myResult = getLMPersistent()->restore();
	ASSERTM("##restoreFileBasicTest restore() Failed##",myResult);
	myResult = false;
	myResult = ExecuteCommand("ls  /data/acs/data/lm | grep -i lmdata");
	ASSERTM("##restoreFileBasicTest lmdata saving to /data/acs/data/lm Failed##",myResult);
}
bool CUTE_LM_Persistent::backupFileBasicTest()
{
	bool myResult = false;
	loadPersistentFileBasicTest();
	ExecuteCommand("rm /cluster/etc/ap/acs/lm/lmdata");
	myResult = getLMPersistent()->backup();
	cout<<"The return code for the backup() is :"<<myResult<<endl;
	ASSERTM("##backupFileBasicTest backup() Failed##",myResult);
	myResult = false;
	myResult = ExecuteCommand("ls  /cluster/etc/ap/acs/lm | grep -i lmdata");
	ASSERTM("##backupFileBasicTest lmdata saving to /cluster/etc/ap/acs/lm Failed##",myResult);
	
}
bool CUTE_LM_Persistent::restoreFileBasicTest()
{
	bool myResult = false;
	loadPersistentFileBasicTest();
	ExecuteCommand("rm /data/acs/data/lm/lmdata");
	myResult = getLMPersistent()->restore();
	ASSERTM("##restoreFileBasicTest restore() Failed##",myResult);
	myResult = false;
	myResult = ExecuteCommand("ls  /data/acs/data/lm | grep -i lmdata");
	ASSERTM("##restoreFileBasicTest lmdata saving to /data/acs/data/lm Failed##",myResult);
	
}

bool CUTE_LM_Persistent::deletePersistentFileBasicTest()
{
	bool myResult = false;
	loadPersistentFileBasicTest();
	getLMPersistent()->deletePersistentFile();
	myResult = false;
	myResult = ExecuteCommand("ls  /data/acs/data/lm | grep -i lmdata");
	cout<<"The return code for the deletePersistentFile1 is :"<<myResult<<endl;
	myResult = (myResult == false);
	cout<<"The return code for the deletePersistentFile is :"<<myResult<<endl;
	ASSERTM("##commitFileBasicTest deletion of  /data/acs/data/lm/lmdata Failed##",myResult);
	
}


//bool CUTE_LM_Persistent::fetchParameterIMMTest(std::string aRDN,std::string aParameterName,std:string & aParameterValue)
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
//	//Fetch the ClusterIPAddress from the IMM
//	if(theOmHandlerPtr->getAttribute( aRDN.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
//	{
//		char* pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
//		//Check the fetched IPAddress is not NULL
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


cute::suite CUTE_LM_Persistent::make_suite_cute_lm_persistent()
{

	theLMPersistent = new ACS_LM_Persistent();
	cute::suite s;

	s.push_back(CUTE(setLmModeBasicTest));
	s.push_back(CUTE(loadPersistentFileBasicTest));
	s.push_back(CUTE(commitFileBasicTest));
	//s.push_back(CUTE(backupFileBasicTest));
	//s.push_back(CUTE(restoreFileBasicTest));
	s.push_back(CUTE(deletePersistentFileBasicTest));
	return s;
}


