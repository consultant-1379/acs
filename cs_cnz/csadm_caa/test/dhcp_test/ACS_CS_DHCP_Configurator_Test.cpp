/*
 * @file ACS_CS_DHCP_Configurator_Test.cpp
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

#include "ACS_CS_DHCP_Configurator.h"
#include "ACS_CS_DHCP_Configurator_Test.h"

#include "ACS_CS_DHCP_Info.h"
#include "ACS_MODD_HWCProtocol.h"

extern cute::suite make_suite_testapi();

using std::string;
using std::cout;
using std::endl;

ACS_CS_DHCP_Configurator * configurator = NULL;

int main(int argc, char * argv[]) {

	setup();

	cute::suite s;
	s.push_back(CUTE(connectTest));
	s.push_back(CUTE(addNormalDhcpBoardTest));
	s.push_back(CUTE(removeNormalDhcpBoardTest));
	s.push_back(CUTE(addClientDhcpBoardTest));
	s.push_back(CUTE(removeClientDhcpBoardTest));
	s.push_back(CUTE(disConnectTest));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[ACS_CS_DHCP_Configurator Test Suite]");

	tearDown();

	return 0;
}

void setup() {
	configurator = new ACS_CS_DHCP_Configurator("127.0.0.1", 7911);
}

void tearDown() {

	if (configurator) {
		delete configurator;
		configurator = 0;
	}
}

void connectTest() {

	if (configurator) {
		configurator->connect();
		ASSERT(configurator->isConnected());
	} else {
		FAIL();
	}
}

void disConnectTest() {

	if (configurator) {
		configurator->disconnect();
		ASSERT(!configurator->isConnected());
	} else {
		FAIL();
	}
}

void addNormalDhcpBoardTest() {

	unsigned char mac[6] = {0x0,0x1,0x2,0x3,0x4,0x5};
	string bootpath("image/a/pxelinux.0");	// Boot path side A

	ACS_CS_DHCP_Info info;

	getDhcpInfoObject(info, ACS_CS_Protocol::FBN_CPUB, ACS_CS_Protocol::DHCP_Normal,
				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "CPUB_SIDEA_ETHA",
				0x01000203, 0, mac, 6, 0xC0A8A939, "192.168.169.1", bootpath);

	bool result = configurator->addNormalDhcpBoard(&info);

	ASSERT(result);
}

void removeNormalDhcpBoardTest() {
	unsigned char mac[6] = {0x0,0x1,0x2,0x3,0x4,0x5};
	string bootpath("image/a/pxelinux.0");	// Boot path side A

	ACS_CS_DHCP_Info info;

	getDhcpInfoObject(info, ACS_CS_Protocol::FBN_CPUB, ACS_CS_Protocol::DHCP_Normal,
				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "CPUB_SIDEA_ETHA",
				0x01000203, 0, mac, 6, 0xC0A8A939, "192.168.169.1", bootpath);

	bool result = configurator->removeNormalDhcpBoard(&info);

	ASSERT(result);
}

void addClientDhcpBoardTest() {
	ACS_CS_DHCP_Info info;

	getDhcpInfoObject(info, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
			ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
			0x01000203, 0, 0, 0, 0xC0A8A938, "", "");

	bool result = configurator->addClientDhcpBoard(&info);

	ASSERT(result);
}

void removeClientDhcpBoardTest() {
	ACS_CS_DHCP_Info info;

	getDhcpInfoObject(info, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
			ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
			0x01000203, 0, 0, 0, 0xC0A8A938, "", "");

	bool result = configurator->removeClientDhcpBoard(&info);

	ASSERT(result);
}


void getDhcpInfoObject(ACS_CS_DHCP_Info &info,
						ACS_CS_Protocol::CS_FBN_Identifier fbn,
						ACS_CS_Protocol::CS_DHCP_Method_Identifier method,
						ACS_CS_Protocol::CS_Side_Identifier side,
						ACS_CS_Protocol::CS_Network_Identifier network,
						std::string name,
						uint32_t mag,
						unsigned short slot,
						unsigned char *mac, int size,
						uint32_t ip,
						std::string bootServer,
						std::string bootPath
						) {

	info.setMAC(mac, size);
	info.setIP(ip);
	info.setDHCPMethod(method);
	info.setMagAddress(mag);
	info.setSlot(slot);
	info.setFBN(fbn);
	info.setSide(side);
	info.setNetwork(network);
	info.setHostName(name);
	info.setBootServer(bootServer);
	info.setBootPath(bootPath);

	info.calcOption61Id();
}


