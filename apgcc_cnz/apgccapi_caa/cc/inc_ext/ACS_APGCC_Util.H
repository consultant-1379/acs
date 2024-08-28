/*
 * ACS_APGCC_Util.H
 *
 *  Created on: Apr 6, 2010
 *      Author: root
 */
//***                         Porting                      ***
//       REV NO          DATE            NAME    DESCRIPTION
//       PR_01           10-04-15        AI      First version
//*****************************************************************************

#ifndef ACS_APGCC_UTIL_H_
#define ACS_APGCC_UTIL_H_

#pragma once
//#pragma warning (disable:4786)  // Disable: identifier truncated in debug mode

#ifndef INCLUDED_IOSTREAM
#  include <iostream>
#  define INCLUDED_IOSTREAM
#endif

#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

#ifndef INCLUDED_SET
#  include <set>
#  define INCLUDED_SET
#endif

#ifndef ACE_OS_H
#  include "ace/OS.h"
#endif
#  include "ACS_APGCC_OS.H"
#  include "ACS_APGCC_EventCodes.H"
#  include "ACS_APGCC_ErrorCodes.H"
#  include "ACS_APGCC_Texts.H"

/*PR_01 BEGIN*/
//Deleted for porting #ifndef WIN32
/*PR_01 END*/
extern "C" {
#  ifndef INCLUDED_UNISTD_H
//ADDs for ACElib porting
#	  include "ace/OS_NS_unistd.h"
//#    include <unistd.h>
//end ADDs
#    define INCLUDED_UNISTD_H
#  endif
#  ifndef INCLUDED_SYS_TYPES_H
#    include <sys/types.h>
#    define INCLUDED_SYS_TYPES_H
#  endif
#  ifndef INCLUDED_SYS_STAT_H
//ADDs for ACElib porting
#	include	"ace/OS_NS_sys_stat.h"
//#	include <ACE.h>
//#    include <sys/stat.h>
#    define INCLUDED_SYS_STAT_H
#  endif
}
/*PR_01 BEGIN*/
//Deleted for porting #endif

#include "ACS_APGCC_Common_Define.h"
#include "ace/config-macros.h" // for DWORD
#include "ace/ACE.h"
#include <sys/sysinfo.h>
/*PR_01 END*/
// General definitions
//#define MCCCXCNAME      "MCS/CXC137429"
//#define MCCLIB1         "libmcs_mcc_r1a_dmdn6.dll"
//#define MCCDLIB         "libmcs_mcc_r1a_dmdn6d.dll"

//#define MCCCXCNAME      "MCS/CXC1371130"             //new CXC number for MCC-apg43

#define ACS_APGCCCXCNAME      "ACS_APGCC/CXC1371222"             //new CXC number for MCC-Blade
#define MCCLIB1         "libmcs_mcc_r1a_dmdn71.dll"  //changed from 6 to 71  -apg43
#define MCCDLIB         "libmcs_mcc_r1a_dmdn71d.dll" //changed from 6 to 71  -apg43


// Defintions used by the log functions MCS::log*().
#define MCS_LOGFILE        "trace.log"
#define MAXLOGSIZE         200000
#define MAXBACKUPFILES     2
using namespace  std;
class ACS_TRA_trace;
//-------------------------------------------------------
// Converts wide std string to ascii character std string
/************************/
/* PR_01 BEGIN*/
/*****Redesigned macro**/
/*char* ab=new char[ws.length()+1]; \
	                     int len; \
								as = ""; \
								len=WideCharToMultiByte(CP_ACP,0,ws.c_str(),ws.length(),ab,ws.length()+1,NULL,NULL); \
								ab[len] = 0; \
								as = ab; \
								delete [] ab; \*/

#define WS2AS(ws,as) {  	std::string s(""); \
		s.assign(ws.begin(), ws.end()); \
		as =s.c_str();\
}



// Converts ascii std string to wide character std string
/* WCHAR* wb=new WCHAR[as.length()+1]; \
	                     int len; \
								ws = L""; \
								len=MultiByteToWideChar(CP_ACP,0,as.c_str(),as.length(),wb,as.length()+1); \
								wb[len] = L'\0'; \
								ws = wb; \
								delete [] wb; \*/
#define AS2WS(as,ws) {	std::wstring widestr = std::wstring(as.begin(), as.end()); \
		ws =widestr.c_str(); \
}

//-------------------------------------------------------
// Adds for porting


// Ex class ACS_APGCC_Export APGCC
//
/************************/
/* PR_01 END*/
class ACS_APGCC
{
public:
	//ACS_APGCC();

	enum {
		CP_EX = 0,
		CP_SEPSB = 1
	};

	enum { MAXTRACEMSGLEN = 300 };

	typedef std::set<char> CHARSET;

	// Description:
	//    Returns true if the user is a member of the given (wide) group.
	// Parameters:
	//    user           Name of user (in ascii)
	//    group          Name of grou (in WIDE characters)
	// Return value:
	//    true           User is a member
	//    false          User is not a member
	// Additional information:
	//    Example:
	//       if ( MCS::isMemberOf("Administrator",L"Administrators") )
	//          cout << "is member" << endl;
	//       else
	//          cout << "is not a member" << endl;
	static bool isMemberOf(const char* user, LPCWSTR group)__attribute__((deprecated));

