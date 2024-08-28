/*=================================================================== */
/**
@file		acs_nsf_common.h

@brief		Header file for common utilities for NSF.

This module contains all the declarations useful to
specify the class.

@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/12/2010   XCHEMAD     Initial Release
N/A       19/04/2011   XTANAGG     Added doxygen comments.
N/A       05/05/2014   XQUYDAO     Update for cable-less environment
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
#if !defined(_ACS_NSF_Common_H_)
#define _ACS_NSF_Common_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include "acs_nsf_command_handler.h"
using namespace std;

/**
* @brief	_MICROSECOND
*/
#define _MICROSECOND    (10)

/**
* @brief	_MILLISECOND
*/
#define _MILLISECOND    (1000 * _MICROSECOND)

/**
* @brief	_SECOND
*/
#define _SECOND         (1000 * _MILLISECOND)

/**
* @brief	DEFDATALEN
* 			Default data length
*/
#define DEFDATALEN      (64-ICMP_MINLEN)

/**
* @brief	MAXIPLEN
*/
#define MAXIPLEN        60

/**
* @brief	MAXICMPLEN
*/
#define MAXICMPLEN      76

/**
* @brief	MAXPACKET
* 			Max packet size
*/
#define MAXPACKET       (65536 - 60 - ICMP_MINLEN)

/**
* @brief	NSF_SURVELLIENCEACTIVEFLAG
*/
#define NSF_SURVELLIENCEACTIVEFLAG   "isSurveillanceActive"

/**
* @brief	NSF_CONFIG_FILE_NAME_A
*/
#define NSF_CONFIG_FILE_NAME_A   "/cluster/etc/ap/acs/nsf/conf/acs_nsf_a.conf"

/**
* @brief	NSF_CONFIG_FILE_NAME_B
*/
#define NSF_CONFIG_FILE_NAME_B   "/cluster/etc/ap/acs/nsf/conf/acs_nsf_b.conf"

/**
* @brief	NSF_CBL_CONFIG_FILE_NAME_A
*/
#define NSF_CBL_CONFIG_FILE_NAME_A   "/cluster/etc/ap/acs/nsf/conf/acs_nsf_cbl_a.conf"

/**
* @brief	NSF_CBL_CONFIG_FILE_NAME_B
*/
#define NSF_CBL_CONFIG_FILE_NAME_B   "/cluster/etc/ap/acs/nsf/conf/acs_nsf_cbl_b.conf"

/**
* @brief	NSF_CONFIG_DIR_NSF_NAME
*/
#define NSF_CONFIG_DIR_NSF_NAME   "/cluster/etc/ap/acs/nsf"

/**
* @brief	NSF_CONFIG_DIR_CONF_NAME
*/
#define NSF_CONFIG_DIR_CONF_NAME   "/cluster/etc/ap/acs/nsf/conf"

/**
* @brief	NSF_NOOFFAILOVER
*/
#define NSF_NOOFFAILOVER     "failoverCount"

/**
* @brief	NSF_LASTFAILOVERTIME
*/
#define NSF_LASTFAILOVERTIME "lastFailoverTime"

/**
* @brief	NSF_LASTFAILOVERTIME_A
*/
#define NSF_LASTFAILOVERTIME_A "lastFailoverTimeA"

/**
* @brief	NSF_LASTFAILOVERTIME_B
*/
#define NSF_LASTFAILOVERTIME_B "lastFailoverTimeB"

/**
* @brief	NSF_ALARMSTATE_A
*/
#define NSF_ALARMSTATE_A     "alarmStateA"

/**
* @brief	NSF_ALARMSTATE_B
*/
#define NSF_ALARMSTATE_B     "alarmStateB"

/**
* @brief	NSF_ALARMSTATE_NET2_A
*/
#define NSF_ALARMSTATE_NET2_A     "alarmState2A"

/**
* @brief	NSF_ALARMSTATE_NET2_B
*/
#define NSF_ALARMSTATE_NET2_B     "alarmState2B"

/**
* @brief	NSF_CBL_BONDINGSTATE_A
*/
#define NSF_CBL_BONDINGSTATE_A     "bondingStateA"

/**
* @brief	NSF_CBL_BONDINGSTATE_B
*/
#define NSF_CBL_BONDINGSTATE_B     "bondingStateB"

