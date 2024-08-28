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
 * @file ACS_CS_ImCpBlade.cpp
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

#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImValuesDefinitionType.h"
#include "ACS_CS_Registry.h"
#include <iostream>
#include <cstring>
#include <sstream>


#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCpBlade);

ACS_CS_ImCpBlade::ACS_CS_ImCpBlade()
{
   type = CPBLADE_T;
}

ACS_CS_ImCpBlade::ACS_CS_ImCpBlade(const ACS_CS_ImCpBlade &other)
   : ACS_CS_ImBlade(other)
{
   *this = other;
}

ACS_CS_ImCpBlade::~ACS_CS_ImCpBlade()
{}

bool ACS_CS_ImCpBlade::isValid(int & errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;


	// Systemtype
	if(systemType != SINGLE_SIDED_CP && systemType != DOUBLE_SIDED_CP)
	{
		ACS_CS_ImUtils::setFaultyCpArchitecture(systemType);
		errNo = TC_CPSYSTYPEFAULTY;
		return false;
	}

	// Valid FBN
	switch(functionalBoardName)
	{
	case CPUB:   break; //Valid CpBlade FBN
	case MAUB:   break; //Valid CpBlade FBN
	case RPBI_S: break; //Valid CpBlade FBN
	default:
		errNo = TC_INVALID_FBN;
		return false;
	}

	int apzType =  ACS_CS_Registry::getAPZType();
	//not possible insert cp and mau boards for an apz 212 3x. 212 40 and 212 50 source system
	if ((functionalBoardName == CPUB) || (functionalBoardName == MAUB))
	{
		if( apzType < 4 && slotNumber < 25) //Check that it is not a system created CP blade
		{
			ACS_CS_ImUtils::setFaultyFbn(functionalBoardName);
			errNo = TC_FORBIDDENFBN;
			return false;
		}
	}

	// Valid slot
//	if(slotNumber > 25)
//	{
//		errNo = TC_SLOTOUTOFRANGE;
//		return false;
//	}
	if(systemType == SINGLE_SIDED_CP && (functionalBoardName == MAUB || functionalBoardName == RPBI_S ))
	{
		ACS_CS_ImUtils::setFaultyCpArchitecture(systemType);
		errNo = TC_CPSYSTYPEFAULTY;
		return false;
	}
	// Side should be defined for CPUBs on DOUBLE_SIDED_CP
	if(functionalBoardName == CPUB && systemType == DOUBLE_SIDED_CP && side == UNDEF_SIDE)
	{
		errNo = TC_SIDENOTSPECIFIED;
		return false;
	}

	// No CPUB or MAUB allowed with same sysNo and side
	if (systemType == DOUBLE_SIDED_CP && (functionalBoardName == CPUB || functionalBoardName == MAUB))
	{
		std::set<const ACS_CS_ImBase *> cpBlades;
		std::set<const ACS_CS_ImBase *>::iterator it;

		model.getObjects(cpBlades, CPBLADE_T);

		for (it = cpBlades.begin(); it != cpBlades.end(); it++)
		{
			const ACS_CS_ImCpBlade* cpBlade = dynamic_cast<const ACS_CS_ImCpBlade*>(*it);

			if (cpBlade && cpBlade->systemType == this->systemType && cpBlade->functionalBoardName == this->functionalBoardName && cpBlade->rdn != this->rdn)
			{
				if (cpBlade->systemNumber == this->systemNumber && cpBlade->side == this->side)
				{
					errNo = (functionalBoardName == CPUB)? TC_CPUBALREADYPRESENT: TC_MAUBALREADYPRESENT;
					return false;
				}
			}
		}
	}
		// Reject if MAUType = MAUS even if no other dualCPBlades are not present
	 if (systemType == DOUBLE_SIDED_CP &&  (functionalBoardName == MAUB || functionalBoardName == RPBI_S ))
	 {
		std::set<const ACS_CS_ImBase *> dualcpBlades;
		std::set<const ACS_CS_ImBase *>::iterator iter;

		model.getObjects(dualcpBlades, DUALSIDEDCP_T);
		if(dualcpBlades.size()==0)
		{
			if(ACS_CS_Registry::getDefaultMauType()== ACS_CS_ImmMapper::MAUS)
			{
				errNo = TC_MAUSFAULTY;
				return false;	
			}
		}
		for (iter = dualcpBlades.begin(); iter != dualcpBlades.end(); iter++) {
			const ACS_CS_ImDualSidedCp* dualcpBlade = dynamic_cast<const ACS_CS_ImDualSidedCp*>(*iter);
			if((dualcpBlade !=0) && (dualcpBlade->systemIdentifier == (this->systemNumber + DOUBLE_SIDED_CP)))
			{
				if(dualcpBlade->mauType == ACS_CS_ImmMapper::MAUS) 
				{
					errNo = TC_MAUSFAULTY;
					return false;	
				}
				else
				{
					break;
				}
			}			
		}
	 }

	// For SINGLE_CP_SYSTEM sequenceNumber must be set 0-63
	if(systemType == SINGLE_SIDED_CP)
	{
		if(sequenceNumber < 0 || sequenceNumber > 63)
		{
			ACS_CS_ImUtils::setFaultyAttribute(sequenceNumber);
			errNo = TC_CPSEQNOFAULTY;
			return false;
		}
	}

	// For DOUBLE_SIDED_CP systemNumber must be set 1 or 2
	if(systemType == DOUBLE_SIDED_CP && functionalBoardName != RPBI_S)
	{
		if(systemNumber != 1 && systemNumber != 2)
		{
			ACS_CS_ImUtils::setFaultyAttribute(systemNumber);
			errNo = TC_SYSTEMNOTOBIG;
			return false;
		}
	}

	// Only GESB boards can be put in a Shelf with address plugs set to 255
	size_t found;
	found = rdn.find("255");
	if (found != string::npos ){
		ACS_CS_ImUtils::setFaultyFbn(functionalBoardName);
		errNo = TC_INCORRECTMAG;
		return false;
	}

	//Validate IP Alias and Netmask
	struct in_addr convaddr;


	if (!ipAliasEthA.empty() && inet_pton(AF_INET, ipAliasEthA.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(ipAliasEthA);
		errNo = TC_IPALIASETHAFAULTY;
		return false;
	}

	if (!ipAliasEthB.empty() && inet_pton(AF_INET, ipAliasEthB.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(ipAliasEthB);
		errNo = TC_IPALIASETHBFAULTY;
		return false;
	}

	if (!aliasNetmaskEthA.empty() && inet_pton(AF_INET, aliasNetmaskEthA.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(aliasNetmaskEthA);
		errNo = TC_ALIASNETMASKETHAFAULTY;
		return false;
	}

	if (!aliasNetmaskEthB.empty() && inet_pton(AF_INET, aliasNetmaskEthB.c_str(), &convaddr) != 1)
	{
		ACS_CS_ImUtils::setFaultyAttribute(aliasNetmaskEthB);
		errNo = TC_ALIASNETMASKETHBFAULTY;
		return false;
	}

	return ACS_CS_ImBlade::isValid(errNo, model);
}

bool ACS_CS_ImCpBlade::modify(ACS_APGCC_AttrModification **attrMods)
{
   bool baseModded = ACS_CS_ImBlade::modify(attrMods);
   bool selfModded = true;

   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_BLADE_ID.c_str()) == 0)
      {
         axeCpBladeId = (char *) modAttr->attrValues[0];
      }
      else              // Bad attributeName
         selfModded =  false;
   }

   return (baseModded || selfModded);
}