	// Description:
	//    Tries to kill the remote process.
	// Parameters:
	//    hProcess       Handle to the remote process's handle.
	// Return value:
	//    true           Succeeded
	//    false          Failed
	// Additional information:
	//    Uses LoadLibInRemoteProcess().
	static bool KillRemoteProcess(ACE_HANDLE hProcess)__attribute__((deprecated));

	static DWORD GetLogoffTimeoutValue(const std::string& user="")__attribute__((deprecated));

	// Description:
	//    Tries to load the given DLL library into the remote process.
	// Parameters:
	//    hProcess       Handle to the remote process's handle.
	//    dllName        A valid path to the DLL to load.
	// Return value:
	//    true           Succeeded
	//    false          Failed
	// Additional information:
	//    Used by  KillRemoteProcess().
	static bool LoadLibInRemoteProcess(ACE_HANDLE hProcess, const char* dllName)__attribute__((deprecated));


	// Description:
	//    Returns name of current service or process.
	// Parameters:
	//    processName    Contains process name after call.
	//                   Must be able to contain atleast 32 characters.
	// Return value:
	//    -
	// Additional information:
	//    -

	//static DWORD GetReleaseTimeoutValue();
	// Description:
	//    Returns the release timeout value used for MML sessions.
	//    CP text: TIME OUT
	// Parameters:
	//    -
	// Return value:
	//    DWORD       Log off timeout in seconds.
	// Additional information:
	//    -
	static void getProcessName(std::string *processName);

	// Description:
	//    Returns name of current service or process.
	// Parameters:
	//    processName    Contains process name after call.
	//                   Must be able to contain atleast 32 characters.
	// Return value:
	//    -
	// Additional information:
	//    DEPRECATED
	static void getProcessName(char* processName)__attribute__((deprecated));


	// Description:
	//    Returns the log off timeout value used for MML sessions for
	//    the specified user.
	//    CP text: LOGGED OFF
	// Parameters:
	//    user        Name of user to fetch timeout value for.
	// Return value:
	//    DWORD       Log off timeout in seconds.
	// Additional information:
	//    An empty user name returns the timeout value for the Default user.

	//static DWORD GetLogoffTimeoutValue(const std::string& user="");


	// Description:
	//    Switch the trace mode active or passive.
	// Parameters:
	//    active      If true, tracings are printed to stdout
	//                in Debug mode.
	// Return value:
	//    -
	// Additional information:
	//    DEPRECATED
	static void set_trace_mode(bool active)__attribute__((deprecated));


	// Description:
	//    Format a message to the thread-safe MCS trace mechanism.  Valid
	//    options (prefixed by '%', as in printf format strings) include:
	//    'c': print a character
	//    'i', 'd': print a decimal number
	//    'e', 'E', 'f', 'F', 'g', 'G': print a double
	//    'o': print as an octal number
	//    'P': print out the current process id
	//    'p': print out the appropriate error text from GetLastError()
	//    's': print out a character string
	//    'S': print out a standard string
	//    'T': print timestamp in hour:minute:sec:usec format.
	//    'D': print timestamp in month/day/year hour:minute:sec:usec format.
	//    't': print thread id (1 if single-threaded)
	//    'u': print as unsigned int
	//    'X', 'x': print as a hex number
	//    '%': print out a single percent sign, '%'
	//
	// Parameters:
	//    tracePoint     ACS Trace point
	//    format_str     Format string
	//    ...            Any data values
	// Return value:
	//    -
	// Additional information:
	//	DEPRECATED
	//    Note that the trace string is prefixed with the tread id as "(%t) "
	//    if %t is not given.
	//    The trace printout is printed both to stdout and ACS TRA in Debug mode,
	//    but only to ACS TRA if in Release.
	static void trace(ACS_TRA_trace& tracePoint, const char* format_str, ...);

	// Description:
	//    Returns true if the current node is the active one.
	// Parameters:
	//    node_name   Name of node to check.
	//                If NULL, checks the current node.
	// Return value:
	//    bool        true - Active node
	//                false- Passive node
	// Additional information:
	//    Returns true also if no cluster is available.
	//static bool is_active_node(const string node_name="");

	// Description:
		//    Returns true if the current node is the active one.
		// Parameters:
		//    node_name   Name of node to check.
		//                If NULL, checks the current node.
		// Return value:
		//    bool        true - Active node
		//                false- Passive node
		// Additional information:
		//	DEPRECATED
		//    Returns true also if no cluster is available.
	static bool is_active_node(const char* node_name=0);

	// Description:
	//    Returns own AP number
	// Parameters:
	//    -
	// Return value:
	//    int            AP number 1-?.
	// Additional information:
	//    -
	//static int get_ap_no(); //REPLACED BY CS API

	// Description:
	//    Returns own AP node name
	// Parameters:
	//    -
	// Return value:
	//    char         AP node 'a', 'b'...
	// Additional information:
	//    -
	//static char get_ap_node(); //REPLACED BY CS API


