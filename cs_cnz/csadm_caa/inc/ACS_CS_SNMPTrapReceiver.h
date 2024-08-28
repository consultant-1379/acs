#ifndef ACS_CS_SNMPTRAPRECEIVER_H_
#define ACS_CS_SNMPTRAPRECEIVER_H_ 1

#include "ACS_TRAPDS_API.h"

//#include <net-snmp/net-snmp-config.h>
//#include <net-snmp/net-snmp-includes.h>
//#include <net-snmp/library/snmp_transport.h>

#include <string.h>

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>

//#include "ACS_CS_ServiceHandler.h"

class ACS_CS_TrapHandler;
class ACS_CS_ServiceHandler;


//#include <fstream>


//#define TRAP_RECEIVER_IP "141.137.47.25"

#define TRAP_RECEIVER_COMMUNITY_NAME "public"
#define TRAP_RECEIVER_SERVICE_NAME "snmptrap_app"
#define TRAP_RECEIVER_PROT_AND_PORT "udp:162"

#define TRAP_RECEIVER_SVC_OK 1
#define TRAP_RECEIVER_SVC_ERR 0

#define TRAP_HANDLER_ERROR (int)0
#define TRAP_HANDLER_OK 	(int)1

#define INT_TRAPVALUE_NONE		(int)-1

#define INT_TRAP_BUFFERSIZE				(int)256

class ACS_CS_SNMPTrapReceiver: public ACS_TRAPDS_API{
// constructor-distructor
public:
	ACS_CS_SNMPTrapReceiver();
	ACS_CS_SNMPTrapReceiver(ACS_CS_TrapHandler *handler);
	ACS_CS_SNMPTrapReceiver(ACS_CS_TrapHandler *handler, int l);
	virtual ~ACS_CS_SNMPTrapReceiver();
	virtual void handleTrap(ACS_TRAPDS_StructVariable var);


private:
	// Define of enum
	enum typTrapEnm{
		enmTrapNull = 0,
		enmBoardPresence = 1,
		enmSCBBoardPresence = 2,
                enmBSPBoardPresence = 3
	};
	ACS_CS_ServiceHandler * serviceHandler;
	void OidToString(oid *iodIn, unsigned int len, std::string *strOut);


//	attributes
public:
	static const oid m_oidSnmpTrap[];
	static const oid m_oidBoardPresence[];
	static const oid m_oidShelfNum[];
	static const oid m_oidSlotPos[];
	static const oid m_oidBoardStatus[];

        static const oid m_BSPoidBoardPresence[];
        static const oid m_BSPoidShelfNum[];
        static const oid m_BSPoidSlotPos[];
        static const oid m_BSPoidBoardStatus[];

	static const oid m_SCBoidBoardPresence[];
	static const oid m_SCBoidMagPlugNum[];
	static const oid m_SCBoidSlotPos[];
	static const oid m_SCBoidBoardStatus[];
};


#endif /* ACS_CS_SNMPTRAPRECEIVER_H_ */
