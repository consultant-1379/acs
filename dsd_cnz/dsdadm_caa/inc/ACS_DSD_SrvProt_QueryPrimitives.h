#ifndef ACS_DSD_SRVPROT_QUERYPRIMITIVES_H_
#define ACS_DSD_SRVPROT_QUERYPRIMITIVES_H_

/** @file ACS_DSD_SrvProt_QueryPrimitives.h
 *	@brief
 *	@author xassore
 *	@date 2011-01-28
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
 *	| 0.1    | 2011-01-28 | xassore      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ProtocolHandler.h"
#include "ACS_DSD_SrvProt_Primitive.h"
#include "ACS_DSD_MacrosConstants.h"

//=================================================================//
// Class for the primitive:  "Query" request  version 1  		   //
//=================================================================//

/** @class SrvProt_query_v1_request ACS_DSD_SrvProt_QueryPrimitives.h
 *	@brief Represents the primitive  "Query request", version 1, of DSD-DSD protocol
 *	@author xassore
 *	@date 2011-01-28
 *	@version 0.1
 *
 *	SrvProt_query_v1_request class detailed description
 *
 */
class SrvProt_query_v1_request: public SrvProt_Primitive {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief SrvProt_query_v1_request default constructor
	 *
	 *	SrvProt_query_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline SrvProt_query_v1_request ()
	: SrvProt_Primitive(), _serv_name(), _serv_domain() {
		_protocol_version = SRVSRV_PROT_V1;
		_primitive_id = SRVSRV_QUERY_REQUEST;
	}

	//============//
	// Destructor //
	//============//
public:
	virtual inline ~SrvProt_query_v1_request () {}

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
	static inline SrvProt_Primitive * make_primitive () { return new(std::nothrow) SrvProt_query_v1_request(); }

	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
		return  pdh.make_primitive(SRVSRV_QUERY_REQUEST, SRVSRV_PROT_V1, _serv_name, _serv_domain);
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
		return pdh.unpack_primitive(_primitive_id, _protocol_version, _serv_name, _serv_domain);
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

	/** @brief service_name method
	 *
	 *	service_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void service_name (const char * str);

	/** @brief service_domain method
	 *
	 *	service_domain method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline const char * service_domain () const { return _serv_domain; }

	/** @brief service_domain method
	 *
	 *	service_domain method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void service_domain (const char *str);

private:
	int query_to_apnode(ACS_DSD_Node apnode, uint8_t is_local_node, ACS_DSD_ServiceHandler *service_handler) const;
	char _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];		/// ApplService field
	char _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];		/// Domain field
};


//=================================================================//
// Class for the primitive:  "Query response"  version 1  		   //
//=================================================================//

/** @class SrvProt_query_v1_response ACS_DSD_SrvProt_QueryPrimitives.h
 *	@brief Represents the primitive  "Query response", version 1, of SERVICE-APPLICATION protocol
 *	@author xassore
 *	@date 2011-01-28
 *	@version 0.1
 *
 *	SrvProt_query_v1_response class detailed description
 *
 */
class SrvProt_query_v1_response : public SrvProt_Primitive {
	//==============//
	// Constructors //
	//==============//
public:
	/** @brief SrvProt_query_v1_response default constructor
	 *
	 *	SrvProt_query_v1_response default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline SrvProt_query_v1_response ()
	: SrvProt_Primitive(), _error(acs_dsd::PCP_ERROR_CODE_SUCCESSFUL), _system_id(acs_dsd::SYSTEM_ID_UNKNOWN),
	  _node_state(acs_dsd::NODE_STATE_UNDEFINED), _node_name() {
		_protocol_version = SRVSRV_PROT_V1;
		_primitive_id = SRVSRV_QUERY_REPLY;
		*_node_name = 0;
	}

	//============//
	// Destructor //
	//============//
public:
	virtual inline ~SrvProt_query_v1_response () {}

	/** @brief Builds and returns a "Register Request IPC Socket" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Query request" primitive, version 1
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static inline SrvProt_Primitive * make_primitive () { return new(std::nothrow) SrvProt_query_v1_response(); }

	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual inline int pack_into_primitive_data_handler (ACS_DSD_PrimitiveDataHandler<> & pdh) const {
		return pdh.make_primitive(SRVSRV_QUERY_REPLY, SRVSRV_PROT_V1, _error, _system_id, _node_state, _node_name);
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
		return pdh.unpack_primitive(_primitive_id, _protocol_version, &_error, &_system_id, &_node_state, _node_name);
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

	inline const char *node_name() const { return _node_name; }

	inline void node_name (const char *str){strncpy(_node_name, str, acs_dsd::PCP_FIELD_SIZE_NODE_NAME);}

private:
	uint8_t _error; 		/// Error field
	int32_t _system_id;		/// SystemId field
	uint8_t _node_state;	/// NodeState field
	char    _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];		/// NodeName  field
};

#endif /* ACS_DSD_SRVPROT_QUERYPRIMITIVES_H_ */
