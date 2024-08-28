/*
 * snmptest..cpp
 *
 *  Created on: Oct 5, 2010
 *      Author: mann
 */


//#include "asnmp/snmp.h"    // class def for this module
#include <asnmp/oid.h>     // class def for oids
#include <asnmp/enttraps.h> // class def for well known trap oids
#include <asnmp/vb.h>        // class def for vbs
#include <asnmp/address.h> // class def for addresses
#include <asnmp/wpdu.h>    // adapter to cmu_library / wire protocol
#include <asnmp/transaction.h> // synchronous transaction processor
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_time.h>


#include <iostream>

#include "ACS_CS_Protocol.h"
#include "ACS_CS_SNMPHandler.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_Util.h"

using std::cout;
using std::endl;

void updateMac(ACS_CS_DHCP_Info &infoobj)
{
	int Length = infoobj.getMacLength();
	unsigned char* Macaddr = new unsigned char[infoobj.getMacLength()];
	infoobj.getMAC(Macaddr,Length);
	cout << "MAC in callback : " ;
	for (int I = 0;I < Length;I++){
		cout << (int)Macaddr[I] << " : ";
	}
	cout << endl;


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
						);

void testSNMPHandler();
void testHwcSync();
void printMac(ACS_CS_DHCP_Info &info);

int main (int argc, char **argv) {

	//testHwcSync();
	testSNMPHandler();

}
//
//void testHwcSync() {
//
//	ACS_CS_EventHandle eventHandle = ACS_CS_Event::CreateEvent(true, false, ACS_CS_NS::EVENT_NAME_SHUTDOWN);
//
//	ACS_CS_SNMPHandler handler(0xC0A8A931, 0xC0A8A931, 161, "public", "private");
//	handler.activate();
//
//	ACS_CS_DHCP_Info info1;
//	ACS_CS_DHCP_Info info2;
//	ACS_CS_DHCP_Info info3;
//
//	getDhcpInfoObject(info1, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
//			ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
//			0x01000203, 0, 0, 0, 0xC0A8A938, "", "");
//
//	getDhcpInfoObject(info2, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
//				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
//				0x01000203, 0, 0, 0, 0xC0A8A938, "", "");
//
//	getDhcpInfoObject(info3, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
//				ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
//				0x01000203, 0, 0, 0, 0xC0A8A938, "", "");
//
//	std::vector<ACS_CS_DHCP_Info*> syncVector;
//	syncVector.push_back(&info1);
//	syncVector.push_back(&info2);
//	syncVector.push_back(&info3);
//
//	bool result = handler.syncClients(syncVector);
//
//	std::vector<ACS_CS_DHCP_Info*>::iterator it;
//
//	for (it = syncVector.begin(); it != syncVector.end(); it++) {
//		printMac(**it);
//	}
//
//	cout << "Sync result: " << result << endl;
//
//	cout << " ---------------- Sleeping ----------------" << endl;
//	sleep(3);
//
//	cout << " ---------------- Shutting down ----------------" << endl;
//	ACS_CS_Event::SetEvent(eventHandle);
//	sleep(3);
//}

void testSNMPHandler() {
	ACS_CS_EventHandle eventHandle = ACS_CS_Event::CreateEvent(true, false, ACS_CS_NS::EVENT_NAME_SHUTDOWN);
	ACS_CS_ServiceHandler* dummyservice = 0x0;
	ACS_CS_SNMPHandler handler(0xC0A8FE16, 0xC0A8FE16, 161, "public", "private",dummyservice);
	handler.activate();

	ACS_CS_DHCP_Info info1;

	getDhcpInfoObject(info1, ACS_CS_Protocol::FBN_SCBRP, ACS_CS_Protocol::DHCP_Client,
			ACS_CS_Protocol::Side_A, ACS_CS_Protocol::Eth_A, "SCB_RP_SIDEA_ETHA",
			0x01000203, 0, 0, 0, 0xC0A8FE16, "", "");
	int length = info1.getMacLength();
	unsigned char * mac = new unsigned char[length];
	info1.getMAC(mac, length);
	cout << "INIT mac: " << ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*)mac, length) << endl;
	for(int i =0; i<length; i++){
			cout << " : " << (char)mac[i] ;

		}
	delete [] mac;


	handler.sendGetMAC(info1);
	handler.sendGetMAC(info1);
	handler.sendGetMAC(info1);
	handler.sendGetMAC(info1);


	 length = info1.getMacLength();
	 mac = new unsigned char[length];
	info1.getMAC(mac, length);
	cout << "Retrieved mac: " << ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*)mac, length) << endl;
	for(int i =0; i<length; i++){
		cout << " : " << (char)mac[i] ;
	}
	delete [] mac;

	cout << " ---------------- Sleeping ----------------" << endl;
	sleep(3);

	handler.sendGetMAC(info1);
	length = info1.getMacLength();
	mac = new unsigned char[length];
	info1.getMAC(mac, length);
	for(int i =0; i<length; i++){
		cout << " : " << (char)mac[i] ;

	}
	cout << "Retrieved mac: " << ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*)mac, length) << endl;
	cout << endl << "Length : " << info1.getMacLength() << endl;
	cout << " ---------------- Sleeping ----------------" << endl;
	sleep(3);

	cout << " ---------------- Shutting down ----------------" << endl;
	ACS_CS_Event::SetEvent(eventHandle);
	sleep(3);

/*
	OctetStr password("public", -1);
	UdpAddress address("127.0.0.1:161");
	UdpAddress address("192.168.254.22:161");

	system_id(address, password);
*/
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

void printMac(ACS_CS_DHCP_Info &info)  {

	int length = info.getMacLength();
	unsigned char * mac = new unsigned char[length];
	info.getMAC(mac, length);
	cout << "Retrieved mac: " << ACS_CS_Protocol::CS_ProtocolChecker::binToString((char*)mac, length) << endl;
	delete [] mac;
}

