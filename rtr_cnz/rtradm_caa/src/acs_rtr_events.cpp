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

#include "acs_rtr_events.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_global.h"
#include "acs_rtr_logger.h"

#include "acs_aeh_evreport.h"

#include <ace/ACE.h>
#include <sstream>

namespace {

		// 'problem cause' messages
		const char * PC_SRV_THREAD_ERROR =	"ACS_RTR, cannot start server thread";
		const char * PC_ACA_PARAMETER_ERROR = "ACS_RTR, failure at parameter reading";
		const char * PC_CREATE_FILE_ERROR =	"ACS_RTR, failure at creating file object";
		const char * PC_STATISTICS_ERROR =	"ACS_RTR, failure at statistics";
		const char * PC_MSD_CONNECTION_ERROR =	"ACS_RTR, connection to MSD failed";
		const char * PC_FREP_NOT_STARTED =	"ACS_RTR, cannot start thread";
		const char * PC_MS_READ_ERROR =	"ACS_RTR, MS reading failed";
		const char * PC_GOH_BLOCK_ERROR =	"ACS_RTR, interwork with GOH";
		const char * PC_CANNOT_CLOSE_BLD =	"ACS_RTR, cannot close BLD file";
		const char * PC_CANNOT_WRITE_BLD =	"ACS_RTR, cannot store in file";
		const char * PC_MS_SKIPPED =	"ACS_RTR, MS buffer state";
		const char * PC_MS_LOST = "ACS_RTR, MS buffer state";
		const char * PC_ATTACH_FAILED = "ACS_RTR, cannot attach to destination";
		const char * PC_EVENTS_CREATE_ERROR = "ACS_RTR, cannot create events";
		const char * PC_COMMIT_READ_ERROR = "ACS_RTR, failure retrieving committed information";
		const char * PC_RESTORING_FILES_ERROR = "ACS_RTR, fault at restoring files";
		const char * PC_SEND_FILE_ERROR = "ACS_RTR, cannot send file to destination";
		const char * PC_FILE_PROBLEMS =	"ACS_RTR, file problems";
		const char * PC_MOVE_FILE_FAILED =	"ACS_RTR, cannot move file";
		const char * PC_BREP_NOT_STARTED =	"ACS_RTR, cannot start thread";
		const char * PC_SEND_BLOCK_FAILED =	"ACS_RTR, cannot send block to destination";
		const char * PC_GOH_COMMIT_ERROR =	"ACS_RTR, interwork with GOH";
		const char * PC_GOH_END_ERROR =	"ACS_RTR, interwork with GOH";
		const char * PC_FIXED_REC_SIZE_ERROR =	"ACS_RTR, interwork with ACA";
		const char * PC_GOH_CONNECT_ERROR =	"ACS_RTR, cannot connect to GOH server";
		const char * PC_CRYPTOAPI_ERROR =	"ACS_RTR, problem with cryptoAPI";
		const char * PC_ACACHANNEL_NOT_STARTED =	"ACS_RTR, cannot start thread";
		const char * PC_CANNOT_START_RTRMANAGER =	"ACS_RTR, cannot start thread";
		const char * PC_DSDERROR =	"DSD error";
		const char * PC_MSIPERROR =	"MSIP error";
		const char * PC_CSERROR =	"CS error";
		const char * PC_GOH_TQ_NOT_DEFINED =	"ACS_RTR, interwork with GOH";
		const char * PC_INTERNAL_ERROR =	"ACS_RTR, internal error";
		const char * PC_CONFIGURATION_FAULT1 =	"ACS_RTR, configuration fault";
		const char * PC_CONFIGURATION_FAULT2 =	"ACS_RTR, configuration fault";
		const char * PC_DEF_PARAM_ERROR =	"ACS_RTR, failure getting parameter";
		const char * PC_TIMER_ERROR =	"ACS_RTR, timer problem";

