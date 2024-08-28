
#include<Cute_acs_logm_logfile.h>
//#include<acs_logm_configurationdata.h>

cute_acs_logm_logfile::cute_acs_logm_logfile()
{
}

cute_acs_logm_logfile::~cute_acs_logm_logfile()
{
}

void cute_acs_logm_logfile::addDaysToCurrentDate(int aNumberDays)
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec += 86400 * aNumberDays;
	settimeofday(&tv, &tz);

}

void cute_acs_logm_logfile::cleanup_unittest()
{

	/* START -- TESTING TRA LOG DELETION*/
	std::string myCommand_del = "";

	myCommand_del =  "touch /var/log/acs/tra/logging/f1.log";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch /var/log/acs/tra/logging/f2.log";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch /var/log/acs/tra/logging/f3.log";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch /var/log/acs/tra/logging/f4.log";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch /var/log/acs/tra/logging/f5.log";
	ExecuteCommand(myCommand_del);

	myCommand_del =  "touch /var/log/acs/tra/logging/f11.gz";
	ExecuteCommand(myCommand_del);

	addDaysToCurrentDate(8);

	acs_logm_configurationdata theConfigData;

	theConfigData.theNumberofOldDays = 7;
	theConfigData.theNumberofFilesToBeMaintained = 0;
	theConfigData.theInitiationHour = 0;
	theConfigData.theInitiationMinute = 0;
	theConfigData.theCleanUpFrequency = acs_logm_configurationdata::DAILY;
	theConfigData.thePeriodicInterval = 0;
	theConfigData.theCleaningEnabled = true;
	theConfigData.theCleanupInitiation = acs_logm_configurationdata::TIMEBASED;
	theConfigData.theCleanFileObject = acs_logm_configurationdata::FILE;
	theConfigData.theFolderPath = "/var/log/acs/tra/logging/";
	theConfigData.theSearchPattern = ".log";

	acs_logm_logfile myLogFile(theConfigData);
	myLogFile.houseKeepingAction();

	bool myRes = acs_logm_filetime::isFileObjectExists("/var/log/acs/tra/logging/f11.gz");
	ASSERTM("cleanup_unittest Failed - LOGM cleaning other files in TRA folder",myRes==true);

	myRes = acs_logm_filetime::isFileObjectExists("/var/log/acs/tra/logging/f1.log");
	ASSERTM("cleanup_unittest Failed - LOGM Failed to clean older *.log files in TRA folder",myRes==false);



	myCommand_del =  "touch /var/log/acs/tra/logging/f5.log";
	ExecuteCommand(myCommand_del);

	addDaysToCurrentDate(4);

	myLogFile.houseKeepingAction();

	myRes = acs_logm_filetime::isFileObjectExists("/var/log/acs/tra/logging/f5.log");
	ASSERTM("cleanup_unittest Failed - LOGM is cleaning younger *.log files in TRA folder",myRes==true);

	/* END -- TESTING TRA LOG DELETION*/

	/* START -- TESTING SCX LOG DELETION*/


		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f1.gz";
		ExecuteCommand(myCommand_del);
		sleep(1);
		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f2.gz";
		ExecuteCommand(myCommand_del);
		sleep(1);
		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f3.gz";
		ExecuteCommand(myCommand_del);
		sleep(1);
		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f4.gz";
		ExecuteCommand(myCommand_del);
		sleep(1);
		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f5.gz";
		ExecuteCommand(myCommand_del);
		sleep(1);
		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f11.log";
		ExecuteCommand(myCommand_del);

		addDaysToCurrentDate(8);


		theConfigData.theNumberofOldDays = 7;
		theConfigData.theNumberofFilesToBeMaintained = 3;
		theConfigData.theInitiationHour = 0;
		theConfigData.theInitiationMinute = 0;
		theConfigData.theCleanUpFrequency = acs_logm_configurationdata::DAILY;
		theConfigData.thePeriodicInterval = 0;
		theConfigData.theCleaningEnabled = true;
		theConfigData.theCleanupInitiation = acs_logm_configurationdata::TIMEBASED;
		theConfigData.theCleanFileObject = acs_logm_configurationdata::FILE;
		theConfigData.theFolderPath = "/data/apz/data/boot/scx/scx_logs/";
		theConfigData.theSearchPattern = ".gz";

		acs_logm_logfile myLogFile1(theConfigData);
		myLogFile1.houseKeepingAction();

		myRes = acs_logm_filetime::isFileObjectExists("/data/apz/data/boot/scx/scx_logs/f11.log");
		ASSERTM("cleanup_unittest Failed - LOGM cleaning other files in SCX folder",myRes==true);

		myRes = acs_logm_filetime::isFileObjectExists("/data/apz/data/boot/scx/scx_logs/f5.gz");
		ASSERTM("cleanup_unittest Failed - LOGM is cleaning older *.gz beyond 3 files ",myRes==true);


		myCommand_del =  "touch /data/apz/data/boot/scx/scx_logs/f12.gz";
		ExecuteCommand(myCommand_del);

		addDaysToCurrentDate(4);

		myLogFile1.houseKeepingAction();

		myRes = acs_logm_filetime::isFileObjectExists("/var/log/acs/tra/logging/f1.gz");
		ASSERTM("cleanup_unittest Failed - LOGM is not cleaning older *.gz files ",myRes==false);

/* END -- TESTING SCX LOG DELETION*/
}

bool cute_acs_logm_logfile::ExecuteCommand(string cmd)
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

cute::suite cute_acs_logm_logfile::make_suite()
{
	cute::suite s;

	s.push_back(CUTE(cute_acs_logm_logfile::cleanup_unittest));


	return s;

}







