//============================================================================
// Name        : RuntimeOwnerUT.cpp
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
#include <sys/poll.h>

#include "Objecthandler.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"

using namespace std;


class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(ObjectHandler *p_user1){

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

	ObjectHandler *implementer;

	bool isStop;

};




/*The Name of the runtime class to be defined*/
char* nomeNewClasse_Runtime = const_cast<char*>("TestUT_runtimeClassCashed");
char* nomeNewClass_Runtime_NC = const_cast<char*>("TestUT_runtimeClassNotCashed");

/*The Name of parent name of teh class to be defined*/
char* nomeParent = const_cast<char*>("safApp=safImmService");

/*The dn of the object to instance*/
char* objName = const_cast<char*>("runtimeObj_1,safApp=safImmService");
char* objName_2 = const_cast<char*>("runtimeObj_2,safApp=safImmService");

/*The implementer name of the object*/
std::string implementerName("Implementer_1");
std::string implementerName_2("Implementer_2");


/*******************Name of Attributes *************************************************/

char attrdn[] 				= 	"RDN_Attribute";
char name_attrFloat[] 		= 	"ATTRIBUTE_1";
char name_attrInt64[]		= 	"ATTRIBUTE_2";
char name_attrInt32[] 		= 	"ATTRIBUTE_3";
char name_attrDouble[] 		= 	"ATTRIBUTE_4";
char name_attrUnsInt64[]	= 	"ATTRIBUTE_5";
char name_attrUnsInt32[] 	= 	"ATTRIBUTE_6";
char name_attrSaName[] 		= 	"ATTRIBUTE_7";
char name_attrString[] 		= 	"ATTRIBUTE_8";
char name_attrTimeT[]		=	"ATTRIBUTE_9";


/***************************************************************************************/

/*******************Values for Attributes *************************************************/

char* rdnValue = const_cast<char *>("runtimeObj_1");
char* rdnValue_2 = const_cast<char *>("runtimeObj_2");

float floatValue_1 = 12.25;
float floatValue_2 = 35.25;

long long int int64Value = -998877;

int int32Value = -112233;

double doubleValue_1 = 11.33;
double doubleValue_2 = 33.44;

unsigned long unsInt64Value = 998877;

unsigned int unsInt32Value = 332211;

long long int timeValue = SA_TIME_ONE_MICROSECOND;

char* saNameValue_1 = const_cast<char *>("provaSaName_1");
char* saNameValue_2 = const_cast<char *>("provaSaName_2");

char* stringValue_1 = const_cast<char *>("provaString_1");
char* stringValue_2 = const_cast<char *>("provaString_2");




/***************************************************************************************/


/*******************New values for Attributes *************************************************/

float new_floatValue_1 = 25.15;
float new_floatValue_2 = 45.3;

long long int new_int64Value = -665544;

int new_int32Value = -123;

double new_doubleValue_1 = 22;
double new_doubleValue_2 = 11.22;

unsigned long long new_unsInt64Value = 654321;

unsigned int new_unsInt32Value = 112233;

long long int new_timeValue = SA_TIME_ONE_HOUR;

char* new_saNameValue_1 = const_cast<char *>("new_provaSaName_1");
char* new_saNameValue_2 = const_cast<char *>("new_provaSaName_2");

char* new_stringValue_1 = const_cast<char *>("new_provaString_1");
char* new_stringValue_2 = const_cast<char *>("new_provaString_2");


/***************************************************************************************/


void createRuntimeClassCached(){

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDefined = nomeNewClasse_Runtime;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = RUNTIME;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;

	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_RUNTIME|ATTR_CACHED,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("ATTRIBUTE_1"), ATTR_FLOATT, ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("ATTRIBUTE_4"),ATTR_DOUBLET,ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("ATTRIBUTE_7"), ATTR_NAMET, ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("ATTRIBUTE_8"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTRIBUTE_9"), ATTR_TIMET,ATTR_RUNTIME|ATTR_CACHED,0};

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

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
	}

	result=immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: defineClass FAILURE!!!\n";
	}else{
		cout<<"Class "<<nameClassToBeDefined<<" defined.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: finalize FAILURE!!!\n";
	}


}


void createRuntimeClassNotCached(){

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDefined = nomeNewClass_Runtime_NC;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = RUNTIME;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;

	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_RUNTIME|ATTR_CACHED,0} ;
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char*>("ATTRIBUTE_1"), ATTR_FLOATT, ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char*>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char*>("ATTRIBUTE_4"),ATTR_DOUBLET,ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char*>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char*>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char*>("ATTRIBUTE_7"), ATTR_NAMET, ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("ATTRIBUTE_8"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
//	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTR_TIME"), ATTR_TIMET,ATTR_RUNTIME|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnI64);
//	classAttributes.push_back(attributeTIME);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeSTRING);

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
	}

	result=immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: defineClass FAILURE!!!\n";
	}else{
		cout<<"Class "<<nameClassToBeDefined<<" defined.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: finalize FAILURE!!!\n";
	}


}


