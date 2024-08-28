/*
 * .NAME: ACS_TRA_util.h
 *
 * .LIBRARY 3C++
 * .PAGENAME
 * .HEADER
 * .LEFT_FOOTER Ericsson Telecom AB
 * .INCLUDE
 *
 * .COPYRIGHT
 *  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 *  All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Utvecklings AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Utvecklings AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This is the implementation of the AP trace utility call.
 *
 * .ERROR HANDLING
 *
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 2010-05-06 XYV XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	        20100506	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 * PA2 	        20101112	XGIOPAP		Implementation of comment
 */

#ifndef ACS_TRA_UTIL_H_
#define ACS_TRA_UTIL_H_

// Module Include Files
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Thread.h>

#define TRAUTIL_LOCKFILE_PATH_PREFIX  "/var/run/ap/trautil_"

/*
 * Name: ACS_TRA_util
 * Description: This class manage the trautil console
 *              and dialogue commands
 */
class ACS_TRA_util
{
public:

	ACS_TRA_util();
	static ACE_INT32 oktracedatap (relpointer p);
	static ACE_INT32 oktaskdatap(relpointer p);
	static ACE_INT32 oktracep (relpointer p);
	static void list_set(trace_setp setp, ACE_INT32 instance);
	static void list_tasks(ACE_INT32 all);
	static void list_free_buffers();
	static void list_free_task_data();
	static void list_trace_elements();
	static void list_hashtable();
	static void list_pid_table();
	static int suspend_thread();
	static int activate_thread(char delimiter);
	static void check_buffer_full(char delimiter);
	static void flush(void* arguments);

	// check if a given thread (belonging to this process or another process) is alive
	static int is_thread_alive(pid_t os_tid);
};

#endif /* ACS_TRA_UTIL_H_ */
