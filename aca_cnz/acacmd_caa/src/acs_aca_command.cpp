#include <time.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

#include "ace/ACE.h"

#include "acs_aca_api_tracer.h"
#include "acs_aca_command.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_Command)

using namespace std;

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_Command::ACS_ACA_Command(ACE_INT32 argc, ACE_TCHAR* argv [])
	: argc_ (argc), argv_ (argv) {
	ACS_ACA_TRACE_FUNCTION;
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_Command::~ACS_ACA_Command(void) {
	ACS_ACA_TRACE_FUNCTION;
}

//------------------------------------------------------------------------------
//      Launch command
//------------------------------------------------------------------------------
void ACS_ACA_Command::launch() {
	ACS_ACA_TRACE_FUNCTION;

	try {
		ACS_ACA_TRACE_MESSAGE("Parsing the given command line...");
		parse(); //parsing command line

		ACS_ACA_TRACE_MESSAGE("Executing the command with the parsed options!");
		execute(); //execution command
	}
	catch (int exc) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Error when executing the command, exception_id = %d", exc);
		char error_message[512] = {0};

		switch (exc) {
		case USAGE_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::INCORRECT_USAGE_SCP_SYSTEM_ERROR_MESSAGE);
			break;

		case USAGE_EXCEPTION_NEW:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::INCORRECT_USAGE_MCP_SYSTEM_ERROR_MESSAGE);
			exc -= 10;
			break;

		case MSNAME_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::MESSAGE_STORE_NOT_FOUND_ERROR_MESSAGE);
			break;

		case DATA_FILE_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::DATA_FILE_ERROR_ERROR_MESSAGE);
			break;

		case COMMIT_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::COMMIT_FILE_ERROR_ERROR_MESSAGE);
			break;

		case STAT_FILE_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::STAT_FILE_ERROR_ERROR_MESSAGE);
			break;

		case EOF_FILE_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::EOF_FILE_ERROR_ERROR_MESSAGE);
			break;

		case SEND_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::SEND_FAILED_ERROR_MESSAGE);
			break;

		case RECV_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::RECV_FAILED_ERROR_MESSAGE);
			break;

		case CS_EXCEPTION55:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::CS_CONNECT_ERROR_MESSAGE);
			break;

		case CS_EXCEPTION56:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::CS_ERROR_ERROR_MESSAGE);
			break;

		case APNAME_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::AP_NOT_DEFINED_ERROR_MESSAGE);
			break;

		case OPTION_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::ILLEGAL_OPTION_ERROR_MESSAGE);
			break;

		case CONNECT_EXCEPTION:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::CONNECT_FAILED_ERROR_MESSAGE);
			break;

		case CS_EXCEPTION118:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), ACS_ACA_MSDLS_ErrorMessages::CP_NOT_DEFINED_ERROR_MESSAGE);
			break;

		case GENERAL_EXCEPTION:
		default:
			::snprintf(error_message, ACS_ACA_ARRAY_SIZE(error_message), "%s: %s",
					ACS_ACA_MSDLS_ErrorMessages::INTERNAL_PROGRAM_FAULT_FORMAT, ACS_ACA_MSDLS_ErrorMessages::ERROR_WHEN_EXECUTING_ERROR_MESSAGE);
		}

		ACS_ACA_TRACE_MESSAGE("ERROR: Exiting with error_code = %d and error_message = '%s'!", exc, error_message);
		::printf("%s\n", error_message);
		::exit(exc);
	}

	ACS_ACA_TRACE_MESSAGE("The command has been executed successfully!");
}
