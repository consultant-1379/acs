//============================================================================
// Name        : transactionalOperationUT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ace/Task.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_APGCC_OiHandler.h"

#include "MyImplementer.h"

using namespace std;


char *className = const_cast<char*>("Test_config");
char *parentName = const_cast<char*>("safApp=safImmService");

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






int createClass(const char* className);
int deleteClass(const char* className);



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
		return -1;
	}

	result = immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);

	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: defineClass failed!!!\n";
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDefined<<" defined.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
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
		return -1;
	}

	result = immHandler.removeClass(nameClassToBeDelete);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: removeClass failed!!!\n";
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDelete<<" deleted.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
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

	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
	}

	std::string transactionName("createTransaction");

	char *rdName_1 = const_cast<char*>("test_1");
	char *rdName_2 = const_cast<char*>("test_2");
	char *rdName_3 = const_cast<char*>("test_3");


	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*the attributes*/
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

	result = omHandler.createObject(className, parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_1, parentName);
	}else{
		printf("creation object %s,%s failed!!!\n", rdName_1, parentName);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}
	ASSERTM("TEST: createObject test 1 - create first object \n", result == ACS_CC_SUCCESS);

	AttrList.clear();

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

	result = omHandler.createObject(className, parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_2, parentName);
	}else{
		printf("creation object %s,%s failed\n", rdName_2, parentName);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;

	}
	ASSERTM("TEST: createObject test 1 - create second object \n", result == ACS_CC_SUCCESS);

	AttrList.clear();

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

	result = omHandler.createObject(className, parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", rdName_3, parentName);
	}else{
		printf("creation object %s,%s failed\n", rdName_3, parentName);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag_1 = false;
	}

	ASSERTM("TEST: createObject test 1 - create third object \n", result == ACS_CC_SUCCESS);

	cout<<endl;

	if( flag_1 ){
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
			printf("Apply request failed\n");
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
			result = omHandler.resetRequest(transactionName);
			if(result == ACS_CC_SUCCESS){
				printf("%s resetRequest SUCCESS\n", transactionName.c_str());
			}else{
				printf("%s resetRequest FAILURE\n", transactionName.c_str());
			}
		}

		ASSERTM("TEST: createObject test 1 - apply Request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
		}

		ASSERTM("TEST: createObject test 1 - reset Request \n", result == ACS_CC_SUCCESS);
	}


	cout<<endl;

	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


