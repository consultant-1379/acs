/*
 * testTransaction.cpp
 *
 *  Created on: Sep 23, 2011
 *      Author: xfabron
 */

#include <iostream>

#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_OiHandler.h"


#include "ace/Task.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "MyImplementer.h"

using namespace std;

char *className = const_cast<char*>("Test_config");
char *className_2 = const_cast<char*>("Test_config_2");
char *parentName1 = const_cast<char*>("safApp=safImmService");
char *parentName2 = const_cast<char*>("safApp=safLogService");
char *parentName3 = const_cast<char*>("safApp=safSmfService");


char attrdn[]= "RDN_Attribute";
char name_attrFloat[]="ATTR_Float";
char name_attrInt32[]="ATTR_Int32";
char name_attrInt64[]="ATTR_Int64";
char name_attrUnsInt32[]="ATTR_Uns_Int32";
char name_attrUnsInt64[]="ATTR_Uns_Int64";
char name_attrDouble[]="ATTR_Double";
char attrString[]= "ATTR_String";
char attrString_2[]= "ATTR_String_2";
char attrName[]= "ATTR_NameT";
char attrName_2[]= "ATTR_NameT_2";



int createClass(const char* className);
int deleteClass(const char* className);




class MyThread : public ACE_Task_Base {
public:

	MyThread(MyImplementer *p_user1, ACE_Reactor *p_reactor ){

		implementer = p_user1;
		reactor = p_reactor;

	}

	~MyThread(){}

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}
private:

	MyImplementer *implementer;
	ACE_Reactor *reactor;

};





int createClass(const char* className){

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDefined = const_cast<char*>(className);

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char *>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char *>("ATTR_Float"), ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTR_Int32"),ATTR_INT32T,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTR_Int64"),ATTR_INT64T,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnsI32 = {const_cast<char*>("ATTR_Uns_Int32"),ATTR_UINT32T,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnsI64 = {const_cast<char*>("ATTR_Uns_Int64"),ATTR_UINT64T,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeDouble = {const_cast<char*>("ATTR_Double"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeString = {const_cast<char*>("ATTR_String"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeString_2 = {const_cast<char*>("ATTR_String_2"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeNameT = {const_cast<char*>("ATTR_NameT"),ATTR_NAMET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeNameT_2 = {const_cast<char*>("ATTR_NameT_2"),ATTR_NAMET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnsI32);
	classAttributes.push_back(attributeUnsI64);
	classAttributes.push_back(attributeDouble);
	classAttributes.push_back(attributeString);
	classAttributes.push_back(attributeString_2);
	classAttributes.push_back(attributeNameT);
	classAttributes.push_back(attributeNameT_2);

	result=immHandler.Init();

	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}

	result = immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);

	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: defineClass failed!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
				cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDefined<<" defined.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}

	return 0;

}



int deleteClass(const char* className){

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	char *nameClassToBeDelete = const_cast<char*>(className);

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}

	result = immHandler.removeClass(nameClassToBeDelete);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: removeClass failed!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDelete<<" deleted.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		cout<<"ERROR CODE: "<<immHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<immHandler.getInternalLastErrorText()<<endl;
		return -1;
	}

	return 0;
}



void createObject_Test_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createObject_Test_1()" << endl;
	cout << " TEST: try to create three object in transactional mode" << endl;
	cout << " RESULT: Three new object of class Test_config are created. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("createTransaction");

	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("test_1");
	char *rdName_2 = const_cast<char*>("test_2");
	char *rdName_3 = const_cast<char*>("test_3");


	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute second object**/
	value[0] = reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}
}








void createObject_Test_2(){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createObject_Test_2()" << endl;
	cout << " TEST: try to create three object in transactional mode" << endl;
	cout << " RESULT: Three new object, children of object previously created are created. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("createTransaction_2");

	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("testChildren_1");
	char *rdName_2 = const_cast<char*>("testChildren_2");
	char *rdName_3 = const_cast<char*>("testChildren_3");


	char *parentName1 = const_cast<char*>("test_1,safApp=safImmService");
	char *parentName2 = const_cast<char*>("test_2,safApp=safLogService");
	char *parentName3 = const_cast<char*>("test_3,safApp=safSmfService");


	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute second object**/
	value[0] = reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}
}




void createObject_Test_3(){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createObject_Test_3()" << endl;
	cout << " TEST: try to create three object in transactional mode" << endl;
	cout << " RESULT: Three new object are created. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("createTransaction_2");

	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("testBisChildren_1");
	char *rdName_2 = const_cast<char*>("testRootObject");
	char *rdName_3 = const_cast<char*>("testChildren_3");


	char *parentName1 = const_cast<char*>("test_1,safApp=safImmService");
	char *parentName2 = 0;
	char *parentName3 = 0;


	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className_2, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute second object**/
	value[0] = reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className_2, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}
}



