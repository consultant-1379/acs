/*
 * NewInitMethodUT_Main.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xpaomaz
 */
#include <iostream>
#include <string>


#include "ACS_CC_Types.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"



int createClassTest(OmHandler omHandler, string p_className){

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
//	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char*>("ATTRIBUTE_9"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeUnI64);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeSTRING);

	result = omHandler.defineClass( const_cast<char*>(p_className.c_str()),categoryClassToBeDefined,classAttributes);

	//ASSERTM("OmHandler::defineClass ", result == ACS_CC_SUCCESS);
	if( result == ACS_CC_SUCCESS ){
		cout << " -> Class " << p_className << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR!!! Class " << p_className << " NOT created" << endl;
		cout << omHandler.getInternalLastError() << endl;
		return -1;
	}

}

int createObjectTest(/*OmHandler omHandler,*/ string p_className, string p_objName){


	cout << " -> Try to create a new object " << p_objName << endl;
	cout << endl ;

	ACS_CC_ReturnType result;
	OmHandler omHandler_NO_OI;

	omHandler_NO_OI.Init();


	//char nameClass[20] = "Test_config";
	char* nomeParent = const_cast<char *>("safApp=safImmService");

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr1;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;
	ACS_CC_ValuesDefinitionType attr5;
	ACS_CC_ValuesDefinitionType attr6;
	ACS_CC_ValuesDefinitionType attr7;
	ACS_CC_ValuesDefinitionType attr8;

	char attrdn[]= "RDN_Attribute";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_NAMET;
	attrRdn.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(const_cast<char *>(p_objName.c_str()))};
	attrRdn.attrValues = valueRDN;

	char name_attFloat[]="ATTRIBUTE_1";
	attr1.attrName = name_attFloat;
	attr1.attrType = ATTR_FLOATT;
	attr1.attrValuesNum = 1;
	float floatValue = 456.78;
	void* float_attr[1] = {reinterpret_cast<void*>(&floatValue)};
	attr1.attrValues = float_attr;

	char name_attInt64[]="ATTRIBUTE_2";
	attr2.attrName = name_attInt64;
	attr2.attrType = ATTR_INT64T;
	attr2.attrValuesNum = 1;
	long long intValue64 = -123456789;
	void* int64_attr[1] = {reinterpret_cast<void*>(&intValue64)};
	attr2.attrValues = int64_attr;

	char name_attInt32[]= "ATTRIBUTE_3";
	attr3.attrName = name_attInt32;
	attr3.attrType = ATTR_INT32T;
	attr3.attrValuesNum = 1;
	int intValue32 = -12345;
	void* int32_attr[1] = {reinterpret_cast<void*>(&intValue32)};
	attr3.attrValues = int32_attr;

	char name_attDouble[]= "ATTRIBUTE_4";
	attr4.attrName = name_attDouble;
	attr4.attrType = ATTR_DOUBLET;
	attr4.attrValuesNum = 1;
	double doubleValue = 112233.445566;
	void* double_attr[1] = {reinterpret_cast<void*>(&doubleValue)};
	attr4.attrValues = double_attr;

	char name_attUInt64[]= "ATTRIBUTE_5";
	attr5.attrName = name_attUInt64;
	attr5.attrType = ATTR_UINT64T;
	attr5.attrValuesNum = 1;
	unsigned long long uintValue64 = 1122334455;
	void* uint64_attr[1] = {reinterpret_cast<void*>(&uintValue64)};
	attr5.attrValues = uint64_attr;

	char name_attUInt32[]= "ATTRIBUTE_6";
	attr6.attrName = name_attUInt32;
	attr6.attrType = ATTR_UINT32T;
	attr6.attrValuesNum = 1;
	unsigned int uintValue32 = 12345;
	void* uint32_attr[1] = {reinterpret_cast<void*>(&uintValue32)};
	attr6.attrValues = uint32_attr;

	/*Fill the attribute ATTRIBUTE_7*/
	char name_attrSaName[]= "ATTRIBUTE_7";
	attr7.attrName = name_attrSaName;
	attr7.attrType =ATTR_NAMET;
	attr7.attrValuesNum = 1;
	void* valueNameT[1]={reinterpret_cast<void*>(const_cast<char *>("This is a SA_NAMET"))};
	attr7.attrValues = valueNameT;

	/*Fill the attribute ATTRIBUTE_8*/
	char name_attrString[]= "ATTRIBUTE_8";
	attr8.attrName = name_attrString;
	attr8.attrType =ATTR_STRINGT;
	attr8.attrValuesNum = 1;
	void* valueString[1]={reinterpret_cast<void*>(const_cast<char *>("This is a SA_STRING"))};
	attr8.attrValues = valueString;


	AttrList.push_back(attrRdn);
	AttrList.push_back(attr1);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);
	AttrList.push_back(attr5);
	AttrList.push_back(attr6);
	AttrList.push_back(attr7);
	AttrList.push_back(attr8);


	result = omHandler_NO_OI.createObject(p_className.c_str(), nomeParent, AttrList );
	//ASSERTM("OmHandler:createObject method ", result == ACS_CC_SUCCESS);

	if( result == ACS_CC_SUCCESS ){
		cout << " -> Object  " << p_objName << "," << nomeParent << " created" << endl;
		cout << endl;
		return 0;
	}else {
		cout << " -> ERROR !!! Object  " << p_objName << "," << nomeParent << " NOT created" << endl;
		cout << omHandler_NO_OI.getInternalLastError() << endl;
		return -1;
	}

	omHandler_NO_OI.Finalize();
}


