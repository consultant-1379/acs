/*
 * acs_alh_common.h
 *
 *  Created on: Dec 01, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_COMMON_H_
#define ACS_ALH_COMMON_H_

#include <time.h>
#include <string.h>
#include <acs_aeh_types.h>

//========================================================================================
//      Structures
//========================================================================================

struct apevent_mess{

	char processName[32];				// The process name
	long specificProblem;				// Problem reporting code
	char percSeverity[8];				// AXE alarm severity:
										// A1, A2, A3, O1, O2, CEASE, EVENT
	char probableCause[128];			// Slogan for the AXE oper.
	char objClassOfReference[8];		// AXE alarm reporting classes:
										// APZ, APT, PWR, EXT, EVENT
	char objectOfReference[64];			// Instance of the Object
										// class of reference
	char problemData[1024];				// Free text to be logged
	char problemText[1024];				// Free text to be printed
	char time[20];						// Time of storing into AL
};



struct AllRecord
{
    struct apevent_mess event;				// The stuff from Alarm Queue
    unsigned short identity[3];				// identifies instant of alarm
											// [0]: Specific Problem (type)
											// [2]: Progressive Nr (instance)
    unsigned short cpAlarmRef;				// Reference from the CP
    unsigned short retransCnt;				// Retransmission counter
	bool ceasePending;						// Only for alarms. true if a cease item for
											// this alarm has been stored in the alarm list,
											// false otherwise
	bool manualCease;						// true if manual ceasing of the alarm is allowed,
											// false otherwise
	unsigned short sendPriority;			// [0..5] [HIGH->LOW]
											// priority in which to send this
											// this item to CP
    bool acknowledge;						// true if this item has been acknowledged
											// by ALCO, false otherwise
	char dummy;								// Not used
};

//--------------------------------------
// Common values
//--------------------------------------

const unsigned int DEF_MESSIZE = 512;		// Default size of message buffers
const unsigned int DEF_AP_NODE_SIZE = 512;
const unsigned int DEF_LOGSIZE = 4096;		// Default size of log buffers

#ifndef ALHD_IMM_PREFIX
#define ALHD_IMM_PREFIX  "ALHD"
#endif



#ifndef ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_RDN_SIZE_MAX 	256
#endif

#ifndef ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_PROCESS_NAME_SIZE_MAX 	32
#endif

#ifndef ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_PERC_SEVERITY_SIZE_MAX 	8
#endif

#ifndef ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_PROBABLE_CAUSE_SIZE_MAX	128
#endif

#ifndef ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_CATEGORY_SIZE_MAX	8
#endif

#ifndef ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_OBJECT_OF_REF_SIZE_MAX	64
#endif

#ifndef ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_PROBLEM_DATA_SIZE_MAX	1024
#endif

#ifndef ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_PROBLEM_TEXT_SIZE_MAX	1024
#endif

#ifndef ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX
#define ACS_ALH_CONFIG_IMM_TIME_SIZE_MAX		20
#endif


//===================================================================================================================
// IMM AlarmHandler CLASS DEFINITION
//===================================================================================================================

#ifndef ACS_ALH_CONFIG_IMM_ALH_CLASS_NAME
#define ACS_ALH_CONFIG_IMM_ALH_CLASS_NAME "AlarmHandler"
#endif


//===================================================================================================================
// IMM AlarmConfigurationTable CLASS DEFINITION
//===================================================================================================================

#ifndef ACS_ALH_CONFIG_IMM_ALARMCONFIGURATIONTABLE_CLASS_NAME
#define ACS_ALH_CONFIG_IMM_ALARMCONFIGURATIONTABLE_CLASS_NAME "AlarmConfigurationTable"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_SPECIFIC_PROBLEM
#define ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_SPECIFIC_PROBLEM "specificProblem"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBLEM_TEXT
#define ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBLEM_TEXT "problemText"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBABLE_CAUSE
#define ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBABLE_CAUSE "probableCause"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PERCEIVED_SEVERITY
#define ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PERCEIVED_SEVERITY "perceivedSeverity"
#endif

#ifndef ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_RDN
#define ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_RDN "objectId"
#endif


//===================================================================================================================
// IMM AlarmListNode CLASS DEFINITION
//===================================================================================================================

#ifndef ACS_ALH_CONFIG_IMM_ALARM_LIST_NODE_CLASS_NAME
#define ACS_ALH_CONFIG_IMM_ALARM_LIST_NODE_CLASS_NAME "AlarmListNode"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN
#define ACS_ALH_CONFIG_IMM_ALN_ATTR_NAME_RDN "alarmListNodeId"
#endif


//===================================================================================================================
// IMM AlarmInfo CLASS DEFINITION
//===================================================================================================================

#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_CLASS_NAME
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_CLASS_NAME "AlarmInfo"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_NAME_RDN
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_NAME_RDN "identityId"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROCESS_NAME
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROCESS_NAME "processName"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SPEC_PROBL
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SPEC_PROBL "specificProblem"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBL_CAUSE
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBL_CAUSE "probableCause"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CATEGORY
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CATEGORY "category"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_OBJ_REF
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_OBJ_REF "objectOfReference"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_DATA
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_DATA "problemData"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_TEXT
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_PROBLEM_TEXT "problemText"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CP_AL_REF "cpAlarmReference"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_RETR_COUNTER "retrasmissionCounter"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_CEASE_PENDING "ceasePending"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SEND_PRIORITY
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_SEND_PRIORITY "sendPriority"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_ACKNOWLEDGE "acknowledge"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_TIME
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_ATTR_TIME "time"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_SEVERITY
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_SEVERITY "severity"
#endif


#ifndef ACS_ALH_CONFIG_IMM_ALARM_INFO_MANUAL_CEASE
#define ACS_ALH_CONFIG_IMM_ALARM_INFO_MANUAL_CEASE "manualCease"
#endif




#ifndef MAX_BUF_SIZE_ALMSG
#define MAX_BUF_SIZE_ALMSG	8192
#endif


#ifndef ACS_ALH_DAEMON_NAME
#define ACS_ALH_DAEMON_NAME 	"acs_dsdd"
#endif

#ifndef ACS_ALH_HA_USER
#define ACS_ALH_HA_USER 	"root"
#endif

#ifndef ALHD_LOCKFILE_PATH
#define ALHD_LOCKFILE_PATH  "/var/run/ap/acs_alhd.lck"
#endif


namespace acs_alh {
enum ErrorConstants {
		ERR_NO_ERRORS												=	0,		///< ERR_NO_ERRORS description (check errno)
		ERR_SYSTEM_ERROR											=	-1,		///< ERR_SYSTEM_ERROR description (check errno)
		ERR_SYSTEM_OPEN												=	-2,		///< ERR_SYSTEM_OPEN description (check errno)
		ERR_SYSTEM_ACCEPT											=	-3,		///< ERR_SYSTEM_ACCEPT description (check errno)
		ERR_SYSTEM_CLOSE											=	-4,		///< ERR_SYSTEM_CLOSE description (check errno)
		ERR_SYSTEM_REMOVE											=	-5,		///< ERR_SYSTEM_REMOVE description (check errno)
		ERR_SYSTEM_GET_ADDRESS										=	-6,		///< ERR_SYSTEM_GET_ADDRESS description (check errno)
		ERR_SYSTEM_CONNECT											=	-7,		///< ERR_SYSTEM_CONNECT description (check errno)
		ERR_SYSTEM_RECEIVE											=	-8,		///< ERR_SYSTEM_RECEIVE description (check errno)
		ERR_SYSTEM_SEND												=	-9,		///< ERR_SYSTEM_SEND description (check errno)
		ERR_SYSTEM_GET_OPTION										=	-10,	///< ERR_SYSTEM_GET_OPTION description (check errno)
		ERR_SYSTEM_SET_OPTION										=	-11,	///< ERR_SYSTEM_SET_OPTION description (check errno)
		ERR_SYSTEM_OPEN_DIRECTORY									=	-12,	///< ERR_SYSTEM_OPEN_DIRECTORY description (check errno)
		ERR_SYSTEM_OUT_OF_MEMORY									=	-13,	///< ERR_SYSTEM_OUT_OF_MEMORY description (check errno)
		ERR_SYSTEM_READ												=	-14,	///< ERR_SYSTEM_READ description (check errno)
		ERR_SYSTEM_CREATE											=	-15,	///< ERR_SYSTEM_CREATE description (check errno)
		ERR_SYSTEM_EPOLL_CTL_ADD									=	-16,	///< ERR_SYSTEM_EPOLL_CTL_ADD description (check errno)
		ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE							=	-17,	///< ERR_SYSTEM_IO_MULTIPLEXING_ACTIVATE description (check errno)
		ERR_SYSTEM_EPOLL_WAIT										=	-18,	///< ERR_SYSTEM_EPOLL_WAIT description (check errno)
		ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE						=	-19,	///< ERR_SYSTEM_SYNC_LOCK_ACQUIRE_FAILURE description (check errno)

		ERR_INPUT_PARAMETER_EMPTY									= 	-20,

		ERR_SAF_IMM_OM_INIT											=	-51,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_OM_GET											=	-52,	///< ERR_SAF_IMM_OM_GET description
		ERR_SAF_IMM_OM_CREATE										=	-53,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_OM_DELETE										=	-54,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_OM_MODIFY										=	-55,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_OM_NO_OBJECT_FOUND								=	-56,	///< ERR_SAF_IMM_OM_INIT description
		ERR_SAF_IMM_NULL_ATTRIBUTE_VALUE							=  	-57,
		ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE							=	-64	///< ERR_SAF_IMM_EMPTY_ATTRIBUTE_VALUE description
	};
}


#endif /* ACS_ALH_COMMON_H_ */