		// 'problem data' messages
		const char * PD_SRV_THREAD_ERROR =	"Server thread failed";
		const char * PD_ACA_PARAMETER_ERROR = "ACA parameter retrieval";
		const char * PD_CREATE_FILE_ERROR =	"error in file creation";
		const char * PD_STATISTICS_ERROR =	"cannot create/initialize statistics file";
		const char * PD_MSD_CONNECTION_ERROR =	"error in 'connect()' method";
		const char * PD_FREP_NOT_STARTED =	"file report thread failed";
		const char * PD_MS_READ_ERROR =	"get message store failed";
		const char * PD_GOH_BLOCK_ERROR =	"error in block transfer mode";
		const char * PD_CANNOT_CLOSE_BLD =	"error in 'fileClose()'";
		const char * PD_CANNOT_WRITE_BLD =	"cannot store in BLD file";
		const char * PD_MS_SKIPPED =	"SKIPPED MS buffer encountered";
		const char * PD_MS_LOST =	"LOST MS buffer encountered";
		const char * PD_ATTACH_FAILED =	"error in 'attach()' method";
		const char * PD_EVENTS_CREATE_ERROR =	"error in 'makeHandles()' method";
		const char * PD_COMMIT_READ_ERROR =	"error in 'getLastCommittedRTRfileNumber()' method";
		const char * PD_RESTORING_FILES_ERROR =	"recovery procedure failed";
		const char * PD_SEND_FILE_ERROR =	"error in 'send()' method";
		const char * PD_FILE_PROBLEMS =	"existing RTR files";
		const char * PD_MOVE_FILE_FAILED =	"error in 'MoveFile()' method";
		const char * PD_BREP_NOT_STARTED =	"block report thread failed";
		const char * PD_SEND_BLOCK_FAILED =	"error in 'send()' method";
		const char * PD_GOH_COMMIT_ERROR =	"COMMIT failed";
		const char * PD_GOH_END_ERROR =	"END failed";
		const char * PD_FIXED_REC_SIZE_ERROR =	"problem with fixed record size";
		const char * PD_GOH_CONNECT_ERROR =	"error verifying the transfer queue";
		const char * PD_CRYPTOAPI_ERROR =	"";
		const char * PD_ACACHANNEL_NOT_STARTED =	"aca channel thread failed";
		const char * PD_CANNOT_START_RTRMANAGER =	"rtr manager object failed";
		const char * PD_DSDERROR =	"";
		const char * PD_MSIPERROR =	"";
		const char * PD_CSERROR =	"";
		const char * PD_GOH_TQ_NOT_DEFINED =	"transfer queue not defined";
		const char * PD_INTERNAL_ERROR =	"";
		const char * PD_CONFIGURATION_FAULT1 =	"error in 'setDirConfig(...)'";
		const char * PD_CONFIGURATION_FAULT2 =	"Directory problems";
		const char * PD_DEF_PARAM_ERROR =	"Default parameter file problems";
		const char * PD_TIMER_ERROR =	"";
}

namespace eventInfo
{
		const int TimeIntervalSec = 300; // Minimum time to report the same event again
		const long specificProblemOffset = 10300;
		const char severity[] = "EVENT";
		const char objReferenceClass[] = "APZ";
		const char objReference[] = "RTR SERVER";
}


ACS_RTR_TRACE_DEFINE(ACS_RTR_Events)
//------------
// Constructor
//------------
RTR_Events::RTR_Events()
 : m_Mutex(),
   m_EventReportObj(new (std::nothrow) acs_aeh_evreport()),
   m_timeReportedEvent()
{
	// assemble process name with pid
	pid_t processPid = getpid();
	std::stringstream procName;
	procName << RTR::PROCNAME << parseSymbol::colon << processPid << std::ends;
	m_ProcessName = procName.str();
}

//-----------
// Destructor
//-----------
RTR_Events::~RTR_Events()
{
	delete m_EventReportObj;
}

