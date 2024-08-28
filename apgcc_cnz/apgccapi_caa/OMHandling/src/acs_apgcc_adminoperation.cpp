/*
 * ACS_APGCC_AdminOperation.cpp
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperation.h"
#include "acs_apgcc_adminoperation_impl.h"

/*default constructor*/
acs_apgcc_adminoperation::acs_apgcc_adminoperation() {

	adminoperation_impl = new acs_apgcc_adminoperation_impl;

}

/*constructor*/
acs_apgcc_adminoperation::acs_apgcc_adminoperation(acs_apgcc_adminoperation &objRef) {

	this->adminoperation_impl = new acs_apgcc_adminoperation_impl(objRef.adminoperation_impl);

}

/*init**/

ACS_CC_ReturnType acs_apgcc_adminoperation::init ( ){

	return adminoperation_impl->init();
}

/*finalize*/

ACS_CC_ReturnType acs_apgcc_adminoperation::finalize ( ){

	return adminoperation_impl->finalize();
}

/*adminOperationInvoke*/

ACS_CC_ReturnType acs_apgcc_adminoperation::adminOperationInvoke ( const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector,
		int* returnVal,
		long long int timeoutVal){

	return adminoperation_impl->adminOperationInvoke(p_objName, continuationId, operationId, paramVector, returnVal, timeoutVal );

}

/*int getInternalLastError();*/

int acs_apgcc_adminoperation::getInternalLastError(){

	return adminoperation_impl->getInternalLastError();

}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperation::getInternalLastErrorText(){

	return adminoperation_impl->getInternalLastErrorText();

}

acs_apgcc_adminoperation::~acs_apgcc_adminoperation() {

	delete adminoperation_impl;
}

ACS_CC_ReturnType acs_apgcc_adminoperation::adminOperationInvoke ( const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector,
		int* returnVal,
		long long int timeoutVal,
		vector<ACS_APGCC_AdminOperationParamType> &outparamVector){

	return adminoperation_impl->adminOperationInvoke(p_objName, continuationId, operationId, paramVector, returnVal, timeoutVal, outparamVector );

}

ACS_CC_ReturnType acs_apgcc_adminoperation::freeadminOperationReturnValue ( vector<ACS_APGCC_AdminOperationParamType> &outparamVector ){

	return adminoperation_impl->freeadminOperationReturnValue ( outparamVector );

}
