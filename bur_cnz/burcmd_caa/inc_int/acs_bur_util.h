/*
 * acs_bur_util.h
 *
 *  Modified on: 2 Apr 2014
 *      Author: xquydao
 *  Created on: Feb 10, 2012
 *      Author: egimarr
 */

#ifndef ACS_BUR_UTIL_H_
#define ACS_BUR_UTIL_H_
#include <string>

#include "acs_bur_BrmAsyncActionProgress.h"
#include "acs_apgcc_adminoperationasync_V2.h" //HX68319
#include "acs_apgcc_adminoperationtypes.h"

using namespace std;

class acs_bur_util
{
public:
  acs_bur_util();
  ~acs_bur_util();

  static int getBBMInstance(string *dnBrmBackupManager,int *code,string *message);
  static int getBBInstance(string archiveFileName,string *dnBrmBackup, int *code, string *message);
  static int invokeAction(int actionId,string dn,string backupName,int *code, string *message,bool isSecured = false,string password="" ,string label="",bool isRestore = false);
 static int waitForTeminate(int actionId,string dn,string *backupCreationTime,string backupName, int *code, string *message);
  static bool validFileName(string name);
  static void clearCRLF(char *buff);
  static int getParentProcessPid(int processid);
  static int getParentProcessPid(int processid, string& parentName);
  static int do_command(const char* command,string *result);
  static bool matchKeyWord(string keyWord, string str);
  static string strip(const char buf[]);
  static bool affirm(string text);
  static bool isTroubleShooterUser();
  //Below methods included to handle password
  static string getPassword();
  static bool getNewConfirmPassword(string pNew);
  static void echo(bool on);
  static std::string exec(const char* cmd);

  /*! @brief Returns the current action waiting the end of a possible DELETEBACKUP action
   *
   * Fixes TR HS17819
   *
   * First time is used the passed async object, and then for each attempt is get a new one.
   * The new object is reassigned to the passed pointer,
   * in oder to return the new object to the caller.
   *
   * @param[in]  actionId		The action requested
   * @param[out] async                  The address of the pointer to the acs_bur_BrmAsyncActionProgress object
   * @param[in]  dnAsyncActionProgress  The Action Progress name
   * @return 				The current action
   */ 
  static int waitCurrentOperation(int actionId, acs_bur_BrmAsyncActionProgress **async, string dnAsyncActionProgress);

  static int getBrmBackupMangerDN(string *dnBrmBackupManager, int *code, string *message);
  static int getBrmBackupDN(string *dnBrmBackup, string archiveFileName, int *code, string *message);

  static std::string prompt_;
  static int getImmInstances(const char *className, string backupName);
};

class acs_bur_util_admOpAsync: public acs_apgcc_adminoperationasync_V2 //HX68319
{

public:
        acs_bur_util_admOpAsync()
        {
        }
        ~acs_bur_util_admOpAsync()
        {
        }
        virtual void objectManagerAdminOperationCallback( ACS_APGCC_InvocationType invocation, int returnVal , int error, ACS_APGCC_AdminOperationParamType** outParamVector );


};

#endif /* ACS_BUR_UTIL_H_ */
