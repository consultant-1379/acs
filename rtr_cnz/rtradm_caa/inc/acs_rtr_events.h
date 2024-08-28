//********************************************************
//
// COPYRIGHT Ericsson AB 2014.
// All rights reserved.
//
// The Copyright to the computer program(s) herein
// is the property of Ericsson AB.
// The program(s) may be used and/or copied only with
// the written permission from Ericsson AB or in
// accordance with the terms and conditions stipulated in
// the agreement/contract under which the program(s) have
// been supplied.
//
//********************************************************


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_events_h
#define ACS_RTR_events_h

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Recursive_Thread_Mutex.h>

#include <string>
#include <map>

namespace alarmInfo
{
	const char probableCause[] = "DATA OUTPUT, AP TRANSMISSION FAULT";
	const char category[] = "A2";
	const char ceaseText[] = "CEASING";
	const char objReferenceClass[] = "APZ";

	const long specificProblem1 = 10346;
	const char problemData1[] = "Remote Server Unavailable";
	const char problemText1[] = "attach failed";

	const long specificProblem2 = 10399;
	const char problemData2[] = "CHARGING DESTINATION FAULT";

	const char problemText2part1[] = "CAUSE\nRECORD TRANSFER, DESTINATION\n \nFILE NAME\n-\n \nTRANSFER QUEUE\ntransferQueueId=";
	const char problemText2part2[] = "DESTINATION SET\n-\n \nDESTINATION\n-\n";
}

/*=====================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
enum eventType {
	// Specific return codes for RTR
	SRV_THREAD_ERROR =	0,	// server thread failed
	ACA_PARAMETER_ERROR =	1,	// failure at aca parameter reading
	CREATE_FILE_ERROR	=	2,	// failure at creating file object
	STATISTICS_ERROR =	3,	// cannot initialize statistics file
	MSD_CONNECTION_ERROR =	4,	// connection to MSD failed
	FREP_NOT_STARTED =	5,	// cannot start thread file report thread
	MS_READ_ERROR =	6,	// MS reading failed
	GOH_BLOCK_ERROR =	7,	// interwork with GOH block transfer mode error
	CANNOT_CLOSE_BLD =	8,	// cannot close BLD file
	CANNOT_WRITE_BLD =	9,  // cannot store in BLD file
	MS_SKIPPED =	10, // SKIPPED MS buffer encountered
	MS_LOST =	11, // LOST MS buffer encountered
	ATTACH_FAILED =	12, // cannot attach to destination
	EVENTS_CREATE_ERROR =	13, // cannot create windows events
	COMMIT_READ_ERROR =	14,	// failure getting committed information
	RESTORING_FILES_ERROR =	15, // fault at restoring files
	SEND_FILE_ERROR =	16, // cannot send file to destination
	FILE_PROBLEMS =	17, // file problems
	MOVE_FILE_FAILED =	18,	// cannot move file
	BREP_NOT_STARTED =	19,	// cannot start thread block report thread
	SEND_BLOCK_FAILED =	20, // cannot send block to destination
	GOH_COMMIT_ERROR =	21, // interwork with GOH transactionCOMMIT failed
	GOH_END_ERROR	=	22, // interwork with GOH transactionEND failed
	FIXED_REC_SIZE_ERROR =	23,	// fixed record size error
	GOH_CONNECT_ERROR =	24,	// cannot connect to GOH server
	CRYPTOAPI_ERROR =	25, // problem with cryptoAPI
	ACACHANNEL_NOT_STARTED =	26,	// cannot start aca channel thread
	CANNOT_START_RTRMANAGER =	27,	// cannot start rtr manager object
	DSDERROR =	28,	// DSD Error
	MSIPERROR =	29,	// MSIP Error
	CSERROR	=	30,	// CS ERROR
	GOH_TQ_NOT_DEFINED =	31,	// transfer queue not defined
	INTERNAL_ERROR =	32,	// internal error
	CONFIGURATION_FAULT1 =	33, // setDirConfig(msName,siteName) error
	CONFIGURATION_FAULT2 =	34, // Directory problems
	DEF_PARAM_ERROR =	35, // failure getting parameter

	TIMER_ERROR =	90 // timer problem
};



class acs_aeh_evreport;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class RTR_Events
{
 public:

	/**
	   @brief    Constructor for RTR_Events class.
	   @param    proc
	   @param    suppressAlarm
	*/
	RTR_Events();

    virtual ~RTR_Events();

	/**
	   @brief    reportEvent
	   @param    problemNo
	   @param    errstr
	   @param    void
	*/
    void reportEvent(eventType problemNo, const char* probText);

	void initAlarm(const long specificProblem, const std::string& tqName);

	void ceaseAlarm(const long specificProblem, const std::string& tqName);

 private:

	/**
	   @brief    getEventInfo
	   @param    idx
	   @param    probableCause
	   @param    problemDataFormat
	   @param    void
	*/
	void getEventInfo(int idx, std::string& probableCause, std::string& problemDataFormat);

	/**
	   @brief    tooFrequent
	   @param    problemNo
	   @param    bool
	*/
	bool tooFrequent(eventType problemNo);

	/**
	   @brief    checkForFrequency
	   @param    problemNo
	   @param    bool
	*/
	bool checkForFrequency(int problemNo);

	/**
	 * 	@brief	m_mutex
	 *
	 * 	Mutex for internal map access
	*/
	ACE_Recursive_Thread_Mutex m_Mutex;

	/**
	 * 	@brief	m_EventReportObj
	 *
	*/
	acs_aeh_evreport* m_EventReportObj;

	/**
	 *    @brief    m_timeReportedEvent
	 *	  Map of all reported event and their reporting time
	*/
	std::map<eventType, time_t> m_timeReportedEvent;

	/**
	 * 	@brief	m_ProcessName
	*/
	std::string m_ProcessName;

};

#endif
