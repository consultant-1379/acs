/*=================================================================== */
/**
        @file          acs_aca_common.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_ACA_Common class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       19/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */
#ifndef ACS_ACA_COMMON_H
#define ACS_ACA_COMMON_H
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>

#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"
#include "acs_aca_logger.h"


/*=====================================================================
                                DEFINE DECLARATION SECTION
==================================================================== */
#define ULONGLONG unsigned long long

//DirectDataTransferM class defines
#define ACS_CONFIG_IMM_DDT_CLASS_NAME "AxeDirectDataTransferDataSource"
#define DDT_DATA_SOURCE_ID "dataSourceId"

// DataRecordM class defines
#define ACS_CONFIG_IMM_ACA_CLASS_NAME "AxeDataRecordDataRecordM"
#define ACA_DATA_RECORD_M_ID "AxeDataRecorddataRecordMId"
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
#define ACA_OS_CACHING_ENABLED "osCachingEnabled"
#endif

// MessageStore class defines
#define ACS_CONFIG_IMM_ACA_MS_CLASS_NAME "AxeDataRecordMessageStore"
#define ACA_MSG_STORE_IMPL_NAME "MsgStoreImplmenter"
#define ACA_MSG_STORE_ID "messageStoreId"
#define ACA_WIN_SIZE "windowSize"
#define ACA_ECHO_VALUE "echo"
#define ACA_ALARM_SEVERITY "alarmSeverity"
#define ACA_ALARM_LEVEL "alarmLevel"
#define ACA_RESEND_TIME "resendTime"
#define ACA_RECORD_SIZE "recordSize"
#define ACA_CONNECTION_TIME "reConnectionTime"
#define ACA_NUMBER_OF_RESENDS "numberOfResends"
#define ACA_NUMBER_OF_RECORDS "numberOfRecords"
#define ACA_NUMBER_OF_FILES "numberOfFiles"
#define ACA_NUMBER_OF_CONNECTIONS "numberOfConnections"
#define ACA_FILECLOSURE_AGING_THRESHOLD "fileClousureAgingThreshold"
#define ACA_MESSAGESTORE_NAME "messageStoreName"

// Jobs class defines
#define ACA_BLOCK_BASED_JOB_ID "blockBasedJobId"
#define ACA_FILE_BASED_JOB_ID "fileBasedJobId"
#define ACA_FIXED_RECORDS_FLAG "fixedFileRecordsFlag"
#define ACA_FIXED_RECORDS_FLAG_FIXED 0
#define ACA_RECORD_LENGTH "recordlength"
#define ACA_BLOCK_LENGTH "length"


//Alarm Severity
namespace alarmSeverity
{
	const int ALARM_HIGH    = 0;
	const int ALARM_MEDIUM  = 1;
	const int ALARM_LOW     = 2;
	const int ALERT_HIGH    = 3;
	const int ALERT_LOW     = 4;
};


// Thread state type
enum thread_state_t {
	THREAD_STATE_NOT_STARTED,
	THREAD_STATE_RUNNING,
	THREAD_STATE_STOPPED
};

namespace apgccErrorCodes
{
	const int ACS_APGCC_ERR_NOT_FOUND = -41;
};

/*====================================================================
                                CONSTANT DECLARATION SECTION
==================================================================== */
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

/*====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_ACA_Common
{
	/*====================================================================
	  PUBLIC DECLARATION SECTION
  ==================================================================== */
	public:
	static bool GetDataDiskPath (std::string & pszLogicalName, std::string & pszPath);
	static bool GetDataDiskPathForCp (std::string & pszLogicalName, unsigned int cpId, std::string & pszPath);
	static bool getIMMData (std::vector<ACS_APGCC_ImmAttribute *> * attributes, short index);
	static bool  GetDDTDataSourceNames(std::vector<std::string> & dsNames);
	static bool GetLastFieldValueFromDN(const std::string& stringToParser, std::string& value);
};
#endif
