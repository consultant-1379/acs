// MCLI.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <iostream>
//#include <tchar.h>
//#include <crtdbg.h>
//#include <acs_prc_process.h>
//#include <acs_exceptionhandler.H>
#include <acs_aca_msd_cmd_stub.h>
#include "acs_aca_msd_cmd_msdls.h"
#include "acs_aca_command_invoker.h"
#include "acs_aca_msdls_api_struct.h"
#include <ace/ACE.h>

static ACS_TRA_trace ACS_ACA_CMD_MCLI_DebugTrace("ACS_ACA_CMD_MCLI_DebugTrace", "C512");

int main(ACE_INT32 argc, ACE_TCHAR* argv[])
{
#if 0
	//Priority of the process 
	AP_InitProcess("msdls",AP_COMMAND);
	//ACS Excepion Handling 
	AP_SetCleanupAndCrashRoutine("msdls",NULL);
#endif

        if( ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering acs_aca_mcli main()");
                ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	ACS_ACA_MSDCMD_Stub stub;					//create Receiver
	ACS_ACA_MSDCMD_msdls ls(argc, argv, &stub); //create Concrete Command
	ACS_ACA_CommandInvoker invoker(&ls);		//create Invoker
	if( ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "To invoke command  calling ACS_ACA_CommandInvoker::msdls()");
                ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	invoker.msdls(); //invoke command
        if( ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving acs_aca_mcli main()");
                ACS_ACA_CMD_MCLI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	return 0;
}
