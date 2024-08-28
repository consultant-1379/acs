//	*********************************************************
//	********************
//	 COPYRIGHT Ericsson 2011.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2011.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2011 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************
//	********************

#ifndef ACS_CS_API_HWCTableChange_R1_h
#define ACS_CS_API_HWCTableChange_R1_h 1

#include "ACS_CS_API_Common_R1.h"


struct ACS_CS_API_HWCTableData_R1
{
	static const uint32_t UNSPECIFIED_MAGAZINE = 0xFFFFFFFF;
	static const uint32_t UNSPECIFIED_ADDRESS = 0xFFFFFFFF;
	static const uint16_t UNSPECIFIED_SLOT = 0xFFFF;
	static const uint16_t UNSPECIFIED_SYSTYPE = 0xFFFF;
	static const uint16_t UNSPECIFIED_SYSNO = 0xFFFF;
	static const uint16_t UNSPECIFIED_FBN = 0xFFFF;
	static const uint16_t UNSPECIFIED_SIDE = 0xFFFF;
	static const uint16_t UNSPECIFIED_SEQNO = 0xFFFF;
	static const uint16_t UNSPECIFIED_DHCPMETHOD = 0xFFFF;
	static const uint16_t UNSPECIFIED_SYSID = 0xFFFF;

	ACS_CS_API_HWCTableData_R1() :
		operationType(ACS_CS_API_TableChangeOperation::Unspecified),
		magazine(UNSPECIFIED_MAGAZINE),
		slot(UNSPECIFIED_SLOT),
		sysType(UNSPECIFIED_SYSTYPE),
		sysNo(UNSPECIFIED_SYSNO),
		fbn(UNSPECIFIED_FBN),
		side(UNSPECIFIED_SIDE),
		seqNo(UNSPECIFIED_SEQNO),
		ipEthA(UNSPECIFIED_ADDRESS),
		ipEthB(UNSPECIFIED_ADDRESS),
		aliasEthA(UNSPECIFIED_ADDRESS),
		aliasEthB(UNSPECIFIED_ADDRESS),
		netmaskAliasEthA(UNSPECIFIED_ADDRESS),
		netmaskAliasEthB(UNSPECIFIED_ADDRESS),
		dhcpMethod(UNSPECIFIED_DHCPMETHOD),
		sysId(UNSPECIFIED_SYSID)
	{}

	ACS_CS_API_TableChangeOperation::OpType operationType;
	uint32_t magazine;
	uint16_t slot;
	uint16_t sysType;
	uint16_t sysNo;
	uint16_t fbn;
	uint16_t side;
	uint16_t seqNo;
	uint32_t ipEthA;
	uint32_t ipEthB;
	uint32_t aliasEthA;
	uint32_t aliasEthB;
	uint32_t netmaskAliasEthA;
	uint32_t netmaskAliasEthB;
	uint16_t dhcpMethod;
	uint16_t sysId;
	std::string softwarePackage;
	std::string productId;
	std::string productRevision;
};

struct ACS_CS_API_HWCTableChange_R1
{
	int dataSize;
	ACS_CS_API_HWCTableData_R1 *hwcData;
};


#endif
