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
#ifndef ACS_CS_IMCPCLUSTER_H_
#define ACS_CS_IMCPCLUSTER_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCpCluster : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCpCluster();
	ACS_CS_ImCpCluster(const ACS_CS_ImCpCluster &other);

   virtual ~ACS_CS_ImCpCluster();

   string axeCpClusterId;
   uint16_t alarmMaster;
   uint16_t clockMaster;
//   string omProfile;
//   OmProfilePhaseEnum phase;
   ClusterOpModeEnum clusterOpMode;
   ClusterOpModeTypeEnum clusterOpModeType;
   ApNameEnum frontAp;
   OgClearCode ogClearCode;
//   string aptProfile;
//   string apzProfile;
   set<string> operativeGroup;
   set<string> allBcGroup;
   string reportProgress;
//   string activeCcFile;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCpCluster &operator=(const ACS_CS_ImCpCluster &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCpCluster &copyObj(const ACS_CS_ImCpCluster &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpCluster & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCpCluster, "ACS_CS_ImCpCluster");

#endif /* ACS_CS_IMBLADECLUSTERINFO_H_ */
