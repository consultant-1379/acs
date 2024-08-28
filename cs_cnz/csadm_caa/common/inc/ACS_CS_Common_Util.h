/*
 * @file ACS_CS_Common_Util.h
 * @author xminaon
 * @date Nov 8, 2010
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



#ifndef ACS_CS_COMMON_UTIL_H_
#define ACS_CS_COMMON_UTIL_H_

#include "ACS_CS_Protocol.h"
#include "ACS_CS_ImModel.h"

// APGCC DEFINES

typedef enum {
	ACS_CS_HWTYPE_NOVALUE	= 0,
	ACS_CS_HWTYPE_VM		= 1,
	ACS_CS_HWTYPE_GEP1		= 2,
	ACS_CS_HWTYPE_GEP2		= 3,
	ACS_CS_HWTYPE_GEP5		= 4,
	ACS_CS_HWTYPE_GEP5_400	= 5,
	ACS_CS_HWTYPE_GEP5_1200	= 6,
	ACS_CS_HWTYPE_GEP5_64_1200	= 7,
	ACS_CS_HWTYPE_GEP7L_400		= 8,
	ACS_CS_HWTYPE_GEP7L_1600	= 9,
	ACS_CS_HWTYPE_GEP7_128_1600	=10
}ACS_CS_HWTYPE_typeT;

class ACS_CS_TableEntry;
class ACS_CS_Attribute;

class ACS_CS_Common_Util
{
	public:
		static unsigned short getSysId (const ACS_CS_TableEntry *tableEntry);
		//static uint16_t getSysId (const ACS_CS_ImCp *cp);
		static bool getAddress(const ACS_CS_Attribute *attribute, uint32_t &address);
		static bool getMacAddress(const ACS_CS_Attribute *attribute, char ** mac, int length);
		static bool getDhcpMethod(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_DHCP_Method_Identifier &method);
		static bool getSlot(const ACS_CS_Attribute *attribute, uint16_t &slot);
		static bool getSide(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_Side_Identifier &side);
		static bool getNetwork(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_Network_Identifier &network);
		static bool getFBN(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_FBN_Identifier &fbn);
		static bool getSysType(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_SystemType_Identifier &sysType);
		static bool getBladeProductNumber(const ACS_CS_Attribute *attribute, char ** bl);
		static bool getCurrentLoadModuleVersion(const ACS_CS_Attribute *attribute, char ** cm);
		static bool isIpAddress(const std::string ipAddress);
		static int GetHWType(); //This method gets the hardware variant from APGCC


	private:
		ACS_CS_Common_Util();
};


#endif /* ACS_CS_COMMON_UTIL_H_ */