	// Description:
	//    Expands name of network share if found
	// Parameters:
	//    path     Path (share) to expand
	// Return value:
	//    -
	// Additional information:
	//    -
	/*static void expandShare(std::string& path);*/ // commented for apg43

	// Description:
	//    Get next valid number from a range of numbers.
	// Parameters:
	//    range       String which includes a range of numbers.
	//                Valid separators are comma (,), dash (-)
	//                and double periods (..). A range of numbers
	//                is indicated by a dash or double periods, as in
	//                "1,2,4-6,8..10".
	//    value       Fetched value. Only valid if the function returns true.
	//    idx         Index variable used internally.
	//                May not be changed by the caller.
	// Return value:
	//    bool        true  - A valid value is returned.
	//                false - No more values are present.
	// Additional information:
	//    DEPRECATED
	static bool fchnum(const char* range, int& value, int& idx)__attribute__((deprecated));

	// Description:
	//    Get next valid number from a range of numbers.
	// Parameters:
	//    range       String which includes a range of numbers.
	//                Valid separators are comma (,), dash (-)
	//                and double periods (..). A range of numbers
	//                is indicated by a dash or double periods, as in
	//                "1,2,4-6,8..10".
	//    value       Fetched value. Only valid if the function returns true.
	//    idx         Index variable used internally.
	//                May not be changed by the caller.
	// Return value:
	//    bool        true  - A valid value is returned.
	//                false - No more values are present.
	// Additional information:
	//    -
	static bool fchnum(const std::string arg, int& value, int& idx);

	// Description:
	//    Check the range of numbers for syntax correctness.
	//    This function can be used prior to fchnum() to check if
	//    the range is valid.
	// Parameters:
	//    range       String which includes a range of numbers.
	//                Valid separators are comma (,), dash (-)
	//                and double periods (..). A range of numbers
	//                is indicated by a dash or double periods, as in
	//                "1,2,4-6,8..10".
	//    min         The minimum value that is allowed in the range.
	//    max         The maximum value that is allowed in the range.
	// Return value:
	//    bool        true  - The range is syntaxically correct.
	//                false - Syntax/Format error.
	// Additional information:
	//    -
	static bool checkRangeFormat(const std::string& range, int min, int max);


	// Description:
	//    Converts an ascii string to a DWORD.
	// Parameters:
	//    s              Value to convert.
	// Return value:
	//    DWORD            Converted value.
	// Additional information:
	//    -
	static DWORD atod(const char* s)__attribute__((deprecated));

	// Description:
		//    Converts an ascii string to a ACE_UINT32.
		// Parameters:
		//    s              Value to convert.
		// Return value:
		//    ACE_UINT32            Converted value.
		// Additional information:
		//    DEPRECATED
	static ACE_UINT32 atod(const std::string& s);


	// Description:
	//    Converts an ascii string to an integer.
	// Parameters:
	//    s              Value to convert.
	// Return value:
	//    int            Converted value.
	// Additional information:
	//    DEPRECATED
	static int atoi(const char* s)__attribute__((deprecated));

	// Description:
	//    Converts an ascii string to an integer.
	// Parameters:
	//    s              Value to convert.
	// Return value:
	//    int            Converted value.
	// Additional information:
	//    -
	static int atoi(const std::string& s);


	// Description:
	//    Converts an integer to an ascii string.
	// Parameters:
	//    n              Value to convert.
	//    width          Indicates how long the resulting
	//                   string should be. Zeros ('0') are inserted
	//                   preceeding the data up to the given width.
	// Return value:
	//    Numeral string.
	// Additional information:
	//    Uses a internal static variable to hold
	//    the resulting string.
	static const char* itoa(int n, int width=0);

	// Description:
	//    Converts an integer to an ascii string.
	// Parameters:
	//    n              Value to convert.
	//    width          Indicates how long the resulting
	//                   string should be. Zeros ('0') are inserted
	//                   preceeding the data up to the given width.
	// Return value:
	//    Numeral string.
	// Additional information:
	//    DEPRECATED
	static void itoa(char* outStr, int n, int width)__attribute__((deprecated));

	// Description:
	//    Converts an integer to an ascii string.
	// Parameters:
	//    n              Value to convert.
	//    width          Indicates how long the resulting
	//                   string should be. Zeros ('0') are inserted
	//                   preceeding the data up to the given width.
	// Return value:
	//    Numeral string.
	// Additional information:
	//    -
	static void itoa(std::string &outStr, int inputInteger);

	// Description:
	//    Converts an integer to an ascii string in hex format.
	// Parameters:
	//    n              Value to convert.
	//    width          Indicates how long the resulting
	//                   string should be. Zeros ('0') are inserted
	//                   preceeding the data up to the given width.
	// Return value:
	//    -
	// Additional information:
	//    Uses a internal static variable to hold
	//    the resulting string.
	static const char* itoa_hex(int n, int width=0);




	/*=================================================================== */
	/**
	@brief		Returns a formatted string with date and time.

				Returns a formatted string with date and time.
				See strftime() for format options.
				Uses a internal static variable to hold
				the resulting string.
				Example code:
					int main(){
						string mystr=string(ACS_APGCC::formatDateTime("%Y-%m-%d-%I:%M%S%p"));
						puts (mystr.c_str());
					    return 1;
					}

	@par		Deprecated
					never
	@pre		none
	@post		none
	@param		format
					Format orders
	@return       const char*    Formatted string.
         @exception   none
	 */
	/*=================================================================== */
	static const char* formatDateTime(const std::string& format);