void createObject_Test_4(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createObject_Test_4()" << endl;
	cout << " TEST:  lunch two transaction one to create object and another one to modify attribute" << endl;
	cout << " RESULT: The first transaction failed the other one work fine. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;
	bool flag_2 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("createTransaction_4");
	std::string transactionName_2("modifyTransaction_2");

	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("testChildren_1");
	char *rdName_2 = const_cast<char*>("testChildren_4");
	char *rdName_3 = const_cast<char*>("testChildren_5");


	char *parentName1 = const_cast<char*>("test_1,safApp=safImmService");
	char *parentName2 = const_cast<char*>("test_2,safApp=safLogService");
	char *parentName3 = const_cast<char*>("test_3,safApp=safSmfService");

	char *objName_1 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,safApp=safLogService");



	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result != ACS_CC_SUCCESS);
	AttrList.clear();



	ACS_CC_ImmParameter parToModify;
	float newFloatValue = 19.21;
	parToModify.attrName = name_attrFloat;
	parToModify.attrType = ATTR_FLOATT;
	parToModify.attrValuesNum = 1;
	void* new_valueFloat[1] = {reinterpret_cast<void*>(&newFloatValue)};
	parToModify.attrValues = new_valueFloat;

	result = omHandler.modifyAttribute(objName_1, &parToModify, transactionName_2);
	if ( result != ACS_CC_SUCCESS ){
		cout << "Change parameter " << name_attrFloat <<" of object "<< objName_1 << " failed "<< endl;
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_2 = false;
	}else {
		cout << "Parameter " << name_attrFloat << " of object " <<objName_1 <<" change successfully "<< endl;
	}


	ASSERTM("ACS_APGCC_OmHandler::modifyAttribute  ", result == ACS_CC_SUCCESS);

	/**Define Attribute second object**/
	value[0] = reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	AttrList.clear();


	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	/*Modify object*/
	ACS_CC_ImmParameter parToModify_2;
	int newInt32Value = 23;
	parToModify_2.attrName = name_attrInt32;
	parToModify_2.attrType = ATTR_INT32T;
	parToModify_2.attrValuesNum = 1;
	void* new_valueInt32[1] = {reinterpret_cast<void*>(&newInt32Value)};
	parToModify_2.attrValues = new_valueInt32;

	result = omHandler.modifyAttribute(objName_2, &parToModify_2, transactionName_2);
	if ( result != ACS_CC_SUCCESS ){
		cout << "Change parameter " << name_attrInt32 <<" of object "<< objName_2 << " failed "<< endl;
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_2 = false;
	}else {
		cout << "Parameter " << name_attrInt32 << " of object " <<objName_2 <<" change successfully "<< endl;
	}

	ASSERTM("ACS_APGCC_OmHandler::modifyAttribute  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result != ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			ASSERTM("ACS_APGCC_OmHandler::resetRequest  ", result == ACS_CC_SUCCESS);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	/**if all ok apply transaction**/
	if( flag_2 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName_2);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName_2.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName_2);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName_2.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName_2.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName_2.c_str());
		result = omHandler.resetRequest(transactionName_2);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName_2.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName_2.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}



	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}

}