void deleteRuntimeClass(const char* nameClassToDelete){
	ACS_CC_ReturnType result;
	OmHandler immHandler;

	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
		cout << "ERROR CODE: "<< immHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << immHandler.getInternalLastErrorText() << endl;
	}

	result = immHandler.removeClass(const_cast<char*>(nameClassToDelete));
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: removeClass FAILURE!!!\n";
	}else{
		cout<<"Class "<<nameClassToDelete<<" deleted.\n";
	}

	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: finalize FAILURE!!!\n";
		cout << "ERROR CODE: "<< immHandler.getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << immHandler.getInternalLastErrorText() << endl;
	}
}


/*This test call the methotd init() of class ACS_APGCC_RuntimeOwner to perform
 * initialization of IMM interaction*/
void initMethodTest(ObjectHandler *obj, std::string implName){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " INIT_METHOD_TEST(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::init(...)   " << endl;
	cout << " RESULT: Initialize the IMM connection and registers the invoking process . \n"
		 <<	" \t as Object implementer having the name which is specified in the  \n"
		 << " \t implementerName parameter \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	returnCode = obj->init(implName);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:init method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Initialization completed \n");
	}else{
		printf(" --> Initialization failed\n");
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);
}


/**This method return with ACS_CC_FAILURE because try to register an Object Implementer with
 * a name that is already used for another object implementer registered with the IMM service**/
void initiMethodTest_FAIL(ObjectHandler *obj, std::string implName){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " INITI_METHOD_TEST_FAIL(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::init(...)   " << endl;
	cout << " RESULT: The initialization of IMM connection failed because an Object implementer \n"
		 <<	" \t with the same name is alreadyregistered with the IMM service \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	returnCode = obj->init(implName);

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:init method ", returnCode == ACS_CC_FAILURE);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Initialization completed \n");
	}else{
		printf(" --> Initialization failed\n");
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}


/*This method call the method Finalize() of class ACS_APGCC_RuntimeOwner to clear
 * the implementer name associated with immHandle and then release the handle */
void finalizeMethodTest(ObjectHandler *obj){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " FINALIZE_METHOD_TEST(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::finalize(...)   " << endl;
	cout << " RESULT: The method clear the implementer name associated with the immOiHandle and \n"
		 <<	" \t unregisters the invoking process as an Object Implementer. \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	returnCode = obj->finalize();

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:finaize method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Finalize completed \n");
	}else{
		printf(" --> Finalize failed \n");
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);
}


/*This method call the method Finalize() of class ACS_APGCC_RuntimeOwner to clear
 * the implementer name associated with immHandle and then release the handle.
 * The method return with error  ACS_CC_FAILURE because called without the method init*/
void finalizeMethodTestFAIL(ObjectHandler *obj){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " FINALIZE_METHOD_TEST_FAIL(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::finalize(...)   " << endl;
	cout << " RESULT: the finalize failed because the handle passed as parameter is uninitialized \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	returnCode = obj->finalize();
	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Finalize completed \n");
	}else{
		printf(" --> Finalize failed \n");
		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
	}

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:finaize method ", returnCode == ACS_CC_FAILURE);

//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Finalize completed \n");
//	}else{
//		printf(" --> Finalize failed \n");
//	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;


	sleep(1);
}


