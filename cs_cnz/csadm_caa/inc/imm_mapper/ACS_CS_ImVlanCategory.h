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
 * @file ACS_CS_ImVlanCategory.h
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

#ifndef ACS_CS_IMVLANCATEGORY_H_
#define ACS_CS_IMVLANCATEGORY_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImVlan.h"
#include "ACS_CC_Types.h"


using namespace std;

class ACS_CS_ImVlanCategory : public ACS_CS_ImBase
{
public:

   ACS_CS_ImVlanCategory();
   ACS_CS_ImVlanCategory(const ACS_CS_ImVlanCategory &other);
   virtual ~ACS_CS_ImVlanCategory();

   string vlanCategoryId;
   set<ACS_CS_ImVlan *> vlans;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImVlanCategory & operator=(const ACS_CS_ImVlanCategory &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:

   void copyObj(const ACS_CS_ImVlanCategory &object);
};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImVlanCategory & vlanCategory, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImVlanCategory, "ACS_CS_ImVlanCategory");

#endif /* ACS_CS_IMVLANCATEGORY_H_ */
