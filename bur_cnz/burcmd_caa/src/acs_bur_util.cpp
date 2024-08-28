
/*
 * acs_bur_util.cpp
 *
 *  Modified on: 2 Apr 2014
 *      Author: xquydao
 *  Created on: Feb 10, 2012
 *      Author: egimarr
 */

#include "acs_bur_util.h"
#include "acs_bur_Define.h"
#include "acs_bur_BrmBackup.h"
#include "acs_bur_BrmBackupManager.h"
#include "ACS_APGCC_CommonLib.h"
#include <ace/ACE.h>
#include "ACS_CS_API.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include <ACS_APGCC_Util.H>
#include <stdio.h>

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
#include <stdexcept>

std::string acs_bur_util::prompt_("\x03:"); // ETX :

using namespace std;

//========================================================================================
//	Match a string towards a keyword
//========================================================================================

bool acs_bur_util::matchKeyWord(string keyWord, string str)
{
	if (str.empty())
		return false;

	string temp = str;

	int i = 0;
	while (i < (int)temp.size())
	{
		temp[i] = toupper(temp[i]);
		i++;
	}

	return !keyWord.find(temp);
}

//========================================================================================
//	Strip leading and trailing spaces from a string buffer
//========================================================================================

string
acs_bur_util::strip(const char buf[])
{
	string str(buf);
	string::size_type first = str.find_first_not_of(" \t");
	if (first != string::npos)
	{
		string::size_type last = str.find_last_not_of(" \t");
		return str.substr(first, last - first + 1);
	}
	else
	{
		return "";
	}
}

//========================================================================================
//	Expect "YES" or "NO" answer
//========================================================================================

bool acs_bur_util::affirm(string text)
{
	char ibuf[128];
	string answer;

	cout << text << MSG_YESNOT << prompt_;
	while (1)
	{
		cin.clear();
		cin.getline(ibuf, 128);
		// sleep(1);
		answer = strip(ibuf);
		if (matchKeyWord("YES", answer))
		{
			return true;
		}
		if (matchKeyWord("NO", answer))
		{
			return false;
		}
		cout << MSG_YESNOT << prompt_;
	} // End of while
} // End of affirm

//========================================================================================
//	Expect "YES" or "NO" answer
//========================================================================================
/*
bool
acs_bur_util::isTroubleShooterUser()
{
	int portNo = 0;
	char* port = getenv("PORT");
	if ( port == 0 ){
		return false;
	}
	portNo = atoi(port); // Will return zero if conversion fails

	if ( (portNo == 4422) || (portNo == 4423) ) {
		return true;
	}

	return false;
} // End of isTroubleShooterUser
*/
bool acs_bur_util::isTroubleShooterUser()
{
	char buffer[25];
	sprintf(buffer, "%ld", (long)getppid());
	int processid = atoi(buffer);
	// trace DN for BrmBackup
	ACS_TRA_trace *util_trace = new ACS_TRA_trace("acs_bur_util");
	// trace DN for BrmBackup

	const int MAX_NUM_PARENTS = 5;
	string parentName;
	const string com_process = "com";
	bool isTsUser = true;

	for (int i = 0; i < MAX_NUM_PARENTS; i++)
	{

		processid = getParentProcessPid(processid, parentName); // Fetching the parent processid and its name
		TRACE(util_trace, "isTroubleShooterUser: parent process id = %d, processName = %s ", processid, parentName.c_str());

		if (parentName.compare(com_process) == 0)
		{ // if the parent process is COM then he burbackup has been executed from cliss
			isTsUser = false;
			break;
		}
	}
	// delete trace object
	delete util_trace;
	return isTsUser;
}
int acs_bur_util::getParentProcessPid(int processid, string &parentName)
{

	FILE *filePointer;
	char buffer[25], _pid[25];
	sprintf(_pid, "%d", processid);
	bool found = false;
	int parentId = 0;
	string processStatusFile = "/proc/" + string(_pid) + "/status";
	const string PPid_tag = "PPid:";

	filePointer = fopen(processStatusFile.c_str(), "r"); // opening the file to get the parent pid

	if (filePointer != NULL)
	{
		fgets(buffer, sizeof(buffer), filePointer);
		parentName = string(buffer).substr(6, std::string::npos); // the first line contains the name of the process
		ACS_APGCC::trim(parentName);
		while (!found && !feof(filePointer))
		{
			fgets(buffer, sizeof(buffer), filePointer);
			if (string(buffer).substr(0, 5).compare(PPid_tag) == 0)
			{ // searching for the PPid tag to take the parent process
				found = true;
				parentId = atoi(string(buffer).substr(6, std::string::npos).c_str());
			}
		}
		fclose(filePointer);
	}
	return parentId;
}

