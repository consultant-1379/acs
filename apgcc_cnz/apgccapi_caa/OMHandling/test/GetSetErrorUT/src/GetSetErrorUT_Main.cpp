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


#include "AdminOperation_MyImplementer.h"
#include "MyClassImplementer.h"
#include "acs_apgcc_adminoperation.h"
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
/*thread*/
class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(AdminOperation_MyImplementer *p_user1){

		cout<<"Costruttore Thread"<<endl;

		implementer = p_user1;

		isStop = false;
	}


	/**Destructor**/
	~MyThread(){
		cout<<"Distruttore Thread"<<endl;
	}


	/**This method is used to stop the thread**/
	void stop(){

		cout<<"Method Stop invocated"<<endl;

		isStop=true;
	}


	/**svc method **/
	int svc(void){


		cout<<"Thread AVVIATO"<<endl;

		int ret;

		cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

		struct pollfd fds[1];

		fds[0].fd = implementer->getSelObj();
		fds[0].events = POLLIN;


		while(!isStop)
		{
			ret = poll(fds, 1, 1);
			if (ret == 0)
			{
				//cout<<"timeout off"<<endl;

			}else if (ret == -1){
				printf("poll error: \n");
			}else{

				cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		cout<<"Thread TERMINATO"<<endl;

		return 0;
	}

private:

	AdminOperation_MyImplementer *implementer;

	bool isStop;

};

/*end define thread*/

/***/
class MyThread_Reactor : public ACE_Task_Base {

public:
	MyThread_Reactor(MyClassImplementer *p_implementer, ACE_Reactor *p_reactor){

		implementer = p_implementer;
		reactor = p_reactor;
	}

	~MyThread_Reactor(){};

	int svc(void){

		implementer->dispatch(reactor, ACS_APGCC_DISPATCH_ALL);

		reactor->run_reactor_event_loop();

		return 0;
	}


private:
	MyClassImplementer *implementer;
	ACE_Reactor *reactor;
};
/***/

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
int createObjectTest(/*OmHandler omHandler,*/const char* p_className, const char *p_objName){


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

/**************1 modify object*/
int modifyObjectTest1(const char *p_objName){
	std::cout << "Starting modifyObjectTest:::: "<< endl;
	ACS_CC_ReturnType result;
	float valueFlo=1.5;
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

/**************2 modify object*/
int modifyObjectTest2(const char *p_objName){
	std::cout << "Starting modifyObjectTest:::: "<< endl;
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
/**************end modify object*/

/***************crea oggetto trans*/
int createObjectTestTrans(std::string transName,const char* p_className, const char *p_objName){


	cout << " -> TRANSACTINAL:Try to create a new object in  " << p_objName << endl;
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
	float floatValue = 333.9;
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
	char* stringValue = const_cast<char*>("transaction");
	attr4.attrValues = new void*[1];
	attr4.attrValues[0] =	reinterpret_cast<void*>(stringValue);
/**/
	AttrList.push_back(attrRdn);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);

	result = omHandler_NO_OI.createObject(p_className, nomeParent, AttrList, transName );


	if( result == ACS_CC_SUCCESS ){
		cout << " ->TRANSACTIONAL: Object  " << p_objName << "," << nomeParent << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " ->TRANSACTIONAL: ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created errCode= " << omHandler_NO_OI.getInternalLastError()<< endl;
		cout << endl;
		return -1;
	}

}
/***************end crea oggetto trans*/

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
 * Test_1 (Object Implementer) performs the following actions:
 *
 * 1.1) 	initialize omHandler_NO_OI
 * 1.1) 	create an obj objNameCreate1
 * 1.2) 	create an obj objNameCreate2
 * 			expected result Success .
 * 1.2.1)	fianlize omHndler
 *
 * 1.3)		init the Administrative Operation Implementer
 *
 * 1.4)		set as obj implementer and activate thread to wait for operations
 *
 * 1.5)		Prepare to send AdminOperation on created objects: obj.admOp.init
 *
 * 1.5.1)	admOp.adminOperationInvoke on objNameCreate1
 * 1.5.1)	admOp.adminOperationInvoke check result
 *
 * 1.5.2)	send another AdminOperation expected to fail because of timeout
 * 1.5.3)	admOp.adminOperationInvoke check return code = 5
 * 1.5.4)	admOp.adminOperationInvoke check return code message TIMEOUT
 *
 *
 * 1.6)		Prepare to send Admin operation on Not Implemented OBJ objNameCreate2
 * 1.6)   	admOp.adminOperationInvoke Failure
 * 1.6)		admOp.adminOperationInvoke check result NOT_EXIST
 *
 * 1.7)		finalize admOp
 *
 * 1.8)		stop thread and release the object implemented:obj.releaseObjectImplemented
 *
 */

