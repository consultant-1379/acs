/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2011
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImBlade.h
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

#ifndef ACS_CS_IMBLADE_H_
#define ACS_CS_IMBLADE_H_


#include <boost/serialization/export.hpp>


#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImValuesDefinitionType.h"


class ACS_CS_ImBlade : public ACS_CS_ImBase
{
public:
   ACS_CS_ImBlade();
   ACS_CS_ImBlade(const ACS_CS_ImBlade &other);
   virtual ~ACS_CS_ImBlade();
   ACS_CS_ImBlade &operator=(const ACS_CS_ImBlade &rhs);

   uint16_t					systemNumber;
   SideEnum					side;
   int32_t					sequenceNumber;
   IpDNSAddress				ipAliasEthB;
   IpDNSAddress				ipAliasEthA;
   IpDNSAddress				ipAddressEthB;
   IpDNSAddress				ipAddressEthA;
   FunctionalBoardNameEnum	functionalBoardName;
   IpDNSAddress				aliasNetmaskEthB;
   IpDNSAddress				aliasNetmaskEthA;
   uint16_t					entryId;
   uint16_t					slotNumber;
   uint16_t					hwVersion;
   SystemTypeEnum			systemType;
   string					macAddressEthA;
   string					macAddressEthB;
   DhcpEnum					dhcpOption;
   string					uuid;
 
   // Methods
   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const = 0;


protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;
   std::vector<ACS_CS_ImValuesDefinitionType> getApBladeAttributes();
   std::vector<ACS_CS_ImValuesDefinitionType> getCpBladeAttributes();
   std::vector<ACS_CS_ImValuesDefinitionType> getOtherBladeAttributes();


protected:
   void copyObj(const ACS_CS_ImBlade &object);

};

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImBlade & blade, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImBlade, "ACS_CS_ImBlade");

#endif /* ACS_CS_IMBLADE_H_ */
