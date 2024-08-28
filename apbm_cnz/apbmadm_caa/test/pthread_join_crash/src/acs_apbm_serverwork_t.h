#ifndef HEADER_GUARD_CLASS__acs_apbm_serverwork
#define HEADER_GUARD_CLASS__acs_apbm_serverwork acs_apbm_serverwork


#include "ace/Task.h"

//#include "acs_apbm_csreader.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_serverwork


class __CLASS_NAME__ : public ACE_Task_Base {
	//==============//
	// Constructors //
	//==============//
public:
	inline explicit __CLASS_NAME__ () : ACE_Task_Base(), _exit_from_svc_function(0) /*, _cs_reader()*/ {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	int work ();
	inline void stop_working () { _exit_from_svc_function = 1; }

	// BEGIN: ACE_Task_Base interface //
	virtual int svc ();
	// END: ACE_Task_Base interface //

private:
	const char * get_node_state () const;
//	void send_start_work_operation_by_node_state ();
//	void enter_reactor_event_loop (ACE_Reactor & reactor);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	int _exit_from_svc_function;
//	acs_apbm_csreader _cs_reader;
};
#endif // HEADER_GUARD_CLASS__acs_apbm_serverwork
