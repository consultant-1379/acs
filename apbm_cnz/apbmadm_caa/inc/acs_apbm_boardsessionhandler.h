#ifndef ACS_APBM_BOARDSESSIONHANDLER_H_
#define ACS_APBM_BOARDSESSIONHANDLER_H_

/** @file acs_apbm_boardsessionhandler.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-09-23
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
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2011-09-23 | xludesi      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include "acs_apbm_primitivedatahandler.h"
#include "acs_apbm_trapsubscriptionmanager.h"
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <list>


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


typedef acs_apbm_primitivedatahandler<1024> acs_apbm_primitive;

/* @class acs_apbm_boardsessionhandler acs_apbm_boardsessionhandler.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-09-23
 *	@version 0.1
 *
 *	acs_apbm_boardsessionhandler class detailed description
 */
class acs_apbm_boardsessionhandler: public ACE_Event_Handler
{
public:
	enum acs_apbm_boardsessionhandler_constants
	{
		ACS_APBM_BSH_SUCCESS = 0,
		ACS_APBM_BSH_SUCCESS_CLOSE_CONN = 1,
		ACS_APBM_BSH_ERROR = -1,
		ACS_APBM_BSH_INVALID_REACTOR = -2,
		ACS_APBM_BSH_REGISTER_HANDLE_ERROR = -3,
		ACS_APBM_BSH_UNREGISTER_HANDLE_ERROR = -4,
		ACS_APBM_BSH_RECEIVE_GENERIC_ERROR = -5,
		ACS_APBM_BSH_RECEIVE_PEER_CLOSED = -6,
		ACS_APBM_BSH_RECEIVE_TIMEOUT = -7,
		ACS_APBM_BSH_RECEIVE_UNKNOWN_PRIMITIVE = -8,
		ACS_APBM_BSH_SEND_ERROR = -9,
		ACS_APBM_BSH_UNPACK_PRIMITIVE_ERROR = -10,
		ACS_APBM_BSH_MAKE_PRIMITIVE_ERROR = -11,
		ACS_APBM_BSH_SUBSCRIBE_ERROR = -12,
		ACS_APBM_BSH_UNSUBSCRIBE_ERROR = -13,
		ACS_APBM_BSH_INVALID_TRAP_MESSAGE = -14,
		ACS_APBM_BSH_INVALID_PRIMITIVE_PARAM = -15,
		ACS_APBM_BSH_CANNOT_PROCESS_PRIMITIVE = -16,
		ACS_APBM_BSH_OP_NOT_PERMITTED = -17,
		ACS_APBM_BSH_CANNOT_RETRIEVE_INFO_FROM_CS = -18,
		ACS_APBM_BSH_BOARD_NOT_CONFIGURED = -19
	};

	static const int SEND_PRIMITIVE_TIMEOUT = 5000;   	 // milliseconds
	static const int RECEIVE_PRIMITIVE_TIMEOUT = 5000;   // milliseconds


	/** @brief acs_apbm_boardsessionhandler constructor
	 *
	 *	acs_apbm_boardsessionhandler constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline acs_apbm_boardsessionhandler():_reactor(0), _object_ID(acs_apbm_boardsessionhandler::sync_get_ID()), _subscribed(false), _subscription_bitmap(0), _pending_trap_notification(0),_server_working_set(0) {};


	/** @brief acs_apbm_boardsessionhandler constructor
	 *
	 *	acs_apbm_boardsessionhandler constructor detailed description
	 *
	 *	@param[in] reactor the reactor to be used to wait for data
	 *	@param[in] server_working_set detailed description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler(ACE_Reactor * reactor, acs_apbm_serverworkingset * server_working_set);


	/** @brief acs_apbm_boardsessionhandler desttructor
	 *
	 *	acs_apbm_boardsessionhandler destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_apbm_boardsessionhandler() {  _session.close(); };


	/** @brief open method
	 *
	 *	Initializes the object registering it with the reactor (this->_reactor), and starts waiting data on the DSD API Session Object (this->_session).
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int open();


	/** @brief close method
	 *
	 *	Unregister the object from the reactor (this->_reactor)
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int close();


	//==================================//
	// Callback methods used by Reactor //
	//==================================//

	/**	@brief handle_input method
	 *
	 *	Callback method invoked by the reactor (this->_reactor) when data is ready to be read on the DSD API Session (this->_session).
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


	// for TEST purpose
	//virtual int handle_timeout ( const ACE_Time_Value &  current_time,  const void *  act = 0);


	/**	@brief peer method
	 *
	 *	Returns the DSD API Session object to be used for accepting a new client connection.
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ACS_DSD_Session &  peer() { return _session;}


	/// return the ID assigned to the object during construction
	uint32_t getID() { return _object_ID; };


	/**	@brief Send a primitive to the peer.
	 *
	 *	Sends a primitive to the peer
	 *
	 *	@param[in] pdh the primitive that has to be sent to the peer
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants sendPrimitive(const acs_apbm_primitive  & pdh, uint32_t timeoout_ms = 0);


	/**	@brief Send a NOTIFY_TRAP primitive to the peer.
	 *
	 *	Sends a NOTIFY_TRAP primitive to the peer
	 *
	 *	@param[in] n_pending_traps number of pending traps
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants sendNotifyPrimitive(uint32_t n_pending_traps, uint32_t timeout_ms = 0);


	/**	@brief Reads a primitive sent by the peer
	 *
	 *	Reads a primitive sent by the peer and returns it to the caller
	 *
	 *	@param[out] pdh used to return the primitive to the caller
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks
	 */
	acs_apbm_boardsessionhandler_constants recvPrimitive(acs_apbm_primitive & pdh, uint32_t timeout_ms = 0);


