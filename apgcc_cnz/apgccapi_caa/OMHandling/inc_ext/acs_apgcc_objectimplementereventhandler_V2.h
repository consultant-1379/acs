/*
 * acs_apgcc_objectimplementereventhandler_V2.h
 *
 *  Created on: Aug 25, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_OBJECTIMPLEMENTEREVENTHANDLER_V2_H_
#define ACS_APGCC_OBJECTIMPLEMENTEREVENTHANDLER_V2_H_

#include "ace/Reactor.h"
#include "acs_apgcc_objectimplementerinterface_V2.h"


class acs_apgcc_objectimplementereventhandler_V2_impl;

/** @class acs_apgcc_objectimplementereventhandler_V2 acs_apgcc_objectimplementereventhandler_V2.h
 *	@brief acs_apgcc_objectimplementereventhandler_V2 class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-16
 *	@version R1A
 *
 *	acs_apgcc_objectimplementereventhandler_V2 Class detailed description
 */

class acs_apgcc_objectimplementereventhandler_V2 : public acs_apgcc_objectimplementerinterface_V2 , public ACE_Event_Handler {

	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_V2_impl;

public:


	//==============//
	// Constructors //
	//==============//

	/** @brief acs_apgcc_objectimplementereventhandler_V2 constructor
	 *
	 *	acs_apgcc_objectimplementereventhandler_V2 Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	acs_apgcc_objectimplementereventhandler_V2(string p_impName);


	/** @brief acs_apgcc_objectimplementereventhandler_V2 constructor
	 *
	 *	acs_apgcc_objectimplementereventhandler_V2 Constructor this constructor is used to define a Object Implementer
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
	acs_apgcc_objectimplementereventhandler_V2(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief acs_apgcc_objectimplementereventhandler_V2 Destructor
	 *
	 *	acs_apgcc_objectimplementereventhandler_V2 Destructor
	 *
	 *	@remarks Remarks
	 */
	~acs_apgcc_objectimplementereventhandler_V2();


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
	void dispatch(ACE_Reactor *p_reactor, ACS_APGCC_DispatchFlags p_flag);


private:

	//acs_apgcc_objectimplementer_eventhandler_impl *impl;
	acs_apgcc_objectimplementereventhandler_V2_impl *impl;

	//===========//
	// Functions //
	//===========//

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
	int register_handle();


	/** @brief get_handle method
	 *
	 *	get_handle method Get the I/O handle.
	 *
	 *	@return ACE_HANDLE I/O handle
	 *
	 */
	ACE_HANDLE get_handle (void) const ;


	/** @brief release_handle method
	 *
	 *	release_handle method remove handler from reactor.
	 *
	 *
	 */
	int release_handle();


};

#endif /* ACS_APGCC_OBJECTIMPLEMENTEREVENTHANDLER_V2_H_ */
