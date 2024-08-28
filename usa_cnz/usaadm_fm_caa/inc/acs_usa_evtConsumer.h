/*============================================================== */
/**
 *	@file   "acs_usa_evtConsumer.h"
 *
 *	@brief
 *
 *
 *	@version 1.0.0
 *
 *
 *	HISTORY
 *
 *
 *
 *
 *	PR           DATE      INITIALS    DESCRIPTION
 *-------------------------------------------------------------
 *	N/A       DD/MM/YYYY     NS       Initial Release
 * XFABPAG   05/05/2014	    FP       Added getBondingProblemDescription
 * XGIACRI   22/05/2014	    GC       USAFM_ETHERNET_BONDING_FAILED disabled for APG CABLE LESS
 * XQUYDAO   12/06/2014	    QD       Updated to let USAFM re-sends alarm after reboot
 * XQUYDAO   19/08/2014     QD       Updated to subscribe event type _2 due to the change of COM 4.0
 * XQUYDAO   19/09/2014     QD       Added isClusterRebootInProgress method
 * XGIACRI   13/10/2014     GC       Fixed problem related to wrong slot number in case of missing info in hwctable TR HT11945
 * TEISMA, TEIMON 12/12/2019         Trusted Certificate Expiry alarm handling APG 3.9
 * TEISMA, TEIMON 29/01/2020         Trusted Certificate Expiry alarm handling APG 3.9 rework
 * XCSRPAD   18/06/2021              Alarm for Log Streaming
 *============================================================= */

/*==============================================================
 *		DIRECTIVE DECLARATION SECTION
 *================================================================ */


#ifndef ACS_USA_EVTCONSUMER_H
#define ACS_USA_EVTCONSUMER_H

#include <ComOamSpiEvent_1.h>
#include <ComMgmtSpiInterfacePortalAccessor.h>
#include <ComMgmtSpiInterfacePortal_1.h>
#include <ComOamApiFmEvent_2.h>

#include <map>
#include <list>
#include "ACS_TRA_Logging.h"
#include "acs_aeh_evreport.h"

/*===============================================================
 *		DIRECTIVE DECLARATION SECTION
 *=============================================================== */

#define EVENT_CONSUMER_PROC_NAME                "ACS_USAFM"
#define USAFM_NODE_UNAVALABLE                   849346561U
#define USAFM_ETHERNET_BONDING_FAILED           3341942786U
#define USAFM_DISK_USAGE_THRESHOLD_REACHED      3341942787U
#define USAFM_DISK_REPLICATION_COMM_FAILED      3341942788U
#define USAFM_MEMORY_USAGE_THRESHOLD_REACHED    3341942789U
#define USAFM_DISK_REPLICATION_INCONSISTANT     3341942790U
#define USAFM_CANCEL_RESTORE_BKUP               2115371009U
#define USAFM_MODEL_ERROR                       849346562U
#define USAFM_COMP_INTANTIATE_FAILED            131074U
#define USAFM_COMP_CLEANUP_FAILED               131075U
#define USAFM_AP_PRCESS_STOPPED                 131077U
#define USAFM_VALIDCERT_NOT_AVAILABLE           6946817U
#define USAFM_CERT_ABOUTTO_EXPIRE               6946818U
#define USAFM_AUTO_ENROLL_FAILED                6946819U
#define USAFM_TIME_SYNC_FAULT			3341942785
#define USAFM_AUTH_FAILURE_LIMIT_REACHED        6946820U
#define USAFM_TRUSTED_CERT_ABOUTTO_EXPIRE       6946821U         // minor type for Trusted Certificate about to expiry alarm 
#define USAFM_LOG_STREAMING_FAILED              3341942792U          // minor type for log streaming failure 

/**
* @brief        AP_NODE_NUMBER_ATTR_NAME
*/
#define AP_NODE_NUMBER_ATTR_NAME "apNodeNumber"

/**
* @brief	APG_OAM_ACCESS_ATTR_NAME
*/
#define APG_OAM_ACCESS_ATTR_NAME "apgOamAccess"

/**
* @brief	AXEFUNCTIONS_OBJ_DN
*/
#define AXEFUNCTIONS_OBJ_DN	"axeFunctionsId=1"

