#ifndef ACS_DSD_SRVPROT_LISTPRIMITIVES_H_
#define ACS_DSD_SRVPROT_LISTPRIMITIVES_H_

/** @file ACS_DSD_SrvProt_ListPrimitives.h
 *	@brief
 *	@author xcasale
 *	@date
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2013
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
 *	| 0.1    |2013-04-09  | xcasale      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <sys/types.h>

#include "ACS_DSD_SrvProt_Primitive.h"
#include "ACS_DSD_PrimitiveDataHandler.h"

//=================================================================//
// Class for the primitive:  "List" request  vers 1   		       //
//=================================================================//

/** @class SrvProt_list_v1_request ACS_DSD_SrvProt_ListPrimitives.h
 *	@brief Represents the primitive  "List request" of DSD_Server-DSD_Server protocol
 *	@author xcasale
 *	@date 2013-04-09
 *	@version 0.1
 *
 *	This class is used to handle the list requests (for DSD_Server-DSD_Server protocol)
 *
 */
class SrvProt_list_v1_request : public SrvProt_Primitive {
public:

	/** @brief SrvProt_list_v1_request default constructor
	 *
	 *	SrvProt_list_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_list_v1_request ();

	/** @brief Builds and returns a "List" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "List" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_list_v1_request(); };


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

	/** @brief get_system_id method
	 *
	 *	get_system_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int32_t get_system_id() { return _system_id; };


	/** @brief set_system_id method
	 *
	 *	set_system_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_system_id( int32_t sys_id) { _system_id = sys_id; };


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
	int getServicesOnLocalNode(ACS_DSD_ServiceHandler * service_handler, char* nodeName) const;
	int getServicesOnPartnerNode(ACS_DSD_ServiceHandler * service_handler, char * nodeName) const;

	int32_t _system_id;												/// SystemId field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];			/// Node name field
};




//====================================================================//
// Class for the primitive:  "List INET Socket" response  vers 1      //
//====================================================================//

/** @class SrvProt_list_INET_v1_reply ACS_DSD_SrvProt_ListPrimitives.h
 *	@brief Represents the primitive  "List INET socket response" of SERVICE-SERVICE protocol
 *	@author xcasale
 *	@date 2013-04-09
 *	@version 0.1
 *
 *	This class is used to handle List responses for INET Socket Services (SERVICE-SERVICE protocol)
 *
 */
class SrvProt_list_inet_v1_reply: public SrvProt_Primitive
{
public:
	/** @brief SrvProt_list_inet_v1_reply default constructor
	 *
	 *	SrvProt_list_inet_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_list_inet_v1_reply();


	/** @brief Builds and returns an "List INET Socket Response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "List INET Socket response" primitive, version 1
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new (std::nothrow) SrvProt_list_inet_v1_reply(); };


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

	/** @brief get_error_field method
	 *
	 *	get_error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */

	inline uint8_t get_error_field()const{ return _error; };

	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */

	inline void set_error_field(uint8_t val) { _error = val; };

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


	/** @brief get_ip_address_1 method
	 *
	 *	get_ip_address_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint32_t get_ip_address_1()const { return _ip1; };


	/** @brief set_ip_address_1 method
	 *
	 *	set_ip_address_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_ip_address_1( uint32_t val) { _ip1 = val; };


	/** @brief get_port_1 method
	 *
	 *	get_port_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint16_t get_port_1() const{ return _port1; };


	/** @brief set_port_1 method
	 *
	 *	set_port_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_port_1(uint16_t val) { _port1 = val; };


	/** @brief get_ip_address_2 method
	 *
	 *	get_ip_address_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint32_t get_ip_address_2()const { return _ip2; };


	/** @brief set_ip_address_2 method
	 *
	 *	set_ip_address_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_ip_address_2( uint32_t val) { _ip2 = val; };


	/** @brief get_port_2 method
	 *
	 *	get_port_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint16_t get_port_2() const { return _port2; };


	/** @brief set_port_2 method
	 *
	 *	set_port_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_port_2(uint16_t val) { _port2 = val; };


	/** @brief get_process_name method
	 *
	 *	get_unix_path method detailed description
	 *
	 *	@param[in] buff Descriptionnotify_register_to_allAP
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
	uint8_t _error;																										/// Error field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];						/// Node name field
	uint8_t _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];			/// Service name field
	uint8_t _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];		/// Service domain field
	uint8_t _process_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME];			/// ProcessName field
	uint32_t _pid;																										/// Pid field
	uint8_t _visibility;																							/// Visibility field
	uint32_t _ip1;																										/// IP-address network 1 field
	uint16_t _port1;																									/// Port1 field
	uint32_t _ip2;																										/// IP-address network 2 field
	uint16_t _port2;																									/// Port2 field
	uint8_t  _unix_sock_path[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket Path field
};



//=================================================================//
// Class for the primitive:  "List UNIX Socket response" vers 1    //
//=================================================================//

/** @class SrvProt_list_unix_v1_reply ACS_DSD_SrvProt_ListPrimitives.h
 *	@brief Represents the primitive  "List UNIX socket response" of SERVICE-SERVICE protocol
 *	@author xcasale
 *	@date 2013-04-09
 *	@version 0.1
 *
 *	This class is used to handle List responses for UNIX Socket Services (SERVICE-SERVICE protocol)
 *
 */
class SrvProt_list_unix_v1_reply: public SrvProt_Primitive
{
public:
	/** @brief SrvProt_list_unix_v1_reply default constructor
	 *
	 *	SrvProt_list_unix_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_list_unix_v1_reply();

	/** @brief Builds and returns an "List UNIX Socket response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "List UNIX Socket response" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new (std::nothrow) SrvProt_list_unix_v1_reply(); };


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


	/** @brief get_unix_sock_path1 method
	 *
	 *	get_unix_sock_path method1 detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_unix_sock_path1(char *buff, size_t buffsize);


	/** @brief set_unix_sock_path1 method
	 *
	 *	set_unix_sock_path1 method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_unix_sock_path1(const char *str);

	/** @brief get_unix_sock_path2 method
		 *
		 *	get_unix_sock_path2 method detailed description
		 *
		 *	@param[in] buff Description
		 *	@param[in] buffsize Description
		 *	@return Return Description
		 *	@remarks Remarks
	 */

	int get_unix_sock_path2(char *buff, size_t buffsize);


	/** @brief set_unix_sock_path2 method
		 *
		 *	set_unix_sock_path2 method detailed description
		 *
		 *	@param[in] str Description
		 *	@return Return Description
		 *	@remarks Remarks
	 */
	int set_unix_sock_path2(const char *str);

private:
	uint8_t _error;																											/// Error field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];							/// Node name field
	uint8_t _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];				/// Service name field
	uint8_t _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];			/// Service domain field
	uint8_t _process_name[acs_dsd::PCP_FIELD_SIZE_PROCESS_NAME];				/// ProcessName field
	uint32_t _pid;																											/// Pid field
	uint8_t _visibility;																								/// Visibility field
	uint8_t  _unix_sock_path1[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket Path 1 field
	uint8_t  _unix_sock_path2[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket Path 2 field
};


#endif /* ACS_DSD_SRVPROT_LISTPRIMITIVES_H_ */