void acs_bur_util::clearCRLF(char *buff)
{
	int i = 0;
	for (i = 0; i < (int)strlen(buff); i++)
	{
		if ((10 == buff[i]) || (12 == buff[i]))
		{
			buff[i] = 0;
			break;
		}
	}
}
//
int acs_bur_util::do_command(const char *command, string *result)
{
	FILE *in;
	// save current uid for commands impersonations
	int current_uid = getuid();
	// create trace object
	ACS_TRA_trace *util_trace = new ACS_TRA_trace("acs_bur_util");
	// trace DN for BrmBackup
	TRACE(util_trace, "do_command: current_id = %d ", current_uid);
	// delete trace object
	delete util_trace;
	// change uid to root
	if (-1 == setuid(0))
	{
		return SETUIDERR;
	}

	setuid(0);

	if (!(in = popen(command, "r")))
	{
		return POPENERR;
	}
	// clear buffer
	*result = string("");
	char buff[256];

	// read the output of export command, one line at a time
	while (fgets(buff, sizeof(buff), in) != (char *)NULL)
	{
		acs_bur_util::clearCRLF(buff);
		*result = *result + string(buff);
	}

	// restore uid for commands impersonations
	/* setuid(current_uid);*/
	// close the pipe
	pclose(in);
	return RTN_OK;
}

// get BrmBackupManager instance
int acs_bur_util::getBBMInstance(string *dnBrmBackupManager, int *code, string *message)
{
	// Get DN for BrmBackupManager object
	if (getBrmBackupMangerDN(dnBrmBackupManager, code, message) == RTN_FAIL)
	{
		return RTN_FAIL;
	}
	// create object BrmBackupManager
	acs_bur_BrmBackupManager *brm = new acs_bur_BrmBackupManager((char *)(*dnBrmBackupManager).c_str());
	// set error code
	(*code) = brm->getOpCode();
	// set error message
	(*message) = brm->getOpMessage();
	// test error code occurs
	if (NOERROR != (*code))
	{
		// remove object
		delete brm;
		// return with error
		return RTN_FAIL;
	}

	// Checking if BRF-C is busy for an action in progress
	// Create an BrmAsyncActionProgress object
	acs_bur_BrmAsyncActionProgress *async;
	// Get distinguished name from BrmBackupManager attribute
	string dn = brm->getAsyncActionProgress();
	// Creates BrmAsyncActionProgress object
	async = new acs_bur_BrmAsyncActionProgress((char *)dn.c_str());
	// set error code
	(*code) = async->getOpCode();
	// set error message
	(*message) = async->getOpMessage();
	// Gets result field from BrmAsyncActionProgress object
	int result = async->getResult();
	// Gets state field from BrmAsyncActionProgress object
	int state = async->getState();
	// Remove BrmBackupManager object
	delete brm;

	// test if BrmAsyncActionProgress instance is getted correctly
	if (NOERROR != (*code))
	{
		// Remove BrmAsyncActionProgress object
		delete async;
		// Return with error
		return RTN_FAIL;
	}

	// test if BrmBackupManager instance is in init state
	if ((NOT_AVAILABLE == result) && (99 == async->getActionId()))
	{
		// Remove BrmAsyncActionProgress object
		delete async;
		// Return ok because the object is int initial state
		return RTN_OK;
	}

	// test if an action is in progress
	if ((NOT_AVAILABLE == result) ||
		(CANCELLING == state) ||
		(RUNNING == state))
	{
		// Clear error message
		(*message) = "";
		// Sets error code Operation not accepted
		*code = BRFCONFLICT;
		// Remove BrmAsyncActionProgress object
		delete async;
		// Return with error
		return RTN_FAIL;
	}
	// Remove BrmAsyncActionProgress object
	delete async;
	// Return without error
	return RTN_OK;
}

