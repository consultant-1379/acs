/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>

#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"



OmHandler omHandler_NO_OI;

//----------------------------------------------------------------------------------------------------------------------------------//
/********************************************************crea classe Result**********************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int createClassResult(OmHandler omHandler, const char *p_className){

	cout << " -> Try to create a new class " << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char*>("operationResult"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char*>("operationName"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};


	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
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



//----------------------------------------------------------------------------------------------------------------------------------//
/********************************************************crea classe Test************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
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
	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;

		return 0;
	}else {
		cout << " -> ERROR!!! Class " << p_className << " NOT created errCode = " << omHandler.getInternalLastError() << endl;
		cout << endl;

		return -1;
	}

}



//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************create Object Test************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
int createObjectTest(const char* p_className, const char *p_objName){


	cout << " -> Try to create a new object " << p_objName << endl;
	cout << endl ;

	ACS_CC_ReturnType result;

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
	char* stringValue = const_cast<char*>("test");
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
		cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created errCode= " << omHandler_NO_OI.getInternalLastError()<< endl;
		cout << endl;
		return -1;
	}

}




//----------------------------------------------------------------------------------------------------------------------------------//
/*****************************************************PrepareEnvironment************************************************************/
//----------------------------------------------------------------------------------------------------------------------------------//
void PrepareEnvironment(){

	OmHandler omHandler;
	ACS_CC_ReturnType result;

	result = omHandler.Init();

	/**create a class **/
	char nameClass[20] = "Test_config";


	int res;
	res = createClassTest(omHandler, nameClass);

	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_config ok "<<endl;
	} else {
		cout<<"PrepareEnvironment:createClassTest:Test_config ko "<<endl;
	}

	char nameClassResult[20] = "Test_Result";
	res = createClassResult(omHandler, nameClassResult);
	if(res == 0){
		cout<<"PrepareEnvironment:createClassTest:Test_Result ok "<<endl;
	}else{
		cout<<"PrepareEnvironment:createClassTest:Test_Result ko "<<endl;
	}
}



