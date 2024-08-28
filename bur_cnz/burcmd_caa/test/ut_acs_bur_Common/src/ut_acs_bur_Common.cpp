/*
 * ut_acs_bur_Common.cpp
 *
 *  Created on: Mar 21, 2012
 *      Author: egiacri
 */

#include "ut_acs_bur_Common.h"
#include "string.h"

int ut_acs_bur_Common::invokeAction_s(int actionId,string dn,string backupName,int *code, string *message){

	int returnValue;
		acs_apgcc_adminoperation admOp;
		ACS_CC_ReturnType result;

		const char *dnObjName1 = dn.c_str();

		long long int timeOutVal_5sec = 5*(1000000000LL);

		result = admOp.init();
		if (result != ACS_CC_SUCCESS){
			*code = INITERROR;
			*message=string("Init()");
			return RTN_FAIL;
		}

		std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
		// Prepare vectorIN parameter list
		// waiting for 5 seconds to accept response
		ACS_APGCC_AdminOperationParamType firstElem;
		/*create parameter for list*/
		char attName1[]= "name";
		firstElem.attrName =attName1;
		firstElem.attrType=ATTR_STRINGT;
		char * attrValue1 = const_cast<char*>(backupName.c_str());
		firstElem.attrValues = reinterpret_cast<void*>(attrValue1);

		vectorIN.push_back(firstElem);
		returnValue = 0;

		result = admOp.adminOperationInvoke(dnObjName1, 0, actionId, vectorIN , &returnValue, timeOutVal_5sec);

		admOp.finalize();

		if (result != ACS_CC_SUCCESS){
			*code = INVOKEERR;
			*message=string("Finalize()");
			return RTN_FAIL;
		}

		*message="";
		// Testing returnValue if request was accepted
		if (returnValue == SA_AIS_OK){
			*code = NOERROR;
			return RTN_OK;
		}
		// Server busy or BRF-C in progress from an other action
		*code = OPNOTACEPT;
		return RTN_FAIL;

}
void ut_acs_bur_Common::clearCRLF(char *buff)
{
	int i=0;
	for(i=0;i<(int)strlen(buff);i++)
	{
		if((10==buff[i])||(12==buff[i]))
		{
			buff[i]=0;
			break;
		}
	}
}


int ut_acs_bur_Common::waitForActionEnd(int actionId,string backupName,int *code){
	//acs_bur_BrmBackupManager *brm ;
	//acs_bur_BrmAsyncActionProgress *async;
	//acs_bur_BrmBackup *backup;
	char cBrmBackup[256];
	char currentOperation_s[10];
	char progressPercentage_s[10];
	char backupstatus_s [50];
	char result_s[10];
	int backupstatus = 0;
	//int backupState = 0;
	string dnBrmBackup = DN_TEST_BRMBACKUP;
	string progressInfo;
	string dnAsyncActionProgress;
	string timeOfTheLastStatusUpdate = "";
	string old_timeOfTheLastStatusUpdate = "";
	//int lastProgressPercentage = -1;
	int progressPercentage = 0;
	int result = 0;
	int currentOperation = actionId;
	//char lastOperation [100];
	int watchDogCounter = SLEEPDELAY*60; //2 minutes

	// Starting printout
	if (CREATEBACKUP==actionId)
	{
		cout << endl << MSG_CREATESTART << endl;
		//cout << endl << MSG_BKPSTART << endl;
		//cout << MSG_INFOARC << endl;
	}

	if (DELETEBACKUP==actionId)
	{
		cout << endl << MSG_DELETESTART << endl;
		//cout << endl << MSG_BKPSTART << endl;
		//cout << MSG_INFOARC << endl;
	}

	while(FOREVER){
		// wait for read next update
		sleep(SLEEPDELAY);

		if(CREATEBACKUP==actionId || RESTOREBACKUP==actionId || DELETEBACKUP==actionId )
		{
			char char_value_s[100];
			if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMBACKUPMANAGER,"asyncActionProgress",char_value_s)!=RTN_FAIL )
				dnAsyncActionProgress = char_value_s;
			else
					return RTN_FAIL;

		}



