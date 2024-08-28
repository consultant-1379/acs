
/*****************************************************************************
*
* COPYRIGHT Ericsson Telecom AB 2014
*
* The copyright of the computer program herein is the property of
* Ericsson Telecom AB. The program may be used and/or copied only with the
* written permission from Ericsson Telecom AB or in the accordance with the
* terms and conditions stipulated in the agreement/contract under which the
* program has been supplied.
*
----------------------------------------------------------------------*/
/**
*
* @file acs_bur_burbackup.cpp
*
* @brief
* Main file for creating backup
*
* @details
* Parses the arguments passed to the burbackup command and then
* creates and/or exports the backup with dispalying progess information
*
* @author
*
-------------------------------------------------------------------------*/
/*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 01/12/2011     EGIMARR       Initial Release
 * 13/02/2014     XVENJAM       Fix for TRs HS38315, HS38252 & cpp check error correction
 * 11/09/2018     ZGXXNAV       Fix for TR HX17971
 * 12/04/2019     ZSURMAN       Added Implemantation for Secure Backup
 * 03/08/2023     XSRAVAN	Added internal option -r for MSC Backup Feature
 ****************************************************************************/
/* * Modified on: 3 Aug 2023
 *       Author: xsravan
 * Modified on: 12 Apr 2019
 *       Author: zsurman
 *  Modified on: 16 Jun 2014
 *      Author: xfabpag
 *  Modified on: 2 Apr 2014
 *      Author: xquydao
 *  Modified on: 26 Feb 2014
 *      Author: xfabpag
 *  Created on: 1 Dec 2011
 *      Author: egimarr
 */

#include "acs_bur_Define.h"
#include "acs_bur_burbackup.h"
#include "acs_bur_BrmAsyncActionProgress.h"
#include "acs_bur_BrmBackupManager.h"
#include "acs_bur_BrmBackup.h"
#include "ACS_TRA_trace.h"
#include "acs_bur_trace.h"
#include <ace/ACE.h>
#include "ACS_TRA_Logging.h"
#include "ACS_APGCC_CommonLib.h"
#include "ACS_APGCC_CLibTypes.h"
#include "ACS_CS_API.h"
#include "acs_prc_api.h"
#include "acs_bur_util.h"

#include <stdlib.h>
#include <sys/quota.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <sys/mount.h>

#define BURBACKUP_ERROR 0xffffffffffffLL
#define KILOBYTE (1024ULL)
#define MEGABYTE (KILOBYTE * KILOBYTE)
#define BURBACKUP_SIZE (400 * MEGABYTE)
#define ACS_SSU_DATA_BLOCK_DEVICE "/dev/md0"
#define SSU_QUOTASTRING "grpjquota"
#define SSU_ACS_DATAPATH "/data"
extern bool is_swm_2_0;
string getBackupver(string swVersion, bool isInternal);
acs_bur_burbackup::acs_bur_burbackup(int argc, char *argv[]) : m_argc_(argc), m_argv_(argv)
{
	m_burb = new ACS_TRA_trace("acs_bur_burbackup");
	TRACE(m_burb, "%s", "acs_bur_burbackup::acs_bur_burbackup()");
	m_burLog = new ACS_TRA_Logging();
	m_burLog->Open("BUR");
	setOpCode(NOERROR);
	// Init private attributes
	m_errorMessage = "";
	m_dnBrmBackupManager = "";
	m_dnBrmBackup = "";
	m_backupFileName = "";
	m_only_export = true;
	m_archive_filename = "";
	m_labelName = "";
	m_backup_nodename = "";
	m_backupCreationTime = "";
	m_szBlockDevice.clear();
	m_isSecured = false; // anssi
	m_backupPasswd = "";
	m_isInternal = false;
	m_newBackupName = "";
}
//
// Printout handler and returns specific exit code
//
int acs_bur_burbackup::error_message_handler()
{
	char msg[256];
	int code = getOpCode();
	TRACE(m_burb, "%s %d", "error_messages_handler() internal code error: ", code);
	switch (code)
	{
	case NOERROR:
		// OK Successful Execution command
		if (false == m_only_export)
		{
			if (true == m_isSecured)
				cout << MSG_SECBKPSUCC << endl;
			else
				cout << MSG_BKPSUCC << endl;
		}
		return 0;

	case FORCEDQUIT:
		// OK Forced to quit forced exit (n)
		return 0;

	case GENERALFAULT:
		sprintf(msg, MSG_GENERAL, m_errorMessage.c_str());
		cout << msg << endl;
		return 1;

	case SYNTAXERR:
		// KO Incorrect Usage print usage
		cout << MSG_INCUSA << endl;
		print_usage();
		return 2;

	case OPNOTACEPT:
		// KO BRF not available
		cout << MSG_SRVBUSY << endl;
		return 3;

	// Are handled the same but we are generate by different paths
	case DONTEXIST:
	case INVALIDIMAGE:
		cout << MSG_DONTEXIST << endl;
		return 4;

	case CHGCUROP:
		// KO Operation was been Abort
		cout << MSG_OPABORT << endl;
		return 5;

	case INITERROR:
	case GETATTRERROR:
	case FINALIZEERROR:
	case BRMISTABS:
	case ERRHOSTNAME:
	case EXPFAIL:
	case POPENERR:
	case SETUIDERR:
	case EXTALFNUM:
	case LDEBRFEXPERR:
		// Internal fault : < detailed description >
		cout << MSG_INTFAULT << m_errorMessage.c_str() << endl;
		return 6;

	case BACKUPFAIL:
	case BACKUPINCOMPLETE:
	case BACKUPNOTEXIST:
		cout << MSG_BACKUP_FAIL << endl;
		return 7;

	case INVOPTION:
		// KO Invalid option
		cout << MSG_ILLOPT << m_errorMessage.c_str() << endl;
		return 8;

	/* case PARINVNAME:
		char msg[256];
		sprintf(msg,MSG_IMGNAMNOTVLD,m_errorMessage.c_str());
		cout << msg << endl;
		return 9; */

	// Errors on lde-brf export command
	case NOSPACELEFT:
		cout << MSG_NOSPACELEFT << endl;
		return 10;

	case OPEINPROG:
		cout << MSG_EXPCONF << endl;
		return 11;

	case ALREADYEXIST:
		cout << MSG_ALREADYEXIST << endl;
		return 12;
	case USERNOTALLOWED:
		cout << MSG_USERNOTALLOWED << endl;
		return 14;
	// BRF Busy
	case INVOKEERR:
	case BRFCONFLICT:
		cout << MSG_BRF_CONFLICT << endl;
		return 15;

	case PWDMISMATCH:
		cout << endl
			 << MSG_PWDMISMATCH << endl;
		return 16;
	case INVALIDPWD:
		cout << endl
			 << MSG_INVALIDPWD << endl;
		return 17;
	case INCORRECTPWD:
		cout << endl
			 << MSG_INCORRECTPWD << endl;
		return 18;
	}

	return 0;
}