	/**	@brief Processes a primitive received by the peer.
	 *
	 *	Processes a primitive received by the peer..
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handlePrimitive(const acs_apbm_primitive & pdh);


	/**	@brief Notify a TRAP
	 *
	 *	Notify a TRAP (to the BOARD SESSION HANDLER)
	 *
	 *	@param[in] trap  trap to be notified
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants notifyTrap(const trapdata & trap);

private:
	acs_apbm_boardsessionhandler(const acs_apbm_boardsessionhandler &);
	acs_apbm_boardsessionhandler & operator=(const acs_apbm_boardsessionhandler &);


	/**	@brief Processes a SUBSCRIBE primitive.
	 *
	 *	Processes a SUBSCRIBE primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_subscribetrap_request(const acs_apbm_primitive & pdh);


	/**	@brief Processes a UNSUBSCRIBE primitive.
	 *
	 *	Processes a UNSUBSCRIBE primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_unsubscribetrap_request(const acs_apbm_primitive & pdh);


	/**	@brief Processes a GET_TRAP primitive.
	 *
	 *	Processes a GET_TRAP primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_gettrap_request(const acs_apbm_primitive & pdh);


	/**	@brief Processes a SWITCH_BOARD_DATA primitive.
	 *
	 *	Processes a SWITCH_BOARD_DATA primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_switchboarddata_request(const acs_apbm_primitive & pdh);


	/**	@brief Processes a GET_BIOS_VERSION primitive.
	 *
	 *	Processes a GET_BIOS_VERSION primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_getbiosversion_request(const acs_apbm_primitive & pdh);


	/**	@brief Processes a GET_DISK_STATUS primitive.
	 *
	 *	Processes a GET_DISK_STATUS primitive
	 *
	 *	@param[in] pdh the primitive that must be processed
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants handle_getdiskstatus_request(const acs_apbm_primitive & pdh);


	 /**     @brief Processes a GET_THUMBDRIVE_STATUS primitive.
         *
         *      Processes a GET_THUMBDRIVE_STATUS primitive
         *
         *      @param[in] pdh the primitive that must be processed
         *      @return Return Description
         *      @remarks Remarks
         */
	//Redesign as per TR-HS30773
        acs_apbm_boardsessionhandler_constants handle_getthumbdrivestatus_request(const acs_apbm_primitive & pdh);

        /**     @brief Processes a SET_NIC_INFO primitive.
         *
         *      Processes a SET_NIC_INFO primitive
         *
         *      @param[in] pdh the primitive that must be processed
         *      @return Return Description
         *      @remarks Remarks
         */

        acs_apbm_boardsessionhandler_constants handle_set_nic_info_request(const acs_apbm_primitive & pdh);

	 /**     @brief Processes a GET_OWN_SLOT primitive.
         *
         *      Processes a GET_OWN_SLOT primitive
         *
         *      @param[in] pdh the primitive that must be processed
         *      @return Return Description
         *      @remarks Remarks
         */

