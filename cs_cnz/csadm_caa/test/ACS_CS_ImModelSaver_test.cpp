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
 * @file ACS_CS_ImModelSaver_test.cpp
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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImAp.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImModelSaver.h"
#include "acs_apgcc_omhandler.h"
#include <set>


TEST(ModelSaver_save, ShelfSavesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   model->addObject(shelf);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ShelfUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   shelf->address="9.8.7.6";
   shelf->action = ACS_CS_ImBase::MODIFY;
   model->addObject(shelf);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ShelfHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("address") == 0)
      {
         ASSERT_STREQ("9.8.7.6", reinterpret_cast<char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, AdvancedConfigurationSavesSuccessfully)
{
   ACS_CS_ImAdvancedConfiguration *ac = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(ac);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, AdvancedConfigurationUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImAdvancedConfiguration *ac = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();
   ac->apzProfileCurrent = 254;
   ac->action = ACS_CS_ImBase::MODIFY;

   model->addObject(ac);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, AdvancedConfigurationHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "advancedConfigurationId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("apzProfileCurrent") == 0)
      {
         EXPECT_EQ(254, *(reinterpret_cast<int *>(attr.attrValues[0])));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ApSavesSuccessfully)
{
   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(ap);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ApUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();
   ap->name = AP2;
   ap->action = ACS_CS_ImBase::MODIFY;

   model->addObject(ap);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ApHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "apId=1,apCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("name") == 0)
      {
         EXPECT_EQ(AP2, *(reinterpret_cast<int *>(attr.attrValues[0])));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ApCategorySavesSuccessfully)
{
   ACS_CS_ImApCategory *apCategory = ACS_CS_ImDefaultObjectCreator::createApCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(apCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ApBladeSavesSuccessfully)
{
   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(apBlade);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ApBladeUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();
   apBlade->macAddressEthA = "1.2.3.4";
   apBlade->action = ACS_CS_ImBase::MODIFY;

   model->addObject(apBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ApBladeHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "apBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("apzProfileCurrent") == 0)
      {
         EXPECT_STREQ("1234", reinterpret_cast<const char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ApServiceCategorySavesSuccessfully)
{
   ACS_CS_ImApServiceCategory *apServiceCategory = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(apServiceCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ApServiceSavesSuccessfully)
{
   ACS_CS_ImApService *apService = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(apService);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ApServiceDeletesSuccessfully)
{
   ACS_CS_ImApService *apService = ACS_CS_ImDefaultObjectCreator::createApServiceObject();
   apService->action = ACS_CS_ImBase::DELETE;
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(apService);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, BladeClusterInfoSavesSuccessfully)
{
   ACS_CS_ImBladeClusterInfo *bladeClusterInfo = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   bool addObject = model->addObject(bladeClusterInfo);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(addObject);
   EXPECT_EQ(ACS_CC_SUCCESS, result);
}

TEST(ModelSaver_save, BladeClusterInfoUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImBladeClusterInfo *bladeClusterInfo = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();
   bladeClusterInfo->alarmMaster = 1;
   bladeClusterInfo->action = ACS_CS_ImBase::MODIFY;

   model->addObject(bladeClusterInfo);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, BladeClusterInfoHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "bladeClusterInfoId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("alarmMaster") == 0)
      {
         EXPECT_EQ(1, (*(reinterpret_cast<uint16_t *>(attr.attrValues[0]))));
      }
      if (attr.attrName.compare("clockMaster") == 0)
      {
         EXPECT_EQ(2, (*(reinterpret_cast<uint16_t *>(attr.attrValues[0]))));
      }


   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ConfigurationInfoSavesSuccessfully)
{
   ACS_CS_ImConfigurationInfo *configurationInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(configurationInfo);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, ConfigurationInfoUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImConfigurationInfo *configurationInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();
   configurationInfo->isBladeClusterSystem = true;
   configurationInfo->action = ACS_CS_ImBase::MODIFY;

   model->addObject(configurationInfo);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ConfigurationInfoHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("isBladeClusterSystem") == 0)
      {
         EXPECT_EQ(true, *(reinterpret_cast<bool *>(attr.attrValues[0])));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save, CpSavesSuccessfully)
{
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(cp);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   cp->alias = "alias";
   cp->action = ACS_CS_ImBase::MODIFY;

   model->addObject(cp);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, CpHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "cpId=1,cpCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("alias") == 0)
      {
         EXPECT_STREQ("alias", reinterpret_cast<const char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, CpBladeSavesSuccessfully)
{
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(cpBlade);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpBladeUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   cpBlade->slotNumber = 23;
   cpBlade->action = ACS_CS_ImBase::MODIFY;

   model->addObject(cpBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, CpBladeHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "cpBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("slotNumber") == 0)
      {
         EXPECT_EQ(234, *(reinterpret_cast<int *>(attr.attrValues[0])));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save, CpCategorySavesSuccessfully)
{
   ACS_CS_ImCpCategory *cpCategory = ACS_CS_ImDefaultObjectCreator::createCpCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(cpCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpGroupSavesSuccessfully)
{
   ACS_CS_ImCpGroup *cpGroup = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(cpGroup);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpGroupUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImCpGroup *cpGroup = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();
   cpGroup->groupName = "groupName";
   cpGroup->action = ACS_CS_ImBase::MODIFY;

   model->addObject(cpGroup);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, CpGroupHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "cpGroupId=1,cpGroupCategoryId=1,bladeClusterInfoId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("apzProfileCurrent") == 0)
      {
         EXPECT_STREQ("groupName", reinterpret_cast<const char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, CpGroupCategorySavesSuccessfully)
{
   ACS_CS_ImCpGroupCategory *cpGroupCategory = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(cpGroupCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, HardwareConfigurationCategorySavesSuccessfully)
{
   ACS_CS_ImHardwareConfigurationCategory *hardwareConfigurationCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(hardwareConfigurationCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, OtherBladeSavesSuccessfully)
{
   ACS_CS_ImOtherBlade *otherBlade = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(otherBlade);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, OtherBladeUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImOtherBlade *otherBlade = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();
   otherBlade->macAddressEthA = "9.8.7.6";
   otherBlade->action = ACS_CS_ImBase::MODIFY;

   model->addObject(otherBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, OtherBladeHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "otherBladeId=1,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("macAddressEthA") == 0)
      {
         EXPECT_STREQ("9.8.7.6", reinterpret_cast<const char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save, VlanSavesSuccessfully)
{
   ACS_CS_ImVlan *vlan = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(vlan);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, VlanUpdatesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();

   ACS_CS_ImVlan *vlan = ACS_CS_ImDefaultObjectCreator::createVlanObject();
   vlan->name = "rockers";
   vlan->action = ACS_CS_ImBase::MODIFY;

   model->addObject(vlan);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, VlanHaveCorrectAttributes)
{
   // Arrange
   ACS_CC_ReturnType result;
   OmHandler immHandle;

   result = immHandle.Init();

   if ( result != ACS_CC_SUCCESS ){
      FAIL();
   }

   string rdn = "vlanId=1,vlanCategoryId=1,configurationInfoId=1";
   ACS_APGCC_ImmObject object;
   object.objName = rdn;

   // Act
   result = immHandle.getObject(&object);

   std::vector<ACS_APGCC_ImmAttribute>::iterator it;

   for (it = object.attributes.begin(); it != object.attributes.end(); it++)
   {

      ACS_APGCC_ImmAttribute attr = *it;

      if (attr.attrName.compare("name") == 0)
      {
         EXPECT_STREQ("rockers", reinterpret_cast<const char *>(attr.attrValues[0]));
      }
   }

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, VlanCategorySavesSuccessfully)
{
   ACS_CS_ImVlanCategory *vlanCategory = ACS_CS_ImDefaultObjectCreator::createVlanCategoryObject();
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   model->addObject(vlanCategory);

   ACS_CS_ImModelSaver saver(model);

   ACS_CC_ReturnType result = saver.save();

   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, AdvancedConfigurationDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImAdvancedConfiguration *ac = ACS_CS_ImDefaultObjectCreator::createAdvancedConfigurationObject();

   ac->action = ACS_CS_ImBase::DELETE;
   model->addObject(ac);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ApDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();

   ap->action = ACS_CS_ImBase::DELETE;
   model->addObject(ap);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save, ApCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImApCategory *apCategory = ACS_CS_ImDefaultObjectCreator::createApCategoryObject();

   apCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(apCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ApBladeDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImApBlade *apBlade = ACS_CS_ImDefaultObjectCreator::createApBladeObject();

   apBlade->action = ACS_CS_ImBase::DELETE;
   model->addObject(apBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ApServiceCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImApServiceCategory *apServiceCategory = ACS_CS_ImDefaultObjectCreator::createApServiceCategoryObject();

   apServiceCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(apServiceCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, BladeClusterInfoDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImBladeClusterInfo *ac = ACS_CS_ImDefaultObjectCreator::createBladeClusterInfoObject();

   ac->action = ACS_CS_ImBase::DELETE;
   model->addObject(ac);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ConfigurationInfoDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImConfigurationInfo *configurationInfo = ACS_CS_ImDefaultObjectCreator::createConfigurationInfoObject();

   configurationInfo->action = ACS_CS_ImBase::DELETE;
   model->addObject(configurationInfo);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();

   cp->action = ACS_CS_ImBase::DELETE;
   model->addObject(cp);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpBladeDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();

   cpBlade->action = ACS_CS_ImBase::DELETE;
   model->addObject(cpBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCpCategory *cpCategory = ACS_CS_ImDefaultObjectCreator::createCpCategoryObject();

   cpCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(cpCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, CpGroupDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCpGroup *cpGroup = ACS_CS_ImDefaultObjectCreator::createCpGroupObject();

   cpGroup->action = ACS_CS_ImBase::DELETE;
   model->addObject(cpGroup);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, CpGroupCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImCpGroupCategory *cpGroupCategory = ACS_CS_ImDefaultObjectCreator::createCpGroupCategoryObject();

   cpGroupCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(cpGroupCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, HardwareConfigurationCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImHardwareConfigurationCategory *hardwareConfigurationCategory = ACS_CS_ImDefaultObjectCreator::createHardwareConfigurationCategoryObject();

   hardwareConfigurationCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(hardwareConfigurationCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, OtherBladeDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImOtherBlade *otherBlade = ACS_CS_ImDefaultObjectCreator::createOtherBladeObject();

   otherBlade->action = ACS_CS_ImBase::DELETE;
   model->addObject(otherBlade);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, VlanDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImVlan *vlan = ACS_CS_ImDefaultObjectCreator::createVlanObject();

   vlan->action = ACS_CS_ImBase::DELETE;
   model->addObject(vlan);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

TEST(ModelSaver_save, VlanCategoryDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImVlanCategory *vlanCategory = ACS_CS_ImDefaultObjectCreator::createVlanCategoryObject();

   vlanCategory->action = ACS_CS_ImBase::DELETE;
   model->addObject(vlanCategory);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save, ShelfDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

   shelf->action = ACS_CS_ImBase::DELETE;
   model->addObject(shelf);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save_transaction, ShelfAndShelf2SavesSuccessfullyAsTransaction)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   shelf2->rdn = "shelfId=2,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   shelf2->shelfId = "shelfId=2";
   shelf->action = ACS_CS_ImBase::CREATE;
   shelf2->action = ACS_CS_ImBase::CREATE;
   model->addObject(shelf);
   model->addObject(shelf2);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save_transaction, ShelfAndShelf2DeletesSuccessfullyAsTransaction)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   shelf2->rdn = "shelfId=2,hardwareConfigurationCategoryId=1,configurationInfoId=1";
   shelf2->shelfId = "shelfId=2";
   shelf->action = ACS_CS_ImBase::DELETE;
   shelf2->action = ACS_CS_ImBase::DELETE;
   model->addObject(shelf);
   model->addObject(shelf2);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}




TEST(ModelSaver_save_transaction, ShelfAndFaultyShelf2SavesUnSuccessfullyAsTransaction)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImShelf *shelf2 = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   shelf2->rdn = "shelfId=2,hardwareConfigurationCategoryId=11234nonExsistingParent,configurationInfoId=1";
   shelf2->shelfId = "shelfId=2";
   shelf->action = ACS_CS_ImBase::CREATE;
   shelf2->action = ACS_CS_ImBase::CREATE;
   model->addObject(shelf);
   model->addObject(shelf2);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction");

   // Assert
   EXPECT_TRUE(result == ACS_CC_FAILURE);
}


TEST(ModelSaver_save_transaction, deletingObjectsThatDoesNotExsistsReturnsACS_CC_FAILURE)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   shelf->action = ACS_CS_ImBase::DELETE;
   model->addObject(shelf);
   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save();

   // Assert
   EXPECT_TRUE(result == ACS_CC_FAILURE);
}


TEST(ModelSaver_save_transaction, ShelfAndCpBladeSavedInSameTransactionAggregated)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   shelf->action = ACS_CS_ImBase::CREATE;
   cpBlade->action = ACS_CS_ImBase::CREATE;
   model->addObject(shelf);
   model->addObject(cpBlade);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction_createshelfandcp");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

/*
TEST(ModelSaver_save_transaction, ShelfAndCpBladeModifiedInSameTransactionAggregated)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   shelf->action = ACS_CS_ImBase::MODIFY;
   cpBlade->action = ACS_CS_ImBase::MODIFY;
   shelf->address = "9.8.7.6";
   cpBlade->aliasNetmaskEthB = "aliasNetmaskEthB";
   model->addObject(shelf);
   model->addObject(cpBlade);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction_modifyshelfandcp");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}

*/


TEST(ModelSaver_save_transaction, ShelfAndCpBladeDeletedInSameTransactionAggregated)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImShelf *shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
   ACS_CS_ImCpBlade *cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
   shelf->action = ACS_CS_ImBase::DELETE;
   cpBlade->action = ACS_CS_ImBase::DELETE;
   model->addObject(cpBlade);
   model->addObject(shelf);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction_deleteshelfandcp");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}


TEST(ModelSaver_save_transaction, ApAndCpCreatedSuccessfullyAsTransaction)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   model->addObject(ap);
   model->addObject(cp);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction_createapandcp");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}



TEST(ModelSaver_save_transaction, ApAndCpDeletesSuccessfully)
{
   // Arrange
   ACS_CS_ImModel *model = new ACS_CS_ImModel();
   ACS_CS_ImAp *ap = ACS_CS_ImDefaultObjectCreator::createApObject();
   ACS_CS_ImCp *cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
   ap->action = ACS_CS_ImBase::DELETE;
   cp->action = ACS_CS_ImBase::DELETE;
   model->addObject(ap);
   model->addObject(cp);

   ACS_CS_ImModelSaver saver(model);

   // Act
   ACS_CC_ReturnType result = saver.save("transaction_deleteapandcp");

   // Assert
   EXPECT_TRUE(result == ACS_CC_SUCCESS);
}
