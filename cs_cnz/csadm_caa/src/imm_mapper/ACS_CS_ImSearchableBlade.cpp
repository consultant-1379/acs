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
 * @file ACS_CS_ImSearchableBlade.cpp
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
 * 2011-11-07  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImSearchableBlade.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImIMMReader.h"



const uint16_t DEFAULT_UINTVALUE = 0;


ACS_CS_ImSearchableBlade::ACS_CS_ImSearchableBlade()
{
   reset();
}


void ACS_CS_ImSearchableBlade::reset()
{

   magazine = "0.0.0.0";
   systemType = UNDEF_SYSTEMTYPE;
   systemNumber = 0;
   slotNumber = 0;
   entryId = 0;
   side = UNDEF_SIDE;
   sequenceNumber = -1;
   macAddressEthA = "0.0.0.0";
   macAddressEthB = "0.0.0.0";
   ipAliasEthA = "0.0.0.0";
   ipAliasEthB = "0.0.0.0";
   ipAddressEthB = "0.0.0.0";
   ipAddressEthA = "0.0.0.0";
   functionalBoardName = UNDEF_FUNCTIONALBOARDNAME;
   dhcpOption = UNDEF_DHCP;
   aliasNetmaskEthA = "0.0.0.0";
   aliasNetmaskEthB = "0.0.0.0";
   swVerType = 2;

   magazineChanged = false;
   systemTypeChanged = false;
   systemNumberChanged = false;
   slotNumberChanged = false;
   entryIdChanged = false;
   sideChanged = false;
   sequenceNumberChanged = false;
   macAddressEthAChanged = false;
   macAddressEthBChanged = false;
   ipAliasEthAChanged = false;
   ipAliasEthBChanged = false;
   ipAddressEthBChanged = false;
   ipAddressEthAChanged = false;
   functionalBoardNameChanged = false;
   dhcpOptionChanged = false;
   aliasNetmaskEthAChanged = false;
   aliasNetmaskEthBChanged = false;
   swVerTypeChanged = false;
   uuidChanged = false;
}


ACS_CS_ImSearchableBlade::ACS_CS_ImSearchableBlade(const ACS_CS_ImSearchableBlade &other)
   : ACS_CS_ImBlade(other)
{
   *this = other;
}


ACS_CS_ImSearchableBlade::~ACS_CS_ImSearchableBlade()
{
}


ACS_CS_ImBase * ACS_CS_ImSearchableBlade::clone() const
{
   ACS_CS_ImBase *searchableBlade = new ACS_CS_ImSearchableBlade(*this);
   return searchableBlade;
}


ACS_CS_ImSearchableBlade & ACS_CS_ImSearchableBlade::operator=(const ACS_CS_ImSearchableBlade &rhs)
{
   ACS_CS_ImBlade::operator=(rhs);
   copyObj(rhs);
   return *this;

}


ACS_CS_ImSearchableBlade & ACS_CS_ImSearchableBlade::copyObj(const ACS_CS_ImSearchableBlade &object)
{
   ACS_CS_ImBlade::copyObj(object);

   magazine = object.magazine;
   magazineChanged = object.magazineChanged;
   systemTypeChanged = object.systemTypeChanged;
   systemNumberChanged = object.systemNumberChanged;
   slotNumberChanged = object.slotNumberChanged;
   sideChanged = object.sideChanged;
   sequenceNumberChanged = object.sequenceNumberChanged;
   macAddressEthAChanged = object.macAddressEthAChanged;
   macAddressEthBChanged = object.macAddressEthBChanged;
   ipAliasEthAChanged = object.ipAliasEthAChanged;
   ipAliasEthBChanged = object.ipAliasEthBChanged;
   ipAddressEthBChanged = object.ipAddressEthBChanged;
   ipAddressEthAChanged = object.ipAddressEthAChanged;
   functionalBoardNameChanged = object.functionalBoardNameChanged;
   dhcpOptionChanged = object.dhcpOptionChanged;
   aliasNetmaskEthAChanged = object.aliasNetmaskEthAChanged;
   aliasNetmaskEthBChanged = object.aliasNetmaskEthBChanged;
   entryIdChanged = object.entryIdChanged;
   swVerTypeChanged = object.swVerTypeChanged;
   swVerType = object.swVerType;
   uuidChanged = object.uuidChanged;
   return *this;
}

