/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>




#include "AdminOperation_MyImplementer.h"
#include "MyClassImplementer.h"
#include "acs_apgcc_adminoperation.h"
#include "acs_apgcc_omhandler.h"





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
	char nameClass2[20] = "Test_config2";

	int res;
	res = createClassTest(omHandler, nameClass);

	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_config ok "<<endl;
		}else
		cout<<"PrepareEnvironment:createClassTest:Test_config ko "<<endl;
	ASSERTM("PrepareEnvironment:createClassTest", res == 0);


	res = createClassTest(omHandler, nameClass2);
	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_config2 ok "<<endl;
		}else
		cout<<"PrepareEnvironment:createClassTest:Test_config2 ko "<<endl;


	res = omHandler.Finalize();
	ASSERTM("PrepareEnvironment()::omHandler.Finalize()", res == ACS_CC_SUCCESS);

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
	char nameClass2[20] = "Test_config2";

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

	result = omHandler.removeClass(nameClass2);
	ASSERTM("ClearEnvironment:removeClass2 ", result == ACS_CC_SUCCESS  );

	result = omHandler.Finalize();
	ASSERTM("ClearEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

	ASSERTM("ClearEnvironment()", result == ACS_CC_SUCCESS);

}

/*
 * Clear environment. the  class is deleted.
 */
