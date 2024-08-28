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
#include "AdminOperationAsync_MyImplementer.h"
#include "acs_apgcc_adminoperation.h"
#include "acs_apgcc_omhandler.h"
#include <map>



//#include "ACS_CC_Types.h"




#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"
#include <sys/poll.h>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"


//std::map<ACS_APGCC_InvocationType,char> mymap;
std::map<ACS_APGCC_InvocationType,structCode> mymap;
std::map<ACS_APGCC_InvocationType, structCode> expectedResult;

OmHandler omHandler_NO_OI;
/*thread*/
//class MyThread : public ACE_Task_Base {
//public :
//
//	/**Constructor**/
//	MyThread(AdminOperation_MyImplementer *p_user1){
//
//		cout<<"Costruttore Thread"<<endl;
//
//		implementer = p_user1;
//
//		isStop = false;
//	}
//
//
//	/**Destructor**/
//	~MyThread(){
//		cout<<"Distruttore Thread"<<endl;
//	}
//
//
//	/**This method is used to stop the thread**/
//	void stop(){
//
//		cout<<"Method Stop invocated"<<endl;
//
//		isStop=true;
//	}
//
//
//	/**svc method **/
//	int svc(void){
//
//
//		cout<<"Thread AVVIATO"<<endl;
//
//		int ret;
//
//		cout<<"getSelObj: "<<implementer->getSelObj()<<endl;
//
//		struct pollfd fds[1];
//
//		fds[0].fd = implementer->getSelObj();
//		fds[0].events = POLLIN;
//
//
//		while(!isStop)
//		{
//			ret = poll(fds, 1, 1);
//			if (ret == 0)
//			{
//				//cout<<"timeout off"<<endl;
//
//			}else if (ret == -1){
//				printf("poll error: \n");
//			}else{
//
//				cout<<"FD THREAD: "<<fds[0].fd<<endl;
//				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
//
//			}
//		}
//
//		cout<<"Thread TERMINATO"<<endl;
//
//		return 0;
//	}
//
//private:
//
//	AdminOperation_MyImplementer *implementer;
//
//	bool isStop;
//
//};

/*end define thread*/


/*thread for */
class MyThreadOM : public ACE_Task_Base {
public :

	/**Constructor**/
	//MyThread(AdminOperationAsync_MyImplementer *p_user1){
	MyThreadOM(){

		cout<<"Costruttore Thread OM"<<endl;

		//implementer = p_user1;

		isStop = false;
	}


	void AddOMImplementer(AdminOperationAsync_MyImplementer *p_user1){

		cout<<"AddOMImplementer"<<endl;

		implementerOM = p_user1;


	}

	void AddOIImplementer(AdminOperation_MyImplementer *p_user1){

			cout<<"AddOMImplementer"<<endl;

			implementerOI = p_user1;


		}

	/**Destructor**/
	~MyThreadOM(){
		cout<<"Distruttore Thread OM"<<endl;
	}


	/**This method is used to stop the thread**/
	void stop(){

		cout<<"Method Stop invocated OM"<<endl;

		isStop=true;
	}


