#ifndef ACS_DSD_CPPROT_UNKNOWNPRIMITIVE_H_
#define ACS_DSD_CPPROT_UNKNOWNPRIMITIVE_H_

/** @file ACS_DSD_CpProt_UnknownPrimitive.h
 *	@brief
 *	@author xludesi (Luca De Simone)
 *	@date 2010-12-15
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
 *	| 0.1    | 2010-12-15 | xludesi      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ACS_DSD_CpProt_Primitive.h"


//=================================================================//
// Class for the primitive:  "Unknown" reply  of CP-AP protocol    //
//=================================================================//

/** @class CpProt_unknown_reply ACS_DSD_CpProt_UnknownPrimitive.h
 *	@brief Represents the primitive  "Unknown reply"  of CP-AP protocol
 *	@author xludesi (Luca De Simone)
 *	@date 2010-11-15
 *	@version 0.1
 *
 *	This class is used to handle the Unknown reply primitive of CP-AP protocol
 *
 */
class CpProt_unknown_reply: public CpProt_Primitive
{
public:
	/** @brief CpProt_unknown_reply default constructor
	 *
	 *	CpProt_unknown_reply default constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	CpProt_unknown_reply();

	/** @brief Builds and returns a "Unknown reply" primitive
	 *
	 *	Called by ProtocolPrimiteFactory object to build a "Unknown reply" primitive
	 *
	 *	@return Return Description
	 *	@remarks the method allocates the primitive object on the heap; so the caller must take care to release memory allocated.
	 */
	static CpProt_Primitive * make_primitive() { return new(std::nothrow) CpProt_unknown_reply(); }


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
	void set_Unknown_field(uint8_t val) { _Unknown = val; }


private:
	uint8_t _Unknown;					/// Unknown field
};


#endif /* ACS_DSD_CPPROT_UNKNOWNPRIMITIVE_H_ */
