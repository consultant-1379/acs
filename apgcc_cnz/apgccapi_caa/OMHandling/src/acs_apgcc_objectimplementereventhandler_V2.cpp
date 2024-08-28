/*
 * acs_apgcc_objectimplementereventhandler_V2.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

//#include "ACS_APGCC_ObjectImplementer_EventHandler.h"
//#include "acs_apgcc_objectimplementer_eventhandler_impl.h"

#include <iostream>
#include <saImmOi.h>

#include "acs_apgcc_objectimplementereventhandler_V2.h"
#include "acs_apgcc_objectimplementereventhandler_V2_impl.h"



/////////
acs_apgcc_objectimplementereventhandler_V2::acs_apgcc_objectimplementereventhandler_V2(string p_impName):acs_apgcc_objectimplementerinterface_V2(p_impName){


	//impl = new acs_apgcc_objectimplementer_eventhandler_impl;
	impl = new acs_apgcc_objectimplementereventhandler_V2_impl;

}



acs_apgcc_objectimplementereventhandler_V2::acs_apgcc_objectimplementereventhandler_V2(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): acs_apgcc_objectimplementerinterface_V2(p_objName, p_impName, p_scope){

	//impl = new acs_apgcc_objectimplementer_eventhandler_impl;
	impl = new acs_apgcc_objectimplementereventhandler_V2_impl;

}



ACE_HANDLE acs_apgcc_objectimplementereventhandler_V2::get_handle (void) const {

	return impl->get_handle(const_cast<acs_apgcc_objectimplementereventhandler_V2 *>(this));
}



int acs_apgcc_objectimplementereventhandler_V2::handle_input ( ACE_HANDLE fd ){

	impl->handle_input(fd);
	//ACS_APGCC_ObjectImplementerInterface::dispatch(impl->getFlagDisp());
	acs_apgcc_objectimplementerinterface_V2::dispatch(impl->getFlagDisp());


	return 0;

}



int acs_apgcc_objectimplementereventhandler_V2::register_handle(){

	return impl->register_handle(this);

}



int acs_apgcc_objectimplementereventhandler_V2::release_handle(){

	return impl->release_handle(this);

}



void acs_apgcc_objectimplementereventhandler_V2::dispatch(ACE_Reactor *reactor, ACS_APGCC_DispatchFlags p_flag){

	impl->dispatch(reactor, p_flag, this);

}

acs_apgcc_objectimplementereventhandler_V2::~acs_apgcc_objectimplementereventhandler_V2(){

	delete impl;
}