int acs_bur_burbackup::getOpCode()
{
	return m_opCode;
}

void acs_bur_burbackup::setOpCode(int code)
{
	m_opCode = code;
	m_errorMessage = "";
}

void acs_bur_burbackup::setOpCode(int code, string mes)
{
	m_opCode = code;
	m_errorMessage = mes;
}

// Printing data info
void acs_bur_burbackup::print_backup_info(string backupFileName, string backupCreationTime)
{
	cout << MSG_SYSBCKSUCC << endl;
	cout << MSG_INFORPT << endl
		 << endl;
	cout << "Imagename: " << backupFileName.c_str() << endl;
	cout << "Creation date: " << backupCreationTime.c_str() << endl;
	cout << endl;
}

int acs_bur_burbackup::print_archive_info(string backupFileName, string *backupCreationTime)
{
	int errorCode;
	string message = "";
	string dnBrmBackup = "";
	// Get DN of BrmBackup according to backupFileName
	if (acs_bur_util::getBrmBackupDN(&dnBrmBackup, backupFileName, &errorCode, &message) == RTN_FAIL)
	{
		m_burLog->Write("print_archive_info: failed to retrieve DN of BrmBackup", LOG_LEVEL_DEBUG);
		setOpCode(errorCode, message);
		return RTN_FAIL;
	}

	acs_bur_BrmBackup *backup = new acs_bur_BrmBackup((char *)dnBrmBackup.c_str());

	if (backup->getOpCode() != NOERROR)
	{
		m_burLog->Write("print_archive_info: acs_bur_BrmBackup() failed ", LOG_LEVEL_DEBUG);
		setOpCode(INVALIDIMAGE);
		delete backup;
		return RTN_FAIL;
	}

	*backupCreationTime = backup->getCreationTime();
	delete backup;

	cout << MSG_INFOEXPORT << endl
		 << endl;
	cout << "Imagename: " << backupFileName.c_str() << endl;
	cout << "Creation date: " << (*backupCreationTime).c_str() << endl;
	cout << endl;

	return RTN_OK;
}

