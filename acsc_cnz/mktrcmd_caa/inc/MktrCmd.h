/** @file MktrCmd.h
 *	@brief  class to launch Mktr Command Table
 *	@author
 *	@date
 *	@version 1.0.0
 *
 */
#ifndef MKTRCMD_H_
#define MKTRCMD_H_

#include <iostream>
#include <string>
#include <list>

/** @define block
 *
 *	define utilities for manipulate files
*/
#define MAXBUF 1024
#define OPEN_ALWAYS "a"
#define CREATE_ALWAYS "w"

/** @class MktrCmd
 *	@brief class that manage the launch of child process to execute commands
 *	@author
 *	@date
 *	@version 1.0.0
 *
 */
class MktrCmd
{
public:

	// Constants used to store details about command execution
	enum MktrCmd_AppErrorCode
	{
		MKTRCMD_NO_INFO = -3,		// no info available
		MKTRCMD_APPERROR = -2,		// an application specific error occurred during command execution
		MKTRCMD_SYSTEM_ERROR = -1,	// a system error (resulting from a system call)  occurred during command execution
		MKTRCMD_NORMAL_EXIT_0  = 0, // the executor process terminated  normally with exit code 0;
		MKTRCMD_NORMAL_EXIT_NOT_0 = 1, // the executor process terminated normally with exit code other than 0;
		MKTRCMD_TIMEOUT = 2, 		   // the executor process was killed for timeout expiration;
		MKTRCMD_SIGNAL_NOT_CAUGTH = 3, // the executor process exited because of a signal which was not caught;
		MKTRCMD_ABNORMAL_TERM = 4 	// the executor process terminated abnormally
	};

	/** @brief constructor method
	 *
	 *	default constructor
	 *
	*/
    MktrCmd();


	/** @brief destructor method
	 *
	 *	destructor
	 *
	 */
    ~MktrCmd();


	 /** @brief execute method
	 *
	 *	launch a child process and execute the passed command
	 *
	 *	@param[in] string& cmd
	 *	@param[in] const int& timeout
	 *	\return boolean
	 */
    bool execute(const std::string& cmd, int timeout = 0);


    /** @brief execute method
	*
	*	launch a child process and execute (using "sh", the Standard Command Language Interpreter) the passed command;
	*   the output of the command (standard output and standard error) will be redirected to the specified log file;
	*   the mode specifies if the log file must be open in append mode or truncated;
	*   optionally can be specified the working directory and a timeout (in seconds).
	*	@param[in] const string& cmd
	*	@param[in] const string& log
	*	@param[in] const string mode
	*	@param[in] const string& dir
	*	@param[in] const int& timeout
	*	\return boolean
	*/
    bool execute(const std::string& cmd, const std::string& log, const std::string mode, const std::string& dir = "",const int timeout = 0, const char *userName = "", const char *groupName = "");


    /** @brief execute method
	*
	*	Creates a process for executing the command "cmd". The command is executed using "sh", the Standard Command Language Interpreter;
	*	if the parameter "dir" is other than "" (empty string), the new process sets "dir" as its working directory.
	*	The standard output and standard error are redirected to the file 'fLog'.
	*	If "timeout" > 0, the process is killed if it doesn't terminate in "timeout" seconds.
	*
	*	@param[in] const string & cmd
	*	  the command that has to be executed.
	*	@param[in] const string & dir
	*	  if other than "", specifies the working directory.
	*	@param[in] FILE * fLog
	*	  file to which "standard output" and "standard error" are redirected.
	*	@param[in] int timeout
	*	  if greater than 0, indicates a timeout (in seconds) for the command execution. If the command is not executed in
	*	  timeout seconds, the process created to execute the command is killed.
	*/
    bool execute(const std::string& cmd, FILE *fLog, const std::string& dir = "",const int timeout = 0, const char *userName = "", const char *groupName = "");


	/** @brief getLastErrorMsg method
	*
	*	return the last error message
	*
	*	\return string
	*/
    std::string getLastErrorMsg();

    /// Get the result of command execution
    MktrCmd_AppErrorCode getCommandExecutionResult() { return appErrorCode; }

    /// Get exit code of command executor process, if that process terminated normally
    unsigned long getCommandExecutorProcessExitCode() { return exitCode; }

private:
	unsigned long exitCode;		// used to store the "exit code" of the process created to execute the command (executor process).
	unsigned long errorCode;	// used to store an "error code" when a system call fails
	std::string errorMsg;		// used to store an "error message"

	// command execution general result
	MktrCmd_AppErrorCode appErrorCode;

	// PID of the executor process
	int cmdExecutorPid;


    /** @brief writeHeader method
     *
     *	write and header string on file output
     *
     *	@param[in] FILE **
     *	@param[in] string
     *	\return boolean
     */
    bool writeHeader(FILE **, std::string );


    /** @brief execute_cmd_helper method
	 *
	 *	Creates a process for executing the command "cmd". The command is executed using "sh",
	 *	the Standard Command Language Interpreter; if the parameter "dir" is other than "" (empty string), the new process sets
	 *	"dir" as working directory.  The standard output is redirected to the file  "fdOut"  and standard error is redirected
	 *	to the file "fdErr". If "timeout" > 0, the process is killed if it doesn't terminate in "timeout" seconds.
	 *
	 *	@param[in] const string & cmd
	 *	  the command that has to be executed.
	 *	@param[in] const string & dir
	 *	  if other than "", specifies the working directory.
	 *	@param[in] int fdOut
	 *	  file descriptor of the file to which "standard output" must be redirected.
	 *	@param[in] int fdErr
	 *	  file descriptor of the file to which "standard error" must be redirected.
	 *	@param[in] int timeout
	 *	  if greater than 0, indicates a timeout (in seconds) for the command execution. If the command is not executed in
	 *	  timeout seconds, the process created to execute the command is killed.
    */
    bool execute_cmd_helper(const std::string & cmd, const std::string & dir, int fdOut,  int fdErr, int timeout, const char *userName, const char *groupName);


    /** @brief terminateProcessTree method
	 *
	 *	Terminates the processes in the process tree rooted at the specified process id. The process tree is visited
	 *	in post-order and for every process found, a SIGKILL signal is sent to terminate it.
	 *
	 *	@param[in] int rootPID
	 *	  the pid of the root of the process tree.
	 *	\return boolean
    */
    bool terminateProcessTree(int rootPID);

    // other helper methods
    bool buildPostOrderPTreeList(int rootPid, std::list<int> & outPidList);
    bool terminateProcessList(const std::list<int> & pidlist);
};

#endif /* MKTRCMD_H_ */
