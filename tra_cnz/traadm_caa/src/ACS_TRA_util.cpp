/*
 * NAME: ACS_TRA_util.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
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
 * 	This file contains the main program to manage the trautil console.
 *  From this function is possible to fetch the trautil command, the
 *  dialogue commands (C create task, D delete task, F flush task,
 *  L list tasks, Q quit, X check and diasplay contents of internal
 *  data structures), with all parameters.
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0023
 *
 * AUTHOR
 * 	2010-05-10 XYV XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1	        20100510	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 * PA2 	        20101112	XGIOPAP		Implementation of comment
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <sys/file.h>

#include "ace/Guard_T.h"

// Module Include Files
#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"
#include "ACS_TRA_util.h"
#include "ACS_TRA_AutoflushThread.h"
#include "ACS_TRA_AutoflushThreadManager.h"

extern ACS_TRA_AutoflushThreadManager autoflush_thread_manager;

namespace {
	const char * strip_blank (char * str) {
		char * s = str;
		while (::isblank(*s)) ++s;

		if (*s == 0) return s;

		char * t = s + ::strlen(s) - 1;
		while ((t > s) && ::isblank(*t)) --t;
		*++t = '\0';

		return s;
	}

	/*
	 *  Check if there are other running trautil instances having the given USER:
	 *  Returns :
	 *  0  -->  there are no other running trautil instances having the same USER
	 *  1  -->  there are other running trautil instances having the same USER
	 *  < 0 --> an error occurred
	 */
	int check_for_multiple_instance_running(char *username)
	{
		if(!username || !*username)
			return -1;

		char lock_file_pathname[PATH_MAX]={0};
		int retval = snprintf(lock_file_pathname, PATH_MAX, "%s%s",TRAUTIL_LOCKFILE_PATH_PREFIX, username);
		if(retval >= PATH_MAX)
			return -2;

		int fd = ::open(lock_file_pathname, O_CREAT | O_WRONLY | O_APPEND, 0664);
		if(fd < 0)
			return -3;

		if(flock(fd, LOCK_EX | LOCK_NB) < 0)
			return (errno == EWOULDBLOCK) ? 1 /*another trautil instance running with the given user*/ : -4;

		return 0; // no other trautil instances with the given user
	}
}

// extern declaration
ACE_INT32 syntax_error;


//static ACS_TRA_lib ACS_TRA_lib;
/*
 * Class Name: ACS_TRA_util
 * Method Name: oktracedatap
 * Description: Check a trace data offset.
 * Used: This method is used in the following member:
 *       ACS_TRA_util::list_trace_elements, ACS_TRA_util::list_hashtable
 *       and ACS_TRA_util::list_pid_table in ACS_TRA_util.cpp file
 */
