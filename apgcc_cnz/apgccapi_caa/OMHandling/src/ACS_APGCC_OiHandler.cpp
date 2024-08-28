/*
 * ACS_APGCC_OiHandler.cpp
 *
 *  Created on: Sep 2, 2010
 *      Author: xfabron
 */

#include <iostream>
#include "ACS_APGCC_OiHandler.h"
#include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "ACS_TRA_trace.h"

#include "acs_apgcc_oihandler_impl.h"


ACS_APGCC_OiHandler::ACS_APGCC_OiHandler(){

	oiHandlerimpl = new acs_apgcc_oihandler_impl;

}



ACS_APGCC_OiHandler::ACS_APGCC_OiHandler(ACS_APGCC_OiHandler &oi){

	this->oiHandlerimpl =  new acs_apgcc_oihandler_impl(oi.oiHandlerimpl);

}



ACS_CC_ReturnType ACS_APGCC_OiHandler::addObjectImpl(ACS_APGCC_ObjectImplementerInterface *obj){

	return oiHandlerimpl->addObjectImpl(obj);
}



ACS_CC_ReturnType ACS_APGCC_OiHandler::addClassImpl(ACS_APGCC_ObjectImplementerInterface *obj, const char * className){

	return oiHandlerimpl->addClassImpl(obj, className);

}



ACS_CC_ReturnType ACS_APGCC_OiHandler::addMultipleClassImpl(ACS_APGCC_ObjectImplementerInterface *p_obj, vector<string> p_classNameVec){

	return oiHandlerimpl->addMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType ACS_APGCC_OiHandler::removeObjectImpl(ACS_APGCC_ObjectImplementerInterface *obj){

	return oiHandlerimpl->removeObjectImpl(obj);

}


ACS_CC_ReturnType ACS_APGCC_OiHandler::removeMultipleClassImpl(ACS_APGCC_ObjectImplementerInterface *p_obj, vector<string> p_classNameVec ){

	return oiHandlerimpl->removeMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType ACS_APGCC_OiHandler::removeClassImpl(ACS_APGCC_ObjectImplementerInterface *obj, const char * className ){

	return oiHandlerimpl->removeClassImpl( obj, className );

}



std::string ACS_APGCC_OiHandler::getErrorText(int p_errorId){

	return oiHandlerimpl->getErrorText(p_errorId);

}



ACS_APGCC_OiHandler& ACS_APGCC_OiHandler::operator=(const ACS_APGCC_OiHandler &oiHandler){
	ACE_UNUSED_ARG(oiHandler);

	return *this;
}



ACS_APGCC_OiHandler::~ACS_APGCC_OiHandler(){

	delete oiHandlerimpl;
}


