#ifndef ACS_DSD_APPPROT_PRIMITIVE_H_
#define ACS_DSD_APPPROT_PRIMITIVE_H_

#include "ACS_DSD_PrimitiveDataHandler.h"

/** @file ACS_DSD_SVRAPP_ProtocolPrimitive.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
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
 *	| 0.1    | 2010-11-11 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_ServicePrimitive.h"

/** @class AppProt_Primitive ACS_DSD_AppProt_Primitive.h
 *	@brief Represents a SERVICE-APPLICATION protocol primitive
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
 *	@version 0.1
 *
 *	Represents a SERVICE-APPLICATION protocol primitive
 *
 */
class AppProt_Primitive: public ACS_DSD_ServicePrimitive
{
public:
	/** @brief AppProt_Primitive constructor
	 *
	 *	SVRAPP_Primitive constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_Primitive() : ACS_DSD_ServicePrimitive(PROTOCOL_SVRAPP){}


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

protected:

	/// Constants used by "notification" methods to communicate the notification result
	enum Notification_Result
	{
		NOTIFICATION_OK = 0,
		NOTIFICATION_CONN_ERROR = -1,
		NOTIFICATION_UNSUPPORTED = -2,
		NOTIFICATION_GENERIC_ERROR = -3,
		NOTIFICATION_PROTOCOL_ERROR = -4,
		NOTIFICATION_RESP_RECEIVE_ERROR = -5,
		NOTIFICATION_REQ_SEND_ERROR = -6
	};

	/** @brief Notifies the primitive to an AP system
	 *
	 *	This method is invoked to notify the primitive to an AP system.
	 *
	 *	@param[in] ap_system_id  identifier of the AP System to be notified.
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int notify_to_AP(int32_t /*ap_system_id*/) const { return NOTIFICATION_UNSUPPORTED; }


	/** @brief Notifies the primitive to all (remote) AP systems
	 *
	 *	This method is invoked to notify the primitive to all remote AP systems.
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int notify_to_all_AP() const;
};

#endif /* ACS_DSD_APPPROT_PRIMITIVE_H_ */
