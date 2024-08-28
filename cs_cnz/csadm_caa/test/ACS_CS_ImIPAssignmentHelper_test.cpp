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
 * @file ACS_CS_ImIPAssignmentHelper_test.cpp
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
 * 2011-08-29  ESTEVOL  Starting from scratch
 *
 ****************************************************************************/

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ACS_CS_ImUtils.h"

#include "ACS_CS_ImIPAssignmentHelper.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"

//#include "ACS_CS_ImDefaultObjectCreator.h"

class ACS_CS_ImIPAssignmentHelper_test : public ::testing::Test {
protected:
	virtual void SetUp() {
		model = new ACS_CS_ImModel();
		ACS_CS_ImIMMReader reader;

		reader.loadModel(model);

		ACS_CS_ImHardwareMgmt* hw = dynamic_cast<ACS_CS_ImHardwareMgmt*>(model->getObject(ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

		table = hw->staticIpMap;

//		table.insert("1.2.0.4_0_192.168.169.56_192.168.170.56");
//		table.insert("1.2.0.4_25_192.168.169.57_192.168.170.57");
//		table.insert("2.0.0.0_0_192.168.169.61_192.168.170.61");
//		table.insert("2.0.0.0_25_192.168.169.62_192.168.170.62");

	}

	virtual void TearDown() {
		delete model;

	}

protected:
	ACS_CS_ImModel * model;
	ACS_CS_ImIPAssignmentHelper helper;
	std::set<std::string> table;

public:
	bool isStaticIP(string ethA, string ethB);
	bool isAssignedIP(string ethA, string ethB);
	void split(const std::string &s, char delim, std::vector<std::string> &elems);
};


bool ACS_CS_ImIPAssignmentHelper_test::isStaticIP(string ethA, string ethB) {

	//		string ethA = "192.168.169." + lastPlug;
	//		string ethB = "192.168.170." + lastPlug;

	set<string>::iterator it;
	char delim = '_';
	bool found = false;


	for (it = table.begin(); it != table.end() && !found; it++) {
		std::vector<string> splitted;

		split(*it,delim,splitted);

		if (splitted.size() == 4) {
			if (ethA == splitted.at(2) || ethB == splitted.at(3))
				found = true;
		}
	}

	return found;
}

bool ACS_CS_ImIPAssignmentHelper_test::isAssignedIP(string ethA, string ethB) {

	set<const ACS_CS_ImBase*> objs;
	model->getObjects(objs, BLADE_T);
	set<const ACS_CS_ImBase*>::iterator it;
	bool found = false;

	for (it = objs.begin(); it != objs.end() && !found; it++) {

		const ACS_CS_ImBlade* blade = dynamic_cast<const ACS_CS_ImBlade*>(*it);

		if (blade) {
			if (ethA == blade->ipAddressEthA || ethB == blade->ipAddressEthB)
				found = true;

		}
	}

	return found;

}

void ACS_CS_ImIPAssignmentHelper_test::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

TEST_F(ACS_CS_ImIPAssignmentHelper_test, assignIPtoEPB1) {

	ACS_CS_ImOtherBlade* blade = new ACS_CS_ImOtherBlade();

	blade->functionalBoardName = EPB1;
	blade->slotNumber = 1;
	blade->action = ACS_CS_ImBase::CREATE;

	bool res = helper.assignIp(model, blade);

	EXPECT_TRUE(res) << "assignIP to EPB1 board failed";

	cout << "Checking IP address ethA: " << blade->ipAddressEthA << endl;
	EXPECT_TRUE(blade->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;

	cout << "Checking IP address ethB: " << blade->ipAddressEthB << endl;
	EXPECT_TRUE(blade->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;

	cout << "Check if it is a non-static IP address" << endl;
	EXPECT_FALSE(isStaticIP(blade->ipAddressEthA, blade->ipAddressEthB)) << "Static IP address assigned to EPB1 board" << endl;

	cout << "Check if it is an already assigned IP address" << endl;
	EXPECT_FALSE(isAssignedIP(blade->ipAddressEthA, blade->ipAddressEthB)) << "Already assigned IP address assigned to EPB1 board" << endl;

}

TEST_F(ACS_CS_ImIPAssignmentHelper_test, assignIPtoTwoEPB1) {

	ACS_CS_ImOtherBlade* blade_1 = new ACS_CS_ImOtherBlade();
	ACS_CS_ImOtherBlade* blade_2 = new ACS_CS_ImOtherBlade();

	blade_1->functionalBoardName = EPB1;
	blade_1->slotNumber = 1;
	blade_1->action = ACS_CS_ImBase::CREATE;
	blade_1->rdn = "Blade1";

	blade_2->functionalBoardName = EPB1;
	blade_2->slotNumber = 2;
	blade_2->action = ACS_CS_ImBase::CREATE;
	blade_2->rdn = "Blade2";

	ACS_CS_ImModel * subset = new ACS_CS_ImModel();
	subset->addObject(blade_1);
	subset->addObject(blade_2);

	ACS_CS_ImModel modelCopy(*model);

	modelCopy.applySubset(subset);

	bool res = helper.assignIp(&modelCopy, blade_1);

	EXPECT_TRUE(res) << "assignIP to EPB1 board failed";

	cout << "Checking IP address ethA: " << blade_1->ipAddressEthA << endl;
	EXPECT_TRUE(blade_1->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_1->ipAddressEthB << endl;
	EXPECT_TRUE(blade_1->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a non-static IP address... ";
	EXPECT_FALSE(isStaticIP(blade_1->ipAddressEthA, blade_1->ipAddressEthB)) << "Static IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if it is an already assigned IP address... ";
	EXPECT_FALSE(isAssignedIP(blade_1->ipAddressEthA, blade_1->ipAddressEthB)) << "Already assigned IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;


	model->applySubset(subset);

	res = helper.assignIp(model, blade_2);

	EXPECT_TRUE(res) << "assignIP to EPB1 board failed";

	cout << "Checking IP address ethA: " << blade_2->ipAddressEthA << endl;
	EXPECT_TRUE(blade_2->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_2->ipAddressEthB << endl;
	EXPECT_TRUE(blade_2->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a non-static IP address... ";
	EXPECT_FALSE(isStaticIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Static IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if it is an already assigned IP address... ";
	EXPECT_FALSE(isAssignedIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Already assigned IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if the two boards have different IP addresses... ";
	EXPECT_FALSE(blade_2->ipAddressEthA == blade_1->ipAddressEthA) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthA << endl;
	EXPECT_FALSE(blade_2->ipAddressEthB == blade_1->ipAddressEthB) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthB << endl;
	cout << "DONE!" << endl;

	delete subset;
}

TEST_F(ACS_CS_ImIPAssignmentHelper_test, assignStaticIPtoTwoSCXB) {

	ACS_CS_ImOtherBlade* blade_1 = new ACS_CS_ImOtherBlade();
	ACS_CS_ImOtherBlade* blade_2 = new ACS_CS_ImOtherBlade();

	blade_1->functionalBoardName = SCXB;
	blade_1->slotNumber = 0;
	blade_1->action = ACS_CS_ImBase::CREATE;
	blade_1->rdn = "otherBladeId=0,shelfId=1.2.0.4,hardwareMgmtId=1,AxeEquipmentequipmentMId=1";

	blade_2->functionalBoardName = SCXB;
	blade_2->slotNumber = 25;
	blade_2->action = ACS_CS_ImBase::CREATE;
	blade_2->rdn = "otherBladeId=25,shelfId=1.2.0.4,hardwareMgmtId=1,AxeEquipmentequipmentMId=1";

	ACS_CS_ImModel * subset = new ACS_CS_ImModel();
	subset->addObject(blade_1);
	subset->addObject(blade_2);

	ACS_CS_ImModel modelCopy(*model);

	modelCopy.applySubset(subset);

	bool res = helper.assignIp(&modelCopy, blade_1);

	EXPECT_TRUE(res) << "assignIP to SCXB board failed";

	cout << "Checking IP address ethA: " << blade_1->ipAddressEthA << endl;
	EXPECT_TRUE(blade_1->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_1->ipAddressEthB << endl;
	EXPECT_TRUE(blade_1->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a static IP address... ";
	EXPECT_TRUE(isStaticIP(blade_1->ipAddressEthA, blade_1->ipAddressEthB)) << "Non Static IP address assigned to SCXB board" << endl;
	cout << "DONE!" << endl;


	ACS_CS_ImModel modelCopy_2(*model);

	modelCopy_2.applySubset(subset);

	res = helper.assignIp(&modelCopy_2, blade_2);

	EXPECT_TRUE(res) << "assignIP to EPB1 board failed";

	cout << "Checking IP address ethA: " << blade_2->ipAddressEthA << endl;
	EXPECT_TRUE(blade_2->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_2->ipAddressEthB << endl;
	EXPECT_TRUE(blade_2->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a static IP address... ";
	EXPECT_TRUE(isStaticIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Non Static IP address assigned to SCXB board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if the two boards have different IP addresses... ";
	EXPECT_FALSE(blade_2->ipAddressEthA == blade_1->ipAddressEthA) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthA << endl;
	EXPECT_FALSE(blade_2->ipAddressEthB == blade_1->ipAddressEthB) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthB << endl;
	cout << "DONE!" << endl;

	delete subset;
}

TEST_F(ACS_CS_ImIPAssignmentHelper_test, assignDynamicIPtoTwoSCXB) {

	ACS_CS_ImOtherBlade* blade_1 = new ACS_CS_ImOtherBlade();
	ACS_CS_ImOtherBlade* blade_2 = new ACS_CS_ImOtherBlade();

	blade_1->functionalBoardName = SCXB;
	blade_1->slotNumber = 0;
	blade_1->action = ACS_CS_ImBase::CREATE;
	blade_1->rdn = "OtherBladeId=0,ShelfId=9.0.0.0,HardwareMgmtId=1";

	blade_2->functionalBoardName = SCXB;
	blade_2->slotNumber = 25;
	blade_2->action = ACS_CS_ImBase::CREATE;
	blade_2->rdn = "OtherBladeId=25,ShelfId=9.0.0.0,HardwareMgmtId=1";

	ACS_CS_ImModel * subset = new ACS_CS_ImModel();
	subset->addObject(blade_1);
	subset->addObject(blade_2);

	ACS_CS_ImModel modelCopy(*model);

	modelCopy.applySubset(subset);

	bool res = helper.assignIp(&modelCopy, blade_1);

	EXPECT_TRUE(res) << "assignIP to SCXB board failed";

	cout << "Checking IP address ethA: " << blade_1->ipAddressEthA << endl;
	EXPECT_TRUE(blade_1->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_1->ipAddressEthB << endl;
	EXPECT_TRUE(blade_1->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a static IP address... ";
	EXPECT_FALSE(isStaticIP(blade_1->ipAddressEthA, blade_1->ipAddressEthB)) << "Static IP address not assigned to SCXB board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if it is an already assigned IP address... ";
	EXPECT_FALSE(isAssignedIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Already assigned IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;


	model->applySubset(subset);

	res = helper.assignIp(model, blade_2);

	EXPECT_TRUE(res) << "assignIP to EPB1 board failed";

	cout << "Checking IP address ethA: " << blade_2->ipAddressEthA << endl;
	EXPECT_TRUE(blade_2->ipAddressEthA.size() > 0) << "IP address ethA has size 0" << endl;
	cout << "DONE!" << endl;

	cout << "Checking IP address ethB: " << blade_2->ipAddressEthB << endl;
	EXPECT_TRUE(blade_2->ipAddressEthB.size() > 0) << "IP address ethB has size 0" << endl;;
	cout << "DONE!" << endl;

	cout << "Check if it is a static IP address... ";
	EXPECT_FALSE(isStaticIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Static IP address assigned to SCXB board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if it is an already assigned IP address... ";
	EXPECT_FALSE(isAssignedIP(blade_2->ipAddressEthA, blade_2->ipAddressEthB)) << "Already assigned IP address assigned to EPB1 board" << endl;
	cout << "DONE!" << endl;

	cout << "Check if the two boards have different IP addresses... ";
	EXPECT_FALSE(blade_2->ipAddressEthA == blade_1->ipAddressEthA) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthA << endl;
	EXPECT_FALSE(blade_2->ipAddressEthB == blade_1->ipAddressEthB) << "ERROR: Two EPB1 boards have the same IP address " << blade_2->ipAddressEthB << endl;
	cout << "DONE!" << endl;

	delete subset;
}
