/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>

#include "ACS_APGCC_OiHandler.h"
#include "MyClassImplementer.h"
//#include "MyImplementer_2.h"
#include "acs_apgcc_omhandler.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"
#include <sys/poll.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

OmHandler omHandler_NO_OI;
/*thread*/
class MyThread : public ACE_Task_Base {

public:
	MyThread(MyClassImplementer *p_implementer, ACE_Reactor *p_reactor){

		implementer = p_implementer;
		reactor = p_reactor;
	}

	~MyThread(){};

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}


private:
	MyClassImplementer *implementer;
	ACE_Reactor *reactor;
};
/*end define thread*/

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
int createObjectTest(/*OmHandler omHandler, */const char* p_className, const char *p_objName){


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

/**************modify object*/
int modifyObjectTest1(/*OmHandler omHandler, */const char *p_objName){
	std::cout << "Starting modifyObjectTest1:::: "<< endl;
	ACS_CC_ReturnType result;
	float valueFlo=1.5;
	char name_attFloat[]="ATTRIBUTE_1";
	ACS_CC_ImmParameter par;
	par.attrName=name_attFloat;
	par.attrType=ATTR_FLOATT;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);


	/*Set a new value for ATT_STRING*/
	char str1[30]="stringTest_555";
	ACS_CC_ImmParameter par2;
	par2.attrName=const_cast<char*>("ATTRIBUTE_8");
	par2.attrType=ATTR_STRINGT;
	par2.attrValuesNum=1;
	par2.attrValues=new void*[par2.attrValuesNum];
	par2.attrValues[0] =reinterpret_cast<void*>(str1);


	/*Set a new value for ATT_NAME*/
	char str3[30]="SaNameString_555";
	ACS_CC_ImmParameter par3;
	par3.attrName=const_cast<char*>("ATTRIBUTE_7");
	par3.attrType=ATTR_NAMET;
	par3.attrValuesNum=1;
	par3.attrValues=new void*[par3.attrValuesNum];
	par3.attrValues[0] =reinterpret_cast<void*>(str3);



	if ((result = omHandler_NO_OI.modifyAttribute(p_objName, &par) )!=ACS_CC_SUCCESS){
		cout << " ERROR modify par1" << endl;
		cout << endl;
		return -1;
	}

	return 0;


}
/**************end modify object*/

/**************2 modify object*/
int modifyObjectTest2(/*OmHandler omHandler, */const char *p_objName){
	std::cout << "Starting modifyObjectTest2:::: "<< endl;
	ACS_CC_ReturnType result;
	float valueFlo=200.1;
	char name_attFloat[]="ATTRIBUTE_1";
	ACS_CC_ImmParameter par;
	par.attrName=name_attFloat;
	par.attrType=ATTR_FLOATT;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);


	if ((result = omHandler_NO_OI.modifyAttribute(p_objName, &par) )!=ACS_CC_SUCCESS){
		cout << " ERROR modify par1" << endl;
		cout << endl;
		return -1;
	}

	return 0;


}

/*
 * Prepare environment. A class is defined and an Object Obj1.
 */