/**
* @brief	APOS_HA_FILE_PROC_DRBD
*/
#define FILE_PROC_DRBD				"/proc/drbd"

/*===============================================================
 *		CLASS DECLARATION SECTION
 *=============================================================== */

const char* const ACS_USAFM_APRelPublicInterfaceFault = "RELIABLE PUBLIC INTERFACE, AP, FAULT";
const char* const ACS_USAFM_APFault                   = "AP FAULT";
const char* const ACS_USAFM_APSystemAnalysis          = "AP SYSTEM ANALYSIS";
const char* const ACS_USAFM_APProcessStop             = "AP PROCESS STOPPED";
const char* const ACS_USAFM_APFunctionNotAvailable    = "AP FUNCTION NOT AVAILABLE";
const char* const ACS_USAFM_APInternalFault           = "AP INTERNAL FAULT";
const char* const ACS_USAFM_APCertMgmtFault	      = "AP, CERTIFICATE MANAGEMENT, FAULT";
const char* const ACS_USAFM_APTrustedCertMgmtFault    = "AP, TRUSTED CERTIFICATE MANAGEMENT, FAULT";
const char* const ACS_USAFM_NTPSyncFault	      =	"NTP SYNCHRONIZATION FAULT";		
const char* const ACS_USAFM_AuthenticationFailure     = "AP, LOCAL USER AUTHENTICATION, FAULT" ;
const char* const ACS_USAFM_APLogMgmtFault	      = "AP, LOG MANAGEMENT, FAULT";
const char* const ACS_USAFM_objOfClassRef             = "APZ";
const char* const ACS_USAFM_objOfRefOS                = "OS";
const char* const ACS_USAFM_objOfRefMiddleware        = "Middleware";
const char* const ACS_USAFM_Severity_A1               = "A1";
const char* const ACS_USAFM_Severity_A2               = "A2";
const char* const ACS_USAFM_Severity_A3               = "A3";
const char* const ACS_USAFM_Severity_O1               = "O1";
const char* const ACS_USAFM_Severity_O2               = "O2";
const char* const ACS_USAFM_Severity_CEASING          = "CEASING";
const char* const ACS_USAFM_Severity_EVENT            = "EVENT";

const char* const ACS_USAFM_activeFmAlarmList         = "activeFmAlarmList";
const char* const ACS_USAFM_activeNode                = "activeNode";
const char* const ACS_USAFM_alarm                     = "alarm";
const char* const ACS_USAFM_processName               = "processName";
const char* const ACS_USAFM_specificProblem           = "specificProblem";
const char* const ACS_USAFM_severity                  = "severity";
const char* const ACS_USAFM_probableCause             = "probableCause";
const char* const ACS_USAFM_objClassOfReference       = "objClassOfReference";
const char* const ACS_USAFM_objectOfReference         = "objectOfReference";
const char* const ACS_USAFM_problemData               = "problemData";
const char* const ACS_USAFM_problemText               = "problemText";
const char* const ACS_USAFM_manualCease               = "manualCease";
const char* const ACS_USAFM_node		      = "node";
const char* const ACS_USAFM_eventTime		      = "originalEventTime";

const char* const ACS_USAFM_PSO_CONFIG_PATH           = "/usr/share/pso/storage-paths/clear";
const char* const ACS_USAFM_acs_usafm                 = "acs_usafm";
const char* const ACS_USAFM_xml_filename              = "active_fm_alarm_list.xml";
const char* const ACS_USAFM_xml_attr                  = ".<xmlattr>.";

/*==============================================================
 *              Problem Text Constants
 *==============================================================*/
//LOTC TIME SYNCHRONIZATION
const char* const NTP_TIME_NOT_SYNC                         = "PROBLEM\nTIME NOT SYNCHRONIZED\n\nOFFSET\n%s\n"; 
const char* const NTP_SERVERS_UNSTABLE                      = "PROBLEM\nSERVERS UNSTABLE\n\nSERVERS\n%s\n";
const char* const NTP_SERVER_UNREACHABLE                    = "PROBLEM\nSERVER UNREACHABLE\n\nSERVERS\n%s\n";

/*===============================================================
 *		Specific Problem Constants
 *=============================================================== */
