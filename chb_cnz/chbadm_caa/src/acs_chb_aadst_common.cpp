/*
 * acs_chb_aadst_common.cpp
 *
 *  Created on: Jul 24, 2014
 *      Author: xassore
 */

/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ACS_CS_API.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_Util.H>

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <cctype>
#include <list>
#include "acs_chb_tra.h"
#include <acs_prc_api.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <grp.h> /* defines 'struct group', and getgrnam(). */
#include "ACS_DSD_Client.h"
#include "acs_chb_common.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_chb_aadst_common.h"

extern struct APtimeZones  TimeZones[];

using namespace std;



void acs_chb_aadst_common::timeZones_dump(ACE_UINT32 tz_num)
{
	for( ACE_UINT32 itr = 0 ; itr < tz_num ; itr++)
	{
		DEBUG(1,"TimeZones[%d] :   TZid = %d     TZname = %s", itr,TimeZones[itr].TZid, TimeZones[itr].TZname.c_str());
	}
}


int  acs_chb_aadst_common::takeAPtimeZones (ACE_UINT32 &tz_num)
{
	ACS_CC_ReturnType enResult;
	OmHandler omHandler;
	ACE_TCHAR tzName[NTZoneSize];
	ACE_INT32 tzId = -1;

	if (omHandler.Init() != ACS_CC_SUCCESS)
	{
		DEBUG(1,"%s", "ERRORE");
		return CHB_AADST_FAILURE;
	}

	std::vector<std::string> dn_list;

	// if (omHandler.getClassInstances(ACS_IMM_TZLINK_ROOT_CLASS_NAME, dn_list) != ACS_CC_SUCCESS)
	if (omHandler.getClassInstances("TimeZoneLink", dn_list) != ACS_CC_SUCCESS)
	{
		DEBUG(1,"%s", "ERRORE");
		omHandler.Finalize();
		return CHB_AADST_FAILURE;
	}
	DEBUG(1, "Found %d %s ", dn_list.size()," TimeZones in APG ");
//	tz_num = dn_list.size();
	tz_num =0;
	for( ACE_UINT32 itr = 0 ; itr < dn_list.size() && itr < MAX_NO_TIME_ZONES ; itr++)
	{
		tz_num ++;
		ACE_OS::memset(tzName, 0, sizeof(tzName));
		ACS_CC_ImmParameter paramToFind;
		//Set paramToFind
		paramToFind.attrName = (char*)TZ_ID;

		if( ( enResult = omHandler.getAttribute( dn_list[itr].c_str(), &paramToFind ) ) == ACS_CC_SUCCESS )
		{
			tzId = *((ACE_UINT32*)(*(paramToFind.attrValues)));
			TimeZones[itr].TZid = tzId;
		}
		else
		{
			//TBD error handling.
			DEBUG(1,"%s", "ERRORE getAttribute TZ_ID !");

		}
		paramToFind.attrName = (char*)TZ_STRING;

		if( ( enResult = omHandler.getAttribute(dn_list[itr].c_str(), &paramToFind ) ) == ACS_CC_SUCCESS )
		{
			ACE_OS::strcpy(tzName, (reinterpret_cast<char*>(*(paramToFind.attrValues))));
			//	tzName is the attribute 'timeZoneString' which value is something like
			//	"(GMT+01:00) Europe/Rome"
			ACE_TCHAR* pZoneName = 0;
			if( (pZoneName = ACE_OS::strstr(tzName,") ")) != 0 )
			{
//				pZoneName points to substring ") Europe/Rome"
//              pZoneName+2 points to "Europe/Rome"
				TimeZones[itr].TZname = pZoneName+2;

			}
			else {
				DEBUG(1,"%s", "ERRORE INCORRECT TzName !!");
				TimeZones[itr].TZname = "";
			}
		}
		else
		{
			DEBUG(1,"%s", "ERRORE getAttribute TZ_STRING !");
		}
	}
	omHandler.Finalize();
	//TimeZones_dump(dn_list.size());
	return CHB_AADST_SUCCESS;
}

int  acs_chb_aadst_common::getNodeState()
{
	int prcapi_res = -1;
	int node_state = AADST_NODE_STATE_UNDEFINED;  // UNDEFINED
	ACS_PRC_API prcObj;

	// Get the node state
	for (unsigned i = 0; (i <= 3) && (prcapi_res < 0); ++i) {
		prcapi_res = prcObj.askForNodeState();
		if (prcapi_res < 0) {
			ERROR(1, "Call 'askForNodeState' failed! loop == %u, call_result == %d", i, prcapi_res);
			::sleep(1);
		}else {
			node_state = (prcapi_res == ACS_PRC_NODE_ACTIVE)? AADST_NODE_STATE_ACTIVE: AADST_NODE_STATE_PASSIVE;
			//DEBUG (1,"ACS_CHB_aadst_common::getNodeState(): prcObj.askForNodeState() returns nodeState == %d", node_state );
		}
	}

	if (prcapi_res < 0) {
		ERROR(1,"%s", "Error in getting the local node state");
	}
	return node_state;
}

