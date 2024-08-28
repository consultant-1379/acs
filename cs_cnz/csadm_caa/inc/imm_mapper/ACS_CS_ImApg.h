/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2013
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
  *
  *
  *
  * ACS_CS_ImApg.h
  *
  *  Created on: May 3, 2013
  *      Author: egiacri
  *
  *
 -------------------------------------------------------------------------*//*
  *
  * REVISION HISTORY
  *
  * DATE        USER     DESCRIPTION/TR
  * --------------------------------
  * 2013-05-02  EGIACRI  Starting from scratch
  *
  ****************************************************************************/
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CS_IMAPG_H_
#define ACS_CS_IMAPG_H_

#include <boost/serialization/export.hpp>

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "ACS_CS_ImBase.h"

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class ACS_CS_ImImmObject;
/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_CS_ImApg : public ACS_CS_ImBase
{
 public:

	/**		@brief	constructor of ACS_CS_ImApg class
	 *
	 */
	ACS_CS_ImApg();

	/**		@brief	constructor with parameter of ACS_CS_ImApg class
	 *
	 */
	ACS_CS_ImApg(const ACS_CS_ImApg &other);

	/**		@brief	destructor of ACS_CS_ImApg class
	 *
	 */
	virtual ~ACS_CS_ImApg();
	/**		@brief	operator = overload
	 *
	 */

	ACS_CS_ImApg& operator=(const ACS_CS_ImApg &rhs);

	/** @brief	isValid
	 *
	 *  This method extracts an attribute value of an object from the DN
	 *
	 *  @param errNo : the
	 *
	 *  @param model : the attribute name
	 *
	 *  @param attributeValue: the attribute value retrieved
	 *
	 *  @return true on success, false otherwise
	 *
	 *  @remarks Remarks
	 */
	virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);

	/** @brief	modify
	 *
	 *  This method extracts an attribute value of an object from the DN
	 *
	 *  @param attrMods : the
	 *
	 *
	 *  @return true on success, false otherwise
	 *
	 *  @remarks Remarks
	 */
	virtual bool modify(ACS_APGCC_AttrModification** attrMods);

	/** @brief	clone
	 *
	 *  This method clone the current class returning the pointer to the clone
	 *
	 *
	 *
	 *  @remarks Remarks
	 */
	virtual ACS_CS_ImBase* clone() const;

	/** @brief	toImmObject
	 *
	 *  This method creates an object of the class ACS_CS_ImImmObject
	 *  extracting the content of the current class
	 *
	 *  @remarks Remarks
	 */
	ACS_CS_ImImmObject* toImmObject(bool onlyModifiedAttrs = false);

	/** @brief	toImmObject
	 *
	 *  This method get the APG node name (eg. AP1)
	 *
	 *  @remarks Remarks
	 */
	bool getAPGNodeName(std::string& apgNode) const;

	/**		@brief	m_ApgId member
	 *
	 */
	std::string m_ApgId;

protected:

	/** @brief	equals
	 *
	 *  This method extracts all attribute values of an object comparing with those contained in the current class
	 *   and returns true if all of the elements in both objects match.
	 *
	 *  @param object : the
	 *
	 *  @return true on success, false otherwise
	 *
	 *  @remarks Remarks
	 */
	virtual bool equals(const ACS_CS_ImBase &other) const;

private:

	/** @brief	copyObj
	 *
	 *  This method is the copy constructor of the class ACS_CS_ImApg it creates a copy of this class returning it.
	 *
	 *  @param object : the copy of the class
	 *
	 *  @remarks Remarks
	 */
	void copyObj(const ACS_CS_ImApg &object);

};

namespace boost {
	namespace serialization {
	template<class Archive>
	void serialize(Archive & ar, ACS_CS_ImApg & imapg, const unsigned int /*version*/);
	}
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImApg, "ACS_CS_ImApg");

#endif /* ACS_CS_IMAPG_H_ */
