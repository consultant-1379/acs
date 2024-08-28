#ifndef ACS_DSD_APPPROT_QUERYPRIMITIVES_H_
#define ACS_DSD_APPPROT_QUERYPRIMITIVES_H_

/** @file ACS_DSD_AppProt_QueryPrimitives.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2011-01-14
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
 *	| 0.1    | 2011-01-14 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_AppProt_Primitive.h"
#include "ACS_DSD_MacrosConstants.h"

//=================================================================//
// Class for the primitive:  "Query" request  version 1  		   //
//=================================================================//

/** @class AppProt_query_v1_request ACS_DSD_AppProt_QueryPrimitives.h
 *	@brief Represents the primitive  "Query request", version 1, of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2011-01-14
 *	@version 0.1
 *
 *	AppProt_query_v1_request class detailed description
 *
 */
class AppProt_query_v1_request: public AppProt_Primitive {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief AppProt_query_v1_request default constructor
	 *
	 *	AppProt_query_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline AppProt_query_v1_request ()
	: AppProt_Primitive(), _serv_name(), _serv_domain(), _query_order(acs_dsd::SYSTEM_TYPE_UNKNOWN), _timeout_ms(0xFFFFFFFFU) {
		_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
		_primitive_id = SVRAPP_QUERY_REQUEST;
	}

	//============//
	// Destructor //
	//============//
public:
	virtual inline ~AppProt_query_v1_request () {}

	//===========//
	// Functions //
	//===========//
public:
	/** @brief Builds and returns a "Query request v1" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Query request v1" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static inline AppProt_Primitive * make_primitive () { return new(std::nothrow) AppProt_query_v1_request(); }

	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
		return  pdh.make_primitive(SVRAPP_QUERY_REQUEST, SVRAPP_PROTOCOL_VERSION_1, _serv_name, _serv_domain, _query_order, _timeout_ms);
	}

	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int build_from_primitive_data_handler (const ACS_DSD_PrimitiveDataHandler<> & pdh) {
		return pdh.unpack_primitive(_primitive_id, _protocol_version, _serv_name, _serv_domain, &_query_order, &_timeout_ms);
	}

	/// Returns the data members in text format
	virtual std::string to_text() const;

	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process (ACS_DSD_ServicePrimitive * & response_primitive, ACS_DSD_ServiceHandler * service_handler) const;

	//===================//
	// ACCESSOR methods  //
	//===================//
public:

	/** @brief service_name method
	 *
	 *	service_name method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const char * service_name () const { return _serv_name; }

	/** @brief set_service_name method
	 *
	 *	set_service_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
//	int set_service_name(const char *str);

	/** @brief service_domain method
	 *
	 *	service_domain method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const char * service_domain () const { return _serv_domain; }

	/** @brief set_service_domain method
	 *
	 *	set_service_domain method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
//	int set_service_domain(const char *str);

	/** @brief get_query_order method
	 *
	 *	get_query_order method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::SystemTypeConstants query_order() const { return static_cast<acs_dsd::SystemTypeConstants>(_query_order); }

	/** @brief set_query_order method
	 *
	 *	set_query_order method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void query_order(acs_dsd::SystemTypeConstants val) { _query_order = val; }

	/** @brief get_timeout method
	 *
	 *	get_timeout method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint32_t timeout_ms() const { return _timeout_ms;}

	/** @brief get_timeout method
	 *
	 *	get_timeout method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void timeout_ms(uint32_t timeout_ms) { _timeout_ms = timeout_ms; }


private:
	int process_query_for_ap (ACS_DSD_ServiceHandler * service_handler) const;

	int process_query_for_cp_bc (ACS_DSD_ServiceHandler * service_handler, acs_dsd::SystemTypeConstants system_type) const;

private:
	char _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];		/// ApplService field
	char _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];		/// Domain field
	uint8_t _query_order;												/// QueryOrder field
	uint32_t _timeout_ms;													/// timeout field
};


//=================================================================//
// Class for the primitive:  "Query response"  version 1  		   //
//=================================================================//

/** @class AppProt_query_v1_response ACS_DSD_AppProt_QueryPrimitives.h
 *	@brief Represents the primitive  "Query response", version 1, of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2011-01-14
 *	@version 0.1
 *
 *	AppProt_query_v1_response class detailed description
 *
 */
class AppProt_query_v1_response : public AppProt_Primitive {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief AppProt_query_v1_response default constructor
	 *
	 *	AppProt_query_v1_response default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline AppProt_query_v1_response ()
	: AppProt_Primitive(), _error(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL), _system_id(acs_dsd::SYSTEM_ID_UNKNOWN), _node_state(acs_dsd::NODE_STATE_UNDEFINED) {
		_protocol_version = SVRAPP_PROTOCOL_VERSION_1;
		_primitive_id = SVRAPP_QUERY_RESPONSE;
	}

	//============//
	// Destructor //
	//============//
public:
	virtual inline ~AppProt_query_v1_response () {}

	/** @brief Builds and returns a "Register Request IPC Socket" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Query request" primitive, version 1
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static inline AppProt_Primitive * make_primitive () { return new(std::nothrow) AppProt_query_v1_response(); }

	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
		return pdh.make_primitive(SVRAPP_QUERY_RESPONSE, SVRAPP_PROTOCOL_VERSION_1, _error, _system_id, _node_state);
	}

	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int build_from_primitive_data_handler (const ACS_DSD_PrimitiveDataHandler<> & pdh) {
		return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_system_id, &_node_state);
	}

	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int process (ACS_DSD_ServicePrimitive * & /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const {
		return ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE;
	}

	/// Returns the data members in text format
	virtual std::string to_text () const;

	/** @brief get_error_field method
	 *
	 *	get_error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::PCP_ErrorCodesConstants error_field () const { return static_cast<acs_dsd::PCP_ErrorCodesConstants>(_error); }

	inline void error_field (acs_dsd::PCP_ErrorCodesConstants error_code) { _error = error_code; }
	inline void error_field (uint8_t error_code) { _error = error_code; }

	/** @brief get_error_field method
	 *
	 *	get_error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline int32_t system_id () const { return _system_id; }

	inline void system_id (int32_t system_id) { _system_id = system_id; }

	/** @brief get_node_state method
	 *
	 *	get_node_state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline acs_dsd::NodeStateConstants node_state () const { return static_cast<acs_dsd::NodeStateConstants>(_node_state); }

	inline void node_state (acs_dsd::NodeStateConstants node_state) { _node_state = node_state; }

private:
	uint8_t _error; 			/// Error field
	int32_t _system_id;		/// SystemId field
	uint8_t _node_state;		/// NodeState filed
};

#endif /* ACS_DSD_APPPROT_QUERYPRIMITIVES_H_ */
