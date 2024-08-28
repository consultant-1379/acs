//============================================================================
// Name        : RuntimeOwnerUT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>


#include "ace/Task.h"
#include <sys/poll.h>

#include "Objecthandler.h"

#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperation.h"

using namespace std;


class MyThread : public ACE_Task_Base {
public :

	/**Constructor**/
	MyThread(ObjectHandler *p_user1){

		implementer = p_user1;

		isStop = false;
	}


	/**Destructor**/
	~MyThread(){ }


	/**This method is used to stop the thread**/
	void stop(){

		isStop=true;
	}


	/**svc method **/
	int svc(void){

		int ret;

		//cout<<"getSelObj: "<<implementer->getSelObj()<<endl;

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

				//cout<<"FD THREAD: "<<fds[0].fd<<endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			}
		}

		//cout<<"Thread TERMINATO"<<endl;

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


///*This test call the methotd init() of class ACS_APGCC_RuntimeOwner to perform
// * initialization of IMM interaction*/
//void initMethodTest(ObjectHandler *obj, std::string implName){
//
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " INIT_METHOD_TEST(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::init(...)   " << endl;
//	cout << " RESULT: Initialize the IMM connection and registers the invoking process . \n"
//		 <<	" \t as Object implementer having the name which is specified in the  \n"
//		 << " \t implementerName parameter \n" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	ACS_CC_ReturnType returnCode;
//
//	returnCode = obj->init(implName);
//
//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:init method ", returnCode == ACS_CC_SUCCESS);
//
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Initialization completed \n");
//	}else{
//		printf(" --> Initialization failed\n");
//	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//	sleep(1);
//}
//
//
///**This method return with ACS_CC_FAILURE because try to register an Object Implementer with
// * a name that is already used for another object implementer registered with the IMM service**/
//void initiMethodTest_FAIL(ObjectHandler *obj, std::string implName){
//
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " INITI_METHOD_TEST_FAIL(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::init(...)   " << endl;
//	cout << " RESULT: The initialization of IMM connection failed because an Object implementer \n"
//		 <<	" \t with the same name is alreadyregistered with the IMM service \n" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	ACS_CC_ReturnType returnCode;
//
//	returnCode = obj->init(implName);
//
//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:init method ", returnCode == ACS_CC_FAILURE);
//
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Initialization completed \n");
//	}else{
//		printf(" --> Initialization failed\n");
//	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//	sleep(1);
//
//}
//
//
///*This method call the method Finalize() of class ACS_APGCC_RuntimeOwner to clear
// * the implementer name associated with immHandle and then release the handle */
//void finalizeMethodTest(ObjectHandler *obj){
//
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " FINALIZE_METHOD_TEST(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::finalize(...)   " << endl;
//	cout << " RESULT: The method clear the implementer name associated with the immOiHandle and \n"
//		 <<	" \t unregisters the invoking process as an Object Implementer. \n" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	ACS_CC_ReturnType returnCode;
//
//	returnCode = obj->finalize();
//
//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:finaize method ", returnCode == ACS_CC_SUCCESS);
//
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Finalize completed \n");
//	}else{
//		printf(" --> Finalize failed \n");
//	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//	sleep(1);
//}
//
//
///*This method call the method Finalize() of class ACS_APGCC_RuntimeOwner to clear
// * the implementer name associated with immHandle and then release the handle.
// * The method return with error  ACS_CC_FAILURE because called without the method init*/
//void finalizeMethodTestFAIL(ObjectHandler *obj){
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " FINALIZE_METHOD_TEST_FAIL(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::finalize(...)   " << endl;
//	cout << " RESULT: the finalize failed because the handle passed as parameter is uninitialized \n" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	ACS_CC_ReturnType returnCode;
//
//	returnCode = obj->finalize();
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Finalize completed \n");
//	}else{
//		printf(" --> Finalize failed \n");
//		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
//		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
//	}
//
//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:finaize method ", returnCode == ACS_CC_FAILURE);
//
////	if(returnCode == ACS_CC_SUCCESS){
////		printf(" --> Finalize completed \n");
////	}else{
////		printf(" --> Finalize failed \n");
////	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//
//	sleep(1);
//}


///*This test instances the class above defined and provide values for all attribute*/
//void CreateRuntimeObjectCached(ObjectHandler *obj) {
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " CREATE_RUNTIME_OBJECT_CACHED(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::createRuntimeObj(...)   " << endl;
//	cout << " RESULT: create a runtime object with cached attribute in IMM structure \n" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//
//	ACS_CC_ReturnType returnCode;
//
//	/*The list of attributes*/
//	vector<ACS_CC_ValuesDefinitionType> AttrList;
//
//	/*the attributes*/
//	ACS_CC_ValuesDefinitionType attributeRDN;
//	ACS_CC_ValuesDefinitionType attributeFLOAT;
//	ACS_CC_ValuesDefinitionType attributeI64;
//	ACS_CC_ValuesDefinitionType attributeI32;
//	ACS_CC_ValuesDefinitionType attributeDOUBLE;
//	ACS_CC_ValuesDefinitionType attributeUnI64;
//	ACS_CC_ValuesDefinitionType attributeUnI32;
//	ACS_CC_ValuesDefinitionType attributeNAME;
//	ACS_CC_ValuesDefinitionType attributeSTRING;
//	ACS_CC_ValuesDefinitionType attributeTIME;
//
//
//	/*Fill the rdn Attribute */
//	attributeRDN.attrName = attrdn;
//	attributeRDN.attrType = ATTR_NAMET;
//	attributeRDN.attrValuesNum = 1;
//	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
//	attributeRDN.attrValues = valueRDN;
//
//
//	/*Fill the attribute ATTRIBUTE_1*/
//	attributeFLOAT.attrName = name_attrFloat;
//	attributeFLOAT.attrType =ATTR_FLOATT;
//	attributeFLOAT.attrValuesNum = 2;
//	void* valueFLOAT[2] = {reinterpret_cast<void*>(&floatValue_1), reinterpret_cast<void*>(&floatValue_2) };
//	attributeFLOAT.attrValues = valueFLOAT;
//
//
//	/*Fill the attribute ATTRIBUTE_2*/
//	attributeI64.attrName = name_attrInt64;
//	attributeI64.attrType =ATTR_INT64T;
//	attributeI64.attrValuesNum = 1;
//	void* valuesInt64[1] = {reinterpret_cast<void*>(&int64Value)};
//	attributeI64.attrValues = valuesInt64;
//
//
//	/*Fill the attribute ATTRIBUTE_3*/
//	attributeI32.attrName = name_attrInt32;
//	attributeI32.attrType =ATTR_INT32T;
//	attributeI32.attrValuesNum = 1;
//	void* valuesInt32[1] = {reinterpret_cast<void*>(&int32Value)};
//	attributeI32.attrValues = valuesInt32;
//
//
//	/*Fill the attribute ATTRIBUTE_4*/
//	attributeDOUBLE.attrName = name_attrDouble;
//	attributeDOUBLE.attrType =ATTR_DOUBLET;
//	attributeDOUBLE.attrValuesNum = 1;
//	void* valuesDouble[2] = {reinterpret_cast<void*>(&doubleValue_1), reinterpret_cast<void*>(&doubleValue_2) };
//	attributeDOUBLE.attrValues = valuesDouble;
//
//
//	/*Fill the attribute ATTRIBUTE_5*/
//	attributeUnI64.attrName = name_attrUnsInt64;
//	attributeUnI64.attrType =ATTR_UINT64T;
//	attributeUnI64.attrValuesNum = 1;
//	void* valuesUnsInt64[1] = {reinterpret_cast<void*>(&unsInt64Value) };
//	attributeUnI64.attrValues = valuesUnsInt64;
//
//
//	/*Fill the attribute ATTRIBUTE_6*/
//	attributeUnI32.attrName = name_attrUnsInt32;
//	attributeUnI32.attrType =ATTR_UINT32T;
//	attributeUnI32.attrValuesNum = 1;
//	void* valuesUnsInt32[1] = {reinterpret_cast<void*>(&unsInt32Value) };
//	attributeUnI32.attrValues = valuesUnsInt32;
//
//
//	/*Fill the attribute ATTRIBUTE_7*/
//	attributeNAME.attrName = name_attrSaName;
//	attributeNAME.attrType =ATTR_NAMET;
//	attributeNAME.attrValuesNum = 2;
//	void* valueNameT[2]={reinterpret_cast<void*>(saNameValue_1), reinterpret_cast<void*>(saNameValue_2)};
//	attributeNAME.attrValues = valueNameT;
//
//
//
//	/*Fill the attribute ATTRIBUTE_8*/
//	attributeSTRING.attrName = name_attrString;
//	attributeSTRING.attrType =ATTR_STRINGT;
//	attributeSTRING.attrValuesNum = 2;
//	void* valueString[2]={reinterpret_cast<void*>(stringValue_1), reinterpret_cast<void*>(stringValue_2)};
//	attributeSTRING.attrValues = valueString;
//
//	/*Fill the attribute ATTRIBUTE_9*/
//	attributeTIME.attrName = name_attrTimeT;
//	attributeTIME.attrType =ATTR_TIMET;
//	attributeTIME.attrValuesNum = 1;
//	void* valueTime[1]={reinterpret_cast<void*>(&timeValue)};
//	attributeTIME.attrValues = valueTime;
//
//	/*Add the atributes to vector*/
//	AttrList.push_back(attributeRDN);
//	AttrList.push_back(attributeFLOAT);
//	AttrList.push_back(attributeI64);
//	AttrList.push_back(attributeI32);
//	AttrList.push_back(attributeDOUBLE);
//	AttrList.push_back(attributeUnI64);
//	AttrList.push_back(attributeUnI32);
//	AttrList.push_back(attributeNAME);
//	AttrList.push_back(attributeSTRING);
//	AttrList.push_back(attributeTIME);
//
//	returnCode = obj->createRuntimeObj(nomeNewClasse_Runtime, nomeParent, AttrList);
//
//	/*Create the class*/
////	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:createRuntimeObj method ", returnCode == ACS_CC_SUCCESS);
//
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Creation object %s completed\n", rdnValue);
//	}else{
//		printf(" --> Creation object %s failed \n", rdnValue);
//	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//	sleep(1);
//
//}

//
///*This test try to instances an object already instantiated previously*/
//void CreateRuntimeObject_FAIL(ObjectHandler *obj){
//
//	cout << " -----------------------------------------------------------------------------------" << endl;
//	cout << " CREATE_RUNTIME_OBJECT_FAIL(...)" << endl;
//	cout << " TEST: lunch the method ACS_APGCC_RuntimeOwner::createRuntimeObj(...)   " << endl;
//	cout << " RESULT: the method createRuntimeObj return with error because an object with the same \n"
//			" \tname is already instantiated" << endl;
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//
//
//	ACS_CC_ReturnType returnCode;
//
//	/*The list of attributes*/
//	vector<ACS_CC_ValuesDefinitionType> AttrList;
//
//	/*the attributes*/
//	ACS_CC_ValuesDefinitionType attributeRDN;
//	ACS_CC_ValuesDefinitionType attributeFLOAT;
//
//	/*Fill the rdn Attribute */
//	char attrdn[]= "RDN_Attribute";
//	attributeRDN.attrName = attrdn;
//	attributeRDN.attrType = ATTR_NAMET;
//	attributeRDN.attrValuesNum = 1;
//	void* valueRDN[1]={reinterpret_cast<void*>(rdnValue)};
//	attributeRDN.attrValues = valueRDN;
//
//
//	/*Fill the attribute ATTRIBUTE_1*/
//	char name_attrFloat[]="ATTRIBUTE_1";
//	attributeFLOAT.attrName = name_attrFloat;
//	AttrList.push_back(attributeFLOAT);
//	attributeFLOAT.attrType =ATTR_FLOATT;
//	attributeFLOAT.attrValuesNum = 2;
//	void* valueFLOAT[2] = {reinterpret_cast<void*>(&floatValue_1), reinterpret_cast<void*>(&floatValue_2) };
//	attributeFLOAT.attrValues = valueFLOAT;
//
//	AttrList.push_back(attributeRDN);
//	AttrList.push_back(attributeFLOAT);
//
//	returnCode = obj->createRuntimeObj(nomeNewClasse_Runtime, nomeParent, AttrList);
//	if(returnCode == ACS_CC_SUCCESS){
//		printf("object %s,%s created\n", attrdn, nomeParent);
//	}else{
//		printf("creation object %s,%s failed\n", attrdn, nomeParent);
//		cout << "ERROR CODE: "<< obj->getInternalLastError() << endl;
//		cout << "ERROR MESSAGE: " << obj->getInternalLastErrorText() << endl;
//	}
//
//	/*Create the class*/
//	ASSERTM("CUTE: ACS_APGCC_RuntimeOwner:createRuntimeObj method ", returnCode == ACS_CC_FAILURE);
//
//	if(returnCode == ACS_CC_SUCCESS){
//		printf(" --> Creation object %s completed\n", rdnValue);
//	}else{
//		printf(" --> Creation object %s failed \n", rdnValue);
//	}
//
//	cout << " ----------------------------------------------------------------------------------- " << endl;
//
//	cout<<endl;
//	cout<<endl;
//
//	sleep(1);
//
//}




/*This test instances the class above defined and provide values for all attribute*/
void CreateRuntimeObjectNotCached(ObjectHandler *obj) {


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


	if(returnCode == ACS_CC_SUCCESS){
		printf("object %s created\n", rdnValue);
	}else{
		printf("object %s not created \n", rdnValue);
	}

	cout<<endl;

	sleep(1);

}




void deleteObject(ObjectHandler *obj, const char *objName){


	ACS_CC_ReturnType returnCode;

	returnCode = obj->deleteRuntimeObj(const_cast<char*>(objName));

	if(returnCode == ACS_CC_SUCCESS){
		printf("object %s deleted\n", objName);
	}else{
		printf("object %s not deleted\n", objName);
	}

	cout<<endl;

	sleep(1);

}


int ACE_TMAIN (int, ACE_TCHAR *[]) {

	createRuntimeClassNotCached();

	ObjectHandler obj;
	ACS_CC_ReturnType returnCode;

	returnCode = obj.init(implementerName);

	if(returnCode == ACS_CC_SUCCESS){
		printf("Initialization Runtime Owner completed \n");
	}else{
		printf("Initialization Runtime Owner failed\n");
		cout<<"ERROR CODE: "<<obj.getInternalLastError()<<endl;
	}

	CreateRuntimeObjectNotCached(&obj);


	MyThread th(&obj);

	th.activate();

	sleep(1);

	OmHandler omHand;

	returnCode = omHand.Init();
	if(returnCode != ACS_CC_SUCCESS){
		printf("Initialization OmHandler failed\n");
		cout<<"ERROR CODE: "<<omHand.getInternalLastError()<<endl;
	}

	ACS_CC_ImmParameter paramAfterChange_1;
	paramAfterChange_1.attrName = name_attrFloat;
	returnCode = omHand.getAttribute(objName_2, &paramAfterChange_1 );

	if(returnCode != ACS_CC_SUCCESS){
		printf("getattribute failed\n");
		cout<<"ERROR CODE: "<<omHand.getInternalLastError()<<endl;
	}

	returnCode = omHand.Finalize();
	if(returnCode != ACS_CC_SUCCESS){
		printf("Finalization OmHandler failed\n");
		cout<<"ERROR CODE: "<<omHand.getInternalLastError()<<endl;
	}

	printf(" --> \nPress a key to terminate...\n");
	getchar();

	th.stop();

	sleep(1);

	deleteObject(&obj, objName_2);

	returnCode = obj.finalize();

	if(returnCode == ACS_CC_SUCCESS){
		printf("De-registration of Runtime Owner completed \n");
	}else{
		printf("De-registration of Runtime Owner failed \n");
		cout<<"ERROR CODE: "<<obj.getInternalLastError()<<endl;
	}

	//runSuite();



	deleteRuntimeClass(nomeNewClass_Runtime_NC);

	cout << "!!!End!!!" << endl;

	return 0;
}
