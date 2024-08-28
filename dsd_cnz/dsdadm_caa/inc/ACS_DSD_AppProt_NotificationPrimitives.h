#ifndef ACS_DSD_APPPROT_NOTIFICATIONPRIMITIVES_H_
#define ACS_DSD_APPPROT_NOTIFICATIONPRIMITIVES_H_

/** @file ACS_DSD_AppProt_NotificationPrimitives.h
 *	@brief
 *	@author xassore
 *	@date 2011-01-07
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
 *	| 0.1    | 2011-01-07 | xassore      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_AppProt_Primitive.h"
#include <sys/types.h>
#include "ACS_DSD_PrimitiveDataHandler.h"

//=================================================================//
// Class for the primitive:  "Address" request  vers 1   		   //
//=================================================================//

/** @class AppProt_Notification_request ACS_DSD_AppProt_NotificationPrimitive.h
 *	@brief Represents the primitive  "Notification request" of SERVICE-APPLICATION protocol
 *	@author xassore
 *	@date 2011-01-07
 *	@version 0.1
 *
 *	This class is used to handle the notification requests (for SERVICE-APPLICATION protocol)
 *
 */
class AppProt_notification_v1_request: public AppProt_Primitive
{
public:
	/** @brief AppProt_notification_v1_request default constructor
	 *
	 *	AppProt_notification_v1_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_notification_v1_request();

	/** @brief Builds and returns an "Notification" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Notification" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static AppProt_Primitive* make_primitive() { return new(std::nothrow) AppProt_notification_v1_request(); };


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

private:
	int32_t _system_id;								/// SystemId field
};


//====================================================================//
// Class for the primitive:  Notification  Indication vers 1   //
//====================================================================//

/** @class AppProt_notification_v1_indication ACS_DSD_AppProt_NotificationPrimitives.h
*	@brief Represents the primitive  "Notification indication" of SERVICE-APPLICATION protocol
 *	@author xassore
 *	@date 2011-01-07
 *	@version 0.1
 *
 *	This class is used to handle Notification indication primitive (SERVICE-APPLICATION protocol)
 *
 */
class AppProt_notification_v1_indication: public AppProt_Primitive
{
public:
	/** @brief AppProt_notification_v1_indication default constructor
	 *
	 *	AppProt_notification_v1_indication default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	AppProt_notification_v1_indication();


	/** @brief Builds and returns an "Notification INET Socket Response" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build an "Notification INET Socket response" primitive, version 1
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static AppProt_Primitive* make_primitive() { return new (std::nothrow) AppProt_notification_v1_indication(); };


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
	uint8_t get_cpState() { return _cpState; };


	/** @brief set_error_fielsd method
	 *
	 *	set_error_fielsd method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	void set_cpState(uint8_t val) { _cpState = val; };



private:
	uint8_t _cpState;						/// CP state (0-parallel/1-separated/2-undefined)
};

#endif /* ACS_DSD_APPPROT_NOTIFICATIONPRIMITIVES_H_ */
