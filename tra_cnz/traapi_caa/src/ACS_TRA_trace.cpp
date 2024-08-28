/*
 *
 * NAME: ACS_TRA_trace.cpp
 *
 * COPYRIGHT Ericsson Telecom AB 2014
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This file contains the definitions of all methods
 *  of class ACS_TRA_trace
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 	2010-05-06 XYV XGIOPAP
 *
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1	        20100506	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 * PA2 	        20101104	XGIOPAP		Implementation of comment
 
 *              20140327        XSARSES         TRAUTILBIN - core dump fix
 */

// Module Include Files
#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"
#include "ACS_TRA_trace.h"
#include "ACS_TRA_SigMaskUtil.h"
#include <vector>
#include "ace/Atomic_Op_T.h"
#include "ace/Mutex.h"

#ifdef	__TRALIB_DEBUG_ENABLED__
#include <syslog.h>
#endif

using namespace std;

// global variables
ACE_INT32 ACS_TRA_flag = 1;			// initial trace mask for the process
static ACS_TRA_lib ACS_TRA_lib;

#ifdef	__TRALIB_DEBUG_ENABLED__
static ACE_Atomic_Op<ACE_Mutex, unsigned long> next_available_trace_key = 1; // sequence used to identify calls to ACS_TRA_Trace_ACS_TRA_Event()
#endif

struct trace_task  tab_Tasks[MAX_TASK_COUNT];		// HO92979


void  store_Tasks (void)     // HO92979
{
	trace_taskp   taskp;
	register int  j;

	for (j=0; j<MAX_TASK_COUNT; j++)
	{
		if (shmp->tasksTab[j].refcount != -1)
		{
				taskp = &shmp->tasksTab[j];

				tab_Tasks[j].refcount  = taskp->refcount;
				tab_Tasks[j].autoflush = taskp->autoflush;
				tab_Tasks[j].size      = taskp->size;
				tab_Tasks[j].video     = taskp->video;
				tab_Tasks[j].set       = taskp->set;
				tab_Tasks[j].thread_id = taskp->thread_id;

				strcpy (tab_Tasks[j].user, taskp->user);
				strcpy (tab_Tasks[j].name, taskp->name);
				strcpy (tab_Tasks[j].filename, taskp->filename);
		}
		else
				tab_Tasks[j].refcount = -1;
	}
}