	// Description:
	//    Get the text that corresponds to the text value.
	// Parameters:
	//    text           MCS text value
	//
	// Return value:
	//    const char*    Returns a string that explains the text code.
	//
	// Additional information:
	// 		DEPRECATED
	//    See MCS_Texts.H for available texts.
	static const char* getText(const ACS_APGCC_Text text)__attribute__((deprecated));

	// Description:
	//    Get the error text that describes the error code.
	// Parameters:
	//    error          MCS error code
	//
	// Return value:
	//    const char*    Returns a string that explains the error.
	//
	// Additional information:
	//    See MCS_ErrorCodes.H for all defined errors.
	static const char* getErrorText(const ACS_APGCC_ErrorCode error);

	// Description:
	//    Get the system error code for the last occurred error.
	// Parameters:
	//    -
	// Return value:
	//    int            Return error code.
	//
	// Additional information:
	//    Win32: Calls GetLastError().
	//    UNIX : Returns errno.
	static  int getLastError();

	// Description:
	//    Get the system error text that describes the error code.
	// Parameters:
	//    error          Error code from getLastError().
	//                   Calls getLastError() if no error code is given.
	//
	// Return value:
	//    const char*    Returns a string that explains the error.
	//
	// Additional information:
	//    -
	static const char* getSystemErrorText(int error=-1);

	// Description:
	//    Get the system's memory usage in percent.
	// Parameters:
	//    -
	// Return value:
	//    int            Percent of used system memory.
	//
	// Additional information:
	//    DEPRECATED
	static int getMemoryUsage()__attribute__((deprecated));



	enum DirAccess {
		DIR_ACCESS_MODE = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
	};

	// Description:
	//    Create given directories.
	// Parameters:
	//    dirs           Directories to create, separated
	//                   by a semicolon. "$HOME/data;/opt;$LOGS/PDS".
	//
	// Return value:
	//    Returns zero on success, or -1 on failure.
	//
	// Additional information:
	//    All directories in the given path will be created if
	//    they do not exists. For example, if path "HOME/dir/log" is
	//    to be created, directory "HOME", "HOME/dir" and finally
	//    "HOME/dir/log" is created.
	//    Different paths must be separated with semicolon.
	//    Directories in a path must be separated with either "/" or "\".
	static int create_directories(const char* dirs, mode_t access=DIR_ACCESS_MODE);


	// Description:
	//    Checks if the given file is a Console application.
	// Parameters:
	//    appPath       Complete path to file to check.
	//
	// Return value:
	//    Returns true if the file is a console application or false otherwise.
	//
	// Additional information:
	//    Expands the file path with MCS_Path.
	//    Check for ERROR_FILE_NOT_FOUND to differ between a non-console app and
	//    an non-existing file.
	//    Note that internal commands to cmd.exe, such as dir and type, returns false.
	static bool isConsoleApp(const char* appPath);

	// Description:
	//    Checks if the given file is a binary (i.e. can be executed).
	// Parameters:
	//    filePath       Complete path of file to check.
	//
	// Return value:
	//    Returns true if the file can be executed or false otherwise.
	//
	// Additional information:
	//		DEPRECATED
	//    Expands the file path with ACS_APGCC_Path.
	static bool isFileExecutable(const char* filePath)__attribute__((deprecated));


	// Description:
	//    Expand the program path in response to environment variables,
	//
	// Parameters:
	//    filePath       Path to expand.
	//
	// Return value:
	//    -
	//
	// Additional information:
	//		DEPRECATED
	//    Expands the file path using ACS_APGCC_Path.
	static void expandFilePath(std::string& filePath)__attribute__((deprecated));


	// Description:
	//    Given a service name this functions returns the port number
	//    using the file %SystemRoot%/System32/drivers/etc/services.
	// Parameters:
	//    serviceName    Name of service to obtain port
	//                   number for.
	//
	// Return value:
	//    0              Failed to retrieve port number
	//    >0             Port number
	//
	// Additional information:
	//
	// Syntax of the file "services" must conform to:
	//
	// <port name1>  <port number1>/<port type>
	// <port name2>  <port number2>/<port type>
	//  ...  ...      ...  ...       ...  ....
	// <port nameX>  <port numberX>/<port type>
	//
	// Comments starts with '#'. Blanks or tabs are used to separate tokens.
	//
	//
	// Example:
	//
	// # Start of lines belonging to CXC137442 P1K TOCAPBIN
	// OCS_ocpEcho	   14007/ocp
	// # End of lines belonging to CXC137442
	// #
	// ACS_ds		   14000/ocp
	// ACS_dsIntern	14001/tcp
	static int getPortNoForService(const std::string serviceName);

	// Description:
	//    Given a service name this functions returns the port number
	//    using the file %SystemRoot%/System32/drivers/etc/services.
	// Parameters:
	//    serviceName    Name of service to obtain port
	//                   number for.
	//
	// Return value:
	//    0              Failed to retrieve port number
	//    >0             Port number
	//
	// Additional information:
	//		DEPRECATED
	static int getPortNoForService(const char* serviceName) __attribute__((deprecated));