void deleteObject_Test_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " deleteObject_Test_1()" << endl;
	cout << " TEST: try to delete three object in transactional mode" << endl;
	cout << " RESULT: The object created with the previously method are deleted. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("deleteTransaction_1");

	/**DN object to delete**/
	char *objName_1 = const_cast<char*>("testChildren_1,test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("testChildren_2,test_2,safApp=safLogService");
	char *objName_3 = const_cast<char*>("testChildren_3,test_3,safApp=safSmfService");



	/**DELETE FIRST OBJECT**/
	result = omHandler.deleteObject(objName_1, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("deletion object %s failed!!!\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE SECOND OBJECT**/
	result = omHandler.deleteObject(objName_2, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("deletion object %s failed!!!\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE THIRD OBJECT**/
	result = omHandler.deleteObject(objName_3, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_3);
	}else{
		printf("deletion object %s failed!!!\n", objName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}

}



void createDeleteObject_Test_1(){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createDeleteObject_Test_1()" << endl;
	cout << " TEST:  lunch two transaction one to create object and another one to modify attribute" << endl;
	cout << " RESULT: The first transaction create three object under the same parent,\n"
			" the other transaction delete two object previously created." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;
	bool flag_2 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("createTransaction_4");
	std::string transactionName_2("deleteTransaction_2");



	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("test_1");
	char *rdName_2 = const_cast<char*>("test_2");
	char *rdName_3 = const_cast<char*>("test_3");


	char *parentName1 = const_cast<char*>("test_1,safApp=safImmService");
	char *parentName2 = const_cast<char*>("test_1,safApp=safImmService");
	char *parentName3 = const_cast<char*>("test_1,safApp=safImmService");


	/**DN object to delete**/
	char *dnObjName_1 = const_cast<char*>("testRootObject");
	char *dnObjName_2 = const_cast<char*>("testChildren_3");


	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);
	AttrList.clear();



	/**DELETE FIRST OBJECT**/
	result = omHandler.deleteObject(dnObjName_1, transactionName_2);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", dnObjName_1);
	}else{
		printf("deletion object %s failed!!!\n", dnObjName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_2 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**Define Attribute second object**/
	value[0] = reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);
	AttrList.clear();



	/**DELETE SECOND OBJECT**/
	result = omHandler.deleteObject(dnObjName_2, transactionName_2);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", dnObjName_2);
	}else{
		printf("deletion object %s failed!!!\n", dnObjName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_2 = false;

	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply first transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	/**if all ok apply second transaction**/
	if( flag_2 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName_2);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName_2.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName_2);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName_2.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName_2.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName_2.c_str());
		result = omHandler.resetRequest(transactionName_2);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName_2.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName_2.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}

	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}



}