//--------------------------------------------------------------------------------------------------------------//
/*************************************MAIN***********************************************************************/
//--------------------------------------------------------------------------------------------------------------//
int main() {

	ACS_CC_ReturnType result;

	int res;
	char choice;
	char nameClass[20] = "Test_config";


	const char *objNameCreate1 = "object1";
	const char *dnObjName1 = "object1,safApp=safImmService";

	acs_apgcc_adminoperation admOp;

	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;

	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;


	std::cout << " - insert:"<<std::endl;
	std::cout << " - 1 To prepare environment"<<std::endl;
	std::cout << " - 2 To start demo"<<std::endl;
	std::cin >> choice;

	/*create first Elemenet of param list*/
	char attName1[]= "par1";

	firstElem.attrName = attName1;
	firstElem.attrType = ATTR_INT32T;
	int intValue1 = 18;
	firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

	/*create second Elemenet of param list*/
	char attName2[]= "par2";

	secondElem.attrName = attName2;
	secondElem.attrType = ATTR_INT32T ;
	unsigned int intValue2 = 2;
	secondElem.attrValues = reinterpret_cast<void*>(&intValue2);


	/*insert parameter into the vector*/
	vectorIN.push_back(firstElem);
	vectorIN.push_back(secondElem);


	switch(choice){

	case '1':
	{
		cout << " 1 To prepare environment" <<endl;
		result = omHandler_NO_OI.Init(NO_REGISTERED_OI);

		cout << "Calling Prepare Env................" << endl;
		PrepareEnvironment();

		res = createObjectTest(nameClass, objNameCreate1);

		if (res != 0)
		{
			std::cout<<"objNameCreate1 :: FAILURE"<<std::endl;
		}
		else
			std::cout<<"objNameCreate1 :: SUCCESS"<<std::endl;

		omHandler_NO_OI.Finalize();
		return 0;
	}
	break;

	case '2':
	{
		cout << " 2 start demo" <<endl;
		result = admOp.init();

		if (result != ACS_CC_SUCCESS){
			std::cout <<"ERROR ::admOp.init()FAILED"<< std::endl;
			std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
			return -1;
		}

		std::cout <<"admOp.init:: SUCCESS"<< std::endl;

		int returnValue;


		long long int timeOutVal_5sec = 5*(1000000000LL);
		long long int timeOutVal_30sec = 30*(1000000000LL);

		std::cout <<"Insert Y to invoke admin operation with id = 1 on: "<< dnObjName1 << std::endl;
		cin>>choice;

		if (choice == 'Y'){
			std::cout <<  std::endl;
			std::cout <<"Operation SUM."<< std::endl;
			std::cout <<"Parameter passed as argument: "<< std::endl;
			std::cout <<attName1<<": "<<intValue1<<std::endl;
			std::cout <<attName2<<": "<<intValue2<<std::endl;

			result = admOp.adminOperationInvoke(dnObjName1, 0, 1, vectorIN, &returnValue, timeOutVal_5sec);

			if (result != ACS_CC_SUCCESS){
				std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
				return -1;
			}

			//std::cout <<"SUCCESS::admOp.adminOperationInvoke"<< std::endl;
			std::cout <<"SUCCESS:: Invoke result: "<< returnValue <<  std::endl;

		}
		std::cout <<endl<<endl<<"Insert Y to invoke admin operation with id = 3 on: "<< dnObjName1 << std::endl;
		cin>>choice;

		if (choice == 'Y'){
			std::cout <<  std::endl;
			std::cout <<"Operation MUL."<< std::endl;
			std::cout <<"Parameter passed as argument: "<< std::endl;
			std::cout <<attName1<<": "<<intValue1<<std::endl;
			std::cout <<attName2<<": "<<intValue2<<std::endl;

			result = admOp.adminOperationInvoke(dnObjName1, 0, 3, vectorIN, &returnValue, timeOutVal_5sec);

			if (result != ACS_CC_SUCCESS){
				std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
				return -1;
			}
			//std::cout <<"SUCCESS::admOp.adminOperationInvoke"<< std::endl;
			std::cout <<"SUCCESS:: Invoke result: "<< returnValue <<  std::endl;

		}

		std::cout <<endl<<endl<<"Insert Y to invoke admin operation with id = 5 on: "<< dnObjName1<<" and timeout 30 sec"<< std::endl;
		cin>>choice;

		if (choice == 'Y'){
			vectorIN.clear();
			result = admOp.adminOperationInvoke(dnObjName1, 0, 5, vectorIN, &returnValue, timeOutVal_30sec);
			if (result != ACS_CC_SUCCESS){
				std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
				return -1;
			}
			//std::cout <<"SUCCESS::admOp.adminOperationInvoke"<< std::endl;
			std::cout <<"SUCCESS:: Invoke result :"<< returnValue <<  std::endl;

		}

		std::cout <<endl<<endl<<"Insert Y to invoke admin operation with id = 5 on: "<< dnObjName1<<" and timeout 5 sec"<< std::endl;
		std::cout <<"(expected failure because of timeout)"<< std::endl;
		cin>>choice;

		if (choice == 'Y'){
			vectorIN.clear();
			result = admOp.adminOperationInvoke(dnObjName1, 0, 5, vectorIN, &returnValue, timeOutVal_5sec);

			if (result != ACS_CC_SUCCESS){
				std::cout <<"ERROR ::admOp.adminOperationInvoke() FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<<std::endl;
				std::cout <<"ERROR MESSAGE = " << admOp.getInternalLastErrorText() <<std::endl;

			}else{
				std::cout <<"SUCCESS::admOp.adminOperationInvoke"<< std::endl;
				std::cout <<"SUCCESS:: Invoke result :"<< returnValue <<  std::endl;

			}

		}

		std::cout <<endl<<endl<<"Insert Y to invoke admin operation on: "<< dnObjName1<<" id = 6 and timeout 5 sec "<< std::endl;
		std::cout <<"(expected failure and result no_op)"<< std::endl;
		cin>>choice;

		if (choice == 'Y'){

			result = admOp.adminOperationInvoke(dnObjName1, 0, 6, vectorIN, &returnValue, timeOutVal_5sec);

			if (result != ACS_CC_SUCCESS){
				std::cout <<"ERROR ::admOp.adminOperationInvoke()FAILED"<< std::endl;
				std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<<std::endl;
				std::cout <<"ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
				return -1;
			}
			std::cout <<"SUCCESS::admOp.adminOperationInvoke"<< std::endl;
			std::cout <<"SUCCESS:: Invoke result :"<< returnValue <<  std::endl;

		}

		cout << "!!!EST STARTING!! END!!!" << endl;

		result = admOp.finalize();
		if (result != ACS_CC_SUCCESS){
			std::cout <<"ERROR ::admOp.finalize()FAILED"<< std::endl;
			std::cout <<"ERROR CODE = "<< admOp.getInternalLastError()<< std::endl << "ERROR MESSAGE = " << admOp.getInternalLastErrorText()<<std::endl;
			return -1;
		}

		return 0;

	}
		break;
	default:
	{
		cout << " OPTION NOT RECOGNISED" <<  endl;
		return -1;
	}
	break;

	}/*end switch*/



	return 0;

}