	// Description:
	//    Returns true if the given port already is defined in the
	//    /etc/services file, or otherwise improper to use.
	// Parameters:
	//    port           Port number to check.
	//
	// Return value:
	//    true           Port number is defined in /etc/services or
	//                   improper to use.
	//    false          Port number is free use.
	//
	// Additional information:
	//    -
	static bool isPortReserved(int port);


	// Description:
	//    Returns true if the given IP address part number is valid.
	//    The number must be 0>= and <=255.
	// Parameters:
	//    numstr         IP address part number to check.
	//
	// Return value:
	//    true           Valid IP address part number.
	//    false          Not valid.
	//
	// Additional information:
	//    -
	static bool isValidIpAddressNumber(const std::string& numstr);

	// Description:
	//    Returns true if the given IP address is a valid one.
	//    Performs range checks as well as host lockup.
	// Parameters:
	//    ipaddr         IP address to check.
	//    portSep        Port separator
	//                   Port number is not allowed (and not checked) if portSep
	//                   is an empty string.
	//                   The string portSep should contain the string that
	//                   separates the ip address from the port number.
	//                   Example:
	//                      "1.2.3.4:9876" gives portSep = ":"
	//                      "1.2.3.4 9876" gives portSep = " "
	//
	// Return value:
	//    true           Yepp, valid IP address (and port number).
	//    false          Not valid IP address.
	//
	// Additional information:
	//    Checks performed:
	//       - Ranges
	//       - Host (ip address) lockup
	//       - Port cannot be defined in /etc/services.
	static bool isValidIpAddress(const std::string& ipaddr, const std::string& portSep="");

	// Description:
	//    Returns true if the given string contains a valid IP mask,
	//    on the form "*.12?.*.34" or similair.
	// Parameters:
	//    maskStr        IP mask to check.
	//
	// Return value:
	//    true           Yepp, valid IP mask.
	//    false          Not valid IP mask.
	//
	// Additional information:
	//    -
	static bool isValidIpMask(const std::string& maskStr);

	// Description:
	//    Returns true if the given string contains valid hex numbers.
	// Parameters:
	//    str            String to check.
	//
	// Return value:
	//    true           String contains valid hex numbers.
	//    false          Not valid hex numbers.
	//
	// Additional information:
	//    -
	static bool isValidHexChars(const std::string& str);

	// Description:
	//    Returns number of found characters that equals the given one.
	// Parameters:
	//    str            String to search in.
	//    ch             Character to search for.
	//
	// Return value:
	//     0             No character 'ch' was found.
	//    >0             Found number of 'ch' characters.
	//
	// Additional information:
	//    -
	static int countChar(const std::string& str, char ch);

	// Description:
	//    Get the name of the remote host.
	// Parameters:
	//    fd     Socket file descriptor
	//           If the file descriptor is not
	//           a valid socket, the remote host
	//           will be fecthed from the envrionment
	//           variable $REMOTEHOST, if $REMOTEHOST
	//           doesn't exists the TTY entry in
	//           '/etc/inittab' is read.
	//
	// Return value:
	//    Returns the remote host name or an empty string.
	//
	// Additional information:
	//    -
	static const char* getRemoteHost( ACE_HANDLE fd )__attribute__((deprecated));

	// Description:
		//    Get the name of the remote host.
		// Parameters:
		// -
		//
		// Return value:
		//    Returns the remote host name or an empty string.
		//
		// Additional information:
		//    -
	static const char* getRemoteHost();


	// Description:
	//    Get the name of the local host.
	// Parameters:
	//    -
	// Return value:
	//    Returns the local host name or an empty string.
	//
	// Additional information:
	//    -
	static const char* getLocalHost();

	// Description:
	//    Get the name of the domain for the running process.
	// Parameters:
	//    domainName     Returned domain name.
	//    domainNameSize When calling: Size of domainName
	//                   When returning: Size of returned domain name
	// Return value:
	//    true     Returns the domain name.
	//    false    No domain could be found.
	// Additional information:
	//    DEPRECATED
	static bool getDomainName(char* domainName, DWORD& domainNameSize)__attribute__((deprecated));


	// Description:
	//    Get the login name of the user.
	// Parameters:
	//    -
	// Return value:
	//    const char*    Returns the user name.
	// Additional information:
	//    -
	static const char* getUserLoginName();

	// Description:
	//    Read the user's password from stdin.
	// Parameters:
	//    -
	// Return value:
	//    Returns the user's password.
	// Additional information:
	//    -
	static const char* readUserPassword(const char* prompt);

	// Description:
	//    Make a short delay.
	// Parameters:
	//    delayInSec     Delay in seconds
	//    delayInUsec    Delay in micro seconds
	//
	// Return value:
	//    -
	// Additional information:
	//    Uses a select() statement to block the
	//    indicated time delay.
	static void delay(int delayInSec, int delayInUsec=0);

	// Description:
	//    Converts string to captital characters.
	// Parameters:
	//    Source         Source string
	//
	// Return value:
	//    -
	// Additional information:
	//    -
	static void toUpper(std::string& Source);

