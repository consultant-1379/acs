#ifndef ACS_ASEC_GLOBAL_H
#define ACS_ASEC_GLOBAL_H

#include <ace/Task.h>
#include <time.h>
#include <ace/Basic_Types.h>
#include <ace/Manual_Event.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Singleton.h>
#include <ace/Monotonic_Time_Policy.h>

#define RELATIVETIME(x)    (ACE_OS::time(NULL) + x)

class ACE_event_t;
class ACE_Message_Block;
class ACE_Time_Value;
class ACE_Reactor;
class ACE_TP_Reactor;
class ACE_Reactor;
class ACS_ASEC_DSDSrv;
class ACS_ASEC_ObjImpl;
class ACS_ASEC_ImmHandler;


//class ACE_Task;

typedef ACE_Task< ACE_MT_SYNCH, ACE_Monotonic_Time_Policy > AceTask_AsecTask;

using namespace std;

//----------------------------------------------------------------------------
// Lets make Global a Singleton
//----------------------------------------------------------------------------
class Global;
typedef ACE_Singleton<Global, ACE_Recursive_Thread_Mutex> ACS_ASEC_Global;


//----------------------------------------------------------------------------
class Global
{
public:

	
	Global();

	virtual ~Global();

	void deactivate(); // call to deactivate object

	//=== Access methods ===

	bool shutdown_ordered();
	void shutdown_ordered(bool set);
	// Get/Set if the service is ordered to shutdown

	//HANDLE
	ACE_event_t shutdown_event();
	// Returns handle to global shutdown event

	bool shutdown_received();
	// Returns true if shutdown has occurred, otherwise false


   	ACE_Reactor* reactor();

	void notifyGlobalShutdown();
	// Send a shutdown message to all registred handlers

	// Returns the uptime for the service in minutes
	ACS_ASEC_ObjImpl* asecImpl();
	ACS_ASEC_DSDSrv* asecDsd();
	ACS_ASEC_ImmHandler* immHandler();

private:

	// Internal Constants
	static ACE_Recursive_Thread_Mutex varLock_;

	bool m_shutdownOrdered;

	ACE_Manual_Event m_eventShutdown;
	// Shutdown event
	
   	ACE_TP_Reactor* m_reactorImpl;
   	ACE_Reactor* m_reactor;
	ACS_ASEC_ObjImpl* m_ObjImpl;
	ACS_ASEC_DSDSrv* m_dsdObj;
	/**
		@brief	Handler to IMM objects
	*/
	ACS_ASEC_ImmHandler* m_immHandler;


	ACE_Recursive_Thread_Mutex thread_lock_;
	// Serialize access

};


#endif // ACS_ASEC_GLOBAL_H
