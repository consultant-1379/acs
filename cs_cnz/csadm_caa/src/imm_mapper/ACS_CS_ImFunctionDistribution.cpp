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
  *  ACS_CS_ImFunctionDistribution.cpp
  *
  *  Created on: May 3, 2013
  *
  *  Author: egiacri
  *
  *
 -------------------------------------------------------------------------*//*
  *
  * REVISION HISTORY
  *
  * DATE        USER     DESCRIPTION/TR
  * --------------------------------
  * 2013-05-03  EGIACRI  Starting from scratch
  *
  ****************************************************************************/


#include "ACS_CS_ImFunctionDistribution.h"

#include "ACS_CS_ImUtils.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImFunctionDistribution);

ACS_CS_ImFunctionDistribution::ACS_CS_ImFunctionDistribution()
{
	type=FUNCTIONDISTRIBUTION_T;
}

ACS_CS_ImFunctionDistribution::ACS_CS_ImFunctionDistribution(const ACS_CS_ImFunctionDistribution &other) {

	*this = other;
}

ACS_CS_ImFunctionDistribution::~ACS_CS_ImFunctionDistribution()
{

}

ACS_CS_ImFunctionDistribution& ACS_CS_ImFunctionDistribution::operator=(const ACS_CS_ImFunctionDistribution &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImFunctionDistribution::isValid(int &errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;
	return true;
}

bool ACS_CS_ImFunctionDistribution::modify(ACS_APGCC_AttrModification **attrMods)
{
	return false;
}

ACS_CS_ImBase* ACS_CS_ImFunctionDistribution::clone() const
{
	   ACS_CS_ImBase *ImFunctionDistribution = new ACS_CS_ImFunctionDistribution(*this);
	   return ImFunctionDistribution;
}

ACS_CS_ImImmObject* ACS_CS_ImFunctionDistribution::toImmObject(bool onlyModifiedAttrs){

	   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

	   immObject->setClassName(ACS_CS_ImmMapper::CLASS_FUNCTION_DIST.c_str());
	   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

	   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

	   if(!onlyModifiedAttrs)
	   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_FUNCTION_DIST_ID.c_str(), m_FunctionDistributionMId));

	   immObject->setAttributeList(attrList);

	   return immObject;
}

bool ACS_CS_ImFunctionDistribution::equals(const ACS_CS_ImBase &obj) const
{
	bool result = false;
	if (ACS_CS_ImBase::equals(obj))
	{
	 	try
		{
			const ACS_CS_ImFunctionDistribution& other = dynamic_cast<const ACS_CS_ImFunctionDistribution &>(obj);
			result = (m_FunctionDistributionMId.compare(other.m_FunctionDistributionMId) == 0);
		}
		catch (const std::bad_cast &e)
		{
			// failed reference cast
		}
	}
	return result;
}

void ACS_CS_ImFunctionDistribution::copyObj(const ACS_CS_ImFunctionDistribution &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	m_FunctionDistributionMId = object.m_FunctionDistributionMId;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImFunctionDistribution & imfd, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(imfd);
         ar & imfd.m_FunctionDistributionMId;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImFunctionDistribution & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImFunctionDistribution & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImFunctionDistribution & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImFunctionDistribution & base, const unsigned int /*version*/);
   }
}
