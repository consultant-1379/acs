//============================================================================
// Name        : ut_prot_reg_part.cpp
// Author      : egimarr
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "ut_prot_reg_part.h"
int removePartecipant() {

	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;

	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"Error in Init() function for BRMDemo removePartecipant";
		return -1;
	}

	//Now delete the Object
	//   brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1
	//
	char *objName = const_cast<char*>("brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1");
	returnCode=immHandler.deleteObject(objName) ;
	if(returnCode != ACS_CC_SUCCESS){
		printf("Object Deletion %s Failure\n", objName);
		returnCode=immHandler.Finalize();
		return -1;
	}

	/* Close the connection with IMM, and reset all handlers */
	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS)
	{
		cout << "Error in Finalize() function for BRMDemo removePartecipant" << endl;
		return -1;
	}

	return 0;

}
int createPartecipant(void){

	char attrdn[]= "brfPersistentDataOwnerId";
	char attrVersion[]="version";
	char attrBackupType[]="backupType";
	char attrRebootAfterRestore[]="rebootAfterRestore";
	char *className = const_cast<char*>("BrfPersistentDataOwner");

	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;

	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"Error in Init() function for BrfPersistentDataOwner Object";
		return -1;
	}

	//char* nomeParent = const_cast<char*>("brfPartecipantContainerId=1");
	// brfPersistentDataOwnerId=CoreMW,brfParticipantContainerId=1
	char* nomeParent = const_cast<char*>("brfParticipantContainerId=1");

	//Create attributes list
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeVersion;
	ACS_CC_ValuesDefinitionType attributeBackupType;
	ACS_CC_ValuesDefinitionType attributeRebootAfterRestore;
	/*Fill the rdn Attribute */

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ERIC-APG-ACS-PRC");
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;

	attributeVersion.attrName = attrVersion;
	attributeVersion.attrType = ATTR_STRINGT;
	attributeVersion.attrValuesNum = 1;
	char* strValue = const_cast<char*>("1.0");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	attributeVersion.attrValues = valueStr;

	attributeBackupType.attrName = attrBackupType;
	attributeBackupType.attrType = ATTR_INT32T;
	attributeBackupType.attrValuesNum = 1;
	int intAttributeBackupType = 1;
	void* valueInt[1]={ reinterpret_cast<void*>(&intAttributeBackupType)};
	attributeBackupType.attrValues = valueInt;

	attributeRebootAfterRestore.attrName = attrRebootAfterRestore;
	attributeRebootAfterRestore.attrType = ATTR_INT32T;
	attributeRebootAfterRestore.attrValuesNum = 1;
	int intRebootAfterRestore = 0;
	void* valueInt2[1]={ reinterpret_cast<void*>(&intRebootAfterRestore)};
	attributeRebootAfterRestore.attrValues = valueInt2;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeVersion);
	AttrList.push_back(attributeBackupType);
	//AttrList.push_back(attributeRebootAfterRestore);

	returnCode=immHandler.createObject(className, nomeParent, AttrList);

	if(returnCode != ACS_CC_SUCCESS){
			printf("BrfPartecipantDataOwner Object Creation o %s Failure\n", rdnValue);
			returnCode=immHandler.Finalize();
			return -1;
	}

	/* Close the connection with IMM, and reset all handlers */
	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS)
	{
		return -1;
	}

	return 0;
}

void test_create_BrfPersistentDataOwner(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_create_BrfPersistentDataOwner " << endl;
	cout << " TEST: Check BrfPersistentDataOwner object creation with a specific DN as  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: immfind and immcfg commands lauched from pipe shell" << endl;
	cout << " RESULT: Positive cases with parameters accepted  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	int result = -1;
	string resultBuffer = "";
	result = acs_bur_util::do_command("immfind -c BrfPersistentDataOwner | grep ERIC-APG-ACS-PRC",&resultBuffer);
	if(0==resultBuffer.compare("brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1")){
		cout << "Already created "<<endl;
		ASSERT_EQUALM("CUTE: check return value of test_create_BrfPersistentDataOwner ", 0 , -1  );
		return;
	}
	result = createPartecipant();
	sleep(1);
	result = acs_bur_util::do_command("immfind -c BrfPersistentDataOwner | grep ERIC-APG-ACS-PRC",&resultBuffer);
	result = resultBuffer.compare("brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1");
	ASSERT_EQUALM("CUTE: check return value of test_create_BrfPersistentDataOwner ", 0 , result  );
	//result = removePartecipant();
	//sleep(1);
}

