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
 ----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImAp.h
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

#ifndef ACS_CS_IMAP_H_
#define ACS_CS_IMAP_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImImmObject.h"

using namespace std;

class ACS_CS_ImAp : public ACS_CS_ImBase
{
public:

   ACS_CS_ImAp();
   ACS_CS_ImAp(const ACS_CS_ImAp &other);
   virtual ~ACS_CS_ImAp();

   string axeApgId;
   std::set<string> axeApBladeRefTo;

   int getAxeApBladeRefToNumberOfElements();
   void addAxeApBladeRefToElement(std::string apBladeRefTo);
   std::set<string> getAxeApBladeRefToElements();

   uint16_t entryId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;
   void copyObj(const ACS_CS_ImAp &object);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);


protected:
   virtual bool equals(const ACS_CS_ImBase &other) const;

private:
   //std::set<string> axeApBladeRefTo[5];

};

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImAp & ap, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImAp, "ACS_CS_ImAp");

#endif /* ACS_CS_IMAP_H_ */
