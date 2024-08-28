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
 * @file ACS_CS_ImOtherBlade.cpp
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

#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImValuesDefinitionType.h"
#include "ACS_CS_Protocol.h"
#include <syslog.h>
#include "ACS_CS_Trace.h"
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImOtherBlade);
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ImOtherBlade_TRACE);
ACS_CS_ImOtherBlade::ACS_CS_ImOtherBlade()
{
	type = OTHERBLADE_T;
}

ACS_CS_ImOtherBlade::~ACS_CS_ImOtherBlade()
{
}

ACS_CS_ImOtherBlade::ACS_CS_ImOtherBlade(const ACS_CS_ImOtherBlade &other)
: ACS_CS_ImBlade(other)
{
	copyObj(other);
}

ACS_CS_ImOtherBlade &ACS_CS_ImOtherBlade::operator=(const ACS_CS_ImOtherBlade &rhs)
{
	ACS_CS_ImBlade::operator=(rhs);
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImOtherBlade::isValid(int &errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;	
	ACS_CS_FTRACE((ACS_CS_ImOtherBlade_TRACE,LOG_LEVEL_INFO,                     
                        "ACS_CS_ImOtherBlade::isValid(): "
                        "functionalBoardName=%d",functionalBoardName ));
	switch(functionalBoardName)
	{
	case SMXB:
	{
		if (slotNumber != 0 && slotNumber != 25) {
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = TC_SMXBSLOTFAULTY;
			return false;
		}

		if (bladeProductNumber.compare("-") == 0) {
			errNo = TC_SMXBNOPRODNO;
			return false;
		}	
		break;
	}
	case SCXB:
	{
		if (slotNumber != 0 && slotNumber != 25) {
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = TC_SCXBSLOTFAULTY;
			return false;
		}

		if (bladeProductNumber.compare("-") == 0) {
			errNo = TC_SCXBNOPRODNO;
			return false;
		}

		break;
	}
	case EPB1:
	case EVOET:
	{
		if (slotNumber == 0 || slotNumber == 25 || slotNumber == 26 || slotNumber == 28)
		{
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = (functionalBoardName == EPB1 ? TC_EPB1SLOTFAULTY : TC_EVOETSLOTFAULTY);
			return false;
		}
		break;
	}
	case CMXB:
	{
		if (slotNumber != 26 && slotNumber != 28)
		{
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = TC_CMXBSLOTFAULTY;
			return false;
		}


//		const ACS_CS_ImBlade * scx0 = 0;
//		const ACS_CS_ImBlade * scx25 = 0;
//		model.getApzMagazineScx(scx0, scx25);
//		if (!scx0 || !scx25)
//		{
//			errNo = TC_MISSINGSCXINAPZMAGAZINE;
//			return false;
//		}

		break;
	}
	case SCB_RP: break; //Valid OtherBlade FBN
	case GESB:   break; //Valid OtherBlade FBN
	case IPTB:   
	{
		if (slotNumber == 0 || slotNumber == 25 || slotNumber == 26 || slotNumber == 28) {
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = TC_IPTBSLOTFAULTY;
			return false;
		}
	break;
	}

	case IPLB:
	{
		if (slotNumber == 0 || slotNumber == 25 || slotNumber == 26 || slotNumber == 28) {
			ACS_CS_ImUtils::setFaultyAttribute(slotNumber);
			errNo = TC_IPLBSLOTFAULTY;
			return false;
		}

		if(side == UNDEF_SIDE) {
			errNo = TC_SIDENOTSPECIFIED;
			return false;
		}

		// No IPLB allowed with same sysNo and side

			std::set<const ACS_CS_ImBase *> otherBlades;
			std::set<const ACS_CS_ImBase *>::iterator it;

			model.getObjects(otherBlades, OTHERBLADE_T);

			for (it = otherBlades.begin(); it != otherBlades.end(); it++) {
				const ACS_CS_ImOtherBlade* otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade*>(*it);

				if (otherBlade && otherBlade->functionalBoardName == IPLB && otherBlade->rdn != this->rdn) {
					if (otherBlade->systemNumber == this->systemNumber && otherBlade->side == this->side) {
						errNo = TC_IPLBALREADYPRESENT;
						return false;
					}
				}
			}



		break;
	}

	default:
		errNo = TC_INVALID_FBN;
		return false;
	}


	return ACS_CS_ImBlade::isValid(errNo, model);
}


bool ACS_CS_ImOtherBlade::modify(ACS_APGCC_AttrModification **attrMods)
{
	bool baseModded = ACS_CS_ImBlade::modify(attrMods);
	bool selfModded = true;

	for (int i = 0; attrMods[i] != NULL; i++ )
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID.c_str()) == 0)
		{
			axeOtherBladeId = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "aptEthInPort") == 0)
		{
			aptEthInPort = (*(uint16_t *) modAttr->attrValues[0]);
		}
		else if (strcmp(modAttr->attrName, "currentLoadModuleVersion") == 0)
		{
			currentLoadModuleVersion = (char *) modAttr->attrValues[0];
			//Getting swVerType from CCH IMM model
			ACS_CS_ImUtils::getImmAttributeInt(currentLoadModuleVersion, "swVerType", swVerType);
		}
		else if (strcmp(modAttr->attrName, "bladeProductNumber") == 0)
		{
			bladeProductNumber = (char *) modAttr->attrValues[0];
		}
		else              // Bad attributeName
			selfModded = false;

	}

	return (baseModded || selfModded);
}