//
void acs_bur_burbackup::print_usage()
{
	cout << "Usage:" << endl;
	cout << " burbackup -o [-p -l userlabel] [-f]" << endl;
	cout << " burbackup -a imagename [-f]" << endl;
}
int acs_bur_burbackup::parseForInternalUse(int argc_, char **argv_, string *newBackupName, bool *onlyExport)
{
	TRACE(m_burb, "%s", "Enter in function: parse()");
	*newBackupName = "";
	*onlyExport = false;
	for (int n = 1; n < argc_; n++)
	{
		if (strcmp(argv_[n], "-r") == 0)
		{
			TRACE(m_burb, "%s", "parse() found -r option ");
			m_isInternal = true;
			if ((argc_ >= (n + 2)))
			{
				if (argv_[n + 1][0] == '-')
				{
					TRACE(m_burb, "%s", "parse() -r with an option instead of backupName value");
					setOpCode(SYNTAXERR);
					return RTN_FAIL;
				}
				else
				{
					TRACE(m_burb, "%s", " found backupName  value for  -r  option ");
					*newBackupName = argv_[n + 1];
				}
			}
			else
			{
				TRACE(m_burb, "%s", "parse() -r without backupName  value ");
				setOpCode(SYNTAXERR);
				return RTN_FAIL;
			}
		}
	}
	return RTN_OK;
}
// This method return RTN_OK   if command is correctly parsering
//					  RTN_FAIL error or parse interrupted
int acs_bur_burbackup::parse(int argc_, char **argv_, string *archiveFileName, string *labelName, bool *onlyExport, string *backupPasswd, string *newBackupName)
{
	int num_o = 0;
	int num_f = 0;
	int num_a = 0;
	int num_filename = 0;
	int num_p = 0;
	int num_l = 0;
	int num_labelname = 0;
	int num_newBackup = 0;

	TRACE(m_burb, "%s", "Enter in function: parse()");
	*archiveFileName = "";
	*labelName = "";
	*backupPasswd = "";
	*newBackupName = "";

	for (int n = 1; n < argc_; n++)
	{
		if (strcmp(argv_[n], "-o") == 0)
		{
			num_o++;
			TRACE(m_burb, "%s", "parse() found -o option");
		}
		else if (strcmp(argv_[n], "-p") == 0)
		{
			num_p++;
			m_isSecured = true;
			TRACE(m_burb, "%s", "parse() found -p option");
			if ((argc_ >= 5))
			{
				continue;
			}
			else
			{
				for (int x = 1; x < argc_; x++)
				{
					if (strcmp(argv_[x], "-o") == 0)
					{
						if ((argc_ == (n + 1)))
						{
							TRACE(m_burb, "%s", "parse() -p without -l option");
							setOpCode(SYNTAXERR);
							return RTN_FAIL;
						}
						if ((argc_ == (n + 2)))
						{
							TRACE(m_burb, "%s", "parse() -l without userlabel value ");
							setOpCode(SYNTAXERR);
							return RTN_FAIL;
						}
					}
					else
					{
						TRACE(m_burb, "%s", "parse() -o option not found");
						setOpCode(SYNTAXERR);
						return RTN_FAIL;
					}
				}
			}
		}
		else if (strcmp(argv_[n], "-l") == 0)
		{
			num_l++;
			TRACE(m_burb, "%s", "parse() found -l option ");
			if ((argc_ >= (n + 2)))
			{
				if (argv_[n + 1][0] == '-')
				{
					TRACE(m_burb, "%s", "parse() -l with an option instead of userlabel value");
					setOpCode(SYNTAXERR);
					return RTN_FAIL;
				}
				else
				{
					TRACE(m_burb, "%s", " found user label value for  -l option ");
					num_labelname++;
					*labelName = argv_[n + 1];
					n++;
				}
			}
			else
			{
				TRACE(m_burb, "%s", "parse() -l without userlabel value ");
				setOpCode(SYNTAXERR);
				return RTN_FAIL;
			}
		}
		else if (strcmp(argv_[n], "-r") == 0)
		{
			TRACE(m_burb, "%s", "parse() found -r option ");
			m_isInternal = true;
			if ((argc_ >= (n + 2)))
			{
				if (argv_[n + 1][0] == '-')
				{
					TRACE(m_burb, "%s", "parse() -r with an option instead of backupName value");
					setOpCode(SYNTAXERR);
					return RTN_FAIL;
				}
				else
				{
					TRACE(m_burb, "%s", " found backupName  value for  -r  option ");
					*newBackupName = argv_[n + 1];
					num_newBackup++;
					n++;
				}
			}
			else
			{
				TRACE(m_burb, "%s", "parse() -r without backupName  value ");
				setOpCode(SYNTAXERR);
				return RTN_FAIL;
			}
		}
		else if (strcmp(argv_[n], "-f") == 0)
		{
			num_f++;
			TRACE(m_burb, "%s", "parse() found -f option ");
		}
		else if (strcmp(argv_[n], "-a") == 0)
		{
			num_a++;
			TRACE(m_burb, "%s", "parse() found -a option ");
			if ((argc_ == (n + 1)))
			{
				TRACE(m_burb, "%s", "parse() -a without <backupname> ");
				setOpCode(SYNTAXERR);
				return RTN_FAIL;
			}
			else
			{
				if (argv_[n + 1][0] == '-')
				{
					TRACE(m_burb, "%s", "parse() -a with an option instead of backupname");
					setOpCode(SYNTAXERR);
					return RTN_FAIL;
				}
			}
		}
		else if (argv_[n][0] == '-')
		{
			setOpCode(INVOPTION, string(argv_[n]));
			return RTN_FAIL;
		}
		else if (num_filename == 1)
		{
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}
		else
		{
			num_filename++;
			*archiveFileName = argv_[n];
		}

		if ((num_f > 1) || (num_o > 1) || (num_a > 1) || (num_filename > 1) || (num_p > 1) || (num_l > 1) || (num_labelname > 1) || (num_newBackup > 1))
		{
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}
	}
	// option -o -a together
	if (!m_isInternal)
	{
		if ((1 == num_o) && (1 == num_a))
		{
			m_burLog->Write("parse(): -o -a options not are valid", LOG_LEVEL_DEBUG);
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}

		// option -o and -a not present
		if ((0 == num_o) && (0 == num_a))
		{
			m_burLog->Write("parse(): -o or -a option must be present", LOG_LEVEL_DEBUG);
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}

		// -o option with filename
		if ((1 == num_o) && (1 == num_filename))
		{
			m_burLog->Write("parse(): -o with filename not is valid", LOG_LEVEL_DEBUG);
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}

		// command with filename without -a
		if ((num_filename == 0) && (num_a == 1))
		{
			setOpCode(SYNTAXERR);
			m_burLog->Write("parse(): <filename> without -a option not is valid", LOG_LEVEL_DEBUG);
			return RTN_FAIL;
		}

		// command with -f without -o OR without -a
		if ((1 == num_f) && (0 == num_a) && (0 == num_o))
		{
			setOpCode(SYNTAXERR);
			m_burLog->Write("parse(): -f without -o OR without -a is not valid", LOG_LEVEL_DEBUG);
			return RTN_FAIL;
		}
		// command without -p and with -l
		if ((0 == num_p) && (1 == num_l))
		{
			m_burLog->Write("parse(): -l without -p option is not  valid", LOG_LEVEL_DEBUG);
			setOpCode(SYNTAXERR);
			return RTN_FAIL;
		}

		bool answer = true;

		if (0 == num_f)
		{

			if (1 == num_o)
			{

				if (1 == num_p)
				{
					std::cout << MSG_EXEBBACK << std::endl;
					std::cout << "burbackup -o -p -l"
							  << " " << (*labelName).c_str() << std::endl;
				}
				else
				{
					std::cout << MSG_EXEBBACK << std::endl;
					std::cout << "burbackup -o" << std::endl;
				}
			}
			else
			{
				std::cout << MSG_EXEARCH << std::endl;
				cout << "burbackup -a " << (*archiveFileName).c_str() << std::endl;
			}

			string question = "";

			answer = acs_bur_util::affirm(question);
		}

		if (!answer)
		{
			TRACE(m_burb, "%s", "parse(): Quit by command, no execution.");
			setOpCode(FORCEDQUIT);
			return RTN_FAIL; //
		}
	}
	*onlyExport = false;

	if (1 == num_a)
		*onlyExport = true;
	if (1 == num_p)
	{

		cout << "Password\03: " << flush;
		string backupPass = acs_bur_util::getPassword();
		if (backupPass.length() == 0)
		{
			setOpCode(INCORRECTPWD);
			return RTN_FAIL;
		}

		string str = backupPass;
		int _len = str.length();
		char backup_arr[_len + 1];
		int count = 0;
		strcpy(backup_arr, str.c_str());

		for (int i = 0; i < _len; i++)
		{
			if (backup_arr[i] == ' ')
			{
				count++;
				if (count == _len)
				{
					setOpCode(INCORRECTPWD);
					return RTN_FAIL;
				}
			}
		}
		*backupPasswd = backupPass;

		if (acs_bur_util::getNewConfirmPassword(backupPass) != true)
		{
			setOpCode(PWDMISMATCH);
			return RTN_FAIL;
		}
	}

	return RTN_OK;
}