	// Description:
	//    Converts string to lower case characters.
	// Parameters:
	//    Source         Source string
	//
	// Return value:
	//    -
	// Additional information:
	//    -
	static void toLower(std::string& Source);

	// Description:
	//    Converts an ASCII string to Wide format.
	// Parameters:
	//    Source         Source string to convert
	//
	// Return value:
	//    Returns the converted wide string
	// Additional information:
	//    DEPRECATED
	static const wchar_t* toWide(const std::string& Source)__attribute__((deprecated));

	// Description:
	//    Converts an Wide string to ASCII format.
	// Parameters:
	//    Source         Source string to convert
	//
	// Return value:
	//    Returns the converted ASCII string
	// Additional information:
	//    DEPRECATED
	static const char* toAscii(const std::wstring& wSource)__attribute__((deprecated));

	// Description:
	//    Removes space characters from the beginning and end
	//    of the source string.
	// Parameters:
	//    Source         Source string
	// Return value:
	//    -
	// Additional information:
	//    Uses isspace() to determine if a character is space or not.
	static void trim(std::string& Source);

	// Description:
	//    Removes space characters from the end
	//    of the source string.
	// Parameters:
	//    Source         Source string
	// Return value:
	//    -logWarning
	// Additional information:
	//    Uses isspace() to determine if a character is space or not.
	static void trimEnd(std::string& Source);

	// Description:
	//     Trims the given MML command from comments and
	//     redundant spaces.
	// Parameters:
	//     mmlCmd        MML command
	// Return value:
	//     -
	// Additional information:
	//     Uses isspace() to determine if a character is space or not.
	//     MML comments starts with "!" and ends with either "!" or ";".
	static void trimMmlCommand(std::string& mmlCmd);

	// Description:
	//    Replace the Pattern found in Source with Target.
	// Parameters:
	//    Source         Source string
	//    Pattern        Pattern string to serach for.
	//     Target        Target string to replace with.
	//     caseSensitive If true, compare with original case not changed.
	//
	// Return value:
	//    true           Pattern was found in Source and replaced with Target.
	//     false         Pattern was NOT found in Source.
	//
	// Additional information:
	//    -
	static bool replace(std::string& Source, const std::string& Pattern, const std::string& Target, bool caseSensitive=false);

	// Description:
	//    Get the string that is before 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is before 'pattern', or
	//    the whole string if 'pattern' cannot be found.
	//
	// Additional information:
	//    -
	static const char* before(const std::string& str, const std::string& pattern);

	// Description:
	//    Get the string that is before 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is before 'pattern', or
	//    the whole string if 'pattern' cannot be found.
	//
	// Additional information:
	//    DEPRECATED
	static const char* before(const char* str, const char* pattern)__attribute__((deprecated));

	// Description:
	//    Get the string that is before the last occurrences of 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	// Return value:
	//    Returns the string that is before the last occurances if 'pattern',
	//    or the source string if 'pattern' cannot be found.
	// Additional information:
	//    -
	static const char* beforeLast(const std::string& str, const std::string& pattern);

	// Description:
	//    Get the string that is before the last occurrences of 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	// Return value:
	//    Returns the string that is before the last occurances if 'pattern',
	//    or the source string if 'pattern' cannot be found.
	// Additional information:
	//    DEPRECATED
	static const char* beforeLast(const char* str, const char* pattern)__attribute__((deprecated));

	// Description:
	//    Get the string that is after 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is after 'pattern', or
	//    an empty string if 'pattern' cannot be found.
	//
	// Additional information:
	//    -
	static const char* after(const std::string& str, const std::string& pattern);

	// Description:
	//    Get the string that is after 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is after 'pattern', or
	//    an empty string if 'pattern' cannot be found.
	//
	// Additional information:
	//    DEPRECATED
	static const char* after(const char* str, const char* pattern)__attribute__((deprecated));

	// Description:
	//    Get the string that is after the last occurances of 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is after the last occurances if 'pattern',
	//    or en empry string if 'pattern' cannot be found.
	//
	// Additional information:
	//    -
	static const char* afterLast(const std::string& str, const std::string& pattern);

	// Description:
	//    Get the string that is after the last occurances of 'pattern'.
	// Parameters:
	//    str            Source string
	//    pattern        Pattern string
	//
	// Return value:
	//    Returns the string that is after the last occurances if 'pattern',
	//    or en empry string if 'pattern' cannot be found.
	//
	// Additional information:
	//    DEPRECATED
	static const char* afterLast(const char* str, const char* pattern)__attribute__((deprecated));

	// Description:
	//    Returns true if the string in Source matches the string in Pattern
	//    The pattern may contain any number of the wild characters '*' and '?'
	//    '?' matches any single character
	//    '*' matches any sequence of charcters (including a zero length sequence)
	//    E.g. '*m?t*i*' will match 'Automatic'
	//
	// Parameters:
	//    Source         Source string
	//    Pattern        Pattern string
	//
	// Return value:
	//    Returns true if the Pattern was found in Source,
	//
	// Additional information:
	//    -
	static bool match(const std::string& Source, const std::string& Pattern);

