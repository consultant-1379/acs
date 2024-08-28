#ifndef ACS_DSD_CPPROT_STARTUPPRIMITIVES_H_
#define ACS_DSD_CPPROT_STARTUPPRIMITIVES_H_

/** @file ACS_DSD_CpProt_StartupPrimitives.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-14
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
 *	| 0.1    | 2010-12-14 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_CpProt_Primitive.h"

//=================================================================//
// Class for the primitive:  "Start up" request  unknown version   //
//=================================================================//

/** @class CpProt_startup_v0_request ACS_DSD_CpProt_StartupPrimitives.h
 *	@brief Represents the primitive  "Startup request" of CP-AP protocol for unsupported version
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-12
 *	@version 0.1
 *
 *	This class is used to handle the startup request primitives sent by CP, when the protocol version is unsupported
 *
 */
class CpProt_startup_v0_request: public CpProt_Primitive
{
public:
	/** @brief CpProt_startup_v0_request default constructor
	 *
	 *	CpProt_startup_v0_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_startup_v0_request();

	/** @brief Builds and returns a "Startup request" primitive of CP-AP protocol, vers. 0
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Startup request v0" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_startup_v0_request(); }


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

private:
	uint64_t _AP_mask;													/// AP_mask field
	uint32_t _CP_EX_mask;												/// CP_EX_mask field
	uint32_t _CP_SB_mask;												/// CP_SB_mask field
	uint8_t	 _APidentity;												/// APidentity field
};




//=================================================================//
// Class for the primitive:  "Start up" request  vers 2   		   //
//=================================================================//

/** @class CpProt_startup_v2_request ACS_DSD_CpProt_StartupPrimitives.h
 *	@brief Represents the primitive  "Startup request"  vers. 2,  of CP-AP protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-12
 *	@version 0.1
 *
 *	This class is used to handle the startup requests sent by CP
 *
 */
class CpProt_startup_v2_request: public CpProt_Primitive
{
public:
	/** @brief CpProt_startup_v2_request default constructor
	 *
	 *	CpProt_startup_v2_request default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_startup_v2_request();

	/** @brief Builds and returns a "Startup request" primitive of CP-AP protocol, vers. 2
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Startup request v2" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_startup_v2_request(); }


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


private:
	uint64_t _AP_mask;													/// AP_mask field
	uint32_t _CP_EX_mask;												/// CP_EX_mask field
	uint32_t _CP_SB_mask;												/// CP_SB_mask field
	uint8_t	 _APidentity;												/// APidentity field
};



//=================================================================//
// Class for the primitive:  "Start up" reply  vers 2   		   //
//=================================================================//

/** @class CpProt_startup_v2_reply ACS_DSD_CpProt_StartupPrimitives.h
 *	@brief Represents the primitive  "Startup reply"  vers. 2,  of CP-AP protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-12
 *	@version 0.1
 *
 *	This class is used to handle the startup replies to be sent to CPs
 *
 */
class CpProt_startup_v2_reply: public CpProt_Primitive
{
public:
	/** @brief CpProt_startup_v2_reply default constructor
	 *
	 *	CpProt_startup_v2_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_startup_v2_reply();

	/** @brief Builds and returns a "Startup reply v2 " primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Startup reply v2" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_startup_v2_reply(); }


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


	/// Set error field
	void set_Error_field(uint8_t err_val) { _Error = err_val; }

	/// Set APidentity field
	void set_APidentity(uint8_t val) { _APidentity = val; }

	/// Set Reserved field
	void set_Reserved(uint8_t val) { _reserved = val; }

	/// Set IP1 field
	void set_ip_address_1(uint32_t val) { _ip1 = val; }

	/// Set IP2 field
	void set_ip_address_2(uint32_t val) { _ip2 = val; }

private:
	uint8_t _Error;						/// Error field
	uint8_t	_APidentity;				/// APidentity field
	uint8_t	_reserved;					/// Reserved field
	uint32_t _ip1;						/// IP address 1 field
	uint32_t _ip2;						/// IP address 2 field
};



#endif /* ACS_DSD_CPPROT_STARTUPPRIMITIVES_H_ */
