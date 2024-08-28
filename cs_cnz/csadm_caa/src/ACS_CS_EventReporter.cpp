//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************

#include "ACS_CS_Util.h"

#include "ACS_CS_Trace.h"

#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "ACS_APGCC_Util.H"



// ACS_CS_EventReporter
#include "ACS_CS_EventReporter.h"

ACS_CS_Trace_TDEF(ACS_CS_EventReporter_TRACE);

using std::ostringstream;
using std::set;
using std::string;



ACS_CS_EventReporter::ACS_CS_EventReporter()
{
}

ACS_CS_EventReporter::~ACS_CS_EventReporter()
{
}

ACS_CS_EventReporter * ACS_CS_EventReporter::instance()
{
	return instance_::instance();
}

void ACS_CS_EventReporter::report(int event, Severity severity, const std::string &probableCause, const std::string &data, const std::string &text, std::string procName)
{
	pair<set<int>::iterator, bool> res = sentEvents.insert(event);

	if(false == res.second)
	{
		return;
	}

	ACS_AEH_ReturnType aehRes = ACS_AEH_error;

	string processName("");
	string objOfRef("");
	std::stringstream pid;

	if (strcmp(procName.c_str(),"") == 0)
	{
		ACS_APGCC::getProcessName(&procName);
		pid << getpid();
	}
	else
	{
		//HS33984
		int pidProcess = getProcIdByName(procName);
		pid << pidProcess;
		ACS_CS_FTRACE((ACS_CS_EventReporter_TRACE, LOG_LEVEL_INFO,"(%t) ACS_CS_EventReporter::report() process=[%s:%d]",procName.c_str(), pidProcess));
	}

	processName = procName + ":" + pid.str();
//	objOfRef = procName + "/" + pid.str();	//HS21311
	objOfRef = procName;	//HS21311

	string severityStr;
	severityToStr(severity, &severityStr);

	aehRes = eventReporter.sendEventMessage(
									processName.c_str(),
									event,
									severityStr.c_str(),
									probableCause.c_str(),
									"APZ",
									objOfRef.c_str(),
									text.c_str(),
									data.c_str(),
									true
									);

	if(aehRes != ACS_AEH_ok)
	{
		sentEvents.erase(event);

		ACS_CS_FTRACE((ACS_CS_EventReporter_TRACE, LOG_LEVEL_ERROR,
			"(%t) ACS_CS_EventReporter::report()\n"
			"Could not send a message!\n"
			"%d: %s", eventReporter.getError(), eventReporter.getErrorText()));
	}
}

void ACS_CS_EventReporter::cease(int event, std::string procName)
{
//	if(sentEvents.end() == sentEvents.find(event))
//	{
//		return;
//	}

	ACS_AEH_ReturnType aehRes = ACS_AEH_error;
	//char procName[64];
//	char processName[64];
//	char objOfRef[64];

	string processName;
	string objOfRef;
	std::stringstream pid;

	if (procName.empty())
	{
		ACS_APGCC::getProcessName(&procName);
		pid << getpid();
	}
	else
	{
		//HS33984
		int pidProcess = getProcIdByName(procName);
		pid << pidProcess;
	}

	processName = procName + ":" + pid.str();
//	objOfRef = procName + "/" + pid.str();	//HS21311
	objOfRef = procName;	//HS21311


	aehRes = eventReporter.sendEventMessage(
									processName.c_str(),
									event,
									"CEASING",
									"",
									"APZ",
									objOfRef.c_str(),
									"",
									""
									);

	if(aehRes == ACS_AEH_ok)
	{
		sentEvents.erase(event);
	} else {
		ACS_CS_FTRACE((ACS_CS_EventReporter_TRACE, LOG_LEVEL_ERROR,
			"(%t) ACS_CS_EventReporter::cease()\n"
			"Could not send a message!\n"
			"%d: %s", eventReporter.getError(), eventReporter.getErrorText()));
	}
}

void ACS_CS_EventReporter::severityToStr(Severity severity, std::string *out)
{
	switch(severity)
	{
		case Severity_A1: *out = "A1";
		break;
		case Severity_A2: *out = "A2";
		break;
		case Severity_A3: *out = "A3";
		break;
		case Severity_O1: *out = "O1";
		break;
		case Severity_O2: *out = "O2";
		break;
		case Severity_Event: *out = "EVENT";
		break;

		default:
		ACS_CS_TRACE((ACS_CS_EventReporter_TRACE,
			"(%t) %s\n"
			"Invalid Alarm severity specified %d\n", __FUNCTION__, severity));
		break;
	}
}

void ACS_CS_EventReporter::resetAllEvents()
{
	ACS_CS_TRACE((ACS_CS_EventReporter_TRACE,
		"(%t) ACS_CS_EventReporter::resetEvent()\n"
		"Resetting all events\n"));

	sentEvents.clear();
}

int ACS_CS_EventReporter::getProcIdByName(std::string procName)
{

	int pid = -1;

	// Open the /proc directory
	DIR *dp = opendir("/proc");
	if (dp != NULL)
	{
		// Enumerate all entries in directory until process found
		struct dirent *dirp;
		while (pid < 0 && (dirp = readdir(dp)))
		{
			// Skip non-numeric entries
			int id = atoi(dirp->d_name);
			if (id > 0)
			{
				// Read contents of virtual /proc/{pid}/cmdline file
				string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
				ifstream cmdFile(cmdPath.c_str());
				string cmdLine;
				getline(cmdFile, cmdLine);
				if (!cmdLine.empty())
				{
					// Keep first cmdline item which contains the program path
					size_t pos = cmdLine.find('\0');
					if (pos != string::npos)
						cmdLine = cmdLine.substr(0, pos);
					// Keep program name only, removing the path
					pos = cmdLine.rfind('/');
					if (pos != string::npos)
						cmdLine = cmdLine.substr(pos + 1);
					// Compare against requested process name
					 if (strcmp(procName.c_str(), cmdLine.c_str()) == 0)
						pid = id;
				}
			}
		}
	}

	closedir(dp);

	return pid;
}
