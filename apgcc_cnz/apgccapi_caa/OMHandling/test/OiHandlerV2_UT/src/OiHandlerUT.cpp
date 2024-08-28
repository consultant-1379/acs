//============================================================================
// Name        : OiHandlerUT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>

#include "acs_apgcc_oihandler_V2.h"
#include "MyImplementer.h"
#include "MyImplementer_2.h"
#include "acs_apgcc_omhandler.h"

#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"

#include "ace/Task.h"
#include <sys/poll.h>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "saImmOm.h"


using namespace std;


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


class MyThread_2 : public ACE_Task_Base {
public :

	MyThread_2(MyImplementer_2 *p_user1){

		implementer = p_user1;
		isStop = false;

	}

	~MyThread_2(){}

	void stop(){
		isStop=true;
	}

	int svc(void){

		int ret;

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

		return 0;
	}

private:
	MyImplementer_2 *implementer;
	bool isStop;



};


void createClassTest(OmHandler omHandler, const char *p_className);
void createClassTestWithRuntimeAttr(OmHandler omHandler, const char *p_className);
void deleteClassTest(OmHandler omHandler);
void createObjectTest(OmHandler omHandler, const char* p_className, const char *p_objName);
void createObjectTest_2(OmHandler omHandler, const char* p_className, const char *p_objName);
void deleteObjectTest(OmHandler omHandler, const char *p_objName);
void modifyObjectTest(OmHandler omHandler, const char *p_objName);
void modifyFAILObjectTest(OmHandler omHandler, const char *p_objName);
void deleteObjectTestFailure(OmHandler omHandler, const char *p_objName);

void createClassTest(OmHandler omHandler, const char *p_className){

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

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;
	}else {
		cout << " -> ERROR: Class " << p_className << " not created" << endl;
	}


	//ASSERTM("OmHandler::defineClass ", result == ACS_CC_SUCCESS);

}


void createClassTestWithRuntimeAttr(OmHandler omHandler, const char *p_className){

	cout << " -> Try to create a new class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("AttrFloatRunNC"), ATTR_FLOATT, ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("AttrInt64RunC"),ATTR_INT64T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("AttrInt32RunC"),ATTR_INT32T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("AttrDoubleConf"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("AttrUnsInt64NC"),ATTR_UINT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("AttrUnsInt32NC"),ATTR_UINT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("AttrNameTRunNC"), ATTR_NAMET, ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("AttrStringRunC"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("AttrTimeConf"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};

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
	}
}


void createObjectTest(OmHandler omHandler, const char* p_className, const char *p_objName){


	cout << " -> Try to create a new object " << endl;
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

	result = omHandler.createObject(p_className, nomeParent, AttrList );
//	ASSERTM("OmHandler:createObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_className << "," << nomeParent << " created" << endl;
		cout << endl;
	}else {
		cout << " -> ERROR: Object  " << p_className << "," << nomeParent << " not created" << endl;
		cout << endl;
	}

}