bool ACS_CS_ImSearchableBlade::match(const ACS_CS_ImBlade &obj) const
{
   bool stillMatching = true;

   try {

      if (aliasNetmaskEthAChanged && (aliasNetmaskEthA != ((const ACS_CS_ImBlade &) obj).aliasNetmaskEthA))
         stillMatching = false;
      if (aliasNetmaskEthBChanged && (aliasNetmaskEthB != ((const ACS_CS_ImBlade &) obj).aliasNetmaskEthB))
         stillMatching = false;
      if (dhcpOptionChanged && (dhcpOption != ((const ACS_CS_ImBlade &) obj).dhcpOption))
         stillMatching = false;
      if (functionalBoardNameChanged && (functionalBoardName != ((const ACS_CS_ImBlade &) obj).functionalBoardName))
         stillMatching = false;
      if (ipAddressEthAChanged && (ipAddressEthA != ((const ACS_CS_ImBlade &) obj).ipAddressEthA))
         stillMatching = false;
      if (ipAddressEthBChanged  && (ipAddressEthB != ((const ACS_CS_ImBlade &) obj).ipAddressEthB))
         stillMatching = false;
      if (ipAliasEthAChanged && (ipAliasEthA != ((const ACS_CS_ImBlade &) obj).ipAliasEthA))
         stillMatching = false;
      if (ipAliasEthBChanged && (ipAliasEthB != ((const ACS_CS_ImBlade &) obj).ipAliasEthB))
         stillMatching = false;
      if (macAddressEthAChanged && (macAddressEthA != ((const ACS_CS_ImBlade &) obj).macAddressEthA))
         stillMatching = false;
      if (macAddressEthBChanged && (macAddressEthB != ((const ACS_CS_ImBlade &) obj).macAddressEthB))
         stillMatching = false;
      if (sequenceNumberChanged  && (sequenceNumber != ((const ACS_CS_ImBlade &) obj).sequenceNumber))
         stillMatching = false;
      if (sideChanged && (side != ((const ACS_CS_ImBlade &) obj).side))
         stillMatching = false;
      if (slotNumberChanged  && (slotNumber != ((const ACS_CS_ImBlade &) obj).slotNumber))
         stillMatching = false;
      if (systemNumberChanged  && (systemNumber != ((const ACS_CS_ImBlade &) obj).systemNumber))
         stillMatching = false;
      if (systemTypeChanged && (systemType != ((const ACS_CS_ImBlade &) obj).systemType))
         stillMatching = false;
      if (entryIdChanged  && (entryId != ((const ACS_CS_ImBlade &) obj).entryId))
         stillMatching = false;
      if (uuidChanged)
      {
	 std::string tmpstr = ((const ACS_CS_ImBlade &) obj).uuid;
         if(tmpstr.compare(uuid) != 0)
           stillMatching = false;
      }
	  
	if(swVerTypeChanged)
	{
		int swVer=2;
		const ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<const ACS_CS_ImOtherBlade *>(&obj);
		stillMatching = false;
		if (otherBlade  && (otherBlade->functionalBoardName == CMXB)){					
			swVer = otherBlade->swVerType;	
			if (swVerType == swVer)
	        	 stillMatching = true;			
		}				         	
	 }
	
      if (magazineChanged)
      {
         string name = ACS_CS_ImUtils::getParentName(&obj);

         ACS_CS_ImIMMReader reader;
         ACS_CS_ImBase* base = NULL;
         base = reader.getObject(name);

         if (base) {
        	 ACS_CS_ImShelf *shelf = dynamic_cast<ACS_CS_ImShelf *>(base);

        	 if (shelf == NULL)
        		 stillMatching = false;
        	 else
        		 if (magazine.compare(shelf->address) != 0)
        			 stillMatching = false;

        	 delete base;
         } else
        	 stillMatching = false;
      }

   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return stillMatching;
}

