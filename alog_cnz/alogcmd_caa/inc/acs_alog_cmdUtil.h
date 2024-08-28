//****************************************************************************
//
//  NAME
//     acs_alog_cmdUtil.h
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
//  N/A       07/08/2012     xbhakat        Final Release
//  ===================================================================
//
//  SEE ALSO
//     -
//
//****************************************************************************
#ifndef ACS_ALOG_CMDUTIL_H
#define ACS_ALOG_CMDUTIL_H

#include <acs_alog_defs.h>
#include <ace/ACE.h>
#include <string.h>
#include <ACS_APGCC_CommonLib.h>

#define MAX_PASSWORD_LENGTH 30
#define MIN_PASSWORD_LENGTH 12

using namespace std;
class acs_alog_cmdUtil
{
private:
   // This class is not instanciable
   acs_alog_cmdUtil();

public:
   static int HandleReturnCode(enumCmdExecuteResult res);
   static int HandleReturnCode(enumCmdExecuteResult res, bool eol);
   static int Authenticate(char * cPw);
   static bool getPassword(char *pPass, int max);
   static int getNewConfirmPassword(char *pOld,char *pNew,char *pConfirm);
   static bool checkComplexity(char *pPass);
   static void echo(bool on);
   static int CheckALOGSysFile( bool check_size_zero , bool if_writable,bool if_print_error);
   static int CheckAlogMain();
   static bool getAlogDataDiskPath( string& location);
   static int AuthenticateNoHandle(char * cPw);
   static int PasswordHandling(char *psw);
   static int PasswordHandling(void);
   static int  getUserName(std::string& user);
   static int  checkRoles();
   static bool runCommand(const string command, string& output);
   static std::string getPasswordInFile();
   static bool isNumeric(const char *s);
   static bool execute7zCmd(string command, string plogPassword, int &subprocStatus);		// TR HX49170
};

#endif