/*This test instances the class above defined and provide values for all attribute*/
void CreateRuntimeObjectCached(ObjectHandler *obj) {

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " CREATE_RUNTIME_OBJECT_CACHED(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::createRuntimeObj(...)   " << endl;
	cout << " RESULT: create a runtime object with cached attribute in IMM structure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
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
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = valueRDN;


	/*Fill the attribute ATTRIBUTE_1*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType =ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 2;
	void* valueFLOAT[2] = {reinterpret_cast<void*>(&floatValue_1), reinterpret_cast<void*>(&floatValue_2) };
	attributeFLOAT.attrValues = valueFLOAT;


	/*Fill the attribute ATTRIBUTE_2*/
	attributeI64.attrName = name_attrInt64;
	attributeI64.attrType =ATTR_INT64T;
	attributeI64.attrValuesNum = 1;
	void* valuesInt64[1] = {reinterpret_cast<void*>(&int64Value)};
	attributeI64.attrValues = valuesInt64;


	/*Fill the attribute ATTRIBUTE_3*/
	attributeI32.attrName = name_attrInt32;
	attributeI32.attrType =ATTR_INT32T;
	attributeI32.attrValuesNum = 1;
	void* valuesInt32[1] = {reinterpret_cast<void*>(&int32Value)};
	attributeI32.attrValues = valuesInt32;


	/*Fill the attribute ATTRIBUTE_4*/
	attributeDOUBLE.attrName = name_attrDouble;
	attributeDOUBLE.attrType =ATTR_DOUBLET;
	attributeDOUBLE.attrValuesNum = 1;
	void* valuesDouble[2] = {reinterpret_cast<void*>(&doubleValue_1), reinterpret_cast<void*>(&doubleValue_2) };
	attributeDOUBLE.attrValues = valuesDouble;


	/*Fill the attribute ATTRIBUTE_5*/
	attributeUnI64.attrName = name_attrUnsInt64;
	attributeUnI64.attrType =ATTR_UINT64T;
	attributeUnI64.attrValuesNum = 1;
	void* valuesUnsInt64[1] = {reinterpret_cast<void*>(&unsInt64Value) };
	attributeUnI64.attrValues = valuesUnsInt64;


	/*Fill the attribute ATTRIBUTE_6*/
	attributeUnI32.attrName = name_attrUnsInt32;
	attributeUnI32.attrType =ATTR_UINT32T;
	attributeUnI32.attrValuesNum = 1;
	void* valuesUnsInt32[1] = {reinterpret_cast<void*>(&unsInt32Value) };
	attributeUnI32.attrValues = valuesUnsInt32;


	/*Fill the attribute ATTRIBUTE_7*/
	attributeNAME.attrName = name_attrSaName;
	attributeNAME.attrType =ATTR_NAMET;
	attributeNAME.attrValuesNum = 2;
	void* valueNameT[2]={reinterpret_cast<void*>(saNameValue_1), reinterpret_cast<void*>(saNameValue_2)};
	attributeNAME.attrValues = valueNameT;



	/*Fill the attribute ATTRIBUTE_8*/
	attributeSTRING.attrName = name_attrString;
	attributeSTRING.attrType =ATTR_STRINGT;
	attributeSTRING.attrValuesNum = 2;
	void* valueString[2]={reinterpret_cast<void*>(stringValue_1), reinterpret_cast<void*>(stringValue_2)};
	attributeSTRING.attrValues = valueString;

	/*Fill the attribute ATTRIBUTE_9*/
	attributeTIME.attrName = name_attrTimeT;
	attributeTIME.attrType =ATTR_TIMET;
	attributeTIME.attrValuesNum = 1;
	void* valueTime[1]={reinterpret_cast<void*>(&timeValue)};
	attributeTIME.attrValues = valueTime;

	/*Add the atributes to vector*/
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

	returnCode = obj->createRuntimeObj(nomeNewClasse_Runtime, nomeParent, AttrList);

	/*Create the class*/
	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:createRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Creation object %s completed\n", rdnValue);
	}else{
		printf(" --> Creation object %s failed \n", rdnValue);
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}


/*This test try to instances an object already instantiated previously*/
void CreateRuntimeObject_FAIL(ObjectHandler *obj){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " CREATE_RUNTIME_OBJECT_FAIL(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::createRuntimeObj(...)   " << endl;
	cout << " RESULT: the method createRuntimeObj return with error because an object with the same \n"
			" \tname is already instantiated" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;



	ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	/*the attributes*/
	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeFLOAT;

	/*Fill the rdn Attribute */
	char attrdn[]= "RDN_Attribute";
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = valueRDN;


	/*Fill the attribute ATTRIBUTE_1*/
	char name_attrFloat[]="ATTRIBUTE_1";
	attributeFLOAT.attrName = name_attrFloat;
	AttrList.push_back(attributeFLOAT);
	attributeFLOAT.attrType =ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 2;
	void* valueFLOAT[2] = {reinterpret_cast<void*>(&floatValue_1), reinterpret_cast<void*>(&floatValue_2) };
	attributeFLOAT.attrValues = valueFLOAT;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);

	returnCode = obj->createRuntimeObj(nomeNewClasse_Runtime, nomeParent, AttrList);
	if(returnCode == ACS_CC_SUCCESS){
		printf("object %s,%s created\n", attrdn, nomeParent);
	}else{
		printf("creation object %s,%s failed\n", attrdn, nomeParent);
		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
	}

	/*Create the class*/
	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:createRuntimeObj method ", returnCode == ACS_CC_FAILURE);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Creation object %s completed\n", rdnValue);
	}else{
		printf(" --> Creation object %s failed \n", rdnValue);
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}




