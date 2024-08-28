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
 * @file ACS_CS_ServiceHandler_test.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XPLODAM
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XPLODAM  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_DHCP_Configurator.h"
#include "ACS_CS_Util.h"

#include <set>
#include <cstring>
#include <sstream>
#include <iostream>

#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImBase.h"


#include "gtest/gtest.h"
#include "gmock/gmock.h"

using std::cout;
using std::endl;

// PREREQUISITES:
// a shelf with rdn="shelfId=1,hardwareConfigurationCategoryId=1,configurationInfoId=1" in imm

class dhcp : public ::testing::Test {
protected:
	virtual void SetUp()
	{
	   shelf = ACS_CS_ImDefaultObjectCreator::createShelfObject();

		//cpBladeClient = new ACS_CS_ImCpBlade();

		cpBladeClient = ACS_CS_ImDefaultObjectCreator::createCpBladeObject();
      cpBladeClient->slotNumber = 20;

//		cpBladeClient->rdn = "cpBladeId=20,shelfId=1_2_0_4,hardwareConfigurationCategoryId=1,configurationInfoId=1";
//		cpBladeClient->systemType = SINGLE_SIDED_CP;
//		cpBladeClient->action = ACS_CS_ImBase::CREATE;
//		cpBladeClient->systemNumber = 0;
//		cpBladeClient->side = A;
//		cpBladeClient->sequenceNumber = 1;
		cpBladeClient->macAddressEthA = "654321";
//		cpBladeClient->macAddressEthB = "";
//		cpBladeClient->ipAliasEthA = "";
//		cpBladeClient->ipAliasEthB = "";
//		cpBladeClient->ipAddressEthA = "";
//		cpBladeClient->ipAddressEthB = "";
//		cpBladeClient->functionalBoardName = CPUB;
//		cpBladeClient->dhcpOption = DHCP_CLIENT;
//		cpBladeClient->aliasNetmaskEthA = "";
//		cpBladeClient->aliasNetmaskEthB = "";
		cpBladeClient->entryId = 234;

		cpBladeNormal = dynamic_cast<ACS_CS_ImCpBlade *>(cpBladeClient->clone());

		cpBladeNormal->dhcpOption = DHCP_NORMAL;

		host_nameA << "1.2.0.4" << "_" << cpBladeClient->slotNumber << "_A";
		host_nameB << "1.2.0.4" << "_" << cpBladeClient->slotNumber << "_B";


		ACS_CS_ImRepository::instance()->addObject(shelf);
		ACS_CS_ImRepository::instance()->addObject(cpBladeClient);
		ACS_CS_ImRepository::instance()->save();

	}

	virtual void TearDown()
	{
	   shelf->action = ACS_CS_ImBase::DELETE;
	   cpBladeClient->action = ACS_CS_ImBase::DELETE;
      ACS_CS_ImRepository::instance()->save();
      ACS_CS_ImRepository::instance()->applySubset(ACS_CS_ImRepository::instance()->getModelCopy());
	}

	static ACS_CS_ServiceHandler serviceHandler;
	ACS_CS_ImCpBlade *cpBladeClient;
	ACS_CS_ImCpBlade *cpBladeNormal;
	ACS_CS_ImShelf *shelf;

	std::stringstream host_nameA;
	std::stringstream host_nameB;



};

ACS_CS_ServiceHandler dhcp::serviceHandler;

TEST_F(dhcp, DISABLED_addingBladeClient)
{
	// Arrange
	string resultAddressA;
	string resultAddressB;
	bool resultA;
	bool resultB;
	ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Act
	serviceHandler.addToDHCPTable(cpBladeClient);

	dhcp.connect();
	resultA = dhcp.fetchIp(host_nameA.str(), resultAddressA);
	resultB = dhcp.fetchIp(host_nameB.str(), resultAddressB);
	dhcp.disconnect();

	// Assert
	EXPECT_TRUE(resultA);
	EXPECT_TRUE(resultAddressA == cpBladeClient->ipAddressEthA);
	EXPECT_TRUE(resultB);
	EXPECT_TRUE(resultAddressB == cpBladeClient->ipAddressEthB);
}

TEST_F(dhcp, DISABLED_removingBladeClient)
{
	// Arrange
	string resultAddressA;
	string resultAddressB;
	bool resultA;
	bool resultB;
	ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Act
	serviceHandler.removeFromDHCPTable(cpBladeClient);

	dhcp.connect();
	resultA = dhcp.fetchIp(host_nameA.str(), resultAddressA);
	resultB = dhcp.fetchIp(host_nameB.str(), resultAddressB);
	dhcp.disconnect();

	// Assert
	EXPECT_FALSE(resultA || resultB);
}

TEST_F(dhcp, DISABLED_addingBladeNormal)
{
	// Arrange
	string resultAddressA;
	string resultAddressB;
	bool resultA;
	bool resultB;
	ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Act
	serviceHandler.addToDHCPTable(cpBladeNormal);

	dhcp.connect();
	resultA = dhcp.fetchIp(host_nameA.str(), resultAddressA);
	resultB = dhcp.fetchIp(host_nameB.str(), resultAddressB);
	dhcp.disconnect();

	// Assert
	EXPECT_TRUE(resultA && resultAddressA == cpBladeNormal->ipAddressEthA && resultB && resultAddressB == cpBladeNormal->ipAddressEthB);
}

TEST_F(dhcp, DISABLED_removingBladeNormal)
{
	// Arrange
	string resultAddressA;
	string resultAddressB;
	bool resultA;
	bool resultB;
	ACS_CS_DHCP_Configurator dhcp(ACS_CS_NS::DHCP_Server_IP, ACS_CS_NS::DHCP_Server_Port);

	// Act
	serviceHandler.removeFromDHCPTable(cpBladeNormal);

	dhcp.connect();
	resultA = dhcp.fetchIp(host_nameA.str(), resultAddressA);
	resultB = dhcp.fetchIp(host_nameB.str(), resultAddressB);
	dhcp.disconnect();

	// Assert
	EXPECT_FALSE(resultA || resultB);
}



TEST_F(dhcp, MacUpdate_updateDhcpInfoCorrect)
{
   // Arrange
   ACS_CS_DHCP_Info dhcpInfo;

   dhcpInfo.setMAC("123456", 6);
   dhcpInfo.setMagAddress(0x04000201);
   dhcpInfo.setSlot(20);
   dhcpInfo.setNetwork(ACS_CS_Protocol::Eth_A);

   // Act
   serviceHandler.MacUpdate(&dhcpInfo);

   const ACS_CS_ImBlade *blade = ACS_CS_ImRepository::instance()->getModel()->getBlade(234);
   // Assert
   EXPECT_TRUE(blade != NULL);
   EXPECT_STREQ("123456", blade->macAddressEthA.c_str());

}
