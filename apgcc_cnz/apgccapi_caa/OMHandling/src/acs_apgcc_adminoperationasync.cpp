/*
 * acs_apgcc_adminoperationasync.cpp
 *
 *  Created on: Jul 10, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperationasync.h"
#include "acs_apgcc_adminoperationasync_impl.h"

/*default constructor*/
acs_apgcc_adminoperationasync::acs_apgcc_adminoperationasync() {

	adminoperationAsync_impl = new acs_apgcc_adminoperationasync_impl;

}

/*constructor*/
acs_apgcc_adminoperationasync::acs_apgcc_adminoperationasync(acs_apgcc_adminoperationasync &objRef) {

	this->adminoperationAsync_impl = new acs_apgcc_adminoperationasync_impl(objRef.adminoperationAsync_impl);


}

/*init**/
ACS_CC_ReturnType acs_apgcc_adminoperationasync::init ( ){

	return adminoperationAsync_impl->init(this);
}

/*finalize*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync::finalize ( ){

	return adminoperationAsync_impl->finalize();
}

/*adminOperationInvoke*/

ACS_CC_ReturnType acs_apgcc_adminoperationasync::adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_ContinuationIdType continuationId,
		ACS_APGCC_AdminOperationIdType operationId,
		vector<ACS_APGCC_AdminOperationParamType> paramVector
		){

	return adminoperationAsync_impl->adminOperationInvokeAsync( invocation, p_objName, continuationId, operationId, paramVector );

}

/*int getInternalLastError();*/

int acs_apgcc_adminoperationasync::getInternalLastError(){

	return adminoperationAsync_impl->getInternalLastError();

}

/*char* getInternalLastErrorText();*/
char* acs_apgcc_adminoperationasync::getInternalLastErrorText(){

	return adminoperationAsync_impl->getInternalLastErrorText();

}

/*getSelObj*/
int acs_apgcc_adminoperationasync::getSelObj() const{

	return adminoperationAsync_impl->getSelObj();

}

/*dispatch*/
ACS_CC_ReturnType acs_apgcc_adminoperationasync::dispatch(ACS_APGCC_DispatchFlags p_flag) {

	return adminoperationAsync_impl->dispatch(p_flag);

}

acs_apgcc_adminoperationasync& acs_apgcc_adminoperationasync::operator=(const acs_apgcc_adminoperationasync &admOpAsync){

	ACE_UNUSED_ARG(admOpAsync);

	return *this;
}

acs_apgcc_adminoperationasync::~acs_apgcc_adminoperationasync() {

	delete adminoperationAsync_impl;
}
