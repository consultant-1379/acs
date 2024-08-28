/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2012
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file acs_cs_opensession.cpp
 *
 * @brief   Defines the entry point for the console application.
 *
 *
 * @details
 *
 *
 * @author XNHAPHA
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2013-11-25  XNHAPHA  First updated
 * 2014-04-08  XNHAPHA  Fix the TRs: HS46371 and HS42399
 *
 ****************************************************************************/

#include "acs_cs_moddcommands.h"
#include <signal.h>

void sighandler(int signum);
int setupSignalHandler(const struct sigaction* sigAction);
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
   int exitCode = 0;
   struct sigaction sa;
   
   // Initialize Tracing
   CommandTrace::TraceObject =
            new ACS_TRA_trace(const_cast<char*>("OPENSESSION_TRACE"), const_cast<char*>("C1024"));
            
   TRACE("Opensession - main()");

   // set the signal handler for the main
   sa.sa_handler = sighandler;
   sa.sa_flags = SA_RESTART;
   sigemptyset(&sa.sa_mask );
   int res = setupSignalHandler(&sa);
   if (res !=0)
   {
      TRACE("setupSignalHandle fail");
      cout<<endl<<endl<<"Error when executing (general fault)"<<endl<<endl;
      return (res);
   }
      
   try
   {
      ACS_CS_MODDCommands opensession("opensession");
      
      exitCode = opensession.getExitCode();
      if (exitCode != NSCommand::Exit_Success)
      {
         cout<<endl<<endl<<opensession.getExitCodeDescription(exitCode)<<opensession.getExitCodeDescriptionArg()<<endl<<endl;
         return exitCode;
      }
      
      if (!opensession.checkOpenSessionFormat(argc, argv))
      {
         exitCode = opensession.getExitCode();
         cout<<endl<<endl<<opensession.getExitCodeDescription(exitCode)<<opensession.getExitCodeDescriptionArg()<<endl<<endl;
         return exitCode;
      }
      
      if (!opensession.handleOpenSession())
      {
         exitCode = opensession.getExitCode();
         cout<<endl<<endl<<opensession.getExitCodeDescription(exitCode)<<opensession.getExitCodeDescriptionArg()<<endl<<endl;
      }
   }
   catch(...)
   {
      exitCode = NSCommand::Exit_Error_Executing;
      cout<<endl<<endl<<"Error when executing (general fault)"<<endl<<endl;
   }
   
   return exitCode;
}
int setupSignalHandler(const struct sigaction* sigAction)
{
   if (sigaction(SIGINT, sigAction, NULL ) == -1)
   {
      return NSCommand::Exit_Error_Executing;
   }
   
   if (sigaction(SIGHUP, sigAction, NULL ) == -1)
   {
      return NSCommand::Exit_Error_Executing;
   }

   return 0;
}

void sighandler(int signum)
{
   // do nothing
}