	// Description:
	//    Get the n:th string from string 'source' that is separated by the characters
	//    in the string 'delimiters'.
	//
	// Parameters:
	//    out            Output string. Returns empty string
	//                   if no item was found.
	//                   The string MUST BE large enough for the largest
	//                   possible item.
	//    source         Source string
	//    delimiters     String with delimiters.
	//    n              n:th item to retrieve. 0 is first item.
	//
	// Return value:
	//    bool           true  - Item was found.
	//                   false - No item was found.
	//
	// Additional information:
	//    -
	static bool item(char* out, const char* source, const char* delimiters, int n);

	// Description:
	//    Get the n:th string that is separated by the characters in the
	//    string delimiters.
	//
	// Parameters:
	//    out            Output string
	//    source         Source string
	//     delimiters    String with delimiters.
	//    n              n:th item to retrieve. 0 is first item.
	//
	// Return value:
	//    true           Item was retrieved.
	//     false         No item was found.
	// Additional information:
	//    WARNING: Can cause access error when used dynamically from the MCC DLL.
	//             This needs to be solved. For now, use the above method instead.
	static bool item(std::string& out, const std::string& source, const std::string& delimiters, int n);

	// Description:
	//    Get the n:th string that is separated by the characters in the
	//    character set cs.
	// Parameters:
	//    out            Output string
	//                   If not STL: The out string must be large enough to
	//                               hold the largest possible item.
	//    source         Source string
	//    cs             Character set of delimiters.
	//    n              n:th item to retrieve. 0 is first item.
	//
	// Return value:
	//    true           Item was retrieved.
	//     false         No item was found.
	//
	// Additional information:
	//    -
	static bool item(char* out, const char* source, const CHARSET& cs, int n);
	static bool item(std::string& out, const std::string& source, const CHARSET& cs, int n);

	// Description:
	//    Return the value of the "name=value" pair from
	//    the given source string.
	//    Multiple name=value-pairs must be separated with the
	//    characters given in the delimiter string.
	//    If no delimiters are given, semicolon(;) is used.
	// Parameters:
	//    value       Returned value string
	//    source      Source string
	//    name        Name indicating the "name=value" pair.
	//    delimiters  Character(s) that separate the name=value-pairs.
	//
	// Return value:
	//    true        Value was retrieved.
	//    false       No value was found.
	//
	// Additional information:
	//    -
	static bool getNameValuePair(std::string& value, const std::string& source, const std::string& name, const std::string& delimiters="");
	static bool getNameValuePair(char* value, const std::string& source, const std::string& name, const std::string& delimiters="");

	// Description:
	//    Return the command code in the given MML command.
	//
	// Parameters:
	//    s           MML command
	//
	// Return value:
	//    MML command code
	//
	// Additional information:
	//    Returns the command code only.
	static const char* getMmlCmdCode(const char* s);

	// Description:
	//    Compares two strings case-insensitive.
	// Parameters:
	//    s1          Source string 1
	//    s2          Source string 2
	// Return value:
	//    -1          s1 < s2
	//     0          s1 == s2
	//     1          s1 > s2
	// Additional information:
	//    See further 'man strcasecmp'.
	static int strcasecmp(const char* s1, const char* s2);
	static int strcasecmp(const std::string& s1, const std::string& s2);

	// Description:
	//    Compares two strings case-insensitive upto
	//    n number of characters.
	//
	// Parameters:
	//    s1          Source string 1
	//    s2          Source string 2
	//
	// Return value:
	//    -1          s1 < s2
	//     0          s1 == s2
	//     1          s1 > s2
	//
	// Additional information:
	//    See further 'man strncasecmp'.
	static int strncasecmp(const char* s1, const char* s2, int n);
	static int strncasecmp(const std::string& s1, const std::string& s2, int n);

	// Description:
	//    Removes all space characters in the string.
	//
	// Parameters:
	//    str         Source string
	//
	// Return value:
	//    -
	//
	// Additional information:
	//    A space character is defined by isspace().
	static void strip_space(char* str);

	// Description:
	//    Removes all preceeding control characters, that are
	//    characters below ascii(space). LF, CR, TAB etc.
	//
	// Parameters:
	//    str         Source string
	//
	// Return value:
	//    -
	//
	// Additional information:
	//    A control character is defined by iscntrl().
	static void strip_pre_ctrl(char* str);

	// Description:
	//    Convert string to upper case.
	// Parameters:
	//    str         Source string
	// Return value:
	//    -
	// Additional information:
	//    -
	static string capstr(string str);

	// Description:
	//    Remove a certain character from the string.
	// Parameters:
	//    str         Source string
	//    ch          Character to remove
	//   len          Length of string
	// Return value:
	//    int         New string length
	// Additional information:
	//    -
	static int  strip_ch(char *str, char ch, int len);
	static int  strip_ch(unsigned char *str, int ch, int len);

	// Description:
	//    Remove a certain character upto the first non-space character.
	// Parameters:
	//    str         Source string
	//    ch          Character to remove
	//   len          Length of string
	//
	// Return value:
	//    int        New string length
	//
	// Additional information:
	//    A space character is defined by isspace().
	static int  strip_pre_ch(char *str, char ch, int len);

