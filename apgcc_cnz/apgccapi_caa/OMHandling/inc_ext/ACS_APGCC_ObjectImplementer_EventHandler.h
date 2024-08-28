/*
 * ACS_APGCC_ObjectImplementer_EventHandler.h
 *
 *  Created on: Sep 3, 2010
 *      Author: xfabron
 */



#ifndef ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_
#define ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_

#include "ace/Reactor.h"
#include "ACS_APGCC_ObjectImplementerInterface.h"


class acs_apgcc_objectimplementer_eventhandler_impl;

/** @class ACS_APGCC_ObjectImplementer_EventHandler ACS_APGCC_ObjectImplementer_EventHandler.h
 *	@brief ACS_APGCC_ObjectImplementer_EventHandler class
 *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
 *	@date 2010-09-16
 *	@version R1A
 *
 *	ACS_APGCC_ObjectImplementer_EventHandler Class detailed description
 */
class ACS_APGCC_ObjectImplementer_EventHandler : public ACS_APGCC_ObjectImplementerInterface , public ACE_Event_Handler {

	//=========//
	// Friends //
	//=========//
	friend class acs_apgcc_oihandler_impl;

public:

//	ACS_APGCC_ObjectImplementer_EventHandler();

	//==============//
	// Constructors //
	//==============//

	/** @brief ACS_APGCC_ObjectImplementer_EventHandler constructor
	 *
	 *	ACS_APGCC_ObjectImplementer_EventHandler Constructor this constructor is used to define a Object Implementer
	 *	for a class
	 *
	 *	@param p_impName Name of the Object Implementer
	 *
	 */
	ACS_APGCC_ObjectImplementer_EventHandler(string p_impName);


	/** @brief ACS_APGCC_ObjectImplementer_EventHandler constructor
	 *
	 *	ACS_APGCC_ObjectImplementer_EventHandler Constructor this constructor is used to define a Object Implementer
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
	ACS_APGCC_ObjectImplementer_EventHandler(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );


	/** @brief ACS_APGCC_ObjectImplementer_EventHandler Destructor
	 *
	 *	ACS_APGCC_ObjectImplementer_EventHandler Destructor
	 *
	 *	@remarks Remarks
	 */
	~ACS_APGCC_ObjectImplementer_EventHandler();


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

	acs_apgcc_objectimplementer_eventhandler_impl *impl;

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


	//========//
	// Fields //
	//========//
//
//	ACS_APGCC_DispatchFlags flag;
//
//	bool isHandleRegister;


};

#endif /* ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_ */







//#ifndef ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_
//#define ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_
//
//#include "ace/Reactor.h"
//#include "ACS_APGCC_ObjectImplementerInterface.h"
//
//
///** @class ACS_APGCC_ObjectImplementer_EventHandler ACS_APGCC_ObjectImplementer_EventHandler.h
// *	@brief ACS_APGCC_ObjectImplementer_EventHandler class
// *	@author xpaomaz (Paola Mazzone) & xfabron (Fabio Ronca)
// *	@date 2010-09-16
// *	@version R1A
// *
// *	ACS_APGCC_ObjectImplementer_EventHandler Class detailed description
// */
//class ACS_APGCC_ObjectImplementer_EventHandler : public ACS_APGCC_ObjectImplementerInterface , public ACE_Event_Handler {
//
//	//=========//
//	// Friends //
//	//=========//
////	friend class ACS_APGCC_OiHandler;
//	friend class acs_apgcc_oihandler_impl;
//
//public:
//
////	ACS_APGCC_ObjectImplementer_EventHandler();
//
//	//==============//
//	// Constructors //
//	//==============//
//
//	/** @brief ACS_APGCC_ObjectImplementer_EventHandler constructor
//	 *
//	 *	ACS_APGCC_ObjectImplementer_EventHandler Constructor this constructor is used to define a Object Implementer
//	 *	for a class
//	 *
//	 *	@param p_impName Name of the Object Implementer
//	 *
//	 */
//	ACS_APGCC_ObjectImplementer_EventHandler(string p_impName);
//
//
//	/** @brief ACS_APGCC_ObjectImplementer_EventHandler constructor
//	 *
//	 *	ACS_APGCC_ObjectImplementer_EventHandler Constructor this constructor is used to define a Object Implementer
//	 *	for a object.
//	 *
//	 *	@param p_objName Name of the Object
//	 *
//	 *	@param p_impName Name of the Object Implementer
//	 *
//	 *	@param p_scope Scope of operation. The value of this parameter can be:
//	 *					ACS_APGCC_ONE: 		the scope of operation is the object designed by name to which
//	 *								  		p_objName point.
//	 *					ACS_APGCC_SUBLEVEL: the scope of the operation is the object designed by name to which
//	 *										p_objName point and its direct children
//	 *					ACS_APGCC_SUBTREE: 	the scope of the operation is the object designed by name to which
//	 *										objName point and the entire subtree rooted at that object
//	 *
//	 */
//	ACS_APGCC_ObjectImplementer_EventHandler(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );
//
//
//	/** @brief ACS_APGCC_ObjectImplementer_EventHandler Destructor
//	 *
//	 *	ACS_APGCC_ObjectImplementer_EventHandler Destructor
//	 *
//	 *	@remarks Remarks
//	 */
//	~ACS_APGCC_ObjectImplementer_EventHandler(){}
//
//
//	//===========//
//	// Functions //
//	//===========//
//
//	/**	@brief dispatch method
//	 *	dispatch method: return the operating system handle that the invoking process can use to detect pending
//	 *	callback .
//	 *
//	 *	@param p_reactor A pointer to the reactor that handle the events
//	 *
//	 *  @param p_flag flags that specify the callback execution behavior of the dispatcher.
//	 *  The value of this parameter can be:
//	 *  	ACS_APGCC_DISPATCH_ONE
//	 *		ACS_APGCC_DISPATCH_ALL
//	 *		ACS_APGCC_DISPATCH_BLOCKING
//	 *
//	 */
//	void dispatch(ACE_Reactor *p_reactor, ACS_APGCC_DispatchFlags p_flag);
//
//
//private:
//
//	//===========//
//	// Functions //
//	//===========//
//
//	/** @brief handle_input method
//	 *
//	 *	handle_input method this method is called by event handler (reactor) when an input event occurs
//	 *
//	 *	@param fd file descriptor
//	 *
//	 */
//	int handle_input (ACE_HANDLE fd);
//
//
//	/** @brief register_handle method
//	 *
//	 *	register_handle method register handler for I/O events
//	 *
//	 *	@return
//	 *
//	 */
//	int register_handle();
//
//
//	/** @brief get_handle method
//	 *
//	 *	get_handle method Get the I/O handle.
//	 *
//	 *	@return ACE_HANDLE I/O handle
//	 *
//	 */
//	ACE_HANDLE get_handle (void) const ;
//
//
//	/** @brief release_handle method
//	 *
//	 *	release_handle method remove handler from reactor.
//	 *
//	 *
//	 */
//	int release_handle();
//
//
//	//========//
//	// Fields //
//	//========//
//
//	ACS_APGCC_DispatchFlags flag;
//
//	bool isHandleRegister;
//
//
//};
//
//#endif /* ACS_APGCC_OBJECTIMPLEMENTER_EVENTHANDLER_H_ */