// get BrmBackup object instance
int acs_bur_util::getBBInstance(string archiveFileName, string *dnBrmBackup, int *code, string *message)
{
	// Define DN variable for BrmBAckup
	if (getBrmBackupDN(dnBrmBackup, archiveFileName, code, message) == RTN_FAIL)
	{
		return RTN_FAIL;
	}
	// create trace object
	ACS_TRA_trace *util_trace = new ACS_TRA_trace("acs_bur_util");
	// trace DN for BrmBackup
	TRACE(util_trace, "getBBInstance(): DN for BrmBackup = %s ", (*dnBrmBackup).c_str());
	// create BrmBackup object
	acs_bur_BrmBackup *brm = new acs_bur_BrmBackup((char *)(*dnBrmBackup).c_str());
	// get error
	int status = brm->getOpCode();

	// Check if instance is created with DN specified
	if (GETATTRERROR == status)
	{
		// set error code
		/*	*code = INVALIDIMAGE;
		// set error message
		 *message = brm->getOpMessage();
		// delete BrmBackup object
		delete brm;
		// return error
		return RTN_FAIL;*/

		int watchDogCounter = SLEEPDELAY; // two retry 4 seconds
		// string message;

		// Waiting until backup will be imported in IMM or quit for fail

		for (int j = 1; j = watchDogCounter; j++)
		{
			// wait
			delete brm;
			sleep(SLEEPDELAY);
			TRACE(util_trace, "getBBInstance(): DN for BrmBackup= %s ", (*dnBrmBackup).c_str());
			brm = new acs_bur_BrmBackup((char *)(*dnBrmBackup).c_str());
			// get error code
			status = brm->getOpCode();
			if (NOERROR == status)
			{
				break;
			}

			if (j == watchDogCounter)
			{
				// timeout expired and backup isn't been imported in IMM
				TRACE(util_trace, "acs_bur_util:: error getBBInstance() %s", "timeout expired");
				// set error code
				*code = INVALIDIMAGE;
				(*message) = brm->getOpMessage();
				// delete BrmAsyncActionProgress object
				delete brm;
				return (RTN_FAIL);
			}
		}
	}
	// Check if other error occurs
	if (NOERROR != status)
	{
		// set error code
		*code = status;
		// set error message
		*message = brm->getOpMessage();
		// delete BrmBackup object
		delete brm;
		// return error
		return RTN_FAIL;
	}

	// Checking if BRF-C is busy for an action in progress
	acs_bur_BrmAsyncActionProgress *async;
	// get distinguished asyncActionProgress field from BrmBAckup object
	string dn = brm->getAsyncActionProgress();
	// create BrmAsyncActionProgress object
	async = new acs_bur_BrmAsyncActionProgress((char *)dn.c_str());
	// get error code
	status = async->getOpCode();
	// prepares error message
	(*message) = async->getOpMessage();
	// get result field
	int result = async->getResult();
	// get state field
	int state = async->getState();

	// delete BrmBackup object
	delete brm;

	// Retry for IMM councurrent operation Defect Fixing R1A16

	if (GETATTRERROR == status)
	{

		int watchDogCounter = SLEEPDELAY; // two retry 4 seconds
		// string message;

		// Waiting until backup will be imported in IMM or quit for fail

		for (int j = 1; j = watchDogCounter; j++)
		{
			// wait
			delete async;
			sleep(SLEEPDELAY);
			TRACE(util_trace, "getBBInstance(): DN for BrmAsyncActionProgress = %s ", (char *)dn.c_str());
			async = new acs_bur_BrmAsyncActionProgress((char *)dn.c_str());
			// get error code
			status = async->getOpCode();
			// prepares error message
			(*message) = async->getOpMessage();
			// get result field
			result = async->getResult();
			// get state field
			state = async->getState();

			if (NOERROR == status)
			{

				break;
			}

			if (j == watchDogCounter)
			{
				// timeout expired and backup isn't been imported in IMM
				TRACE(util_trace, "acs_bur_util:: error getBBInstance() %s", "timeout expired");
				// set error code
				*code = status;
				(*message) = async->getOpMessage();
				// delete BrmAsyncActionProgress object
				delete async;
				return (RTN_FAIL);
			}
		}
	}
	else if (NOERROR != status && GETATTRERROR != status)
	{
		// set error code
		*code = status;
		(*message) = async->getOpMessage();
		TRACE(util_trace, "acs_bur_util:: error getBBInstance() %s", message);
		// delete BrmAsyncActionProgress object
		delete async;

		// return error
		return RTN_FAIL;
	}

	// test if BrmBackup instance is in init state !
	if ((NOT_AVAILABLE == result) && (99 == async->getActionId()) && (FINISHED == state))
	{
		// set no error code
		*code = NOERROR;
		// set no message for error code
		*message = "";
		// delete BrmAsyncActionProgress object
		delete async;
		// return successful
		return RTN_OK;
	}

	// No error message
	*message = "";

	// test if an action is in progress
	if ((NOT_AVAILABLE == result) ||
		(CANCELLING == state) ||
		(RUNNING == state))
	{
		// set error code to OPERATION NOT ACCEPTED value
		*code = BRFCONFLICT;
		// delete BrmAsyncActionProgress object
		delete async;
		// return error
		return RTN_FAIL;
	}
	// no error code will be return
	*code = NOERROR;
	// remove BrmAsyncActionProgress object
	delete async;
	// delete trace object
	if (util_trace)
	{
		delete util_trace;
		util_trace = NULL;
	}
	// return successful
	return RTN_OK;
}

