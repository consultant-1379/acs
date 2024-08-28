/*
 * @file ACS_CS_DHCP_Info_Test.cpp
 * @author xminaon
 * @date 2010-09-10
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#include <iostream>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_DHCP_Info_Test.h"

extern cute::suite make_suite_testapi();

using std::string;
using std::cout;
using std::endl;

int main(int argc, char * argv[]) {

	cute::suite s;

	s.push_back(CUTE(constructorTest));
	s.push_back(CUTE(assignmentOperatorTest));
	s.push_back(CUTE(copyConstructorTest));
	s.push_back(CUTE(mixedOperatorTest));
	s.push_back(CUTE(option61Test));

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[ACS_CS_DHCP_Info_Test Test Suite]");

	return 0;
}

void constructorTest() {

	unsigned char mac1[] = {0x1,0x2,0x3,0x4,0x5,0x6};

	// Create info object and populate it
	ACS_CS_DHCP_Info info1(0xC0A8A938, mac1, 6, "somename", "Some comment");

	ASSERTM("Name not matching", info1.getHostName().compare("somename")==0);
	ASSERTM("Comment not matching", info1.getComment().compare("Some comment")==0);
	ASSERTM("IP not matching", info1.getIP() == 0xC0A8A938 );

	int macLength = info1.getMacLength();
	unsigned char * mac2 = new unsigned char[macLength];
	info1.getMAC(mac2, macLength);
	ASSERTM("MACs not matching", memcmp(mac1, mac2, macLength) == 0 );
	delete [] mac2;
	mac2 = 0;

}

void assignmentOperatorTest() {

	// Create info object and populate it
	ACS_CS_DHCP_Info info1;
	unsigned char mac1[] = {0x1,0x2,0x3,0x4,0x5,0x6};
	info1.setBootPath("somebootpath");
	info1.setBootServer("someserver");
	info1.setComment("Some comment");
	info1.setDHCPMethod(ACS_CS_Protocol::DHCP_Client);
	info1.setFBN(ACS_CS_Protocol::FBN_SCBRP);
	info1.setHostName("somename");
	info1.setIP(0xC0A8A938);
	info1.setMAC(mac1, 6);
	info1.setMagAddress(0x01000203);
	info1.setNetwork(ACS_CS_Protocol::Eth_A);
	info1.setPersistent(true);
	info1.setSCBRP(true);
	info1.setScope(1);
	info1.setSide(ACS_CS_Protocol::Side_A);
	info1.setSlot(0);
	info1.setSync(false);
	info1.setSysId(1001);
	info1.setSystemType(ACS_CS_Protocol::SysType_CP);

	// Create new object and use assignment operator
	ACS_CS_DHCP_Info info2;
	info2 = info1;

	ASSERTM("Bootpath not matching", info1.getBootPath().compare(info2.getBootPath())==0);
	ASSERTM("Bootserver not matching", info1.getBootServer().compare(info2.getBootServer())==0);
	ASSERTM("Comment not matching", info1.getComment().compare(info2.getComment())==0);
	ASSERTM("DHCP method not matching", info1.getDHCPMethod() == info2.getDHCPMethod() );
	ASSERTM("FBN method not matching", info1.getFBN() == info2.getFBN() );
	ASSERTM("Hostname not matching", info1.getHostName().compare(info2.getHostName())==0);
	ASSERTM("IP not matching", info1.getIP() == info2.getIP() );
	ASSERTM("IP string not matching", info1.getIPString().compare(info2.getIPString())==0);
	ASSERTM("Magazine not matching", info1.getMagAddress() == info2.getMagAddress() );
	ASSERTM("Network not matching", info1.getNetwork() == info2.getNetwork() );
	ASSERTM("Persistence not matching", info1.isPersistent() == info2.isPersistent() );
	ASSERTM("SCBRP not matching", info1.isSCBRP() == info2.isSCBRP() );
	ASSERTM("Scope not matching", info1.getScope() == info2.getScope() );
	ASSERTM("Side not matching", info1.getSide() == info2.getSide() );
	ASSERTM("Slot not matching", info1.getSlot() == info2.getSlot() );
	ASSERTM("SCBRP not matching", info1.isSync() == info2.isSync() );
	ASSERTM("Sysid not matching", info1.getSysId() == info2.getSysId() );
	ASSERTM("Slot not matching", info1.getSystemType() == info2.getSystemType() );

	int mac2Length = info2.getMacLength();
	unsigned char * mac2 = new unsigned char[mac2Length];
	info2.getMAC(mac2, mac2Length);
	ASSERTM("MACs not matching", memcmp(mac1, mac2, mac2Length) == 0 );
	delete [] mac2;
	mac2 = 0;
}

void copyConstructorTest() {

	// Create info object and populate it
	ACS_CS_DHCP_Info info1;
	unsigned char mac1[] = {0x1,0x2,0x3,0x4,0x5,0x6};
	info1.setBootPath("somebootpath");
	info1.setBootServer("someserver");
	info1.setComment("Some comment");
	info1.setDHCPMethod(ACS_CS_Protocol::DHCP_Client);
	info1.setFBN(ACS_CS_Protocol::FBN_SCBRP);
	info1.setHostName("somename");
	info1.setIP(0xC0A8A938);
	info1.setMAC(mac1, 6);
	info1.setMagAddress(0x01000203);
	info1.setNetwork(ACS_CS_Protocol::Eth_A);
	info1.setPersistent(true);
	info1.setSCBRP(true);
	info1.setScope(1);
	info1.setSide(ACS_CS_Protocol::Side_A);
	info1.setSlot(0);
	info1.setSync(false);
	info1.setSysId(1001);
	info1.setSystemType(ACS_CS_Protocol::SysType_CP);

	// Create new object and use copy constructor
	ACS_CS_DHCP_Info info2(info1);

	ASSERTM("Bootpath not matching", info1.getBootPath().compare(info2.getBootPath())==0);
	ASSERTM("Bootserver not matching", info1.getBootServer().compare(info2.getBootServer())==0);
	ASSERTM("Comment not matching", info1.getComment().compare(info2.getComment())==0);
	ASSERTM("DHCP method not matching", info1.getDHCPMethod() == info2.getDHCPMethod() );
	ASSERTM("FBN method not matching", info1.getFBN() == info2.getFBN() );
	ASSERTM("Hostname not matching", info1.getHostName().compare(info2.getHostName())==0);
	ASSERTM("IP not matching", info1.getIP() == info2.getIP() );
	ASSERTM("IP string not matching", info1.getIPString().compare(info2.getIPString())==0);
	ASSERTM("Magazine not matching", info1.getMagAddress() == info2.getMagAddress() );
	ASSERTM("Network not matching", info1.getNetwork() == info2.getNetwork() );
	ASSERTM("Persistence not matching", info1.isPersistent() == info2.isPersistent() );
	ASSERTM("SCBRP not matching", info1.isSCBRP() == info2.isSCBRP() );
	ASSERTM("Scope not matching", info1.getScope() == info2.getScope() );
	ASSERTM("Side not matching", info1.getSide() == info2.getSide() );
	ASSERTM("Slot not matching", info1.getSlot() == info2.getSlot() );
	ASSERTM("SCBRP not matching", info1.isSync() == info2.isSync() );
	ASSERTM("Sysid not matching", info1.getSysId() == info2.getSysId() );
	ASSERTM("Slot not matching", info1.getSystemType() == info2.getSystemType() );

	int mac2Length = info2.getMacLength();
	unsigned char * mac2 = new unsigned char[mac2Length];
	info2.getMAC(mac2, mac2Length);
	ASSERTM("MACs not matching", memcmp(mac1, mac2, mac2Length) == 0 );
	delete [] mac2;
	mac2 = 0;
}

void mixedOperatorTest() {

	unsigned char mac1[] = {0x1,0x2,0x3,0x4,0x5,0x6};
	ACS_CS_DHCP_Info info1(0xC0A8A938, mac1, 6, "somename", "Some comment");
	ACS_CS_DHCP_Info info2(0xC0A8A938, mac1, 6, "somename", "Some comment");
	ACS_CS_DHCP_Info info3(0xC0A8A939, mac1, 6, "somename", "Some comment");
	ACS_CS_DHCP_Info info4(0xC0A8A940, mac1, 6, "somename", "Some comment");

	ASSERT(info1 == info2);
	ASSERT(info1 < info3);
	ASSERT(info3 > info1);
	ASSERT(info3 <= info4);
	ASSERT(info4 >= info3);
	ASSERT(info1 != info3);
}

void option61Test() {
	// Create info object and populate it
	ACS_CS_DHCP_Info info1;
	unsigned char mac1[] = {0x1,0x2,0x3,0x4,0x5,0x6};
	info1.setFBN(ACS_CS_Protocol::FBN_SCBRP);
	info1.setDHCPMethod(ACS_CS_Protocol::DHCP_Client);
	info1.setHostName("somename");
	info1.setIP(0xC0A8A938);
	info1.setMAC(mac1, 6);
	info1.setMagAddress(0x01000203);
	info1.setNetwork(ACS_CS_Protocol::Eth_A);
	info1.setSide(ACS_CS_Protocol::Side_A);
	info1.setSlot(25);

	info1.calcOption61Id(ACS_CS_Protocol::FBN_SCBRP);

	int macLength = info1.getMacLength();
	unsigned char * mac2 = new unsigned char[macLength];
	info1.getMAC(mac2, macLength);
	//ASSERTM("MACs not matching", memcmp(mac1, mac2, macLength) == 0 );
	delete [] mac2;
	mac2 = 0;

}


