/*
 * ACS_PRC_RunTimeOwnerThread.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: xlucpet
 */

#include "ACS_PRC_RunTimeOwnerThread.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>

using namespace std;

ACS_PRC_RunTimeOwnerThread::ACS_PRC_RunTimeOwnerThread():sleep(false),implementer(0) {
}

ACS_PRC_RunTimeOwnerThread::~ACS_PRC_RunTimeOwnerThread() {
}

int ACS_PRC_RunTimeOwnerThread:: svc ( void ){

    int ret;

    struct pollfd fds[1];

    fds[0].fd = implementer->getSelObj();
    fds[0].events = POLLIN;

    const char pipeName[] = "/var/run/ap/PrcManServer";

    while( !sleep )
    {
    	ret = poll(fds, 1, 400);

		if (ret == -1) // poll error
				printf("poll error: \n");
		else {
			if ( ret == 0 ){ // timeout
				if ( sleep ) { // the thread has to be stopped
					return 0;
				}
			}
			else {
				if ( implementer->dispatch(ACS_APGCC_DISPATCH_ALL) == ACS_CC_FAILURE ){
					ACE_LSOCK_Stream Stream;
					ACE_LSOCK_Connector Connector;
					ACE_UNIX_Addr addr (pipeName);

					Connector.connect ( Stream, addr );

					Stream.send_n ( "SHUTDOWN", 9 );

					Stream.close();

					sleep = true;
				}
			}
		}
    }

    return 0;
}