// invoke an actionId on instance
int acs_bur_util::invokeAction(int actionId, string dn, string backupName, int *code, string *message, bool isSecured, string password, string label, bool isRestore)
{
	ACS_TRA_trace *util_trace = new ACS_TRA_trace("acs_bur_util");
	TRACE(util_trace, "acs_bur_util:: invokeAction actionId = %d backupName = %s isSecured = %d isRestore = %d", actionId, backupName.c_str(), isSecured, isRestore);
	bool isResult = true;
	// Define a return variable for operation status
	//	int returnValue;
	// Create an Administrator Operation Object
	//	acs_apgcc_adminoperation admOp;
	ACS_CC_ReturnType result;
	acs_bur_util_admOpAsync admOpasync;
	// devune result variable
	result = admOpasync.init();
	const char *dnObjName1 = dn.c_str();
	// Define timeout of 5 seconds
	//	long long int timeOutVal_5sec = 5*(1000000000LL);
	// Initialize Administrator Operation Object
	//	result = admOp.init();
	if (result != ACS_CC_SUCCESS)
	{
		// set error code
		*code = INITERROR;
		// set error message
		*message = string("Init()");
		cout << "init failed" << endl;
		// return fail
		return RTN_FAIL;
	}

	// Define structure to pass parameter
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	// Prepare vectorIN parameter list
	// waiting for 5 seconds to accept response
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;
	ACS_APGCC_AdminOperationParamType thirdElem;
	/*create parameter for list*/
	char attName1[] = "name";
	// set name into the struct
	firstElem.attrName = attName1;
	// set type of data
	firstElem.attrType = ATTR_STRINGT;
	char *attrValue1 = const_cast<char *>(backupName.c_str());
	firstElem.attrValues = reinterpret_cast<void *>(attrValue1);
	vectorIN.push_back(firstElem);
	// anssi
	if (isSecured == isResult)
	{
		char attName2[] = "backupPassword";
		secondElem.attrName = attName2;
		secondElem.attrType = ATTR_STRINGT;

		char *attrValue2 = const_cast<char *>(password.c_str());
		secondElem.attrValues = reinterpret_cast<void *>(attrValue2);
		vectorIN.push_back(secondElem);

		if (!isRestore)
		{
			char attName3[] = "userLabel";
			thirdElem.attrName = attName3;
			thirdElem.attrType = ATTR_STRINGT;

			char *attrValue3 = const_cast<char *>(label.c_str());
			thirdElem.attrValues = reinterpret_cast<void *>(attrValue3);

			vectorIN.push_back(thirdElem);
		}
	}

	// insert into the vector
	// initialize result operation
	//	returnValue = 0;
	// Invoke action
	// result = admOp.adminOperationInvoke(dnObjName1, 0, actionId, vectorIN , &returnValue, timeOutVal_5sec);
	// Check operation result
	ACS_APGCC_InvocationType invocation = 1;
	result = admOpasync.adminOperationInvokeAsync(invocation, dnObjName1, 0, actionId, vectorIN); // HX68319
	if (result != ACS_CC_SUCCESS)
	{
		// set error code
		*code = INVOKEERR;
		// set message error
		*message = string("adminOperationInvokeAsync()");
		// release Administrator Operation Object
		admOpasync.finalize();
		// return fail
		return RTN_FAIL;
	}
	// release Administrator Operation Object
	result = admOpasync.finalize();
	// test if error occurs
	if (result != ACS_CC_SUCCESS)
	{
		*code = FINALIZEERROR;
		*message = string("finalize()");
		return RTN_FAIL;
	}
	else
	{
		*code = NOERROR;
		return RTN_OK;
	}
	/*	 no error message
		*message="";
		// Testing returnValue if request was accepted
		if (returnValue == SA_AIS_OK){
			// no error code
			*code = NOERROR;
			// return success
			return RTN_OK;
		}
		if (returnValue == 21 && isRestore){
			if (isSecured)
				*code = INVALIDOPT;
			else
				*code = MISSINGOPT;
			// return success
			return RTN_FAIL;
		}
		// Server busy or BRF-C in progress from an other action
		*code = BRFCONFLICT;
		// return fail
		return RTN_FAIL;*/
}

