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
 * @file ACS_CS_ImCpCluster.cpp
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

#include "ACS_CS_ImCpCluster.h"
#include "ACS_CS_ImUtils.h"
#include <typeinfo>
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCpCluster);

ACS_CS_ImCpCluster::ACS_CS_ImCpCluster()
{
   type = CPCLUSTER_T;
}

ACS_CS_ImCpCluster::ACS_CS_ImCpCluster(const ACS_CS_ImCpCluster &other)
{
   *this = other;
}

ACS_CS_ImCpCluster::~ACS_CS_ImCpCluster()
{}


bool ACS_CS_ImCpCluster::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCpCluster::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID.c_str()) == 0)
      {
         axeCpClusterId = (char *) modAttr->attrValues[0];
      }
      else if (strcmp(modAttr->attrName, "alarmMaster") == 0)
      {
         alarmMaster = (*(uint16_t *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "clockMaster") == 0)
      {
         clockMaster = (*(uint16_t *) modAttr->attrValues[0]);
      }
//      else if (strcmp(modAttr->attrName, "omProfile") == 0)
//      {
//    	  if (modAttr->attrValuesNum > 0)
//    		  omProfile = (char *) modAttr->attrValues[0];
//    	  else
//    		  omProfile = '\0';
//      }
//      else if (strcmp(modAttr->attrName, "phase") == 0)
//      {
//    	  if (modAttr->attrValuesNum > 0)
//    		  phase = (*(OmProfilePhaseEnum *) modAttr->attrValues[0]);
//    	  else
//    		  phase = UNDEF_OMPROFILEPHASE;
//      }
      else if (strcmp(modAttr->attrName, "clusterOpMode") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  clusterOpMode = (*(ClusterOpModeEnum *) modAttr->attrValues[0]);
    	  else
    		  clusterOpMode = UNDEF_CLUSTEROPMODE;
      }
      else if (strcmp(modAttr->attrName, "clusterOpModeType") == 0)
      {
    	  clusterOpModeType = (*(ClusterOpModeTypeEnum *) modAttr->attrValues[0]);
      }
      else if (strcmp(modAttr->attrName, "frontAp") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  frontAp = (*(ApNameEnum *) modAttr->attrValues[0]);
    	  else
    		  frontAp = UNDEF_APNAME;
      }
      else if (strcmp(modAttr->attrName, "clearOgCode") == 0)
      {
         ogClearCode = (*(OgClearCode *) modAttr->attrValues[0]);
      }
//      else if (strcmp(modAttr->attrName, "aptProfile") == 0)
//      {
//    	  aptProfile = ((char *) modAttr->attrValues[0]);
//      }
//      else if (strcmp(modAttr->attrName, "apzProfile") == 0)
//      {
//    	  apzProfile = ((char *) modAttr->attrValues[0]);
//      }
      else if (strcmp(modAttr->attrName, "operativeGroup") == 0)
      {
    	  operativeGroup.clear();
    	  for (uint32_t j = 0; j < modAttr->attrValuesNum; j++)
    	  {
    		  operativeGroup.insert(reinterpret_cast<char *>(modAttr->attrValues[j]));
    	  }
      }
      else if (strcmp(modAttr->attrName, "allBcGroup") == 0)
      {
    	  allBcGroup.clear();
    	  for (uint32_t j = 0; j < modAttr->attrValuesNum; j++)
    	  {
    		  allBcGroup.insert(reinterpret_cast<char *>(modAttr->attrValues[j]));
    	  }
      }
      else if (strcmp(modAttr->attrName, "reportProgress") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  reportProgress = ((char *) modAttr->attrValues[0]);
    	  else
    		  reportProgress = '\0';
      }
//      else if (strcmp(modAttr->attrName, "activeCcFile") == 0)
//      {
//    	  if (modAttr->attrValuesNum > 0)
//    		  activeCcFile = ((char *) modAttr->attrValues[0]);
//    	  else
//    		  activeCcFile = '\0';
//      }

      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCpCluster::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImCpCluster(*this);
   return bladeClusterInfo;
}


