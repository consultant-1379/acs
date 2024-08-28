#ifndef ACS_DSD_CPPROT_SVCHANDLER_H_
#define ACS_DSD_CPPROT_SVCHANDLER_H_

/** @file ACS_DSD_CPProt_SvcHandler.h
 *	@brief Class used to handle CP-AP Protocol connections
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-10
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
 *	| 0.1    | 2010-12-10 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ConfigParams.h"

/** @class ACS_DSD_CpProt_SvcHandler ACS_DSD_CPProt_SvcHandler.h
 *	@brief  Class ACS_DSD_CPProt_SvcHandler
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-10
 *	@version 0.1
 *
 *	This class represents a specialized DSD Service handler. It's used by DSD Daemon to communicate with a client using CP-AP Protocol.
 *
 */
class ACS_DSD_CpProt_SvcHandler: public ACS_DSD_ServiceHandler
{
public:

	// Return codes for class methods
	enum CPProt_SvcHandler_Constants
	{
		CPPROT_SVC_OK = 0,
		CPPROT_SVC_RECEIVE_ERROR = -1,
		CPPROT_SVC_RECEIVE_TIMEOUT = -2,
		CPPROT_SVC_RECEIVE_PEER_CLOSED = -3,
		CPPROT_SVC_INTERNAL_ERROR = -4,
		CPPROT_SVC_GENERIC_ERROR = - 5,
		CPPROT_SVC_INVALID_PRIMITIVE = -6,
		CPPROT_SVC_UNIMPLEMENTED_PRIMITIVE = -7,
		CPPROT_SVC_SEND_ERROR = -8,
		CPPROT_SVC_SEND_TIMEOUT = -9,
		CPPROT_SVC_HDLPRIM_OK_WITH_RESP = 0,
		CPPROT_SVC_HDLPRIM_OK_NO_RESP = 1,
		CPPROT_SVC_HDLPRIM_OK_RESP_ALREADY_SENT = 2,
		CPPROT_SVC_HDLPRIM_ERROR = -1,
		CPPROT_SVC_HDLPRIM_PROCESS_ERROR = -2
	};


	/** @brief ACS_DSD_CpProt_SvcHandler constructor
	 *
	 *	ACS_DSD_CpProt_SvcHandler constructor detailed description
	 *
	 *	@param[in] reactor the reactor with which we have to register to be alerted (invoking the "handle_input" callback method) when data is ready to be read on the DSD API Session object
	 *	@remarks Remarks
	 */
	ACS_DSD_CpProt_SvcHandler(ACE_Reactor * reactor = 0);


	/** @brief ACS_DSD_CpProt_SvcHandler  destructor
	 *
	 *	ACS_DSD_CpProt_SvcHandler  destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_CpProt_SvcHandler() { };


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


	/**	@brief Reads a primitive sent by the peer
	 *
	 *	Reads a primitive sent by the peer, using the DSD API MessageReceiver attribute, and returns it to the caller
	 *
	 *	@param[out] receivedPrimitive used to return DSD Primitive to the caller
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks The method allocates memory for the returned ACS_DSD_ServicePrimitive using new. It's up to the caller to release memory when done with the ACS_DSD_ServicePrimitive object
	 */
	virtual int recvPrimitive(ACS_DSD_ServicePrimitive * & receivedPrimitive, uint32_t timeout_ms = RECV_PRIM_DEFAULT_TIMEOUT);


	/**	@brief Send a primitive to the peer.
	 *
	 *	Sends a primitive to the peer, using the DSD API MessageSender attribute
	 *
	 *	@param[in] primitiveToSend the primitive that has to be sent to the peer
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int sendPrimitive(const ACS_DSD_ServicePrimitive * primitiveToSend, uint32_t timeoout_ms = 0);


	/// Set the name of the connected node
	void set_remote_node_name(char node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX]);


	/// Returns the name of the remote node
	const char *get_remote_node_name() { return _remote_node_name; }


	/// used to keep info about link with CpNodesManager
	void set_linked_to_cp_nodes_manager(bool val) {this->_linked_to_cp_nodes_manager = val; }

protected:

	/**	@brief Processes a primitive received by the peer.
	 *
	 *	Processes a primitive received by the peer..
	 *
	 *	@param[in] primitive the primitive that must be processed
	 *	@param[out]	response used to return the response, if any
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int handlePrimitive(const ACS_DSD_ServicePrimitive * primitive, ACS_DSD_ServicePrimitive * & response);


	/// Invoked by handle_input() before returning -1, i.e. when connection with the peer has to be closed. This hook method gives the
	/// opportunity to execute some actions just before the event handler is removed from the service handlers' reactor
	virtual int on_connection_close_hook_func();

private:
	static const uint32_t RECV_PRIM_DEFAULT_TIMEOUT = 60 * 60 * 1000;   // default timeout when receiving primitives, in milliseconds

	/// node name of the peer
	char _remote_node_name[acs_dsd::CONFIG_NODE_NAME_SIZE_MAX];

	//  attributes set by recvPrimitve() method, in case of error, to make available to other methods the primitive id and primitive version
	int _last_received_prim_id;
	int _last_received_prim_vers;

	/// indicates if the object has been linked to CP Nodes Manager
	bool _linked_to_cp_nodes_manager;
};

#endif /* ACS_DSD_CPPROT_SVCHANDLER_H_ */