/**
* @brief	NSF_CBL_NUMOFTIMES_A
*/
#define NSF_CBL_NUMOFTIMES_A     "numOfTimeA"

/**
* @brief	NSF_CBL_NUMOFTIMES_B
*/
#define NSF_CBL_NUMOFTIMES_B    "numOfTimeB"

/**
* @brief	NSF_CBL_BONDING_INTERFACE
*/
#define NSF_CBL_BONDING_INTERFACE    "bond1"

/**
* @brief	NSF_PING_PERIOD
* 			PHA PARAMETER
*/
#define NSF_PING_PERIOD     "pingPeriod"

/**
* @brief	NSF_ROUTER_RESPONSE
* 			PHA PARAMETER
*/
#define NSF_ROUTER_RESPONSE "routerResponse"

/**
* @brief	NSF_RESET_TIME
* 			PHA PARAMETER
*/
#define NSF_RESET_TIME      "resetTime"

/**
* @brief	NSF_FAIL_OVERS
* 			PHA PARAMETER
*/
#define NSF_FAIL_OVERS      "maxFailoverAttempts"

/**
* @brief	NSF_ACTIVE_FLAG
* 			PHA PARAMETER
*/
#define NSF_ACTIVE_FLAG     "isSurveillanceEnabled"

/**
* @brief	NSF_FAILOVER_PRIORITY
* 			PHA PARAMETER
*/
#define NSF_FAILOVER_PRIORITY     "failoverPriority"

/**
* @brief	RESET_FAILOVER_COUNT
* 			Admin Operation
*/
#define RESET_FAILOVER_COUNT     1


// BEGIN  TR:H053106
/**
 * @brief	ACS_NSF_TIMESTAMPFILE_PATH
 */
#define ACS_NSF_TIMESTAMPFILE_PATH "/opt/ap/acs/etc/nsf/conf/"


/**
 * @brief        ACS_NSF_TIMESTAMPFILE_NAME
 */
#define ACS_NSF_TIMESTAMPFILE_NAME "NSF_Timestamp.txt"
// END  TR:H053106
/**
* @brief	ACS_NSF_LOGFILE_PATH
*/
#define ACS_NSF_LOGFILE_PATH "/var/run/ap/nsf/ACS_NSF_Server.log"


/**
* @brief	PHYSICAL_SEPARATION_ATTR_NAME
*/
#define PHYSICAL_SEPARATION_ATTR_NAME "physicalSeparationStatus"

/**
* @brief	NORTHBOUND_OBJ_DN
*/
#define NORTHBOUND_OBJ_DN	"northBoundId=1,networkConfigurationId=1"

/**
* @brief	APG_OAM_ACCESS_ATTR_NAME
*/
#define APG_OAM_ACCESS_ATTR_NAME "apgOamAccess"

/**
* @brief	AXEFUNCTIONS_OBJ_DN
*/
#define AXEFUNCTIONS_OBJ_DN	"axeFunctionsId=1"

/**
* @brief	ACS_NSF_DIRPATH
*/
#define ACS_NSF_DIRPATH "/var/run/ap/nsf"

/**
* @brief	NANOSECONDS_TIME_MAX_LENGTH
*/
#define NANOSECONDS_TIME_MAX_LENGTH 100

/**
* @brief	NSF_ConfigurationRDN
*/
#define RDN_OF_NSF_CONFIG_OBJ 	"networkSurveillanceConfigId=1";

#define RDN_OF_NSF_INITIAL_PARMS_OBJ "networkSurveillanceConfigId=1"

/**
* @brief	ACS_CONFIG_IMM_NSF_CLASS_NAME
*/
#define ACS_CONFIG_IMM_NSF_CLASS_NAME 	"AxeExtNetworkSurveillanceExternalNetworkSurveillanceM"
/**
* @brief        ACS_PARAM_IMM_NSF_CLASS_NAME
*/
#define ACS_PARAM_IMM_NSF_CLASS_NAME 	"AxeExtNetworkSurveillanceNetworkSurveillanceParameters"
/**
* @brief        ACS_NSF_PARAM_OBJ_DN
*/
#define ACS_NSF_PARAM_OBJ_DN	"networkSurveillanceParametersId=1"

/**
* @brief	APOS_CLUSTERCONF_BIN
* 			This is required for communication
* 			between the nodes.
*/
#define APOS_CLUSTERCONF_BIN "/opt/ap/apos/bin/clusterconf/clusterconf"

