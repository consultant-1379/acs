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
 * @file ACS_CS_ImAdvancedConfiguration.h
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

#ifndef ACS_CS_IMADVANCEDCONFIGURATION_H_
#define ACS_CS_IMADVANCEDCONFIGURATION_H_

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImImmObject.h"

#include <boost/serialization/export.hpp>

using namespace std;

class ACS_CS_ImAdvancedConfiguration : public ACS_CS_ImBase
{
public:

   ACS_CS_ImAdvancedConfiguration();
   virtual ~ACS_CS_ImAdvancedConfiguration();

   OmProfilePhaseEnum omProfileSupervisionPhase;
   unsigned int omProfileSupervisionTimeout;
   ProfileChangeTriggerEnum omProfileChangeTrigger;
   unsigned int phaseSubscriberCountCpNotify;
   unsigned int phaseSubscriberCountApNotify;
   unsigned int phaseSubscriberCountCommit;
   unsigned int phaseSubscriberCountValidate;
   int startupProfileSupervision;
   uint16_t trafficLeaderCp;
   uint16_t trafficIsolatedCp;
   int omProfileRequested;
   int omProfileCurrent;
   OmProfilePhaseEnum omProfilePhase;
   unsigned int neSubscriberCount;
   unsigned int initiateUpdatePhaseChange;
   OmProfilePhaseEnum cancelProfileSupervision;
   bool cancelClusterOpModeSupervision;

   int apzProfileRequested;
   int apzProfileQueued;
   int apzProfileCurrent;

   int aptProfileRequested;
   int aptProfileQueued;
   int aptProfileCurrent;

   uint16_t alarmMaster;
   uint16_t clockMaster;

   bool isTestEnvironment;
   string axeAdvancedConfigurationId;
   int clusterAligned;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &other) const;


};

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImAdvancedConfiguration & ac, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImAdvancedConfiguration, "ACS_CS_ImAdvancedConfiguration");

#endif /* ACS_CS_IMADVANCEDCONFIGURATION_H_ */
