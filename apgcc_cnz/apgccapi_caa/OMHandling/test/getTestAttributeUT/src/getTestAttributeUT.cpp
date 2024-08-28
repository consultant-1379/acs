//============================================================================
// Name        : getTestAttributeUT.cpp
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


int createClass();
int deleteClass();
int createObject();
int deleteObject();
void printChildren(const char* p_rootName, std::vector<std::string> &p_rdnList);
void printObject(const char *p_nameObj, vector<ACS_APGCC_ImmAttribute> &objAttributeList);
void printAttribute(ACS_APGCC_ImmAttribute *attribute);




int createClass(){
	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDefined = const_cast<char*>("Test_Config");

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
		cout<<"ERROR: defineClass FAILURE!!!\n";
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



int deleteClass(){

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	char *nameClassToBeDelete = const_cast<char*>("Test_Config");

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	result = immHandler.removeClass(nameClassToBeDelete);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: removeClass FAILURE!!!\n";
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



int createObject(){

	char *className = const_cast<char*>("Test_Config");

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
	char* rdnValue = const_cast<char*>("ObjectTest");
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


	returnCode=immHandler.createObject(className, nomeParent, AttrList);
	if(returnCode == ACS_CC_SUCCESS){
		printf("Creazione oggetto %s eseguita\n", rdnValue);
	}else{
		printf("Creazione oggetto %s fallita\n", rdnValue);
	}


	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	return 0;

}



int deleteObject(){

	ACS_CC_ReturnType returnCode;
	OmHandler immHandler;

	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	char *objName = const_cast<char*>("ObjectTest,safApp=safImmService");

	returnCode=immHandler.deleteObject(objName) ;
	if(returnCode == ACS_CC_SUCCESS){
		printf("Cancellazione oggetto %s eseguita\n", objName);
	}else{
		printf("Cancellazione oggetto %s fallita\n", objName);
	}


	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		return -1;
	}

	return 0;
}

void printChildren(const char* p_rootName, std::vector<std::string> &p_rdnList){

	cout << "Object with root: " << p_rootName << endl;
	cout << "Number of object: " << p_rdnList.size() << endl;
	for ( unsigned int i = 0; i < p_rdnList.size(); i++ ){
		cout << " " << i+1 <<". " << p_rdnList[i] << endl;
	}
}



void printObject(const char *p_nameObj, vector<ACS_APGCC_ImmAttribute> &objAttributeList){
	cout << "Elenco attributi "<< p_nameObj << endl;

	for ( unsigned int i = 0; i < objAttributeList.size(); i++ ){
		cout << " " << i+1 <<". " << objAttributeList[i].attrName << endl;
		cout << "     " <<"attrType: " << objAttributeList[i].attrType << endl;
		cout << "     " <<"attrValueNumber: " << objAttributeList[i].attrValuesNum << endl;

		for (unsigned int j = 0; j < objAttributeList[i].attrValuesNum; j++){
			if ( (objAttributeList[i].attrType == 9) || (objAttributeList[i].attrType == 6) ){
				cout << "     " << (char*)(objAttributeList[i].attrValues[j]);
			}else if (objAttributeList[i].attrType == 2){
				cout << "     " << *(reinterpret_cast<unsigned int*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 1){
				cout << "     " << *(reinterpret_cast<int*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 3){
				cout << "     " << *(reinterpret_cast<long long*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 4){
				cout << "     " << *(reinterpret_cast<unsigned long long*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 7){
				cout << "     " << *(reinterpret_cast<float*>(objAttributeList[i].attrValues[j]));
			}else if (objAttributeList[i].attrType == 8){
				cout << "     " << *(reinterpret_cast<double*>(objAttributeList[i].attrValues[j]));
			}
		}
		cout << endl;
	}
}



void printAttribute(ACS_APGCC_ImmAttribute *attribute){

	cout << "attribute " << attribute->attrName << endl;
	cout << "attrType: " << attribute->attrType << endl;
	cout << "attrValueNumber: " << attribute->attrValuesNum << endl;


	for (unsigned int j = 0; j < attribute->attrValuesNum; j++){
		if ( (attribute->attrType == 9) || (attribute->attrType == 6) ){
			cout << "     " << (char*)(attribute->attrValues[j]);
		}else if (attribute->attrType == 2){
			cout << "     " << *(reinterpret_cast<unsigned int*>(attribute->attrValues[j]));
		}else if (attribute->attrType == 1){
			cout << "     " << *(reinterpret_cast<int*>(attribute->attrValues[j]));
		}else if (attribute->attrType == 3){
			cout << "     " << *(reinterpret_cast<long long*>(attribute->attrValues[j]));
		}else if (attribute->attrType == 4){
			cout << "     " << *(reinterpret_cast<unsigned long long*>(attribute->attrValues[j]));
		}else if (attribute->attrType == 7){
			cout << "     " << *(reinterpret_cast<float*>(attribute->attrValues[j]));
		}else if (attribute->attrType == 8){
			cout << "     " << *(reinterpret_cast<double*>(attribute->attrValues[j]));
		}
	}
	cout << endl;
}



void getChildrenTest(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	char *rootName = const_cast<char*>("safApp=safImmService");
	std::vector<std::string> rdnList;
	result = immHandle.getChildren(rootName, ACS_APGCC_SUBTREE, &rdnList);
	ASSERTM("TEST: getChildren - getChildren test 1", result== ACS_CC_SUCCESS);

	printChildren(rootName, rdnList);
	cout<< endl;

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}

void getChildrenTest_2(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	char *rootName = const_cast<char*>("safApp=safImmService");
	std::vector<std::string> rdnList_2;
	result = immHandle.getChildren(rootName, ACS_APGCC_SUBLEVEL, &rdnList_2);
	ASSERTM("TEST: getChildren - getChildren test 2", result== ACS_CC_SUCCESS);

	printChildren(rootName, rdnList_2);
	cout<< endl;

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


void getChildrenTest_3(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	char *rootNameWrong = const_cast<char*>("safApp=safImmServiceWrong");
	std::vector<std::string> rdnList_3;
	result = immHandle.getChildren(rootNameWrong, ACS_APGCC_SUBTREE, &rdnList_3);
	ASSERTM("TEST: getChildren - getChildren negative test ", result== ACS_CC_FAILURE);

	printChildren(rootNameWrong, rdnList_3);
	cout<< endl;

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


void getObjectTest(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	char *nameObject =  const_cast<char*>("ObjectTest,safApp=safImmService");
	ACS_APGCC_ImmObject object;
	object.objName = nameObject;
	result = immHandle.getObject(&object);
	ASSERTM("TEST: getObject - getObject test 1", result == ACS_CC_SUCCESS);

	printObject(nameObject, object.attributes);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}



void getObjectTest_2(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	char *nameObjectWrong =  const_cast<char*>("ObjectTest_Wrong,safApp=safImmService");
	ACS_APGCC_ImmObject object_2;
	object_2.objName = nameObjectWrong;
	result = immHandle.getObject(&object_2);
	ASSERTM("TEST: getObject - getObject test 2", result == ACS_CC_FAILURE);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}
}




void getAttribute(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	char *nameObject =  const_cast<char*>("ObjectTest,safApp=safImmService");

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	ACS_APGCC_ImmAttribute attribute_1;
	ACS_APGCC_ImmAttribute attribute_2;
	ACS_APGCC_ImmAttribute attribute_3;
	ACS_APGCC_ImmAttribute attribute_4;
	ACS_APGCC_ImmAttribute attribute_5;
	ACS_APGCC_ImmAttribute attribute_6;
	ACS_APGCC_ImmAttribute attribute_7;
	ACS_APGCC_ImmAttribute attribute_8;
	ACS_APGCC_ImmAttribute attribute_9;
	ACS_APGCC_ImmAttribute attribute_10;
	ACS_APGCC_ImmAttribute attribute_11;


	attribute_1.attrName = attrdn;
	attribute_2.attrName = name_attrFloat;
	attribute_3.attrName = name_attrInt32;
	attribute_4.attrName = name_attrInt64;
	attribute_5.attrName = name_attrUnsInt32;
	attribute_6.attrName = name_attrUnsInt64;
	attribute_7.attrName = name_attrDouble;
	attribute_8.attrName = attrString;
	attribute_9.attrName = attrString_2;
	attribute_10.attrName = attrName;
	attribute_11.attrName = attrName_2;

	std::vector<ACS_APGCC_ImmAttribute *> attributes;
	attributes.push_back(&attribute_1);
	attributes.push_back(&attribute_2);
	attributes.push_back(&attribute_3);
	attributes.push_back(&attribute_4);
	attributes.push_back(&attribute_5);
	attributes.push_back(&attribute_6);
	attributes.push_back(&attribute_7);
	attributes.push_back(&attribute_8);
	attributes.push_back(&attribute_9);
	attributes.push_back(&attribute_10);
	attributes.push_back(&attribute_11);

	result = immHandle.getAttribute(nameObject, attributes);
	ASSERTM("TEST: getAttribute - getAttribute test 1", result == ACS_CC_SUCCESS);


	cout << endl;
	cout << endl;

	for(int i=0; i<(int)attributes.size(); i++){
		printAttribute(attributes[i]);
	}
	cout << endl;

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}
}



void getAttribute_2(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	char *nameObject =  const_cast<char*>("ObjectTest,safApp=safImmService");

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	std::vector<ACS_APGCC_ImmAttribute *> attributes_2;
	ACS_APGCC_ImmAttribute attribute_2;
	ACS_APGCC_ImmAttribute attribute_3;

	attribute_2.attrName = name_attrFloat;
	attribute_3.attrName = "test_attribute";
	attributes_2.push_back(&attribute_2);
	attributes_2.push_back(&attribute_3);

	result = immHandle.getAttribute(nameObject, attributes_2);
	ASSERTM("TEST: getAttribute - getAttribute test 2", result == ACS_CC_FAILURE);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}


void getAttribute_3(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	std::vector<ACS_APGCC_ImmAttribute *> attributes_2;
	ACS_APGCC_ImmAttribute attribute_2;
	ACS_APGCC_ImmAttribute attribute_3;
	attribute_2.attrName = name_attrFloat;
	attribute_3.attrName = "test_attribute";
	attributes_2.push_back(&attribute_2);
	attributes_2.push_back(&attribute_3);

	char *nameObject_2 =  const_cast<char*>("ObjectTest_2,safApp=safImmService");
	result = immHandle.getAttribute(nameObject_2, attributes_2);
	ASSERTM("TEST: getAttribute - getAttribute test 3", result == ACS_CC_FAILURE);


	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}

}



void getSingleAttributeTest(){

	OmHandler immHandle;
	ACS_CC_ReturnType result;
	char *nameObject =  const_cast<char*>("ObjectTestTR,safApp=safImmService");

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
	}

	ACS_CC_ImmParameter attribute;
	attribute.attrName = const_cast<char*>("test_attribute");

	result = immHandle.getAttribute(nameObject, &attribute);
	ASSERTM("TEST: getSingleAttributeTest - getSingleAttributeTest test 1", result == ACS_CC_FAILURE);

	result = immHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
	}
}

void runSuite(){
	cute::suite s;

	s.push_back(CUTE(getChildrenTest));
	s.push_back(CUTE(getChildrenTest_2));
	s.push_back(CUTE(getChildrenTest_3));
	s.push_back(CUTE(getObjectTest));
	s.push_back(CUTE(getObjectTest_2));
	s.push_back(CUTE(getAttribute));
	s.push_back(CUTE(getAttribute_2));
	s.push_back(CUTE(getAttribute_3));
	s.push_back(CUTE(getSingleAttributeTest));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}


int main() {

	createClass();
	createObject();

	runSuite();

	deleteObject();
	deleteClass();


	return 0;
}
