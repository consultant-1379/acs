//============================================================================
// Name        : getInstancesUT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "acs_apgcc_omhandler.h"

using namespace std;


int createClass(const char* className);
int deleteClass(const char* className);
int createObject(const char* p_className, const char* rdName);
int deleteObject(const char* objectName);


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

char *className = const_cast<char*>("Test_config");
char *className_2 = const_cast<char*>("Test_config_2");
char *className_3 = const_cast<char*>("Test_config_3");

char *rdName_1 = const_cast<char*>("instanceTest_config_1");
char *rdName_2 = const_cast<char*>("instanceTest_config_2");
char *rdName_3 = const_cast<char*>("instanceTest_config_3");
char *rdName_4 = const_cast<char*>("instanceTest_config_4");
char *rdName_5 = const_cast<char*>("instanceTest_config_5");

char *rdName3_1 = const_cast<char*>("instanceTest_config_3_1");
char *rdName3_2 = const_cast<char*>("instanceTest_config_3_2");
char *rdName3_3 = const_cast<char*>("instanceTest_config_3_3");

char *objectName_1 = const_cast<char*>("instanceTest_config_1,safApp=safImmService");
char *objectName_2 = const_cast<char*>("instanceTest_config_2,safApp=safImmService");
char *objectName_3 = const_cast<char*>("instanceTest_config_3,safApp=safImmService");
char *objectName_4 = const_cast<char*>("instanceTest_config_4,safApp=safImmService");
char *objectName_5 = const_cast<char*>("instanceTest_config_5,safApp=safImmService");

char *objectName3_1 = const_cast<char*>("instanceTest_config_3_1,safApp=safImmService");
char *objectName3_2 = const_cast<char*>("instanceTest_config_3_2,safApp=safImmService");
char *objectName3_3 = const_cast<char*>("instanceTest_config_3_3,safApp=safImmService");

char *objectName[] = {
		objectName_1,
		objectName_2,
		objectName_3,
		objectName_4,
		objectName_5
};


char *objectName3[] = {
		objectName3_1,
		objectName3_2,
		objectName3_3,
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
		return -1;
	}
	result=immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);

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


int createObject(const char* p_className, const char *rdName){

	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;

	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	char* nomeParent = const_cast<char*>("safApp=safImmService");

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
	char* rdnValue = const_cast<char*>(rdName);
	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
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


	returnCode=immHandler.createObject(p_className, nomeParent, AttrList);
	if(returnCode == ACS_CC_SUCCESS){
		printf("Object %s created\n", rdnValue);
	}else{
		printf("Creation of object %s failed\n", rdnValue);
	}


	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	return 0;

}




int deleteObject(const char * objectName){

	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;

	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	char *objName = const_cast<char*>(objectName);

	returnCode=immHandler.deleteObject(objName) ;
	if(returnCode == ACS_CC_SUCCESS){
		printf("Object %s deleted\n", objName);
	}else{
		printf("Cancellation of object %s failed\n", objName);
	}


	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	return 0;
}


void getInstaces_Test_1(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;


	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}


	result = immHandle.getClassInstances(className, p_dnList);
	if(result != ACS_CC_SUCCESS){
		cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
		cout << "ERROR CODE: "<< immHandle.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << immHandle.getInternalLastErrorText() << endl;
	}
	ASSERTM("TEST: getClassInstances test 1 - method return ", result == ACS_CC_SUCCESS);

	int instancesNumber  = p_dnList.size();
	cout<<" Number of instances found: "<<instancesNumber<<endl;
	ASSERTM("TEST: getClassInstances test 1 - number of instances found ", instancesNumber == 5);

	int maching = 0;
	for (unsigned i=0; i<p_dnList.size(); i++){

		cout<<" Instances "<< i+1 <<": "<<p_dnList[i].c_str()<<endl;

		if ( (maching = strcmp(p_dnList[i].c_str(), objectName[i])) != 0 ){
			cout<<"ERROR: instance " << p_dnList[i].c_str() <<" not matching with " <<objectName[i]<<endl;
		}

		ASSERTM("TEST: getClassInstances test 1 - Instance matching ", maching == 0);
	}

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}