void Test_1(){

	ACS_CC_ReturnType returnCode;

	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	string implName = "NomeImplementer";
	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";
	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";


	AdminOperation_MyImplementer obj;
	obj.setObjName(objImplemented);
	obj.setImpName(implName);
	obj.setScope(scope);

	acs_apgcc_oihandler_V2 oiHandler;

	/*Add class Implementer*/

	returnCode = oiHandler.addClassImpl(&obj, classImplemented.c_str() );
	ASSERTM("Test_1: oiHandler.addClassImpl::FAILURE ", returnCode == ACS_CC_SUCCESS);

	/*start thread*/
	MyThread th(&obj);
	th.activate();


	/**/
	int res;
	ACS_CC_ReturnType resultgeterror;
	int intVal;
	std::string strinVal;

	returnCode = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_1: omHandler_NO_OI.Init ::FAILURE ", returnCode == ACS_CC_SUCCESS);
/*CREATE OBJ1 expected success*/

	res = createObjectTest(nameClass, objNameCreate1);
	ASSERTM("Test_1: createObjectTest objNameCreate1 ::FAILURE ", res == 0);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);

	/*expected SUCCES string null and 0*/
	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 0);
	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "");

/*CREATE OBJ2 expected failure: 101, CreateFailure*/
	res = createObjectTest(nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTest objNameCreate2 ::SUCCESS ", res == -1);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);


/*expected FAILURE string CreateFailure  and 101*/
	cout << "intVal =" <<intVal<< endl;
	cout << "strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 101);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "CreateFailure");

/*CREATE OBJ3 expected success*/
	res = createObjectTest(nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTest objNameCreate3 ::FAILURE ", res == 0);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);

	/*expected SUCCES string null and 0*/
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 0);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "");res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans);

