/*
 * @file dhcptestclient.cpp
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

#include "ACS_CS_DHCP_Configurator.h"

#include "ACS_CS_DHCP_Info.h"
#include "ACS_MODD_HWCProtocol.h"


using std::string;
using std::cout;
using std::endl;

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
						);

string bootpath1("image/a/pxelinux.0");	// Boot path side A
string bootpath2("image/b/pxelinux.0"); // Boot pathisth side B

// Multi-CP system
string bootpath3("/CP1/CPA/pxelinux.0"); // Boot path side A
string bootpath4("/CP1/CPB/pxelinux.0"); // Boot path side B
string bootpath5("/BC1/CPA/pxelinux.0"); // Boot path BC (only one blade)

// Boot server
string bootIp1("192.168.169.1");		// Boot IP backplane A Node A
string bootIp2("192.168.170.1");		// Boot IP backplane B Node A

unsigned char id[7] = {0x1, 0x0,0x50,0x56,0xc0,0x0,0x1};
unsigned char mac1[6] = {0x0,0x50,0x56,0xc0,0x0,0x1};
unsigned char mac2[6] = {0xa0,0xc1,0xe0,0xb7,0xa0,0xc1};

unsigned char cpMac1[6] = {0x0,0x1,0x2,0x3,0x4,0x5};
unsigned char cpMac2[6] = {0x1,0x2,0x3,0x4,0x5,0x6};

int main (int argc, char **argv) {

	ACS_CS_DHCP_Configurator dhcp("127.0.0.1", 7911);

	ACS_CS_DHCP_Info info1;
	ACS_CS_DHCP_Info info2;
	ACS_CS_DHCP_Info info3;
	ACS_CS_DHCP_Info info4;

	getDhcpInfoObject(info1, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
			ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
			0x01000203, 0, 0, 0, 0xC0A8A938, "", "");

	getDhcpInfoObject(info2, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_B, "SCB_RP_SIDEA_ETHB",
				0x01000203, 0, 0, 0, 0xC0A8AA38, "", "");

	getDhcpInfoObject(info3, ACS_CS_Protocol::FBN_CPUB, ACS_CS_Protocol::DHCP_Normal,
				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "CPUB_SIDEA_ETHA",
				0x01000203, 0, cpMac1, 6, 0xC0A8A939, "192.168.169.1", bootpath1);

	getDhcpInfoObject(info4, ACS_CS_Protocol::FBN_CPUB, ACS_CS_Protocol::DHCP_Normal,
					ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_B, "CPUB_SIDEA_ETHB",
					0x01000203, 0, cpMac2, 6, 0xC0A8AA39, "192.168.170.1", bootpath2);


	dhcp.connect();
/*
	dhcp.addClientDhcpBoard(&info1);
	dhcp.addClientDhcpBoard(&info2);
	dhcp.addNormalDhcpBoard(&info3);
	dhcp.addNormalDhcpBoard(&info4);
*/
	string hname("5.4.0.1_1_A");
	string outname;
	dhcp.fetchIp(hname,&outname);
	std::cout << outname.c_str() << endl;
	dhcp.disconnect();




	return 0;
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