// LOTC
const unsigned int ACS_USAFM_EthernetBondingFailed1	= 8741;
const unsigned int ACS_USAFM_DiskReplicationFailed	= 8742;
const unsigned int ACS_USAFM_DiskReplicationInconsitant	= 8743;
const unsigned int ACS_USAFM_DiskUsageThresholdReached	= 8744;
const unsigned int ACS_USAFM_MemoryUsageThresholdReached= 8745;
const unsigned int ACS_USAFM_DiskReplication = 8746;
const unsigned int ACS_USAFM_EthernetBondingFailed2	= 8747;
const unsigned int ACS_USAFM_TimeSynchronizationFault	= 8748;

//CoreMw/COM
const unsigned int ACS_USAFM_ComponentCleanupFailed	= 8761;
const unsigned int ACS_USAFM_ComponentInstFailed	= 8762;
const unsigned int ACS_USAFM_CancelRestoreBackup	= 8763;
const unsigned int ACS_USAFM_NodeUnavailable		= 8764;
const unsigned int ACS_USAFM_ModelError			= 8765;
const unsigned int ACS_USAFM_AmfSIUnassigned		= 6022; // To retain design base

const unsigned int ACS_USAFM_LogStream_Reg_1_Failed			= 8766;
const unsigned int ACS_USAFM_LogStream_Reg_2_Failed			= 8767;
const unsigned int ACS_USAFM_LogStream_Sec_1_Failed			= 8768;
const unsigned int ACS_USAFM_LogStream_Sec_2_Failed			= 8769;

//PRC events
const unsigned int ACS_USAFM_InitiatePRCToAPReboot		= 8761;
const unsigned int ACS_USAFM_IntimatePRCHANodeUnavailable	= 8762;
//SECM events
const unsigned int ACS_USAFM_ValidCertUnavailable	= 8771;
const unsigned int ACS_USAFM_CertAboutToExpire		= 8772;
const unsigned int ACS_USAFM_AutoEnrollFailed		= 8773;
const unsigned int ACS_USAFM_AuthenticationFailureLimitReached = 8774;

enum swUpdateState
{
	SW_UP_UNKNOWN  = 0,
	SW_UP_INITIALIZED  = 1,
	SW_UP_PREPARE_IN_PROGRESS = 2,
	SW_UP_PREPARE_COMPLETED = 3,
	SW_UP_ACTIVATION_IN_PROGRESS = 4,
	SW_UP_ACTIVATION_STEP_COMPLETED = 5,
	SW_UP_WAITING_FOR_COMMIT = 6,
	SW_UP_COMMIT_COMPLETED = 7,
	SW_UP_DEACTIVATION_IN_PROGRESS = 8
};

enum swUpdatePrgState
{
	UNKNOWN = 0,
	CANCELLING = 1,
	RUNNING = 2,
	FINISHED = 3,
	CANCELLED = 4
};

enum nodeOpState
{
	UNKNOWN_STATE = 0,
	NODE_ENABLED = 1,
	NODE_DISABLED = 2
};

struct AlarmInfo
{
	std::string processName;
	long        specificProblem;
	std::string severity;
	std::string probableCause;
	std::string objClassOfReference;
	std::string objectOfReference;
	std::string problemData;
	std::string problemText;
	bool manualCease;
	long originalEventTime;  
	int node; // 0: cluster, 1: node 1, 2: node 2
	
	AlarmInfo() 
	{
		this->processName = "";
		this->specificProblem = -1;
		this->severity = "";
		this->probableCause = "";
		this->objClassOfReference = "";
		this->objectOfReference = "";
		this->problemData = "";
		this->problemText = "";
		this->manualCease = true;
		this->originalEventTime = 0;
		this->node = 0;
	}
	
	AlarmInfo(std::string processName, long specificProblem, std::string severity, std::string probableCause, 
				std::string objClassOfReference, std::string objectOfReference, std::string problemData, std::string problemText, 
				bool manualCease, long originalEventTime, int node)
	{
		this->processName = processName;
		this->specificProblem = specificProblem;
		this->severity = severity;
		this->probableCause = probableCause;
		this->objClassOfReference = objClassOfReference;
		this->objectOfReference = objectOfReference;
		this->problemData = problemData;
		this->problemText = problemText;
		this->manualCease = manualCease;
		this->originalEventTime = originalEventTime;
		this->node = node;
	}
	