/*This test instances the class above defined and provide values for all attribute*/
void CreateRuntimeObjectNotCached(ObjectHandler *obj) {

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " CREATE_RUNTIMEOBJECT_NOT_CACHED(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::createRuntimeObj(...)   " << endl;
	cout << " RESULT: create a runtime object with not cached attribute in IMM structure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
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


	/*Fill the rdn Attribute */
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_NAMET;
	attributeRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue_2)};
	attributeRDN.attrValues = valueRDN;


	/*Fill the attribute ATTRIBUTE_1*/
	attributeFLOAT.attrName = name_attrFloat;
	attributeFLOAT.attrType =ATTR_FLOATT;
	attributeFLOAT.attrValuesNum = 0;
	attributeFLOAT.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_2*/
	attributeI64.attrName = name_attrInt64;
	attributeI64.attrType =ATTR_INT64T;
	attributeI64.attrValuesNum = 0;
	attributeI64.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_3*/
	attributeI32.attrName = name_attrInt32;
	attributeI32.attrType =ATTR_INT32T;
	attributeI32.attrValuesNum = 0;
	attributeI32.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_4*/
	attributeDOUBLE.attrName = name_attrDouble;
	attributeDOUBLE.attrType =ATTR_DOUBLET;
	attributeDOUBLE.attrValuesNum = 0;
	attributeDOUBLE.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_5*/
	attributeUnI64.attrName = name_attrUnsInt64;
	attributeUnI64.attrType =ATTR_UINT64T;
	attributeUnI64.attrValuesNum = 0;
	attributeUnI64.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_6*/
	attributeUnI32.attrName = name_attrUnsInt32;
	attributeUnI32.attrType =ATTR_UINT32T;
	attributeUnI32.attrValuesNum = 0;
	attributeUnI32.attrValues = 0;


	/*Fill the attribute ATTRIBUTE_7*/
	attributeNAME.attrName = name_attrSaName;
	attributeNAME.attrType =ATTR_NAMET;
	attributeNAME.attrValuesNum = 0;
	attributeNAME.attrValues = 0;



	/*Fill the attribute ATTRIBUTE_8*/
	attributeSTRING.attrName = name_attrString;
	attributeSTRING.attrType =ATTR_STRINGT;
	attributeSTRING.attrValuesNum = 0;
	attributeSTRING.attrValues = 0;

	/*Add the atributes to vector*/
	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeFLOAT);
	AttrList.push_back(attributeI64);
	AttrList.push_back(attributeI32);
	AttrList.push_back(attributeDOUBLE);
	AttrList.push_back(attributeUnI64);
	AttrList.push_back(attributeUnI32);
	AttrList.push_back(attributeNAME);
	AttrList.push_back(attributeSTRING);

	returnCode = obj->createRuntimeObj(nomeNewClass_Runtime_NC, nomeParent, AttrList);

	/*Create the class*/
	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:createRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Creation object %s completed\n", rdnValue);
	}else{
		printf(" --> Creation object %s failed \n", rdnValue);
	}

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}




