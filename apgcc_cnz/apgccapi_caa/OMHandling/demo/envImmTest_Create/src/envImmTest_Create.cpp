//============================================================================
// Name        : envImmTest_Create.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "OmHandler.h"
using namespace std;

int main(int argc, char * argv[]) {

	ACS_CC_ReturnType result;
	OmHandler immHandler;

	/* Name of the class to be defined*/
	char *nameClassToBeDefined = const_cast<char*>("Test_config");

	int c = 0;

	while( (c = getopt(argc, argv, "c:")) != -1){
		switch (c) {
			case 'c':

				nameClassToBeDefined = optarg;

				break;

			case '?':
				if( argc > 3)
					ACE_OS::fprintf(stdout, "error - Too many arguments\n ");

				else if( optopt == 'c' )
					ACE_OS::fprintf(stdout, "error - Option -%c requires an operand\n", optopt);

				else if ( isprint(optopt) )
					ACE_OS::fprintf(stdout, "error - Unrecognized option: -%c\n", optopt);

				return -1;
				break;
			default:
				break;
		}//END switch(c)

	}



//	char nameClassToBeDefined[30] = "Test_config";

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = CONFIGURATION;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;
	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char *>("RDN_Attrbute"),ATTR_NAMET,ATTR_RDN|ATTR_CONFIG,0} ;
	ACS_CC_AttrDefinitionType attributeI32 = {const_cast<char *>("ATTRIBUTE_3"),ATTR_INT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI32 = {const_cast<char *>("ATTRIBUTE_6"),ATTR_UINT32T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeI64 = {const_cast<char *>("ATTRIBUTE_2"),ATTR_INT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeUnI64 = {const_cast<char *>("ATTRIBUTE_5"),ATTR_UINT64T,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeTIME = {const_cast<char *>("ATTR_TIME"), ATTR_TIMET,ATTR_CONFIG|ATTR_WRITABLE,0};
	ACS_CC_AttrDefinitionType attributeNAME = {const_cast<char *>("ATTRIBUTE_7"), ATTR_NAMET, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeFLOAT = {const_cast<char *>("ATTRIBUTE_1"), ATTR_FLOATT, SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeDOUBLE = {const_cast<char *>("ATTRIBUTE_4"),ATTR_DOUBLET,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attributeSTRING = {const_cast<char *>("ATTRIBUTE_8"),ATTR_STRINGT,SA_IMM_ATTR_CONFIG|ATTR_WRITABLE|ATTR_MULTI_VALUE,0};

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
		return -1;
	}
	result=immHandler.defineClass( nameClassToBeDefined,categoryClassToBeDefined,classAttributes);
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: defineClass FAILURE!!!\n";
		return -1;
	}else{
		cout<<"Class "<<nameClassToBeDefined<<" defined.\n";
	}


	return 0;
}