int acs_bur_util::waitForTeminate(int actionId, string dn, string *backupCreationTime, string backupName, int *code, string *message)
{
	acs_bur_BrmBackupManager *brm;
	acs_bur_BrmAsyncActionProgress *async;
	acs_bur_BrmBackup *backup;
	int backupState = 0;
	string dnBrmBackup = "";
	string progressInfo;
	string dnAsyncActionProgress;
	string timeOfTheLastStatusUpdate = "";
	string old_timeOfTheLastStatusUpdate = "";
	string additionalInfo = "";
	int lastProgressPercentage = -1;
	int progressPercentage = 0;
	int result = 0;
	int currentOperation = actionId;
	int watchDogCounter = SLEEPDELAY * 60; // 2 minutes

	// Get DN of BrmBackup according to backupName
	if (acs_bur_util::getBrmBackupDN(&dnBrmBackup, backupName, code, message) == RTN_FAIL)
	{
		return RTN_FAIL;
	}

	// if create backup
	if (CREATEBACKUP == actionId)
	{
		// Printout backup information
		cout << endl
			 << MSG_BKPSTART << endl;
		cout << MSG_INFOARC << endl;
	}
	// if restore backup
	if (RESTOREBACKUP == actionId)
	{
		// Printout restore backup information
		cout << endl
			 << MSG_RSTSTART << endl;
		cout << MSG_INFOBKP << endl;
	}
	// loop until operation will be end or an error occurs
	while (FOREVER)
	{
		// wait for read next update
		sleep(SLEEPDELAY);
		// if action is CREATE BACKUP
		if ((CREATEBACKUP == actionId) || (DELETEBACKUP == actionId))
		{
			// Create a BrmBackupManager object
			brm = new acs_bur_BrmBackupManager((char *)dn.c_str());
			// gets error code
			*code = brm->getOpCode();
			// gets message error
			*message = brm->getOpMessage();
			// if error occurs
			if (*code != NOERROR)
			{
				// Delete BrmBackupManager object
				delete brm;
				// return fail
				return RTN_FAIL;
			}
			// get distinguished name from BrmBAckupManager to create BrmAsyncActionProgress object
			dnAsyncActionProgress = brm->getAsyncActionProgress();
			// delete BrmBAckupManager object
			delete brm;
		}
		// if action is a RESTORE BACKUP
		if (RESTOREBACKUP == actionId)
		{
			// creates a BrmBackup object
			backup = new acs_bur_BrmBackup((char *)dn.c_str());
			// gets error code
			*code = backup->getOpCode();
			// gets message error
			*message = backup->getOpMessage();
			// gets a distinguish name of BrmAsyncActionProgress
			dnAsyncActionProgress = backup->getAsyncActionProgress();
			// delete BrmBackup object
			delete backup;
			// test if error code occurs
			if (*code != NOERROR)
			{
				// return fail
				return RTN_FAIL;
			}
		}
		// Creates BrmAsyncActionProgress object
		async = new acs_bur_BrmAsyncActionProgress((char *)dnAsyncActionProgress.c_str());
		// get error code
		*code = async->getOpCode();
		// get error message
		*message = async->getOpMessage();
		// Checks if error occurs
		if (async->getOpCode() != NOERROR)
		{
			// remove BrmAsyncActionProgress object
			delete async;
			// return fail
			return RTN_FAIL;
		}

		// Check if Brf-C is alive
		timeOfTheLastStatusUpdate = async->getTimeOfLastStatusUpdate();
		// if a new Status update occurs
		if (timeOfTheLastStatusUpdate != old_timeOfTheLastStatusUpdate)
			// Reset watchdog
			watchDogCounter = SLEEPDELAY * 60;
		else
		{
			// no new status update
			// decrement watchdog
			watchDogCounter--;
			// Check if timeout expired
			if (0 == watchDogCounter)
			{
				// set error code
				*code = OPNOTACEPT;
				// set error message
				*message = "";
				// delete BrmAsyncActionProgress object
				delete async;
				// return fail
				return RTN_FAIL;
			}
		}
		// update old compare variable for next time
		old_timeOfTheLastStatusUpdate = timeOfTheLastStatusUpdate;
		// Check for action ID in progress must be CreateBackup
		currentOperation = waitCurrentOperation(actionId, &async, dnAsyncActionProgress);
		// Test if other action in progress , cancel current action for example
		if (currentOperation != actionId)
		{
			// set error code
			*code = CHGCUROP;
			// set error message
			*message = "";
			// delete BrmAsyncActionProgress object
			delete async;
			// return fail
			return RTN_FAIL;
		}
		// get state
		backupState = async->getState();
		// get ProgressInfo
		progressInfo = async->getProgressInfo();
		// get ProgressPercentage info
		progressPercentage = async->getProgressPercentage();
		// get Result of operation
		result = async->getResult();
		// Get Additional Info for Restore Action
		additionalInfo = async->getAdditionalInfo();
		if (additionalInfo.find("rejected the request due to decryption of config file failed") != string::npos)
		{
			*code = INVALIDPWD;
			// set error message
			*message = "";
			// delete BrmAsyncActionProgress object
			delete async;
			// return fail
			return RTN_FAIL;
		}

		// workaround because Brf-C quickly change progress from 0% to 33% we don't lost 0% progress
		if ((-1 == lastProgressPercentage) && (RESTOREBACKUP == actionId))
		{
			lastProgressPercentage = 0;
			cout << lastProgressPercentage << MSG_ADVANCERES << endl
				 << endl;
		}

		// if change progressPercentage value print a message
		if (progressPercentage != lastProgressPercentage)
		{
			// if action is create backup printout backup information
			if ((CREATEBACKUP == actionId) && progressPercentage)
				cout << progressPercentage << MSG_ADVANCE << endl
					 << endl;
			// if action is restore printout restore information
			if ((RESTOREBACKUP == actionId) && progressPercentage)
				cout << progressPercentage << MSG_ADVANCERES << endl
					 << endl;

			// No progress info printout for DELETEBACKUP action in progress

			// update last value for printing change only
			lastProgressPercentage = progressPercentage;
		}
		// delete BrmAsyncActionProgress object
		delete async;
		// set error message
		*message = "";

		// if result is equal to NOT_AVAILABLE continue
		if ((result == SUCCESS) || (result == FAILURE))
		{
			// exit from forever loop for SUCCESS or FAIL of result field
			(result == SUCCESS) ? *code = NOERROR : *code = BACKUPFAIL;
			break;
		}

	} // end while(FOREVER)
	// Reset backup information time string
	*backupCreationTime = string("** DEFAULT **");

	// if action is success test BrmBackup object is correct
	if (SUCCESS == result)
	{

		// Check BrmBackupStatus
		// Creates BrmBackup object
		backup = new acs_bur_BrmBackup((char *)dnBrmBackup.c_str());
		// get error code
		*code = backup->getOpCode();
		// get error message
		*message = backup->getOpMessage();
		// get status
		int backupstatus = backup->getStatus();
		// get CreationTime information
		*backupCreationTime = backup->getCreationTime();
		// remove BrmBackup object
		delete backup;
		// test if no error
		if (NOERROR == *code)
		{
			// test if backup is COMPLETE
			if (backupstatus != BRM_BACKUP_COMPLETE)
			{
				// set error message
				*message = "";
				// set error code
				*code = BACKUPINCOMPLETE;
				// return fail
				return RTN_FAIL;
			}
		}
		else
		{
			// if the action is DELETEBACKUP object is not present in BRM
			if (DELETEBACKUP == actionId)
				return RTN_OK;
			// in every other cases error was generate
			// set error message
			*message = "";
			// set error code
			*code = BACKUPNOTEXIST;
			// return fail
			return RTN_FAIL;
		}

	} // end if (result == SUCCESS)
	// return depending on result field
	return ((SUCCESS == result) ? RTN_OK : RTN_FAIL);
}
//
bool acs_bur_util::validFileName(string name)
{
	// if name is empty returns not valid
	if (true == name.empty())
		return false;
	// Define iterator for string scanning
	string::iterator it;
	for (it = name.begin(); it != name.end(); it++)
	{
		// if character is not alphanumeric
		if (false == isalnum(*it))
		{
			switch (*it)
			{
			// case of '_' character is accepted
			case '_':
			// case of '-' character is accepted
			case '-':
				break;
			// otherwise is error
			default:
				return false;
			}
		}
	}
	// string is valid
	return true;
}

