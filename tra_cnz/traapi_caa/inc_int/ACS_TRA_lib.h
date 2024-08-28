/*
 * .NAME: ACS_TRA_lib.h
 *
 * .LIBRARY 3C++
 * .PAGENAME
 * .HEADER
 * .LEFT_FOOTER Ericsson Telecom AB
 * .INCLUDE
 *
 * .COPYRIGHT
 *  * COPYRIGHT Ericsson Telecom AB 2014
 *  All rights reserved.
 *
 *  The Copyright to the computer mainprogram(s) herein is the property of
 *  Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with the written
 *  permission from Ericsson Telecom AB or in accordance with
 *  the terms and conditions stipulated in the agreement/contract under
 *  which the program(s) have been supplied.
 *
 * .DESCRIPTION
 *	This file contains the class ACS_TRA_lib with all methods and
 *  function to initialize and manage the shared memory and the semaphore
 *
 * .ERROR HANDLING
 *
 * DOCUMENT NO
 * 190 89-CAA 109 0024
 *
 * AUTHOR
 * 	2010-05-04 by XYV  XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1	        	20100504	XGIOPAP		Impact due to IP:
 *	  						2/159 41-8/FCP 121 1642
 *	  						"APG50 Tracing and MKTR"
 *                      20140327        XSARSES         TRAUTILBIN - core dump fix
 *
 */

#ifndef ACS_TRA_LIB_H_
#define ACS_TRA_LIB_H_

// Module Include Files
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Shared_Memory_SV.h>
#include <ace/Thread.h>
#include <ace/Process_Mutex.h>

// Name of the system-wide mutex used to synchronize access to TRA shared memory
#define TRALIB_SHARED_MEMORY_MUTEX_NAME 				ACE_TEXT("tralib.shared_memory_mutex")

#ifdef	__TRALIB_DEBUG_ENABLED__
// Path of the file used to enable TRALIB debug messages in sys log
#define TRALIB_DEBUG_ENABLER_FILE						"/var/log/acs/tra/tralib_debug_active"
#endif

#define TRACETASK_LOGFILENAME_EXTENSION ".log"
#define TRACETASK_LOGFILENAME_EXTENSION_LENGTH (sizeof(TRACETASK_LOGFILENAME_EXTENSION) - 1)

/*
 * Name: ACS_TRA_lib
 * Description: This class create, initialize and manage the shared
 *              memory and the semaphore
 */
class ACS_TRA_lib {
	static relpointer create_trace_set();
	static relpointer create_basic_set();
	static relpointer create_op_set(char op, relpointer left, relpointer right);

public:
	ACS_TRA_lib();

	static void trace_error(const char *errmsg, ACE_INT32 errorMsg, bool out_to_console = true);
	static void update_mask(ACE_INT32 i, relpointer relp, trace_elementp trcep);
	static void reset_mask(trace_elementp trcep, ACE_INT32 i);
	static ACE_INT32 traverse_set(relpointer relp, trace_elementp trcep);
	static void init_shared_memory(ACE_INT32 size);
	static void clean_shared_memory(const char *info);
	static bool  consistencyOfSM (ACE_INT64 bp, const char *info);
	static ACE_INT32 get_shared_memory();
	// static ACE_INT32 get_semaphore();
	// static ACE_INT32 release_semaphore();
	static void free_temp(relpointer relp);
	static void free_log_buffers(relpointer startbuf);
	static relpointer get_one_log_buffer();
	//static void display_log(ACE_INT32 inew, char delimiter, trace_taskp taskp);
	static void display_log(ACE_INT32 inew, char delimiter, trace_taskp taskp, int dont_acquire_mutex = 0, void * task_shmp = 0);
	static void process_row(char* row, FILE* file, ACE_INT32 autoflush);
	static int create_log(trace_taskp taskp, ACE_INT32 n, ACE_INT32 pos, ACE_INT32 pid);
	static void delete_set(relpointer setrel);
	static ACE_UINT32 hashvl(char *s, ACE_INT32 l);
	static relpointer get_trace_data_p(char *key, enum trace_data_type typ, ACE_INT32 command);
	static relpointer get_task_data_p();
	static trace_taskp get_task(char *name);
	static trace_taskp get_filename(char *filename);
	static relpointer create_task(char *name, char delimiter);
	static void delete_task(char *name, ACE_INT32 command, char delimiter);
	static relpointer search_pid(ACE_INT32 pid);
	static key_t get_trace_key();
	static void autoflush(void* arguments);
	static void delete_thread(trace_taskp pointer);
	static void resetSharedMemory();
	static FILE* checkFileSize(FILE* file_desc, char* file, ACE_INT32 filesize);
	static void set_cmd_chars(char* par_s);
	static char* get_cmd_chars();
	static void reset_cmd_chars();
	static ACE_INT32 acquire_shared_memory_mutex(unsigned long tag = 0);	// used to get exclusive access to top level shared memory
	static ACE_INT32 release_shared_memory_mutex(unsigned long tag = 0);	// used to release exclusive access to top level shared memory
	static ACE_Process_Mutex * shared_memory_mutex_p();		// get pointer to process mutex used to synchronize access to top level shared memory

	/* Attach the shared memory of the input task to the virtual address space of the process.
	 * Return:
	 * 	- 0,  on failure
	 *  - the pointer to the attached memory, on success
	 */
	static void * attach_task_shm(trace_taskp taskp);

	/*
	 * Check existence (on the node) of the System V semaphore used to implement the shared memory mutex,
	 * and verify that it has the right owner (root) and permissions (0777). If owner or permissions are wrong, try to adjust them.
	 * Returns :
	 *   0 --> shared memory semaphore exists and has the right owner (root) and permissions (0777).
	 *   1 --> shared memory semaphore did exist with wrong owner or permissions, and such properties have been successfully adjusted.
	 *   2 --> shared memory semaphore doesn't exist.
	 *  -1 --> shared memory semaphore exists with wrong owner or permissions, and failed to adjust them.
	 *  -2 --> generic error.
	*/
	static int check_and_adjust_shared_memory_system_V_sem();

	/** @brief check_trace_data_p method
	 *
	 *	Check if the passed address is a valid 'trace_data' offset (with respect to top level shared memory)
	 *
	 *	@param[in] relp Address to check
	 *	@param[in] n_bytes Number of bytes associated to 'relp' address
	 *	@param[in] nolog Indicates if the method must log in syslog (if false) or not (true)
	 *	@return 0 if 'relp' is a valid offset, 1 otherwise
	 *	@remarks Remarks
	 */
	static int check_trace_data_p(relpointer relp, size_t n_bytes, bool nolog);

	/** @brief check_trace_set_p method
	 *
	 *	Check if the passed address is a valid 'trace_set' offset (with respect to top level shared memory)
	 *
	 *	@param[in] relp address to check
	 *	@param[in] n_bytes number of bytes associated to 'relp' address
	 *	@param[in] nolog Indicates if the method must log in syslog (if false) or not (true)
	 *	@return 0 if 'relp' is a valid offset, 1 otherwise
	 *	@remarks Remarks
	 */
	static int check_trace_set_p(relpointer relp, size_t n_bytes, bool nolog);
};

#endif /* ACS_TRA_LIB_H_ */
