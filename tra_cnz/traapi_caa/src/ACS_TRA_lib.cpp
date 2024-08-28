/*
 * NAME: ACS_TRA_lib.cpp
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
 *  of class ACS_TRA_lib
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
 *
 * AUTHOR
 * 	2010-05-04 XYV XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	        20100504	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *				  						"APG50 Tracing and MKTR"
 * PA2 	        20101104	XGIOPAP		ImplemeMAX_CHARS_LOG_FILENAMEntation of comment
 * PA3 	        20110117	XGIOPAP		Implementation of comment
 *              20140327        XSARSES         TRAUTILBIN - core dump fix
 */

#include <sys/file.h>
#include <syslog.h>
#include "ace/Process_Mutex.h"

// Module Include Files
#include "ACS_TRA_common.h"
#include "ACS_TRA_lib.h"
#include "ACS_TRA_AutoflushThreadManager.h"

// Extern declaration
ACE_INT32 		syntax_error = 0;
ACE_INT32 		instance = 0;
ACS_TRA_AutoflushThreadManager autoflush_thread_manager;


static ACE_Shared_Memory_SV Shared_Memory;
static ACE_Process_Mutex * sm_mutex_p = 0; // pointer to process mutex used to synchronize access to "Shared_Memory" object
static bool notify_errors_in_syslog = true;

//static sigset_t sigsettrc;
//static sigset_t osigsettrc;

/*
 * Class Name: ACS_TRA_lib
 * Method Name: maskset
 * Description: Routines to set and reset trace mask flags
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::update_mask, ACS_TRA_lib::resetMAX_CHARS_LOG_FILENAME_mask
 *       in ACS_TRA_lib.cpp file
 */
ACE_INT32 maskset[32] =
{
	0x1, 0x2, 0x4, 0x8,
	0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000,
	0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000
};

/*
 * Class Name: ACS_TRA_lib
 * Method Name: ACS_TRA_lib
 * Description: Class constructor, initialize the shared memory
 * Used: This method is used in the function Main in the ACS_TRA_util.cpp file.
 */
ACS_TRA_lib::ACS_TRA_lib()
{
	#define  		PROT 0666
	key_t   		key = get_trace_key();
	ACE_INT32       shmflg;
	ACE_INT32		size = 0;
	char 			*tmpTraceMem;
	char			On_Off[10];
	char			Memory_Size[20];

	/*
	 * Reset the On_Off and the MemorySize
	 * so do not have wrong characters
	 */
	ACE_OS::memset(On_Off, 0, 10);
	ACE_OS::memset(Memory_Size, 0, 20);

	/*
	 * Open the file that contains the ON_OFF parameter,
	 * if it unsuccessful, the initialization of shared 
	 * memory is aborted. If successful, read the value
	 * from the file Memory Size and initialize the
	 * shared memory with this new value.
	 */
	if (ACE_OS::strcmp(ACS_TRA_common::readTraceStatus(On_Off), TRACESTATUSON) == 0)
	{
		/*
		 * Open the file that contains the Memory Size parameter,
		 * if it unsuccessful, will be used the default value (1000kb).
		 * If successful, read the new value of memory size and then
		 * set the size variable fot initialization of shrae memory.
		 */
		tmpTraceMem = ACS_TRA_common::readMemorySize(Memory_Size);

		if (tmpTraceMem != NULL)
		{
			size = ACE_OS::atoi(tmpTraceMem);
			if (size == 0)  size = SHM_DEFAULT_SIZE;
		}
		else  size = SHM_DEFAULT_SIZE;

		shmflg = PROT;

		// Open the TRA top-level shared memory and initialize the global pointer 'shmp'
		if (get_shared_memory() == -1)
		{
			/*
			 * The 'TRA top-level shared memory' doesn't exist yet; so we must create and initialize it.
			 * Anyway, we must avoid that other processes execute concurrently the same initialization activity;
			 * in order to ensure this, we'll acquire lock on the 'TRA shared memory mutex'
			 */

			// create the TRA shared memory mutex, if not already existing
			if(!sm_mutex_p)
			{
				if ((sm_mutex_p = new (std::nothrow) ACE_Process_Mutex(TRALIB_SHARED_MEMORY_MUTEX_NAME, 0, 0777)) == 0)
				{
					ACE_OS::fprintf(stdout, "Failure to get mutex : %s \n",	sys_errlist[errno]);
					exit(1);
				}
			}

			// try to acquire lock on it
			int acquire_result = ACS_TRA_lib::acquire_shared_memory_mutex();
			if(acquire_result != 0)
				syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message: ACS_TRA_lib::ACS_TRA_lib(): unable to acquire lock on the shared memory mutex (errno == %d) ! \n ", getpid(), syscall(SYS_gettid), errno);

			// create the TRA top-level shared memory
			if (Shared_Memory.open (key, TOP_LEVEL_SHM_SIZE, shmflg | IPC_CREAT) == -1)
			{
				ACE_OS::fprintf(stdout, "Failure to get shared memory : %s errno %d \n", sys_errlist[errno], errno);
				exit(1);
			}

			// initialize the shared memory open over
			init_shared_memory (size);

			// release lock on the TRA shared memory mutex
			if( (acquire_result == 0) && (0 != ACS_TRA_lib::release_shared_memory_mutex()) )
				syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message: ACS_TRA_lib::ACS_TRA_lib(): unable to release lock on the shared memory mutex (errno == %d) ! \n ", getpid(), syscall(SYS_gettid), errno);
		}
	}
	// else the Trace Status is OFF
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: trace_error
 * Description: Display the error on file or stdout
 * Used: This method is used in the following member:
 *       ACS_TRA_trace::do_log, ACS_TRA_trace::install_pid,
 *       ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp file
 */
void ACS_TRA_lib::trace_error(const char *errmsg, ACE_INT32 errnoMsg, bool out_to_console)
{
	static ACE_INT32 reported = 0;
	FILE * console_fd = NULL;

	if(out_to_console && (reported == 0))
	{
		reported = 1;

		/*
		 * If the file is not created it fails and print
		 * an error message and return to trautil console
		 */
		console_fd = ACE_OS::fopen("/dev/console", "a");
		if(console_fd == NULL)
			console_fd = stderr;

		ACE_OS::fprintf(console_fd, "  %d Trace failure: %s \n ", getpid(), errmsg ? errmsg : "(NULL)");

		if (errnoMsg != 0)
			ACE_OS::fprintf(console_fd, "%s\n", sys_errlist[errnoMsg]);

		if(console_fd != stderr)
			ACE_OS::fclose(console_fd);
	}

#ifdef __TRALIB_DEBUG_ENABLED__
	if(::access(TRALIB_DEBUG_ENABLER_FILE, F_OK) == 0)
	{
		// TRALIB debug enabled !
		struct timeval tv;
		time_t nowtime;
		struct tm *nowtm;
		char tmbuf[64], buf[64];
		gettimeofday(&tv, NULL);
		nowtime = tv.tv_sec;
		nowtm = localtime(&nowtime);
		strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);

		// write debug message to sys log file
		syslog(LOG_DEBUG, "%s. PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", buf, getpid(), syscall(SYS_gettid), errmsg ? errmsg : "(NULL)");
	}
#endif
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: update_mask
 * Description: Update trace mask flags in trace elements
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_trace.cpp file
 *       and  ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp
 *       file
 */
void ACS_TRA_lib::update_mask(ACE_INT32 i, relpointer relp, trace_elementp trcep)
{
	if ((trcep->mask & maskset[i]) != maskset[i])
		// Check which trace set is used
		if (traverse_set(relp, trcep))
			trcep->mask |= maskset[i];
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: reset_mask
 * Description: Reset trace mask flags in trace elements
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::delete_task in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::reset_mask(trace_elementp trcep, ACE_INT32 i)
{
	if ((trcep->mask & maskset[i]) == maskset[i])
		trcep->mask ^= maskset[i];
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: traverse_set
 * Description: Return the// NT port PROC -> PRCS trace set used
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::update_mask, ACS_TRA_lib::traverse_set
 *       in ACS_TRA_lib.cpp file
 */
ACE_INT32 ACS_TRA_lib::traverse_set(relpointer relp, trace_elementp trcep)
{
	char *tracename, *classname;

	/* check 'relp' offset before using it. In general, 'relp' can be either the offset of a 'trace_set' struct, or the offset of a 'trace_data' struct.
	 * Note that a 'trace_set' struct is like a 'trace_data' struct, followed by some extra fields. We should distinguish these two cases; however,
	 * for semplicity, we take into account only the 'trace_data' part of 'trace_set' strcut, ignoring the extra fields.
	 */
	if(0 != ACS_TRA_lib::check_trace_set_p(relp, sizeof(struct trace_data), false))
		return 0;  // memory corruption detected !

	trace_setp sp = ABS(trace_setp, relp);

	switch (sp->typ)
	{
		// Check which trace set is used
		case PROC:
			return (trcep->proc == relp);
		case REC:
			return (trcep->record == relp);
		case CLASS:
			if( 0 != ACS_TRA_lib::check_trace_data_p(relp, sizeof(struct trace_data), false) )
				return 0; // memory corruption detected !
			classname = ABS(char *, relp) + sizeof(struct trace_data);
			if( 0 != ACS_TRA_lib::check_trace_data_p(trcep->record, sizeof(struct trace_data) + ACE_OS::strlen(classname), false) )
				return 0;  // the recordname associated to 'trcep->record' cannot be matched against 'classname'
			tracename = ABS(char *, trcep->record) + sizeof(struct trace_data);
			return (ACE_OS::strncmp(tracename, classname, ACE_OS::strlen(classname)) == 0);
		case PLUS:
			return (traverse_set(sp->left, trcep) || traverse_set(sp->right, trcep));
		case MINUS:
			return (traverse_set(sp->left, trcep) && (!traverse_set(sp->right, trcep)));
		case INTERSECTION:
			return (traverse_set(sp->left, trcep) && (traverse_set(sp->right, trcep)));
		default:
			return(0);
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: init_shared_memory
 * Description: Initialize shared memory
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::ACS_TRA_lib in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::init_shared_memory(ACE_INT32 size)
{
#ifdef __TRALIB_DEBUG_ENABLED__
	syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), "called 'init_shared_memory()'");
#endif

	// Initialize global pointer 'shmp'
	shmp = (struct trace_mem *) Shared_Memory.malloc(TOP_LEVEL_SHM_SIZE);

	// check if the shared memory area has been already initialized
	if(shmp->lev == TRACE_LEVEL)
	{
#ifdef __TRALIB_DEBUG_ENABLED__
		syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message: ACS_TRA_lib::init_shared_memory(): shared memory area is already initialized ! \n ", getpid(), syscall(SYS_gettid));
#endif
	}
	else
	{	// perform shared memory initialization
		shmp->free_trace_data = sizeof(struct trace_mem);
		shmp->maxTraceSize = size;
		shmp->free_task_data = TOP_LEVEL_SHM_SIZE;
		ACE_OS::strcpy (shmp->rev, TRACE_REV);
		shmp->lev = TRACE_LEVEL;

		for (int j=0; j<MAX_PIDS; j++)  shmp->pidTab[j].pid = 0;

		for (int j=0; j<MAX_TASK_COUNT; j++)  shmp->tasksTab[j].refcount = -1;
	}
}


/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_shared_memory
 * Description: Attachs the shared memory to the global
 *              pointer shmp. If first time, gets and
 *              initializes the trace shared memory.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::ACS_TRA_lib in ACS_TRA_lib.cpp file,
 *       ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace,cpp
 *       file and Main in ACS_TRA_util.cpp file
 */
ACE_INT32 ACS_TRA_lib::get_shared_memory()
{
	 #define  PROT   0666
	 ACE_INT32       shmid;
	 key_t           key = get_trace_key();
	 ACE_INT32       shmflg;
	 typedef union semun arg;
	 int check_res = -1;

#ifdef	__TRALIB_DEBUG_ENABLED__
	 syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), "called 'get_shared_memory()'");
#endif

	 /*
	  * Create a Shared Memory wrapper
	  * based on System V shared memory
	  */
	 shmflg = ACE_Shared_Memory_SV::ACE_CREATE | PROT;

	 // Try to open the shared memory
	 if (Shared_Memory.open(key, 0, shmflg) == -1)
	 {
#ifdef	__TRALIB_DEBUG_ENABLED__
		 syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : get_shared_memory(). Shared_Memory.open() returned -1. Errno = %d \n ", getpid(), syscall(SYS_gettid), errno);
#endif
		 return (-1);
	 }

	 // If the shared memory is open retrieve the id
	 shmid = Shared_Memory.get_id();

	 // Try to allocate the shared memory
	 if ((ACE_INT64)(shmp = (struct trace_mem *) Shared_Memory.malloc ()) == -1)
	 {
#ifdef	__TRALIB_DEBUG_ENABLED__
		 syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), "get_shared_memory(). Shared_Memory.malloc () returned -1");
#endif
		 return (-1);
	 }

	 // Try to allocate the shared memory mutex
	 if ((sm_mutex_p = new (std::nothrow) ACE_Process_Mutex(TRALIB_SHARED_MEMORY_MUTEX_NAME, 0, 0777)) == 0)
	 {
#ifdef	__TRALIB_DEBUG_ENABLED__
		 syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), "sm_mutex_p == NULL");
