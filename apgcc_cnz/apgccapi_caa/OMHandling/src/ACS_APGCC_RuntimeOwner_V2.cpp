/*
 * ACS_APGCC_RuntimeOwner_V2.cpp
 *
 *  Created on: Aug 31, 2011
 *      Author: xfabron
 */

#include "ACS_APGCC_RuntimeOwner_V2.h"
#include "acs_apgcc_runtimeowner_impl_v2.h"
#include "ace/OS.h"



ACS_APGCC_RuntimeOwner_V2::ACS_APGCC_RuntimeOwner_V2() {

	runtimeOwnerImpl = new acs_apgcc_runtimeowner_impl_v2;

}


ACS_APGCC_RuntimeOwner_V2::ACS_APGCC_RuntimeOwner_V2(ACS_APGCC_RuntimeOwner_V2 &runTimeImp){

	this->runtimeOwnerImpl =  new acs_apgcc_runtimeowner_impl_v2(runTimeImp.runtimeOwnerImpl);
}


ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::init(std::string impName){

	return runtimeOwnerImpl->init(impName, this);
}


ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::finalize(){


	return runtimeOwnerImpl->finalize();
}



/**This method creates a runtime Object**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::createRuntimeObj(const char* p_className, const char* p_parentName,
		vector<ACS_CC_ValuesDefinitionType> m_attrValuesList){

	return runtimeOwnerImpl->createRuntimeObj(p_className, p_parentName, m_attrValuesList);
}




/**This method deletes the runtime Object**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::deleteRuntimeObj(const char *p_objName){

	return runtimeOwnerImpl->deleteRuntimeObj( p_objName );
}


/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	return runtimeOwnerImpl->modifyRuntimeObj(p_objName, p_attributeValue);
}




/**This method invokes pending callbacks for the IMM handle **/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::dispatch(ACS_APGCC_DispatchFlags p_flag){

	return runtimeOwnerImpl->dispatch(p_flag);
}



/**This method is used to get the operating system handle that invoking
	 * process can use to detect pending callbacks**/
int ACS_APGCC_RuntimeOwner_V2::getSelObj() const{

	return runtimeOwnerImpl->getSelObj();
}


/*adminOperationResult*/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner_V2::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result){

	return runtimeOwnerImpl->adminOperationResult(oiHandle, invocation, result);
}


int ACS_APGCC_RuntimeOwner_V2::getInternalLastError(){

	return runtimeOwnerImpl->getInternalLastError();
}



char* ACS_APGCC_RuntimeOwner_V2::getInternalLastErrorText(){

	return runtimeOwnerImpl->getInternalLastErrorText();
}



ACS_APGCC_RuntimeOwner_V2& ACS_APGCC_RuntimeOwner_V2::operator=(const ACS_APGCC_RuntimeOwner_V2 &runOwner){

	ACE_UNUSED_ARG(runOwner);

	return *this;
}



ACS_APGCC_RuntimeOwner_V2::~ACS_APGCC_RuntimeOwner_V2(){

	delete runtimeOwnerImpl;
}

/**Admin operation result with parameters methods overloaded */
ACS_CC_ReturnType  ACS_APGCC_RuntimeOwner_V2::adminOperationResult(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, int result, vector<ACS_APGCC_AdminOperationParamType> outParamVector){

	return runtimeOwnerImpl->adminOperationResult(oiHandle, invocation, result, outParamVector );
}