void removeResidualElements(){
	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();
	/**create a class **/
	char nameClass[20] = "Test_config";

	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";
	const char *dnObjName1Child = "FirstChild,provaConfObj1,safApp=safImmService";
	const char *dnObjName5 = "CreatedbyprovaConfObj5";

	cout << "removeResidualElements" << endl;

	result = omHandler.deleteObject(dnObjName1Child);
	ASSERTM("removeResidualElements:omHandler.deleteObject dnObjName1Child ", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject(dnObjName1);
	ASSERTM("removeResidualElements:omHandler.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject(dnObjName5);
	ASSERTM("removeResidualElements:omHandler.deleteObject dnObjName5 ", result == ACS_CC_SUCCESS);

	result = omHandler.Finalize();
	ASSERTM("removeResidualElements::omHandler.Finalize()", result == ACS_CC_SUCCESS);

	ASSERTM("removeResidualElements()", result == ACS_CC_SUCCESS);

}
/*Test_1*/
/*using polling
 *
 */

void Test_1(){

	cout << "TEST1 STARTING" << endl;
	ACS_CC_ReturnType result;
	int res;


	char nameClass[20] = "Test_config";


	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate4 = "provaConfObj4";
	const char *dnObjName4 = "provaConfObj4,safApp=safImmService";

	const char *objNameCreate5 = "provaConfObj5";
	const char *dnObjName5 = "provaConfObj5,safApp=safImmService";

	const char *objNameCreate6 = "provaConfObj6";
	const char *dnObjName6 = "provaConfObj6,safApp=safImmService";

//	const char *objNameCreate5_1 = "CreatedbyprovaConfObj5";
//	const char *dnObjName5_1 = "CreatedbyprovaConfObj5";

//	const char *objNameCreateTrans = "objNameCreateTransactional";
//	const char *dnObjNameCreateTrans = "objNameCreateTransactional,apzFunctionsId=1,managedElementId=1";
//
//	const char *objNameCreateTrans2 = "objNameCreateTransactional2";
//	const char *dnObjNameCreateTrans2 = "objNameCreateTransactional2,apzFunctionsId=1,managedElementId=1";

	result = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_1: omHandler_NO_OI.Init ::FAILURE ", result == ACS_CC_SUCCESS);

	/*Add class Implementer*/
	string implName = "NomeImplementer";

	string objImplemented = "provaConfObj1,safApp=safImmService";

	string classImplemented = "Test_config";

	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	AdminOperation_MyImplementer obj;

	//obj.setObjName(objImplemented);

	obj.setImpName(implName);

	obj.setScope(scope);

	acs_apgcc_oihandler_V2 oiHandler;

	result = oiHandler.addClassImpl(&obj, classImplemented.c_str() );
	ASSERTM("Test_1: oiHandler.addClassImpl::FAILURE ", result == ACS_CC_SUCCESS);

	/*start thread*/
	MyThread th(&obj);
	th.activate();

	/*Now creating obj1*/
	res = createObjectTest(nameClass, objNameCreate1);
	ASSERTM("Test_1: createObjectTest objNameCreate1 ::FAILURE ", res == 0);


	/*creating obj2*/
	res = createObjectTest(nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTest objNameCreate2 ::FAILURE ", res == 0);

	/*creating obj3*/
	res = createObjectTest(nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTest objNameCreate3 ::FAILURE ", res == 0);

	/*creating obj4: TR HO92807 when fixed decomment the following lines*/
//	res = createObjectTest(nameClass, objNameCreate4);
//	ASSERTM("Test_1: createObjectTest objNameCreate4 ::FAILURE ", res == 0);

	/*creating obj5*/
	res = createObjectTest(nameClass, objNameCreate5);
	ASSERTM("Test_1: createObjectTest objNameCreate5 ::FAILURE ", res == 0);

	/*creating obj6 : Expected Failure*/
	res = createObjectTest(nameClass, objNameCreate6);
	ASSERTM("Test_1: createObjectTest objNameCreate6 ::", res == -1);


	/*modify obj1 value < 100: Expected Success*/
	res = modifyObjectTest1( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest1 ::", res == 0);

	/*modify obj1 value > 100: Expected Failure because of trHO92851 this object has pending callbacks
	* once resolved the object and related child can be deleted see following lines and in removeResidualElements() commented */
	res = modifyObjectTest2( dnObjName1);
	ASSERTM("Test_1: modifyObjectTest2 ::", res == -1);

	/*delete obj1 decomment the following 2 lines when tr CMW2.1 HO92851 is fixed*/
	//result = omHandler_NO_OI.deleteObject(dnObjName1);
	//ASSERTM("Test_1: deleteObject(dnObjName1)", result == ACS_CC_SUCCESS);

	/*delete obj2*/
	result = omHandler_NO_OI.deleteObject(dnObjName2);
	ASSERTM("Test_1: deleteObject(dnObjName2) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj3*/
	result = omHandler_NO_OI.deleteObject(dnObjName3);
	ASSERTM("Test_1: deleteObject(dnObjName3) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj4: HO92807 when fixed decomment the following lines*/
//	result = omHandler_NO_OI.deleteObject(dnObjName4);
//	ASSERTM("Test_1: deleteObject(dnObjName4) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj5*/
	result = omHandler_NO_OI.deleteObject(dnObjName5);
	ASSERTM("Test_1: deleteObject(dnObjName5) :: FAILURE", result == ACS_CC_SUCCESS);

	result =omHandler_NO_OI.Finalize();
	ASSERTM("Test_1: omHandler_NO_OI.Finalize :: FAILURE", result == ACS_CC_SUCCESS);

	/*to be commented deletion of obj1 in remove residuals code when the tr trHO92851 is resolved*/
	//removeResidualElements();

	sleep(2);

	th.stop();


	sleep(2);
	result = oiHandler.removeClassImpl(&obj, classImplemented.c_str());
	ASSERTM("Test_1: removeClassImpl  ::SUCCESS ", result == ACS_CC_SUCCESS);

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
	int res;


	char nameClass[20] = "Test_config";


	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *objNameCreate3 = "provaConfObj3";
	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	const char *objNameCreate4 = "provaConfObj4";
	const char *dnObjName4 = "provaConfObj4,safApp=safImmService";

	const char *objNameCreate5 = "provaConfObj5";
	const char *dnObjName5 = "provaConfObj5,safApp=safImmService";

	const char *objNameCreate6 = "provaConfObj6";
	const char *dnObjName6 = "provaConfObj6,safApp=safImmService";

//	const char *objNameCreate5_1 = "CreatedbyprovaConfObj5";
//	const char *dnObjName5_1 = "CreatedbyprovaConfObj5";

	string implName = "NomeImplementer";

	string objImplemented = "provaConfObj1,safApp=safImmService";

	string classImplemented = "Test_config";

	//ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	result = omHandler_NO_OI.Init(NO_REGISTERED_OI);
	ASSERTM("Test_2: omHandler_NO_OI.Init ::FAILURE ", result == ACS_CC_SUCCESS);



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
	/*Now creating obj1*/
	res = createObjectTest(nameClass, objNameCreate1);
	ASSERTM("Test_2: createObjectTest objNameCreate1 ::FAILURE ", res == 0);


	/*creating obj2*/
	res = createObjectTest(nameClass, objNameCreate2);
	ASSERTM("Test_1: createObjectTest objNameCreate2 ::FAILURE ", res == 0);

	/*creating obj3*/
	res = createObjectTest(nameClass, objNameCreate3);
	ASSERTM("Test_1: createObjectTest objNameCreate3 ::FAILURE ", res == 0);

	/*creating obj4: TR HO92807 when fixed decomment the following lines*/
//	res = createObjectTest(nameClass, objNameCreate4);
//	ASSERTM("Test_1: createObjectTest objNameCreate4 ::FAILURE ", res == 0);

	/*creating obj5*/
	res = createObjectTest(nameClass, objNameCreate5);
	ASSERTM("Test_2: createObjectTest objNameCreate5 ::FAILURE ", res == 0);

	/*creating obj6 : Expected Failure*/
	res = createObjectTest(nameClass, objNameCreate6);
	ASSERTM("Test_2: createObjectTest objNameCreate6 ::", res == -1);


	/*modify obj1 value < 100: Expected Success*/
	res = modifyObjectTest1( dnObjName1);
	ASSERTM("Test_2: modifyObjectTest1 ::", res == 0);

	/*modify obj1 value > 100: Expected Failure because of trHO92851 this object has pending callbacks
	 * once resolved the object and related childcan be deleted see following lines and in removeResidualElements() commented */
	res = modifyObjectTest2( dnObjName1);
	ASSERTM("Test_2: modifyObjectTest2 ::", res == -1);

	/*delete obj1 decomment the following 2 lines when tr CMW2.1 HO92851 is fixed*/
	//result = omHandler_NO_OI.deleteObject(dnObjName1);
	//ASSERTM("Test_1: deleteObject(dnObjName1)", result == ACS_CC_SUCCESS);

	/*delete obj2*/
	result = omHandler_NO_OI.deleteObject(dnObjName2);
	ASSERTM("Test_2: deleteObject(dnObjName2) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj3*/
	result = omHandler_NO_OI.deleteObject(dnObjName3);
	ASSERTM("Test_2: deleteObject(dnObjName3) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj4: HO92807 when fixed decomment the following lines*/
//	result = omHandler_NO_OI.deleteObject(dnObjName4);
//	ASSERTM("Test_1: deleteObject(dnObjName4) :: FAILURE", result == ACS_CC_SUCCESS);

	/*delete obj5*/
	result = omHandler_NO_OI.deleteObject(dnObjName5);
	ASSERTM("Test_2: deleteObject(dnObjName5) :: FAILURE", result == ACS_CC_SUCCESS);

	result =omHandler_NO_OI.Finalize();
	ASSERTM("Test_2: omHandler_NO_OI.Finalize :: FAILURE", result == ACS_CC_SUCCESS);

	sleep(2);
/*to be commented deletion of obj1 in remove residuals code when the tr trHO92851 is resolved*/
	//removeResidualElements();

	/*end  */

	obj2.reactor()->end_reactor_event_loop();

	sleep(1);
	returnCode = oiHandler.removeClassImpl(&obj2, classImplemented.c_str());
	ASSERTM("Test_2: removeClassImpl  ::SUCCESS ", returnCode == ACS_CC_SUCCESS);





	sleep (5);

}
/*end test 2*/


/*SUITE*/
void runSuite(){

	cute::suite s;

	/*define class*/
	s.push_back(CUTE(PrepareEnvironment));

	/*using polling*/
	s.push_back(CUTE(Test_1));

	s.push_back(CUTE(removeResidualElements));

	sleep(1);

	/*using reactor*/
	s.push_back(CUTE(Test_2));

	sleep(1);
	s.push_back(CUTE(removeResidualElements));

	sleep(1);
	s.push_back(CUTE(ClearEnvironment));

	sleep(1);

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "Augmented Ccb CUTE Test!!!" << endl;

	runSuite();

	cout << "!!!Augmented Ccb CUTE Test!! END!!!" << endl;

	return 0;
}