void createObjectTest_2(OmHandler omHandler, const char* p_className, const char *p_objName){


	cout << " -> Try to create a new object " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	//char nameClass[20] = "Test_config";
	char* nomeParent = const_cast<char *>("safApp=safImmService");

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;
	ACS_CC_ValuesDefinitionType attributeI64;
	ACS_CC_ValuesDefinitionType attributeI32;
	ACS_CC_ValuesDefinitionType attributeDOUBLE;
	ACS_CC_ValuesDefinitionType attributeUnI64;
	ACS_CC_ValuesDefinitionType attributeUnI32;
	ACS_CC_ValuesDefinitionType attributeNAME;
	ACS_CC_ValuesDefinitionType attributeSTRING;
	ACS_CC_ValuesDefinitionType attributeTIME;


	/*Fill the rdn Attribute */
	char *attrdn = const_cast<char*>("RDN_Attribute");
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(const_cast<char*>(p_objName))};
	attributeRDN.attrValues = valueRDN;


	/*Fill the attribute attributeFLOAT*/
	char *name_attrFloat =  const_cast<char*>("AttrFloatRunNC");
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType =ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 0;
	attributeFLOAT.attrValues = 0;


	/*Fill the attribute attributeI64*/
	char *name_attrInt64 =  const_cast<char*>("AttrInt64RunC");
	attributeI64.attrName = name_attrInt64;
	attributeI64.attrType = ATTR_INT64T;
	attributeI64.attrValuesNum = 0;
	attributeI64.attrValues = 0;


	/*Fill the attribute attributeI32*/
	char *name_attrInt32 =  const_cast<char*>("AttrInt32RunC");
	attributeI32.attrName = name_attrInt32;
	attributeI32.attrType = ATTR_INT32T;
	attributeI32.attrValuesNum = 0;
	attributeI32.attrValues = 0;


	/*Fill the attribute attributeDOUBLE*/
	char *name_attrDouble =  const_cast<char*>("AttrDoubleConf");
	int intDouble_1 = 23.25;
	int intDouble_2 = 305;
	attributeDOUBLE.attrName = name_attrDouble;
	attributeDOUBLE.attrType =ATTR_DOUBLET;
	attributeDOUBLE.attrValuesNum = 2;
	void* valueDouble[2]={reinterpret_cast<void*>(&intDouble_1), reinterpret_cast<void*>(&intDouble_2)};
	attributeDOUBLE.attrValues = valueDouble;


	/*Fill the attribute attributeUnI64*/
	char *name_attrUnsI64 =  const_cast<char*>("AttrUnsInt64NC");
	attributeUnI64.attrName = name_attrUnsI64;
	attributeUnI64.attrType =ATTR_UINT64T;
	attributeUnI64.attrValuesNum = 0;
	attributeUnI64.attrValues = 0;


	/*Fill the attribute attributeUnI32*/
	char *name_attrUnsI32 =  const_cast<char*>("AttrUnsInt32NC");
	attributeUnI32.attrName = name_attrUnsI32;
	attributeUnI32.attrType = ATTR_UINT32T;
	attributeUnI32.attrValuesNum = 0;
	attributeUnI32.attrValues = 0;


	/*Fill the attribute attributeNAME*/
	char *name_attrNameT =  const_cast<char*>("AttrNameTRunNC");
	attributeNAME.attrName = name_attrNameT;
	attributeNAME.attrType = ATTR_NAMET;
	attributeNAME.attrValuesNum = 0;
	attributeNAME.attrValues = 0;


	/*Fill the attribute attributeSTRING*/
	char *name_attrString =  const_cast<char*>("AttrStringRunC");
	attributeSTRING.attrName = name_attrString;
	attributeSTRING.attrType = ATTR_STRINGT;
	attributeSTRING.attrValuesNum = 0;
	attributeSTRING.attrValues = 0;


	/*Fill the attribute attributeTIME*/
	char *name_attrTimeT =  const_cast<char*>("AttrTimeConf");
	long long int timeValue = SA_TIME_ONE_MICROSECOND;
	attributeTIME.attrName = name_attrTimeT;
	attributeTIME.attrType = ATTR_TIMET;
	attributeTIME.attrValuesNum = 1;
	void* valueTimeT[1]={reinterpret_cast<void*>(&timeValue)};
	attributeTIME.attrValues = valueTimeT;


	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);
	AttrList.push_back(attributeI64);
	AttrList.push_back(attributeI32);
	AttrList.push_back(attributeDOUBLE);
	AttrList.push_back(attributeUnI64);
	AttrList.push_back(attributeUnI32);
	AttrList.push_back(attributeNAME);
	AttrList.push_back(attributeSTRING);
	AttrList.push_back(attributeTIME);


	result = omHandler.createObject(p_className, nomeParent, AttrList );
	ASSERTM("OmHandler:createObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_className << "," << nomeParent << " created" << endl;
		cout << endl;
	}

}



void deleteObjectTest(OmHandler omHandler, const char *p_objName){

	cout << " -> Try to delete a object " << p_objName <<endl;
	cout << endl;

	ACS_CC_ReturnType result;

	result = omHandler.deleteObject(const_cast<char*>(p_objName));

	ASSERTM("OmHandler:deleteObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << " deleted" << endl;
		cout << endl;
	}

}

void deleteObjectTestFailure(OmHandler omHandler, const char *p_objName){

	cout << " -> Try to delete a object " << p_objName <<endl;
	cout << endl;

	ACS_CC_ReturnType result;

	result = omHandler.deleteObject(const_cast<char*>(p_objName));

	ASSERTM("OmHandler:deleteObject method ", result == ACS_CC_FAILURE);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << " deleted" << endl;
		cout << endl;
	}

}



void deleteClassTest(OmHandler omHandler, const char *p_className){

	cout << " -> Try to delete a class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;


	result = omHandler.removeClass(const_cast<char *>(p_className));

	ASSERTM("OmHandler::removeClass ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout<< " -> Class " << p_className << " deleted." << endl;
		cout<<endl;
	}
}