	/**svc method **/
	int svc(void){


		cout<<"Thread AVVIATO OM"<<endl;

		int ret;

		cout<<"getSelObj: "<<implementerOI->getSelObj()<<endl;

		struct pollfd fds[2];

		fds[0].fd = implementerOI->getSelObj();
		fds[0].events = POLLIN;


		fds[1].fd = implementerOM->getSelObj();
		fds[1].events = POLLIN;

		while(!isStop)
		{
			ret = poll(fds, 2, 1);
			if (ret == 0)
			{
				//cout<<"timeout off"<<endl;

			}else if (ret == -1){
				printf("poll error: \n");
			}else{

				cout<<"OM FD THREAD: "<<fds[0].fd<<endl;
				if (fds[0].revents == POLLIN)
					implementerOI->dispatch(ACS_APGCC_DISPATCH_ALL);

				if (fds[1].revents == POLLIN)
					implementerOM->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		cout<<"Thread TERMINATO OM"<<endl;

		return 0;
	}

private:

	AdminOperationAsync_MyImplementer 	*implementerOM;
	AdminOperation_MyImplementer 		*implementerOI;

	bool isStop;

};

/**/

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

	void * value[1]= {reinterpret_cast<void*>(rdnValue)};
	attrRdn.attrValues= value;

	cout<<"rdn"<<endl;


	/*Fill the ATTRIBUTE_1 */
	char name_attFloat[]="ATTRIBUTE_1";
	attr2.attrName = name_attFloat;
	attr2.attrType = ATTR_FLOATT;
	attr2.attrValuesNum = 1;
	float floatValue = 222.9;

	void * value2[1]= {reinterpret_cast<void*>(&floatValue)};
	attr2.attrValues= value2;

	cout<<"rdn1"<<endl;

	/*Fill the ATTRIBUTE_6 */
	char name_attUint32[]="ATTRIBUTE_6";
	attr3.attrName = name_attUint32;
	attr3.attrType = ATTR_UINT32T;
	attr3.attrValuesNum = 1;
	unsigned int intValueU32 = 123;

	void * value3[1]= {reinterpret_cast<void*>(&intValueU32)};
	attr3.attrValues= value3;

	cout<<"rdn2"<<endl;

	char name_attrSaString[]= "ATTRIBUTE_8";
	attr4.attrName = name_attrSaString;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	char* stringValue = const_cast<char*>("pippo");
//	attr4.attrValues = new void*[1];
//	attr4.attrValues[0] =	reinterpret_cast<void*>(stringValue);
	void * value4[1]= {reinterpret_cast<void*>(stringValue)};
	attr4.attrValues= value4;
	cout<<"rdn3"<<endl;
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
 * 1.4)		set as obj implementer
 * 1.4.1)	initialize the object to send AdminOperation Asyncronous
 * 			define thread to wait for callbacks
 * 			add the reference to OI
 *
 * 1.5)		Prepare to send AdminOperation on created objects: obj.admOp.init
 *
 * 1.5.1)	invoke adminoperation on not existing obj: expected result failure
 * 1.5.1)	check result
 * 			Add OM referece for thread
 * 			start thread
 *
 * 1.5.2)	invoke adminoperation on  existing  and implemented obj expected result success OI callback
 * 			will return after 11 sec with succes. Invocation=1 operation = 100
 *
 * 1.5.3)	invoke adminoperation on  existing  and  implemented obj expected. here the same invocation id than
 * 			previous is used so the invoke should fail because of Invalid Param. Invocation=1 operation = 10
 *
 * 1.5.4)	invoke adminoperation on  existing  and not implemented obj expected result success but the OM callback
 * 			should return with error code 12 not existing. Invocation=2 operation = 101
 *
 * 1.5.5)	invoke adminoperation on  existing  and  implemented obj expected result success, the OI callback
 * 			should return with error code 1 and return val 28 since opId< 10
 *
 *
 *
 * 1.7)		finalize admOp
 *
 * 1.8)		sleep 60 sec to wait for all callbacks then check for each invocation the result
 *
 * 1.9)		stop the thread
 *
 * 1.10)	release the object implementedmented
 *
 * 1.11)	finalize
 *
 */