/**
* @brief	NODE_A_PORT
* 			Port for Ping Server on Node A.
*/
#define NODE_A_PORT 5656

/**
* @brief	NODE_B_PORT
* 			Port for Ping Server on Node B.
*/
#define NODE_B_PORT 5657


#define ACS_NSF_NODE_STATE_FILE_NAME	"acs_nsf_node_state.sh"

#define INTERACE_NAME_PUBLIC_NETWORK_CMD "cat /etc/cluster/nodes/this/networks/public/primary/interface/name"

#define INTERACE_NAME_PUBLIC_NETWORK2_CMD "cat /etc/cluster/nodes/this/networks/public2/primary/interface/name"

#define ACS_NSF_RESTORE_FLAG_PATH "/storage/clear/acs-nsf/"
#define ACS_NSF_RESTORE_FLAG_FILENAME_NODEA "acs_nsf_restore_flag_a"
#define ACS_NSF_RESTORE_FLAG_FILENAME_NODEB "acs_nsf_restore_flag_b"

//! ACE Trace Macros
#define DEBUG_PREFIX "\n DEBUG%I"
#define INFO_PREFIX "\n INFO%I"
#define NOTICE_PREFIX "\n NOTICE%I"
#define WARNING_PREFIX "\n WARNING%I"
#define ERROR_PREFIX "\n ERROR%I"
#define CRITICAL_PREFIX "\n CRITICAL%I"
#define ALERT_PREFIX "\n ALERT%I"
#define EMERGENCY_PREFIX "\n EMERGENCY%I"


#ifdef NSFDEBUG
#define DEBUG(FMT, ...) \
		ACE_DEBUG(( LM_DEBUG, \
				DEBUG_PREFIX FMT ,\
				__VA_ARGS__))


#define WARNING(FMT, ...) \
		ACS_NSF_Common::helperTraceWarning(FMT,__VA_ARGS__);
#define TRA_ERROR(FMT, ...) \
		ACS_NSF_Common::helperTraceWarning(FMT,__VA_ARGS__);

#define INFO(FMT, ...) \
		ACE_DEBUG(( LM_INFO, \
				INFO_PREFIX FMT ,\
				__VA_ARGS__))

#define NOTICE(FMT, ...) \
		ACE_DEBUG(( LM_NOTICE, \
				NOTICE_PREFIX FMT ,\
				__VA_ARGS__))

#define ERROR(FMT, ...) \
		ACE_DEBUG(( LM_ERROR, \
				ERROR_PREFIX FMT ,\
				__VA_ARGS__))

#define CRITICAL(FMT, ...) \
		ACE_DEBUG(( LM_CRITICAL, \
				CRITICAL_PREFIX FMT ,\
				__VA_ARGS__))

#define ALERT(FMT, ...) \
		ACE_DEBUG(( LM_ALERT, \
				ALERT_PREFIX FMT ,\
				__VA_ARGS__))

#define EMERGENCY(FMT, ...) \
		ACE_DEBUG(( LM_EMERGENCY, \
				EMERGENCY_PREFIX FMT ,\
				__VA_ARGS__))


#else //NSFDEBUG

#define INFO(FMT, ...) \
		ACS_NSF_Common::helperTraceInformation(FMT,__VA_ARGS__);

#define DEBUG(FMT, ...) \
		ACS_NSF_Common::helperTraceDebug(FMT,__VA_ARGS__);
#define WARNING(FMT, ...) \
		ACS_NSF_Common::helperTraceWarning(FMT,__VA_ARGS__);

#define ERROR(FMT, ...) \
		ACS_NSF_Common::helperTraceError(FMT,__VA_ARGS__);

#define FATAL(FMT, ...) \
		ACS_NSF_Common::helperTraceFatal(FMT,__VA_ARGS__);

#define NOTICE(FMT, ...) \
		ACS_NSF_Common::helperTraceDebug(FMT,__VA_ARGS__);

#endif

/**
 * @brief	NODE_NAME
 */
/*=================================================================== */
enum NODE_NAME
{
	NODE_A=1,    //!< NODE_A
	NODE_B=2, 	 //!< NODE_B
	UNDEFINED=3  //!< UNDEFINED
};
/*=================================================================== */

