
#include<Cute_acs_logm_filetime.h>

cute_acs_logm_filetime::cute_acs_logm_filetime()
{
}

cute_acs_logm_filetime::~cute_acs_logm_filetime()
{
}

void cute_acs_logm_filetime::isFileObjectExists_unittest()
{

	std::string myDirPath = "/var/temp1/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_filetime myFiletime(myDirPath);
	bool myResult = myFiletime.isFileObjectExists(myDirPath);
	ASSERTM("isFileObjectExists_unittest Failed",(myResult == true));
}

void cute_acs_logm_filetime::isDirectory_unittest()
{

	std::string myDirPath = "/var/temp2/";
	std::string myCommand = "mkdir -p " + myDirPath;
	std::string myCommand_del = "rm -rf " + myDirPath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_filetime myFiletime(myDirPath);
	bool myResult = myFiletime.isDirectory();
	ASSERTM("isDirectory_unittest  Directory checking Failed",(myResult == true));



	std::string myFilePath = "/var/temp2/file1";
	myCommand = "touch " + myFilePath;
	myCommand_del = "rm " + myFilePath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	acs_logm_filetime myFiletime1(myFilePath);
	myResult = myFiletime1.isDirectory();
	ASSERTM("isDirectory_unittest File Checking Failed",(myResult == false));
}

void cute_acs_logm_filetime::isOlder_unittest()
{
	std::string myFilePath = "/var/temp2/file1";
	std::string myCommand = "touch " + myFilePath;
	std::string myCommand_del = "rm " + myFilePath;
	ExecuteCommand(myCommand_del);
	ExecuteCommand(myCommand);
	sleep(3);
	acs_logm_filetime myFiletime1(myFilePath);
	bool myResult = myFiletime1.isFileOlderThanGivenDays(1);
	ASSERTM("isOlder_unittest  isFileOlderThanGivenDays() Failed",(myResult == false));

	myResult = myFiletime1.isFileOlderThanGivenHours(1);
	ASSERTM("isOlder_unittest  isFileOlderThanGivenHours() Failed",(myResult == false));

	cout<<endl<<" Waiting for 1 minute"<<endl;
	cout.flush();
	sleep(60);
	myResult = myFiletime1.isFileOlderThanGivenMinutes(1);
	ASSERTM("isOlder_unittest  isFileOlderThanGivenMinutes() Failed",(myResult == true));

}


bool cute_acs_logm_filetime::ExecuteCommand(string cmd)
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

cute::suite cute_acs_logm_filetime::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_filetime::isFileObjectExists_unittest));
	s.push_back(CUTE(cute_acs_logm_filetime::isDirectory_unittest));
	s.push_back(CUTE(cute_acs_logm_filetime::isOlder_unittest));

	return s;
}



