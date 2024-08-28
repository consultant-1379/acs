/**
 * @file Cute_ACS_ACSC_Logmaint.cpp
 * @brief
 *Test functions ACS_ACSC_Logmaint are defined in this file
 */

/* INCLUDE SECTION */
/* Inclusion of Cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Cute_ACS_ACSC_Logmaint.h"

/*Inclusion of logm block header files */
#include <acs_logm_logmaint.h>
#include <acs_logm_svc_loader.h>
#include <acs_logm_types.h>
#include "acs_prc_api.h"

/* Inclusion of system memory library files */
#include <mntent.h>
#include <sys/statvfs.h>

int logcount;
ACS_PRC_API prcObj;

//Static variable declaration
ACS_ACSC_Logmaint * Cute_ACS_ACSC_Logmaint::m_poCleanup = 0;

/*======================================================
 * CONSTRUCTOR
 *====================================================== */
Cute_ACS_ACSC_Logmaint::Cute_ACS_ACSC_Logmaint()
{

}

void Cute_ACS_ACSC_Logmaint::vTestgetCurrentTime()
{
	system("date -s \"Tue Nov 2 04:39:35 IST 2010\"");
	bool bResult = false;
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	if((difftime(mktime(timeinfo),mktime(m_poCleanup->getCurrentTime())) == 0))
	bResult = true;
	else bResult = false;

	ASSERTM("Unable to get correct time from getCurrentTime() function",bResult==true);
}

/*======================================================
 * ROUTINE:vTestGetDiskMonitorSettings()
 *======================================================*/

void Cute_ACS_ACSC_Logmaint::vTestGetLogFileParams()
{
	cout<<endl<<"Inside vTestGetLogFileParams"<<endl;
	_ACS_LOGM_LOGFILESPEC ptrParamsLOGFILESPECTest;
	_ACS_LOGM_SERVICEPARAMS ptrParamsSERVICEPARAMStest;
	cout<<endl<<"Before  GetServiceparams"<<endl;
   int bResult = m_poCleanup->GetServiceparams(&ptrParamsSERVICEPARAMStest);
   cout<<endl<<"After  GetServiceparams"<<endl;
   cout<<endl<<"The return value of GetServiceparams"<<bResult<<endl;
	if (bResult == 0)
	{
		cout << "Error Occured while getting service parameters" << endl;
		return;
	}
	else
	{
		cout << "iLogTypCnt : " << ptrParamsSERVICEPARAMStest.iLogTypCnt<< endl;
		int iIndexTest = ptrParamsSERVICEPARAMStest.iLogTypCnt;
		logcount = iIndexTest;

		//Functions for testing
		for (int i = 1; i <= iIndexTest; ++i)
		{
			bResult = m_poCleanup->GetLogFileParams(&ptrParamsLOGFILESPECTest, i);

			if (!bResult)
			{
				bResult = false;
				break;
			}
			else
			{
				if (i == 7)
				{
					if (!(strcmp("/var/log/acs/tra",ptrParamsLOGFILESPECTest.strPath.c_str() ))	&& !(strcmp("TRALOG",ptrParamsLOGFILESPECTest.strPath_LOGNAME.c_str()))	&& (ptrParamsLOGFILESPECTest.iNoOFDays == 7))
					{
						bResult = TRUE;
					}
					else
					{
						bResult = FALSE;
					}
				}
			}
		}
	}

	ASSERTM("Unable to get LOGM Parameters from IMM", bResult == true);
} /* end vTestGetDiskMonitorSettings()*/

void Cute_ACS_ACSC_Logmaint::vTestGetServiceparams()
{

	_ACS_LOGM_SERVICEPARAMS ptrParamsSERVICEPARAMStest;
	bool bResult = FALSE;

	bResult = m_poCleanup->GetServiceparams(&ptrParamsSERVICEPARAMStest);
	cout<<endl<<"The value of bResult inside vTestGetServiceparams is :"<<bResult<<endl;
	if ((bResult == true) && (ptrParamsSERVICEPARAMStest.iLogTypCnt == 6) && (ptrParamsSERVICEPARAMStest.iStsTrg == 1))
	{
		bResult = true;
	}
	else
	{
		bResult = false;
		cout <<endl<<"Unable to get service trigger Parameters from IMM"<<endl;
		return ;
	}
	ASSERTM("Unable to get service trigger Parameters from IMM", bResult
			== true );
}

