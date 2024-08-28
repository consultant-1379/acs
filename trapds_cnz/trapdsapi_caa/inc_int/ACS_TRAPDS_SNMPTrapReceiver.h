/*
 * ACS_TRAPDS_SNMPTrapReceiver.h
 *
 *  Created on: Jan 24, 2012
 *      Author: eanform
 */

#ifndef ACS_TRAPDS_SNMPTRAPRECEIVER_H_
#define ACS_TRAPDS_SNMPTRAPRECEIVER_H_

#include <stdlib.h>
#include <stdio.h>

#include <ace/Task.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/library/snmp_transport.h>

#include <string.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fstream>
#include <map>
#include <set>

#include "ACS_TRAPDS_ServerConnection.h"
#include "ACS_TRAPDS_Event.h"
#include "ACS_TRAPDS_Util.h"
#include "ACS_TRAPDS_ServerConnectionImpl.h"
#include "ACS_TRAPDS_SnmpObserver.h"

using namespace std;

#define TRAP_RECEIVER_COMMUNITY_NAME "public"


#define TRAP_RECEIVER_SERVICE_NAME "snmptrap_app"

#define TRAP_RECEIVER_PROT_AND_PORT1_AP1 "udp:192.168.169.1:162"
#define TRAP_RECEIVER_PROT_AND_PORT2_AP1 "udp:192.168.170.1:162"
#define TRAP_RECEIVER_PROT_AND_PORT3_AP1 "udp:192.168.169.2:162"
#define TRAP_RECEIVER_PROT_AND_PORT4_AP1 "udp:192.168.170.2:162"

#define TRAP_RECEIVER_PROT_AND_PORT1_AP2 "udp:192.168.169.3:162"
#define TRAP_RECEIVER_PROT_AND_PORT2_AP2 "udp:192.168.170.3:162"
#define TRAP_RECEIVER_PROT_AND_PORT3_AP2 "udp:192.168.169.4:162"
#define TRAP_RECEIVER_PROT_AND_PORT4_AP2 "udp:192.168.170.4:162"

#define TRAP_RECEIVER_PROT_AND_PORT1_AP1_DMX "udp:192.254.15.19:162"
#define TRAP_RECEIVER_PROT_AND_PORT2_AP1_DMX "udp:192.254.16.19:162"
#define TRAP_RECEIVER_PROT_AND_PORT3_AP1_DMX "udp:192.254.15.20:162"
#define TRAP_RECEIVER_PROT_AND_PORT4_AP1_DMX "udp:192.254.16.20:162"

//TODO: Confirm AP2 addresses on BGCI Vlans
#define TRAP_RECEIVER_PROT_AND_PORT1_AP2_DMX "udp:192.254.15.21:162"
#define TRAP_RECEIVER_PROT_AND_PORT2_AP2_DMX "udp:192.254.16.21:162"
#define TRAP_RECEIVER_PROT_AND_PORT3_AP2_DMX "udp:192.254.15.22:162"
#define TRAP_RECEIVER_PROT_AND_PORT4_AP2_DMX "udp:192.254.16.22:162"

// For SMX env
#define TRAP_RECEIVER_PROT_AND_CLUSTER1_AP1 "udp:192.168.169.33:162"
#define TRAP_RECEIVER_PROT_AND_CLUSTER2_AP1 "udp:192.168.170.33:162"
#define TRAP_RECEIVER_PROT_AND_CLUSTER1_AP2 "udp:192.168.169.34:162"
#define TRAP_RECEIVER_PROT_AND_CLUSTER2_AP2 "udp:192.168.170.34:162"


#define TRAP_RECEIVER_SVC_OK 1
#define TRAP_RECEIVER_SVC_ERR 0

#define TRAP_HANDLER_ERROR (int)0
#define TRAP_HANDLER_OK 	(int)1

#define INT_TRAPVALUE_NONE		(int)-1

#define INT_TRAP_BUFFERSIZE				(int)256

#define UNUSED(expr) do { (void)(expr); } while (0)

typedef std::map<std::string,ACS_TRAPDS_SnmpObserver*> mapObservers;

class ACS_TRAPDS_SNMPTrapReceiver: public ACE_Task_Base
{

public:

    static int startup();
    static  ACS_TRAPDS_SNMPTrapReceiver* getInstance ();

	static netsnmp_transport* getSnmpTransport();
	int initialize();
	void finalize ();
	bool isRunning(void){return bRun;};
	void addSubscriber(ACS_TRAPDS_ServerConnection *subsciber, std::string dn);
	void removeSubscriber(std::string dn);

private:

	ACS_TRAPDS_SNMPTrapReceiver();
	virtual ~ACS_TRAPDS_SNMPTrapReceiver();

	//static methods
	static int trapDispatch(int op,struct snmp_session *session, int reqid, struct snmp_pdu *pdu, void *magic);
	virtual int svc(void);
	virtual int close ();
	void printMap();

	// Define of enum
	static netsnmp_transport* transport1;
	static netsnmp_transport* transport2;
	static netsnmp_transport* transport3;
	static netsnmp_transport* transport4;


	int trapHandler(int op,struct snmp_session *session, int reqid, struct snmp_pdu *pdu, void *magic);
	in_addr getRemoteIP(struct snmp_session *session, struct snmp_pdu *pdu);
	void OidToString(oid *iodIn, unsigned int len, std::string *strOut);


	bool bExit;
	bool bRun;
	ACS_TRAPDS_EventHandle shutdownEvent;

	mapObservers observers;

	static ACS_TRAPDS_SNMPTrapReceiver* m_instance;

};


#endif /* ACS_TRAPDS_SNMPTRAPRECEIVER_H_ */
