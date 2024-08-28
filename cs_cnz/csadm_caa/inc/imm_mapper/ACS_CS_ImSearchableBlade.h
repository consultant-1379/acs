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
 * @file ACS_CS_ImSearchableBlade.h
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

#ifndef ACS_CS_IMSEARCHABLEBLADE_H_
#define ACS_CS_IMSEARCHABLEBLADE_H_

#include "ACS_CS_ImBlade.h"


class ACS_CS_ImSearchableBlade : public ACS_CS_ImBlade
{
public:

   ACS_CS_ImSearchableBlade();
   ACS_CS_ImSearchableBlade(const ACS_CS_ImSearchableBlade &other);
   ~ACS_CS_ImSearchableBlade();


   string magazine;
   bool magazineChanged;

   bool systemTypeChanged;
   bool systemNumberChanged;
   bool slotNumberChanged;
   bool sideChanged;
   bool sequenceNumberChanged;

   bool macAddressEthAChanged;
   bool macAddressEthBChanged;
   bool ipAliasEthAChanged;
   bool ipAliasEthBChanged;
   bool ipAddressEthBChanged;
   bool ipAddressEthAChanged;
   bool functionalBoardNameChanged;
   bool dhcpOptionChanged;
   bool aliasNetmaskEthAChanged;
   bool aliasNetmaskEthBChanged;
   bool entryIdChanged;
   int  swVerType;
   bool swVerTypeChanged;
   bool uuidChanged;

   ACS_CS_ImSearchableBlade & operator=(const ACS_CS_ImSearchableBlade &rhs);
   virtual ACS_CS_ImBase * clone() const;
   bool match(const ACS_CS_ImBlade &obj) const;
   void reset();

private:
   ACS_CS_ImSearchableBlade & copyObj(const ACS_CS_ImSearchableBlade &object);
   virtual ACS_CS_ImImmObject * toImmObject(bool /*onlyModifiedAttrs = false*/) { return 0; }


};




#endif /* ACS_CS_IMSEARCHABLEBLADE_H_ */
