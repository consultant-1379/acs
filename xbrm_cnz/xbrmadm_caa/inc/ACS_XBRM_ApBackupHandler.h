/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2023
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
* @file ACS_XBRM_ApBackupHanlder.h
*
* @brief
* Main file for creating AP backup as part of externalizing the Node|AP backup
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
 * 05/17/2023     XSRAVAN       Initial Release
 ****************************************************************************/

#ifndef ACS_XBRM_ApBackupHanlder_H_
#define ACS_XBRM_ApBackupHandler_H_

#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include <ACS_TRA_trace.h>
#include <string>

#include "ACS_XBRM_Server.h"
#include <fstream>
#include <iostream>
#include "ACS_XBRM_Utilities.h"

using namespace std;
void *generateBackupandExport(void *);
int do_command(void *);
int checkForErrors(string *result, void *arguements);
int getBackupName(string *result, void *arguements);

class ACS_XBRM_ApBackupHandler //: public ACE_Task_Base
{
public:
  ACS_XBRM_ApBackupHandler(string arg1, string arg2, string arg3, string arg4, string reportProgressDn);
  ~ACS_XBRM_ApBackupHandler();
  ACE_INT32 execute();
  ACS_XBRM_Utilities *utilities = NULL;
  string reportProgressDn;
  pthread_t pThreadId;
  inline pthread_t getPthreadId() { return pThreadId; }
  inline void setPthreadId(pthread_t pThreadId) { this->pThreadId = pThreadId; }
  inline string getOriBackupName() { return m_ori_backupName; }

private:
  ACS_TRA_Logging *m_xbrmLog;
  int m_opCode;
  string m_returnError;
  bool m_isSecured; // anssi
  string m_backupPasswd;
  int m_argc_;
  char **m_argv_;
  ACS_TRA_trace *m_xbrm;
  string m_tasktartTime;
  string m_transferQueue;
  string m_labelName;
  string m_backupDir;
  string m_ori_backupName;
};

#endif
