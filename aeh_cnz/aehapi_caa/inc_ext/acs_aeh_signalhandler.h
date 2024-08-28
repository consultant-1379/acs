//========================================================================================
/** @file acs_aeh_signalhendler.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-09-09
 *	@version 1.0.1
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
 *	The purpose of this handler is to provide ACS components with
 *	a uniform way to handle signals. This is done by setting
 *	reasonable default values to all signals in the system.
 *	Any C or C++ program should call this handler once to
 *	initiate default action for signals. This is done by use
 *	of the following statement:
 *  ACS_AEH_setSignalExceptionHandler_R1(const char  *processName,
 *                                          SUPERVISEDBY supervisedBy)
 *	ProcessName is eg "ACS_AEH_alarmfilter".
 *
 *
 *  ERROR HANDLING
 *	-
 *
 *	SEE ALSO
 *	Derived classes using this base class, i.e ACS_AEH_EvReport.
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-09-08 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-02-09 | xfabron      | Released for ITSTEAM2 sprint 15     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================


#ifndef ACS_AEH_SIGNALHANDLER_H_
#define ACS_AEH_SIGNALHANDLER_H_

// Description:
//		Set default signal signal handler.
//		This routine set-up a default ACS action for an incoming
//		signal. These actions differs between different groups
//
// Parameters:
//		processName:IN			Pointer to character string which
//								contains your process name.

extern void acs_aeh_setSignalExceptionHandler(const char * process_name_);

#endif /* ACS_AEH_SIGNALHANDLER_H_ */
