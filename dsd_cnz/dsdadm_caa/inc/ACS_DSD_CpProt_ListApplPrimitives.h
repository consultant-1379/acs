#ifndef ACS_DSD_CPPROT_LISTAPPLPRIMITIVES_H_
#define ACS_DSD_CPPROT_LISTAPPLPRIMITIVES_H_

/** @file ACS_DSD_CpProt_ListApplPrimitives.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-30
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
 *	| 0.1    | 2010-12-30 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_CpProt_Primitive.h"
#include "ACS_DSD_Utils.h"

//=================================================================//
// Class for the primitive:  "List Application" request  version 1 //
//=================================================================//

/** @class CpProt_listAppl_v1_request ACS_DSD_CpProt_ListApplPrimitives.h
 *	@brief Represents the primitive  "List Application request" of CP-AP protocol,  version 1
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-30
 *	@version 0.1
 *
 *	This class is used to handle the listAppl request primitive of CP-AP protocol, version 1
 *
 */
class CpProt_listAppl_v1_request: public CpProt_Primitive
{
public:
	/** @brief CpProt_listAppl_v1_request default constructor
	 *
	 *	CpProt_listAppl_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_listAppl_v1_request();

	/** @brief Builds and returns a "ListAppl request" primitive of CP-AP protocol, vers. 1
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "ListAppl request v1" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_listAppl_v1_request(); }


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
	uint8_t _NodeName[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];	/// NodeName field
};


//=================================================================//
// Class for the primitive:  "List Application" reply  version 1   //
//=================================================================//

/** @class CpProt_listAppl_v1_reply ACS_DSD_CpProt_ListApplPrimitives.h
 *	@brief Represents the primitive  "List Application reply" of CP-AP protocol,  version 1
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-30
 *	@version 0.1
 *
 *	This class is used to handle the listAppl reply primitive of CP-AP protocol, version 1
 *
 */
class CpProt_listAppl_v1_reply: public CpProt_Primitive
{
public:
	/** @brief CpProt_listAppl_v1_reply default constructor
	 *
	 *	CpProt_listAppl_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_listAppl_v1_reply();

	/** @brief Builds and returns a "ListAppl reply" primitive of CP-AP protocol, vers. 1
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "ListAppl reply v1" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_listAppl_v1_reply(); }


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


	/** @brief get_Error_field method
	 *
	 *	get_Error_field method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_Error_field() { return _Error; }


	/** @brief get_service_domain method
	 *
	 *	get_service_domain method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_domain(char *buff, size_t buffsize) { return ACS_DSD_Utils::get_primitive_string_field(_Domain, buff, buffsize); }


	/** @brief get_service_name method
	 *
	 *	get_service_name method detailed description
	 *
	 *	@param[in] buff Description
	 *	@param[in] buffsize Description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int get_service_name(char *buff, size_t buffsize) { return ACS_DSD_Utils::get_primitive_string_field(_ApplService, buff, buffsize); }

	/** @brief get_conn_type method
	 *
	 *	get_conn_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_conn_type() { return _ConnType; }

	/** @brief get_num_of_conn method
	 *
	 *	get_num_of_conn method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_num_of_conn () { return _NrOfConn; }

	/** @brief get_ip1 method
	 *
	 *	get_ip1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_ip1() { return _ip1; }

	/** @brief get_port1 method
	 *
	 *	get_port1 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_port1() { return _port1; }

	/** @brief get_ip2 method
	 *
	 *	get_ip2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint32_t get_ip2() { return _ip2; }

	/** @brief get_port2 method
	 *
	 *	get_port2 method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_port2() { return _port2; }

public:
	uint8_t  _Error;													/// Error field
	uint8_t  _Domain[acs_dsd::PCP_FIELD_SIZE_APP_DOMAIN_NAME];		    /// Domain field
	uint8_t  _ApplService[acs_dsd::PCP_FIELD_SIZE_APP_SERVICE_NAME];	/// ApplService field
	uint16_t _ConnType;													/// ConnType field
	uint16_t _NrOfConn;													/// NrOfConn field
	uint32_t _ip1;														/// IP1 field
	uint16_t _port1;													/// Port1 field
	uint32_t _ip2;														/// IP2 field
	uint16_t _port2;													/// Port2 field
};

#endif /* ACS_DSD_CPPROT_LISTAPPLPRIMITIVES_H_ */
