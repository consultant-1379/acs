/*
 * acs_apgcc_objectimplementereventhandler_V3_impl.cpp
 *
 *  Created on: Aug 26, 2011
 *      Author: xpaomaz
 */

#include "acs_apgcc_objectimplementereventhandler_V3_impl.h"
#include "acs_apgcc_objectimplementereventhandler_V3.h"
#include <iostream>
#include <saImmOi.h>
#include "ACS_TRA_trace.h"


namespace {
	ACS_TRA_trace trace(const_cast<char*>("acs_apgcc_objectimplementereventhandler_V3"), const_cast<char *>("C300"));
}


using namespace std;



/////////////

acs_apgcc_objectimplementereventhandler_V3_impl::acs_apgcc_objectimplementereventhandler_V3_impl(){

	isHandleRegister = false;
}


//acs_apgcc_objectimplementereventhandler_V3_impl::acs_apgcc_objectimplementereventhandler_V3_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): ACS_APGCC_ObjectImplementerInterface(p_objName, p_impName, p_scope){
//
//	isHandleRegister = false;
//}


int acs_apgcc_objectimplementereventhandler_V3_impl::get_handle (acs_apgcc_objectimplementereventhandler_V3 *eventHanlder) {


	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_objectimplementereventhandler_V3::get_handle (void)");
		trace.ACS_TRA_event(1, buffer);
	}

	SaSelectionObjectT selObj_2 = eventHanlder->getSelObj();
	return (int)selObj_2;
}


int acs_apgcc_objectimplementereventhandler_V3_impl::handle_input ( ACE_HANDLE fd ){

	fd = ACE_INVALID_HANDLE;

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_objectimplementereventhandler_V3::handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE)");
		trace.ACS_TRA_event(1, buffer);
	}

	//ACS_APGCC_ObjectImplementerInterface::dispatch(flag);

	return 0;
}



int acs_apgcc_objectimplementereventhandler_V3_impl::register_handle(acs_apgcc_objectimplementereventhandler_V3 *eventHanlder){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_objectimplementereventhandler_V3::register_handle()");
		trace.ACS_TRA_event(1, buffer);
	}

	isHandleRegister = true;
	return eventHanlder->reactor()->register_handler(eventHanlder, ACE_Event_Handler::ALL_EVENTS_MASK);

}



int acs_apgcc_objectimplementereventhandler_V3_impl::release_handle(acs_apgcc_objectimplementereventhandler_V3 *eventHanlder){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_objectimplementereventhandler_V3::release_handle()");
		trace.ACS_TRA_event(1, buffer);
	}

//	cout<<"selOBJ: "<<(int)getSelObj()<<endl;

	if ((this->get_handle (eventHanlder) != ACE_INVALID_HANDLE) && isHandleRegister) {
		ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
		eventHanlder->reactor()->remove_handler (eventHanlder, m);
	}

	return 0;

}



void acs_apgcc_objectimplementereventhandler_V3_impl::dispatch(ACE_Reactor *reactor, ACS_APGCC_DispatchFlags p_flag, acs_apgcc_objectimplementereventhandler_V3 *eventHanlder){

	if (trace.ACS_TRA_ON()){
		char buffer[300];
		sprintf(buffer, "acs_apgcc_objectimplementereventhandler_V3::dispatch(ACE_Reactor *reactor, ACS_APGCC_DispatchFlags p_flag)");
		trace.ACS_TRA_event(1, buffer);
	}

	flag = p_flag;

	eventHanlder->reactor(reactor);

	register_handle(eventHanlder);

}



ACS_APGCC_DispatchFlags acs_apgcc_objectimplementereventhandler_V3_impl::getFlagDisp(){

	return flag;
}

