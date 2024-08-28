/*
 * acs_apgcc_oihandler_V3.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_oihandler_V3.h"

#include <iostream>
//#include "ACS_APGCC_OiHandler.h"
//da aggiungere #include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "ACS_TRA_trace.h"

#include "acs_apgcc_oihandler_V3_impl.h"

acs_apgcc_oihandler_V3::acs_apgcc_oihandler_V3(){

	oiHandlerimpl = new acs_apgcc_oihandler_V3_impl;

}



acs_apgcc_oihandler_V3::acs_apgcc_oihandler_V3(acs_apgcc_oihandler_V3 &oi){

	this->oiHandlerimpl =  new acs_apgcc_oihandler_V3_impl(oi.oiHandlerimpl);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3::addObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj){

	return oiHandlerimpl->addObjectImpl(obj);
}



ACS_CC_ReturnType acs_apgcc_oihandler_V3::addClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className){

	return oiHandlerimpl->addClassImpl(obj, className);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3::addMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec){

	return oiHandlerimpl->addMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3::removeObjectImpl(acs_apgcc_objectimplementerinterface_V3 *obj){

	return oiHandlerimpl->removeObjectImpl(obj);

}


ACS_CC_ReturnType acs_apgcc_oihandler_V3::removeMultipleClassImpl(acs_apgcc_objectimplementerinterface_V3 *p_obj, vector<string> p_classNameVec ){

	return oiHandlerimpl->removeMultipleClassImpl(p_obj, p_classNameVec);

}



ACS_CC_ReturnType acs_apgcc_oihandler_V3::removeClassImpl(acs_apgcc_objectimplementerinterface_V3 *obj, const char * className ){

	return oiHandlerimpl->removeClassImpl( obj, className );

}



std::string acs_apgcc_oihandler_V3::getErrorText(int p_errorId){

	return oiHandlerimpl->getErrorText(p_errorId);

}



acs_apgcc_oihandler_V3& acs_apgcc_oihandler_V3::operator=(const acs_apgcc_oihandler_V3 &oiHandler){
	ACE_UNUSED_ARG(oiHandler);

	return *this;
}



acs_apgcc_oihandler_V3::~acs_apgcc_oihandler_V3(){

	delete oiHandlerimpl;
}