void  update_Tasks (void)       // HO92979
{
	trace_taskp   taskp;
	register int  j;

	for (j=0; j<MAX_TASK_COUNT; j++)
	{
		if (shmp->tasksTab[j].refcount != -1  &&  tab_Tasks[j].refcount != -1)
		{
				taskp = &shmp->tasksTab[j];

				taskp->refcount  = tab_Tasks[j].refcount;
				taskp->autoflush = tab_Tasks[j].autoflush;
				taskp->size      = tab_Tasks[j].size;
				taskp->video     = tab_Tasks[j].video;
				taskp->set       = tab_Tasks[j].set;
				taskp->thread_id = tab_Tasks[j].thread_id;

				strcpy (taskp->user, tab_Tasks[j].user);
				strcpy (taskp->name, tab_Tasks[j].name);
				strcpy (taskp->filename, tab_Tasks[j].filename);
		}
	}
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: ACS_TRA_trace
 * Description: Class constructor
 * Used: This method is used in the code that you want to trace
 */
ACS_TRA_trace::ACS_TRA_trace (char* name, char* format)
{
	memset(recordname,0,100);
	memset(dataformat,0,100);
	traceflagptr = &ACS_TRA_flag;
	strncpy(recordname, name, sizeof(recordname) - 1);
	strncpy(dataformat, format, sizeof(dataformat) - 1);
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: ACS_TRA_trace
 * Description: Class constructor
 * Used: This method is used in the code that you want to trace
 */
ACS_TRA_trace::ACS_TRA_trace (const char* name, const char* format)
{
	memset(recordname,0,100);
	memset(dataformat,0,100);
	traceflagptr = &ACS_TRA_flag;
	strncpy(recordname, name, sizeof(recordname) - 1);
	strncpy(dataformat, format, sizeof(dataformat) - 1);
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: ACS_TRA_trace
 * Description: Class constructor
 * Used: This method is used in the code that you want to trace
 */
ACS_TRA_trace::ACS_TRA_trace (string name, string format)
{
	memset(recordname,0,100);
	memset(dataformat,0,100);
	traceflagptr = &ACS_TRA_flag;
	strncpy(recordname, name.c_str(), sizeof(recordname) - 1 );
	strncpy(dataformat, format.c_str(), sizeof(dataformat) - 1);
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: ACS_TRA_trace
 * Description: Class constructor
 * Used: This method is used in the code that you want to trace
 */
ACS_TRA_trace::ACS_TRA_trace (const string &className)
{
	memset(recordname,0,100);
	memset(dataformat,0,100);
	traceflagptr = &ACS_TRA_flag;
	strncpy(recordname, className.c_str(), sizeof(recordname) - 1);
	strcpy(dataformat, "C");
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: ACS_TRA_ON
 * Description: Check if the trace is available
 * 				for the related task
 * Used: This method is used in the code that you want to trace
 */
//int ACS_TRA_trace::ACS_TRA_ON()
//{
//	if (  traceflagptr == 0 ) {
//		traceflagptr = &ACS_TRA_flag;
//		memset(recordname,0,100);
//		memset(dataformat,0,100);
//		return 0;
//	}

//	return *traceflagptr;
//}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: isOn
 * Description: Check if the trace is available
 * 				for the related task
 * Used: This method is used in the code that you want to trace
 */
//bool ACS_TRA_trace::isOn()
//{
//	return (ACS_TRA_ON() != 0);
//}


void  writeOnFile (const char *task, const char *info)
{

	char  fileName[100];

	strcpy (fileName, "/root/TRA_");
	strcat (fileName, task);
	strcat (fileName, ".log");

	FILE  *file_param = ACE_OS::fopen(fileName, "a");

	if (file_param != NULL)
	{
			ACE_OS::fputs (info, file_param);
			ACE_OS::fflush (file_param);
			ACE_OS::fclose (file_param);
	}
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: do_log
 * Description: This method check the buffer position
 * 				and create the log in the buffer with
 * 				the date and time of creation.
 * Used: This method is used in the following member:
 *       ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp file
 */
void  ACS_TRA_trace::do_log (trace_taskp taskp, char proc[], char record[], struct timeval * timeofday, va_list ap)
{

		ACE_INT64       i, strl;
		char      		c;
		ACE_INT64       bp;
		ACE_INT32       n, count;
		char           *p, startTag[5];
		bool		    bpError = false;

		int  		shmid;
		relpointer  shmp;

		if (strlen(proc) > 50  ||  strlen(record) > 50  ||
			strlen(dataformat) > 50  ||  strlen(dataformat) == 0)
		{
				return;
		}

		shmid = shmget ((key_t)(55500 + taskp->refcount), (size_t)taskp->shm_size, 0666);
		if (shmid == -1)
		{
				writeOnFile (taskp->name, "\nERROR with the shmget !!!\n\n");
				return;
		}

		shmp = (relpointer) shmat (shmid, NULL, 0);
		if (shmp == -1)
		{
				writeOnFile (taskp->name, "\nERROR with the shmat !!!\n\n");
				return;
		}

		startTag[0] = 'w';  startTag[1] = 'x';
		startTag[2] = 'y';  startTag[3] = 'z';  startTag[4] = 0;

		logp  lp = &(taskp->logBuff);

		if (lp->currpos + 512 >= taskp->shm_size)
		{
				lp->circular = 1;
				lp->currpos  = 0;
		}

		bp = (ACE_INT64) (shmp + lp->currpos);

		ACE_OS::memcpy ((char *) bp, (char *) startTag, 4);
		bp = bp + 4;

		(*(ACE_INT64 *) bp) = timeofday->tv_sec;		// time stamp
		ADVANCE (sizeof(ACE_INT64));
		(*(ACE_INT64 *) bp) = timeofday->tv_usec;
		ADVANCE (sizeof(ACE_INT64));

		(*(ACE_INT64 *) bp) = ACE_OS::getpid();			// pid
		ADVANCE(sizeof(ACE_INT64));

		memcpy ((char *) bp, (char *) proc, strlen(proc)+1);
		ADVANCE(strlen(proc)+1);

		memcpy ((char *) bp, (char *) record, strlen(record)+1);
		ADVANCE(strlen(record)+1);

		memcpy ((char *) bp, (char *) dataformat, strlen(dataformat)+1);
		ADVANCE(strlen(dataformat)+1);

//char            buffer[50];
//sprintf (buffer, "DATAF = %s\n", dataformat);
//writeOnFile (taskp->name, buffer);

		i = 0;		// get parameters

		while (dataformat[i] != '\0')
		{
			while (isspace(dataformat[i]))  i ++;

			c = dataformat[i];
			i ++;  n = 0;

			if (isdigit(dataformat[i]))  n = ACE_OS::atoi(&(dataformat[i]));

			while (isdigit(dataformat[i]))  i++;

			if (n > 0  ||  c == 'C')
			{
				p = va_arg(ap, char *);
				if (p != NULL)
				{
					switch (c)
					{
						case 's':
						case 'S':  ALIGN(bp, sizeof(ACE_INT16));
								   n *= sizeof(ACE_INT16);
								   break;
						case 'l':
						case 'L':  ALIGN(bp, sizeof(ACE_INT64));
								   n *= sizeof(ACE_INT64);
								   break;
						case 'i':
						case 'I':
						case 'p':  ALIGN(bp, sizeof(ACE_INT32));
								   n *= sizeof(ACE_INT32);
								   break;

						case 'C':  strl = ACE_OS::strlen(p) + 1;
								   n = (n < strl) && (n > 0) ? n : strl;
								   break;
					}

					do {
							if (bp >= (ACE_INT64) (shmp + lp->currpos + 512))
							{
									bpError = true;
									break;
							}
							if ((bp + n) >= (ACE_INT64) (shmp + taskp->shm_size))
							{
									n = (ACE_INT64) (shmp + taskp->shm_size - bp - 4);
									p[n-1] = '\0';
							}

							count = n;
							ACE_OS::memcpy ((char *) bp, (char *) p, (size_t) count);

							bp += count;
							n = 0;

					} while (n > 0);
				}
				else {      // allow empty strings

						if (c == 'C')
						{
								(*(char*)bp) = '\0';
								ADVANCE(sizeof(char));
						}
				}
			}
			else {

				switch (c)
				{
					case ('b'):
					case ('B'):
					case ('c'):	 (*(char *) bp) = va_arg(ap, ACE_INT32);
								 ADVANCE(sizeof(char));
								 break;
					case ('s'):
					case ('S'):  ALIGN(bp, sizeof(ACE_INT16));
								 (*(ACE_INT16 *) bp) = va_arg(ap, ACE_INT32);
								 ADVANCE(sizeof(ACE_INT16));
								 break;
					case ('i'):
					case ('I'):
					case ('p'):  ALIGN(bp, sizeof(ACE_INT32));
								 (*(ACE_INT32 *) bp) = va_arg(ap, ACE_INT32);
								 ADVANCE(sizeof(ACE_INT32));
								 break;
					case ('l'):
					case ('L'):  ALIGN(bp, sizeof(ACE_INT64));
								 (*(ACE_INT64 *) bp) = va_arg(ap, ACE_INT64);
								 ADVANCE(sizeof(ACE_INT64));
								 break;
				}

				while (isspace(dataformat[i]))  i++;

				if (dataformat[i] == ',')  i++;
			}
		}
		va_end(ap);

		if (bpError == false)
		{
				lp->currpos = bp - (ACE_INT64) shmp;

				if (lp->circular == 1)
				{
						bool  setStartPos = false;

						while (bp < ((ACE_INT64) shmp + taskp->shm_size - 5))
						{
								if (*(char *) bp   == 'w'  &&  *(char *) bp+1 == 'x'  &&
									*(char *) bp+2 == 'y'  &&  *(char *) bp+3 == 'z')
								{
										lp->startpos = (relpointer) (bp - (ACE_INT64) shmp);
										setStartPos = true;
										break;
								}
								bp ++;
						}
						if (setStartPos == false)  lp->startpos = 0;
//char  buffer[50];
//sprintf (buffer, "CURR-POS = %d    ", (int)lp->currpos);
//writeOnFile (taskp->name, buffer);
//sprintf (buffer, "START-POS = %d\n", (int)lp->startpos);
//writeOnFile (taskp->name, buffer);
				}
		}

		shmdt ((void *)shmp);
}

/*
 * Class Name: ACS_TRA_trace
 * Method Name: remove_pid
 * Description: Remove the pid from the pid table.
 * Used: This method is used in the following member:
 *       ACS_TRA_trace::install_pid in ACS_TRA_trace.cpp file
 */
void remove_pid(void)
{

	register int  j = 0;

    if (ACS_TRA_lib::acquire_shared_memory_mutex() == -1)
    {
    	char buf[256] = {0};
		snprintf(buf, 256, "ERROR: '%s' --> Unable to acquire shared memory mutex !", __func__);
		ACS_TRA_lib::trace_error(buf, 0);
    }

    while (j < MAX_PIDS  &&  shmp->pidTab[j].pid != pid)  j ++;

    if (j < MAX_PIDS)  shmp->pidTab[j].pid = 0;

	if (ACS_TRA_lib::release_shared_memory_mutex() == -1)
	{
		char buf[256] = {0};
		snprintf(buf, 256, "ERROR: '%s' --> Unable to release shared memory mutex !", __func__);
		ACS_TRA_lib::trace_error(buf, 0);
	}
}

/*
 * Class Name: ACS_TRA_trace
 * Method  Name: install_pid
 * Description: Install the pid in the pid table.
 *              pid records are dynamically allocated,
 *              and kept in a free list. But they are
 *              allocated from the trace data area, in
 *              order to be able to reset the task data
 *              area, when all tasks are deleted, without
 *              losing the pid to process conversion table.
 * Used: This method is used in the following member:
 *       ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp file
 */
void ACS_TRA_trace::install_pid(relpointer proc)
{

	register int  j = 0;

	pid = ACE_OS::getpid();

	while (j < MAX_PIDS  &&  shmp->pidTab[j].pid != 0)  j ++;

	if (j < MAX_PIDS)
	{
			shmp->pidTab[j].proc = proc;
			shmp->pidTab[j].pid  = pid;

			ACE_OS::atexit(remove_pid);
	}
}

/*
 * Class Name: ACS_TRA_trace
 * Method  Name: getProcName
 * Description: Retrieve the process name from the pid
 * Used: This method is used in the following member:
 *       ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp file
 */
char* ACS_TRA_trace::getProcName(char* b)
{
	char  	 ProcPath[100];
	FILE 	*file_proc;
	char  	 buffer[255];

	ACE_OS::sprintf(ProcPath, "/proc/%i/cmdline", ACE_OS::getpid());

	// Reset the process name element
	ACE_OS::memset(buffer, 0, sizeof(buffer));

	// Open the cmdline file and read the process name
	file_proc = ACE_OS::fopen (ProcPath, "r");

	if (file_proc == NULL)
		b = const_cast<char *>("NONAME");
	else
	{
		// Read the value
		ACE_OS::fgets (buffer , sizeof(buffer) - 1, file_proc);

		//close the file
		ACE_OS::fclose (file_proc);

		// copy the value which return
		ACE_OS::strcpy(b, buffer);

		/*
		 * If the process name contains the "./" cut it,
		 * otherwise return the process name typed
		 */
		if (strrchr(b, '/') != NULL)
		{
			b = strrchr(b, '/');
			b++;
		}
	}

	return (b);
}

/*
 * Class Name: ACS_TRA_trace
 * Method  Name: getRecordName
 * Description: Retrieve the value of the private variable recordname
 * Used: This method is used in the following member:
 *       This is a public API
 */
string ACS_TRA_trace::getRecordName()
{
	string tmp = recordname;
	return tmp;
}

/*
 * Class Name: ACS_TRA_trace
 * Method  Name: ACS_TRA_event
 * Description: This routine will log the trace event with
 *              its data parameter in the ACS_TRA shared memory.
 *  			The resulting trace log is displayed via the
 *    			trace_utility program.
 * Parameters:	trcbp - pointer to the trace control block for this
 *		                trace event
 *
 *				...   - variable list of data parameters
 *
 * The number of data parameters, and the type of each data parameter
 * must agree with the descriptor specified in the format parameter of
 * the ACS_TRA_DEF macro
 * Used: This method is used in the code that you want to trace.
 */
void ACS_TRA_trace::ACS_TRA_event (int num_par,...)
{

	static char	    	procname[255];
	relpointer  		procaddr = 0;
	static  ACE_INT32   pid_installed = 0;
	ACE_INT32           i, n = 0;
	ACE_INT32           mask = 0;
	trace_elementp	    trcep = 0;
	relpointer          trippel[3] = {0,0,0};
	va_list             ap;
	trace_taskp         taskp = NULL;
	relpointer          trceoffs = 0;
	struct timeval      timeofday;
	ACE_INT32	    	semflag = 0;
	char		    	*tmp = 0;
	ACE_INT32	    	inew = 1;
	char				b[255];

#ifdef	__TRALIB_DEBUG_ENABLED__
	unsigned long trace_key = next_available_trace_key ++;
#endif

	std::vector<int> sig_to_block;
	sig_to_block.push_back(SIGTERM);
	sig_to_block.push_back(SIGINT);
	sig_to_block.push_back(SIGTSTP);
	ACS_TRA_SigMaskUtil sigMaskUtil(sig_to_block);

	// if (shmp  &&  shmp->taskcount == 0   &&  pid_installed == 1) return;

	if (shmp == NULL)
	{

			if (ACS_TRA_lib::get_shared_memory() == -1)
			{
					ACS_TRA_flag = 0;
#ifdef	__TRALIB_DEBUG_ENABLED__
					syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : ACS_TRA_event() check point 1. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
					return;
			}
	}

	if (traceflagptr != &ACS_TRA_flag)
	{
		/*
		 * not first call -
		 * check that the trace_element recname corresponds
		 * to the trace control block recname
		 */
		trcep = (trace_elementp)((ACE_INT64) traceflagptr - 4 * sizeof(relpointer));

		trceoffs = (relpointer)((ACE_INT64) traceflagptr - (ACE_INT64) shmp - 4 * sizeof(relpointer));

		// Check 'trcep->record' to verify that it's a valid OFFSET (in particular, it must be the OFFSET of a "REC" trace_data, having as key a string
		// whose length is at least equal to the length of "recordname" string
		if( 0 == ACS_TRA_lib::check_trace_data_p(trcep->record, sizeof(trace_data) + strlen(recordname) + 1, false ) )
			tmp = ABS(char *,trcep->record) + sizeof(struct trace_data);

		if (tmp != NULL)
		{
				inew = ACE_OS::strncmp(tmp, recordname, sizeof(recordname));
		}
		else  inew = 1;
	}

	if (inew)		// first call
	{
//		if (shmp == NULL)
//			if (ACS_TRA_lib::get_shared_memory() == -1)
//			{
				// trace not installed or started, reset trace flag
				// ACS_TRA_lib::trace_error ("Failure to attach trace shared memory\n", 0);
//				ACS_TRA_flag = 0;
//				return;
//			}

		if (shmp->lev == 0)
		{
				ACS_TRA_lib::trace_error("Incompatible version of trace shared memory\n", 0);
				ACS_TRA_flag = 0;
#ifdef	__TRALIB_DEBUG_ENABLED__
				syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : ACS_TRA_event() check point 2. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
				return;
		}

#ifdef	__TRALIB_DEBUG_ENABLED__
		if (ACS_TRA_lib::acquire_shared_memory_mutex(trace_key) == -1)
#else
		if (ACS_TRA_lib::acquire_shared_memory_mutex() == -1)
#endif
		{
				ACS_TRA_lib::trace_error( "Failure to acquire shared memory mutex\n", 0);
#ifdef	__TRALIB_DEBUG_ENABLED__
				syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : ACS_TRA_event() check point 3. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
				return;
		}
		semflag = 1;

		/*
		 * build trace element
		 * check process
 		 * check pid-installed flag  instead of procaddr
		 * as procaddr is NULL if no PROCNAME
		 */
		if (! pid_installed)
		{
				ACE_OS::memset(b, 0, sizeof(b));

//				procname = getProcName(b);
				strcpy (procname, getProcName(b));

				// NT port PROC->PRCS
				procaddr = ACS_TRA_lib::get_trace_data_p(procname, PROC, 0);
				install_pid (procaddr);
				pid_installed = 1;
		}
		else
			procaddr = ACS_TRA_lib::get_trace_data_p(procname, PROC, 0);

		trippel[0] = procaddr;

		// check record name
		if(trippel[0] != 0)
			trippel[1] = ACS_TRA_lib::get_trace_data_p((char *)recordname, REC, 0);

		// check format
		if(trippel[1] != 0)
			trippel[2] = ACS_TRA_lib::get_trace_data_p((char *)dataformat, FORMAT, 0);

		// check trace element
		if(trippel[2] != 0)
		{
			trceoffs = ACS_TRA_lib::get_trace_data_p((char *) &trippel, TRCE, 0) + sizeof(struct trace_data);
			if(trceoffs != sizeof(struct trace_data))
				trcep = ABS(trace_elementp, trceoffs);
		}

		// if trace element has been obtained, set 'traceflagptr'
		if(trcep != 0)
		{
			traceflagptr = &(trcep->mask);

			// set up trace mask  for this trace element
			for (i = 0; n < shmp->taskcount; i++)
			{
					if (shmp->tasksTab[i].refcount != -1)
					{
							n ++;
							ACS_TRA_lib::update_mask (i, shmp->tasksTab[i].set, trcep);
					}
			}
		}
	}

	// execute tasks (but only if trace element has been obtained)
	if(trcep)
		mask = *traceflagptr;

	if (mask)
	{
		if (semflag == 0)
		{
#ifdef	__TRALIB_DEBUG_ENABLED__
			if (ACS_TRA_lib::acquire_shared_memory_mutex(trace_key) == -1)
#else
			if (ACS_TRA_lib::acquire_shared_memory_mutex() == -1)
#endif
			{
				ACS_TRA_lib::trace_error("Failure to acquire shared memory mutex", 0);
#ifdef	__TRALIB_DEBUG_ENABLED__
				syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : ACS_TRA_event() check point 4. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
				return;
			}
		}

		semflag = 1;

		// get a time stamp for the trace event
		gettimeofday(&timeofday, 0);

//		store_Tasks ();     // HO92979

		i = 0;  n = 0;

		while ((mask != 0) && (i < MAX_TASK_COUNT /*shmp->taskcount)*/))
		{
			if ((mask % 2) == 1  &&  shmp->tasksTab[i].refcount != -1)
			{
				taskp = &shmp->tasksTab[i];

				switch (taskp->logBuff.typ)
				{
					case  LOG :
					case  PID :	 va_start(ap, num_par);

								 char  proc[50], record[50];

								 tmp = ABS(char *, trcep->proc) + sizeof(struct trace_data);
								 if (tmp[0] == '\0')  sprintf (proc, "NONAME ");
								 else 				  sprintf (proc, "%s ", tmp);

								 tmp = ABS(char *, trcep->record) + sizeof(struct trace_data);
								 sprintf (record, "%s ", tmp);

								 if (taskp->logBuff.typ == PID)
								 {
										 if (taskp->logBuff.pid == pid)
												 do_log (taskp, proc, record, &timeofday, ap);
								 }
								 else  do_log (taskp, proc, record, &timeofday, ap);

//								 update_Tasks ();   // HO92979
								 break;

					default :  ;
				}
			}
			mask >>= 1;
			i++;
		}
	}

	if (semflag == 1)
	{
#ifdef	__TRALIB_DEBUG_ENABLED__
		if (ACS_TRA_lib::release_shared_memory_mutex(trace_key) == -1)
#else
		if (ACS_TRA_lib::release_shared_memory_mutex() == -1)
#endif
		{
			ACS_TRA_lib::trace_error("Failure to release shared memory mutex ", 0);
#ifdef	__TRALIB_DEBUG_ENABLED__
			syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : ACS_TRA_event() check point 5. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
		}
	}
}

/*
 * Class Name: ACS_TRA_trace
 * Method  Name: trace
 * Description: This routine will log the trace event with
 *              its data parameter in the ACS_TRA shared memory.
 *  			The resulting trace log is displayed via the
 *    			trace_utility program.
 * Parameters:	s - the info to trace
 *
 * Used: This method is used in the code that you want to trace.
 */
void ACS_TRA_trace::trace (const string &s)
{
	ACS_TRA_event(1, s.c_str());
}