	bool operator == (const AlarmInfo& rhs) const 
	{	
	   bool result = false;		
		
		if (rhs.processName.compare(this->processName) == 0 &&
			rhs.specificProblem == this->specificProblem &&			
			rhs.probableCause.compare(this->probableCause) == 0 &&
			rhs.objClassOfReference.compare(this->objClassOfReference) == 0 &&
			rhs.objectOfReference.compare(this->objectOfReference) == 0)
		{
			result = true;
		}
		
		return result;
	}
};

typedef std::map<std::string, int> tSiSuMap;
typedef std::list<AlarmInfo> AlarmList;

/*===============================================================
 *		CLASS DECLARATION SECTION
 *=============================================================== */


/** @class acs_usa_evtConsumer acs_usa_evtConsumer.h
 * 	@brief 	acs_usa_evtConsumer class
 * 	@author
 * 	@date
 * 	@version
 * 	acs_usa_evtConsumer consumes the events from com and forwards it to Alarm handler
 */

class acs_usa_evtConsumer {
	
	public:

		/*  **************
		 *  Constructor
		 *  **************/

		/*  @brief acs_usa_class Default constructor
		 *  @remarks	-
		 */
		acs_usa_evtConsumer();
		/*  **************
		 *  Destructor
		 *  *************/
		/*  @brief acs_usa_class Default Destructor
		 *  @remarks    -
		 */
		~acs_usa_evtConsumer();


		/*  **************
		 *  Functions
		 *  *************/

		/* @brief init method
		 * init method is used to register USAFM component with com process
		 * @param accessor
		 * @param config
		 * @return ComOk
		 */
		ComReturnT init(ComMgmtSpiInterfacePortalAccessorT* accessor, const char* config);
		/* @brief terminate method
		 *  terminate method unregistars the USAFM component with com process
		 * @return ComOk
		 */
		ComReturnT terminate();

		ComReturnT start();
		/* @brief start  method
		 * start method starts the USAFM component
		 * @return ComOk
		 */
		ComReturnT stop();

		/* @brief notify method
		 * notify method will be invoked when an event is reported in com
		 * @param handle com handle
		 * @param eventType  Notified event type
		 * @param filter
		 * @param value Notification event value
		 * @return ComOk
		 */
		ComReturnT notify(ComOamSpiEventConsumerHandleT handle, const char* eventType, ComNameValuePairT** filter, void* value);

		/* @brief igetInstance method
		 * getInstance method is used to get the acs_usa_evtConsumer object instance
		 * @return acs_usa_eventConsumer instance
		 */
		static acs_usa_evtConsumer& getInstance(void);

		static const uint64_t NANOS_PER_SEC = 1000000000L;

	private:

		ComMgmtSpiComponent_1T _component;
		ComMgmtSpiInterface_1T* _ifArray[1];
		ComMgmtSpiInterface_1T* _depArray[4];

		// pointer to event router interface, consumer interface and consumer handle
		ComOamSpiEventRouter_1T* _eventRouter;
		ComOamSpiEventConsumer_1T _eventConsumer;
		ComOamSpiEventConsumerHandleT _consumerHandle;

		ComNameValuePairT _fmNotificationFilter;
		ComNameValuePairT* _fmNotificationFilters[2];

		ComMgmtSpiInterfacePortal_1T* _portal;
		//ComReturnT loadConfguration( const std::string& config );

		static acs_usa_evtConsumer _instance;
		int mThisNodeId;
		int mPeerNodeId;
		std::string getSeverityStr(MafOamSpiNotificationFmSeverityT severity);
		std::string getTimeStr(uint64_t datetime);
		void getProcessName(const char *dnPart, char* prosName);
		bool isSwUpdateInProgress(void);
		bool isClusterRebootInProgress(void);
		bool isClusterRebootInProgress(const char* fileName);
		bool isClusterReplicated();

		tSiSuMap mSiSuList;
		AlarmList alarmList;
		std::string strPeerNodeAmfDN;
		
		std::list<std::string> mFalseAlarmList;
		bool removeFalseAlarm(std::string siPros);
		bool insertIntoFalseAlarmList(std::string appName);
		
		int fetchClsAttributes( const char *clsName );
		int getNodeId (const char* nodeIdfilePath);

