//========================================================================================
/** @file acs_aeh_exception.h
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-23
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
 *	-
 *
 *
 *  ERROR HANDLING
 *	-
 *
 *
 *	SEE ALSO
 *	-
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-05-23 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 | 2011-06-08 | xfabron      | Released for ITSTEAM2 sprint 12     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 */
//========================================================================================

#ifndef ACS_AEH_EXCEPTION_H_
#define ACS_AEH_EXCEPTION_H_


#include <string.h>
#include <iostream>
#include <sstream>


// The first two error codes are defined for the purpose of
// being compatible with "Design Rule for AP Commands".

enum errorType {
	// Standard return codes for AP exceptions
	GENERAL_FAULT =		 1,	// Error when executing (general fault)
	INCORRECT_USAGE =	 2,	// Incorrect usage
	PARAMERROR =		16,	// Parameter error
	SYSTEMERROR =		17,	// System error
	INTERNALERROR =		18	// Internal error
};


#define GENERAL_FAULT_STR 		"Execution error"
#define INCORRECT_USAGE_STR 	"Usage"
#define PARAMERROR_STR 			"Parameter error"
#define SYSTEMERROR_STR 		"System error"
#define INTERNALERROR_STR 		"Internal error"




//==============================================================================
//	Class declaration
//==============================================================================

class acs_aeh_exception{

	/** @brief operator<<
	 *
	 *	Stream operator method.
	 *
	 *	@param[in] s:			Output stream
	 *	@param[in] ex:			acs_aeh_exception instance
	 *
	 *	@return std::ostream&:	Output stream.
	 *
	 *	@remarks -
	 */
	friend std::ostream& operator<<(std::ostream& s, const acs_aeh_exception& ex);

public:
	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks -
	 */
	acs_aeh_exception(errorType errcode);


	/** @brief class constructor
	 *
	 *	Constructor of class
	 *
	 *	@param[in] p_errcode:		error code.
	 *	@param[in] p_lerrno:		linux error number.
	 *
	 *	@remarks -
	 */
	acs_aeh_exception(errorType p_errcode, int p_lerrno);


	/** @brief class constructor
	 *
	 *	Constructor of class
	 *
	 *	@param[in] p_errcode:		error code.
	 *	@param[in] p_detailInfo:	optional textual information
	 *	@param[in] p_lerrno:		linux error number.
	 *
	 *	@remarks -
	 */
	acs_aeh_exception(errorType p_errcode, std::string p_detailInfo, int p_lerrno = 0);


	/** @brief copy constructor
	 *
	 *	Copy constructor
	 *
	 *	@param[in] p_ex:	 acs_aeh_exception object
	 *
	 *	@remarks 	-
	 */
	acs_aeh_exception(const acs_aeh_exception& p_ex);


	/** @brief destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	~acs_aeh_exception();


	//===========//
	// Functions //
	//===========//

	/**	@brief getErrorCode method
	 *	Read the error code.
	 *
	 *	@return errorType:	The error code number of the problem.
	 *
	 */
	errorType getErrorCode() const;


	/**	@brief getResultCode method
	 *	Read the Linux error number.
	 *
	 *	@return int:	Linux error number.
	 *
	 */
	int getResultCode() const;


	/**	@brief errorText method
	 *	Read the error text.
	 *
	 *	@return std::string:	error Text.
	 *
	 */
	const std::string errorText() const;


	/**	@brief detailInfo method
	 *	Read the detailed information.
	 *
	 *	@return std::string:	Optional textual information.
	 *
	 */
	const std::string detailInfo() const;


	/**	@brief operator() method
	 *	Stream detailed information
	 *
	 *	@return std::string:	OStringstream
	 *
	 */
	std::stringstream& operator()();


	/**	@brief strError) method
	 *	Translate a error number code into a fault string
	 *
	 *	@param[in] p_lerrno:	Linux error number code.
	 *
	 *	@return std::string:	Error text
	 *
	 */
	std::string strError(int p_lerrno) const;

private:

	errorType error_;		// Error code
	int lerrno;				// Linux error number
	std::stringstream s_;	// Error stream


};


#endif /* ACS_AEH_EXCEPTION_H_ */
