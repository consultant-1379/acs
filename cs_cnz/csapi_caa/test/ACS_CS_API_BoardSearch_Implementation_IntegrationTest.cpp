#include "gtest/gtest.h"
#include "gmock/gmock.h"


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
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"

class ACS_CS_API_BoardSearch_Implementation_IntegrationTest : public ::testing::Test
{
protected:

   ACS_CS_ImCp *cp;
   ACS_CS_ImCpBlade *cpBlade;
   ACS_CS_ImCpBlade *cpBlade2;
   ACS_CS_ImCpBlade *cpBlade3;

   ACS_CS_ImShelf *shelf;
   ACS_CS_API_HWC_R1 * hwc;
   ACS_CS_API_BoardSearch_R1 *boardSearch;

   ACS_CS_ImModel *model;


   virtual void SetUp()
   {
      hwc =  ACS_CS_API_R1::createHWCInstance();
      boardSearch = hwc->createBoardSearchInstance();
      boardSearch->reset();


      shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
      cp = ACS_CS_ImDefaultObjectCreator::createCpObject();
      cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade->entryId = 999;
      cpBlade->slotNumber = 20;

      cpBlade2 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade2->slotNumber = 55;

      cpBlade3 = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade3->slotNumber = 56;

      model = new ACS_CS_ImModel();

      model->addObject(shelf);
      model->addObject(cp);
      model->addObject(cpBlade);
      model->addObject(cpBlade2);
      model->addObject(cpBlade3);

      ACS_CS_ImModelSaver saver(model);
      ACS_CC_ReturnType result = saver.save();

      if (result == ACS_CC_FAILURE)
         FAIL() << "Could not save objects!";


   }

   virtual void TearDown()
   {
      cpBlade->action = ACS_CS_ImBase::DELETE;
      cpBlade2->action = ACS_CS_ImBase::DELETE;
      cpBlade3->action = ACS_CS_ImBase::DELETE;
      shelf->action = ACS_CS_ImBase::DELETE;

      ACS_CS_ImModelSaver saver(model);
      ACS_CC_ReturnType result = saver.save();

      if (result == ACS_CC_FAILURE)
         FAIL() << "Could not save objects!";

      hwc->deleteBoardSearchInstance(boardSearch);
      ACS_CS_API_R1::deleteHWCInstance(hwc);

   }

};


TEST_F(ACS_CS_API_BoardSearch_Implementation_IntegrationTest, SetSlotFindsOneEntry)
{

   // Arrange
   boardSearch->setSlot(55);

   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;

   // Act
   cs_call_result = hwc->getBoardIds(boardList, boardSearch);
   int hits = boardList.size();

   // Assert
   EXPECT_EQ(1, hits);
}



TEST_F(ACS_CS_API_BoardSearch_Implementation_IntegrationTest, FindAllThreeBoardsWithSameSide)
{

   // Arrange
   boardSearch->setSide(A);

   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;

   // Act
   cs_call_result = hwc->getBoardIds(boardList, boardSearch);
   int hits = boardList.size();

   // Assert
   EXPECT_EQ(3, hits);
}



TEST_F(ACS_CS_API_BoardSearch_Implementation_IntegrationTest, FindAllThreeBoardsWithSameMagazine)
{

   // Arrange
   boardSearch->setMagazine(0x04000201);

   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;

   // Act
   cs_call_result = hwc->getBoardIds(boardList, boardSearch);
   int hits = boardList.size();

   // Assert
   EXPECT_EQ(3, hits);
}




TEST_F(ACS_CS_API_BoardSearch_Implementation_IntegrationTest, FindAOneBoardWithMagazineAndSlotSet)
{

   // Arrange
   boardSearch->setMagazine(0x04000201);
   boardSearch->setSlot(55);

   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;

   // Act
   cs_call_result = hwc->getBoardIds(boardList, boardSearch);
   int hits = boardList.size();

   // Assert
   EXPECT_EQ(1, hits);
}













