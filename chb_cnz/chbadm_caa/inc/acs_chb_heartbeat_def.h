/*=================================================================== */
/**
   @file   acs_chb_heartbeat_def.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/01/2011   XNADNAR   Initial Release
*/
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */


#ifndef ACS_CHB_HEARTBEATDEF_H
#define ACS_CHB_HEARTBEATDEF_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include<ace/ACE.h>


/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief CPOBJECT_FROM_HEARTBEAT
 */
/*=================================================================== */


const unsigned char CPOBJECT_FROM_HEARTBEAT             = 0x02;
/*=================================================================== */
/**
   @brief TERMINATE_FROM_HEARTBEAT
 */
/*=================================================================== */
const unsigned char TERMINATE_FROM_HEARTBEAT            = 0x03;
/*=================================================================== */
/**
   @brief TEXT_TERMINATE_FROM_HEARTBEAT
		  Terminate request string from HeartBeat.
 */
/*=================================================================== */
const char* const TEXT_TERMINATE_FROM_HEARTBEAT         = "[TERMINATE]";
/*=================================================================== */
/**
   @brief TEXT_CP_OBJECT_FROM_HEARTBEAT
		  CP object sent from HeartBeat.
 */
/*=================================================================== */
const char* const TEXT_CP_OBJECT_FROM_HEARTBEAT         = "[CPOBJECT]";
/*=================================================================== */
/**
   @brief MAX_LENGTH_FOR_REQUEST_FROM_HEARTBEAT
		  Max allowed number of characters eq. 100
 */
/*=================================================================== */
const int MAX_LENGTH_FOR_REQUEST_FROM_HEARTBEAT         = 1024;
/*=================================================================== */
/**
   @brief ACS_CHB_HEARTBEAT
		  Normal HeartBeat.
 */
/*=================================================================== */
const int ACS_CHB_HEARTBEAT     = 0;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ_TYPE
		  The type of APZ
 */
/*=================================================================== */
const int ACS_CHB_APZ_TYPE      = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ210
 */
/*=================================================================== */
const int   ACS_CHB_APZ210      = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ211
 */
/*=================================================================== */
const int   ACS_CHB_APZ211      = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ212
 */
/*=================================================================== */
const int   ACS_CHB_APZ212      = 3;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ213
 */
/*=================================================================== */
const int   ACS_CHB_APZ213      = 4;
/*=================================================================== */
/**
   @brief ACS_CHB_APZ214
 */
/*=================================================================== */
const int   ACS_CHB_APZ214      = 5;
/*=================================================================== */
/**
   @brief ACS_CHB_EX_SIDE
		  The side of the APZ that is executable
 */
/*=================================================================== */
const int ACS_CHB_EX_SIDE       = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_SIDE_A
 */
/*=================================================================== */
const int   ACS_CHB_SIDE_A      = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_SIDE_B
 */
/*=================================================================== */
const int   ACS_CHB_SIDE_B      = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_SB_STATUS
		  The status of the standby side.
 */
/*=================================================================== */
const int ACS_CHB_SB_STATUS     = 3;
/*=================================================================== */
/**
   @brief ACS_CHB_SBSE
 */
/*=================================================================== */
const int   ACS_CHB_SBSE        = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_SBHA
 */
/*=================================================================== */
const int   ACS_CHB_SBHA        = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_SBUP
 */
/*=================================================================== */
const int   ACS_CHB_SBUP        = 3;
/*=================================================================== */
/**
   @brief ACS_CHB_SBWO
 */
/*=================================================================== */
const int   ACS_CHB_SBWO        = 4;
/*=================================================================== */
/**
   @brief ACS_CHB_NOINFO
 */
/*=================================================================== */
const int   ACS_CHB_NOINFO      = 5;
/*=================================================================== */
/**
   @brief ACS_CHB_CP_STATUS
		  the status of the EX side.
 */
/*=================================================================== */
const int ACS_CHB_CP_STATUS     = 4;
/*=================================================================== */
/**
   @brief ACS_CHB_CP_PARWO
 */
/*=================================================================== */
const int   ACS_CHB_CP_PARWO    = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_CP_SINEX
 */
