//============================================================================
// Name        : NewTest.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "OmHandler.h"
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
using namespace std;
/*The Name of the configuration class to be defined*/
char* nomeNewClasse_config = "CLASSTEST_config";

/*The Name of the configuration class whit initialization values, to be defined*/
char* nomeNewClasse_initialised = "CLASSTEST_initilized";

/*The Name of a not defined class used to test error case*/
char* nomeNewClasse_ERROR = "CLASSTEST_ERROR";

/*The Name of the runtime class to be defined*/
char* nomeNewClasse_runtime = "CLASSTEST_runtime";

/*The Name of the object created used to retrieve the values*/
char* nomeObjToGet = "RDN_Attribute=RDN_2,MeId=1";

/*The Name of the object created as instance of the class whit default values*/
char* nomeObjToGetDefaultValues = "RDN_Attribute=RDN_ClassInitialized,MeId=1";
char* nomeObjToGetDefaultValues2 = "RDN_Attribute=RDN_ClassInitialized2,MeId=1";

/*The Name of the object with MULTIVALUES created used to retrieve the values*/
char* nomeObjToGetMULTI = "RDN_Attribute=RDN_MULTI,MeId=1";

OmHandler OMHANDLER_NEW;


/*******************Values for Attributes *************************************************/
long long int 			intValueForObject = 56789;					/*Int 64*/
int 					int32ValueForObject = 123456;				/*Int 32*/
unsigned int 			intValueU32ForObject = 11111;				/*Unsigned Int 32*/
unsigned long long int 	intValueU64ForObject = 1112223334445556667;	/*Unsigned Int64*/
double 					doubleValueForObject = 222.56789;			/*Double*/
float 					floatValueForObject = 222.9;				/*Float*/

char 					val_attStringForObject[]="StringValue";		/*String*/
char 					val_attString2ForObject[]="StrinvValue2";
char 					val_attString3ForObject[]="StrinvValue3";

char 					*val_attNameForObject="NameTValue";			/*Name*/
char 					val_attName2ForObject[]="NameTValue222";
char 					val_attName3ForObject[]="NameTValue333";

long long int 			timeValueForObject = SA_TIME_ONE_MICROSECOND;/*Time*/

/*******************Values for Attributes MULTIVALUE*****************************************/
long long int 			intValueForObjectMULTI1 = 565656;					/*Int 64*/
long long int 			intValueForObjectMULTI2 = 575757;					/*Int 64*/
long long int 			intValueForObjectMULTI3 = 585858;					/*Int 64*/

int 					int32ValueForObjectMULTI1 = -128;					/*Int 32*/
int 					int32ValueForObjectMULTI2 = -129;					/*Int 32*/
int 					int32ValueForObjectMULTI3 = -130;					/*Int 32*/

unsigned int 			intValueU32ForObjectMULTI1 = 11111;					/*Unsigned Int 32*/
unsigned int 			intValueU32ForObjectMULTI2 = 22222;					/*Unsigned Int 32*/
unsigned int 			intValueU32ForObjectMULTI3 = 55555;					/*Unsigned Int 32*/

unsigned long long int intValueU64ForObjectMULTI1 = 1112223334445556667;	/*Unsigned Int64*/
unsigned long long int intValueU64ForObjectMULTI2 = 2112223334445556667;	/*Unsigned Int64*/
unsigned long long int intValueU64ForObjectMULTI3 = 3112223334445556667;	/*Unsigned Int64*/

double 					doubleValueForObjectMULTI1 = 222.56789;				/*Double*/
double 					doubleValueForObjectMULTI2 = -33.9;					/*Double*/
double 					doubleValueForObjectMULTI3 = 44.9;					/*Double*/

float 					floatValueForObjectMULTI1 = 25.256;					/*Float*/
float 					floatValueForObjectMULTI2 = -35.356;				/*Float*/
float 					floatValueForObjectMULTI3 = 45.457;					/*Float*/

char 					val_attStringForObjectMULTI[]= "StringValue1MULTI";	/*String*/
char 					val_attString2ForObjectMULTI[]="StrinvValue2MULTI";
char 					val_attString3ForObjectMULTI[]="StrinvValue3MULTI";

char 					*val_attNameForObjectMULTI="NameTValueMULTI";		/*Name*/
char 					val_attName2ForObjectMULTI[]="NameTValue222MULTI";
char 					val_attName3ForObjectMULTI[]="NameTValue333MULTI";

long long int 			timeValueForObjectMULTI1 = SA_TIME_ONE_MICROSECOND;	/*Time*/
long long int			timeValueForObjectMULTI2 = SA_TIME_ONE_HOUR;		/*Time*/

/*******************Default values ONLY for class nomeNewClasse_initialised = "CLASSTEST_initilized" **/
int 					valueInt = 132;
unsigned int 			valueIntUsigned = 13333;
long long int 			valueInt64 = 123456789;
unsigned long long int 	valueInt64Unsigned = 987654321;
long long int 			valueTime = SA_TIME_ONE_MICROSECOND;
float 					floatValue = 1.234;
double 					doubleValue = 123.567;
char 					nameDefault[]   = "NAME_VALUE_DEFAULT";
char					stringDefault[] = "STRING_VALUE_DEFAULT";

