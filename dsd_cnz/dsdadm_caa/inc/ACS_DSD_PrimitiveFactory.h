#ifndef ACS_DSD_PRIMITIVEFACTORY_H_
#define ACS_DSD_PRIMITIVEFACTORY_H_

/** @file ACS_DSD_PrimitiveFactory.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-12
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
 *	| 0.1    | 2010-11-12 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ProtocolsDefs.h"

class ACS_DSD_ServicePrimitive;
typedef ACS_DSD_ServicePrimitive* (*CreatePrimitiveFunc)();

/** @class ACS_DSD_PrimitiveFactory ACS_DSD_PrimitiveFactory.h
 *	@brief Used to build DSD primitives
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-12
 *	@version 0.1
 *
 *	This class is used by DSD Service Handler classes, to manipulate DSD protocols primitives. When a DSD Service Handler receives a message from the peer,
 *	it needs to parse the message to obtain the DSD primitive packed in the message. The first step is to build a primitive based on the primitive ID and the protocol version.
 *
 */
class ACS_DSD_PrimitiveFactory
{
public:
	/// ACS_DSD_PrimitiveFactory Constants
	typedef enum
	{
		PF_PRIMITIVE_IS_IMPLEMENTED = 0,
		PF_PRIMITIVE_NOT_IMPLEMENTED = 1,
		PF_CHECK_OK = 2,
		PF_BUILD_PRIMITIVE_OK = 0,
		PF_UNKNOWN_PROTOCOL = -1,
		PF_INVALID_VERSION = -2,
		PF_INVALID_PRIMITIVE = -3,
		PF_MEMORY_ERROR = -4
	} PF_Const;

	/** @brief ACS_DSD_PrimitiveFactory constructor
	 *
	 *	Initializes the Primitive Factory building the internal function table that will be used to make DSD primitives
	 *
	 *	@remarks Remarks
	 */
	ACS_DSD_PrimitiveFactory();

	/** @brief ACS_DSD_PrimitiveFactory destructor
	 *
	 *	ACS_DSD_PrimitiveFactory destructor detailed descrition
	 *
	 *	@remarks Remarks
	 */
	virtual ~ACS_DSD_PrimitiveFactory() {}

	/** @brief Registers a DSD primitive with the Primitive Factory
	 *
	 *	During DSD daemon startup, this method is invoked to register a DSD Service Primitive, indicating
	 *  the address of the "make_primitive()" static method of the class representing the primitive.
	 *
	 *	@param[in] prot_id ID of the DSD Service Protocol
	 *	@param[in] prot_version Version of the DSD Service Protocol
	 *	@param[in] primitive_id ID of the DSD Service Primitive
	 *	@param[in] create_primitive_func address of the static method "make_primitive()" of the class representing the DSD primitive.
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	int register_primitive(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id, CreatePrimitiveFunc create_func);


	/** @brief Creates a (raw) ACS_DSD_ServicePrimitive
	 *
	 *	Creates a (raw) DSD Service Primitive
	 *
	 *	@param[in] prot_id ID of the DSD Service Protocol
	 *	@param[in] prot_version Version of the DSD Service Protocol
	 *	@param[in] primitive_id ID of the DSD Service Primitive
	 *	@param[out] p_prim_pointer to the address of a DSD primitive object.
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	PF_Const build_primitive(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id, ACS_DSD_ServicePrimitive * & p_prim);

private:
	// registers primitives for SVRAPP protocol
	int register_AppProt_primitives();

	// registers primitives for CPAP protocol
	int register_CPAP_primitives();

	// registers primitives for SVRSVR protocol
	int register_SRVSRV_primitives();


	/** @brief check method
	 *
	 *	search_primitive_func method detailed description
	 *
	 *	@param[in] prot_id ID of the DSD Service Protocol
	 *	@param[in] prot_version Version of the DSD Service Protocol
	 *	@param[in] primitive_id ID of the DSD Service Primitive
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	PF_Const check(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id);


	/** @brief search_make_primitive_func method
	 *
	 *	search_make_primitive_func method detailed description
	 *
	 *	@param[in] prot_id ID of the DSD Service Protocol
	 *	@param[in] prot_version Version of the DSD Service Protocol
	 *	@param[in] primitive_id ID of the DSD Service Primitive
	 *	@param[out] make_primitive_func description.
	 *	@param[in] serch_default_on_unsupported_version description
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	PF_Const get_make_primitive_func_address(ACS_DSD_ProtocolID prot_id, uint prot_version, uint primitive_id, CreatePrimitiveFunc *& make_prim_func_addr, bool search_default_on_unsupported_version = false);

private:
	static const uint num_CPAP_primitives = CPAP_MAX_PRIMITIVE_ID - CPAP_MIN_PRIMITIVE_ID + 1;
	static const uint num_SVRAPP_primitives = SVRAPP_MAX_PRIMITIVE_ID - SVRAPP_MIN_PRIMITIVE_ID + 1;
	static const uint num_SRVSRV_primitives = SRVSRV_MAX_PRIMITIVE_ID - SRVSRV_MIN_PRIMITIVE_ID + 1;

	// table of "Make_primitive" functions for CP-AP Protocol primitives
	CreatePrimitiveFunc _tbl_CPAP[CPAP_PROTOCOL_VERSION_MAX + 1][num_CPAP_primitives];

	// table of "Make_primitive" functions for SV_APP Protocol primitives
	CreatePrimitiveFunc _tbl_SVRAPP[SVRAPP_PROTOCOL_VERSION_MAX + 1][num_SVRAPP_primitives];

	// table of "Make_primitive" functions for SRV-SRV Protocol primitives
	CreatePrimitiveFunc _tbl_SRVSRV[SRVSRV_PROT_VERS_MAX + 1][num_SRVSRV_primitives];
};

#endif /* ACS_DSD_PRIMITIVEFACTORY_H_ */
