#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "cute_lm_common.h"
#include <string>
using namespace std;

bool cute_lm_common::ExecuteCommand(string cmd)
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
void cute_lm_common::isDirBasicTest()
{
	ExecuteCommand("rm -rf /data/ftpvol/lm"); // removing all directories in ftpvol path
	ExecuteCommand("mkdir -p /data/ftpvol/lm/testDir");
	std::string path("/data/ftpvol/lm/testDir");
	bool myresult = ACS_LM_Common::isDir(path);
	ASSERTM("##isDirBasicTest Test case fails##", myresult);
}

void cute_lm_common::createLMDirectoryBasicTest()
{
	ExecuteCommand("rm -rf /data/acs/data/lm");
	std::string myStr("/data/acs/data/lm");
    bool myResult = ACS_LM_Common::createLmDir(myStr);
	ASSERTM("##createLMDirectoryBasicTest Test case Failed##",myResult);
}

void cute_lm_common::getFileTypeBasicTest()
{
	ExecuteCommand("touch /cluster/test1.txt");
	std::string myStr("/data/acs/data/lm");
	ACS_LM_FileType myFileType = ACS_LM_Common::getFileType("/cluster/test1.txt");
	bool myResult = false;
	myResult = (myFileType == ACS_LM_FILE_ISFILE);
	ASSERTM("##createLMDirectoryTest checking file Test case Failed##",myResult);

	createLMDirectoryBasicTest();
	myFileType = ACS_LM_Common::getFileType("/data/acs/data/lm");
	myResult = (myFileType == ACS_LM_FILE_ISDIR);
	ASSERTM("##getFileTypeBasicTest checking directory Test case Failed##",myResult);
}



void cute_lm_common::nodeStatusBasicTest()
{
	unsigned int myNodeStatus = ACS_LM_Common::nodeStatus();
	bool myResult = (myNodeStatus == 1);
	ASSERTM("##nodeStatusBasicTest Test case Failed##",myResult);
}

void cute_lm_common::getFullPathBasicTest()
{
	std::string myStr("/data/acs/data/lm");
	std::string myFullPathStr;
	myFullPathStr = ACS_LM_Common::getFullPath(myStr);
	bool myResult = (myFullPathStr == myStr);
	ASSERTM("##getFullPathBasicTest Full path given Test case Failed##",myResult);

	ExecuteCommand("cd /cluster");
	ExecuteCommand("rm testLM.txt");
	ExecuteCommand("touch testLM.txt");
	std::string myStr1("testLM.txt");
	std::string myFullPathStr1;
	myFullPathStr1 = ACS_LM_Common::getFullPath(myStr1);
	if(myFullPathStr1.compare("/cluster/testLM.txt"))
	{
		myResult = false;
	}
	else
	{
		myResult = true;
	}
	ASSERTM("##getFullPathBasicTest absolute path given Test case Failed##",myResult);
}

void cute_lm_common::restoreFileBasicTest()
{
	ExecuteCommand("cd /cluster");
	ExecuteCommand("touch /cluster/testSrc.txt");
	ExecuteCommand("ls > /cluster/testSrc.txt ");
	ExecuteCommand("touch /cluster/testDest.txt");

	std::string myStr("testLM.txt");
	std::string myFullPathStr;
	bool myResult = ACS_LM_Common::restoreFile("/cluster/testSrc.txt","/cluster/testDest.txt");

	ASSERTM("##restoreFileBasicTest Test case Failed##",myResult);
	myResult = ACS_LM_Common::restoreFile("/data/opt/ap/nbi/license_install/lservrc_test.txt","/cluster/testDest.txt");
	ASSERTM("##restoreFileBasicTest Test case Failed /data/opt/ap/nbi/license_install/lservrc_test.txt##",myResult);
}

void cute_lm_common::isFileExistsBasicTest()
{
	ExecuteCommand("touch /cluster/testFileExists.txt");
	std::string myPath("/cluster/testFileExists.txt");
	bool myResult = ACS_LM_Common::isFileExists(myPath);
	ASSERTM("##isFileExistsBasicTest Test case Failed##",myResult);
}

void cute_lm_common::deleteFileBasicTest()
{
	ExecuteCommand("touch /cluster/testFileDelete.txt");
	std::string myPath("/cluster/testFileDelete.txt");
	bool myResult = ACS_LM_Common::deleteFile(myPath);
	if(!myResult)
	{
		myResult = false;
	}
	ASSERTM("##deleteFileBasicTest Test case Failed##",myResult);
}

void cute_lm_common::backupFileBasicTest()
{
	ExecuteCommand("cd /cluster");
	ExecuteCommand("touch /cluster/testSrc.txt");
	ExecuteCommand("ls > /cluster/testSrc.txt ");
	ExecuteCommand("touch /cluster/testDest.txt");

	std::string myStr("testLM.txt");
	std::string myFullPathStr;
	bool myResult = ACS_LM_Common::restoreFile("/cluster/testSrc.txt","/cluster/testDest.txt");

	ASSERTM("##backupFileBasicTest Test case Failed##",myResult);
}

