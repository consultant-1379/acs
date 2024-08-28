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
 * @file ACS_CS_API_BoardSearch_Test.cpp
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
 * 2011-11-03  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_R1.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_IdList_Implementation.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"

using namespace std;

const uint32_t uIPA = 0xC0A8A93A;
const uint32_t uIPB = 0xC0A8AA3A;

const string sIPA = "192.168.169.58";
const string sIPB = "192.168.170.58";


class ACS_CS_API_BoardSearch_ImplementationTest : public ::testing::Test
{
protected:


   ACS_CS_API_BoardSearch_Implementation *boardSearch;
   ACS_CS_API_HWC_R1 * hwc;

   virtual void SetUp()
   {
      hwc =  ACS_CS_API_R1::createHWCInstance();
      boardSearch = dynamic_cast<ACS_CS_API_BoardSearch_Implementation *>(hwc->createBoardSearchInstance());
      boardSearch->reset();
   }

   virtual void TearDown()
   {
      delete boardSearch;
   }
};

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setMagazineSetsCorrectMagazine)
{
   // Arrange
   boardSearch->setMagazine(0x01020004);

   // Act
   string str = boardSearch->searchableBlade->magazine;

   // Assert
   // NOTE: This is a very special deviation from the other ones.
   // This string representation is on network byte order by default, and not host byte order like the others
   EXPECT_EQ("4.0.2.1", str);

}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSlotSetsCorrectSlot)
{
   boardSearch->setSlot(5);


}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysTypeSetsCorrectSysType)
{
   // Arrange
   uint16_t systemType = 1000;
   boardSearch->setSysType(systemType);

   // Act
   SystemTypeEnum type = boardSearch->searchableBlade->systemType;

   // Assert
   EXPECT_EQ(type, DOUBLE_SIDED_CP);

}



TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setFBNSetsCorrectFBN)
{
   // Arrange
   uint16_t fbn = 300;
   boardSearch->setFBN(fbn);

   // Act
   FunctionalBoardNameEnum functionalBoardName = boardSearch->searchableBlade->functionalBoardName;

   // Assert
   EXPECT_EQ(functionalBoardName, APUB);

}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSideSetsCorrectSide)
{
   // Arrange
   boardSearch->setSide(0);

   // Act
   SideEnum side = boardSearch->searchableBlade->side;

   // Assert
   EXPECT_EQ(side, A);
}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setIPEthASetsCorrectIPEthA)
{
   // Arrange
   boardSearch->setIPEthA(uIPA);

   // Act
   IpDNSAddress address = boardSearch->searchableBlade->ipAddressEthA;
   string str = (string) address;

   // Assert
   EXPECT_STREQ(str.c_str(), sIPA.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setIPEthBSetsCorrectIPEthB)
{
   // Arrange
   boardSearch->setIPEthB(uIPB);

   // Act
   string address = (string) boardSearch->searchableBlade->ipAddressEthB;

   // Assert
   EXPECT_STREQ(address.c_str(), sIPB.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setAliasEthASetsCorrectAliasEthA)
{
   // Arrange
   boardSearch->setAliasEthA(uIPA);

   // Act
   string address = (string) boardSearch->searchableBlade->ipAliasEthA;

   // Assert
   EXPECT_STREQ(address.c_str(), sIPA.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setAliasEthBSetsCorrectAliasEthB)
{
   // Arrange
   boardSearch->setAliasEthB(uIPB);

   // Act
   string address = (string) boardSearch->searchableBlade->ipAliasEthB;

   // Assert
   EXPECT_STREQ(address.c_str(), sIPB.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setAliasNetmaskEthASetsCorrectAliasNetmaskEthA)
{
   // Arrange
   boardSearch->setAliasNetmaskEthA(uIPA);

   // Act
   string address = (string) boardSearch->searchableBlade->aliasNetmaskEthA;

   // Assert
   EXPECT_STREQ(address.c_str(), sIPA.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setAliasNetmaskEthBSetsCorrectAliasNetmaskEthB)
{
   // Arrange
   boardSearch->setAliasNetmaskEthB(uIPB);

   // Act
   string address = (string) boardSearch->searchableBlade->aliasNetmaskEthB;

   // Assert
   EXPECT_STREQ(address.c_str(), sIPB.c_str());
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setDhcpMethodSetsCorrectDhcpMethod)
{
   // Arrange
   boardSearch->setDhcpMethod(1);
   // Act
   DhcpEnum dhcp = boardSearch->searchableBlade->dhcpOption;

   // Assert
   EXPECT_EQ(DHCP_NORMAL, dhcp);
}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysId1_SystemNumberAndSequenceNumberIsSetCorrectly)
{
   // Arrange
   boardSearch->setSysId(1);

   // Act
   SystemTypeEnum sysType = boardSearch->searchableBlade->systemType;
   uint16_t sequencNumber = boardSearch->searchableBlade->sequenceNumber;

   // Assert
   EXPECT_EQ(SINGLE_SIDED_CP, sysType);
   EXPECT_EQ(1, sequencNumber);
}

TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysId1001_SystemNumberAndSystemNumberIsSetCorrectly)
{
   // Arrange
   boardSearch->setSysId(1001);

   // Act
   SystemTypeEnum sysType = boardSearch->searchableBlade->systemType;
   uint16_t systemNumber = boardSearch->searchableBlade->systemNumber;

   // Assert
   EXPECT_EQ(DOUBLE_SIDED_CP, sysType);
   EXPECT_EQ(1, systemNumber);
}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysId2001_SystemNumberAndSystemNumberIsSetCorrectly)
{
   // Arrange
   boardSearch->setSysId(2001);

   // Act
   SystemTypeEnum sysType = boardSearch->searchableBlade->systemType;
   uint16_t systemNumber = boardSearch->searchableBlade->systemNumber;

   // Assert
   EXPECT_EQ(AP, sysType);
   EXPECT_EQ(1, systemNumber);
}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysId1002_SystemNumberAndSystemNumberIsSetCorrectly)
{
   // Arrange
   boardSearch->setSysId(1002);

   // Act
   SystemTypeEnum sysType = boardSearch->searchableBlade->systemType;
   uint16_t systemNumber = boardSearch->searchableBlade->systemNumber;

   // Assert
   EXPECT_EQ(DOUBLE_SIDED_CP, sysType);
   EXPECT_EQ(2, systemNumber);
}


TEST_F(ACS_CS_API_BoardSearch_ImplementationTest, setSysId2001_SystemNumberAndSystemNumberIsSetCorrectlySequenceNumberStillZero)
{
   // Arrange
   boardSearch->setSysId(2001);

   // Act
   SystemTypeEnum sysType = boardSearch->searchableBlade->systemType;
   uint16_t sequencNumber = boardSearch->searchableBlade->sequenceNumber;

   // Assert
   EXPECT_EQ(0, sequencNumber);
}
