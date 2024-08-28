/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>

//#include "ACS_APGCC_OiHandler.h"
#include "acs_apgcc_oihandler_V2.h"


#include "acs_apgcc_omhandler.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"
#include <sys/poll.h>

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
	//ASSERTM("OmHandler::defineClass ", result == ACS_CC_SUCCESS);
	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR!!! Class " << p_className << " NOT created errCode= " << omHandler.getInternalLastError() << endl;

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
		cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created errCode= " << omHandler_NO_OI.getInternalLastError()<< endl;
		cout << endl;
		return -1;
	}

}
/***************end crea oggetto*/

/**************1 modify object*/
int modifyObjectTest1(/*OmHandler omHandler, */const char *p_objName){
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


	/*Set a new value for ATT_STRING*/
//	char str1[30]="stringTest_555";
//	ACS_CC_ImmParameter par2;
//	par2.attrName=const_cast<char*>("ATTRIBUTE_8");
//	par2.attrType=ATTR_STRINGT;
//	par2.attrValuesNum=1;
//	par2.attrValues=new void*[par2.attrValuesNum];
//	par2.attrValues[0] =reinterpret_cast<void*>(str1);


	/*Set a new value for ATT_NAME*/
//	char str3[30]="SaNameString_555";
//	ACS_CC_ImmParameter par3;
//	par3.attrName=const_cast<char*>("ATTRIBUTE_7");
//	par3.attrType=ATTR_NAMET;
//	par3.attrValuesNum=1;
//	par3.attrValues=new void*[par3.attrValuesNum];
//	par3.attrValues[0] =reinterpret_cast<void*>(str3);



	if ((result = omHandler_NO_OI.modifyAttribute(p_objName, &par) )!=ACS_CC_SUCCESS){
		cout << " ERROR modify par1" << endl;
		cout << endl;
		return -1;
	}
//	if ((result = omHandler.modifyAttribute(p_objName, &par2) )!=ACS_CC_SUCCESS){
//		cout << " ERROR modify par2" << endl;
//		cout << endl;
//		return -1;
//	}
//	if ((result = omHandler.modifyAttribute(p_objName, &par3) )!=ACS_CC_SUCCESS){
//		cout << " ERROR modify par3" << endl;
//		cout << endl;
//		return -1;
//	}
	return 0;


}
/**************end modify object*/

/**************2 modify object*/
int modifyObjectTest2(/*OmHandler omHandler, */const char *p_objName){
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
	//char nameClassNO[20] = "Test_configNO";
	//const char *objNameCreate = "provaConfObj1";
	int res;
	res = createClassTest(omHandler, nameClass);

	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_config ok "<<endl;
		}else
		cout<<"PrepareEnvironment:createClassTest:Test_config ko "<<endl;
	res = createClassTest(omHandler, nameClass2);
	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_config2 ok "<<endl;
		}else
		cout<<"PrepareEnvironment:createClassTest:Test_config2 ko "<<endl;

}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "TEST STARTING" << endl;
	ACS_CC_ReturnType result;
	int res;
	char choice;

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

	const char *objNameCreate5_1 = "CreatedbyprovaConfObj5";
	const char *dnObjName5_1 = "CreatedbyprovaConfObj5";

	const char *objNameCreateTrans = "objNameCreateTransactional";
	const char *dnObjNameCreateTrans = "objNameCreateTransactional,apzFunctionsId=1,managedElementId=1";

	const char *objNameCreateTrans2 = "objNameCreateTransactional2";
	const char *dnObjNameCreateTrans2 = "objNameCreateTransactional2,apzFunctionsId=1,managedElementId=1";

	result = omHandler_NO_OI.Init(NO_REGISTERED_OI);

	cout << "Calling Prepare Env................" << endl;
	PrepareEnvironment();

/****************Create obj 1*/
	std::cout << " - insert a char to create an obj objNameCreate1"<<endl;
	std::cout << " - As a consequence a child will be created"<<endl;
	std::cin >> choice;

	res = createObjectTest(nameClass, objNameCreate1);

	if (res != 0)
	{
		std::cout<<"objNameCreate1 :: FAILURE"<<std::endl;
		std::cout<<"... getting data"<<std::endl;
		ACS_CC_ReturnType resultgeterror;
		int intVal;
		std::string strinVal;
		resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
		if(resultgeterror != ACS_CC_SUCCESS){
			std::cout<<"... getting data:FAILURE!!!"<<std::endl;
		}else{
			std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
			std::cout<<"... intVal= "<<intVal<<std::endl;
			std::cout<<"... strinVal= "<<strinVal<<std::endl;
			std::cout<<"-----------------------------------------------------"<<std::endl;
		}

	}
	else
		std::cout<<"objNameCreate1 :: SUCCESS"<<std::endl;