#endif
		 return -1;
	 }

	 // Check existence, owner and permissions of the System V Semaphore used to implement the TRALIB shared memory mutex.
	 // If owner or permissions are wrong, try to adjust them.
	 // PAY ATTENTION: this check assumes that ACE toolkit is configured to use ACE_SV_Semaphore_Complex as underlying mechanism to
	 // implement 'ACE_Process_Mutex' class
	 check_res = ACS_TRA_lib::check_and_adjust_shared_memory_system_V_sem();
#ifdef	__TRALIB_DEBUG_ENABLED__
	 if(check_res != 0)
		 syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : check_and_adjust_shared_memory_system_V_sem() --> check_res == %d, errno == %d\n ", getpid(), syscall(SYS_gettid), check_res, errno);
#endif

	 return (0);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_semaphore
 * Description: Return the value of a named or unnamed semaphore.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::display_log in ACS_TRA_lib.cpp file
 *       and ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp
 *       file
 */
#if 0
static sigset_t sigsettrc;
static sigset_t osigsettrc;
ACE_INT32 ACS_TRA_lib::get_semaphore()
{
	ACE_INT32	ret;

	/*
	 * Capture the key events and
	 * prevents that trautil interrupt
	 */
	if (ACE_OS::sigdelset (&sigsettrc, SIGTERM) < 0)  return (-1);

	if (ACE_OS::sigdelset (&sigsettrc, SIGBUS) < 0)  return (-1);

	if (ACE_OS::sigdelset (&sigsettrc, SIGSEGV) < 0)  return (-1);

	if (ACE_OS::sigprocmask(SIG_BLOCK, &sigsettrc,&osigsettrc) < 0)  return (-1);

	// Acquire the ACE semaphore
	ret = smid->acquire();

	/*
	 * Define a new signal mask to manage
	 * the extern program interrupt
	 * (Eg. kill process, segmentation fault, etc...)
	 */
	if (ACE_OS::sigprocmask (SIG_SETMASK, &osigsettrc, NULL) < 0)  return (-1);

	return (ret);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: release_semaphore
 * Description: Release the semaphore.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::display_log in ACS_TRA_lib.cpp file
 */
ACE_INT32 ACS_TRA_lib::release_semaphore()
{
	ACE_INT32	ret;

	// Release the ACE semaphore
	ret = smid->release();

	/*
	 * Define a new signal mask to manage
	 * the extern program interrupt
	 * (Eg. kill process, segmentation fault, ect...)
	 */
	if (ACE_OS::sigprocmask (SIG_SETMASK, &osigsettrc, NULL) < 0)  return (-1);

	return (ret);
}
#endif


/*
 * Class Name: ACS_TRA_lib
 * Method Name: free_temp
 * Description: Link in free temp list of shared memory.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::delete_set, ACS_TRA_lib::create_log,
 *       ACS_TRA_lib::create_task and ACS_TRA_lib::delete_task
 *       in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::free_temp(relpointer relp)
{
	struct list	*p;

	// If relp is valid link it in free temp list
	if (relp != 0)
	{
		p = ABS(struct list *, relp);
		p->next = shmp->free_temp_head;
		shmp->free_temp_head = relp;
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: process_row
 * Description: Check if is possible to print only
 * 				on stdout output or both stdout and file
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::display_log in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::process_row(char* row, FILE* file, ACE_INT32 autoflush)
{
	/*
	 * If is present the AUTOFLUDH and SCREEN parameters
	 * the trace is printed only on the standard output.
	 * If is present only the filename parameter the trace
	 * is printed both on standard output and on a file when
	 * the flush command is given. If is present both AUTOFLUSH
	 * and filename parameters the trace are printed only on a file.
	 */
	if(autoflush > 0)
	{
			if (file != NULL)  fprintf(file, "%s", row);
			else			   fprintf(stdout, "%s", row);
	}
	else {
			if (file != NULL)  fprintf(file, "%s", row);
			fprintf(stdout, "%s", row);
	}
}

void  writeInFile (const char *info)
{

	FILE  *file_param = ACE_OS::fopen("/root/Display.log", "a");

	if (file_param != NULL)
	{
			ACE_OS::fputs (info, file_param);
			ACE_OS::fflush (file_param);
			ACE_OS::fclose (file_param);
	}
}


void  stampa_tasks (const char *info)
{

		ACE_OS::fprintf (stdout, "%s\n", info);

		trace_taskp  p;

		for (int j=0; j<MAX_TASK_COUNT; j++)
		{
				if (shmp->tasksTab[j].refcount != -1)
				{
					ACE_OS::fprintf(stdout, "J = %d :  ", j);
					p = &shmp->tasksTab[j];
					ACE_OS::fprintf(stdout, "Name = %s  ", p->name);
					ACE_OS::fprintf(stdout, "User = %s  ", p->user);
					ACE_OS::fprintf(stdout, "RefC = %d\n", p->refcount);
				}
		}
}

#if 0
ACE_INT64  skipToNextRecord (trace_taskp taskp, ACE_INT64 bp)
{

		logp lp = &taskp->logBuff;

		while (bp < ((ACE_INT64) lp->shmp + taskp->shm_size - 5))
		{
				if (*(char *) bp   == 'w'  &&  *(char *) bp+1 == 'x'  &&
					*(char *) bp+2 == 'y'  &&  *(char *) bp+3 == 'z')
				{
						return bp;
				}
				bp ++;
		}
		if (lp->circular == 0)  lp->currpos = 0;
		lp->circular = 0;
		return (ACE_INT64) lp->shmp;
}
#endif

ACE_INT64  skipToNextRecord (trace_taskp taskp, ACE_INT64 bp, relpointer real_task_shmp = 0)
{

		logp lp = &taskp->logBuff;

		relpointer task_shmp = (real_task_shmp ? real_task_shmp : lp->shmp);

		//while (bp < ((ACE_INT64) lp->shmp + taskp->shm_size - 5))
		while (bp < ((ACE_INT64) task_shmp + taskp->shm_size - 5))
		{
				if (*(char *) bp   == 'w'  &&  *(char *) bp+1 == 'x'  &&
					*(char *) bp+2 == 'y'  &&  *(char *) bp+3 == 'z')
				{
						return bp;
				}
				bp ++;
		}
		if (lp->circular == 0)  lp->currpos = 0;
		lp->circular = 0;
		//return (ACE_INT64) lp->shmp;
		return (ACE_INT64) task_shmp;
}


