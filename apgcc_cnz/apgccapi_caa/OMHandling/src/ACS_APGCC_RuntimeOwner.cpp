/*
 * ACS_APGCC_RuntimeOwnerV2.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: xfabron
 */

#include "ACS_APGCC_RuntimeOwner.h"
#include "acs_apgcc_runtimeowner_impl.h"
#include "ace/OS.h"



ACS_APGCC_RuntimeOwner::ACS_APGCC_RuntimeOwner() {

	runtimeOwnerImpl = new acs_apgcc_runtimeowner_impl;

}


ACS_APGCC_RuntimeOwner::ACS_APGCC_RuntimeOwner(ACS_APGCC_RuntimeOwner &runTimeImp){

	this->runtimeOwnerImpl =  new acs_apgcc_runtimeowner_impl(runTimeImp.runtimeOwnerImpl);
}


ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::init(std::string impName){

	return runtimeOwnerImpl->init(impName, this);
}


ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::finalize(){


	return runtimeOwnerImpl->finalize();
}



/**This method creates a runtime Object**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::createRuntimeObj(const char* p_className, const char* p_parentName,
		vector<ACS_CC_ValuesDefinitionType> m_attrValuesList){

	return runtimeOwnerImpl->createRuntimeObj(p_className, p_parentName, m_attrValuesList);
}




/**This method deletes the runtime Object**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::deleteRuntimeObj(const char *p_objName){

	return runtimeOwnerImpl->deleteRuntimeObj( p_objName );
}


/**This method modifies a runtime object owned by an application**/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::modifyRuntimeObj(const char* p_objName, ACS_CC_ImmParameter* p_attributeValue){

	return runtimeOwnerImpl->modifyRuntimeObj(p_objName, p_attributeValue);
}




/**This method invokes pending callbacks for the IMM handle **/
ACS_CC_ReturnType ACS_APGCC_RuntimeOwner::dispatch(ACS_APGCC_DispatchFlags p_flag){

	return runtimeOwnerImpl->dispatch(p_flag);
}



/**This method is used to get the operating system handle that invoking
	 * process can use to detect pending callbacks**/
int ACS_APGCC_RuntimeOwner::getSelObj() const{

	return runtimeOwnerImpl->getSelObj();
}



int ACS_APGCC_RuntimeOwner::getInternalLastError(){

	return runtimeOwnerImpl->getInternalLastError();
}



char* ACS_APGCC_RuntimeOwner::getInternalLastErrorText(){

	return runtimeOwnerImpl->getInternalLastErrorText();
}



ACS_APGCC_RuntimeOwner& ACS_APGCC_RuntimeOwner::operator=(const ACS_APGCC_RuntimeOwner &runOwner){

	ACE_UNUSED_ARG(runOwner);

	return *this;
}



ACS_APGCC_RuntimeOwner::~ACS_APGCC_RuntimeOwner(){

	delete runtimeOwnerImpl;
}


