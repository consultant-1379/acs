/*
 * OmHandler.cpp
 *
 *  Created on: May 26, 2010
 *      Author: designer
 */

#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_omhandler_impl.h"
#include <iostream>
#include "ACS_TRA_trace.h"
//#include "acs_apgcc_error.h"



OmHandler::OmHandler() {

	omHandlerImpl = new acs_apgcc_omhandler_impl;

}

OmHandler::OmHandler(OmHandler &obj){

	this->omHandlerImpl =  new acs_apgcc_omhandler_impl(obj.omHandlerImpl);

}


/**OmHandler::Init(int p_registeredImpl) method used to initialize the handlers needed to interact with
 * IMM. The following handler are initialized:
 * immHandle
 * ccbHandle
 * ownerHandle
 * This Init method has one parameter that is used to specify if the Application now registering and initializating
 * on IMM intends to performs operations on Objects only if an Object Implementer is registered for it or regardless
 * of Object Implementer running.
 */
ACS_CC_ReturnType OmHandler::Init(int p_registeredImpl) {


	return omHandlerImpl->Init(p_registeredImpl);;

}
/* END Added for new Init method*****/



/**OmHandler::Finalize() method used to release all handler retrieved in Init()
 * method. the following handler have to be released:
 *
 * ccbHandle
 * ownerHandle
 * immHandle
 *
 */
ACS_CC_ReturnType OmHandler::Finalize() {


	return omHandlerImpl->Finalize();

}


/** OmHandler::defineClass method. This method define a new class in IMM
 * according to parameter provided as input. Input parameter are:
 *
 * char* className : the name of the class to be created
 *
 * ACS_CC_ClassCategoryType classCategory: the class category. It may asuume the following values
 * 					CONFIGURATION if the class to be defined is related to IMM Configuration Objects
 * 					RUNTIME       if the class to be defined is related to IMM Runtime Object
 *vector<ACS_CC_AttrDefinitionType> attrList: a vector containing the definition of all attributes
 *					that are present in the class
 *
 */
ACS_CC_ReturnType OmHandler::defineClass(char* m_className,ACS_CC_ClassCategoryType m_classCategory,vector<ACS_CC_AttrDefinitionType> m_attrList) {

	return omHandlerImpl->defineClass(m_className, m_classCategory, m_attrList);

}



ACS_CC_ReturnType OmHandler::removeClass(char* m_className) {

	return omHandlerImpl->removeClass(m_className);

}



ACS_CC_ReturnType OmHandler::createObject (const char* m_className, const char* m_parentName, vector<ACS_CC_ValuesDefinitionType> m_attrValuesList){

	return omHandlerImpl->createObject (m_className, m_parentName, m_attrValuesList);

}/*End OmHandler::createObject */



/*Added For Object creation without apply the request to CCB*/
ACS_CC_ReturnType OmHandler::createObject (const char* p_className, const char* p_parentName, vector<ACS_CC_ValuesDefinitionType> p_attrValuesList, std::string p_transactionName ){

	return omHandlerImpl->createObject (p_className, p_parentName, p_attrValuesList, p_transactionName );

}/*End OmHandler::createObject */



ACS_CC_ReturnType OmHandler::deleteObject (const char* m_objectName){

	return omHandlerImpl->deleteObject (m_objectName);

}/*End OmHandler::deleteObject */

ACS_CC_ReturnType OmHandler::deleteObject (const char* m_objectName, ACS_APGCC_ScopeT p_scope){

	return omHandlerImpl->deleteObject (m_objectName, p_scope);

}/*End OmHandler::deleteObject with scope*/


ACS_CC_ReturnType OmHandler::deleteObject (const char* p_objectName, std::string p_transactionName){

	return omHandlerImpl->deleteObject (p_objectName, p_transactionName);

}/*End OmHandler::deleteObject */


ACS_CC_ReturnType OmHandler::deleteObject (const char* p_objectName, std::string p_transactionName,ACS_APGCC_ScopeT p_scope){

	return omHandlerImpl->deleteObject (p_objectName, p_transactionName, p_scope);

}/*End OmHandler::deleteObject transaction with scope*/



ACS_CC_ReturnType OmHandler::getAttribute (const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue ){


	return omHandlerImpl->getAttribute (m_objectName,  m_attributeValue);

}/*End  OmHandler::getAttribute*/



ACS_CC_ReturnType OmHandler::getAttribute (const char* p_objectName, std::vector<ACS_APGCC_ImmAttribute *> p_attributeList ){

	return omHandlerImpl->getAttribute (p_objectName,  p_attributeList );

}



ACS_CC_ReturnType OmHandler::getObject(ACS_APGCC_ImmObject *object){

	return omHandlerImpl->getObject( object );

}



ACS_CC_ReturnType OmHandler::getChildren(const char* p_rootName, ACS_APGCC_ScopeT p_scope, std::vector<std::string>* p_rdnList) {

	return  omHandlerImpl->getChildren(p_rootName, p_scope,  p_rdnList);

}



ACS_CC_ReturnType OmHandler::getClassInstances(const char* p_className, std::vector<std::string> &p_dnList){


	return omHandlerImpl->getClassInstances( p_className, p_dnList);

}



ACS_CC_ReturnType OmHandler::modifyAttribute(const char* m_objectName, ACS_CC_ImmParameter* m_attributeValue ) {

	return omHandlerImpl->modifyAttribute(m_objectName,  m_attributeValue );

}/*End OmHandler::modifyAttribute*/



ACS_CC_ReturnType OmHandler::modifyAttribute(const char* p_objectName, ACS_CC_ImmParameter* p_attributeValue, std::string p_transactionName ) {

	return omHandlerImpl->modifyAttribute(p_objectName, p_attributeValue,  p_transactionName );

}/*End OmHandler::modifyAttribute*/


ACS_CC_ReturnType OmHandler::getLastErrorText(std::string p_parentDN, int& p_errorId, std::string& p_errorText){

	return omHandlerImpl->getLastErrorText( p_parentDN,  p_errorId, p_errorText);

}/*End OmHandler::getLastErrorText*/



/*applyRequest applies all requests included in the configuration change bundle identified
 */
ACS_CC_ReturnType OmHandler::applyRequest(std::string p_transactionName){


	return omHandlerImpl->applyRequest(p_transactionName);

}


ACS_CC_ReturnType OmHandler::resetRequest(std::string p_transactionName){


	return omHandlerImpl->resetRequest(p_transactionName);

}

int OmHandler::getInternalLastError(){

	return omHandlerImpl->getInternalLastError();
}


char* OmHandler::getInternalLastErrorText(){


	return omHandlerImpl->getInternalLastErrorText();

}
ACS_CC_ReturnType OmHandler::getExitCode( int& p_errorId, std::string& p_errorText){

	return omHandlerImpl->getExitCode(p_errorId, p_errorText);

}

ACS_CC_ReturnType OmHandler::getExitCode(std::string p_transactionName, int& p_errorId, std::string& p_errorText){

	return omHandlerImpl->getExitCode( p_transactionName, p_errorId, p_errorText);
}

unsigned long long int OmHandler::getCcbHandleValue(){

	return omHandlerImpl->getCcbHandleValue();

}


OmHandler& OmHandler::operator=(const OmHandler &omHandler){

	ACE_UNUSED_ARG(omHandler);

	return *this;
}

OmHandler::~OmHandler() {
	// TODO Auto-generated destructor stub
	delete omHandlerImpl;
}





