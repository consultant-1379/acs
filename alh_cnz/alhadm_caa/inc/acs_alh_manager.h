/*
 * acs_alh_manager.h
 *
 *  Created on: Nov 16, 2011
 *      Author: xgiopap
 */

#ifndef ACS_ALH_MANAGER_H_
#define ACS_ALH_MANAGER_H_


/** @file acs_alh_manager
 *	@brief
 *	@author xgiopap (Giovanni Papale)
 *	@date 2011-11-16
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
 *	The acs_alh_manager class is responsible for
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
 *	| 0.1    | 2011-11-16 | xgiopap      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_eventmgr.h"
#include "acs_alh_common.h"
#include "acs_alh_jtpmgr.h"
#include "acs_alh_cpnotifier.h"
#include "acs_alh_alarm_manager.h"
#include "acs_apgcc_paramhandling.h"
#include "ACS_CC_Types.h"
#include "acs_alh_alarm_manager.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Guard_T.h"
#include "ace/Time_Value_T.h"
#include "ace/Condition_Attributes.h"
#include "ace/Monotonic_Time_Policy.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <ACS_JTP.h>

enum acs_alh_event_error {
	ACS_ALH_EVENT_OK   		= 0,
	ACS_ALH_EVENT_DISCARDED = 1,
	ACS_ALH_EVENT_CORRUPTED = 2
};

//========================================================================================
//	Class declarations for acs_alh_manager
//========================================================================================
class acs_alh_manager
{
public:

	acs_alh_manager(acs_alh_imm_connection_handler *connHandler, ACE_Condition<ACE_Thread_Mutex>* cond);
	// Description:
	//		Class Constructor
	// Parameters:
	//		None
	// Return value:
	//	None

	virtual ~acs_alh_manager();
	// Description:
	//		Class Destructor
	// Parameters:
	//		None
	// Return value:
	//		None

	//==================//
	// Public Functions //
	//==================//

	/** @brief runALH method
	 *
	 *  Main program
	 *
	 *	@return int:	return value.
	 *
	 *	@remarks Remarks
	 */
	int runALH();


	/** @brief getEvent method
	 *
	 *  Read from alhfifo the alarem reported by syslog-ng
	 *
	 *	@return int:	return value.
	 *
	 *	@remarks Remarks
	 */
	int getEvent();

	int getIMMNDState();
	/** @brief isApReboot method
	 *
	 *  Read from alhfifo the alarm reported by syslog-ng
	 *
	 *	@return bool:	true if node is rebooted, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool isApReboot();


	/** @brief start_activity method
	 *
	 *  start the ALH_Manager_Thread
	 *
	 *	@return int:	return value
	 *
	 *	@remarks Remarks
	 */
	int start_activity();


	/** @brief stop_activity method
	 *
	 *  stop the ALH_Manager_Thread
	 *
	 *	@return int:	return value
	 *
	 *	@remarks Remarks
	 */
	int stop_activity();


	/** @brief is_working method
	 *
	 *  check if the ALH_Manager_Thread is running
	 *
	 *	@return int:	return value
	 *
	 *	@remarks Remarks
	 */
	bool is_working() { return is_working_; }


	/** @brief get_alhManager_thread_handle method
	 *
	 *  	 *
	 *	@return ACE_thread_t:	ALH_Manager_Thread thread id
	 *
	 *	@remarks Remarks
	 */
	ACE_thread_t get_alhManager_thread_handle() { return alh_manager_work_thread_id; }

	void set_event_reader_thread_failed_flag(bool flag) { event_reader_thread_failed = flag; }
	void set_immnd_monitoring_thread_failed(bool flag){ immnd_monitoring_thread_failed =flag;}

	//ADD for TR HP54702
	bool storeTimestampInformation();
	//END