void modifyObjectTest(OmHandler omHandler, const char *p_objName){

	float valueFlo=1.3;
	char name_attFloat[]="ATTRIBUTE_1";
	ACS_CC_ImmParameter par;
	par.attrName=name_attFloat;
	par.attrType=ATTR_FLOATT;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);


	/*Set a new value for ATT_STRING*/
	char str1[30]="stringTest_1";
	ACS_CC_ImmParameter par2;
	par2.attrName=const_cast<char*>("ATTRIBUTE_8");
	par2.attrType=ATTR_STRINGT;
	par2.attrValuesNum=1;
	par2.attrValues=new void*[par2.attrValuesNum];
	par2.attrValues[0] =reinterpret_cast<void*>(str1);


	/*Set a new value for ATT_NAME*/
	char str3[30]="SaNameString_1";
	ACS_CC_ImmParameter par3;
	par3.attrName=const_cast<char*>("ATTRIBUTE_7");
	par3.attrType=ATTR_NAMET;
	par3.attrValuesNum=1;
	par3.attrValues=new void*[par3.attrValuesNum];
	par3.attrValues[0] =reinterpret_cast<void*>(str3);


	cout<<"NELLA MODIFY OBJECT"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_1 )", omHandler.modifyAttribute(p_objName, &par)==ACS_CC_SUCCESS);
	cout<<"MODIFICHE 1 EFFETTUATE"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_8 )", omHandler.modifyAttribute(p_objName, &par2)==ACS_CC_SUCCESS);
	cout<<"MODIFICHE 2 EFFETTUATE"<<endl;

	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTRIBUTE_7 )", omHandler.modifyAttribute(p_objName, &par3)==ACS_CC_SUCCESS);

	cout<<"MODIFICHE 3 EFFETTUATE"<<endl;

}

void modifyFAILObjectTest(OmHandler omHandler, const char *p_objName){

	int valueInt = 100;
	char name_attInt[]="ATTRIBUTE_3";
	ACS_CC_ImmParameter par;
	par.attrName=name_attInt;
	par.attrType=ATTR_INT32T;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueInt);


	//omHandler.modifyAttribute(p_objName, &par);
	ASSERTM("OmHandler:modifyAttributes method (ModifyFAIL attribute ATTRIBUTE_3 )", omHandler.modifyAttribute(p_objName, &par)==ACS_CC_FAILURE);


}


void createDeleteCallbackTest() {

	sleep(1);

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);


	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");

	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();


	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);


	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );
/**/
	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);


	implementer_1.reactor()->end_reactor_event_loop();

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);


}

void classImplTest_1() {

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addClassImpl(...) " << endl;
	cout << " TEST: Register an Object Implementer for the object class  " << endl;
	cout << " RESULT: The current process becomes the current implementer of the class \n"
		 <<	" \t whose name is specified by class name parameter and the method return \n"
		 << " \t with value ACS_CC_SUCCESS \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);


	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");

	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);


	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}

void classImplTest_2() {

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addClassImpl(...) " << endl;
	cout << " TEST: Try to register two Object Implementer for the same object class  " << endl;
	cout << " RESULT: The call of addClassImpl(...) method to registry the second  \n"
		 <<	" \tObject Implementer return with value ACS_CC_FAILURE because object  \n"
		 << " \tclass  whose name is specified by the className parameter has already \n"
		 << " \tan Object Implementer. " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);


	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");
	MyImplementer implementer_2("impName_20");

	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_SUCCESS);

	result = oiHandler.addClassImpl(&implementer_2, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_FAILURE);

	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("ACS_APGCC_OiHandler::removeClassImpl ", result == ACS_CC_SUCCESS);

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);



}

void classImplTest_3(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addClassImpl(...) " << endl;
	cout << " TEST: Try to register a Object Implementer for object class that not exist " << endl;
	cout << " RESULT: The call of addClassImpl(...) method return with value \n"
		 <<	" \t ACS_CC_FAILURE because the class name parameter does not specify  \n"
		 << " \t the name of an existing class.." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_100");

	result = oiHandler.addClassImpl(&implementer_1, "Pippo_Class");
	ASSERTM("ACS_APGCC_OiHandler::addClassImpl  ", result == ACS_CC_FAILURE);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void classImplTest_4(){

	/**Non lo permette perche la saImmOiImplementerSet fallisce restiuendomi come
	 * codice di errore 14 cioe' SA_AIS_ERR_EXIT. Quindi IMM non permette di
	 * registrare due Object implementer con lo stesso nome anche se fanno riferimento
	 * ad oggetti diversi.**/

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_OiHandler::addClassImpl(...) " << endl;
	cout << " TEST: Try to register a Object Implementer with the same name for two object class " << endl;
	cout << " RESULT: The second call of addClassImpl(...) method return with value \n"
		 <<	" \t ACS_CC_FAILURE because an Object Implementer with the same name is \n"
		 << " \t already registered with the IMM Service." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");
	MyImplementer implementer_2("impName_1");

	char nameClass_1[20] = "Test_config";
	createClassTest(omHandler, nameClass_1);

	char nameClass_2[20] = "Test_config_2";
	createClassTest(omHandler, nameClass_2);

	result = oiHandler.addClassImpl(&implementer_1, nameClass_1);
	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl for implementer_1 ", result == ACS_CC_SUCCESS);

	result = oiHandler.addClassImpl(&implementer_2, nameClass_2);
	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl for implementer_2", result == ACS_CC_FAILURE);

	result = oiHandler.removeClassImpl(&implementer_1, nameClass_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl for implementer_1 ", result == ACS_CC_SUCCESS);

	deleteClassTest(omHandler, nameClass_1);

	deleteClassTest(omHandler, nameClass_2);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void classImplTest_5(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::removeClassImpl(...) " << endl;
	cout << " TEST: Remove a Class Implementer previously registered as implementer of object class " << endl;
	cout << " RESULT: The method removeClassImpl(...) return with value ACS_CC_SUCCESS and the class\n"
		 <<	" \t implementer passed as parameter must not be considered anymore as the implementer  \n"
		 << " \t of the objects that are instances of the object class whose name as specified as parameter." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_2");
	cout<<"SEL_OBJ: "<<implementer_1.getSelObj()<<endl;

	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl  ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered " << endl;
	}

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor_2 = new ACE_Reactor(tp_reactor_impl) ;

	reactor_2->open(1);

	MyThread thread_2(&implementer_1, reactor_2);
	thread_2.activate();

	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}

	sleep(1);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	implementer_1.reactor()->end_reactor_event_loop();

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}

void classImplTest_6(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::removeClassImpl(...) " << endl;
	cout << " TEST: Try to remove a Class Implementer that was not previously registered \n"
		 <<	" \t as implementer of object class " << endl;
	cout << " RESULT: The method removeClassImpl(...) return with value ACS_CC_FAILURE " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");

	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_FAILURE);

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}

