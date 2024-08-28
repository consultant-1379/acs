//======================================================================
//
// NAME
//      HcjlsCmd.cpp
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

#include "acs_hcs_exitcodes.h"
#include "acs_hcs_jobtotxt.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_hcjlscmd.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class HcjlsCmd
	//================================================================================

	HcjlsCmd::HcjlsCmd(char* cmdMsg, ACE_HANDLE  pipe) : HealthCheckCmd(cmdMsg, pipe)
	{
	}

	HcjlsCmd::~HcjlsCmd()
	{
	}

	int HcjlsCmd::execute()
	{
		stringstream msg;

		try
		{
			JobToTxt toTxt(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_JOB_NAME), this->cmdArgs.exists(HealthCheckCmd::CmdArgs::OPT_DETAILS));
			toTxt.toString(msg);
		}
		catch (const JobToTxt::ExceptionJobNotFound& ex)
		{
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::*/EC_JOB_NOT_FOUND, ex.what());
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
			ExitCodes::exitCodeToReply(msg, ExitCodes::/*ExitCode::i*/EC_GENERAL_FAULT, "Unknown fault.");
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