void Cute_ACS_ACSC_Logmaint::vGetInitialTimeToWait()
{
	_ACS_LOGM_SERVICEPARAMS ptrParamsSERVICEPARAMStest;
	bool bResult;
	double dSecondsToWaitTest = -1;
	system("date -s \"Tue Nov 2 04:39:35 IST 2010\"");
	ACE_INT32 serviceParamsStatus = m_poCleanup->GetServiceparams(&ptrParamsSERVICEPARAMStest);

	cout<<"Return value of GetServiceparams"<<serviceParamsStatus<<endl;
	if (serviceParamsStatus)
	{
		bResult = m_poCleanup->GetInitialTimeToWait(&ptrParamsSERVICEPARAMStest, &dSecondsToWaitTest);
		if (dSecondsToWaitTest != -1)
		{
			cout << "dSecondsToWaitTest :" << dSecondsToWaitTest << endl;
		}
		else
		{
			bResult = FALSE;
		}
	}
	else
	{
		cout << "Error in getting service parameters" << endl;
		bResult = FALSE;
	}

	ASSERTM("Unable to calculate initial waiting time ", bResult == true);
}

void Cute_ACS_ACSC_Logmaint :: vTestisDirectory() {

	bool status1, status2;
	bool bResult = false;
	system("mkdir -p /root/logm_test");
	system("touch  /root/logm_test.log");
	status1 = m_poCleanup->isDirectory("/root/logm_test");
	status2 = m_poCleanup->isDirectory("/root/logm_test.log");
	if( (status1 == true) && (status2 == false))
		bResult = true;
	ASSERTM("isDirectory function returned incorrect value",bResult==true);
	system("rm -rf /root/logm_test");
}

void Cute_ACS_ACSC_Logmaint::vTestElapsedTimeCal() {

	bool bResult = false;
	double elapsedTime1 = -1, elapsedTime2 = -1;
	time_t rawtime;
	struct tm *timeinfo1, *timeinfo2;
	time(&rawtime);
	timeinfo1 = localtime(&rawtime);
	elapsedTime1 = m_poCleanup->ElapsedTimeCal(timeinfo1);
	timeinfo2 = localtime(&rawtime);
	timeinfo2->tm_hour = timeinfo2->tm_hour + 1;
	elapsedTime2 = m_poCleanup->ElapsedTimeCal(timeinfo2);
	double absolute = fabs(elapsedTime2);


	if((elapsedTime1 == 0) && (absolute == 3600))
	{
		bResult = true;
	}

	ASSERTM("TestElapsedTimeCal() function not working properly",bResult==true);
}


void Cute_ACS_ACSC_Logmaint::vTestFindFileAndFolders() { /*to be implemented*/

	bool bResult = false;
	FILE *fp1, *fp2;

	_ACS_LOGM_LOGFILESPEC ptrParams;

	int iStsTrg = 1;
	ptrParams.strPath_LOGNAME = "TRALOG";
	//strcpy(ptrParams.strPath_LOGNAME, "TRALOG");
	ptrParams.strType = "*.log" ;
	//strcpy(ptrParams.strType, "*.log");
	ptrParams.iNoOFDays = 7;
	ptrParams.strPath = "/var/log/acs/tra/";
	//strcpy(ptrParams.strPath, "/var/log/acs/tra/");
	string sRelativePath = ptrParams.strPath;

	system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
	//system("mkdir -p /var/log/acs/tra/");
	system("touch /var/log/acs/tra/test.log");
	system("touch /var/log/acs/tra/test1.log1");

	m_poCleanup->FindFileAndFolders(sRelativePath.c_str(), &ptrParams, iStsTrg);
	fp1 = fopen("/var/log/acs/tra/test.log", "r");
	fp2 = fopen("/var/log/acs/tra/test1.log1", "r");

	if ((fp1) && (fp2)) {
		bResult = true;
		fclose(fp1);
		fclose(fp2);
	}

	ASSERTM(
			"FindFileAndFolders() function for TRA with less age not working properly",
			bResult == true);

	system("date -s \"Tue Nov 10 09:39:35 IST 2010\"");
	system("touch /var/log/acs/tra/test4.log");
	m_poCleanup->FindFileAndFolders(sRelativePath.c_str(), &ptrParams, iStsTrg);

	bResult = false;

	fp1 = fopen("/var/log/acs/tra/test.log", "r");
	fp2 = fopen("/var/log/acs/tra/test1.log1", "r");

	if ((fp1) || (fp2)) {
		bResult = false;
		fclose(fp1);
		fclose(fp2);
	} else {
		bResult = true;
	}

	ASSERTM(
			"FindFileAndFolders() function for TRA with old file not working properly",
			bResult == true);

}

