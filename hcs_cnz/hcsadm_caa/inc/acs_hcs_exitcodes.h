//======================================================================
//
// NAME
//      ExitCodes.h
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
//      Implementation of the Exit Codes of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_ExitCodes_h
#define AcsHcs_ExitCodes_h

#include <string>
#include <sstream>

namespace AcsHcs
{
	/**
	* ExitCode - Definition of exit codes and utilities for exit code handling.
	*/
	class ExitCodes
	{
	public:
		enum ExitCode
		{
			EC_OK                     = 0,
			EC_GENERAL_FAULT          = 1,
			EC_INCORRECT_USAGE        = 2,
			EC_JOB_NOT_FOUND          = 3,
			EC_FILE_NOT_FOUND         = 4,
			EC_TRANSQ_NOT_FOUND       = 5,
			EC_UNREASONABLE_VALUE     = 6,
			EC_SERVER_NOT_RESPONDING  = 7,
			EC_JOB_ALREADY_EXISTS     = 8,
			EC_JOB_STARTTIME_EXPIRED  = 9,
			EC_REDUCE_DATA_SIZE       = 10,
			EC_CMD_AUTH_FAILURE       = 11,
			EC_CPU_LIMIT              = 12,
			EC_MEMORY_LIMIT           = 13,
			EC_CMD_PASSIVE_NODE       = 14,
			EC_CMD_REBOOT             = 15,
			EC_JOB_IN_PROGRESS        = 16,
			EC_TRANSQ_FAILURE         = 17,
			EC_JOB_INVALID_NAME       = 18,
			EC_DIR_NOT_FOUND          = 19,
			EC_CP_NOT_FOUND           = 20
		};
		
		static std::stringstream& exitCodeToReply(std::stringstream& ss, ExitCode ec, const std::string& details);

		static const char* SEP;
		static const char* exitCodeToStr[];
	};
}

#endif // AcsHcs_ExitCodes_h