/*Modify Obj1 with value < 100 expected success*/
	res = modifyObjectTest1( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest1 dnObjName1 ::FAILURE ", res == 0);

/*Modify Obj1 with value > 100 expected failure*/
	res = modifyObjectTest2( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest2 ::SUCCESS ", res == -1);

	/*expected FAILURE string 102, "ModifyFailure"*/
	cout << "102 intVal =" <<intVal<< endl;
	cout << "102 strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 102);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "ModifyFailure");




	/*delete created objects*/
	//returnCode = omHandler_NO_OI.deleteObject(dnObjNameNot);
//	ASSERTM("Test_1: omHandler_NO_OI.deleteObject not existing ", returnCode == ACS_CC_FAILURE);
//
//	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//	cout << "intVal =" <<intVal<< endl;
//	cout << "strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_FAILURE);

	returnCode = omHandler_NO_OI.deleteObject(dnObjName1);
	returnCode = omHandler_NO_OI.deleteObject(dnObjName2);
	returnCode = omHandler_NO_OI.deleteObject(dnObjName3);

	const char *objNameCreateTrans = "objNameCreateTransactional";
	//const char *dnObjNameCreateTrans = "objNameCreateTransactional,safApp=safImmService";

	const char *objNameCreateTrans2 = "objNameCreateTransactional2";
	//const char *dnObjNameCreateTrans2 = "objNameCreateTransactional2,safApp=safImmService";
	cout<<"----TRANSACTIONAL-------------"<<endl;

	res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTransactional ::FAILURE ", res == 0);

	/*create obj 2 expected failure on create*/
	res = createObjectTestTrans("transaction1", nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTransactional2  ", res == -1);

	returnCode = omHandler_NO_OI.getExitCode("transaction1",intVal, strinVal);
	ASSERTM("Test_1: omHandler_NO_OI.getExitCode transaction1::FAILURE ", returnCode == ACS_CC_SUCCESS);

	/*apply request calling ....*/
	returnCode = omHandler_NO_OI.resetRequest("transaction1");
	ASSERTM("Test_1: omHandler_NO_OI.resetRequest transaction1::FAILURE ", returnCode == ACS_CC_SUCCESS);

	/*new transaction to be failed after apply */
	res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans2);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTrans2 ::FAILURE ", res ==0);

	res = createObjectTestTrans("transaction1", nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTestTrans objNameCreate3 ::FAILURE ", res ==0);

	returnCode = omHandler_NO_OI.applyRequest("transaction1");
	ASSERTM("Test_1: omHandler_NO_OI.applyRequest transaction1 ", returnCode == ACS_CC_FAILURE);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	cout << "intVal =" <<intVal<< endl;
	cout << "strinVal =" <<strinVal<< endl;
	ASSERTM("Test_1: omHandler_NO_OI.getExitCode ", resultgeterror == ACS_CC_SUCCESS);

	cout<<"----TRANSACTIONAL END-------------"<<endl;
	/*stop thread*/


	th.stop();


	sleep(2);
	returnCode = oiHandler.removeClassImpl(&obj, classImplemented.c_str());
	ASSERTM("Test_1: removeClassImpl  ::SUCCESS ", returnCode == ACS_CC_SUCCESS);

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

	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	string implName = "NomeImplementer";
	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";
	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";



	MyClassImplementer obj2(objImplemented, implName, ACS_APGCC_ONE );
	acs_apgcc_oihandler_V2 oiHandler;

	returnCode = oiHandler.addClassImpl(&obj2, classImplemented.c_str());
	ASSERTM("Test_2: oiHandler.addClassImpl::FAILURE ", returnCode == ACS_CC_SUCCESS);

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;
	reactor->open(1);
	MyThread_Reactor thread_1(&obj2, reactor);
	thread_1.activate();
	/*start*/
	/**/
	int res;
	ACS_CC_ReturnType resultgeterror;
	int intVal;
	std::string strinVal;

	returnCode = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_1: omHandler_NO_OI.Init ::FAILURE ", returnCode == ACS_CC_SUCCESS);
/*CREATE OBJ1 expected success*/

	res = createObjectTest(nameClass, objNameCreate1);
	ASSERTM("Test_1: createObjectTest objNameCreate1 ::FAILURE ", res == 0);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);

	/*expected SUCCES string null and 0*/
	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 0);
	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "");

/*CREATE OBJ2 expected failure: 101, CreateFailure*/
	res = createObjectTest(nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTest objNameCreate2 ::SUCCESS ", res == -1);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);


/*expected FAILURE string CreateFailure  and 101*/
	cout << "intVal =" <<intVal<< endl;
	cout << "strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 101);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "CreateFailure");

/*CREATE OBJ3 expected success*/
	res = createObjectTest(nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTest objNameCreate3 ::FAILURE ", res == 0);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_SUCCESS);

	/*expected SUCCES string null and 0*/
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 0);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "");res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans);