/*Cancella tre oggetti creati prima*/
void deleteObject_Test_1(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " deleteObject_Test_1()" << endl;
	cout << " TEST: try to delete three object in transactional mode" << endl;
	cout << " RESULT: The object created with the previously method are deleted. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;
	bool flag = true;

	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization failed!!!\n";
	}

	std::string transactionName("deleteTransaction");

	char *objName_1 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,safApp=safImmService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safImmService");

	result = omHandler.deleteObject(objName_1, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("deletion object %s failed\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 1 - delete first object \n", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject(objName_2, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("deletion object %s failed\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 1 - delete second object \n", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject(objName_3, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_3);
	}else{
		printf("deletion object %s failed\n", objName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 1 - delete third object \n", result == ACS_CC_SUCCESS);

	if( flag ){
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Apply request success\n", transactionName.c_str());
		}else{
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
		ASSERTM("TEST: deleteObject test 1 - apply Request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
		ASSERTM("TEST: deleteObject test 1 - reset Request \n", result == ACS_CC_SUCCESS);
	}

	cout<<endl;
	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}


}

/*Fallimento nella creazione di uno degli oggetti*/
void createObject_Test_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " createObject_Test_2()" << endl;
	cout << " TEST: try to create three object in transactional mode" << endl;
	cout << " RESULT: The objects not are created because one creation request is failed. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;
	bool flag = true;

	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	std::string transactionName("createTransaction_2");

	char *rdName_1 = const_cast<char*>("test_1");
	char *rdName_2 = const_cast<char*>("test_2");
	char *rdName_3 = const_cast<char*>("test_3");


	//CREAZIONE NUOVO OGGETTO
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;
	ACS_CC_ValuesDefinitionType attributeI32;
	ACS_CC_ValuesDefinitionType attributeI64;
	ACS_CC_ValuesDefinitionType attributeUnsI32;
	ACS_CC_ValuesDefinitionType attributeUnsI64;
	ACS_CC_ValuesDefinitionType attributeDouble;
	ACS_CC_ValuesDefinitionType attributeString;
	ACS_CC_ValuesDefinitionType attributeString_2;
	ACS_CC_ValuesDefinitionType attributeNameT;
	ACS_CC_ValuesDefinitionType attributeNameT_2;


	/*Fill the rdn Attribute */
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* value[1]={ reinterpret_cast<void*>(rdName_1)};
	attributeRDN.attrValues = value;


	/*Fill the attribute ATTR_Float*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType = ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 1;
	float floatValueForObject = 12.34;
	void* valueFloat[1]={ reinterpret_cast<void*>(&floatValueForObject)};
	attributeFLOAT.attrValues = valueFloat;


	/*Fill the attribute ATTR_Int32*/
	attributeI32.attrName = name_attrInt32;
	attributeI32.attrType = ATTR_INT32T;
	attributeI32.attrValuesNum = 1;
	int integerValueForObject = 1234;
	void* valueInt[1]={ reinterpret_cast<void*>(&integerValueForObject)};
	attributeI32.attrValues = valueInt;


	/*Fill the attribute ATTR_Int64*/
	attributeI64.attrName = name_attrInt64;
	attributeI64.attrType = ATTR_INT64T;
	attributeI64.attrValuesNum = 1;
	long long integer64ValueForObject = -9999;
	void* valueInt64[1]={ reinterpret_cast<void*>(&integer64ValueForObject)};
	attributeI64.attrValues = valueInt64;


	/*Fill the attribute ATTR_Uns_Int32*/
	attributeUnsI32.attrName = name_attrUnsInt32;
	attributeUnsI32.attrType = ATTR_UINT32T;
	attributeUnsI32.attrValuesNum = 1;
	long long unsInteger32ValueForObject = 1234;
	void* valueUnsInt32[1]={ reinterpret_cast<void*>(&unsInteger32ValueForObject)};
	attributeUnsI32.attrValues = valueUnsInt32;


	/*Fill the attribute ATTR_Uns_Int64*/
	attributeUnsI64.attrName = name_attrUnsInt64;
	attributeUnsI64.attrType = ATTR_UINT64T;
	attributeUnsI64.attrValuesNum = 1;
	unsigned long long unsInteger64ValueForObject = 9999;
	void* valueUnsInt64[1]={ reinterpret_cast<void*>(&unsInteger64ValueForObject)};
	attributeUnsI64.attrValues = valueUnsInt64;


	/*Fill the attribute ATTR_Double*/
	attributeDouble.attrName = name_attrDouble;
	attributeDouble.attrType = ATTR_DOUBLET;
	attributeDouble.attrValuesNum = 1;
	double doubleValueForObject = 2.35;
	void* valueDouble[1]={ reinterpret_cast<void*>(&doubleValueForObject)};
	attributeDouble.attrValues = valueDouble;


	attributeString.attrName = attrString;
	attributeString.attrType = ATTR_STRINGT;
	attributeString.attrValuesNum = 1;
	char* strValue = const_cast<char*>("pippoStr");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	attributeString.attrValues = valueStr;


	attributeString_2.attrName = attrString_2;
	attributeString_2.attrType = ATTR_STRINGT;
	attributeString_2.attrValuesNum = 2;
	char* strValue_2 = const_cast<char*>("pippoStr_2");
	char* strValue_2bis = const_cast<char*>("pippoStr_2");
	void* valueStr_2[2]={ reinterpret_cast<void*>(strValue_2), reinterpret_cast<void*>(strValue_2bis)};
	attributeString_2.attrValues = valueStr_2;


	attributeNameT.attrName = attrName;
	attributeNameT.attrType = ATTR_NAMET;
	attributeNameT.attrValuesNum = 1;
	char* nameValue= const_cast<char*>("pippoNameT");
	void* valueName[1]={ reinterpret_cast<void*>(nameValue)};
	attributeNameT.attrValues = valueName;


	attributeNameT_2.attrName = attrName_2;
	attributeNameT_2.attrType = ATTR_NAMET;
	attributeNameT_2.attrValuesNum = 2;
	char* nameValue_2 = const_cast<char*>("pippoNameT_2");
	char* nameValue_2bis = const_cast<char*>("pippoNameT_2bis");
	void* valueName_2[2]={ reinterpret_cast<void*>(nameValue_2), reinterpret_cast<void*>(nameValue_2bis)};
	attributeNameT_2.attrValues = valueName_2;


	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);
	AttrList.push_back(attributeI32);
	AttrList.push_back(attributeI64);
	AttrList.push_back(attributeUnsI32);
	AttrList.push_back(attributeUnsI64);
	AttrList.push_back(attributeDouble);
	AttrList.push_back(attributeString);
	AttrList.push_back(attributeString_2);
	AttrList.push_back(attributeNameT);
	AttrList.push_back(attributeNameT_2);


	result = omHandler.createObject(className, parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("Object %s created\n", rdName_1);
	}else{
		printf("Creation of object %s failed\n", rdName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}

	ASSERTM("TEST: createObject test 2 - create first object \n", result == ACS_CC_SUCCESS);

	AttrList.clear();


	value[0]=reinterpret_cast<void*>(rdName_2);
	attributeRDN.attrValues = value;

	char* strValue2_2 = const_cast<char*>("pippoStr2_2");
	char* strValue2_2bis = const_cast<char*>("pippoStr2_2");
	void* valueStr2_2[2]={ reinterpret_cast<void*>(strValue2_2), reinterpret_cast<void*>(strValue2_2bis)};
	attributeString_2.attrValues = valueStr2_2;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeString_2);

	result = omHandler.createObject("className", parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("Object %s created\n", rdName_2);
	}else{
		printf("Creation of object %s failed\n", rdName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag=false;
	}
	ASSERTM("TEST: createObject test 2 - create second object \n", result == ACS_CC_FAILURE);

	AttrList.clear();

	value[0]=reinterpret_cast<void*>(rdName_3);
	attributeRDN.attrValues = value;

	/*Fill the attribute ATTR_Uns_Int32*/
	unsInteger32ValueForObject = 1234;
	valueUnsInt32[0]=reinterpret_cast<void*>(&unsInteger32ValueForObject);
	attributeUnsI32.attrValues = valueUnsInt32;


	/*Fill the attribute ATTR_Uns_Int64*/
	unsInteger64ValueForObject = 9999;
	valueUnsInt64[0] = reinterpret_cast<void*>(&unsInteger64ValueForObject);
	attributeUnsI64.attrValues = valueUnsInt64;


	/*Fill the attribute ATTR_Double*/
	doubleValueForObject = 2.35;
	valueDouble[0] = reinterpret_cast<void*>(&doubleValueForObject);
	attributeDouble.attrValues = valueDouble;


	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeUnsI32);
	AttrList.push_back(attributeUnsI64);
	AttrList.push_back(attributeDouble);

	result = omHandler.createObject(className, parentName, AttrList, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("Object %s created\n", rdName_3);
	}else{
		printf("Creation of object %s failed\n", rdName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag=false;
	}
	ASSERTM("TEST: createObject test 2 - create third object \n", result == ACS_CC_SUCCESS);


	if( flag ){
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Aplly request success\n", transactionName.c_str());
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

		ASSERTM("TEST: createObject test 2 - apply Request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
		}

		ASSERTM("TEST: createObject test 2 - reset Request \n", result == ACS_CC_SUCCESS);
	}

	result = omHandler.Finalize();
	if(result != ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
	}

}


void deleteObject_Test_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " deleteObject_Test_2()" << endl;
	cout << " TEST: try to delete three object in transactional mode" << endl;
	cout << " RESULT: The objects not are deleted because one deletion request is failed. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;
	bool flag = true;

	result = omHandler.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	std::string transactionName("deleteTransaction_2");

	char *objName_1 = const_cast<char*>("test_1,safApp=safImmService");
	char *objName_2 = const_cast<char*>("test_2,safApp=safImmService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safImmService");

	result = omHandler.deleteObject(objName_1, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_1);
	}else{
		printf("Deletion object %s failed\n", objName_1);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 2 - delete first object \n", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject("objName_2", transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_2);
	}else{
		printf("Deletion object %s failed\n", objName_2);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 2 - delete second object \n", result == ACS_CC_FAILURE);

	result = omHandler.deleteObject(objName_3, transactionName);
	if(result == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName_3);
	}else{
		printf("Deletion object %s failed\n", objName_3);
		cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		flag = false;
	}
	ASSERTM("TEST: deleteObject test 2 - delete third object \n", result == ACS_CC_SUCCESS);

	if( flag ){
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Requests applied\n", transactionName.c_str());
		}else{
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
		ASSERTM("TEST: deleteObject test 2 - apply Request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
			cout << "ERROR CODE: "<< omHandler.getInternalLastError() << endl;
			cout << "ERROR MESSAGE: " << omHandler.getInternalLastErrorText() << endl;
		}
		ASSERTM("TEST: deleteObject test 2 - reset Request \n", result == ACS_CC_SUCCESS);
	}

	cout<<endl;
	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}

void modifyAttribute_Test_1(){

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
	char *objName_2 = const_cast<char*>("test_2,safApp=safImmService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safImmService");

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
	ASSERTM("TEST: modifyAttribute test 1 - modify first Attribute \n", result == ACS_CC_SUCCESS);

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
	ASSERTM("TEST: modifyAttribute test 1 - modify second Attribute \n", result == ACS_CC_SUCCESS);


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
	ASSERTM("TEST: modifyAttribute test 1 - modify third Attribute \n", result == ACS_CC_SUCCESS);


	if( flag ){
		result = omHandler.applyRequest(transactionName);
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
		ASSERTM("TEST: modifyAttribute test 1 - apply request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());
		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
		}
		ASSERTM("TEST: modifyAttribute test 1 - reset request \n", result == ACS_CC_SUCCESS);
	}


	cout<<endl;
	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


void modifyAttribute_Test_2(){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " modifyAttribute_Test_2()" << endl;
	cout << " TEST: try to modify three attribute of three different object in transactional mode" << endl;
	cout << " RESULT: The changes not are applied because one change request is failed. \n" << endl;
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
	char *objName_2 = const_cast<char*>("test_2,safApp=safImmService");
	char *objName_3 = const_cast<char*>("test_3,safApp=safImmService");

	ACS_CC_ImmParameter parToModify;
	float newFloatValue = 8.8;
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
	ASSERTM("TEST: modifyAttribute test 1 - modify first Attribute \n", result == ACS_CC_SUCCESS);

	ACS_CC_ImmParameter parToModify_2;
	int newInt32Value = 8;
	parToModify_2.attrName = const_cast<char*>("name_attrInt32");
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
	ASSERTM("TEST: modifyAttribute test 1 - modify second Attribute \n", result == ACS_CC_FAILURE);


	ACS_CC_ImmParameter parToModify_3;
	char *newStringValue = const_cast<char*>("pippo_8");
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
	ASSERTM("TEST: modifyAttribute test 1 - modify third Attribute \n", result == ACS_CC_SUCCESS);


	if( flag ){
		result = omHandler.applyRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s Request applied\n", transactionName.c_str());

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
		ASSERTM("TEST: modifyAttribute test 1 - apply request \n", result == ACS_CC_SUCCESS);
	}else {
		printf("%s, requests not applied because one is failed\n", transactionName.c_str());
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS){
			printf("%s resetRequest SUCCESS\n", transactionName.c_str());

			ACS_CC_ImmParameter paramAfterChange;
			paramAfterChange.attrName = name_attrFloat;
			result = omHandler.getAttribute(objName_1, &paramAfterChange );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange.attrValuesNum; i++){
					cout<<"attribute "<< name_attrFloat <<" old value: "<<*(float*)paramAfterChange.attrValues[i]<<endl;
				}
			}

			ACS_CC_ImmParameter paramAfterChange_2;
			paramAfterChange_2.attrName = name_attrInt32;
			result = omHandler.getAttribute(objName_2, &paramAfterChange_2 );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange_2.attrValuesNum; i++){
					cout<<"attribute "<< name_attrInt32 <<" old value: "<<*(int*)paramAfterChange_2.attrValues[i]<<endl;
				}
			}

			ACS_CC_ImmParameter paramAfterChange_3;
			paramAfterChange_3.attrName = attrString;
			result = omHandler.getAttribute(objName_3, &paramAfterChange_3 );

			if( result == ACS_CC_SUCCESS ){
				for(unsigned int i=0; i<paramAfterChange_3.attrValuesNum; i++){
					cout<<"attribute "<< attrString <<" old value: "<<(char*)paramAfterChange_3.attrValues[i]<<endl;
				}
			}


		}else{
			printf("%s resetRequest FAILURE\n", transactionName.c_str());
		}
		ASSERTM("TEST: modifyAttribute test 1 - reset request \n", result == ACS_CC_SUCCESS);
	}


	cout<<endl;
	result = omHandler.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}

