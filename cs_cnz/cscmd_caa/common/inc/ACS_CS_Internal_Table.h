/*
 * @file ACS_CS_Internal_Table.h
 * @author xminaon
 * @date Sep 16, 2010
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

#ifndef ACS_CS_Internal_Table_h_
#define ACS_CS_Internal_Table_h_

#include <string>
#include <vector>
#include "ACS_CS_API.h"

class ACS_CS_Attribute;
class ACS_CS_TableEntry;

namespace ACS_CS_INTERNAL_API
{
	enum CS_INTERNAL_API_TableType {
		Table_HWC,
		Table_CP,
		Table_NE,
		Table_FD,
		Table_AP,
		Table_CPGroupNames,
		Table_CPGroupMembers,
		Table_NotSpecified
	};

	typedef struct {
		std::string mimName;
		std::string mimVersion;
		std::string mimRelease;
		bool isMultipleCPSystem;
		std::string aptType;
		bool isTestEnvironment;
		uint32_t frontAp;
	} CS;

	typedef struct {
		std::string vlanName;
		std::string vlanAddress;
		std::string vlanNetmask;
		std::string vlanStack;
		uint16_t pcp;
		uint16_t vlanType;
		uint32_t vlanTag;
		std::string vNICName;
	} VLAN;

	typedef struct {
	    uint16_t entryId;
	    std::vector<ACS_CS_Attribute> attributes;
	} EntryAttributes_t;

	//Exit Codes
	enum CS_INTERNAL_API_ExitCode
	{
		Exit_Success =					0,
		Exit_Error_Executing =			1,
		Exit_Unable_Contact_Server =	117
	};
}

#endif