/****************Create obj 2 EXpected FAILURE*/
		std::cout << " - insert  char:A to create an obj objNameCreate2"<<endl;
		std::cin >> choice;

		if (choice == 'A'){
		res = createObjectTest(nameClass, objNameCreate2);

		if (res != 0){
			std::cout<<"objNameCreate2 :: FAILURE"<<std::endl;
			std::cout<<"... getting data"<<std::endl;
			ACS_CC_ReturnType resultgeterror;
			int intVal;
			std::string strinVal;
			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
			if(resultgeterror != ACS_CC_SUCCESS){
				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
			}else{
				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
				std::cout<<"... intVal= "<<intVal<<std::endl;
				std::cout<<"... strinVal= "<<strinVal<<std::endl;
				std::cout<<"-------------------------------------------------"<<std::endl;
			}

		}
		else
			std::cout<<"objNameCreate2 :: SUCCESS"<<std::endl;
		}




/****************Create obj 3*/
		std::cout << " - insert a char:A to create an obj objNameCreate3"<<endl;
		std::cout << " - As a consequence the value of ATTRIBUTE_1 willl be set  to 11111.222"<<endl;
		std::cin >> choice;
		if (choice == 'A'){
			res = createObjectTest(/*omHandler_NO_OI, */nameClass, objNameCreate3);

			if (res != 0)
			{
				std::cout<<"objNameCreate3 :: FAILURE"<<std::endl;
				std::cout<<"... getting data"<<std::endl;
				ACS_CC_ReturnType resultgeterror;
				int intVal;
				std::string strinVal;
				resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
				if(resultgeterror != ACS_CC_SUCCESS){
					std::cout<<"... getting data:FAILURE!!!"<<std::endl;
				}else{
					std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
					std::cout<<"... intVal= "<<intVal<<std::endl;
					std::cout<<"... strinVal= "<<strinVal<<std::endl;
					std::cout<<"-------------------------------------------------"<<std::endl;
				}

			}
			else
				std::cout<<"objNameCreate3 :: SUCCESS"<<std::endl;

		}
/****************Create obj 4*/

		std::cout << " - insert a char:A to create an obj objNameCreate4";
		std::cout << " - As a consequence an instance of obj2 will be deleted caution!!!!"<<endl;
		std::cin >> choice;
		if (choice == 'A'){
			res = createObjectTest(nameClass, objNameCreate4);

			if (res != 0)
			{
				std::cout<<"objNameCreate4 :: FAILURE"<<std::endl;
				std::cout<<"... getting data"<<std::endl;
				ACS_CC_ReturnType resultgeterror;
				int intVal;
				std::string strinVal;
				resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
				if(resultgeterror != ACS_CC_SUCCESS){
					std::cout<<"... getting data:FAILURE!!!"<<std::endl;
				}else{
					std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
					std::cout<<"... intVal= "<<intVal<<std::endl;
					std::cout<<"... strinVal= "<<strinVal<<std::endl;
					std::cout<<"-------------------------------------------------"<<std::endl;
				}

			}
			else
				std::cout<<"objNameCreate4 :: SUCCESS"<<std::endl;

		}

