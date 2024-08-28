/*
 * MyClassImplementer.cpp
 *
 *  Created on: Dec 22, 2010
 *      Author: xpaomaz
 */

#include "MyClassImplementer.h"
#include <stdio.h>
#include <string.h>
using namespace std;

//MyClassImplementer::MyClassImplementer() {
//	// TODO Auto-generated constructor stub
//
//}

MyClassImplementer::~MyClassImplementer() {
	// TODO Auto-generated destructor stub
}

ACS_CC_ReturnType MyClassImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " class Name: " << className << endl;
	cout << " parent Name: " << parentName << endl;

	cout << endl;

	int toReturn = 0;

	int dim=0;
	int i=0;
	while(attr[i]){
		i++;
		dim++;
	}

	i = 0;
	while( attr[i] ){
		switch ( attr[i]->attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " attrName: " << attr[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(attr[i]->attrValues[0]) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << attr[i]->attrName << "\t value: " << reinterpret_cast<char *>(attr[i]->attrValues[0]) << endl;
			std::string nome(reinterpret_cast<const char *>(attr[i]->attrName));
			if (nome.compare("RDN_Attribute") ==0 )
			{
				std::string s(reinterpret_cast<const char *>(attr[i]->attrValues[0]));
				if(s.compare("provaConfObj2") == 0)
					toReturn = -1;
				cout << " S string: " << s << endl;
			}
		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << attr[i]->attrName << "\t value: " << reinterpret_cast<char *>(attr[i]->attrValues[0]) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	/*added*/
	//setExitCode(10, "CODE_SET_10");
	cout << endl;
	
	if (toReturn < 0){
		cout << " ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}
		

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType MyClassImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	cout << endl;

	//setExitCode(11);
	//setExitCode(11, "CODE_SET_11");
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType MyClassImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){

	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	int toReturn = 0;

	cout << endl;

	int i = 0;
	while( attrMods[i] ){
		switch ( attrMods[i]->modAttr.attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<long long *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_FLOATT:
		{
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<float *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			std::string nome(reinterpret_cast<const char *>(attrMods[i]->modAttr.attrName ));
			if (nome.compare("ATTRIBUTE_1") ==0 )
			{
				float s(*(reinterpret_cast<float *>(attrMods[i]->modAttr.attrValues[0])));
				if(s >  100)
					toReturn = -1;
				cout << " S float: " << s << endl;
			}
		}

			break;
		case ATTR_DOUBLET:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " <<*reinterpret_cast<double *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_NAMET:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " << reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << attrMods[i]->modAttr.attrName << "\t value: " << reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]) << endl;
			break;
		default:
			break;

		}
		i++;
	}


	cout << endl;
//	OmHandler omHandler;
//	ACS_CC_ImmParameter paramToFind;
//	ACS_CC_ReturnType resultGet;
//
//	resultGet = omHandler.Init();
//	unsigned long long int ccbValRetrieved;
//	if (resultGet == ACS_CC_SUCCESS ){
//		char  nomeccb[] = "ccbClassValue" ;
//		char* nomeOBJCCB = const_cast<char *>("ccbClassRDN=1,serviceId=1,safApp=safImmService");
//		paramToFind.attrName = nomeccb;
//		resultGet = omHandler.getAttribute(nomeOBJCCB,&paramToFind);
//		if (resultGet != ACS_CC_SUCCESS)
//			cout << " MyClassImplementer::modify::omHandler.getAttribute FAILURE" << endl;
//		else{
//			ccbValRetrieved =  *(unsigned long long int*)paramToFind.attrValues[0];
//
//			cout << " MyClassImplementer::modify::omHandler.getAttribute ccbValRetrieved=" << ccbValRetrieved<<endl;
//		}
//		//setExitCode(12);
//		//setExitCode(10, "CODE_SET_12");
//		return ACS_CC_SUCCESS;
//	}

	//setExitCode(13);
	//setExitCode(13, "CODE_SET_13");
	if (toReturn < 0){
		cout << "MyClassImplementer::modify:: ACS_CC_FAILURE " << endl;
		return ACS_CC_FAILURE;
	}
	cout << "MyClassImplementer::modify:: ACS_CC_Success " << endl;
	return ACS_CC_SUCCESS;


}

ACS_CC_ReturnType MyClassImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
	//setExitCode(0, "SUCCESS_EXIT_CODE");
	return ACS_CC_SUCCESS;


}

void MyClassImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){

	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}

void MyClassImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){


	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << endl;
}


ACS_CC_ReturnType MyClassImplementer::updateRuntime(const char* p_objName, const char* p_attrName){

	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;
	cout<<"Attribute Name: "<<p_attrName<<endl;


	printf("------------------------------------------------------------\n");
	return ACS_CC_SUCCESS;

}