class ACS_NSF_CommandHandler;
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		ACS_NSF_Common

@brief		This class implements the generic functions required for NSF.
			Also this class implements the call backs for HA.
**/
/*=================================================================== */

class ACS_NSF_Common
{
	/*=============================================================================
	            PUBLIC DECLARATION SECTION
	  =============================================================================*/


public :


	/*=================================================================== */
	/**
	* @brief	theOmHandler
	*/
	/*=================================================================== */
	OmHandler theOmHandler;
	/*=================================================================== */

	/**
	* @brief	theConfigFileMutex
	*/
	/*=================================================================== */
	ACE_Recursive_Thread_Mutex  theConfigFileMutex; 
	/*=================================================================== */
	/**
	* @brief	ACS_NSF_InformationTrace
	*/
	/*=================================================================== */
	static ACS_TRA_trace ACS_NSF_InformationTrace;
	/*=================================================================== */

	/**
	* @brief	ACS_NSF_DebugTrace
	*/
	/*=================================================================== */
	static ACS_TRA_trace ACS_NSF_DebugTrace;
	/*=================================================================== */

	/**
	* @brief	ACS_NSF_WarningTrace
	*/
	/*=================================================================== */
	static ACS_TRA_trace ACS_NSF_WarningTrace;
	/*=================================================================== */

	/**
	* @brief	ACS_NSF_ErrorTrace
	*/
	/*=================================================================== */
	static ACS_TRA_trace ACS_NSF_ErrorTrace;
	/*=================================================================== */

	/**
	* @brief	ACS_NSF_FatalTrace
	*/
	/*=================================================================== */
	static ACS_TRA_trace ACS_NSF_FatalTrace;
	/*=================================================================== */

	/**
	* @brief	ACS_NSF_Logging
	*/
	/*=================================================================== */
	static ACS_TRA_Logging ACS_NSF_Logging;
	/*=================================================================== */


	/*=============================================================================
            PUBLIC METHODS
	=============================================================================*/
	/***************************************************************************/
	/**
	* @brief	helperTraceInformation
	* 			This is a static method used to trace informational statements.
	* @param	messageFormat
	*/
	/*=================================================================== */
	static void helperTraceInformation(const ACE_TCHAR* messageFormat, ...);
	/*=================================================================== */

	/**
	* @brief	helperTraceDebug
	* 			This is a static method used to trace debug statements.
	* @param 	messageFormat
	*/
	/*=================================================================== */
	static void helperTraceDebug(const ACE_TCHAR* messageFormat, ...);
	/*=================================================================== */

	/**
	* @brief	helperTraceWarning
	* 			This is a static method used to trace warnings.
	* @param 	messageFormat
	*/
	/*=================================================================== */
	static void helperTraceWarning(const ACE_TCHAR* messageFormat, ...);
	/*=================================================================== */


	/**
	* @brief	helperTraceError
	* 			This is a static method used to trace errors.
	* @param 	messageFormat
	*/
	/*=================================================================== */
	static void helperTraceError(const ACE_TCHAR* messageFormat, ...);
	/*=================================================================== */

	/**
	* @brief	helperTraceFatal
	* 			This is a static method used to trace fatal errors.
	* @param 	messageFormat
	*/
	/*=================================================================== */
	static void helperTraceFatal(const ACE_TCHAR* messageFormat, ...);
	/*=================================================================== */

	/**
	* @brief	reportError
	* @param 	specificProblem
	* @param 	s1
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem,string s1) ;
	/*=================================================================== */

	/**
	* @brief	reportError
	* 			This is an overloaded method which is used to report events.
	* @param 	specificProblem
	* @param 	s1
	* @param 	s2
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem,string s1,string s2) ;
	/*=================================================================== */


	/**
	* @brief	reportError
	* @param 	specificProblem
	* @param 	s1
	* @param 	s2
	* @param 	s3
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem, string s1, string s2, string s3) ;
	/*=================================================================== */

	/**
	* @brief	reportError
	* @param	specificProblem
	* @param	s1
	* @param	dw2
	* @param	s3
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem, string s1, ACE_UINT32 dw2, string s3) ;
	/*=================================================================== */

	/**
	* @brief	reportError
	* @param	specificProblem
	* @param	s1
	* @param	s2
	* @param	s3
	* @param	s4
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem,string s1,string s2,string s3,ACE_UINT32 s4) ;
	/*=================================================================== */