void Test_1(){

	ACS_CC_ReturnType returnCode;
	AdminOperation_MyImplementer obj;
	AdminOperationAsync_MyImplementer objAsync;
	structCode retVal1 = {1,1};
	structCode retVal2 = {12,27};
	structCode retVal3 = {1,28};
	expectedResult[1] = retVal1;
	expectedResult[2] = retVal2;
	expectedResult[3] = retVal3;



	char choice;
	string implName = "NomeImplementer";
	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";
	char nameClass[20] = "Test_config";

	const char *objNameCreate1 = "provaConfObj1";
	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";



	const char *objNameCreate2 = "provaConfObj2";
	const char *dnObjName2 = "provaConfObj2,safApp=safImmService";

	const char *dnObjName3 = "provaConfObj3,safApp=safImmService";

	/*Param for AdminOperation*/

	acs_apgcc_adminoperation admOp;

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;


	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;

	ACS_APGCC_AdminOperationParamType terzoElem;

	ACS_APGCC_AdminOperationParamType quartoElem;

	ACS_APGCC_AdminOperationParamType quintoElem;

	ACS_APGCC_AdminOperationParamType sestoElem;

	ACS_APGCC_AdminOperationParamType settimoElem;

	ACS_APGCC_AdminOperationParamType ottavoElem;

	ACS_APGCC_AdminOperationParamType nonoElem;

	/*1.1*/
	returnCode = omHandler_NO_OI.Init();

	ASSERTM("1.1 Test_1:omHandler_NO_OI.Init", returnCode == ACS_CC_SUCCESS );

	/**1.1 create OBJ1   **/
	int res;
	res = createObjectTest( nameClass, objNameCreate1);

	ASSERTM("1.1 Test_1:createObject objNameCreate1 ", res == 0 );

	/**1.2 create OBJ2 **/

	res = createObjectTest(nameClass, objNameCreate2);
	expectedResult[1] = retVal1;
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

	/*1.4.1 initialize the object to send AdminOperation Asyncronous*/
	returnCode = objAsync.init();

	ASSERTM("1.4.1 Test_1:objAsync.init ", returnCode == ACS_CC_SUCCESS );

	/*Define thread to wait for operations*/
	MyThreadOM th;
	th.AddOIImplementer(&obj);


	/*1.5Prepare to send Admin operation on Implemented OBJ*/


/*List of all param*/
	/*create first Elemenet of param list*/
	char attName1[]= "attName1";

	firstElem.attrName =attName1;

	firstElem.attrType=ATTR_INT32T;

	int intValue1 = 18;

	firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

	/*create second Elemenet of param list*/
	char attName2[]= "attName2";

	secondElem.attrName =attName2;

	secondElem.attrType=ATTR_UINT32T ;

	unsigned int intValue2 =222;

	secondElem.attrValues=reinterpret_cast<void*>(&intValue2);

	/*create 3 Elemenet of param list*/
	char attName3[]= "attName3";

	terzoElem.attrName =attName3;

	terzoElem.attrType=ATTR_INT64T ;

	long long int intValue3 =3333;

	terzoElem.attrValues=reinterpret_cast<void*>(&intValue3);

	/*create 4 Elemenet of param list*/
	char attName4[]= "attName4";

	quartoElem.attrName =attName4;

	quartoElem.attrType=ATTR_UINT64T ;

	unsigned long long int intValue4 =4444;

	quartoElem.attrValues=reinterpret_cast<void*>(&intValue4);

	/*create 5 Elemenet of param list*/
	char attName5[]= "attName5";
	quintoElem.attrName =attName5;

	quintoElem.attrType=ATTR_UINT64T ;

	long long int intValue5 =1000000000;

	quintoElem.attrValues=reinterpret_cast<void*>(&intValue5);

	/*create 6 Elemenet of param list*/
	char attName6[]= "attName6";

	sestoElem.attrName =attName6;

	sestoElem.attrType=ATTR_NAMET ;

	char *value6= const_cast<char*>("StringValue6");

	sestoElem.attrValues=reinterpret_cast<void*>(&value6);

	/*create 7 Elemenet of param list*/
	char attName7[]= "attName7";

	settimoElem.attrName =attName7;

	settimoElem.attrType=ATTR_FLOATT ;

	float value7= 18.53;

	settimoElem.attrValues=reinterpret_cast<void*>(&value7);

	/*create 8 Elemenet of param list*/
	char attName8[]= "attName8";

	ottavoElem.attrName =attName8;

	ottavoElem.attrType=ATTR_FLOATT ;

	float value8= 188.553;

	ottavoElem.attrValues=reinterpret_cast<void*>(&value8);

	/*create 9 Elemenet of param list*/
	char attName9[]= "attName9";

	nonoElem.attrName =attName9;

	nonoElem.attrType=ATTR_STRINGT ;


	char *value9= "StringValue9";

	nonoElem.attrValues=reinterpret_cast<void*>(&value9);
/*end list of all param*/


	/*insert into the vector*/

	vectorIN.push_back(firstElem);

	vectorIN.push_back(secondElem);

	vectorIN.push_back(terzoElem);

	vectorIN.push_back(quartoElem);

	vectorIN.push_back(quintoElem);

	vectorIN.push_back(sestoElem);

	vectorIN.push_back(settimoElem);

	vectorIN.push_back(ottavoElem);

	vectorIN.push_back(nonoElem);


	returnCode = objAsync.init();

	ASSERTM("1.5 Test_1:objAsync.init ", returnCode == ACS_CC_SUCCESS );

	/*prepare invocation and operation*/
	ACS_APGCC_InvocationType invocation = 1;
	ACS_APGCC_AdminOperationIdType operationId=100;

	/*1.5.1 invoke adminoperation on not existing obj expected result failure*/
	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName3, 0, 120, vectorIN);
	ASSERTM("1.5.1 Test_1:admOp.adminOperationInvoke dnObjName3", returnCode == ACS_CC_FAILURE );/*element not exixting*/
	ASSERTM("1.5.1.1 Test_1:admOp.adminOperationInvoke dnObjName", objAsync.getInternalLastError() == -12 );

	th.AddOMImplementer(&objAsync);

	th.activate();

	/*1.5.2 invoke adminoperation on  existing  and implemented obj expected result success OI callback will return after
	 * 11 sec with succes */
	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName1, 0, operationId, vectorIN);
	ASSERTM("1.5.2 Test_1:admOp.adminOperationInvoke on dnObjName1 ", returnCode == ACS_CC_SUCCESS );

	/*1.5.3 invoke adminoperation on  existing  and  implemented obj expected. here the same invocation id than
	 * previous is used so the invoke should fail because of Invalid Param
	 */
	operationId=10;
	invocation = 1;
	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName1, 0, operationId, vectorIN);
	ASSERTM("1.5.3 Test_1:admOp.adminOperationInvoke dnObjName1", returnCode == ACS_CC_FAILURE );
	ASSERTM("1.5.3.1 Test_1:admOp.adminOperationInvoke dnObjName1", objAsync.getInternalLastError() == -7 );

	sleep(1);

	/*1.5.4 invoke adminoperation on  existing  and not implemented obj expected result success but the OM callback
	 * should return with error code 12 not existing*/
	operationId=101;
	invocation = 2;
	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName2, 0, operationId, vectorIN);
	ASSERTM("1.5.4 Test_1:admOp.adminOperationInvoke dnObjName2", returnCode == ACS_CC_SUCCESS );


	/*1.5.5 invoke adminoperation on  existing  and  implemented obj expected result success the OI callback
	 * should return with error code 28 since opId< 10*/
	operationId=9;
	invocation = 3;
	returnCode = objAsync.adminOperationInvokeAsync(invocation, dnObjName1, 0, operationId, vectorIN);
	ASSERTM("1.5.5 Test_1:admOp.adminOperationInvoke dnObjName2", returnCode == ACS_CC_SUCCESS );




	/*1.8 sleep 60 sec to wait for all callbacks then check for each invocation the result*/

	sleep (80);
	cout<<"...........     40 sec passed "<<endl;
	map<ACS_APGCC_InvocationType,structCode>::iterator it;


	for ( it=mymap.begin() ; it != mymap.end(); it++ ){
		ASSERTM("CHECK RESULT OM Callback ", expectedResult[(*it).first].retVal == ((*it).second).retVal);
		ASSERTM("CHECK RESULT OM Callback ", expectedResult[(*it).first].errorCode == ((*it).second).errorCode);
		cout <<"...........     mymap["<< (*it).first <<"] ="<< " => " << ((*it).second).errorCode<< "  "<< ((*it).second).retVal<< endl;
		cout <<"...........    expectedResult["<< (*it).first <<"] = retval->" << expectedResult.find((*it).first)->second.retVal << "errorcode->"<<expectedResult.find((*it).first)->second.errorCode<< endl<< endl;
	}



	cout<<"............... now stopping thread"<<endl;
	/*1.9 stop the thread*/
	th.stop();
	/*1.10 release the object implementedmented*/
	returnCode = obj.releaseObjectImplemented(objImplemented);
	ASSERTM("1.8 Test_1:obj.releaseObjectImplemented ", returnCode == ACS_CC_SUCCESS );

	/*1.11 finalize*/
	returnCode = objAsync.finalize();
	ASSERTM("1.7 Test_1:objAsync.finalize() ", returnCode == ACS_CC_SUCCESS );


	cout<<"--------------------------------/////---------------------------------"<<endl;

	sleep (5);

}
/*end test 1*/




/*SUITE*/
void runSuite(){

	cute::suite s;

	/*define class*/
	s.push_back(CUTE(PrepareEnvironment));
	/*Obj crreation*/
	s.push_back(CUTE(Test_1));

	//s.push_back(CUTE(Test_2));

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
