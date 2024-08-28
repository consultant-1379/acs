
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
 * @file ACS_CS_ImApBlade.h
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
#ifndef ACS_CS_IMAPBLADE_H_
#define ACS_CS_IMAPBLADE_H_



#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImAp.h"

#include <boost/serialization/export.hpp>


class ACS_CS_ImApBlade : public ACS_CS_ImBlade
{
public:
   ACS_CS_ImApBlade();
   ACS_CS_ImApBlade(const ACS_CS_ImApBlade &other);
   virtual ~ACS_CS_ImApBlade();

   string axeApBladeId;
   //string apRdn;

   //ACS_CS_ImAp *ap;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImApBlade & operator=(const ACS_CS_ImApBlade &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);


protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   ACS_CS_ImApBlade & copyObj(const ACS_CS_ImApBlade &object);

};

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImApBlade & apBlade, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImApBlade, "ACS_CS_ImApBlade");

#endif /* ACS_CS_IMAPBLADE_H_ */