void PrepareEnvironment(){

	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();

	char nameClass[20] = "Test_config";

	string objNameCreate1 = "provaConfObj1";
	string dnObjName1 = "provaConfObj1,safApp=safImmService";

	int res = createClassTest(omHandler, nameClass);

	res = createObjectTest( nameClass, objNameCreate1);

	result = omHandler.Finalize();

}

void ClearEnvironment(){
	OmHandler omHandler;
	ACS_CC_ReturnType result;
	result = omHandler.Init();
	/**create a class **/
	char nameClass[20] = "Test_config";

	const char *dnObjName1 = "provaConfObj1,safApp=safImmService";

	result = omHandler.deleteObject(dnObjName1);
	ASSERTM("ClearEnvironment:omHandler.deleteObject dnObjName1 ", result == ACS_CC_SUCCESS);

	result = omHandler.removeClass(nameClass);
	ASSERTM("ClearEnvironment:removeClass ", result == ACS_CC_SUCCESS  );

	result = omHandler.Finalize();
	ASSERTM("ClearEnvironment()::omHandler.Finalize()", result == ACS_CC_SUCCESS);

	ASSERTM("ClearEnvironment()", result == ACS_CC_SUCCESS);

}

void Test_1(){

	ACS_CC_ReturnType returnCode;

	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";

	acs_apgcc_paramhandling PHA;

	float float_value;
	int int_value;
	long long long_value;
	unsigned int uint_value;
	unsigned long long ulong_value;
	double double_value;
	char char_value[50];

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_1",float_value);
	cout << "Value FLOAT : " << float_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test FLOAT Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_2",long_value);
	cout << "Value INT64 : " << long_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test INT64 Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_3",int_value);
	cout << "Value INT32 : " << int_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test INT32 Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_4",double_value);
	printf("Value DOUBLE : %lf returnCode : %i\n",double_value,returnCode);
	ASSERTM("Test DOUBLE Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_5",ulong_value);
	cout << "Value UINT64 : " << ulong_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test UINT64 Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_6",uint_value);
	cout << "Value UINT32 : " << uint_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test UINT32 Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_7",char_value);
	cout << "Value SA_NAMET : " << char_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test SA_NAMET Fails", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_8",char_value);
	cout << "Value SA_STRINGT : " << char_value << " returnCode : "<< returnCode << endl;
	ASSERTM("Test SA_STRINGT Fails", returnCode == ACS_CC_SUCCESS );

}

