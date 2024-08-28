/*
 * ACS_PRC_prcmanSignalHandler.cpp
 *
 *  Created on: Nov 25, 2010
 *      Author: xpaomaz
 */

#include "ACS_PRC_SignalHandler.h"
#include "ACS_TRA_Logging.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>

const char pipeName[] = "/var/run/ap/PrcManServer";

ACS_PRC_SignalHandler::ACS_PRC_SignalHandler(bool & main_running):running(main_running), signal(0){
}

ACS_PRC_SignalHandler::~ACS_PRC_SignalHandler() {
}

int ACS_PRC_SignalHandler::handle_signal (int signum, siginfo_t *,ucontext_t*) {

	running = false;
	signal = signum;
	signal_string = strsignal(signum);

	return 0;
}