void classImplTest_7(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::removeClassImpl(...) " << endl;
	cout << " TEST: Try to remove a Class Implementer that was previously registered \n"
		 <<	" \tas implementer of another object class " << endl;
	cout << " RESULT: The method removeClassImpl(...) return with value ACS_CC_FAILURE " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init - Initialize IMM service ", result == ACS_CC_SUCCESS);

	char nameClass[20] = "Test_config_2";
	createClassTest(omHandler, nameClass);

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("impName_1");

	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl  ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered " << endl;
	}

	char nameClass_12[20] = "Test_config_12";
	result = oiHandler.removeClassImpl(&implementer_1, nameClass_12);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_FAILURE);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}

	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void objImplTest_1(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::addObjectImpl(...) " << endl;
	cout << " TEST: Register an Object Implementer for the object  " << endl;
	cout << " RESULT: The current process becomes the current implementer of the object \n"
		 <<	" \tidentifies by the scope and whose name is specified by  ObjectName parameter, \n"
		 << " \tthe method return with value ACS_CC_SUCCESS \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_SUCCESS);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void objImplTest_2(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::addObjectImpl(...) " << endl;
	cout << " TEST: Try to register two Object Implementer for the same object " << endl;
	cout << " RESULT: The call of addObjectImpl(...) method to registry the second  \n"
		 <<	" \tObject Implementer return with value ACS_CC_FAILURE because object  \n"
		 << " \twhose name is specified by the objectName parameter has already \n"
		 << " \tan Object Implementer. " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_1", ACS_APGCC_ONE );
	MyImplementer implementer_2(dnObjName, "impObjName_10", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);

	result = oiHandler.addObjectImpl(&implementer_2);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_FAILURE);

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl - implementer_1 ", result == ACS_CC_SUCCESS);

	result = oiHandler.removeObjectImpl(&implementer_2);
//	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_FAILURE);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);


}


void objImplTest_3(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::addObjectImpl(...) " << endl;
	cout << " TEST: Try to register a Object Implementer for object that not exist " << endl;
	cout << " RESULT: The call of addObjectImpl(...) method return with value \n"
		 <<	" \tACS_CC_FAILURE because the  ObjectName parameter does not specify  \n"
		 << " \tthe name of an existing class.." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);


	const char *dnObjName = "provaConf_100,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_100", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_FAILURE);

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_FAILURE);

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);


}


void objImplTest_4(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::addObjectImpl(...) " << endl;
	cout << " TEST: Try to register a Object Implementer with the same name for two object " << endl;
	cout << " RESULT: The second call of addObjectImpl(...) method return with value \n"
		 <<	" \tACS_CC_FAILURE because an Object Implementer with the same name is \n"
		 << " \talready registered with the IMM Service." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;



	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *objNameCreate_2 = "provaConf_2";
	createObjectTest(omHandler, nameClass, objNameCreate_2);

	const char *dnObjName = "provaConf,safApp=safImmService";
	const char *dnObjName_2 = "provaConf_2,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_1", ACS_APGCC_ONE );
	MyImplementer implementer_2(dnObjName_2, "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);

	result = oiHandler.addObjectImpl(&implementer_2);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_FAILURE);

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_SUCCESS);

	result = oiHandler.removeObjectImpl(&implementer_2);
//	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_FAILURE);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	const char *objNameDelete_2 = "provaConf_2,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete_2 );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);


}


void objImplTest_5(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::removeObjectImpl(...) " << endl;
	cout << " TEST: Remove a Object Implementer previously registered as implementer of object " << endl;
	cout << " RESULT: The method removeObjectImpl(...) return with value ACS_CC_SUCCESS and the object\n"
		 <<	" \timplementer passed as parameter must not be considered anymore as the implementer  \n"
		 << " \tof the object." << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;



	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_SUCCESS);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}



