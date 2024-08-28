/*
 * .NAME: ACS_TRA_common.h
 *
 * .LIBRARY 3C++
 * .PAGENAME
 * .HEADER
 * .LEFT_FOOTER Ericsson Telecom AB
 * .INCLUDE
 *
 * .COPYRIGHT
 *  COPYRIGHT Ericsson Telecom AB 2014
 *  All rights reserved.
 *
 *  The Copyright to the computer program(s) herein is the property of
 *  Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with the written
 *  permission from Ericsson Telecom AB or in accordance with
 *  the terms and conditions stipulated in the agreement/contract under
 *  which the program(s) have been supplied.
 *
 * .DESCRIPTION
 *	This is the pseudo data structure used in the shared memory trace
 *	kernel.
 *	All pointers are relative the start address of the shared memory.
 *	Two types of memory allocation:
 *      permanent trace data
 *		trace_element, proc, record, format
 * 	are allocated from the start of the shared memory, and accessed
 *	through a hash table.
 *ACE_OS::getenv("USER")
 *	temporary task data
 *		trace_task, trace_set, filters, log buffers , ...
 *	are allocated from free lists or from the end of the shared memory
 *      two types :
 *	log buffers 	size BUFFERSIZE
 *	other trace tasks, trace_set  size TEMPSIZE
 *
 * .ERROR HANDLING
 *
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0024
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
 * PA1	        20100504	XGIOPAP		Impact due to IP:
 *	  									2/159 41-8/FCP 121 1642
 *	  									"APG50 Tracing and MKTR"
 * PA2 	        20101111	XGIOPAP		Implementation of comment
 *              20140327        XSARSES         TRAUTILBIN - core dump fix
 */

#ifndef ACS_TRA_COMMON_H_
#define ACS_TRA_COMMON_H_

// Module Include Files
#include <pthread.h>
#include <string>

/*
 * GLOBAL VIARIABLES
 * Type declaration for relpointer type: offset relative to
 * the start of shared memory trace segmant
 */
typedef long     relpointer;

// DEFINE
#define TRACE_REV  "A   "				// Trace revision
#define TRACE_LEVEL 1					// Trace level
#define TEMPSIZE 64  					// username stored with 32 chars instead of an integer
#define BUFFERSIZE 1024					// log buffer size
#define MAIN_SHM_SIZE 		100000 		// NOT USED (its use has been replaced by 'TOP_LEVEL_SHM_SIZE')! Kept only for backward compatibility
#define TOP_LEVEL_SHM_SIZE 	(8 * 1024 * 1024) // Fixed size of TOP LEVEL SHM (bytes)
#define SHM_DEFAULT_SIZE  1000			// Default memory size of the trace shared memory (kilobytes)
#define MAX_PIDS 200					// max number of Process IDs
#define HASHSIZE 256					// max number in hash table
#define MAX_TASK_COUNT 32  				// max number of task records
#define MAX_TASK_NAME 12				// max number of chars for task name
#define MAX_CHARS_COM 256  				// max number of chars in trautil command
#define MAX_CHARS_LOG_FILENAME 256 		// max number of chars for parameter log_filename
#define DEFAULT_SIZE_LOG_FILENAME 50	// default size in megabytes for trace log filename
#define TRACESTATUSON "ON"				//Trace status ON
#define TRACESTATUSOFF "OFF"			//Trace status OFF
#define TRA_log_path "/var/log/acs/tra/"	// Path to store the trace produced
#define LOG4CPLUSPATH "/cluster/etc/ap/acs/tra/conf/log4cplus.properties"	// Path for log4cplus configuration file
#define TRACESTATUSPATH "/cluster/etc/ap/acs/tra/conf/Trace_Status"		// Path for Trace Status file
#define MEMORYSIZEPATH "/cluster/etc/ap/acs/tra/conf/Memory_Size"			// Path for Memory Size file
#define StringUsage "usage: C taskname trace_type trace_set LOG number_of_buffers [log_filename [SIZE megabytes]]\n       C taskname trace_type trace_set LOG number_of_buffers [AUTOFLUSH seconds SCREEN]\n       C taskname trace_type trace_set LOG number_of_buffers [AUTOFLUSH seconds log_filename [SIZE megabytes]]\n"
#define UsageOptionL "Usage: trautil -l\n"
#define UsageOptionD "Usage: trautil -dx\n"
#define UsageOptionM "Usage: trautil -m memory_size\n"
#define UsageOptionO "Usage: trautil -o console_activation\n"
#define UsageTrautil "Wrong parameter use:\ntrautil [-l]\ntrautil [-dx]\ntrautil [-m memory_size]\ntrautil [-o console_activation]\n"

// EXTERNAL DEFINE
extern struct trace_mem *shmp;
extern int 				pid;
extern int  			instance;
extern int 				syntax_error;

/*
 * The struct trace_data is used for permanent data accessed via the hash
 * table process name record name format trace element
 *
 * First element PROC in UNIX seems to be used somewhere in NT
 * therefore changed to PRCS
 */