ACE_INT32 ACS_TRA_util::oktracedatap (relpointer p)
{
	if ((p < (ACE_INT64)sizeof(struct trace_mem)) || (p > shmp->free_trace_data) || ((p%4) != 0))
	{
		ACE_OS::fprintf (stdout, "\n*****************CORRUPTED TRACE DATA OFFSET 0x%x ****************\n", (unsigned int)p);
		return (0);
	}
	else
		return (1);
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: oktracep
 * Description: Check a trace or task data offset.
 * Used: This method is used in the following member:
 *       ACS_TRA_util::list_set and ACS_TRA_util::list_tasks
 *       in ACS_TRA_util.cpp file
 */
ACE_INT32 ACS_TRA_util::oktracep (relpointer p)
{
	if ((p < (ACE_INT64)sizeof(struct trace_mem)) || (p > TOP_LEVEL_SHM_SIZE ) || ((p%4) != 0))
	{
		ACE_OS::fprintf (stdout, "\n*****************CORRUPTED TRACE SET DATA OFFSET 0x%x ****************\n", (unsigned int)p);
		return (0);
	}
	else
		return (1);
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_set
 * Description: Print the trace set related to the trace task.
 * Used: This method is used in the following member:
 *       ACS_TRA_util::list_set and ACS_TRA_util::list_tasks
 *       in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_set(trace_setp setp, ACE_INT32 instance)
{ 
	ACE_INT32 	complex;
	trace_setp 	right;
	char * 		datap;

	datap = (char *)setp + sizeof(struct trace_data);

	switch (setp->typ)
	{
		// NT port PROC ->  PRCS
		case PROC:
			if (instance == 0)
				ACE_OS::fprintf(stdout, " P %s", datap);
			else
				ACE_OS::fprintf(stdout, " I %d", instance);
			break;
		case CLASS:
			ACE_OS::fprintf(stdout, " C %s", datap);
			break;
		case REC:
			ACE_OS::fprintf(stdout, " R %s", datap);
			break;
		case PLUS:
			if ((oktracep(setp->left)) && (oktracep(setp->right)))
			{
				list_set(ABS(trace_setp, setp->left), instance);
				ACE_OS::fprintf(stdout, " +  ");
				right = ABS( trace_setp, setp->right);
				complex = (right->typ == PLUS) || (right->typ == MINUS) || (right->typ == INTERSECTION);

				if (complex)
					ACE_OS::fprintf(stdout, " ( ");

				list_set(ABS(trace_setp, setp->right), instance);

				if (complex )
					ACE_OS::fprintf(stdout, " ) ");
			}
			break;
		case MINUS:
			if ((oktracep(setp->left)) && (oktracep(setp->right)))
			{
				list_set(ABS(trace_setp, setp->left), instance);
				ACE_OS::fprintf(stdout, " -  ");
				right = ABS( trace_setp, setp->right);
				complex = (right->typ == PLUS) || (right->typ == MINUS) || (right->typ == INTERSECTION);

				if (complex)
					ACE_OS::fprintf(stdout, " ( ");

				list_set(ABS(trace_setp, setp->right), instance);

				if (complex)
					ACE_OS::fprintf(stdout, " ) ");
			}
			break;
		case INTERSECTION:
			if ((oktracep(setp->left)) && (oktracep(setp->right)))
			{
				list_set(ABS(trace_setp, setp->left), instance);
				ACE_OS::fprintf(stdout, " * ");
				right = ABS( trace_setp, setp->right);
				complex = (right->typ == PLUS) || (right->typ == MINUS) || (right->typ == INTERSECTION);

				if (complex)
					ACE_OS::fprintf(stdout, " ( ");

				list_set(ABS(trace_setp, setp->right), instance);

				if (complex)
					ACE_OS::fprintf(stdout, " ) ");
			}
			break;
		default:
			;
	}
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_tasks
 * Description: Print all trace tasks on stdout.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_tasks(ACE_INT32 all)
{

	ACE_INT32       i;
	ACE_INT32       instance = 0;
	trace_taskp     taskp;
	trace_setp      sp;
	ACE_INT32		listed = 0;
	ACE_INT32		n_other_user_tasks = 0;
//	ACE_INT32 		bufcount;
//	relpointer 		buf;
//	relpointer * 	bufp;

	for (i=0; i < MAX_TASK_COUNT; i++)
	{
			if (shmp->tasksTab[i].refcount == -1)  continue;

			taskp = &shmp->tasksTab[i];

			if ((ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER"))) == 0  ||  all)
			{
					listed = 1;
					ACE_OS::fprintf(stdout, " TASK = %s", taskp->name);
					ACE_OS::fprintf(stdout, ", USER = %s", taskp->user);

					logp  lp = &taskp->logBuff;

					if (lp->typ == PID)  instance = lp->pid;

					ACE_OS::fprintf(stdout, ", TRACE SET =");

					if (oktracep(taskp->set))
					{
							sp = ABS(trace_setp, taskp->set);
							list_set(sp, instance);
					}

					ACE_OS::fprintf(stdout, ", LOG = %d", lp->nobuf);

					if (taskp->autoflush > 0)
							ACE_OS::fprintf(stdout, ", AUTOFLUSH = %d", taskp->autoflush);
					else
							ACE_OS::fprintf(stdout, ", AUTOFLUSH = none");

					if (taskp->video == 1)
							ACE_OS::fprintf(stdout, ", SCREEN\n");

					if (taskp->filename[0] != 0)
					{
							ACE_OS::fprintf(stdout, ", FILENAME = %s", taskp->filename);
							ACE_OS::fprintf(stdout, ", SIZE = %iMb\n", taskp->size);
					}
					else
							if (taskp->video == 0)
									ACE_OS::fprintf(stdout, ", FILENAME = none\n");
/**
						if (all)
						{
							ACE_OS::fprintf (stdout, " STARTBUF = 0x%x, STARTPOS = 0x%x, CURRBUF = 0x%x, CURRPOS = 0x%x\n", lp->startbuf, lp->startpos, lp->currbuf, lp->currpos);
							ACE_OS::fprintf (stdout, " LOG BUFFERS CHAIN : \n" );
							buf = lp->startbuf;
							bufcount = 0;
							if (lp->startbuf != 0)
								do
								{
									//  code folded from here
									ACE_OS::fprintf (stdout,"0x%x  ", buf);
									bufcount++;
									if ((bufcount%6) == 0)
									{
										ACE_OS::fprintf (stdout, "\n");
									}

									bufp = ABS( relpointer *, buf);
									buf = *bufp;

									if (!oktaskdatap(buf))
										break;

									// unfolding
								}

							while ((buf != lp->startbuf) && (bufcount <= lp->nobuf));
							ACE_OS::fprintf (stdout, "0x%x \n\n", buf);
						}
					}
**/
			}
			else
				++ n_other_user_tasks;
	}

	if (listed == 0) 
	{
		if (n_other_user_tasks > 0)
				ACE_OS::fprintf(stdout, "trace tasks owned by other users\n");
		else
				ACE_OS::fprintf(stdout, "No trace tasks\n");
	}
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_free_buffers
 * Description: List the free buffer chain.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_free_buffers()
{
/*
	ACE_INT32	 bufcount;
	relpointer 	 buf;
	relpointer 	 * bufp;
	
	ACE_OS::fprintf (stdout, "FREE LOG BUFFERS CHAIN: \n");
	buf = shmp->free_buff_head;
	bufcount = 0;

	while ((buf != 0) && (oktaskdatap(buf)))
	{
		ACE_OS::fprintf (stdout, "0x%x  ", buf);
		bufcount++;

		if (bufcount > 6)
		{
			ACE_OS::fprintf(stdout, "\n");
			bufcount = 0;
		}

		bufp = ABS(relpointer *, buf);
		buf = *bufp;
	}
	
	ACE_OS::fprintf (stdout, "\n"); */
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_free_task_data
 * Description: List free task data.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_free_task_data()
{
	ACE_INT32	count;
	relpointer 	temp;
	relpointer 	*p;

	ACE_OS::fprintf (stdout, "FREE TASK DATA CHAIN: \n");
	
	temp = shmp->free_temp_head;
	count = 0;
	
	while (temp != 0)
	{
		ACE_OS::fprintf (stdout, "0x%x  ", (unsigned int)temp);
		count++;

		if (count > 6)
		{
			ACE_OS::fprintf(stdout, "\n");
			count = 0;
		}

		p = ABS(relpointer *, temp);
		temp = *p;
	}
	
	ACE_OS::fprintf (stdout, "\n\n");
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_trace_elements
 * Description: List all trace elements.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_trace_elements()
{
	char * 		    	tmp;
	trace_elementp  	trcep;
	relpointer 			trcoffs;
	ACE_INT32  			trccount;
	
	trcoffs = shmp->trce_head;
	
	trccount = 0;
	trcep = ABS (trace_elementp, shmp->trce_head);

	while ((trcep != (trace_elementp)shmp) && (oktracedatap(trcoffs)))
	{
		trccount ++;
		ACE_OS::fprintf (stdout, "\n\n");
		ACE_OS::fprintf (stdout, "OFFSET = 0x%x, MASK = 0x%x \n", (unsigned int)trcoffs, trcep->mask);
		/* procname */
		ACE_OS::fprintf (stdout,  "0x%x    PROCNAME", (unsigned int)trcep->proc);

		if (trcep->proc == 0)
			ACE_OS::fprintf (stdout, " NONAME\n");
		else 
			if (oktracedatap(trcep->proc))
			{
				tmp = ABS(char *, trcep->proc) + sizeof(struct trace_data);
				ACE_OS::fprintf (stdout, "  %s\n", tmp);
			}

			/* trace point name */
			ACE_OS::fprintf (stdout, "0x%x     NAME", (unsigned int)trcep->record);

			if (oktracedatap(trcep->record))
			{
				tmp = ABS(char *, trcep->record) + sizeof(struct trace_data);
				ACE_OS::fprintf (stdout, "   %s\n", tmp);
			}

			/* format */
			ACE_OS::fprintf (stdout, "0x%x    FORMAT", (unsigned int)trcep->format);

			if (oktracedatap(trcep->format))
			{
				tmp = ABS(char *, trcep->format) + sizeof(struct trace_data);
				ACE_OS::fprintf (stdout, "   %s\n", tmp);
			}

			trcoffs = trcep->next;
			trcep = ABS(trace_elementp,trcep->next);
	}

	ACE_OS::fprintf (stdout, "\n NUMBER OF TRACE ELEMENTS= %d \n\n", trccount );
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_hashtable
 * Description: List the entries in the hash chains.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_hashtable()
{
	ACE_INT32 		i;
	ACE_INT64 		count;
	relpointer 		data;
	trace_datap 	datap;

	ACE_OS::fprintf (stdout, "\nCHAINS OF TRACE DATA :\n\n");

	for (i = 0; i < HASHSIZE; i++)
	{
		count = 0;
		data = shmp->hashtab[i];

		while ((data != 0) && (oktracedatap(data)))
		{
			datap = ABS(trace_datap, data);
			if ((datap->typ < 0) || (datap->typ > CLASS))
				ACE_OS::fprintf(stdout,"\n***************CORRUPTED  TYPE DATA %d **********\n",datap->typ);
			
			ACE_OS::fprintf(stdout, " OFFSET = 0x%x, TYP=%d \n", (unsigned int)data, datap->typ);
			count++;
			data = datap->next;
		}
		
		ACE_OS::fprintf (stdout, " NUMBER OF ENTRIES IN HASH BUCKET %d= %d\n \n",i, (int)count);
	}
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: list_pid_table
 * Description: List the pid tables.
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::list_pid_table()
{  

	char   *datap;
	bool  	header = false;

	for (int j=0; j<MAX_PIDS; j++)
	{
			if (shmp->pidTab[j].pid != 0)
			{
					if (header == false)
					{
							ACE_OS::fprintf (stdout, " ACTIVE PROCESS ENTRIES\n");
							header = true;
					}
					ACE_OS::fprintf (stdout, "PID = %d , PROCNAME =", shmp->pidTab[j].pid);

					if (shmp->pidTab[j].proc == 0)  ACE_OS::fprintf(stdout, "NONAME\n");
					else
					{
							datap = ABS(char *, shmp->pidTab[j].proc) + sizeof(struct trace_data);
							ACE_OS::fprintf (stdout, "%s\n", datap);
					}
			}
	}
/*********
	tmp = shmp->free_pid_head;

	if (tmp != 0)
		ACE_OS::fprintf (stdout, " OLD ENTRIES\n");

	while ((tmp != 0) && oktracedatap(tmp))
	{
		p = ABS(struct pidrec *, tmp);
		ACE_OS::fprintf (stdout, "PID = %d , PROCNAME =", p->pid);

		if (p->proc == 0)
			ACE_OS::fprintf(stdout, "NONAME\n");
		else
			if (oktracedatap(p->proc))
			{
				datap = ABS(char *, p->proc) +sizeof(struct trace_data);
				ACE_OS::fprintf (stdout, "%s\n", datap);
			}
		
			tmp = p->next;
	}
***************/
}

/*
 * Name: main
 * Description: This is the main program.
 */
int main(int argc, char *argv[])
{
	trace_taskp     taskp;
	char            *command, *taskname;
	char            tname[MAX_TASK_NAME];
	ACE_INT32		l = 0;
	char * 			timep;
	char 			MemorySize[20];
	char			On_Off[10];
	char 			delimiter = ',';
	ACE_INT32		i, Memtemp;
	char			*filter;

	// enable command history
	using_history();

	/*
	 * Reset the MemorySize, the On_Off 
     * and tname so do not have wrong characters
	 */
	ACE_OS::memset(MemorySize, 0, 20);
	ACE_OS::memset(On_Off, 0, 10);
	ACE_OS::memset(tname, 0, MAX_TASK_NAME);
	taskname = const_cast<char *>("");

	/*
	 * Command description: Fetch the parameter of tautil command
	 * Syntax: trautil [-l] [-dc], trautil [-m memory_size],
	 * 		   trautil [-o ON_or_OFF]
	 */
	for (i = 1; i < argc; i++)
	{
		if (ACE_OS::strncmp (argv[i], "-l", 2) == 0)	// Fetch parameter -l of the trautil command
		{
			// Check if there are other option
			if (argv[2])
			{
				ACE_OS::fprintf(stdout, UsageTrautil);
				exit (2);
			}

			if(strlen(argv[i]) > 2)
			{
				ACE_OS::fprintf(stdout, UsageOptionL);
				exit (2);
			}

			l = 1;
		}
		else if (ACE_OS::strncmp (argv[i], "-d", 2) == 0)	// Fetch parameter -d of the trautil command
		{
			// Check if there are other option
			if (argv[2])
			{
				ACE_OS::fprintf(stdout, UsageTrautil);
				exit (2);
			}

			if(strlen(argv[i]) > 3)
			{
				ACE_OS::fprintf(stdout, UsageOptionD);
				exit (2);
			}

			delimiter = argv[i][2];
		}
		else if (ACE_OS::strcmp (argv[i], "-m") == 0)	// Fetch parameter -m of the trautil command
		{
			// Fetch the value of memory size
			if (argv[2])
			{
				if (ACS_TRA_common::isDigit(argv[2]) == 1)
				{
					ACE_OS::fprintf(stdout,"Illegal size: %s\n", argv[2]);
					exit (2);
				}

				if (argv[3])
				{
					// Check if there are other option
					if (ACE_OS::strcmp(argv[3], "-l") == 0 || ACE_OS::strcmp(argv[3], "-d") == 0 || ACE_OS::strcmp(argv[3], "-o") == 0 || ACE_OS::strcmp(argv[3], "-m") == 0)
					{
						ACE_OS::fprintf(stdout, UsageTrautil);
						exit (2);
					}
					else
					{
						ACE_OS::fprintf(stdout, UsageOptionM);
						exit (2);
					}
				}

				Memtemp = ACE_OS::atoi(argv[2]);

				if (Memtemp <= 0 || Memtemp > 10000)
				{
					ACE_OS::fprintf(stdout,"Illegal size: %s\n", argv[2]);
					exit (2);
				}
				else
				{
					ACE_OS::strcpy (MemorySize, argv[2]);

					if (ACS_TRA_common::writeMemorySize(MemorySize) == 0)
						exit (0);
					else
						exit (1);
				}
			}
			else
			{
				ACE_OS::fprintf(stdout,"Memory size missing\n");
				exit (2);
			}
		}
		else
		{
			// Fetch parameter -o of the trautil command
			if (ACE_OS::strcmp (argv[i], "-o") == 0)
			{
				// Fetch the value of trace on_off
				if (argv[2])
				{
					if (argv[3])
					{
						// Check if there are other option
						if (ACE_OS::strcmp(argv[3], "-l") == 0 || ACE_OS::strcmp(argv[3], "-d") == 0 || ACE_OS::strcmp(argv[3], "-o") == 0 || ACE_OS::strcmp(argv[3], "-m") == 0)
						{
							ACE_OS::fprintf(stdout, UsageTrautil);
							exit (2);
						}
						else
						{
							ACE_OS::fprintf(stdout, UsageOptionO);
							exit (2);
						}
					}

					ACE_OS::strcpy(On_Off, argv[2]);

					if (ACE_OS::strcmp(On_Off, TRACESTATUSON) == 0)
						if (ACS_TRA_common::writeTraceStatus(1) == 0)
							exit(0);
						else
							exit(1);
					else
					{
						if (ACE_OS::strcmp(On_Off, TRACESTATUSOFF) == 0)
							if (ACS_TRA_common::writeTraceStatus(0) == 0)
								exit(0);
							else
								exit(1);
						else
						{
							ACE_OS::fprintf(stdout,"Illegal value %s, use:\n", argv[2]);
							ACE_OS::fprintf(stdout,"%s -o ON to turn trace on.\n", argv[0]);
							ACE_OS::fprintf(stdout,"%s -o OFF to turn trace off.\n", argv[0]);
							exit (2);
						}
					}
				}
				else
				{
					ACE_OS::fprintf(stdout,"ON or OFF parameter missing use:\n");
					ACE_OS::fprintf(stdout,"%s -o ON to turn trace on\n", argv[0]);
					ACE_OS::fprintf(stdout,"%s -o OFF to turn trace off\n", argv[0]);
					exit (2);
				}
			}
			else
			{
				ACE_OS::fprintf(stdout, UsageTrautil);
				exit (2);
			}
		}
	}

	/*
	 * Attach the shared memory to the global pointer shmp.
	 * If first time, gets and initializes the trace shared memory
	 */
	if (ACS_TRA_lib::get_shared_memory() != 0)
	{
		ACE_OS::fprintf(stdout, "Probable cause: Shared memory not installed.\n");
		ACE_OS::fprintf(stdout, "Use %s -o %s to turn trace on.\n", argv[0], TRACESTATUSON);
		ACE_OS::fprintf(stdout, "Note the applications that shall be trace need to be (re)started.\n");
		exit (1);
	}

	// If the allocate of the shared memory fails, release and exit to trautil console
	if (shmp->lev == 0)
	{
		ACE_OS::fprintf(stdout, "Incompatible version of trace shared memory\n");
		ACE_OS::fprintf(stdout, "Failure to allocate the shared memory\n");
		exit (1);
	}

	// print the trautil console information
	ACE_OS::fprintf(stdout, "trace_utility level %d version %s 2010\n", shmp->lev, "F");
	ACE_OS::fprintf(stdout, "Copyright Ericsson Utvecklings AB, 2010\n");
	ACE_OS::fflush(stdout);

	/*
	 * Check all tasks belonging to the current user having autoflush feature activated on FILE;
	 * For each of such tasks, activate an autoflush thread if there is no other autoflush thread (belonging to another trautil instance)
	 * that is already flushing data on file for such task
	 */
	ACS_TRA_util::activate_thread(delimiter);

	/*
	 * Check if the current user have some traces,
	 * if yes start a thread to check when the
	 * buffer is full and flush automatically it.
	 */
	// ACS_TRA_util::check_buffer_full(delimiter);

	// Reset the task pointer for next use
	taskp = NULL;

	char * input_line = 0; // To keep user input by means of GNU readline library interface

	// Fetch the stream line of command if it is no null
	while (1) {
		::free(input_line);
		if (!(input_line = ::readline("trc> "))) break;

		const char * input_line_stripped = strip_blank(input_line);

		if (*input_line_stripped) ::add_history(input_line_stripped);

		// Copy input line into the command parsing buffer
		::strncpy(ACS_TRA_lib::get_cmd_chars(), input_line_stripped, MAX_CHARS_COM);
		(ACS_TRA_lib::get_cmd_chars())[MAX_CHARS_COM - 1] = '\0';

		if (l)
			ACE_OS::fputs(ACS_TRA_lib::get_cmd_chars(), stdout);

		command = ACS_TRA_common::get_token();

		if (command != NULL)
		{
			if (ACE_OS::strcmp (command, "quit") == 0)
				exit(0);
			
			switch (command[0])
			{
				case 'C':
					/*
					 * Command description: Create a trace task
					 * Syntax: C task_name trace_set LOG number_of_buffers [log_filename] [AUTOFLUSH seconds]
					 */

					if (ACE_OS::strlen(command) > 1)
						ACE_OS::fprintf (stdout, StringUsage);
					else
					{
						// get exclusive access to top shared memory
						ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
						if(!top_sm_mutex_guard.locked())
						{
							ACE_OS::fprintf(stdout, "Error creating TASK: unable to acquire shared memory mutex\n");
							break;
						}

						/*
						 * If the trace tasks have reached the maximum number
						 * (MAX_TASK_COUNT), the command is rejected and the
						 * error message is printed. Then return to trautil console
						 */
						if (shmp->taskcount == MAX_TASK_COUNT)
						{
							ACE_OS::fprintf(stdout, "too many active tasks\n");
							break;
						}

						/*
						 * Fetch the first parameter (task name) and check
						 * if it is specified in the command
						 */
						taskname = ACS_TRA_common::get_token();
						if (taskname == NULL)
							ACE_OS::fprintf (stdout, StringUsage);
						else
						{
							/*
							 * Check if name that you typed already exists
							 * in the shared memory. If already exist print an
							 * error message and return to trautil console
							 */
							taskp = ACS_TRA_lib::get_task(taskname);
							if (taskp != NULL)
							{
								ACE_OS::strncpy(tname, taskname, ACE_OS::strlen(taskname));
								ACE_OS::fprintf(stdout, "task %s already exists\n", tname);
								ACE_OS::memset(tname, 0, MAX_TASK_NAME);
							}
							else
							{
								/*
								 * Check if name that you typed does not exceed
								 * the max number of chars (MAX_TASK_NAME) for
								 * task name parameter. If it is grater than of
								 * MAX_TASK_NAME then an error message is printed
								 * and return to trautil console
								 */
								if (ACE_OS::strlen(taskname) > MAX_TASK_NAME)
								{
									ACE_OS::fprintf(stdout, "Failure: task name exceeds maximum length\n");
									break;
								}

								/*
								 * Check if there is free space in the shared memory,
								 * if there isn't print an error message and return to
								 * trautil console. Otherwise Create the trace task
								 */
								if (ACS_TRA_lib::create_task(taskname, delimiter) == 0)
									if (syntax_error == 0)
										ACE_OS::fprintf(stdout, "no trace memory left, delete tasks\n");
							}
						}
					}
					break;

				case 'F':
					/*
					 * Command description: Flush the task name
					 * Syntax: F task_name
					 */

					if (ACE_OS::strlen(command) > 1)
							ACE_OS::fprintf (stdout, "Usage: F taskname\n");
					else
					{
						// get exclusive access to top shared memory
						ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
						if(!top_sm_mutex_guard.locked())
						{
							ACE_OS::fprintf(stdout, "Error flushing TASK: unable to acquire shared memory mutex\n");
							break;
						}

						taskname = ACS_TRA_common::get_token();

						/*
						 * If task name is not specified in command line then
						 * print error message and return to trautil console
						 */
						if (taskname == NULL)
							ACE_OS::fprintf(stdout, "Usage: F taskname\n");
						else
						{
							// check if there are other parameters
							if (ACS_TRA_common::get_token() != NULL)
							{
								ACE_OS::fprintf(stdout, "Usage: F taskname\n");
								break;
							}

							/*
							 * The task name is specified in command line, check
							 * if it is greater than of max number chars allowed
							 */
							if (ACE_OS::strlen(taskname) > MAX_TASK_NAME)
							{
								ACE_OS::fprintf(stdout, "Failure: task name exceeds maximum length\n");
							}
							else
							{
								/*
								 * The task name is specified in command line, and
								 * if it is less than of max number chars allowed
								 */
								taskp = ACS_TRA_lib::get_task(taskname);

								/*
								 * Check if the specified task name is found in
								 * trace shared memory, if there isn't print an
								 * error message and return to trautil console.
								 */
								if (taskp == NULL)
									ACE_OS::fprintf(stdout, "task %s not found\n", taskname);
								else
								{
									/*
									 * If the specified task name is found in trace shared memory,
									 * retrieve the user that use the console trautil and check
									 * if he has permission to flush this trace log.
									 */
									if ((ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER"))) != 0)
											ACE_OS::fprintf(stdout, "Not allowed to flush other's trace log\n");
									else
									{
											if (taskp->logBuff.typ == PID  ||  taskp->logBuff.typ == LOG)
											{
												/*
												int shmid = shmget ((key_t)(55500 + taskp->refcount), (size_t)taskp->shm_size, 0666);
												if (shmid == -1)
												{
													ACE_OS::fprintf(stdout, "ERROR getting reference to shared memory of the task\n");
													break;
												}

												void * shmp = shmat (shmid, NULL, 0);
												if (shmp == (void *) -1)
												{
													ACE_OS::fprintf(stdout, "ERROR attaching shared memory of the task\n");
													break;
												}
												*/
												ACS_TRA_lib::display_log (1, delimiter, taskp, 1);
											}
											else
													ACE_OS::fprintf(stdout, "task filter is not log \n");
									}
								}
							}
						}
					}
					break;

				case 'D':
					/*
					 * Command description: Delete the task name
					 * Syntax: D task_name
					 */

					if (ACE_OS::strlen(command) > 1)
							ACE_OS::fprintf (stdout, "Usage: D [taskname]\n");
					else
					{
						// get exclusive access to top shared memory
						ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
						if(!top_sm_mutex_guard.locked())
						{
							ACE_OS::fprintf(stdout, "Error deleting TASK: unable to acquire shared memory mutex\n");
							break;
						}

						taskname = ACS_TRA_common::get_token();

						// check if there are other parameters
						filter = ACS_TRA_common::get_token();

						if (filter != NULL)
						{
							ACE_OS::fprintf (stdout, "Usage: D [taskname]\n");
							break;
						}

						// Delete the specified task name or all tasks name
						ACS_TRA_lib::delete_task(taskname, 2, delimiter);
					}
					break;

				case 'L':
					/*
					 * Command description: List all trace tasks
					 * Syntax: L
					 */

					if (ACE_OS::strlen(command) > 1)
						ACE_OS::fprintf (stdout, "Usage: L\n");
					else
					{
						// get exclusive access to top shared memory
						ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
						if(!top_sm_mutex_guard.locked())
						{
							ACE_OS::fprintf(stdout, "Error listing TASKs: unable to acquire shared memory mutex\n");
							break;
						}

						filter = ACS_TRA_common::get_token();
						if (filter != NULL)
							ACE_OS::fprintf (stdout, "Usage: L\n");
						else
							ACS_TRA_util::list_tasks(0);
					}
					break;

				case 'W':
					/*
					 * Command description: Puts ACS trace funcion to sleep
					 *                      for some specified seconds
					 * Syntax: W time_in_seconds
					 */

					if (ACE_OS::strlen(command) > 1)
						ACE_OS::fprintf (stdout, "Usage: W number_of_seconds\n");
					else
					{
						/*
						 * Fetch the number of seconds that the ACS trace function
						 * sleep. If the seconds not specified print error
						 * message and return to trautil console
						 */
						if ((timep = ACS_TRA_common::get_token()) == NULL)
							ACE_OS::fprintf(stdout, "Time parameter missing\n");
						else
						{
							if (ACS_TRA_common::isDigit(timep) == 1)
								ACE_OS::fprintf(stdout, "Usage: W number_of_seconds\n");
							else
							{
								// check if there are other parameters
								if (ACS_TRA_common::get_token() != NULL)
								{
									ACE_OS::fprintf(stdout, "Usage: W number_of_seconds\n");
									break;
								}

								ACS_TRA_util::suspend_thread();

								ACE_OS::sleep (ACE_OS::atoi(timep));

								//ACS_TRA_util::activate_thread(delimiter);
								autoflush_thread_manager.restart_stopped_threads();
							}
						}
					}
					break;

				case '?':
					/*
					 * Command description: List the trace commands available
					 * Syntax: ?
					 */

					if (ACE_OS::strlen(command) > 1)
						ACE_OS::fprintf (stdout, "Usage: ?\n");
					else
					{
						filter = ACS_TRA_common::get_token();
						if (filter != NULL)
							ACE_OS::fprintf (stdout, "Usage: ?\n");
						else
						{
							ACE_OS::fprintf (stdout, "Trace commands:\n");
							ACE_OS::fprintf (stdout, "    Create task\n");
							ACE_OS::fprintf (stdout, "      Usage: C taskname trace_type trace_set LOG number_of_buffers [log_filename [SIZE megabytes]]\n             C taskname trace_type trace_set LOG number_of_buffers [AUTOFLUSH seconds SCREEN]\n             C taskname trace_type trace_set LOG number_of_buffers [AUTOFLUSH seconds log_filename [SIZE megabytes]]\n");
							ACE_OS::fprintf (stdout, "    Delete task\n");
							ACE_OS::fprintf (stdout, "      Usage: D [taskname] \n");
							ACE_OS::fprintf (stdout, "    Flush trace log\n");
							ACE_OS::fprintf (stdout, "      Usage: F taskname\n");
							ACE_OS::fprintf (stdout, "    List tasks\n");
							ACE_OS::fprintf (stdout, "      Usage: L\n");
							ACE_OS::fprintf (stdout, "    Wait \n");
							ACE_OS::fprintf (stdout, "      Usage: W number_of_seconds\n");
							ACE_OS::fprintf (stdout, "    quit trace_utility\n");
							ACE_OS::fprintf (stdout, "      Usage: quit\n");
							ACE_OS::fprintf (stdout, "    Check and display shared trace memory internal data\n");
							ACE_OS::fprintf (stdout, "      Usage: X\n");
							ACE_OS::fprintf (stdout, "    Help to trace_utility\n");
							ACE_OS::fprintf (stdout, "      Usage: ?\n");
						}
					}

					break;

				case 'X':
					/*
					 * Command description: Check and display content of internal
					 * 						data structures
					 * Syntax: X
					 */

					if (ACE_OS::strlen(command) > 1)
						ACE_OS::fprintf (stdout, "Usage: X\n");
					else
					{
						filter = ACS_TRA_common::get_token();
						if (filter != NULL)
							ACE_OS::fprintf (stdout, "Usage: X\n");
						else
						{
							if (ACS_TRA_lib::acquire_shared_memory_mutex() == -1)
							{
								ACE_OS::fprintf(stdout, "Failure to acquire shared memory mutex: %s\n ", strerror(errno));
								exit(1);
							}

							ACE_OS::fprintf (stdout, "TRACE MEMORY REV %s SIZE = %d \n\n",shmp->rev, shmp->maxTraceSize);
							ACE_OS::fprintf (stdout, "FREE TRACE DATA = 0x%x, FREE TASK DATA =0x%x \n",	 (unsigned int)shmp->free_trace_data, (unsigned int)shmp->free_task_data);
							ACE_OS::fprintf (stdout, "\n\n");

							// list the trace tasks
							ACS_TRA_util::list_tasks(1);

							// list the pid tables
							ACS_TRA_util::list_pid_table();

							// list all trace elements
							ACS_TRA_util::list_trace_elements();

							// list the free buffer chain
//							ACS_TRA_util::list_free_buffers();

							// list free task data
//							ACS_TRA_util::list_free_task_data();

							// list the entries in the hash chains
							ACS_TRA_util::list_hashtable();

							// After deleting the task, release the shared memory mutex
							if (ACS_TRA_lib::release_shared_memory_mutex() == -1)
							{
								ACE_OS::fprintf(stdout, "Failure to release shared memory mutex: %s\n ", strerror(errno));
								exit(1);
							}
						}
					}
					break;

				default:
					/*
					 * If you press any button other than the commands over,
					 * the list if the trace commands available is printed
					 */
					ACE_OS::fprintf(stdout, "usage : C (create task), F (flush log), D (delete task), L (list tasks), W (wait), quit, X, ? (help)\n");
			}
		}

		ACS_TRA_lib::reset_cmd_chars();
	}

	// unreachable line but to satisfy Visual
	return (0);
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: suspend_thread
 * Description: Suspend the thread for autoflush
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
int ACS_TRA_util::suspend_thread()
{
	trace_taskp     taskp;
	ACE_INT32		t, n;

	// get exclusive access to top shared memory
	ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
	if(!top_sm_mutex_guard.locked())
	{
		ACE_OS::fprintf(stdout, "ERROR: '%s()': unable to acquire shared memory mutex\n", __PRETTY_FUNCTION__);
		return -1;
	}

	for (t = 0, n = 0; n < shmp->taskcount; t++)
	{
		if (shmp->tasksTab[t].refcount != -1)
		{
			n++;
			taskp = &shmp->tasksTab[t];

			if ((ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER"))) == 0)
			{
					if (taskp->autoflush != 0)
					{
						/*
						// Cancel all threads
						if (ACE_Thread::cancel(taskp->thread_id) != 0)
								ACE_OS::fprintf(stdout, "Is not possible delete the thread\n");
						*/
						// stop autoflush thread, but only if has been created by this process
						ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl = autoflush_thread_manager.find_thread_handle_by_OS_thread_id(taskp->thread_id);
						if(thr_hdl && (autoflush_thread_manager.stop_thread(thr_hdl) != 0))
							ACE_OS::fprintf(stdout, "ERROR: unable to stop autoflush thread %lu for task '%s'\n", taskp->thread_id, taskp->name);
					}
			}
		}
	}

	return 0;
}


/*
 * Class Name: ACS_TRA_util
 * Method Name: activate_thread
 * Description: Activate the threads
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
int ACS_TRA_util::activate_thread(char delimiter)
{
	trace_taskp     taskp;
	ACE_INT32		t, n;
	//ACE_thread_t    thread_id;

	// get exclusive access to top shared memory
	ACE_Guard<ACE_Process_Mutex> top_sm_mutex_guard( * ACS_TRA_lib::shared_memory_mutex_p());
	if(!top_sm_mutex_guard.locked())
	{
		ACE_OS::fprintf(stdout, "ERROR: '%s()': unable to acquire shared memory mutex\n", __PRETTY_FUNCTION__);
		return -1;
	}

	for (t = 0, n = 0; n < shmp->taskcount; t++)
	{
		if (shmp->tasksTab[t].refcount != -1)
		{
			n ++;
			taskp = &shmp->tasksTab[t];

			if ((ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER"))) == 0)
			{
				if ((taskp->autoflush != 0) && !taskp->video && !is_thread_alive(taskp->thread_id))
				{
					/*
					// Create a new object with the taskp and the delimiter
					Args* arg = new Args(taskp, delimiter);
					ACE_Thread::spawn((ACE_THR_FUNC)ACS_TRA_lib::autoflush, (void*)arg, THR_NEW_LWP|THR_JOINABLE, &thread_id);
					*/
					ACE_thread_t autoflush_thread_handle = autoflush_thread_manager.create_thread(taskp, delimiter);
					if(autoflush_thread_handle == 0)
						ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_lib::create_task(): unable to create autoflush thread for task %s\n", taskp->name);
					else if(autoflush_thread_manager.start_thread(autoflush_thread_handle) < 0)
						ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_lib::create_task(): unable to start autoflush thread for task %s\n", taskp->name);
					else
						taskp->thread_id = autoflush_thread_manager.get_OS_thread_id(autoflush_thread_handle);
				}
			}
		}
	}

	return 0;
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: flush
 * Description: Automatic flush on a file
 * Used: This method is used in the following member:
 *       main program of ACS_TRA_util.cpp file
 */
void ACS_TRA_util::flush(void* arguments)
{
	ACE_INT32 		i = 1;
	char            *taskname;
	char 			delimiter;
	trace_taskp     pointer;

	// Retrieve the taskp and the delimiter
	Args *arg = (Args*) arguments;

	// Store the delimiter and the taskp in local variable
	pointer   = arg->pointer;
	delimiter = arg->delimiter;

	while (i == 1)
	{
		// Wait the time of autoflush
		ACE_OS::sleep(1);

		// Retrieve the task name
		taskname = pointer->name;
		/*
		 * If task name is NULL an error message is
		 * printed and return to trautil console
		 */
		if (taskname == NULL)  ACE_OS::fprintf(stdout, "Failure: taskname not found\n");
		else
		{
			/*
			 * The task name is valid and found in trace shared memory,
			 * retrieve the user that use the console trautil and check
			 * if he has permission to flush this trace log.
			 */
			if ((ACE_OS::strcmp(pointer->user, ACE_OS::getenv("USER"))) != 0)
							ACE_OS::fprintf(stdout, "Not allowed to flush other's trace log\n");
			else
			{
				if (pointer->logBuff.typ == PID  ||  pointer->logBuff.typ == LOG)
				{
/**
					ACE_OS::fprintf(stdout, "startbuf: %x \n", lp->startbuf);
					ACE_OS::fprintf(stdout, "startpos: %x \n", lp->startpos);
					ACE_OS::fprintf(stdout, "currbuf: %x \n", lp->currbuf);
					ACE_OS::fprintf(stdout, "currpos: %x \n", lp->currpos);
					ACE_OS::fprintf(stdout, "nobuf: %i \n", lp->nobuf);

					// Check if log buffer available
					if (lp->startbuf + 8 != lp->startpos)
					{
						relpointer tmp;
						tmp = lp->startbuf;
						if (pointer->filename[0] == 0)
							ACE_OS::fprintf(stdout, "\n");
**/
						// Call method to display and save the log
						ACS_TRA_lib::display_log (1, delimiter, pointer);

						// Print on standard output the prompt of the command
						ACE_OS::fputs("trc> ", stdout);
						// Flush the specified trace task name on the standard output
						ACE_OS::fflush (stdout);
//					}
				}
				else  ACE_OS::fprintf(stdout, "Task filter is not log\n");
			}
		}
	}
}

/*
 * Class Name: ACS_TRA_util
 * Method Name: check_buffer_full
 * Description: Activate the threads
 * Used: This method is used in the following member:
 *       main program in ACS_TRA_util.cpp file
 */
void ACS_TRA_util::check_buffer_full(char delimiter)
{
	trace_taskp     taskp;
	ACE_INT32		t, n;
	ACE_thread_t    thread_id;

	/*
	 * Check if the current user have some traces,
	 * if yes re-activate the threads for autoflush
	 */
	for (t = 0, n = 0; n < shmp->taskcount; t++)
	{
		if (shmp->tasksTab[t].refcount != -1)
		{
			n++;
			taskp = &shmp->tasksTab[t];

			if ((ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER"))) == 0)
			{
					// Create a new object with the taskp and the delimiter
					Args* arg = new Args(taskp, delimiter);
					ACE_Thread::spawn((ACE_THR_FUNC)ACS_TRA_util::flush, (void*)arg, THR_NEW_LWP|THR_JOINABLE, &thread_id);
			}
		}
	}
}

int ACS_TRA_util::is_thread_alive(pid_t os_tid)
{
	// Pay attention : here we should check not only the existence of '/proc/*/<thr_id>/cmdline' file,
	// but even the thread state (could be a "zombie"). Anyway, for the moment we don't do this check !
	char cmd[1024] = {0};
	snprintf(cmd, 1024, "grep ^trautil /proc/*/task/%d/cmdline >& /dev/null", os_tid);
	//ACE_OS::fprintf(stdout, "launching command '%s'\n", cmd);
	int call_result = system(cmd);
	return !call_result;
}