/*Modify Obj1 with value < 100 expected success*/
	res = modifyObjectTest1( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest1 dnObjName1 ::FAILURE ", res == 0);

/*Modify Obj1 with value > 100 expected failure*/
	res = modifyObjectTest2( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest2 ::SUCCESS ", res == -1);

	/*expected FAILURE string 102, "ModifyFailure"*/
	cout << "102 intVal =" <<intVal<< endl;
	cout << "102 strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: COMPARE VALUES int ", intVal == 102);
//	ASSERTM("Test_1: COMPARE VALUES string ", strinVal == "ModifyFailure");




	/*delete created objects*/
	//returnCode = omHandler_NO_OI.deleteObject(dnObjNameNot);
//	ASSERTM("Test_1: omHandler_NO_OI.deleteObject not existing ", returnCode == ACS_CC_FAILURE);
//
//	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//	cout << "intVal =" <<intVal<< endl;
//	cout << "strinVal =" <<strinVal<< endl;
//	ASSERTM("Test_1: resultgeterror::FAILURE ", resultgeterror == ACS_CC_FAILURE);

	returnCode = omHandler_NO_OI.deleteObject(dnObjName1);
	returnCode = omHandler_NO_OI.deleteObject(dnObjName2);
	returnCode = omHandler_NO_OI.deleteObject(dnObjName3);

	const char *objNameCreateTrans = "objNameCreateTransactional";
	//const char *dnObjNameCreateTrans = "objNameCreateTransactional,safApp=safImmService";

	const char *objNameCreateTrans2 = "objNameCreateTransactional2";
	//const char *dnObjNameCreateTrans2 = "objNameCreateTransactional2,safApp=safImmService";
	cout<<"----TRANSACTIONAL-------------"<<endl;

	res = createObjectTestTrans("transaction2",nameClass, objNameCreateTrans);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTransactional ::FAILURE ", res == 0);

	/*create obj 2 expected failure on create*/
	res = createObjectTestTrans("transaction2", nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTransactional2  ", res == -1);

	returnCode = omHandler_NO_OI.getExitCode("transaction2",intVal, strinVal);
	ASSERTM("Test_1: omHandler_NO_OI.getExitCode transaction1::FAILURE ", returnCode == ACS_CC_SUCCESS);

	/*apply request calling ....*/
	returnCode = omHandler_NO_OI.resetRequest("transaction2");
	ASSERTM("Test_1: omHandler_NO_OI.resetRequest transaction1::FAILURE ", returnCode == ACS_CC_SUCCESS);

	/*new transaction to be failed after apply */
	res = createObjectTestTrans("transaction2",nameClass, objNameCreateTrans2);
	ASSERTM("Test_1: createObjectTestTrans objNameCreateTrans2 ::FAILURE ", res ==0);

	res = createObjectTestTrans("transaction2", nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTestTrans objNameCreate3 ::FAILURE ", res ==0);

	returnCode = omHandler_NO_OI.applyRequest("transaction2");
	ASSERTM("Test_1: omHandler_NO_OI.applyRequest transaction1 ", returnCode == ACS_CC_FAILURE);

	resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
	cout << "intVal =" <<intVal<< endl;
	cout << "strinVal =" <<strinVal<< endl;
	ASSERTM("Test_1: omHandler_NO_OI.getExitCode ", resultgeterror == ACS_CC_SUCCESS);

	cout<<"----TRANSACTIONAL END-------------"<<endl;
	/*end  */

	obj2.reactor()->end_reactor_event_loop();

	sleep(1);
	returnCode = oiHandler.removeClassImpl(&obj2, classImplemented.c_str());
	ASSERTM("Test_2: removeClassImpl  ::SUCCESS ", returnCode == ACS_CC_SUCCESS);

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
	s.push_back(CUTE(Test_1));

	s.push_back(CUTE(Test_2));

	sleep(1);
	s.push_back(CUTE(ClearEnvironment));

	sleep(1);

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "GET SET ERROR CUTE Test!!!" << endl;

	runSuite();

	cout << "!!!GET SET ERROR CUTE Test!! END!!!" << endl;

	return 0;
}
