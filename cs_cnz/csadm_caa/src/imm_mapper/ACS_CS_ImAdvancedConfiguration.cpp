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
 * @file ACS_CS_ImAdvancedConfiguration.cpp
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

#include "ACS_CS_ImAdvancedConfiguration.h"
#include <cstring>
#include "ACS_CC_Types.h"
#include "ACS_CS_ImUtils.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImAdvancedConfiguration);

ACS_CS_ImAdvancedConfiguration::ACS_CS_ImAdvancedConfiguration()
{
   type = ADVANCEDCONFIGURATION_T;
}


ACS_CS_ImAdvancedConfiguration::~ACS_CS_ImAdvancedConfiguration()
{}


bool ACS_CS_ImAdvancedConfiguration::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImAdvancedConfiguration::modify(ACS_APGCC_AttrModification **attrMods)
{

   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, "omProfileSupervisionPhase") == 0)
      {
         omProfileSupervisionPhase = (*(OmProfilePhaseEnum *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "omProfileSupervisionTimeout") == 0)
      {
         omProfileSupervisionTimeout = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "omProfileChangeTrigger") == 0)
      {
         omProfileChangeTrigger = (*(ProfileChangeTriggerEnum *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "phaseSubscriberCountCpNotify") == 0)
      {
         phaseSubscriberCountCpNotify = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "phaseSubscriberCountApNotify") == 0)
      {
    	  phaseSubscriberCountApNotify = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "phaseSubscriberCountCommit") == 0)
      {
    	  phaseSubscriberCountCommit = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "phaseSubscriberCountValidate") == 0)
      {
    	  phaseSubscriberCountValidate = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "startupProfileSupervision") == 0)
      {
         startupProfileSupervision = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "trafficLeaderCp") == 0)
      {
         trafficLeaderCp = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "trafficIsolatedCp") == 0)
      {
         trafficIsolatedCp = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "omProfileRequested") == 0)
      {
         omProfileRequested = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "omProfileCurrent") == 0)
      {
    	  omProfileCurrent = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "omProfilePhase") == 0)
      {
    	  omProfilePhase = (*(OmProfilePhaseEnum *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "neSubscriberCount") == 0)
      {
         neSubscriberCount = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "initiateUpdatePhaseChange") == 0)
      {
         initiateUpdatePhaseChange = (*(unsigned int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "cancelProfileSupervision") == 0)
      {
         cancelProfileSupervision = (*(OmProfilePhaseEnum *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "cancelClusterOpModeSupervision") == 0)
      {
         cancelClusterOpModeSupervision = (*(bool *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "apzProfileRequested") == 0)
      {
         apzProfileRequested = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "apzProfileQueued") == 0)
      {
         apzProfileQueued = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "apzProfileCurrent") == 0)
      {
         apzProfileCurrent = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "aptProfileRequested") == 0)
      {
         aptProfileRequested = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "aptProfileQueued") == 0)
      {
         aptProfileQueued = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "aptProfileCurrent") == 0)
      {
         aptProfileCurrent = (*(int *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "alarmMaster") == 0)
      {
    	  alarmMaster = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "clockMaster") == 0)
      {
    	  clockMaster = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "isTestEnvironment") == 0)
      {
         isTestEnvironment = (*(bool *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_ADVANCED_CONF_ID.c_str()) == 0)
      {
         axeAdvancedConfigurationId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "clusterAligned") == 0)
      {
    	  clusterAligned = (*(int *) modAttr->attrValues[0]);
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}

ACS_CS_ImBase * ACS_CS_ImAdvancedConfiguration::clone() const
{
   ACS_CS_ImBase *advancedConfiguration = new ACS_CS_ImAdvancedConfiguration(*this);
   return advancedConfiguration;
}


bool ACS_CS_ImAdvancedConfiguration::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImAdvancedConfiguration & other = dynamic_cast<const ACS_CS_ImAdvancedConfiguration &>(obj);

      if (rdn != other.rdn)
         return false;
      if (omProfileSupervisionPhase != other.omProfileSupervisionPhase)
         return false;
      if (omProfileSupervisionTimeout != other.omProfileSupervisionTimeout)
          return false;
      if (omProfileChangeTrigger != other.omProfileChangeTrigger)
         return false;
      if (phaseSubscriberCountCpNotify != other.phaseSubscriberCountCpNotify)
    	  return false;
      if (phaseSubscriberCountApNotify != other.phaseSubscriberCountApNotify)
    	  return false;
      if (phaseSubscriberCountCommit != other.phaseSubscriberCountCommit)
    	  return false;
      if (phaseSubscriberCountValidate != other.phaseSubscriberCountValidate)
    	  return false;
      if (startupProfileSupervision != other.startupProfileSupervision)
         return false;
      if (trafficLeaderCp != other.trafficLeaderCp)
         return false;
      if (trafficIsolatedCp != other.trafficIsolatedCp)
         return false;
      if (omProfileRequested != other.omProfileRequested)
         return false;
      if (omProfileCurrent != other.omProfileCurrent)
         return false;
      if (omProfilePhase != other.omProfilePhase)
    	  return false;
      if (neSubscriberCount != other.neSubscriberCount)
         return false;
      if (initiateUpdatePhaseChange != other.initiateUpdatePhaseChange)
         return false;
      if (cancelProfileSupervision != other.cancelProfileSupervision)
         return false;
      if (cancelClusterOpModeSupervision != other.cancelClusterOpModeSupervision)
         return false;
      if (apzProfileRequested != other.apzProfileRequested)
         return false;
      if (apzProfileQueued != other.apzProfileQueued)
         return false;
      if (apzProfileCurrent != other.apzProfileCurrent)
         return false;
      if (aptProfileRequested != other.aptProfileRequested)
         return false;
      if (aptProfileQueued != other.aptProfileQueued)
         return false;
      if (aptProfileCurrent != other.aptProfileCurrent)
         return false;
      if (clockMaster != other.clockMaster)
         return false;
      if (alarmMaster != other.alarmMaster)
         return false;
      if (isTestEnvironment != other.isTestEnvironment)
         return false;
      if (axeAdvancedConfigurationId != other.axeAdvancedConfigurationId)
         return false;
      if (clusterAligned != other.clusterAligned)
    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }

   return true;

}