MyThread *simThread;
MyImplementer implementer,implementer1;

void test_successful_backup_creation(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_successful_backup_creation " << endl;
	cout << " TEST: Check Successful Backup Creation Scenario  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Positive cases object created \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	int result = -1;
	string resultBuffer = "";

	/*result = createPartecipant();
	if (-1 == result ){
		ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", 0 , result  );
		return;
	}*/

	const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1";
	const char *impName = "ut_prot_reg_part";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	acs_apgcc_oihandler_V3 oiHandler;

	implementer.setObjName(dnObjName);
	implementer.setImpName(impName);
	implementer.setScope(scope);

	int r = oiHandler.addObjectImpl(&implementer);

	if(r != ACS_CC_SUCCESS){
		cout << " ->ERROR: Implementer not registered for PRC Participant " << endl;
	}

	int fd = 0;
	fd= implementer.getSelObj();

	simThread = new MyThread(&implementer);

	simThread->activate();
	int last;

	// Here start create backup
	cout << "Create backup by immadm command" << endl;

	int resultSystem = -1;
	string bufferOutput = "";

	acs_bur_util::do_command("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1",&bufferOutput);

	if( bufferOutput.find("SA_AIS_ERR_BUSY") != string::npos ){
		cout << "administravive operation on create backup can't be called "<< endl;
		ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", 0 , resultSystem  );
		simThread->stop();
		oiHandler.removeObjectImpl(&implementer);
		delete simThread;
		return;
	}

	sleep(2);
	int max_cnt = 120;
	while(1){
		sleep(1);
		if(last!=implementer.getLastMessageReceived()){
			last = implementer.getLastMessageReceived();
			cout << "last actionId received is " << last <<endl;
		}
		if((last == BRF_PARTICIPANT__COMMIT_BACKUP) || (last == BRF_PARTICIPANT__CANCEL_BACKUP))
			break;
		if(last==-1){
			ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", BRF_PARTICIPANT__COMMIT_BACKUP , last  );
			simThread->stop();
			oiHandler.removeObjectImpl(&implementer);
			delete simThread;
			return;
		}
		max_cnt--;
		if(max_cnt==0){
			ASSERT_EQUALM("CUTE: (timeout)check return value of test_successful_backup_creation ", 0 , -1  );
			simThread->stop();
			oiHandler.removeObjectImpl(&implementer);
			delete simThread;
			return;
		}

	}

	// wait until create backup ends
	max_cnt = 360;
	while(1){
		sleep(1);
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'result ' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if result == SUCCESS
		if(bufferOutput.compare("1")!=0){
			break;
		}
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'state' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if state == CANCELLED
		if(bufferOutput.compare("4")!=0){
			break;
		}
		max_cnt--;
		if(max_cnt==0){
			ASSERT_EQUALM("CUTE: (timeout)check return value of test_successful_backup_creation ", 0 , -1  );
			simThread->stop();
			oiHandler.removeObjectImpl(&implementer);
			delete simThread;
			return;
		}
	}

	ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", BRF_PARTICIPANT__COMMIT_BACKUP , last  );

	cout << "Wait 5 second before to delete backup "<<endl;

	sleep(5);

	cout << "Deleting BACKUPUNITTEST backup "<<endl;
	// Delete last test unit backup BACKUPUNITTEST
	acs_bur_util::do_command("immadm -o 1 -p name:SA_STRING_T:BACKUPUNITTEST brmBackupManagerId=SYSTEM_DATA,brMId=1",&bufferOutput);

	cout << "Wait 5 second after delete backup "<<endl;

	sleep(5);
	bufferOutput = "0";

	// wait until delete backup ends
	while(bufferOutput.compare("1")!=0){
		sleep(1);
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'result ' | awk -F ' ' '{print $3}'",&bufferOutput);
		// cout << "bufferOutput="<<bufferOutput.c_str()<< endl;
	}

	// Stop Thread
	cout << "Stopping simulation thread " << endl;
	simThread->stop();
	sleep(1);
	delete simThread;
	sleep(1);
	oiHandler.removeObjectImpl(&implementer);
	// sleep(3);
	// Delete Object
	// cout << "Delete brfPersistentDataOwner object " << endl;
	// int counter = 0;
	// result = -1;
	// immcfg -d  brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1 -u
	//resultSystem = system("immcfg -d  brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1 -u");
}

