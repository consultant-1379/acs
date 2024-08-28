/*
 * acs_apgcc_adminoperationimplementer.cpp
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_adminoperationimplementer.h"
#include "acs_apgcc_adminoperationimplementer_impl.h"
#include "ace/OS.h"

/*default constructor*/
acs_apgcc_adminoperationimplementer::acs_apgcc_adminoperationimplementer() {

	adminOperationImplementerImpl = new acs_apgcc_adminoperationimplementer_impl;

}

/*copy constructor*/
acs_apgcc_adminoperationimplementer::acs_apgcc_adminoperationimplementer(acs_apgcc_adminoperationimplementer& obj) {

	this->adminOperationImplementerImpl = new acs_apgcc_adminoperationimplementer_impl(obj.adminOperationImplementerImpl);


}

/*init*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::init(std::string p_implementerName) {

	return adminOperationImplementerImpl->init(p_implementerName, this);
}

/*setObjectImplemented*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::setObjectImplemented(std::string p_objName) {

	return adminOperationImplementerImpl->setObjectImplemented(p_objName);

}

/*setClassImplemented*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::setClassImplemented(std::string p_className) {

	return adminOperationImplementerImpl->setClassImplemented(p_className);

}

/*dispatch*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::dispatch(ACS_APGCC_DispatchFlags p_flag) {

	return adminOperationImplementerImpl->dispatch(p_flag);

}
/*releaseObjectImplemented*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::releaseObjectImplemented(std::string p_objName) {

	return adminOperationImplementerImpl->releaseObjectImplemented(p_objName);

}

/*releaseClassImplemented*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::releaseClassImplemented(std::string p_className) {

	return adminOperationImplementerImpl->releaseClassImplemented(p_className);

}


///*finalize*/
//ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::finalize() {
//
//	return adminOperationImplementerImpl->finalize();
//
//}

/*getSelObj*/
int acs_apgcc_adminoperationimplementer::getSelObj() const{

	return adminOperationImplementerImpl->getSelObj();

}
/*getInternalLastError*/
int acs_apgcc_adminoperationimplementer::getInternalLastError(){

	return adminOperationImplementerImpl->getInternalLastError();
}


/*getInternalLastErrorText*/
char* acs_apgcc_adminoperationimplementer::getInternalLastErrorText(){

	return adminOperationImplementerImpl->getInternalLastErrorText();
}

/*adminOperationResult*/
ACS_CC_ReturnType acs_apgcc_adminoperationimplementer::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	return adminOperationImplementerImpl->adminOperationResult(oiHandle, invocation, result);
}

acs_apgcc_adminoperationimplementer& acs_apgcc_adminoperationimplementer::operator=(const acs_apgcc_adminoperationimplementer &admOpImpl){

	ACE_UNUSED_ARG(admOpImpl);

	return *this;
}

acs_apgcc_adminoperationimplementer::~acs_apgcc_adminoperationimplementer() {

	delete adminOperationImplementerImpl;
}
