#ifndef ACS_DSD_SRVPROT_SVCHANDLER_H_
#define ACS_DSD_SRVPROT_SVCHANDLER_H_

/** @file ACS_DSD_SrvProt_SvcHandler.h
 *	@brief Class used to handle DSD server to DSD server Protocol connections
 *	@author xassore
 *	@date 2011-01-20
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
 *	| 0.1    | 2011-01-20 | xassore      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ServiceHandler.h"
#include "ACS_DSD_ConfigParams.h"

/** @class ACS_DSD_SrvProt_SvcHandler ACS_DSD_SrvProt_SvcHandler.h
 *	@brief  Class ACS_DSD_SrvProt_SvcHandler
 *	@author xassore
 *	@date 2011-01-20
 *	@version 0.1
 *
 *	This class represents a specialized DSD Service handler. It's used by DSD Daemon to communicate with a client using CP-AP Protocol.
 *
 */
class ACS_DSD_SrvProt_SvcHandler: public ACS_DSD_ServiceHandler
{
public:

	// Return codes for class methods
	enum SRVProt_SvcHandler_Constants
	{
		SRVPROT_SVC_OK = 0,
		SRVPROT_SVC_RECEIVE_ERROR = -1,
		SRVPROT_SVC_RECEIVE_TIMEOUT = -2,
		SRVPROT_SVC_RECEIVE_PEER_CLOSED = -3,
		SRVPROT_SVC_SEND_ERROR = -4,
		SRVPROT_SVC_INTERNAL_ERROR = -5,
		SRVPROT_SVC_GENERIC_ERROR = - 6,
		SRVPROT_SVC_SEND_TIMEOUT = -7,
		SRVPROT_SVC_INVALID_PRIMITIVE = -8,
		SRVPROT_SVC_UNIMPLEMENTED_PRIMITIVE = -9
	};


	/** @brief ACS_DSD_SrvProt_SvcHandler constructor
	 *
	 *	ACS_DSD_SrvProt_SvcHandler constructor detailed description
	 *
	 *	@param[in] reactor the reactor with which we have to register to be alerted (invoking the "handle_input" callback method) when data is ready to be read on the DSD API Session object
	 *	@remarks Remarks
	 */
	ACS_DSD_SrvProt_SvcHandler(ACE_Reactor * reactor = 0);


	/** @brief ACS_DSD_SrvProt_SvcHandler  destructor
	 *
	 *	ACS_DSD_SrvProt_SvcHandler  destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_SrvProt_SvcHandler() { };


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

	/// Set the system_id of the connected node
	void inline set_remote_note_system_id(int32_t system_id) { _remote_ap_system_id = system_id; }

	/// Returns the system of the remote node
	int32_t get_remote_node_system_id() { return _remote_ap_system_id; }

	/// used to keep info about link with Remote AP Nodes Manager
	void set_linked_to_remote_ap_nodes_manager(bool val) { _linked_to_remote_ap_nodes_manager = val; }

	/**	@brief handle_close method
	 *
	 *	Called when a <handle_*()> method returns -1 or when the <remove_handler> method is called on the Reactor.
	 *	The close_mask indicates which event has triggered the <handle_close> method callback on a particular handle.
	 *
	 *	@param[in] handle Description
	 *	@param[in] close_mask Description
	 *	@return Return Description
	 *	@remarks Remarks
	 **/
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

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

	// indicates if the object has been linked to Remote AP Nodes Manager
	bool _linked_to_remote_ap_nodes_manager;

	// indicates the system_id of the remote node (if the object is linked to Remote AP Nodes Manager)
	int32_t _remote_ap_system_id;
};

#endif /* ACS_DSD_SRVPROT_SVCHANDLER_H_ */
