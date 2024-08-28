
//******************************************************************************
// 
// .NAME
//  	ACS_USA_Error - USA error handling
// .LIBRARY 3C++
// .PAGENAME <CLASSNAME>
// .HEADER  TMOS/ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	This class handles setting error codes and error texts
//	for AP Event reporting.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR 
// 	1995-08-15 by ETX/TX/T	XKKHEIN

// .REVISION
//	PA2	990915

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO	DATE 	NAME	DESCRIPTION
//	PA1		950829	XKKHEIN	First Revision
//  PA2		990915	UABDMT	Ported to Windows NT4.0

// .LINKAGE
//	

// .SEE ALSO 
//	ACS_AEH_Error

//******************************************************************************

#ifndef ACS_USA_ERROR_H 
#define ACS_USA_ERROR_H

#include <ACS_AEH_Error.H>

typedef ACS_AEH_ErrorType ACS_USA_ErrorType;

//******************************************************************************
// Error codes in USA
//******************************************************************************
const	ACS_USA_ErrorType	ACS_USA_NoError						= 0;
const   ACS_USA_ErrorType	ACS_USA_SyntaxError 				= 1;
const	ACS_USA_ErrorType	ACS_USA_SystemCallError 			= 3;
const	ACS_USA_ErrorType	ACS_USA_BadState 					= 4;
const	ACS_USA_ErrorType	ACS_USA_Memory 						= 5;
const	ACS_USA_ErrorType	ACS_USA_TimeStampMatch 				= 6;	
const	ACS_USA_ErrorType	ACS_USA_ErrorTimeStampFormat 		= 7; 
const	ACS_USA_ErrorType	ACS_USA_CannotExtractTime 			= 8;
const	ACS_USA_ErrorType	ACS_USA_WarnTimeStampFormat			= 9;
const	ACS_USA_ErrorType	ACS_USA_QueueExceeded				= 10;
const	ACS_USA_ErrorType	ACS_USA_APeventHandlingError		= 11;

const	ACS_USA_ErrorType	ACS_USA_FileIOError 				= 1;
const	ACS_USA_ErrorType	ACS_USA_ParameterHandlingError		= 1;
// Changed 990706 due to TR HB53440. Supposed to send 8701 alarm 
// upon parameter and IO problems, The definitions used to be:
// const	ACS_USA_ErrorType	ACS_USA_FileIOError 			= 2;
// const	ACS_USA_ErrorType	ACS_USA_ParameterHandlingError	= 12;

const	ACS_USA_ErrorType	ACS_USA_TmpFileIOError 				= 2;
// Changed due to TR HB95441. USA.TMP file errors should not cause
// USA to be stoped by the cluster. (no 8701 alarm)

const	ACS_USA_ErrorType	ACS_USA_ServiceError				= 13;
const	ACS_USA_ErrorType	ACS_USA_EventFrequencyExceeded		= 14;
const	ACS_USA_ErrorType	ACS_USA_SecurityLogCleared			= 15;															 

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
const char* const ACS_USA_IllegalName = "Illegal characters in name";
const char* const ACS_USA_UnrecognizedParameter = "Unrecognized parameter";
const char* const ACS_USA_NoValidObject = "No valid Analysis Objects";
const char* const ACS_USA_IllegalList = "Illegal list definition";

const char* const ACS_USA_RegistryOpenError = "Cannot open specified key in Registry";
const char* const ACS_USA_RegistryReadError = "Cannot query Registry about specified value";
const char* const ACS_USA_EnvironmentVariableReadError = "Cannot read AP_HOME environment variable";
const char* const ACS_USA_EventFrequencyError = "Too high event frequency. USA might fail detecting errors between latest boot and present time.";
const char* const ACS_USA_BootEventError = "No Boot Event found.";
const char* const ACS_USA_ClearedLogEvent = "The Security log has been manually cleared. No Boot Event found. Ignoring events older than the time of when the Security log was cleared.";

//Forward declaration(s)

//Member functions, constructors, destructors, operators

class ACS_USA_Error : public ACS_AEH_Error
{

 public:

  ACS_USA_Error();
  // Description:
  // 	Default constructor
  // Parameters: 
  //    none 
  // Return value: 
  //    none 
  // Additional information:
  //     
  
  virtual ~ACS_USA_Error();
  // Description:
  // 	Destructor
  // Parameters: 
  //    none 
  // Return value: 
  //    none 
  // Additional information:
  //     

  const char* getProblemText(const ACS_USA_ErrorType idx) const;
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.H 
  // Return value: 
  //    none 
  // Additional information:
  //     
  
  void setError(const ACS_AEH_ErrorType errorType);
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.H 
  // Return value: 
  //    none 
  // Additional information:
  //     
      
  void setErrorText(const char* const errorMessage);
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.h
  // Return value: 
  //    none 
  // Additional information:
  //     

  void setErrorText(const char* const prefix,
		    const char* const errorMessage);
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.H 
  // Return value: 
  //    none 
  // Additional information:
  //     
                        
  void setErrorText(const char* const prefix,
		    const char* const errorMessage,
		    const char* const suffix);
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.H 
  // Return value: 
  //    none 
  // Additional information:
  //     	
	
  void setErrorText(const char* const prefix,
		    const char* const errorMessage,
		    const long suffix);
  // Description:
  // 	Calls appropriate ACS_AEH_Error method
  // Parameters: 
  //    See descrition in ACS_AEH_Error.H 
  // Return value: 
  //    none 
  // Additional information:
  //     

};


//******************************************************************************
//	setError()
//******************************************************************************
inline
void
ACS_USA_Error::setError(const ACS_AEH_ErrorType errorType)
{
    ACS_AEH_Error::setError(errorType);
}



//******************************************************************************
//	setErrorText()
//******************************************************************************
inline
void
ACS_USA_Error::setErrorText(const char* const errorMessage)
{
    ACS_AEH_Error::setErrorText(errorMessage);
}



//******************************************************************************
//	setErrorText()
//******************************************************************************
inline
void
ACS_USA_Error::setErrorText(const char* const prefix,
			    const char* const errorMessage)
{
    ACS_AEH_Error::setErrorText(prefix, errorMessage);
}




//******************************************************************************
//	setErrorText()
//******************************************************************************
inline
void
ACS_USA_Error::setErrorText(const char* const prefix,
			    const char* const errorMessage,
			    const char* const suffix)
{
    ACS_AEH_Error::setErrorText(prefix, errorMessage, suffix);
}



//******************************************************************************
//	setErrorText()
//******************************************************************************
inline
void
ACS_USA_Error::setErrorText(
		    const char* const prefix,
		    const char* const errorMessage,
		    const long suffix)
{
    ACS_AEH_Error::setErrorText(prefix, errorMessage, suffix); 
}

extern ACS_USA_Error error;	// The global error handling object in USA


#endif
