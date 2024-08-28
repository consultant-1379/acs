/*
 * Objecthandler.cpp
 *
 *  Created on: Sep 29, 2010
 *      Author: xfabron
 */

#include "Objecthandler.h"

ObjectHandler::ObjectHandler(){

}

ACS_CC_ReturnType ObjectHandler::updateCallback(const char* p_objName, const char** p_attrName){

	printf("------------------------------------------------------------\n");
	printf("                   updateCallbak called                     \n");
	printf("------------------------------------------------------------\n");

	cout<<"Object Name: "<<p_objName<<endl;

	while(*p_attrName != 0){
		cout<<"Attribute Name: "<<*p_attrName<<endl;

		p_attrName++;
	}


	printf("------------------------------------------------------------\n");
	return ACS_CC_SUCCESS;

}


void ObjectHandler:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType**paramList) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;

	/*start*/
	cout << endl;

	int dim=0;
	int i=0;
	while(paramList[i]){
		i++;
		dim++;
	}

	i = 0;
	while( paramList[i] ){
		switch ( paramList[i]->attrType ) {
		case ATTR_INT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT32T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned int *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_INT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_UINT64T:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<unsigned long long *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_FLOATT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<float *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_DOUBLET:
			cout << " paramListName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(paramList[i]->attrValues) << endl;
			break;
		case ATTR_NAMET:
		{
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;

		}
			break;
		case ATTR_STRINGT:
			cout << " attrName: " << paramList[i]->attrName << "\t value: " << reinterpret_cast<char *>(paramList[i]->attrValues) << endl;
			break;
		default:
			break;

		}
		i++;
	}

	cout << " Implementing Admin Operation with ID = " << operationId << endl;

	/////////////RETURN PARAMETER//////////////
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOUT;

	ACS_APGCC_AdminOperationParamType Param_1;
	ACS_APGCC_AdminOperationParamType Param_2;
	ACS_APGCC_AdminOperationParamType Param_3;
	ACS_APGCC_AdminOperationParamType Param_4;
	ACS_APGCC_AdminOperationParamType Param_5;
	ACS_APGCC_AdminOperationParamType Param_6;
	ACS_APGCC_AdminOperationParamType Param_7;
	ACS_APGCC_AdminOperationParamType Param_8;
	ACS_APGCC_AdminOperationParamType Param_w;
	char attName1[]= "Param_1";
	Param_1.attrName =attName1;
	Param_1.attrType=ATTR_FLOATT;
	float floatValue = 666.999;
	Param_1.attrValues=reinterpret_cast<void*>(&floatValue);

	char attName2[]= "Param_2";
	Param_2.attrName =attName2;
	Param_2.attrType=ATTR_INT32T ;
	int intValue = -123456;
	Param_2.attrValues=reinterpret_cast<void*>(&intValue);

	char attName3[]= "Param_3";
	Param_3.attrName =attName3;
	Param_3.attrType=ATTR_INT64T ;
	long long longlongValue = -999999999;
	Param_3.attrValues=reinterpret_cast<void*>(&longlongValue);

	char attName4[]= "Param_4";
	Param_4.attrName =attName4;
	Param_4.attrType=ATTR_UINT32T ;
	unsigned int uintValue = 7777777;
	Param_4.attrValues=reinterpret_cast<void*>(&uintValue);

	char attName5[]= "Param_5";
	Param_5.attrName =attName5;
	Param_5.attrType=ATTR_UINT64T ;
	unsigned long long ulonglongValue = 555555555555555;
	Param_5.attrValues=reinterpret_cast<void*>(&ulonglongValue);

	char attName6[]= "Param_6";
	Param_6.attrName =attName6;
	Param_6.attrType=ATTR_DOUBLET ;
	double doubleValue = 555555555555555;
	Param_6.attrValues=reinterpret_cast<void*>(&doubleValue);
		/*the following lines commented because of tr*/
//		char attName7[]= "Param_7";
//		Param_7.attrName =attName7;
//		Param_7.attrType=ATTR_NAMET ;
//		char charValue[] = "This is a NAMET Param";
//		Param_7.attrValues=reinterpret_cast<void*>(const_cast<char *>(charValue));
//
//		char attName8[]= "Param_8";
//		Param_8.attrName =attName8;
//		Param_8.attrType=ATTR_STRINGT ;
//		char stringValue[] = "This is a STRINGT Param";
//		Param_8.attrValues=reinterpret_cast<void*>(const_cast<char *>(stringValue));
	///////END RETURN PATAMETER////////////////////

	int retVal =0;
	if (operationId > 10){

		retVal =1;
		//cout << " ..... returning  = " << retVal << endl;
		/*****************************/
		/*insert into the vector*/

		vectorOUT.push_back(Param_1);
		vectorOUT.push_back(Param_2);
		vectorOUT.push_back(Param_3);
		vectorOUT.push_back(Param_4);
		vectorOUT.push_back(Param_5);
		vectorOUT.push_back(Param_6);
//		vectorOUT.push_back(Param_7);
//		vectorOUT.push_back(Param_8);
//		vectorOUT.push_back(Param_1);
//		vectorOUT.push_back(Param_2);
//		vectorOUT.push_back(Param_3);
//		vectorOUT.push_back(Param_4);
//		vectorOUT.push_back(Param_5);
//		vectorOUT.push_back(Param_6);
		if (operationId == 12)
			vectorOUT.push_back(Param_w);/*wrong parameter added only to generate a failure in return adminoperation*/


		//cout << " before returning adminOperation " << endl;
		sleep(11);
		this->adminOperationResult( oiHandle , invocation, retVal, vectorOUT );
		//cout << " after return adminOperation " << endl;
		/*****************************/

		return;
	}
		sleep (3);

		//	ACS_CC_ReturnType  adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result);
		retVal = 28;
		cout << " ..... returning  = " << retVal << endl;

	this->adminOperationResult( oiHandle , invocation, retVal );
	/*end*/


	printf("------------------------------------------------------------\n");


}
