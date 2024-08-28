//======================================================================
//
// NAME
//      HealthCheckCmd.cpp
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
//      2011-06-22 by EGINSAN
// CHANGES
//     
//======================================================================

#include <iostream>
#include <map>
#include <string>

#include <ace/Thread_Manager.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_tracer.h"

#include "acs_hcs_directory.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_scheduler.h"
#include "acs_hcs_healthcheckcmd.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class HealthCheckCmd
	//================================================================================

	bool HealthCheckCmd::stopEventFlag = false ;

	HealthCheckCmd::HealthCheckCmd(char* cmdMsg, ACE_HANDLE pipe) : pipe(pipe)
	{
		char* pch;
		string option;
		string temp;
		int nArgs=0;

		pch = strtok(cmdMsg, " ");

		while (pch != 0)
		{
			if (!option.empty())
			{
				this->cmdArgs.set(option, pch);
				option.clear();
			}

			if (*pch == '-')
			{
				option = pch;
			}

			temp = pch;
			nArgs++;
			pch = strtok (0, " ");
		}	

		if ( nArgs != 1 )
		{
			this->cmdArgs.set(HealthCheckCmd::CmdArgs::OPT_JOB_NAME, temp);
		}
	}

	HealthCheckCmd::~HealthCheckCmd()
	{
		this->disconnect();
	}

	void HealthCheckCmd::disconnect()
	{
		// Flush the pipe to allow the client to read the pipe's contents 
		// before disconnecting. Then disconnect the pipe, and close the 
		// handle to this pipe instance. 

		//FlushFileBuffers(this->pipe); 
		//DisconnectNamedPipe(this->pipe); 
		//close(this->pipe); 
	}


	int HealthCheckCmd::sendStatus(const string& statusMsg)
	{
		ACE_UINT32 numBytesWritten = 0;

		//bool success ;//= WriteFile(this->pipe, statusMsg.c_str(), statusMsg.size(), &numBytesWritten, NULL);

		if (/*!success ||*/ numBytesWritten != statusMsg.size())
		{
			//ACS_HCS_TRACE("sendStatus(): Error sending message '" << statusMsg.substr(0, 1024) << "'. Error code=" <<  ACE_OS::last_error());
			return -1;
		}

		return 0;
	}

	int HealthCheckCmd::Start(HealthCheckCmd* cmd)
	{
		cmd->activate();
		/*
		ACE_hthread_t hThread = NULL;
		if (ACE_Thread_Manager::instance ()->spawn(ACE_THR_FUNC (&HealthCheckCmd::Execute),
			                                       cmd,
												   THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED,
												   0,
												   &hThread,
												   ACE_DEFAULT_THREAD_PRIORITY,
												   -1,
												   0,
												   ACE_DEFAULT_THREAD_STACKSIZE,
												   0) == -1)
		{
			cout << "Start(): Error starting thread for command execution." << endl;
			return -1;
		}

		close(hThread);
                */
		return 0;
	}

	//void HealthCheckCmd::Execute(void* cmd)
	int HealthCheckCmd::svc(void)
	{
		if ( !Synchron::ThrExitHandler::init() )
		{
			cout << "Error Occured while registering for Thread Exit Handler."<<endl;
			//ACS_HCS_TRACE("Error Occured while registering for Thread Exit Handler.");
		}

		//((HealthCheckCmd*)cmd)->execute();
		this->execute();
		return 0;
	}

	//================================================================================
	// Class HealthCheckCmd::CmdArgs
	//================================================================================

	const char* HealthCheckCmd::CmdArgs::OPT_ADD_RULE    = "-a";
	const char* HealthCheckCmd::CmdArgs::OPT_BACKGROUND  = "-b";
	const char* HealthCheckCmd::CmdArgs::OPT_CATS        = "-c";
	const char* HealthCheckCmd::CmdArgs::OPT_CP_NAME     = "-n";
	const char* HealthCheckCmd::CmdArgs::OPT_DETAILS     = "-l";
	const char* HealthCheckCmd::CmdArgs::OPT_EXPERT_USER = "-e";
	const char* HealthCheckCmd::CmdArgs::OPT_JOB_NAME    = "jobName";
	const char* HealthCheckCmd::CmdArgs::OPT_REPORT      = "-f";
	const char* HealthCheckCmd::CmdArgs::OPT_RULES       = "-r";
	const char* HealthCheckCmd::CmdArgs::OPT_RULE_SET    = "-f";
	const char* HealthCheckCmd::CmdArgs::OPT_RULE_SETS   = "-f";
	const char* HealthCheckCmd::CmdArgs::OPT_SCHEDULE    = "-s";
	const char* HealthCheckCmd::CmdArgs::OPT_ITERATIONS  = "-i";
	const char* HealthCheckCmd::CmdArgs::OPT_USER_NAME   = "userName";
	const char* HealthCheckCmd::CmdArgs::OPT_TRANS_QUEUE = "-q";

	bool HealthCheckCmd::CmdArgs::exists(const string& key) const
	{
		Args::const_iterator arg = this->args.find(key.c_str());

		return (arg != this->args.end());
	}

	void HealthCheckCmd::CmdArgs::set(const string& key, const string& value)
	{
		this->args[key] = value;
	}

	string HealthCheckCmd::CmdArgs::get(const string& key) const
	{
		Args::const_iterator arg = this->args.find(key.c_str());

		return (arg != this->args.end() ? (*arg).second : "");
	}
}
