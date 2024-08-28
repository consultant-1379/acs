/*
 * acs_apgcc_objectimplementerinterface_V2.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_objectimplementerinterface_V2.h"
#include "acs_apgcc_objectimplementerinterface_V2_impl.h"
#include "ace/Reactor.h"
#include <iostream>
#include <vector>


/*start**/


acs_apgcc_objectimplementerinterface_V2::acs_apgcc_objectimplementerinterface_V2():objName(), impName(){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V2_impl;
}



acs_apgcc_objectimplementerinterface_V2::acs_apgcc_objectimplementerinterface_V2(string p_impName ):objName(), impName(p_impName){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V2_impl(p_impName);
}



acs_apgcc_objectimplementerinterface_V2::acs_apgcc_objectimplementerinterface_V2(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) {

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V2_impl(p_objName, p_impName, p_scope);

}



std::string acs_apgcc_objectimplementerinterface_V2::getObjName(){

	return objectImplInt_impl->getObjName();
}



void acs_apgcc_objectimplementerinterface_V2::setObjName(std::string p_objName){

	objectImplInt_impl->setObjName(p_objName);

}



std::string acs_apgcc_objectimplementerinterface_V2::getImpName(){

	return objectImplInt_impl->getImpName();
}



void acs_apgcc_objectimplementerinterface_V2::setImpName(std::string p_impName){

	objectImplInt_impl->setImpName(p_impName);

}



void acs_apgcc_objectimplementerinterface_V2::setImmOiHandle(SaUint64T immOiHandle){

	objectImplInt_impl->setImmOiHandle(immOiHandle);

}



SaUint64T acs_apgcc_objectimplementerinterface_V2::getImmOiHandle(){

	return objectImplInt_impl->getImmOiHandle();

}



ACS_APGCC_ScopeT acs_apgcc_objectimplementerinterface_V2::getScope(){

	return objectImplInt_impl->getScope();
}



void acs_apgcc_objectimplementerinterface_V2::setScope(ACS_APGCC_ScopeT p_scope){

	objectImplInt_impl->setScope(p_scope);

}



void acs_apgcc_objectimplementerinterface_V2::setSelObj(SaSelectionObjectT p_selObj){

	objectImplInt_impl->setSelObj(p_selObj);

}



int acs_apgcc_objectimplementerinterface_V2::getSelObj() const{

	return objectImplInt_impl->getSelObj();

}



ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::dispatch(ACS_APGCC_DispatchFlags p_flag){

	return objectImplInt_impl->dispatch(p_flag);
}



/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	return objectImplInt_impl->modifyRuntimeObj(p_objName, p_attributeValue);

}


/*adminOperationResult*/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	return objectImplInt_impl->adminOperationResult(oiHandle, invocation, result);
}

acs_apgcc_objectimplementerinterface_V2::acs_apgcc_objectimplementerinterface_V2(acs_apgcc_objectimplementerinterface_V2 &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

}



acs_apgcc_objectimplementerinterface_V2& acs_apgcc_objectimplementerinterface_V2::operator=(const acs_apgcc_objectimplementerinterface_V2 &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

	return *this;

}

//OM.OI_OM_ERROR_COM
//void acs_apgcc_objectimplementerinterface_V2::setExitCode(int p_exitCode){
void acs_apgcc_objectimplementerinterface_V2::setExitCode(int p_exitCode, string p_exitCodeMessage){

	objectImplInt_impl->setExitCode(p_exitCode, p_exitCodeMessage);
}

string acs_apgcc_objectimplementerinterface_V2::getExitCodeString(){

	return objectImplInt_impl->getExitCodeString();
}

void acs_apgcc_objectimplementerinterface_V2::setExitCode(int p_exitCode){
	return objectImplInt_impl->setExitCode(p_exitCode);

}

SaAisErrorT acs_apgcc_objectimplementerinterface_V2::getExitCode(){

	return objectImplInt_impl->getExitCode();

}

/*1963*/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::getCcbAugmentationInitialize(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, ACS_APGCC_CcbHandle* ccbHandle, ACS_APGCC_AdminOwnerHandle* adminOwnerHandler){

	return objectImplInt_impl->getCcbAugmentationInitialize(oiHandle, ccbId, ccbHandle, adminOwnerHandler );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::createObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList){

	return objectImplInt_impl->createObjectAugmentCcb( ccbHandle, adminOwnerHandler, p_className, p_parentName, p_attrValuesList );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::modifyObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue){

	return objectImplInt_impl->modifyObjectAugmentCcb( ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::deleteObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* m_objectName){

	return objectImplInt_impl->deleteObjectAugmentCcb( ccbHandle, adminOwnerHandler, m_objectName );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V2::applyAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle){

	return objectImplInt_impl->applyAugmentCcb( ccbHandle);

}

int acs_apgcc_objectimplementerinterface_V2::getInternalLastError(){

	return objectImplInt_impl->getInternalLastError();
}


const char* acs_apgcc_objectimplementerinterface_V2::getInternalLastErrorText(){


	return objectImplInt_impl->getInternalLastErrorText();

}
/*end 1963*/


acs_apgcc_objectimplementerinterface_V2::~acs_apgcc_objectimplementerinterface_V2(){

	delete objectImplInt_impl;

}



/*end**/
