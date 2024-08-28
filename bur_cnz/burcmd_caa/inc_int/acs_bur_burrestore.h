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
 ----------------------------------------------------------------------*//**
 *
 * @file acs_bur_burrestore.h
 *
 * @brief
 * Main file for restoring backup
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 19/12/2011     EGIMARR       Initial Release
 * 13/02/2014     XVENJAM       cpp check error correction
 ****************************************************************************/
/*
 *  Modified on: 2 Apr 2014
 *      Author: xquydao*/

#ifndef ACS_BUR_BURRESTORE_H_
#define ACS_BUR_BURRESTORE_H_
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include "acs_bur_Define.h"
#include <ACS_TRA_trace.h>
#include <string>



using namespace std;

class acs_bur_burrestore
{
public:
  acs_bur_burrestore(int argc , char* argv[]);
  ~acs_bur_burrestore();

  // Get code error
  int 	getOpCode();
  int 	parse(int argc_ ,char ** argv_,string *archiveFileName,string *backupPasswd,bool *onlyImport,bool *isSecuredBackup);
  void 	print_usage();
  int 	execute();
  int 	import_backup(string backupFileName,string *backupName);
  int 	error_message_handler();
  void 	setOpCode(int code);
  void 	setOpCode(int code, string message);
  int 	getLabel(const string pathFileName,string *label);
  int 	print_archive_info(string backupFileName, string *backupCreationTime);
  int 	getBackupNameFromTarFile(string tarFileName,string *backupName);

private:
  ACS_TRA_Logging* m_burLog;

  string m_dnBrmBackup;
  string m_dnBrmBackupManager;

  // Backup file name parameter  
  string m_archive_filename;
  string m_backupCreationTime;
  string m_label;
  string m_backupName;
  string m_backupPasswd;

  int opCode;
  string m_errorMessage;

  int argc_;
  char** argv_;

  ACS_TRA_trace* m_restore_trace;
  bool m_only_import;
  bool m_isSecured;

};



#endif /* ACS_BUR_BURRESTORE_H_ */
