#ifndef ACS_ALH_EVENTMGR_H_
#define ACS_ALH_EVENTMGR_H_


/** @file acs_alh_eventmgr
 *	@brief
 *	@author efabron (Fabio Ronca)
 *	@date 2011-10-31
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
 *	The acs_alh_eventmgr class is responsible for handling the event reporting
 *	to EventHandler from the ALH
 *
 * ERROR HANDLING
 * Discoverage of error when reporting event to EvHandler results in print out to console
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-10-31 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */

#include "acs_alh_common.h"
#include "acs_aeh_evreport.h"

//========================================================================================
//	Constants in event message record.
//========================================================================================

const char ALH_DEFAULT_PROBABLE_CAUSE[] = "AP INTERNAL FAULT";
const char ALH_DEFAULT_OBJECT_CLASS_OF_REF[] = "APZ";


//========================================================================================
//	Class declarations for acs_alh_eventmgr
//========================================================================================

class acs_alh_eventmgr{

public:

	/** @brief acs_alh_eventmgr default constructor
	 *
	 *	The default constructor of the class.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_eventmgr();


	/** @brief ~acs_alh_eventmgr default destructor
	 *
	 *	The default destructor of the class.
	 *
	 *	@remarks Remarks
	 */
	~acs_alh_eventmgr();

	//===========//
	// Functions //
	//===========//

	/** @brief setProcessName method
	 *
	 *	setProcessName is called once at the init (preferably by the alhd) before any other instances of this class are used
	 *	to set the processName to be used by all objects in common when sending Event Reports.
	 *
	 *	@param[in] processName The name of the process running
	 *
	 *	@return none
	 *
	 *	@remarks Remarks
	 */
	void setProcessName(const char processName[]) const;


	/** @brief sendEvent method
	 *
	 *	sendEvent maintains the reporting of an event to the EventHandler API:
	 *
	 *	@param[in] 	specificProblem: 	The error code number of the problem.
	 *	@param[in] 	percSeverity: 		The severity level of the problem according to AXE standard.
	 *	@param[in] 	objectOfRef: 		Shows in which instance in the object the fault was detected.
	 *	@param[in] 	problemData: 		Free text description to be logged.
	 *	@param[in] 	problemText: 		Free text description to be printed.
	 *
	 *	@return true if report, false otherwise
	 *
	 *	@remarks Remarks
	 */
	bool sendEvent(const long specificProblem, const char percSeverity[], const char objectOfRef[],
			const char problemData[], const char problemText[]);


	//============//
	// Attributes //
	//============//
	acs_aeh_evreport eventReport_;					// Event Report API
    static char processName_[PROCESS_NAME_MAX_LEN];	// Common for all objects

};



#endif /* ACS_ALH_EVENTMGR_H_ */