void objImplTest_6(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::removeObjectImpl(...) " << endl;
	cout << " TEST: Try to remove a Object Implementer that was not previously registered \n"
		 <<	" \tas implementer of object " << endl;
	cout << " RESULT: The method removeClassImpl(...) return with value ACS_CC_FAILURE " << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1(dnObjName, "impObjName_1", ACS_APGCC_ONE );

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_FAILURE);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void ModifyCallbackTest(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::ModifyCallback(...) " << endl;
	cout << " TEST: call a ModifyCallback method  " << endl;
	cout << " RESULT: The method ModifyCallback(...) is called after a change of an configuration \n"
		 <<	" \attributes  of object "<< endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("provaConf,safApp=safImmService", "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();

	modifyObjectTest(omHandler, dnObjName);

	implementer_1.reactor()->end_reactor_event_loop();

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler::removeObjectImpl ", result == ACS_CC_SUCCESS);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);


}


void abortCallbackTest(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::AbortCallback(...) " << endl;
	cout << " TEST: call a AbortCallback method  " << endl;
	cout << " RESULT: The method AbortCallback(...) is called after attempting to make a change \n"
		 <<	" \tnot allowed "<< endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("provaConf,safApp=safImmService", "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_2(&implementer_1, reactor);
	thread_2.activate();

	modifyFAILObjectTest(omHandler, dnObjName);



	implementer_1.reactor()->end_reactor_event_loop();

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_SUCCESS);

	sleep(1);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

	/**workaround: sul SIMAP3 non mi permette di eliminare l'oggetto al seguito della abort**/
	OmHandler omHandler_2;

	result = omHandler_2.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	const char *objNameDelete = "provaConf,safApp=safImmService";

	cout<<"NAME CLASS: "<<objNameDelete<<endl;
	deleteObjectTest(omHandler_2, objNameDelete );

	deleteClassTest(omHandler_2, nameClass);

	result = omHandler_2.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void myImplementerTest_1(){

	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " ACS_APGCC_ObjectImplementerInterface::" << endl;
	cout << " TEST: test a method of class ACS_APGCC_ObjectImplementerInterface  " << endl;
	cout << " RESULT: The method of class ACS_APGCC_ObjectImplementerInterface return \n"
		 <<	" \twith correct value  "<< endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	cout<<"SONO TORNATO DALLA CREATEOBJECT"<<endl;

	const char *dnObjName = "provaConf,safApp=safImmService";
	const char *impName = "impName_18";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer_2 implementer;

	implementer.setObjName(dnObjName);
	implementer.setImpName(impName);
	implementer.setScope(scope);

	cout<<"PRIMA DI CHIAMARE LA ADDOBJECT"<<endl;
	result = oiHandler.addObjectImpl(&implementer);
//	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl  ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer registered " << endl;
	}else{
		cout << " ->ERROR: Implementer not registered " << endl;
	}

	int fd = implementer.getSelObj();
	cout << "NEW" << endl;
	ASSERTM("ACS_APGCC_ObjectImplementerInterface::getSelObj()  ", fd == implementer.getSelObj());

	ASSERTM("ACS_APGCC_ObjectImplementerInterface::getObjName()  ", strcmp(dnObjName, implementer.getObjName().c_str()) == 0);

	ASSERTM("ACS_APGCC_ObjectImplementerInterface::getImpName()  ", strcmp(impName, implementer.getImpName().c_str()) == 0);

	ASSERTM("ACS_APGCC_ObjectImplementerInterface::getScope()  ",  implementer.getScope() == scope);

	cout<<"STO PER ISTANZIARE IL THREAD"<<endl;

	MyThread_2 th(&implementer);
	th.activate();

	cout<<"THREAD PARTITO ED AVVIATO"<<endl;

	modifyObjectTest(omHandler, dnObjName);

	cout<<"SONO USCITO DALLA MODIFY OBJECT"<<endl;

	th.stop();

	sleep(1);

	result = oiHandler.removeObjectImpl(&implementer);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_SUCCESS);
	if(result == ACS_CC_SUCCESS){
		cout << " -> Implementer removed " << endl;
	}

	sleep(1);

	const char *objNameDelete = "provaConf,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

}