ACS_CS_ImBase * ACS_CS_ImOtherBlade::clone() const
{
	ACS_CS_ImBase *otherBlade = new ACS_CS_ImOtherBlade(*this);
	return otherBlade;
}

void ACS_CS_ImOtherBlade::copyObj(const ACS_CS_ImOtherBlade &object)
{
	ACS_CS_ImBlade::copyObj(object);
	axeOtherBladeId = object.axeOtherBladeId;
	currentLoadModuleVersion = object.currentLoadModuleVersion;
	bladeProductNumber = object.bladeProductNumber;
	aptEthInPort=object.aptEthInPort;
	swVerType=object.swVerType;
}

bool ACS_CS_ImOtherBlade::equals(const ACS_CS_ImBase &obj) const
{

	try {
		const ACS_CS_ImOtherBlade & other = dynamic_cast<const ACS_CS_ImOtherBlade &>(obj);

		if (!ACS_CS_ImBlade::equals(obj))
			return false;

		if (axeOtherBladeId != other.axeOtherBladeId)
			return false;

		if (bladeProductNumber != other.bladeProductNumber)
			return false;

		if (currentLoadModuleVersion != other.currentLoadModuleVersion )
			return false;

		if (aptEthInPort != other.aptEthInPort )
			return false;

	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;

}



ACS_CS_ImImmObject * ACS_CS_ImOtherBlade::toImmObject(bool onlyModifiedAttrs)
{

	//IMPORTANT NOTE: swVerType is not part of Equipment IMM model. It is fetched from BladeSwM model, so it shall not be
	// converted in IMM attribute when storing the object on IMM!!!

	ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

	immObject->setClassName(ACS_CS_ImmMapper::CLASS_OTHER_BLADE.c_str());
	immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

	std::vector<ACS_CS_ImValuesDefinitionType> attrList = ACS_CS_ImBlade::getOtherBladeAttributes();

	if(!onlyModifiedAttrs)
		attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID.c_str(), axeOtherBladeId));

	if(strlen(currentLoadModuleVersion.c_str()) != 0)
		attrList.push_back(ACS_CS_ImUtils::createNameType("currentLoadModuleVersion", currentLoadModuleVersion));
	if(strlen(bladeProductNumber.c_str()) != 0)
		attrList.push_back(ACS_CS_ImUtils::createStringType("bladeProductNumber", bladeProductNumber));

	int port = aptEthInPort;

	attrList.push_back(ACS_CS_ImUtils::createIntType("aptEthInPort", port));



	immObject->setAttributeList(attrList);

	return immObject;
}



namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOtherBlade & otherBlade, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBlade>(otherBlade);
         ar & otherBlade.axeOtherBladeId;
         ar & otherBlade.currentLoadModuleVersion;
         ar & otherBlade.bladeProductNumber;
         ar & otherBlade.aptEthInPort;
         ar & otherBlade.swVerType;

      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImOtherBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImOtherBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImOtherBlade & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImOtherBlade & base, const unsigned int /*version*/);
   }
}
