/*
 * acs_chb_aadst_common.h
 *
 *  Created on: Jul 24, 2014
 *      Author: xassore
 */

#ifndef ACS_CHB_AADST_COMMON_H_
#define ACS_CHB_AADST_COMMON_H_
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */


/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

#define AP1_SYSID       	2001
#define AP2_SYSID       	2002

/*=================================================================== */
/**
   @brief ACS_CHB_hbAADSTdayRetry
		  This IMM parameter represents the number of retry of CASTC command
 */
/*=================================================================== */
const char
ACS_CHB_hbAADSTdayRetry [] = "heartBeatAADSTdayRetry";
/*=================================================================== */
/**
   @brief ACS_CHB_hbAADSTdayRetry
		 This IMM parameter states the number of days before starting of the DST update
 */
/*=================================================================== */
const char
ACS_CHB_hbAADSTstart[] = "heartBeatAADTSstart";

#define ACS_CHB_AADSTSTART_MAX_VALUE   20
#define ACS_CHB_AADSTSTART_MIN_VALUE  -1

#define IMM_ATTR_NAME_MAX_SIZE 128

#define  SECONDS_IN_A_DAY  		86400
#define  MAX_NO_TIME_ZONES  	24
#define  TZ_ID					"timeZoneId"
#define  TZ_STRING				"timeZoneString"

#define ACS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

static const int   NTZoneSize = 200;

enum NodeStateConstants {
		AADST_NODE_STATE_UNDEFINED = 0,
		AADST_NODE_STATE_ACTIVE = 1,
		AADST_NODE_STATE_PASSIVE = 2
	};

struct  APtimeZones
{
		int       TZid;
		string    TZname;
};

// return codes
	enum aadst_common_rc{
		CHB_AADST_SUCCESS = 0,
		CHB_AADST_FAILURE = 1
	};

class  acs_chb_aadst_common
{
public:

	inline acs_chb_aadst_common(){};
	inline ~acs_chb_aadst_common(){};

	static int  getNodeState();
	static void  timeZones_dump (ACE_UINT32 tz_num);
	static int   takeAPtimeZones (ACE_UINT32 &tz_num);

	static int set_hbAADSTdayRetry_attribute(ACE_UINT32 hbretry);
	static int get_hbAADSTdayRetry_attribute(ACE_UINT32 & hbretry);
	static int get_hbAADSTstart_attribute(ACE_INT32 & hbstart);
	static int get_local_node_sysId(ACE_INT32 & sys_id);
};

#endif /* ACS_CHB_AADST_COMMON_H_ */
