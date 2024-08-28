#ifndef ACS_DSD_SERVICEPRIMITIVE_H_
#define ACS_DSD_SERVICEPRIMITIVE_H_

/** @file ACS_DSD_ServicePrimitive.h
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

#include "ACS_DSD_ProtocolsDefs.h"
#include "ACS_DSD_PrimitiveDataHandler.h"
#include "ace/Log_Msg.h"
#include <string>

class ACS_DSD_ServiceHandler;

/** @class ACS_DSD_ServicePrimitive ACS_DSD_ServicePrimitive.h
 *	@brief Represents a single PRIMITIVE belonging to one among the three PROTOCOLS exposed by DSD Daemon
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-11
 *	@version 0.1
 *
 *	DSD Daemon exposes three protocols (CP-AP protocol, SERV-APP protocol, SERV-SERV protocol). This class represents a single  primitive belonging to one among the three DSD Protocols.
 *
 */
class ACS_DSD_ServicePrimitive
{
public:
	/// Return codes used by process() method
	enum SP_Process_Result
	{
		ACS_DSD_PRIM_PROCESS_OK_CLOSE_FLAG = 0x40,

		ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE = 0,
		ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE = 1,
		ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT = 2,

		ACS_DSD_PRIM_PROCESS_OK_CLOSE_WITH_RESPONSE = ACS_DSD_PRIM_PROCESS_OK_CLOSE_FLAG + ACS_DSD_PRIM_PROCESS_OK_WITH_RESPONSE,
		ACS_DSD_PRIM_PROCESS_OK_CLOSE_NO_RESPONSE = ACS_DSD_PRIM_PROCESS_OK_CLOSE_FLAG + ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE,
		ACS_DSD_PRIM_PROCESS_OK_CLOSE_RESPONSE_ALREADY_SENT = ACS_DSD_PRIM_PROCESS_OK_CLOSE_FLAG + ACS_DSD_PRIM_PROCESS_OK_RESPONSE_ALREADY_SENT,

		ACS_DSD_PRIM_PROCESS_ERROR = -1,
		ACS_DSD_PRIM_PROCESS_MEMORY_ERROR = -2,
		ACS_DSD_PRIM_PROCESS_IMM_ERROR = -3,
		ACS_DSD_PRIM_PROCESS_SEND_REPLY_ERROR = -4
	};

	enum SP_Validation_Result
		{
			ACS_DSD_PRIM_VALIDATION_OK = 0,
			ACS_DSD_PRIM_BAD_ERROR_PARAMETER = 1,
			ACS_DSD_PRIM_BAD_IP_ADDRESSES_COUNT = 2,
			ACS_DSD_PRIM_SERVICE_NOT_REGISTERED = 3,
			ACS_DSD_PRIM_ADDRESS_REQUEST_FAILED = 4
		};


	//===========================//
	//  Constructors / Destructor//
	//===========================//

	/** @brief ACS_DSD_ServicePrimitive constructor
	 *
	 *	 ACS_DSD_ServicePrimitive constructor
	 *
	 *	@param[in] protocol_id protocol to which the primitive belongs
	 *	@param[in] primitive_id primitive ID
	 *	@param[in] protocol_version protocol version
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ACS_DSD_ServicePrimitive(ACS_DSD_ProtocolID protocol_id = PROTOCOL_UNKNOWN, uint8_t primitive_id = acs_dsd::PCP_PRIMITIVE_ID_UNKNOWN, uint8_t protocol_version = 0)
	:_protocol_id(protocol_id),_primitive_id(primitive_id),_protocol_version(protocol_version) { }

	/// Destructor
	virtual ~ACS_DSD_ServicePrimitive() { /*ACS_DSD_LOG(dsd_logger, LOG_LEVEL_DEBUG, ACS_DSD_TEXT("Releasing primitive < %d >"), _primitive_id); */}


	//====================//
	// ACCESSOR methods   //
	//====================//

	/** @brief get_protocol_id method
	 *
	 *	get_protocol_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	ACS_DSD_ProtocolID get_protocol_id() { return this->_protocol_id; };


	/** @brief get_primitive_id_field method
	 *
	 *	get_protocol_id method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_primitive_id() { return this->_primitive_id; };


	/** @brief get_protocol_version method
	 *
	 *	get_protocol_version method detailed description
	 *
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	uint8_t get_protocol_version() { return this->_protocol_version; };


	/******************************************************************************************************/

	//===========================//
	//  PACK / UNPACK  methods   //
	//===========================//

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


	/********************************************************************************************************/

	/** @brief Processes the primitive
	 *
	 *	Processes the primitive
	 *
	 *	@param[out] response_primitive receive the primitive response
	 *	@param[in] service_handler protocol handler used to process the primitive and prepare the response
	 *	@return Return Description
	 *	@remarks Remarks
	 */
	virtual int process(ACS_DSD_ServicePrimitive * & /*response_primitive*/, ACS_DSD_ServiceHandler * /*service_handler*/) const
	{
		// default implementation
		return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
	}

	/// Returns the data members in text format
	virtual std::string to_text() const;

	/// Returns the process method error detailed description
	std::string process_error_descr() const { return _process_error_descr; };

protected:
	ACS_DSD_ProtocolID _protocol_id;			///	Protocol to which the primitive belongs
	int  _primitive_id;							/// Primitive Number
	uint32_t _protocol_version;					///	Protocol Version
	mutable std::string _process_error_descr;	/// Error description in case of process method failure
};

#endif /* ACS_DSD_SERVICEPRIMITIVE_H_ */