void updateCallbackTest(){


	sleep(1);

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::OiRtAttUpdateCallback(...) " << endl;
	cout << " TEST: call a OiRtAttUpdateCallback method  " << endl;
	cout << " RESULT: The method OiRtAttUpdateCallback(...) is called after attempting to update a \n"
		 <<	" \tnot-cached runtime attribute"<< endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;
	acs_apgcc_oihandler_V2 oiHandler;


	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);


	/**create a class **/
	char nameClass[30] = "Test_config_with_runAttr";
	createClassTestWithRuntimeAttr(omHandler, nameClass);

	MyImplementer implementer_1("impName_1");


	result = oiHandler.addClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::addClassImpl  ", result == ACS_CC_SUCCESS);

	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_1(&implementer_1, reactor);
	thread_1.activate();

	/**create object**/
	const char *objNameCreate = "obj_conf_with_run";
	createObjectTest_2(omHandler, nameClass, objNameCreate);

	const char *objName = "obj_conf_with_run,safApp=safImmService";


	sleep(1);

	/******************************Change the NOT-CACHED attribute of object*************************************/

	/*Set a new value for attributeFLOAT*/
	ACS_CC_ImmParameter parToModify_1;
	float new_floatValue_1 = 67.34;
	char *name_attrFloat = const_cast<char*>("AttrFloatRunNC");
	parToModify_1.attrName = name_attrFloat;
	parToModify_1.attrType = ATTR_FLOATT;
	parToModify_1.attrValuesNum = 1;
	void* new_valueFLOAT[1] = {reinterpret_cast<void*>(&new_floatValue_1)};
	parToModify_1.attrValues = new_valueFLOAT;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_1);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (not-cached) changed\n", name_attrFloat);
	}else{
		printf(" --> Parameter %s (not-cached) NOT changed\n", name_attrFloat);
	}

//	parToModify_1.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_1;
	paramAfterChange_1.attrName = name_attrFloat;
	result = omHandler.getAttribute(objName, &paramAfterChange_1 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_1.attrValuesNum; i++){
			cout<<" --> Float value: "<<*(float*)paramAfterChange_1.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);


	/*Set a new value for attributeUnsInt64*/
	ACS_CC_ImmParameter parToModify_2;
	unsigned long new_unsInt64Value = 998877;
	char *name_attrUnsInt64 = const_cast<char*>("AttrUnsInt64NC");
	parToModify_2.attrName = name_attrUnsInt64;
	parToModify_2.attrType = ATTR_UINT64T;
	parToModify_2.attrValuesNum = 1;
	void* new_valueUnsInt64[1] = {reinterpret_cast<void*>(&new_unsInt64Value)};
	parToModify_2.attrValues = new_valueUnsInt64;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_2);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (not-cached) changed\n", name_attrUnsInt64);
	}else{
		printf(" --> Parameter %s (not-cached) NOT changed\n", name_attrUnsInt64);
	}

//	parToModify_2.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_2;
	paramAfterChange_2.attrName = name_attrUnsInt64;
	result = omHandler.getAttribute(objName, &paramAfterChange_2 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_2.attrValuesNum; i++){
			cout<<" --> Unsigned int64 value: "<<*(unsigned long*)paramAfterChange_2.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);



	/*Set a new value for attributeUnsInt32*/
	ACS_CC_ImmParameter parToModify_3;
	unsigned int new_unsInt32Value = 332211;
	char *name_attrUnsInt32 = const_cast<char*>("AttrUnsInt32NC");
	parToModify_3.attrName = name_attrUnsInt32;
	parToModify_3.attrType = ATTR_UINT32T;
	parToModify_3.attrValuesNum = 1;
	void* new_valueUnsInt32[1] = {reinterpret_cast<void*>(&new_unsInt32Value)};
	parToModify_3.attrValues = new_valueUnsInt32;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_3);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (not-cached) changed\n", name_attrUnsInt32);
	}else{
		printf(" --> Parameter %s (not-cached) NOT changed\n", name_attrUnsInt32);
	}

//	parToModify_3.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_3;
	paramAfterChange_3.attrName = name_attrUnsInt32;
	result = omHandler.getAttribute(objName, &paramAfterChange_3 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_3.attrValuesNum; i++){
			cout<<" --> Unsigned int32 value: "<<*(unsigned int*)paramAfterChange_3.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);


	/*Set a new value for AttrNameTRunNC*/
	ACS_CC_ImmParameter parToModify_6;
	char* saNameValue_1 = const_cast<char *>("provaSaName_1");
	char* saNameValue_2 = const_cast<char *>("provaSaName_2");
	char *name_attrSaNameT = const_cast<char*>("AttrNameTRunNC");
	parToModify_6.attrName = name_attrSaNameT;
	parToModify_6.attrType = ATTR_NAMET;
	parToModify_6.attrValuesNum = 2;
	void* new_valueSaNameT[2] = {reinterpret_cast<void*>(saNameValue_1), reinterpret_cast<void*>(saNameValue_2)};
	parToModify_6.attrValues = new_valueSaNameT;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_6);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (not-cached) changed\n", name_attrSaNameT);
	}else{
		printf(" --> Parameter %s (not-cached) NOT changed\n", name_attrSaNameT);
	}

