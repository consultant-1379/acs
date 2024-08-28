/*=================================================================== */
/**
   @file   acs_chb_poll.cpp

   @brief Header file for CHB module.

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
   N/A       21/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

//#include <winsock2.h>
#include <ace/OS_NS_sys_socket.h>
#include <ace/Event.h>
//#include <sys/socket.h>

#include "acs_chb_poll.h"

/*===================================================================
   ROUTINE: poll
=================================================================== */
int poll(struct pollfd *fds, unsigned long nfds, int timeout)
{
	
	// Init
	
	unsigned long i;
	int iPollResult;
	ACE_HANDLE iMaxFd=0;
	fd_set sRead;
	struct timeval sTime, *spTime;

	spTime = NULL;


	memset((char*) &sTime, 0, sizeof(sTime));

	if ( timeout != -1) {
		sTime.tv_sec = timeout/1000;
		sTime.tv_usec = timeout * 1000;
		spTime = &sTime;
	}

	FD_ZERO(&sRead);

	// fill the params for Select function
	
	for (i=0;i<nfds;i++) {
		if (fds[i].fd > iMaxFd )
			iMaxFd = fds[i].fd;
		if (((fds[i].fd) >= 0) && (((fds[i].events) & POLLIN) > 0))
			FD_SET(fds[i].fd, &sRead);
		// check if illegal events are requested
		if( fds[i].events & (~POLLIN)) return (-1);
	}

	iPollResult = select(iMaxFd+1, &sRead, 0, 0, spTime);


	// Fill the output struct with result

	for (i=0;i<nfds;i++) {
		if ( FD_ISSET(fds[i].fd, &sRead))
			fds[i].revents = POLLIN;
		else
			fds[i].revents = 0;
	}

	return iPollResult;
}