/*=================================================================== */
const int   ACS_CHB_CP_SINEX    = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_CP_SBSE
 */
/*=================================================================== */
const int   ACS_CHB_CP_SBSE    = 3;
/*=================================================================== */
/**
   @brief ACS_CHB_CP_CONNECTION
		  The status of the CP-AP connection
          for object service.
 */
/*=================================================================== */
const int ACS_CHB_CP_CONNECTION = 5;
/*=================================================================== */
/**
   @brief ACS_CHB_CONNECTED
 */
/*=================================================================== */
const int   ACS_CHB_CONNECTED   = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_NOT_CONNECTED
 */
/*=================================================================== */
const int   ACS_CHB_NOT_CONNECTED = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_EX_EXCHANGE_IDENTITY
		  The exchange identity in the EX-Side
 */
/*=================================================================== */
const int ACS_CHB_EX_EXCHANGE_IDENTITY = 6;     

/*=================================================================== */
/**
   @brief ACS_CHB_SB_EXCHANGE_IDENTITY
		  The exchange identity in the SB-Side
 */
/*=================================================================== */
const int ACS_CHB_SB_EXCHANGE_IDENTITY = 7;

/*=================================================================== */
/**
   @brief ACS_CHB_LAST_RESTART_VALUE
		  The last restart value.
 */
/*=================================================================== */
const int ACS_CHB_LAST_RESTART_VALUE = 8;

/*=================================================================== */
/**
   @brief ACS_CHB_SMALL_C
 */
/*=================================================================== */
const int   ACS_CHB_SMALL_C      = 1;
/*=================================================================== */
/**
   @brief ACS_CHB_SMALL_F
 */
/*=================================================================== */
const int   ACS_CHB_SMALL_F      = 2;
/*=================================================================== */
/**
   @brief ACS_CHB_LARGE_C
 */
/*=================================================================== */
const int   ACS_CHB_LARGE_C      = 3;
/*=================================================================== */
/**
   @brief ACS_CHB_LARGE_F
 */
/*=================================================================== */
const int   ACS_CHB_LARGE_F      = 4;
/*=================================================================== */
/**
   @brief ACS_CHB_RELOAD_C
 */
/*=================================================================== */
const int   ACS_CHB_RELOAD_C     = 5;
/*=================================================================== */
/**
   @brief ACS_CHB_RELOAD_F
 */
/*=================================================================== */
const int   ACS_CHB_RELOAD_F     = 6;
/*=================================================================== */
/**
   @brief ACS_CHB_TMZ_ALARM
 */
/*=================================================================== */
const int ACS_CHB_TMZ_ALARM =9;
/*=================================================================== */
/**
   @brief ACS_CHB_SMALL_BUFSIZE
		  Default size for CHB buffer variables.
 */
/*=================================================================== */
const int ACS_CHB_SMALL_BUFSIZE = 128;
/*=================================================================== */
/**
   @brief ACS_CHB_LARGE_BUFSIZE
		  Default size for CHB buffer variables.
 */
/*=================================================================== */
const int ACS_CHB_LARGE_BUFSIZE = 4096;
/*=================================================================== */
/**
   @brief nameOfCHBIPCPipe
		  Full path to common named communication
 */
/*=================================================================== */
const char* const nameOfCHBIPCPipe = "CHBfifo";

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  FILE_MAX_PATH

 */
/*=================================================================== */
#define FILE_MAX_PATH  260
/*=================================================================== */
/**
   @brief  CHB_AEH_TEXT_INT_SUPERVISOR_ERROR

 */
/*=================================================================== */
#define CHB_AEH_TEXT_INT_SUPERVISOR_ERROR "CHB Internal error";
/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_AEH_SpecificProblem
 */
/*=================================================================== */

typedef const ACE_UINT32 ACS_AEH_SpecificProblem;
/*=================================================================== */
/**
   @brief ACS_AEH_PercSeverity
 */
/*=================================================================== */
typedef const char ACS_AEH_PercSeverity[50];
/*=================================================================== */
/**
   @brief ACS_AEH_ProbableCause
 */
