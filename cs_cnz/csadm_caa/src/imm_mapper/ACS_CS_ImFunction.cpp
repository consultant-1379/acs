/*
 * * @file ACS_CS_ImFunction.cpp
 *	@brief
 *	Class method implementation for ACS_CS_ImFunction.
 *
 *  This module contains the implementation of class declared in
 *  the ACS_CS_ImFunction.h module
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-05-07
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2013
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-05-07 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_CS_ImFunction.h"
#include "ACS_CS_ImImmObject.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Trace.h"

#include "ACS_CC_Types.h"

#include <sstream>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

ACS_CS_Trace_TDEF(ACS_CS_ImFunction_TRACE);

namespace FunctionDistribution {
		char domainAttribute[] = "domain";
		char descriptionAttribute[] = "description";
		char funReferenceAttribute[] = "functionDn";
	};

namespace parseSymbol {
		const char comma = ',';
		const char equal = '=';
		const char atSign = ':';
};

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImFunction);

ACS_CS_ImFunction::ACS_CS_ImFunction()
: ACS_CS_ImBase(),
  m_FunctionId(),
  m_Domain(),
  m_Description(),
  m_FunctionReferenceDn()
{
	type = APFUNCTION_T;
}

ACS_CS_ImFunction::ACS_CS_ImFunction(const ACS_CS_ImFunction &other )
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	*this = other;
}

ACS_CS_ImFunction::~ACS_CS_ImFunction()
{

}

bool ACS_CS_ImFunction::isValid(int& errNo, const ACS_CS_ImModel& model)
{
	errNo = 0;
	return true;
}

bool ACS_CS_ImFunction::modify(ACS_APGCC_AttrModification** attrMods)
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;
	return result;
}

ACS_CS_ImBase* ACS_CS_ImFunction::clone() const
{
	ACS_CS_ImBase* basePtr = new ACS_CS_ImFunction(*this);
	return basePtr;
}

ACS_CS_ImFunction& ACS_CS_ImFunction::operator=(const ACS_CS_ImFunction& rhs)
{
	//ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	// copy base class member
	rdn.assign(rhs.rdn);
	action = rhs.action;
	type = rhs.type;

	// copy class member
	m_FunctionId.assign(rhs.m_FunctionId);
	m_Domain.assign(rhs.m_Domain);
	m_Description.assign(rhs.m_Description);
	m_FunctionReferenceDn.assign(rhs.m_FunctionReferenceDn);
	//ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d]", __FUNCTION__, __LINE__ ));
	return *this;
}

ACS_CS_ImImmObject* ACS_CS_ImFunction::toImmObject(bool onlyModifiedAttrs)
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

    immObject->setClassName(ACS_CS_ImmMapper::CLASS_FUNCTION.c_str());
    immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

    std::vector<ACS_CS_ImValuesDefinitionType> attrList;

    if(!onlyModifiedAttrs)
	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_FUNCTION_ID.c_str(), m_FunctionId ));

    attrList.push_back(ACS_CS_ImUtils::createStringType(FunctionDistribution::domainAttribute, m_Domain));
    attrList.push_back(ACS_CS_ImUtils::createStringType(FunctionDistribution::descriptionAttribute, m_Description));
    attrList.push_back(ACS_CS_ImUtils::createNameType(FunctionDistribution::funReferenceAttribute, m_FunctionReferenceDn));

    immObject->setAttributeList(attrList);

    ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d]", __FUNCTION__, __LINE__ ));
    return immObject;
}

bool ACS_CS_ImFunction::getFunctionName(std::string& functionName) const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;

	// Split functionId to key and value
	size_t equalPos = m_FunctionId.find_first_of(parseSymbol::equal);

	if( (std::string::npos != equalPos) )
	{
		// Assemble Function Name as "domain:service"
		functionName = m_Domain;
		functionName += parseSymbol::atSign;
		functionName += m_FunctionId.substr(equalPos + 1);
		result = true;
	}

	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] functionName:<%s>, result:<%s>", __FUNCTION__, __LINE__, functionName.c_str(), (result ? "TRUE" : "FALSE") ));
	return result;
}

bool ACS_CS_ImFunction::getServiceName(std::string& serviceName) const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;

	// Split functionId to key and value
	size_t equalPos = m_FunctionId.find_first_of(parseSymbol::equal);

	if( (std::string::npos != equalPos) )
	{
		// Get service name
		serviceName = m_FunctionId.substr(equalPos + 1);
		result = true;
	}

	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] Service Name:<%s>, result:<%s>", __FUNCTION__, __LINE__, serviceName.c_str(), (result ? "TRUE" : "FALSE") ));
	return result;
}

bool ACS_CS_ImFunction::getAPNodeId(uint16_t& apId) const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	apId = 0;
	bool result = false;
	std::string apName;

	// Get AP name from DN of Function object
	// e.g."functionId=CHS,apgId=AP1,AxeFunctionDistributionfunctionDistributionMId=1"
	if( getAPName(apName) )
	{
		// Check if given name equals any of "AP1" to "AP16"
	    for (uint16_t i = 1; i <= 16; ++i)
	    {
		   std::stringstream tmpAPName;
		   tmpAPName<<"AP"<<i;

		   if (apName.compare(tmpAPName.str()) == 0)
		   {
			   //store AP identifier , e.g. 2000 + 1 => 2001
			   apId = static_cast<uint16_t>(ACS_CS_Protocol::SysType_AP) + i;
			   result = true;
			   break;
		   }
	   }
	}

	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] apId:<%d>", __FUNCTION__, __LINE__, apId ));
	return result;
}

uint16_t ACS_CS_ImFunction::getAPNodeId() const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	uint16_t apId = 0;
	std::string apName;
	// Get AP name from DN of Function object
	// e.g."functionId=CHS,apgId=AP1,AxeFunctionDistributionfunctionDistributionMId=1"
	if( getAPName(apName) )
	{
		// Check if given name equals any of "AP1" to "AP16"
		for (uint16_t i = 1; i <= 16; ++i)
		{
		   std::stringstream tmpAPName;
		   tmpAPName<<"AP"<<i;

		   if (apName.compare(tmpAPName.str()) == 0)
		   {
			   //store AP identifier, e.g. 2000 + 1 => 2001
			   apId = static_cast<uint16_t>(ACS_CS_Protocol::SysType_AP) + i;
			   break;
		   }
	   }
	}
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] apId:<%d>", __FUNCTION__, __LINE__, apId ));
	return apId;
}

bool ACS_CS_ImFunction::equals(const ACS_CS_ImBase& other) const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;
	// Check DN
	if(ACS_CS_ImBase::equals(other))
	{
		try
		{
			const ACS_CS_ImFunction& functionObjRef = dynamic_cast<const ACS_CS_ImFunction& >(other);
			result = (m_Domain.compare(functionObjRef.m_Domain) == 0);
		}
		catch (const std::bad_cast &e)
		{
			// failed reference cast
		}
	}
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d]", __FUNCTION__, __LINE__, (result ? "TRUE" : "FALSE") ));
	return result;
}

bool ACS_CS_ImFunction::getAPName(std::string& apName) const
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	bool result = false;
	size_t tagStartPos = rdn.find(ACS_CS_ImmMapper::ATTR_APGNODE_ID);
	// Check if the tag is present
	if( std::string::npos != tagStartPos )
	{
		// get DN of apg object
		std::string apNodeDN = rdn.substr(tagStartPos);

		size_t equalPos = apNodeDN.find_first_of(parseSymbol::equal);
		size_t commaPos = apNodeDN.find_first_of(parseSymbol::comma);

		if( (std::string::npos != equalPos) &&  (std::string::npos != commaPos))
		{
			apName = apNodeDN.substr(equalPos + 1, (commaPos - equalPos - 1) );
			// make the value in upper case
			ACS_APGCC::toUpper(apName);
			result = true;
		}
	}
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] apName:<%s>, result:<%s>", __FUNCTION__, __LINE__, apName.c_str(), (result ? "TRUE" : "FALSE") ));
	return result;
}

void ACS_CS_ImFunction::setParentDn(const std::string& newParent)
{
	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	rdn.clear();
	// Assemble the new parent DN: apgId=AP<i>,...
	std::ostringstream newParentDn;
	newParentDn << m_FunctionId << parseSymbol::comma;
	newParentDn << ACS_CS_ImmMapper::ATTR_APGNODE_ID << parseSymbol::equal << newParent;
	newParentDn << parseSymbol::comma << ACS_CS_ImmMapper::RDN_FUNCTION_DIST << std::ends;

	rdn = newParentDn.str();

	ACS_CS_TRACE((ACS_CS_ImFunction_TRACE, "Leave [%s@%d] new parent:<%s>", __FUNCTION__, __LINE__, rdn.c_str() ));
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive& ar, ACS_CS_ImFunction& function, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(function);
         ar & function.m_FunctionId;
         ar & function.m_Domain;
         ar & function.m_Description;
         ar & function.m_FunctionReferenceDn;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImFunction & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImFunction & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImFunction & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImFunction & base, const unsigned int /*version*/);
   }
}