/*! @brief Returns the current action waiting the end of a possible DELETEBACKUP action
 *
 * Fixes TR HS17819
 *
 * First time is used the passed async object, and then for each attempt is get a new one.
 * The new object is reassigned to the passed pointer,
 * in oder to return the new object to the caller.
 *
 * @param[in]  actionId		        The action requested
 * @param[out] async                    The address of the pointer to the acs_bur_BrmAsyncActionProgress object
 * @param[in]  dnAsyncActionProgress    The Action Progress name
 * @return 				The current action
 */
int acs_bur_util::waitCurrentOperation(int actionId, acs_bur_BrmAsyncActionProgress **async, string dnAsyncActionProgress)
{
	int currentOperation = (*async)->getActionId();

	for (int i = 0; i < 30; i++, currentOperation = (*async)->getActionId()) // Try at most for 1 minute (2 seconds * 30)
	{
		if ((CREATEBACKUP == actionId) && (DELETEBACKUP == currentOperation))
			sleep(2);
		else if ((DELETEBACKUP == actionId) && (CREATEBACKUP == currentOperation)) // HX78717
			sleep(2);
		else
			break;

		delete *async;
		*async = new acs_bur_BrmAsyncActionProgress((char *)dnAsyncActionProgress.c_str());
	}
	return currentOperation;
}

