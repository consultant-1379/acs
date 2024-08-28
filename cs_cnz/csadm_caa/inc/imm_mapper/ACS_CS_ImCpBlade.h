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
 * @file ACS_CS_ImCpBlade.h
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

#ifndef ACS_CS_IMCPBLADE_H_
#define ACS_CS_IMCPBLADE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CC_Types.h"
//#include "ACS_CS_ImCp.h"




class ACS_CS_ImCpBlade : public ACS_CS_ImBlade
{
public:
   ACS_CS_ImCpBlade();
   ACS_CS_ImCpBlade(const ACS_CS_ImCpBlade &other);

   virtual ~ACS_CS_ImCpBlade ();

   string axeCpBladeId;
   string cpHwVersion;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCpBlade &operator=(const ACS_CS_ImCpBlade &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCpBlade &copyObj(const ACS_CS_ImCpBlade &object);
};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpBlade & cpBlade, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCpBlade, "ACS_CS_ImCpBlade");


#endif /* ACS_CS_IMCPBLADE_H_ */