/****************Create obj 5*/
	std::cout << " - insert A char to create an obj objNameCreate5"<<endl;
	std::cout << " - As a consequence a child will be created : "
			<<"CreatedbyprovaConfObj5"<<endl;
	std::cin >> choice;
	if (choice == 'A'){
		res = createObjectTest(nameClass, objNameCreate5);

		if (res != 0)
		{
			std::cout<<"objNameCreate5 :: FAILURE"<<std::endl;
			std::cout<<"... getting data"<<std::endl;
			ACS_CC_ReturnType resultgeterror;
			int intVal;
			std::string strinVal;
			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
			if(resultgeterror != ACS_CC_SUCCESS){
				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
			}else{
				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
				std::cout<<"... intVal= "<<intVal<<std::endl;
				std::cout<<"... strinVal= "<<strinVal<<std::endl;
				std::cout<<"-----------------------------------------------------"<<std::endl;
			}

		}
		else
			std::cout<<"objNameCreate5 :: SUCCESS"<<std::endl;
	}

	/****************Create obj 6*/
		std::cout << " - insert A char to create an obj objNameCreate6"<<endl;
		std::cout << " - As a consequence a child will fail and the obj6 not created : "<<endl;
		std::cin >> choice;
		if (choice == 'A'){
			res = createObjectTest(nameClass, objNameCreate6);

			if (res != 0)
			{
				std::cout<<"objNameCreate6 :: FAILURE"<<std::endl;
				std::cout<<"... getting data"<<std::endl;
//				ACS_CC_ReturnType resultgeterror;
//				int intVal;
//				std::string strinVal;
//				resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//				if(resultgeterror != ACS_CC_SUCCESS){
//					std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//				}else{
//					std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//					std::cout<<"... intVal= "<<intVal<<std::endl;
//					std::cout<<"... strinVal= "<<strinVal<<std::endl;
//					std::cout<<"-----------------------------------------------------"<<std::endl;
//				}

			}
			else
				std::cout<<"objNameCreate5 :: SUCCESS"<<std::endl;
		}
/****************modify obj 1 con valore < 100*/
		std::cout << " - insert a char:A to Modify  objNameCreate1 with value < 100"<<endl;
		std::cout << " - As a consequence the CreatedbyprovaConfObj5 will be modified"<<endl;
		std::cout << " - in ATTRIBUTE_1"<<endl;
		std::cin >> choice;
		if (choice == 'A'){
			res = modifyObjectTest1( dnObjName1);

			if (res != 0)
			{
				std::cout<<"Modify  objNameCreate1:: FAILURE"<<std::endl;
				std::cout<<"... getting data"<<std::endl;
				ACS_CC_ReturnType resultgeterror;
				int intVal;
				std::string strinVal;
				resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
				if(resultgeterror != ACS_CC_SUCCESS){
					std::cout<<"... getting data:FAILURE!!!"<<std::endl;
				}else{
					std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
					std::cout<<"... intVal= "<<intVal<<std::endl;
					std::cout<<"... strinVal= "<<strinVal<<std::endl;
					std::cout<<"-------------------------------------------------"<<std::endl;
				}

			}
			else
				std::cout<<"Modify  objNameCreate1:: SUCCESS"<<std::endl;

		}
///****************modify obj 2 con valore > 100*/
//		std::cout << " - insert a char:A to Modify  objNameCreate1 with value > 100";
//		std::cin >> choice;
//		if (choice == 'A'){
//			res = modifyObjectTest2( dnObjName1);
//
//			if (res != 0)
//			{
//				std::cout<<"Modify  objNameCreate2:: FAILURE"<<std::endl;
//				std::cout<<"... getting data"<<std::endl;
//				ACS_CC_ReturnType resultgeterror;
//				int intVal;
//				std::string strinVal;
//				resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//				if(resultgeterror != ACS_CC_SUCCESS){
//					std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//				}else{
//					std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//					std::cout<<"... intVal= "<<intVal<<std::endl;
//					std::cout<<"... strinVal= "<<strinVal<<std::endl;
//					std::cout<<"-------------------------------------------------"<<std::endl;
//				}
//
//			}
//			else
//				std::cout<<"Modify  objNameCreate1:: SUCCESS"<<std::endl;
//
//		}