/*=================================================================== */
typedef const char ACS_AEH_ProbableCause[256];
/*=================================================================== */
/**
   @brief ACS_AEH_ProblemData
 */
/*=================================================================== */
typedef const char ACS_AEH_ProblemData[256];
/*=================================================================== */
/**
   @brief ACS_AEH_ProblemText
 */
/*=================================================================== */
typedef const char ACS_AEH_ProblemText[256];
/*=================================================================== */
/**
   @brief ACS_AEH_ObjectOfReference
 */
/*=================================================================== */
typedef const char ACS_AEH_ObjectOfReference[50];

// pipe in CHB
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_processName
		  Name of the heart beat process.
 */
/*=================================================================== */
const char 
ACS_CHB_processName [] =	"acs_chb_heartbeat";
/*=================================================================== */
/**
   @brief ACS_CHB_processNameChild
		  Name of the heart beat child
		  process (ACS_CHB_eventManager)
 */
/*=================================================================== */
const char
ACS_CHB_processNameChild [] =	"acs_chbheartbeatchildd";
/*=================================================================== */
/**
   @brief ACS_CHB_HeartBeat
		  Name of the heart beat application.
 */
/*=================================================================== */
const char
ACS_CHB_HeartBeat [] =		"HEARTBEAT";
/*=================================================================== */
/**
   @brief ACS_CHB_hbTimeout
		  Parameter name. This parameter
		  states the maximum time in
		  seconds to wait for a data
		  indication.
 */
/*=================================================================== */
const char
ACS_CHB_hbTimeout [] =		"heartBeatTimeout";
/*=================================================================== */
/**
   @brief ACS_CHB_hbInterval
		  Parameter name. This parameter
		  states the number of seconds
		  between two heart beat requests.
 */
/*=================================================================== */
const char
ACS_CHB_hbInterval [] =		"heartBeatInterval";

//
// Exit codes
//
/*=================================================================== */
/**
   @brief ACS_AEH_exitOfHeartBeatSuccessful
		  HearBeat terminated on req.
 */
/*=================================================================== */
const int ACS_AEH_exitOfHeartBeatSuccessful = 0;  
/*=================================================================== */
/**
   @brief ACS_AEH_exitOfEventManagerSuccessful
		  Event manager terminated on req.
 */
/*=================================================================== */
const int ACS_AEH_exitOfEventManagerSuccessful = 0;  
/*=================================================================== */
/**
   @brief ACS_AEH_exitInitClientHandlerFailure
		  Setup of client handler failed
 */
/*=================================================================== */
const int ACS_AEH_exitInitClientHandlerFailure = 1;  
/*=================================================================== */
/**
   @brief ACS_AEH_exitReadParameterFailure
		  Failed to read parameters.
 */
/*=================================================================== */
const int ACS_AEH_exitReadParameterFailure = 2;  


//*****************************************************************************
// Error texts and codes
//*****************************************************************************
/*=================================================================== */
/**
   @brief ACS_CHB_NumberOfTimes
		  The number of times to filter printouts
		  to the application log before printing out
		  Link missing between tmz and TZ
 */
/*=================================================================== */
const int	ACS_CHB_NumberOfTimes = 30;
/*=================================================================== */
/**
   @brief ACS_CHB_noErrorID
		  Code indicating no error.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_noErrorID 	=  	0;
/*=================================================================== */
/**
   @brief ACS_CHB_noErrorText
 */
