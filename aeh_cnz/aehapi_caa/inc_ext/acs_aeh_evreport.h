//========================================================================================
/** @file acs_aeh_evreport.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-06
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
 *	The acs_aeh_evreport class shall be used for all event
 *	reporting in the Adjunct Processor. An instance of this
 *	class should be created by every program that reports
 *	events.
 *
 *	The acs_aeh_evreport class offers three methods:
 *
 *	- sendEventMessage, send event to the event logging system.
 *	- getError,         get additional info about the problem.
 *	- getErrorText,     get detailed info about the problem.
 *
 *	For a description of sendEventMessage, see below. For a
 *	description of the getError and getErrorText methods, see
 *	the class ACS_AEH_Error.
 *
 *
 *  ERROR HANDLING
 *	The Application using the method sendEventMessage has to check the
 *	return code. If error (return code == ACS_AEH_error), the getError
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
 *	If error (ACS_AEH_error), the method getError should be called.
 *	It returns further information of the error causes.
 *	Text error string describing the error is retreived by getErrorText.
 *
 *
 *	SEE ALSO
 *	getError and getErrorText methods included in the ACS_AEH_Error class.
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P0.9.1 | 2011-05-06 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P0.9.1 | 2011-05-13 | xfabron      | Released for ITSTEAM2 sprint 11     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================


#ifndef ACS_AEH_EVREPORT_H
#define ACS_AEH_EVREPORT_H

#include "acs_aeh_error.h"
#include "acs_aeh_types.h"

#include <stdlib.h>


namespace ACS_AEH {
class acs_aeh_evreportimpl;
}

//==============================================================================
//	Class declaration
//
//	NOTE - The size of the class must be retained in order to keep it
//	backwards compatible.
//==============================================================================

class acs_aeh_evreport: public virtual acs_aeh_error
{
public:

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks -
	 */
	acs_aeh_evreport();


	/** @brief  Copy constructor
	 *
	 *	Copy constructor
	 *
	 *	@param[in] 	evReport acs_aeh_evreport object
	 *	@remarks 	-
	 */
	acs_aeh_evreport(const acs_aeh_evreport& evReport);


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	virtual ~acs_aeh_evreport();


	/** @brief  Assignment operator.
	 *	Assignment operator.
	 *
	 *	@param[in] 	evReport: an acs_aeh_evreport object
	 *	@return 	Reference to self
	 *	@remarks 	-
	 */
	acs_aeh_evreport& operator=(const acs_aeh_evreport& evReport);


	//===========//
	// Functions //
	//===========//

	/** @brief
	 *	Used to send event message to Event Handler.<br>
	 *  Before sending, it performs syntax checking on the parameters. Upon success, it returns ACS_AEH_ok, upon failure, ACS_AEH_error<br>
	 *
	 *	@param[in] 	processNamePtr: 		The name of the reporting process.
	 *	@param[in] 	specificProblem: 		The error code number of the problem.
	 *	@param[in] 	percSeverityPtr: 		The severity level of the problem according to AXE standard.
	 *	@param[in] 	probableCausePtr:		The cause of the problem.
	 *	@param[in] 	objClassOfReferencePtr:	Contains the name of the reporting object according to AXE alarm standard.<br>
	 *	                                    Can have the following values: APZ, APT, PWR, EXT, EVENT.
	 *	@param[in] 	objectOfReferencePtr: 	Shows in which instance in the object the fault was detected.
	 *	@param[in] 	problemDataPtr: 		Free text description to be logged.
	 *	@param[in] 	problemTextPtr: 		Free text description to be printed.
	 *	@return 	ACS_AEH_ReturnType
	 *	@remarks	-
	 */
	ACS_AEH_ReturnType sendEventMessage(
						acs_aeh_processName processNamePtr,
						acs_aeh_specificProblem specificProblem,
						acs_aeh_percSeverity percSeverityPtr,
						acs_aeh_probableCause probableCausePtr,
						acs_aeh_objClassOfReference objClassOfReferencePtr,
						acs_aeh_objectOfReference objectOfReferencePtr,
						acs_aeh_problemData problemDataPtr,
						acs_aeh_problemText problemTextPtr);


	/** @brief
	 *	Used to send event message to Event Handler.<br>
	 *  Before sending, it performs syntax checking on the parameters. Upon success, it returns ACS_AEH_ok, upon failure, ACS_AEH_error<br>
	 *
	 *	@param[in] 	processNamePtr: 		The name of the reporting process.
	 *	@param[in] 	specificProblem: 		The error code number of the problem.
	 *	@param[in] 	percSeverityPtr: 		The severity level of the problem according to AXE standard.
	 *	@param[in] 	probableCausePtr:		The cause of the problem.
	 *	@param[in] 	objClassOfReferencePtr:	Contains the name of the reporting object according to AXE alarm standard.<br>
	 *	                                    Can have the following values: APZ, APT, PWR, EXT, EVENT.
	 *	@param[in] 	objectOfReferencePtr: 	Shows in which instance in the object the fault was detected.
	 *	@param[in] 	problemDataPtr: 		Free text description to be logged.
	 *	@param[in] 	problemTextPtr: 		Free text description to be printed.
	 *	@param[in]  manualCease:			if true manual cease allowed, else manual cease not allowed
	 *	@return 	ACS_AEH_ReturnType
	 *	@remarks	-
	 */
	ACS_AEH_ReturnType sendEventMessage(
						acs_aeh_processName processNamePtr,
						acs_aeh_specificProblem specificProblem,
						acs_aeh_percSeverity percSeverityPtr,
						acs_aeh_probableCause probableCausePtr,
						acs_aeh_objClassOfReference objClassOfReferencePtr,
						acs_aeh_objectOfReference objectOfReferencePtr,
						acs_aeh_problemData problemDataPtr,
						acs_aeh_problemText problemTextPtr,
						bool manualCease);

private:

	ACS_AEH::acs_aeh_evreportimpl *evReportImplp_;	// Pointer to event implementation class
};

#endif

