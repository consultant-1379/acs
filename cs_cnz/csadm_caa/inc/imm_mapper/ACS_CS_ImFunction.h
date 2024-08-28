/*
 * * @file ACS_CS_ImFunction.h
 *	@brief
 *	Header file for ACS_CS_ImFunction class.
 *  This module contains the declaration of the class ACS_CS_ImFunction.
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-05-06
 *	@version 1.0.0
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
 *	| 1.0.0  | 2013-05-06 | qvincon      | File created.                      |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CS_IMFUNCTION_H_
#define ACS_CS_IMFUNCTION_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CC_Types.h"
#include <string>

#include <boost/serialization/export.hpp>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class ACS_CS_ImImmObject;

namespace FunctionDistribution {
		extern char domainAttribute[];
		extern char descriptionAttribute[];
		extern char funReferenceAttribute[];
	};

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_CS_ImFunction : public ACS_CS_ImBase
{
 public:

	ACS_CS_ImFunction();
	ACS_CS_ImFunction(const ACS_CS_ImFunction& other );

	virtual ~ACS_CS_ImFunction();

	virtual bool isValid(int& errNo, const ACS_CS_ImModel& model);
    virtual bool modify(ACS_APGCC_AttrModification** attrMods);
    virtual ACS_CS_ImBase* clone() const;

    ACS_CS_ImFunction& operator=(const ACS_CS_ImFunction& rhs);

    ACS_CS_ImImmObject* toImmObject(bool onlyModifiedAttrs = false);

    bool getFunctionName(std::string& functionName) const;
    bool getServiceName(std::string& serviceName) const;

    bool getAPName(std::string& apName) const;

    bool getAPNodeId(uint16_t& apId) const;
    uint16_t getAPNodeId() const;

    inline const char* getFunctionDN() const { return m_FunctionId.c_str(); };

    void setParentDn(const std::string& newParent);

    inline void setToBeDelete() { action = DELETE; };
    inline void setToBeCreate() { action = CREATE; };

    std::string m_FunctionId;
	std::string m_Domain;
	std::string m_Description;
	std::string m_FunctionReferenceDn;

 protected:

	virtual bool equals(const ACS_CS_ImBase& other) const;

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive& ar, ACS_CS_ImFunction& function, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImFunction, "ACS_CS_ImFunction");


#endif /* ACS_CS_IMFUNCTION_H_ */