ACS_CS_ImBase * ACS_CS_ImCpBlade::clone() const
{
   ACS_CS_ImBase *cpBlade = new ACS_CS_ImCpBlade(*this);
   return cpBlade;
}


ACS_CS_ImCpBlade &ACS_CS_ImCpBlade::operator=(const ACS_CS_ImCpBlade &rhs)
{
	ACS_CS_ImBlade::operator=(rhs);
   copyObj(rhs);
   return *this;
}



ACS_CS_ImCpBlade &ACS_CS_ImCpBlade::copyObj(const ACS_CS_ImCpBlade &object)
{
   ACS_CS_ImBlade::copyObj(object);
   axeCpBladeId = object.axeCpBladeId;
   cpHwVersion = object.cpHwVersion;
   return *this;
}


bool ACS_CS_ImCpBlade::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBlade::equals(obj))
       return false;

   try {
      const ACS_CS_ImCpBlade & other = dynamic_cast<const ACS_CS_ImCpBlade &>(obj);

      if (axeCpBladeId != other.axeCpBladeId)
         return false;

      if (cpHwVersion != other.cpHwVersion)
    	  return false;

   }
   catch (const std::bad_cast &e)
   {
      std::cerr << e.what() << std::endl;
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCpBlade::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CP_BLADE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList = ACS_CS_ImBlade::getCpBladeAttributes();

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CP_BLADE_ID.c_str(), axeCpBladeId));

   immObject->setAttributeList(attrList);

   return immObject;

}



namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpBlade & cpBlade, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBlade>(cpBlade);
         ar & cpBlade.axeCpBladeId;
         ar & cpBlade.cpHwVersion;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCpBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCpBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCpBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCpBlade & base, const unsigned int /*version*/);
   }
}
