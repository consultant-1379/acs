//****************************************************************************
//
//  NAME
//     alogpls.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2004. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson  AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  PR           DATE      INITIALS    DESCRIPTION
//  -----------------------------------------------------------
//  N/A       05/08/2012     xbhakat       Initial Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************

#include <iostream>
//#include <fstream>
#include <list>
#include <string>
#include <ACS_CS_API.h>
#include <acs_prc_api.h>
#include <acs_alog_defs.h>
#include <acs_alog_fileDef.h>
#include <acs_alog_cmdUtil.h>
#include <acs_alog_PLFileIterator.h>

using namespace std;

class Alogpls_Print
{
public:
	char Type[11];
	char Data[1001];

public:
	Alogpls_Print(string lpszType, string lpszData)
	{
		strcpy(Type, lpszType.c_str());
		strcpy(Data, lpszData.c_str());
	}
};

static string g_lpszName = "alogpls";

extern char* optarg;
extern int   optind;
extern int   opterr;

int getNodeState() {
	int nodeState = 0;
	ACS_PRC_API prc;

	try {
		nodeState = prc.askForNodeState();
	} catch (int e) {
		return -1;
	}
	return nodeState;
}

int main(int argc, char* argv[])
{
	opterr = 0;
	int nodeStateVal = 0;
	int nRetCode;

	//get node state
	nodeStateVal = getNodeState();
	if (nodeStateVal == 2) {
		fprintf(stderr,
				"This is the passive node, you must execute the command on the active node\n");
		return 3;
	}
	else if (nodeStateVal != 1 && nodeStateVal != 2)
	{
		fprintf(stderr, "Undefined node state\n");
		return -1;
	}

	ACS_CS_API_CommonBasedArchitecture::ApgNumber  APtype;

	ACS_CS_API_NetworkElement::getApgNumber (APtype);

	if (APtype == 2)		//  AP2
	{
			fprintf(stderr,"This is the AP2 node, the Protected Logging function doesn't exist\n");
			return 3;
	}

	if (argc != 1)		// At least one program argument was given.
	{
//		string lpArg = argv[1];

//		if (*lpArg.c_str() == '-' && strcmp(lpArg.c_str(),"-") != 0)
//		{
//			cerr << "Illegal option \"" << lpArg << "\"" << endl;
//			return 27;
//		}
//		else
//		{
			cerr << "Incorrect usage" << endl;
			cerr << "Usage: alogpls " << endl;
			return 2;
//		}
	}

	// Password Handling

	int retcode = acs_alog_cmdUtil::PasswordHandling();
	if(retcode == 29)
		cerr << endl << "Internal Audit Log Problems "   << endl;
	if(retcode != 0)
		return(retcode);

	nRetCode = 0;
	bool bFileAccessError = false;
	char szFilename[20];
	char szType[11];
	char szData[1001];
	list<Alogpls_Print> itemList;

	for (int nIdx = 1; nIdx <= 3; nIdx++) //HK94762
	{
		switch (nIdx)
		{
		case 1:
			strcpy(szFilename, "PMMLCMD");
			break;
		case 2:
			strcpy(szFilename, "PMMLPRT");
			break;
		case 3:
			strcpy(szFilename, "PUSR");
			break;
		default:
			// Should never occur
			break;
		}

		ACS_ALOG_Exclude *pExclude;
		ACS_ALOG_PLFileIterator FileIter(szFilename);

		for (; FileIter; ++FileIter)
		{
			pExclude = FileIter();

			switch (nIdx)
			{
			case 1:
				strcpy(szType, "pmc    ");
				strcpy(szData, pExclude->Data1);
				break;
			case 2:
				strcpy(szType, "pmp    ");
				strcpy(szData, pExclude->Data1);
				break;
			case 3:
				strcpy(szType, "user   ");
				strcpy(szData, pExclude->Data1);
				break;
			}

			// Save all excluded items in a stl list object
			itemList.push_back(Alogpls_Print(szType, szData));

			delete pExclude;
			pExclude = NULL;
		} // for-loop

		nRetCode = FileIter.GetRetCode();

		if (FileIter.GetRetCode() != 0)
			bFileAccessError = true;
	}

	// Check for file access errors
	if (bFileAccessError)
	{
		cerr << "File access error"  << endl;
		itemList.clear();
		return 22;
	}

	// Printing header
	cout << "TYPE   ITEM" << endl;

	// Printing excluded items
	char* lpszData;
	int len;
	list<Alogpls_Print>::iterator itemListIter = itemList.begin();
	while (itemListIter != itemList.end())
	{
		cout << (char*)(*itemListIter).Type;

		// Add a " character first and last in the printout if the excluded string
		// begins or ends with a space character
		lpszData = (char*)(*itemListIter).Data;
		len = strlen(lpszData);
		if (*lpszData == ' ' || lpszData[len-1] == ' '){
			cout << "\"" << lpszData << "\"" << endl;
		}
		else
		{
			cout << (char*)(*itemListIter).Data << endl;
		}
		itemListIter++;
	}

	// Clears the list of items
	itemList.clear();

	return 0;
}

