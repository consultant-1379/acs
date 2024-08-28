/*
 *	acs_usa_types.h
 *	Created on: 
 *	Author: 
 */

// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//                      2018-03-16      XFURULL         Fix for TR HW69050 
//******************************************************************************
#ifndef ACS_USA_TYPES_H
#define ACS_USA_TYPES_H


#include "ACS_APGCC_Util.H"

//******************************************************************************
// Boolean delcaration
//******************************************************************************

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//******************************************************************************
// constant delcaration
//******************************************************************************
 
#define ACS_USA_APFAULT "AP FAULT"
#define ACS_USA_DIAGFAULT "AP DIAGNOSTIC FAULT"
#define ACS_USA_INTLFAULT "AP INTERNAL FAULT"
#define ACS_USA_ILLGAL_LOGON_ATTEMPTS "ILLEGAL LOGON ATTEMPTS"
#define ACS_USA_EXTERNAL_NETWK_FAULT "AP EXTERNAL NETWORK FAULT"
#define ACS_USA_AP_PROCESS_STOPPED "AP PROCESS STOPPED"
#define ACS_USA_AP_FUNCTION_NOT_AVAILABLE "AP FUNCTION NOT AVAILABLE"
#define ACS_USA_NTPSERVER_FAULT "NTP SERVER NOT CONFIGURED"
#define ACS_USA_AP_LDAP_COMMUNICATION_FAULT "AP LDAP COMMUNICATION FAULT"
#define ACS_USA_SPECIF_PROBELM_MIN 8700L
#define ACS_USA_SPECIF_PROBELM_MAX 8799L
#define ACS_USA_NTP_SPECIFIC_PROBLEM 8724

//constant declaration related to ntf subscription
#define ACS_USA_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#define AP_NODE_HOSTNAME_FILEPATH "/etc/cluster/nodes/this/hostname"
#define AP_NODE_ID_FILEPATH "/etc/cluster/nodes/this/id"
#define AP_OTHER_NODE_ID_FILEPATH  "/etc/cluster/nodes/peer/id"

//========================================================================================
// The allowed values for perceived severity
//========================================================================================

#ifndef ACS_USA_PERCEIVED_SEVERITY_A1
#define ACS_USA_PERCEIVED_SEVERITY_A1   "A1"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_A2
#define ACS_USA_PERCEIVED_SEVERITY_A2   "A2"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_A3
#define ACS_USA_PERCEIVED_SEVERITY_A3   "A3"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_O1
#define ACS_USA_PERCEIVED_SEVERITY_O1   "O1"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_O2
#define ACS_USA_PERCEIVED_SEVERITY_O2   "O2"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_CEASING
#define ACS_USA_PERCEIVED_SEVERITY_CEASING      "CEASING"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_EVENT
#define ACS_USA_PERCEIVED_SEVERITY_EVENT        "EVENT"
#endif

#ifndef ACS_USA_PERCEIVED_SEVERITY_NONE
#define ACS_USA_PERCEIVED_SEVERITY_NONE        "NONE"
#endif


//========================================================================================
//	The allowed values for object class of reference
//========================================================================================

#ifndef ACS_USA_OBJ_CLASS_REFERENZE_APZ
#define ACS_USA_OBJ_CLASS_REFERENZE_APZ         "APZ"
#endif

#ifndef ACS_USA_OBJ_CLASS_REFERENZE_EVENT
#define ACS_USA_OBJ_CLASS_REFERENZE_EVENT       "EVENT"
#endif

//========================================================================================
//	Typedef declarations for return type
//========================================================================================

typedef enum{

	ACS_USA_Ok=0,
	ACS_USA_Error=1

}ACS_USA_ReturnType;

typedef enum{

	SHRT_TIMEOUT=0,
	ZMBE_TIMEOUT=1,
	LDAG_TIMEOUT=2,
	ZMBE_CLOSE=3,
	LDAG_CLOSE=4,
	SHRT_CLOSE=5,
	USA_SHUTDOWN=6,
	APRB_TIMEOUT=7,
	APRB_CLOSE=8,
	IMPL_TIMEOUT=9,
	NTF_TIMEOUT=10,
	NTF_CLOSE=11
		
}ACS_USA_Msgs;

