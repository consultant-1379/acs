#ifndef _TIMER_H_
#define _TIMER_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <iostream>
#include <ace/ACE.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

class ACS_LM_TimerListener
{
	/*=====================================================================
                                PUBLIC DECLARATION SECTION
     ==================================================================== */
public:
	/*===================================================================
		   						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
		   							   PUBLIC METHOD
	=================================================================== */
	/*==================================================================== */
	/**
	 * @brief timeout
	 *
	 *  @return void
	 */
	/*==================================================================== */
	virtual void timeout() = 0;
};
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Timer
{
private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */
	ACS_LM_TimerListener* theListener;
	ACE_UINT64 thePeriod;
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_Timer(const ACS_LM_Timer&); // disabled.
	//*=================================================================== */
	/**
			@brief      operator
	 */
	/*=================================================================== */
	const ACS_LM_Timer& operator=(const ACS_LM_Timer&); // disabled.

public:
	/*=================================================================== */
	/**
				@brief       Parameterised constructor for ACS_LM_Timer

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Timer(ACS_LM_TimerListener* listener);
	/*=================================================================== */
		/**
				@brief       Default destructor for ACS_LM_Timer

				@par         None

				@pre         None

				@post        None

				@exception   None
		 */
		/*=================================================================== */
	~ACS_LM_Timer();
	/*=================================================================== */
	/**
	 *  @brief timerAPC
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void timerAPC();
	/*=================================================================== */
	/**
	 *  @brief waitForTimeout
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void waitForTimeout();
	/*=================================================================== */
	/**
	 *  @brief start
	 *
	 *  @param aSeconds 				: ACE_UINT64
	 *
	 *  @return bool 					: true/false
	 */
	/*=================================================================== */
	bool start(ACE_UINT64 aSeconds);
	/*=================================================================== */
	/**
	 *  @brief cancel
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void cancel();
	/*=================================================================== */
	/**
	 *  @brief getPeriod
	 *
	 *  @return ACE_UINT64
	 */
	/*=================================================================== */
	ACE_UINT64 getPeriod() const;
	//ACE_UINT64 const getPeriod();
	ACE_HANDLE theStopRequestEventHandles[2];
	ACE_HANDLE theJobCompleteEventHandles[2];
	/*=================================================================== */
	/**
	 *  @brief timerThread
	 *
	 *  @return ACE_THR_FUNC_RETURN
	 */
	/*=================================================================== */
	static ACE_THR_FUNC_RETURN timerThread(void*);

};


#endif