//	parToModify_6.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_6;
	paramAfterChange_6.attrName = name_attrSaNameT;
	result = omHandler.getAttribute(objName, &paramAfterChange_6 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_6.attrValuesNum; i++){
			cout<<" --> SaNameT value: "<<(char*)paramAfterChange_6.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);



	/******************************CHANGE CACHED RUNTIME ATTRIBUTE NO CALLBACK CALLED *************************************/

	/*Set a new value for AttrInt64RunC*/
	ACS_CC_ImmParameter parToModify_4;
	long long int new_int64Value = -998877;
	char *name_attrInt64 = const_cast<char*>("AttrInt64RunC");
	parToModify_4.attrName = name_attrInt64;
	parToModify_4.attrType = ATTR_INT64T;
	parToModify_4.attrValuesNum = 1;
	void* new_valueInt64[1] = {reinterpret_cast<void*>(&new_int64Value)};
	parToModify_4.attrValues = new_valueInt64;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_4);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (cached) changed\n", name_attrInt64);
	}else{
		printf(" --> Parameter %s (cached) NOT changed\n", name_attrInt64);
	}

//	parToModify_4.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_4;
	paramAfterChange_4.attrName = name_attrInt64;
	result = omHandler.getAttribute(objName, &paramAfterChange_4 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_4.attrValuesNum; i++){
			cout<<" --> int64 value: "<<*(long long int*)paramAfterChange_4.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);

	/*Set a new value for AttrInt32RunC*/
	ACS_CC_ImmParameter parToModify_5;
	int new_int32Value = -112233;
	char *name_attrInt32 = const_cast<char*>("AttrInt32RunC");
	parToModify_5.attrName = name_attrInt32;
	parToModify_5.attrType = ATTR_INT32T;
	parToModify_5.attrValuesNum = 1;
	void* new_valueInt32[1] = {reinterpret_cast<void*>(&new_int32Value)};
	parToModify_5.attrValues = new_valueInt32;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_5);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (cached) changed\n", name_attrInt32);
	}else{
		printf(" --> Parameter %s (cached) NOT changed\n", name_attrInt32);
	}

//	parToModify_5.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_5;
	paramAfterChange_5.attrName = name_attrInt32;
	result = omHandler.getAttribute(objName, &paramAfterChange_5 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_5.attrValuesNum; i++){
			cout<<" --> int32 value: "<<*(int*)paramAfterChange_5.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;

	sleep(1);

	/*Set a new value for AttrStringRunC*/
	ACS_CC_ImmParameter parToModify_7;
	char* stringValue_1 = const_cast<char *>("provaString_1");
	char* stringValue_2 = const_cast<char *>("provaString_2");
	char *name_attrStringT = const_cast<char*>("AttrStringRunC");
	parToModify_7.attrName = name_attrStringT;
	parToModify_7.attrType = ATTR_STRINGT;
	parToModify_7.attrValuesNum = 2;
//	void* new_valueString[2] = {reinterpret_cast<void*>(stringValue_1), reinterpret_cast<void*>(stringValue_2)};
//	parToModify_7.attrValues = new_valueString;

	parToModify_7.attrValues=new void*[parToModify_7.attrValuesNum];
	parToModify_7.attrValues[0] =reinterpret_cast<void*>(stringValue_1);
	parToModify_7.attrValues[1] =reinterpret_cast<void*>(stringValue_2);

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_7);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_SUCCESS);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (cached) changed\n", name_attrStringT);
	}else{
		printf(" --> Parameter %s (cached) NOT changed\n", name_attrStringT);
	}

//	parToModify_7.attrValues = 0;

	delete parToModify_7.attrValues;

	ACS_CC_ImmParameter paramAfterChange_7;
	paramAfterChange_7.attrName = name_attrStringT;
	result = omHandler.getAttribute(objName, &paramAfterChange_7 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_7.attrValuesNum; i++){
			cout<<" --> String value: "<<(char*)paramAfterChange_7.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;


	sleep(1);

	/******************************   CHANGE ATTRIBUTE FAILED   *************************************/

	/*Set a new value for AttrStringRunC*/
	ACS_CC_ImmParameter parToModify_8;
	char* stringValue = const_cast<char *>("provaString_1");
	char *name_attrStr = const_cast<char*>("wrong_name");
	parToModify_8.attrName = name_attrStr;
	parToModify_8.attrType = ATTR_STRINGT;
	parToModify_8.attrValuesNum = 1;
	void* new_valueStr[1] = {reinterpret_cast<void*>(stringValue)};
	parToModify_8.attrValues = new_valueStr;

	result = implementer_1.modifyRuntimeObj(objName, &parToModify_8);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", result == ACS_CC_FAILURE);

	if(result == ACS_CC_SUCCESS){
		printf(" --> Parameter %s (cached) changed\n", name_attrStr);
	}else{
		printf(" --> Parameter %s (cached) NOT changed\n", name_attrStr);
	}

//	parToModify_8.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_8;
	paramAfterChange_8.attrName = name_attrStr;
	result = omHandler.getAttribute(objName, &paramAfterChange_8 );

	if( result == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_8.attrValuesNum; i++){
			cout<<" --> String value: "<<(char*)paramAfterChange_8.attrValues[i]<<endl;
		}

	}

	cout<<endl;
	cout<<endl;
	cout<<endl;


	sleep(1);


	result = oiHandler.removeClassImpl(&implementer_1, nameClass);
	ASSERTM("acs_apgcc_oihandler_V2::removeClassImpl ", result == ACS_CC_SUCCESS);


	/**delete object**/
	const char *objNameDelete = "obj_conf_with_run,safApp=safImmService";
	deleteObjectTest(omHandler, objNameDelete );

	implementer_1.reactor()->end_reactor_event_loop();

	/**delete a class**/
	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

	delete reactor;
	delete tp_reactor_impl;


}