///****************Delete obj 1*/
//
//	std::cout << " - insert a char Y to Delete an obj objNameCreate1";
//	std::cin >> choice;
//	if (choice == 'Y'){
//		result = omHandler_NO_OI.deleteObject(dnObjName1);
//
//		if (result != ACS_CC_SUCCESS)
//		{
//			std::cout<<"objNameDelete1 :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//				std::cout<<"-------------------------------------------------"<<std::endl;
//			}
//		}
//		else
//			std::cout<<"objNameDelete1 :: SUCCESS"<<std::endl;
//
//	}
///****************Delete obj 2*/
//
//	std::cout << " - insert a char Y to Delete an obj objNameCreate2";
//	std::cin >> choice;
//
//	if (choice == 'Y'){
//
//		result = omHandler_NO_OI.deleteObject(dnObjName2);
//
//		if (result != ACS_CC_SUCCESS)
//		{
//			std::cout<<"objNameDelete2 :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//				std::cout<<"-------------------------------------------------"<<std::endl;
//			}
//		}
//		else
//			std::cout<<"objNameDelete2 :: SUCCESS"<<std::endl;
//	}
//
///****************Delete obj 3*/
//
//	std::cout << " - insert a char Y to Delete an obj objNameCreate3";
//	std::cin >> choice;
//
//	if (choice == 'Y'){
//
//		result = omHandler_NO_OI.deleteObject(dnObjName3);
//
//		if (result != ACS_CC_SUCCESS)
//		{
//			std::cout<<"objNameDelete3 :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//				std::cout<<"-------------------------------------------------"<<std::endl;
//			}
//		}
//		else
//			std::cout<<"objNameDelete3 :: SUCCESS"<<std::endl;
//	}
///****************Delete obj 4*/
//
//	std::cout << " - insert a char Y to Delete an obj objNameCreate4";
//	std::cin >> choice;
//
//	if (choice == 'Y'){
//
//		result = omHandler_NO_OI.deleteObject(dnObjName4);
//
//		if (result != ACS_CC_SUCCESS)
//		{
//			std::cout<<"objNameDelete4 :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//				std::cout<<"-------------------------------------------------"<<std::endl;
//			}
//		}
//		else
//			std::cout<<"objNameDelete4 :: SUCCESS"<<std::endl;
//	}
/****************Delete obj 5*/

	std::cout << " - insert a char Y to Delete an obj objNameCreate5";
	std::cin >> choice;

	if (choice == 'Y'){

		result = omHandler_NO_OI.deleteObject(dnObjName5);

		if (result != ACS_CC_SUCCESS)
		{
			std::cout<<"objNameDelete5 :: FAILURE"<<std::endl;
			std::cout<<"... getting data"<<std::endl;
			ACS_CC_ReturnType resultgeterror;
			int intVal;
			std::string strinVal;
			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
			if(resultgeterror != ACS_CC_SUCCESS){
				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
			}else{
				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
				std::cout<<"... intVal= "<<intVal<<std::endl;
				std::cout<<"... strinVal= "<<strinVal<<std::endl;
				std::cout<<"-------------------------------------------------"<<std::endl;
			}
		}
		else
			std::cout<<"objNameDelete5 :: SUCCESS"<<std::endl;
	}


