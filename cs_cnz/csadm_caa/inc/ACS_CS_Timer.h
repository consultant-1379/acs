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

#ifndef ACS_CS_Timer_h
#define ACS_CS_Timer_h 1

#include <string>
#include <map>
#include <vector>
#include <set>

#include <ace/Singleton.h>
#include "ace/RW_Mutex.h"
#include <ace/Recursive_Thread_Mutex.h>

#include "ACS_CS_Event.h"

class ACS_CS_TimerEventHandler;


/** @class ACS_CS_Timer
 *	@brief timer implementation class for CS
 *	@author xminaon
 *
 *	A class used to mimic the functionality of waitable timers on Windows. This class uses ACE
 *	timers with the addition to use names to identify timers. The handles returned by this class
 *	can be used by the ACS_CS_Event class to wait for the timer. This class is a Singleton and
 *	its public functions are all static.
 */
class ACS_CS_Timer
{
    public:

		/** @brief Function to create timer
		 *
		 *	param[in]	manualReset	If the timer should be reset automatically
		 *	param[in]	name		Name for the timer, or NULL for an unamed timer
		 *	@return		Handle to the created timer or ACS_CS_EVENT_RC_ERROR if the
		 *				timer could not be created
		 */
        static ACS_CS_EventHandle CreateTimer(bool manualReset, const char* name);

        /** @brief Function to open existing timer based on name
		 *
		 *	param[in]	name		Name of the timer
		 *	@return		Handle to the named timer or ACS_CS_EVENT_RC_ERROR if a
		 *				timer with the specified name could not be found
		 */
        static ACS_CS_EventHandle OpenNamedTimer(std::string name);

        /** @brief Function to close (remove) a timer
		 *
		 *	param[in]	timerHandle		Handle to the timer to be closed
		 *
		 *	This will release any resources used by the timer.
		 */
        static void CloseTimer(ACS_CS_EventHandle timerHandle);

        /** @brief Function to set a timer
		 *
		 *	param[in]	timerHandle		Handle to the timer to be set
		 *	param[in]	timeout			A timeout (delay) in seconds after which
		 *								the timer should be signaled.
		 *	@return		True if the timer could be set, or false otherwise.
		 */
        static bool SetTimer(ACS_CS_EventHandle timerHandle, int timeout);

        /** @brief Function to cancel a running timer
         *
		 *	param[in]	timerHandle		Handle to the timer to be canceled
		 *	@return		True if the timer could be canceled, or false otherwise.
		 *
		 *	This function will not change the state of the timer. Any thread
		 *	already waiting on this timer (through ACS_CS_Event::WaitForEvents())
		 *	will continue doing so.
		 */
        static bool CancelTimer(ACS_CS_EventHandle timerHandle);

        friend class ACE_Singleton<ACS_CS_Timer, ACE_Recursive_Thread_Mutex>;

        friend class ACS_CS_TimerEventHandler;

    protected:

        ACS_CS_Timer();

        ~ACS_CS_Timer();

        static ACS_CS_Timer* instance();

    private:

        typedef struct {
			ACS_CS_EventHandle handle;
			bool manualReset;
			long timer_id;
		} TimerProperties_t;

		std::vector<TimerProperties_t*> timers_;
		std::map<string, TimerProperties_t*> namedTimers_;
		std::set<ACS_CS_EventHandle> runningTimers_;

		typedef std::pair<string, ACS_CS_Timer::TimerProperties_t*> NamedTimerKeyTypePair;
		typedef std::vector<ACS_CS_Timer::TimerProperties_t*>::iterator TimerIteratorType;
		typedef std::map<string, ACS_CS_Timer::TimerProperties_t*>::iterator NamedTimerIteratorType;
		typedef std::set<ACS_CS_Timer::TimerProperties_t*>::iterator RunningTimersIteratorType;

		typedef ACE_Singleton<ACS_CS_Timer, ACE_Recursive_Thread_Mutex> instance_;

		NamedTimerIteratorType findNamedTimerByHandle(ACS_CS_EventHandle timerHandle);
		TimerIteratorType findTimerByHandle(ACS_CS_EventHandle timerHandle);

		void timerFinished(ACS_CS_EventHandle timerHandle);

		ACS_CS_TimerEventHandler* eventHandler;
		ACE_RW_Mutex lock_;

		// To prevent use
		ACS_CS_Timer(const ACS_CS_Timer &right);
		ACS_CS_Timer & operator=(const ACS_CS_Timer &right);

};

#endif // ACS_CS_Timer_h