void Test_2(){

	ACS_CC_ReturnType returnCode;

	string objImplemented = "provaConfObj1,safApp=safImmService";
	string classImplemented = "Test_config";

	acs_apgcc_paramhandling PHA;

	short tmp_short;
	int tmp_int;
	long long tmp_long_long;
	unsigned int tmp_uint;
	unsigned long long tmp_ulong_long;
	double tmp_double;
	float tmp_float;

	char* pippo;
	char pippo2[50];
	char pippo3[10];

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_1",tmp_short);
	ASSERTM("Test Negative FLOAT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "FLOAT / short Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_2",tmp_uint);
	ASSERTM("Test Negative INT64 Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "INT64 / UINT32 Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_3",tmp_double);
	ASSERTM("Test Negative INT32 Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "INT32 / DOUBLE Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_4",tmp_long_long);
	ASSERTM("Test Negative DOUBLE Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "DOUBLE / INT64 Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_5",tmp_int);
	ASSERTM("Test Negative UINT64 Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "UINT64 / INT32 Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_6",tmp_float);
	ASSERTM("Test Negative UINT32 Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "UINT32 / FLOAT Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_7",tmp_ulong_long);
	ASSERTM("Test Negative SA_NAMET Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_NAMET / UINT64 Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_8",tmp_short);
	ASSERTM("Test Negative SA_STRINGT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_STRINGT / short Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_2",pippo);
	ASSERTM("Test Negative SA_STRINGT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "INT64 / char* Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_3",pippo2);
	ASSERTM("Test Negative SA_STRINGT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "INT32 / char*[] Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_7",pippo);
	ASSERTM("Test Negative SA_NAMET Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_NAMET / char* Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_8",pippo);
	ASSERTM("Test Negative SA_STRINGT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_STRINGT / char* Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_7",pippo3);
	ASSERTM("Test Negative SA_NAMET Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_NAMET / char[10] Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("provaConfObj1,safApp=safImmService","ATTRIBUTE_8",pippo3);
	ASSERTM("Test Negative SA_STRINGT Fails", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "SA_STRINGT / char[10] Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

}

void Test_3(){

	ACS_CC_ReturnType returnCode;

	acs_apgcc_paramhandling PHA;

	int tmp;
	char tmp_str[50];

	returnCode = PHA.getParameter("prcNodeStatus,prvPrcId=1,safApp=safImmService","stateNodeA",tmp);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_SUCCESS );

	returnCode = PHA.getParameter("prcNodeStatus,prvPrcId=1,safApp=safImmService","SaImmAttrAdminOwnerName",tmp_str);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_SUCCESS );

}

void Test_4(){

	ACS_CC_ReturnType returnCode;

	acs_apgcc_paramhandling PHA;

	int tmp;

	returnCode = PHA.getParameter("prcNodeStatus,prvPrcId=1,safApp=safImmService","test_test_test",tmp);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("test_test_test","SaImmAttrAdminOwnerName",tmp);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("test_test_test","test_test_test",tmp);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

	returnCode = PHA.getParameter("","",tmp);
	ASSERTM("<EMPTY> value handled", returnCode == ACS_CC_FAILURE );
	if ( returnCode == ACS_CC_FAILURE ){
		cout << "Error - errorcode : " << PHA.getInternalLastError() << " ==> " << PHA.getInternalLastErrorText() << endl;
	}

}

void runSuite(){

	cute::suite s;

	s.push_back(CUTE(PrepareEnvironment));

	s.push_back(CUTE(Test_1));

	s.push_back(CUTE(Test_2));

	s.push_back(CUTE(Test_3));

	s.push_back(CUTE(Test_4));

	cute::ide_listener lis;

	cute::makeRunner(lis)(s, "The Suite");
}


/*Main*/
int ACE_TMAIN (int, ACE_TCHAR *[]) {
	cout << "Parameter Handling CUTE Test START" << endl;

	runSuite();

	cout << "Parameter handling CUTE Test END" << endl;

	return 0;
}