/*ERROR TEST nome classe non esistente*/
void getInstaces_Test_2(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	result = immHandle.getClassInstances("Test_not_exist", p_dnList);

	ASSERTM("TEST: getClassInstances test 2 - method return ", result == ACS_CC_FAILURE);
	ASSERTM("TEST: getClassInstances test 2 - method errorCode check ", immHandle.getInternalLastError() == -41);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


/*ERROR TEST classe senza instanze*/
void getInstaces_Test_3(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	result = immHandle.getClassInstances(className_2, p_dnList);

	ASSERTM("TEST: getClassInstances test 3 - method return ", result == ACS_CC_FAILURE);
	ASSERTM("TEST: getClassInstances test 3 - method errorCode check ", immHandle.getInternalLastError() == -41);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


/*ERROR TEST nome Class Nullo*/
void getInstaces_Test_4(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	char *nameClass = 0;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}


	result = immHandle.getClassInstances(nameClass, p_dnList);

	ASSERTM("TEST: getClassInstances test 4 - method return ", result == ACS_CC_FAILURE);
	ASSERTM("TEST: getClassInstances test 4 - method errorCode check ", immHandle.getInternalLastError() == -7);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}



/*TEST piu oggetti*/
void getInstaces_Test_5(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	char *nameClass = 0;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}


	result = immHandle.getClassInstances(className, p_dnList);
	if(result != ACS_CC_SUCCESS){
		cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
		cout << "ERROR CODE: "<< immHandle.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << immHandle.getInternalLastErrorText() << endl;
	}
	ASSERTM("TEST: getClassInstances test 5 - method return ", result == ACS_CC_SUCCESS);

	int instancesNumber  = p_dnList.size();
	cout<<" Class "<< className <<" number of instances found: "<<instancesNumber<<endl;
	ASSERTM("TEST: getClassInstances test 5 - number of instances found ", instancesNumber == 5);

	int maching = 0;
	for (unsigned i=0; i<p_dnList.size(); i++){

		cout<<" Instances "<< i+1 <<": "<<p_dnList[i].c_str()<<endl;

		if ( (maching = strcmp(p_dnList[i].c_str(), objectName[i])) != 0 ){
			cout<<"ERROR: instance " << p_dnList[i].c_str() <<" not matching with " <<objectName[i]<<endl;
		}

		ASSERTM("TEST: getClassInstances test 5 - Instance matching ", maching == 0);
	}


	p_dnList.clear();
	cout<<endl;

	result = immHandle.getClassInstances(className_3, p_dnList);
	if(result != ACS_CC_SUCCESS){
		cout << "Method OmHandler::getCLassInstances Failure!!!" << endl;
		cout << "ERROR CODE: "<< immHandle.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << immHandle.getInternalLastErrorText() << endl;
	}
	ASSERTM("TEST: getClassInstances test 5 - method return ", result == ACS_CC_SUCCESS);

	instancesNumber  = p_dnList.size();
	cout<<" Class "<< className_3 <<" number of instances found: "<<instancesNumber<<endl;
	ASSERTM("TEST: getClassInstances test 5 - number of instances found ", instancesNumber == 3);

	maching = 0;
	for (unsigned i=0; i<p_dnList.size(); i++){

		cout<<" Instances "<< i+1 <<": "<<p_dnList[i].c_str()<<endl;

		if ( (maching = strcmp(p_dnList[i].c_str(), objectName3[i])) != 0 ){
			cout<<"ERROR: instance " << p_dnList[i].c_str() <<" not matching with " <<objectName3[i]<<endl;
		}

		ASSERTM("TEST: getClassInstances test 5 - Instance matching ", maching == 0);
	}

	p_dnList.clear();
	cout<<endl;

	result = immHandle.getClassInstances(className_2, p_dnList);

	ASSERTM("TEST: getClassInstances test 3 - method return ", result == ACS_CC_FAILURE);
	ASSERTM("TEST: getClassInstances test 3 - method errorCode check ", immHandle.getInternalLastError() == -41);

	instancesNumber  = p_dnList.size();
	cout<<" Class "<< className_2 <<" number of instances found: "<<instancesNumber<<endl;


	p_dnList.clear();
	cout<<endl;

	result = immHandle.getClassInstances(nameClass, p_dnList);

	ASSERTM("TEST: getClassInstances test 2 - method return ", result == ACS_CC_FAILURE);
	ASSERTM("TEST: getClassInstances test 2 - method errorCode check ", immHandle.getInternalLastError() == -7);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


void runSuite(){

	cute::suite s;

	s.push_back(CUTE(getInstaces_Test_1));
	s.push_back(CUTE(getInstaces_Test_2));
	s.push_back(CUTE(getInstaces_Test_3));
	s.push_back(CUTE(getInstaces_Test_4));
	s.push_back(CUTE(getInstaces_Test_5));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}

int main() {


	createClass(className);
	createClass(className_2);
	createClass(className_3);

	createObject(className, rdName_1);
	createObject(className, rdName_2);
	createObject(className, rdName_3);
	createObject(className, rdName_4);
	createObject(className, rdName_5);

	createObject(className_3, rdName3_1);
	createObject(className_3, rdName3_2);
	createObject(className_3, rdName3_3);

	runSuite();

	deleteObject(objectName_1);
	deleteObject(objectName_2);
	deleteObject(objectName_3);
	deleteObject(objectName_4);
	deleteObject(objectName_5);

	deleteObject(objectName3_1);
	deleteObject(objectName3_2);
	deleteObject(objectName3_3);

	deleteClass(className);
	deleteClass(className_2);
	deleteClass(className_3);

	return 0;
}

