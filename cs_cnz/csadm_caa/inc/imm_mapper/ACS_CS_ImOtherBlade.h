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
 * @file ACS_CS_ImOtherBlade.h
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

#ifndef ACS_CS_IMOTHERBLADE_H_
#define ACS_CS_IMOTHERBLADE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImImmObject.h"

#include "ACS_CC_Types.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CS_ImBase.h"



class ACS_CS_ImOtherBlade : public ACS_CS_ImBlade
{
public:
   ACS_CS_ImOtherBlade();
   ACS_CS_ImOtherBlade(const ACS_CS_ImOtherBlade &other);
   virtual ~ACS_CS_ImOtherBlade();
   ACS_CS_ImOtherBlade &operator=(const ACS_CS_ImOtherBlade &rhs);

   string axeOtherBladeId;
   string currentLoadModuleVersion;
   string bladeProductNumber;
   uint16_t aptEthInPort;
   int swVerType; //IMPORTANT NOTE: This attribute is not part of Equipment IMM model. It is fetched from BladeSwM model, so it shall not be
   	   	   	   	  // converted in IMM attribute when storing the object on IMM!!!
				  //This attribute is only supported for CMX

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:
   void copyObj(const ACS_CS_ImOtherBlade &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOtherBlade & otherBlade, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImOtherBlade, "ACS_CS_ImOtherBlade");


#endif /* ACS_CS_IMOTHERBLADE_H_ */
