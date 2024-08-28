/*
 * ACS_APGCC_ObjectImplementerInterface_EventHandler.cpp
 *
 *  Created on: Sep 3, 2010
 *      Author: xfabron
 */



#include "ACS_APGCC_ObjectImplementer_EventHandler.h"
#include "acs_apgcc_objectimplementer_eventhandler_impl.h"

#include <iostream>
#include <saImmOi.h>



ACS_APGCC_ObjectImplementer_EventHandler::ACS_APGCC_ObjectImplementer_EventHandler(string p_impName):ACS_APGCC_ObjectImplementerInterface(p_impName){


	impl = new acs_apgcc_objectimplementer_eventhandler_impl;

}



ACS_APGCC_ObjectImplementer_EventHandler::ACS_APGCC_ObjectImplementer_EventHandler(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): ACS_APGCC_ObjectImplementerInterface(p_objName, p_impName, p_scope){

	impl = new acs_apgcc_objectimplementer_eventhandler_impl;

}



ACE_HANDLE ACS_APGCC_ObjectImplementer_EventHandler::get_handle (void) const {

	return impl->get_handle(const_cast<ACS_APGCC_ObjectImplementer_EventHandler *>(this));
}



int ACS_APGCC_ObjectImplementer_EventHandler::handle_input ( ACE_HANDLE fd ){

	impl->handle_input(fd);
	ACS_APGCC_ObjectImplementerInterface::dispatch(impl->getFlagDisp());

	return 0;

}



int ACS_APGCC_ObjectImplementer_EventHandler::register_handle(){

	return impl->register_handle(this);

}



int ACS_APGCC_ObjectImplementer_EventHandler::release_handle(){

	return impl->release_handle(this);

}



void ACS_APGCC_ObjectImplementer_EventHandler::dispatch(ACE_Reactor *reactor, ACS_APGCC_DispatchFlags p_flag){

	impl->dispatch(reactor, p_flag, this);

}

ACS_APGCC_ObjectImplementer_EventHandler::~ACS_APGCC_ObjectImplementer_EventHandler(){

	delete impl;
}




