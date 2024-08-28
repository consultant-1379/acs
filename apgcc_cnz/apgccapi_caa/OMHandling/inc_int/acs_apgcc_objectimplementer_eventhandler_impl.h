/*
 * acs_apgcc_objectimplementer_eventhandler_impl.h
 *
 *  Created on: Mar 3, 2011
 *      Author: xfabron
 */

#ifndef ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_IMPL_H_
#define ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_IMPL_H_


#include "ace/Reactor.h"
#include "ACS_APGCC_ObjectImplementerInterface.h"


class ACS_APGCC_ObjectImplementer_EventHandler;

/** @class acs_apgcc_objectimplementer_eventhandler_impl acs_apgcc_objectimplementer_eventhandler_impl.h
 *	@brief acs_apgcc_objectimplementer_eventhandler_impl class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-16
 *	@version R1A
 *
 *	ACS_APGCC_ObjectImplementer_EventHandler Class detailed description
 */
class acs_apgcc_objectimplementer_eventhandler_impl {

	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_impl;

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_objectimplementer_eventhandler_impl constructor
	 *
	 *	acs_apgcc_objectimplementer_eventhandler_impl Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	acs_apgcc_objectimplementer_eventhandler_impl();


	/** @brief acs_apgcc_objectimplementer_eventhandler_impl constructor
	 *
	 *	acs_apgcc_objectimplementer_eventhandler_impl Constructor this constructor is used to define a Object Implementer
	 *	for a object.
	 *
	 *	@param p_objName Name of the Object
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 *	@param p_scope Scope of operation. The value of this parameter can be:
	 *					ACS_APGCC_ONE: 		the scope of operation is the object designed by name to which
	 *								  		p_objName point.
	 *					ACS_APGCC_SUBLEVEL: the scope of the operation is the object designed by name to which
	 *										p_objName point and its direct children
	 *					ACS_APGCC_SUBTREE: 	the scope of the operation is the object designed by name to which
	 *										objName point and the entire subtree rooted at that object
	 *
	 */
//	acs_apgcc_objectimplementer_eventhandler_impl(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief ACS_APGCC_ObjectImplementer_EventHandler Destructor
	 *
	 *	ACS_APGCC_ObjectImplementer_EventHandler Destructor
	 *
	 *	@remarks Remarks
	 */
	~acs_apgcc_objectimplementer_eventhandler_impl(){}


	//===========//
	// Functions //
	//===========//

	/**	@brief dispatch method
	 *	dispatch method: return the operating system handle that the invoking process can use to detect pending
	 *	callback .
	 *
	 *	@param p_reactor A pointer to the reactor that handle the events
	 *
	 *  @param p_flag flags that specify the callback execution behavior of the dispatcher.
	 *  The value of this parameter can be:
	 *  	ACS_APGCC_DISPATCH_ONE
	 *		ACS_APGCC_DISPATCH_ALL
	 *		ACS_APGCC_DISPATCH_BLOCKING
	 *
	 */
	void dispatch(ACE_Reactor *p_reactor, ACS_APGCC_DispatchFlags p_flag, ACS_APGCC_ObjectImplementer_EventHandler *eventHanlder);


	/** @brief handle_input method
	 *
	 *	handle_input method this method is called by event handler (reactor) when an input event occurs
	 *
	 *	@param fd file descriptor
	 *
	 */
	int handle_input (ACE_HANDLE fd);


	/** @brief register_handle method
	 *
	 *	register_handle method register handler for I/O events
	 *
	 *	@return
	 *
	 */
	int register_handle(ACS_APGCC_ObjectImplementer_EventHandler *eventHanlder);


	/** @brief get_handle method
	 *
	 *	get_handle method Get the I/O handle.
	 *
	 *	@return ACE_HANDLE I/O handle
	 *
	 */
	int get_handle (ACS_APGCC_ObjectImplementer_EventHandler *eventHanlder);


	/** @brief release_handle method
	 *
	 *	release_handle method remove handler from reactor.
	 *
	 *
	 */
	int release_handle(ACS_APGCC_ObjectImplementer_EventHandler *eventHanlder);

	ACS_APGCC_DispatchFlags getFlagDisp();

private:

	//========//
	// Fields //
	//========//

	ACS_APGCC_DispatchFlags flag;

	bool isHandleRegister;


};



#endif /* ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_IMPL_H_ */