void modifyAttribute_Test(ObjectHandler *obj){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " MODIFY_ATTRIBUTE_TEST(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::modifyRuntimeObj(...)   " << endl;
	cout << " RESULT: modify the value of a attribute belonging to a runtime object.  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	OmHandler omHand;

	MyThread th(obj);

	th.activate();

	returnCode = omHand.Init();

	/*Set a new value for ATTRIBUTE_1*/
	ACS_CC_ImmParameter parToModify_1;
	parToModify_1.attrName = name_attrFloat;
	parToModify_1.attrType = ATTR_FLOATT;
	parToModify_1.attrValuesNum = 2;
	void* new_valueFLOAT[2] = {reinterpret_cast<void*>(&new_floatValue_1), reinterpret_cast<void*>(&new_floatValue_2) };
	parToModify_1.attrValues = new_valueFLOAT;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_1);

	//ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrFloat);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrFloat);
	}

	parToModify_1.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_1;
	paramAfterChange_1.attrName = name_attrFloat;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_1 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_1.attrValuesNum; i++){
			cout<<"Float value: "<<*(float*)paramAfterChange_1.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_2*/
	ACS_CC_ImmParameter parToModify_2;
	parToModify_2.attrName = name_attrInt64;
	parToModify_2.attrType = ATTR_INT64T;
	parToModify_2.attrValuesNum = 1;
	void* new_valuesInt64[1] = {reinterpret_cast<void*>(&new_int64Value)};
	parToModify_2.attrValues = new_valuesInt64;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_2);

	//ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Modifica parametro %s (cached) eseguita\n", name_attrInt64);
	}else{
		printf("Modifica parametro %s (cached) fallita\n", name_attrInt64);
	}

	parToModify_2.attrValues = 0;


	ACS_CC_ImmParameter paramAfterChange_2;
	paramAfterChange_2.attrName = name_attrInt64;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_2 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_2.attrValuesNum; i++){
			cout<<"Int64 value: "<<*(long long int*)paramAfterChange_2.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	/*Set a new value for ATTRIBUTE_3*/
	ACS_CC_ImmParameter parToModify_3;
	parToModify_3.attrName = name_attrInt32;
	parToModify_3.attrType = ATTR_INT32T;
	parToModify_3.attrValuesNum = 1;
	void* new_valuesInt32[1] = {reinterpret_cast<void*>(&new_int32Value)};
	parToModify_3.attrValues = new_valuesInt32;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_3);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Modifica parametro %s (cached) eseguita\n", name_attrInt32);
	}else{
		printf("Modifica parametro %s (cached) fallita\n", name_attrInt32);
	}


	parToModify_3.attrValues = 0;


	ACS_CC_ImmParameter paramAfterChange_3;
	paramAfterChange_3.attrName = name_attrInt32;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_3 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_3.attrValuesNum; i++){
			cout<<"Int32 value: "<<*(int*)paramAfterChange_3.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	/*Set a new value for ATTRIBUTE_4*/
	ACS_CC_ImmParameter parToModify_4;
	parToModify_4.attrName = name_attrDouble;
	parToModify_4.attrType = ATTR_DOUBLET;
	parToModify_4.attrValuesNum = 2;
	void* new_valuesDouble[2] = {reinterpret_cast<void*>(&new_doubleValue_1), reinterpret_cast<void*>(&new_doubleValue_2) };
	parToModify_4.attrValues = new_valuesDouble;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_4);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrDouble);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrDouble);
	}

	parToModify_4.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_4;
	paramAfterChange_4.attrName = name_attrDouble;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_4 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_4.attrValuesNum; i++){
			cout<<"Double value: "<<*(double*)paramAfterChange_4.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_5*/
	ACS_CC_ImmParameter parToModify_5;
	parToModify_5.attrName = name_attrUnsInt64;
	parToModify_5.attrType = ATTR_UINT64T;
	parToModify_5.attrValuesNum = 1;
	void* new_valuesUnsInt64[1] = {reinterpret_cast<void*>(&new_unsInt64Value) };
	parToModify_5.attrValues = new_valuesUnsInt64;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_5);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrUnsInt64);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrUnsInt64);
	}

	parToModify_5.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_5;
	paramAfterChange_5.attrName = name_attrUnsInt64;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_5 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_5.attrValuesNum; i++){
			cout<<"UNSINT64 value: "<<*(unsigned long long*)paramAfterChange_5.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_6*/
	ACS_CC_ImmParameter parToModify_6;
	parToModify_6.attrName = name_attrUnsInt32;
	parToModify_6.attrType = ATTR_UINT32T;
	parToModify_6.attrValuesNum = 1;
	void* new_valuesUnsInt32[1] = {reinterpret_cast<void*>(&new_unsInt32Value) };
	parToModify_6.attrValues = new_valuesUnsInt32;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_6);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrUnsInt64);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrUnsInt64);
	}

	parToModify_6.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_6;
	paramAfterChange_6.attrName = name_attrUnsInt32;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_6 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_6.attrValuesNum; i++){
			cout<<"UNSINT32 value: "<<*(unsigned int*)paramAfterChange_6.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_7*/
	ACS_CC_ImmParameter parToModify_7;
	parToModify_7.attrName = name_attrSaName;
	parToModify_7.attrType = ATTR_NAMET;
	parToModify_7.attrValuesNum = 2;
	void* new_valueNameT[2]={reinterpret_cast<void*>(new_saNameValue_1), reinterpret_cast<void*>(new_saNameValue_2)};
	parToModify_7.attrValues = new_valueNameT;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_7);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrSaName);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrSaName);
	}

	parToModify_7.attrValues = 0;


	ACS_CC_ImmParameter paramAfterChange_7;
	paramAfterChange_7.attrName = name_attrSaName;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_7 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_7.attrValuesNum; i++){
			cout<<"SaNameT value: "<<(char *)paramAfterChange_7.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	/*Set a new value for ATTRIBUTE_8*/
	ACS_CC_ImmParameter parToModify_8;
	parToModify_8.attrName = name_attrString;
	parToModify_8.attrType = ATTR_STRINGT;
	parToModify_8.attrValuesNum = 2;
	void* new_valueString[2]={reinterpret_cast<void*>(stringValue_1), reinterpret_cast<void*>(stringValue_2)};
	parToModify_8.attrValues = new_valueString;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_8);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Modifica parametro %s (cached) eseguita\n", name_attrString);
	}else{
		printf("Modifica parametro %s (cached) fallita\n", name_attrString);
	}

	parToModify_8.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_8;
	paramAfterChange_8.attrName = name_attrString;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_8 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_8.attrValuesNum; i++){
			cout<<"String value: "<<(char *)paramAfterChange_8.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	/*Set a new value for ATTRIBUTE_9*/
	ACS_CC_ImmParameter parToModify_9;
	parToModify_9.attrName = name_attrTimeT;
	parToModify_9.attrType = ATTR_TIMET;
	parToModify_9.attrValuesNum = 1;
	void* new_valueTimeT[1]={reinterpret_cast<void*>(&new_timeValue)};
	parToModify_9.attrValues = new_valueTimeT;

	returnCode = obj->modifyRuntimeObj(objName, &parToModify_9);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrTimeT);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrTimeT);
	}

	parToModify_9.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_9;
	paramAfterChange_9.attrName = name_attrTimeT;
	returnCode = omHand.getAttribute(objName, &paramAfterChange_9 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_9.attrValuesNum; i++){
			cout<<"Time value: "<<*(long long int *)paramAfterChange_9.attrValues[i]<<endl;
		}

	}


	th.stop();

	cout<<endl;

	returnCode = omHand.Finalize();

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

	return;

}


