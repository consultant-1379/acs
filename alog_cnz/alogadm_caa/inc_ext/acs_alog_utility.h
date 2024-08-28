
/*=================================================================== */
/**
   @file   acs_alog_utility.h

   @brief Header file for acs_alog_utility type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       07/08/2012     xbhakat               Final Release
==================================================================== */


#include <acs_alog_errorHandler.h>
#include <list>
#include <ACS_CS_API.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_ApplicationManager.h>
#include <aes_ohi_extfilehandler2.h>
#include <aes_ohi_filehandler.h>
#include <syslog.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_stat.h>
#include <string.h>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <sec/crypto_api.h> /*start of HX86564*/
#include <sec/crypto_status.h>/*end of HX86564*/

using namespace std;

void  ACSALOGLOG (ACS_TRA_LogLevel levelErr, string errorText);
void  ACSALOGTRACE (string message);
string  TEXTERROR (string errText);
void  AEHEVENTMESSAGE (const char * processName,long int specificProblem, const char * Severity, const char * probCause, const char * classReference, const char * objOfReference,const char * problemData, const char * problemText );
void  rotateAuditLog ();
void  handleLoggingDirSize (const char *dirName, const int dirSize);
void  handleCLogDirSize (const char *dirName, const int dirSize);
void  rotatePAuditLog ();
void  handleProtectedLoggingDirSize (const char *dirName, const int dirSize);
int  validateTransferQueue (string TQ, bool ClusteLog);
int  getTransferQueueDn (string TQ);
int  getcommandLogTransferQueueDn (string commandLogTQ);
bool createDir(string& alogDir);
bool getAlogDataDiskPath(string& szFolderName);
bool getClearDataDiskPath(string& szFolderName);
bool runCommand(const string command, string& output);
bool execute7zCmd(string command, string plogPassword, int &subprocStatus);			// TR HX49170

// Methods to encode/decode cmds and prts
	ACE_TCHAR* Encode(ACE_TCHAR* lpszData);
	ACE_TCHAR* Decode(ACE_TCHAR* lpszData);
 bool checkFile(const ACE_TCHAR* lpszFile, const bool bIsDir);

 extern "C"{/*start of HX86564*/
 extern SecCryptoStatus sec_crypto_encrypt_ecimpassword_legacy(/*OUT*/ char** ciphertext, const char* plaintext);
 extern SecCryptoStatus sec_crypto_decrypt_ecimpassword_legacy(/*OUT*/ char** plaintext, const char* ciphertext);
 }

 std::string encryptString(const std::string& inputString);
 std::string decryptString(const std::string &strPwdCrypt);/*end of HX86564*/
