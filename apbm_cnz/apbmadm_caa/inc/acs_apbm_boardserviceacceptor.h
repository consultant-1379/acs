#ifndef ACS_APBM_BOARDSERVICEACCEPTOR_H_
#define ACS_APBM_BOARDSERVICEACCEPTOR_H_

/** @file acs_apbm_boardserviceacceptor.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-09-22
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
 *	| R-001 | 2011-09-22 | xludesi      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <ace/Event_Handler.h>
#include <ACS_DSD_Server.h>
#include <ace/Reactor.h>
#include <strings.h>
#include <string>


/*
 * Forward declarations
 */
class acs_apbm_serverworkingset;


/* @class acs_apbm_boardserviceacceptor acs_apbm_boardserviceacceptor.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-09-22
 *	@version 0.1
 *
 *	acs_apbm_boardserviceacceptor class detailed description
 */
class acs_apbm_boardserviceacceptor: public ACE_Event_Handler
{
public:
	// Service Access Point types
	enum SAP_Type {UNIX_SAP, INET_SAP, UNKNOWN_SAP};

	enum acs_apbm_boardserviceacceptor_constants
	{
		ACS_APBM_BSA_SUCCESS = 0,
		ACS_APBM_BSA_INVALID_SERVICE_ACCEPTOR_REACTOR = -1,
		ACS_APBM_BSA_INVALID_SESSION_HANDLER_REACTOR = -2,
		ACS_APBM_BSA_INVALID_SAP_ID = -3,
		ACS_APBM_BSA_INVALID_SAP_TYPE = -4,
		ACS_APBM_BSA_ACCEPTOR_OPEN_ERROR = -5,
		ACS_APBM_BSA_REGISTER_HANDLE_ERROR = -6,
		ACS_APBM_ACCEPTOR_GET_HANDLES_ERROR = -7,
		ACS_APBM_REGISTER_HANDLE_ERROR = -8
	};

	/** @brief acs_apbm_boardserviceacceptor default constructor
	 *
	 *	acs_apbm_boardserviceacceptor default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	explicit acs_apbm_boardserviceacceptor(acs_apbm_serverworkingset * server_working_set);


	/** @brief acs_apbm_boardserviceacceptor constructor
	 *
	 *	acs_apbm_boardserviceacceptor constructor detailed description
	 *
	 *	@param[in] service_acceptor_reactor detailed description
	 *	@param[in] session_handler_reactor detailed description
	 *	@param[in] sap_type	detailed description
	 *  @param[in] sap_id	detailed description
	 *	@remarks Remarks
	 */
	acs_apbm_boardserviceacceptor(ACE_Reactor * service_acceptor_reactor, ACE_Reactor * session_handler_reactor,  SAP_Type sap_type, const char *sap_id, acs_apbm_serverworkingset * server_working_set);


	/** @brief acs_apbm_boardserviceacceptor Destructor
	 */
	virtual ~acs_apbm_boardserviceacceptor();


	/** @brief set_service_acceptor_reactor method
	 *
	 *	set_service_acceptor_reactor method detailed description
	 *
	 *	@param[in] reactor description
	 *	@remarks Remarks
	 */
	void set_service_acceptor_reactor(ACE_Reactor * reactor) { _service_acceptor_reactor = reactor; };


	/** @brief set_session_handler_reactor method
	 *
	 *	set_session_handler_reactor method detailed description
	 *
	 *	@param[in] session_handler_reactor Description
	 *	@remarks Remarks
	 */
	void set_session_handler_reactor(ACE_Reactor * session_handler_reactor) { _session_handler_reactor = session_handler_reactor; };


	/** @brief set_sap method
	 *
	 *	set_sap method detailed description
	 *
	 *	@param[in] sap_type description
	 *	@param[in] sap_id description
	 *	@remarks Remarks
	 */
	void set_sap(SAP_Type sap_type, const char *sap_id);


	/** @brief open method
	 *
	 *	Initializes the object registering it with <this->service_acceptor_reactor> and start listening on the passed SAP
	 *
	 *	@param[in] sap_type service access point type (TCP port or Unix socket path)
	 *	@param[in] sap_id  	service access point id
	 *	@return description
	 *	@remarks Remarks
	 */
	acs_apbm_boardserviceacceptor_constants open(SAP_Type sap_type, const char * sap_id);


	/** @brief open method
	 *
	 *	Initializes the object registering it with <this->_service_acceptor_reactor> and start listening on the SAP <this->_sap_id>
	 *
	 *	@return description
	 *	@remarks Remarks
	 */
	acs_apbm_boardserviceacceptor_constants open() { return open(_sap_type, _sap_id); }


	/** @brief close method
	 *
	 *	close method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int close();


	/** @brief is_open method
	 *
	 *	is_open method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	bool is_open() { return (_acceptor.state() != acs_dsd::SERVER_STATE_CLOSED); }


	/**	@brief handle_input method
	 *
	 *	Callback method invoked by the Reactor (this->_service_acceptor_reactor) when a client connection is ready to be accepted.
	 *	Accepts the connection and creates an object of type acs_apbm_sessionhanlder to handle it.
	 *
	 *	@param[in] fd handle of the socket on which a connection is waiting to be accepted
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int handle_input (ACE_HANDLE /*fd*/ = ACE_INVALID_HANDLE);


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
	int  handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/);


private:
	/**	@brief remove_handlers_from_reactor method
	 *
	 *	remove_handlers_from_reactor method description.
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int remove_handlers_from_reactor();


	//  convert an integer containing an IP address in a string
	static int get_ip_addr_str(uint32_t ip_addr, char *buff, int len);

private:
	ACS_DSD_Server _acceptor;
	SAP_Type _sap_type;
	char _sap_id[acs_dsd::CONFIG_UNIX_SOCK_NAME_SIZE_MAX + 1];
	ACE_Reactor * _service_acceptor_reactor;
	ACE_Reactor * _session_handler_reactor;
	acs_apbm_serverworkingset * _server_working_set;
};

#endif /* ACS_APBM_BOARDSERVICEACCEPTOR_H_ */