void modifyNotCachedAttribute_Test(ObjectHandler *obj){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " MODIFY_NOT_CACHED_ATTRIBUTE_TEST(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::modifyRuntimeObj(...)   " << endl;
	cout << " RESULT: modify the value of an not cached attribute belonging to a runtime object.  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType returnCode;

	OmHandler omHand;

	returnCode = omHand.Init();

	MyThread th(obj);

	th.activate();

	/*Set a new value for ATTRIBUTE_1*/
	ACS_CC_ImmParameter parToModify_1;
	parToModify_1.attrName = name_attrFloat;
	parToModify_1.attrType = ATTR_FLOATT;
	parToModify_1.attrValuesNum = 2;
	void* new_valueFLOAT[2] = {reinterpret_cast<void*>(&new_floatValue_1), reinterpret_cast<void*>(&new_floatValue_2) };
	parToModify_1.attrValues = new_valueFLOAT;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_1);

	//ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrFloat);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrFloat);
	}

	parToModify_1.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_1;
	paramAfterChange_1.attrName = name_attrFloat;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_1 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_1.attrValuesNum; i++){
			cout<<"Float value: "<<*(float*)paramAfterChange_1.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_2*/
	ACS_CC_ImmParameter parToModify_2;
	parToModify_2.attrName = name_attrInt64;
	parToModify_2.attrType = ATTR_INT64T;
	parToModify_2.attrValuesNum = 1;
	void* new_valuesInt64[1] = {reinterpret_cast<void*>(&new_int64Value)};
	parToModify_2.attrValues = new_valuesInt64;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_2);

	//ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrInt64);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrInt64);
	}

	parToModify_2.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_2;
	paramAfterChange_2.attrName = name_attrInt64;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_2 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_2.attrValuesNum; i++){
			cout<<"Int64 value: "<<*(long long int*)paramAfterChange_2.attrValues[i]<<endl;
		}

	}


	cout<<endl;



	/*Set a new value for ATTRIBUTE_3*/
	ACS_CC_ImmParameter parToModify_3;
	parToModify_3.attrName = name_attrInt32;
	parToModify_3.attrType = ATTR_INT32T;
	parToModify_3.attrValuesNum = 1;
	void* new_valuesInt32[1] = {reinterpret_cast<void*>(&new_int32Value)};
	parToModify_3.attrValues = new_valuesInt32;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_3);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrInt32);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrInt32);
	}

	parToModify_3.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_3;
	paramAfterChange_3.attrName = name_attrInt32;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_3 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_3.attrValuesNum; i++){
			cout<<"Int32 value: "<<*(int*)paramAfterChange_3.attrValues[i]<<endl;
		}

	}

	cout<<endl;




	/*Set a new value for ATTRIBUTE_4*/
	ACS_CC_ImmParameter parToModify_4;
	parToModify_4.attrName = name_attrDouble;
	parToModify_4.attrType = ATTR_DOUBLET;
	parToModify_4.attrValuesNum = 2;
	void* new_valuesDouble[2] = {reinterpret_cast<void*>(&new_doubleValue_1), reinterpret_cast<void*>(&new_doubleValue_2) };
	parToModify_4.attrValues = new_valuesDouble;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_4);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrDouble);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrDouble);
	}

	parToModify_4.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_4;
	paramAfterChange_4.attrName = name_attrDouble;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_4 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_4.attrValuesNum; i++){
			cout<<"Double value: "<<*(double*)paramAfterChange_4.attrValues[i]<<endl;
		}

	}

	cout<<endl;




	/*Set a new value for ATTRIBUTE_5*/
	ACS_CC_ImmParameter parToModify_5;
	parToModify_5.attrName = name_attrUnsInt64;
	parToModify_5.attrType = ATTR_UINT64T;
	parToModify_5.attrValuesNum = 1;
	void* new_valuesUnsInt64[1] = {reinterpret_cast<void*>(&new_unsInt64Value) };
	parToModify_5.attrValues = new_valuesUnsInt64;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_5);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrUnsInt64);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrUnsInt64);
	}

	parToModify_5.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_5;
	paramAfterChange_5.attrName = name_attrUnsInt64;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_5 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_5.attrValuesNum; i++){
			cout<<"UNSINT64 value: "<<*(unsigned long long*)paramAfterChange_5.attrValues[i]<<endl;
		}

	}

	cout<<endl;




	/*Set a new value for ATTRIBUTE_6*/
	ACS_CC_ImmParameter parToModify_6;
	parToModify_6.attrName = name_attrUnsInt32;
	parToModify_6.attrType = ATTR_UINT32T;
	parToModify_6.attrValuesNum = 1;
	void* new_valuesUnsInt32[1] = {reinterpret_cast<void*>(&new_unsInt32Value) };
	parToModify_6.attrValues = new_valuesUnsInt32;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_6);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrUnsInt64);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrUnsInt64);
	}

	parToModify_6.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_6;
	paramAfterChange_6.attrName = name_attrUnsInt32;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_6 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_6.attrValuesNum; i++){
			cout<<"UNSINT32 value: "<<*(unsigned int*)paramAfterChange_6.attrValues[i]<<endl;
		}

	}

	cout<<endl;



	/*Set a new value for ATTRIBUTE_7*/
	ACS_CC_ImmParameter parToModify_7;
	parToModify_7.attrName = name_attrSaName;
	parToModify_7.attrType = ATTR_NAMET;
	parToModify_7.attrValuesNum = 2;
	void* new_valueNameT[2]={reinterpret_cast<void*>(new_saNameValue_1), reinterpret_cast<void*>(new_saNameValue_2)};
	parToModify_7.attrValues = new_valueNameT;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_7);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrSaName);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrSaName);
	}

	parToModify_7.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_7;
	paramAfterChange_7.attrName = name_attrSaName;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_7 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_7.attrValuesNum; i++){
			cout<<"SaNameT value: "<<(char *)paramAfterChange_7.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	/*Set a new value for ATTRIBUTE_8*/
	ACS_CC_ImmParameter parToModify_8;
	parToModify_8.attrName = name_attrString;
	parToModify_8.attrType = ATTR_STRINGT;
	parToModify_8.attrValuesNum = 2;
	void* new_valueString[2]={reinterpret_cast<void*>(stringValue_1), reinterpret_cast<void*>(stringValue_2)};
	parToModify_8.attrValues = new_valueString;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_8);