void cute_lm_common::fetchDnOfRootObjFromIMMBasicTest()
{
	OmHandler* theOmHandlerPtr = new OmHandler();
	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Init() Failed##",false);
	}
	int myValue = ACS_LM_Common::fetchDnOfRootObjFromIMM(theOmHandlerPtr);
	bool myResult = (myValue == 0);
	ASSERTM("##fetchDnOfRootObjFromIMMBasicTest Test case Failed##",myResult);
	if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Finalize() Failed##",false);
	}
	delete theOmHandlerPtr;
}


void cute_lm_common::generateFingerPrintBasicTest()
{
	OmHandler* theOmHandlerPtr = new OmHandler();
	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Init() Failed##",false);
	}
	std::string myFPString;
	bool myResult = ACS_LM_Common::generateFingerPrint(myFPString,theOmHandlerPtr);
	ASSERTM("##generateFingerPrintBasicTest Test case Failed##",myResult);
	if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Finalize() Failed##",false);
	}
	delete theOmHandlerPtr;
}

void cute_lm_common::getClusterIPAddressBasicTest()
{
	OmHandler* theOmHandlerPtr = new OmHandler();
	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Init() Failed##",false);
	}
	std::string myFPString;
	bool myResult = ACS_LM_Common::getClusterIPAddress(myFPString,theOmHandlerPtr);
	ASSERTM("##getClusterIPAddressBasicTest Test case Failed##",myResult);
	if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Finalize() Failed##",false);
	}
	delete theOmHandlerPtr;
}

void cute_lm_common::getNodeNameBasicTest()
{
	OmHandler* theOmHandlerPtr = new OmHandler();
	if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Init() Failed##",false);
	}
	std::string myFPString;
	bool myResult = ACS_LM_Common::getNodeName(myFPString,theOmHandlerPtr);
	ASSERTM("##getNodeNameBasicTest Test case Failed##",myResult);
	if( theOmHandlerPtr->Finalize() == ACS_CC_FAILURE )
	{
		ASSERTM("##theOmHandlerPtr->Finalize() Failed##",false);
	}
	delete theOmHandlerPtr;
}
void cute_lm_common::getlatestFileBasicTest()
{
	ExecuteCommand("rm -f /data/ftpvol/f*");
	ExecuteCommand("touch /data/ftpvol/f1.txt");
	sleep(3);
	ExecuteCommand("touch /data/ftpvol/f2.txt");
	sleep(5);
	ExecuteCommand("touch /data/ftpvol/f3.txt"); 

	std::string myLatestFile;

	bool myResult = ACS_LM_Common::getlatestFile("/data/ftpvol/",myLatestFile);

	//std::cout << "First call:" << myLatestFile << endl;

	ASSERTM("##getlatestFileBasicTest - getlatestFile Test case Failed##",myResult);

	myResult = false;

	if(myLatestFile.compare("f3.txt") == 0)
	{
		myResult = true;
	}


	ASSERTM("##getlatestFileBasicTest - getlatestFile for f3.txt Test case Failed##",myResult);
	myResult=false;
	sleep(1);
	ExecuteCommand("ls /data/ftpvol/ > /data/ftpvol/f2.txt ");

	//std::cout << "Second call for getlatestFile :" << myLatestFile << endl;
	myResult = ACS_LM_Common::getlatestFile("/data/ftpvol/",myLatestFile);


	ASSERTM("##getlatestFileBasicTest - getlatestFile Test case Failed##",myResult);

	myResult = false;

	if(myLatestFile.compare("f2.txt") == 0)
	{
		myResult = true;
	}

	ASSERTM("##getlatestFileBasicTest - getlatestFile f2.txt Test case Failed##",myResult);

	/*
	ExecuteCommand("rm -f /data/ftpvol/f*");
	myResult = ACS_LM_Common::getlatestFile("/data/ftpvol",myLatestFile);
	myResult = (myResult == true);
	ASSERTM("##getlatestFileBasicTest Test case Failed##",myResult); */
}



cute::suite cute_lm_common::make_suite_cute_lm_common()
{
	cute::suite s;

	s.push_back(CUTE(createLMDirectoryBasicTest));
	s.push_back(CUTE(getFileTypeBasicTest));
	s.push_back(CUTE(nodeStatusBasicTest));
	s.push_back(CUTE(getFullPathBasicTest));
	s.push_back(CUTE(restoreFileBasicTest));
	s.push_back(CUTE(isFileExistsBasicTest));
	s.push_back(CUTE(deleteFileBasicTest));
	s.push_back(CUTE(backupFileBasicTest));
	s.push_back(CUTE(fetchDnOfRootObjFromIMMBasicTest));
	s.push_back(CUTE(generateFingerPrintBasicTest));
	s.push_back(CUTE(getClusterIPAddressBasicTest));
	s.push_back(CUTE(getNodeNameBasicTest)); 
	s.push_back(CUTE(getlatestFileBasicTest));
	s.push_back(CUTE(isDirBasicTest));

	return s;
}


