/*=================================================================== */
/**
   @file   acs_alog_main.h

   @brief Header file for acs_alog_main type module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_alog_main_H_
#define acs_alog_main_H_


/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
//Include file for external library.
#include <iostream>
#include <fstream>
#include <string.h>
#include <pthread.h>
#include <algorithm>
#include <vector>
#include <libaudit.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/epoll.h>

#include <ACS_TRA_trace.h>

#include <acs_alog_activeWorks.h>
#include <acs_alog_ha_AppManager.h>


#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_OiHandler.h>
#include <acs_apgcc_paramhandling.h>
#include <acs_prc_api.h>
#include <acs_alog_Imm_Implementer.h>

using namespace std;

int parse_command_line(int argc, char **argv);
void child_handler(int signum);
void parent_handler(int signum);
ACE_THR_FUNC_RETURN run_alog_daemon_HA( void* );
ACE_THR_FUNC_RETURN run_alog_daemon_NoHA( void* );
void stop_alog_daemon_HA();
void stop_alog_daemon_NoHA();
void clean();
ACS_ALOG_ReturnType removeImplementer();
ACS_ALOG_ReturnType registerImplementers();

ACS_ALOG_ReturnType ImplementerAuditLogging();
ACS_ALOG_ReturnType ImplementerCommandLog();
ACS_ALOG_ReturnType ImplementerExclusionItem();
ACS_ALOG_ReturnType setInitialNodeState();
void nodeStateActions(bool init);

class MyThread_2 : public ACE_Task_Base {
public:

                MyThread_2(acs_alog_Imm_Implementer *p_user1)
		{
                               implementer = p_user1;
                               isStop = false;
                }

                ~MyThread_2(){}

                void stop()
                {
                               isStop = true;
                }

                void start()
		{
                	isStop = false;
                }


                int  svc (void)
                {

					    epoll_event  event_to_check_in, event_to_check_out;
					    char  strError[1024];
						int   ret, ep_fd = ::epoll_create1 (EPOLL_CLOEXEC);

					    event_to_check_in.events = EPOLLIN;

					    ::epoll_ctl (ep_fd, EPOLL_CTL_ADD, implementer->getSelObj(), &event_to_check_in );

						while (! isStop)
						{
					       	ret = ::epoll_wait (ep_fd, &event_to_check_out, 1, 400);

							if (ret == 0)
							{
							}
							else if (ret == -1)
							{
									sprintf (strError,"Main ::epoll_wait return %i, errno = %i", ret, errno);
									ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(strError));
									if (! isStop)
									{
											::epoll_ctl (ep_fd, EPOLL_CTL_ADD, implementer->getSelObj(), &event_to_check_in );
									}
							}
							else {
									if (! isStop  &&  event_to_check_out.events == EPOLLIN)
									{
											implementer->dispatch (ACS_APGCC_DISPATCH_ALL);
									}
							}
							usleep (20000);
						}
						return 0;
                }

private:
                acs_alog_Imm_Implementer *implementer;
                bool isStop;
};

#endif /* acs_alog_main_H_ */
