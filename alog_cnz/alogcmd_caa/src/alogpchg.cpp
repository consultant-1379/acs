//****************************************************************************
//
//  NAME
//     alogpchg.cpp
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
#include <acs_alog_defs.h>
#include <acs_alog_cmdUtil.h>
#include <acs_alog_execute.h>
#include <getoption.h>
#include <ACS_CS_API.h>
#include <acs_prc_api.h>
#include <pwd.h>

using namespace std;
#define ALARM_SLOGAN _T("*** ALARM")

using namespace std;


//static char* g_lpsName = "alogpchg";

char* optargPL;
int   optindPL = 1;
int   optoptPL;

int getoptPL(int argc,  char* const argv[], const  char* opts)
{
	static int sp = 1;
	int c;
	register char* cp;

	if (sp == 1)
	{
		if (optindPL >= argc || argv[optindPL][0] != '-' || argv[optindPL][1] == '\0')
		{
			return -1;
		}
		else if (strcmp(argv[optind], "--") == 0)
		{
			optindPL++;
			return -1;
		}
	}
	// TR HM58079
	if (2 < strlen(argv[optindPL]))
	{
		fprintf(stderr, "Illegal option: \"%s\" \n",  argv[optindPL]);
		return '?';
	}

	optoptPL = c = argv[optindPL][sp];

	if (c == ':' || (cp = strchr((char*)opts, c)) == 0)
	{

		if (argv[optindPL] [++sp] == '\0')
		{
			optindPL++;
			sp = 1;
		}

		return '*';
	}


	if (*(++cp) == ':')
	{
		if (argv[optindPL][sp+1] != '\0')
			optargPL = &argv[optindPL++][sp+1];
		else if (++optindPL >= argc)
		{
			sp = 1;
			return '!';
		}
		else
		{
			optargPL = argv[optindPL++];
		}

		sp = 1;
	}
	else
	{
		if (argv[optindPL][++sp] == '\0')
		{
			sp = 1;
			optindPL++;
		}

		optargPL = 0;
	}

	return c;
}
//--------------------------------------------------------------------------------------------------