void Cute_ACS_ACSC_Logmaint::vTestisWildCard() 
{ 

	bool bResult = false;
	string wildcardString1 = "/sat/MSFTPSVC*";
	string wildcardString2 = "/sat/MSFTPSVC/*.log";
	string wildcardString3 = "/sat/MSFTPSVC/logm.log";

	if((m_poCleanup->isWildCard(wildcardString1)))
	bResult = true;
	if((m_poCleanup->isWildCard(wildcardString2)))
	bResult = true;
	if((m_poCleanup->isWildCard(wildcardString3)))
	bResult = false;

	ASSERTM("Error in isWildCard() function",bResult==true);
}

void Cute_ACS_ACSC_Logmaint::vTestwildCardMatch() 
{

	bool bResult = true;
	string wildCardString1 = "*.log";
	string wildCardString2 = "/root/sat/MSFTPSVC*";
	string path1 = "/root/sat/logm.log";
	string path2 = "/root/sat/MSFTPS/logm.log";

	cout<<"Result : "<<m_poCleanup->wildCardMatch(wildCardString1,path1)<<endl;
	cout<<"Result : "<<m_poCleanup->wildCardMatch(wildCardString2,path2)<<endl;

	if(!(m_poCleanup->wildCardMatch(wildCardString1,path1)))
	bResult = false;
	if(!(m_poCleanup->wildCardMatch(wildCardString2,path2)))
	bResult = true;

	ASSERTM("wildCardMatch() function failed ",bResult==true);
}


#if 0
void Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesFTLOG() {

	bool bResult = false;
	FILE *fp1, *fp2;

	system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
	//system("mkdir -p /var/log");
	system("mkdir -p /var/log/MSFTPSVC");
	system("mkdir -p /var/log/MSFTPSVC1");
	system("touch /var/log/MSFTPSVC/test.log");
	system("chmod 777 /var/log/MSFTPSVC/test.log");
	system("touch /var/log/MSFTPSVC1/test1.log1");
	system("chmod 777 /var/log/MSFTPSVC1/test1.log1");

	m_poCleanup->DeleteLogFiles(7, 0, 0); //delete more than 7 days

	fp1 = fopen("/var/log/MSFTPSVC/test.log", "r");
	fp2 = fopen("/var/log/MSFTPSVC1/test1.log1", "r");

	if ((fp1) && (fp2)) {
		bResult = true;
		fclose(fp1);
		fclose(fp2);
	}

	ASSERTM(
			"DeleteLogFiles() function for FTLOG with less age not working properly",
			bResult == true);

	system("date -s \"Tue Nov 10 09:39:35 IST 2010\"");

	m_poCleanup->DeleteLogFiles(7, 0, 0);

	bResult = false;

	fp1 = fopen("/var/log/MSFTPSVC/test.log", "r");
	fp2 = fopen("/var/log/MSFTPSVC1/test1.log1", "r");

	if ((fp1) || (fp2)) {
		bResult = false;
		fclose(fp1);
		fclose(fp2);
	} else {
		bResult = true;
	}

	ASSERTM(
			"DeleteLogFiles() function for FTLOG with old file not working properly",
			bResult == true);

}
#endif

void Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesTRA() 
{
	bool bResult = false;
	FILE *fp1, *fp2;

	system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
	system("mkdir -p /var/log/acs/tra/");
	system("touch /var/log/acs/tra/test.log");
	system("touch /var/log/acs/tra/test1.log.1");

	m_poCleanup->DeleteLogFiles(7, 0, 0); //delete more than 7 days

	fp1 = fopen("/var/log/acs/tra/test.log", "r");
	fp2 = fopen("/var/log/acs/tra/test1.log.1", "r");

	if ((fp1) && (fp2)) {
		bResult = true;
		fclose(fp1);
		fclose(fp2);
	}

	ASSERTM(
			"DeleteLogFiles() function for TRA with less age: working properly",
			bResult == true);

	system("date -s \"Tue Nov 10 09:39:35 IST 2010\"");

	m_poCleanup->DeleteLogFiles(6, 0, 0);

	bResult = false;

	fp1 = fopen("/var/log/acs/tra/test.log", "r");
	fp2 = fopen("/var/log/acs/tra/test1.log.1", "r");

	if ((fp1) || (fp2)) {
		bResult = false;
		fclose(fp1);
		fclose(fp2);
	} else {
		bResult = true;
	}

	ASSERTM("DeleteLogFiles() function for TRA with old file is not working properly",bResult == true);

}

void Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesSTSMP() 
{ 


	bool bResult = false;
	bool fp1 = false, fp2 = false;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();
	if(nodeState == 1)
	{
		system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
		system("mkdir -p /data/sts/output");
		system("mkdir -p /data/sts/output/ststestMP");
		system("mkdir -p /data/sts/output/ststestMP1");
		system("touch /data/sts/output/ststestMP/status");
		system("touch /data/sts/output/ststestMP1/status");


		m_poCleanup->DeleteLogFiles(6, 0, 0);

		fp1 = m_poCleanup->isDirectory("/data/sts/output/ststestMP/");
		fp2 = m_poCleanup->isDirectory("/data/sts/output/ststestMP1/");

		if ((fp1) && (fp2)) {
			bResult = true;
		}

		ASSERTM(
				"DeleteLogFiles() function for STS (status) with less age not working properly",
				bResult == true);

		system("date -s \"Tue Jan 31 09:39:35 IST 2011\"");

		m_poCleanup->DeleteLogFiles(6, 0, 0); //delete more than 60 days
		bResult = false;

		fp1 = m_poCleanup->isDirectory("/data/sts/output/ststestMP/");
		fp2 = m_poCleanup->isDirectory("/data/sts/output/ststestMP1/");

		if ((fp1) || (fp2)) {
			bResult = false;
		} else {
			bResult = true;
		}
	}
	else
	{
		bResult = true;
	}

	ASSERTM(
			"DeleteLogFiles() function for STS (status) with less age not working properly",
			bResult == true);

}

void Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesDDI() 
{ 

	bool bResult = false;
	FILE *fp1, *fp2, *fp3;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();
	if(nodeState == 1)
	{

		system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
		system("mkdir -p /data/acs/data/bur/test");
		system("touch /data/acs/data/bur/test/test1.ddi");
		system("touch /data/acs/data/bur/test/test2.ddi");
		system("touch /data/acs/data/bur/test/test3.ddi");
		system("touch /data/acs/data/bur/test/test4.ddi");
		system("touch /data/acs/data/bur/test/test5.ddi");
		system("touch /data/acs/data/bur/test/test6.ddi");

		m_poCleanup->DeleteLogFiles(6, 0, 0); //delete more than 7 days

		fp1 = fopen("/data/acs/data/bur/test/test1.ddi", "r");
		fp2 = fopen("/data/acs/data/bur/test/test2.ddi", "r");
		fp3 = fopen("/data/acs/data/bur/test/test3.ddi", "r");

		if ((fp1) && (fp2) && (fp3))
		{
			cout<<endl<<"Able to read the created directory and file in vTestDeleteLogFilesDDI"<<endl;
			bResult = true;
		}

		system("date -s \"Tue Nov 6 08:39:35 IST 2010\"");

		m_poCleanup->DeleteLogFiles(6, 0, 0);
		fp1 = fopen("/data/acs/data/bur/test/test1.ddi", "r");
		fp2 = fopen("/data/acs/data/bur/test/test2.ddi", "r");
		fp3 = fopen("/data/acs/data/bur/test/test3.ddi", "r");
		if ((fp1) || (fp2) || (fp3))
		{
			bResult = false;
		}
		else
		{
			bResult = true;
		}

		ASSERTM(
				"DeleteLogFiles() function for STS (status) with less age not working properly",
				bResult == true);
	}
	else
	{
		cout<<"Node state is passive"<<endl;
		bResult = false;
	}
	ASSERTM("DeleteLogFiles() function for ddiRM working properly", bResult
			== true);

}

void Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesSTSMPDummy() 
{ 

	bool bResult = false;
	bool fp1 = false;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();
	if(nodeState == 1)
	{
		system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
		//system("mkdir -p /data/");
		//system("mkdir -p /data/STS/");
		//system("mkdir -p /data/STS/Data");
		system("mkdir -p /data/sts/data/ststestMP");
		system("touch /data/sts/data/ststestMP/Dummy");

		m_poCleanup->DeleteLogFiles(6, 0, 0);

		fp1 = m_poCleanup->isDirectory("/data/sts/data/ststestMP");

		if ((fp1)) {
			bResult = true;
		}

		ASSERTM(
				"DeleteLogFiles() function for STS (Dummy) with less ageworking properly",
				bResult == true);

		system("date -s \"Tue Jan 31 09:39:35 IST 2011\"");

		m_poCleanup->DeleteLogFiles(6, 0, 0); //delete more than 60 days

		bResult = false;

		fp1 = m_poCleanup->isDirectory("/data/sts/data/ststestMP");

		if ((fp1)) {
			bResult = false;
		} else {
			bResult = true;
		}
	}
	else
	{
		bResult = TRUE;
	}
	ASSERTM(
			"DeleteLogFiles() function for STS (Dummy) with less age not working properly",
			bResult == true
			);

}

void Cute_ACS_ACSC_Logmaint::vTestDeleteEmptySTSMPFolder()
{

	bool bResult = false;
	bool fp1 = false;
	int nodeState = 0;
	nodeState = prcObj.askForNodeState();
	if(nodeState == 1)
	{
		system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
		system("mkdir -p /data/sts/data/ststestMP");
		m_poCleanup->DeleteLogFiles(6, 0, 0);
		system("date -s \"Tue Nov 3 08:40:35 IST 2010\"");
		fp1 = m_poCleanup->isDirectory("/data/sts/data/ststestMP");

		if ((fp1))
		{
			bResult = true;
		}

		ASSERTM(
				"DeleteLogFiles() function for STS (Dummy) with less age working properly",
				bResult == true);
		system("date -s \"Tue Nov 7 08:40:35 IST 2010\"");
		//system("date -s \"Tue Jan 31 09:39:35 IST 2011\"");

		m_poCleanup->DeleteLogFiles(6, 0, 0); //delete more than 60 days

		bResult = false;

		fp1 = m_poCleanup->isDirectory("/data/sts/data/ststestMP");

		if ((fp1)) {
			bResult = false;
		} else {
			bResult = true;
		}
	}
	else
	{
		bResult = TRUE;
	}
	ASSERTM(
			"DeleteLogFiles() function for STS (Dummy) with less age not working properly",
			bResult == true
			);

}



void Cute_ACS_ACSC_Logmaint::vTeststringReverse() 
{

	bool bResult = false;
	string testString = "Hello world";
	string testStringRev = "dlrow olleH";

	cout<<"Reverse string : "<<m_poCleanup->stringReverse(testString);
	if(testStringRev.compare((m_poCleanup->stringReverse(testString))))
	bResult = true;

	ASSERTM("stringReverse() function failed ",bResult==true);
}


void Cute_ACS_ACSC_Logmaint::vFindFirstFile() 
{
	LIN_64_FIND_DATA fdFindFileData;
	FHANDLE hFindFile = NULL;
	bool bResult = FALSE;
	system("mkdir /root/testDirectory_logm");
	system("mkdir /root/testDirectory_logm/insideTestDirectory_logm");
	system("touch /root/testDirectory_logm/insideTestDirectory_logm/logm.log");

	hFindFile = m_poCleanup->FindFirstFile("/root/testDirectory_logm/insideTestDirectory_logm/*.log", &fdFindFileData);
	if( ((int)hFindFile != INVALID_HANDLE_VALUE) && ((int)hFindFile != ERROR_FILE_NOT_FOUND))
	{
		if(!(strcmp(fdFindFileData.cFileName, "/root/testDirectory_logm/insideTestDirectory_logm/logm.log")))
		{
			bResult = TRUE;
		}

	}
	system("rm -rf /root/testDirectory_logm");

	ASSERTM("Error in vFindFirstFile() function",bResult==true);
}

