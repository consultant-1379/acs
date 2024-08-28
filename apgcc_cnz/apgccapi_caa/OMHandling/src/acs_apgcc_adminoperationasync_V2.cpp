/*
 * acs_apgcc_adminoperationasync_V2.cpp
 *
 *  Created on: Nov 15, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperationasync_V2_R1.h"
#include "acs_apgcc_adminoperationasync_V2_R1_impl.h"

/*default constructor*/
acs_apgcc_adminoperationasync_V2_R1::acs_apgcc_adminoperationasync_V2_R1() {
	adminoperationAsync_impl = new acs_apgcc_adminoperationasync_V2_R1_impl;
}

/*constructor*/
acs_apgcc_adminoperationasync_V2_R1::acs_apgcc_adminoperationasync_V2_R1(acs_apgcc_adminoperationasync_V2_R1 &objRef) {
	this->adminoperationAsync_impl = new acs_apgcc_adminoperationasync_V2_R1_impl(objRef.adminoperationAsync_impl);
}

/*init**/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1::init ( ){
	return adminoperationAsync_impl->init(this);
}

/*finalize*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1::finalize ( ){
	return adminoperationAsync_impl->finalize();
}

/*adminOperationInvoke*/

ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1::adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector
		){
	return adminoperationAsync_impl->adminOperationInvokeAsync( invocation, p_objName, continuationId, operationId, paramVector );
}

/*int getInternalLastError();*/

int acs_apgcc_adminoperationasync_V2_R1::getInternalLastError(){
	return adminoperationAsync_impl->getInternalLastError();
}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperationasync_V2_R1::getInternalLastErrorText(){
	return adminoperationAsync_impl->getInternalLastErrorText();
}

/*getSelObj*/
int acs_apgcc_adminoperationasync_V2_R1::getSelObj() const{
	return adminoperationAsync_impl->getSelObj();
}

/*dispatch*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync_V2_R1::dispatch(ACS_APGCC_DispatchFlags p_flag) {
	return adminoperationAsync_impl->dispatch(p_flag);
}

acs_apgcc_adminoperationasync_V2_R1& acs_apgcc_adminoperationasync_V2_R1::operator=(const acs_apgcc_adminoperationasync_V2_R1 &admOpAsync){
	ACE_UNUSED_ARG(admOpAsync);

	return *this;
}

acs_apgcc_adminoperationasync_V2_R1::~acs_apgcc_adminoperationasync_V2_R1() {
	delete adminoperationAsync_impl;
}