//---------------------
// reportEvent
//
// Report event to AEH.
// return value: -
//---------------------
void RTR_Events::reportEvent(eventType problemNo, const char* probText)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	ACS_RTR_TRACE_MESSAGE("In, problemNo:<%d>", problemNo);

	// only certain events should be blocked if they come to often.
	if (checkForFrequency(problemNo) && tooFrequent(problemNo))
	{
		ACS_RTR_TRACE_MESSAGE("event must not be again reported");
	}
	else
	{
		std::string probableCause;
		std::string problemDataFormat;

		getEventInfo(problemNo, probableCause, problemDataFormat);

		acs_aeh_specificProblem eventSpecificProblem = eventInfo::specificProblemOffset + problemNo;

		m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
										   eventSpecificProblem,
										   eventInfo::severity,
										   probableCause.c_str(),
										   eventInfo::objReferenceClass,
										   eventInfo::objReference,
										   problemDataFormat.c_str(),
										   probText
										  );
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//-------------------------------------------------------------------------
// tooFrequent
//
// Checks whether this particular event has been reported recently (within
// the last 300 seconds) and if so, suppress the event reporting. One tick
// of 'dwHighDateTime' is equal to roughly 300 seconds.
//
// return value:
//   false : not too frequent, go ahead and report event.
//   true  : suppress event.
//-------------------------------------------------------------------------
bool RTR_Events::tooFrequent(eventType problemNo)
{
	ACS_RTR_TRACE_MESSAGE("In, problemNo:<%d>", problemNo);
	bool result = false;

	// Get current time in seconds since January 1, 1970
	time_t currentTime = time(NULL);

	// find event from already reported event
	std::map<eventType, time_t>::iterator element = m_timeReportedEvent.find(problemNo);

	if(m_timeReportedEvent.end() != element)
	{
		// event present check time elapsed in sec
		double elapsedTime = difftime(currentTime, element->second);

		// check if the minimum time interval is elapsed from the latest report
		if( eventInfo::TimeIntervalSec > elapsedTime )
		{
			// no report again the event
			result = true;
		}
		else
		{
			// report the event
			// update the last reported time
			element->second = currentTime;
		}
		ACS_RTR_TRACE_MESSAGE("Elapsed <%g> sec from the last report time", elapsedTime);
	}
	else
	{
		// first time event reported
		// update the event reported map
		m_timeReportedEvent.insert(std::make_pair(problemNo, currentTime));
	}

	ACS_RTR_TRACE_MESSAGE("Out, report event <%s>", (result ? "YES":"NO"));
	return result;
}

//------------------------------------------------------------------------
// checkForFrequency
//
// Decides whether to check if this event should be reported or not.
// It's used to check if the event has been reported within the last 429 sec.
//
// return value:
//   true: the event must not to be reported if it happened within the last 429 sec.
//   false: the event must be reported always.
//------------------------------------------------------------------------
bool RTR_Events::checkForFrequency(int problemNo)
{
	switch (problemNo)
	{
		case(SEND_BLOCK_FAILED):
					return false;
		default:
			return true;
	}
}

