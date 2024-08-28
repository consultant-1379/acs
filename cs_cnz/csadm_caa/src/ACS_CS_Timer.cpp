//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

#include "ACS_CS_Timer.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_Timer_TRACE);

using std::string;

//----------------------------------------------------------------------

/** @class ACS_CS_TimerEventHandler
 *	@brief Internal class to handle events callbacks from the ACE Reactor
 *	@author xminaon
 *
 *	This class is not a Singleton but only one class should be created.
 */

class ACS_CS_TimerEventHandler: public ACE_Event_Handler
{
	public:
		virtual int handle_timeout(const ACE_Time_Value &current_time, const void *act=0);
		virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask);

		virtual ~ACS_CS_TimerEventHandler();
};

/*
 * @brief callback function when timer is signaled
 */
int ACS_CS_TimerEventHandler::handle_timeout(const ACE_Time_Value &/*current_time*/, const void *act)
{
	if (act)
	{
		const ACS_CS_EventHandle * timerHandle = reinterpret_cast<const ACS_CS_EventHandle*>(act);

		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_TimerEventHandler::handle_timeout(current_time, %d)", *timerHandle));

		if (!ACS_CS_Event::SetEvent(*timerHandle))
		{
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_TimerEventHandler::handle_timeout(): "
				"Failed to set event for timer: %d", *timerHandle));
		}

		ACS_CS_Timer::instance()->timerFinished(*timerHandle);
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_TimerEventHandler::handle_timeout(current_time, 0)"));
	}

	// Stay registered with reactor
	return 0;
}

/*
 * @brief callback function when event handler is unregistered with the reactor
 */
int ACS_CS_TimerEventHandler::handle_close(ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*mask*/)
{
	return 0;
}


ACS_CS_TimerEventHandler::~ACS_CS_TimerEventHandler() {}

//----------------------------------------------------------------------


ACS_CS_Timer::ACS_CS_Timer()
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
						"ACS_CS_Timer::ACS_CS_Timer(): "
						"Entered..."));

	eventHandler = new ACS_CS_TimerEventHandler();
}


ACS_CS_Timer::~ACS_CS_Timer() {

	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
					"ACS_CS_Timer::~ACS_CS_Timer(): Entered..."
					"Timers = %d Named Timers %d", instance()->timers_.size(), instance()->namedTimers_.size()));

    TimerIteratorType utit = instance()->timers_.begin();

    for (;utit != instance()->timers_.end(); utit++) {
    	TimerProperties_t * t = *utit;
    	instance()->CancelTimer(t->handle);
    }

	NamedTimerIteratorType ntit = instance()->namedTimers_.begin();

    for (;ntit != instance()->namedTimers_.end(); ntit++) {
        // check if the handle is in the named timers list
    	TimerProperties_t * t = ntit->second;
    	instance()->CancelTimer(t->handle);
    }

    delete eventHandler;
}


ACS_CS_EventHandle ACS_CS_Timer::CreateTimer(bool manualReset, const char* name)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::CreateTimer(%b, %s)", manualReset, name != 0 ? name : "0"));

	ACS_CS_EventHandle timerHandle = ACS_CS_EVENT_RC_ERROR;

	if (name != 0) {
		// named timer, check if it already exists
		if ((timerHandle = instance()->OpenNamedTimer(name)) != -1) {
			// do not create any new timer as this already exists
			return timerHandle;
		}
	}

	timerHandle = ACS_CS_Event::CreateEvent(manualReset, false, 0);

	if (timerHandle != ACS_CS_EVENT_RC_ERROR)
	{
		// create timer object
		TimerProperties_t *properties = new TimerProperties_t();
		properties->handle = timerHandle;
		properties->manualReset = manualReset;
		properties->timer_id = 0;

		 if (name != 0 && strlen(name) > 0) {
			// named timer
			NamedTimerKeyTypePair timer(name, properties);

			instance()->lock_.acquire_write();
			instance()->namedTimers_.insert(timer);
			instance()->lock_.release();

			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::CreateTimer(): "
				"Created named timer: \"%s\", handle: %d", name, timerHandle));
		} else {
			// unnamed timer
			instance()->lock_.acquire_write();
			instance()->timers_.push_back(properties);
			instance()->lock_.release();

			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::CreateTimer(): "
				"Created unnamed timer: %d", timerHandle))
		}
	}

	return timerHandle;
}


