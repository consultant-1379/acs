/*
 * acs_apgcc_objectimplementerinterface_V3.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_objectimplementerinterface_V3_impl.h"
#include "ace/Reactor.h"
#include <iostream>
#include <vector>


/*start**/


acs_apgcc_objectimplementerinterface_V3::acs_apgcc_objectimplementerinterface_V3():objName(), impName(){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V3_impl;
}



acs_apgcc_objectimplementerinterface_V3::acs_apgcc_objectimplementerinterface_V3(string p_impName ):objName(), impName(p_impName){

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V3_impl(p_impName);
}



acs_apgcc_objectimplementerinterface_V3::acs_apgcc_objectimplementerinterface_V3(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ) {

	objectImplInt_impl = new acs_apgcc_objectimplementerinterface_V3_impl(p_objName, p_impName, p_scope);

}



std::string acs_apgcc_objectimplementerinterface_V3::getObjName(){

	return objectImplInt_impl->getObjName();
}



void acs_apgcc_objectimplementerinterface_V3::setObjName(std::string p_objName){

	objectImplInt_impl->setObjName(p_objName);

}



std::string acs_apgcc_objectimplementerinterface_V3::getImpName(){

	return objectImplInt_impl->getImpName();
}



void acs_apgcc_objectimplementerinterface_V3::setImpName(std::string p_impName){

	objectImplInt_impl->setImpName(p_impName);

}



void acs_apgcc_objectimplementerinterface_V3::setImmOiHandle(SaUint64T immOiHandle){

	objectImplInt_impl->setImmOiHandle(immOiHandle);

}



SaUint64T acs_apgcc_objectimplementerinterface_V3::getImmOiHandle(){

	return objectImplInt_impl->getImmOiHandle();

}



ACS_APGCC_ScopeT acs_apgcc_objectimplementerinterface_V3::getScope(){

	return objectImplInt_impl->getScope();
}



void acs_apgcc_objectimplementerinterface_V3::setScope(ACS_APGCC_ScopeT p_scope){

	objectImplInt_impl->setScope(p_scope);

}



void acs_apgcc_objectimplementerinterface_V3::setSelObj(SaSelectionObjectT p_selObj){

	objectImplInt_impl->setSelObj(p_selObj);

}



int acs_apgcc_objectimplementerinterface_V3::getSelObj() const{

	return objectImplInt_impl->getSelObj();

}



ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::dispatch(ACS_APGCC_DispatchFlags p_flag){

	return objectImplInt_impl->dispatch(p_flag);
}



/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	return objectImplInt_impl->modifyRuntimeObj(p_objName, p_attributeValue);

}


/*adminOperationResult*/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	return objectImplInt_impl->adminOperationResult(oiHandle, invocation, result);
}


acs_apgcc_objectimplementerinterface_V3::acs_apgcc_objectimplementerinterface_V3(acs_apgcc_objectimplementerinterface_V3 &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

}



acs_apgcc_objectimplementerinterface_V3& acs_apgcc_objectimplementerinterface_V3::operator=(const acs_apgcc_objectimplementerinterface_V3 &objImp){

	this->objName = objImp.objName;
	this->impName = objImp.impName;
	this->scope = objImp.scope;

	return *this;

}

//OM.OI_OM_ERROR_COM
//void acs_apgcc_objectimplementerinterface_V3::setExitCode(int p_exitCode){
void acs_apgcc_objectimplementerinterface_V3::setExitCode(int p_exitCode, string p_exitCodeMessage){

	objectImplInt_impl->setExitCode(p_exitCode, p_exitCodeMessage);
}

string acs_apgcc_objectimplementerinterface_V3::getExitCodeString(){

	return objectImplInt_impl->getExitCodeString();
}

void acs_apgcc_objectimplementerinterface_V3::setExitCode(int p_exitCode){
	return objectImplInt_impl->setExitCode(p_exitCode);

}

SaAisErrorT acs_apgcc_objectimplementerinterface_V3::getExitCode(){

	return objectImplInt_impl->getExitCode();

}

/*1963*/
ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::getCcbAugmentationInitialize(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, ACS_APGCC_CcbHandle* ccbHandle, ACS_APGCC_AdminOwnerHandle* adminOwnerHandler){

	return objectImplInt_impl->getCcbAugmentationInitialize(oiHandle, ccbId, ccbHandle, adminOwnerHandler );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::createObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList){

	return objectImplInt_impl->createObjectAugmentCcb( ccbHandle, adminOwnerHandler, p_className, p_parentName, p_attrValuesList );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::modifyObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue){

	return objectImplInt_impl->modifyObjectAugmentCcb( ccbHandle, adminOwnerHandler, p_objectName, p_attributeValue );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::deleteObjectAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* m_objectName){

	return objectImplInt_impl->deleteObjectAugmentCcb( ccbHandle, adminOwnerHandler, m_objectName );

}

ACS_CC_ReturnType acs_apgcc_objectimplementerinterface_V3::applyAugmentCcb(  ACS_APGCC_CcbHandle ccbHandle){

	return objectImplInt_impl->applyAugmentCcb( ccbHandle);

}

int acs_apgcc_objectimplementerinterface_V3::getInternalLastError(){

	return objectImplInt_impl->getInternalLastError();
}


const char* acs_apgcc_objectimplementerinterface_V3::getInternalLastErrorText(){


	return objectImplInt_impl->getInternalLastErrorText();

}
/*end 1963*/


acs_apgcc_objectimplementerinterface_V3::~acs_apgcc_objectimplementerinterface_V3(){

	delete objectImplInt_impl;

}

/**Admin operation result with parameters methods overloaded */
ACS_CC_ReturnType  acs_apgcc_objectimplementerinterface_V3::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result, vector<ACS_APGCC_AdminOperationParamType> outParamVector){

	return objectImplInt_impl->adminOperationResult(oiHandle, invocation, result, outParamVector );
}



/*end**/
