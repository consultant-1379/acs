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
 * @file ACS_CS_ImBase.h
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
#ifndef ACS_CS_IMBASE_H_
#define ACS_CS_IMBASE_H_

#include <istream>
#include <ostream>
#include <set>
#include <stdint.h>
#include "ACS_CC_Types.h"
#include "ACS_CS_ImTypes.h"

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImModel;

class ACS_CS_ImBase {

public:

	enum Action { UNDEFINED, CREATE, MODIFY, DELETE };

   ACS_CS_ImBase() :
	  type(UNDEFINED_T),
      action(UNDEFINED)
   {};

   virtual ~ACS_CS_ImBase() {};

   string rdn;
   ClassType type;
   Action action;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model) = 0;
   virtual bool modify(ACS_APGCC_AttrModification **attrMods) = 0;
   virtual ACS_CS_ImBase * clone() const = 0;

   bool operator==(ACS_CS_ImBase const &other) const;
   bool operator!=(ACS_CS_ImBase const &other) const;

   virtual ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false) = 0;

protected:

   virtual bool equals(const ACS_CS_ImBase &other) const;


};

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImBase & base, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImBase, "ACS_CS_ImBase");
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ACS_CS_ImBase)

#endif /* ACS_CS_IMBASE_H_ */
