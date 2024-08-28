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
 * @file ACS_CS_ImClusterCp.h
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
#ifndef ACS_CS_IMCLUSTERCP_H_
#define ACS_CS_IMCLUSTERCP_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImClusterCp : public ACS_CS_ImBase
{
	//------------------------------
	// IMM OPERATIONS
	//------------------------------

public:

   ACS_CS_ImClusterCp();
   ACS_CS_ImClusterCp(const ACS_CS_ImClusterCp *other);
   virtual ~ACS_CS_ImClusterCp();

   string		axeClusterCpId;
   uint16_t		systemIdentifier;
   string		alias;
   string		apzSystem;
   uint16_t 	cpType;
   int32_t 		cpState;
   int32_t 		applicationId;
   int32_t 		apzSubstate;
   int32_t 		aptSubstate;
   int32_t 		stateTransition;
   uint16_t		blockingInfo;
   uint32_t 	cpCapacity;
   string 		axeCpBladeRefTo;

   bool isValid(int &errNo, const ACS_CS_ImModel &model);
   bool modify(ACS_APGCC_AttrModification **attrMods);
   ACS_CS_ImBase * clone() const;
   void copyObj(const ACS_CS_ImClusterCp *other);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImClusterCp & cp, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImClusterCp, "ACS_CS_ImClusterCp");

#endif /* ACS_CS_IMCLUSTERCP_H_ */