void PrepareEnvironment(){
	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();
	/**create a class **/
	char nameClass[20] = "Test_config";

	int res;
	res = createClassTest(omHandler, nameClass);
	ASSERTM("PrepareEnvironment:createClassTest", result == 0);

	result = omHandler.Finalize();
	ASSERTM("PrepareEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

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
	result = omHandler.removeClass(nameClass);

	result = omHandler.Finalize();
	ASSERTM("ClearEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

	ASSERTM("ClearEnvironment()", result == ACS_CC_SUCCESS);

}
/*Test_1*/
/*
 * Test_2 performs the following actions:
 * 1.0) Preparation define and starts a class implementer for the class "Test_config"
 * 1.1) initialize omHandler_NO_OI
 * 1.2) try to create objNameCreate1 instance of the previously defined class "Test_config":
 * 		expected result Success .
 * 1.3) try to create objNameCreate2:
 * 		expected result Failure since an OI reject creation.
 * 1.4) try to create objNameCreate3:
 * 		expected result Success .
 * 1.5) try to modify the object created in 1.2::
 * 		expected result Succes: the new value is < 100.
 * 1.6) try to modify the object created in 1.2::
 * 		expected result Failure the new value is < 100.
 * 1.7) try to delete objNameCreate2
 *	    expected result Failure the obj does not exists .
 * 1.8) try to delete objNameCreate1
 * 		expected result Success .
 * 1.9) try to delete objNameCreate3
 * 		expected result Success
 * 1.11)try to create objNameCreate2:
 * 		expected success
 * 1.12)try to del objNameCreate2:
 * 		expected success
 * 1.10)Finalize class implementer
 *
 *
 */

void Test_1(){

	ACS_APGCC_OiHandler oiHandler;
	char nameClass[20] = "Test_config";

	ACS_CC_ReturnType result;

	int res;

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate4 = "provaConfObj4";
	const char *dnObjName4 = "provaConfObj4,safApp=safImmService";



	/*1.0 Starts the Class Implementer*/
	MyClassImplementer implementer("ImplementerDefined");

	/**Initialize IMM and set MyImplementer in the map**/
	result = oiHandler.addClassImpl(&implementer, nameClass);

	ASSERTM("Test_1:oiHandler.addClassImpl", result == ACS_CC_SUCCESS );

	if ( result == ACS_CC_FAILURE ){
		std::cout << "ERROR: Set implementer  for the IMM class '"<< nameClass <<"' failure " << std::endl;

	}

	/**Instance a Reactor to handle the events**/
	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer, reactor);

	thread_1.activate();

	/*Now the class implementer is running*/

	/*1.1*/
	result = omHandler_NO_OI.Init(REGISTERED_OI);

	ASSERTM("1.1 Test_1:omHandler_NO_OI.Init", result == ACS_CC_SUCCESS );

	/**1.2 create OBJ1   **/
	res = createObjectTest( nameClass, objNameCreate1);

	ASSERTM("1.2 Test_1:createObjectTest objNameCreate1 ", res == 0 );

	/**1.3 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate2);

	ASSERTM("1.3 Test_1:createObjectTest objNameCreate2", res != 0 );

	/**1.4 create OBJ3 **/

	res = createObjectTest(nameClass, objNameCreate3);

	ASSERTM("1.4 Test_1:createObjectTest objNameCreate3", res == 0 );


	/**1.5 modify OBJ1 **/

	res = modifyObjectTest1( dnObjName1);

	ASSERTM("1.5 Test_1:modifyObjectTest1 objNameCreate1 ", res == 0 );

	/**1.6 modify OBJ1 **/

	res = modifyObjectTest2( dnObjName1);

	ASSERTM("1.6 Test_1:modifyObjectTest1 objNameCreate1 ", res != 0 );

	/**1.7 del OBJ1 **/

	result = omHandler_NO_OI.deleteObject(dnObjName1);

	ASSERTM("1.7 Test_1:omHandler_NO_OI.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS  );

	/**1.8 del OBJ2 **/

	result = omHandler_NO_OI.deleteObject(dnObjName2);

	ASSERTM("1.8 Test_1:omHandler_NO_OI.deleteObject dnObjName2 ", result == ACS_CC_FAILURE);

	/**1.9 del OBJ3 **/

	result = omHandler_NO_OI.deleteObject(dnObjName3);

	ASSERTM("1.9 Test_1:omHandler_NO_OI.deleteObject dnObjName1", result == ACS_CC_SUCCESS  );



	/**1.11 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate4);

	ASSERTM("1.11 Test_1:createObjectTest objNameCreate4", res == 0 );

	/**1.12 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate4);

	ASSERTM("1.12 Test_1:createObjectTest objNameCreate4", res != 0 );

	/**1.13 del OBJ2 **/

	result = omHandler_NO_OI.deleteObject(dnObjName4);

	ASSERTM("1.13 Test_1:omHandler_NO_OI.deleteObject dnObjName2 ", result == ACS_CC_SUCCESS);

	/*1.10 remove classImplementer*/
	result = oiHandler.removeClassImpl(&implementer, nameClass);


	result = omHandler_NO_OI.Finalize();
	ASSERTM("omHandler_NO_OI.finalize() ", result == ACS_CC_SUCCESS);
}
/*end test 1*/

/*Test_2*/
/*
 * Test_2 performs the following actions:
 * 1.0) Preparation define and starts a class implementer for the class "Test_config"
 * 1.1) initialize omHandler_NO_OI
 * 1.2) try to create objNameCreate1 instance of the previously defined class "Test_config":
 * 		expected result Success .
 * 1.3) try to create objNameCreate2:
 * 		expected result Failure since an OI reject creation.
 * 1.3.1) try to re create objNameCreate1:
 * 		expected result failure
 * 1.4) try to create objNameCreate3:
 * 		expected result Success .
 * 1.4.1) try to create objNameCreate3 as instance of not existing class:
 * 		expected result failure .
 * 1.10) remove classImplementer
 * 1.5) try to modify the object created in 1.2::
 * 		expected result Succes:
 * 1.6) try to modify the object created in 1.2::
 * 		expected result Succes.
 * 1.7) try to delete objNameCreate1
 *	    expected result Succes.
 * 1.8) try to delete objNameCreate2
 * 		expected result Failure the obj does not exists .
 * 1.9) try to delete objNameCreate3
 * 		expected result Success
 *
 * 1.11)try to create objNameCreate4:
 * 		expected success
 * 1.12)try to  recreate objNameCreate4:
 * 		expected failure
 *1.13)try delete objNameCreate3:
 * 		expected failure
 *1.14)try delete objNameCreate4:
 * 		expected success
 */

void Test_2(){


	char nameClass[20] = "Test_config";
	char nameClassNotExisting[30] = "Test_configNotExisting";

	ACS_CC_ReturnType result;

	int res;

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate4 = "provaConfObj4";
	const char *dnObjName4 = "provaConfObj4,safApp=safImmService";


	ACS_APGCC_OiHandler oiHandler;

	/*1.0 Starts the Class Implementer*/
	MyClassImplementer implementer("ImplementerDefined");

	/**Initialize IMM and set MyImplementer in the map**/
	result = oiHandler.addClassImpl(&implementer, nameClass);

	ASSERTM("Test_1:oiHandler.addClassImpl", result == ACS_CC_SUCCESS );

	if ( result == ACS_CC_FAILURE ){
		std::cout << "ERROR: Set implementer  for the IMM class '"<< nameClass <<"' failure " << std::endl;

	}

	/**Instance a Reactor to handle the events**/
	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer, reactor);

	thread_1.activate();

	/*Now the class implementer is running*/


	/*1.1*/
	result = omHandler_NO_OI.Init();

	ASSERTM("1.1 Test_2:omHandler_NO_OI.Init", result == ACS_CC_SUCCESS );

	/**1.2 create OBJ1   **/
	res = createObjectTest( nameClass, objNameCreate1);

	ASSERTM("1.2 Test_2:createObjectTest objNameCreate1 ", res == 0 );

	/**1.3 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate2);

	ASSERTM("1.3 Test_2:createObjectTest objNameCreate2", res != 0 );

	/**1.3.1 re create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate1);

	ASSERTM("1.3 Test_2:createObjectTest objNameCreate1", res != 0 );


	/**1.4 create OBJ3 **/

	res = createObjectTest(nameClass, objNameCreate3);

	ASSERTM("1.4 Test_2:createObjectTest objNameCreate3", res == 0 );

	/**1.4.1 create OBJ3 **/

	res = createObjectTest(nameClassNotExisting, objNameCreate3);

	ASSERTM("1.4 Test_2:createObjectTest nameClassNotExisting objNameCreate3", res != 0 );

	/*1.10 remove classImplementer*/
	result = oiHandler.removeClassImpl(&implementer, nameClass);

	/**1.5 modify OBJ1 **/

	res = modifyObjectTest1( dnObjName1);

	ASSERTM("1.5 Test_2:modifyObjectTest1 objNameCreate1 ", res == 0 );

	/**1.6 modify OBJ1 **/

	res = modifyObjectTest2( dnObjName1);

	ASSERTM("1.6 Test_2:modifyObjectTest1 objNameCreate1 ", res == 0 );

	/**1.7 del OBJ1 **/

	result = omHandler_NO_OI.deleteObject(dnObjName1);

	ASSERTM("1.7 Test_2:omHandler_NO_OI.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS  );

	/**1.8 del OBJ2 **/

	result = omHandler_NO_OI.deleteObject(dnObjName2);

	ASSERTM("1.8 Test_1:omHandler_NO_OI.deleteObject dnObjName2 ", result == ACS_CC_FAILURE);

	/**1.9 del OBJ3 **/

	result = omHandler_NO_OI.deleteObject(dnObjName3);

	ASSERTM("1.9 Test_1:omHandler_NO_OI.deleteObject dnObjName1", result == ACS_CC_SUCCESS  );



	/**1.11 create OBJ4 **/

	res = createObjectTest(nameClass, objNameCreate4);

	ASSERTM("1.11 Test_2:createObjectTest objNameCreate4", res == 0 );

	/**1.12 re create OBJ4 **/

	res = createObjectTest(nameClass, objNameCreate4);

	ASSERTM("1.12 Test_2:createObjectTest objNameCreate4", res != 0 );

	/**1.13 re del OBJ3 **/

	result = omHandler_NO_OI.deleteObject(dnObjName3);

	ASSERTM("1.13 Test_2:omHandler_NO_OI.deleteObject dnObjName3 ", result == ACS_CC_FAILURE);

	/**1.14  del OBJ4 **/

	result = omHandler_NO_OI.deleteObject(dnObjName4);

	ASSERTM("1.13 Test_2:omHandler_NO_OI.deleteObject dnObjName4 ", result == ACS_CC_SUCCESS);

}
/*end test 2*/


/*SUITE*/
void runSuite(){

	cute::suite s;

	/*define class*/
	s.push_back(CUTE(PrepareEnvironment));
	/*Obj crreation*/
	s.push_back(CUTE(Test_1));

	s.push_back(CUTE(Test_2));

	s.push_back(CUTE(ClearEnvironment));

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "New CCB Management CUTE Test!!!" << endl;



	runSuite();

	cout << "!!!New CCB Management CUTE Test!! END!!!" << endl;

	return 0;
}
