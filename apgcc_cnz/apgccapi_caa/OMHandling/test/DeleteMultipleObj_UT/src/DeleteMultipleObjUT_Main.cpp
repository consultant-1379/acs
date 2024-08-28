/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>

//#include "ACS_APGCC_OiHandler.h"
//#include "MyClassImplementer.h"
//#include "MyImplementer_2.h"


//#include "AdminOperation_MyImplementer.h"
//#include "MyClassImplementer.h"
//#include "acs_apgcc_adminoperation.h"
#include "acs_apgcc_omhandler.h"

//#include "ACS_CC_Types.h"



#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"
#include <sys/poll.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "acs_apgcc_oihandler_V2.h"

OmHandler omHandler_NO_OI;


/***************crea classe*/
int createClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to create a new class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("ATTRIBUTE_1"), ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("ATTRIBUTE_4"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("ATTRIBUTE_7"), ATTR_NAMET, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("ATTRIBUTE_8"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTR_TIME"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnI64);
	classAttributes.push_back(attributeTIME);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeSTRING);

	result = omHandler.defineClass( const_cast<char *>(p_className),categoryClassToBeDefined,classAttributes);
	ASSERTM("OmHandler::defineClass ", result == ACS_CC_SUCCESS);
	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR!!! Class " << p_className << " NOT created" << endl;
		cout << endl;
		return -1;

	}

}
/***************end crea classe*/

/***************crea oggetto*/
int createObjectTest(const char* p_className, const char *p_objName){


	cout << " -> Try to create a new object " << p_objName << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	//char nameClass[20] = "Test_config";
	char* nomeParent = const_cast<char *>("safApp=safImmService");

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>(p_objName);
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 222.9;
	attr2.attrValues = new void*[1];
	attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

	/*Fill the ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	attr3.attrName = name_attUint32;
	attr3.attrType = ATTR_UINT32T;
	attr3.attrValuesNum = 1;
	unsigned int intValueU32 = 123;
	attr3.attrValues = new void*[1];
	attr3.attrValues[0] = reinterpret_cast<void*>(&intValueU32);


	char name_attrSaString[]= "ATTRIBUTE_8";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* stringValue = const_cast<char*>("pippo");
	attr4.attrValues = new void*[1];
	attr4.attrValues[0] =	reinterpret_cast<void*>(stringValue);
/**/
	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);

	result = omHandler_NO_OI.createObject(p_className, nomeParent, AttrList );
	//ASSERTM("OmHandler:createObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << "," << nomeParent << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created" << endl;
		cout << endl;
		return -1;
	}

}
/***************end crea oggetto*/

/***************crea oggetto*/
int createChildObjectTest(const char* p_className, const char *p_objName, char *p_parentName){


	cout << " -> Try to create a new object " << p_objName << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	//char nameClass[20] = "Test_config";
	//char* nomeParent = const_cast<char *>("safApp=safImmService");

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>(p_objName);
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 222.9;
	attr2.attrValues = new void*[1];
	attr2.attrValues[0] = reinterpret_cast<void*>(&floatValue);

	/*Fill the ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	attr3.attrName = name_attUint32;
	attr3.attrType = ATTR_UINT32T;
	attr3.attrValuesNum = 1;
	unsigned int intValueU32 = 123;
	attr3.attrValues = new void*[1];
	attr3.attrValues[0] = reinterpret_cast<void*>(&intValueU32);


	char name_attrSaString[]= "ATTRIBUTE_8";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* stringValue = const_cast<char*>("pippo");
	attr4.attrValues = new void*[1];
	attr4.attrValues[0] =	reinterpret_cast<void*>(stringValue);
/**/
	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);

	result = omHandler_NO_OI.createObject(p_className, p_parentName, AttrList );
	//ASSERTM("OmHandler:createObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << "," << p_parentName << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR !!! Object  " << p_objName << "," << p_parentName << " NOT created" << endl;
		cout << endl;
		return -1;
	}

}
/***************end crea oggetto*/


/*
 * Prepare environment. A class is defined and an Object Obj1.
 */
