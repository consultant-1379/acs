//****************************************************************************
//
//  NAME
//     alogset.cpp
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
//  N/A       26/07/2012     xbhakat       Initial Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************


#include <ACS_CS_API.h>
#include <acs_prc_api.h>
#include "acs_alog_cmdUtil.h"
#include "acs_alog_execute.h"
#include "acs_alog_defs.h"
#include "ACS_APGCC_CommonLib.h"
#include "ACS_APGCC_ApplicationManager.h"
#include "aes_ohi_extfilehandler2.h"
#include "aes_ohi_filehandler.h"
#include <ace/ACE.h>
#include "fstream"
#include <grp.h> /* defines 'struct group', and getgrnam(). */
using namespace std;

bool isMemeberOfTSGroup (const std::string user);
bool isMemberOfComEmergency (const std::string user);
static int ValidateFileDestination(char* pDest);
static void printUsage();


void  printUsage ()
{
	cerr << "Incorrect usage" << endl;
	cerr << "Usage" << endl;
	cerr << "       Format1: alogset -p [-c | -r] " << endl;
	cerr << "       Format2: alogset -p -d trasfer_queue " << endl;
	cerr << "                | -i idtaglogging" << endl;
}

//--------------------------------------------------------------------------------------------------
int ValidateFileDestination(char* pDest)
{

	char  rootPath[15];
	char PlogFilesPath[300];
	int   lenPath = 300;
	strcpy (rootPath, "auditLog");

	ACE_INT32  nReturn = 0;
	unsigned int  errorCode = AES_OHI_NOERRORCODE;
	AES_OHI_FileHandler  *extFileH;

	ACS_APGCC_CommonLib  apgccLib;

	if (apgccLib.GetFileMPath (rootPath, PlogFilesPath, lenPath) != ACS_APGCC_DNFPATH_SUCCESS)
	{
		syslog(LOG_ERR, "acs_alogmaind, PATH AUDIT definition failed !");
		return ACS_APGCC_FAILURE;
	}

	if (*pDest)          //  Transfer Queue not configured
	{
		extFileH = new AES_OHI_FileHandler ("ACS", "ALOG", pDest, "Transfer Queue handling", PlogFilesPath);

		errorCode = extFileH->attach();

		switch (errorCode)
		{
			case AES_OHI_NOERRORCODE:
				break;
			case AES_OHI_NOPROCORDER:
			case AES_OHI_NODESTINATION:
				cerr << "Transfer Queue \"" << pDest << "\" not defined in the output handler" << endl << endl;
				nReturn = 31;
				break;
			case AES_OHI_NOSERVERACCESS:
				cerr << "\"AES_AFP_server\" not responding" << endl << endl;
				nReturn = 33;
				break;
			default:
				cerr << "Invalid Transfer Queue" << endl;
				nReturn = 34;
				break;
		}
		extFileH->detach();

	}
	return nReturn;
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


/*============================================================================
	ROUTINE: isMemeberOfComEmergency
 ============================================================================ */

bool isMemberOfComEmergency (const std::string user)
{
   const std::string COM_EM_GRP = "com-emergency";
   bool isMember = false;
   /* get the information about the "strange" group. */
   struct group* group_info = getgrnam(COM_EM_GRP.c_str());
   /* make sure this group actually exists. */
   if (!group_info) {
	   //MCS_ADH_TRACE_MESSAGE("'com-emergency' does not exist. user=%s", user.c_str());
   }
   else {
	   char** p_member;
	   string str_member;
	   for (p_member = group_info->gr_mem; *p_member; p_member++) {
		   //MCS_ADH_TRACE_MESSAGE("COM Emergency member = <%s>", *p_member );
		   str_member = *p_member;
		   if (strcmp(user.c_str(), str_member.c_str()) == 0){
			   //MCS_ADH_TRACE_MESSAGE("The user <%s> belongs to com-emergency group", user.c_str());
			   isMember = true;
			   break;
		   }
	   }
   }

   return isMember;
}

/*============================================================================
	ROUTINE: isMemeberOfTSGroup
 ============================================================================ */

bool isMemeberOfTSGroup (const std::string user)
{
   const std::string TSGROUP_GRP = "tsgroup";
   bool isMember = false;
   /* get the information about the "strange" group. */
   struct group* group_info = getgrnam(TSGROUP_GRP.c_str());
   /* make sure this group actually exists. */
   if (!group_info) {
	   //MCS_ADH_TRACE_MESSAGE("'com-emergency' does not exist. user=%s", user.c_str());
   }
   else {
	   char** p_member;
	   string str_member;
	   for (p_member = group_info->gr_mem; *p_member; p_member++) {
		   //MCS_ADH_TRACE_MESSAGE("COM Emergency member = <%s>", *p_member );
		   str_member = *p_member;
		   if (strcmp(user.c_str(), str_member.c_str()) == 0){
			   //MCS_ADH_TRACE_MESSAGE("The user <%s> belongs to com-emergency group", user.c_str());
			   isMember = true;
			   break;
		   }
	   }
   }

   return isMember;
}


//--------------------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	opterr = 0;
	int nodeStateVal = 0;

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

	int  d_flag = 0;
	int  i_flag = 0;
	int  p_flag = 0;
	int  c_flag = 0;
	int  r_flag = 0;
	int  u_flag = 0;

	int  totalOptions = 0;
	int  nReturn = 0;

	char  *pDest  = NULL;
	char  *pIdTag = NULL;
	char  *user   = NULL;

	for (int j=1; j<argc; j++)
	{
		if (argv[j][0] != '-')
		{
				printUsage();  return 2;
		}
		if (strlen(argv[j]) == 1  ||  strlen(argv[j]) > 2)
		{
				cerr << "Illegal option: \"" << argv[j] << "\"" << endl;
				return 27;
		}

		switch (argv[j][1])
		{
			case 'u' :  if (u_flag > 0)		//  -u is set by alogset.sh script
						{
								cerr << "Illegal option: \"-u\"" << endl;
								return 27;

						}
						u_flag ++;  j ++;		//  j++ skips to the user name
						user = argv[j];
						break;

			case 'p' :  p_flag ++;
						totalOptions ++;
						break;

			case 'c' :	c_flag ++;
						totalOptions ++;
						break;

			case 'd' :  d_flag ++;  j ++;
						if (j >= argc)
						{
								printUsage();  return 2;
						}
						pDest = argv[j];
						totalOptions ++;
						break;

			case 'i' :	i_flag ++;  j ++;
						if (j >= argc)
						{
								printUsage();  return 2;
						}
						pIdTag = argv[j];
						totalOptions ++;
						break;

			case 'r' :	r_flag ++;
						totalOptions ++;
						break;

			default :	cerr << "Illegal option: \"-" << argv[j][1] << "\"" << endl;
						return 27;
		}
	}

	// alogset without parameter :  Incorrect Usage
	// --------------------------------------------
	if (c_flag == 0  &&  p_flag == 0  &&  d_flag == 0  &&  i_flag == 0  &&  r_flag == 0)
	{
			printUsage();
			return 2;
	}

	// The same parameter is used more times :  Incorrect Usage
	// --------------------------------------------------------
	if (c_flag > 1  &&  p_flag > 1  &&  d_flag > 1  &&  i_flag > 1  &&  r_flag > 1)
	{
			printUsage();
			return 2;
	}

	// Considering the Format 1 and Format 2, at most 2 options are allowed :  Incorrect Usage
	// ---------------------------------------------------------------------------------------
	if (totalOptions > 2)
	{
			printUsage();
			return 2;
	}

	if (p_flag == 0) 		//  -p must be present in both Formats
	{
			printUsage();
			return 2;
	}

	char cOldPass[101];
	char cNewPass[101];
	char cConfirmPass[101];
	acs_alog_execute CmdExecute;
	enumCmdExecuteResult ExecResult = cerSuccess;

	if (c_flag == 1)		//  Change Password
	{
		nReturn = acs_alog_cmdUtil::CheckALOGSysFile(false,true,true);

		if (nReturn != 0)  return nReturn;

		nReturn = acs_alog_cmdUtil::CheckAlogMain();	// Command not allowed if alog_main() server is not active

		if (nReturn != 0)  return nReturn;

		cout << "type the old password\03: " << flush;

		acs_alog_cmdUtil::getPassword(cOldPass,101);

		nReturn = acs_alog_cmdUtil::Authenticate(cOldPass);

		if (nReturn != 0)  return (nReturn);

		nReturn = acs_alog_cmdUtil::getNewConfirmPassword(cOldPass,cNewPass,cConfirmPass);

		if (nReturn != 0)  return (nReturn);

		if  (strcmp(cNewPass, cOldPass ) == 0 )			// test if new and old password are the same
		{
				cerr << endl << "Incorrect Password" << endl;
				return 39;
		}

		ExecResult = CmdExecute.Execute(ctSetNewPw, not_defined,  cOldPass,  cNewPass);
		nReturn = acs_alog_cmdUtil::HandleReturnCode(ExecResult);
		return(nReturn);
	}

	if (c_flag == 0  &&  d_flag == 0  &&  i_flag == 0  &&  r_flag == 0)   //  Set password for the first time
	{

		nReturn = acs_alog_cmdUtil::CheckALOGSysFile(true,true,true);

		if (nReturn != 0)  return nReturn;

		nReturn = acs_alog_cmdUtil::CheckAlogMain();		// Command not allowed if alog_main() server is not active

		if (nReturn != 0)  return nReturn;

		cOldPass[0] = 0;
		nReturn = acs_alog_cmdUtil::getNewConfirmPassword(cOldPass,cNewPass,cConfirmPass);

		if (nReturn != 0)  return(nReturn);

		enumCmdExecuteResult ExecResult = CmdExecute.Execute(ctSetPw,  not_defined, cNewPass);
		nReturn = acs_alog_cmdUtil::HandleReturnCode(ExecResult);

		if (nReturn == 0)
		{
			string lpszFile;

			acs_alog_cmdUtil::getAlogDataDiskPath(lpszFile);
			lpszFile += "/ACS_ALOG/ATTRFILEP";

			ACE_HANDLE hFile;
			ACS_ALOG_AttrFileRecord m_AttrFileRecord;
			(*m_AttrFileRecord.FileDest) = NULL;
			m_AttrFileRecord.IdTag = false;

			hFile = ACE_OS::open(lpszFile.c_str(),O_WRONLY | O_TRUNC);

			ACE_OS::write (hFile, (void*)&m_AttrFileRecord, sizeof(ACS_ALOG_AttrFileRecord));

			ACE_OS::close(hFile);
		}

		return(nReturn);
	}

	if (r_flag == 1) 		// Reset password
	{
		nReturn = acs_alog_cmdUtil::CheckALOGSysFile(false,true,true);

		if (nReturn != 0)  return nReturn;

		nReturn = acs_alog_cmdUtil::CheckAlogMain();

		if(nReturn != 0) return nReturn;

		if((!isMemberOfComEmergency(user)) && (!isMemeberOfTSGroup(user)))
		{
			char adminPassword[101];

			cout << "\nType System Security Administrator Password\03: " << flush;
			acs_alog_cmdUtil::getPassword(adminPassword,101);

			enumCmdExecuteResult  ExecResult = CmdExecute.Execute(ctAdminPasswordHandling, not_defined, adminPassword,user);
			nReturn = acs_alog_cmdUtil::HandleReturnCode(ExecResult);

			if (nReturn == 0)
			{
				cout << "\nAll protected MML items and protected log-files are removed." << endl;
				cout << "Are you sure you want to reset password of Protected Audit Log" << endl;
				cout << "Function? y or n [default: n]\03: " << flush;
				char sResponce[64+1];
				cin.getline(sResponce, 64);

				if (!(*sResponce) || strcasecmp(sResponce, "no") == 0 || strcasecmp(sResponce, "n") == 0)
				{
						return 0;
				}

				if (strcasecmp(sResponce, "yes") != 0 && strcasecmp(sResponce, "y") != 0)
				{
						cerr << "Illegal answer: \"" << sResponce << "\"" << endl;
						return 2;
				}

				char spare[10];
				enumCmdExecuteResult ExecResult = CmdExecute.Execute(ctResetPw, not_defined, (char*)spare);
				nReturn = acs_alog_cmdUtil::HandleReturnCode(ExecResult);
			}
		}
		else  nReturn = acs_alog_cmdUtil::HandleReturnCode(cerAuthFailure);

		return(nReturn);
	}

	if (i_flag  ||  d_flag)
	{
		string lpszFile;

		acs_alog_cmdUtil::getAlogDataDiskPath(lpszFile);
		lpszFile += "/ACS_ALOG/ATTRFILEP";

		ACE_stat statBuff;
		if( ACE_OS::stat(lpszFile.c_str(), &statBuff)  != 0 )
		{
			cerr << endl << "File access error" << endl ;
			return 22;
		}
	}

	if (i_flag)
	{
		int resIT, rITLen;
		rITLen = strlen(pIdTag);
		if (rITLen < 3)  rITLen = 3;

		resIT = strncmp(pIdTag, "yes", rITLen); 	//case-insensitive
		if (resIT)
		{
			rITLen = strlen(pIdTag);
			if (rITLen < 2)  rITLen = 2;

			resIT = strncmp(pIdTag, "no", rITLen);
			if (resIT)
			{
				cerr << endl << "Faulty idtaglogging value" << endl ;
				return 37;
			}
		}
	}

	if (d_flag)			// Check if file destination is already defined
	{
			nReturn = ValidateFileDestination(pDest );

			if (nReturn != 0)  return nReturn;
	}

	int retcode = acs_alog_cmdUtil::PasswordHandling();

	if (retcode == 29)  cerr << endl << "Failed to change Audit Log attributes " << endl;

	if (retcode != 0)  return(retcode);

	cout << "Are you sure you want to change attributes of Audit Log Function?" << endl;
	cout << "Enter y[es] or n[o] [default: n]\03: " << flush;

	char sResponce[64+1];
	cin.getline(sResponce, 64);

	if (!(*sResponce) || strcasecmp(sResponce, "no") == 0 || strcasecmp(sResponce, "n") == 0)
	{
			return 0;
	}

	if (strcasecmp(sResponce, "yes") != 0 && strcasecmp(sResponce, "y") != 0)
	{
			cerr << "Illegal answer: \"" << sResponce << "\"" << endl;
			return 2;
	}

	enumCmdType cmdToExe;

	if (d_flag == 1)
	{
			cmdToExe = ctSetFileDestProt;
			ExecResult = CmdExecute.Execute(cmdToExe, not_defined, pDest);
	}

	if (i_flag == 1)
	{
			cmdToExe = ctIdTagProt;
			ExecResult = CmdExecute.Execute(cmdToExe, not_defined, pIdTag);
	}

	switch (ExecResult)
	{
		case cerSuccess:
				cout << endl;
			break;

		case cerPwAlreadyDefined:
			cerr << endl <<  "Password already defined" << endl << endl;
			nReturn = 41;
			break;

		case cerPwNotDefined:
			cerr << endl << "Password not defined" << endl << endl;
			nReturn = 42;
			break;

		case cerItemAlreadyDefined:
			if (d_flag == 1)
			{
				cerr << endl << "Transfer Queue \"" << pDest << "\" already defined" << endl ;
				nReturn = 32;
			}
			if (i_flag == 1)
			{
				if (!strcasecmp(pIdTag, "yes"))
					cerr << endl <<  "Id tag logging is already active" << endl ;
				else
					cerr << endl <<  "Id tag logging is already deactive" << endl ;
				nReturn = 38;
			}
			break;

		case cerServerNotResponding:
			cerr << endl << "Unable to connect to server" << endl ;
			nReturn = 117;
			break;

		case cerFileOpenError:
		case cerFileReadError:
		case cerFileWriteError:
			cerr << endl << "File access error" <<  endl;
			nReturn = 22;
			break;

		case cerOtherError:
			nReturn = 0;
			break;

		case cerBackupInProgress:
			cerr<<"Command not executed, AP backup in progress"<<endl <<endl;
			nReturn = 114;
			break;

		default:
			cerr << endl << "Failed to change Audit Log attributes "  << endl;
			nReturn = 29;
			break;
	}

	return nReturn;
}
