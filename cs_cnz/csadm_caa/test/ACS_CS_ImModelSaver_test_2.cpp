#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_R1.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_IdList_Implementation.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_ImRepository.h"

class ACS_CS_ImModelSaver_test_2 : public ::testing::Test
{
protected:

   ACS_CS_ImCpBlade *cpBlade;
   ACS_CS_ImCpBlade *cpBlade2;
   ACS_CS_ImCpBlade *cpBlade3;

   ACS_CS_ImShelf *shelf;
   ACS_CS_API_HWC_R1 * hwc;
   ACS_CS_API_BoardSearch_R1 *boardSearch;

   virtual void SetUp()
   {
      shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
      cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade->entryId = 11;
      cpBlade->slotNumber = 11;

      cpBlade2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade2->rdn = "cpBladeId=2,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
      cpBlade2->cpBladeId = "cpBladeId=2";
      cpBlade2->entryId = 12;
      cpBlade2->slotNumber = 12;

      cpBlade3 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade3->rdn = "cpBladeId=3,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
      cpBlade3->cpBladeId = "cpBladeId=3";
      cpBlade3->entryId = 13;
      cpBlade3->slotNumber = 13;

      ACS_CS_ImRepository::instance()->addObject(shelf);
      ACS_CS_ImRepository::instance()->addObject(cpBlade);
      ACS_CS_ImRepository::instance()->addObject(cpBlade2);
      ACS_CS_ImRepository::instance()->addObject(cpBlade3);

      ACS_CC_ReturnType result = ACS_CS_ImRepository::instance()->save();
      if (result == ACS_CC_FAILURE)
         FAIL() << "ACS_CS_ImRepository::instance()->save() failed!";
   }

   virtual void TearDown()
   {
      //cpBlade->action = ACS_CS_ImBase::DELETE;
      //cpBlade2->action = ACS_CS_ImBase::DELETE;
      //cpBlade3->action = ACS_CS_ImBase::DELETE;
      shelf->action = ACS_CS_ImBase::DELETE;

      ACS_CC_ReturnType result = ACS_CS_ImRepository::instance()->save();
      if (result == ACS_CC_FAILURE)
         FAIL() << "ACS_CS_ImRepository::instance()->save() failed!";

      bool applyResult = ACS_CS_ImRepository::instance()->applySubset(ACS_CS_ImRepository::instance()->getModelCopy());

      if (!applyResult)
         FAIL() << "ACS_CS_ImRepository::instance()->applySubset(ACS_CS_ImRepository::instance()->getModelCopy() failed!";

   }

};


TEST_F(ACS_CS_ImModelSaver_test_2, modifySequenceNumberAndSaveSavesCorrectly)
{
   // Arrange
   cpBlade->sequenceNumber = 875;
   cpBlade->action = ACS_CS_ImBase::MODIFY;

   // Act
   ACS_CC_ReturnType result = ACS_CS_ImRepository::instance()->save();

   // Assert
   EXPECT_EQ(ACS_CC_SUCCESS, result);
}


