//--------------------------------------------------------------------------------------
// getEventInfo
//
// Set the problem cause and the problem data format to use for the event at index 'idx'
//--------------------------------------------------------------------------------------
void RTR_Events::getEventInfo(int problemNo, std::string& probableCause, std::string& problemDataFormat)
{
	switch(problemNo)
	{
		case SRV_THREAD_ERROR:
			probableCause.assign(PC_SRV_THREAD_ERROR);
			problemDataFormat.assign(PD_SRV_THREAD_ERROR);
			break;
		case ACA_PARAMETER_ERROR:
			probableCause.assign(PC_ACA_PARAMETER_ERROR);
			problemDataFormat.assign(PD_ACA_PARAMETER_ERROR);
			break;
		case CREATE_FILE_ERROR:
			probableCause.assign(PC_CREATE_FILE_ERROR);
			problemDataFormat.assign(PD_CREATE_FILE_ERROR);
			break;
		case STATISTICS_ERROR:
			probableCause.assign(PC_STATISTICS_ERROR);
			problemDataFormat.assign(PD_STATISTICS_ERROR);
			break;
		case MSD_CONNECTION_ERROR:
			probableCause.assign(PC_MSD_CONNECTION_ERROR);
			problemDataFormat.assign(PD_MSD_CONNECTION_ERROR);
			break;
		case FREP_NOT_STARTED:
			probableCause.assign(PC_MSD_CONNECTION_ERROR);
			problemDataFormat.assign(PD_MSD_CONNECTION_ERROR);
			break;
		case MS_READ_ERROR:
			probableCause.assign(PC_MS_READ_ERROR);
			problemDataFormat.assign(PD_MS_READ_ERROR);
			break;
		case GOH_BLOCK_ERROR:
			probableCause.assign(PC_GOH_BLOCK_ERROR);
			problemDataFormat.assign(PD_GOH_BLOCK_ERROR);
			break;
		case CANNOT_CLOSE_BLD:
			probableCause.assign(PC_CANNOT_CLOSE_BLD);
			problemDataFormat.assign(PD_CANNOT_CLOSE_BLD);
			break;
		case CANNOT_WRITE_BLD:
			probableCause.assign(PC_CANNOT_WRITE_BLD);
			problemDataFormat.assign(PD_CANNOT_WRITE_BLD);
			break;
		case MS_SKIPPED:
			probableCause.assign(PC_MS_SKIPPED);
			problemDataFormat.assign(PD_MS_SKIPPED);
			break;
		case MS_LOST:
			probableCause.assign(PC_MS_LOST);
			problemDataFormat.assign(PD_MS_LOST);
			break;
		case ATTACH_FAILED:
			probableCause.assign(PC_ATTACH_FAILED);
			problemDataFormat.assign(PD_ATTACH_FAILED);
			break;
		case EVENTS_CREATE_ERROR:
			probableCause.assign(PC_EVENTS_CREATE_ERROR);
			problemDataFormat.assign(PD_EVENTS_CREATE_ERROR);
			break;
		case COMMIT_READ_ERROR:
			probableCause.assign(PC_COMMIT_READ_ERROR);
			problemDataFormat.assign(PD_COMMIT_READ_ERROR);
			break;
		case RESTORING_FILES_ERROR:
			probableCause.assign(PC_RESTORING_FILES_ERROR);
			problemDataFormat.assign(PD_RESTORING_FILES_ERROR);
			break;
		case SEND_FILE_ERROR:
			probableCause.assign(PC_SEND_FILE_ERROR);
			problemDataFormat.assign(PD_SEND_FILE_ERROR);
			break;
		case FILE_PROBLEMS:
			probableCause.assign(PC_FILE_PROBLEMS);
			problemDataFormat.assign(PD_FILE_PROBLEMS);
			break;
		case MOVE_FILE_FAILED:
			probableCause.assign(PC_MOVE_FILE_FAILED);
			problemDataFormat.assign(PD_MOVE_FILE_FAILED);
			break;
		case BREP_NOT_STARTED:
			probableCause.assign(PC_BREP_NOT_STARTED);
			problemDataFormat.assign(PD_BREP_NOT_STARTED);
			break;
		case SEND_BLOCK_FAILED:
			probableCause.assign(PC_SEND_BLOCK_FAILED);
			problemDataFormat.assign(PD_SEND_BLOCK_FAILED);
			break;
		case GOH_COMMIT_ERROR:
			probableCause.assign(PC_GOH_COMMIT_ERROR);
			problemDataFormat.assign(PD_GOH_COMMIT_ERROR);
			break;
		case GOH_END_ERROR:
			probableCause.assign(PC_GOH_END_ERROR);
			problemDataFormat.assign(PD_GOH_END_ERROR);
			break;
		case FIXED_REC_SIZE_ERROR:
			probableCause.assign(PC_FIXED_REC_SIZE_ERROR);
			problemDataFormat.assign(PD_FIXED_REC_SIZE_ERROR);
			break;
		case GOH_CONNECT_ERROR:
			probableCause.assign(PC_GOH_CONNECT_ERROR);
			problemDataFormat.assign(PD_GOH_CONNECT_ERROR);
			break;
		case CRYPTOAPI_ERROR:
			probableCause.assign(PC_CRYPTOAPI_ERROR);
			problemDataFormat.assign(PD_CRYPTOAPI_ERROR);
			break;
		case ACACHANNEL_NOT_STARTED:
			probableCause.assign(PC_ACACHANNEL_NOT_STARTED);
			problemDataFormat.assign(PD_ACACHANNEL_NOT_STARTED);
			break;
		case CANNOT_START_RTRMANAGER:
			probableCause.assign(PC_CANNOT_START_RTRMANAGER);
			problemDataFormat.assign(PD_CANNOT_START_RTRMANAGER);
			break;
		case DSDERROR:
			probableCause.assign(PC_DSDERROR);
			problemDataFormat.assign(PD_DSDERROR);
			break;
		case MSIPERROR:
			probableCause.assign(PC_MSIPERROR);
			problemDataFormat.assign(PD_MSIPERROR);
			break;
		case CSERROR:
			probableCause.assign(PC_CSERROR);
			problemDataFormat.assign(PD_CSERROR);
			break;
		case GOH_TQ_NOT_DEFINED:
			probableCause.assign(PC_GOH_TQ_NOT_DEFINED);
			problemDataFormat.assign(PD_GOH_TQ_NOT_DEFINED);
			break;
		case INTERNAL_ERROR:
			probableCause.assign(PC_INTERNAL_ERROR);
			problemDataFormat.assign(PD_INTERNAL_ERROR);
			break;
		case CONFIGURATION_FAULT1:
			probableCause.assign(PC_CONFIGURATION_FAULT1);
			problemDataFormat.assign(PD_CONFIGURATION_FAULT1);
			break;
		case CONFIGURATION_FAULT2:
			probableCause.assign(PC_CONFIGURATION_FAULT2);
			problemDataFormat.assign(PD_CONFIGURATION_FAULT2);
			break;
		case DEF_PARAM_ERROR:
			probableCause.assign(PC_DEF_PARAM_ERROR);
			problemDataFormat.assign(PD_DEF_PARAM_ERROR);
			break;
		case TIMER_ERROR:
			probableCause.assign(PC_TIMER_ERROR);
			problemDataFormat.assign(PD_TIMER_ERROR);
			break;
		default:
			probableCause.clear();
			problemDataFormat.clear();
			break;
	}
}


