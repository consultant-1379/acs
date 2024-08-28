#ifndef ACS_DSD_SRVPROT_PRIMITIVE_H_
#define ACS_DSD_SRVPROT_PRIMITIVE_H_

#include "ACS_DSD_PrimitiveDataHandler.h"

/** @file ACS_DSD_SrvProt_ProtocolPrimitive.h
 *	@brief
 *	@author xassore
 *	@date 2011-01-20
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
 *	| 0.1    | 2011-01-20 | xassore      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ServicePrimitive.h"

/** @class SrvProt_Primitive ACS_DSD_SrvProt_Primitive.h
 *	@brief Represents a DSD_SERVER-DSD_SERVER  protocol primitive
 *	@author xassore
 *	@date 2011-01-20
 *	@version 0.1
 *
 *	Represents a DSD_SERVER-DSD_SERVER protocol primitive
 *
 */
class SrvProt_Primitive: public ACS_DSD_ServicePrimitive
{
public:
	/** @brief SrvProt_Primitive constructor
	 *
	 *	SVRAPP_Primitive constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	SrvProt_Primitive() : ACS_DSD_ServicePrimitive(PROTOCOL_SRVSRV) {}


	/** @brief Packes the primitive into a DSD API PrimitiveDataHandler object
	 *
	 *	Used to pack the primitive into a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[out] pdh the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<> & pdh) const = 0;


	/** @brief Builds the primitive from a DSD API PrimitiveDataHandler object
	 *
	 *	Used to build the primitive from a DSD API PrimitiveDataHandler object.
	 *
	 *	@param[in] pdh pointer to the DSD API PrimitiveDataHandler object
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh) = 0;
};

#endif /* ACS_DSD_SRVPROT_PRIMITIVE_H_ */