// get Network Element Identification to build nodename
int acs_bur_burbackup::getNEID(string *id)
{
	char nodename[256];

	memset((void *)nodename, 0, sizeof(nodename));

	ACS_CS_API_Name_R1 neid;

	ACS_CS_API_NS::CS_API_Result returnValue;

	returnValue = ACS_CS_API_NetworkElement::getNEID(neid);

	if (returnValue == ACS_CS_API_NS::Result_Failure)
	{
		m_burLog->Write("acs_bur_burbackup::getNEID() error on ACS_CS_API_NetworkElement::getNEID() function", LOG_LEVEL_ERROR);
		setOpCode(ERRHOSTNAME, "ACS_CS_API_NetworkElement::getNEID() FAILED");
		return RTN_FAIL;
	}
	size_t name_length = sizeof(nodename);

	returnValue = neid.getName(nodename, name_length);

	for (int i = 0; i < (int)strlen(nodename); i++)
	{
		TRACE(m_burb, "%c %d", nodename[i], i);
	}
	string sNodeName_tmp = nodename;
	string sNodeName;
	if (sNodeName_tmp.length() > MAXNODENAMELENGHT)
	{
		sNodeName = sNodeName_tmp.substr(0, MAXNODENAMELENGHT);
	}
	else
	{
		sNodeName = sNodeName_tmp;
	}
	int length = sNodeName.length();
	for (int i = 0; i < length; i++)
	{
		if (sNodeName[i] == '.')
			sNodeName[i] = '_';
	}

	// test if node name contains extra alfanumeric character
	if (false == acs_bur_util::validFileName(sNodeName))
	{
		m_burLog->Write("acs_bur_burbackup::getNEID() nodename is an invalid name (extra alfanumeric char)", LOG_LEVEL_ERROR);
		setOpCode(EXTALFNUM, "ACS_CS_API_NetworkElement::getNEID() nodename is an invalid name");
		return RTN_FAIL;
	}

	//
	if (0 == sNodeName.length())
	{
		TRACE(m_burb, "%s %s %d", "acs_bur_burbackup::getNEID() nodename is empty string ", sNodeName.c_str(), name_length);
		m_burLog->Write("acs_bur_burbackup::getNEID() returned an empty nodename renaming in BUR-BACKUP-", LOG_LEVEL_ERROR);
		sNodeName = "BUR-BACKUP";
		// strcpy(nodename ,"BUR-BACKUP");
	}

	TRACE(m_burb, "%s %s %d", "acs_bur_burbackup::getNEID() nodename: ", sNodeName.c_str(), name_length);

	// m_backup_nodename = nodename;
	*id = string(sNodeName);

	return RTN_OK;
}
//
// renameBackupName &m_backup_nodename &m_backupFileName
//

int acs_bur_burbackup::renameBackupName(string *backupFileName) //(string *node,string *name)
{

	string burbakcup_path = "/data/opt/ap/internal_root/backup_restore/";
	std::string final_backup_name = "";
	if (m_newBackupName != "")
	{
		std::string delimiter = "-";
		string taskstartTime = m_newBackupName;
		string m_domainName = "APG";
		string s = *backupFileName;
		int pos_start = s.find(delimiter);
		string a = s.substr(0, pos_start);
		string b = s.substr(pos_start, s.length());
		final_backup_name = final_backup_name + a + delimiter + taskstartTime + delimiter + m_domainName + b;
		if (rename((burbakcup_path + m_backupFileName).c_str(), (burbakcup_path + final_backup_name).c_str()) != 0)
		{
			m_burLog->Write("acs_bur_burbackup::renameBackupName() Failed to rename the original file)", LOG_LEVEL_ERROR);
			// TRACE(m_burb," acs_bur_burbackup: Failed while renaming the original burbackup filename with new");
			return RTN_FAIL;
		}
		m_backupFileName = final_backup_name;
	}

	return RTN_OK;
}
//
// makeBackupName &m_backup_nodename &m_backupFileName
//
int acs_bur_burbackup::makeBackupName(string *backupNodeName, string *backupFileName) //(string *node,string *name)
{
	time_t raw_time;

	TRACE(m_burb, "%s", "acs_bur_burbackup::makeBackupName() function called");
	ACE_OS::time(&raw_time);
	// localtime or gmtime
	m_backup_time = ACE_OS::localtime(&raw_time);

	// string neid_node;

	// if (RTN_FAIL == getNEID()  )
	if (RTN_FAIL == getNEID(backupNodeName))
		return RTN_FAIL;

	int maxLength = 13;
	std::string str = (*backupNodeName).c_str();
	if (m_isSecured == true)
	{
		if (str.length() >= maxLength)
		{
			str.resize(11);
		}
		*backupNodeName = str;
	}
	else
	{
		if (str.length() >= maxLength)
		{
			str.resize(13);
		}
		*backupNodeName = str;
	}

	string swVersion("");

	swVersion = getSwVersion();

	if (!swVersion.empty())
	{
		int length = swVersion.length();
		for (int i = 0; i < length; i++)
		{
			if (swVersion[i] == '.')
				swVersion[i] = '_';
		}
	}

	char filename[100];
	if (swVersion.empty())
	{
		m_burLog->Write("acs_bur_burbackup::getSwVersion could not get SwVersion", LOG_LEVEL_ERROR);

		sprintf(filename, "%s-%4d-%2.2d-%2.2dT%2.2d-%2.2d-%2.2d",
				(*backupNodeName).c_str(),
				(m_backup_time->tm_year) + 1900,
				(m_backup_time->tm_mon) + 1,
				m_backup_time->tm_mday,
				m_backup_time->tm_hour,
				m_backup_time->tm_min,
				m_backup_time->tm_sec);
		if (m_isInternal)
		{
			string m_domainName = "AP";

			ACS_CS_API_CommonBasedArchitecture::ApgNumber APtype;
			ACS_CS_API_NetworkElement::getApgNumber(APtype);

			if (ACS_CS_API_CommonBasedArchitecture::AP2 == APtype)
			{
				m_domainName = "AP2";
			}
			sprintf(filename, "%s-%s-%s-%s",
					(*backupNodeName).c_str(), m_newBackupName.c_str(), m_domainName.c_str());

			// cout << "Internal filename:::" << filename;
		}
	}
	else
	{
		sprintf(filename, "%s-%s-%4d-%2.2d-%2.2dT%2.2d-%2.2d-%2.2d",
				(*backupNodeName).c_str(),
				swVersion.c_str(),
				(m_backup_time->tm_year) + 1900,
				(m_backup_time->tm_mon) + 1,
				m_backup_time->tm_mday,
				m_backup_time->tm_hour,
				m_backup_time->tm_min,
				m_backup_time->tm_sec);

		if (m_isInternal)
		{
			string m_domainName = "AP";

			ACS_CS_API_CommonBasedArchitecture::ApgNumber APtype;
			ACS_CS_API_NetworkElement::getApgNumber(APtype);

			if (ACS_CS_API_CommonBasedArchitecture::AP2 == APtype)
			{
				m_domainName = "AP2";
			}
			sprintf(filename, "%s-%s-%s-%s",
					(*backupNodeName).c_str(), m_newBackupName.c_str(), m_domainName.c_str(),
					swVersion.c_str());

			// cout << "Internal filename:::" << filename;
		}
	}

	TRACE(m_burb, "%s %s", "acs_bur_burbackup::makeBackupName() function called", filename);

	*backupFileName = string(filename);
	/*if(m_isInternal)
	{
		if(m_newBackupName != "")
		{
			std::string delimiter = "-";
				std::string token;
				string final_backup_name = "";
				string taskstartTime = m_newBackupName;
		string m_domainName="AP";
		string s  = *backupFileName;
				int pos_start = s.find(delimiter);
				string a = s.substr(0,pos_start);
				string b = s.substr(pos_start,s.length());
				final_backup_name=final_backup_name+a+delimiter+taskstartTime+delimiter+m_domainName+b;
				*backupFileName=final_backup_name;
		}

	}*/
	if (m_isSecured == true)
	{
		*backupFileName = *backupFileName + "-S";
	}

	return RTN_OK;
}

