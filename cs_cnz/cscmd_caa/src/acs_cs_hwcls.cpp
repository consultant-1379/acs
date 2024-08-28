// acs_modd_hwcls.cpp : Defines the entry point for the console application.
//
#include "acs_cs_moddcommands.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	int exitCode = 0;
	try
	{
		ACS_CS_MODDCommands hwcLs("HWCLS");

		if (! hwcLs.handleHWCLS(argc, argv))
		{
			exitCode = hwcLs.getExitCode();
			if (exitCode == NSCommand::Exit_Illegal_Command)	// Fix for TR HW39060
				cout<<hwcLs.getExitCodeDescription(exitCode)<<hwcLs.getExitCodeDescriptionArg()<<endl;
			else
				cout<<endl<<endl<<hwcLs.getExitCodeDescription(exitCode)<<hwcLs.getExitCodeDescriptionArg()<<endl<<endl;
		}
	}

	catch(...)
	{
		exitCode = NSCommand::Exit_Error_Executing;
		cout<<endl<<endl<<"Error when executing (general fault)"<<endl<<endl;
	}

	return exitCode;
}
