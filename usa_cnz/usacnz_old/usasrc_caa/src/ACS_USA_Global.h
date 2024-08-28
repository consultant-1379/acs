//******************************************************************************
//
// .NAME 
//      ACS_USA_Global - Common constants and typedefs used in USA.
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Global
// .HEADER  ACS
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_Global.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2005.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//      Common constants and typedefs used in USA.

// .ERROR HANDLING
//
//      General rule:
//	    The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	    190 89-CAA 109 0259

// AUTHOR
//      1995-08-01 by ETX/TX/T XKKHEIN

// REV  DATE    NAME     DESCRIPTION
// A		950829	XKKHEIN	 First version.
// B		990915	UABDMT	 Ported to Windows NT4.0.
// C    030212  UABPEK   New AP VIRUS alarms.
// D    040219  UABPEK   AP LOG STATISTICS changed to ILLEGAL LOGON ATTEMPTS.
// E    050918  UABPEK   Added probable cause AP EXTERNAL NETWORK FAULT.

// .LINKAGE
//

// .SEE ALSO
//

//******************************************************************************

#ifndef ACS_USA_GLOBAL_H
#define ACS_USA_GLOBAL_H

#include "ACS_USA_Error.h"
#include "ACS_USA_Debug.h"

//******************************************************************************
// Various types used in USA classes
//******************************************************************************

typedef long     ACS_USA_TimeType;        // Time used in USA
typedef long 		 ACS_USA_SpecificProblem; // Specific problem
typedef unsigned ACS_USA_ByteCount;       // type for counting bytes
typedef	unsigned ACS_USA_CriteriaCount;	  // Number of criteria
typedef	int 		 ACS_USA_Index;			      // Index type
typedef int      ACS_USA_Flag;            // flag type
typedef int      ACS_USA_StatusType;      // Type returned from .isValid()
typedef int			 ACS_USA_AnalysisType;	  // Type for analysis
typedef int			 ACS_USA_Boolean;		      // Boolean type
typedef char     ACS_USA_TimeStampFormat; // Time stamp format
typedef char     ACS_USA_Expression;      // Type for regular expressions
typedef char*    ACS_USA_RecordType;      // Type for log records
typedef int			 ACS_USA_ActivationType;  // Type for activation

typedef unsigned int	ACS_USA_NodeState;	// For active or passive node


//******************************************************************************
// Constants used in Event Reporting
//******************************************************************************
const ACS_USA_SpecificProblem ACS_USA_defaultSpecificProblem = 8799L;
 
const char* const ACS_USA_APAntivirusFunctionFault = "AP ANTIVIRUS FUNCTION FAULT";
const char* const ACS_USA_APExternalNetworkFault   = "AP EXTERNAL NETWORK FAULT"; 
const char* const ACS_USA_APFault                  = "AP FAULT"; 
const char* const ACS_USA_APVirus                  = "AP VIRUS";
const char* const ACS_USA_IllegalLogonAttempts     = "ILLEGAL LOGON ATTEMPTS";

const char* const ACS_USA_DiagnosticFault = "AP DIAGNOSTIC FAULT";
const char* const ACS_USA_FaultCause 	    = "AP INTERNAL FAULT";

const char* const ACS_USA_processName            = "ACS_USA_SyslogAnalyser";
const char* const ACS_USA_objectClassOfReference = "APZ";

const char* const	Key_perceivedSeverity_A1		  = "A1";
const char* const	Key_perceivedSeverity_A2		  = "A2";
const char* const	Key_perceivedSeverity_A3		  = "A3";
const char* const	Key_perceivedSeverity_O1		  = "O1";
const char* const	Key_perceivedSeverity_O2		  = "O2";
const char* const	Key_perceivedSeverity_CEASING = "CEASING";
const char* const	Key_perceivedSeverity_EVENT	  = "EVENT";
const char* const	Key_perceivedSeverity_NONE	  = "NONE";

const long minSpecificProblem = 8700L; // Minimum value of specific problem
const long maxSpecificProblem = 8799L; // Maximum value of specific problem

//******************************************************************************
// Node state types
//******************************************************************************
const ACS_USA_NodeState		ACS_USA_activeNodeState	 = 1;
const ACS_USA_NodeState		ACS_USA_passiveNodeState = 2;


//******************************************************************************
// Analysis types
//******************************************************************************
const	ACS_USA_AnalysisType    invalidType = -1;// Invalid analysis type
const	ACS_USA_AnalysisType    shortTerm = 0;	// Short term analysis
const	ACS_USA_AnalysisType    longTerm = 1;	// Long term analysis

const	ACS_USA_AnalysisType    shortInstant = 2;	// Short instant analysis,
													// used for notification of 
													// changes in event log

//******************************************************************************
// Status returned from various functions
//******************************************************************************
const   ACS_USA_StatusType      ACS_USA_error = 0;      // An object is invalid
const   ACS_USA_StatusType      ACS_USA_ok    = 1;      // An object is OK

const   ACS_USA_StatusType      ACS_USA_ok_conditional  // An object is 
											  = 3;		// conditionally OK

//******************************************************************************
// TRUE/FALSE for comparison in conditional statements
//******************************************************************************
const   ACS_USA_Boolean	ACS_USA_False   = 0;			// False
const   ACS_USA_Boolean	ACS_USA_True    = 1;    		// True


//******************************************************************************
// Flags for activation reason of USA
//******************************************************************************
const	ACS_USA_ActivationType	Activation_shortTerm = 1;	// short term
const	ACS_USA_ActivationType	Activation_longTerm  = 2;	// long term

const	ACS_USA_ActivationType	
						Activation_instantShortTerm	= 3;	// Instant short term
const	ACS_USA_ActivationType								// Start up analysis -
						Activation_startUp			= 4;	// look for the latest	
															// boot event.
const	ACS_USA_ActivationType 
						Activation_FirstShortTerm	= 5;	// First short term
const	ACS_USA_ActivationType 
						Activation_FirstLongTerm	= 6;	// First long term

//******************************************************************************
// Some vital characters
//******************************************************************************
const	char	endOfStringChar = '\0';		// End of string
const	char	newLineChar	= '\n';		// New line character


//******************************************************************************
// Various constants
//******************************************************************************
const	int systemCallError = -1;			// returned from system call(s)
const	int	ACS_USA_exitOK = 0;				// exit ok
const 	int	ACS_USA_exitError = -1;			// error exit
const 	ACS_USA_TimeType  initialTime  = 0;	// Initial time

const char ACS_USA_applicationKey[] = 
		"SOFTWARE\\Ericsson\\Adjunct Processor\\ACS\\USA"; 

#endif
