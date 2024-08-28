//======================================================================
//
// NAME
//      HcjdefCmd.cpp
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

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "acs_hcs_exitcodes.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_transferqueue.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_environment.h"
#include "acs_hcs_scheduler.h"
#include "acs_hcs_hcjdefcmd.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class HcjdefCmd
	//================================================================================

	HcjdefCmd::HcjdefCmd(char* cmdMsg, ACE_HANDLE pipe) : HealthCheckCmd(cmdMsg, pipe)
	{
	}

	HcjdefCmd::~HcjdefCmd()
	{
	}

	int HcjdefCmd::execute()
	{
		stringstream replyMsg;

		//ACS_HCS_TRACE("execute(): Started.");

		JobExecutor::Job job(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_JOB_NAME));
		job.setRuleSetIds(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_RULE_SETS)); 
		job.setRuleIds   (this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_RULES));
		job.setRuleCats  (this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_CATS));
		job.setCpName    (this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_CP_NAME));
		job.setExpertUser(this->cmdArgs.exists(HealthCheckCmd::CmdArgs::OPT_EXPERT_USER));
		job.setTransferQueue(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_TRANS_QUEUE));

		try
		{
			
			if(this->cmdArgs.exists(HealthCheckCmd::CmdArgs::OPT_TRANS_QUEUE))
			{
				TransferQueue TQ;
				string transferQueueName = job.getTransferQueue();
				if(!TQ.isTQValid(transferQueueName))
				{
					ACS_HCS_THROW(TransferQueue::ExceptionTQNotFound, "execute()", "Transfer Queue '" << transferQueueName << "' was not found.", "Transfer queue does not exist.");
				}
			}
			
		    if (this->cmdArgs.exists(HealthCheckCmd::CmdArgs::OPT_SCHEDULE))
			{
				string schedule = this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_SCHEDULE);
				transform(schedule.begin(),schedule.end(),schedule.begin(),::toupper);

				if( schedule == "NOW" )// Schedule a background job
				{
					job.setTimeStamp();
	
					job.setIterations(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_ITERATIONS));

					Scheduler::Singleton::get()->jobAdd(job);

					replyMsg << "Job '" << job.getName() << "' has been scheduled for execution asap.";
				}
				else
				{
					job.setTimeStamp (this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_SCHEDULE));	

					job.setIterations(this->cmdArgs.get(HealthCheckCmd::CmdArgs::OPT_ITERATIONS));

					Scheduler::Singleton::get()->jobAdd(job);

					replyMsg << "Job '" << job.getName() << "' has been scheduled for execution.";
				}
			}
			else
			{
				Environment env;
				env.assertEnvSetHcjdef();

				bool expertMode;
				Environment::APType apt;
				env.getMode(expertMode);
				env.getApt(apt);
				
				if (!expertMode || job.isExpertUser())
				{
					if(! Scheduler::Singleton::get()->isJobNameUnique( job.getName() ))
						ACS_HCS_THROW( Scheduler::ExceptionJobAlreadyExists, "execute()", "Job '" << job.getName() << "' cannot be executed.", "Job name already exists.");

					JobExecutor::Singleton::get()->start(job, this);

					replyMsg << "Job '" << job.getName() << "' has been executed.";
				}
				else
				{
					ACS_HCS_THROW( HealthCheckCmd::ExceptionAuth, "execute()", "Job '" << job.getName() << "' cannot be executed as the cluster operation mode is 'EXPERT'.", "");
				}
			}
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_DIR_NOT_FOUND, ex.what());
		}
		catch (const Environment::ExceptionLimitCpu& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_CPU_LIMIT, ex.what());
		}
		catch (const Environment::ExceptionLimitMemory& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_MEMORY_LIMIT, ex.what());
		}
		catch (const HealthCheckCmd::ExceptionAuth& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_CMD_AUTH_FAILURE, ex.what());
		}
		catch (const JobExecutor::ExceptionJobDataSize& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_REDUCE_DATA_SIZE, ex.what());
		}
		catch (const JobExecutor::ExceptionJobInProgress& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_JOB_IN_PROGRESS, ex.what());
		}
		catch (const Scheduler::ExceptionJobAlreadyExists& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_JOB_ALREADY_EXISTS, ex.what());
		}
		catch (const Scheduler::ExceptionJobNotFound& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_JOB_NOT_FOUND, ex.what());
		}
		catch (const Scheduler::ExceptionJobStartTimeExpired& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_JOB_STARTTIME_EXPIRED, ex.what());
		}
		catch (const TransferQueue::ExceptionTQNotFound& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_TRANSQ_NOT_FOUND, ex.what());
		}		
		catch (const Exception& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, ex.what());
		}
		catch (const exception& ex)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, ex.what());
		}
		catch (...)
		{
			ExitCodes::exitCodeToReply(replyMsg, ExitCodes::/*ExitCode::*/EC_GENERAL_FAULT, "Unknown fault.");
		}

		replyMsg << "$$$";

		if (this->sendStatus(replyMsg.str()) == -1)
		{
			//ACS_HCS_TRACE("execute(): Failed.");

			delete this; // Must be followed by return statement.
			return -1;
		}

		//ACS_HCS_TRACE("execute(): Done.");

		delete this; // Must be followed by return statement.
		return 0;
	}
}
