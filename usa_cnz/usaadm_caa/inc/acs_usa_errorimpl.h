//========================================================================================
/** @file acs_usa_errorimpl.h
 *	@brief
 *	@author 
 *	@date 
 *	@version
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
 *	This class serves as a base class to the ACS_USA::EvReport object
 *	The class provides the public methods getError, getErrorText and the
 *	protected methods setError and setErrorText (which is overloaded).
 *	getError returns the type of error as set by a previous	setError.
 *	getErrorText returns a text error string as set by a previous
 *	setErrorText.
 *
 *
 *  ERROR HANDLING
 *	This is class to be used when an error occurs and no specific
 *	error handling is necessary when using this class.
 *
 *
 *	SEE ALSO
 *	-
 *
 *
 */
//========================================================================================


#ifndef ACS_USA_ERRORIMPL_H 
#define ACS_USA_ERRORIMPL_H

#include "acs_usa_error.h"
#include <string>

namespace ACS_USA {

//========================================================================================
// Class declaration
//========================================================================================

class acs_usa_errorimpl
{ 
public:
	//==============//
	// Constructors //
	//==============//

	/** @brief Default constructor
	 *
	 *	Constructor of class
	 *
	 *	@remarks Is used by derived objects.
	 */
	acs_usa_errorimpl();


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	virtual ~acs_usa_errorimpl();


	//===========//
	// Functions //
	//===========//

	/** @brief
	 *	Gets the error type value set by a previous setError.<br>
	 *  The user of this base class should provide descriptions and definitions of these values.<br>
	 *
	 *	@return 	ACS_USA_ErrorReturnType.
	 *	@remarks	-
	 */
	ACS_USA_ErrorType getError() const;


	/** @brief
	 *	Gets an error text string string set by a previous setErrorText.<br>
	 *  The user of the Error base class should provide descriptions and definitions of these strings.<br>
	 *
	 *	@return 	A pointer to an error text string.
	 *	@remarks 	-
	 */
	const char* getErrorText() const;


	/** @brief
	 *	Sets an error type value supplied by the user of this class.<br>
	 *  The user of this base class method should provide descriptions and definitions of these values.<br>
	 *
	 *  @param[in] 	errorType: The error type value provided by user.
	 *	@return 	-
	 *	@remarks 	-
	 */
	void setError(const ACS_USA_ErrorType errorType);


	/** @brief
	 *
	 *	Sets an error string describing the error.<br>
	 *  This method is typically used at the lowest level to be retrieved by the second lowest level in the function hierarchy.
	 *  (Using getErrorText)<br>
	 *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText. <br>
	 *
	 *  @param[in] 	errorMessage: The error message string.
	 *	@return 	-
	 *	@remarks 	-
	 */
	void setErrorText(const char errorMessage[]);


	/** @brief
	 *
	 *	Sets an error string describing the error and where it occurred.<br>
	 *  The first argument is used to describe in which higher level API, object or function unit the error occurred.<br>
	 *  A " : " string is added to the prefix. Then the error string follows. <br>
	 *  Used typically in situations when no value is needed, see below. An example string will look as follows:<br>
	 *  "ACS_USA_EvReport : ThisIsASeriousError string".<br>
	 *  <br>
	 *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText.
	 *
	 *
	 *  @param[in] 	prefix: The string describing in which unit the error occurred.
	 *  @param[in] 	errorMessage: The error message string.
	 *	@return 	-
	 *	@remarks 	-
	 */
	void setErrorText(const char prefix[], const char errorMessage[]);


	/** @brief
	 *
	 *	Sets an error string describing the error, where it occurred and to what value.<br>
	 *  The first argument is used to describe in which higher level API, object or function unit the error occurred. .<br>
	 *  A " : " string is added to the prefix. Then the error string follows. The last argument string is used to indicate some value.<br>
	 *  The last argument is preceded by a dash ( " - " ). <br>
	 *  An example string can be as follows:<br>
	 *  "ACS_FCH_EvReport : ThisIsASeriousError string - AH4"<br>
	 *  <br>
	 *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText.
	 *
	 *
	 *  @param[in] 	prefix: The string describing in which unit the error occurred.
	 *  @param[in] 	errorMessage: The error message string.
	 *  @param[in] 	suffix: The value useful in identifying the problem.
	 *	@return 	-
	 *	@remarks 	-
	 */
	void setErrorText(const char prefix[], const char errorMessage[], const char suffix[]);


	/** @brief
	 *
	 *	Sets an error string describing the error, where it occurred and to what value.<br>
	 *  The first argument is used to describe in which higher level API, object or function unit the error occurred.<br>
	 *  A " : " string is added to the prefix. Then the error string follows. The last argument which is a long integer is
	 *  used to indicate some value.<br>
	 *  The last argument is preceded by a dash ( " - " ) when it is transformed into a string. <br>
	 *  An example string can be as follows:<br>
	 *  "ACS_FCH_EvReport : ThisIsASeriousError string - 100006"<br>
	 *  <br>
	 *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText.
	 *
	 *
	 *  @param[in] 	prefix: The string describing in which unit the error occurred.
	 *  @param[in] 	errorMessage: The error message string.
	 *  @param[in] 	suffix: The value useful in identifying the problem.
	 *	@return 	-
	 *	@remarks 	-
	 */
	void setErrorText(const char prefix[], const char errorMessage[], const long suffix);


protected:
	static const char prefix_[];	// Used to specify where the error occurred.

private:
	ACS_USA_ErrorType errorId_;		// The error type value to be set and retreived. The initial value
									// is set to ACS_FCH_noErrorType.

	std::string errorText_;	  		// The error text string to be  set and retreived. The initial
									// value is set to ACS_FCH_noError.

	static const char dash_[];		// Used to compose error string

	static const char colon_[];		// Used to compose error string
};

}
#endif

