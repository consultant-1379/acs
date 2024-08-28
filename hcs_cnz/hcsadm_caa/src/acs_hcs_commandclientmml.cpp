//======================================================================
//
// NAME
//      CommandClientMml.cpp
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
//      2011-05-20 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <sstream>
#include <stdexcept>

#ifdef ACS_HCS_SIMULATION_USE
#include <fstream>
#include <ace/Os.h>
#endif // ACS_HCS_SIMULATION_USE

#include "ace/Process.h"
//#include <ace/OS.h>

#include "acs_hcs_cache.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_commandclientmml.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_jobscheduler.h"
#include <syslog.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace ACE_OS;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	CommandClientMml::CommandClientMml()
	{
		
	}

	CommandClientMml::~CommandClientMml()
	{
	}

#ifdef ACS_HCS_SIMULATION_USE

	string CommandClientMml::executeSimulated(const DocumentXml::Node& command, const string&) const
	{
		DEBUG("%s","Entering CommandClientMml::executeSimulated()");
		string fileNameCmdResult = Cache::Singleton::get()->get(command);

		if (!fileNameCmdResult.empty())
		{
			DEBUG("%s","Leaving CommandClientMml::executeSimulated() ");
			//cout << "executeSimulated(): Printout file found in cache: " << fileNameCmdResult << "." << endl;
			return fileNameCmdResult;
		}

		fileNameCmdResult = Cache::Singleton::get()->touch(command);

		string dirNames;

		// Read simulated data from directories pointed to by environment variable "ACS_HCS_DIR_SIMULATION".

		const char* envDirSimulation = ACE_OS::getenv("ACS_HCS_DIR_SIMULATION");

		if (envDirSimulation != 0)
			dirNames = envDirSimulation;
		else
			dirNames = Configuration::Singleton::get()->getPrintoutDirs();

		if (dirNames.empty())
			ACS_HCS_THROW(CommandClient::ExceptionIo, "executeSimulated()", "Unspecified directory for printouts.", "");

		//cout << "executeSimulated(): Printouts are read from directori es '" << dirNames << "'." << endl;

		unsigned int posStart = 0;
		unsigned int posEnd;

		list<string> fileNames;

		do
		{
			posEnd = dirNames.find_first_of(";", posStart);
			string dirName = dirNames.substr(posStart, posEnd - posStart);
			Directory dir(dirName);
			dir.ls(fileNames, true);
			posStart = posEnd + 1;
		}
		while (posEnd != string::npos);

		list<string>::const_iterator fileName;

		bool found = false;
		string cmd = command.getValue();

		for (fileName = fileNames.begin(); !found && fileName != fileNames.end(); ++fileName)
		{
			ifstream ifs((*fileName).c_str());

			if (ifs.is_open())
			{
				string line;

				if (ifs.good())
				{
					getline (ifs, line);

					unsigned int pos = line.rfind(cmd);

					if (pos != string::npos)
					{
						found = true;

						//cout << "executeSimulated(): Printout file found: " << (*fileName) << ". Copying..." << endl;

						unsigned long nBytes = 0;
						ofstream ofs(fileNameCmdResult.c_str());

						if (ofs.is_open())
						{
							ofs << line.substr(pos) << endl << endl;
							nBytes += line.size();

							const int BUF_SIZE = 8192;
							char buf[BUF_SIZE];

							while (ifs.good())
							{
								ifs.read(buf, BUF_SIZE);
								ofs.write(buf, ifs.gcount());
								nBytes += ifs.gcount();
							}

							ofs.close();
						}
						else
						{
							ifs.close();

							ACS_HCS_THROW(CommandClient::ExceptionIo, "executeSimulated()", "Error opening file '" << fileNameCmdResult << "'.", "");
						}

						//cout << "executeSimulated(): Printout file found: " << (*fileName) << ". Copied " << nBytes << " bytes" << endl;
					}
				}

				ifs.close();
			}
			else
			{
				ACS_HCS_THROW(CommandClient::ExceptionIo, "executeSimulated()", "Error opening file '" << fileNameCmdResult << "'.", "");
			}
		}

		if (!found)
		{
			stringstream s;
			s << "Unknown command '" << cmd << "'.";
			throw CommandClient::ExceptionIo("executeSimulated()", s.str(), "");
		}

		DEBUG("%s","Leaving CommandClientMml::executeSimulated()");
		return fileNameCmdResult;
	}