/*
 * Class Name: ACS_TRA_lib
 * Method Name: display_log
 * Description: Display the contents of shared memory
 *              to the stdout and then clean the buffer
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::delete_task, ACS_TRA_lib::autoflush
 *       in ACS_TRA_lib.cpp file and in main of ACS_TRA_util.cpp file
 */
void  ACS_TRA_lib::display_log (ACE_INT32 fstCall, char delimiter, trace_taskp taskp, int dont_acquire_mutex, void * task_shmp)
{
	#define MAX_STRING_SIZE 5000;
	ACE_INT32        n, i, j;
	char	         c, c1;
	char	         format[50];
	ACE_INT32        endstring;
	ACE_INT64        bp, recstart;
	struct tm       *own_time;
	ACE_INT32		 nopar;
	ACE_INT32 		 emptylog = 0;
	char 			 my_row[255];
	FILE 			*file_desc;

	n = 0;
	file_desc = NULL;

	if (taskp->filename[0] != 0)
			file_desc = ACE_OS::fopen (taskp->filename, "a");

	ACE_OS::memset(my_row, 0, 255);

	if (fstCall && !dont_acquire_mutex && (acquire_shared_memory_mutex() == -1))
	{
			char buf[256] = {0};
			snprintf(buf, 256, "ERROR: ACS_TRA_lib::display_log(): Failure to get shared memory mutex !");
			ACE_OS::fprintf(stdout, "%s\n ", buf);
			ACS_TRA_lib::trace_error(buf, 0);
			return;
	}

	logp lp = &taskp->logBuff;

	// take address of attached shared memory associated to the task
	relpointer real_task_shmp = (relpointer) (task_shmp ? task_shmp: attach_task_shm(taskp));
	if(!real_task_shmp) return;	// cannot continue without a valid pointer to the task shared memory. Exit function

	if (lp->startpos == lp->currpos)  emptylog = 1;

	ACE_OS::sprintf(my_row, "**********************%s BEGIN**********************\n", taskp->name);
	process_row(my_row, file_desc, taskp->autoflush);

	if (file_desc != NULL)
			file_desc = checkFileSize (file_desc, taskp->filename, taskp->size);

	if (emptylog  &&  fstCall)
	{
		ACE_OS::sprintf(my_row, "No new trace data in the task trace log\n");
		process_row(my_row, file_desc, taskp->autoflush);

		if (file_desc != NULL)
				file_desc = checkFileSize(file_desc, taskp->filename, taskp->size);
	}

/*
	if (fstCall && !dont_acquire_sem)	// If is the first call
	{
			if (release_semaphore() == -1)
			{
					ACE_OS::fprintf(stdout, "Failure to release semaphore: %s\n ", sys_errlist[errno]);
					exit(1);
			}
	}
*/
	if (! emptylog)			// get log records from startpos to currpos
	{
			char     buffer[250];
			bool     printFirst = false;
			int      maxRecs = (int) (taskp->shm_size / 102);

			bp = (ACE_INT64) (real_task_shmp + lp->startpos);

			while (1)
			{
				if (lp->circular == 0  &&  printFirst == true)
				{
						if (bp >= (ACE_INT64) (real_task_shmp + lp->currpos))  break;
				}
				printFirst = true;

				if (maxRecs <= 0)  break;		//  Further check
				maxRecs --;

				if (bp + 512 >= (ACE_INT64)(real_task_shmp + (relpointer)taskp->shm_size))  bp = real_task_shmp;
//char  buff[50];
//buff[0] = *(char *) bp;    buff[1] = *(char *) bp+1;
//buff[2] = *(char *) bp+2;  buff[3] = *(char *) bp+3;  buff[4] = 0;
//sprintf (buffer, "START TAG = %s\n", buff);
//writeInFile (buffer);

				recstart = bp;
				bp       = bp + 4;			//  skip the start Tag

				own_time = localtime((ACE_INT64 *) bp);			//  Print the time stamp
				if (own_time == NULL)
				{
						ACE_OS::sprintf(my_row, "00 00:00:00 ");
				}
				else  ACE_OS::sprintf(my_row, "%02d %02d:%02d:%02d ",
											  own_time->tm_mday, own_time->tm_hour, own_time->tm_min, own_time->tm_sec);
				ADVANCE(sizeof(ACE_INT64));

				ACE_OS::sprintf(buffer, "%06lu ", (*(ACE_UINT64 *) bp));
				strcat (my_row, buffer);
				ADVANCE(sizeof(ACE_INT64));

				ACE_OS::sprintf(buffer, "%06lu ", (*(ACE_INT64 *) bp));		// Print the pid
				strcat (my_row, buffer);
				ADVANCE(sizeof(ACE_INT64));

				j = 0;
				while (*(char *) bp != '\0'  &&  j < 50)			//  process id
				{
						buffer[j] = *(char *) bp;
						ADVANCE(sizeof(char));
						j ++;
				}
				if (j == 50)
				{
						bp = skipToNextRecord (taskp, bp, real_task_shmp);
						continue;
				}
				buffer[j] = '\0';
				strcat (my_row, buffer);
				ADVANCE(sizeof(char));

				j = 0;
				while (*(char *) bp != '\0'  &&  j < 50)			//  record id
				{
						buffer[j] = *(char *) bp;
						ADVANCE(sizeof(char));
						j ++;
				}
				if (j == 50)
				{
						bp = skipToNextRecord (taskp, bp, real_task_shmp);
						continue;
				}
				buffer[j] = '\0';
				strcat (my_row, buffer);
				ADVANCE(sizeof(char));

				j = 0;
				while (*(char *) bp != '\0'  &&  j < 45)			//  format
				{
						format[j] = *(char *) bp;
						ADVANCE(sizeof(char));
						j ++;
				}
				if (j == 45)
				{
						bp = skipToNextRecord (taskp, bp, real_task_shmp);
						continue;
				}
				format[j] = '\0';
				ADVANCE(sizeof(char));

				strcat (my_row, " ");
				process_row(my_row, file_desc, taskp->autoflush);

				i = 0;  nopar = 0;
				while (format[i] != '\0')		// get and translate parameters
				{
						while (isspace(format[i]))  i ++;

						c = format[i];
						i ++;  n = 0;

						if (isdigit(format[i]))  n = ACE_OS::atoi(&format[i]) - 1;

						while (isdigit(format[i]))  i ++;

						endstring = 0;

						if (nopar != 0)
						{
								ACE_OS::sprintf(my_row,"%c", delimiter);
								process_row(my_row, file_desc, taskp->autoflush);
						}

						nopar ++;

						if  (c == 'C'  &&  n == 0)  n = MAX_STRING_SIZE;

						for (j=0; j<=n; j++)
						{
								if (bp > recstart + 512)
								{
										bp = skipToNextRecord (taskp, bp, real_task_shmp);
										ACE_OS::sprintf(my_row, "\n");
										process_row(my_row, file_desc, taskp->autoflush);
										i = strlen(format);
										break;
								}

								switch (c)
								{
									case 'b':  ACE_OS::sprintf(my_row, "%d", (*(char *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(char));
											   break;

									case 'B':  ACE_OS::sprintf(my_row, "%u", (*(unsigned char *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(char));
											   break;

									case 's':  ALIGN(bp, sizeof(ACE_INT16));		// Align the bp with the type passed
											   ACE_OS::sprintf(my_row, "%d", (*(ACE_INT16*) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT16));
											   break;

									case 'S':  ALIGN(bp, sizeof(ACE_INT16));		// Align the bp with the type passed
											   ACE_OS::sprintf(my_row, "%u", (*(ACE_UINT16 *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT16));
											   break;

									case 'l':  ALIGN(bp, sizeof(ACE_INT64));		// Align the bp with the type passed
									   		   ACE_OS::sprintf(my_row, "%lu", (*(ACE_INT64 *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT64));
											   break;

									case 'L':  ALIGN(bp, sizeof(ACE_INT64));
									   		   ACE_OS::sprintf(my_row, "%lu", (*(ACE_UINT64 *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT64));
											   break;

									case 'i':  ALIGN(bp, sizeof(ACE_INT32));
											   ACE_OS::sprintf(my_row, "%d", (*(ACE_INT32 *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT32));
											   break;

									case 'I':  ALIGN(bp, sizeof(ACE_INT32));
											   ACE_OS::sprintf(my_row, "%u", (*(ACE_UINT32 *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(ACE_INT32));
											   break;

									case 'c':  ACE_OS::sprintf(my_row, "%c", (*(char *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(char));
											   break;

									case 'p':  ALIGN(bp, sizeof(relpointer));
//											   ACE_OS::sprintf(my_row, "0x%x", (*(relpointer *) bp));
											   ACE_OS::sprintf(my_row, "0x%x", (*(unsigned int *) bp));
											   process_row(my_row, file_desc, taskp->autoflush);
											   ADVANCE(sizeof(relpointer));
											   break;

									case 'C':  if (endstring != 1)
											   {
													if ((c1 = (*(char *) bp)) == '\0')  endstring = 1;

													ACE_OS::sprintf(my_row, "%c", (*(char *) bp));
													process_row(my_row, file_desc, taskp->autoflush);
													ADVANCE(sizeof(char));
											   }
											   break;
								}

								if (c != 'C'  &&  j != n)
								{
										ACE_OS::sprintf(my_row," ");
										process_row(my_row, file_desc, taskp->autoflush);
								}
						}

						while (isspace(format[i]))  i ++;
						if (format[i] == ',')  i ++;
				}

				ACE_OS::sprintf(my_row, "\n");
				process_row(my_row, file_desc, taskp->autoflush);

				if (file_desc != NULL)
						file_desc = checkFileSize(file_desc, taskp->filename, taskp->size);

				ACE_OS::fflush(stdout);
				if (file_desc != NULL)  ACE_OS::fflush(file_desc);

				bp = skipToNextRecord (taskp, bp, real_task_shmp);
			}
	}

	lp->startpos = 0;
	lp->currpos  = 0;
	lp->circular = 0;

	ACE_OS::sprintf(my_row, "**********************%s END**********************\n", taskp->name);
	process_row(my_row, file_desc, taskp->autoflush);

	// if we have attached the shared memory segment of the task in this method, detach it
	if(!shmp) shmdt ((void *) real_task_shmp);

	if (file_desc != NULL)  ACE_OS::fclose(file_desc);

	if (fstCall && !dont_acquire_mutex)	// If is the first call
	{
		if (ACS_TRA_lib::release_shared_memory_mutex() == -1)
		{
			char buf[256] = {0};
			snprintf(buf, 1024, "ERROR: ACS_TRA_lib::display_log(): Failure to release shared memory mutex !");
			ACE_OS::fprintf(stdout, "%s\n", buf);
			ACS_TRA_lib::trace_error(buf, 0);
			return;
		}
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: delete_set
 * Description: Delete the trace set selected from the trace task.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::delete_set, ACS_TRA_lib::create_basic_set,
 *       ACS_TRA_lib::create_task and ACS_TRA_lib::delete_task
 *       in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::delete_set(relpointer setrel)
{
	trace_setp      setp;

	setp = ABS(trace_setp, setrel);

	switch (setp->typ)
	{
		case PROC: // NT port PROC-> PRCS
		case REC:
		case CLASS:
			break;
		case PLUS:
		case MINUS:
			delete_set(setp->left);
			delete_set(setp->right);
			free_temp(setrel);
		default:
			;
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: hashvl
 * Description: Return value of the hashtable.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::get_trace_data_p in ACS_TRA_lib.cpp file
 */
ACE_UINT32 ACS_TRA_lib::hashvl(char *s, ACE_INT32 l)
{
	ACE_INT64 a = 0, b = 0;

	while (l--)
	{
		a += *s++;
		b += a;
	}

	return (((ACE_UINT64) ((b << 8) + a)) % HASHSIZE);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_trace_data_p
 * Description: Return relative address to permanent data.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set in ACS_TRA_lib.cpp file
 *       and ACS_TRA_trace::ACS_TRA_event in ACS_TRA_trace.cpp file
 */
relpointer ACS_TRA_lib::get_trace_data_p(char *key, enum trace_data_type typ, ACE_INT32 /*command*/)
{
	trace_datap     np1, np2;
	relpointer      np;
	trace_elementp  trcep;
	ACE_INT64       keysize;
	ACE_INT64       datasize;
	ACE_UINT64		hashval;
	char			*datap;
	int skip_key_cmp;

#ifdef	__TRALIB_DEBUG_ENABLED__
	syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : called 'get_trace_data_p()' - typ == %d, command == %d \n ", getpid(), syscall(SYS_gettid), typ, command);
#endif

	if(!key)
	{
#ifdef	__TRALIB_DEBUG_ENABLED__
		syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : 'get_trace_data_p()' called with parameter 'key == 0' - typ == %d, command == %d \n ", getpid(), syscall(SYS_gettid), typ, command);
#endif
		return 0;
	}

	if (typ == TRCE)
	{
		keysize = 3 * sizeof(relpointer);
		datasize = sizeof(struct trace_element) + sizeof(struct trace_data);
	}
	else
	{
		keysize = ACE_OS::strlen(key) + 1;
		datasize = keysize + sizeof(struct trace_data);
	}

	// lookup hashtable
	hashval = hashvl(key, keysize);
	np = shmp->hashtab[hashval];
	np1 = 0;
	skip_key_cmp = 0;
	while (np != 0)
	{
		if((skip_key_cmp = ACS_TRA_lib::check_trace_data_p(np, datasize, true)) != 0)
			if(ACS_TRA_lib::check_trace_data_p(np, sizeof(trace_data), false) != 0)
				break;   // np is not a valid offset. Top level shared memory area seems to be corrupted !

		np1 = ABS(trace_datap, np);
		if (np1 != NULL)
		{
			datap = (char *)np1 + sizeof(struct trace_data);
			if (datap != NULL && key != NULL && !skip_key_cmp)
			{
				if ((typ == np1->typ) && (ACE_OS::memcmp(key, datap, keysize) == 0))
				{
				//	syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : get_trace_data_p() >>> [ typ == %d ] \n ", getpid(), syscall(SYS_gettid), typ);
					return (np);
				}
			}

			np = np1->next;
		}
	}
	// not found, allocate trace memory

	np = shmp->free_trace_data;
	shmp->free_trace_data += datasize;
	ALIGN(shmp->free_trace_data, 4);

	/*
	 * if not enough memory left, delete tasks and reset
	 * free_task_data or if in command mode, return NULL
	 */
	if (shmp->free_trace_data >= shmp->free_task_data)
	{
		// we cannot proceed further ! Restore previous value of 'free_trace_data' pointer and return 0
		shmp->free_trace_data = np;
#ifdef	__TRALIB_DEBUG_ENABLED__
		if(typ != TRCE)
			syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message : no space left in shared memory to allocate 'trace_data': [ typ == '%d', key == '%s' ]  \n ", getpid(), syscall(SYS_gettid), typ, key);
		else
		{	/* typ == TRCE */
			relpointer tmp[3];
			::memcpy(tmp, key, sizeof(tmp));  // the input argument 'key' should be an array of 3 relpointer !
			syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message : no space left in shared memory to allocate 'trace_data': [ typ == '%d', PROC ADDR == %ld, REC ADDR == %ld, FORMAT ADDR == %ld]  \n ", getpid(), syscall(SYS_gettid), typ, tmp[0], tmp[1], tmp[2]);
		}
#endif
		// if (command)  return (0);

		// ACS_TRA_lib::delete_task(NULL,0, ',');

		return 0;
	}

	// copy data
	np2 = ABS(trace_datap, np);
	if (np2 != 0)
	{
		np2->next = 0;
		np2->typ = typ;
		datap= (char *)np2 + sizeof(struct trace_data);
		ACE_OS::memcpy(datap, key, keysize);
	}

	// Link trace element in trace element list
	if (typ == TRCE)
	{
		trcep = ABS(trace_elementp, np + sizeof(struct trace_data));
		trcep->mask = 0;
		trcep->next = shmp->trce_head;
		shmp->trce_head = np + sizeof(struct trace_data);
	}

	// and link it in the hash table
	if ((shmp->hashtab[hashval] == 0) || (np1 == 0))
		shmp->hashtab[hashval] = np;  // Note that '(shmp->hashtab[hashval] != 0) && (np1 == 0)' is symptom of shared memory corruption !!
	else
		np1->next = np;

	return (np);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_task_data_p
 * Description: Return absolute address to permanent data.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_log, ACS_TRA_lib::create_op_set
 *       and ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
relpointer ACS_TRA_lib::get_task_data_p()
{
	relpointer      prel;
	struct list     *p;

	if ((prel = shmp->free_temp_head) != 0)
	{
		p = ABS(struct list *, prel);
		shmp->free_temp_head = p->next;
	}
	else
	{
		prel = shmp->free_task_data - TEMPSIZE;

		// check if enough memory left
		if (prel <= shmp->free_trace_data)  return (0);

		shmp->free_task_data = prel;
		p = ABS(struct list *, prel);
	}

	p->next = 0;
	return (prel);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_task
 * Description: Return the task name in the shared memory.
 * Used: This method is used in the following member:
 *       main in ACS_TRA_util.cpp file
 */
trace_taskp ACS_TRA_lib::get_task(char *name)
{
	trace_taskp     taskp;

	for (int  idx=0; idx < MAX_TASK_COUNT; idx++)
	{
		if (shmp->tasksTab[idx].refcount != -1)
		{
			taskp = &shmp->tasksTab[idx];

			if (ACE_OS::strncmp (taskp->name, name, MAX_TASK_NAME) == 0  &&
				ACE_OS::strncmp (taskp->user, getenv("USER"), ACE_OS::strlen(getenv("USER"))) == 0)  return (taskp);
		}
	}

	return (NULL);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_filename
 * Description: Return the file name in the shared memory.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
trace_taskp ACS_TRA_lib::get_filename(char *filename)
{

	trace_taskp     taskp;

	for (int idx=0; idx < MAX_TASK_COUNT; idx++)
	{
		if (shmp->tasksTab[idx].refcount != -1)
		{
			taskp = &shmp->tasksTab[idx];

			if (ACE_OS::strncmp (taskp->filename, filename, ACE_OS::strlen(filename)) == 0  &&
				ACE_OS::strncmp (taskp->user, getenv("USER"), ACE_OS::strlen(getenv("USER"))) == 0)  return (taskp);
		}
	}

	return (NULL);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: create_log
 * Description: Create the log in the shared memory when
 * 				the task is created.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
int  ACS_TRA_lib::create_log (trace_taskp taskp, ACE_INT32 n, ACE_INT32 pos, ACE_INT32 pid)
{

	int  memAllocated = n;

	for (int j=0; j<MAX_TASK_COUNT; j++)
	{
			if (shmp->tasksTab[j].refcount != -1)
			{
					memAllocated += shmp->tasksTab[j].logBuff.nobuf;
			}
	}
	if (memAllocated > shmp->maxTraceSize)  return (1);

	taskp->refcount = (int) pos;
	taskp->logBuff.nobuf = n;
	taskp->logBuff.circular = 0;
	taskp->shm_size = n * 1024 * 1.20;

	taskp->shm_id = shmget ((key_t)(55500 + pos), (size_t)taskp->shm_size, IPC_CREAT | 0666);
	if (taskp->shm_id == -1)
	{
			ACE_OS::fprintf (stdout, "\nERROR with the shmget !!!\n\n");
			return (1);
	}

	taskp->logBuff.shmp = (relpointer) shmat (taskp->shm_id, NULL, 0);
	if (taskp->logBuff.shmp == -1)
	{
			ACE_OS::fprintf (stdout, "\nERROR with the shmat !!!\n\n");
			return (1);
	}

//	shmdt ((void *)taskp->logBuff.shmp);

	if (instance != 0)
	{
			taskp->logBuff.typ = PID;
			taskp->logBuff.pid = pid;
	}
	else {
			taskp->logBuff.typ = LOG;
			taskp->logBuff.pid = 0;
	}

	taskp->logBuff.startpos = 0;
	taskp->logBuff.currpos  = 0;

	return  0;
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: create_basic_set
 * Description: Check if the inserted parameters (P, C, R, I)
 *              to create a trace set are correct.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_trace_set in ACS_TRA_lib.cpp file
 */
relpointer ACS_TRA_lib::create_basic_set()
{
	char        *set, typ;
	relpointer  ret;

	/*
	 * Check if there is a missing chars
	 * in the command to create trace set
	 */
	if (ACS_TRA_common::get_char() == '(')
	{
			ret = create_trace_set ();
			if (ACS_TRA_common::get_char() != ')')
			{
					syntax_error = 1;
					ACE_OS::fprintf (stdout, "Missing ')' \n");
					delete_set(ret);
					return (0);
			}

			return (ret);
	}
	else  ACS_TRA_common::unget_char();

	typ = ACS_TRA_common::get_char();

	if ((*get_cmd_chars() == ' ') || (*get_cmd_chars() == '\t'))
	{
		if ((set = ACS_TRA_common::get_token()) != NULL)
		{
		/*
			// cast below to remove Visual C++ warning (int)
			if  ((shmp->free_task_data - shmp->free_trace_data) < (ACE_INT32)(strlen(set) + 1 + sizeof(struct trace_data)))
				return (0);
		*/
			/*
			 * Check which is the trace set typed in
			 * the create task command, from the
			 * following: (P, C, R, I)
			 */
			switch (typ)
			{
				case 'P':
					if(0 == (ret = get_trace_data_p(set, PROC, 1)))
					{
						syntax_error = 1;
						ACE_OS::fprintf(stdout, "no space left \n");
					}
					return ret;
				case 'C':
					if(0 == (ret = get_trace_data_p(set, CLASS, 1)))
					{
						syntax_error = 1;
						ACE_OS::fprintf(stdout, "no space left \n");
					}
					return ret;
				case 'R':
					if(0 == (ret = get_trace_data_p(set, REC, 1)))
					{
						syntax_error = 1;
						ACE_OS::fprintf(stdout, "no space left \n");
					}
					return ret;
				case 'I':
					if (isdigit(set[0]))  instance = ACE_OS::atoi(set);
					else				  break;

					ret = search_pid(instance);

					if (ret == 0)
					{
							ACE_OS::fprintf(stdout, "not active pid \n");
							syntax_error = 1;
							return (0);
					}
					return (ret);

				default:
					ACS_TRA_common::unget_char();
			}
		}
		else {
			/*
			 * When an error in trace set syntax occurs,
			 * the message with correct trace set to digit
			 * are printed in the trautil console
			 */
			syntax_error = 1;
			ACE_OS::fprintf(stdout, "error in trace set format\n");
			ACE_OS::fprintf(stdout, "basic set syntax:\n");
			ACE_OS::fprintf(stdout, " P process_name | C class_name | R trace_record_name | I process_pid\n");
			ACE_OS::fprintf(stdout, "trace set syntax:\n");
			ACE_OS::fprintf(stdout, " basic_set +|-|*  basic_set\n");
			return (0);
		}
	}

	/*
	 * When an error in trace set syntax occurs,
	 * the message with correct trace set to digit
	 * are printed in the trautil console
	 */
	syntax_error = 1;
	ACE_OS::fprintf(stdout, "error in trace set format\n");
	ACE_OS::fprintf(stdout, "basic set syntax:\n");
	ACE_OS::fprintf(stdout, " P process_name | C class_name | R trace_record_name | I process_pid\n");
	ACE_OS::fprintf(stdout, "trace set syntax:\n");
	ACE_OS::fprintf(stdout, " basic_set +|-|*  basic_set\n");
	return (0);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: create_op_set
 * Description: Check if the inserted parameters (+, -, *)
 *              to create a trace set are correct.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_trace_set in ACS_TRA_lib.cpp file
 */
relpointer ACS_TRA_lib::create_op_set(char op, relpointer left, relpointer right)
{
	relpointer      relp;
	trace_setp      setp;

	if ((relp = get_task_data_p()) != 0)
	{
		/*
		 * Check which is the trace set operand to create
		 * typed in command C task_name (+, -, *)
		 */
		setp = ABS(trace_setp, relp);

		switch (op)
		{
			case '+' :
				setp->typ = PLUS;
				break;
			case '-' :
				setp->typ = MINUS;
				break;
			case '*' :
				setp->typ = INTERSECTION;
				break;
		}
		setp->left = left;
		setp->right = right;
	}

	return (relp);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: create_trace_set
 * Description: Create the trace set (P, C, R, I, +, -, *)
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set, ACS_TRA_lib::create_task
 *       in ACS_TRA_lib.cpp file
 */
relpointer ACS_TRA_lib::create_trace_set()
{
	char           	op;
	relpointer      left, right;

	instance = 0;

	left = ACS_TRA_lib::create_basic_set();

	while (left != 0)
	{
		// Check if in the trace set there is an operand (+ , -, *)
		op = ACS_TRA_common::get_char();

		if ((op == '+') || (op == '-') || (op == '*'))
		{
			/*
			 * Check if the trace set to create is also an P, C, R or I,
			 * if not create a trace set only with operand
			 */
			if ((right = ACS_TRA_lib::create_basic_set()) == 0)
				return(0);

			left = ACS_TRA_lib::create_op_set(op, left, right);
		}
		else
		{
			ACS_TRA_common::unget_char();
			break;
		}
	}

	return (left);
}


void * ACS_TRA_lib::attach_task_shm(trace_taskp taskp)
{
	if(!taskp || (taskp->refcount < 0))
	{
		ACE_OS::fprintf(stdout, "ERROR: '%s()': Invalid trace task pointer received !\n", __PRETTY_FUNCTION__);
		return 0;
	}

	// get shared memory identifier referencing the task shared memory
	int shmid = shmget ((key_t)(55500 + taskp->refcount), (size_t)taskp->shm_size, 0666);
	if (shmid == -1)
	{
		ACE_OS::fprintf(stdout, "ERROR: '%s()': Failure getting reference to shared memory of the task '%s'\n", __PRETTY_FUNCTION__, taskp->name);
		return 0;
	}

	// attach task shared memory in our process virtual address space
	void * new_shmp = shmat (shmid, NULL, 0);
	if (new_shmp == (void *) -1)
	{
		ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_lib::display_log(): Failure attaching shared memory of the task '%s'\n", taskp->name);
		return 0;
	}

	return new_shmp;
}


/*
 * Class Name: ACS_TRA_lib
 * Method Name: create_task
 * Description: Create the task (command C in the trautil console).
 *              Check if all inserted (task_name, trace_set, LOG,
 *              number_of_buffer, log_filename, AUTOFLUSH)
 *              parameters are correct.
 * Used: This method is used in the following member:
 *       main in ACS_TRA_util.cpp file
 */
relpointer  ACS_TRA_lib::create_task(char *name, char delimiter)
{
	int      			idx;
	trace_taskp     	taskp, tmp_taskp;
	ACE_INT32       	n;
	trace_elementp  	trcep = 0;
	char 			    *filter, *buffpar;
	char            	*logfilename;
	ACE_INT32			autoflush;
	ACE_INT32			size;
	char           		tfilename[MAX_CHARS_LOG_FILENAME];
	FILE * 				output_file;
	// ACE_thread_t     	thread_id;
	//char 				*tmpfile;
	ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE  autoflush_thread_handle = 0;

	syntax_error = 0;
	output_file = NULL;

	for (idx = 0;  idx < MAX_TASK_COUNT;  idx ++)
			if (shmp->tasksTab[idx].refcount == -1)  break;

	if (idx >= MAX_TASK_COUNT)  return 0;

	taskp = &shmp->tasksTab[idx];

	ACE_OS::strncpy(taskp->name, name, ACE_OS::strlen(name));
	taskp->name[ACE_OS::strlen(name)] = '\0';

	if ((taskp->set = ACS_TRA_lib::create_trace_set()) == 0)
	{
			syntax_error = 1;
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}
	/*
	 * Check if the LOG string exist in typed command C,
	 * if it is not exist print an error message and
	 * return to trautil console
	 */
	if ((filter = ACS_TRA_common::get_token()) == NULL)
	{
			ACE_OS::fprintf(stdout, StringUsage);
			syntax_error = 1;
			delete_set(taskp->set);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}
	if (ACE_OS::strcmp(filter, "LOG"))
	{
			ACE_OS::fprintf(stdout, StringUsage);
			syntax_error = 1;
			delete_set(taskp->set);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}
	/*
	 * Fetch and save the value of parameter LOG,
	 * if it is not typed in command C, print an
	 * error message and return to trautil console
	 */
	if ((buffpar = ACS_TRA_common::get_token()) == NULL)
	{
			syntax_error = 1;
			ACE_OS::fprintf(stdout, StringUsage);
			delete_set(taskp->set);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}
	/*
	 * Check if the value of parameter LOG is
	 * only a digit, if not, print an error
	 * message and return to trautil console
	*/
	if (ACS_TRA_common::isDigit(buffpar) == 1)
	{
			ACE_OS::fprintf(stdout, StringUsage);
			syntax_error = 1;
			delete_set(taskp->set);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}
	/*
	 * Check if the value of parameter LOG is
	 * less than 0, if yes, print an error
	 * message and return to trautil console
	 */
	if ((n = ACE_OS::atoi(buffpar)) <= 0)
	{
			ACE_OS::fprintf(stdout, StringUsage);
			syntax_error = 1;
			delete_set(taskp->set);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}

	taskp->filename[0] = '\0';
	ACE_OS::memset(tfilename, 0, MAX_CHARS_LOG_FILENAME);
	taskp->autoflush = autoflush = 0;     //  HO92981
	taskp->size = size = 0;               //  HO92981
	taskp->video = 0;                     //  HO92981

	/*
	 * Fetch the AUTOFLUSH parameter.
	 * Check if typed the string AUTOFLUSH in
	 * command C, if exist, fetch the value
	 * and store it in the autoflush trace element
	 */
	filter = ACS_TRA_common::get_token();

	if (filter != NULL)
	{
		// Check if the string AUTOFLUSH is typed in command
		if (ACE_OS::strcmp(filter, "AUTOFLUSH") == 0)
		{
			/*
			 * Fetch the value of the autoflush parameter,
			 * and check if it is a valid value
			 */
			filter = ACS_TRA_common::get_token();

			if (filter != NULL)
			{
				/*
				 * Check if the value of parameter AUTOFLUSH
				 * is only a digit, if not, print an error
				 * message and return to trautil console
				*/
				if (ACS_TRA_common::isDigit(filter) == 1)
				{
						ACE_OS::fprintf(stdout, StringUsage);
						syntax_error = 1;
						delete_set(taskp->set);
						shmp->tasksTab[idx].refcount = -1;
						return (1);
				}

				autoflush = ACE_OS::atoi(filter);

				if (autoflush <= 0  ||  autoflush > 9999)
				{
						if (autoflush <= 0)  ACE_OS::fprintf(stdout, "AUTOFLUSH seconds parameter value must be greater than zero\n");
						else				 ACE_OS::fprintf(stdout, "AUTOFLUSH seconds parameter value must be less than 9999\n");

						syntax_error = 1;
						delete_set(taskp->set);
						shmp->tasksTab[idx].refcount = -1;
						return (1);
				}
				else
				{
					/*
					 * The number of seconds are greater than zero, then
					 * store it in the autoflush parameter trace element
					 */
					taskp->autoflush = autoflush;
				}
			}
			else
			{
				ACE_OS::fprintf(stdout, "AUTOFLUSH parameter is missing\n");
				syntax_error = 1;
				delete_set(taskp->set);
				shmp->tasksTab[idx].refcount = -1;
				return (1);
			}
		}
		else		// autoflush parameter missing
		{
				ACS_TRA_common::unget_token(filter);
		}
	}
	/*
	 * Fetch the lof_filename parameter and check
	 * it if exists in typed command C, if it is
	 * exist store it in the filename element and
	 * create a file to store the trace in the
	 * default path (/var/log/ACS/TRA/)
	 */
	logfilename = ACS_TRA_common::get_token();

	if (logfilename != NULL)
	{
		// Check if the typed string is the SCREEN parameter
		if (ACE_OS::strcmp(logfilename, "SCREEN") != 0)
		{
			/*
			 * The log_filename typed in the command not exists
			 * in the shared memory, then check if in the filename
			 * is typed a path, if yes print an error message and
			 * return to trautil console
			 */
			if (ACE_OS::strchr(logfilename, '/') == 0)
			{
				/*
				 * Check if the default path /var/log/ACS/TRA/ exists,
				 * if not exist and it is not possible to create
				 * return an error message
				 */
				if (ACE_OS::mkdir(TRA_log_path, 07000) == -1)
				{
					if (errno == EEXIST)
					{
						/*
						 * Check if the log_filename typed in the command
						 * already contains the foreseen extension, if yes continue,
						 * otherwise add the extension.
						 */
						size_t len = ACE_OS::strlen(logfilename);
						bool extension_is_present = ( (len >= TRACETASK_LOGFILENAME_EXTENSION_LENGTH) &&
								  (ACE_OS::strcmp(logfilename + len - TRACETASK_LOGFILENAME_EXTENSION_LENGTH, TRACETASK_LOGFILENAME_EXTENSION) == 0) );

						ACE_OS::snprintf(tfilename, MAX_CHARS_LOG_FILENAME, "%s%s%s", TRA_log_path, logfilename, extension_is_present ? "" : TRACETASK_LOGFILENAME_EXTENSION);

						/*
						 * Check if log_filename that you typed already
						 * exists in the shared memory. If already exist
						 * print an error message and return to trautil console
						 */
						tmp_taskp = ACS_TRA_lib::get_filename(tfilename);
						if (tmp_taskp != NULL)
						{
								ACE_OS::fprintf(stdout, "File name already exists %s\n", tfilename);
								syntax_error = 1;
								delete_set(taskp->set);
								//delete_thread(taskp);
								shmp->tasksTab[idx].refcount = -1;
								return (1);
						}

						/*
						 * Check if the file typed in command line
						 * already exists in the path /var/log/ACS/TRA,
						 * if yes print an error message and return
						 * to trautil, otherwise continue
						 */
						output_file = ACE_OS::fopen (tfilename, "r");
						if (output_file != NULL)
						{
								ACE_OS::fprintf(stdout, "File name already exists %s\n", tfilename);
								syntax_error = 1;
								delete_set(taskp->set);
								//delete_thread(taskp);
								shmp->tasksTab[idx].refcount = -1;
								ACE_OS::fclose(output_file);
								return (1);
						}
						/*
						 * If the file is not created it fails and print
						 * an error message and return to trautil console,
						 * otherwise store the filename in the filename trace element
						 */
						output_file = ACE_OS::fopen (tfilename, "w");
						if (output_file == NULL)
						{
								output_file = stderr;
								ACE_OS::fprintf(output_file, "It is impossible to create the file in the default path\n");
								syntax_error = 1;
								delete_set(taskp->set);
								//delete_thread(taskp);
								shmp->tasksTab[idx].refcount = -1;
								return (1);
						}

						// Copy the log_filename in filename trace element
						ACE_OS::strncpy(taskp->filename, tfilename, ACE_OS::strlen(tfilename));
						taskp->filename[ACE_OS::strlen(tfilename)] = '\0';

						/*
						 * Check if the string SIZE is typed in command,
						 * if not a default value for trace log filename
						 * is consider, otherwise fetch the new value
						 */
						filter = ACS_TRA_common::get_token();

						if (filter != NULL)
						{
							if (ACE_OS::strcmp(filter, "SIZE"))
							{
									ACE_OS::fprintf(stdout, StringUsage);
									syntax_error = 1;
									delete_set(taskp->set);
									//delete_thread(taskp);
									if (output_file != NULL)  remove(tfilename);
									shmp->tasksTab[idx].refcount = -1;
									return (1);
							}
							else {
								/*
								 * Fetch the value of the SIZE parameter,
								 * and check if it is a valid value
								 */
								filter = ACS_TRA_common::get_token();

								if (filter != NULL)
								{
									/*
									 * Check if the value of parameter SIZE
									 * is only a digit, if not, print an error
									 * message and return to trautil console
									*/
									if (ACS_TRA_common::isDigit(filter) == 1)
									{
											ACE_OS::fprintf(stdout, StringUsage);
											syntax_error = 1;
											delete_set(taskp->set);
											// delete_thread(taskp);
											if (output_file != NULL)  remove(tfilename);
											shmp->tasksTab[idx].refcount = -1;
											return (1);
									}

									size = ACE_OS::atoi(filter);

									if (size <= 0  ||  size > 100)
									{
											if (size <= 0)  ACE_OS::fprintf(stdout, "SIZE megabytes value must be greater than zero\n");
											else			ACE_OS::fprintf(stdout, "SIZE megabytes value must be less than or equal to 100 Mb\n");
											syntax_error = 1;
											delete_set(taskp->set);
											// delete_thread(taskp);
											if (output_file != NULL)  remove(tfilename);
											shmp->tasksTab[idx].refcount = -1;
											return (1);
									}
								}
								else     // size parameter missing
								{
										ACE_OS::fprintf(stdout, "SIZE megabytes value is missing\n");
										syntax_error = 1;
										delete_set(taskp->set);
										// delete_thread(taskp);
										if (output_file != NULL)  remove(tfilename);
										shmp->tasksTab[idx].refcount = -1;
										return (1);
								}
							}
						}
						else  size = DEFAULT_SIZE_LOG_FILENAME;

						taskp->size = size;
					}
					else {
							ACE_OS::fprintf(stdout, "The default path %s does not exist or cannot be accessed\n", TRA_log_path);
							syntax_error = 1;
							delete_set(taskp->set);
							// delete_thread(taskp);
							shmp->tasksTab[idx].refcount = -1;
							return (1);
					}
				}
			}
			else	// The specified filename contains a path then abort
			{
					ACE_OS::fprintf(stdout, "It is not possible to specify any path in the trace log file name\n");
					syntax_error = 1;
					delete_set(taskp->set);
					// delete_thread(taskp);
					shmp->tasksTab[idx].refcount = -1;
					return (1);
			}
		}
		else {
				if (autoflush == 0)
				{
						ACE_OS::fprintf(stdout, StringUsage);
						syntax_error = 1;
						delete_set(taskp->set);
						// delete_thread(taskp);
						shmp->tasksTab[idx].refcount = -1;
						return (1);
				}

				taskp->video = 1;			// type a SCREEN parameter
		}
	}
	else {
			if (taskp->autoflush > 0  ||  taskp->filename[0] != 0)
			{
					ACE_OS::fprintf(stdout, StringUsage);
					syntax_error = 1;
					delete_set(taskp->set);
					//delete_thread(taskp);
					shmp->tasksTab[idx].refcount = -1;
					return (1);
			}
	}

	filter = ACS_TRA_common::get_token();

	if (filter != NULL)
	{
			ACE_OS::fprintf(stdout, StringUsage);
			syntax_error = 1;
			delete_set(taskp->set);
			//delete_thread(taskp);
			if (output_file != NULL)  remove(tfilename);
			shmp->tasksTab[idx].refcount = -1;
			return (1);
	}
	/*
	 * Retrieve the user and save it in the
	 * user element of trace_task structure
	 */
	ACE_OS::strcpy(taskp->user, ACE_OS::getenv("USER"));

	if (create_log(taskp, n, idx, instance) != 0)
	{
			delete_set(taskp->set);
			// delete_thread(taskp);
			if (output_file != NULL)  remove(tfilename);
			shmp->tasksTab[idx].refcount = -1;
			return (0);
	}


	// update trace masks of the current TRACE ELEMENTS, matching them against the trace set of the task just created.
	// Doing that, we pay attention to shared memory corruption issues
	if( 0 == ACS_TRA_lib::check_trace_data_p( shmp->trce_head, sizeof(trace_element), false) )
		trcep = ABS(trace_elementp, shmp->trce_head);
	else
		trcep = 0;	// CORRUPTED OFFSET !!

	while (trcep != 0)
	{
			update_mask (idx, taskp->set, trcep);

			if( (trcep->next != 0) &&  (0 == ACS_TRA_lib::check_trace_data_p(trcep->next, sizeof(trace_element), false)) )
				trcep = ABS(trace_elementp, trcep->next);
			else
				trcep = 0;	// reached the end of the trace element list, or found a corrupted OFFSET
	}

	shmp->taskcount ++;

	// if AUTOFLUSH feature has been requested for the task, create an autoflush thread to handle it
	if(taskp->autoflush > 0)
	{
		/*
		 * Create a new object with the taskp and the delimiter.
		 * Create and start the thread to manage the
		 * timer related the autoflush on a specified file
		 */
		autoflush_thread_handle = autoflush_thread_manager.create_thread(taskp, delimiter);
		if(autoflush_thread_handle == 0)
			ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_lib::create_task(): unable to create autoflush thread for task %s\n", taskp->name);
		else if(autoflush_thread_manager.start_thread(autoflush_thread_handle) < 0)
			ACE_OS::fprintf(stdout, "ERROR: ACS_TRA_lib::create_task(): unable to start autoflush thread for task %s\n", taskp->name);
		else
			taskp->thread_id = autoflush_thread_manager.get_OS_thread_id(autoflush_thread_handle);
	}

	return  1;
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: delete_task
 * Description: Delete the task (command D in the trautil console).
 *              Check if the task_name parameter is correct, decrease
 *              the taskcount variable and reset the treace mask.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::get_trace_data_p in ACS_TRA_lib.cpp file
 *       and in main of ACS_TRA_util.cpp file
 */
void ACS_TRA_lib::delete_task (char *name, ACE_INT32 command, char delimiter)
{
	ACE_INT32			idx;
	trace_taskp     	taskp;
	relpointer      	trceoffs;
	trace_elementp  	trcep;
	char               *tmp;
	bool				deleted = false;

	// Check if there are trace task in the shared memory to delete
	for (idx = 0; idx < MAX_TASK_COUNT; idx++)
	{
		if (shmp->tasksTab[idx].refcount != -1)
		{
			taskp = &shmp->tasksTab[idx];
			/*
			 * Fetch the command D delete to check if you want
			 * delete a specific task name or all tasks name.
			 * If you want delete a specific task name, fetch
			 * the name and check it if really exists in the shared memory
			 */
			if ((name == NULL) || (ACE_OS::strcmp(taskp->name, name) == 0))
			{
				if ((command)  && (ACE_OS::strcmp(taskp->user, ACE_OS::getenv("USER")) != 0))
						continue;

				deleted = true;

				// reset trace masks
				trceoffs = shmp->trce_head;

				// iterate on the list of TRACE ELEMENTS, paying attention to eventually corrupted OFFSETs
				while ( (trceoffs != 0) && (0 == ACS_TRA_lib::check_trace_data_p(trceoffs, sizeof(trace_element), false)) )
				{
						trcep = ABS(trace_elementp, trceoffs);
						reset_mask(trcep, idx);
						trceoffs = trcep->next;
				}

				shmp->taskcount --;

				delete_set(taskp->set);
				//delete_thread(taskp);
				// Don't destroy the autoflush thread eventually associated to the task explicitly ! It will automatically
				// terminate checking 'shmp->tasksTab[idx].refcount' and finding that it equals -1. Destroying the thread explicitly, calling
				// 'ACS_TRA_Lib::delete_thread()', can lead to deadlock, due to the fact that :
				// 1 - 'ACS_TRA_lib::delete_task()'is called having yet acquired shared memory mutex;
				// 2 - "ACS_TRA_lib::delete_thread()" waits for graceful termination of the autoflush thread.
				// In particular, if 'delete_thread()' is invocated and autoflush thread is blocked waiting for the mutex,
				// then autoflush thread will never obtain the mutex and will never check for stopping request,
				// and so the thread invoking 'ACS_TRA_Lib::delete_task() will wait forever (deadlock)

				if (command)
				{
						if (command != 2)  display_log (0, delimiter, taskp);
				}

				shmctl (taskp->shm_id, IPC_RMID, 0);

				shmp->tasksTab[idx].refcount = -1;
			}
		}
	}
	/*
	 * If a specified trace task to delete it is
	 * not exist, print an error message and return
	 * to trautil console.
	 */
	if (deleted == false  &&  name != NULL)
	{
			ACE_OS::fprintf(stdout, "task not found \n");
			ACE_OS::fflush(stdout);
	}

	if (shmp->taskcount == 0)
	{
		// backwards compatibility check
		if (shmp->maxTraceSize == 0)
		{
			if ((tmp = (char *) ACE_OS::getenv("TRACE_MEM_SIZE")) != NULL)
						shmp->maxTraceSize = ACE_OS::atoi(tmp);

			if (shmp->maxTraceSize == 0)  shmp->maxTraceSize = SHM_DEFAULT_SIZE;
		}

		shmp->free_task_data = TOP_LEVEL_SHM_SIZE;
		shmp->free_temp_head = 0;
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: search_pid
 * Description: Return pid to proc element.
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_basic_set in ACS_TRA_lib.cpp file
 */
relpointer ACS_TRA_lib::search_pid(ACE_INT32 pid)
{

	register int  j = 0;

	while (j < MAX_PIDS  &&  shmp->pidTab[j].pid != pid)  j ++;

	if (j < MAX_PIDS)  return (shmp->pidTab[j].proc);

	return (0);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_trace_key
 * Description: -
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::ACS_TRA_lib and ACS_TRA_lib::get_shared_memory
 *       in ACS_TRA_lib.cpp file
 */
key_t ACS_TRA_lib::get_trace_key()
{
	return ftok("/etc/host.conf", 2);
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: autoflush
 * Description: Automatic flush on a file
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp
 *       and in main program of ACS_TRA_util.cpp file
 */
void ACS_TRA_lib::autoflush(void* arguments)
{
	ACE_INT32 		i = 1;
	char            *taskname;
	char 			delimiter;
	trace_taskp     pointer;

	// Retrieve the taskp and the delimiter
	Args *arg = (Args*) arguments;

	// Store the delimiter and the taskp in local variable
	pointer = arg->pointer;
	delimiter = arg->delimiter;

	while(i == 1)
	{
		// Retrieve the task name
		taskname = pointer->name;

		// Wait the time of autoflush
		ACE_OS::sleep(pointer->autoflush);

		/*
		 * If task name is NULL an error message is
		 * printed and return to trautil console
		 */
		if (taskname == NULL)
			ACE_OS::fprintf(stdout, "Failure: taskname not found\n");
		else
		{
			/*
			 * The task name is valid and found in trace shared memory,
			 * retrieve the user that use the console trautil and check
			 * if he has permission to flush this trace log.
			 */
			if ((ACE_OS::strcmp(pointer->user, ACE_OS::getenv("USER"))) != 0)
				ACE_OS::fprintf(stdout, "Not allowed to flush log owns by other users\n");
			else
			{
				if (pointer->logBuff.typ == PID  ||  pointer->logBuff.typ == LOG)
				{
					if (pointer->filename[0] == 0)
							ACE_OS::fprintf(stdout, "\n");

					// Call method to display and save the log
					ACS_TRA_lib::display_log(1, delimiter, pointer);
				}
				else
					ACE_OS::fprintf(stdout, "Task filter is not log\n");
			}

			/*
			 * Pint the prompt command line if is
			 * active only the AUTOFLUSH option
			 * without the filename option
			 */
			if(pointer->autoflush > 0)
			{
				if (pointer->filename[0] == 0)
				{
					// Print on standard output the prompt of the command
					ACE_OS::fputs("trc> ", stdout);
					// Flush the specified trace task name on the standard output
					ACE_OS::fflush (stdout);
				}
			}
		}
	}
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: delete_thread
 * Description: Automatic flush on a file
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::delete_task in ACS_TRA_lib.cpp
 */
void ACS_TRA_lib::delete_thread(trace_taskp pointer)
{
	/*
	 * Check if there are tasks with the
	 * file name parameter and the autoflush
	 * parameter, so delete the thread that manage it
	 */
	if (pointer->filename != NULL)
	{
		if (pointer->autoflush != 0)
		{
			/*
			 * Delete the thread if possible,
			 * otherwise print an error message
			 */
			/*
			if(ACE_Thread::cancel(pointer->thread_id) != 0)
				ACE_OS::fprintf(stdout, "It is not possible to delete the thread\n");
			*/

			// destroy autoflush thread, but only if has been created by this process
			ACS_TRA_AutoflushThreadManager::ACS_TRA_THREAD_HANDLE thr_hdl = autoflush_thread_manager.find_thread_handle_by_OS_thread_id(pointer->thread_id);
			if(thr_hdl && (autoflush_thread_manager.destroy_thread(thr_hdl) != 0))
				ACE_OS::fprintf(stdout, "It is not possible to delete the thread\n");
		}
	}
}


/*
 * Class Name: ACS_TRA_lib
 * Method Name: checkFileSize
 * Description: Check if the specified file reached the maximum
 * 				size, if yes rename it added 1 after the name
 * 				and re-create the file for new trace
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::display_log in ACS_TRA_lib.cpp file
 */
FILE* ACS_TRA_lib::checkFileSize(FILE* file_desc, char* filename, ACE_INT32 filesize)
{
	FILE *			new_file_desc;
	ACE_INT64		sizeOfFile;
	ACE_INT32 		result;
	char 			oldname[MAX_CHARS_LOG_FILENAME];
	char 			newname[MAX_CHARS_LOG_FILENAME];

	fseek (file_desc, 0, SEEK_END);
	sizeOfFile = ftell (file_desc);

	filesize = (filesize * 1000000);

	/*
	 * Check if the file reached the maximum
	 * size, if yes rename it added 1 after the
	 * name and re-create the file for new trace
	 */
	if(sizeOfFile > filesize)
	{
		// Reset the oldname and the newname variable
		ACE_OS::memset(oldname, 0, MAX_CHARS_LOG_FILENAME);
		ACE_OS::memset(newname, 0, MAX_CHARS_LOG_FILENAME);

		// Close the file
		ACE_OS::fclose(file_desc);

		// Save the actual filename
		ACE_OS::strcpy(oldname, filename);
		ACE_OS::strcpy(newname, filename);
		ACE_OS::strcat(newname, ".1");

		/*
		 * Check if the file already exists,
		 * if yes delete it
		 */
		new_file_desc = ACE_OS::fopen(newname, "r");
		if (new_file_desc != NULL)
		{
			// Close the file
			ACE_OS::fclose(new_file_desc);
			// Delete the file with suffix .1
			remove(newname);
		}

		// Rename the file
		result = ACE_OS::rename(oldname , newname);

		if (result != 0)
			ACE_OS::fprintf(stdout, "Error while renaming the file\n");

		// Open the new file
		file_desc = ACE_OS::fopen (oldname, "a");
	}

	return file_desc;
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: set_cmd_chars
 * Description: Check if exists a character in the string
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::set_cmd_chars(char *par_s)
{
	ACS_TRA_common::s = par_s;
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: get_cmd_chars
 * Description: Check if exists a character in the string
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
char* ACS_TRA_lib::get_cmd_chars()
{
	return ACS_TRA_common::s;
}

/*
 * Class Name: ACS_TRA_lib
 * Method Name: reset_cmd_chars
 * Description: Check if exists a character in the string
 * Used: This method is used in the following member:
 *       ACS_TRA_lib::create_task in ACS_TRA_lib.cpp file
 */
void ACS_TRA_lib::reset_cmd_chars()
{
	ACS_TRA_common::s = ACS_TRA_common::sarea;
}


ACE_INT32 ACS_TRA_lib::acquire_shared_memory_mutex(unsigned long tag)
{
	int retval = 0;
	int my_errno = 0;
	char buf[1024]={0};
	bool work_completed = false;
	int result = 0;

#ifdef __TRALIB_DEBUG_ENABLED__
	snprintf(buf, 1024, "*** Entered %s. TAG = %lu", __func__, tag);
	ACS_TRA_lib::trace_error(buf, 0, false);
#endif

	if(!sm_mutex_p)
	{
		// very strange. Should never occur ! ( static initialization order fiasco ? )
		snprintf(buf, 1024, "*** Failure acquiring shared memory mutex. (sm_mutex_p == NULL) ! TAG = %lu", tag);
		ACS_TRA_lib::trace_error(buf, 0);
		retval = -1;
		work_completed = true;
	}

	if(!work_completed)
	{
		// try to acquire shared memory mutex, in blocking mode
		do
		{
			result = sm_mutex_p->acquire();
			if(result == 0)
			{
#ifdef __TRALIB_DEBUG_ENABLED__
				snprintf(buf, 1024, "*** OK acquired shared memory mutex. TAG = %lu", tag);
				ACS_TRA_lib::trace_error(buf, 0, false);
#endif
			}
			else
			{
				my_errno = errno;
				snprintf(buf, 1024, "*** Failure acquiring shared memory mutex. ERRNO = %d. TAG = %lu !", my_errno, tag);
#ifdef __TRALIB_DEBUG_ENABLED__
				ACS_TRA_lib::trace_error(buf, 0, false);
#else
				ACS_TRA_lib::trace_error(buf, 0, (my_errno != EINTR));
#endif
			}
		}
		while((result != 0) && (my_errno == EINTR));

		if(result != 0)
			retval = -1; // we didn't manage to acquire shared memory mutex
	}

#ifdef __TRALIB_DEBUG_ENABLED__
	snprintf(buf, 1024, "*** Exiting %s. TAG = %lu", __func__, tag);
	ACS_TRA_lib::trace_error(buf, 0, false);
#endif

	return retval;
}


ACE_INT32 ACS_TRA_lib::release_shared_memory_mutex(unsigned long tag)
{
	char buf[1024]={0};
	int retval = 0;
	int my_errno = 0;

#ifdef	__TRALIB_DEBUG_ENABLED__
	snprintf(buf, 1024, "*** Entered %s. TAG ) %lu", __func__, tag);
	ACS_TRA_lib::trace_error(buf, 0, false);
#endif

	if(!retval && !sm_mutex_p)
	{
		snprintf(buf, 1024, "*** Failure releasing shared memory mutex. (sm_mutex_p == 0) ! TAG = %lu", tag);
		ACS_TRA_lib::trace_error(buf, 0);
		retval = -1;
	}

	if(!retval)
	{
		do
		{
			retval = sm_mutex_p->release();
			if(!retval)
			{
#ifdef	__TRALIB_DEBUG_ENABLED__
				snprintf(buf, 1024, "*** OK released shared memory mutex. TAG = %lu", tag);
				ACS_TRA_lib::trace_error(buf, 0, false);
#endif
			}
			else
			{
				my_errno = errno;
				snprintf(buf, 1024, "*** Failure releasing shared memory mutex. ERRNO = %d ! TAG = %lu", my_errno, tag);
#ifdef	__TRALIB_DEBUG_ENABLED__
				ACS_TRA_lib::trace_error(buf, 0, false);
#else
				ACS_TRA_lib::trace_error(buf, 0, (my_errno != EINTR));
#endif
			}
		}
		while(retval && (my_errno == EINTR));
	}

#ifdef	__TRALIB_DEBUG_ENABLED__
	snprintf(buf, 1024, "*** Exiting %s. TAG = %lu", __func__, tag);
	ACS_TRA_lib::trace_error(buf, 0, false);
#endif

	return retval;
}

ACE_Process_Mutex * ACS_TRA_lib::shared_memory_mutex_p()
{
	return sm_mutex_p;
}

int ACS_TRA_lib::check_and_adjust_shared_memory_system_V_sem()
{
	// Try to open the System V semaphore used to implement the shared memory mutex
	ACE_SV_Semaphore_Complex sem;
	if (sem.open(TRALIB_SHARED_MEMORY_MUTEX_NAME, ACE_SV_Semaphore_Complex::ACE_OPEN, 1, 1, ACE_DEFAULT_FILE_PERMS))
		 return (errno == ENOENT) ? 2 /* the semaphore doesn't exist*/: -2 /* an error occurred while opening the semaphore */;

	// get owner and permissions associated to the semaphore
	semid_ds semInfo;
	semun semArg;
	semArg.buf = &semInfo;
	int op_result = sem.control(IPC_STAT, semArg, 0);
	if(op_result < 0)
	{
		char trace_msg[1024]={0};
		snprintf(trace_msg, 1024, "*** Unable to retrieve info about the System V semaphore used to implement the shared memory mutex, errno == %d", errno);
		syslog(LOG_WARNING, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), trace_msg);
		return -2;
	}

	if( (semInfo.sem_perm.uid == 0) && (semInfo.sem_perm.gid == 0) && (semInfo.sem_perm.mode == 0777))
		return 0;	// the owner and permissions are right

	// semaphore owner or permissions are wrong. Try to set right values
	semInfo.sem_perm.uid = 0 /*root*/;
	semInfo.sem_perm.gid = 0 /*root*/;
	semInfo.sem_perm.mode = 0777;
	semArg.buf = & semInfo;
	op_result = sem.control(IPC_SET, semArg, 0);
	if(op_result < 0)
	{
		char trace_msg[1024]={0};
		snprintf(trace_msg, 1024, "*** Unable to set owner or permissions for the System V semaphore used to implement the shared memory mutex, errno == %d", errno);
		syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message : %s \n ", getpid(), syscall(SYS_gettid), trace_msg);
		return -1;
	}

	syslog(LOG_DEBUG, "PID = <%d> - TID = <%ld>. TRALIB message : shared memory semaphore owner and permissions successfully adjusted\n ", getpid(), syscall(SYS_gettid));
	return 1;	// semaphore ownership and/or permissions successfully adjusted
}

int ACS_TRA_lib::check_trace_data_p(relpointer relp, size_t n_bytes, bool nolog)
{
	if ((relp < static_cast<long>(sizeof(struct trace_mem))) || ((relp + static_cast<long>(n_bytes)) > shmp->free_trace_data) || ((relp % 4) != 0))
	{
		if(!nolog && notify_errors_in_syslog)
		{
			syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message : check_trace_data_p() >>> Invalid offset: [ %ld]  \n ", getpid(), syscall(SYS_gettid), relp);
			notify_errors_in_syslog = false;  // allow only one syslog notification per process
		}
		return 1;
	}
	else
		return 0;
}

int ACS_TRA_lib::check_trace_set_p(relpointer relp, size_t n_bytes, bool nolog)
{
	if ((relp < static_cast<long>(sizeof(struct trace_mem))) || ((relp + static_cast<long>(n_bytes)) > (TOP_LEVEL_SHM_SIZE - 1) ) || ((relp % 4) != 0))
	{
		if(!nolog && notify_errors_in_syslog)
		{
			syslog(LOG_ERR, "PID = <%d> - TID = <%ld>. TRALIB message : check_trace_set_p() >>> Invalid offset: [ %ld]  \n ", getpid(), syscall(SYS_gettid), relp);
			notify_errors_in_syslog = false;  // allow only one syslog notification per process
		}
		return 1;
	}
	else
		return 0;
}
