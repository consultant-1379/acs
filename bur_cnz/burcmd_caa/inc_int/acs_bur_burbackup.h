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
 * @file acs_bur_burbackup.h
 *
 * @brief
 * Main file for creating backup
 *
 * @author
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE            USER         DESCRIPTION/TR
 * ----------------------------------------------
 * 01/12/2011     EGIMARR       Initial Release
 * 13/02/2014     XVENJAM       cpp check error correction
 * 03/08/2023	  XSRAVAN	Changed as part MSC Backup Improvements Feature
 ****************************************************************************/

#ifndef ACS_BUR_BURBACKUP_H_
#define ACS_BUR_BURBACKUP_H_

#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include "acs_bur_Define.h"
#include <ACS_TRA_trace.h>
#include <string>



using namespace std;

class acs_bur_burbackup
{
public:
  acs_bur_burbackup(int argc , char* argv[]);
  ~acs_bur_burbackup();

  // Make file name with BACKUP_HHHH_MM_DD_HH_MM
  int 	makeBackupName(string *backupNodeName,string *backupFileName);

  // Get operation datail code
  int 	getOpCode();
  void 	setOpCode(int code);
  void 	setOpCode(int code,string mes);
  int 	parse(int argc_ ,char ** argv_,string *archiveFileName,string *labelName,bool *onlyImport,string *backupPasswd,string *newBackupName);//anssi
  int   parseForInternalUse(int argc_ ,char ** argv_,string *newBackupName,bool *onlyImport);//anssi
  void 	print_usage();
  int 	error_message_handler();
  int 	execute();
  int 	export_backup(string backupFileName);
  void 	print_backup_info(string backupFileName, string backupCreationTime);
  int  	print_archive_info(string backupFileName, string *backupCreationTime);
  int 	getNEID(string *id);
  string getSwVersion();
  int   renameBackupName(string *backupFileName);

private:
  ACS_TRA_Logging* m_burLog;
  int m_opCode;
  string m_errorMessage;
  string m_dnBrmBackupManager;
  string m_dnBrmBackup;
  bool m_isSecured;//anssi
  string m_backupPasswd;
  bool m_isInternal;
  string m_newBackupName;

  // Backup file name formatted by makeFileName method
  // as to imagename
  string m_backupFileName;
  string m_labelName;
  bool m_only_export;

  int m_argc_;
  char** m_argv_;

  ACS_TRA_trace* m_burb;
  // used with -a option
  string m_archive_filename;
  string m_backup_nodename;
  struct tm *m_backup_time;
  string m_backupCreationTime ;

  string m_szBlockDevice;

 /*! @brief Calculates whether is there enough space in the quota for exporting
  *
  * @return           RTN_OK if there is space, RTN_FAIL otherwise
  */ 
  int isThereEnoughSpace(void);
  
 /*! @brief Returns the Block device associated to the current folder
  *
  * Function cloned from SSU
  *
  * @return 							The device
  */ 
  string szGetBlockDevice(void);

 /*! @brief Calculates the size of the passed folder and subfolders
  *
  * Currently not used function
  *
  * @param[in] 	dirName		The name of the folder to calculate
  * @return 							The folder size in bytes
  */ 
  uint64_t calculateFolderSize(string dirName);
 
 /*! @brief Extracts the occupied quota and the quota soft limit size
  *
  * Function snipped from SSU
  *
  * @param[in] 	szPath		       The path of the folder to calculate the quotas
  * @param[out] u64QuotaSize     The occupied quota in bytes
  * @param[out] u64SoftLimitSize The quota soft limit in bytes
  * @return 							       true, if values have been extracted, false otherwise
  */ 
  bool calculateQuotas(string szPath, ACE_UINT64 &u64QuotaSize, ACE_UINT64 &u64SoftLimitSize);
};



#endif /* ACS_BUR_BURBACKUP_H_ */