	/**
	* @brief	reportError
	* @param	specificProblem
	* @param	s1
	* @param	s2
	* @param	s3
	* @param	s4
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem,string s1,wstring s2,string s3,ACE_UINT32 s4) ;
	/*=================================================================== */

	/**
	* @brief	reportError
	* @param	specificProblem
	* @param	s1
	* @param	s2
	*/
	/*=================================================================== */
	void reportError(ACE_UINT32 specificProblem,string s1,ACE_UINT32 s2) ;
	/*=================================================================== */

	/**
	* @brief	setRegWord
	* 			This method is used to set valueName to val in IMM.
	* @param 	valueName
	* @param 	val
	* @param 	msg
	* @return	int
	*/
	/*=================================================================== */
	int setRegWord( string valueName, ACE_UINT32 val,  char** msg = NULL) ;
	/*=================================================================== */


	/**
	* @brief		getRegWord
	* @param		valueName
	* @param		val
	* @return	int
	* 			 0:                 OK. time value returned
	*  			-1:                 Error detected and reported
	*			 1:                 Value does not exist
	*/
	/*=================================================================== */
	int getRegWord (string valueName, ACE_UINT32 &val) ;
	/*=================================================================== */


	/**
	* @brief		getRegTime
	* 			This method is used to read a time-value from the IMM.
	* @param 	valueName
	* @param 	diffTime
	* 			Seconds from current time
	* @return	int
	* 			0:                  OK. time value returned
	* 			-1:                 Error detected and reported
	* 			1:                  Value does not exist
	*/
	/*=================================================================== */
	int getRegTime(string valueName,ACE_UINT32 &diffTime) ;
	/*=================================================================== */


	/**
	* @brief		setRegTime
	* 			This method is used to set a time-value to the IMM.
	* @param 	valueName
	* @return	int
	* 			 0: OK. time value returned
	* 		    -1: Error detected and reported
	*/
	/*=================================================================== */
	int setRegTime(string valueName) ;
	/*=================================================================== */

	/**
	* @brief		GetPSState
	* 			This method is used to get the status of physical
				separation.
	* @return		ACE_INT32
	* 			 0: Physical Separation is Not Configured.
	* 			 1: Physical Separation is Configured.
	* 		    	-1: Error detected and reported
	*/
	/*=================================================================== */
	ACE_INT32 GetPSState() ;
	/*=================================================================== */

	/**
	* @brief		GetApgOamAccess
	* 			This method is used to get the APG OAM access.
	* @return		ACE_INT32
	* 			 0: front cable
	* 			 1: no cable (cable-less)
	* 			 2: no applicable
	*/
	/*=================================================================== */
	ACE_INT32 GetApgOamAccess() ;
	
	/*=====================================================================
                                CLASS CONSTRUCTORS
	==================================================================== */

	/**
	* @brief	ACS_NSF_Common
	* @return
	*/
	/*=================================================================== */
	ACS_NSF_Common() ;
	/*=================================================================== */

	/*=====================================================================
                                CLASS DESTRUCTORS
	  ==================================================================== */

	/**
	* @brief	~ACS_NSF_Common
	* @return
	*/
	/*=================================================================== */
	~ACS_NSF_Common() ;
	/*=================================================================== */

	/**
	* @brief	getPublicdnName
	* @return
	*/
	/*=================================================================== */
	string  getPublicdnName( )  ;
	/*=================================================================== */

	/**
	* @brief	setStartStopValue
	* @param aStartStopValue
	*/
	/*=================================================================== */
	void setStartStopValue(unsigned int aStartStopValue);
	/*=================================================================== */

	/**
	* @brief	getStartStopValue
	* @return
	*/
	/*=================================================================== */
	unsigned int getStartStopValue();
	/*=================================================================== */

	/**
	* @brief	getFailOverSetFlag
	* @return
	*/
	/*=================================================================== */
	bool getFailOverSetFlag();
	/*=================================================================== */

	/**
	* @brief	setFailOverSetFlag
	* @param aFailOverSetFlag
	*/
	/*=================================================================== */
	void setFailOverSetFlag(bool aFailOverSetFlag);
	/*=================================================================== */

	/**
	* @brief	getNode
	* @return
	*/
	/*=================================================================== */
	NODE_NAME getNode();
	/*=================================================================== */