	acs_apbm_boardsessionhandler_constants handle_getownslot_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_board_presence_request primitive.
		 *
		 *	Processes  Board Presence request primitive
		 *
		 *	@param[in] pdh the primitive that must be processed
		 *	@return Return Description
		 *	@remarks Remarks
		 */
	acs_apbm_boardsessionhandler_constants handle_board_presence_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_board_status_request primitive.
		 *
		 *	Processes  Board Presence request primitive
		 *
		 *	@param[in] pdh the primitive that must be processed
		 *	@return Return Description
		 *	@remarks Remarks
		 */
	acs_apbm_boardsessionhandler_constants handle_board_status_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_board_location_request primitive.
		 *
		 *	Processes  Board location  request primitive
		 *
		 *	@param[in] pdh the primitive that must be processed
		 *	@return Return Description
		 *	@remarks Remarks
		 */
	acs_apbm_boardsessionhandler_constants handle_board_location_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_set_board_status_request primitive.
			 *
			 *	Processes  Board set board status request primitive
			 *
			 *	@param[in] pdh the primitive that must be processed
			 *	@return Return Description
			 *	@remarks Remarks
			 */
	acs_apbm_boardsessionhandler_constants handle_set_board_status_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_get_ipmifw_status_request primitive.
			 *
			 *	Processes  ipmif status request primitive
			 *
			 *	@param[in] pdh the primitive that must be processed
			 *	@return Return Description
			 *	@remarks Remarks
			 */
	acs_apbm_boardsessionhandler_constants handle_get_ipmifw_status_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_get_ipmifw_upgrade_request primitive.
			 *
			 *	Processes  ipmifw upgrade request primitive
			 *
			 *	@param[in] pdh the primitive that must be processed
			 *	@return Return Description
			 *	@remarks Remarks
			 */
	acs_apbm_boardsessionhandler_constants handle_get_ipmifw_upgrade_request(const acs_apbm_primitive & pdh);

	/**	@brief  handle_get_ipmifw_data_request primitive.
			 *
			 *	Processes  ipmifw data request primitive
			 *
			 *	@param[in] pdh the primitive that must be processed
			 *	@return Return Description
			 *	@remarks Remarks
			 */
	acs_apbm_boardsessionhandler_constants handle_get_ipmifw_data_request(const acs_apbm_primitive & pdh);

	/**	@brief Send a SEL_TRAP_REPLY primitive to the peer.
	 *
	 *	Sends a SEL_TRAP_REPLY primitive to the peer
	 *
	 *	@param[in] trap_data TRAP data
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants sendBoardPresenceTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms = 0);

	/**	@brief Send a BOARD_PRESENCE_TRAP_REPLY primitive to the peer.
	 *
	 *	Sends a BOARD_PRESENCE_TRAP_REPLY primitive to the peer
	 *
	 *	@param[in] trap_data TRAP data
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants sendSensorStateChangeTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms = 0);

	/**	@brief Send a SENSOR_STATE_CHANGE_TRAP_REPLY primitive to the peer.
	 *
	 *	Sends a SENSOR_STATE_CHANGE_TRAP_REPLY primitive to the peer
	 *
	 *	@param[in] trap_data TRAP data
	 *	@param[in] timeout_ms timeout_ms description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants sendSelTrapReplyPrimitive(const trapdata & trap, uint32_t timeout_ms = 0);

	acs_apbm_boardsessionhandler_constants send_set_board_status_reply(int error_code);

	/// returns the next available ID to be assigned to a newly created object instance
	static uint32_t sync_get_ID();


	acs_apbm_boardsessionhandler_constants _notifyTrapToPeer(uint32_t n_pending_traps);


	int _reconstruct_client_commamd_from_request(char * buf, int len, int8_t architecture_type, uint32_t magazine);


	/**	@brief Send a GET_SCB_DATA_REPLY or GET_SCXB_DATA_REPLY primitive to the peer.
	 *
	 *	Send a GET_SCB_DATA_REPLY or GET_SCXB_DATA_REPLY primitive to the peer
	 *
	 *	@param[in] architecture_type architecture_type description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	acs_apbm_boardsessionhandler_constants send_invalid_arch_switchboarddata_reply_primitive(int8_t architecture_type);


private:
	ACS_DSD_Session  _session;										///	DSD API Session object
	ACE_Reactor * _reactor;											///	Reactor that will dispatch the object when data is ready to be read on DSD API session object
	uint32_t _object_ID;											/// object ID
	bool _subscribed;												/// indicates if the object is subscribed for receiving TRAP notifications
	unsigned _subscription_bitmap;									/// if(_subscribed == true), indicates the BITMAP specified when subscription occurred
	int _pending_trap_notification;									/// indicates if a NOTIFY_TRAP primitive has been sent to the peer and is still waiting to be consumed by the peer
	ACE_Recursive_Thread_Mutex	_pending_trap_notification_mutex;	/// used to synchronize access to '_pending_notification_traps' attribute
	std::list<trapdata> _received_traps;							/// list of received TRAPs
	ACE_Recursive_Thread_Mutex	_received_traps_mutex;				/// used to synchronize access to '_received_traps' attribute
	acs_apbm_serverworkingset * _server_working_set;

	static ACE_Thread_Mutex _next_available_ID_mutex; 				/// used to synchronize access to '_next_available_ID' static attribute
	static uint32_t _next_available_ID;								/// next available ID that can be assigned to a class object
};


#endif /* ACS_APBM_BOARDSESSIONHANDLER_H_ */