void modifyObject_Test_1(){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " modifyAttribute_Test_1()" << endl;
	cout << " TEST: try to modify three attribute of three different object in transactional mode" << endl;
	cout << " RESULT: The changes are  applied. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;
	bool flag = true;

	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	std::string transactionName("modifyTransaction");

	char *objName_1 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,safApp=safLogService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safSmfService");


	ACS_CC_ImmParameter parToModify;
	float newFloatValue = 9.9;
	parToModify.attrName = name_attrFloat;
	parToModify.attrType = ATTR_FLOATT;
	parToModify.attrValuesNum = 1;
	void* new_valueFloat[1] = {reinterpret_cast<void*>(&newFloatValue)};
	parToModify.attrValues = new_valueFloat;

	result = omHandler.modifyAttribute(objName_1, &parToModify, transactionName);
	if ( result != ACS_CC_SUCCESS ){
		cout << "Change parameter " << name_attrFloat <<" of object "<< objName_1 << " failed "<< endl;
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}else {
		cout << "Parameter " << name_attrFloat << " of object " <<objName_1 <<" change successfully "<< endl;
	}
	ASSERTM("ACS_APGCC_OmHandler::modifyAttribute  ", result == ACS_CC_SUCCESS);

	ACS_CC_ImmParameter parToModify_2;
	int newInt32Value = 9;
	parToModify_2.attrName = name_attrInt32;
	parToModify_2.attrType = ATTR_INT32T;
	parToModify_2.attrValuesNum = 1;
	void* new_valueInt32[1] = {reinterpret_cast<void*>(&newInt32Value)};
	parToModify_2.attrValues = new_valueInt32;

	result = omHandler.modifyAttribute(objName_2, &parToModify_2, transactionName);
	if ( result != ACS_CC_SUCCESS ){
		cout << "Change parameter " << name_attrInt32 <<" of object "<< objName_2 << " failed "<< endl;
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}else {
		cout << "Parameter " << name_attrInt32 << " of object " <<objName_2 <<" change successfully "<< endl;
	}
	ASSERTM("ACS_APGCC_OmHandler::modifyAttribute  ", result == ACS_CC_SUCCESS);


	ACS_CC_ImmParameter parToModify_3;
	char *newStringValue = const_cast<char*>("pippo_9");
	parToModify_3.attrName = attrString;
	parToModify_3.attrType = ATTR_STRINGT;
	parToModify_3.attrValuesNum = 1;
	void* new_valueString[1] = {reinterpret_cast<void*>(newStringValue)};
	parToModify_3.attrValues = new_valueString;

	result = omHandler.modifyAttribute(objName_3, &parToModify_3, transactionName);
	if ( result != ACS_CC_SUCCESS ){
		cout << "Change parameter " << attrString <<" of object "<< objName_3 << " failed "<< endl;
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}else {
		cout << "Parameter " << attrString << " of object " <<objName_3 <<" change successfully "<< endl;
	}
	ASSERTM("ACS_APGCC_OmHandler::modifyAttribute  ", result == ACS_CC_SUCCESS);


	if( flag ){
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Requests applied\n", transactionName.c_str());
			ACS_CC_ImmParameter paramAfterChange;
			paramAfterChange.attrName = name_attrFloat;
			result = omHandler.getAttribute(objName_1, &paramAfterChange );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange.attrValuesNum; i++){
					cout<<"attribute "<< name_attrFloat <<" new value: "<<*(float*)paramAfterChange.attrValues[i]<<endl;
				}
			}

			ACS_CC_ImmParameter paramAfterChange_2;
			paramAfterChange_2.attrName = name_attrInt32;
			result = omHandler.getAttribute(objName_2, &paramAfterChange_2 );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange_2.attrValuesNum; i++){
					cout<<"attribute "<< name_attrInt32 <<" new value: "<<*(int*)paramAfterChange_2.attrValues[i]<<endl;
				}
			}

			ACS_CC_ImmParameter paramAfterChange_3;
			paramAfterChange_3.attrName = attrString;
			result = omHandler.getAttribute(objName_3, &paramAfterChange_3 );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange_3.attrValuesNum; i++){
					cout<<"attribute "<< attrString <<" new value: "<<(char*)paramAfterChange_3.attrValues[i]<<endl;
				}
			}

		}else{
			printf("Apply requests failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
			}
		}

	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
		}

	}


	cout<<endl;
	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}



}



