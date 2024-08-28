/*
 * acs_alh_athmgr.h
 *
 *  Created on: Nov 3, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_AHTMGR_H_
#define ACS_ALH_AHTMGR_H_

/** @file acs_alh_ahtmgr.h
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-11-03
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
 * 	The acs_alh_ahtmgr class is responsible for
 *
 *	- retrieving the Alarm Handler Table info from the corresponding
 *	  objects of class AlarmConfigurationTable and storing
 *	  it in a properly allocated memory area
 *
 *	- checking for criteria matching from arriving event
 *	  messages versus Alarm handler Table.
 *
 *	- deallocating the Alarm Handler Table
 *
 * ERROR HANDLING
 * In case problem discovered, this is described via setError
 * and bad return code back to caller.
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-11-03 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "acs_alh_ahtrecord.h"
#include "acs_alh_error.h"
#include "acs_alh_util.h"
#include <vector>

//========================================================================================
// Class declaration
//========================================================================================

class acs_alh_ahtmgr : public virtual acs_alh_error
{

public:

	/** @brief acs_alh_ahtmgr default constructor
	 *
	 *	The default constructor of the class.
	 *	OBS! Only one object of this class may be created!
	 *
	 *	@remarks Remarks
	 */
	acs_alh_ahtmgr();


	/** @brief ~acs_alh_ahtmgr default destructor
	 *
	 *	The default destructor of the class.
	 *	Deallocates the memory where the Alarm Handler Table resides
	 *
	 *	@remarks Remarks
	 */
	virtual ~acs_alh_ahtmgr();

	/** @brief getParameters method
	 *
	 *  getParameters uses the APGCC method to retrieve all the records of Alarm Handler Table
	 *  from IMM The records are stored there at system configuration time using
     *  the Parameter Handler utility implying formatting and syntax check
	 *  An estimation has to be made concerning the amount of memory
	 *  that has to be allocated to store the table. This is constant
	 *  during operation phase since a change in Alarm Handler Table
	 *  involves reboot of the AP
	 *
	 *
	 *	@return true if executed, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool getParameters();


	/** @brief getMatch method
	 *
	 *  syntaxCheck If the Specific Problem attribute of received message is found
	 *  in a record in Alarm Handler table, The Perceived Severity
	 *  from the record is substituting the Perceived Severity in the message
	 *
	 *  @param[in] 	alarmMessage: 	Event record received from EventLog
	 *
	 *	@return tnone
	 *
	 *	@remarks Remarks
	 */
	void getMatch(ACS_ALH_AlarmMessage& alarmMessage) const;

	void setAlarmTableFlag ( bool value );

	bool Alarm_Table_Already_Loaded() { return alhList_Already_loaded; }

private:

	/** @brief syntaxCheck method
	 *
	 *  syntaxCheck Checks that a parameter [specificProblem/percSeverity] is correct
	 *  in the following respects:
	 *	-	the specificProblem must not occur for any other parameter
	 *		previously stored in the Alarm Handler Table
	 *	-	percSeverity must be of type [A1, A2, A3, O1, O2, EVENT]
	 *
	 *  @param[in] 	ahtRecord: 	Parameter to be checked.
	 *
	 *	@return true if executed, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool syntaxCheck(const acs_alh_ahtrecord& ahtRecord);

	bool alhList_Already_loaded;

	std::vector<acs_alh_ahtrecord> ahtList_;	// Alarm Handler Tab list object

	acs_alh_util util_;
};

#endif /* ACS_ALH_AHTMGR_H_ */
