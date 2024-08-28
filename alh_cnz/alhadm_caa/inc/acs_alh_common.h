/*
 * acs_alh_common.h
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_COMMON_H_
#define ACS_ALH_COMMON_H_

#include <time.h>
#include <string.h>

//========================================================================================
//      Structures
//========================================================================================

struct apevent_mess{

	char processName[32];				// The process name
	long specificProblem;				// Problem reporting code
	char percSeverity[9];				// AXE alarm severity:
										// A1, A2, A3, O1, O2, CEASE, EVENT
	char probableCause[128];			// Slogan for the AXE oper.
	char objClassOfReference[8];		// AXE alarm reporting classes:
										// APZ, APT, PWR, EXT, EVENT
	char objectOfReference[64];			// Instance of the Object
										// class of reference
	char problemData[1024];				// Free text to be logged
	char problemText[1024];				// Free text to be printed
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
    time_t time;							// Time of storing into AL
};


// Stuff for ALCO communication must be declared
// as aggregates of chars for portability reasons
struct AlaIniStruct
{
    unsigned char apNode[2];
    unsigned char alarmId[3][2];
    unsigned char alarmClass;
    unsigned char alarmCategory;
    unsigned char bufSize[2];
    char buffer[1432];
};

const short sizeOfNonBufferAlaIni = 12;

struct AlaCsgStruct
{
    unsigned char apNode[2];
    unsigned char alarmId[3][2];
    unsigned char cpAlarmRef[2];
};

union AlaRecordStruct
{
    struct AlaIniStruct alaIni;
    struct AlaCsgStruct alaCsg;
};

struct ACS_ALH_AlarmMessage
{
	struct apevent_mess event;

	bool manualCease;
	time_t eventTime;
	char user[32];
	char priority[16];
	char nodeName[16];
	char nodeState[16];
	char eventType[16];
};

//--------------------------------------
//	Constants in event message record
//--------------------------------------

//	The allowed values for perceived severity field
const char STRING_A1[] =		"A1";
const char STRING_A2[] =		"A2";
const char STRING_A3[] =		"A3";
const char STRING_O1[] =		"O1";
const char STRING_O2[] =		"O2";
const char STRING_CEASING[] =	"CEASING";
const char STRING_EVENT[] =		"EVENT";



//========================================================================================
//      Constants
//========================================================================================
//const char ALH_FIFO_NAME[] = "/var/run/ap/alhfifo";
const char ALH_FIFO_NAME[] = "/var/run/ap/acs_alhfifo";
const char ALH_REBOOT_FILE[] = "/tmp/ALH_reboot";


//--------------------------------------
// Common values
//--------------------------------------

const unsigned int DEF_MESSIZE = 512;		// Default size of message buffers
const unsigned int DEF_AP_NODE_SIZE = 512;
const unsigned int DEF_LOGSIZE = 4096;		// Default size of log buffers
const unsigned int OFFSET_AP_NODE = 145;	// Offset for ApNode Number
const int NO_OF_CP_SIDES  = 2;				// CP specifics
const int EX_SIDE 		= 0;				// Buffer indexes
const int STANDBY_SIDE  = 1;

// LIMIT_FOR_CONNECT_ATTEMPT gives the number of loops in the main
// loop that shall be perfomed before a new attempt is performed
// to make a connection over JTP. (updated by uabsnm 970429)
const int LIMIT_FOR_CONNECT_ATTEMPT = 5;

const int CP_WORKING 		= 0;		// CP running in parallell mode
const int CP_SEPARATED 		= 1;		// CP running separated.
const int CP_UNDEFINED		= 2;		// CP not defined in cluster
const int CP_NOCONTACT   	= -1;		// Can not get CP state




//-------------------------------------------------------------
// Constant strings sent as problemText to CP.
// They should be general enough to be used in many situations.
//-------------------------------------------------------------

const char ACS_ALH_Text_Startup_problem[] =
	"Problem Startup";
const char ACS_ALH_Text_AlarmListMarginReached_problem[] =
	"Alarm List allmost filled";
const char ACS_ALH_Text_AlarmListFullAlarmLost_problem[] =
	"Alarm List overfilled, Alarm lost";
const char ACS_ALH_Text_EventLog_problem[] =
	"Problem with EventLog";
const char ACS_ALH_Text_JtpMgr_problem[] =
	"Problem communicating with JTP";
const char ACS_ALH_Text_EventTreating_problem[] =
	"Problem treating event";
const char ACS_ALH_Text_EventReading_problem[] =
	"Problem reading event";
const char ACS_ALH_Text_AlarmListWriting_problem[] =
	"Problem writing the alarm in AlarmList";
const char ACS_ALH_Text_AlarmListUpdating_problem[] =
	"Problem updating the alarm in AlarmList";
const char ACS_ALH_Text_Event_corrupted[] =
	"The Event read from syslog-ng is corrupted";

//--------------------------------------
// Error codes used by parts in ALH
// (eventMessage.specificProblem)
//--------------------------------------

const long ACS_ALH_EventLogInitializationError = 8305;
											// Problem during the initialization of the
											// EventLog handles
const long ACS_ALH_EventReadingError = 8306; //Problem during the reading of event from Syslog-ng



const long ACS_ALH_ServiceError = 8307;		// Problem in ALH Service
const long ACS_ALH_alarmListRemoveError	= 8311;
											// Problem removing item from Alarm List
const long ACS_ALH_alarmListMarginReachedWarning = 8312;
											// Alarm List Top margin reached but no alarm lost
const long ACS_ALH_alarmListOverFullError = 8313;
											// Alarm List over full and alarm lost
const long ACS_ALH_alarmListWriteError = 8324;
											// Problem writing Alarm List File
const long ACS_ALH_alarmListFileCreateError = 8325;
											// Problem creating Alarm List File
const long ACS_ALH_alarmHandTabSyntaxError = 8330;
											// Syntax error found in the Alarm handler Table DB
const long ACS_ALH_alarmHandTabMultipleError = 8331;
											// Two parameters with same specific Problem no
											// found in Alarm handler Table DB
const long ACS_ALH_alarmHandTabWrongSizeError = 8332;
											// Wrong Size found in the Alarm handler Table DB
const long ACS_ALH_JtpMgrDiscRecProblem = 8370;
											// Spontaneous JEXDISCREQ arrived from JTP
const long ACS_ALH_JtpMgrBadReturnCodeProblem = 8371;
											// Bad return code has arrived
const long ACS_ALH_EventCorrupted = 8391;
											// The Event read from syslog-ng is corrupted
const long ACS_ALH_RegRetrievalError = 8393;
											// Problem retrieving data from NT Registry
const long ACS_ALH_SetRegValError = 8394;	// Problem setting data in NT Registry
const long ACS_ALH_RetrievalError = 8395;	// Problem retrieving data

const long ACS_ALH_RetrievalDataFromIMMError = 8341;




#endif /* ACS_ALH_COMMON_H_ */