int  acs_chb_aadst_common::get_local_node_sysId(ACE_INT32 &sys_id)
{
	ACS_DSD_Client dsd_client;
	ACS_DSD_Node local_ap_node;

	if (const int dsd_error = dsd_client.get_local_node(local_ap_node)) { // ERROR: getting my AP node by DSD
			ERROR(1,"ACS_DSD_Client::get_local_node failed: cannot retrieve local AP node from DSD: DSD last error == %d", dsd_error);

			return CHB_AADST_FAILURE;
	}
	sys_id = local_ap_node.system_id;

	return CHB_AADST_SUCCESS;
}



/* ********************************************************** */
/******     Methods to get AADST parameters from IMM    ***** */
/* ********************************************************** */

int acs_chb_aadst_common::get_hbAADSTstart_attribute(ACE_INT32 & hbstart)
{
	int  rcode = CHB_AADST_SUCCESS;

	std::string dnObject("");
	dnObject.append(ACS_CHB_Common::dnOfHbeatRootObj);
	acs_apgcc_paramhandling pha;

	int attribute_value;
	ACS_CC_ReturnType imm_result = pha.getParameter( dnObject.c_str(), ACS_CHB_hbAADSTstart, &attribute_value );

	if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting attribute from IMM
			ERROR(1,"'acs_apgcc_paramhandling::getParameter()' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ACS_CHB_hbAADSTstart, pha.getInternalLastError(), pha.getInternalLastErrorText());
			return CHB_AADST_FAILURE;
		}

	hbstart = attribute_value;

	if( hbstart < ACS_CHB_AADSTSTART_MIN_VALUE || hbstart > ACS_CHB_AADSTSTART_MAX_VALUE)
	{
		DEBUG(1, "%s", "'acs_apgcc_paramhandling::getParameter()' get parameter value not in the range. The parameter value is changed to Default ");
		hbstart = 15;
	}
	return rcode;
} // End of getParameters


int acs_chb_aadst_common::get_hbAADSTdayRetry_attribute(ACE_UINT32 & hbretry)
{

	int  rcode = CHB_AADST_SUCCESS;

	std::string dnObject("");
	dnObject.append(ACS_CHB_Common::dnOfHbeatRootObj);
	acs_apgcc_paramhandling pha;

	unsigned int attribute_value;
	ACS_CC_ReturnType imm_result = pha.getParameter( dnObject.c_str(), ACS_CHB_hbAADSTdayRetry, &attribute_value );

	if (imm_result != ACS_CC_SUCCESS) { // ERROR: getting attribute from IMM
			ERROR(1,"'acs_apgcc_paramhandling::getParameter()' failed: cannot get the attribute '%s' from IMM: internal last error == %d, internal last error text  == %s",
					ACS_CHB_hbAADSTdayRetry, pha.getInternalLastError(), pha.getInternalLastErrorText());
			return CHB_AADST_FAILURE;
		}

	hbretry = attribute_value;

	return rcode;
} // End of getParameters


int acs_chb_aadst_common::set_hbAADSTdayRetry_attribute(ACE_UINT32 hbretry)
{
	int rcode = CHB_AADST_SUCCESS;

	OmHandler immHandle;
	ACS_CC_ReturnType res;
	res = immHandle.Init();
	if ( res != ACS_CC_SUCCESS )
	{
		ERROR(1,"OmHandler Init() ERROR: OmHandler Initialization FAILURE!!! AADST cannot update AADSTdayRetry  attribute <ret_code == %d", res);
		return CHB_AADST_FAILURE;
	}
	//void * attr_value [1] = { const_cast<void *>(reinterpret_cast<const void *>(&hbretry)) };
	void * attr_value [1] = { reinterpret_cast< void *>(&hbretry) };

	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = const_cast<char*>(ACS_CHB_hbAADSTdayRetry);
	paramToChange.attrType = ATTR_UINT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = attr_value;

	std::string dnObject("");
	dnObject.append(ACS_CHB_Common::dnOfHbeatRootObj);

	res = immHandle.modifyAttribute( dnObject.c_str(),&paramToChange);
	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"%s","OmHandler modifyAttribute - ERROR: Unable to modify 'hbAADSTdayRetry' attribute");
		rcode = CHB_AADST_FAILURE;
	}
	DEBUG(1,"acs_chb_aadst_common::set_hbAADSTdayRetry_attribute - parameter %s modified to %d ", ACS_CHB_hbAADSTdayRetry,hbretry);

	immHandle.Finalize();

	return rcode;
}