// Get BrmBackupManager DN
int acs_bur_util::getBrmBackupMangerDN(string *dnBrmBackupManager, int *code, string *message)
{
	static string brmBackupManagerDN = "";

	// Check if DN if retrieved successfully before if yes, just return the DN previously retrieved
	if (brmBackupManagerDN != "")
	{
		*dnBrmBackupManager = brmBackupManagerDN;
		return RTN_OK;
	}

	OmHandler myOmHandler;
	if (myOmHandler.Init() != ACS_CC_SUCCESS)
	{
		myOmHandler.getExitCode(*code, *message);
		return RTN_FAIL;
	}
	int errCode = RTN_FAIL;
	std::vector<std::string> dnBrmBackupManagerList;
	// Get DN for BrmBackupManager object
	if ((myOmHandler.getClassInstances("BrmBackupManager", dnBrmBackupManagerList) == ACS_CC_SUCCESS) ||
		(myOmHandler.getClassInstances("BrMBrmBackupManager", dnBrmBackupManagerList) == ACS_CC_SUCCESS))
	{
		std::vector<string>::const_iterator it;
		for (it = dnBrmBackupManagerList.begin(); it != dnBrmBackupManagerList.end(); ++it) // HX78724
		{
			brmBackupManagerDN = *it;
			if (brmBackupManagerDN.find("SYSTEM_DATA") != string::npos)
			{
				*dnBrmBackupManager = brmBackupManagerDN;
				break;
			}
		}
		errCode = RTN_OK;
	}
	else
	{
		myOmHandler.getExitCode(*code, *message);
	}

	myOmHandler.Finalize();
	return errCode;
}

