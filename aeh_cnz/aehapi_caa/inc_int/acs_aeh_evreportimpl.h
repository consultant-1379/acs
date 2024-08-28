//========================================================================================
/** @file acs_aeh_evreportimpl.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-09
 *	@version 0.9.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *	DESCRIPTION
 *	The acs_aeh_evreportimpl class shall be used for all event
 *	reporting in the Adjunct Processor. An instance of this
 *	class should be created by every program that reports
 *	events.
 *
 *	The acs_aeh_evreportimpl class offers three methods:
 *
 *	- sendEventMessage, send event to the event logging system.
 *	- getError,         get additional info about the problem.
 *	- getErrorText,     get detailed info about the problem.
 *
 *	For a description of sendEventMessage, see below. For a
 *	description of the getError and getErrorText methods, see
 *	the class acs_aeh_error.
 *
 * 	ERROR HANDLING
 *	The Application using the method sendEventMessage has to check the
 *	return code. If error (return code == acs_aeh_error), the getError
 *	method should be called. Errors may occur for the following
 *	situations:
 *
 *  - Syntax error, incoming data contains non ascii characters, or
 *	  data has length outside the defined limit. For additional info
 *	  about limit values, see section "CONSTANTS" below.
 *
 *	- Event delivery failure, communication between event report API
 *	  and event logging system broken. This means that no reported
 *	  event will be logged.
 *
 *	If error (acs_aeh_error), the method getError should be called.
 *	It returns further information of the error causes.
 *	Text error string describing the error is retreived by getErrorText.
 *
 *
 *	SEE ALSO
 *	getError and getErrorText methods included in the acs_aeh_errorimpl class.
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.1 | 2011-05-09 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-05-13 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-27 | xfabron      | Released for ITSTEAM2 sprint 13     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================




#ifndef ACS_AEH_EVREPORTIMPL_H
#define ACS_AEH_EVREPORTIMPL_H

#include "acs_aeh_evreport.h"
#include "acs_aeh_eventmsg.h"

#include  <string>
#include <time.h>
#include <vector>


namespace ACS_AEH {

struct eventData {
	time_t lastSent;
	acs_aeh_eventmsg event;
};

struct parameter {
	const char* name;
	int maxsize;
} ;

//========================================================================================
// Class declaration
//========================================================================================

class acs_aeh_evreportimpl
{

public:

	/** @brief Default constructor
	 *
	 *	The constructor of the class.
	 *
	 *	@remarks -
	 */
	acs_aeh_evreportimpl();


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	virtual ~acs_aeh_evreportimpl();


	/** @brief
	 *	Used to send event message to Event Handler.<br>
	 *  Before sending, it performs syntax checking on the parameters. Upon success, it returns ACS_AEH_ok, upon failure, ACS_AEH_error<br>
	 *
	 *	@param[in] 	processNamePtr: 		The name of the reporting process.
	 *	@param[in] 	specificProblem: 		The error code number of the problem.
	 *	@param[in] 	percSeverityPtr: 		The severity level of the problem according to AXE standard.
	 *	@param[in] 	probableCausePtr:		Detailed explanation of the problem (same as an AXE slogan).
	 *	@param[in] 	objClassOfReferencePtr:	Contains the name of the reporting object according to AXE alarm standard.<br>
	 *	                                    Can have the following values: APZ, APT, PWR, EXT, EVENT.
	 *	@param[in] 	objectOfReferencePtr: 	Shows in which instance in the object the fault was detected.
	 *	@param[in] 	problemDataPtr: 		Free text description to be logged.
	 *	@param[in] 	problemTextPtr: 		Free text description to be printed.
	 *	@param[in]  manualCease:			if true manual cease allowed, else manual cease not allowed
	 *	@return 	-
	 *	@remarks	-
	 */
	void sendEventMessage(
				acs_aeh_processName processNamePtr,
				acs_aeh_specificProblem specificProblem,
				acs_aeh_percSeverity percSeverityPtr,
				acs_aeh_probableCause probableCausePtr,
				acs_aeh_objClassOfReference objClassOfReferencePtr,
				acs_aeh_objectOfReference objectOfReferencePtr,
				acs_aeh_problemData problemDataPtr,
				acs_aeh_problemText problemTextPtr,
				bool manualCease)const ;

private:

	void checkParameter(parameter &par, std::string p_paramValue)const;

	void checkSeverity(std::string p_severity, acs_aeh_eventmsg &msg)const;

	void checkClassOfReference(std::string p_classOfReference)const;

	bool eventToSend(acs_aeh_eventmsg &msg) const;

	static std::vector<eventData> eventVector;

}; //end class definition

}; //end namespace ACS_AEH

#endif