	// Description:
	//    Checks if the string contains only spaces.
	// Parameters:
	//    str         Source string
	//
	// Return value:
	//    bool        Returns true if string contains only spaces
	//                otherwise false
	//
	// Additional information:
	//    A space character is defined by isspace().
	static bool is_only_space(const char* str);

	// Description:
	//    Checks if the string contains a valid decimal number.
	// Parameters:
	//    str         Source string
	// Return value:
	//    bool        Returns true if string contains a valid number,
	//                otherwise false
	// Additional information:
	//    A valid digit is defined by isdigit().
	static bool is_number(const char* str);


	// Description:
	//    Log a info/warning/error text in a log file.
	//
	// Parameters:
	//    s     String to log. Can contain formaters, which
	//          in that case, s must be followed by the
	//          items to put in to the string.
	//          Possible formaters are:
	//             %c    Insert a character
	//             %s    Insert a string.
	//             %i    Insert an integer
	//             %x    Insert an integer in hex format
	//             %n    Insert a line break (new line)
	//
	// Return value:
	//    bool        true  - Successful
	//                false - Failed
	// Additional Information:
	//		DEPRECATED
	//    Thread safe.
	static bool logInfo(const char* s, ...)__attribute__((deprecated));
	static bool logWarning(const char* s, ...)__attribute__((deprecated));
	static bool logError(const char* s, ...)__attribute__((deprecated));

	// Description:
	//    Used to create backup files. The current file (fname) is
	//    moved to a backup file when it's size is greater than the
	//    given max size (maxSize).
	//
	// Parameters:
	//    fname             Name of current log file (path/name.ext).
	//    maxSize           Size when the file should be switched.
	//                      The file is then renamed to path/name.1, where
	//                      the name of the extension is controlled by the
	//                      given number of backup files. If the numOfBackupFiles
	//                      is 100, the range of backup files will be:
	//                      path/name.01 to path/name.99. The name.99 is then
	//                      the oldest backup file.
	//    numOfBackupFiles  Number of backup files.
	//                      Must be larger than 1.
	//
	// Return value:
	//    bool        true  - Successful
	//                false - Failed
	// Additional Information:
	//    Thread safe.

	static bool switchFileOnSize(const char* fname, long maxSize, int numOfBackupFiles);

	// Description:
	//    Formats the input buffer as a hex dump into the
	//    output buffer.
	// Parameters:
	//    buf         Data buffer to format
	//    buf_sz      Size of data buffer
	//    obuf        Output buffer; Must be supplied by caller.
	//    obuf_sz     Size of output buffer; To avoid overflow.
	//    ostr        Output string (contains formatted hex dump).
	// Return value:
	//    Size of the number of actually formatted data.
	// Additional information:
	//    -
	static int format_hexdump (const void* buf, int buf_sz, std::string& ostr);
	static int format_hexdump (const void* buf, int buf_sz, char* obuf, int obuf_sz);


	// Description:
	//    Displays a buffer in hex format on stdout.
	// Parameters:
	//    buf         Data buffer
	//   bufsize      Size of the data buffer
	//   text         Text string to print before the
	//                data is printed.
	// Return value:
	//    -
	// Additional information:
	//    For debug purposes only. Requires that OSF_NLOGGING
	//    is undefined.
	static void dump_hex(const char* buf, int bufsize, const char* text=0);
	static void dump_hex(const unsigned char* buf, int bufsize, const char* text=0);


	// Description:
	//    Read item separated by the delimiters from the istream.
	// Parameters:
	//    is             Input stream to read from.
	//    out            Read string.
	//    delimiters     Characters to skip over.
	// Return value:
	//    Returns true if successful, otherwise false.
	// Additional information:
	//    -
	static bool RdItem(std::istream& is, std::string& out, const CHARSET& delimiters);


private:

	ACS_APGCC();       // private constructor to avoid instances of this class.

	// Internal functions

	// Description:
	//    Retrieves the number indicated by the index idx.
	// Parameters:
	//    str         Input string
	//    value       Returned value.
	//    idx         Numeral index
	// Return value:
	//    int         Position of find number.
	// Additional Information:
	//    -
	static int getValue(const std::string& str, int& value, const int& idx);

	// Description:
	//    Writes a time date stamp on the ofstream.
	//    Formatted as "YYYYMMDD HHMMSS".
	// Parameters:
	//    os    ostream
	// Return value:
	//    bool  true  - Successful
	//          false - Failed
	// Additional Information:
	//    -
	static bool writeTimeMark(std::ofstream& os);

	// Description:
	//    Writes name of process and PID value to the ofstream.
	//    Formatted as "PROCESS:PID".
	// Parameters:
	//    os    ostream
	// Return value:
	//    bool  true  - Successful
	//          false - Failed
	// Additional Information:
	//    -
	static bool writeProcessInfo(std::ofstream& os);

	// Used by match().
	static bool Rmatch(const std::string& s, int i, const std::string& p, int j);


#ifdef USE_AEH
	static char* getErrorData( const char* category,
			const char* faultText,
			const char* addinfo );

	static ACS_AEH_EvReport eventMsg;
#endif // SKIP_AEH

};


#endif /* ACS_APGCC_UTIL_H_ */