void implementer_Test_1(){

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);


	ACS_APGCC_OiHandler oiHandler;

	string implementerName("impName_1");
	MyImplementer implementer_1(implementerName);

	result = oiHandler.addClassImpl(&implementer_1, className);
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s registered  \n", implementerName.c_str(), className );
	}else{
		printf("Set Implementer %s failed\n", implementerName.c_str());
		return;
	}

	//ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();


	createObject_Test_1();
	deleteObject_Test_1();



	result = oiHandler.removeClassImpl(&implementer_1, className);
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s removed  \n", implementerName.c_str(), className );
	}else{
		printf("Remove Implementer %s failed\n", implementerName.c_str());
		return;
	}
//	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);


	implementer_1.reactor()->end_reactor_event_loop();


	result = omHandler.Finalize();
//	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void implementer_Test_2(){

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);


	ACS_APGCC_OiHandler oiHandler;

	string implementerName("impName_1");
	MyImplementer implementer_1(implementerName);

	result = oiHandler.addClassImpl(&implementer_1, "className");
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s registered  \n", implementerName.c_str(), className );
	}else{
		printf("Set Implementer %s failed\n", implementerName.c_str());
		return;
	}

	//ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);

}


void implementer_Test_3(){

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);


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

	//ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();


	createObject_Test_2();
	createObject_Test_1();
	modifyAttribute_Test_2();
	deleteObject_Test_1();



	result = oiHandler.removeClassImpl(&implementer_1, className);
	if(result == ACS_CC_SUCCESS){
		printf("Implementer %s for class %s removed  \n", implementerName.c_str(), className );
	}else{
		printf("Remove Implementer %s failed\n", implementerName.c_str());
		return;
	}
//	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);


	implementer_1.reactor()->end_reactor_event_loop();


	result = omHandler.Finalize();
//	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}



void runSuite(){

	cute::suite s;

	s.push_back(CUTE(createObject_Test_1));
	s.push_back(CUTE(deleteObject_Test_2));
	s.push_back(CUTE(modifyAttribute_Test_1));
	s.push_back(CUTE(modifyAttribute_Test_2));
	s.push_back(CUTE(deleteObject_Test_1));
	s.push_back(CUTE(createObject_Test_2));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}

int main() {

	createClass(className);

	runSuite();
//	implementer_Test_1();
//	cout<<"-------------------------------------------------------------------------\n\n";
//	implementer_Test_3();
//	cout<<"-------------------------------------------------------------------------\n\n";
//	implementer_Test_2();

	deleteClass(className);

	return 0;
}
