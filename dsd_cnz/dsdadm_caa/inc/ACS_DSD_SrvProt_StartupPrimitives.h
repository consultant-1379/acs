#ifndef ACS_DSD_SRVPROT_STARTUPPRIMITIVES_H_
#define ACS_DSD_SRVPROT_STARTUPPRIMITIVES_H_

/** @file ACS_DSD_SrvProt_StartupPrimitives.h
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
 *	| 0.1    |2013-04-10  | xcasale      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <sys/types.h>

#include "ACS_DSD_SrvProt_Primitive.h"
#include "ACS_DSD_PrimitiveDataHandler.h"

//=================================================================//
// Class for the primitive:  "Startup" request  vers 1   		   //
//=================================================================//

/** @class SrvProt_startup_v1_request ACS_DSD_SrvProt_StartupPrimitives.h
 *	@brief Represents the primitive  "Startup request" of DSD_Server-DSD_Server protocol
 *	@author xcasale
 *	@date 2013-04-10
 *	@version 0.1
 *
 *	This class is used to handle the startup requests (for DSD_Server-DSD_Server protocol)
 *
 */
class SrvProt_startup_v1_request : public SrvProt_Primitive {
public:
	/** @brief SrvProt_startup_v1_request default constructor
	 *
	 *	SrvProt_startup_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_startup_v1_request();

	/** @brief Builds and returns a "Startup" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Startup" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_startup_v1_request(); };


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
	int32_t _system_id;												/// SystemId field
	uint8_t _node_state;											/// NodeState field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];			/// Node name field
};




//=================================================================//
// Class for the primitive:  "Startup" reply  vers 1      		   //
//=================================================================//

/** @class SrvProt_startup_v1_reply ACS_DSD_SrvProt_StartupPrimitives.h
 *	@brief Represents the primitive  "Startup reply" of DSD_Server-DSD_Server protocol
 *	@author xcasale
 *	@date 2013-04-10
 *	@version 0.1
 *
 *	This class is used to handle the startup responses (for DSD_Server-DSD_Server protocol)
 *
 */
class SrvProt_startup_v1_reply : public SrvProt_Primitive {
public:
	/** @brief SrvProt_startup_v1_reply default constructor
	 *
	 *	SrvProt_startup_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_startup_v1_reply();

	/** @brief Builds and returns a "Startup" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Startup" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_startup_v1_reply(); };


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
	uint8_t _error;													/// Error field
	int32_t _system_id;												/// SystemId field
	uint8_t _node_state;											/// NodeState field
	uint8_t _node_name[acs_dsd::PCP_FIELD_SIZE_NODE_NAME];			/// Node name field
};


#endif /* ACS_DSD_SRVPROT_STARTUPPRIMITIVES_H_ */