typedef std::string String;
//========================================================================================
//      constant declarations 
//========================================================================================

typedef enum{
	ACS_USA_InvalidType = -1,
	ACS_USA_Startup_Analysis=0,
	ACS_USA_ShortTerm_Analysis=1,
	ACS_USA_Instant_Analysis=2
}ACS_USA_AnalysisType;

enum NodeStateConstants {
	NODE_STATE_UNDEFINED = 0,
	NODE_STATE_ACTIVE = 1,
	NODE_STATE_PASSIVE = 2
};


//========================================================================================
//      error declarations 
//========================================================================================
//========================================================================================
//      Constant error types
//========================================================================================

typedef enum {

        ACS_USA_NoError =0,
        ACS_USA_FileIOError=1,
        ACS_USA_ParameterHandlingError=1,
        ACS_USA_SyntaxError=1,
        ACS_USA_TmpFileIOError,
        ACS_USA_SystemCallError,
        ACS_USA_BadState,
        ACS_USA_Memory,
        ACS_USA_TimeStampMatch,
        ACS_USA_ErrorTimeStampFormat,
        ACS_USA_CannotExtractTime,
        ACS_USA_WarnTimeStampFormat,
        ACS_USA_QueueExceeded,
        ACS_USA_APeventHandlingError,
        ACS_USA_EventStringError,
        ACS_USA_ServiceError,
        ACS_USA_EventFrequencyExceeded,
        ACS_USA_SecurityLogCleared

}ACS_USA_ErrorType;

//========================================================================================
//      constant declarations 
//========================================================================================
typedef time_t 		ACS_USA_TimeType;
typedef long 		ACS_USA_SpecificProblem;
typedef unsigned 	ACS_USA_CriteriaCount;
typedef int		ACS_USA_Index;
typedef int     	ACS_USA_Flag;
typedef int 		ACS_USA_Boolean;
typedef char 		ACS_USA_Expression;
typedef char*    	ACS_USA_RecordType;
typedef unsigned int	ACS_USA_NodeStatee;
typedef unsigned 	ACS_USA_ByteCount;       // type for counting bytes
typedef char     	ACS_USA_TimeStampFormat;
typedef char    	ACS_USA_RecordBuffer;// Record buffering

//========================================================================================
//      constant declarations 
//========================================================================================
const char* const ACS_USA_processName = "acs_usad";
const char* const ACS_USA_ntpProblemText = "PROBLEM\nNTP SERVER";
const char* const ACS_USA_ntpProblemData = "External NTP server not configured";
const ACS_USA_SpecificProblem ACS_USA_defaultSpecificProblem = 8799L;
static const string IMPLEMENTER_NTP = "@ACS_USA_NtpSrvM";
static const string classNtpServer = "LdeNtpServerConf";

//******************************************************************************
// TRUE/FALSE for comparison in conditional statements
//******************************************************************************
const   ACS_USA_Boolean ACS_USA_False   = 0;                    // False
const   ACS_USA_Boolean ACS_USA_True    = 1;                    // True

//========================================================================================
//      constant declarations 
//========================================================================================
const   int 			systemCallError = -1;       
const   int     		ACS_USA_exitOK = 0;
const   int     		ACS_USA_exitError = -1;
const   ACS_USA_TimeType  	initialTime  = 0;

//========================================================================================
//      constant declarations 
//========================================================================================

//******************************************************************************
// Flags for activation reason of USA
//******************************************************************************
typedef int ACS_USA_ActivationType;

const   ACS_USA_ActivationType  Activation_shortTerm 		= 1;    // short term
const   ACS_USA_ActivationType  Activation_longTerm  		= 2;    // long term
const   ACS_USA_ActivationType  Activation_instantShortTerm     = 3;    // Instant short term
const   ACS_USA_ActivationType  Activation_startUp  		= 4;    // Start up analysis -
const   ACS_USA_ActivationType  Activation_FirstShortTerm       = 5;    // First short term
const   ACS_USA_ActivationType  Activation_FirstLongTerm        = 6;    // First long term


#endif
