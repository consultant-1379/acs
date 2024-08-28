//======================================================================
//
// NAME
//      ExitCodes.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-09-20 by EEDSTL
// CHANGES
//     
//======================================================================

#include <string>

#include "acs_hcs_exitcodes.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class ExitCodes
	//================================================================================

	const char* ExitCodes::SEP = "_|_";

	const char* ExitCodes::exitCodeToStr[] =
	{
		"Ok",                                                                  // EC_OK                     =  0
		"General fault.",                                                      // EC_GENERAL_FAULT          =  1
		"Incorrect usage.",                                                    // EC_INCORRECT_USAGE        =  2
		"Job not found.",                                                      // EC_JOB_NOT_FOUND          =  3,
		"File not found.",                                                     // EC_FILE_NOT_FOUND         =  4,
		"Transfer queue not found.",                                           // EC_TRANSQ_NOT_FOUND       =  5,
		"Unreasonable value.",                                                 // EC_UNREASONABLE_VALUE   =    6,
		"HC Engine not responding.",		                                   // EC_SERVER_NOT_RESPONDING  =  7,
		"Job already exists.",                                                 // EC_JOB_ALREADY_EXISTS     =  8,
		"Start time of the job has expired.",                                  // EC_JOB_STARTTIME_EXPIRED  =  9,
		"Failed to reduce the data size below the maximum allowed limit.",     // EC_REDUCE_DATA_SIZE       = 10,
		"Authorization failure: Cluster operation mode is EXPERT.",            // EC_CMD_AUTH_FAILURE       = 11,
		"Current CPU usage exceeds the maximum allowed limit.",                // EC_CPU_LIMIT              = 12,
		"Current memory usage exceeds the maximum allowed limit.",             // EC_MEMORY_LIMIT           = 13,
		"Command allowed only on AP1 active node.",                            // EC_CMD_PASSIVE_NODE       = 14,
		"Command cannot be executed. Node may go for a reboot.",               // EC_CMD_REBOOT             = 15,
		"Another job is in progress.",                                         // EC_JOB_IN_PROGRESS        = 16,
		"Failed to transfer the report file(s) to the specified destination.", // EC_TRANSQ_FAILURE         = 17,
		"Job name should not exceed 20 characters.",                           // EC_JOB_INVALID_NAME       = 18,
		"HCS directory not found.",                                            // EC_DIR_NOT_FOUND          = 19,
		"CP Name or Group not found."                                          // EC_CP_NOT_FOUND           = 20
	};

	stringstream& ExitCodes::exitCodeToReply(stringstream& ss, ExitCode ec, const string& details)
	{
		ss << ec << SEP << exitCodeToStr[ec] << SEP << details;

		return ss;
	}
}