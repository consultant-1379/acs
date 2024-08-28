#ifndef ACS_DSD_APPPROT_ADDRESSPRIMITIVES_H_
#define ACS_DSD_APPPROT_ADDRESSPRIMITIVES_H_

/** @file ACS_DSD_AppProt_AddressPrimitives.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-21
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
 *	| 0.1    | 2010-11-21 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_AppProt_Primitive.h"
#include <sys/types.h>
#include "ACS_DSD_PrimitiveDataHandler.h"

//=================================================================//
// Class for the primitive:  "Address" request  vers 1   		   //
//=================================================================//

/** @class AppProt_address_v1_request ACS_DSD_AppProt_AddressPrimitives.h
 *	@brief Represents the primitive  "Address request" of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-21
 *	@version 0.1
 *
 *	This class is used to handle the address requests (for SERVICE-APPLICATION protocol)
 *
 */
class AppProt_address_v1_request: public AppProt_Primitive
{
public:
	/** @brief AppProt_address_v1_request default constructor
	 *
	 *	AppProt_address_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_address_v1_request();

	/** @brief Builds and returns an "Address" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Address" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static AppProt_Primitive* make_primitive() { return new(std::nothrow) AppProt_address_v1_request(); };


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


	/** @brief get_node_state method
	 *
	 *	get_node_state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_node_state() { return _node_state; };


	/** @brief set_node_state method
	 *
	 *	set_node_state method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_node_state(uint8_t val) { _node_state = val; };


private:
	int queryAddressestoCP(int32_t cp_system_id, ACE_INET_Addr inet_addresses[acs_dsd::CONFIG_NETWORKS_SUPPORTED]) const;
	int queryAddressestoAP(ACS_DSD_ServicePrimitive *& response_primitive, uint8_t remote_node_state) const;
	uint8_t _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];		/// Service name field
	uint8_t _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];	/// Service domain field
	int32_t _system_id;												/// SystemId field
	uint8_t _node_state;											/// NodeState field
};




//====================================================================//
// Class for the primitive:  "Address INET Socket" response  vers 1   //
//====================================================================//

/** @class AppProt_addressINET_v1_response ACS_DSD_AppProt_AddressPrimitives.h
 *	@brief Represents the primitive  "Address INET socket response" of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-21
 *	@version 0.1
 *
 *	This class is used to handle Address responses for INET Socket Services (SERVICE-APPLICATION protocol)
 *
 */
class AppProt_addressINET_v1_response: public AppProt_Primitive
{
public:
	/** @brief AppProt_addressINET_v1_response default constructor
	 *
	 *	AppProt_addressINET_v1_response default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_addressINET_v1_response();


	/** @brief Builds and returns an "Address INET Socket Response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Address INET Socket response" primitive, version 1
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	inline static AppProt_Primitive* make_primitive() { return new (std::nothrow) AppProt_addressINET_v1_response(); };


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
	inline uint8_t get_error_field() { return _error; };


	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_error_field(uint8_t val) { _error = val; };


	/** @brief get_ip_address_1 method
	 *
	 *	get_ip_address_1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline uint32_t get_ip_address_1() { return _ip1; };


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
	inline uint16_t get_port_1() { return _port1; };


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
	inline uint32_t get_ip_address_2() { return _ip2; };


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
	inline uint16_t get_port_2() { return _port2; };


	/** @brief set_port_2 method
	 *
	 *	set_port_2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_port_2(uint16_t val) { _port2 = val; };

private:
	uint8_t _error;														/// Error field
	uint32_t _ip1;														/// IP-address network 1 field
	uint16_t _port1;													/// Port1 field
	uint32_t _ip2;														/// IP-address network 2 field
	uint16_t _port2;													/// Port2 field
};



//=================================================================//
// Class for the primitive:  "Address UNIX Socket response" vers 1 //
//=================================================================//

/** @class AppProt_addressUNIX_v1_response ACS_DSD_AppProt_AddressPrimitives.h
 *	@brief Represents the primitive  "Address UNIX socket response" of SERVICE-APPLICATION protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-21
 *	@version 0.1
 *
 *	This class is used to handle Address responses for UNIX Socket Services (SERVICE-APPLICATION protocol)
 *
 */
class AppProt_addressUNIX_v1_response: public AppProt_Primitive
{
public:
	/** @brief AppProt_addressUNIX_v1_response default constructor
	 *
	 *	AppProt_addressUNIX_v1_response default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_addressUNIX_v1_response();

	/** @brief Builds and returns an "Address UNIX Socket response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Address UNIX Socket response" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static AppProt_Primitive* make_primitive() { return new (std::nothrow) AppProt_addressUNIX_v1_response(); };


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
	 inline void set_error_field(uint8_t val) { _error = val; };

	/** @brief get_unix_sock_path method
	 *
	 *	get_unix_sock_path method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_unix_sock_path1(char *buff, size_t buffsize);


	/** @brief set_unix_sock_path method
	 *
	 *	set_unix_sock_path method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_unix_sock_path1(const char *str);

	/** @brief get_unix_sock_path method
	 *
	 *	get_unix_sock_path method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_unix_sock_path2(char *buff, size_t buffsize);


	/** @brief set_unix_sock_path method
	 *
	 *	set_unix_sock_path method detailed description
	 *
	 *	@param[in] str Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int set_unix_sock_path2(const char *str);


private:
	uint8_t _error;														/// Error field
	uint8_t _unix_sock_path1[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket name field
	uint8_t _unix_sock_path2[acs_dsd::PCP_FIELD_SIZE_UNIX_SOCK_PATH];	/// Unix Socket name field
};


#endif /* ACS_DSD_AppProt_ADDRESSPRIMITIVES_H_ */
