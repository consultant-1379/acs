

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_EventReporter_h
#define ACS_CS_EventReporter_h 1



#include <ace/Singleton.h>

#include <string>
#include <set>
#include "acs_aeh_evreport.h"




#define ACS_CS_EVENT ACS_CS_EventReporter::instance()->report
#define ACS_CS_CEASE ACS_CS_EventReporter::instance()->cease

namespace ACS_CS_Event_NS
{
    enum EventCodes
    {
        BASE_ACS_CS_OMProfile           = 3000,                       // Event numbers for OM profile events, 3000 - 3099
        Event_ProfileChangeEvent        = BASE_ACS_CS_OMProfile + 1,  // OmProfile Phase Change Notification Events
        Event_ProfileChangeFailure      = BASE_ACS_CS_OMProfile + 2,  // OmProfile Phase Change Failure Event
        Event_ClusterOpModeExpert       = BASE_ACS_CS_OMProfile + 3,  // ClusterOpMode Expert


        BASE_ACS_CS                     = 10600,             // Event numbers for MODD, 10600 - 10699
        Event_NonCritical               = BASE_ACS_CS,       // Non critcal anomaly in the software.
        Event_GeneralFailure            = BASE_ACS_CS + 1,   // A general critical error
        Event_ProgrammingError          = BASE_ACS_CS + 2,   // A logcial error in the application code.
        Event_StartupFailure            = BASE_ACS_CS + 3,   // An error in the service startup.
        Event_SubSystemError            = BASE_ACS_CS + 4,   // Problem with general Windows functions.
        Event_IOFailure                 = BASE_ACS_CS + 5,   // Problems with disk I/O
        Event_SocketAPIFailure          = BASE_ACS_CS + 6,   // Problems reported by Socket API
        Event_DHCPAPIFailure            = BASE_ACS_CS + 7,   // Problems reported by DHCP API
        Event_SNMPAPIFailure            = BASE_ACS_CS + 8,   // Problems reported by SNMP API
        Event_RegisterAPIFailure        = BASE_ACS_CS + 9,   // Problems reported by Windows Register API
        Event_APAPIFailure              = BASE_ACS_CS + 10,  // Problems reported by the AP API
        Event_AddressAssignmentFailure  = BASE_ACS_CS + 11   // Problems assigning addresses for boards
    };
}

using namespace ACS_CS_Event_NS;



class ACS_CS_EventReporter 
{
public:

	typedef enum
	{
		Severity_A1,
		Severity_A2,
		Severity_A3,
		Severity_O1,
		Severity_O2,
		Severity_Event
	} Severity;


public:

	virtual ~ACS_CS_EventReporter();


	void report(int event, Severity severity, const std::string &probableCause, const std::string &data, const std::string &text, std::string procName = "");

	void cease(int event, std::string procName = "");

	void resetAllEvents();

	static ACS_CS_EventReporter * instance();


private:

	ACS_CS_EventReporter();

	ACS_CS_EventReporter(const ACS_CS_EventReporter &right);

	ACS_CS_EventReporter & operator=(const ACS_CS_EventReporter &right);

	void severityToStr(Severity severity, std::string *out);

	int getProcIdByName(std::string procName);


private:

	acs_aeh_evreport eventReporter;

	std::set<int> sentEvents;

	typedef ACE_Singleton<ACS_CS_EventReporter, ACE_Recursive_Thread_Mutex> instance_;

	friend class ACE_Singleton<ACS_CS_EventReporter, ACE_Recursive_Thread_Mutex>;
};


// Class ACS_CS_EventReporter 



#endif