private:

	//==================//
	// Private Functions //
	//==================//

	/** @brief sendToCP method
	 *
	 *	Used to send alarms or ceasings to the CP.
	 *
	 *	@return int:	return value.
	 *					-1:	Error detected
	 *					 0: OK but nothing to do
	 *					 1: OK
	 *
	 *	@remarks Remarks
	 */
	int sendToCP();


	/** @brief createJtp method
	 *
	 *  Create JTP manager
	 *
	 *	@return bool:	return value.
	 *			 		true if executed, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool createJtp();


	/** @brief getAPNodeNumber method
	 *
	 *  Retrieve the AP Node Number
	 *
	 *	@return int:	the node number.
	 *
	 *	@remarks Remarks
	 */
	int getAPNodeNumber();


	/** @brief getAPNodeName method
	 *
	 *  Retrieve the AP Node Name
	 *
	 *	@return int:	0 if successful
	 *
	 *	@remarks Remarks
	 */
	int getAPNodeName();


	/** @brief checkALCO method
	 *
	 *  Check connection to ALCO
	 *
	 *	@return None:
	 *
	 *	@remarks Remarks
	 */
	void checkALCO();


	/** @brief startedByReboot method
	 *
	 *  Initialization routine
	 *
	 *	@return bool:	 true if booted, false otherwise
	 *
	 *	@remarks Remarks
	 */
//	bool startedByReboot();


	/** @brief create_managers method
	 *
	 *  Create ExeMgr
	 *
	 *	@return bool:	 true if executed, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool create_managers();


	void getEventMessage(ACS_ALH_AlarmMessage& event_message, std::string record);

	//bool checkEventMessage(std::string record);
	int checkEventMessage(std::string record);

	std::string getFormattedProblemDataText(std::string p_text);

	time_t convertEventTime(std::string eventData, std::string eventTime);

	time_t convertEventTime(const char *timeStr);

	bool  treatEvent(ACS_ALH_AlarmMessage& alarmMessage);

	bool handleCpSeparatedState();

	bool handleCpNormal();

	int startIMMNDMonitoringThread();

	int startEventReadThread();

	int stopEventReadThread();

	int startCpNotifierThread(acs_alh_cpnotifier *cpNotifier_ptr);

	bool  createAlarmManager(bool separated);

	bool  clearAlarmTemp(int i);

	ACE_thread_t get_application_thread_handle() { return alh_event_reader_work_thread_id; }

	ACE_thread_t get_cpNotifier_thread_handle() { return alh_cpNotifier_work_thread_id; }

	ACE_thread_t get_immnd_thread_handle() { return alh_immnd_monitoring_thread_id; }

	void set_IMMND_thread_termination(bool stopImmndThread){stopIMMNDMonitoringThread = stopImmndThread;}

	bool get_IMMND_thread_termination(){ return stopIMMNDMonitoringThread; }

	//============//
	// Attributes //
	//============//

	acs_alh_log 						log; 					//Log object
	acs_alh_util 						util; 					//Trace object
	unsigned int        				apNodeNumber_;
	unsigned int        				kapNodeNumber_;
	char                    			cluster_[3];			// Clusterinfo
	char								clusterName_[100];		// Cluster Name
	char								nodename_[100];			// Hostname
	unsigned short      				cpAlarmRef_;
	int                 				CP_state_;
	//added paola
	int                                 prev_CP_state;
	bool   								stopIMMNDMonitoringThread;
	acs_alh_eventmgr    				eventMgr_;
	char                				problemData_[DEF_MESSIZE];
	acs_alh_alarm_manager*  			exeMgr_[NO_OF_CP_SIDES];
	short								jtpMgrState_[NO_OF_CP_SIDES];
	char                    			apNodeSite_[DEF_AP_NODE_SIZE];
	short               				alhStatus_[NO_OF_CP_SIDES];
	AllRecord*          				allRecordToAlco_[NO_OF_CP_SIDES];
	acs_alh_jtpmgr*     				jtpMgr_[NO_OF_CP_SIDES];

	FILE *								pipe;
	acs_alh_imm_connection_handler *	connHandler_;

	ACE_Condition<ACE_Thread_Mutex>* 	term_cond_;
	ACE_thread_t						alh_event_reader_work_thread_id;
	ACE_thread_t 						alh_cpNotifier_work_thread_id;
	ACE_thread_t 						alh_manager_work_thread_id;
	ACE_thread_t						alh_immnd_monitoring_thread_id;

	bool 								signalTermination_;
	bool 								alhManagerTermination_;
	bool 								reboot_;
	bool 								event_reader_thread_failed;
	bool 								is_working_;
	bool								waitTermination_;
	bool								immnd_monitoring_thread_failed;
	bool 								synch_Imm_Access_;

	//ADD for TR HP54702 - START
	char 								startServiceTime_[20];

	//END TR

	AllRecord alarmTemp[2];
};

#endif /* ACS_ALH_MANAGER_H_ */
