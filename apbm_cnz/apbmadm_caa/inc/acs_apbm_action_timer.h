#ifndef ACS_APBM_ACTION_TIMER_H_
#define ACS_APBM_ACTION_TIMER_H_

#include <ace/Event_Handler.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include "acs_apbm_logger.h"
#include <time.h>
#include <ace/Thread.h>
#include "acs_apbm_serverworkingset.h"
#include "acs_apbm_immrepositoryhandler.h"
#include "acs_apbm_logger.h"

class acs_apbm_serverworkingset;

class acs_apbm_action_timer: ACE_Event_Handler
{
public:
	enum check_action_result
	{
		CHECK_ACTION_COMPLETED = 1,
		CHECK_ACTION_PENDING = 0,
		CHECK_ACTION_GET_IPMI_STATUS_ERROR = -1,
		CHECK_ACTION_UPDATE_IPMI_STATUS_ERROR = -2,
		CHECK_ACTION_GET_IPMI_DATA_ERROR = -3,
		CHECK_ACTION_UPDATE_IPMI_DATA_ERROR = -4,
		CHECK_ACTION_INTERNAL_ERROR = -5
	};

public:
	static const uint32_t IPMIUPGRADE_TIMER_DELAY = 1;
	static const uint32_t IPMIUPGRADE_TIMER_INTERVAL = 60;//300;  // seconds

private:
	// Constructor is declared private to avoid direct object creation.
	// The instances of this class must be created invoking 'make_instance()' method.
	acs_apbm_action_timer(uint32_t delay, uint32_t interval, int32_t slot_id, uint32_t magazine_id, ACS_APGCC_AdminOperationIdType operation_id, ACS_APGCC_InvocationType invocation_id, acs_apbm_serverworkingset * server_working_set);

public:
	virtual ~acs_apbm_action_timer() {};

	// creates a class instance on the HEAP and returns it to the caller
	static acs_apbm_action_timer  * make_instance(uint32_t delay, uint32_t interval, int32_t slot_id, uint32_t magazine_id, ACS_APGCC_AdminOperationIdType operation_id, ACS_APGCC_InvocationType invocation_id, acs_apbm_serverworkingset * server_working_set)
	{
		return new (std::nothrow) acs_apbm_action_timer(delay, interval, slot_id, magazine_id, operation_id, invocation_id, server_working_set);
	}

	// CALLBACK invoked when timer is activated and a tick occurs
	virtual int handle_timeout ( const ACE_Time_Value &  current_time,  const void * act);

	// CALLBACK invoked when timer is deactivated
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	// activates the timer registering it with the APBM reactor
	int activate();

	// deactivates the timer canceling it from APBM main reactor
	// NOTE: this method destroys the object on which it is invoked. So, when control returns to the caller, the timer object will be no longer valid !
	int deactivate();

	// returns the operation_id
	ACS_APGCC_AdminOperationIdType operation_id() { return _operation_id; }

	// Checks action state and updates internal data structures and IMM
	check_action_result check_action();

private:
	uint32_t _delay;
	uint32_t _interval;
	int32_t _slot_id;
	uint32_t _magazine_id;
	ACS_APGCC_AdminOperationIdType _operation_id;
	ACS_APGCC_InvocationType _invocation_id;
	acs_apbm_serverworkingset * _server_working_set;
	time_t _start_time;

	ACE_Recursive_Thread_Mutex _check_action_mutex;
};

#if 0
class acs_apbm_action_thread: public ACE_Task_Base
{
public:
	  acs_apbm_action_thread(acs_apbm_serverworkingset * server_working_set, uint32_t magazine, int32_t slot): _server_working_set(server_working_set), _magazine(magazine), _slot(slot) {};

	  virtual int svc (void)
	  {
		  ACS_APBM_LOG(LOG_LEVEL_INFO, "ACTION THREAD start .... slot = %d, magazine == 0x%08X", _slot, _magazine);

		  sleep(30);
		  while(1)
		  {
			  ACS_APBM_LOG(LOG_LEVEL_INFO, "ACTION THREAD iteration .... slot = %d, magazine == 0x%08X", _slot, _magazine);

			  _server_working_set->imm_repository_handler->notify_asynch_action_completion(_slot, _magazine);
			  sleep(10);
		  }

		  return 0;
	  }

private:
	 acs_apbm_serverworkingset * _server_working_set;
	 uint32_t _magazine;
	 int32_t _slot;
};
#endif

#endif /* ACS_APBM_ACTION_TIMER_H_ */
