/*
 * acs_alh_athrecord.h
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_AHTRECORD_H_
#define ACS_ALH_AHTRECORD_H_


/** @file acs_alh_ahtrecord
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
 *	The acs_alh_ahtrecord class is responsible forthe Alarm Handler Table record
 *
 *
 * ERROR HANDLING
 * -
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 0.1    | 2011-10-31 | efabron      | File created.                       |
 *	+========+============+==============+=====================================+
 */


#include "acs_aeh_evreport.h"
#include "acs_alh_common.h"


//========================================================================================
//	Class declarations for acs_alh_ahtrecord
//========================================================================================
class acs_alh_ahtrecord{

public:

	/** @brief acs_alh_ahtrecord default constructor
	 *
	 *	The default constructor of the class.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_ahtrecord();


	/** @brief acs_alh_ahtrecord constructor
	 *
	 *	The constructor of the class.
	 *
	 *	@param[in] 	specProb: 		The error code number of the problem.
	 *	@param[in] 	percSev:	 	The severity level of the problem according to AXE standard.
	 *	@param[in] 	probCause: 		The cause of the problem.
	 *	@param[in] 	problemText:	Free text description to be printed.
	 *
	 *	@remarks Remarks
	 */
	acs_alh_ahtrecord(long specProb,
					  const char percSev[],
					  const char probCause[],
					  const char probText[]);


	/** @brief acs_alh_ahtrecord copy constructor
	 *
	 *	The copy constructor of the class.
	 *
	 *	@param[in] 	other: 		acs_alh_ahtrecord object
	 *
	 *	@remarks Remarks
	 */
	acs_alh_ahtrecord(const acs_alh_ahtrecord& other);


	/** @brief ~acs_alh_ahtrecord default destructor
	 *
	 *	The default destructor of the class.
	 *
	 *	@remarks Remarks
	 */
	~acs_alh_ahtrecord();


	//========================//
	//  Operator Overloading  //
	//========================//

	/** @brief  Assignment operator.
	 *	Assignment operator.
	 *
	 *	@param[in] 	other:		an acs_alh_ahtrecord object
	 *
	 *	@return 	Reference to self
	 *
	 *	@remarks 	-
	 */
	acs_alh_ahtrecord& operator=(const acs_alh_ahtrecord& other);


	/** @brief  comparison operator ==
	 *	comparison operator ==
	 *
	 *	@param[in] 	other:		an acs_alh_ahtrecord object
	 *
	 *	@return true if equal, false otherwise
	 *
	 *	@remarks 	-
	 */
	bool operator==(const acs_alh_ahtrecord& other) const;


	/** @brief  comparison operator !=.
	 *	comparison operator !=.
	 *
	 *	@param[in] 	other:	an acs_alh_ahtrecord object
	 *
	 *	@return true if not equal, false otherwise
	 *
	 *	@remarks 	-
	 */
	bool operator!=(const acs_alh_ahtrecord& other) const;


	//===========//
	// Functions //
	//===========//

	/** @brief getSpecProb method
	 *
	 *	getSpecProb used to get the specific problem value of record.
	 *
	 *	@return long:	SpecificProblem value.
	 *
	 *	@remarks Remarks
	 */
	long getSpecProb() const;


	/** @brief getPercSev method
	 *
	 *	getPercSev used to get the severity value of record.
	 *
	 *	@return const char*:	severity value.
	 *
	 *	@remarks Remarks
	 */
	const char* getPercSev() const;


	/** @brief getProbCause method
	 *
	 *	getProbCause used to get the probable cause value of record.
	 *
	 *	@return const char*:	probable cause.
	 *
	 *	@remarks Remarks
	 */
	const char* getProbCause() const;


	/** @brief getProbText method
	 *
	 *	getProbText used to get the problem text value of record.
	 *
	 *	@return const char*:	problem text.
	 *
	 *	@remarks Remarks
	 */
	const char* getProbText() const;

private:

	//============//
	// Attributes //
	//============//

    long specificProblem_;							//specificProblem
    char percSeverity_[PERC_SEVERITY_MAX_LEN];		//severity
    char probableCause_[PROBABLE_CAUSE_MAX_LEN];	//probable cause
    char problemText_[PROBLEM_TEXT_MAX_LEN];		//problemText

};


#endif /* ACS_ALH_AHTRECORD_H_ */
