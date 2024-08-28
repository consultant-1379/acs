/*
 * ut_acs_bur_Common.h
 *
 *  Created on: March 21, 2012
 *      Author: egiacri
 */

#ifndef UT_ACS_BUR_COMMON_H_
#define UT_ACS_BUR_COMMON_H_
#include <string>
//#include "acs_bur_util.h"
#include "acs_bur_Define.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperation.h"
//#include "acs_bur_BrmBackupManager.h"
//#include "acs_bur_BrmBackup.h"
//#include "acs_bur_BrmAsyncActionProgress.h"

using namespace std;
#define GETCMD_FIRST "immlist -a "
#define TEST_BKP "utBrmBackup"
#define DN_TEST_BRMBACKUP "brmBackupId=utBrmBackup,brmBackupManagerId=SYSTEM_DATA,brMId=1"
#define DN_BRMAAP "id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1"
#define MSG_CREATESTART "Create backup Action Invoked waiting for operation end...."
#define MSG_DELETESTART "Delete backup Action Invoked waiting for operation end...."
#define NOMORECURRENT -41
#define DELETEBKPFAILED -42
#define UNESPECTEDWAIT -43
#define NULLSTDERR " 2>/dev/null "

class ut_acs_bur_Common
{
public:
  ut_acs_bur_Common();
  ~ut_acs_bur_Common();

 /* static int getBBMInstance(string *dnBrmBackupManager,int *code,string *message);
  static int getBBInstance(string archiveFileName,string *dnBrmBackup, int *code, string *message);
  static int invokeAction(int actionId,string dn,string backupName,int *code, string *message);
  static int waitForTeminate(int actionId,string dn,string *backupCreationTime,string backupName, int *code, string *message);
  static bool validFileName(string name);*/
  static void clearCRLF(char *buff);
  static int waitForActionEnd(int actionId,string backupName,int *code);
  static int invokeAction_s(int actionId,string dn,string backupName,int *code, string *message);
  template <size_t size2>
  static int test_getObjectAttribute_s(string m_objectName, string m_attributeName, char (&value) [size2]){
	  /*char char_value[50];
  	if ( strlen(tmp) <= size ){
  					strncpy( value, tmp, size );
  	char getcmd[] = GETCMD_FIRST;
  	string exportCmd =getcmd + m_attributeName +" "+ m_objectName;
	   */
	  char getcmd[] = GETCMD_FIRST;
	  string immlstCmd =getcmd + m_attributeName +" "+ m_objectName + NULLSTDERR;
	  FILE *in;
	  char buff[512];
	  memset(buff,0,512);

	  if (!(in = popen(immlstCmd.c_str(), "r"))) {
		  char msg[256];
		  sprintf(msg,"%s , error: %s ","getObjectAttribute_s :: Error calling popen()",strerror( errno ));
		  cout << msg << endl;
		  return RTN_FAIL;
	  }
	  // read the output of immlist command, one line at a time
	  //fgets(buff, sizeof(buff), in);
	  string total = "";
	  while( fgets(buff, sizeof(buff), in) != (char *)NULL){
		  //cout<< "Read_buff before clear is : "<< buff<<endl;
		  ut_acs_bur_Common::clearCRLF(buff);
		  //cout<< "Read_buff after clear is : "<< buff<<endl;
		  total = total + string(buff);
		  //cout<< "Total_buff is : "<<total<<endl;
	  }
	  // close the pipe
	  string outBuffer = total;
	  pclose(in);
	  if (outBuffer.empty()){
		  //cout << "Unable to find attribute value" << endl << endl;
		  return RTN_FAIL;
	  }
	  //string::size_type pos = outBuffer.find_first_of("=");
	  size_t found;
	  // different member versions of find in the same order as above:
	  found=outBuffer.find_first_of("=");
	  //cout << " first position " << int(found) << endl;
	  if (found!=string::npos)
	  {
		  outBuffer = outBuffer.substr(found + 1);
		  //cout<< "Read Attribute is : "<<outBuffer<<endl;
		  if ( outBuffer.length() <= size2)
		  {
			  strncpy(value, outBuffer.c_str(), size2);
			  return RTN_OK;
		  }
		  else
			  return RTN_FAIL;
	  }


	  cout << "Unable to read attribute "<< m_attributeName <<" by immlist" << endl;
	  return RTN_FAIL;
  }

  template <size_t size>
  static int test_getObjectAttribute(string m_objectName, string m_attributeName, char (&value) [size]){

	  ACS_CC_ReturnType returnCode;
	  acs_apgcc_paramhandling PHA;
	  returnCode = PHA.getParameter(m_objectName,m_attributeName,value);
	  return returnCode;
  }


};

#endif /* UT_ACS_BUR_COMMON_H_ */