void Cute_ACS_ACSC_Logmaint::vFindNextFile() 
{
	LIN_64_FIND_DATA fdFindFileData;
	FHANDLE hFindFile = NULL;
	bool bResult = FALSE;
	system("date -s \"Tue Nov 2 08:39:35 IST 2010\"");
	system("mkdir /root/testDirectory_logm");
	system("mkdir /root/testDirectory_logm/insideTestDirectory_logm");
	system("touch /root/testDirectory_logm/insideTestDirectory_logm/logm.log");
	system("date -s \"Tue Nov 3 08:39:35 IST 2010\"");
	system("touch /root/testDirectory_logm/insideTestDirectory_logm/logm1.log");
	hFindFile = m_poCleanup->FindFirstFile("/root/testDirectory_logm/insideTestDirectory_logm/*.log", &fdFindFileData);
	if( (hFindFile != INVALID_HANDLE_VALUE) && (hFindFile != ERROR_FILE_NOT_FOUND))
	{
		bResult = m_poCleanup->FindNextFile(&hFindFile, &fdFindFileData);
		if(!(strcmp(fdFindFileData.cFileName, "/root/testDirectory_logm/insideTestDirectory_logm/logm.log")))
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}
	}
	system("rm -rf /root/testDirectory_logm");

	ASSERTM("Error in vFindFirstFile() function",bResult==true);
}
/* end vTestGetDiskMonitorSettings()*/



void Cute_ACS_ACSC_Logmaint::vTestremovedirectoryrecursively() 
{

	bool bResult = true;
	DIR *pDir;
	system("mkdir /root/testDirectory_logm");
	system("mkdir /root/testDirectory_logm/insideTestDirectory_logm");
	system("touch /root/testDirectory_logm/insideTestDirectory_logm/logm.log");

	const char* pzPath = "/root/testDirectory_logm";

	if((m_poCleanup->removedirectoryrecursively(pzPath)))
	{
		pDir = opendir(pzPath);

		if(pDir != NULL)
		{
			bResult = false;
			(void) closedir (pDir);
		}

	}

	ASSERTM("removedirectoryrecursively() function failed ",bResult==true);
}


void Cute_ACS_ACSC_Logmaint::vTestextractFileNameFrompath() 
{

	bool bResult = false;
	string fileName = "filename.log";
	string path = "/root/sat/filename.log";

	if(fileName == (m_poCleanup->extractFileNameFrompath(path)))
	bResult = true;

	ASSERTM("extractFileNameFrompath() function failed ",bResult==true);
}

void Cute_ACS_ACSC_Logmaint::vTestfindPathSearchPatternFromWildCardDirectory() 
{

	bool bResult = false;
	string directoryName = "/var/log/MSFTPSVC*";
	string searchPattern = "/var/log/MSFTPSVC";

	if(searchPattern == (m_poCleanup->findPathSearchPatternFromWildCardDirectory(directoryName)))
	bResult = true;

	ASSERTM("findPathSearchPatternFromWildCardDirectory() function failed ",bResult==true);
}


void Cute_ACS_ACSC_Logmaint::vTestsearchPathFromWildCard() 
{

	bool bResult = true;
	string directoryName1 = "/var/log/MSFTPSVC*";
	string directoryName2 = "/var/log/MSFTPSVC/*.log";

	string directoryNamePath1 = "/var/log/";
	string directoryNamePath2 = "/var/log/MSFTPSVC/";

	if(!(directoryNamePath1 == (m_poCleanup->searchPathFromWildCard(directoryName1))))
	bResult = false;
	if(!(directoryNamePath2 == (m_poCleanup->searchPathFromWildCard(directoryName2))))
	bResult = false;

	ASSERTM("findPathSearchPatternFromWildCardDirectory() function failed ",bResult==true);
}

void Cute_ACS_ACSC_Logmaint::vremoveFile() 
{

	bool bResult = true;
	FILE *fp1;
	system("mkdir /root/testRemoveFile_logm");
	system("touch /root/testRemoveFile_logm/Remove.log");

	char* fileName = "/root/testRemoveFile_logm/Remove.log";

	if((m_poCleanup->removeFile(fileName)))
	{
		fp1 = fopen("/root/testRemoveFile_logm/Remove.log", "r");
		if (!(fp1)) {
			bResult = true;
		} else {
			bResult = false;
		}
	}

	system("rm -rf /root/testRemoveFile_logm");
	ASSERTM("removeFile() function failed ",bResult==true);
}


void Cute_ACS_ACSC_Logmaint::vgetTimeStampOfFile() {

	bool bResult = false;

	system("date -s \"Tue Nov 2 04:39:35 IST 2010\"");
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);


	system("mkdir /root/testRemoveFile_logm");
	system("touch /root/testRemoveFile_logm/Remove.log");

	string fileName("/root/testRemoveFile_logm/Remove.log");

	if( (asctime(timeinfo)) == asctime(m_poCleanup->getTimeStampOfFile(fileName)))
	{
		bResult = true;
	}
	system("rm -rf /root/testRemoveFile_logm");


	ASSERTM("vgetTimeStampOfFile() function failed ",bResult==true);
}

