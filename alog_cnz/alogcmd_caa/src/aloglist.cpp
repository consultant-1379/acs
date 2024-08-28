//******************************************************************************
//
// NAME
//      aloglist.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
// .DESCRIPTION
// 	List Attributes in Audit Log
//
// DOCUMENT NO
//	190 89-CAA 109 0443
//
// AUTHOR
// 	1997-11-03  UAB/I/LN  UABUSOD
//
// REVISION
//      PA3     97-12-10
//
// CHANGES
//
//      REVISION HISTORY
//
//      REV           DATE            NAME            DESCRIPTION
//      A             97-11-03        UABUSOD         First release
//      B             05-12-04        UABMAGN         Id tag logging added
//      C             2008-10-28      QNICMUT         CR42
//
//******************************************************************************

#define _WIN32_WINNT 0x0400

#include <iostream>
#include <iomanip>
#include <ACS_CS_API.h>
#include <ACS_APGCC_CommonLib.h>
#include <getoption.h>
#include <acs_alog_Lister.h>
#include <acs_prc_api.h>
#include <acs_alog_cmdUtil.h>
#include <ace/ACE.h>
using namespace std;

//static const char* g_lpszName = "aloglist";

extern char* optarg;
extern int optind;
extern int opterr;


ACE_TCHAR * replaceChar(ACE_TCHAR find, ACE_TCHAR repl, ACE_TCHAR * buffer) {
	ACE_TCHAR * copy_buffer = buffer;
	while (ACE_TCHAR(*buffer) != 0) {
		if (ACE_TCHAR(*buffer) == find)
			(*buffer) = repl;
		buffer++;
	}
	return copy_buffer;
}

ACE_TCHAR * cutSubString(ACE_TCHAR *source, ACE_TCHAR *find) {
	ACE_TCHAR * pToExtention = ACE_OS::strstr(source, find);
	if (pToExtention != NULL)
		*pToExtention = 0;
	return source;
}


void printUsage()
{
		cerr << "Incorrect usage" << endl;
        cerr << "Usage:" << endl;
        cerr << "       Format1: aloglist " << endl;
        cerr << "       Format2: aloglist -p [ -d ] " << endl;
}


int getNodeState() {
	int nodeState = 0;
	ACS_PRC_API prc;

	try {
		nodeState = prc.askForNodeState();
	}
	catch (int e) {
		return -1;
	}
	return nodeState;
}


