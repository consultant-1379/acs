/*
 * ACS_CS_NetworkDefinitions.h
 *
 *  Created on: Jan 19, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_NETWORKDEFINITIONS_H_
#define CSADM_CAA_INC_ACS_CS_NETWORKDEFINITIONS_H_


#include <string>
#include <map>
#include <set>

#define ACS_CS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

namespace Network
{

	const std::string NET_APZ_A("APZ-A");
	const std::string NET_APZ_B("APZ-B");
	const std::string NET_UPD("UPD");
	const std::string NET_LDE("LDE");
	const std::string NET_DRBD("DRBD");
	const std::string NET_TIPC_A("TIPC-A");
	const std::string NET_TIPC_B("TIPC-B");
	const std::string NET_AXE_DEF("AXE-DEF");
	const std::string NET_OM("OM");
	const std::string NET_SIG_INT("INT-SIG");
	const std::string NET_UPD2("UPD2");


	const std::string NIC_APZ_A("VNICAPZA");
	const std::string NIC_APZ_B("VNICAPZB");
	const std::string NIC_UPD("VNICUPD");
	const std::string NIC_LDE("VNICLDE");
	const std::string NIC_DRBD("VNICDRBD");
	const std::string NIC_TIPC_A("VNICTIPCA");
	const std::string NIC_TIPC_B("VNICTIPCB");
	const std::string NIC_AXE_DEF("VNICDEF");
	const std::string NIC_OM("VNICOM");
	const std::string NIC_SIG_INT("VNICINT");


	const std::string DESCRIPTION_APZ_A("First internal APZ components communication");
	const std::string DESCRIPTION_APZ_B("Second internal APZ components communication");
	const std::string DESCRIPTION_UPD("Internal dual CP update channel");
	const std::string DESCRIPTION_LDE("Internal AP synchronization traffic");
	const std::string DESCRIPTION_DRBD("Internal AP data redundancy traffic");
	const std::string DESCRIPTION_TIPC_A("TIPC communication between CP and RP on plane A");
	const std::string DESCRIPTION_TIPC_B("TIPC communication between CP and RP on plane B");
	const std::string DESCRIPTION_AXE_DEF("Coordination between APZ users");
	const std::string DESCRIPTION_OM("APG OaM and provisioning traffic");
	const std::string DESCRIPTION_SIG_INT("Internal application communication");
	const std::string DESCRIPTION_UPD2("Internal update channel for CP2");


	typedef std::pair<std::string, std::string> NetNicPair_t;
	typedef std::map<std::string, std::string> NetNicMap_t;

	typedef std::pair<std::string, std::string> NetDescriptionPair_t;
	typedef std::map<std::string, std::string> NetDescriptionMap_t;

	//HW14669 Fix,Added INT-SIG in internal networks list
	const std::string INTERNAL_NETWORK_NAMES[] = {
			NET_APZ_A,
			NET_APZ_B,
			NET_UPD,
			NET_LDE,
			NET_DRBD,
			NET_TIPC_A,
			NET_TIPC_B,
			NET_AXE_DEF,
			NET_SIG_INT,
			NET_UPD2
	};

	const std::set<std::string> INTERNAL_NETWORK_NAMES_SET(INTERNAL_NETWORK_NAMES, INTERNAL_NETWORK_NAMES + ACS_CS_ARRAY_SIZE(INTERNAL_NETWORK_NAMES) );


	const NetNicPair_t STATIC_NIC_NAMES[] = {
			NetNicPair_t(NET_APZ_A, NIC_APZ_A),
			NetNicPair_t(NET_APZ_B, NIC_APZ_B),
			NetNicPair_t(NET_UPD, NIC_UPD),
			NetNicPair_t(NET_LDE, NIC_LDE),
			NetNicPair_t(NET_DRBD, NIC_DRBD),
			NetNicPair_t(NET_TIPC_A,NIC_TIPC_A),
			NetNicPair_t(NET_TIPC_B,NIC_TIPC_B),
			NetNicPair_t(NET_AXE_DEF, NIC_AXE_DEF),
			NetNicPair_t(NET_OM, NIC_OM),
			NetNicPair_t(NET_SIG_INT, NIC_SIG_INT),
			NetNicPair_t(NET_UPD2, NIC_UPD)
	};

	const NetNicMap_t STATIC_NIC_NAMES_MAP(STATIC_NIC_NAMES, STATIC_NIC_NAMES + ACS_CS_ARRAY_SIZE(STATIC_NIC_NAMES) );


	const NetDescriptionPair_t STATIC_NET_DESCRIPTIONS[] = {
			NetDescriptionPair_t(NET_APZ_A, DESCRIPTION_APZ_A),
			NetDescriptionPair_t(NET_APZ_B, DESCRIPTION_APZ_B),
			NetDescriptionPair_t(NET_UPD, DESCRIPTION_UPD),
			NetDescriptionPair_t(NET_LDE, DESCRIPTION_LDE),
			NetDescriptionPair_t(NET_DRBD, DESCRIPTION_DRBD),
			NetDescriptionPair_t(NET_TIPC_A, DESCRIPTION_TIPC_A),
			NetDescriptionPair_t(NET_TIPC_B, DESCRIPTION_TIPC_B),
			NetDescriptionPair_t(NET_AXE_DEF, DESCRIPTION_AXE_DEF),
			NetDescriptionPair_t(NET_OM, DESCRIPTION_OM),
			NetDescriptionPair_t(NET_SIG_INT, DESCRIPTION_SIG_INT),
			NetDescriptionPair_t(NET_UPD2, DESCRIPTION_UPD2)
	};

	const NetDescriptionMap_t STATIC_NET_DESCRIPTIONS_MAP(STATIC_NET_DESCRIPTIONS, STATIC_NET_DESCRIPTIONS + ACS_CS_ARRAY_SIZE(STATIC_NET_DESCRIPTIONS));

	const std::string VNIC_PREFIX("VNIC");

	enum Domain
	{
		AP_DOMAIN = 0,
		CP_DOMAIN,
		BC_DOMAIN,
		IPLB_DOMAIN,
		DOMAIN_NR
	};

	enum Type
	{
		INTERNAL_NETWORK = 0,
		EXTERNAL_NETWORK,
		TYPE_NR
	};

	const int NO_INDEX = 0;
}


#endif /* CSADM_CAA_INC_ACS_CS_NETWORKDEFINITIONS_H_ */