/*this test define a configuration class without default values for attributes */
void thisTestCreateConfClass(OmHandler objectHandler) {
	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {"RDN_Attribute",ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {"ATTR_Int32",ATTR_INT32T,ATTR_CONFIG|ATTR_INITIALIZED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {"ATTR_UnsignedInt32",ATTR_UINT32T,ATTR_CONFIG|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {"ATTR_Int64",ATTR_INT64T,ATTR_CONFIG|ATTR_INITIALIZED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {"ATTR_UnsignedInt64",ATTR_UINT64T,ATTR_CONFIG|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {"ATTR_TIME", ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {"ATT_NAME", ATTR_NAMET, ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeFLOAT = {"ATT_FLOAT", ATTR_FLOATT, ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {"ATT_DOUBLE",ATTR_DOUBLET,ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {"ATT_STRING",ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeANYT = {"ATT_ANYT",ATTR_ANYT,ATTR_CONFIG,0};
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
	classAttributes.push_back(attributeANYT);
	/*assert create class*/
	result=objectHandler.defineClass( nomeNewClasse_config,categoryClassToBeDefined,classAttributes);
	ASSERTM("OmHandler:defineClass a new CONFIG Class ", result==ACS_CC_SUCCESS);

	/*try to recreate the same class intended to tesr error case*/
	result=objectHandler.defineClass( nomeNewClasse_config,categoryClassToBeDefined,classAttributes);
	ASSERTM("OmHandler:defineClass try to recreate the same CONFIG Class ", result==ACS_CC_FAILURE);
	sleep(3);
}
/*************************************************************************************************/
/*
 *
 * The following test are intended to test definition creation and check values on Classes
 * and OBJECT with attributes having default values
 */
/*************************************************************************************************/
/*this test define a class with attributes with default values*/
void thisTestCreateConfClassDefaultValues() {

	ACS_CC_ReturnType result;

	//P ASSERTM("OmHandler:Init method ", OMHANDLER_NEW.Init()==0);

	/*Category of class to be defined nomeNewClasse_initialised*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;

	ACS_CC_AttrDefinitionType attributeRDN = {"RDN_Attribute",ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0};
	ACS_CC_AttrDefinitionType attributeII32_INIT = {"ATTR_Int32_I",ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeIUn32_INIT = {"ATTR_UnsignedInt32_I",ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeII64_INIT = {"ATTR_Int64_I",ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeIUn64_INIT = {"ATTR_UnsignedInt64_I",ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeITIME_INIT = {"ATTR_TIME_I", ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeINAME_INIT = {"ATT_NAME_I", ATTR_NAMET, ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeIFLOAT_INIT = {"ATT_FLOAT_I", ATTR_FLOATT, ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeIDOUBLE_INIT = {"ATT_DOUBLE_I",ATTR_DOUBLET,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeISTRING_INIT = {"ATT_STRING_I",ATTR_STRINGT,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeIANYT_INIT = {"ATT_ANYT_I",ATTR_ANYT,ATTR_CONFIG|ATTR_WRITABLE,0};

	/*set default values*/
	/*attributeI32*/
	attributeII32_INIT.attrDefaultVal=reinterpret_cast<void*>(&valueInt);

	/*attributeUnI32*/
	attributeIUn32_INIT.attrDefaultVal=reinterpret_cast<void*>(&valueIntUsigned);


	/*attributeI64*/
	attributeII64_INIT.attrDefaultVal=reinterpret_cast<void*>(&valueInt64);

	/*attributeUnI64*/
	attributeIUn64_INIT.attrDefaultVal=reinterpret_cast<void*>(&valueInt64Unsigned);

	/*attributeTIME?*/
	attributeITIME_INIT.attrDefaultVal= reinterpret_cast<void*>(&valueTime);

	/*attributeNAME?*/
	char *name;
	int len = strlen("NAME_VALUE_DEFAULT");
	name = new char[len+1];
	strcpy(name, "NAME_VALUE_DEFAULT" );
	attributeINAME_INIT.attrDefaultVal=reinterpret_cast<void*>(name);

	/*attributeFLOAT*/
	attributeIFLOAT_INIT.attrDefaultVal=reinterpret_cast<void*>(&floatValue);

	/*attributeDOUBLE*/
	attributeIDOUBLE_INIT.attrDefaultVal=reinterpret_cast<void*>(&doubleValue);

	/*attributeSTRING?*/
	char *nameS;
	int lenS = strlen("STRING_VALUE_DEFAULT");
	//nameS = (char*) malloc(lenS+1);
	nameS = new char[lenS+1];
	strcpy(nameS, "STRING_VALUE_DEFAULT" );
	attributeISTRING_INIT.attrDefaultVal=reinterpret_cast<void*>(&nameS);
	/*attributeANYT?*/

	/*end prepare attributes**/
	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeII32_INIT);
	classAttributes.push_back(attributeIUn32_INIT);
	classAttributes.push_back(attributeII64_INIT);
	classAttributes.push_back(attributeIUn64_INIT);
	classAttributes.push_back(attributeITIME_INIT);
	classAttributes.push_back(attributeINAME_INIT);
	classAttributes.push_back(attributeIFLOAT_INIT);
	classAttributes.push_back(attributeIDOUBLE_INIT);
	classAttributes.push_back(attributeISTRING_INIT);
	classAttributes.push_back(attributeIANYT_INIT);

	/*assert create class*/
	result=OMHANDLER_NEW.defineClass( nomeNewClasse_initialised,categoryClassToBeDefined,classAttributes);
	ASSERTM("thisTestCreateClass:defineClass Config Class Attributes whit INIT VALUES",result==ACS_CC_SUCCESS);


}

/*This test instances the class above defined with default values and provide values only for ATTR_Int32_I, the
 * other attributes will have the default values */
void thisIsATest_CreateObjectDefaultValues() {

		char* nomeParent = "MeId=1";
		/*The list of attributes*/
		/*the vector*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType first;


		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = "RDN_Attribute=RDN_ClassInitialized";
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the attribute ATTR_Int32*/
		char name_attint32[]="ATTR_Int32_I";
		first.attrName = name_attint32;
		first.attrType =ATTR_INT32T;
		first.attrValuesNum = 1;
		int int32ValueForObject = 223355;
		first.attrValues = new void*[1];
		first.attrValues[0] = reinterpret_cast<void*>(&int32ValueForObject);




		/*Add the atributes to vector*/
		AttrList.push_back(attrRdn);
		AttrList.push_back(first);
		sleep(5);

		/*Create the class*/
		ASSERTM("CUTE: OmHandler:createObject method ", OMHANDLER_NEW.createObject(nomeNewClasse_initialised, nomeParent, AttrList )==ACS_CC_SUCCESS);


}
void thisIsATest_CreateObjectDefaultValues2() {

		char* nomeParent = "MeId=1";
		/*The list of attributes*/
		/*the vector*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType first;


		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = "RDN_Attribute=RDN_ClassInitialized2";
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the attribute ATTR_Int32*/
		char name_attint32[]="ATTR_Int32_I";
		first.attrName = name_attint32;
		first.attrType =ATTR_INT32T;
		first.attrValuesNum = 1;
		int int32ValueForObject = 445588;
		first.attrValues = new void*[1];
		first.attrValues[0] = reinterpret_cast<void*>(&int32ValueForObject);




		/*Add the atributes to vector*/
		AttrList.push_back(attrRdn);
		AttrList.push_back(first);
		sleep(5);

		/*Create the class*/
		ASSERTM("CUTE: OmHandler:createObject method ", OMHANDLER_NEW.createObject(nomeNewClasse_initialised, nomeParent, AttrList )==ACS_CC_SUCCESS);


}

void thisIsATest_CheckValuesDefaultValues() {

	ACS_CC_ImmParameter paramToFind;
	char nome[] 			= "ATTR_Int64_I";
	char nomeUnsiged[] 		= "ATTR_UnsignedInt64_I";
	char nome32[] 			= "ATTR_Int32_I";
	char nomeUnsiged32[] 	= "ATTR_UnsignedInt32_I";
	char nomeTimeAtt[] 		= "ATTR_TIME_I";
	char nomeFloatAtt[] 	= "ATT_FLOAT_I";
	char nomeDoubleAtt[] 	= "ATT_DOUBLE_I";
	char nomeName[]			= "ATT_NAME_I";
	char nomeString[]		= "ATT_STRING_I";



	sleep(3);

	/* Get Int64*/
	paramToFind.attrName = nome;

	ASSERTM("CUTE: OmHandler:getAttribute method Int64  ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get INT64 is correct", valueInt64 , *(long long int*)paramToFind.attrValues[0]  );

	/*Get Unsigned Int64*/
	paramToFind.attrName = nomeUnsiged;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt64 ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get INT64 is correct", valueInt64Unsigned , *(unsigned long long int*)paramToFind.attrValues[0]  );


	/* Get Int32*/
	paramToFind.attrName = nome32;

	ASSERTM("CUTE: OmHandler:getAttribute method Int32  ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if NEW value Get INT32 is correct", 223355 , *( int*)paramToFind.attrValues[0]  );


	/*Get Unsigned Int32*/
	paramToFind.attrName = nomeUnsiged32;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt32 ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get INT32 is correct", valueIntUsigned , *(unsigned  int*)paramToFind.attrValues[0]  );


	/* Get Time*/
	paramToFind.attrName = nomeTimeAtt;

	ASSERTM("CUTE: OmHandler:getAttribute method TIME Default value ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get TIME is correct", valueTime , *( long long *)paramToFind.attrValues[0]  );


	/* Get FLOAT*/
	paramToFind.attrName = nomeFloatAtt;

	ASSERTM("CUTE: OmHandler:getAttribute method FLOAT Default value  ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get FLOAT is correct", floatValue , *( float*)paramToFind.attrValues[0]  );

	/*Get DOUBLE*/
	paramToFind.attrName = nomeDoubleAtt;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt32 ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if Default value Get Double is correct", doubleValue , *(double*)paramToFind.attrValues[0]  );


	/* Get Name*/
	paramToFind.attrName = nomeName;

	ASSERTM("CUTE: OmHandler:getAttribute method NAME  ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);
	char *actualName=(char *)paramToFind.attrValues[0];
	//string u(nameDefault);
	string d(actualName);
	ASSERT_EQUALM("CUTE: check if value Get NAME is correct", nameDefault ,d );

	/*Get String*/
	paramToFind.attrName = nomeString;
	ASSERTM("CUTE: OmHandler:getAttribute method STRING ", OMHANDLER_NEW.getAttribute(nomeObjToGetDefaultValues, &paramToFind )==ACS_CC_SUCCESS);

	string stringRetrieved(reinterpret_cast<char*>(paramToFind.attrValues[0] ));
	ASSERT_EQUALM("CUTE: check if value Get STRING is correct", stringDefault ,stringRetrieved);






}



void thisIsATest_CreateObject() {


		ASSERTM("OmHandler:Init method ", OMHANDLER_NEW.Init()==0);

		thisTestCreateConfClass(OMHANDLER_NEW);

		//char* nomeParent = "safApp=safImmService\0";
		char* nomeParent = "MeId=1";
		/*The list of attributes*/
		/*the vector*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attrUserLAbel;
		ACS_CC_ValuesDefinitionType second;
		ACS_CC_ValuesDefinitionType third;
		ACS_CC_ValuesDefinitionType forth;
		ACS_CC_ValuesDefinitionType fifth;
		ACS_CC_ValuesDefinitionType sixth;
		ACS_CC_ValuesDefinitionType seven;
		ACS_CC_ValuesDefinitionType eight;
		ACS_CC_ValuesDefinitionType nine;

		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = "RDN_Attribute=RDN_2\0";
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the attribute ATTR_Int64*/
		char name_attint64[]="ATTR_Int64";
		attrUserLAbel.attrName = name_attint64;
		attrUserLAbel.attrType = ATTR_INT64T;
		attrUserLAbel.attrValuesNum = 1;
		//moved long long int intValueForObject = 56789;
		attrUserLAbel.attrValues = new void*[1];
		attrUserLAbel.attrValues[0] = reinterpret_cast<void*>(&intValueForObject);

		/*Fill the attribute ATTR_Int32*/
		char name_attint32[]="ATTR_Int32";
		second.attrName = name_attint32;
		second.attrType =ATTR_INT32T;
		second.attrValuesNum = 1;
		// moved int int32ValueForObject = 123456;
		second.attrValues = new void*[1];
		second.attrValues[0] = reinterpret_cast<void*>(&int32ValueForObject);

		/*Fill the attribute ATTR_UInt32*/
		char name_attUint32[]="ATTR_UnsignedInt32";
		third.attrName = name_attUint32;
		third.attrType = ATTR_UINT32T;
		third.attrValuesNum = 1;
		// moved unsigned int intValueU32ForObject = 11111;
		third.attrValues = new void*[1];
		third.attrValues[0] = reinterpret_cast<void*>(&intValueU32ForObject);

		/*forth UINT64T*/
		char name_attUint64[]="ATTR_UnsignedInt64";
		forth.attrName = name_attUint64;
		forth.attrType = ATTR_UINT64T;
		forth.attrValuesNum = 1;
		// moved unsigned long long int intValueU64ForObject = 1112223334445556667;
		forth.attrValues = new void*[1];
		forth.attrValues[0] = reinterpret_cast<void*>(&intValueU64ForObject);

		/*fifth  double*/
		char name_attDouble[]="ATT_DOUBLE";
		fifth.attrName = name_attDouble;
		fifth.attrType = ATTR_DOUBLET;
		fifth.attrValuesNum = 1;
		// moved double doubleValueForObject = 222.56789;
		fifth.attrValues = new void*[1];
		fifth.attrValues[0] = reinterpret_cast<void*>(&doubleValueForObject);

		/*sixth float*/
		char name_attFloat[]="ATT_FLOAT";
		sixth.attrName = name_attFloat;
		sixth.attrType = ATTR_FLOATT;
		sixth.attrValuesNum = 1;
		// moved float floatValueForObject = 222.9;
		sixth.attrValues = new void*[1];
		sixth.attrValues[0] = reinterpret_cast<void*>(&floatValueForObject);

		/*seven  ATT_STRING*/
		char name_attString[]="ATT_STRING";
		seven.attrName = name_attString;
		seven.attrType = ATTR_STRINGT;
		seven.attrValuesNum = 1;
		/* moved char val_attStringForObject[]="StringValue";
		char val_attString2ForObject[]="StrinvValue2";
		char val_attString3ForObject[]="StrinvValue3";*/
		seven.attrValues = new void*[1];
		std::cout << "!!!Hello World 3.2 !!!" << std::endl;
		seven.attrValues[0] = reinterpret_cast<void*>(val_attStringForObject);
		/*
		seven.attrValues[1] = reinterpret_cast<void*>(val_attString2ForObject);
		seven.attrValues[2] = reinterpret_cast<void*>(val_attString3ForObject);
		 */

		/*seven  ATT_SANAME*/
		char name_attName[]="ATT_NAME";
		eight.attrName = name_attName;
		eight.attrType = ATTR_NAMET;
		eight.attrValuesNum = 1;
		/* moved char val_attNameForObject[]="NameTValue";
		char val_attName2ForObject[]="NameTValue222";
		char val_attName3ForObject[]="NameTValue333";*/
		eight.attrValues = new void*[1];
		std::cout << "!!!Hello World 3.2 !!!" << std::endl;
		eight.attrValues[0] = reinterpret_cast<void*>(val_attNameForObject);
		/*
		eight.attrValues[1] = reinterpret_cast<void*>(val_attName2ForObject);
		eight.attrValues[2] = reinterpret_cast<void*>(val_attName3ForObject);
		 */

		/*Fill the attribute ATTR_TIME*/
		char name_atttime[]="ATTR_TIME";
		nine.attrName = name_atttime;
		nine.attrType = ATTR_TIMET;
		nine.attrValuesNum = 1;
		nine.attrValues = new void*[1];
		nine.attrValues[0] = reinterpret_cast<void*>(&timeValueForObject);

		/*Add the atributes to vector*/
		AttrList.push_back(attrRdn);
		AttrList.push_back(attrUserLAbel);
		AttrList.push_back(second);
		AttrList.push_back(third);
		AttrList.push_back(forth);
		AttrList.push_back(fifth);
		AttrList.push_back(sixth);
		AttrList.push_back(seven);
		AttrList.push_back(eight);
		AttrList.push_back(nine);

		/*Create the class*/
		ASSERTM("CUTE: OmHandler:createObject method ", OMHANDLER_NEW.createObject(nomeNewClasse_config, nomeParent, AttrList )==ACS_CC_SUCCESS);
		/*Create The same class to test error condition*/
		ASSERTM("CUTE: OmHandler:createObject method; object already exist  ", OMHANDLER_NEW.createObject(nomeNewClasse_config, nomeParent, AttrList )==ACS_CC_FAILURE);


}


void thisIsATest_CreateObjectMultiValue() {


	sleep(5);
		//char* nomeParent = "safApp=safImmService";
		char* nomeParent = "MeId=1";
		/*The list of attributes*/
		/*the vector*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;

		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attrUserLAbel;
		ACS_CC_ValuesDefinitionType second;
		ACS_CC_ValuesDefinitionType third;
		ACS_CC_ValuesDefinitionType forth;
		ACS_CC_ValuesDefinitionType fifth;
		ACS_CC_ValuesDefinitionType sixth;
		ACS_CC_ValuesDefinitionType seven;
		ACS_CC_ValuesDefinitionType eight;
		ACS_CC_ValuesDefinitionType nine;

		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attribute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = "RDN_Attribute=RDN_MULTI";
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

		/*Fill the attribute ATTR_Int64*/
		char name_attint64[]="ATTR_Int64";
		attrUserLAbel.attrName = name_attint64;
		attrUserLAbel.attrType = ATTR_INT64T;
		attrUserLAbel.attrValuesNum = 3;

		attrUserLAbel.attrValues = new void*[3];
		attrUserLAbel.attrValues[0] = reinterpret_cast<void*>(&intValueForObjectMULTI1);
		attrUserLAbel.attrValues[1] = reinterpret_cast<void*>(&intValueForObjectMULTI2);
		attrUserLAbel.attrValues[2] = reinterpret_cast<void*>(&intValueForObjectMULTI3);

		/*Fill the attribute ATTR_Int32*/
		char name_attint32[]="ATTR_Int32";
		second.attrName = name_attint32;
		second.attrType =ATTR_INT32T;
		second.attrValuesNum = 3;

		second.attrValues = new void*[3];
		second.attrValues[0] = reinterpret_cast<void*>(&int32ValueForObjectMULTI1);
		second.attrValues[1] = reinterpret_cast<void*>(&int32ValueForObjectMULTI2);
		second.attrValues[2] = reinterpret_cast<void*>(&int32ValueForObjectMULTI3);

		/*Fill the attribute ATTR_UInt32*/
		char name_attUint32[]="ATTR_UnsignedInt32";
		third.attrName = name_attUint32;
		third.attrType = ATTR_UINT32T;
		third.attrValuesNum = 3;

		third.attrValues = new void*[3];
		third.attrValues[0] = reinterpret_cast<void*>(&intValueU32ForObjectMULTI1);
		third.attrValues[1] = reinterpret_cast<void*>(&intValueU32ForObjectMULTI2);
		third.attrValues[2] = reinterpret_cast<void*>(&intValueU32ForObjectMULTI3);

		/*forth UINT64T*/
		char name_attUint64[]="ATTR_UnsignedInt64";
		forth.attrName = name_attUint64;
		forth.attrType = ATTR_UINT64T;
		forth.attrValuesNum = 3;

		forth.attrValues = new void*[3];
		forth.attrValues[0] = reinterpret_cast<void*>(&intValueU64ForObjectMULTI1);
		forth.attrValues[1] = reinterpret_cast<void*>(&intValueU64ForObjectMULTI2);
		forth.attrValues[2] = reinterpret_cast<void*>(&intValueU64ForObjectMULTI3);

		/*fifth  double*/
		char name_attDouble[]="ATT_DOUBLE";
		fifth.attrName = name_attDouble;
		fifth.attrType = ATTR_DOUBLET;
		fifth.attrValuesNum = 3;

		fifth.attrValues = new void*[3];
		fifth.attrValues[0] = reinterpret_cast<void*>(&doubleValueForObjectMULTI1);
		fifth.attrValues[1] = reinterpret_cast<void*>(&doubleValueForObjectMULTI2);
		fifth.attrValues[2] = reinterpret_cast<void*>(&doubleValueForObjectMULTI3);

		/*sixth float*/
		char name_attFloat[]="ATT_FLOAT";
		sixth.attrName = name_attFloat;
		sixth.attrType = ATTR_FLOATT;
		sixth.attrValuesNum = 3;

		sixth.attrValues = new void*[3];
		sixth.attrValues[0] = reinterpret_cast<void*>(&floatValueForObjectMULTI1);
		sixth.attrValues[1] = reinterpret_cast<void*>(&floatValueForObjectMULTI2);
		sixth.attrValues[2] = reinterpret_cast<void*>(&floatValueForObjectMULTI3);

		/*seven  ATT_STRING*/
		char name_attString[]="ATT_STRING";
		seven.attrName = name_attString;
		seven.attrType = ATTR_STRINGT;
		seven.attrValuesNum = 3;

		seven.attrValues = new void*[3];
		std::cout << "!!!Hello World 3.2 !!!" << std::endl;
		seven.attrValues[0] = reinterpret_cast<void*>(val_attStringForObjectMULTI);
		seven.attrValues[1] = reinterpret_cast<void*>(val_attString2ForObjectMULTI);
		seven.attrValues[2] = reinterpret_cast<void*>(val_attString3ForObjectMULTI);


		/*seven  ATT_SANAME*/
		char name_attName[]="ATT_NAME";
		eight.attrName = name_attName;
		eight.attrType = ATTR_NAMET;
		eight.attrValuesNum = 3;
		eight.attrValues = new void*[3];
		std::cout << "!!!Hello World 3.2 !!!" << std::endl;
		eight.attrValues[0] = reinterpret_cast<void*>(val_attNameForObjectMULTI);
		eight.attrValues[1] = reinterpret_cast<void*>(val_attName2ForObjectMULTI);
		eight.attrValues[2] = reinterpret_cast<void*>(val_attName3ForObjectMULTI);


		/*Fill the attribute ATTR_TIME*/
		char name_atttime[]="ATTR_TIME";
		nine.attrName = name_atttime;
		nine.attrType = ATTR_TIMET;
		nine.attrValuesNum = 2;
		nine.attrValues = new void*[2];
		nine.attrValues[0] = reinterpret_cast<void*>(&timeValueForObjectMULTI1);
		nine.attrValues[1] = reinterpret_cast<void*>(&timeValueForObjectMULTI2);


		/*Add the atributes to vector*/
		AttrList.push_back(attrRdn);
		AttrList.push_back(attrUserLAbel);
		AttrList.push_back(second);
		AttrList.push_back(third);
		AttrList.push_back(forth);
		AttrList.push_back(fifth);
		AttrList.push_back(sixth);
		AttrList.push_back(seven);
		AttrList.push_back(eight);
		AttrList.push_back(nine);

		/*Create the class*/
		ASSERTM("CUTE: OmHandler:createObject method ", OMHANDLER_NEW.createObject(nomeNewClasse_config, nomeParent, AttrList )==ACS_CC_SUCCESS);


		sleep(2);
}
/*This test to get attribute INT64 & UINT64*/
void thisIsATest_GetINT64() {

	ACS_CC_ImmParameter paramToFind;
	char nome[] = "ATTR_Int64";
	char nomeUnsiged[] = "ATTR_UnsignedInt64";
	char nomeNotPresent[] = "ATTR_ERROR";

	sleep(3);
	/* Get Int64*/
	paramToFind.attrName = nome;

	ASSERTM("CUTE: OmHandler:getAttribute method Int64  ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get INT64 is correct", intValueForObject , *(long long int*)paramToFind.attrValues[0]  );

	/*Get Unsigned Int64*/
	paramToFind.attrName = nomeUnsiged;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt64 ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get INT64 is correct", intValueU64ForObject , *(unsigned long long int*)paramToFind.attrValues[0]  );

	/*Get Unsigned Int64*/
	paramToFind.attrName = nomeNotPresent;

	ASSERTM("CUTE: OmHandler:getAttribute with wrong att name ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_FAILURE);


}

/*This test to get attribute INT32 & UINT32*/
void thisIsATest_GetINT32() {

	ACS_CC_ImmParameter paramToFind;
	char nome[] = "ATTR_Int32";
	char nomeUnsiged[] = "ATTR_UnsignedInt32";


	sleep(3);
	/* Get Int32*/
	paramToFind.attrName = nome;

	ASSERTM("CUTE: OmHandler:getAttribute method Int32  ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get INT32 is correct", int32ValueForObject , *( int*)paramToFind.attrValues[0]  );

	/*Get Unsigned Int32*/
	paramToFind.attrName = nomeUnsiged;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt32 ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get INT32 is correct", intValueU32ForObject , *(unsigned  int*)paramToFind.attrValues[0]  );
}

/*This test to get attribute Float & Double*/
void thisIsATest_GetFloatDouble() {

	ACS_CC_ImmParameter paramToFind;
	char nomeFloat[] = "ATT_FLOAT";
	char nomeDouble[] = "ATT_DOUBLE";


	sleep(5);
	/* Get FLOAT*/
	paramToFind.attrName = nomeFloat;

	ASSERTM("CUTE: OmHandler:getAttribute method FLOAT  ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get FLOAT is correct", floatValueForObject , *( float*)paramToFind.attrValues[0]  );

	/*Get DOUBLE*/
	paramToFind.attrName = nomeDouble;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt32 ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get Double is correct", doubleValueForObject , *(double*)paramToFind.attrValues[0]  );

}

/*This test to get attribute Name & String*/
void thisIsATest_GetNameAndString() {

	ACS_CC_ImmParameter paramToFind;
	char nomeName[] = "ATT_NAME";
	char nomeString[] = "ATT_STRING";


	sleep(5);
	/* Get Name*/
	paramToFind.attrName = nomeName;

	ASSERTM("CUTE: OmHandler:getAttribute method NAME  ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);
    char *ddd=(char *)paramToFind.attrValues[0];
    int i = strlen(ddd);
    cout<<"LUNGHEZZA========="<<i <<endl;
	string u(val_attNameForObject);
	string d(ddd);

    cout<<"1"<<ddd <<endl;
	ASSERT_EQUALM("CUTE: check if value Get NAME is correct", u ,d );
	cout<<"2"<<ddd <<endl;

	/*Get String*/
	paramToFind.attrName = nomeString;


	ASSERTM("CUTE: OmHandler:getAttribute method STRING ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	string stringRetrieved(reinterpret_cast<char*>(paramToFind.attrValues[0] ));
	ASSERT_EQUALM("CUTE: check if value Get STRING is correct", val_attStringForObject ,stringRetrieved);

}
/*This test to get attribute TIME*/
void thisIsATest_GetTIME() {

	ACS_CC_ImmParameter paramToFind;
	char nomeTime[] = "ATTR_TIME";



	sleep(5);
	/* Get Time*/
	paramToFind.attrName = nomeTime;

	ASSERTM("CUTE: OmHandler:getAttribute method TIME  ", OMHANDLER_NEW.getAttribute(nomeObjToGet, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value Get TIME is correct", timeValueForObject , *( long long *)paramToFind.attrValues[0]  );


}
/*The following methods get multiple values for OBJECT*/
void thisIsATest_GetINT64MultiValue() {

	ACS_CC_ImmParameter paramToFind;
	char nome[] = "ATTR_Int64";
	char nomeUnsiged[] = "ATTR_UnsignedInt64";

	sleep(3);
	/* Get Int64*/
	paramToFind.attrName = nome;

	ASSERTM("CUTE: OmHandler:getAttribute MultiValue method Int64  ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);
	ASSERT_EQUALM("CUTE: check Num of att values", 3 , paramToFind.attrValuesNum  );
	ASSERT_EQUALM("CUTE: check if value 0 Get INT64 is correct", intValueForObjectMULTI1 , *(long long int*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 1 Get INT64 is correct", intValueForObjectMULTI2 , *(long long int*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value 2 Get INT64 is correct", intValueForObjectMULTI3 , *(long long int*)paramToFind.attrValues[2]  );


	/*Get Unsigned Int64*/
	paramToFind.attrName = nomeUnsiged;

	ASSERTM("CUTE: OmHandler:getAttribute MultiValue method UnsignedInt64 ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check Num of att values", 3 , paramToFind.attrValuesNum  );
	ASSERT_EQUALM("CUTE: check if value Get INT64 is correct", intValueU64ForObjectMULTI1 , *(unsigned long long int*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value Get INT64 is correct", intValueU64ForObjectMULTI2 , *(unsigned long long int*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value Get INT64 is correct", intValueU64ForObjectMULTI3 , *(unsigned long long int*)paramToFind.attrValues[2]  );


}

/*This test to get attribute INT32 & UINT32 for multivalued object*/
void thisIsATest_GetINT32MultiValue() {

	ACS_CC_ImmParameter paramToFind;
	char nome[] = "ATTR_Int32";
	char nomeUnsiged[] = "ATTR_UnsignedInt32";


	sleep(3);
	/* Get Int32*/
	paramToFind.attrName = nome;

	ASSERTM("CUTE: OmHandler:getAttribute method Int32  ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value number is correct Int32", 3 , paramToFind.attrValuesNum );
	ASSERT_EQUALM("CUTE: check if value 0 Get INT32 is correct", int32ValueForObjectMULTI1 , *( int*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 1 Get INT32 is correct", int32ValueForObjectMULTI2 , *( int*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value 2 Get INT32 is correct", int32ValueForObjectMULTI3 , *( int*)paramToFind.attrValues[2]  );

	/*Get Unsigned Int32*/
	paramToFind.attrName = nomeUnsiged;

	ASSERTM("CUTE: OmHandler:getAttribute method UnsignedInt32 ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value number is correct UnsignedInt32", 3 , paramToFind.attrValuesNum );
	ASSERT_EQUALM("CUTE: check if value 0 Get INT32 is correct", intValueU32ForObjectMULTI1 , *(unsigned  int*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 1 Get INT32 is correct", intValueU32ForObjectMULTI2 , *(unsigned  int*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value 2 Get INT32 is correct", intValueU32ForObjectMULTI3 , *(unsigned  int*)paramToFind.attrValues[2]  );
}

/*This test to get attribute Float & Double for MULTIVALUED OBJECT*/
void thisIsATest_GetFloatDoubleMultiValue() {

	ACS_CC_ImmParameter paramToFind;
	char nomeFloat[] = "ATT_FLOAT";
	char nomeDouble[] = "ATT_DOUBLE";


	sleep(5);
	/* Get FLOAT*/
	paramToFind.attrName = nomeFloat;

	ASSERTM("CUTE: OmHandler:getAttribute method FLOAT  ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value number is correct FLOAT", 3 , paramToFind.attrValuesNum );

	ASSERT_EQUALM("CUTE: check if value 0 Get FLOAT is correct", floatValueForObjectMULTI1 , *( float*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 1 Get FLOAT is correct", floatValueForObjectMULTI2 , *( float*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value 2 Get FLOAT is correct", floatValueForObjectMULTI3 , *( float*)paramToFind.attrValues[2]  );

	/*Get DOUBLE*/
	paramToFind.attrName = nomeDouble;

	ASSERTM("CUTE: OmHandler:getAttribute method DOUBLE ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value number is correct DOUBLE", 3 , paramToFind.attrValuesNum );

	ASSERT_EQUALM("CUTE: check if value 1 Get Double is correct", doubleValueForObjectMULTI1 , *(double*)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 2 Get Double is correct", doubleValueForObjectMULTI2 , *(double*)paramToFind.attrValues[1]  );
	ASSERT_EQUALM("CUTE: check if value 3 Get Double is correct", doubleValueForObjectMULTI3 , *(double*)paramToFind.attrValues[2]  );


}

/*This test to get attribute TIME for MULTIVALUE OBJECT*/
void thisIsATest_GetTIMEMultiValue() {

	ACS_CC_ImmParameter paramToFind;
	char nomeTime[] = "ATTR_TIME";
	sleep(5);
	/* Get Time*/
	paramToFind.attrName = nomeTime;

	ASSERTM("CUTE: OmHandler:getAttribute method TIME  ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	ASSERT_EQUALM("CUTE: check if value number is correct TIME", 2 , paramToFind.attrValuesNum );
	ASSERT_EQUALM("CUTE: check if value 0 Get TIME is correct", timeValueForObjectMULTI1 , *( long long *)paramToFind.attrValues[0]  );
	ASSERT_EQUALM("CUTE: check if value 1 Get TIME is correct", timeValueForObjectMULTI2 , *( long long *)paramToFind.attrValues[1]  );


}

/*This test to get attribute Name & String*/
void thisIsATest_GetNameAndStringMultiValue() {

	ACS_CC_ImmParameter paramToFind;
	char nomeName[] = "ATT_NAME";
	char nomeString[] = "ATT_STRING";


	sleep(5);
	/* Get Name*/
	paramToFind.attrName = nomeName;

	ASSERTM("CUTE: OmHandler:getAttribute method NAME  ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	string expected_0(val_attNameForObjectMULTI );
	string expected_1(val_attName2ForObjectMULTI );
	string expected_2(val_attName3ForObjectMULTI );

	string actual_0(reinterpret_cast<char*>(paramToFind.attrValues[0]));
	string actual_1(reinterpret_cast<char*>(paramToFind.attrValues[1]));
	string actual_2(reinterpret_cast<char*>(paramToFind.attrValues[2]));

	ASSERT_EQUALM("CUTE: check if value number is correct NAME", 3 , paramToFind.attrValuesNum );
	ASSERT_EQUALM("CUTE: check if value Get NAME is correct", expected_0 ,actual_0 );
	ASSERT_EQUALM("CUTE: check if value Get NAME is correct", expected_1 ,actual_1 );
	ASSERT_EQUALM("CUTE: check if value Get NAME is correct", expected_2 ,actual_2 );


	/*Get String*/
	paramToFind.attrName = nomeString;

	ASSERTM("CUTE: OmHandler:getAttribute method STRING ", OMHANDLER_NEW.getAttribute(nomeObjToGetMULTI, &paramToFind )==ACS_CC_SUCCESS);

	string expectedString_0(val_attStringForObjectMULTI );
	string expectedString_1(val_attString2ForObjectMULTI );
	string expectedString_2(val_attString3ForObjectMULTI );

	string actualString_0(reinterpret_cast<char*>(paramToFind.attrValues[0]));
	string actualString_1(reinterpret_cast<char*>(paramToFind.attrValues[1]));
	string actualString_2(reinterpret_cast<char*>(paramToFind.attrValues[2]));

	ASSERT_EQUALM("CUTE: check if value number is correct STRING", 3 , paramToFind.attrValuesNum );
	ASSERT_EQUALM("CUTE: check if value Get STRING is correct", expectedString_0 ,actualString_0 );
	ASSERT_EQUALM("CUTE: check if value Get STRING is correct", expectedString_1 ,actualString_1 );
	ASSERT_EQUALM("CUTE: check if value Get STRING is correct", expectedString_2 ,actualString_2 );
}
/*Test case to delete OBJECT not existing and Class not existing*/
void thisIsATest_DeleteteObjectErrorCase() {

	sleep(3);
	/*Delete NOT Existing OBJECT: Expected result FAILURE*/
	ASSERTM("OmHandler:deleteObject method ", OMHANDLER_NEW.deleteObject("RDN_Attribute=RDN_2ERROR,MeId=1")==ACS_CC_FAILURE);

	/*Delete NOT Existing ClassExpected result FAILURE*/
	ASSERTM("OmHandler:removeClass method ", OMHANDLER_NEW.removeClass(nomeNewClasse_ERROR)==ACS_CC_FAILURE);

	sleep(2);
}

/*Test case to delete previously created OBJECT RDN_Attribute=RDN_2,MeId=1*/
void thisIsATest_DeleteteObject() {

	sleep(3);

	/*Delete Existing OBJECT*/ASSERTM("OmHandler:Init method ", OMHANDLER_NEW.Init()==0);
	ASSERTM("OmHandler:deleteObject method ", OMHANDLER_NEW.deleteObject("RDN_Attribute=RDN_2,MeId=1")==ACS_CC_SUCCESS);

	sleep(2);
}

/*Test case to delete previously created OBJECT RDN_Attribute=RDN_MULTI,MeId=1*/
void thisIsATest_DeleteteObjectMultiValue() {

	sleep(3);

	/*Delete Existing OBJECT*/
	ASSERTM("OmHandler:deleteObject method on MULTI valued Attributes ", OMHANDLER_NEW.deleteObject("RDN_Attribute=RDN_MULTI,MeId=1")==ACS_CC_SUCCESS);

	sleep(2);
}

/*Test case to delete previously created OBJECT RDN_Attribute=RDN_MULTI,MeId=1*/
void thisIsATest_DeleteteObjectDefaulValues() {

	sleep(3);

	/*Delete Existing OBJECT*/
	ASSERTM("OmHandler:deleteObject method on DEFAULT valued Attributes ", OMHANDLER_NEW.deleteObject(nomeObjToGetDefaultValues)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:deleteObject method on DEFAULT valued Attributes  2", OMHANDLER_NEW.deleteObject(nomeObjToGetDefaultValues2)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:removeClass method ", OMHANDLER_NEW.removeClass(nomeNewClasse_initialised)==ACS_CC_SUCCESS);
	sleep(2);
}

/*Test case to delete previously created CLASS*/
void thisIsATest_DeleteteClass() {

	sleep(2);

	/*Delete Existing Class*/
	ASSERTM("OmHandler:removeClass method ", OMHANDLER_NEW.removeClass(nomeNewClasse_config)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:Finalize method ", OMHANDLER_NEW.Finalize()==ACS_CC_SUCCESS);

	sleep(2);
}


/****     Added Fabio*/
void thisTestCreateConfClassNI_config(OmHandler objectHandler) {
	ACS_CC_ReturnType result;
	/* Name of the class to be defined*/
	//char nameClassToBeDefined[30] = "NewConfigClassNI_CA";
	char nameClassToBeDefined[30] = "CATUT_config";
	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;
	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {"RDN_Attrbute",ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {"ATTR_Int32",ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {"ATTR_UnsignedInt32",ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {"ATTR_Int64",ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {"ATTR_UnsignedInt64",ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {"ATTR_TIME", ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {"ATT_NAME", ATTR_NAMET, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeFLOAT = {"ATT_FLOAT", ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {"ATT_DOUBLE",ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {"ATT_STRING",ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeANYT = {"ATT_ANYT",ATTR_ANYT,SA_IMM_ATTR_CONFIG,0};
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
	classAttributes.push_back(attributeANYT);
	/*assert create class*/
	result=objectHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);
	ASSERTM("OmHandler:defineClass a new CONFIG Class ", result==ACS_CC_SUCCESS);
	sleep(2);
}

void thisTestCreateConfClassNI_runtime(OmHandler objectHandler) {
	ACS_CC_ReturnType result;
	/* Name of the class to be defined*/
	//char nameClassToBeDefined[30] = "NewConfigClassNI_CA";
	char nameClassToBeDefined[30] = "CATUT_runtime";
	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = RUNTIME;
	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {"RDN_Attrbute",ATTR_NAMET,ATTR_RDN|ATTR_RUNTIME|ATTR_PERSISTENT,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {"ATTR_Int32",ATTR_INT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {"ATTR_UnsignedInt32",ATTR_UINT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeI64 = {"ATTR_Int64",ATTR_INT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {"ATTR_UnsignedInt64",ATTR_UINT64T,ATTR_RUNTIME,0};
	/*ACS_CC_AttrDefinitionType attributeTIME = {"ATTR_TIME", ATTR_TIMET,ATTR_RUNTIME|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {"ATT_NAME", ATTR_NAMET, SA_IMM_ATTR_RUNTIME|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeFLOAT = {"ATT_FLOAT", ATTR_FLOATT, SA_IMM_ATTR_RUNTIME|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {"ATT_DOUBLE",ATTR_DOUBLET,SA_IMM_ATTR_RUNTIME|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {"ATT_STRING",ATTR_STRINGT,SA_IMM_ATTR_RUNTIME|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeANYT = {"ATT_ANYT",ATTR_ANYT,SA_IMM_ATTR_RUNTIME,0};
	*/classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attributeI32);
	classAttributes.push_back(attributeUnI32);
	classAttributes.push_back(attributeI64);
	classAttributes.push_back(attributeUnI64);
	/*classAttributes.push_back(attributeTIME);
	classAttributes.push_back(attributeNAME);
	classAttributes.push_back(attributeFLOAT);
	classAttributes.push_back(attributeDOUBLE);
	classAttributes.push_back(attributeSTRING);
	classAttributes.push_back(attributeANYT);*/
	/*assert create class*/
	result=objectHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);
	ASSERTM("OmHandler:defineClass Define a new RUNTIME Class: ", result==ACS_CC_SUCCESS);
	sleep(2);
}


void thisTestModifyAttributes(OmHandler objectHandler){

	/*Set a new value for ATT_FLOAT*/
	float valueFlo=1.3;
	char name_attFloat[]="ATT_FLOAT";
	ACS_CC_ImmParameter par;
	par.attrName=name_attFloat;
	par.attrType=ATTR_FLOATT;
	par.attrValuesNum=1;
	par.attrValues=new void*[par.attrValuesNum];
	par.attrValues[0] =reinterpret_cast<void*>(&valueFlo);


	/*Set a new value for ATTR_Int64*/
	long long int valueInt64 = -98765432;
	char name_attint64[]="ATTR_Int64";
	ACS_CC_ImmParameter par2;
	par2.attrName=name_attint64;
	par2.attrType=ATTR_INT64T;
	par2.attrValuesNum=1;
	par2.attrValues=new void*[par2.attrValuesNum];
	par2.attrValues[0] =reinterpret_cast<void*>(&valueInt64);


	/*Set a new value for ATTR_Int32*/
	int valueInt32 = -2345;
	char name_attint32[]="ATTR_Int32";
	ACS_CC_ImmParameter par3;
	par3.attrName=name_attint32;
	par3.attrType=ATTR_INT32T;
	par3.attrValuesNum=1;
	par3.attrValues=new void*[par3.attrValuesNum];
	par3.attrValues[0] =reinterpret_cast<void*>(&valueInt32);


	/*Set a new value for ATTR_UnsignedInt32*/
	unsigned int valueUnsInt32 = 2345;
	char name_attUint32[]="ATTR_UnsignedInt32";
	ACS_CC_ImmParameter par4;
	par4.attrName=name_attUint32;
	par4.attrType=ATTR_UINT32T;
	par4.attrValuesNum=1;
	par4.attrValues=new void*[par4.attrValuesNum];
	par4.attrValues[0] =reinterpret_cast<void*>(&valueUnsInt32);


	/*Set a new value for ATTR_UnsignedInt64*/
	unsigned long long int valueUnsInt64 = 778899112233;
	char name_attUint64[]="ATTR_UnsignedInt64";
	ACS_CC_ImmParameter par5;
	par5.attrName=name_attUint64;
	par5.attrType=ATTR_UINT64T;
	par5.attrValuesNum=1;
	par5.attrValues=new void*[par5.attrValuesNum];
	par5.attrValues[0] =reinterpret_cast<void*>(&valueUnsInt64);


	/*Set a new value for ATT_DOUBLE*/
	double valueDouble = 778899112233;
	char name_attDouble[]="ATT_DOUBLE";
	ACS_CC_ImmParameter par6;
	par6.attrName=name_attDouble;
	par6.attrType=ATTR_DOUBLET;
	par6.attrValuesNum=1;
	par6.attrValues=new void*[par6.attrValuesNum];
	par6.attrValues[0] =reinterpret_cast<void*>(&valueDouble);


	/*Set a new value for ATT_STRING*/
	char str1[30]="stringTest_1";
	char str2[40]="StringTest_2";
	ACS_CC_ImmParameter par7;
	par7.attrName="ATT_STRING";
	par7.attrType=ATTR_STRINGT;
	par7.attrValuesNum=2;
	par7.attrValues=new void*[par7.attrValuesNum];
	par7.attrValues[0] =reinterpret_cast<void*>(str1);
	par7.attrValues[1] =reinterpret_cast<void*>(str2);


	/*Set a new value for ATT_NAME*/
	char str3[30]="SaNameString_1";
	char str4[30]="SaNameString_2";
	ACS_CC_ImmParameter par8;
	par8.attrName="ATT_NAME";
	par8.attrType=ATTR_NAMET;
	par8.attrValuesNum=2;
	par8.attrValues=new void*[par8.attrValuesNum];
	par8.attrValues[0] =reinterpret_cast<void*>(str3);
	par8.attrValues[1] =reinterpret_cast<void*>(str4);

	/*Set a new value for ATT_INTERO (test error condition)*/
	int valueInt2=222;
	ACS_CC_ImmParameter par9;
	par9.attrName="ATT_INTERO";
	par9.attrType=ATTR_INT32T;
	par9.attrValuesNum=1;
	par9.attrValues=new void*[par8.attrValuesNum];
	par9.attrValues[0] =reinterpret_cast<void*>(&valueInt2);


	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATT_FLOAT )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTR_Int64 )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par2)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTR_Int32 )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par3)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTR_UnsignedInt32 )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par4)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATTR_UnsignedInt64 )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par5)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATT_DOUBLE )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par6)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATT_STRING )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par7)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATT_NAME )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par8)==ACS_CC_SUCCESS);
	ASSERTM("OmHandler:modifyAttributes method (Modify attribute ATT_INTERO )", objectHandler.modifyAttribute("RDN_2,MeId=1", &par9)==ACS_CC_FAILURE);

}


void thisIsATestForModify() {

	OmHandler Connected_config;
		ASSERTM("OmHandler:Init method ", Connected_config.Init()==0);
		thisTestCreateConfClassNI_config(Connected_config);
		char* nomeClasse_config = "CATUT_config";
		//modified char* nomeParent = "safApp=safImmService\0";
		char* nomeParent = "MeId=1";
		/*The list of attributes*/
		/*the vector*/
		vector<ACS_CC_ValuesDefinitionType> AttrList;
		/*the attributes*/
		ACS_CC_ValuesDefinitionType attrRdn;
		ACS_CC_ValuesDefinitionType attrUserLAbel;
		ACS_CC_ValuesDefinitionType second;
		ACS_CC_ValuesDefinitionType third;
		ACS_CC_ValuesDefinitionType forth;
		ACS_CC_ValuesDefinitionType fifth;
		ACS_CC_ValuesDefinitionType sixth;
		/*Fill the rdn Attribute */
		char attrdn[]= "RDN_Attrbute";
		attrRdn.attrName = attrdn;
		attrRdn.attrType = ATTR_NAMET;
		attrRdn.attrValuesNum = 1;
		char* rdnValue = "RDN_2\0";
		attrRdn.attrValues = new void*[1];
		attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);
		/*Fill the attribute ATTR_Int64l*/
		char name_attint64[]="ATTR_Int64";
		attrUserLAbel.attrName = name_attint64;
		attrUserLAbel.attrType = ATTR_INT64T;
		attrUserLAbel.attrValuesNum = 1;
		long long int intValue = 456;
		attrUserLAbel.attrValues = new void*[1];
		attrUserLAbel.attrValues[0] = reinterpret_cast<void*>(&intValue);
		/*Fill the attribute ATTR_Int32l*/
		char name_attint32[]="ATTR_Int32";
		second.attrName = name_attint32;
		second.attrType =ATTR_INT32T;
		second.attrValuesNum = 1;
		int int32Value = 123456;
		second.attrValues = new void*[1];
		second.attrValues[0] = reinterpret_cast<void*>(&int32Value);
		/*Fill the attribute ATTR_UInt32*/
		char name_attUint32[]="ATTR_UnsignedInt32";
		third.attrName = name_attUint32;
		third.attrType = ATTR_UINT32T;
		third.attrValuesNum = 1;
		unsigned int intValueU32 = 333;
		third.attrValues = new void*[1];
		third.attrValues[0] = reinterpret_cast<void*>(&intValueU32);
		/*forth*/
		char name_attUint64[]="ATTR_UnsignedInt64";
		forth.attrName = name_attUint64;
		forth.attrType = ATTR_UINT64T;
		forth.attrValuesNum = 1;
		unsigned long long int intValueU64 = 1112223334445556667;
		forth.attrValues = new void*[1];
		forth.attrValues[0] = reinterpret_cast<void*>(&intValueU64);
		/*fifth  double*/
		char name_attDouble[]="ATT_DOUBLE";
		fifth.attrName = name_attDouble;
		fifth.attrType = ATTR_DOUBLET;
		fifth.attrValuesNum = 1;
		double doubleValue = 222.56789;
		fifth.attrValues = new void*[1];
		fifth.attrValues[0] = reinterpret_cast<void*>(&doubleValue);
		/*sixth float*/
		char name_attFloat[]="ATT_FLOAT";
		sixth.attrName = name_attFloat;
		sixth.attrType = ATTR_FLOATT;
		sixth.attrValuesNum = 1;
		float floatValue = 222.9;
		sixth.attrValues = new void*[1];
		sixth.attrValues[0] = reinterpret_cast<void*>(&floatValue);

		/*Add the atributes to vector*/
		AttrList.push_back(attrRdn);
		AttrList.push_back(attrUserLAbel);
		AttrList.push_back(second);
		AttrList.push_back(third);
		AttrList.push_back(forth);
		AttrList.push_back(fifth);
		AttrList.push_back(sixth);
	//	ASSERTM("OmHandler:createObject 1 method ", Connected_config.createObject(nomeClasse_config, nomeParent, AttrList )==ACS_CC_SUCCESS);
	/*	if (Connected_config.createObject(nomeClasse_config, nomeParent, AttrList )!=0){
			printf("createObject FAILURE\n");
			return ;
		}*/
		ASSERTM("OmHandler:createObject method ", Connected_config.createObject(nomeClasse_config, nomeParent, AttrList )==ACS_CC_SUCCESS);
		ASSERTM("OmHandler:createObject method; object already exist  ", Connected_config.createObject(nomeClasse_config, nomeParent, AttrList )==ACS_CC_FAILURE);

		thisTestModifyAttributes(Connected_config);

		//ASSERTM("OmHandler:deleteObject method ", Connected_config.deleteObject("RDN_2,safApp=safImmService")==ACS_CC_SUCCESS);
		ASSERTM("OmHandler:deleteObject method ", Connected_config.deleteObject("RDN_2,MeId=1")==ACS_CC_SUCCESS);
		ASSERTM("OmHandler:removeClass method ", Connected_config.removeClass(nomeClasse_config)==ACS_CC_SUCCESS);
		ASSERTM("OmHandler:Finalize method ", Connected_config.Finalize()==ACS_CC_SUCCESS);
		OmHandler Connected_runtime;
		char* nomeClasse_runtime = "CATUT_runtime";
		ASSERTM("OmHandler:Init method (runtime param) ", Connected_runtime.Init()==ACS_CC_SUCCESS);
		thisTestCreateConfClassNI_runtime(Connected_runtime);
		ASSERTM("OmHandler:removeClass method  ", Connected_runtime.removeClass(nomeClasse_runtime)==ACS_CC_SUCCESS);

}
/**** END Added Fabio*/

void runSuite(){
	cute::suite s;

	s.push_back(CUTE(thisIsATest_CreateObject));
	s.push_back(CUTE(thisIsATest_CreateObjectMultiValue));
	s.push_back(CUTE(thisTestCreateConfClassDefaultValues));
	s.push_back(CUTE(thisIsATest_CreateObjectDefaultValues));
	s.push_back(CUTE(thisIsATest_CreateObjectDefaultValues2));

	/*GET VALUES FOR OBJECT SINGLE VALUED*/

	s.push_back(CUTE(thisIsATest_GetINT64));
	s.push_back(CUTE(thisIsATest_GetINT32));
	s.push_back(CUTE(thisIsATest_GetFloatDouble));
	s.push_back(CUTE(thisIsATest_GetNameAndString));
	s.push_back(CUTE(thisIsATest_GetTIME));
	s.push_back(CUTE(thisIsATest_DeleteteObjectErrorCase));

	/*GET VALUES FOR OBJECT MULTI VALUED*/

	s.push_back(CUTE(thisIsATest_GetINT64MultiValue));
	s.push_back(CUTE(thisIsATest_GetINT32MultiValue));
	s.push_back(CUTE(thisIsATest_GetFloatDoubleMultiValue));
	s.push_back(CUTE(thisIsATest_GetTIMEMultiValue));
	s.push_back(CUTE(thisIsATest_GetNameAndStringMultiValue));

	/* CHECK CORRECT VALUES FOR OBJECT with ALL default values except one */
	s.push_back(CUTE(thisIsATest_CheckValuesDefaultValues));


	s.push_back(CUTE(thisIsATestForModify));

	/*Delete Object SINGLE VALUED AND MULTI VALUED*/
	s.push_back(CUTE(thisIsATest_DeleteteObject));
	s.push_back(CUTE(thisIsATest_DeleteteObjectMultiValue));
	s.push_back(CUTE(thisIsATest_DeleteteObjectDefaulValues));
	s.push_back(CUTE(thisIsATest_DeleteteClass));



	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
}
int main() {
	runSuite();



}