//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrString);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrString);
	}

	parToModify_8.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_8;
	paramAfterChange_8.attrName = name_attrString;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_8 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_8.attrValuesNum; i++){
			cout<<"String value: "<<(char *)paramAfterChange_8.attrValues[i]<<endl;
		}

	}

	cout<<endl;


	returnCode = omHand.Finalize();

	th.stop();


	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}


void modifyAttribute_FAIL(ObjectHandler *obj){


	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " MODIFY_ATTRIBUTE_FAIL(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::modifyRuntimeObj(...)   " << endl;
	cout << " RESULT: modify the value of an wrong attribute  \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;



	ACS_CC_ReturnType returnCode;

	OmHandler omHand;

	MyThread th(obj);

	th.activate();

	returnCode = omHand.Init();

	/*Set a new value for ATTRIBUTE_1*/
	ACS_CC_ImmParameter parToModify_1;
	parToModify_1.attrName = name_attrFloat;
	parToModify_1.attrType = ATTR_INT32T;
	parToModify_1.attrValuesNum = 2;
	void* new_valueFLOAT[2] = {reinterpret_cast<void*>(&new_floatValue_1), reinterpret_cast<void*>(&new_floatValue_2) };
	parToModify_1.attrValues = new_valueFLOAT;

	returnCode = obj->modifyRuntimeObj(objName_2, &parToModify_1);
	if(returnCode == ACS_CC_SUCCESS){
		printf("object %s modified\n", objName_2 );
	}else{
		printf("modify object %s failed\n", objName_2);
		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
	}

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:modifyRuntimeObj method ", returnCode == ACS_CC_FAILURE);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> Modifica parametro %s (cached) eseguita\n", name_attrFloat);
	}else{
		printf(" --> Modifica parametro %s (cached) fallita\n", name_attrFloat);
	}


	parToModify_1.attrValues = 0;

	ACS_CC_ImmParameter paramAfterChange_1;
	paramAfterChange_1.attrName = name_attrFloat;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_1 );

	if( returnCode == ACS_CC_SUCCESS ){
		for(unsigned int i=0; i<paramAfterChange_1.attrValuesNum; i++){
			cout<<"Float value: "<<*(float*)paramAfterChange_1.attrValues[i]<<endl;
		}

	}


	cout<<endl;

	returnCode = omHand.Finalize();

	th.stop();

	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}




