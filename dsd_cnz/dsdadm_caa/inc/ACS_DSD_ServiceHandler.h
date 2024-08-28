#ifndef ACS_DSD_SERVICEHANDLER_H_
#define ACS_DSD_SERVICEHANDLER_H_

/** @file ACS_DSD_ServiceHandler.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2010-11-11 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Event_Handler.h"
#include "ACS_DSD_Session.h"
#include "ACS_DSD_ProtocolsDefs.h"
#include <string>
#include "ace/Reactor.h"
#include "ACS_DSD_ServicePrimitive.h"

/** @class ACS_DSD_ServiceHandler ACS_DSD_ServiceHandler.h
 *	@brief Base class for specialized Service Handler classes
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
 *	@version 0.1
 *
 *	This abstract class represents a generic DSD Service Handler, ie an entity used by DSD Daemon to handle the communication with a peer, on an yet established DSD API Session.
 *	The communication is performed using one of the three DSD protocols.
 *
 */
class ACS_DSD_ServiceHandler: public ACE_Event_Handler
{
public:
	// Constants used as return codes by handlePrimitive() method
	enum ACS_DSD_Prot_ServiceHandler_Constants
	{
		ACS_DSD_PROT_SHDLR_OK_WITH_RESP = 0,
		ACS_DSD_PROT_SHDLR_OK_NO_RESP = 1,
		ACS_DSD_PROT_SHLDR_OK_RESP_ALREADY_SENT = 2,
		ACS_DSD_PROT_SHDLR_OK_CLOSE_WITH_RESP = 3,
		ACS_DSD_PROT_SHDLR_OK_CLOSE_NO_RESP = 4,
		ACS_DSD_PROT_SHLDR_OK_CLOSE_RESP_ALREADY_SENT = 5,
		ACS_DSD_PROT_SHDLR_ERROR = -1
	};

	/// Constants used as return codes
	enum ACS_DSD_ServiceHandler_constants
	{
		ACS_DSD_SHDLC_SUCCESS = 0,
		ACS_DSD_SHDLC_INVALID_REACTOR = -1,
		ACS_DSD_SHDLC_REGISTER_HANDLE_ERROR = -2,
		ACS_DSD_SHDLC_UNREGISTER_HANDLE_ERROR = -3,
		ACS_DSD_SHDLC_ERROR = -4
	};

	//==============//
	// Constructors //
	//==============//
protected:
	/** @brief ACS_DSD_ServiceHandler constructor
	 *
	 *	ACS_DSD_ServiceHandler constructor detailed description
	 *
	 *	@param[in] prot_id Protocol to be used to communicate with the peer
	 *	@param[in] reactor the reactor with which we have to register to be alerted (by mean of "handle_input" callback method) when data is ready to be read on the DSD API Session object
	 *	@remarks Remarks
	 */
	ACS_DSD_ServiceHandler(ACS_DSD_ProtocolID prot_ID, ACE_Reactor * reactor);

	//==============//
	// Destructor   //
	//==============//
	virtual ~ACS_DSD_ServiceHandler() {  _session.close(); };

public:
	/** @brief open method
	 *
	 *	Initializes the object registering it with the REACTOR (<this->_reactor>), and starts waiting data on the DSD API Session Object (<this->_session>).
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int open();


	/** @brief close method
	 *
	 *	if the object has an associated reactor (ie <this-_reactor> != 0), invokes "remove_handler()" method to remove itself from reactor
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close();


	//==================================//
	// Callback methods used by Reactor //
	//==================================//
	// ***********************************

	/**	@brief handle_input method
	 *
	 *	Callback method invoked by the Reactor (_reactor attribute) when data is ready to be read on the DSD API Session.
	 *	Reads the data, parses it to get the Protocol Primitive, and then processes the primitive
	 *
	 *	@param[in] fd handle of the socket on which data are ready to be read
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);


	/**	@brief handle_close method
	 *
	 *	Called when a <handle_*()> method returns -1 or when the <remove_handler> method is called on the Reactor.
	 *	The close_mask indicates which event has triggered the <handle_close> method callback on a particular handle.
	 *
	 *	@param[in] handle Description
	 *	@param[in] close_mask Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int  handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);


	/**	@brief get_handle method
	 *
	 *	Used by Reactor to obtain the I/O handle associated to the object, during registration phase
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual ACE_HANDLE get_handle (void) const;

	// ***********************************


	/**	@brief peer method
	 *
	 *	Called by ACS_DSD_ServiceAccepor objects to obtain the DSD API Session object to be used for accepting a new client connection.
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ACS_DSD_Session &  peer() { return this->_session;}


	/// return the ID assigned to the object during construction
	uint32_t getID() { return _object_ID; };


	/**	@brief Send a primitive to the peer.
	 *
	 *	Sends a primitive to the peer
	 *
	 *	@param[in] primitiveToSend the primitive that has to be sent to the peer
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeoout_ms = 0) = 0;


	/// get 'notification to service handler task' feature status (true --> active; false --> not active)
	bool notification_to_service_handler_task() { return _notification_to_service_handler_task; }


	/// enable/disable 'notification to service handler task' feature
	void set_notification_to_service_handler_task(bool val) { _notification_to_service_handler_task = val; }

protected:
	/**	@brief Reads a primitive sent by the peer
	 *
	 *	Reads a primitive sent by the peer and returns it to the caller
	 *
	 *	@param[out] receivedPrimitive used to return DSD Primitive to the caller
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks The method allocates memory for the returned ACS_DSD_ServicePrimitive using new. It's up to the caller to release memory when done with the ACS_DSD_ServicePrimitive object
	 */
	virtual int recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms = 0) = 0;


	/**	@brief Processes a primitive received by the peer.
	 *
	 *	Processes a primitive received from the peer..
	 *
	 *	@param[in] primitive the primitive that must be processed
	 *	@param[out]	response used to return the response, if any
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int handlePrimitive(const ACS_DSD_ServicePrimitive * primitive, ACS_DSD_ServicePrimitive * & response);


	/// Invoked by handle_input() before returning -1, i.e. when connection with the peer has to be closed. This hook method gives the
	/// opportunity to execute some actions just before the event handler is removed from the service handlers' reactor
	virtual int on_connection_close_hook_func() { return 0; }


private:
	/// returns the next available ID to be assigned to a newly created object instance
	static uint32_t sync_get_ID();

	//========//
	// Fields //
	//========//
protected:
	ACS_DSD_Session  _session;					///	DSD API Session object
	ACS_DSD_ProtocolID _protocol_id;			///	Protocol used to communicate with the client
	std::string _last_error_descr;				/// Description of the last occurred error

private:
	ACE_Reactor * _reactor;						///	Reactor that will dispatch the object when data is ready to be read on DSD API session object
	uint32_t _object_ID;						/// object ID
	bool _notification_to_service_handler_task;	/// true if and only if handle_*() methods must send work notification to Service Handler Task

	static ACE_Thread_Mutex _svcHandler_mutex; 	/// used to synchronize access to the
	static uint32_t _next_available_ID;			/// next available ID that can be assigned to a class object
};

#endif /* ACS_DSD_SERVICEHANDLER_H_ */
