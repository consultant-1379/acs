//======================================================================
//
// NAME
//      HcrflsCmd.cpp
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
//      2011-06-10 by XCHVKUM
// CHANGES
//     
//======================================================================

#include <sstream>
#include <string>

#include "acs_hcs_exitcodes.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_reporttotxt.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_hcrflscmd.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class HcrflsCmd
	//================================================================================

	HcrflsCmd::HcrflsCmd(char* cmdMsg, ACE_HANDLE pipe) : HealthCheckCmd(cmdMsg, pipe)
	{
	}

	HcrflsCmd::~HcrflsCmd()
	{
	}

	int HcrflsCmd::execute()
	{
		stringstream msg;

		try
		{
			ReportToTxt toTxt(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_REPORT));
			toTxt.toString(msg);
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_DIR_NOT_FOUND, ex.what());
		}
		catch (const ReportToTxt::ExceptionFileNotFound& ex)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_FILE_NOT_FOUND, ex.what());
		}
		catch (const Exception& ex)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, ex.what());
		}
		catch (const exception& ex)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, ex.what());
		}
		catch (...)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, "Unknown fault.");
		}

		msg << "$$$";

		if (this->sendStatus(msg.str()) == -1)
		{
			delete this; // Must be followed by return statement.
			return -1;
		}

		delete this; // Must be followed by return statement.
		return 0;
	}
}
