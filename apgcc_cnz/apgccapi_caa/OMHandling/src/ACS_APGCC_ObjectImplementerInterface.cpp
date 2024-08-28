/*
 * ACS_APGCC_ObjectImplementerInterface.cpp
 *
 *  Created on: Sep 2, 2010
 *      Author: xfabron
 */



#include "ACS_APGCC_ObjectImplementerInterface.h"
#include "acs_apgcc_objectimplementerinterface_impl.h"
#include "ace/Reactor.h"
#include <iostream>
#include <vector>



ACS_APGCC_ObjectImplementerInterface::ACS_APGCC_ObjectImplementerInterface():objName(), impName(){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_impl;
}



ACS_APGCC_ObjectImplementerInterface::ACS_APGCC_ObjectImplementerInterface(string p_impName ):objName(), impName(p_impName){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_impl(p_impName);
}



ACS_APGCC_ObjectImplementerInterface::ACS_APGCC_ObjectImplementerInterface(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) {

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_impl(p_objName, p_impName, p_scope);

}



std::string ACS_APGCC_ObjectImplementerInterface::getObjName(){

	return objectImplInt_impl->getObjName();
}



void ACS_APGCC_ObjectImplementerInterface::setObjName(std::string p_objName){

	objectImplInt_impl->setObjName(p_objName);

}



std::string ACS_APGCC_ObjectImplementerInterface::getImpName(){

	return objectImplInt_impl->getImpName();
}



void ACS_APGCC_ObjectImplementerInterface::setImpName(std::string p_impName){

	objectImplInt_impl->setImpName(p_impName);

}



void ACS_APGCC_ObjectImplementerInterface::setImmOiHandle(SaUint64T immOiHandle){

	objectImplInt_impl->setImmOiHandle(immOiHandle);

}



SaUint64T ACS_APGCC_ObjectImplementerInterface::getImmOiHandle(){

	return objectImplInt_impl->getImmOiHandle();

}



ACS_APGCC_ScopeT ACS_APGCC_ObjectImplementerInterface::getScope(){

	return objectImplInt_impl->getScope();
}



void ACS_APGCC_ObjectImplementerInterface::setScope(ACS_APGCC_ScopeT p_scope){

	objectImplInt_impl->setScope(p_scope);

}



void ACS_APGCC_ObjectImplementerInterface::setSelObj(SaSelectionObjectT p_selObj){

	objectImplInt_impl->setSelObj(p_selObj);

}



int ACS_APGCC_ObjectImplementerInterface::getSelObj() const{

	return objectImplInt_impl->getSelObj();

}



ACS_CC_ReturnType ACS_APGCC_ObjectImplementerInterface::dispatch(ACS_APGCC_DispatchFlags p_flag){

	return objectImplInt_impl->dispatch(p_flag);
}



/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType ACS_APGCC_ObjectImplementerInterface::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	return objectImplInt_impl->modifyRuntimeObj(p_objName, p_attributeValue);

}


ACS_APGCC_ObjectImplementerInterface::ACS_APGCC_ObjectImplementerInterface(ACS_APGCC_ObjectImplementerInterface &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

}



ACS_APGCC_ObjectImplementerInterface& ACS_APGCC_ObjectImplementerInterface::operator=(const ACS_APGCC_ObjectImplementerInterface &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

	return *this;

}

//OM.OI_OM_ERROR_COM
//void ACS_APGCC_ObjectImplementerInterface::setExitCode(int p_exitCode){
void ACS_APGCC_ObjectImplementerInterface::setExitCode(int p_exitCode, string p_exitCodeMessage){

	objectImplInt_impl->setExitCode(p_exitCode, p_exitCodeMessage);
}

string ACS_APGCC_ObjectImplementerInterface::getExitCodeString(){

	return objectImplInt_impl->getExitCodeString();
}

void ACS_APGCC_ObjectImplementerInterface::setExitCode(int p_exitCode){
	return objectImplInt_impl->setExitCode(p_exitCode);

}

SaAisErrorT ACS_APGCC_ObjectImplementerInterface::getExitCode(){

	return objectImplInt_impl->getExitCode();

}

ACS_APGCC_ObjectImplementerInterface::~ACS_APGCC_ObjectImplementerInterface(){

	delete objectImplInt_impl;

}

