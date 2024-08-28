#ifndef ACS_DSD_SRVPROT_HWCCHANGEPRIMITIVES_H_
#define ACS_DSD_SRVPROT_HWCCHANGEPRIMITIVES_H_

/** @file ACS_DSD_SrvProt_HwcChange_Primitives.h
 *	@brief
 *	@author xcasale
 *	@date
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| 0.1    |2014-06-19  | xcasale      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include <sys/types.h>

#include "ACS_DSD_SrvProt_Primitive.h"
#include "ACS_DSD_PrimitiveDataHandler.h"

//=================================================================//
// Class for the primitive:  "HWC Change Notify" request  vers 1	 //
//=================================================================//

/** @class SrvProt_HwcChange_notify_v1_request ACS_DSD_SrvProt_HwcChange_Primitives.h
 *	@brief Represents the primitive  "HWC Change Notify request" of DSD_Server-DSD_Server protocol
 *	@author xcasale
 *	@date 2013-06-19
 *	@version 0.1
 *
 *	This class is used to handle the HWC Change Notify requests (for DSD_Server-DSD_Server protocol)
 *
 */
class SrvProt_HwcChange_notify_v1_request : public SrvProt_Primitive {
public:
	/** @brief SrvProt_HwcChange_notify_v1_request default constructor
	 *
	 *	SrvProt_HwcChange_notify_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_HwcChange_notify_v1_request();

	/** @brief Builds and returns a "HWC Change Notify" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "HWC Change Notify" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_HwcChange_notify_v1_request(); };

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

	/** @brief get_fbn method
	 *
	 *	get_fbn method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_fbn() { return _fbn; };

	/** @brief set_fbn method
	 *
	 *	set_fbn method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_fbn(uint16_t fbn) { _fbn = fbn; };

	/** @brief get_op_type method
	 *
	 *	get_op_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_op_type() { return _op_type; };

	/** @brief set_op_type method
	 *
	 *	set_op_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_op_type(uint8_t op_type) { _op_type = op_type; };

	/** @brief get_system_type method
	 *
	 *	get_system_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint16_t get_system_type() { return _system_type; };

	/** @brief set_system_type method
	 *
	 *	set_system_type method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_system_type(uint16_t system_type) { _system_type = system_type; };

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
	void set_system_id(int32_t sys_id) { _system_id = sys_id; };

	/** @brief get_side method
	 *
	 *	get_side method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int8_t get_side() { return _side; };

	/** @brief set_side method
	 *
	 *	set_side method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_side(int8_t side) { _side = side; };

private:
	uint16_t _fbn;							/// Functional Board Name field
	uint8_t _op_type;						/// Operation Type field
	uint16_t _system_type;			/// System Type field
	int32_t _system_id;					/// System ID field
	int8_t _side;								/// Side field
};



//=================================================================//
// Class for the primitive:  "HWC Change Notify" reply  vers 1	   //
//=================================================================//

/** @class SrvProt_HwcChange_notify_v1_reply ACS_DSD_SrvProt_HwcChange_Primitives.h
 *	@brief Represents the primitive  "HWC Change Notify reply" of DSD_Server-DSD_Server protocol
 *	@author xcasale
 *	@date 2013-06-19
 *	@version 0.1
 *
 *	This class is used to handle the HWC Change Notify responses (for DSD_Server-DSD_Server protocol)
 *
 */
class SrvProt_HwcChange_notify_v1_reply : public SrvProt_Primitive {
public:
	/** @brief SrvProt_HwcChange_notify_v1_reply default constructor
	 *
	 *	SrvProt_HwcChange_notify_v1_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_HwcChange_notify_v1_reply();

	/** @brief Builds and returns a "HWC Change Notify" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "HWC Change Notify" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static SrvProt_Primitive* make_primitive() { return new(std::nothrow) SrvProt_HwcChange_notify_v1_reply(); };

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
	inline uint8_t get_error_field() const { return _error; };

	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	inline void set_error_field(uint8_t val) { _error = val; };


private:
	uint8_t _error;													/// Error field
};

#endif /* ACS_DSD_SRVPROT_HWCCHANGEPRIMITIVES_H_ */