enum trace_data_type {
	PROC, REC, FORMAT, TRCE, CLASS, PLUS, MINUS, INTERSECTION
};

struct trace_data {
	relpointer      next;
	enum trace_data_type typ;
};
typedef struct trace_data *trace_datap;

// trace element
struct trace_element {
	relpointer      proc;
	relpointer      record;
	relpointer      format;
	relpointer      next;
	int       		mask;
};
typedef struct trace_element *trace_elementp;

// for linking in free lists
struct list {
	relpointer      next;
};

enum filters {
	LOG, PID, IF, COUNTER, ABORT, TRIGGER, EXEC, DUMP
};

// log filter
struct log
{
	enum filters    typ;
	int       		nobuf;
	int				pid;
	int				circular;
	relpointer      shmp;
	relpointer      startpos;
	relpointer      currpos;
};
typedef struct log  *logp;


// The elements of the trace task
struct trace_task
{
	int       		refcount;
	char	 		user[MAX_TASK_NAME];		  		// user name
	char            name[MAX_TASK_NAME];		  		// task name
	char            filename[MAX_CHARS_LOG_FILENAME]; 	// trace log file name
	int				autoflush;			  				// auto flush in seconds
	int				size;								// size of the trace log file
	int				video;								// flag for the standard output
	int       		shm_id;
	int       		shm_size;
	struct log		logBuff;
	relpointer      set;
	pthread_t    	thread_id;			  				// thread id
};
typedef struct trace_task  *trace_taskp;

// trace set
struct trace_set
{
	// same header format as trace data
	relpointer      next;
	enum trace_data_type typ;	// operator or basic type
	// extra data for set tree
	relpointer      left;		// basic set relpointer
	relpointer      right;		// basic set relpointer
	int 		    refcount;
};
typedef struct trace_set *trace_setp;

struct filter
{
	enum filters    typ;
};
typedef struct filter *filterp;

// pid filter
struct pidfilter
{
	enum filters    typ;
	int       		refcount;
	int       		pid;
	relpointer      filter;
};

// pid table record
struct pidrec
{
	relpointer      proc;
	int       		pid;
};

// threshold filter
struct threshold
{
	enum filters    typ;
	int       		refcount;
	int       		limit;
	int       		reccount;
	relpointer      lessfilter;
	relpointer      equalfilter;
	relpointer      greaterfilter;
};

/*
 * log record format pointer to next record timestamp
 * pid pointer to trace element aligned parameters
 */
struct logbuff
{
	relpointer      next;
	char	        data[BUFFERSIZE - sizeof(relpointer)];
};

// struct of trace memory
struct trace_mem
{
	char				rev[4];
	char				lev;
	int					state;
	relpointer      	trce_head;
	relpointer      	free_trace_data;
	relpointer      	free_task_data;
	relpointer      	free_temp_head;
	struct pidrec		pidTab[MAX_PIDS];
	struct trace_task	tasksTab[MAX_TASK_COUNT];
	relpointer      	hashtab[HASHSIZE];
	int	       			taskcount;
	int					maxTraceSize;                 //  number of Ks to be used for the tracing
};


/*
 * Name: ACS_TRA_common
 * Description: This class contains method and attribute common
 */
class ACS_TRA_common {

public:
	static char    	sarea[MAX_CHARS_COM];
	static char*    s;

	ACS_TRA_common();
	static char get_char();
	static void unget_char();
	static char* get_token();
	static void unget_token(char * token);
	static char* readTraceStatus(char* b);
	static int writeTraceStatus(int on_off);
	static char* readMemorySize(char* newsize);
	static int writeMemorySize(char* newsize);
	static int isDigit(char *str);
};

/*
 * Name: Args
 * Description: This class is used to transfer the
 * 				parameter to the thread for the autoflush
 */
class Args{
public:
	Args(trace_task* p, char d){pointer = p; delimiter = d;};
	trace_task* pointer;
	char delimiter;
};

/***** START MACRO DEFINE *****/
//It calculates the shared memory absolute address based on entry shmp and offset
#define ABS(typ, p)\
		(typ)(p + (long)shmp)

//It calculates the shared memory relative address
#define REL(p)\
		(long)p - (long)shmp

//It align the buffer pointer
#define ALIGN(p, n)\
		p += ((p%n) == 0) ? 0 : (n - (p%n));

//Increment buffer pointer
#define ADVANCE(size)\
		bp += size;

//If the buffer pointer is greater than of end buffer pointer,
//it calculates the new start and end buffer pointer.
#define CHECK_BUFFER(size)\
	 if (bp + size > endbuf)\
	 {\
		 startbufp = ABS(relpointer *, *startbufp);\
		 endbuf = (long)startbufp + BUFFERSIZE;\
		 bp = (long)startbufp + sizeof(relpointer);\
	 }

//It calculates the new start position in the buffer
#define CHECK_STARTPOS(size)\
		while ((bp <= (long)startposp) && ((long)startposp < bp + size))\
		{\
			startposp = ABS(relpointer*, *startposp);\
		}
/***** END MACRO DEFINE *****/
#endif /* ACS_TRA_COMMON_H_ */
