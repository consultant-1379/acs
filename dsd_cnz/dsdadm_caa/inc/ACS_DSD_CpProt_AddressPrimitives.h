#ifndef ACS_DSD_CPPROT_ADDRESSPRIMITIVES_H_
#define ACS_DSD_CPPROT_ADDRESSPRIMITIVES_H_


/** @file ACS_DSD_CpProt_AddressPrimitives.h
 *	@brief
 *	@author xssore
 *	@date 2010-12-13
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
 *	| 0.1    | 2010-12-13 | xassore      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_CpProt_Primitive.h"
#include <sys/types.h>
#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ACS_CS_API.h"

//=================================================================//
// Class for the primitive:  "Address" request           		   //
//=================================================================//

/** @class CpProt_address_request ACS_DSD_CpProt_AddressPrimitives.h
 *	@brief Represents the primitive  "Address request" of CP-APPLICATION protocol
 *	@author xassore
 *	@date 2010-12-13
 *	@version 0.1
 *
 *	This class is used to handle the address requests (for CP-APPLICATION protocol)
 *
 */
class CpProt_address_request: public CpProt_Primitive
{
public:
	/** @brief CpProt_address_request default constructor
	 *
	 *	CpProt_address_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_address_request();

	/** @brief Builds and returns an "Address" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Address" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive* make_primitive() { return new(std::nothrow) CpProt_address_request(); };


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
	//bool isFunctionDistributable(char * /*serv_domain*/, char * /*serv_name*/, int32_t /*local system_id*/, int32_t /*remote system id*/) const;
	//bool isFunctionDistributed(ACS_CS_API_FunctionDistribution * funcDist, int32_t /* apid*/, int32_t /*cpid*/, const char * /*fName*/) const;
	bool isFunctionDistributable(char * serv_domain, char * serv_name, int32_t system_id) const;
	bool isFunctionDistributed(int32_t apid, const char * fName) const;
	bool isMessageStore(char* msName, std::string & ms_id) const;
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];			/// Node name field
	uint8_t _serv_name[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];		/// Service name field
	uint8_t _serv_domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];	/// Service domain field
};




//====================================================================//
// Class for the primitive:  Address response   //
//====================================================================//

/** @class CpProt_address_response ACS_DSD_CpProt_AddressPrimitives.h
 *	@brief Represents the primitive  "Address response" of CP-APPLICATION protocol
 *	@author xassore
 *	@date 2010-12-13
 *	@version 0.1
 *
 *	This class is used to handle Address responses Socket Services (CP-APPLICATION protocol)
 *
 */

class CpProt_address_response: public CpProt_Primitive
{
public:

	struct process_prim_info_t{
		uint8_t data_validation_result;
	}mutable _process_prim_info;

	/** @brief CpProt_address_response default constructor
	 *
	 *	CpProt_address_response default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_address_response();


	/** @brief Builds and returns an "Address Response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Address response" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive* make_primitive() { return new (std::nothrow) CpProt_address_response(); }


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

	/** @brief get_conn_type  method
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
	void set_conn_type( uint16_t val) { _conn_type = val; };

	/** @brief get_conn_num  method
	 *
	 *	get_conn_num method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_conn_num() { return _conn_num; };


	/** @brief set_conn_num method
	 *
	 *	set_conn_num method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_conn_num( uint16_t val) { _conn_num = val; };


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
	 *	@return Return Descriptionp_CpProt_addrReply
	 *	@remarks Remarks
	 */
	void set_ip_address_1( uint32_t val) { _ip1 = val; };


	/** @brief get_port_1 method
	 *p_CpProt_addrReply
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

private:
	uint8_t _error;														/// Error field
	uint16_t _conn_type;												/// Connection type field
	uint16_t _conn_num;													/// number of connections field
	uint32_t _ip1;														/// IP-address network 1 field
	uint16_t _port1;													/// Port1 field
	uint32_t _ip2;														/// IP-address network 2 field
	uint16_t _port2;													/// Port2 field
};


#endif /* ACS_DSD_CpProt_ADDRESSPRIMITIVES_H_ */

