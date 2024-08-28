/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImClusterCp.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ImClusterCp.h"
#include "ACS_CS_ImModel.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>



BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImClusterCp);

ACS_CS_ImClusterCp::ACS_CS_ImClusterCp()
{
   type = CLUSTERCP_T;
}

ACS_CS_ImClusterCp::ACS_CS_ImClusterCp(const ACS_CS_ImClusterCp *other)
{
	copyObj(other);
}

ACS_CS_ImClusterCp::~ACS_CS_ImClusterCp()
{}


bool ACS_CS_ImClusterCp::isValid(int & errNo, const ACS_CS_ImModel & model)
{
	errNo = 0;

	string cpName = ACS_CS_ImUtils::getIdValueFromRdn(this->rdn);

	// Validate Cp name
	if( cpName.compare(0, 2, "BC") != 0){
		errNo = TC_CPNAMEFAULTY;
		return false;
	}

	if (!ACS_CS_ImUtils::isValidAPZSystem(this->apzSystem)){
		ACS_CS_ImUtils::setFaultyAttribute(apzSystem);
		errNo = TC_INVALIDAPZSYSTEM;
		return false;
	}

	if (!ACS_CS_ImUtils::isValidCPType(this->cpType)){

		if (this->cpType == 65535)
			ACS_CS_ImUtils::setFaultyAttribute("<Empty>");
		else
			ACS_CS_ImUtils::setFaultyAttribute(cpType);

		errNo = TC_INVALIDCPTYPE;
		return false;
	}

	if (this->alias.compare("-") != 0)
	{
		if (!ACS_CS_ImUtils::isAllowedAliasName(this->alias))
		{
			ACS_CS_ImUtils::setFaultyAttribute(alias);
			errNo = TC_CPNAMEFAULTY;
			return false;
		}

		if (ACS_CS_ImUtils::isReservedCPName(this->alias))
		{
			ACS_CS_ImUtils::setFaultyAttribute(alias);
			errNo = TC_CPALIASINUSE;
			return false;
		}

		std::set<const ACS_CS_ImBase *> cpBlades;
		std::set<const ACS_CS_ImBase *>::iterator it;

		model.getObjects(cpBlades, CLUSTERCP_T);

		for (it = cpBlades.begin(); it != cpBlades.end(); it++) {
			const ACS_CS_ImClusterCp* cpBlade = dynamic_cast<const ACS_CS_ImClusterCp*>(*it);
			if (cpBlade->rdn.compare(this->rdn.c_str()) != 0 && cpBlade->alias.compare(this->alias.c_str()) == 0) {
				ACS_CS_ImUtils::setFaultyAttribute(alias);
				errNo = TC_CPALIASINUSE;
				return false;
			}
		}
	}

	return true;
}

bool ACS_CS_ImClusterCp::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID.c_str()) == 0)
		{
			axeClusterCpId = (char *) modAttr->attrValues[0];
		} else if (strcmp(modAttr->attrName, "systemIdentifier") == 0)
		{
			systemIdentifier = (*(uint16_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "cpAlias") == 0)
		{
			alias = (char *) modAttr->attrValues[0];
		} else if (strcmp(modAttr->attrName, "apzSystem") == 0)
		{
			apzSystem = (char *) modAttr->attrValues[0];
		} else if (strcmp(modAttr->attrName, "cpType") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				cpType = (*(uint16_t *) modAttr->attrValues[0]);
			else
				cpType = 65535;
		} else if (strcmp(modAttr->attrName, "cpState") == 0)
		{
			cpState = (*(int32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "applicationId") == 0)
		{
			applicationId = (*(int32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "apzSubstate") == 0)
		{
			apzSubstate = (*(int32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "aptSubstate") == 0)
		{
			aptSubstate = (*(int32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "stateTransition") == 0)
		{
			stateTransition = (*(int32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "blockingInfo") == 0)
		{
			blockingInfo = (*(uint16_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, "cpCapacity") == 0)
		{
			cpCapacity = (*(uint32_t *) modAttr->attrValues[0]);
		} else if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str()) == 0)
		{
			axeCpBladeRefTo = (char *) modAttr->attrValues[0];
		} else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImClusterCp::clone() const
{
   ACS_CS_ImBase *cp = new ACS_CS_ImClusterCp(*this);
   return cp;
}


bool ACS_CS_ImClusterCp::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImClusterCp & other = dynamic_cast<const ACS_CS_ImClusterCp &>(obj);

		if (axeClusterCpId != other.axeClusterCpId)
			return false;
		if (systemIdentifier != other.systemIdentifier)
			return false;
		if (alias != other.alias)
			return false;
		if (apzSystem != other.apzSystem)
			return false;
		if (cpType != other.cpType)
			return false;
		if (cpState != other.cpState)
			return false;
		if (applicationId != other.applicationId)
			return false;
		if (apzSubstate != other.apzSubstate)
			return false;
		if (aptSubstate != other.aptSubstate)
			return false;
		if (stateTransition != other.stateTransition)
			return false;
		if (blockingInfo != other.blockingInfo)
			return false;
		if (cpCapacity != other.cpCapacity)
			return false;
		if (axeCpBladeRefTo != other.axeCpBladeRefTo)
			return false;

	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImClusterCp::copyObj(const ACS_CS_ImClusterCp *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	axeClusterCpId = other->axeClusterCpId;
	systemIdentifier = other->systemIdentifier;
	alias = other->alias;
	apzSystem = other->apzSystem;
	cpType = other->cpType;
	cpState = other->cpState;
	applicationId = other->applicationId;
	apzSubstate = other->apzSubstate;
	aptSubstate = other->aptSubstate;
	stateTransition = other->stateTransition;
	blockingInfo = other->blockingInfo;
	cpCapacity = other->cpCapacity;
	axeCpBladeRefTo = other->axeCpBladeRefTo;
}
ACS_CS_ImImmObject * ACS_CS_ImClusterCp::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CLUSTER_CP.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID.c_str(), axeClusterCpId));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("systemIdentifier", systemIdentifier));
   attrList.push_back(ACS_CS_ImUtils::createStringType("cpAlias", alias));
   attrList.push_back(ACS_CS_ImUtils::createStringType("apzSystem", apzSystem));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("cpType", cpType));
   attrList.push_back(ACS_CS_ImUtils::createIntType("cpState", cpState));
   attrList.push_back(ACS_CS_ImUtils::createIntType("applicationId", applicationId));
   attrList.push_back(ACS_CS_ImUtils::createIntType("apzSubstate", apzSubstate));
   attrList.push_back(ACS_CS_ImUtils::createIntType("aptSubstate", aptSubstate));
   attrList.push_back(ACS_CS_ImUtils::createIntType("stateTransition", stateTransition));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("blockingInfo", blockingInfo));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("cpCapacity", cpCapacity));
   attrList.push_back(ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str(), axeCpBladeRefTo));

   immObject->setAttributeList(attrList);

   return immObject;
}



namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImClusterCp & cp, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(cp);
         ar & cp.axeClusterCpId;
         ar & cp.systemIdentifier;
         ar & cp.alias;
         ar & cp.apzSystem;
         ar & cp.cpType;
         ar & cp.cpState;
         ar & cp.applicationId;
         ar & cp.apzSubstate;
         ar & cp.aptSubstate;
         ar & cp.stateTransition;
         ar & cp.blockingInfo;
         ar & cp.cpCapacity;
         ar & cp.axeCpBladeRefTo;


      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImClusterCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImClusterCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImClusterCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImClusterCp & base, const unsigned int /*version*/);
   }
}
