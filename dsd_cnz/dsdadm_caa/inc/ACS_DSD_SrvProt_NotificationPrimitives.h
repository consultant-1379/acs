#ifndef ACS_DSD_SVRPROT_NOTIFICATIONPRIMITIVES_H_
#define ACS_DSD_SVRPROT_NOTIFICATIONPRIMITIVES_H_

/** @file ACS_DSD_SrvProt_NotificationPrimitives.h
 *	@brief
 *	@author xludesi
 *	@date 2011-01-31
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
 *	| 0.1    |2011-01-31  | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_SrvProt_Primitive.h"

//=================================================================//
// Class for the primitive:  notify publish request version 1      //
//=================================================================//

/** @class SrvProt_notify_publish_v1_request ACS_DSD_SrvProt_NotificationPrimitives.h
 *	@brief  Represents the "notify publish v1 request" primitive of DSD_Server-DSD_Server protocol
 *	@author xludesi
 *	@date 2011-01-31
 *	@version 0.1
 *
 *	This primitive is used by a DSD Server to notify to another DSD Server that a service has been registered
 *
 */
class SrvProt_notify_publish_v1_request: public SrvProt_Primitive
{
public:
	/** @brief SrvProt_notify_publish_v1_request default constructor
	 *
	 *	SrvProt_notify_publish_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_notify_publish_v1_request();

	/** @brief Builds and returns an "Address" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "notify publish v1 request" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_notify_publish_v1_request(); }


	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const;


	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh);


	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const;


	/// Returns the data members in text format
	virtual std::string to_text() const;


	//====================//
	// ACCESSOR methods   //
	//====================//

	/** @brief get_service_name method
	 *
	 *	get_service_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_name(char *buff, size_t buffsize);


	/** @brief set_service_name method
	 *
	 *	set_service_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_service_name(const char *str);


	/** @brief get_service_domain method
	 *
	 *	get_service_domain method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_domain(char *buff, size_t buffsize);


	/** @brief set_service_domain method
	 *
	 *	set_service_domain method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_service_domain(const char *str);


	/** @brief get_process_name method
	 *
	 *	get_process_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_process_name(char *buff, size_t buffsize);


	/** @brief set_process_name method
	 *
	 *	set_process_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_process_name(const char *str);

	/** @brief get_node_name method
	 *
	 *	get_node_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_node_name(char *buff, size_t buffsize);


	/** @brief set_node_name method
	 *
	 *	set_node_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_node_name(const char *str);


	/** @brief get_pid method
	 *
	 *	get_pid method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_pid() { return _pid; };


	/** @brief set_pid method
	 *
	 *	set_pid method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_pid(uint32_t val) { _pid = val; };


	/** @brief get_visibility method
	 *
	 *	get_visibility method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	bool get_visibility() { return _visibility; };

	/** @brief set_visibility method
	 *
	 *	set_visibility method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_visibility(uint8_t val) { _visibility = val; };

	/** @brief get_conn_type method
	 *
	 *	get_conn_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_conn_type() { return _conn_type; };


	/** @brief set_conn_type method
	 *
	 *	set_conn_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_conn_type(uint16_t val) { _conn_type = val; };


	/** @brief get_ip_address_1 method
	 *
	 *	get_ip_address_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_ip_address_1() { return _ip1; };


	/** @brief set_ip_address_1 method
	 *
	 *	set_ip_address_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_ip_address_1( uint32_t val) { _ip1 = val; };


	/** @brief get_port_1 method
	 *
	 *	get_port_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_port_1() { return _port1; };


	/** @brief set_port_1 method
	 *
	 *	set_port_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_port_1(uint16_t val) { _port1 = val; };


	/** @brief get_ip_address_2 method
	 *
	 *	get_ip_address_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_ip_address_2() { return _ip2; };


	/** @brief set_ip_address_2 method
	 *
	 *	set_ip_address_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_ip_address_2( uint32_t val) { _ip2 = val; };


	/** @brief get_port_2 method
	 *
	 *	get_port_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_port_2() { return _port2; };


	/** @brief set_port_2 method
	 *
	 *	set_port_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_port_2(uint16_t val) { _port2 = val; };


	/** @brief get_process_name method
	 *
	 *	get_unix_path method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_unix_path(char *buff, size_t buffsize);


	/** @brief set_unix_path method
	 *
	 *	set_unix_path method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_unix_path(const char *str);


private:
	char _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];		/// Service name field
	char _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];		/// Service domain field
	char _process_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME];		/// ProcessName field
	char _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];				/// Node Name field
	uint32_t _pid;														/// Pid field
	uint8_t _visibility;
	uint16_t _conn_type;												/// conntype field
	uint32_t _ip1;														/// IP-address network 1 field
	uint16_t _port1;													/// Port1 field
	uint32_t _ip2;														/// IP-address network 2 field
	uint16_t _port2;													/// Port2 field
	uint8_t  _unix_sock_path[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket Path field
};


//=================================================================//
// Class for the primitive:  notify publish reply version 1        //
//=================================================================//

/** @class SrvProt_notify_publish_v1_reply ACS_DSD_SvrProt_NorificationPrimitives.h
 *	@brief Represents the primitive "notify publish reply version 1" of DSD-DSD protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2011--01-31
 *	@version 0.1
 *
 *	Represents the primitive used by a DSD Server to reply to a notification publish primitive
 *
 */
class SrvProt_notify_publish_v1_reply: public SrvProt_Primitive
{
public:
	/** @brief SrvProt_notify_publish_v1_reply default constructor
	 *
	 *	SrvProt_notify_publish_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_notify_publish_v1_reply();

	/** @brief Builds and returns a "notify publish v1 reply " primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "notify publish v1 reply " primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new (std::nothrow) SrvProt_notify_publish_v1_reply(); };


	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const;


	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh);

	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const;


	/// Returns the data members in text format
	virtual std::string to_text() const;


	/** @brief get_error_field method
	 *
	 *	get_error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_error_field() { return _error; };


	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_error_field(uint8_t val) { _error = val; };

private:
	 uint8_t _error;		/// Error field
};


//=================================================================//
// Class for the primitive:  notyfy unregister request version 1   //
//=================================================================//

/** @class SvrProt_notify_unregister_v1_request ACS_DSD_SvrProt_NotificationPrimitives.h
 *	@brief Represents the primitive  "notify unregister request, version 1 " of DSD-DSD protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-01-31
 *	@version 0.1
 *
 *	This class represents the primitive "notify unregister request, version 1" sent by a DSD Server to another DSD Server to notify that a service
 *	has been unregistered
 *
 */
class SrvProt_notify_unregister_v1_request: public SrvProt_Primitive
{
public:
	/** @brief SvrProt_notify_unregister_v1_request default constructor
	 *
	 *	SvrProt_notify_unregister_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_notify_unregister_v1_request();

	/** @brief Builds and returns a "notify unregister request, version 1" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "notify unregister request, version 1" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive * make_primitive() { return new(std::nothrow) SrvProt_notify_unregister_v1_request(); };


	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const;


	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh);


	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const;


	/// Returns the data members in text format
	virtual std::string to_text() const;


	//====================//
	// ACCESSOR methods   //
	//====================//

	/** @brief get_service_name method
	 *
	 *	get_service_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_name(char *buff, size_t buffsize);


	/** @brief set_service_name method
	 *
	 *	set_service_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_service_name(const char *str);


	/** @brief get_service_domain method
	 *
	 *	get_service_domain method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_domain(char *buff, size_t buffsize);


	/** @brief set_service_domain method
	 *
	 *	set_service_domain method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_service_domain(const char *str);

	/** @brief get_node_name method
	 *
	 *	get_node_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_node_name(char *buff, size_t buffsize);


	/** @brief set_node_name method
	 *
	 *	set_node_name method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_node_name(const char *str);


private:
	uint8_t _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];	/// Service name field
	uint8_t _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];	/// Service domain field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];			/// Node Name field
};


//=================================================================//
// Class for the primitive:  notify unregister reply version 1     //
//=================================================================//

/** @class AppProt_uregister_v1_response ACS_DSD_AppProt_UnregisterPrimitives.h
 *	@brief Represents the primitive  "Unregistration response" of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-21
 *	@version 0.1
 *
 *	This class is used to handle the unregistration responses (for SERVICE-APPLICATION protocol)
 *
 */
class SrvProt_notify_unregister_v1_reply: public SrvProt_Primitive
{
public:
	/** @brief SrvProt_notify_unregister_v1_reply default constructor
	 *
	 *	SrvProt_notify_unregister_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_notify_unregister_v1_reply();

	/** @brief Builds and returns a "notify unregister reply version 1" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "notify unregister reply version 1" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new (std::nothrow) SrvProt_notify_unregister_v1_reply(); };


	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const;


	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh);


	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process(ACS_DSD_ServicePrimitive *& response_primitive, ACS_DSD_ServiceHandler *service_handler) const;


	/// Returns the data members in text format
	virtual std::string to_text() const;


	/** @brief get_error_field method
	 *
	 *	get_error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_error_field() { return _error; };


	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_error_field(uint8_t val) { _error = val; };

private:
	 uint8_t _error;		/// Error field
};



#endif /* ACS_DSD_SVRPROT_NOTIFICATIONPRIMITIVES_H_ */