void clearObject(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " clearObject()" << endl;
	cout << " TEST: try to delete in transactional mode all objects previously created" << endl;
	cout << " RESULT: All the object are deleted are deleted. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("clearTransaction_1");

	/**DN object to delete**/
	char *objName_1 = const_cast<char*>("test_1,test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,test_1,safApp=safImmService");
	char *objName_3 = const_cast<char*>("test_3,test_1,safApp=safImmService");
	char *objName_4 = const_cast<char*>("testBisChildren_1,test_1,safApp=safImmService");
	char *objName_5 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_6 = const_cast<char*>("test_2,safApp=safLogService");
	char *objName_7 = const_cast<char*>("test_3,safApp=safSmfService");



	/**DELETE FIRST OBJECT**/
	result = omHandler.deleteObject(objName_1, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("deletion object %s failed!!!\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE SECOND OBJECT**/
	result = omHandler.deleteObject(objName_2, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("deletion object %s failed!!!\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);


	/**DELETE THIRD OBJECT**/
	result = omHandler.deleteObject(objName_3, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_3);
	}else{
		printf("deletion object %s failed!!!\n", objName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE OBJECT 4**/
	result = omHandler.deleteObject(objName_4, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_4);
	}else{
		printf("deletion object %s failed!!!\n", objName_4);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}


	/**DELETE OBJECT 5**/
	result = omHandler.deleteObject(objName_5, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_5);
	}else{
		printf("deletion object %s failed!!!\n", objName_5);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE OBJECT 6**/
	result = omHandler.deleteObject(objName_6, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_6);
	}else{
		printf("deletion object %s failed!!!\n", objName_6);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE OBJECT 7**/
	result = omHandler.deleteObject(objName_7, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_7);
	}else{
		printf("deletion object %s failed!!!\n", objName_7);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}


}

void transactionPhase_2(OmHandler *omHandler){

	ACS_CC_ReturnType result;

	bool flag_1 = true;

	/**Transaction Name**/
	std::string transactionName("Transaction_1");

	char *rdName_2 = const_cast<char*>("test_2");


	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/**Define Attribute second object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeINT32;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_2);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;




	attributeINT32.attrName = name_attrInt32;
	attributeINT32.attrType = ATTR_INT32T;
	attributeINT32.attrValuesNum = 1;
	int intValueForObject = 12;
	void* valueInt32[1]={ reinterpret_cast<void*>(&intValueForObject)};
	attributeINT32.attrValues = valueInt32;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeINT32);


	/**CREATE SECOND OBJECT**/
	result = omHandler->createObject(className, parentName2, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName2);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName2);
		cout << "ERROR CODE: "<< omHandler->getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler->getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	AttrList.clear();

}


void transactionPhase_1(){

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;


	ACS_APGCC_OiHandler oiHandler;

	string implementerName("impName_2");
	MyImplementer implementer_1(implementerName);

	result = oiHandler.addClassImpl(&implementer_1, className);
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s registered  \n", implementerName.c_str(), className );
	}else{
		printf("Set Implementer %s failed\n", implementerName.c_str());
		return;
	}


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();


	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("Transaction_1");


	/**RDN object to create**/
	char *rdName_1 = const_cast<char*>("test_1");
	char *rdName_3 = const_cast<char*>("test_3");



	vector<ACS_CC_ValuesDefinitionType> AttrList;


	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentName1, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName1);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}


	AttrList.clear();


	transactionPhase_2(&omHandler);


	/**Define Attribute third object**/
	value[0] = reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	ACS_CC_ValuesDefinitionType attributeStringT;

	attributeStringT.attrName = attrString;
	attributeStringT.attrType = ATTR_STRINGT;
	attributeStringT.attrValuesNum = 1;
	char *stringValue = const_cast<char*>("pippoStart");
	void* valueStringT[1]={ reinterpret_cast<void*>(stringValue)};
	attributeStringT.attrValues = valueStringT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeStringT);

	/**CREATE THIRD OBJECT**/
	result = omHandler.createObject(className, parentName3, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName3);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}


	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;





	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}


	result = oiHandler.removeClassImpl(&implementer_1, className);
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s removed  \n", implementerName.c_str(), className );
	}else{
		printf("Remove Implementer %s failed\n", implementerName.c_str());
		return;
	}
