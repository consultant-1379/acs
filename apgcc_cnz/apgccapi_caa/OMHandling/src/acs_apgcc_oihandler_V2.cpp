/*
 * acs_apgcc_oihandler_V2.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_oihandler_V2.h"

#include <iostream>
//#include "ACS_APGCC_OiHandler.h"
//da aggiungere #include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "ACS_TRA_trace.h"

#include "acs_apgcc_oihandler_V2_impl.h"

acs_apgcc_oihandler_V2::acs_apgcc_oihandler_V2(){

	oiHandlerimpl = new acs_apgcc_oihandler_V2_impl;

}



acs_apgcc_oihandler_V2::acs_apgcc_oihandler_V2(acs_apgcc_oihandler_V2 &oi){

	this->oiHandlerimpl =  new acs_apgcc_oihandler_V2_impl(oi.oiHandlerimpl);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V2::addObjectImpl(acs_apgcc_objectimplementerinterface_V2 *obj){

	return oiHandlerimpl->addObjectImpl(obj);
}



ACS_CC_ReturnType acs_apgcc_oihandler_V2::addClassImpl(acs_apgcc_objectimplementerinterface_V2 *obj, const char * className){

	return oiHandlerimpl->addClassImpl(obj, className);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V2::addMultipleClassImpl(acs_apgcc_objectimplementerinterface_V2 *p_obj, vector<string> p_classNameVec){

	return oiHandlerimpl->addMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V2::removeObjectImpl(acs_apgcc_objectimplementerinterface_V2 *obj){

	return oiHandlerimpl->removeObjectImpl(obj);

}


ACS_CC_ReturnType acs_apgcc_oihandler_V2::removeMultipleClassImpl(acs_apgcc_objectimplementerinterface_V2 *p_obj, vector<string> p_classNameVec ){

	return oiHandlerimpl->removeMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V2::removeClassImpl(acs_apgcc_objectimplementerinterface_V2 *obj, const char * className ){

	return oiHandlerimpl->removeClassImpl( obj, className );

}



std::string acs_apgcc_oihandler_V2::getErrorText(int p_errorId){

	return oiHandlerimpl->getErrorText(p_errorId);

}



acs_apgcc_oihandler_V2& acs_apgcc_oihandler_V2::operator=(const acs_apgcc_oihandler_V2 &oiHandler){
	ACE_UNUSED_ARG(oiHandler);

	return *this;
}



acs_apgcc_oihandler_V2::~acs_apgcc_oihandler_V2(){

	delete oiHandlerimpl;
}



