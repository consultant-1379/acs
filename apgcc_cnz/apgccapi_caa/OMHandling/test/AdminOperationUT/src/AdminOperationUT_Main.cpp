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

	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";


	result = omHandler.deleteObject(dnObjName1);
	ASSERTM("ClearEnvironment:omHandler.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS);

	result = omHandler.deleteObject(dnObjName2);
	ASSERTM("ClearEnvironment:omHandler.deleteObject dnObjName2 ", result == ACS_CC_SUCCESS);

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
	AdminOperation_MyImplementer obj;
	char choice;
	string implName = "NomeImplementer";
	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";
	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	/*Param for AdminOperation*/
	long long int timeOutVal_5sec = 5*(1000000000LL);
	long long int timeOutVal_10sec = 10*(1000000000LL);
	acs_apgcc_adminoperation admOp;

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;

	/*1.1*/
	returnCode = omHandler_NO_OI.Init();

	ASSERTM("1.1 Test_1:omHandler_NO_OI.Init", returnCode == ACS_CC_SUCCESS );

	/**1.1 create OBJ1   **/
	int res;
	res = createObjectTest( nameClass, objNameCreate1);

	ASSERTM("1.1 Test_1:createObje1.6Prepare to send Admin operation on Not Implemented OBJctTest objNameCreate1 ", res == 0 );

	/**1.2 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate2);

	ASSERTM("1.2 Test_1:createObjectTest objNameCreate2", res == 0 );

	/*1.2.1*/
	returnCode = omHandler_NO_OI.Finalize();

	ASSERTM("1.1 Test_1:omHandler_NO_OI.finalize", returnCode == ACS_CC_SUCCESS );

	/*1.3  init the Administrative Operation Implementer*/
	returnCode = obj.init(implName);

	ASSERTM("1.3 Test_1:obj.init ", returnCode == ACS_CC_SUCCESS );

	/*1.4 set as obj implementer */

	returnCode = obj.setObjectImplemented(objImplemented);

	ASSERTM("1.4 Test_1:obj.setObjectImplemented ", returnCode == ACS_CC_SUCCESS );

	/*Activate thread to wait for operations*/
	MyThread th(&obj);

	th.activate();

	/*1.5Prepare to send Admin operation on Implemented OBJ*/

	/*create first Elemenet of param list*/
	char attName1[]= "attName1";

	firstElem.attrName =attName1;

	firstElem.attrType=ATTR_FLOATT;

	float floatValue = 333.9;

	firstElem.attrValues=reinterpret_cast<void*>(&floatValue);

	/*create second Elemenet of param list*/
	char attName2[]= "attName2";

	secondElem.attrName =attName2;

	secondElem.attrType=ATTR_INT32T ;

	int intValue =10;

	secondElem.attrValues=reinterpret_cast<void*>(&intValue);

	/*insert into the vector*/

	vectorIN.push_back(firstElem);

	vectorIN.push_back(secondElem);


	returnCode = admOp.init();

	ASSERTM("1.5 Test_1:obj.admOp.init ", returnCode == ACS_CC_SUCCESS );


	int adminOpreturnValue;

	returnCode = admOp.adminOperationInvoke(dnObjName1, 0, 5, vectorIN, &adminOpreturnValue, timeOutVal_5sec);

	ASSERTM("1.5.1 Test_1:admOp.adminOperationInvoke ", returnCode == ACS_CC_SUCCESS );

	ASSERTM("1.5.1 Test_1:admOp.adminOperationInvoke check result ", adminOpreturnValue == 28 );

	/*send another AdminOperation expected to fail because of timeout*/
	returnCode = admOp.adminOperationInvoke(dnObjName1, 0, 100, vectorIN, &adminOpreturnValue, timeOutVal_10sec);

	ASSERTM("1.5.2 Test_1:admOp.adminOperationInvoke ", returnCode == ACS_CC_FAILURE );
	ASSERTM("1.5.3 Test_1:admOp.adminOperationInvoke check result", admOp.getInternalLastError() == -5 );
	ASSERTM("1.5.4 Test_1:admOp.adminOperationInvoke check result", strcmp (admOp.getInternalLastErrorText(), "ERR_IMM_TIMEOUT") == 0 );

	sleep(2);
	/*1.6Prepare to send Admin operation on Not Implemented OBJ*/

	returnCode = admOp.adminOperationInvoke(dnObjName2, 0, 5, vectorIN, &adminOpreturnValue, timeOutVal_5sec);

	ASSERTM("1.6 Test_1:admOp.adminOperationInvoke ", returnCode == ACS_CC_FAILURE );
	ASSERTM("1.6 Test_1:admOp.adminOperationInvoke check result", strcmp(admOp.getInternalLastErrorText(),"ERR_ELEMENT_NOT_EXIST") == 0);


	/*1.7 finalize*/
	returnCode = admOp.finalize();
	ASSERTM("1.7 Test_1:admOp.finalize() ", returnCode == ACS_CC_SUCCESS );


	/*1.8 release the object imple1.8 release the object implementedmented*/
	sleep (2);
	th.stop();

	returnCode = obj.releaseObjectImplemented(objImplemented);
	ASSERTM("1.8 Test_1:obj.releaseObjectImplemented ", returnCode == ACS_CC_SUCCESS );


	cout<<"--------------------------------/////---------------------------------"<<endl;

	sleep (5);

}
/*end test 1*/