		std::string getValue(std::string data, const char* compStr);
		bool isPeerNodeDown(std::string amfNode);

		bool insertIntoSiSuMap(std::string key, int value);
		bool getSuId(std::string siPros , int &suId);

		int severity; //!< Previous severity saved by and used in USAFM_ETHERNET_BONDING_FAILED alarm handling

		/*! @brief Returns the slot and the ap number of the passed node
		 *
		 * @param[in] 	hostName	The name of the node
		 * @param[out] 	slot			The slot number
		 * @param[out] 	ap				The ap number
		 */
		bool getSlotAp(string hostName, unsigned short &slot, unsigned short &ap);

		/*! @brief Returns the first line of the passed file
		 *
		 * @param[in] 	file			The complete name of the file
		 * @return 								The first line (as string)
		 */
		std::string getFileContent(std::string file);

		/*! @brief Extracts the token value following the passed token name
		 *
		 * @param[out] 	dest			The pointer of the buffer where to write the token value
		 * @param[in] 	source		The string where to look for the token name
		 * @param[in] 	tok				The token name
		 * @param[in] 	delimiter The delimiter terminating the token value
		 * @return 								true, if the token has been extracted, false otherwise
		 */
		bool getToken(char *dest, char *source, const char *tok, char delimiter);


		/*! @brief Returns the teaming status
		 *
		 * Function cloned from APBM
		 *
		 * @return 								-1 if error, the teaming status value otherwise
		 */
		int check_teaming_status();

		/*=================================================================== */

		/**
		* @brief		GetApgOamAccess
		* 			This method is used to get the APG OAM access.
		* @return		int
		* 			 0: front cable
		* 			 1: no cable (cable-less)
		* 			 2: no applicable
		*/
		/*=================================================================== */
		int getApgOamAccess() ;
        
        /*=================================================================== */
        /*! @brief Returns the AP NODE NUMBER
        *
        *This method is used to get the APG NODE NUMBER.
        * @return               int
        *                               1: AP1
        *                               2: AP2
        *
        * @return                                                               -1 if error, the APG NODE NUMBER value otherwise
        */
        /*=================================================================== */
        int getApNodeNumber() ;

		
		/*! @brief Compute the problem description for alarm USAFM_ETHERNET_BONDING_FAILED
		 *
		 * @param[out]	description		The pointer of the buffer where to write the description value
		 * @param[in]	originalAdditionalText	Additional text of original notification from COM
		 * @return				true in case of success, false otherwise
		 */
		bool getBondingProblemDescription(char *description,char *originalAdditionalText);	
		
		/*! @brief Read the Additioanl text for alarm USAFM_LOG_STREAMING_FAILED
		 *
		 * @param[out]	hostName		The pointer of the buffer where to write the hostname value
		 * @param[out]	remoteServerId	The pointer of the buffer where to write the remoteid value 
		 * @param[in]	originalAdditionalText	Additional text of original notification from COM
		 * @param[in]	originalNotificationDn	Notification dn of original notification from COM
		 * @return				true in case of success, false otherwise
		 */
		bool getLogmDescription(char *hostName, char *remoteServerId, char *originalAdditionalText , char *originalNotificationDn, bool cease);

		
		/*! @brief Return the persistent storage path for alarm list xml file
		 *
		 *	@return 	string	 persistent storage path
		 */
		std::string getPersistentStoragePath();
		
		/*! @brief Flush the alarm list to xml file
		 */
		void saveAlarmList(bool isStopping = false);
		
		/*! @brief Load the alarm list from xml file
		 */
		void loadAlarmList();
		
		/*! @brief Add alarm to alarm list
		 */
		void addToAlarmList(const AlarmInfo& alarm);
		
		/*! @brief Remove alarm from alarm list
		 */
		void removeFromAlarmList(const AlarmInfo& alarm);

		/*! @brief Erase specific alarm from alarm list
		*/
		void eraseFromAlarmList(const AlarmInfo& alarm);

		bool isVirtual();
                bool readMissedAlarmNotifications(const AlarmInfo& alarm,unsigned int long minorType,int majorType,long eventTime);

		
		ACS_TRA_Logging log;
};

#endif /* end of ACS_USA_EVTCONSUMER_H */

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