#if 0
void Cute_ACS_ACSC_Logmaint::vshowDirectoriesWildCard() {

	bool bResult = FALSE;



	system("mkdir /root/testRemoveFile_logm");
	system("touch /root/testRemoveFile_logm/File1.log");
	system("touch /root/testRemoveFile_logm/File2.log");
	system("touch /root/testRemoveFile_logm/File3.log");
	system("touch /root/testRemoveFile_logm/File3.log");
	string filesPath("/root/testRemoveFile_logm/");

	int status = 0;
	 status = m_poCleanup->showDirectoriesWildCard(filesPath);
	 if((status != INVALID_HANDLE_VALUE) && (status != ERROR_FILE_NOT_FOUND))
	 {
		 bResult = TRUE
		 for(unsigned int i=0;i<m_poCleanup->logFileContainerWC.size();++i)
		 {
			 cout<<"logFileContainer file :"<<m_poCleanup->logFileContainerWC[i].c_str()<<endl;
		 }
	 }



	ASSERTM("vshowDirectoriesWildCard() function failed ",bResult==true);
}

void Cute_ACS_ACSC_Logmaint::vshowFilesandDirectories() {

	bool bResult = FALSE;



	system("mkdir /root/testRemoveFile_logm");
	system("mkdir /root/testRemoveFile_logm/Dir1");
	system("mkdir /root/testRemoveFile_logm/Dir2");
	system("touch /root/testRemoveFile_logm/File1.log");
	system("touch /root/testRemoveFile_logm/File2.log");
	system("touch /root/testRemoveFile_logm/File3.log");
	system("touch /root/testRemoveFile_logm/File3.log");
	string filesPath("/root/testRemoveFile_logm/");

	int status = 0;
	 status = m_poCleanup->vshowFilesandDirectories(filesPath);
	 if((status != INVALID_HANDLE_VALUE) && (status != ERROR_FILE_NOT_FOUND))
	 {
		 bResult = TRUE
		 for(unsigned int i=0;i<logFileContainer.size();++i)
		 {
			 cout<<"logFileContainer file :"<<logFileContainer[i].c_str()<<endl;
		 }
	 }



	ASSERTM("vshowFilesandDirectories() function failed ",bResult==true);
}
#endif



/*==========================================================
 * ROUTINE:make_suite_Cute_ACS_ACSC_Logmaint()
 *==========================================================*/
cute::suite Cute_ACS_ACSC_Logmaint::make_suite_Cute_ACS_ACSC_Logmaint()
{
	ACS_LOGM_Common::ACS_LOGM_Logging = new ACS_TRA_Logging();
	ACS_LOGM_Common::ACS_LOGM_Logging->ACS_TRA_Log_Open("LOGM");
	ACS_LOGM_Common::fetchDnOfRootObjFromIMM();

	m_poCleanup = new ACS_ACSC_Logmaint( Cute_ACS_LOGM_Service::getPlhEventHandles());
	//m_poCleanup = new ACS_ACSC_Logmaint();
	cute::suite s;
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestgetCurrentTime));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestGetLogFileParams));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestGetServiceparams));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vGetInitialTimeToWait));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestisDirectory));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestElapsedTimeCal));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vFindFirstFile));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vFindNextFile));//Implemented



	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestisWildCard));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestwildCardMatch));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestremovedirectoryrecursively));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestextractFileNameFrompath));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestfindPathSearchPatternFromWildCardDirectory));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestsearchPathFromWildCard));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTeststringReverse));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesSTSMPDummy));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteEmptySTSMPFolder));//Implemented
	//s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesFTLOG));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesSTSMP));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesDDI));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestDeleteLogFilesTRA));//Implemented
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vTestFindFileAndFolders));//Implemented

	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vremoveFile));
	s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vgetTimeStampOfFile));
	//s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vshowDirectoriesWildCard));
	//s.push_back(CUTE(Cute_ACS_ACSC_Logmaint::vshowFilesandDirectories));


	return s;
}/* end make_suite_Cute_ACS_ACSC_Logmaint()*/

ACS_ACSC_Logmaint * Cute_ACS_ACSC_Logmaint::getPoCleanup() {
	return m_poCleanup;
}