int main(int argc, char * argv[]) {

	// Check that we are running on the active node
	int nodeStateVal = 0;
	int nRetCode;

	//get node state
	nodeStateVal = getNodeState();
	if (nodeStateVal == 2) {
		fprintf(stderr,
				"This is the passive node, you must execute the command on the active node\n");
		return 3;
	}
	else if (nodeStateVal != 1 && nodeStateVal != 2) {
		fprintf(stderr, "Undefined node state\n");
		return -1;
	}

	ACS_CS_API_CommonBasedArchitecture::ApgNumber  APtype;

	ACS_CS_API_NetworkElement::getApgNumber (APtype);

	if (APtype == 2)		//  AP2
	{
			fprintf(stderr,"This is the AP2 node, the Protected Logging function doesn' t exist\n");
			return 3;
	}

	int d_flag = 0;
	int p_flag = 0;
	opterr = 0;
	int optIndex;

	if (argc != 1) //without parameter
	{
		for (optIndex = 1; optIndex < argc; optIndex++) {

			if (argv[optIndex][0] != '-') {
							printUsage();
							return 2;
			}
			//check for correct input parameter. Two char.
			if (((strlen(argv[optIndex]) > 2) || (strlen(argv[optIndex]) < 2)))
			{
					cerr << "Illegal option: \"" << argv[optIndex] << "\"" << endl;
					return 27;
			}

			if (strcmp(argv[optIndex], "-p") == 0) {      // device
					p_flag++;
			}
			else if (strcmp(argv[optIndex], "-d") == 0) // device
					{
				d_flag++;
			} else {

				cerr << "Illegal option: \"" << argv[optIndex] << "\"" << endl;
				return 27;
			}
		}
	}

	if (p_flag > 1  ||  d_flag > 1)
	{
			printUsage();
			return 2;
	}

	ACS_ALOG_Lister * pLister = new ACS_ALOG_Lister();
	nRetCode = pLister->GetRetCode();

	if (nRetCode == 1) {
		cerr << "File access error" << endl << endl;
		delete pLister;
		return 22;
	}

	// PL if -p option
	if (p_flag) {

		// Command not allowed on AP2

		/*		if (!ACS_ALOG_Util::CheckIfAP1())
		 {
		 cerr << "Command not allowed on AP2"  << endl;
		 return 44;
		 }*/
		char pPassword[260];

		// Password Handling
		int retcode = acs_alog_cmdUtil::PasswordHandling(pPassword);

		if (retcode == 29)  cerr << endl << "Internal Audit Log Problems " << endl;
		if (retcode != 0)	return (retcode);

		cout << "STATUS" << endl << "ACTIVE" << endl << endl;

	}
	else if (!p_flag && !d_flag)
	{
		cout << endl << "PROTECTED LOGGING STATUS" << endl;
		int nReturn;
		nReturn = acs_alog_cmdUtil::CheckALOGSysFile(true, true,false);
		if (nReturn == 41)
		{
				cout << "ACTIVE" << endl << endl;
				return 0;
		}
		if (nReturn == 0)
		{
				cout << "PASSIVE" << endl << endl;
				return 0;
		}
		cout << "Not possible to provide the PLOG status" << endl << endl;
		return nReturn;
	}
	else {
			printUsage();
			return 2;
	}

	ACE_TCHAR * lpszFileDest = (ACE_TCHAR *) pLister->GetFileDest();
	cout << "MAXIMUM LOGFILE SIZE    CURRENT LOGFILE SIZE" << endl;

	char szTemp[64];

	if (*lpszFileDest)  sprintf(szTemp, ("%s"), ("-"));
	else				sprintf(szTemp, ("%s"), ("100 MB"));

	cout << setw(24) << left << szTemp;
	long currentFileSize = pLister->GetCurrentFileSize();

	if (currentFileSize > 0)  cout << currentFileSize << " MB" << endl << endl;
	else					  cout << "< 1 MB" << endl << endl;

	ACE_TCHAR * lpszOldestFile = pLister->GetOldestLogfile();
	if(strstr(lpszOldestFile, ".zip") == NULL)
		lpszOldestFile = cutSubString(lpszOldestFile, ".7z");

	else
		lpszOldestFile = cutSubString(lpszOldestFile, ".zip");


	lpszOldestFile = replaceChar('_', '-', lpszOldestFile);

	ACE_TCHAR * lpszNewestFile = pLister->GetNewestLogfile();
	if(strstr(lpszNewestFile, ".zip") == NULL)
		lpszNewestFile = cutSubString(lpszNewestFile, ".7z");
	else
		lpszNewestFile = cutSubString(lpszNewestFile, ".zip");


	lpszNewestFile = replaceChar('_', '-', lpszNewestFile);

	ACE_TCHAR  szOldestFile[30] = {0};
	ACE_TCHAR  szNewestFile[30] = {0};

	if ((nRetCode = pLister->GetRetCode()) == 0)
	{
			if (strncmp (lpszNewestFile, "paudit.", 7) == 0)
			{
					strncpy (szNewestFile, &lpszNewestFile[7], 4);
					strcat (szNewestFile, "-");  strncat (szNewestFile, &lpszNewestFile[11], 2);
					strcat (szNewestFile, "-");  strncat (szNewestFile, &lpszNewestFile[13], 2);
					strcat (szNewestFile, " ");  strncat (szNewestFile, &lpszNewestFile[16], 4);
			}
			if (strncmp (lpszOldestFile, "paudit.", 7) == 0)
			{
					strncpy (szOldestFile, &lpszOldestFile[7], 4);
					strcat (szOldestFile, "-");  strncat (szOldestFile, &lpszOldestFile[11], 2);
					strcat (szOldestFile, "-");  strncat (szOldestFile, &lpszOldestFile[13], 2);
					strcat (szOldestFile, " ");  strncat (szOldestFile, &lpszOldestFile[16], 4);
			}

			if (strncmp (lpszNewestFile, "PLOGFILE_", 9) == 0)
			{
					strncpy (szNewestFile, &lpszNewestFile[9], 10);
					strcat (szNewestFile, " ");  strncat (szNewestFile, &lpszNewestFile[20], 4);
			}
			if (strncmp (lpszOldestFile, "PLOGFILE_", 9) == 0)
			{
					strncpy (szOldestFile, &lpszOldestFile[9], 10);
					strcat (szOldestFile, " ");  strncat (szOldestFile, &lpszOldestFile[20], 4);
			}
	}
	else {
			ACE_OS::strcpy(szOldestFile, lpszOldestFile);
			ACE_OS::strcpy(szNewestFile, lpszNewestFile);
	}

	std::string firstRecordTime;
	std::string lastRecordTime;
	std::string OldFile;
	std::string NewFile;

	if (pLister->GetLogRecordTimes (firstRecordTime, lastRecordTime, OldFile, NewFile) == 1)
	{
			cerr << "File access error" << endl << endl;
			delete pLister;
			return 22;
	}

	if (strlen(OldFile.c_str()) == 17)
	{
			cout << "LOGGING PERIOD" << endl;
			cout << OldFile.substr(0, 15) << "    " << NewFile.substr(0, 15) << endl	<< endl;
	}
	else {
			cout << "LOGGING PERIOD" << endl;
			cout << szOldestFile << "    " << szNewestFile << endl << endl;
	}

	if (d_flag)
	{
		if (strlen(OldFile.c_str()) == 17)
		{
			std::string resultFirstRecordTime = firstRecordTime.substr(0, 13)
					+ ":" + firstRecordTime.substr(13, 2) + ":"
					+ firstRecordTime.substr(15, 2);
			std::string resultLastRecordTime = lastRecordTime.substr(0, 13)
					+ ":" + lastRecordTime.substr(13, 2) + ":"
					+ lastRecordTime.substr(15, 2);
			cout << "FIRST EVENT DATE AND TIME    LAST EVENT DATE AND TIME" << endl;
			cout << setw(25) << resultFirstRecordTime << "    ";
			cout << setw(25) << resultLastRecordTime << "    ";
			cout << endl << endl;
		}
	}
	cout << "TRANSFER QUEUE" << endl;
	cout << pLister->GetFileDest() << endl << endl;

	 bool nIdTag = pLister->GetIdTag();  // new 0512
	 if (nIdTag) cout << "COMMAND ID AND SESSION ID IN AUDIT LOG IS ACTIVE" << endl << endl;

	delete pLister;

	return 0;
}