/*Test_2*/
/*
 *
 */

void Test_2(){

	ACS_CC_ReturnType returnCode;

	AdminOperation_MyImplementer obj;

	string implName = "NomeImplementer";

	string objImplemented = "provaConfObj1,safApp=safImmService";

	string classImplemented = "Test_config";

	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	/*Param for AdminOperation*/
	long long int timeOutVal_5sec = 5*(1000000000LL);

	long long int timeOutVal_10sec = 10*(1000000000LL);

	acs_apgcc_adminoperation admOp;

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;



	/*1.3  init the Administrative Operation Implementer*/
	returnCode = obj.init(implName);


	ASSERTM("1.1 Test_2:obj.init ", returnCode == ACS_CC_SUCCESS );

	/*1.4 set as class implementer */

	returnCode = obj.setClassImplemented(classImplemented);

	ASSERTM("1.2 Test_2:obj.setClassImplemented ", returnCode == ACS_CC_SUCCESS );

	/*Activate thread to wait for operations*/
	MyThread th2(&obj);

	th2.activate();

	/*1.5Prepare to send Admin operation on Implemented Class*/

	/*create first Elemenet of param list*/
	char attName1[]= "attName1";

	firstElem.attrName =attName1;

	firstElem.attrType=ATTR_FLOATT;

	float floatValue = 444.9;

	firstElem.attrValues=reinterpret_cast<void*>(&floatValue);

	/*create second Elemenet of param list*/
	char attName2[]= "attName2";

	secondElem.attrName =attName2;

	secondElem.attrType=ATTR_INT32T ;

	int intValue =15;

	secondElem.attrValues=reinterpret_cast<void*>(&intValue);

	/*insert into the vector*/

	vectorIN.push_back(firstElem);

	vectorIN.push_back(secondElem);


	returnCode = admOp.init();

	ASSERTM("1.3 Test_2:obj.admOp.init ", returnCode == ACS_CC_SUCCESS );


	int adminOpreturnValue;
	int adminOpreturnValue2;
	int adminOpreturnValue3 = 8;


	returnCode = admOp.adminOperationInvoke(dnObjName1, 0, 5, vectorIN, &adminOpreturnValue, timeOutVal_5sec);

	ASSERTM("1.3.1 Test_2:admOp.adminOperationInvoke ", returnCode == ACS_CC_SUCCESS );

	ASSERTM("1.3.2 Test_2:admOp.adminOperationInvoke check result ", adminOpreturnValue == 28 );
	cout<<"... adminOpreturnValue ="<< adminOpreturnValue<<endl;
	/*send another AdminOperation expected to fail because of timeout*/
	returnCode = admOp.adminOperationInvoke(dnObjName1, 0, 100, vectorIN, &adminOpreturnValue2, timeOutVal_10sec);

	ASSERTM("1.3.3 Test_1:admOp.adminOperationInvoke ", returnCode == ACS_CC_FAILURE );
	ASSERTM("1.3.4 Test_1:admOp.adminOperationInvoke check result", admOp.getInternalLastError() == -5 );
	ASSERTM("1.3.5 Test_1:admOp.adminOperationInvoke check result", strcmp (admOp.getInternalLastErrorText(), "ERR_IMM_TIMEOUT") == 0 );


	sleep (2);
	/*1.4 Prepare to send Admin operation on  OBJ2*/
	cout<<"...11 adminOpreturnValue3 ="<< adminOpreturnValue3<<endl;

	returnCode = admOp.adminOperationInvoke(dnObjName2, 0, 5, vectorIN, &adminOpreturnValue3, timeOutVal_5sec);

	ASSERTM("1.4 Test_1:admOp.adminOperationInvoke ", returnCode == ACS_CC_SUCCESS );

	cout<<"...21 adminOpreturnValue3 ="<< adminOpreturnValue3<<endl;
	ASSERTM("1.4.1 Test_1:admOp.adminOperationInvoke check result", adminOpreturnValue3 == 28);


	/*1.5 finalize*/
	returnCode = admOp.finalize();
	ASSERTM("1.5 Test_1:admOp.finalize() ", returnCode == ACS_CC_SUCCESS );


	/*1.6 release the object imple1.8 release the object implementedmented*/
	sleep(2);
	th2.stop();

	returnCode = obj.releaseClassImplemented(classImplemented);
	ASSERTM("1.6 Test_1:obj.releaseClassImplemented ", returnCode == ACS_CC_SUCCESS );



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
	cout << "Admin Operation CUTE Test!!!" << endl;

	runSuite();

	cout << "!!!Admin Operation CUTE Test!! END!!!" << endl;

	return 0;
}