// Get BrmBackup DN
int acs_bur_util::getBrmBackupDN(string *dnBrmBackup, string archiveFileName, int *code, string *message)
{

	string dnBrmBackupManager = "";
	if (getBrmBackupMangerDN(&dnBrmBackupManager, code, message) == RTN_FAIL)
	{
		return RTN_FAIL;
	}

	*dnBrmBackup = "brmBackupId=" + archiveFileName + "," + dnBrmBackupManager;
	// cout << "Coming here";
	return RTN_OK;
}

//-------------------------------------------------------------------------------------------------------------------------------------

bool acs_bur_util::getNewConfirmPassword(string pNew) // PL
{
	cout << "\nConfirm password\03: " << flush;
	// get Confirm password
	string pConfirm = getPassword();
	if (pNew != pConfirm)
	{
		// Password not matching
		return false;
	}

	cout << endl;
	return true;
}
//--------------------------------------------------------------------------------------------------
string acs_bur_util::getPassword() // PL
{
	char ch;
	bool bContinue = true;
	struct termios newMode;
	string inputPwd;
	tcgetattr(STDIN_FILENO, &newMode);
	newMode.c_lflag =
		(newMode.c_lflag & ~(ECHO));
	newMode.c_lflag &= ~ICANON;
	newMode.c_lflag &= ~BSDLY;
	tcsetattr(STDIN_FILENO, TCSANOW, &newMode);
	while (bContinue)
	{
		ch = getchar();
		switch (ch)
		{
		case '\n':
			bContinue = false;
			break;
		case 127:
			if (inputPwd.length() != 0)
			{
				inputPwd.resize(inputPwd.length() - 1);
				cout << ("\b \b") << flush;
			}
			break;
		default:

			if (ch >= (' '))
			{
				inputPwd += ch;
				cout << ("*") << flush;
			}
			break;
		}
	}

	echo(true);

	return inputPwd;
}

//-----------------------------------------------------------------------------
void acs_bur_util::echo(bool on)
{
	struct termios settings;
	tcgetattr(STDIN_FILENO, &settings);
	settings.c_lflag =
		on ? (settings.c_lflag | ECHO) : (settings.c_lflag & ~(ECHO));
	settings.c_lflag |= ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}

std::string acs_bur_util::exec(const char *cmd)
{
	char buffer[128];
	std::string result = "";
	FILE *pipe = popen(cmd, "r");
	if (!pipe)
		throw std::runtime_error("popen() failed!");
	try
	{
		while (fgets(buffer, sizeof buffer, pipe) != NULL)
		{
			result += buffer;
		}
	}
	catch (...)
	{
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}

void acs_bur_util_admOpAsync::objectManagerAdminOperationCallback(ACS_APGCC_InvocationType invocation, int returnVal, int error, ACS_APGCC_AdminOperationParamType **outParamVector) // HX68319
{
	// declaration of function
}
/*==================================================================
		ROUTINE:getImmInstances
=================================================================== */
int acs_bur_util::getImmInstances(const char *className, string backupName)
{

	OmHandler immHandler;
	ACS_CC_ReturnType returnCode;
	int errorCode;

	returnCode = immHandler.Init();
	if (returnCode != ACS_CC_SUCCESS)
	{
		return ACS_CC_FAILURE;
	}
	vector<string> dnList;

	returnCode = immHandler.getClassInstances(className, dnList);
	if (returnCode != ACS_CC_SUCCESS)
	{
		errorCode = immHandler.getInternalLastError();
		immHandler.Finalize();

		return ACS_CC_FAILURE;
	}
	string backupDN;
	std::vector<string>::const_iterator it;
	bool backupExist = false;

	for (it = dnList.begin(); it != dnList.end(); ++it)
	{
		backupDN = *it;

		if (backupDN.find(backupName) != string::npos)
		{
			backupExist = true;
			break;
		}
	}
	if (!backupExist)
	{

		return ACS_CC_FAILURE;
	}

	immHandler.Finalize();
	return ACS_CC_SUCCESS;
}
