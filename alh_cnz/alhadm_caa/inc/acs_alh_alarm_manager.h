/*
 * acs_alh_alarm_manager.h
 *
 *  Created on: Nov 11, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_ALARM_MANAGER_H_
#define ACS_ALH_ALARM_MANAGER_H_

/** @file acs_alh_alarm_manager.h
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-11
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
 *	DESCRIPTION
 * 	The acs_alh_alarm_manager.h class is responsible for
 *
 *		- checking the events received from Syslog-ng
 *		  for matching criterias in the Alarm Configuration table. If
 *		  criterium is met, the Perceived Severity parameter in
 *		  the received event is superseeded by the one given in
 *		  the Alarm Handler table. If Pereceived Severity indicates
 *		  Alarm or Alarm Ceasing, The record is stored in IMM
 *		  to be sent to the CP.
 *
 *		- retrieving the Alarm record info stored in IMM
 *
 *		- inserting and deleting items in IMM
 *
 *		- re-editing some attributes of the records in the
 *		  Alarm record and update the related IMM object
 *
 *		- search for alarm items in IMM
 *
 *
 * ERROR HANDLING
 *	The ACS_ALH_Exec class will report error event to the
 *	AP Event Handler in the following situations:
 *
 *
 *	In case other problem discovered, this is described via setError
 *	and bad return code back to caller
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-11 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "acs_alh_common.h"
#include "acs_alh_ahtmgr.h"
#include "acs_alh_eventmgr.h"
#include "acs_alh_error.h"
#include "acs_alh_util.h"
#include "acs_alh_log.h"
#include "acs_alh_imm_data_handler.h"
#include "acs_alh_imm_connection_handler.h"

const int MAX_ALARMS_IN_LIST = 1000;


//========================================================================================
// Class declaration
//========================================================================================

class acs_alh_alarm_manager : public virtual acs_alh_error {
public:

	//==============//
	// Constructors //
	//==============//

	/** @brief acs_alh_ahtmgr default constructor
	 *
	 *	The default constructor of the class.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_alarm_manager(acs_alh_imm_connection_handler *connHandler);


	/** @brief acs_alh_alarm_manager constructor
	 *
	 *	The new object is a copy of the 'another' object but the cpSide is SB.
	 *
	 *  @param[in] 	execPtr: 	The object that handle the alarm for cpSide EX
	 *
	 *	@return none
	 *
	 *	@remarks Remarks
	 */
	acs_alh_alarm_manager(const acs_alh_alarm_manager* execPtr);


	/** @brief acs_alh_alarm_manager constructor
	 *
	 *	Constructor. Used for ex-side
	 *
	 *  @param[in] 	execPtr: 	acs_alh_eventmgr object
	 *
	 *	@return none
	 *
	 *	@remarks Remarks
	 */
	acs_alh_alarm_manager(acs_alh_imm_connection_handler *connHandler, const acs_alh_eventmgr& eventMgr);



	/** @brief ~acs_alh_alarm_manager default destructor
	 *
	 *	The default destructor of the class.
	 *
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_alh_alarm_manager();

	//==================//
	// Public Functions //
	//==================//


	bool treatEvent(ACS_ALH_AlarmMessage& eventMessage);

	bool ackAlarm(AllRecord* allPtr, const unsigned short cpAlarmRef);

	bool ackCeasing(const AllRecord* allPtr);

	int getUnAckItem(AllRecord &alarm, int cpstate);

	bool unAckAllItem();

	void setCpState(bool separated) {separatedCP_ = separated;}

	int countAlarmsInList();

	int cleanAlarmIMM();

	int cleanAlarmIMM_slowCP();

	int get_exOrsb (){ return exOrsb;}

private:

	//==================//
	// Private Functions //
	//==================//

	short getAlarmIndex();
	bool setAlarmIndex(short index, unsigned char oneOrZero);
	bool treatAlarm(ACS_ALH_AlarmMessage& eventMessage);
	bool treatCeasing(ACS_ALH_AlarmMessage& eventMessage);
	bool storeToIMM(const AllRecord* allPtr);
	bool appendCeasingToList(AllRecord* arPtr, AllRecord* ceasePtr);


	//============//
	// Attributes //
	//============//

	acs_alh_log log_;
	acs_alh_util util_;
	acs_alh_ahtmgr ahtMgr_;				 // Alarm Handler Table handling object
	acs_alh_eventmgr eventMgr_;
	acs_alh_imm_data_handler immHandler_;   // Imm Handler
	acs_alh_imm_connection_handler *connHandler_;
	unsigned int currentAlarmsInList_;  // Current nr ALARMS in Alarm List
	int exOrsb;
	bool separatedCP_;


};

#endif /* ACS_ALH_ALARM_MANAGER_H_ */
