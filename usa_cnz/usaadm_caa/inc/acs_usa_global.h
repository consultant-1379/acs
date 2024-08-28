#ifndef ACS_USA_GLOBAL_H
#define ACS_USA_GLOBAL_H

#include <time.h>
#include <ace/Basic_Types.h>
#include <ace/Manual_Event.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Singleton.h>
#include <ACS_CS_API.h>

#define RELATIVETIME(x)    (ACE_OS::time(NULL) + x)

class ACE_event_t;
class ACE_Message_Block;
class ACE_Time_Value;
class ACE_Reactor;
class ACE_TP_Reactor;
class ACE_Reactor;
class ACS_USA_ZombieMon;
class ACS_USA_CpuLdAvg;
class ACS_USA_SLA;
class ACS_USA_APRebootMon;
class ACS_USA_NtpSrvM;
class ACS_USA_NtfNotificationThread;
class ACS_USA_AdhIntUserObserver;

using namespace std;

//----------------------------------------------------------------------------
// Lets make Global a Singleton
//----------------------------------------------------------------------------
class Global;
typedef ACE_Singleton<Global, ACE_Recursive_Thread_Mutex> ACS_USA_Global;


//----------------------------------------------------------------------------
class Global
{
public:
	//Ignore sshd events flag
	bool m_ignrSshdEvents;

	// Global Constants
	enum 
	{
		MAX_TASK_IDENTIFIERS = 0xffff
	};
	
   enum MCS_ADH_Result
	{
		Result_Success =			0,
		Result_NoEntry =			-1,
		Result_NoValue =			-2,
		Result_NoAccess =			-3,
		Result_Failure =			-4
	};

	Global();

	virtual ~Global();

	void deactivate(); // call to deactivate object

	//=== Access methods ===
	bool isVirtual();
	void getEnvironment();
	
	bool implementer_set();
	void implementer_set(bool flag);

        bool adh_implementer_set();
        void adh_implementer_set(bool flag);

	bool shutdown_ordered();
	void shutdown_ordered(bool set);
	// Get/Set if the service is ordered to shutdown

	//HANDLE
	ACE_event_t shutdown_event();
	// Returns handle to global shutdown event

	bool shutdown_received();
	// Returns true if shutdown has occurred, otherwise false


	int get_next_msg_id();
	// Get next valid messages id


   	ACE_Reactor* reactor();

	void notifyGlobalShutdown();
	// Send a shutdown message to all registred handlers

	time_t* get_service_start_time();
	// Returns the start time for the service

	ACE_UINT32 get_service_uptime();
	// Returns the uptime for the service in minutes
	ACS_USA_ZombieMon* ZombieMon();
	ACS_USA_CpuLdAvg* CpuLdAvg();
	ACS_USA_SLA* Analyser();
	ACS_USA_APRebootMon* APRebootMon();
	ACS_USA_NtpSrvM* NtpSrvM();
	ACS_USA_NtfNotificationThread* NtfThread();
        ACS_USA_AdhIntUserObserver* AdhIntThread();

private:

	// Internal Constants
	enum 
	{
		MAX_MESSAGE_IDS      = 0xffff,
	};


	void reset(); // Call to reset state to initial state.


	//Internal variables
	//-------------------
	
	bool m_shutdownOrdered;
	// True if shutdown has been ordered 

	bool m_isVirtual;
	//True if the node is virtual

	bool m_implementerSet;
	bool m_adhImplementerSet;

	int m_messageId;
	// Unique id for messages sent between ADH's Tasks
	
	
   	ACE_TP_Reactor* m_reactorImpl;
   	ACE_Reactor* m_reactor;

	ACS_USA_ZombieMon* m_zMonObj;
	ACS_USA_CpuLdAvg* m_ldAvgObj;
	ACS_USA_SLA* m_slaObj;
	ACS_USA_APRebootMon* m_aprObj;
	ACS_USA_NtpSrvM* m_ntpSrvObj;
	ACS_USA_NtfNotificationThread* m_ntfObj; 
        ACS_USA_AdhIntUserObserver* m_adhIntObj;

	// Shutdown event
	ACE_Manual_Event m_eventShutdown;
	
	// Serialize access
	ACE_Recursive_Thread_Mutex thread_lock_;

	static ACE_Recursive_Thread_Mutex varLock_;

};


#endif //ACS_USA_GLOBAL_H