#else // ! ACS_HCS_SIMULATION_USE

	string CommandClientMml::execute(const DocumentXml::Node& command, const string& cpName)
	{
		//DEBUG("%s","Entering CommandClientMml::execute()");
		string fileNameCmdResult = Cache::Singleton::get()->get(command);
		cout<<"fileNameCmdResult is "<<fileNameCmdResult<<endl;
		if (!fileNameCmdResult.empty())
		{
			//ACS_HCS_TRACE("execute(): Printout file found in cache: " << fileNameCmdResult << ".");
			return fileNameCmdResult;
		}

		fileNameCmdResult = Cache::Singleton::get()->touch(command);
                ofstream ofs1("/data/opt/ap/internal_root/cp/mml/test");
		cout << "File created" << endl;
                ofs1 << command.getValue();

                if (ofs1.is_open())
                      ofs1.close();

		ACE_Process_Options options;
        	ACE_HANDLE file;
        	file = ACE_OS::open(fileNameCmdResult.c_str(), O_RDWR, S_IRWXO);
      		options.set_handles(NULL, file, file);
		string mmlCmd = "mml ";
		if(!cpName.empty())
			mmlCmd = mmlCmd + "-cp " + cpName + " ";
		char cmd[256];
		mmlCmd = mmlCmd + "-f /data/opt/ap/internal_root/cp/mml/test";
		ACE_OS::strcpy(cmd, mmlCmd.c_str());
        	options.command_line("%s", cmd);
		options.setenv("PORT=4422");
		cout << "Executing " << cmd << endl;
		syslog(LOG_INFO, "Executing %s", cmd);
        	ACE_Process child;
        	child.spawn (options); // Make the ''recursive'' call.

		//Adding new code when killing child will take time.
		//ACE_Time_Value tv(0, 50000);
		ACE_Time_Value tv;
		tv.sec(ACE_OS::time(0));
		//tv.sec(tv.sec() + .5); 
        	long waitResult = 0;
		do
		{
			if(jobScheduler::stopRequested) //Call for graceful termination is received
			{
				child.terminate();	
				break;
			}
			else
			{
				DEBUG("%s","In command client loop before child.wait ");
				//waitResult = child.wait (tv,0);
				tv.sec(tv.sec() + .5);
				waitResult = child.wait (tv);
				DEBUG("%s","In command client loop after child.wait ");	 //Waiting for the process to exit
			}
		}while(waitResult == 0 || waitResult == -1 );    //'0' means timed out
                                                                 //'-1' means error in closing thread	
		
		//DEBUG("%s","After child.wait");
		ACE_OS::close(file);
		cout << "Executed : " << cmd << endl;
		//DEBUG("%s","Leaving CommandClientMml::execute()");
		remove("/data/opt/ap/internal_root/cp/mml/test");
		//remove("/data/opt/ap/internal_root/cp/test");
		return fileNameCmdResult;
	}

#endif // ACS_HCS_SIMULATION_USE

	void CommandClientMml::assertCommandIsAllowed(const DocumentXml::Node& command) const
	{
		DEBUG("%s","Entering CommandClientMml::assertCommandIsAllowed()");
		bool allowed = false;

		try
		{
			const string& strCommand = command.getValue();

			// Only ....P MML-commands are allowed.
			allowed = (strCommand.find_first_of(";") == strCommand.find_last_of(";") && strCommand.at(4) == 'P');
		}
		catch (const std::out_of_range & )
		{
		}

		if (! allowed)
			ACS_HCS_THROW(CommandClient::ExceptionCommandForbidden, "assertCommandIsAllowed()", "Command '" << command.getValue() << "' is not allowed to be sent.", "Not a single 'PRINT' command.");
		DEBUG("%s","Leaving CommandClientMml::assertCommandIsAllowed()");
	}

	string CommandClientMml::send(const DocumentXml::Node& command, const string& cpName)
	{
		//DEBUG("%s","Entering CommandClientMml::send()");
		assertCommandIsAllowed(command);

		string fileNameCmdResult;

#ifdef ACS_HCS_SIMULATION_USE
		fileNameCmdResult = this->executeSimulated(command, cpName);
#else // ! ACS_HCS_SIMULATION_USE
		fileNameCmdResult = this->execute(command, cpName);
#endif // ACS_HCS_SIMULATION_USE

		//ACS_HCS_TRACE("send(): Passed.");

		//DEBUG("%s","Leaving CommandClientMml::send()");
		return fileNameCmdResult;
	}
}
