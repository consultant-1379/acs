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
 * @file ACS_CS_ImHardwareMgmt.h
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

#ifndef ACS_CS_ImHardwareMgmt_H_
#define ACS_CS_ImHardwareMgmt_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CC_Types.h"


using namespace std;

class ACS_CS_ImHardwareMgmt : public ACS_CS_ImBase
{
public:

   ACS_CS_ImHardwareMgmt();
   ACS_CS_ImHardwareMgmt(const ACS_CS_ImHardwareMgmt &other);
   virtual ~ACS_CS_ImHardwareMgmt();
   ACS_CS_ImHardwareMgmt &operator=(const ACS_CS_ImHardwareMgmt &rhs);

   string axeHardwareMgmtId;
   set<string> staticIpMap;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   void copyObj(const ACS_CS_ImHardwareMgmt &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImHardwareMgmt & hdwm, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImHardwareMgmt, "ACS_CS_ImHardwareMgmt");


#endif /* ACS_CS_ImHardwareMgmt_H_ */
