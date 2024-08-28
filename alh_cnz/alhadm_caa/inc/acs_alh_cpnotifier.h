/*
 * acs_alh_cpnotifier.h
 *
 *  Created on: Nov 14, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_CPNOTIFIER_H_
#define ACS_ALH_CPNOTIFIER_H_


/** @file acs_alh_cpnotifier
 *	@brief
 *	@author xgiopap (Giovanni Papale)
 *	@date 2011-14-11
 *	@version 0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 * DESCRIPTION
 *	The acs_alh_cpnotifier class is responsible for
 *
 *		- handling connect check to the CP state
 *
 * ERROR HANDLING
 *		-
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-03 | xgiopap      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "ace/Task.h"
#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_common.h"
#include <ACS_DSD_CPStateNotifier.h>
#include <ACS_DSD_MacrosConstants.h>
#include <ACS_JTP.h>
#include <iostream>


#include "ace/Synch.h"
#include "ace/OS.h"


//========================================================================================
//	Class declarations for acs_alh_cpnotifier
//========================================================================================
class acs_alh_cpnotifier /*: public ACE_Task_Base*/
{
public:

	acs_alh_cpnotifier(ACE_Condition<ACE_Thread_Mutex>* cond, ACE_Condition<ACE_Thread_Mutex>* stop_cond);
	// Description:
	//		Constructor -
	// Parameters:
	//		None
	// Return value:
	//	None

	virtual ~acs_alh_cpnotifier();
	// Description:
	//		Destructor -
	// Parameters:
	//		None
	// Return value:
	//		None

	acs_dsd::CPStateConstants get_cpstate_value();
	// Description:
	//		Return the CP state
	// Parameters:
	//		-
	//
	// Return value:
	//		int			CP state

	bool CP_Connect();
	// Description:
	//		Setup connection towards HeartBeat
	// Parameters:
	//		None
	// Return value:
	//		true if executed, false otherwise

	//virtual int svc (void);

	int getCpState();

	bool get_cpNotifier_thread_terminated(){return alh_cpNotifier_thread_terminated;}

	int get_cpNotifier_thread_exit_code(){return alh_cpNotifier_thread_exit_code;}

	acs_dsd::CPStateConstants	current_CP_state_;

	void stop_cpNotifier_activity(){alh_cpNotifier_thread_stop = true;}

	bool get(){return cpnotifier_waitTermination_;}

private:

	acs_alh_log 				log; //Log object
	acs_alh_util 				util; //Trace object
	ACS_DSD_CPStateNotifier 	cp_state_notifier;
	ACS_DSD_CPStateNotifier 	cp_current_state_notifier;
	acs_dsd::CPStateConstants 	cp_state_;


	//ADD BY FABIO
	ACE_Condition<ACE_Thread_Mutex>* cond_;
	ACE_Condition<ACE_Thread_Mutex>* stop_cond_;
	bool alh_cpNotifier_thread_terminated;
	int alh_cpNotifier_thread_exit_code;
	bool alh_cpNotifier_thread_stop;
	bool cpnotifier_waitTermination_;

};


#endif /* ACS_ALH_CPNOTIFIER_H_ */