ACS_CS_EventHandle ACS_CS_Timer::OpenNamedTimer(string name)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::OpenNamedTimer(%s)", name.c_str()));

	instance()->lock_.acquire_read();

	NamedTimerIteratorType it = instance()->namedTimers_.end();

	if ((it = instance()->namedTimers_.find(name)) == instance()->namedTimers_.end()) {
		// named timer does not exist in our map
		instance()->lock_.release();
		return ACS_CS_EVENT_RC_ERROR;
	}

	ACS_CS_EventHandle handle = it->second->handle;
	instance()->lock_.release();

	return handle;
}


void ACS_CS_Timer::CloseTimer(ACS_CS_EventHandle timerHandle)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::CloseTimer(%d)", timerHandle));

	// Crashfix? Cancel timer before closing and shutdown to avoid segfault at shutdown
	instance()->CancelTimer(timerHandle);

	instance()->lock_.acquire_write();

	// check the list with named timers
	NamedTimerIteratorType ntit = instance()->findNamedTimerByHandle(timerHandle);

	if (ntit != instance()->namedTimers_.end()) {
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::CloseTimer(): "
			"Removing named timer, %s", ntit->first.c_str()));

		ACS_CS_Event::CloseEvent(timerHandle);	// close timer handle

		delete ntit->second;    // free TimerProperties_t
		instance()->namedTimers_.erase(ntit);
		instance()->lock_.release();
		return;
	}

	// check the list with unnamed timers
	TimerIteratorType utit = instance()->findTimerByHandle(timerHandle);

	if (utit != instance()->timers_.end()) {
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::CloseTimer(): "
			"Removing unnamed timer, handle: %d", timerHandle));

		ACS_CS_Event::CloseEvent(timerHandle);	// close timer handle

		TimerProperties_t *ptr = *utit;
		instance()->timers_.erase(utit);
		delete ptr;

		instance()->lock_.release();
		return;
	}

	instance()->lock_.release();
}


bool ACS_CS_Timer::SetTimer(ACS_CS_EventHandle timerHandle, int timeout)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::SetTimer(%d, %d)", timerHandle, timeout));


	TimerProperties_t* properties = 0;

	instance()->lock_.acquire_read();

	// check the list with named events
	NamedTimerIteratorType ntit = instance()->findNamedTimerByHandle(timerHandle);

	if (ntit != instance()->namedTimers_.end()) {
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::SetTimer(): "
			"Finding named timer, %s (%d)", ntit->first.c_str(), timerHandle));

		properties = ntit->second;
	}

	if ( ! properties)
	{

		// check the list with unnamed events
		TimerIteratorType utit = instance()->findTimerByHandle(timerHandle);

		if (utit != instance()->timers_.end()) {
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::SetTimer(): "
				"Finding unnamed timer, %d", timerHandle));

			properties = (*utit);
		}
	}

	instance()->lock_.release();

	long timer_id = -1;

	if (properties)
	{
		ACE_Time_Value delay(timeout);

		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::SetTimer(): "
			"Setting timer %d to %d s", timerHandle, timeout));

		timer_id = ACE_Reactor::instance()->schedule_timer(instance()->eventHandler, &properties->handle, delay);

		if (timer_id > -1)
		{
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::SetTimer(): "
				"Successfully scheduled timer, timer_id=%d, timerHandle=%d", timer_id, properties->handle));

			properties->timer_id = timer_id;
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::SetTimer(): "
				"Failed to scheduled timer, timerHandle=%d", properties->handle));
		}
	}

	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::SetTimer(): "
		"Leaving SetTimer"));

	return timer_id >= 0;
}


