//******************************************************************************
//
// NAME
//      ACS_USA_Error - USA error handling
//
// COPYRIGHT
//      Ericsson AB 2005-2006 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
// 	    This class handles setting error codes and error texts
//      for AP Event reporting.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    1995-08-15 by ETX/TX/T XKKHEIN
//
// REV  DATE    NAME     DESCRIPTION
// A    950829  XKKHEIN  First version.
// B    990915  UABDMT   Ported to Windows NT4.0.
// C    060426  UABPEK   ACS_USA_EventStringError added.
//
//******************************************************************************

#ifndef ACS_USA_ERROR_H 
#define ACS_USA_ERROR_H

#include <acs_usa_types.h>

const long ACS_USA_FirstErrorCode = 8700;

//******************************************************************************
// Error strings
//******************************************************************************
const char* const noErrorText	 = "";
const char* const beginQuote	 = " '";
const char* const endQuote	 = "' ";

const char* const Sys_systemCall = "system";
const char* const Sys_mktimeCall = "mktime";
const char* const Sys_timeCall   = "time";

const char* const syntaxErrorPrefix = ": Syntax error";
const char* const lineNumberPrefix  = " on line ";
const char* const ioErrorPrefix     = ": I/O error";
const char* const sysErrorPrefix    = "System error";
				       
const char* const ACS_USA_ImmInitFailed = "IMM Initialization Failed";
const char* const ACS_USA_systemCallResult = "System call failed. Error from system";
const char* const ACS_USA_ReadFailed 	 = "Read from file failed";
const char* const ACS_USA_CannotOpenFile = "Cannot open file";
const char* const ACS_USA_CannotEraseFile = "Cannot erase temporary file";
const char* const ACS_USA_WriteFailed     = "Write failed";
const char* const ACS_USA_MissingVitalParameter = "Missing parameter(s)";
const char* const ACS_USA_MultipleNames = "Multiple Names in ACF";
const char* const ACS_USA_UndefinedListMember = "Undefined list member";
const char* const ACS_USA_unknownParameter = "Unknown parameter";
const char* const ACS_USA_UnknownReference = "A criterion refers to unknown Analysis Object";
const char* const ACS_USA_MissingObjectReference = "Missing Analysis Object reference";
const char* const ACS_USA_MissingCommandReference = "A criterion refers to unknown command";
const char* const ACS_USA_NoCriteria = "No criteria specified";
const char* const ACS_USA_NoAnalysisObjects = "No Analysis Objects specified";
const char* const ACS_USA_MissingCommand = "Command not specified";
const char* const ACS_USA_IllegalValue = "Illegal value";
const char* const ACS_USA_NoMemory = "Memory problems";
const char* const ACS_USA_QueueOverflow = "Event queue size exceeded maximum";
const char* const ACS_USA_EventHandlingError = " Cannot deliver events: ";
const char* const ACS_USA_MissingFormatParameter = "Missing needed parameter in TS format";
const char* const ACS_USA_MissingParameterValue = "Missing parameter value";
const char* const ACS_USA_QuoteImbalance = "Quote imbalance";
const char* const ACS_USA_IllegalMatchType = "Illegal MatchType Found ";
const char* const ACS_USA_IllegalServerity = "Illegal Perceived Severity Supplied";
const char* const ACS_USA_IllegalProbableCause = "Illegal Prabable Cause Supplied";
const char* const ACS_USA_UnrecognizedParameter = "Unrecognized parameter";
const char* const ACS_USA_NoValidObject = "No valid Analysis Objects";
const char* const ACS_USA_IllegalList = "Illegal list definition";
const char* const ACS_USA_EnvironmentVariableReadError = "Cannot read AP_HOME environment variable";
const char* const ACS_USA_EventFrequencyError = "Too high event frequency. USA might fail detecting errors between latest boot and present time.";
const char* const ACS_USA_BootEventError = "No Boot Event found.";
const char* const ACS_USA_ClearedLogEvent = "The Security log has been manually cleared. No Boot Event found. Ignoring events older than the time of when the Security log was cleared.";

namespace ACS_USA {
	class acs_usa_errorimpl;
}	

//==============================================================================
//      Class declaration
//
//      NOTE - The size of the class must be retained in order to keep it
//      backwards compatible.
//==============================================================================

class acs_usa_error
{
public:

        //==============//
        // Constructors //
        //==============//

