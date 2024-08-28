
#include<Cute_acs_logm_base.h>

cute_acs_logm_base::cute_acs_logm_base()
{
}

cute_acs_logm_base::~cute_acs_logm_base()
{
}

void cute_acs_logm_base::isFileObjectDirectory_isDirectoryEmpty_unittest()
{

	std::string myDirPath = "/var/temp1/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_base myBase;
	bool myResult = myBase.isFileObjectDirectory(myDirPath);
	ASSERTM("isFileObjectDirectory_unittest Directory checking Failed",(myResult == true));

	myResult = myBase.isDirectoryEmpty(myDirPath);
	ASSERTM("isFileObjectDirectory_unittest isDirectoryEmpty checking Failed",(myResult == true));


	std::string myFilePath = "/var/temp1/file1";
	myCommand = "touch " + myFilePath;
	myCommand_del = "rm " + myFilePath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_base myBase1;
	myResult = myBase1.isFileObjectDirectory(myFilePath);
	ASSERTM("isFileObjectDirectory_unittest File checking Failed" ,(myResult == false));

	myResult = myBase1.isDirectoryEmpty(myDirPath);
    ASSERTM("isFileObjectDirectory_unittest isDirectoryEmpty with File checking Failed",(myResult == false));


}

void cute_acs_logm_base::deletedirectory_unittest()
{

	std::string myDirPath = "/var/temp1/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_base myBase;
	bool myResult = myBase.deletedirectory(myDirPath);
	ASSERTM("deletedirectory_unittest delete directory Failed",(myResult == true));

	myResult = myBase.isDirectoryEmpty(myDirPath);
	ASSERTM("deletedirectory_unittest checking presence of deleted directory Failed",(myResult == false));

}

void cute_acs_logm_base::deleteEmptydirectory_unittest()
{

	std::string myDirPath = "/var/temp1/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_base myBase;
	bool myResult = myBase.deleteEmptydirectory(myDirPath);
	ASSERTM("deleteEmptydirectory_unittest delete Empty directory Failed",(myResult == true));

	myResult = myBase.isDirectoryEmpty(myDirPath);
	ASSERTM("deleteEmptydirectory_unittest checking presence of deleted directory Failed",(myResult == false));

	std::string myFilePath = "/var/temp1/f1.txt";
	std::string myCommand_create = "mkdir -p " + myDirPath;
	myCommand = "touch " + myFilePath;
	ExecuteCommand(myCommand_create);
	ExecuteCommand(myCommand);
	acs_logm_base myBase1;
	myResult = myBase1.deleteEmptydirectory(myDirPath);
	ASSERTM("deleteEmptydirectory_unittest checking deletion of Empty directory with file Failed",(myResult == false));


}
void cute_acs_logm_base::getElapsedTimeSincelastModified_unittest()
{
	std::string myDirPath = "/var/temp1/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_base myBase;
	cout<<endl<<" Waiting for 1 minute";
	cout.flush();
	sleep(60);
	double elaptime = 0;
	bool myResult = myBase.getElapsedTimeSincelastModified(myDirPath,elaptime);
	ASSERTM("getElapsedTimeSincelastModified_unittest  Failed",(elaptime >= 60));

}

void cute_acs_logm_base::deletefile_unittest()
{
	std::string myDirPath = "/var/temp1/";
	std::string myFilePath = "/var/temp1/f1.txt";
	std::string myCommand_create = "mkdir -p " + myDirPath;
	std::string myCommand = "touch " + myFilePath;
	ExecuteCommand(myCommand_create);
	ExecuteCommand(myCommand);
	acs_logm_base myBase1;
	bool myResult = myBase1.deleteFile(myFilePath);
	ASSERTM("deletefile_unittest checking deletion of file Failed",(myResult == true));

}

bool cute_acs_logm_base::ExecuteCommand(string cmd)
{
	FILE *fp1;
	//char * data = new char[512];
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

cute::suite cute_acs_logm_base::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_base::isFileObjectDirectory_isDirectoryEmpty_unittest));
	s.push_back(CUTE(cute_acs_logm_base::deletedirectory_unittest));
	s.push_back(CUTE(cute_acs_logm_base::deleteEmptydirectory_unittest));
	s.push_back(CUTE(cute_acs_logm_base::getElapsedTimeSincelastModified_unittest));
	s.push_back(CUTE(cute_acs_logm_base::deletefile_unittest));

	return s;

}





