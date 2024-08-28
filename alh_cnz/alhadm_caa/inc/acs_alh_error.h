/*
 * acs_alh_error.h
 *
 *  Created on: Nov 3, 2011
 *      Author: efabron
 */

#ifndef ACS_ALH_ERROR_H_
#define ACS_ALH_ERROR_H_

#include <string>

//========================================================================================
//	Constants to be retrieved by the getErrorText method.
//========================================================================================

const char ACS_ALH_noError[] = "No Error";		// The initial string set before first setErrorText call.



class acs_alh_error
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
	acs_alh_error();


	/** @brief  destructor.
	 *
	 *	The destructor of the class.
	 *
	 *	@remarks -
	 */
	virtual ~acs_alh_error();


	//===========//
	// Functions //
	//===========//

	/** @brief
	 *	Gets the error type value set by a previous setError.<br>
	 *  The user of this base class should provide descriptions and definitions of these values.<br>
	 *
	 *	@return 	int
	 *	@remarks	-
	 */
	int getError() const;


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
	void setError(const int errorType);


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



private:
	int errorId_;					// The error type value to be set and retreived. The initial value
									// is set to ACS_FCH_noErrorType.

	std::string errorText_;	  		// The error text string to be  set and retreived. The initial
									// value is set to ACS_FCH_noError.

};

#endif /* ACS_ALH_ERROR_H_ */
