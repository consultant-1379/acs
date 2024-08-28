//#include "StdAfx.h"
#include "acs_aca_command_invoker.h"
#include <ace/ACE.h>

static ACS_TRA_trace ACS_ACA_CMD_CI_DebugTrace("ACS_ACA_CMD_CI_DebugTrace", "C512");

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_CommandInvoker::ACS_ACA_CommandInvoker(ACS_ACA_Command *cmd)
{
	cmd_ = cmd;
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_CommandInvoker::~ACS_ACA_CommandInvoker(void)
{	
}

//------------------------------------------------------------------------------
//      Command request
//------------------------------------------------------------------------------
void ACS_ACA_CommandInvoker::msdls()
{
        if( ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering ACS_ACA_CommandInvoker::msdls()");
                ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }


        if( ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Calling ACS_ACA_Command::launch()");
                ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	cmd_->launch(); //launch the command
        if( ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_ACA_CommandInvoker::msdls()");
                ACS_ACA_CMD_CI_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}
