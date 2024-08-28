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
 * @file ACS_CS_ImObjectCreator.h
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
 * 2011-09-01  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#ifndef ACS_CS_IMOBJECTCREATOR_H_
#define ACS_CS_IMOBJECTCREATOR_H_

#include "ACS_CS_ImBase.h"
#include "acs_apgcc_omhandler.h"
#include "ACS_CS_ImBlade.h"


class ACS_CS_ImObjectCreator {
public:

   ACS_CS_ImObjectCreator();
   virtual ~ACS_CS_ImObjectCreator();

   static ACS_CS_ImBase * createImBaseObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createImBaseObject(const char *className, const char* parentName, ACS_APGCC_AttrValues **attr);


private:

   static bool createImmObject(ACS_APGCC_ImmObject &immObject, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr);

   static ACS_CS_ImBase * createVlanObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createVlanCategoryObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createShelfObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createOtherBladeObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createHardwareMgmtObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCpBladeObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createClusterCpObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createDualSidedCpObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createEquipmentObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCpClusterObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createApServiceCategoryObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createApServiceObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createLogicalMgmtObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createApBladeObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createApObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createAdvancedConfigurationObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCpClusterStructObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createOmProfileObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCcFileObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createOmProfileManagerObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCcFileManagerObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCandidateCcFileObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createOmProfileManagerStructObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createCandidateCcFileStructObject(const ACS_APGCC_ImmObject &object);

   static ACS_CS_ImBase* createFunctionDistributionObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createApgNodeObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createFunctionObject(const ACS_APGCC_ImmObject &object);

   static ACS_CS_ImBase* createCpProductInfoObject(const ACS_APGCC_ImmObject &object);

   static ACS_CS_ImBase* createCrMgmtObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createComputeResourceObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createComputeResourceNetworkObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createIplbClusterObject(const ACS_APGCC_ImmObject &object);

   static ACS_CS_ImBase* createCrmEquipmentObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase* createCrmComputeResourceObject(const ACS_APGCC_ImmObject &object);

   static ACS_CS_ImBase * createTrmTransportObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createTrmHostObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createTrmInterfaceObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createTrmLogicalNetworkObject(const ACS_APGCC_ImmObject &object);
   static ACS_CS_ImBase * createTrmInternalNetworkObject(const ACS_APGCC_ImmObject &object);

   static void initBaseClassParameters(ACS_CS_ImBlade *blade, const ACS_APGCC_ImmObject &object);


};

#endif /* ACS_CS_IMOBJECTCREATOR_H_ */
