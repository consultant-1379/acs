#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "ACS_CS_ImModel.h"
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

class ACS_CS_API_HWC_ImplementationTest : public ::testing::Test
{
protected:

   ACS_CS_ImCpBlade *cpBlade;
   ACS_CS_ImShelf *shelf;
   ACS_CS_API_HWC_R1 * hwc;
   ACS_CS_API_BoardSearch_R1 *boardSearch;

   ACS_CS_ImModel *model;

   virtual void SetUp()
   {
      hwc =  ACS_CS_API_R1::createHWCInstance();
      boardSearch = hwc->createBoardSearchInstance();
      boardSearch->reset();

      model = new ACS_CS_ImModel();

      shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();
      cpBlade = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBlade->entryId = 999;

      model->addObject(shelf);
      model->addObject(cpBlade);

      ACS_CS_ImModelSaver saver(model);
      ACS_CC_ReturnType result = saver.save();

      if (result != ACS_CC_SUCCESS)
         FAIL() << "Could not save objects!";

   }

   virtual void TearDown()
   {
      delete boardSearch;
      cpBlade->action = ACS_CS_ImBase::DELETE;
      shelf->action = ACS_CS_ImBase::DELETE;

      ACS_CS_ImModelSaver saver(model);
      ACS_CC_ReturnType result = saver.save();

      if (result != ACS_CC_SUCCESS)
         FAIL() << "Could not delete objects!";

      ACS_CS_API_R1::deleteHWCInstance(hwc);

      ACS_CS_ImRepository::instance()->getModel()->print();

   }
};



TEST_F(ACS_CS_API_HWC_ImplementationTest, getBoardIds)
{
   // Arrange
   boardSearch->setMagazine(0x01020004);

   ACS_CS_API_IdList boardList;
   ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;

   // Act
   cs_call_result = hwc->getBoardIds(boardList, boardSearch);
   int hits = boardList.size();

   // Assert
   EXPECT_EQ(ACS_CS_API_NS::Result_Success, cs_call_result);
   EXPECT_EQ(1, hits);
}



TEST_F(ACS_CS_API_HWC_ImplementationTest, getMagazine)
{
   // Arrange
   uint32_t magazine;

   // Act
   hwc->getMagazine(magazine, 999);

   // Assert
   EXPECT_EQ(0x04000201, magazine); // NBO for some strange reason
}


TEST_F(ACS_CS_API_HWC_ImplementationTest, getSlot)
{
   // Arrange
   uint16_t slot;

   // Act
   hwc->getSlot(slot, 999);

   // Assert
   EXPECT_EQ(2, slot);
}


TEST_F(ACS_CS_API_HWC_ImplementationTest, getSysType)
{
   // Arrange
   uint16_t sysType;

   // Act
   hwc->getSysType(sysType, 999);

   // Assert
   EXPECT_EQ(SINGLE_SIDED_CP, sysType);
}



TEST_F(ACS_CS_API_HWC_ImplementationTest, getSysNo)
{
   // Arrange
   uint16_t sysNo;

   // Act
   hwc->getSysNo(sysNo, 999);

   // Assert
   EXPECT_EQ(1, sysNo);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getFBN)
{
   // Arrange
   uint16_t fbn;

   // Act
   hwc->getFBN(fbn, 999);

   // Assert
   EXPECT_EQ(SCB_RP, fbn);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getSide)
{
   // Arrange
   uint16_t side;

   // Act
   hwc->getSide(side, 999);

   // Assert
   EXPECT_EQ(A, side);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getSeqNo)
{
   // Arrange
   uint16_t seqNo;

   // Act
   hwc->getSeqNo(seqNo, 999);

   // Assert
   EXPECT_EQ(1, seqNo);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getIPEthA)
{

   // Arrange
   uint32_t ip;

   // Act
   hwc->getIPEthA(ip, 999);

   // Assert
   EXPECT_EQ(0x01020001, ip);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getIPEthB)
{
   // Arrange
   uint32_t ip;

   // Act
   hwc->getIPEthB(ip, 999);

   // Assert
   EXPECT_EQ(0x01020002, ip);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getAliasEthA)
{
   // Arrange
   uint32_t ip;

   // Act
   hwc->getAliasEthA(ip, 999);

   // Assert
   EXPECT_EQ(0x02020001, ip);
}




TEST_F(ACS_CS_API_HWC_ImplementationTest, getAliasEthB)
{
   // Arrange
   uint32_t ip;

   // Act
   hwc->getAliasEthB(ip, 999);

   // Assert
   EXPECT_EQ(0x02020002, ip);
}


TEST_F(ACS_CS_API_HWC_ImplementationTest, getAliasNetmaskEthA)
{
   // Arrange
   uint32_t ip;

   // Act
   hwc->getAliasNetmaskEthA(ip, 999);

   // Assert
   EXPECT_EQ(0x04030201, ip);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, getAliasNetmaskEthB)
{
   // Arrange
   uint32_t ip;

   // Act
   hwc->getAliasNetmaskEthB(ip, 999);

   // Assert
   EXPECT_EQ(0x04030202, ip);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, getDhcpMethod)
{
   // Arrange
   uint16_t dhcp;

   // Act
   hwc->getDhcpMethod(dhcp, 999);

   // Assert
   EXPECT_EQ(DHCP_NORMAL, dhcp);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, getSysId)
{
   // Arrange
   uint16_t sysId;

   // Act
   hwc->getSysId(sysId, 999);

   // Assert
   EXPECT_EQ(1, sysId);
}