/*=================================================================== */
const char * const ACS_CHB_noErrorText =  "NO ERROR";
/*=================================================================== */
/**
   @brief ACS_CHB_ReadFailedPHA
		  Event code. Could not read
		  parameter from PHA.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ReadFailedPHA =		9020;
/*=================================================================== */
/**
   @brief ACS_CHB_BadReturnCodeJTP
		  Event code. A bad return code
		  was received from JTP.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_BadReturnCodeJTP =	9021;
/*=================================================================== */
/**
   @brief ACS_CHB_UnexpMessageJTP
		  Event code. An unexpected message
		  was received from JTP.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_UnexpMessageJTP =	9022;
/*=================================================================== */
/**
   @brief ACS_CHB_DiscIndJTP
		  Event code. A disconnect
		  indication was received from JTP.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_DiscIndJTP =		9023;
/*=================================================================== */
/**
   @brief ACS_CHB_SigTerm
		  Event code. Process termination
		  was requested.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_SigTerm =			9024;
/*=================================================================== */
/**
   @brief ACS_CHB_FatalErr
		  Event code. Process terminated
		  due to a fatal error.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_FatalErr =			9025;
/*=================================================================== */
/**
   @brief ACS_CHB_UnknownHandleJTP
		  Event code. A message with unknown
		  handle was received.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_UnknownHandleJTP =	9026;
/*=================================================================== */
/**
   @brief ACS_CHB_HandleUsedJTP
		  Handle is already in use -
		  connection refused.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_HandleUsedJTP =		9027;
/*=================================================================== */
/**
   @brief ACS_CHB_MTZlinkMissing
		  Link between mtz and TZ not
		  defined. Use 'mtzln' command.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_MTZlinkMissing =	9028;

/*=================================================================== */
/**
   @brief ACS_CHB_ClusterCallFailed
		  Event code. Unexpected return code
		  from cluster.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ClusterCallFailed =	9029;

/*=================================================================== */
/**
   @brief ACS_CHB_ActiveNodeFaulty
		  Event code. A message from CP indicates
		  that the other (active) node of the
		  cluster is faulty. Failover.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ActiveNodeFaulty =	9030;

/*=================================================================== */
/**
   @brief ACS_CHB_PassiveNodeFaulty
		  Event code. A message from CP indicates
		  that the other (passive) node of the
		  cluster is faulty.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_PassiveNodeFaulty =	9031;
/*=================================================================== */
/**
   @brief ACS_CHB_ConnectionLostJTP
		  Event code. Generated every 10 min as
		  long as connection with CP is lost.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ConnectionLostJTP =	9032;
/*=================================================================== */
/**
   @brief ACS_CHB_SERVICEERROR
		  Event code. Service error.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_SERVICEERROR =		9033;
/*=================================================================== */
/**
   @brief ACS_CHB_ActiveNodeFaultyWarning
		  Event code. Message indicates that
		  active node is found faulty.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_ActiveNodeFaultyWarning = 9034;
/*=================================================================== */
/**
   @brief ACS_CHB_CSnotAvailable
		  Event code. Generated every 5 sec as
		  long as connection with CS isn't available.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_CSnotAvailable =    9035;
/*=================================================================== */
/**
   @brief ACS_CHB_CallnotAccepted
		  Event code. Generated when new calls
          from CP are not accepted.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_CallnotAccepted =   9036;
/*=================================================================== */
/**
   @brief ACS_CHB_StatenotConnected
		  Event code. Generated when the
		  Conversation State in APJTP is not connected.
 */
/*=================================================================== */
ACS_AEH_SpecificProblem
ACS_CHB_StatenotConnected = 9037;

/*=================================================================== */
 /**

  @brief ACS_CHB_DSDnotAvailable
		Event code. Generated when DSD Server is not available.
 */
ACS_AEH_SpecificProblem
ACS_CHB_DSDnotAvailable  = 9038;

//*****************************************************************************
// Trace and error texts
//*****************************************************************************

// *** Event manager specifics ***
/*=================================================================== */
/**
   @brief ACS_CHB_TerminationSignalFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_TerminationSignalFailedText 	=  "Termination signal received";
/*=================================================================== */
/**
   @brief ACS_CHB_GetParentPIDFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_GetParentPIDFailedText 		=  "Parent process terminated abnormally";
/*=================================================================== */
/**
   @brief ACS_CHB_IPCPipeAccessFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_IPCPipeAccessFailedText 	=  "IPC communication pipe is lost";

// *** Request handler specifics ***
/*=================================================================== */
/**
   @brief ACS_CHB_IPCPipeOpenClientFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_IPCPipeOpenClientFailedText 	=  "Failed to open CHB_IPC_pipe client";
/*=================================================================== */
/**
   @brief ACS_CHB_IPCPipeOpenServerFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_IPCPipeOpenServerFailedText 	=  "Failed to open CHB_IPC_pipe server";
/*=================================================================== */
/**
   @brief ACS_CHB_SendRequestToEventManagerText
 */