bool ACS_CS_Timer::CancelTimer(ACS_CS_EventHandle timerHandle)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::CancelTimer(%d)", timerHandle));

	long timer_id = 0;
	TimerProperties_t* properties = 0;

	instance()->lock_.acquire_read();

	// check the list with named events
	NamedTimerIteratorType ntit = instance()->findNamedTimerByHandle(timerHandle);

	if (ntit != instance()->namedTimers_.end()) {
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::CancelTimer(): "
			"Finding named timer, %s (%d)", ntit->first.c_str(), timerHandle));

		properties = ntit->second;
	}

	if ( ! properties)
	{
		// check the list with unnamed events
		TimerIteratorType utit = instance()->findTimerByHandle(timerHandle);

		if (utit != instance()->timers_.end()) {
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::CancelTimer(): "
				"Finding unnamed timer, %d", timerHandle));

			properties = (*utit);
		}
	}

	if (properties)
	{
		timer_id = properties->timer_id;
		properties->timer_id = 0;
	}

	instance()->lock_.release();

	int result = 0;

	if (timer_id)
	{
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::CancelTimer(): "
			"Cancelling timer, %d", timerHandle));

		result = ACE_Reactor::instance()->cancel_timer(timer_id);
	}

	return result;
}


ACS_CS_Timer* ACS_CS_Timer::instance()
{
    return instance_::instance();
}


// NOTE! Must be wrapped inside acquire/release statement
ACS_CS_Timer::NamedTimerIteratorType ACS_CS_Timer::findNamedTimerByHandle(ACS_CS_EventHandle timerHandle)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::findNamedTimerByHandle(%d)", timerHandle));

    // check the list with named timers
	NamedTimerIteratorType ntit = instance()->namedTimers_.begin();

    for (;ntit != instance()->namedTimers_.end(); ntit++) {
        // check if the handle is in the named timers list
        if (ntit->second->handle == timerHandle) {
            break;
        }
    }

    return ntit;
}


// NOTE! Must be wrapped inside acquire/release statement
ACS_CS_Timer::TimerIteratorType ACS_CS_Timer::findTimerByHandle(ACS_CS_EventHandle timerHandle)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::findTimerByHandle(%d)", timerHandle));

    // check the list of unnamed timers
    TimerIteratorType utit = instance()->timers_.begin();

    for (;utit != instance()->timers_.end(); utit++) {
        if ((*utit)->handle == timerHandle) {
            break;
        }
    }

    return utit;
}

void ACS_CS_Timer::timerFinished(ACS_CS_EventHandle timerHandle)
{
	ACS_CS_TRACE((ACS_CS_Timer_TRACE,
		"ACS_CS_Timer::timerFinished(%d)", timerHandle));

	TimerProperties_t* properties = 0;

	instance()->lock_.acquire_read();

	// check the list with named events
	NamedTimerIteratorType ntit = instance()->findNamedTimerByHandle(timerHandle);

	if (ntit != instance()->namedTimers_.end()) {
		ACS_CS_TRACE((ACS_CS_Timer_TRACE,
			"ACS_CS_Timer::timerFinished(): "
			"Finding named timer, %s (%d)", ntit->first.c_str(), timerHandle));

		properties = ntit->second;
	}

	if ( ! properties)
	{
		// check the list with unnamed events
		TimerIteratorType utit = instance()->findTimerByHandle(timerHandle);

		if (utit != instance()->timers_.end()) {
			ACS_CS_TRACE((ACS_CS_Timer_TRACE,
				"ACS_CS_Timer::timerFinished(): "
				"Finding unnamed timer, %d", timerHandle));

			properties = (*utit);
		}
	}

	if (properties)
	{
		// Reset timer_id
		properties->timer_id = 0;
	}

	instance()->lock_.release();

}
