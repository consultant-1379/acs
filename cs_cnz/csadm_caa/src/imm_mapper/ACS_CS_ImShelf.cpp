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
 * @file ACS_CS_ImShelf.cpp
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

#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_APGCC_Util.H"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImShelf);

ACS_CS_ImShelf::ACS_CS_ImShelf()
{
   type = SHELF_T;
}


ACS_CS_ImShelf::ACS_CS_ImShelf(const ACS_CS_ImShelf &other)
{
   copyObj(other);
}

ACS_CS_ImShelf::~ACS_CS_ImShelf()
{}


ACS_CS_ImShelf & ACS_CS_ImShelf::operator=(const ACS_CS_ImShelf &rhs)
{
   copyObj(rhs);
   return *this;

}

bool ACS_CS_ImShelf::isValid(int & errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;

	const string dn = rdn;

	set<const ACS_CS_ImBase *> blades;
	model.getChildren(dn, blades);

	set<int> slotsSet;
	set<const ACS_CS_ImBase *>::iterator it;
	pair<set<int>::iterator, bool> result;

	for(it = blades.begin(); it != blades.end(); it++)
	{
		const ACS_CS_ImBlade *board = dynamic_cast<const ACS_CS_ImBlade *>(*it);

		if (!board)
	    {
			errNo = TC_INTERNALERROR;
			return false;
	    }

		result = slotsSet.insert(board->slotNumber);

		if(result.second == false)
		{
			ACS_CS_ImUtils::setFaultyAttribute(board->slotNumber);
			errNo = TC_SLOTOCCUPIED;
			return false;
		}
	}

	int architecture;
	if (ACS_CS_Registry::getNodeArchitecture(architecture))
	{
		in_addr magAddress;
		bool validAddress = false;

		if (inet_aton(address.c_str(), &magAddress) != -1)
		{
			uint32_t thirdPlug = (htonl(magAddress.s_addr) & 0x0000ff00) >> 8;

			validAddress = ( NODE_VIRTUALIZED == architecture )? 1 == thirdPlug : 0 == thirdPlug;
			//Start of TR HV69331
			if (!validAddress && thirdPlug == 99 && ACS_CS_Registry::getAPZType() == 1)
					validAddress = true;
			//End of TR HV69331
		}

		if ( !validAddress )
		{
			ACS_CS_ImUtils::setFaultyAttribute(address);
			errNo = TC_INCORRECTSHELF;
			return false;
		}
	}

	return true;
}

bool ACS_CS_ImShelf::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
	  ACS_APGCC_AttrModification *atrMod = attrMods[i];

	  if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
		 return false;

	  ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

	  if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_SHELF_ID.c_str()) == 0)
	  {
		 axeShelfId = (char *) modAttr->attrValues[0];
		 string addr = ACS_APGCC::after(axeShelfId,"=");
		 address = addr;
	  }
	  else              // Bad attributeName
		 return false;
   }

   return true;
}

void ACS_CS_ImShelf::copyObj(const ACS_CS_ImShelf &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	axeShelfId = object.axeShelfId;
	address = object.address;
}


ACS_CS_ImBase * ACS_CS_ImShelf::clone() const
{
   ACS_CS_ImBase *shelf = new ACS_CS_ImShelf(*this);
   return shelf;
}

bool ACS_CS_ImShelf::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImShelf & other = dynamic_cast<const ACS_CS_ImShelf &>(obj);

      if (axeShelfId != other.axeShelfId)
         return false;

      if (address != other.address)
         return false;

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;
}


ACS_CS_ImImmObject * ACS_CS_ImShelf::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_SHELF.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_SHELF_ID.c_str(), axeShelfId));

   immObject->setAttributeList(attrList);


   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImShelf & shelf, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(shelf);
         ar & shelf.axeShelfId;
         ar & shelf.address;
      }
   }
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImShelf & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImShelf & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImShelf & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImShelf & base, const unsigned int /*version*/);

   }
}