/*=================================================================== */
const char * const
ACS_CHB_SendRequestToEventManagerText 	=  "Send request to HeartBeat";

//
// Client handler specifics
//
/*=================================================================== */
/**
   @brief ACS_CHB_initReceivedFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_initReceivedFailedID 		=  9030;
/*=================================================================== */
/**
   @brief ACS_CHB_initReceivedFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_initReceivedFailedText =  "SETUP OF CLIENT HANDLER FAILED";
/*=================================================================== */
/**
   @brief ACS_CHB_sendValueToClientFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_sendValueToClientFailedID   	=  9031;
/*=================================================================== */
/**
   @brief ACS_CHB_sendValueToClientFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_sendValueToClientFailedText 	=  "TRANSFER OF OBJECT VALUE TO CLIENT "
					   "FAILED";

//
// Receive section specific codes.
//
/*=================================================================== */
/**
   @brief ACS_CHB_SocketCreationOfListenFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_SocketCreationOfListenFailedID =  1;
/*=================================================================== */
/**
   @brief ACS_CHB_SocketCreationOfListenFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_SocketCreationOfListenFailedText =  "Failed to create listen socket";
/*=================================================================== */
/**
   @brief ACS_CHB_BindFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_BindFailedID =  2;
/*=================================================================== */
/**
   @brief ACS_CHB_BindFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_BindFailedText =  "Failed to bind listen socket";
/*=================================================================== */
/**
   @brief ACS_CHB_ListenFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_ListenFailedID =  3;
/*=================================================================== */
/**
   @brief ACS_CHB_ListenFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_ListenFailedText =  "Listen failed ";
/*=================================================================== */
/**
   @brief ACS_CHB_PollFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_PollFailedText =  "Poll failed ";
/*=================================================================== */
/**
   @brief ACS_CHB_AcceptFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_AcceptFailedID =  5;
/*=================================================================== */
/**
   @brief ACS_CHB_AcceptFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_AcceptFailedText =  "Socket accept failed ";
/*=================================================================== */
/**
   @brief ACS_CHB_ReadFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_ReadFailedID =  6;
/*=================================================================== */
/**
   @brief ACS_CHB_ReadFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_ReadFailedText =  "Read from socket failed ";
/*=================================================================== */
/**
   @brief ACS_CHB_WriteFailedID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_WriteFailedID =  7;
/*=================================================================== */
/**
   @brief ACS_CHB_WriteFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_WriteFailedText =  "Write to socket failed ";
/*=================================================================== */
/**
   @brief ACS_CHB_MaxNumberOfAllowedClientsExceededID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_MaxNumberOfAllowedClientsExceededID = 8;
/*=================================================================== */
/**
   @brief ACS_CHB_MaxNumberOfAllowedClientsExceededText
 */
/*=================================================================== */
const char * const
ACS_CHB_MaxNumberOfAllowedClientsExceededText = "Max number of clients value exceed";
/*=================================================================== */
/**
   @brief ACS_CHB_SocketReUseAdrID
 */
/*=================================================================== */
ACS_AEH_SpecificProblem 
ACS_CHB_SocketReUseAdrID =  9;
/*=================================================================== */
/**
   @brief ACS_CHB_SocketReUseAdrFailedText
 */
/*=================================================================== */
const char * const
ACS_CHB_SocketReUseAdrFailedText =  "Socket Option: ReUseAddress Failed";

/**
   @brief       HEARTBEAT_PARAM_IMPL_NAME
*/
/*=================================================================== */
#define HEARTBEAT_PARAM_IMPL_NAME "HeartBeatIMMParamImpl"
/*=================================================================== */

/**
   @brief	HEARTBEAT_PARAM_OBJ_DN
*/
/*=================================================================== */
#define HEARTBEAT_PARAM_OBJ_DN 	"heartBeatId=1"
/*=================================================================== */


#endif

