//======================================================================
//
// NAME
//      HcjrmCmd.cpp
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
#include "acs_hcs_tracer.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_hcjrmcmd.h"
#include "acs_hcs_scheduler.h"
#include "acs_hcs_exitcodes.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class HcjrmCmd
	//================================================================================

	HcjrmCmd::HcjrmCmd(char* cmdMsg, ACE_HANDLE pipe) : HealthCheckCmd(cmdMsg, pipe)
	{
	}

	HcjrmCmd::~HcjrmCmd()
	{
	}

	int HcjrmCmd::execute()
	{
		stringstream msg;
		const string jobInExecution = JobExecutor::Singleton::get()->getCurrentJob().getName();
		const string jobforDeletion = this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_JOB_NAME);
		bool isJobStopped = false;
		bool isJobRemoved = false; //To check if the job has been deleted from the scheduler

		try
		{
			if (!strcmp(jobforDeletion.c_str(),jobInExecution.c_str()))
			{
				JobExecutor::Singleton::get()->cancel();
				isJobStopped = true;
			
				//ACS_HCS_TRACE("execute(): Current Job cancelled");
			}

			if (this->cmdArgs.exists(HealthCheckCmd::CmdArgs::OPT_SCHEDULE))
			{
				Scheduler::JobMap jobs;
				Scheduler::Singleton::get()->jobsGet(jobs);
				Scheduler::JobMap::const_iterator job;

				for (job = jobs.begin(); job != jobs.end(); ++job)
				{
					if(job->second.getName()  == jobforDeletion)
					{
						Scheduler::Singleton::get()->jobRemove(jobforDeletion);
						isJobRemoved = true;
						break;
					}
				}

				if (isJobRemoved)
				{
				//	ACS_HCS_TRACE("execute(): Job removed from scheduler");
				}
			}

			if (isJobStopped || isJobRemoved)
			{
				if(isJobRemoved)
				{
					msg << "Job " << jobforDeletion.c_str() << " has been removed successfully.";
				}
				else
				{
					msg << "Job " << jobforDeletion.c_str() << " has been stopped successfully.";
				}
			}
			else
			{
				ACS_HCS_THROW( HealthCheckCmd::ExceptionJobNotFound, "execute()", "Job '" << jobforDeletion.c_str() << "' not found.", "Job not found.");
			}	
		}
		catch (const HealthCheckCmd::ExceptionJobNotFound& ex)
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
