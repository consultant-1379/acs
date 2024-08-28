/*
 * ACS_APGCC_CmdHandler.H
 *
 *  Created on: Apr 19, 2010
 *      Author: root
 */
#include "ACS_APGCC_Daemon.H"
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_DSD.H"
#include "ACS_PRC_Process.H"
#include "ace/Task.h"
#include <string>
using namespace std;

#ifndef ACS_APGCC_CMDHANDLER_H_
#define ACS_APGCC_CMDHANDLER_H_

class ACS_APGCC_CmdHandler: public ACE_Task<ACE_SYNCH>
{
public:

   enum {
      IDLE = 0,
      BUSY = 1
   };

   ACS_APGCC_CmdHandler(ACS_APGCC_Daemon* service);
   virtual ~ACS_APGCC_CmdHandler();

   virtual int open(void* arg=0);
   virtual int close();

   ACS_APGCC_DSD_Stream& peer();

   // thread methods
   virtual int svc();
   virtual int close(u_long);

   // user methods
   virtual int handle_input(ACE_HANDLE);
   virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask);
   virtual int handle_signal(int signo, siginfo_t* s=0, ucontext_t* u=0);

   bool uses_threads();
   // returns true if we're an active object

   // command reply method
   int sendCommandReply(ACS_APGCC_Daemon::CommandObj& obj, bool closeConnection=true);

protected:

   ACE_HANDLE hThread_;
   APGCC_DSD_Stream stream_;
   ACS_APGCC_Daemon* service_;
   ACS_APGCC_Daemon::CommandObj cmd;
   bool cmd_received_;
   bool shutdown_ordered_;
   bool using_threads_;
   bool thread_running_;
   bool delete_us_;
   char traStr[200];
};


#endif /* ACS_APGCC_CMDHANDLER_H_ */