/*		async = new acs_bur_BrmAsyncActionProgress((char *)dnAsyncActionProgress.c_str());
		*code = async->getOpCode();
		if (async->getOpCode() != NOERROR ){
			delete async;
			return RTN_FAIL;
		}*/

		// Check if Brf-C is alive
		if (!dnAsyncActionProgress.empty())
		{
			char char_value_s[100];
			if(ut_acs_bur_Common::test_getObjectAttribute_s(dnAsyncActionProgress,"timeOfLastStatusUpdate",char_value_s)!=RTN_FAIL )
				timeOfTheLastStatusUpdate = char_value_s;
			else
				return RTN_FAIL;
		}
		else
			return RTN_FAIL;
		//timeOfTheLastStatusUpdate = async->getTimeOfLastStatusUpdate();

		if (timeOfTheLastStatusUpdate != old_timeOfTheLastStatusUpdate)
			watchDogCounter = SLEEPDELAY*60;
		else
		{
			watchDogCounter--;
			if(0==watchDogCounter){
				*code = OPNOTACEPT;
				//delete async;
				return RTN_FAIL;
			}
		}
		//
		old_timeOfTheLastStatusUpdate = timeOfTheLastStatusUpdate;
		// Check for action ID in progress must be CreateBackup


		if(ut_acs_bur_Common::test_getObjectAttribute_s(dnAsyncActionProgress,"actionId",currentOperation_s)!=RTN_FAIL )
			sscanf(currentOperation_s,"%d",&currentOperation);
		else
			return RTN_FAIL;

		if (currentOperation != actionId ){
			*code = CHGCUROP;
			//delete async;
			return RTN_FAIL;
		}

		//backupState = async->getState();
		//progressInfo = async->getProgressInfo();
		if(ut_acs_bur_Common::test_getObjectAttribute_s(dnAsyncActionProgress,"progressPercentage",progressPercentage_s)!=RTN_FAIL )
			sscanf(progressPercentage_s,"%d",&progressPercentage);
		else
			return RTN_FAIL;
		//progressPercentage = async->getProgressPercentage();

		if(ut_acs_bur_Common::test_getObjectAttribute_s(dnAsyncActionProgress,"result",result_s)!=RTN_FAIL )
			sscanf(result_s, "%d", &result);
		else
			return RTN_FAIL;

		if( (result==SUCCESS)||(result==FAILURE))
		{
			(result==SUCCESS)?*code = NOERROR:*code=BACKUPFAIL;
			break;
		}

	} // end while(FOREVER)

	//*backupCreationTime = string("** DEFAULT **");
	//
	if ( SUCCESS==result && CREATEBACKUP==actionId){

		// Check BrmBackupStatus
		sprintf(cBrmBackup,dnBrmBackup.c_str(),backupName.c_str());
		dnBrmBackup = cBrmBackup;

		//backup = new acs_bur_BrmBackup( cBrmBackup );
		//*code = backup->getOpCode();
		if(ut_acs_bur_Common::test_getObjectAttribute_s(cBrmBackup,"status",backupstatus_s)!=RTN_FAIL )
		sscanf(backupstatus_s,"%d",&backupstatus);
		else
		{
			*code = BACKUPFAIL;
			return RTN_FAIL;
		}
		// backup->getStatus();
		//*backupCreationTime = backup->getCreationTime();
		//delete backup;
		if (backupstatus != BRM_BACKUP_COMPLETE)
			{
				*code = BACKUPFAIL;
				return RTN_FAIL;
			}
			cout << "BrmBackup test object  "<< dnBrmBackup<<" create action FINISHED" << endl;
			*code = RTN_OK;

	} // end if (result == SUCCESS && CREATEBACKUP==actionId)
	//Check backup Delete action result
	if ( SUCCESS == result && DELETEBACKUP==actionId){
		char brmBackupName[100];
		cout << "WAIT FOR DELETE::BrmBackup test object get brmBackupId"<<endl;
		//Try to find the BrmBackup test instance
		if(ut_acs_bur_Common::test_getObjectAttribute_s(dnBrmBackup,"brmBackupId",brmBackupName)!=RTN_FAIL )
		{
			//Try to know if Delete Backup Action has been canceled or has failed
			cout << "BrmBackup test object  "<< brmBackupName<<" found in IMM DB" << endl;
			char lastAction[100];
			char lastOpRes[100];
			if (ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMAAP,"actionId",lastAction)!=RTN_FAIL && ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMAAP,"result",lastOpRes)!=RTN_FAIL)
			{
				//No more delete action actionId=1
				if (strcmp(lastAction,"1" )!=0)
				{
					cout << "BrmBackup test object  "<< dnBrmBackup<<" delete action NO MORE CURRENT" << endl;
					*code = NOMORECURRENT;
					return RTN_FAIL;
				}
				//Still delete action actionId=1 but try to understand if action has failed (ACTION RESULT=2)
				else if (strcmp(lastOpRes,"2" )==0)
				{
					cout << "BrmBackup test object  "<< dnBrmBackup<<" delete action FAILED" << endl;
					*code =DELETEBKPFAILED;
					return RTN_FAIL;
				}
				//Still delete action actionId=1 but try to understand if still in progress (ACTION RESULT= NOT_AVAILABLE	3)
				else if (strcmp(lastOpRes,"3" )==0)
				{
					sleep (1);
					if(ut_acs_bur_Common::test_getObjectAttribute_s(DN_BRMAAP,"result",lastOpRes)!=RTN_FAIL && strcmp(lastOpRes,"1" )!=0 )
					{
						cout << "BrmBackup test object  "<< dnBrmBackup<<" delete action UNESPECTED WAIT TIME" << endl;
						*code =UNESPECTEDWAIT;
						return RTN_FAIL;
					}
				}
			}

		}

		cout << "BrmBackup test object  "<< dnBrmBackup<<" delete action FINISHED" << endl;
		*code = RTN_OK;
		return RTN_OK;

	}// end if (result == SUCCESS && DELETEBACKUP==actionId)

	return ((SUCCESS==result)?RTN_OK:RTN_FAIL);
}
