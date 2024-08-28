
#include<Cute_acs_logm_directory.h>


cute_acs_logm_directory::cute_acs_logm_directory()
{
}

cute_acs_logm_directory::~cute_acs_logm_directory()
{
}

void cute_acs_logm_directory::addDaysToCurrentDate(int aNumberDays)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec += 86400 * aNumberDays;
	settimeofday(&tv, &tz);

}

void cute_acs_logm_directory::cleanup_unittest()
{

	/* START -- TESTING TRA LOG DELETION*/
	std::string myCommand_del = "";

	myCommand_del =  "mkdir -p  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_1/";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_1/Dummy";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "mkdir -p  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_2/";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_2/Dummy";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "mkdir -p  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_3/";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "mkdir -p  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_4/";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch  /data/opt/ap/internal_root/data_transfer/source/sts/newtest_4/DummyStatus";
	ExecuteCommand(myCommand_del);


	addDaysToCurrentDate(2);

	acs_logm_configurationdata theConfigData;

	theConfigData.theNumberofOldDays = 3;
	theConfigData.theNumberofFilesToBeMaintained = 0;
	theConfigData.theInitiationHour = 0;
	theConfigData.theInitiationMinute = 0;
	theConfigData.theCleanUpFrequency = acs_logm_configurationdata::DAILY;
	theConfigData.thePeriodicInterval = 0;
	theConfigData.theCleaningEnabled = true;
	theConfigData.theCleanupInitiation = acs_logm_configurationdata::TIMEBASED;
	theConfigData.theCleanFileObject = acs_logm_configurationdata::FOLDER;
	theConfigData.theFolderPath = "/data/opt/ap/internal_root/data_transfer/source/sts/";
	theConfigData.theSearchPattern = "Dummy";

	acs_logm_directory myLogDirectory(theConfigData);
	myLogDirectory.houseKeepingAction();

	bool myRes = acs_logm_filetime::isFileObjectExists("/data/opt/ap/internal_root/data_transfer/source/sts/newtest_1/");
	ASSERTM("cleanup_unittest Failed - LOGM cleaning folders older than 2 days in STS MP",myRes==true);

	addDaysToCurrentDate(4);

	myLogDirectory.houseKeepingAction();

	myRes = acs_logm_filetime::isFileObjectExists("/data/opt/ap/internal_root/data_transfer/source/sts/newtest_2/");
	ASSERTM("cleanup_unittest Failed - LOGM Failed to cleaning folders older than 3 days in STS MP",myRes==false);

	myRes = acs_logm_filetime::isFileObjectExists("/data/opt/ap/internal_root/data_transfer/source/sts/newtest_3/");
	ASSERTM("cleanup_unittest Failed - LOGM Failed to cleaning Empty folders older than 3 days in STS MP",myRes==false);

	myRes = acs_logm_filetime::isFileObjectExists("/data/opt/ap/internal_root/data_transfer/source/sts/newtest_4/");
	ASSERTM("cleanup_unittest Failed - LOGM is cleaning non empty MP folder also STS MP",myRes==true);

	addDaysToCurrentDate(62);



	theConfigData.theNumberofOldDays = 60;
	theConfigData.theNumberofFilesToBeMaintained = 0;
	theConfigData.theInitiationHour = 0;
	theConfigData.theInitiationMinute = 0;
	theConfigData.theCleanUpFrequency = acs_logm_configurationdata::DAILY;
	theConfigData.thePeriodicInterval = 0;
	theConfigData.theCleaningEnabled = true;
	theConfigData.theCleanupInitiation = acs_logm_configurationdata::TIMEBASED;
	theConfigData.theCleanFileObject = acs_logm_configurationdata::FOLDER;
	theConfigData.theFolderPath = "/data/opt/ap/internal_root/data_transfer/source/sts/";
	theConfigData.theSearchPattern = ".*";

	acs_logm_directory myLogDirectory1(theConfigData);
	myLogDirectory1.houseKeepingAction();

	myRes = acs_logm_filetime::isFileObjectExists("/data/opt/ap/internal_root/data_transfer/source/sts/newtest_4/");
	ASSERTM("cleanup_unittest Failed - LOGM is not cleaning full MP folder that are 60 days old",myRes==false);

	/* END -- TESTING STS MP LOG DELETION*/

}

bool cute_acs_logm_directory::ExecuteCommand(string cmd)
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

cute::suite cute_acs_logm_directory::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_directory::cleanup_unittest));


	return s;

}