        /** @brief Default constructor
         *
         *      Constructor of class
         *
         *      @remarks Is used by derived objects.
         */
        acs_usa_error();


        /** @brief  Copy constructor
         *
         *      Copy constructor
         *
         *      @param[in]      error ACS_USA_Error object
         *      @remarks        -
         */
        acs_usa_error(const acs_usa_error& error);


        /** @brief  destructor.
         *
         *      The destructor of the class.
         *
         *      @remarks -
         */
        virtual ~acs_usa_error();


        /** @brief  Assignment operator.
         *      Assignment operator.
         *
         *      @param[in]      error: an ACS_USA_Error object
         *      @return         Reference to self
         *      @remarks        -
         */
        acs_usa_error& operator=(const acs_usa_error& error);


        //===========//
        // Functions //
        //===========//

	/** @brief
         *      Gets the error type value set by a previous setError.<br>
         *  The user of this base class should provide descriptions and definitions of these values.<br>
         *
         *      @return         ACS_USA_ErrorType.
         *      @remarks        -
         */
        ACS_USA_ErrorType getError() const;


        /** @brief
         *      Gets an error text string set by a previous setErrorText.<br>
         *  The user of the Error base class should provide descriptions and definitions of these strings.<br>
         *
         *      @return         A pointer to an error text string.
         *      @remarks        -
         */
        const char* getErrorText() const;

//protected:

        /** @brief
         *      Sets an error type value supplied by the user of this class.<br>
         *  The user of this base class method should provide descriptions and definitions of these values.<br>
         *
         *  @param[in]  errorType: The error type value provided by user.
         *      @return         -
         *      @remarks        -
         */
        void setError(const ACS_USA_ErrorType errorType);


        /** @brief
         *
         *  Sets an error string describing the error.<br>
         *  This method is typically used at the lowest level to be retrieved by the second lowest level in the function hierarchy.
         *  (Using getErrorText)<br>
         *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText. <br>
         *
         *  @param[in]  errorMessage: The error message string.
         *      @return         -
         *      @remarks        -
         */
        void setErrorText(const char errorMessage[]);


        /** @brief
         *
         *  Sets an error string describing the error and where it occurred.<br>
         *  The first argument is used to describe in which higher level API, object or function unit the error occurred.<br>
         *  A " : " string is added to the prefix. Then the error string follows. <br>
         *  Used typically in situations when no value is needed, see below. An example string will look as follows:<br>
         *  "ACS_FCH_EvReport : ThisIsASeriousError string".<br>
         *  <br>
         *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText.
         *
         *  @param[in]  prefix: The string describing in which unit the error occurred.
         *  @param[in]  errorMessage: The error message string.
         *      @return         -
         *      @remarks        -
         */
        void setErrorText(const char prefix[], const char errorMessage[]);


        /** @brief
         *
         *      Sets an error string describing the error, where it occurred and to what value.<br>
         *  The first argument is used to describe in which higher level API, object or function unit the error occurred. .<br>
         *  A " : " string is added to the prefix. Then the error string follows. The last argument string is used to indicate some value.<br>
         *  The last argument is preceded by a dash ( " - " ). <br>
         *  An example string can be as follows:<br>
         *  "ACS_FCH_EvReport : ThisIsASeriousError string - AH4"<br>
         *  <br>
         *  The call to setErrorText will delete and replace any error string set by a previous call to setErrorText.
         *
         *
         *  @param[in]  prefix: The string describing in which unit the error occurred.
         *  @param[in]  errorMessage: The error message string.
         *  @param[in]  suffix: The value useful in identifying the problem.
         *      @return         -
         *      @remarks        -
         */
        void setErrorText(const char prefix[],
                                          const char errorMessage[],
                                          const char suffix[]);


        /** @brief
         *
         *      Sets an error string describing the error, where it occurred and to what value.<br>
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
         *  @param[in]  prefix: The string describing in which unit the error occurred.
         *  @param[in]  errorMessage: The error message string.
         *  @param[in]  suffix: The value useful in identifying the problem.
         *      @return         -
         *      @remarks        -
         */
        void setErrorText(const char prefix[],
                                          const char errorMessage[],
                                          const long suffix);

	const char* getProblemText(const ACS_USA_ErrorType idx) const;

private:
        //========//
        // Fields //
        //========//

        ACS_USA::acs_usa_errorimpl* errorImplp_;        // Pointer to error implementation class


};


extern acs_usa_error error;

#endif


