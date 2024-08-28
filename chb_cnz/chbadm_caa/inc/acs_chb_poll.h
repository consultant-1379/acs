/*=================================================================== */
/**
   @file   acs_chb_poll.h

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

#define POLLIN 0x0001 // fd is readable
#include <ace/ACE.h>
#include <sys/poll.h>

/*=================================================================== */
	/**
	      @brief     Implementation of Poll function. Ported from UNIX.	Only POLLIN is implemented.

		  @param     fds : file descriptors to check

		  @param	 nfds : Number of pollfd structures in the fds array.

		  @param 	 timeout : timeout [ms]

		  @return    returns the total number of socket handles that are ready,zero
				     if the time limit expired, or SOCKET_ERROR if an error occurred.
				     If the return value is SOCKET_ERROR, WSAGetLastError can be used
				     to retrieve a specific error code.
	*/
/*=================================================================== */
int poll(struct pollfd *fds, unsigned long nfds, int timeout);