ACS_CS_ImCpCluster & ACS_CS_ImCpCluster::operator=(const ACS_CS_ImCpCluster &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCpCluster & ACS_CS_ImCpCluster::copyObj(const ACS_CS_ImCpCluster &object)
{
	action = 			object.action;
	type = 				object.type;
	rdn = 				object.rdn;
	axeCpClusterId = 	object.axeCpClusterId;
	alarmMaster = 		object.alarmMaster;
	clockMaster = 		object.clockMaster;
//	omProfile = 		object.omProfile;
//	phase = 			object.phase;
	clusterOpMode = 	object.clusterOpMode;
	clusterOpModeType = object.clusterOpModeType;
	frontAp = 			object.frontAp;
	ogClearCode = 		object.ogClearCode;
//	aptProfile = 		object.aptProfile;
//	apzProfile = 		object.apzProfile;
	operativeGroup =	object.operativeGroup;
	allBcGroup = 		object.allBcGroup;
	reportProgress = 	object.reportProgress;
//	activeCcFile =	 	object.activeCcFile;

   return *this;
}


bool ACS_CS_ImCpCluster::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCpCluster & other = dynamic_cast<const ACS_CS_ImCpCluster &>(obj);

      if (axeCpClusterId != other.axeCpClusterId)
    	  return false;
      if (alarmMaster != other.alarmMaster)
    	  return false;
      if (clockMaster != other.clockMaster)
    	  return false;
//      if (omProfile != other.omProfile)
//    	  return false;
//      if (phase != other.phase)
//    	  return false;
      if (clusterOpMode != other.clusterOpMode)
    	  return false;
      if (clusterOpModeType != other.clusterOpModeType)
    	  return false;
      if (frontAp != other.frontAp)
    	  return false;
      if (ogClearCode != other.ogClearCode)
    	  return false;
//      if (aptProfile != other.aptProfile)
//    	  return false;
//      if (apzProfile != other.apzProfile)
//    	  return false;
      if (operativeGroup != other.operativeGroup)
    	  return false;
      if (allBcGroup != other.allBcGroup)
    	  return false;
      if (reportProgress != other.reportProgress)
          return false;
//      if (activeCcFile != other.activeCcFile)
//          return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCpCluster::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CP_CLUSTER.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;
   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID.c_str(), axeCpClusterId));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("alarmMaster", alarmMaster));
   attrList.push_back(ACS_CS_ImUtils::createUIntType("clockMaster", clockMaster));

//   if (strlen(omProfile.c_str()) != 0)
//	   attrList.push_back(ACS_CS_ImUtils::createStringType("omProfile", omProfile));
//   else
//	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("omProfile"));
//
//   if (phase != UNDEF_OMPROFILEPHASE)
//	   attrList.push_back(ACS_CS_ImUtils::createIntType("phase", phase));
//   else
//   	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("phase"));

   if (clusterOpMode != UNDEF_CLUSTEROPMODE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("clusterOpMode", clusterOpMode));
   else
   	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("clusterOpMode"));

   //Hidden enum accept all the possible values
   attrList.push_back(ACS_CS_ImUtils::createIntType("clusterOpModeType", clusterOpModeType));

   if (frontAp != UNDEF_APNAME)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("frontAp", frontAp));
   else
   	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("frontAp"));

   //Hidden enum accept all the possible values
   attrList.push_back(ACS_CS_ImUtils::createIntType("clearOgCode", ogClearCode));

//   if (aptProfile.size() != 0)
//	   attrList.push_back(ACS_CS_ImUtils::createStringType("aptProfile", aptProfile));
//   if (apzProfile.size() != 0)
//	   attrList.push_back(ACS_CS_ImUtils::createStringType("apzProfile", apzProfile));

   if (strlen(reportProgress.c_str()) != 0)
  	   attrList.push_back(ACS_CS_ImUtils::createNameType("reportProgress", reportProgress));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyNameType("reportProgress"));

//   if (strlen(activeCcFile.c_str()) != 0)
//     	   attrList.push_back(ACS_CS_ImUtils::createStringType("activeCcFile", activeCcFile));
//   else
//   	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("activeCcFile"));

   attrList.push_back(ACS_CS_ImUtils::createMultipleStringType("operativeGroup", operativeGroup));
   attrList.push_back(ACS_CS_ImUtils::createMultipleStringType("allBcGroup", allBcGroup));

   immObject->setAttributeList(attrList);


   return immObject;

}


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpCluster & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.axeCpClusterId;
         ar & bladeClusterInfo.alarmMaster;
         ar & bladeClusterInfo.clockMaster;
//         ar & bladeClusterInfo.omProfile;
//         ar & bladeClusterInfo.phase;
         ar & bladeClusterInfo.clusterOpMode;
         ar & bladeClusterInfo.clusterOpModeType;
         ar & bladeClusterInfo.frontAp;
         ar & bladeClusterInfo.ogClearCode;
         ar & bladeClusterInfo.operativeGroup;
         ar & bladeClusterInfo.allBcGroup;
//         ar & bladeClusterInfo.activeCcFile;
         ar & bladeClusterInfo.reportProgress;
//         ar & bladeClusterInfo.aptProfile;
//         ar & bladeClusterInfo.apzProfile;

      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCpCluster & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCpCluster & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCpCluster & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCpCluster & base, const unsigned int /*version*/);
   }
}