void PrepareEnvironment(){
	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();
	/**create a class **/
	char nameClass[20] = "Test_config";

//	const char *objNameCreate1 = "provaConfObj1";
//	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	int res;
	res = createClassTest(omHandler, nameClass);
	ASSERTM("PrepareEnvironment:createClassTest", result == 0);

	result = omHandler.Finalize();
	ASSERTM("PrepareEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

}


void PrepareEnvironment_CreateObjects(){

	ACS_CC_ReturnType result;
	result = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	int res;

	/**create a class **/
	char nameClass[20] = "Test_config";


	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate1_1 = "provaConfObj1Child1";
	const char *dnObjName1_1 = "provaConfObj1Child1,provaConfObj1,safApp=safImmService";

	const char *objNameCreate1_1_1 = "provaConfObj1Tree1";
	const char *dnObjName1_1_1 = "provaConfObj1Tree1,provaConfObj1Child1,provaConfObj1,safApp=safImmService";

	/*2 sub tree*/
	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1 = "provaConfObj2Child1";
	const char *dnObjName2_1 = "provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1_1 = "provaConfObj2Tree1";
	const char *dnObjName2_1_1 = "provaConfObj2Tree1,provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	/*3 subtree*/
	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1 = "provaConfObj3Child1";
	const char *dnObjName3_1 = "provaConfObj3Child1,provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1_1 = "provaConfObj3Tree1";
	const char *dnObjName3_1_1 = "provaConfObj3Tree1,provaConfObj3Child1,provaConfObj2,safApp=safImmService";

	res = createObjectTest(nameClass, objNameCreate1);
	ASSERTM("PrepareEnvironment_CreateObjects: createObjectTest objNameCreate1 ::FAILURE ", res == 0);

	res = createObjectTest(nameClass, objNameCreate2);
	ASSERTM("PrepareEnvironment_CreateObjects: createObjectTest objNameCreate2 ::FAILURE ", res == 0);

	res = createObjectTest(nameClass, objNameCreate3);
	ASSERTM("PrepareEnvironment_CreateObjects: createObjectTest objNameCreate3 ::FAILURE ", res == 0);

	/*create tree 1*/
	res = createChildObjectTest(nameClass, objNameCreate1_1, const_cast<char *>(dnObjName1) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate1_1  ::FAILURE ", res == 0);

	res = createChildObjectTest(nameClass, objNameCreate1_1_1, const_cast<char *>(dnObjName1_1) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate1_1_1  ::FAILURE ", res == 0);

	/*create tree 2*/
	res = createChildObjectTest(nameClass, objNameCreate2_1, const_cast<char *>(dnObjName2) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate2_1  ::FAILURE ", res == 0);

	res = createChildObjectTest(nameClass, objNameCreate2_1_1, const_cast<char *>(dnObjName2_1) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate2_1_1  ::FAILURE ", res == 0);

	/*create tree 3*/
	res = createChildObjectTest(nameClass, objNameCreate3_1, const_cast<char *>(dnObjName3) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate3_1  ::FAILURE ", res == 0);

	res = createChildObjectTest(nameClass, objNameCreate3_1_1, const_cast<char *>(dnObjName3_1) );
	ASSERTM("PrepareEnvironment_CreateObjects: createChildObjectTest objNameCreate3_1_1  ::FAILURE ", res == 0);

	result = omHandler_NO_OI.Finalize();

}

/*
 * Clear environment. the  class is deleted.
 */
void ClearEnvironment(){
	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();
	/**create a class **/
	char nameClass[20] = "Test_config";

//	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";
//	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";
//
//
//	result = omHandler.deleteObject(dnObjName1);
//	ASSERTM("ClearEnvironment:omHandler.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS);
//
//	result = omHandler.deleteObject(dnObjName2);
//	ASSERTM("ClearEnvironment:omHandler.deleteObject dnObjName2 ", result == ACS_CC_SUCCESS);

	result = omHandler.removeClass(nameClass);
	ASSERTM("ClearEnvironment:removeClass ", result == ACS_CC_SUCCESS  );

	result = omHandler.Finalize();
	ASSERTM("ClearEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

	ASSERTM("ClearEnvironment()", result == ACS_CC_SUCCESS);

}
/*Test_1*/
/*
 */

void Test_1(){

	ACS_CC_ReturnType returnCode;
	ACS_CC_ReturnType result;

	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	/*2 sub tree*/
	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1 = "provaConfObj2Child1";
	const char *dnObjName2_1 = "provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1_1 = "provaConfObj2Tree1";
	const char *dnObjName2_1_1 = "provaConfObj2Tree1,provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	/*3 subtree*/
	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1 = "provaConfObj3Child1";
	const char *dnObjName3_1 = "provaConfObj3Child1,provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1_1 = "provaConfObj3Tree1";
	const char *dnObjName3_1_1 = "provaConfObj3Tree1,provaConfObj3Child1,provaConfObj2,safApp=safImmService";



	/**/
	int res;
	ACS_CC_ReturnType resultgeterror;
	int intVal;
	std::string strinVal;

	returnCode = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_1: omHandler_NO_OI.Init ::FAILURE ", returnCode == ACS_CC_SUCCESS);
/*try delete */

	/*failure expected because of child present*/
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;
	result = omHandler_NO_OI.deleteObject(dnObjName1, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName1  ACS_APGCC_ONE", result == ACS_CC_FAILURE);

	/*failure expected because of child present*/
	scope = ACS_APGCC_SUBLEVEL;
	result = omHandler_NO_OI.deleteObject(dnObjName1, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName1 ACS_APGCC_LEVEL ", result == ACS_CC_FAILURE);

	scope = ACS_APGCC_SUBTREE;
	result = omHandler_NO_OI.deleteObject(dnObjName1, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName1 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);

	scope = ACS_APGCC_SUBLEVEL;
	result = omHandler_NO_OI.deleteObject(dnObjName2_1, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName2_1 ACS_APGCC_LEVEL ", result == ACS_CC_SUCCESS);

	scope = ACS_APGCC_SUBTREE;
	result = omHandler_NO_OI.deleteObject(dnObjName3, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName3 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);

	result = omHandler_NO_OI.deleteObject(dnObjName2, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName2 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);

	result = omHandler_NO_OI.deleteObject(dnObjName1, scope);
	ASSERTM("Test_1:omHandler.deleteObject dnObjName3 ACS_APGCC_TREE ", result == ACS_CC_FAILURE);



	omHandler_NO_OI.Finalize();


	cout<<"--------------------------------/////---------------------------------"<<endl;

	sleep (6);

}
/*end test 1*/

/*Test_2*/
/*
 *
 */

void Test_2(){

	ACS_CC_ReturnType returnCode;
	ACS_CC_ReturnType result;

	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	/*2 sub tree*/
	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1 = "provaConfObj2Child1";
	const char *dnObjName2_1 = "provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	const char *objNameCreate2_1_1 = "provaConfObj2Tree1";
	const char *dnObjName2_1_1 = "provaConfObj2Tree1,provaConfObj2Child1,provaConfObj2,safApp=safImmService";

	/*3 subtree*/
	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1 = "provaConfObj3Child1";
	const char *dnObjName3_1 = "provaConfObj3Child1,provaConfObj3,safApp=safImmService";

	const char *objNameCreate3_1_1 = "provaConfObj3Tree1";
	const char *dnObjName3_1_1 = "provaConfObj3Tree1,provaConfObj3Child1,provaConfObj2,safApp=safImmService";



	/**/
	int res;
	ACS_CC_ReturnType resultgeterror;
	int intVal;
	std::string strinVal;

	returnCode = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_2: omHandler_NO_OI.Init ::FAILURE ", returnCode == ACS_CC_SUCCESS);
/*try delete */

//	/*failure expected because of child present*/
	ACS_APGCC_ScopeT scope = ACS_APGCC_SUBLEVEL;

	result = omHandler_NO_OI.deleteObject(dnObjName1,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName1  ACS_APGCC_ONE", result == ACS_CC_FAILURE);

	/*failure expected because of child present*/
	scope = ACS_APGCC_SUBLEVEL;
	result = omHandler_NO_OI.deleteObject(dnObjName1,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName1 ACS_APGCC_LEVEL ", result == ACS_CC_FAILURE);

	result = omHandler_NO_OI.resetRequest("transaction1");
	ASSERTM("Test_2:omHandler.deleteObject resetRequest ", result == ACS_CC_SUCCESS);

	scope = ACS_APGCC_SUBTREE;
	result = omHandler_NO_OI.deleteObject(dnObjName1,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName1 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);

	scope = ACS_APGCC_SUBLEVEL;
	result = omHandler_NO_OI.deleteObject(dnObjName2_1,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName2_1 ACS_APGCC_LEVEL ", result == ACS_CC_SUCCESS);

	result = omHandler_NO_OI.applyRequest("transaction1");
	ASSERTM("Test_2:omHandler.deleteObject first applyRequest ", result == ACS_CC_SUCCESS);

	scope = ACS_APGCC_SUBTREE;
	result = omHandler_NO_OI.deleteObject(dnObjName3,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName3 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);

	result = omHandler_NO_OI.deleteObject(dnObjName2,"transaction1", scope);
	ASSERTM("Test_2:omHandler.deleteObject dnObjName2 ACS_APGCC_TREE ", result == ACS_CC_SUCCESS);


	result = omHandler_NO_OI.applyRequest("transaction1");
	ASSERTM("Test_2:omHandler.deleteObject second applyRequest ", result == ACS_CC_SUCCESS);



	omHandler_NO_OI.Finalize();


	sleep (5);

}
/*end test 2*/


/*SUITE*/
void runSuite(){

	cute::suite s;

	/*define class*/
	s.push_back(CUTE(PrepareEnvironment));
	/*Obj crreation*/
	s.push_back(CUTE(PrepareEnvironment_CreateObjects));
	sleep (6);

	s.push_back(CUTE(Test_1));

	sleep (6);
	/*Obj crreation*/
	s.push_back(CUTE(PrepareEnvironment_CreateObjects));
	sleep (6);
	s.push_back(CUTE(Test_2));

	sleep(1);
	s.push_back(CUTE(ClearEnvironment));

	sleep(1);

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "Delete Multiple OBJ CUTE Test!!!" << endl;

	runSuite();

	cout << "!!!Delete Multiple OBJ  Test!! END!!!" << endl;

	return 0;
}
