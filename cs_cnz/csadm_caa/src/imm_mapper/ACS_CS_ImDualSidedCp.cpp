/*
 * ACS_CS_ImDualSidedCp.cpp
 *
 *  Created on: Apr 20, 2012
 *      Author: estevol
 */
#include "ACS_CS_ImDualSidedCp.h"
#include "ACS_CS_ImModel.h"
#include <cstring>
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Registry.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImDualSidedCp);

ACS_CS_ImDualSidedCp::ACS_CS_ImDualSidedCp()
{
   type = DUALSIDEDCP_T;
}

ACS_CS_ImDualSidedCp::ACS_CS_ImDualSidedCp(const ACS_CS_ImDualSidedCp *other)
{
	copyObj(other);
}

ACS_CS_ImDualSidedCp::~ACS_CS_ImDualSidedCp()
{}


bool ACS_CS_ImDualSidedCp::isValid(int & errNo, const ACS_CS_ImModel & model)
{
	errNo = 0;
	int architecture;
	bool res = ACS_CS_Registry::getNodeArchitecture(architecture);

	string cpName = ACS_CS_ImUtils::getIdValueFromRdn(this->rdn);

	// Validate Cp name
	if( cpName.compare(0, 2, "CP") != 0){
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
		if (!ACS_CS_ImUtils::isAllowedAliasName(alias))
		{
			ACS_CS_ImUtils::setFaultyAttribute(alias);
			errNo = TC_CPNAMEFAULTY;
			return false;
		}

		if (ACS_CS_ImUtils::isReservedCPName(alias))
		{
			ACS_CS_ImUtils::setFaultyAttribute(alias);
			errNo = TC_CPALIASINUSE;
			return false;
		}

		std::set<const ACS_CS_ImBase *> cpBlades;
		std::set<const ACS_CS_ImBase *>::iterator it;

		model.getObjects(cpBlades, DUALSIDEDCP_T);

		for (it = cpBlades.begin(); it != cpBlades.end(); it++) {
			const ACS_CS_ImDualSidedCp* cpBlade = dynamic_cast<const ACS_CS_ImDualSidedCp*>(*it);
			if (cpBlade->rdn.compare(this->rdn.c_str()) != 0 && cpBlade->alias.compare(this->alias.c_str()) == 0) {
				ACS_CS_ImUtils::setFaultyAttribute(alias);
				errNo = TC_CPALIASINUSE;
				return false;
			}
		}
	}	
	if(this->mauType==ACS_CS_ImmMapper::UNDEFINED)
	{
		errNo = TC_INVALIDMAUTYPE;		
		return  false;
	}
	else if (this->mauType==ACS_CS_ImmMapper::MAUB)
	{
		if (architecture == NODE_VIRTUALIZED)
		{
			errNo = TC_INVALIDMAUTYPE;
			return  false;
		}
	}
	else if(this->mauType==ACS_CS_ImmMapper::MAUS)
	{
		std::set<const ACS_CS_ImBase *> cpBladeObj;
		std::set<const ACS_CS_ImBase *>::iterator cpBladeIt;
		model.getObjects(cpBladeObj, CPBLADE_T);

		for (cpBladeIt = cpBladeObj.begin(); cpBladeIt != cpBladeObj.end(); cpBladeIt++)
		{
			const ACS_CS_ImCpBlade* cpBlade = dynamic_cast<const ACS_CS_ImCpBlade*>(*cpBladeIt);

			if (cpBlade)
			{
				if(res && (architecture != NODE_VIRTUALIZED))
				{
					if (cpBlade->systemNumber == (this->systemIdentifier-DOUBLE_SIDED_CP) && (cpBlade->functionalBoardName == MAUB ||cpBlade->functionalBoardName == RPBI_S))
					{
						errNo = TC_MAUSFAULTY;
						return false ;
					}
				}
			}
		}
	}
	return true;
}

bool ACS_CS_ImDualSidedCp::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID.c_str()) == 0)
		{
			axeDualSidedCpId = (char *) modAttr->attrValues[0];
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
				cpType = (*(uint16_t *)  modAttr->attrValues[0]);
			else
				cpType = 65535;

		} else if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str()) == 0)
		{
			axeCpBladeRefTo.clear();
			for (uint32_t j = 0; j < modAttr->attrValuesNum; j++)
			{
				axeCpBladeRefTo.insert(reinterpret_cast<char *>(modAttr->attrValues[j]));
			}
		}else if (strcmp(modAttr->attrName, "mauType") == 0)
		{
			mauType = (*(int32_t *)  modAttr->attrValues[0]);

		}else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImDualSidedCp::clone() const
{
   ACS_CS_ImBase *cp = new ACS_CS_ImDualSidedCp(*this);
   return cp;
}


bool ACS_CS_ImDualSidedCp::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImDualSidedCp & other = dynamic_cast<const ACS_CS_ImDualSidedCp &>(obj);

		if (axeDualSidedCpId != other.axeDualSidedCpId)
			return false;
		if (systemIdentifier != other.systemIdentifier)
			return false;
		if (alias != other.alias)
			return false;
		if (apzSystem != other.apzSystem)
			return false;
		if (cpType != other.cpType)
			return false;
		if (axeCpBladeRefTo != other.axeCpBladeRefTo)
				return false;
		if (mauType != other.mauType)
                        return false;
	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImDualSidedCp::copyObj(const ACS_CS_ImDualSidedCp *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	axeDualSidedCpId = other->axeDualSidedCpId;
	systemIdentifier = other->systemIdentifier;
	alias = other->alias;
	apzSystem = other->apzSystem;
	cpType = other->cpType;
	mauType = other->mauType;
	axeCpBladeRefTo = other->axeCpBladeRefTo;
	//cpBlades.clear();
}
ACS_CS_ImImmObject * ACS_CS_ImDualSidedCp::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID.c_str(), axeDualSidedCpId));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("systemIdentifier", systemIdentifier));
   attrList.push_back(ACS_CS_ImUtils::createStringType("cpAlias", alias));
   attrList.push_back(ACS_CS_ImUtils::createStringType("apzSystem", apzSystem));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("cpType", cpType));
   attrList.push_back(ACS_CS_ImUtils::createIntType("mauType",mauType));
   attrList.push_back(ACS_CS_ImUtils::createNameType(ACS_CS_ImmMapper::ATTR_CP_BLADE_REF_TO.c_str(), axeCpBladeRefTo));
   immObject->setAttributeList(attrList);
   return immObject;
}



namespace boost {
namespace serialization {
template<class Archive>
void serialize(Archive & ar, ACS_CS_ImDualSidedCp & cp, const unsigned int /*version*/)
{
	ar & boost::serialization::base_object<ACS_CS_ImBase>(cp);
	ar & cp.axeDualSidedCpId;
	ar & cp.systemIdentifier;
	ar & cp.alias;
	ar & cp.apzSystem;
	ar & cp.cpType;
	ar & cp.mauType;
	ar & cp.axeCpBladeRefTo;
}
}
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImDualSidedCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImDualSidedCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImDualSidedCp & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImDualSidedCp & base, const unsigned int /*version*/);
   }
}
