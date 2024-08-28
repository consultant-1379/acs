//============================================================================
// Name        : createRuntimeClass.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "OmHandler.h"
#include "ACS_CC_Types.h"


using namespace std;

/* Name of the class to be defined*/
char *nameClass = const_cast<char *>("runtimeClass_test");

int getInputParameter();
ACS_CC_ReturnType createClass(OmHandler *p_immHandler);
ACS_CC_ReturnType deleteClass(OmHandler *p_immHandler);


int getInputParameter(int argc, char * argv[]){

	int c = 0;
	int flag = -1;

	while( (c = getopt(argc, argv, "c d")) != -1){
		switch (c) {
			case 'c':
				flag = 1;
				break;
			case 'd':
				flag = 2;
			case '?':
				if( argc > 2){
					ACE_OS::fprintf(stdout, "error - Too many arguments\n ");
					return -1;
				}
				if( optopt != 0){
					ACE_OS::fprintf(stdout, "error - Unrecognized format\n", optopt);
					return -1;
				}

				break;
			default:
				break;
		}//END switch(c)
	}

	if(flag == -1){
		cout<<"error - Insert arguments:\n -c to create a class\n -d to delete a class  \n "<<endl;
		return -1;
	}

	return flag;

}


ACS_CC_ReturnType createClass(OmHandler *p_immHandler){


	ACS_CC_ReturnType result;

	/*Category of class to be defined*/
	ACS_CC_ClassCategoryType  categoryClassToBeDefined = RUNTIME;

	/*Attributes list of the class to be defined */
	vector<ACS_CC_AttrDefinitionType> classAttributes;

	ACS_CC_AttrDefinitionType attributeRDN = {const_cast<char*>("RDN_Attribute"),ATTR_NAMET,ATTR_RDN|ATTR_RUNTIME|ATTR_CACHED,0} ;
	ACS_CC_AttrDefinitionType attrFLOAT_C = {const_cast<char*>("ATTR_FLOAT_C"), ATTR_FLOATT, ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrINT64_C = {const_cast<char*>("ATTR_INT64_C"),ATTR_INT64T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attrINT32_C = {const_cast<char*>("ATTR_INT32_C"),ATTR_INT32T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attrDOUBLE_C = {const_cast<char*>("ATTR_DOUBLE_C"),ATTR_DOUBLET,ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrUNSI64_C = {const_cast<char*>("ATTR_UNSI64_C"),ATTR_UINT64T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attrUNSI32_C = {const_cast<char*>("ATTR_UNSI32_C"),ATTR_UINT32T,ATTR_RUNTIME|ATTR_CACHED,0};
	ACS_CC_AttrDefinitionType attrNAMET_C = {const_cast<char*>("ATTR_NAMET_C"), ATTR_NAMET, ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrSTRING_C = {const_cast<char*>("ATTR_STRING_C"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_CACHED|ATTR_MULTI_VALUE,0};

	//Runtime not-cached attribute

	ACS_CC_AttrDefinitionType attrFLOAT_NC = {const_cast<char*>("ATTR_FLOAT_NC"), ATTR_FLOATT, ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrINT64_NC = {const_cast<char*>("ATTR_INT64_NC"),ATTR_INT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attrINT32_NC = {const_cast<char*>("ATTR_INT32_NC"),ATTR_INT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attrDOUBLE_NC = {const_cast<char*>("ATTR_DOUBLE_NC"),ATTR_DOUBLET,ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrUNSI64_NC = {const_cast<char*>("ATTR_UNSI64_NC"),ATTR_UINT64T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attrUNSI32_NC = {const_cast<char*>("ATTR_UNSI32_NC"),ATTR_UINT32T,ATTR_RUNTIME,0};
	ACS_CC_AttrDefinitionType attrNAMET_NC = {const_cast<char*>("ATTR_NAMET_NC"), ATTR_NAMET, ATTR_RUNTIME|ATTR_MULTI_VALUE,0};
	ACS_CC_AttrDefinitionType attrSTRING_NC = {const_cast<char*>("ATTR_STRING_NC"),ATTR_STRINGT,ATTR_RUNTIME|ATTR_MULTI_VALUE,0};

	classAttributes.push_back(attributeRDN);
	classAttributes.push_back(attrFLOAT_C);
	classAttributes.push_back(attrINT64_C);
	classAttributes.push_back(attrINT32_C);
	classAttributes.push_back(attrDOUBLE_C);
	classAttributes.push_back(attrUNSI64_C);
	classAttributes.push_back(attrUNSI32_C);
	classAttributes.push_back(attrNAMET_C);
	classAttributes.push_back(attrSTRING_C);

	classAttributes.push_back(attrFLOAT_NC);
	classAttributes.push_back(attrINT64_NC);
	classAttributes.push_back(attrINT32_NC);
	classAttributes.push_back(attrDOUBLE_NC);
	classAttributes.push_back(attrUNSI64_NC);
	classAttributes.push_back(attrUNSI32_NC);
	classAttributes.push_back(attrNAMET_NC);
	classAttributes.push_back(attrSTRING_NC);


	result = p_immHandler->defineClass( nameClass,categoryClassToBeDefined,classAttributes);

	return result;

}


ACS_CC_ReturnType deleteClass(OmHandler *p_immHandler){

	ACS_CC_ReturnType result;

	result = p_immHandler->removeClass(nameClass);

	return result;
}


int main(int argc, char * argv[]) {

	int flag = 0;

//	while( (c = getopt(argc, argv, "c d")) != -1){
//		switch (c) {
//			case 'c':
//
//				flag = 1;
//
//				break;
//
//			case 'd':
//
//				flag = 2;
//
//			case '?':
//				if( argc > 2){
//					ACE_OS::fprintf(stdout, "error - Too many arguments\n ");
//					return -1;
//				}
//				if( optopt != 0){
//					ACE_OS::fprintf(stdout, "error - Unrecognized format\n", optopt);
//					return -1;
//				}
//
//				break;
//			default:
//				break;
//		}//END switch(c)
//	}
//
//	if(flag == 0){
//		cout<<"error - Insert arguments:\n -c to create a class\n -d to delete a class  \n "<<endl;
//		return 0;
//	}

	flag = getInputParameter(argc, argv);

	if(flag == -1){
		return -1;
	}


	ACS_CC_ReturnType result;
	OmHandler immHandler;



	result=immHandler.Init();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: init FAILURE!!!\n";
	}

	if(flag == 1){

		result = createClass(&immHandler);
		if(result != ACS_CC_SUCCESS){
			cout<<"ERROR: define Class "<< nameClass <<" FAILURE!!!\n";
		}else{
			cout<<"Class "<< nameClass <<" defined.\n";
		}

	}else if(flag ==2){
		result = deleteClass(&immHandler);

		if(result!=ACS_CC_SUCCESS){
			cout<<"ERROR: remove Class "<< nameClass <<" FAILURE!!!\n";
			return -1;
		}else{
			cout<<"Class "<<nameClass<<" deleted.\n";
		}
	}


	result=immHandler.Finalize();
	if(result!=ACS_CC_SUCCESS){
		cout<<"ERROR: finalize FAILURE!!!\n";
	}


	return 0;
}