//	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);


	implementer_1.reactor()->end_reactor_event_loop();

	delete reactor;
	delete tp_reactor_impl;




}




void deleteObject_Test_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " deleteObject_Test_2()" << endl;
	cout << " TEST: try to delete three object in transactional mode" << endl;
	cout << " RESULT: The object created with the previously method are deleted. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}


	/**Transaction Name**/
	std::string transactionName("deleteTransaction_1");

	/**DN object to delete**/
	char *objName_1 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,safApp=safLogService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safSmfService");



	/**DELETE FIRST OBJECT**/
	result = omHandler.deleteObject(objName_1, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("deletion object %s failed!!!\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE SECOND OBJECT**/
	result = omHandler.deleteObject(objName_2, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("deletion object %s failed!!!\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE THIRD OBJECT**/
	result = omHandler.deleteObject(objName_3, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_3);
	}else{
		printf("deletion object %s failed!!!\n", objName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	cout<<endl;

	/**if all ok apply transaction**/
	if( flag_1 ){
		/**if all ok apply transaction**/
		result = omHandler.applyRequest(transactionName);
		ASSERTM("ACS_APGCC_OmHandler::applyRequest  ", result == ACS_CC_SUCCESS);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			/**iff apply failed reste the transaction**/
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
				cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
				cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			}
		}

	}else {
		/**if an error occurs abort transaction**/
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}

}






void testCreateObjectWithParentNull(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " testCreateObjectWithParentNull()" << endl;
	cout << " TEST: try to create an object with parent null " << endl;
	cout << " RESULT: The object is created correctly \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	bool flag_1 = true;

	/**Initialize IMM Service**/
	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
		cout<<"ERROR CODE: "<<omHandler.getInternalLastError()<<endl;
		cout<<"ERROR MESSAGE: "<<omHandler.getInternalLastErrorText()<<endl;
	}



	/**DN object to delete**/
	char *rdName_1 = const_cast<char*>("test_1");
	char *parentRoot = 0;
	char *parentRoot_2 = const_cast<char*>("test_1");
	char *objName_1 = const_cast<char*>("test_1");
	char *objName_2 = const_cast<char*>("test_1,test_1");


	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/**Define Attribute first object**/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	char* rdnValue = const_cast<char*>(rdName_1);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	/**CREATE FIRST OBJECT**/
	result = omHandler.createObject(className, parentRoot, AttrList);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentRoot);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentRoot);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);


	/**CREATE SECOND OBJECT**/
	result = omHandler.createObject(className, parentRoot_2, AttrList);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentRoot_2);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentRoot_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}


	ASSERTM("ACS_APGCC_OmHandler::createObject  ", result == ACS_CC_SUCCESS);


	/**DELETE SECOND OBJECT**/
	result = omHandler.deleteObject(objName_2);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("deletion object %s failed!!!\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);

	/**DELETE FIRST OBJECT**/
	result = omHandler.deleteObject(objName_1);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("deletion object %s failed!!!\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("ACS_APGCC_OmHandler::deleteObject  ", result == ACS_CC_SUCCESS);


	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
	}

}










void runSuite(){

	cute::suite s;

	s.push_back(CUTE(createObject_Test_1));
	s.push_back(CUTE(createObject_Test_2));
	s.push_back(CUTE(createObject_Test_3));
	s.push_back(CUTE(createObject_Test_4));
	s.push_back(CUTE(deleteObject_Test_1));
	s.push_back(CUTE(createDeleteObject_Test_1));
	s.push_back(CUTE(modifyObject_Test_1));
	s.push_back(CUTE(clearObject));
	s.push_back(CUTE(transactionPhase_1));
	s.push_back(CUTE(deleteObject_Test_2));
	s.push_back(CUTE(testCreateObjectWithParentNull));


	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}


int main(){

	createClass(className);
	createClass(className_2);

	runSuite();

	deleteClass(className);
	deleteClass(className_2);

}