acs_bur_burbackup::~acs_bur_burbackup()
{
	TRACE(m_burb, "%s ", "acs_bur_burbackup::~acs_bur_burbackup() ");
	if (NULL != m_burb)
	{
		delete m_burb;
	}
	if (NULL != m_burLog)
	{
		delete m_burLog;
	}
}

int acs_bur_burbackup::export_backup(string backupFileName)
{
	//
	// Syntax lde-brf export -l labelname -t backuptype -f pathfilename
	//

	string defaultBackupRestorePath = "";

	ACS_APGCC_DNFPath_ReturnTypeT ret;
	char p_Path[256];
	memset((void *)p_Path, 0, (size_t)sizeof(p_Path));

	string p_FileMFuncName = "backupRestore";
	int p_Len = 0;

	char exportcmd[] = EXPORTCMD_FIRST;
	string exportCmd = exportcmd + backupFileName + EXPORTCMD_SECOND;

	ACS_APGCC_CommonLib cmLib;
	p_Len = (int)sizeof(p_Path) - 1;

	ACS_PRC_API objPRC;
	// To prevent errors due to lde-brf changes on the passive node the command is rejected immediately
	if (objPRC.askForNodeState() == 2)
	{
		setOpCode(LDEBRFEXPERR, "export fails no such file or directory");
		m_burLog->Write("acs_bur_burbackup::export_backup error No such file or directory on PASSIVE node", LOG_LEVEL_ERROR);
		return (RTN_FAIL);
	}

	ret = cmLib.GetFileMPath(p_FileMFuncName.c_str(), p_Path, p_Len);

	if (ret != ACS_APGCC_DNFPATH_SUCCESS)
	{
		char msg[256];
		sprintf(msg, "%s , error code %d", "export_backup:: Error calling GetFileMPath()", ret);
		setOpCode(EXPFAIL, msg);
		m_burLog->Write(msg, LOG_LEVEL_ERROR);
		return (RTN_FAIL);
	}

	acs_bur_util::clearCRLF(p_Path);
	defaultBackupRestorePath = p_Path;
	// Fix Defect def 044
	exportCmd = exportCmd + defaultBackupRestorePath + "/" + backupFileName + MSG_NOSTDERR;

	TRACE(m_burb, "%s %s", "acs_bur_burbackup::export_backup() launch: ", exportCmd.c_str());

	cout << MSG_STARTARC << endl;

	string outBuffer = string("");
	int result = acs_bur_util::do_command(exportCmd.c_str(), &outBuffer);

	TRACE(m_burb, "%s %s", "acs_bur_burbackup::export_backup() lde-brf output : ", outBuffer.c_str());

	if (RTN_OK != result)
	{
		if (SETUIDERR == result)
			setOpCode(result, "setuid() fail()");
		else
			setOpCode(result, "popen fail()");
		return (RTN_FAIL);
	}

	if (string::npos != outBuffer.find("in progress"))
	{
		setOpCode(OPEINPROG, "operation in progress");
		m_burLog->Write("acs_bur_burbackup::export_backup operation in progress", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

	// 99 code
	if (string::npos != outBuffer.find("already exists"))
	{
		setOpCode(ALREADYEXIST, "file already exists");
		m_burLog->Write("acs_bur_burbackup::export_backup error file archive already exists ", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

	// 2 code
	if (string::npos != outBuffer.find("does not exist"))
	{
		setOpCode(DONTEXIST, "file does not exist");
		m_burLog->Write("acs_bur_burbackup::export_backup error file does not exist", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

	// 1 code
	if (string::npos != outBuffer.find("No space left"))
	{
		setOpCode(NOSPACELEFT, "No space left on Device");
		m_burLog->Write("acs_bur_burbackup::export_backup error No space left on Device ", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}
	// No such file or directory
	if (string::npos != outBuffer.find("failed!"))
	{
		setOpCode(LDEBRFEXPERR, "export fails");
		m_burLog->Write("acs_bur_burbackup::export_backup error No such file or directory buckup_restore", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}

	cout << MSG_BKPSUCCOP << endl;

	return RTN_OK;
}

int acs_bur_burbackup::execute()
{
	TRACE(m_burb, "%s", "acs_bur_burbackup::execute() ");
	bool doforever = true;
	int errorCode = NOERROR;
	string message = "";
	try
	{

		do
		{
			// first parse for  command with -r option which is intended only for internal purpose
			if (RTN_FAIL == parseForInternalUse(m_argc_, m_argv_, &m_newBackupName, &m_only_export))
				break;

			// check for user - TR HQ69027
			if (!m_isInternal)
			{
				if (true == acs_bur_util::isTroubleShooterUser())
				{
					message = "Command not allowed in this session type";
					setOpCode(USERNOTALLOWED, message);
					m_burLog->Write("acs_bur_burbackup::execute is not allowed for TS user group", LOG_LEVEL_ERROR);
					break;
				}
			}

			// parse command with option
			if (RTN_FAIL == parse(m_argc_, m_argv_, &m_archive_filename, &m_labelName, &m_only_export, &m_backupPasswd, &m_newBackupName))
				break;

			// check MOM and If action in progress
			if (RTN_FAIL == acs_bur_util::getBBMInstance(&m_dnBrmBackupManager, &errorCode, &message)) // or use findBBMInstance();
			{
				m_burLog->Write("acs_bur_burbackup::execute getBBMInstance() failed", LOG_LEVEL_ERROR);
				TRACE(m_burb, "%s %d", "acs_bur_burbackup::execute getBBMInstance() failed with error code: ", errorCode);
				if (m_only_export && (errorCode == OPNOTACEPT || errorCode == BRFCONFLICT))
				{
					TRACE(m_burb, "%s %d", "acs_bur_burbackup::execute getBBMInstance() failed only export: ", OPEINPROG)
					setOpCode(OPEINPROG, message);
				}
				else
					setOpCode(errorCode, message);

				break;
			}
			// Check whether is there enough space in the quota for the exporting
			if (RTN_FAIL == isThereEnoughSpace())
				break;

			// Complete backup is required
			if (false == m_only_export)
			{
				// prepare BackupName information
				if (RTN_FAIL == makeBackupName(&m_backup_nodename, &m_backupFileName)) // (&m_backup_nodename,&m_backupFileName))
					break;
				// Delete the already existing backup if same backup already exixts in case on internal option
				if (m_isInternal)
				{
					
					if (RTN_OK == acs_bur_util::getImmInstances( "BrMBrmBackup",m_backupFileName))
					{
						
						m_burLog->Write("acs_bur_burbackup::execute getBBInstance() ", LOG_LEVEL_INFO);
						// Delete the backup if it already exixts
						if (RTN_FAIL == acs_bur_util::invokeAction(DELETEBACKUP, m_dnBrmBackupManager, m_backupFileName, &errorCode, &message, m_isSecured, m_backupPasswd, m_labelName))
						{

							setOpCode(errorCode, message);
							m_burLog->Write("acs_bur_burbackup::execute InvokeAction() failed", LOG_LEVEL_ERROR);
							break;
						}
						// waiting until delete operation was finished or cancelled and state is SUCCESS or FAILED
						if (RTN_FAIL == acs_bur_util::waitForTeminate(DELETEBACKUP, m_dnBrmBackupManager, &m_backupCreationTime, m_backupFileName, &errorCode, &message))
						{
							m_burLog->Write("acs_bur_burbackup::execute waitForTeminate(CREATEBACKUP..) failed", LOG_LEVEL_ERROR);
							setOpCode(errorCode, message);
							break;
						}
					}

					m_burLog->Write("acs_bur_burbackup::execute Deleting fie in NBI", LOG_LEVEL_INFO);
					string fileName = "/data/opt/ap/internal_root/backup_restore/" + m_backupFileName;
					m_burLog->Write("acs_bur_burbackup::execute fileName:", LOG_LEVEL_INFO);
					FILE *file;
					file = fopen(fileName.c_str(), "r");
					if (file)
					{
						fclose(file);
						remove(fileName.c_str());
						
					}
				}

				// invoke createBackup action

				if (RTN_FAIL == acs_bur_util::invokeAction(CREATEBACKUP, m_dnBrmBackupManager, m_backupFileName, &errorCode, &message, m_isSecured, m_backupPasswd, m_labelName))
				{

					setOpCode(errorCode, message);
					m_burLog->Write("acs_bur_burbackup::execute InvokeAction() failed", LOG_LEVEL_ERROR);
					break;
				}
				// waiting until create operation was finished or cancelled and state is SUCCESS or FAILED
				if (RTN_FAIL == acs_bur_util::waitForTeminate(CREATEBACKUP, m_dnBrmBackupManager, &m_backupCreationTime, m_backupFileName, &errorCode, &message))
				{
					m_burLog->Write("acs_bur_burbackup::execute waitForTeminate(CREATEBACKUP..) failed", LOG_LEVEL_ERROR);
					setOpCode(errorCode, message);
					break;
				}
			}
			else
			{

				TRACE(m_burb, "execute: m_archive_filename = %s ", m_archive_filename.c_str());

				// Prepare printout only for export (burbackup -a)
				m_backupFileName = m_archive_filename;

				m_dnBrmBackup = string("");
				// get BrmBackup instance if exist
				// int getBBInstance(string archiveFileName,string *dnBrmBackup);
				if (RTN_FAIL == acs_bur_util::getBBInstance(m_archive_filename, &m_dnBrmBackup, &errorCode, &message))
				{
					m_burLog->Write("acs_bur_burbackup::execute getBBInstance() failed", LOG_LEVEL_ERROR);
					setOpCode(errorCode, message);
					break;
				}
			}

			// Printout handling after export operation successful
			if (!m_isInternal)
			{
				if (false == m_only_export)
				{
					// printout backup information
					print_backup_info(m_backupFileName, m_backupCreationTime);
					// print_backup_info_sec(m_securityBackupFileName,m_backupCreationTime); //anssi feature requirement
				}
				else
				{
					// printout archive information
					if (RTN_FAIL == print_archive_info(m_backupFileName, &m_backupCreationTime))
						break;
				}
			}
			// export_backup
			if (RTN_FAIL == export_backup(m_backupFileName))
				break;
			// tracing process for debug
			if (!m_isInternal)
			{
				TRACE(m_burb, "%s DN: %s NAME: %s", "acs_bur_burbackup::execute() starting invokation action to delete object backup ", m_dnBrmBackupManager.c_str(), m_backupFileName.c_str());
				// // invoke deleteBackup action for removal object in BRM
				if (RTN_FAIL == acs_bur_util::invokeAction(DELETEBACKUP, m_dnBrmBackupManager, m_backupFileName, &errorCode, &message))
				{
					setOpCode(errorCode, message);
					m_burLog->Write("acs_bur_burbackup::execute InvokeAction(DELETEBACKUP...) failed", LOG_LEVEL_ERROR);
					break;
				}
				// // tracing process for debug
				TRACE(m_burb, "%s DN: %s NAME: %s", "acs_bur_burbackup::execute() starting waitForTeminate action to delete object backup ", m_dnBrmBackupManager.c_str(), m_backupFileName.c_str());

				// // waiting until delete backup operation was finished or cancelled and state is SUCCESS or FAILED
				if (RTN_FAIL == acs_bur_util::waitForTeminate(DELETEBACKUP, m_dnBrmBackupManager, &m_backupCreationTime, m_backupFileName, &errorCode, &message))
				{
					m_burLog->Write("acs_bur_burbackup::execute waitForTeminate(DELETEBACKUP...) failed", LOG_LEVEL_ERROR);
					setOpCode(errorCode, message);
					break;
				}
			}
			// If the burbackup command is used for internal use then rename the backup file to new format
			if (m_isInternal)
			{

				// if(RTN_FAIL == renameBackupName(&m_backupFileName)) // (&m_backup_nodename,&m_backupFileName))
				// break;
				if (false == m_only_export)
				{
					// printout backup information

					print_backup_info(m_backupFileName, m_backupCreationTime);
					// print_backup_info_sec(m_securityBackupFileName,m_backupCreationTime); //anssi feature requirement
				}
				else
				{
					// printout archive information
					if (RTN_FAIL == print_archive_info(m_backupFileName, &m_backupCreationTime))
						break;
				}
			}
			doforever = false;

		} while (doforever);

	} // end try
	catch (exception &e)
	{
		m_burLog->Write("acs_bur_burbackup::execute an exception was raised", LOG_LEVEL_ERROR);
		setOpCode(GENERALFAULT, e.what());
	}

	return error_message_handler();
}

string acs_bur_burbackup::getSwVersion()
{
	/*
	 * Modified by XFABPAG for introduction of multiple application's SWVersion - 2014-06-16
	 * Modified by XFABPAG for TR HS29063 Fix - 2014-02-26
	 * 	- Changed the IMM class name from "SwInventory" to "CmwSwIMSwInventory" after CoreMW IMM change
	 * 	- Improved robustness
	 */
	bool swVersionFound = false;
	size_t i = 0, j = 0, pos1 = 0, pos2 = 0;
	std::vector<std::string> dnListSwInventory;
	string finalSwVersion("");
	string SwInventoryId("");
	string swVersion("");
	OmHandler myOmHandler;
	ACS_CC_ImmParameter paramToFind;
	paramToFind.attrName = const_cast<char *>("active");

	if (myOmHandler.Init() == ACS_CC_SUCCESS)
	{
		if ((myOmHandler.getClassInstances("SwInventory", dnListSwInventory) == ACS_CC_SUCCESS) ||
			(myOmHandler.getClassInstances("CmwSwIMSwInventory", dnListSwInventory) == ACS_CC_SUCCESS))
		{
			for (i = 0; !swVersionFound && i < dnListSwInventory.size(); i++)
			{
				SwInventoryId = dnListSwInventory[i];
				if (ACS_CC_SUCCESS == myOmHandler.getAttribute(SwInventoryId.c_str(), &paramToFind))
				{
					for (j = 0; !swVersionFound && j < paramToFind.attrValuesNum; j++)
					{
						swVersion = string(reinterpret_cast<char *>(paramToFind.attrValues[j]));
						if (!swVersion.empty())
						{
							/* TR HX17971 fix */
							if (is_swm_2_0 == true)
							{
								if (std::string::npos != swVersion.find("APG43L")) // TR HX28336
								{
									finalSwVersion = getBackupver(swVersion, m_isInternal);
									swVersionFound = true;
								}
							}
							else
							{
								pos1 = swVersion.find("=");
								if (std::string::npos != pos1)
								{
									string swVersionValue = swVersion.substr(pos1 + 1);
									if (0 == swVersionValue.find("APG43L"))
									{
										pos2 = swVersion.find(",");
										if (std::string::npos != pos2)
										{
											finalSwVersion = swVersion.substr(pos1 + 4, pos2 - pos1 - 4);
											swVersionFound = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
			m_burLog->Write("acs_bur_burbackup::getSwVersion failed to get dn for SwInventory", LOG_LEVEL_ERROR);
	}
	else
		m_burLog->Write("acs_bur_burbackup::getSwVersion failed to initialize OmHandler", LOG_LEVEL_ERROR);

	if (myOmHandler.Finalize() != ACS_CC_SUCCESS)
		m_burLog->Write("acs_bur_burbackup::getSwVersion failed to finalize OmHandler", LOG_LEVEL_ERROR);

	if (finalSwVersion.empty())
		m_burLog->Write("acs_bur_burbackup::getSwVersion found invalid swVersionId", LOG_LEVEL_ERROR);

	return finalSwVersion;
}
string getBackupver(string swVersion, bool isInternal)
{
	string productName, productIdentity, productRevision, ver;
	size_t posSystemName, posSystemNameEnd, posSystemNameEnds, posRevisionNameEnd, posSystemNameMid1, posSystemNameMid2, posRstateindex;
	posSystemName = swVersion.find("@");
	posSystemNameEnd = swVersion.find("-");
	posSystemNameEnds = swVersion.find("-", posSystemNameEnd + 1, 1);
	posRevisionNameEnd = swVersion.find(",");

	posSystemNameMid1 = posSystemNameEnd - posSystemName;
	posSystemNameMid2 = posSystemNameEnds - posSystemNameEnd;
	posRevisionNameEnd = posRevisionNameEnd - posSystemNameEnds;
	productName = swVersion.substr(++posSystemName, --posSystemNameMid1);
	productIdentity = swVersion.substr(++posSystemNameEnd, --posSystemNameMid2);
	productRevision = swVersion.substr(++posSystemNameEnds, --posRevisionNameEnd);

	posRstateindex = productIdentity.find_last_of("R");
	if (std::string::npos != posRstateindex)
	{
		productIdentity = productIdentity.substr(posRstateindex, productIdentity.size());
	}
	ver += swVersion.substr(posSystemName + 3, 3) + "-" + productRevision + "-" + productIdentity;
	if (isInternal)
	{

		ver = "";
		ver += productRevision + "-" + productIdentity;
	}
	return ver;
}

/*! @brief Calculates whether is there enough space in the quota for exporting
 *
 * @return          RTN_OK if there is space, RTN_FAIL otherwise
 */
int acs_bur_burbackup::isThereEnoughSpace(void)
{
	ACE_UINT64 u64QuotaSize;
	ACE_UINT64 u64QuotaLimitSize;

	if (!calculateQuotas("/data/opt/ap/internal_root/backup_restore/", u64QuotaSize, u64QuotaLimitSize))
	{
		char msg[128];
		sprintf(msg, "%s", "export_backup:: Error calling calculateQuotas()");
		setOpCode(POPENERR, msg);
		m_burLog->Write(msg, LOG_LEVEL_ERROR);
		return (RTN_FAIL);
	}
	if ((BURBACKUP_SIZE + u64QuotaSize) > u64QuotaLimitSize)
	{
		setOpCode(NOSPACELEFT);
		m_burLog->Write("acs_bur_burbackup::isThereEnoughSpace Error: Not enough space on data disk ", LOG_LEVEL_ERROR);
		return RTN_FAIL;
	}
	return RTN_OK;
}

/*! @brief Calculates the size of the passed folder and subfolders
 *
 * Currently not used function
 *
 * @param[in] 	dirName		The name of the folder to calculate
 * @return 							  The folder size in bytes
 */
uint64_t acs_bur_burbackup::calculateFolderSize(string dirName)
{
	DIR *dir;
	struct dirent *ent;
	struct stat st;
	char buf[PATH_MAX];
	uint64_t totalsize = 0LL;

	if (!(dir = opendir(dirName.c_str())))
	{
		TRACE(m_burb, " acs_bur_burbackup::calculateFolderSize(): couldn't open %s: %s\n", dirName.c_str(), strerror(errno));
		return BURBACKUP_ERROR;
	}
	while ((ent = readdir(dir)))
	{
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;

		sprintf(buf, "%s/%s", dirName.c_str(), ent->d_name);

		if (-1 == lstat(buf, &st))
		{
			TRACE(m_burb, "Couldn't stat %s: %s\n", buf, strerror(errno));
			continue;
		}
		if (S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode))
		{
			uint64_t dirsize;

			totalsize += st.st_size; // The size of the directory object (0 if empty)
			if (BURBACKUP_ERROR == (dirsize = calculateFolderSize(buf)))
				break;

			totalsize += dirsize;
		}
		else if (S_ISREG(st.st_mode)) // Only regular files
		{
			totalsize += st.st_size;
		}
	}
	closedir(dir);

	return totalsize;
}

/*! @brief Returns the Block device associated to the current folder
 *
 * Function cloned from SSU
 *
 * @return 								The device
 */
string acs_bur_burbackup::szGetBlockDevice(void)
{
	if (m_szBlockDevice.empty())
	{
		FILE *fp = NULL;
		struct mntent *mnt;

		if ((fp = setmntent(MOUNTED, "r")) != NULL)
		{
			while ((mnt = getmntent(fp)) != NULL)
			{
				if (hasmntopt(mnt, SSU_QUOTASTRING) && (ACE_OS::strcmp(SSU_ACS_DATAPATH, mnt->mnt_dir) == 0))
				{
					m_szBlockDevice = mnt->mnt_fsname;
					break;
				}
			}
		}
		if (m_szBlockDevice.empty())
		{
			TRACE(m_burb, " acs_bur_burbackup: Block device for FileSystem:%s not found in /etc/mtab", SSU_ACS_DATAPATH);
			m_szBlockDevice = ACS_SSU_DATA_BLOCK_DEVICE;
		}
	}
	return m_szBlockDevice;
}

/*! @brief Extracts the occupied quota and the quota soft limit size
 *
 * Function snipped from SSU
 *
 * @param[in]  szPath		        The path of the folder to calculate the quotas
 * @param[out] u64QuotaSize     The occupied quota in bytes
 * @param[out] u64SoftLimitSize The quota soft limit in bytes
 * @return 							        true, if values have been extracted, false otherwise
 */
bool acs_bur_burbackup::calculateQuotas(string szPath, ACE_UINT64 &u64QuotaSize, ACE_UINT64 &u64SoftLimitSize)
{
	struct dqblk dq;
	ACE_stat fileStat;

	if (0 == ACE_OS::stat(szPath.c_str(), &fileStat))
	{
		string dev = szGetBlockDevice();
		if (quotactl(QCMD(Q_GETQUOTA, GRPQUOTA), dev.c_str(), fileStat.st_gid, (caddr_t)&dq))
		{
			TRACE(m_burb, " %s", "quotactl Q_GETQUOTA in acs_bur_burbackup::calculateQuotas(): Failed");
			return false;
		}
		if (!(dq.dqb_valid & QIF_BLIMITS) || !(dq.dqb_valid & QIF_SPACE))
		{
			TRACE(m_burb, " quotactl dqb_valid in acs_bur_burbackup::calculateQuotas(): 0x%X value not valid", dq.dqb_valid);
			return false;
		}
		u64QuotaSize = dq.dqb_curspace;
		u64SoftLimitSize = dq.dqb_bsoftlimit * KILOBYTE;
	}
	else
	{
		TRACE(m_burb, " %s", "ACE_OS::stat in acs_bur_burbackup::calculateQuotas(): Failed");
		return false;
	}
	return true;
}