void deleteObject(ObjectHandler *obj, const char *objName){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " DELETE_OBJECT(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::deleteRuntimeObj(...)   " << endl;
	cout << " RESULT: delete a runtime object in IMM structure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;

	ACS_CC_ReturnType returnCode;

	returnCode = obj->deleteRuntimeObj(const_cast<char*>(objName));

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:deleteRuntimeObj method ", returnCode == ACS_CC_SUCCESS);

	if(returnCode == ACS_CC_SUCCESS){
		printf(" --> cancellation of object %s completed\n", objName);
	}else{
		printf(" --> cancellation of object %s failed\n", objName);
	}
	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}


void deleteObject_FAIL(ObjectHandler *obj, const char *objName){

	cout << " -----------------------------------------------------------------------------------" << endl;
	cout << " DELETE_OBJECT_FAIL(...)" << endl;
	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::deleteRuntimeObj(...)   " << endl;
	cout << " RESULT: the method failed because try to delete a runtime object that not exist \n"
			" \tin IMM structure \n" << endl;
	cout << " ----------------------------------------------------------------------------------- " << endl;


	ACS_CC_ReturnType returnCode;

	returnCode = obj->deleteRuntimeObj(const_cast<char*>(objName));
	if(returnCode == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName );
	}else{
		printf("creation object %s failed\n", objName);
		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
	}

	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:deleteRuntimeObj method ", returnCode == ACS_CC_FAILURE);

//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> cancellation of object %s completed\n", objName);
//	}else{
//		printf(" --> cancellation of object %s failed\n", objName);
//	}
	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);

}


void dispatchTest_FAIL(){

	ObjectHandler obj_2;

	initMethodTest(&obj_2, implementerName_2);

	MyThread th(&obj_2);

	th.activate();

	sleep(1);

	finalizeMethodTest(&obj_2);

	th.stop();
	cout << " ----------------------------------------------------------------------------------- " << endl;

	cout<<endl;
	cout<<endl;

	sleep(1);
}




void myTest(){

	ObjectHandler obj;
	ObjectHandler obj_2;

	initMethodTest(&obj, implementerName);
//	initiMethodTest_FAIL(&obj_2, implementerName);


	CreateRuntimeObjectCached(&obj);
	CreateRuntimeObject_FAIL(&obj);

	CreateRuntimeObjectNotCached(&obj);

	modifyAttribute_Test(&obj);
	modifyNotCachedAttribute_Test(&obj);
	modifyAttribute_FAIL(&obj);

	deleteObject(&obj, objName);
	deleteObject(&obj, objName_2);
	deleteObject_FAIL(&obj, const_cast<char*>("runtimeObj_22,safApp=safImmService"));
//	dispatchTest_FAIL();

	finalizeMethodTest(&obj);
	finalizeMethodTestFAIL(&obj);

}


void runSuite(){
	cute::suite s;


	s.push_back(CUTE(myTest));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}


int ACE_TMAIN (int, ACE_TCHAR *[]) {

	createRuntimeClassCached();
	createRuntimeClassNotCached();

	runSuite();

	deleteRuntimeClass(nomeNewClasse_Runtime);
	deleteRuntimeClass(nomeNewClass_Runtime_NC);

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	return 0;
}