/**ONLY for TEST**/
void testAbort(){

	sleep(1);

	SaAisErrorT errorCode;
	SaImmHandleT immHandle;
	SaVersionT version;


	version.releaseCode  = 'C';
	version.majorVersion =  2;
	version.minorVersion =  1;

	errorCode = saImmOmInitialize(&immHandle, NULL, &version);

	printf(" IMM Version Actually Installed:"
			" Major Version %u, Minor Version %u, Relese Code %u\n",
			version.majorVersion,version.minorVersion,version.releaseCode);
//	cout<< 	version.releaseCode << endl;
//	cout<< 	version.majorVersion << endl;
//	cout<< 	version.minorVersion << endl;

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " acs_apgcc_oihandler_V2::AbortCallback(...) " << endl;
	cout << " TEST: call a AbortCallback method  " << endl;
	cout << " RESULT: The method AbortCallback(...) is called after attempting to make a change \n"
		 <<	" \tnot allowed "<< endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	ASSERTM("OmHandler::Init ", result == ACS_CC_SUCCESS);

	/**create a class **/
	char nameClass[20] = "Test_config";
	createClassTest(omHandler, nameClass);

	/**create an object **/
	const char *objNameCreate = "provaConf";
	createObjectTest(omHandler, nameClass, objNameCreate);

	const char *dnObjName = "provaConf,safApp=safImmService";

	acs_apgcc_oihandler_V2 oiHandler;

	MyImplementer implementer_1("provaConf,safApp=safImmService", "impObjName_1", ACS_APGCC_ONE );

	/**Set an object implementer**/
	result = oiHandler.addObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::addObjectImpl  ", result == ACS_CC_SUCCESS);


	ACE_TP_Reactor *tp_reactor_impl = new ACE_TP_Reactor;
	ACE_Reactor *reactor = new ACE_Reactor(tp_reactor_impl) ;

	reactor->open(1);

	MyThread thread_2(&implementer_1, reactor);
	thread_2.activate();

	modifyFAILObjectTest(omHandler, dnObjName);


	implementer_1.reactor()->end_reactor_event_loop();

	sleep(1);



	result = oiHandler.removeObjectImpl(&implementer_1);
	ASSERTM("acs_apgcc_oihandler_V2::removeObjectImpl ", result == ACS_CC_SUCCESS);

	sleep(1);

	cout<<"NAME CLASS: "<<dnObjName<<endl;
	deleteObjectTest(omHandler, dnObjName );
	deleteClassTest(omHandler, nameClass);

	result = omHandler.Finalize();
	ASSERTM("OmHandler::Finalize  ", result == ACS_CC_SUCCESS);

	/**workaround: sul SIMAP3 non mi permette di eliminare l'oggetto al seguito della abort**/
	OmHandler omHandler_2;

	delete reactor;
	delete tp_reactor_impl;


}

void runSuite(){
	cute::suite s;

	s.push_back(CUTE(myImplementerTest_1));

	s.push_back(CUTE(abortCallbackTest));
	s.push_back(CUTE(createDeleteCallbackTest));

	s.push_back(CUTE(classImplTest_1));
	s.push_back(CUTE(classImplTest_2));
	s.push_back(CUTE(classImplTest_3));
	s.push_back(CUTE(classImplTest_4));
	s.push_back(CUTE(classImplTest_5));
	s.push_back(CUTE(classImplTest_6));
	s.push_back(CUTE(classImplTest_7));

	s.push_back(CUTE(objImplTest_1));
	s.push_back(CUTE(objImplTest_2));
	s.push_back(CUTE(objImplTest_3));
	s.push_back(CUTE(objImplTest_4));
	s.push_back(CUTE(objImplTest_5));
	s.push_back(CUTE(objImplTest_6));

	s.push_back(CUTE(ModifyCallbackTest));
/**/

	s.push_back(CUTE(updateCallbackTest));


//	s.push_back(CUTE(testAbort));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}


int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "!!!OiHandler CUTE Test!!!" << endl; // prints !!!Hello World!!!

	runSuite();

	cout << "!!!OiHandler CUTE Test END!!!" << endl;

	return 0;
}
