/*=================================================================== */
/**
@file		acs_logm_definitions.h

@brief		Header file of all LOGM Definitions Data


@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       20/08/2012     XCSSATA       Initial Release
N/A       17/01/2013     XCSRPAD       Deletion of unwanted CMX log files
==================================================================== */

/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LOGM_DEFINITIONS_H_
#define ACS_LOGM_DEFINITIONS_H_

#include <unistd.h>
#include <ACS_APGCC_CommonLib.h>
#include <map>
#include <string>
#include <list>
#include <vector>
using namespace std;

#define 	TIMEBASED_CLASS_NAME		"TimeBasedHouseKeeping"
#define 	TIMEBASED_CLASS_RDN			"timeBasedHouseKeepingId"
#define 	ATTR_SEARCH_PATTERN		 	"searchPattern"
#define 	ATTR_SEARCH_PATH		 	"searchPath"
#define 	ATTR_NUMBER_OF_FILES	 	"numberOfFiles"
#define 	ATTR_FILE_AGE			 	"fileAge"
#define 	ATTR_INITATION_HOUR		 	"initiationHour"
#define 	ATTR_INITATION_MINUTE	 	"initiationMinute"
#define 	ATTR_INITATION_FREQUENCY 	"initiationFrequency"
#define 	ATTR_FILE_TYPE			 	"fileType"
#define 	ATTR_CLEAN_ENABLE		 	"cleanEnable"

#define 	PERIODICBASED_CLASS_NAME	"PeriodicBasedHouseKeeping"
#define 	PERIODICBASED_CLASS_RDN		"periodicBasedHouseKeepingId"
#define 	ATTR_SEARCH_PATTERN		 	"searchPattern"
#define 	ATTR_SEARCH_PATH		 	"searchPath"
#define 	ATTR_NUMBER_OF_FILES	 	"numberOfFiles"
#define 	ATTR_FILE_AGE			 	"fileAge"
#define 	ATTR_PERIODIC_INTERVAL 		"periodicInterval"
#define 	ATTR_FILE_TYPE			 	"fileType"
#define 	ATTR_CLEAN_ENABLE		 	"cleanEnable"

#define 	STSOUTPUT_NBI_FOLDER 		"sourceDataForStsFile"
#define 	STS_MP				 		"STS_MP"
#define 	TRA_LOG				 		"TRA_LOG"
#define     TRA_LOG_PATH		 		"/var/log/acs/tra/logging"
#define 	CORE				 		"CORE"
#define     	CORE_PATH		 		"/var/log/core"
#define 	SCX_LOG				 		"SCX_LOG"
#define     SCX_LOG_PATH		 		"/boot/scx/scx_logs"
#define 	CMX_LOG				 		"CMX_LOG"
#define     CMX_LOG_PATH		 		"/boot/cmx/cmx_logs"
#define 	SMX_LOG				 		"SMX_LOG"
#define     SMX_LOG_PATH		 		"/boot/smx/smx_logs"
#define 	APZ_DATA_SYMBOL		 		"APZ_DATA"

#define 	LOGM_CLEANUP_ACTION 		5 // Periodic minutes for LOGM to perform cleanup action
#define		FIVE_GIGA			5368709120 // 5 * 1024 * 1024 * 1024 = 5GB

#endif /* ACS_LOGM_DEFINITIONS_H_ */