	/**
	* @brief
	* @param aActiveValue
	* @return	calculateStartStopValue
	*/
	/*=================================================================== */
	bool calculateStartStopValue(unsigned int aActiveValue);
	/*=================================================================== */

	/**
	* @brief	getInitialSetFlag
	* @return
	*/
	/*=================================================================== */
	bool getInitialSetFlag();
	/*=================================================================== */

	/**
	* @brief	setInitialSetFlag
	* @param aInitialSetFlag
	*/
	/*=================================================================== */
	void setInitialSetFlag(bool aInitialSetFlag);
	/*=================================================================== */

	/**
	* @brief		getActiveNodeFlag
	* @return
	*/
	/*=================================================================== */
	bool getActiveNodeFlag();
	/*=================================================================== */
	/**
	* @brief	setNodeSpecificParams
	* 		
	* @return	int
	*/
	/*=================================================================== */
	int  setNodeSpecificParams();
	/*=================================================================== */

	/**
	* @brief	fetchDnOfRootObjFromIMM
	*		This method is used to fetch the 
        *		RDN of the root class object.
	*
	* @return	int
	*/
	/*=================================================================== */
	static int fetchDnOfRootObjFromIMM ();
	/*=================================================================== */

	/*=================================================================== */

	/*=================================================================== */
	/**
	 * @brief		setLoadParamsFlag
	 * @param       flag
	 * @return      none
	 */
	/*=================================================================== */
	void setLoadParamsFlag(bool flag);
	/*=================================================================== */

	/**
	 * @brief		isLoadingOfParamsRequired
	 * @return      bool
	 */
	/*=================================================================== */
	bool isLoadingOfParamsRequired();
	/*=================================================================== */
public:
	/**
	* @brief	parentObjDNofNSF
	*/
	/*=================================================================== */
	static std::string parentObjDNofNSF;
	static std::string childObjDNofNSF;
	/*=================================================================== */

	/**
	* @brief	pingPeriod
	*/
	/*=================================================================== */
	ACE_UINT32 pingPeriod ;
	/*=================================================================== */
	
	/**
	* @brief	routerResponse
	*/
	/*=================================================================== */
	ACE_UINT32 routerResponse;
	/*=================================================================== */
	
	/**
	* @brief	maxFailoverAttempts	
	*/
	/*=================================================================== */
	ACE_UINT32 maxFailoverAttempts;
	/*=================================================================== */

	/**
	* @brief	resetTime
	*/
	/*=================================================================== */
	ACE_UINT32 resetTime;
	/*=================================================================== */

	/**
	* @brief	isSurveillanceEnabled
	*/
	/*=================================================================== */
	ACE_INT32 isSurveillanceEnabled;
	/*=================================================================== */

	/**
	* @brief	failoverPriority
	*/
	/*=================================================================== */
	ACE_INT32 failoverPriority;
	/*=================================================================== */

	/**
	* @brief	failoverCount
	*/
	/*=================================================================== */
	ACE_UINT32 failoverCount;
	/*=================================================================== */
	
	/**
	* @brief	isSurveillanceActive
	*/
	/*=================================================================== */
	ACE_INT32 isSurveillanceActive;	
	/*=================================================================== */

	/*===================================================================
      					PRIVATE DECLARATION SECTION
      =================================================================== */
private:
	bool isLoadingParamsRequired;

	/**
	* @brief	dnObject
	*/
	/*=================================================================== */
	string dnObject ;
	/*=================================================================== */
	/**
	* @brief	theStartStopValue
	*/
	/*=================================================================== */
	unsigned int theStartStopValue;
	/*=================================================================== */

	/**
	* @brief	theFailOverSetFlag
	*/
	/*=================================================================== */
	bool theFailOverSetFlag;
	/*=================================================================== */

	/**
	* @brief	theInitialSetFlag
	*/
	/*=================================================================== */
	bool theInitialSetFlag;
	/*=================================================================== */

	/**
	* @brief	theCurrentNode
	*/
	/*=================================================================== */
	NODE_NAME theCurrentNode;
	/*=================================================================== */

	/**
	* @brief	theActiveNodeFlag
	*/
	/*=================================================================== */
	bool theActiveNodeFlag;
	/*=================================================================== */
};

#endif //End of _ACS_NSF_Common_H_
