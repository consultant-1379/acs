/*
 * AdminOperation_MyImplementer.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#include "AdminOperation_MyImplementer.h"
#include "acs_apgcc_omhandler.h"


int createObjectResult(OmHandler &omHandler, const char * rdnObject, const char *operationName );
int saveResult(OmHandler &omHandler, const char *objectName, const char *operation, int value);
int deleteObject(OmHandler &immHandler, const char * nomeObject);

AdminOperation_MyImplementer::AdminOperation_MyImplementer() {
	// TODO Auto-generated constructor stub

}


void AdminOperation_MyImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList) {

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

	vector<int> vettoreInteri;

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
			cout << " attrName: " << paramList[i]->attrName << "\t value: " <<*reinterpret_cast<double *>(paramList[i]->attrValues) << endl;
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
	int retVal;



	switch ( operationId ){
		case 1:
			{

				cout <<"Admin operation 1, return value 1" <<endl;

				retVal = 1;

				this->adminOperationResult( oiHandle , invocation, retVal );

			}
				break;
		case 2:
		{
			cout <<"Admin operation 2, return value 2" <<endl;
			cout <<"sleep 3 second before returns" << endl;
			sleep(3);
			retVal = 2;

			this->adminOperationResult( oiHandle , invocation, retVal );
		}
		break;

		default:{
			cout << " .... : " << operationId << endl;
			retVal = 28;
			this->adminOperationResult( oiHandle , invocation, retVal );

		}
			break;
		}
		return;
}


