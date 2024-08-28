/*=================================================================== */
/**
   @file  acs_rtr_global.h 

   @brief Header file for rtr module.

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
   N/A       24/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_GLOBAL_H_
#define _ACS_RTR_GLOBAL_H_

#define MAX_BLOCK_TRANSACTION 100
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_lib.h"

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

namespace
{
	const int SUCCESS 					= 0;
	const int FAILURE 					= -1;

	const int APGCC_NOT_FOUND 			= -41;
	const int IMM_MO_ALREADYEXIST 		= -14;
	const int REF_FILE_ERROR 			= 999;
	const int APGCC_NOT_EXISTS 			= -12;

	const int hashKeyLength 			= 32;
	const unsigned int kiloByteToByte 	= 1024U;

	const char * const DefHashKey 		="UNDEFINED";
	const char DirDelim 				='/';
}

namespace ACS_RTR_Util
{
	void dump (const char* msg_to_print, const unsigned char* buffer, ssize_t size, ssize_t dumping_size = 512, ssize_t dumping_line_length = 16);
}

namespace RTR
{
	const char* const PROCNAME = "acs_rtrd";
	const char* const SITENAME = "cp0ex";
}

namespace parseSymbol{
		const char minus = '-';
		const char plus = '+';
		const char comma = ',';
		const char equal = '=';
		const char underLine = '_';
		const char dot = '.';
		const char colon = ':';
};

namespace CmdPrintouts
{
	const char* const Executed		= "EXECUTED";
	const char* const APundef               = "AP is not defined";
	const char* const TqDefError            = "Data record transfer already defined";
	const char* const IncUsage              = "Incorrect usage";
	const char* const IntProgFault          = "Internal program fault";
	const char* const MsBufBlock            = "Message Store buffer larger than block size";
	const char* const MsBufFile             = "Message Store buffer larger than fixed record size";
	const char* const MsNotExist            = "Message Store does not exist";
	const char* const NoTQDef               = "Transfer queue not defined";
	const char* const CommError             = "Communication error";
	const char* const ConfigConn            = "Unable to connect to Configuration Server";
	const char* const ConfigError           = "Configuration Server Error";
	const char* const NoDatatransferAccess  = "No data transfer access";
	const char* const NoServerAccess        = "Unable to connect to server";
	const char* const NoMSDef               = "Data record transfer not defined";
	const char* const UnReasonval           = "Unreasonable value";
	const char* const CommCancel            = "Command cancelled by operator";
	const char* const IllegalParam          = "Illegal parameter type";
	const char* const NotAllowBlk           = "Not allowed for block based transfer";
	const char* const RmDenied              = "Data record transfer ongoing";
	const char* const GeneralFault          = "General fault";
}

namespace OHI_USERSPACE{
	const char SUBSYS[] = "ACS";
	const char APPNAME[] = "acs_rtrd";
	const char EVENTBLOCKTXT[] = "RTR block reporting";
	const char EVENTFILETXT[] = "RTR file reporting";
}

namespace rtr_imm{

	// AxeDataRecord MO
	const char* const AxeDataRecordDN = "AxeDataRecorddataRecordMId=1";

	// AxeDataRecord Class Attributes
	extern char IPaddingAttribute[];
	extern char OPaddingAttribute[];

	// IMM CLASSES OF RTR
	const char* const FileBaseJobClassName = "AxeDataRecordFileBasedJob";
	const char* const BlockBaseJobClassName = "AxeDataRecordBlockBasedJob";
	const char* const StatisticsInfoClassName = "AxeDataRecordStatisticsInfo";
	const char* const CpStatisticsInfoClassName = "AxeDataRecordCPStatisticsInfo";
	const char* const EcimPasswordClassName = "AxeDataRecordEcimPassword";

	// Job Common attributes
	extern char TransferQueueAttribute[];
	extern char HoldTimeAttrribute[];
	extern char PaddingCharAttribute[];

	// File Base Job Class attributes
	extern char FileBaseJobId[];
	extern char CdrCounterFlagAttribute[];
	extern char FileRecordlengthAttribute[];
	extern char FixedFileRecordsFlagAttribute[];
	extern char FileSizeAttribute[];
	extern char MinFileSizeAttribute[];

	extern char HashKeyAttribute[];
	extern char HashKeyId[];

	// ECIM Password Structure
	const char* const PasswordRDN = "id=hashKey_0";
	extern char PasswordAttribute[];

	// Block Base Job Class attributes
	extern char BlockBaseJobId[];
	extern char BlockLengthAttribute[];
	extern char BlockLengthTypeAttribute[];

	// CpStatisticsInfo Class attributes
	extern char CpStatisticsInfoId[];
	extern char RecordsReadAttribute[];
	extern char RecordsLostAttribute[];
	extern char RecordsSkippedAttribute[];

	// StatisticsInfo Class attributes
	extern char StatisticsInfoId[];
	extern char VolumeReadAttribute[];
	extern char VolumeReportedAttribute[];

	// Message Store Class attributes
	const char* const MessageStoreId = "messageStoreId";
	extern char MessageStoreNameAttribute[];
	extern char RecordSizeAttibute[];

}

namespace actionResult
{
		const char NBI_PREFIX[] ="@ComNbi@"; // To show message inside Com-Cli
		const int SUCCESS = 1; // SA_AIS_OK
		const int FAILED = 21; // SA_AIS_ERR_FAILED_OPERATION
		const int NOOPERATION = 28; // SA_AIS_ERR_NO_OP

		extern char ErrorCodeAttribute[];
		extern char ErrorMessageAttribute[];
};


#define ULONGLONG unsigned long long


#define END_CURRENT_DATAFILE		1
#define SET_FILE_KEY			2

#define ENABLE 				1

#define VARIABLE			1
#define FIXED				0
#define EVEN				2

#define UNDEFINED			1



namespace RTR_NBI{

	const char* const FILEM_ATTRIBUTE = "sourceCallRecords";
	const char* const FILEBUILDING_FOLDER = "/message_store/";

	const char* const FILETRANSFER_FOLDER = "/transfer_store/";

	const char* const DATADISK_ATTRIBUTE = "ACS_RTR_DATA";

	const char* const DataDiskPath = "/data/acs/data/rtr";

}
/*=====================================================================
                          CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      FAILURE_RETRIES
 */
/*=================================================================== */
const int FAILURE_RETRIES = 5;
/*=================================================================== */
/**
   @brief      normalStart
 */
/*=================================================================== */
const int normalStart = 0;
/*=================================================================== */
/**
   @brief      internalRestart
 */
/*=================================================================== */
const int internalRestart = 1;
/*=================================================================== */
/**
   @brief      noOfBlocksPerTransaction
 */
/*=================================================================== */
const unsigned int noOfBlocksPerTransaction = MAX_BLOCK_TRANSACTION;
/*=================================================================== */
/**
   @brief      commitFileSize
 */
/*=================================================================== */
const int commitFileSize = noOfBlocksPerTransaction*2*sizeof(unsigned int)+10;
/*=================================================================== */
/**
   @brief      DEFAULT_CPID
 */
/*=================================================================== */
const short DEFAULT_CPID = ~0;
/*=================================================================== */
/**
   @brief      INVALID_BLOCK_NUMBER
 */
/*=================================================================== */
const unsigned INVALID_BLOCK_NUMBER = ~0U;
/*=================================================================== */
/**
   @brief      DEFAULT_CPNAME
 */
/*=================================================================== */
const char * const DEFAULT_CPNAME = "default_cpname";


#endif // _ACS_RTR_GLOBAL_H_
