/*
 * acs_alh_eventreader.h
 *
 *  Created on: Nov 4, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_EVENTREADER_H_
#define ACS_ALH_EVENTREADER_H_


/** @file acs_alh_eventreader
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-04
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
 *	The acs_alh_eventreader class is responsible for read the alarm record report by
 *	Syslog-ng int alhpipe
 *
 * ERROR HANDLING
 * -
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-04 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "acs_alh_common.h"
#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_error.h"

#include "acs_alh_alarm_manager.h"

//========================================================================================
//	Class declarations for acs_alh_eventreader
//========================================================================================
class acs_alh_eventreader : public virtual acs_alh_error {

public:

	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks Is used by derived objects.
	 *
	 */
	acs_alh_eventreader(acs_alh_imm_connection_handler *connHandler);


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	~acs_alh_eventreader();


	//===========//
	// Functions //
	//===========//

	/** @brief
	 *	Read new alarm record report from alhpipe
	 *
	 *	@return 	int
	 *
	 *	@remarks	-
	 */
	int getEvent();


	/** @brief
	 *	Read new alarm record report from alhpipe
	 *
	 *	@param[in] 	event_message: The structur where store information of alarm record
	 *
	 *	@param[in] 	record: the alarm reord to store in event_message structure
	 *
	 *	@return 	-
	 *
	 *	@remarks	-
	 */
	void getEventMessage(ACS_ALH_AlarmMessage& event_message, std::string record);

	int startEventReadThread();

	ACE_thread_t get_application_thread_handle() { return alh_event_reader_work_thread_id; }

	bool  createAlarmManager(bool separated);

	void testAck();

private:

	bool checkEventMessage(std::string record);

	std::string getFormattedProblemDataText(std::string p_text);

	time_t convertEventTime(std::string eventData, std::string eventTime);

	bool  treatEvent(ACS_ALH_AlarmMessage& alarmMessage);

	acs_alh_util util_;

	acs_alh_log log_;

	int CP_state_;

	bool signalTermination_;

	acs_alh_eventmgr eventMgr_;

	acs_alh_imm_connection_handler *connHandler_;

	acs_alh_alarm_manager *exeMgr_[NO_OF_CP_SIDES];

	ACE_thread_t alh_event_reader_work_thread_id;
};

#endif /* ACS_ALH_EVENTREADER_H_ */