ACS_CS_ImImmObject * ACS_CS_ImAdvancedConfiguration::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject * immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_ADVANCED_CONF.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   attrList.push_back(ACS_CS_ImUtils::createIntType("omProfileSupervisionPhase", omProfileSupervisionPhase));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("omProfileSupervisionTimeout", omProfileSupervisionTimeout));
   attrList.push_back(ACS_CS_ImUtils::createIntType("omProfileChangeTrigger", omProfileChangeTrigger));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("phaseSubscriberCountCpNotify", phaseSubscriberCountCpNotify));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("phaseSubscriberCountApNotify", phaseSubscriberCountApNotify));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("phaseSubscriberCountCommit", phaseSubscriberCountCommit));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("phaseSubscriberCountValidate", phaseSubscriberCountValidate));
   attrList.push_back(ACS_CS_ImUtils::createIntType("startupProfileSupervision", startupProfileSupervision));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("trafficLeaderCp", trafficLeaderCp));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("trafficIsolatedCp", trafficIsolatedCp));
   attrList.push_back(ACS_CS_ImUtils::createIntType("omProfileRequested", omProfileRequested));
   attrList.push_back(ACS_CS_ImUtils::createIntType("omProfileCurrent", omProfileCurrent));
   attrList.push_back(ACS_CS_ImUtils::createIntType("omProfilePhase", omProfilePhase));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("neSubscriberCount", neSubscriberCount));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("initiateUpdatePhaseChange", initiateUpdatePhaseChange));
   attrList.push_back(ACS_CS_ImUtils::createIntType("cancelProfileSupervision", cancelProfileSupervision));
   attrList.push_back(ACS_CS_ImUtils::createIntType("cancelClusterOpModeSupervision", cancelClusterOpModeSupervision));
   attrList.push_back(ACS_CS_ImUtils::createIntType("apzProfileRequested", apzProfileRequested));
   attrList.push_back(ACS_CS_ImUtils::createIntType("apzProfileQueued", apzProfileQueued));
   attrList.push_back(ACS_CS_ImUtils::createIntType("apzProfileCurrent", apzProfileCurrent));
   attrList.push_back(ACS_CS_ImUtils::createIntType("aptProfileRequested", aptProfileRequested));
   attrList.push_back(ACS_CS_ImUtils::createIntType("aptProfileQueued", aptProfileQueued));
   attrList.push_back(ACS_CS_ImUtils::createIntType("aptProfileCurrent", aptProfileCurrent));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("alarmMaster", alarmMaster));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("clockMaster", clockMaster));
   attrList.push_back(ACS_CS_ImUtils::createIntType("isTestEnvironment", isTestEnvironment));
   attrList.push_back(ACS_CS_ImUtils::createIntType("clusterAligned", clusterAligned));
   if(false == onlyModifiedAttrs)
	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_ADVANCED_CONF_ID.c_str(), axeAdvancedConfigurationId));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImAdvancedConfiguration & ac, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(ac);
         ar & ac.omProfileSupervisionPhase;
         ar & ac.omProfileSupervisionTimeout;
         ar & ac.omProfileChangeTrigger;
         ar & ac.phaseSubscriberCountCpNotify;
         ar & ac.phaseSubscriberCountApNotify;
         ar & ac.phaseSubscriberCountCommit;
         ar & ac.phaseSubscriberCountValidate;
         ar & ac.startupProfileSupervision;
         ar & ac.trafficLeaderCp;
         ar & ac.trafficIsolatedCp;
         ar & ac.omProfileRequested;
         ar & ac.omProfileCurrent;
         ar & ac.omProfilePhase;
         ar & ac.neSubscriberCount;
         ar & ac.initiateUpdatePhaseChange;
         ar & ac.cancelProfileSupervision;
         ar & ac.cancelClusterOpModeSupervision;
         ar & ac.apzProfileRequested;
         ar & ac.apzProfileQueued;
         ar & ac.apzProfileCurrent;
         ar & ac.aptProfileRequested;
         ar & ac.aptProfileQueued;
         ar & ac.aptProfileCurrent;
         ar & ac.alarmMaster;
         ar & ac.clockMaster;
         ar & ac.isTestEnvironment;
         ar & ac.axeAdvancedConfigurationId;
         ar & ac.clusterAligned;
      }
   }
}



// without the explicit instantiations below, the program will
// fail to link for lack of instantiantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImAdvancedConfiguration & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImAdvancedConfiguration & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImAdvancedConfiguration & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImAdvancedConfiguration & base, const unsigned int /*version*/);
   }
}