void test_unsuccessful_backup_creation(){
	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " test_unsuccessful_backup_creation " << endl;
	cout << " TEST: Check unsuccessful Backup Creation Scenario  "<<endl;
	cout << " TARGET TEST ENVIRONMENT: " << endl;
	cout << " RESULT: Negative cases object created \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout << "wait 2 minutes ..." << endl;

	sleep(120);

	int result = -1;
	string resultBuffer = "";

	const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-PRC,brfParticipantContainerId=1";
	const char *impName = "ut_prot_reg_part";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	acs_apgcc_oihandler_V3 oiHandler;

	implementer1.setObjName(dnObjName);
	implementer1.setImpName(impName);
	implementer1.setScope(scope);

	int r = oiHandler.addObjectImpl(&implementer1);

	if(r != ACS_CC_SUCCESS){
		cout << " ->ERROR: Implementer not registered for PRC Participant " << endl;
	}

	int fd = 0;
	fd= implementer1.getSelObj();

	// Set Error diskspace
	implementer1.setStatus(BRF_ERROR_DISKSPACE);

	// Reset last message received code
	implementer1.lastMessageReceived = 0;

	simThread = new MyThread(&implementer1);

	simThread->activate();
	int last;

	// Here start create backup
	cout << "Create backup by immadm command" << endl;

	int resultSystem = -1;
	string bufferOutput = "";

	acs_bur_util::do_command("immadm -o 0 -p name:SA_STRING_T:BACKUPUNITTEST1 brmBackupManagerId=SYSTEM_DATA,brMId=1",&bufferOutput);

	if( bufferOutput.find("SA_AIS_ERR_BUSY") != string::npos ){
		cout << "administrative operation on create backup can't be called "<< endl;
		ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", 0 , resultSystem  );
		simThread->stop();
		oiHandler.removeObjectImpl(&implementer);
		delete simThread;
		return;
	}

	sleep(2);

	while(1){
		sleep(1);
		if(last!=implementer1.getLastMessageReceived()){
			last = implementer1.getLastMessageReceived();
			cout << "last actionId received is " << last <<endl;
		}
		if((last == BRF_PARTICIPANT__COMMIT_BACKUP) || (last == BRF_PARTICIPANT__CANCEL_BACKUP))
			break;
		if(last==-1){
			ASSERT_EQUALM("CUTE: check return value of test_successful_backup_creation ", BRF_PARTICIPANT__COMMIT_BACKUP , last  );
			simThread->stop();
			oiHandler.removeObjectImpl(&implementer1);
			delete simThread;
			return;
		}
	}

	// wait until create backup ends

	while(1){
		sleep(1);
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'result ' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if result == SUCCESS
		if(bufferOutput.compare("1")!=0){
			break;
		}
		acs_bur_util::do_command("immlist id=1,brmBackupManagerId=SYSTEM_DATA,brMId=1 | grep 'state' | awk -F ' ' '{print $3}'",&bufferOutput);
		// if state == CANCELLED
		if(bufferOutput.compare("4")!=0){
			break;
		}
	}

	ASSERT_EQUALM("CUTE: check return value of test_unsuccessful_backup_creation ", BRF_PARTICIPANT__CANCEL_BACKUP , last  );

	// Stop Thread
	cout << "Stopping simulation thread " << endl;
	simThread->stop();
	sleep(1);
	delete simThread;
	sleep(1);
	oiHandler.removeObjectImpl(&implementer1);
}
void runSuite(){
	cute::suite s;
	//TODO add your test here

	s.push_back(CUTE(test_create_BrfPersistentDataOwner));
	s.push_back(CUTE(test_successful_backup_creation));
	s.push_back(CUTE(test_unsuccessful_backup_creation));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "Test suite for Register Participant Prototype");

}

int main() {

	cout << "Start Test Register Participant Prototype" << endl;
	runSuite();
	cout << "End Test Register Participant Prototype" << endl;
	removePartecipant();
	return 0;
}