int PrintIncorrectUsage(void)
{
	cerr << "Incorrect usage" << endl;
	cerr << "Usage: alogpchg -t Type -a Item |-r Item" << endl ;
	return 2;
}
//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int t_flag = 0;
	int a_flag = 0;
	int r_flag = 0;
	int nodeStateVal = 0;
	//int f_flag = 0;

	string pType;
	string pItem;

	opterr = 0;

	// Setting thread priority to below normal

	// Command not allowed on AP2
	/*if (!ACS_ALOG_Util::CheckIfAP1())
	{
		cerr << "Command not allowed on AP2" << endl;
		return 44;
	}*/
	// Check that we are running on the active node
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
			fprintf(stderr,"This is the AP2 node, the Protected Logging function doesn't exist\n");
			return 3;
	}

	int ch;
	int common_flag;
	while ((ch = getoptPL(argc, argv, "t:a:r:")) != EOF)
	{
		switch (ch)
		{
		case 't':
			t_flag++;
			pType = optargPL;
			common_flag = t_flag;
			break;
		case 'a':
			a_flag++;
			common_flag = a_flag;
			pItem = optargPL;
			break;
		case 'r':
			r_flag++;
			common_flag = r_flag;
			pItem = optargPL;
			break;
		case '?':
			return(27);
			break;
		case '!':
			return (PrintIncorrectUsage());
			break;
		default:
			switch ((char)optoptPL)
			{
				case '-':
				case 0:
				case ' ':
					cerr << "Illegal option: \"-" << (char) optoptPL << "\"" << endl;
					return(27);
					break;
				case 't':
				case 'a':
				case 'r':
					return (PrintIncorrectUsage());
					break;
				default:
					cerr << "Illegal option: \"-" << (char) optoptPL << "\"" << endl;
					return(27);
			}

			break;
		}

		if(common_flag > 1){
			return (PrintIncorrectUsage());
		}

	}

	// Now all arguments should have been parsed.
	// If there are anymore then we have a format error.
	//--------------------------------------------------
	if (argv[optindPL] != NULL)
	{
		return (PrintIncorrectUsage());
	}

	if (t_flag > 1 || a_flag > 1 || r_flag > 1 /*|| f_flag > 1*/)
	{
		return (PrintIncorrectUsage());
	}

	///////////////////////////////
	// alogpchg -t pType -a|-r pItem
	///////////////////////////////

	// Check that pItem is entered
	//---------------------------
	if (pItem == "")
	{
		return (PrintIncorrectUsage());
	}

	enumCmdExecuteResult ExecResult = cerSuccess;
	enumCmdType CmdType = ctUndefined;
	acs_alog_dataUnitType DataUnitType = not_defined;

	if (t_flag != 1)
	{
		return (PrintIncorrectUsage());
	}

	// Check if options a and r are given together
	//--------------------------------------------
	if ( (a_flag + r_flag) != 1 )
	{
		return (PrintIncorrectUsage());
	}

	// Check the pType
	//---------------
	if (pType == "")
	{
		return (PrintIncorrectUsage());
	}




	string szValue1;
	string szValue2;

	if (strcasecmp(pType.c_str(), "pmc") == 0)
	{
		////////////////////////////////////////////////////////////////////////////////
		// Protect/unprotect MML command
		////////////////////////////////////////////////////////////////////////////////

		DataUnitType = MMLcmd;
		szValue1 = pItem;
		//strcpy(szValue1, pItem.c_str());
		//_tcsupr(szValue1);
		std::transform(szValue1.begin(),szValue1.end(),szValue1.begin(),::toupper);

		if (a_flag == 1)
		{
			// protect an MML command
			//-----------------------
			CmdType = ctProtectItem;
		}
		else if (r_flag == 1)
		{
			// unprotect an MML command
			//-----------------------
			CmdType = ctUnprotectItem;
		}
	}
	else if (strcasecmp(pType.c_str(), "pmp") == 0)
	{
		////////////////////////////////////////////////////////////////////////////////
		// protect/unprotect MML printout
		////////////////////////////////////////////////////////////////////////////////

		DataUnitType = MMLprt;
		szValue1 = pItem;
		//strcpy(szValue1, (const char*)pItem);
		//toupper(szValue1);
		std::transform(szValue1.begin(),szValue1.end(),szValue1.begin(),::toupper);

		if (a_flag == 1)
		{
			// protect an MML printout
			//------------------------
			CmdType = ctProtectItem;
		}
		else if (r_flag == 1)
		{
			// unprotect an MML printout
			//------------------------
			CmdType = ctUnprotectItem;
		}
	}
	else if (strcasecmp(pType.c_str(), "user") == 0)
	{
		////////////////////////////////////////////////////////////////////////////////
		// protect/unprotect MML events of a user
		////////////////////////////////////////////////////////////////////////////////

		DataUnitType = not_defined;
		szValue1 = pItem;

		//Start of TR HW20931: Check whether username in 'pItem' is an LDAP user. In case of LDAP users, convert 'pItem' to lowercase
		struct passwd *pw = NULL;
		string username = pItem;

		pw = getpwnam(username.c_str());

		if (pw != NULL) {
			if(pw->pw_uid >= 1000)     	// UID for LDAP users is equal to or greater than 1000 as of now. Refer User Management UG
			{
				std::transform(szValue1.begin(),szValue1.end(),szValue1.begin(),::tolower);
			}
		}
		//end of TR HW20931

		if (a_flag == 1)
		{
			// protect MML events of a user
			//------------------------
			CmdType = ctProtectUser;
		}
		else if (r_flag == 1)
		{
			// unprotect  MML events of a user
			//------------------------
			CmdType = ctUnprotectUser;
		}
	}
	else
	{
		cerr << "Unreasonable value" << endl;
		return 28;
	}


	if (CmdType == ctUndefined || ((strcasecmp(pType.c_str(), "user") != 0) && DataUnitType == not_defined))
	{
		cerr << "Unreasonable value" << endl;
		return 28;
	}

	int retcode = acs_alog_cmdUtil::PasswordHandling();
	if (retcode == 29){
		cerr << endl << "Internal Audit Logs Problems " << endl;
	}

	if(retcode !=0)
		return(retcode);

	// Send question to user
	//if (!f_flag)
	//{
		if (CmdType == ctProtectItem || CmdType == ctProtectUser)
			cout << "Are you sure you want to insert protection of item in Audit Log function?" << endl;
		else
			cout << "Are you sure you want to remove protection of item in Audit Log function?" << endl;
		cout << "Enter y or n [default: n]\03: " << flush;

		char sResponce[64+1];
		cin.getline(sResponce, 64);

		// Exit here
		//----------
		if (!(*sResponce) || strcasecmp(sResponce, "no") == 0 || strcasecmp(sResponce, "n") == 0)
		{
			// Command is not to be executed
			//------------------------------
			return 0;
		}

		if (strcasecmp(sResponce, "yes") != 0 && strcasecmp(sResponce, "y") != 0)
		{
			cerr << "Illegal answer: \"" << sResponce << "\"" << endl;
			return 2;
		}
	//}



	// Execute command
		acs_alog_execute Execute;
	ExecResult = Execute.Execute(CmdType, DataUnitType, szValue1.c_str(), szValue2.c_str());

	int nReturn = 0;

	switch (ExecResult)
	{
	case cerSuccess:
		break;
	case cerUserNotDefined:
	case cerItemNotDefined:
		cerr << "Item not found" << endl << endl; //PL
		nReturn = 24;
		break;
	case cerUserAlreadyDefined:
	case cerItemAlreadyDefined:
		cerr << "Item already added" << endl << endl;//PL
		nReturn = 23;
		break;
	case cerServerNotResponding:
		cerr << "Unable to connect to server" << endl << endl;
		nReturn = 117;
		break;
	case cerFileOpenError:
	case cerFileReadError:
	case cerFileWriteError:
		cerr << "File access error" << endl << endl;
		nReturn = 22;
		break;
	case cerBackupInProgress:
		cerr<<"Command not executed, AP backup in progress"<<endl <<endl;
		nReturn = 114;
		break;
	default:
		cerr << "Internal Audit Logs Problems" << endl << endl;
		nReturn = 29;
		break;
	}

	return nReturn;
}
//--------------------------------------------------------------------------------------------------
