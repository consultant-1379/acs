
/*=================================================================== */
/**
   @file   acs_alog_cmdHandler.h

   @brief Header file for acs_alog_cmdHandler type module.

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

#ifndef ACS_ALOG_CMDHANDLER_H_
#define ACS_ALOG_CMDHANDLER_H_


#include <iostream>
#include <set>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <acs_apgcc_omhandler.h>
#include <acs_alog_types.h>
#include <acs_alog_utility.h>
#include <acs_alog_defs.h>
#include <acs_alog_fileDef.h>



using namespace std;

class acs_alog_cmdHandler {
private:
	vector<string>vettore;
	char locBuffer[2048];
	ACS_ALOG_ReturnType File_Exist (char *sPath);
	ACS_ALOG_Exclude        m_Exclude;
	 void ClearExcludedList();

public:
	acs_alog_cmdHandler(){};
	ACS_ALOG_ReturnType getExclCmd();
	ACS_ALOG_ReturnType setExclCmdList(int,string);
	ACS_ALOG_ReturnType delExclCmdList(int,string);
	ACS_ALOG_ReturnType setAlogStatus(int);
	ACS_ALOG_ReturnType setAlogFileStatus(int);
	ACS_ALOG_ReturnType setMmlSyslogStatus(int);
	ACS_ALOG_ReturnType setMmlSyslogAvailabilityStatus(int);
	void setAlogTQ(string);
	void setAlogCmdLogTQ(string);
	void setAlogDataSkipped(int);
	void setAlogIDS(int);
	void setPlogIDS(int);
	void setFileNaming (int format);

	string getAlogTQ();
	string getPlogTQ();
	string getAlogCmdLogTQ();
	int getAlogStatus();
	int getAlogFileStatus();
	int getMmlSyslogStatus();
	int getMmlSyslogAvailabilityStatus();
	int getAlogDataSkipped();
	int getAlogIDS();
	int getFileNaming();
	//PLOG Start
	void UpdatePassword();
	enumCmdExecuteResult UpdatePasswordPresent(bool value);
	enumCmdExecuteResult ChangePassword();
	enumCmdExecuteResult SetPw(const char* cPtrSetPw);
	enumCmdExecuteResult SetNewPw(const char* cPtrOldPw, const char* cPtrNewPw);
	enumCmdExecuteResult CheckPw(const char* cPw);	//PL
	enumCmdExecuteResult ProtectMMLcmd(const char* lpszCmd);
	enumCmdExecuteResult UnprotectMMLcmd(const char* lpszCmd);
	enumCmdExecuteResult ProtectMMLprt(const char* lpszPrt);
	enumCmdExecuteResult UnprotectMMLprt(const char* lpszPrt);
	enumCmdExecuteResult ResetFile(const char* fileName);
	enumCmdExecuteResult TQ_CeaseHandler();
	enumCmdExecuteResult deleteAllPlogFiles();
	enumCmdExecuteResult InitATTRFILEP();
	enumCmdExecuteResult changePlogIdTag(const char* idTag);
	enumCmdExecuteResult changePlogFileDest(const char* TQ);
	enumCmdExecuteResult ProtectUserData(const char* lpszUsr);
	enumCmdExecuteResult UnprotectUserData(const char* lpszUsr);
	int UpdateProtectedPassword();
	void changePasswordToTheZipFile(const char* cPtrNewPw);
	bool checkMMLcmdProtection(const char* lpszCmd, const char* lpszPar);
	~acs_alog_cmdHandler(){};
};

#endif /* ACS_ALOG_CMDHANDLER_H_ */