//--------------------------------------------------------------------------------------
// initAlarm
//
//--------------------------------------------------------------------------------------
void RTR_Events::initAlarm(const long specificProblem, const std::string& tqName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	ACS_RTR_TRACE_MESSAGE("In, raise alarm for TQ:<%s> problem:<%ld>", tqName.c_str(), specificProblem);
	ACS_AEH_ReturnType aehResult = ACS_AEH_error;

	if( alarmInfo::specificProblem1 == specificProblem)
	{
		// New alarm1 to raise
		aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
													alarmInfo::specificProblem1,
													alarmInfo::category,
													alarmInfo::probableCause,
													alarmInfo::objReferenceClass,
													tqName.c_str(),
													alarmInfo::problemData1,
													alarmInfo::problemText1,
													true); // Manual cease
	}
	else if( alarmInfo::specificProblem2 == specificProblem )
	{
		// New alarm2 to raise
		std::stringstream alarmText;
		alarmText << alarmInfo::problemText2part1 << tqName << "\n \n" << alarmInfo::problemText2part2 << std::ends;

		// raise the alarm1
		aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
													alarmInfo::specificProblem2,
													alarmInfo::category,
													alarmInfo::probableCause,
													alarmInfo::objReferenceClass,
													tqName.c_str(),
													alarmInfo::problemData2,
													alarmText.str().c_str(),
													true); // Manual cease

	}
	else
	{
		// Unknown specific problem
		ACS_RTR_TRACE_MESSAGE("Unknown specific problem:<%ld> for TQ <%s>", specificProblem, tqName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Unknown specific problem:<%ld> for TQ <%s>", specificProblem, tqName.c_str());
	}

	if(ACS_AEH_ok != aehResult)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to raise alarm for specific problem:<%ld> of TQ <%s>", specificProblem, tqName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to raise alarm for specific problem:<%ld> of TQ <%s>", specificProblem, tqName.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

//******************************************************************************
//
void RTR_Events::ceaseAlarm(const long specificProblem, const std::string& tqName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	ACS_RTR_TRACE_MESSAGE("In, cease alarm for TQ:<%s> problem:<%ld>", tqName.c_str(), specificProblem);
	ACS_AEH_ReturnType aehResult = ACS_AEH_error;

	if( alarmInfo::specificProblem1 == specificProblem)
	{
		// New alarm1 to raise
		aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
														alarmInfo::specificProblem1,
														alarmInfo::ceaseText,
														alarmInfo::probableCause,
														alarmInfo::objReferenceClass,
														tqName.c_str(),
														alarmInfo::problemData1,
														alarmInfo::problemText1 );
	}
	else if( alarmInfo::specificProblem2 == specificProblem )
	{
		// New alarm2 to raise
		std::stringstream alarmText;
		alarmText << alarmInfo::problemText2part1 << tqName << "\n \n" << alarmInfo::problemText2part2 << std::ends;

		// raise the alarm1
		aehResult = m_EventReportObj->sendEventMessage(m_ProcessName.c_str(),
														alarmInfo::specificProblem2,
														alarmInfo::ceaseText,
														alarmInfo::probableCause,
														alarmInfo::objReferenceClass,
														tqName.c_str(),
														alarmInfo::problemData2,
														alarmText.str().c_str()	);
	}
	else
	{
		// Unknown specific problem
		ACS_RTR_TRACE_MESSAGE("Unknown specific problem:<%ld> for TQ <%s>", specificProblem, tqName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Unknown specific problem:<%ld> for TQ <%s>", specificProblem, tqName.c_str());
	}

	if(ACS_AEH_ok != aehResult)
	{
		ACS_RTR_TRACE_MESSAGE("Failed to cease alarm for specific problem:<%ld> of TQ <%s>", specificProblem, tqName.c_str());
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Failed to cease alarm for specific problem:<%ld> of TQ <%s>", specificProblem, tqName.c_str());
	}

	ACS_RTR_TRACE_MESSAGE("Out");
}