//	/****************-------------transactional Operations---------------------------------*/
//	std::cout << " - insert a char Y to start Transactional Operations 1:::::";
//	std::cin >> choice;
//	if (choice =='Y'){
//
//		std::cout << " - insert a char to create an obj objNameCreateTransactional";
//		std::cin >> choice;
//
//		res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans);
//
//		if (res != 0)
//		{
//			std::cout<<"dnObjNameCreateTrans :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data:SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//				std::cout<<"-----------------------------------------------------"<<std::endl;
//			}
//
//		}
//		else
//			std::cout<<"objNameCreateTransactional :: SUCCESS"<<std::endl;
//
//		std::cout << " - insert  char:Y to create an obj objNameCreate2";
//		std::cin >> choice;
//
//		if (choice == 'Y'){
//		res = createObjectTestTrans("transaction1", nameClass, objNameCreate2);
///*expected failure*/
//		if (res != 0){
//			std::cout<<"objNameCreate2 :: FAILURE"<<std::endl;
//			std::cout<<"... getting data"<<std::endl;
//			ACS_CC_ReturnType resultgeterror;
//			int intVal;
//			std::string strinVal;
//			/*get return code*/
//			resultgeterror = omHandler_NO_OI.getExitCode("transaction1",intVal, strinVal);
//			if(resultgeterror != ACS_CC_SUCCESS){
//				std::cout<<"... getting data for transaction Method:FAILURE!!!"<<std::endl;
//			}else{
//				std::cout<<std::endl<<std::endl<<"... getting data for Transaction Method :SUCCESS!!!"<<std::endl;
//				std::cout<<"... intVal= "<<intVal<<std::endl;
//				std::cout<<"... strinVal= "<<strinVal<<std::endl;
//
//				/*apply request calling ....*/
//				resultgeterror = omHandler_NO_OI.resetRequest("transaction1");
//				if(resultgeterror == ACS_CC_SUCCESS){
//					std::cout<<"... omHandler_NO_OI.resetRequest SUCCESS!!!"<<std::endl;
//				}else{
//					std::cout<<"... .. omHandler_NO_OI.resetRequest:FAILURE!!!"<<std::endl;
//
//
//				}
//
//
//				std::cout<<"-------------------------------------------------"<<std::endl;
//			}
//
//		}
//		else
//			std::cout<<"objNameCreate2 :: SUCCESS"<<std::endl;
//		}
//
//
//	}
//
///*******- 2 - ******/
//	std::cout << " - insert a char Y to start Transactional Operations 2:::::";
//		std::cin >> choice;
//		if (choice =='Y'){
//
//		std::cout << " - 2 - insert a char to create an obj objNameCreateTrans2";
//				std::cin >> choice;
//
//				res = createObjectTestTrans("transaction1",nameClass, objNameCreateTrans2);
//
//				if (res != 0)
//				{
//					std::cout<<"- 2 - objNameCreateTrans2 :: FAILURE"<<std::endl;
//					std::cout<<"- 2 - ... getting data"<<std::endl;
//					ACS_CC_ReturnType resultgeterror;
//					int intVal;
//					std::string strinVal;
//					resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//					if(resultgeterror != ACS_CC_SUCCESS){
//						std::cout<<"- 2 - ... getting data:FAILURE!!!"<<std::endl;
//					}else{
//						std::cout<<std::endl<<std::endl<<"- 2 - ... getting data:SUCCESS!!!"<<std::endl;
//						std::cout<<"- 2 -... intVal= "<<intVal<<std::endl;
//						std::cout<<"- 2 -... strinVal= "<<strinVal<<std::endl;
//						std::cout<<"-----------------------------------------------------"<<std::endl;
//					}
//
//				}
//				else
//					std::cout<<"- 2 - objNameCreateTransactional :: SUCCESS"<<std::endl;
//
//				/****************Delete obj 1*/
//
//				std::cout << " - insert a char Y to create an obj objNameCreate3";
//				std::cin >> choice;
//				if (choice == 'Y'){
//					/****************Create obj 3*/
//						res = createObjectTestTrans("transaction1", nameClass, objNameCreate3);
//
//						if (res != 0)
//						{
//							std::cout<<"- 2 - objNameCreate3 :: FAILURE"<<std::endl;
//							std::cout<<"- 2 - ... getting data"<<std::endl;
//							ACS_CC_ReturnType resultgeterror;
//							int intVal;
//							std::string strinVal;
//							resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//							if(resultgeterror != ACS_CC_SUCCESS){
//								std::cout<<"- 2 -... getting data:FAILURE!!!"<<std::endl;
//							}else{
//								std::cout<<std::endl<<std::endl<<"- 2 -... getting data:SUCCESS!!!"<<std::endl;
//								std::cout<<"- 2 -... intVal= "<<intVal<<std::endl;
//								std::cout<<"- 2 -... strinVal= "<<strinVal<<std::endl;
//								std::cout<<"-------------------------------------------------"<<std::endl;
//							}
//
//						}
//						else{
//							std::cout<<"- 2 - objNameCreate3 :: SUCCESS"<<std::endl;
//							std::cout<<"- 2 - now applying transaction"<<std::endl;
//
//							ACS_CC_ReturnType resultgeterror;
//							resultgeterror = omHandler_NO_OI.applyRequest("transaction1");
//
//							int intVal;
//							std::string strinVal;
//							if(resultgeterror == ACS_CC_SUCCESS){
//								std::cout<<"- 2 - ... omHandler_NO_OI.applyRequest SUCCESS!!!"<<std::endl;
//							}else{
//								std::cout<<"- 2 -... .. omHandler_NO_OI.applyRequest:FAILURE!!!"<<std::endl;
//								resultgeterror = omHandler_NO_OI.getExitCode(intVal, strinVal);
//								if(resultgeterror != ACS_CC_SUCCESS){
//									std::cout<<"- 2 - ... getting data:FAILURE!!!"<<std::endl;
//								}else{
//									std::cout<<std::endl<<std::endl<<"- 2 - ... getting data:SUCCESS!!!"<<std::endl;
//									std::cout<<"- 2 -... intVal= "<<intVal<<std::endl;
//									std::cout<<"- 2 -... strinVal= "<<strinVal<<std::endl;
//									std::cout<<"-----------------------------------------------------"<<std::endl;
//								}
//
//							}
//
//						}
//
//
//				}
//
//
//
//		}



	omHandler_NO_OI.Finalize();	
	cout << "!!!EST STARTING!! END!!!" << endl;

	return 0;
}
