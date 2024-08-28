#ifndef ACS_DSD_APPPROT_SVCHANDLER_H_
#define ACS_DSD_APPPROT_SVCHANDLER_H_

/** @file ACS_DSD_AppProt_SvcHandler.h
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

#include "ACS_DSD_ServiceHandler.h"
#include <vector>

/** @class ACS_DSD_AppProt_SvcHandler ACS_DSD_ServiceHandler.h
 *	@brief Class used to handle SERVICE - APPLICATION Protocol connections
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
 *	@version 0.1
 *
 *	This class represents a specialized DSD Service handler. It's used by DSD Daemon to communicate with a client using SERVICE-APPLICATION Protocol.
 *
 */
class ACS_DSD_AppProt_SvcHandler: public ACS_DSD_ServiceHandler
{
public:

	// Return codes for class methods
	enum AppProt_SvcHandler_Constants
	{
		APPPROT_SVC_OK = 0,
		APPPROT_SVC_RECEIVE_ERROR = -1,
		APPPROT_SVC_RECEIVE_TIMEOUT = -2,
		APPPROT_SVC_RECEIVE_PEER_CLOSED = -3,
		APPPROT_SVC_INTERNAL_ERROR = -4,
		APPPROT_SVC_GENERIC_ERROR = - 5,
		APPPROT_SVC_INVALID_PRIMITIVE = -6,
		APPPROT_SVC_UNIMPLEMENTED_PRIMITIVE = -7,
		APPPROT_SVC_SEND_ERROR = -8,
		APPPROT_SVC_SEND_TIMEOUT = -9
	};

	/** @brief ACS_DSD_ServiceHandler constructor
	 *
	 *	Initializes the object registering it with the passed REACTOR, and starts waiting data on the passed DSD API Session Object.
	 *
	 *	@param[in] reactor the reactor with which we have to register to be alerted (invoking the "handle_input" callback method) when data is ready to be read on the DSD API Session object
	 *	@remarks Remarks
	 */
	ACS_DSD_AppProt_SvcHandler(ACE_Reactor * reactor);


	/** @brief ACS_DSD_ServiceHandler destructor
	 *
	 *	ACS_DSD_ServiceHandler destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_AppProt_SvcHandler() { };


	/**	@brief Send a primitive to the peer.
	 *
	 *	Sends a primitive to the peer
	 *
	 *	@param[in] primitiveToSend the primitive that has to be sent to the peer
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeoout_ms = 0);


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
	virtual int handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/);


	/**	@brief Add an entry to the list of CP whose state changes have to be notified to the peer.
	 *
	 *  This method is invoked when the peer requests to be notified about the state changes of a CP (using primitive 44).
	 *  When a state change for the specified CP occurs, a notification indication (primitive 45) is sent to the peer.
	 *
	 *	@param[in] elem the system ID of the CP for which the peer wants to receive notifications.
	 *	@remarks Remarks
	 */
	inline void add_to_cpNotified(uint32_t elem){_cpNotifiedList.push_back(elem);}

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
	virtual int recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms = 0);


	/// Invoked by handle_input() before returning -1, i.e. when connection with the peer has to be closed. This hook method gives the
	/// opportunity to execute some actions just before the event handler is removed from the service handlers' reactor
	virtual int on_connection_close_hook_func();


	std::vector<uint32_t> _cpNotifiedList;      // list of CP_id for which a Notification request has been sent by the peer

};

#endif /* ACS_DSD_APPROT_SVCHANDLER_H_ */
