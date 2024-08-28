//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Util.cpp - Utility functions.
// .LIBRARY 3C++
// .PAGENAME MCS_EventReport_R1
// .HEADER  MCS MCC
// .LEFT_FOOTER Telefonaktiebolaget LM Ericsson
// .INCLUDE APGC_Util.H

// .COPYRIGHT
//  COPYRIGHT Telefonaktiebolaget LM Ericsson, Sweden 2001.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Telefonaktiebolaget LM Ericsson, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Telefonaktiebolaget LM Ericsson or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//    Contains utility functions.
//strcasecmp
// CHANGES
//
//    RELEASE REVISION HISTORY
//
//    DATE        NAME        DESCRIPTION
//    2000-??-??  QABSJEN     First release
//    2001-09-05  QABSJEN     Rewrote GetLogoffTimeoutValue() to replace the
//                            use of MCS_Registry.
//    2001-09-07  QABSJEN     Corrected getPortNoForService() since getservbyname()
//                            returns port number in Network byte order.
//    2001-09-11  QABSJEN     Changed is_active_node() to check for cluster group "Disk Group".
//    2001-09-17  QABSJEN     Corrected handle leak in MCS::getDomainName().
//    2001-09-18  QABSJEN     Use of thread-local-storage (TLS) to make string functions
//                            and other thread safe.
//    2001-09-21  QABSJEN     Added isMemberOf().
//    2001-09-25  QABSJEN     Changed isMemberOf() to return true if NetUserGetLocalGroups()
//                            fails, and added trace point.
//    2002-11-14  QABSJEN     Corrected create_directories().
//                            Fault code ERROR_ALREADY_EXISTS wasn't handled correctly.
//    2003-08-11  UABLAN      Added text to Texts[94] and changed text to ErrorTexts[30] to
//                            "Rejected due to lack of available AP system memory".
//                            Changed second parameter in NetShareGetInfo(..) to be LMSTR
//                            instead of (char*) to be able to compile with Platform SDK 2001 nov.
//    2004-09-23  QSHAPAT	  Changed the return code in - int MCS::create_directories()
//							  to resolve the error ERROR_ALREADY_EXISTS in ADH_ADM : TR Ref:HE90812
//    2005-02-16  QSHAPAT	  Changed the code for timeoutValue in GetReleaseTimeoutValue() and
//							  GetLogoffTimeoutValue to allow MCS_MML_Release_Timeout more than 300Sec
//    2006-08-09  QCHAROH     Commented the definition of method expandShare() since
//                            shares are no longer used-apg43.Have commented the
//                            declaration of expanShare() as well in MCS_Util.h
//*************************************************************************************
//***                         Porting                      ***
//       REV NO          DATE            NAME    DESCRIPTION
//       PR_01           10-04-15        AI      First version
//*****************************************************************************
#include <map>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <ace/INET_Addr.h>
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_Path.H"
#include "ACS_CC_Types.h"
#include "acs_prc_api.h"
#include "ACS_DSD_Server.h"
#include "ACS_DSD_MacrosConstants.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <string.h>
#ifdef WIN32
// Required libs are: clusapi.lib netapi32.lib
#pragma comment(lib, "clusapi.lib")
#pragma comment(lib, "netapi32.lib")
using namespace std;
#else
extern "C" {
#include <sys/types.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <pwd.h>
#include <utmpx.h>
}
#endif   //WIN32

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

// Trace point
//ACS_TRA_trace traUtil = ACS_TRA_DEF ("MCS_Util",   "C200");


// Definition of thread safe variables

//__declspec(thread) static char g_thr_str[1024];
static char* g_pthr_str;
#define G_PTHR_LEN 128
static char g_pthr_str_arr[G_PTHR_LEN];

// Definition of Static variables
#include "ACS_TRA_trace.h"
static ACE_Thread_Mutex util_lock;
static ACE_Thread_Mutex log_lock;
static ACE_Thread_Mutex dump_lock;
ACE_Thread_Mutex __g_mcs_trace_lock;
char __g_mcs_trace_msg[ACS_APGCC::MAXTRACEMSGLEN];
bool __g_mcs_trace_on = true;
static const char* NullString = "\0";



using namespace std;
using namespace acs_dsd;

// Definition Text strings
//========================

#define No_Of_Texts 	 161	// <<== make sure to update this!!
#define No_Of_ErrTexts 	 123	// <<== make sure to update this!!
static const char* ErrorTexts[] =
{
		"OK",    					//  0
		"Error when executing ( general fault )",    		//  1
		"Incorrect usage",    		//  2
		"3",    					//  3
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"17",
		"18",
		"Ordered",                                // 19
		"Illegal option",									// 20
		"Illegal combination of options",			// 21
		"Unreasonable value",							// 22
		"Operation not allowed",						// 23
		"Authorization failure",						// 24
		"25",
		"26",
		"27",
		"28",
		"29",
		"30",
		"31",
		"32",
		"33",
		"34",
		"35",
		"36",
		"37",
		"38",
		"39",
		"40",
		"41",
		"42",
		"43",
		"44",
		"45",
		"46",
		"47",
		"48",
		"Configuration file not found",           // 49
		"File does not exist",							// 50
		"File already exists",							// 51
		"Cannot access file",                     // 52
		"Cannot open or create file",             // 53
		"File not executable",                    // 54
		"Cannot write to file",                   // 55
		"Cannot write to temporary file",         // 56
		"Cannot open or create temporary file",   // 57
		"Execution stopped at line",              // 58
		"59",
		"60",
		"61",
		"62",
		"63",
		"64",
		"65",
		"66",
		"67",
		"68",
		"69",
		"70",
		"71",
		"72",
		"73",
		"74",
		"75",
		"76",
		"77",
		"78",
		"79",
		"80",
		"81",
		"82",
		"83",
		"84",
		"85",
		"86",
		"87",
		"88",
		"89",
		"90",
		"91",
		"92",
		"93",
		"94",
		"95",
		"96",
		"97",
		"98",
		"99",
		"100",
		"101",
		"102",
		"103",
		"104",
		"105",
		"106",
		"107",
		"108",
		"109",
		"110",
		"111",
		"112",
		"AP IS NOT DEFINED",   //113
		"Command not executed, AP backup in progress",	//114
		"Illegal command in this system configuration",    //115
		"Illegal option in this system configuration",    //116
		"Unable to connect to server", //117
		"CP is not defined",         //118
		"Command partly executed",           //119
		"ILLEGAL COMBINATION OF OPTIONS", //120
		"UNREASONABLE VALUE",             //121
		"ILLEGAL OPTION IN CONFIGURATION",   //122
		NULL

};


static const char* Texts[] =
{
		"EXECUTED",                      //  0
		"ORDERED",
		"<",
		"NOT ACCEPTED",
		"PARTLY EXECUTED",
		"INHIBITED",
		"COMMAND UNKNOWN",
		"COMMAND RESTRICTED",
		"FUNCTION BUSY",
		"FORMAT ERROR",
		"UNREASONABLE VALUE",            // 10
		"FAULT CODE",
		"END",
		"BUFFER EXCEEDED",
		"TIME OUT",
		"SYNTAX FAULT",
		"PARAMETER BLOCK TOO LONG",
		"EOT DURING PRINTOUT",
		"NOT EXECUTED",
		"FAULT INTERRUPT",
		"DEVICE ERROR",                  // 20
		"FILE NOT FOUND",
		"EOT REQUESTED",
		"FULL DEVICE QUEUE",
		"VOLUME ERROR",
		"FILE NAME ERROR",
		"CODE ERROR",
		"ILLOGICAL",
		"CANCELLED",
		"BLOCK SIZE ERROR",
		"DATA ERROR",                    // 30
		"TAPE END",
		"NEXT TAPE, PLEASE",
		"BUFFER CONGESTION",
		"WAIT",
		"DUMP RESTRICTED",
		"NOT LOGGED",
		"COMMAND EXECUTED",
		"COMMAND NOT EXECUTED",
		"FILE ERROR",
		"MEDIA END",                     // 40
		"COMMAND LOG NOT AVAILABLE",
		"42",
		"43",
		"44",
		"45",
		"46",
		"47",
		"48",
		"49",
		"COMMAND RESTRICTED TOWARDS CP GROUP",
		"51",
		"52",
		"53",
		"INTERRUPTED BY SYSTEM RESTART",
		"SYSTEM RESTARTED",
		"56",
		"INTERRUPTED BY FORLOPP RELEASE",
		"REENTRY TO COMMAND ",
		"59",
		"60",
		"INTERRUPTED COMMAND",
		"AUTHORIZATION FAILURE",
		"TOO MANY LOGIN ATTEMPTS",
		"CP NOT OBTAINABLE",                // 64
		"CP STANDBY NOT OBTAINABLE",        // 65,
		"INCOMPLETE CP GROUP",				// 66
		"67",
		"68",
		"69",
		"70",
		"71",
		"72",
		"73",
		"PASSWORD HAS EXPIRED",             // 74
		"NEW PASSWORD: ",                   // 75
		"CONFIRM NEW PASSWORD: ",           // 76
		"INVALID PASSWORD",                 // 77
		"ENTER IO DEVICE NUMBER: ",         // 78
		"IO DEVICE IS OCCUPIED",            // 79
		"USERCODE: ",                       // 80
		"PASSWORD: ",
		"Continue ?\r\n<",
		"CONNECTION TIMED OUT",
		"CONNECTION INTERRUPTED",
		"MML SERVER UNREACHABLE",
		"IO MML, PRINTOUT DESTINATION FAULT",
		"LOGGED OFF",
		"Connect to CP-EX?  yes/[no]: ",
		"YOU MAY NOW ENTER: APLOC, PTCOI OR EXIT COMMAND", //89
		"Additional Information:",          // 90
		"Execution stopped at line",
		"MML SERVER NOT PROPERLY DEFINED",
		"Use option -l to restart file reading",
		"REJECTED DUE TO LACK OF AVAILABLE AP SYSTEM MEMORY",  // 94
		"NESTED APLOC",							// 95
		"WARNING\nACTIONS TOWARDS INCOMPLETE CP GROUP COULD AFFECT DATA CONSISTENCY\n",  //96
		"DOMAIN: ",                         // 97
		"IO DEVICE IS RESERVED",            // 98
		"99",
		"AP IO LOG SERVER FAULT",				// 100
		"IO MML SERVER FAULT",
		"AP ALI FAULT",
		"EXTERNAL ALARM SERVER FAULT",
		"ALARM DISPLAY SERVER FAULT",
		"ALARM INTERFACE SERVICES FAULT",
		"IO STORAGE SPACE WARNING",
		"107",
		"IO PRINTOUT DESTINATION FAULTY",
		"109",
		"PDS LOG DIRECTORY",                // 110
		"PDS PROGRAM DIRECTORY",
		"INSTALLED PDS PROGRAMS",
		"113",
		"114",
		"115",
		"116",
		"117",
		"118",
		"119",
		"Failed to execute command",        // 120
		"The command cannot be used with this function",
		"Execution failed due to program exception",
		"Too many start attempts",
		"124",
		"125",
		"126",
		"127",
		"128",
		"129",
		"EXTERNAL ALARM",                   // 130
		"EXTERNAL ALARM RECEIVER FAULT",
		"EXTERNAL ALARM RECEIVER BLOCKED",
		"133",
		"134",
		"135",
		"136",
		"137",
		"138",
		"139",
		"140",
		"141",
		"142",
		"143",
		"144",
		"145",
		"146",
		"147",
		"148",
		"149",
		"None",                          	// 150
		"151",
		"152",
		"153",
		"154",
		"155",
		"156",
		"157",
		"158",
		"159",
		"160",
		NULL
};


// Definition Error Texts
//=======================

#define No_Of_ErrorTexts 115	// <<== make sure to update this!!

static const char* ErrorTexts_[] =
{
		"",                                       // 0
		"Error when executing ( general fault )", // 1
		"Incorrect usage",                        // 2
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"17",
		"18",
		"Ordered",                                // 19
		"Illegal option",									// 20
		"Illegal combination of options",			// 21
		"Unreasonable value",							// 22
		"Operation not allowed",						// 23
		"Authorization failure",						// 24
		//	"CPT not properly installed",					// 25
		//	"PDS program not found or not installed",	// 26
		"Partly executed",								// 27
		"Operation timed out",							// 28
		"Illegal state for this operation",       // 29
		"Rejected due to lack of available AP system memory", // 30
		//	"PDS program failed to initiate",         // 31
		"Illegal combination of values",          // 32
		"Internal error",                         // 33
		"Cannot connect to remote server",        // 34
		"Lost connection to remote server",       // 35
		//	"Failed listen on port",                  // 36
		//	"Failed to send printout to remote server",// 37
		"38",
		"39",
		//	"CP not obtainable",								// 40
		//	"CP standby not obtainable",					// 41
		"Unable to connect to server",            // 42
		"43",
		"44",
		"45",
		"46",
		"47",
		"48",
		"Configuration file not found",           // 49
		"File does not exist",							// 50
		"File already exists",							// 51
		"Cannot access file",                     // 52
		"Cannot open or create file",             // 53
		"File not executable",                    // 54
		"Cannot write to file",                   // 55
		"Cannot write to temporary file",         // 56
		"Cannot open or create temporary file",   // 57
		"Execution stopped at line",              // 58
		"59",
		//	"Device is occupied",							// 60
		//	"Device does not exist",						// 61
		//	"Device does already exists",					// 62
		//	"Device table is full",							// 63
		//	"Device is defined in routing tables or as standby", // 64
		//	"Operation not allowed for this device type",	// 65
		//	"No free device",                         // 66
		//	"Empty printout queue",                   // 67
		//	"Destination device is faulty",           // 68
		//	"Option -t not allowed on default PDS",   // 69
		"70",
		"71",
		"72",
		"73",
		"74",
		"75",
		"76",
		"77",
		"78",
		"79",
		//	"Command not supported in this mode",     // 80 mml iocmi
		//   "Operation or function not supported",    // 81
		//	"Only allowed from within an mml session",// 82 aploc
		//	"Only allowed from active AP node",       // 83
		"84",
		"85",
		"86",
		"87",
		"88",
		"89",
		//  "Alarm receiver already exist",           // 90
		//	"Alarm receiver does not exist",          // 91
		//	"Alarm receiver is not manually blocked", // 92
		//	"Alarm receiver already manually blocked",// 93
		//	"Alarm receiver is not defined to require manual clear", // 94
		"95",
		"96",
		"97",
		"98",
		"99",
		//  "Alarm display already exist",            // 100
		//   "Alarm display does not exist",           // 101
		//   "Alarm display is not manually blocked",  // 102
		//   "Alarm display already manually blocked", // 103
		//   "Alarm display property already exist",   // 104
		//   "Alarm display property does not exist",  // 105
		"106",
		"107",
		"108",
		"109",
		//   "AP group not found",                     // 110
		//   "AP group already defined",               // 111
		//   "CP user not found",                      // 112
		//   "AP is not defined",                      // 113
		//   "Maximum number of associations exceeded",// 114
		NULL
};



ACS_APGCC::ACS_APGCC(){
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the user is a member of the given (wide) group.
// Parameters:
//    user           Name of user (in ascii)
//    group          Name of grou (in WIDE characters)
// Return value:
//    true           User is a member
//    false          User is not a member
//-----------------------------------------------------------------------------
bool ACS_APGCC::isMemberOf(const char* user, LPCWSTR group)
{
	UNUSED(group);
	UNUSED(user);
	return true;
}


//-----------------------------------------------------------------------------
// Description:
//    Tries to kill the remote process.
// Parameters:
//    hProcess       Handle to the remote process's handle.
//-----------------------------------------------------------------------------
bool ACS_APGCC::KillRemoteProcess(ACE_HANDLE hProcess)
{
	UNUSED(hProcess);
	return true;
	//	pid_t processPid=0,returnPid=0;
//	ACE_exitcode status;
//	ACE_OS:: kill(hProcess,SIGKILL);
//	returnPid = ACE_OS::waitpid(processPid,&status,0,hProcess);
//
//	if (returnPid > -1)
//		return true;
//	else
//		return false;


#if 0
	BOOL ok = FALSE;
	DWORD exitCode;

	//ok = MCS::LoadLibInRemoteProcess(hProcess,"%AP_HOME%/MCS/lib/libmcs_kill_r1a_dmdn6.dll");
	ok = MCS::LoadLibInRemoteProcess(hProcess,"%AP_HOME%/MCS/lib/libmcs_kill_r1a_dmdn71.dll");

	if ( ok ) {

		//cout << "waiting for remote process to die..." << endl;
		if ( ::WaitForSingleObject(hProcess,3000) == WAIT_OBJECT_0 ) {
			GetExitCodeProcess(hProcess,&exitCode);
			//cout << "Child killed. Exits with " << exitCode << endl;
			ok = TRUE;
		}
	}

	if ( !ok || ::WaitForSingleObject(hProcess,0) != WAIT_OBJECT_0 ) {
		// If child still active, terminate the hard way
		ok = ( ::TerminateProcess( hProcess, 255 ) ? true : false );
	}

	return (ok?true:false);
#endif
}


//-----------------------------------------------------------------------------
// Description:
//    Tries to load the given DLL library into the remote process.
// Parameters:
//    hProcess       Handle to the remote process's handle.
//    dllName        A valid path to the DLL to load.
//-----------------------------------------------------------------------------
bool ACS_APGCC::LoadLibInRemoteProcess(ACE_HANDLE hProcess, const char* dllPath)
{
	UNUSED(dllPath);
	UNUSED(hProcess);
	return true;
#if 0
	BOOL fOk = FALSE; // Assume that the function fails
	HANDLE hThread = NULL;
	char* dllFileNameRemote = NULL;
	char dllFileName[MAX_PATH];

	::ExpandEnvironmentStrings(dllPath,dllFileName,MAX_PATH-1);

	__try {

		// Calculate the number of bytes needed for the DLL's pathname
		int cch = 1 + strlen(dllFileName);
		int cb  = cch;

		// Allocate space in the remote process for the pathname
		dllFileNameRemote = (char*) VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (dllFileNameRemote == NULL) __leave;

		// Copy the DLL's pathname to the remote process's address space
		if (!WriteProcessMemory(hProcess, dllFileNameRemote, (PVOID) dllFileName, cb, NULL)) __leave;

		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
        		 GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
		if (pfnThreadRtn == NULL) __leave;

		// Create a remote thread that calls LoadLibraryW(DLLPathname)
		hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, dllFileNameRemote, 0, NULL);
		if (hThread == NULL) __leave;

		// Wait for the remote thread to terminate
		if ( WaitForSingleObject(hThread, 30*1000) == WAIT_OBJECT_0 ) {
			fOk = TRUE; // Everything executed successfully
		}

	}
	__finally { // Now, we can clean everthing up

		// Free the remote memory that contained the DLL's pathname
		if (dllFileNameRemote != NULL) {
			VirtualFreeEx(hProcess, dllFileNameRemote, 0, MEM_RELEASE);
		}

		if (hThread != NULL) {
			CloseHandle(hThread);
		}

	}

	return (fOk?true:false);
#endif
}


//-----------------------------------------------------------------------------
// Description:
//    Returns the release timeout value used for MML sessions.
//    CP text: TIME OUT
//-----------------------------------------------------------------------------
//DWORD ACS_APGCC::GetReleaseTimeoutValue()
//{
//
//	ACS_APGCC_Parameters apgcc;
//   DWORD timeoutValue;
//   DWORD logoffValue = GetLogoffTimeoutValue();
//
//   timeoutValue = apgcc.getVal(ACS_APGCCCXCNAME,"ACS_APGCC_MML_Release_Timeout",300);
//
//
//   /* ------------------------------------------------------------------
//     QSHAPAT changes made as if logofftime is 0 then take the parameter value as it is.
//   	 ------------------------------------------------------------------ */
//
//   if ( logoffValue != 0 && timeoutValue > logoffValue ) {
//      timeoutValue = logoffValue;
//   }
//
//   return timeoutValue;
//
//
//}
//-----------------------------------------------------------------------------
//
// Returns the last occured system error.
//
//-----------------------------------------------------------------------------
int ACS_APGCC::getLastError()
{
	return errno;
}

//-----------------------------------------------------------------------------
// Description:
//    Returns the log off timeout value used for MML sessions.
//    CP text: LOGGED OFF
//-----------------------------------------------------------------------------
DWORD ACS_APGCC::GetLogoffTimeoutValue(const std::string& user)
{
	UNUSED(user);
	const char* REGKEY = "SOFTWARE\\PragmaSystems\\TelnetD\\Users\\";
	DWORD timeoutValue = 0;
	std::string subkey(REGKEY);
	//DWORD type;
	/*PR_01 Waiting for IMM library */
#ifdef IMM
	HKEY hKey;
	char buf[128];
	DWORD buflen = 128;
	bool first_try = true;

	if ( user.length() == 0 )
		subkey += "Default";
	else
		subkey += user;

	Retry:

	if ( ::RegOpenKey(HKEY_LOCAL_MACHINE,TEXT(subkey.c_str()),&hKey) == ERROR_SUCCESS ) {

		if ( ::RegQueryValueEx(hKey,TEXT("IdleSessionTimeout"),NULL,&type,(LPBYTE)buf,&buflen) == ERROR_SUCCESS ) {

			switch (type )
			{
			case REG_SZ:
			case REG_EXPAND_SZ:
				buf[buflen] = 0;
				timeoutValue = ::atol(buf);
				break;
			case REG_DWORD:
				timeoutValue = (DWORD)buf;
				break;
			}
		}

		::RegCloseKey(hKey);
	}

	// If no value was retrieved for the given user,
	// try once more with the default user
	if ( first_try && timeoutValue == 0 && user.length() > 0 ) {
		subkey = REGKEY;
		subkey+= "Default";
		first_try = false;
		goto Retry;
	}

	// Set default timeout value if none given

	/*----------------------------------
   //Commented by QSHAPAT as it don't allow the timout more than 300 Sec Ref HF52076
   //timeoutValue = 0 indicates that there is no timeout of telnet sever

   if ( timeoutValue == 0 ) {
      timeoutValue = 5 * 60;  // 5 minutes
   }

	//End of QSHAPAT'S COMMENTS
	---------------------------------- */
#endif
	return timeoutValue;


}


//-----------------------------------------------------------------------------
// Description:
//    Switch the trace mode active or passive.
//-----------------------------------------------------------------------------
void ACS_APGCC::set_trace_mode(bool active)
{
	__g_mcs_trace_on = active;
}


//-----------------------------------------------------------------------------
// Description:
//    Trace. Use MCS_Trace.h and mcs_trace() instead.
//-----------------------------------------------------------------------------
void ACS_APGCC::trace(ACS_TRA_trace& tracePoint, const char* format_str, ...)
{
	UNUSED(tracePoint);
#if defined (NDEBUG)
	// In Release mode we need to test this flag immediately to
	// avoid unnecesary overhead. We can't do that in Debug mode
	// since this code is used to print on stdout as well.
	if (ACS_TRA_ON(tracePoint)) {
#endif

		ACE_Guard<ACE_Thread_Mutex> guard(__g_mcs_trace_lock);

		char* sp;
		char* bp = __g_mcs_trace_msg;
		char* format = ::strdup(format_str);
		char* saved_p = format;
		int type = 0;

		if ( !format )
			return;

		try {

			// Start of variable args section.
			va_list argp;
			va_start(argp, format_str);

			*bp = 0;

			// Preformat string with thread id if not given in format string
			sp = strstr(const_cast<char*>(format),"%t");
			if ( !sp ) {
				::sprintf(bp, "(%lu) ", ACE_OS::thr_self());
				// Locate end of bp.
				while (*bp != '\0')
					bp++;
			}

			while (*format != '\0') {
				// Copy input to output until we encounter a %, however a
				// % followed by another % is not a format specification.

				//cout << "format='" << format << "'" << endl;

				if (*format != '%')
					*bp++ = *format++;
				else if (format[1] == '%') {  // An "escaped" '%' (just print one '%').
					*bp++ = *format++;         // Store first %
					format++;                  // but skip second %
				}
				else {
					char* fp = format++;
					type = 0;

					switch (*format++)
					{
					case 'p': // Format the string assocated with the value of errno.
					{
						errno = ACE::map_errno(getLastError());
						if (errno >= 0 && errno < sys_nerr) {
							//::sprintf(bp, "%s: %s", va_arg(argp, char *), strerror (errno));
							::sprintf(bp, "%s", strerror (errno));
						}
						else {
							char * lpMsgBuf = 0;
							/* PR_01 BEGIN*/
							lpMsgBuf=strerror( errno );
							/*                      ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        errno,
                        MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf,
                        0,
                        NULL);
                        PR_01 END */

							// If we don't get a valid response from
							// <FormatMessage>, we'll assume this is a
							// WinSock error and so we'll try to convert
							// it into a string.  If this doesn't work it
							// returns "unknown error" which is fine for
							// our purposes.

							if (lpMsgBuf == 0) {
								const char *message = ACE::sock_error(errno);
								//@@::sprintf(bp, "%s: %s", va_arg(argp, const char *), message);
								::sprintf(bp, "%s", message);
							}
							else {
								//::sprintf(bp, "%s: %s", va_arg (argp, char *), lpMsgBuf);
								::sprintf(bp, "%s", lpMsgBuf);
								// Free the buffer.
								/*PR_01      ::LocalFree (lpMsgBuf);*/
							}
						}
						break;
					}

					case 'P':      // Format the current process id.
						::sprintf(bp, "%d", ::getpid());
						break;

					case 't': // Format thread id.
						::sprintf(bp, "%lu", ACE_OS::thr_self());
						break;

					case 'T': // Format the timestamp in hour:minute:sec:usec format.
					{
						char day_and_time[35];
						::sprintf(bp, "%s", ACE::timestamp(day_and_time,sizeof day_and_time));
					}
					break;

					case 's':
						type = 1;
						break;

					case 'S':
						try {
							std::string s = va_arg(argp, char*);
							::sprintf(bp, "%s", s.c_str());
						}
						catch(...) {
							// Nada
						}
						break;

					case 'W':
						// @@ UNICODE
					case 'd': case 'c': case 'i': case 'o':
					case 'u': case 'x': case 'X':
						type = 2;
						break;

					case 'F': case 'f': case 'e': case 'E':
					case 'g': case 'G':
						type = 3;
						break;

					default:
						// ?
						break;

					}// switch

					if ( type != 0 ) {
						char c = *format;
						*format = 0;

						//cout << "fp='" << fp << "'" << endl;
						switch(type)
						{
						case 1:
							// A single %s string formatter causes an
							// exception in the va_arg() call.
							// Since argp points to the first string, we can use
							// argp directly if the exception occurs.
							try {
								::sprintf(bp, fp, va_arg(argp, char*));
							}
							catch(...) {
								try {
									::sprintf(bp, fp, argp);
								}
								catch(...) {
								}
							}
							break;
						case 2:
							::sprintf(bp, fp, va_arg(argp, int));
							break;
						case 3:
							::sprintf(bp, fp, va_arg(argp, double));
							break;
						}
						*format = c;
					}

					while (*bp != '\0') // Locate end of bp.
						bp++;

				}// if

			}// while

			*bp = 0;

			sp = strchr(__g_mcs_trace_msg,'\n');
			if ( sp )
				*sp = 0;

#if defined (NDEBUG)
			ACS_TRA_event(&tracePoint, __g_mcs_trace_msg);
#else
			ACS_TRA_trace  objClass("ACS_APGCC", "C40");
			if ( __g_mcs_trace_on ) {
				//OSF_DEBUG((LM_DEBUG,"%s",this->msg_));
				/*PR_01 waiting for ACS_TRA
				 *
         cout << tracePoint.recordname << "  " << __g_mcs_trace_msg << endl;*/
			}

			if (objClass.ACS_TRA_ON())
				objClass.ACS_TRA_event(1, __g_mcs_trace_msg);

#endif

			va_end(argp);

		}// tryobjPRC
		catch(...) {
			// Trace resulted in an exception
#if defined (_DEBUG)
			try {
				MCS::logError("Exception in MCS::trace() for trace %s",tracePoint.recordname);
			}
			catch(...) {
			}
#endif
		}// catch

		::free(saved_p);

#if defined (NDEBUG)
	}
#endif
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the current node is the active one.
//    Returns true also if no cluster is available.
//-----------------------------------------------------------------------------
//bool ACS_APGCC::is_active_node(const std::string node_name)
//{
//	ACS_PRC_API objPRC;
//
//	bool retVal;
//
//	if ( node_name.length() == 0  )
//		retVal=(objPRC.askForNodeState()==1);
//	else
//		retVal=(objPRC.askForNodeState(node_name)==1);
//
//	return retVal;
//}

bool ACS_APGCC::is_active_node(const char* node_name)
{
	ACS_PRC_API objPRC;

	bool retVal;

	if ( node_name == 0  )
		retVal=(objPRC.askForNodeState()==1);
	else
		retVal=(objPRC.askForNodeState(node_name)==1);

	return retVal;
}


//-----------------------------------------------------------------------------
// NOTE: REPLACED BY CS API
// Description:
//    Returns own AP number
// Return value:
//    int            AP number 1-?.
//-----------------------------------------------------------------------------
// qvincon begin
//int ACS_APGCC::get_ap_no() {
//
//	ACS_DSD_Server server;
//	ACS_DSD_Node node;
//
//	server.get_local_node(node);
//
//	if ( ( node.system_id >= acs_dsd::CONFIG_AP_ID_SET_START ) && ( node.system_id <= acs_dsd::CONFIG_AP_ID_SET_END ) )
//		return ( node.system_id - acs_dsd::CONFIG_AP_ID_SET_START );
//	else
//		return -1;
//}


//-----------------------------------------------------------------------------
// NOTE: REPLACED BY CS API
// Description:
//    Returns own AP node
// Return value:
//    char         AP node 'a', 'b'...
//-----------------------------------------------------------------------------
//char ACS_APGCC::get_ap_node() {
//
//	ACS_DSD_Server server;
//	ACS_DSD_Node node;
//
//	server.get_local_node(node);
//	string name ( node.node_name );
//
//	if ( name.length() > 0 )
//		return name.at( name.length() - 1 );
//	else
//		return 0;
//}

//Commented for APG43-No use of shares-Start

//-----------------------------------------------------------------------------
// Description:
//    Expands name of network share if found
//-----------------------------------------------------------------------------
//void MCS::expandShare(string& path)
//{
//   string share;
//   wstring wshare;
//   NET_API_STATUS ns;
//   PSHARE_INFO_502 buf = 0;
//
//   // Possible share names:
//   //    Share MCS\data
//   //    \\.\Share LOGS\logs
//   //    \\ap74\Share MCS\data
//
//   /**
//      // Convert to wide strings
//      if ( server.length() > 0 ) {
//         AS2WS(server,wserver);
//      }
//   **/
//
//   // Get share name
//   if ( !MCS::item(share,path,"/\\",0) || ::strncmp(path.c_str(),share.c_str(),share.length()) != 0 )
//      return;
//
//   // If colon or % is present then this isn't a share name
//   if ( share.find(":") != string::npos || share[0] == '%' )
//      return;
//
//   // Convert to wide strings
//   AS2WS(share,wshare);
//
//   // Convert Share name to absolute path
//   ns = ::NetShareGetInfo(NULL,  // Use local host
//                          (LMSTR
//
//						  )wshare.c_str(),   // This is really a wide string according to the manual
//                          502,
//                          (LPBYTE*)&buf);
//
//   if ( ns == NERR_Success ) {
//      // Convert to ascii string
//      wshare = (wchar_t*)buf->shi502_path;
//
//      // Replace share name with absolute path
//      path.erase(0,share.length());
//      WS2AS(wshare,share);
//      path.insert(0,share);
//
//
//   }
//   else {
//
//      // Failed to convert share; Try manually
//#pragma TODO(BEWARE: Tries to convert share names manually)
//
//      string abspath;
//
//      if ( share == "Share MCS" )
//         abspath = "M:\\";
//      else if ( share == "Share LOGS" )
//         abspath = "K:\\";
//      else if ( share == "Share K" )
//         abspath = "K:\\";
//
//      if ( abspath.length() > 0 ) {
//         MCS::logWarning("MCS_Path: Converted share %s manually to %s",share.c_str(),abspath.c_str());
//
//         // Replace share name with absolute path
//         path.erase(0,share.length());
//         path.insert(0,abspath);
//      }
//
//   }
//
//   if ( ns == NERR_Success || ns == ERROR_MORE_DATA){
//      ::NetApiBufferFree(buf);
//   }
//}

//-Commented for apg43-No use of shares-End


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int ACS_APGCC::getValue(const std::string& str, int& value, const int& idx)
{
	//#if 0
	int pos;
	int j = 0;
	char num[80];
	std::string DELIMITERS = ",-.";
	memset((void *)num,'\0',80); //added to solve problem with cpdinsert-apg43

	pos = str.find_first_of(DELIMITERS,idx);

	if ( pos == -1 ) {

		for (unsigned int i=idx; i < str.length(); i++) {
			num[j++] = str[i];
		}
	}
	else {

		for (int i=idx; i < pos; i++) {
			num[j++] = str[i];
		}
	}
	std::string strTempNum(num);
	value = atoi(strTempNum);

	return pos;
	//#endif
}


//-----------------------------------------------------------------------------
//
// Fetch a number from a serie of numbers.
// Returns true if succeded.
// The index variable must be set to zero before
// this method is called.
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::fchnum(const char* arg, int& value, int& idx)
{

	int val;
	int pos;
	std::string str = arg;
	//string str = "1,2,10-13,5,7..9,63,50";
	char COMMA = ',';
	char DOT   = '.';
	char MINUS = '-';

	int strLenght = str.length();
	// Check if we're done.
	if ( idx >= strLenght )
		return false;


	if ( str[idx] == COMMA ) {

		idx++;
		pos = getValue(str,value,idx);
	}
	else if ( str[idx] == DOT ) {

		if ( str[idx+1] == DOT ) {

			value++;                         // Increase until range end is reached.

			pos = getValue(str,val,idx+2);

			if ( value < val ) {
				return true;
			}
		}
		else {
			return false;                    // Only single dot!
		}
	}
	else if ( str[idx] == MINUS ) {

		value++;                            // Increase until range end is reached.

		pos = getValue(str,val,idx+1);

		if ( value < val ) {
			return true;
		}
	}
	else {
		pos = getValue(str,value,idx);
	}


	if ( pos == -1 ) {
		idx = strLenght;
	}
	else {
		idx = pos;
	}

	return true;
}

//-----------------------------------------------------------------------------
//
// Fetch a number from a serie of numbers.
// Returns true if succeded.
// The index variable must be set to zero before
// this method is called.
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::fchnum(const std::string str, int& value, int& idx)
{

	int val;
	int pos;
	//std::string str = arg;
	//string str = "1,2,10-13,5,7..9,63,50";
	char COMMA = ',';
	char DOT   = '.';
	char MINUS = '-';

	int strLenght = str.length();
	// Check if we're done.
	if ( idx >= strLenght )
		return false;


	if ( str[idx] == COMMA ) {

		idx++;
		pos = getValue(str,value,idx);
	}
	else if ( str[idx] == DOT ) {

		if ( str[idx+1] == DOT ) {

			value++;                         // Increase until range end is reached.

			pos = getValue(str,val,idx+2);

			if ( value < val ) {
				return true;
			}
		}
		else {
			return false;                    // Only single dot!
		}
	}
	else if ( str[idx] == MINUS ) {

		value++;                            // Increase until range end is reached.

		pos = getValue(str,val,idx+1);

		if ( value < val ) {
			return true;
		}
	}
	else {
		pos = getValue(str,value,idx);
	}


	if ( pos == -1 ) {
		idx = strLenght;
	}
	else {
		idx = pos;
	}

	return true;
}


typedef enum {
	Num    = 1,
	Sep    = 2,
	DotDot = 3,
	Num2   = 4,
	Comma  = 5
} States;

//-----------------------------------------------------------------------------
//
// Check the given range for format errors.
// Used prior to fchnum().
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::checkRangeFormat(const std::string& range, int min, int max)
{

	unsigned int i;
	int cnt;
	States state;
	std::set<char> numset;
	std::set<char> sepset;
	std::string num;
	int start= 0 , end= 0;

	// Insert valid numbers
	for (i=0; i<10; i++) {
		numset.insert( (char)(i+48) );
	}

	// Insert separators
	sepset.insert('.');
	sepset.insert(',');
	sepset.insert('-');

	i = 0;
	state = Num;

	while ( i < range.length() ) {

		switch (state)
		{
		case Num:

			cnt = 0;
			num = "";
			while ( numset.count(range[i+cnt]) > 0 ) {
				num += range[i+cnt];
				cnt++;
			}

			if ( cnt == 0 )
				return false;

			// Save number to be able to check range end
			start = atoi(num);

			i += cnt;

			state = Sep;
			break;

		case Num2:

			cnt = 0;
			num = "";
			while ( numset.count(range[i+cnt]) > 0 ) {
				num += range[i+cnt];
				cnt++;
			}

			if ( cnt == 0 )
				return false;

			end = atoi(num);

			if ( start >= end )
				return false;

			i += cnt;

			state = Comma;
			break;

		case Sep:

			// Check type of sep
			switch (range[i])
			{
			case '.':
				i++;
				state = DotDot;
				break;
			case ',':
				i++;
				state = Num;
				break;
			case '-':
				i++;
				state = Num2;
				break;

			default:
				return false;
			}
			break;

			case DotDot:

				// Must be a dot
				if ( range[i] != '.' )
					return false;

				i++;
				state = Num2;
				break;

			case Comma:

				// Must be a comma
				if ( range[i] != ',' )
					return false;

				i++;
				state = Num;
				break;

			default:
				return false;
		}

	}


	// Last item must be a number
	if ( state != Sep && state != Comma )
		return false;


	// The set of prca values should be reasonable Ok here
	int prca;
	int prcaIdx = 0;
	std::set<int> prcaset;

	while ( ACS_APGCC::fchnum( range, prca, prcaIdx ) ) {

		if ( prcaset.count( prca ) != 0 || prca < min || prca > max) {
			// Duplicate values or too low or too high
			return false;
		}

		prcaset.insert(prca);
	}

	// Check if we got any numbers
	if ( prcaset.size() == 0 )
		return false;
	return true;
}


//-----------------------------------------------------------------------------
// Converts an ascii string to a DWORD.
//-----------------------------------------------------------------------------
DWORD ACS_APGCC::atod(const char* numstr)
{
	char* stop;
	return ::strtoul(numstr,&stop,10);
}


//-----------------------------------------------------------------------------
// Converts an ascii string to a ACE_UINT32.
//-----------------------------------------------------------------------------
ACE_UINT32 ACS_APGCC::atod(const std::string& numstr)
{
	char* stop;
	return ::strtoul(numstr.c_str(),&stop,10);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int ACS_APGCC::atoi(const std::string& s)
{
	return ::atoi(s.c_str());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int ACS_APGCC::atoi(const char* s)
{
	return ::atoi(s);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* PR_01 BEGIN*/
char * GetTLSData(){
	ACE_OS::memset (g_pthr_str_arr, 0, sizeof (g_pthr_str_arr));
	return g_pthr_str_arr;}
/* PR_01 END */

const char* ACS_APGCC::itoa(int n, int width){
	char format[20];


	g_pthr_str = GetTLSData();

	if ( width > 0 )
		::sprintf(format,"%%0%ii",width);
	else
		::strcpy(format,"%i");

	::sprintf(g_pthr_str,format,n);

	return g_pthr_str;
}


//-----------------------------------------------------------------------------
// Description:
//    Converts an integer to an ascii string.
//-----------------------------------------------------------------------------
void ACS_APGCC::itoa(char* outStr, int n, int width)
{
	char format[20];

	// Clear out variable
	outStr[0] = 0;

	if ( width > 0 )
		::sprintf(format,"%%0%ii",width);
	else
		::strcpy(format,"%i");

	::sprintf(outStr,format,n);
}

//-----------------------------------------------------------------------------
// Description:
//    Converts an integer to an ascii string.
//-----------------------------------------------------------------------------
void ACS_APGCC::itoa(std::string &outStr, int inputInteger)
{

	std::ostringstream osstream;
	osstream << inputInteger;
	outStr = osstream.str();
}


//-----------------------------------------------------------------------------
//
// Convert a number to a string
// in hex format.
// Widht indicates the minium width of
// the string.
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::itoa_hex(int n, int width)
{
	char format[20];

	g_pthr_str = GetTLSData();

	if ( width > 0 )
		::sprintf(format,"%%0%ix",width);
	else
		::strcpy(format,"%x");

	::sprintf(g_pthr_str,format,n);

	return g_pthr_str;
}


/*===================================================================
   ROUTINE: formatDateTime
=================================================================== */
//const char* ACS_APGCC::formatDateTime(const std::string& format  //IN
//)
//{
//	/*===================================================================
//	   Default initialization.
//	=================================================================== */
//	g_pthr_str = GetTLSData();
//	time_t rawtime;
//	struct tm * timeinfo;
//	time ( &rawtime );
//	timeinfo = localtime ( &rawtime );
//	strftime ( g_pthr_str ,sizeof (g_pthr_str_arr),format.c_str(),timeinfo);
//	return  g_pthr_str ;
//}

/*===================================================================
   ROUTINE: formatDateTime
=================================================================== */
const char * ACS_APGCC::formatDateTime(const std::string& format  //IN
)
{
	/*===================================================================
	   Default initialization.
	=================================================================== */

	std::string outputString("");
	char formattedString[128] = {0};
	//g_pthr_str = GetTLSData();
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime ( formattedString ,128,format.c_str(),timeinfo);
	outputString.append(formattedString);
	return  outputString.c_str() ;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getText(const ACS_APGCC_Text textNo)
{
	if (textNo >= 0 && textNo < No_Of_Texts)
		return Texts[textNo];
	else
		return NullString;
}


//-----------------------------------------------------------------------------
//
// Returns the text string that
// corresponds to the given error code.
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getErrorText(const ACS_APGCC_ErrorCode errorNo)
{
	if (errorNo >= 0 && errorNo < No_Of_ErrTexts)
		return ErrorTexts[errorNo];
	else
		return NullString;
#if 0
	HMODULE hModule = NULL;
	LPSTR msgBuf = 0;
	DWORD bufLen;
	DWORD last_error = 0;

	g_pthr_str = GetTLSData();
	g_pthr_str[0] = 0;

	DWORD formFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_IGNORE_INSERTS;


#ifndef MCC_STATIC_LIB

#ifdef NDEBUG

	// Load MCC Lib
	hModule = ::LoadLibraryEx( TEXT(MCCLIB1),
			NULL,
			LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES );

	if ( hModule == NULL ) {
		// Failed to load library
		MCS::logError("MCS::getErrorText; Failed to load message lib=%s; Error=%s",MCCLIB1,MCS::getSystemErrorText());
	}

#endif // NDEBUG

	// Load MCC Lib
	if ( hModule == NULL ) {

		hModule = ::LoadLibraryEx( TEXT(MCCDLIB),
				NULL,
				LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES );

		if ( hModule == NULL ) {
			// Get load error code
			last_error = MCS::getLastError();

			// Failed to load library
			MCS::logError("MCS::getErrorText; Failed to load message lib=%s; Error=%s",MCCDLIB,MCS::getSystemErrorText(last_error));
		}
	}

#endif //MCC_STATIC_LIB

	if ( last_error == 0 ) {

		bufLen = FormatMessageA(formFlags,
				hModule,
				errorNo,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR) &msgBuf,
				0,
				NULL );


		// Check if we got the message
		if ( bufLen > 0 && msgBuf != NULL ) {

			msgBuf[bufLen] = 0;

			::strcpy(g_pthr_str,msgBuf);

			// Strip off these new line characters
			int len = ::strlen(g_pthr_str);
			len = ACS_APGCC::strip_ch(g_pthr_str, '\n', len);
			len = ACS_APGCC::strip_ch(g_pthr_str, '\r', len);

			// Release allocated message buffer
			if ( msgBuf )
				::LocalFree(msgBuf);
		}
		else {
			last_error = MCS::getLastError();

			// Failed to get message
			MCS::logError("MCS::getErrorText; Failed to load message for error=%i; Error=%s",errorNo,MCS::getSystemErrorText(last_error));
		}

	}

	if ( hModule )
		::FreeLibrary(hModule);

	// if we failed to load DLL or retreive message
	// try to retreive it from the older (not updated) error text table
	if ( last_error != 0 ) {

		if (errorNo >= 0 && errorNo < No_Of_ErrorTexts)
			return ErrorTexts[errorNo];
		else
			return NullString;
	}

	return g_pthr_str;
#endif
}




//-----------------------------------------------------------------------------
//
// Converts the error code returned by the system
// to a text string.
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getSystemErrorText(int error)
{
	int last_error = (error < 0 ? getLastError() : error);
	//g_pthr_str = GetTLSData();
	char errorString[128] = {0};
std::string outputString("");
	::sprintf(errorString,"%i",last_error);

	char* cp = ::strerror(last_error);

	if ( cp )
		::sprintf(errorString,"%s (%i)",cp,last_error);
	outputString.append(errorString);
	return outputString.c_str();
}

//-----------------------------------------------------------------------------
//
//    Get the system's memory usage in percent.
//
//-----------------------------------------------------------------------------
int ACS_APGCC::getMemoryUsage()
{
	struct sysinfo sys_info;
	if(sysinfo(&sys_info) != 0)
		cout<<"error sysinfo"<<endl;
	long double total = (sys_info.totalram + sys_info.totalswap) / 1024;
	long double avail = (sys_info.freeram + sys_info.freeswap) / 1024;
	double memUsed = 100.0 * (total - avail) / total;


	//long double total2 = (sys_info.totalram + sys_info.totalswap) / 1024;
	//long double avail2 = (sys_info.freeram + sys_info.freeswap) / 1024;
	double memUsed2 = 100.0 * (total - avail) / total;
	cout<<"memUsed:"<<memUsed<<"   memUsed2:"<<memUsed2<<endl;
	return (int)memUsed;

}


//-----------------------------------------------------------------------------
//
// Creates the given directories.
// Directories must be separated with semicolon.
// All none existing directories in the given path are
// created.
//
//-----------------------------------------------------------------------------
int ACS_APGCC::create_directories(const char* directories, mode_t access)
{

	ACS_APGCC_Path dir;
	std::string dirs = directories;
	std::string newDir;
	std::string total("/");
	std::string curr;
	int n;
	int m;
	int last_err = 0;
	bool succeeded = false;
	std::set<char> chset;

	chset.insert('/');
	chset.insert('\\');
	n = 0;
	while ( ACS_APGCC::item(newDir,dirs,";",n) ) {

		// Expand directory path
		dir.setpath(newDir.c_str());
		newDir = dir.path();
		m = 0;
		total = "/";

		// Check if the path is UNC coded: \\machine\dir1\dir2
		if ( chset.count(newDir[0]) && chset.count(newDir[1]) ) {
			// We must skip over the machine name
			ACS_APGCC::item(curr,newDir,"/\\",m++);
			total = "\\\\";
			total+= curr;
			total+= "\\";

		}

		// Must create all directories in the path
		while ( ACS_APGCC::item(curr,newDir,"/\\", m) ) {
			total += curr;
			bool created=(mkdir(total.c_str(),access)==0);


			if ( !created )
				last_err = ACS_APGCC::getLastError();
			if ( created || last_err == ERROR_ALREADY_EXISTS ) {
				succeeded = true;
			}
			else if ( !(total.length()==2 && total[1] == ':') ) {
				/*PR01       	 ACS_APGCC::logWarning("MCS::create_directories(); Failed to create %s in path %s; Error=%s",total.c_str(),directories,MCS::getSystemErrorText(last_err));*/
			}

			total += '/';
			m++;
		}

		n++;
	}

	// the result code does not reflect if all directories
	// could be created... Use sparingly.
	if ( last_err == ERROR_ALREADY_EXISTS )
		succeeded = true;
	// Following line of code is changed for correction in TR HE90812
	return (succeeded ? 1 : 0);
//	return (succeeded ? 0 : 1);
}


//-----------------------------------------------------------------------------
//
// Returns true if the given file is a console application.
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::isConsoleApp(const char* appPath)
{
	UNUSED(appPath);
#if 0
	ACE_HANDLE hImage;
	DWORD  bytes;
	DWORD  sectionOffset;
	DWORD  coffHeaderOffset;
	DWORD  moreDosHeader[16];
	/* PR_01
	 *  ULONG  ntSignature;
   IMAGE_DOS_HEADER      image_dos_header;
   IMAGE_FILE_HEADER     image_file_header;
   IMAGE_OPTIONAL_HEADER image_optional_header;
	 */
	ACS_APGCC_Path app(appPath);

	char path[MAX_PATH];

	/* PR_01 ::_searchenv(app.path(),"PATH",path);*/
	if ( strlen(path) == 0 ) {
		strcpy(path, app.path());
	}

	/*
	 *  Open the reference file.
	 */
	hImage = CreateFile( path,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

	if ( ACE_INVALID_HANDLE == hImage ) {
		return false;
	}

	/*
	 *  Read the MS-DOS image header.
	 */
	/* PR_01 if ( !ReadFile(hImage,&image_dos_header,sizeof(IMAGE_DOS_HEADER),&bytes, NULL) ) {
      goto Cleanup;
   }

   if ( IMAGE_DOS_SIGNATURE != image_dos_header.e_magic ) {
      goto Cleanup;
   }*/

	/*
	 *  Read more MS-DOS header.
	 */
	if ( !ReadFile(hImage,moreDosHeader,sizeof(moreDosHeader),&bytes, NULL) ) {
		goto Cleanup;
	}


	/*
	 *  Get actual COFF header.
	 */
	if ( (coffHeaderOffset=SetFilePointer(hImage,image_dos_header.e_lfanew,NULL,FILE_BEGIN)) == 0xFFFFFFFF ) {
		goto Cleanup;
	}

	coffHeaderOffset += sizeof(ULONG);

	if ( !ReadFile(hImage,&ntSignature,sizeof(ULONG),&bytes, NULL) || IMAGE_NT_SIGNATURE != ntSignature ) {
		goto Cleanup;
	}

	sectionOffset = coffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER + IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

	if ( !ReadFile(hImage,&image_file_header,IMAGE_SIZEOF_FILE_HEADER,&bytes, NULL) ) {
		goto Cleanup;
	}


	/*
	 *  Read optional header.
	 */
	if ( !ReadFile(hImage,&image_optional_header,IMAGE_SIZEOF_NT_OPTIONAL_HEADER,&bytes, NULL) ) {
		goto Cleanup;
	}

	::CloseHandle( hImage );

	if ( image_optional_header.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI ) {
		return true;
	}
	else {
		return false;
	}

	Cleanup:

	::CloseHandle( hImage );
#endif
return false;
}


//-----------------------------------------------------------------------------
//
// Check if the given file can be executed.
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::isFileExecutable(const char* filePath)
{
#ifdef WIN32

	DWORD binType;

	if ( ::GetBinaryType(filePath, &binType) == TRUE )
		return true;

	else {

		// Add extension and try again
		string path = filePath;
		path += ".exe";

		if ( ::GetBinaryType(path.c_str(), &binType) == TRUE )
			return true;
		else
			return false;
	}

#else //UNIX

	if ( ::access(filePath, X_OK) < 0 )
		return false;
	else
		return true;

#endif
}


//-----------------------------------------------------------------------------
//
//    Expand the program path in response to environment variables,
//    and also adds qutation marks around space characters for
//    Windows.
//
//-----------------------------------------------------------------------------
void ACS_APGCC::expandFilePath(std::string& filePath)
{

	ACS_APGCC_Path path( filePath.c_str());
	std::string pgm(path.path());
	std::string::size_type first, last;

	if ( pgm.find(" ") != std::string::npos ) {

		ACS_APGCC::replace(pgm,"\\","/");

		first = pgm.find_first_of("/");
		last  = pgm.find_last_of("/");

		if ( first != last ) {
			if ( first != std::string::npos ) {
				pgm.insert(first+1,"\"");
			}

			if ( last != std::string::npos ) {
				pgm.insert(last,"\"");
			}
		}
	}

	filePath = pgm;

}

//-----------------------------------------------------------------------------
// getPortNoForService()
// Returns the port number associated with this service.
// Uses getservbyname()
//
//-----------------------------------------------------------------------------
int ACS_APGCC::getPortNoForService(const std::string serviceName)
{
	int  portNo = 0;

	struct servent* se = ::getservbyname(serviceName.c_str(),NULL);

	if ( se ) {
		portNo = ntohs( se->s_port );
	}

	//Return portNo
	return portNo;
}


//-----------------------------------------------------------------------------
// getPortNoForService()
// Returns the port number associated with this service.
// Uses getservbyname() or reads the standard service file in
// ../etc/services if getservbyname() fails.
//
//-----------------------------------------------------------------------------
int ACS_APGCC::getPortNoForService(const char* serviceName)
{
	//const char* SERVICES_FILE_NAME = "%SystemRoot%/System32/drivers/etc/services";
	const char* TOKEN_SEPARATOR    = " \t/#";  // blank, tab and slash
	const char  COMMENT_CHAR       = '#';
	const int   PORT_NAME_COLUMN   = 0;
	const int   PORT_NO_COLUMN     = 1;

	ifstream serviceFile;
	int      portNo;
	char     fileName [512];
	std::string line;
	std::string token;

	struct servent* se = ::getservbyname(serviceName,NULL);

	if ( se ) {
		portNo = ntohs( se->s_port );
	}

	if ( !se || portNo < 0 || portNo > 0xffff ) {

		// Retrieve filename
		/* PR_01     ::ExpandEnvironmentStrings(SERVICES_FILE_NAME, fileName, sizeof(fileName));
		 */
		// Open file
		serviceFile.open(fileName);

		//Scan file for serviceName. Port name is the first token.
		portNo = 0;

		while ( serviceFile && serviceFile.eof() == 0 ) {

			getline(serviceFile, line, '\n');

			// Remove leading and trailing spaces
			trim(line);

			// Skip comments and empty lines
			if ( line.length() > 0 && line[0] != COMMENT_CHAR ){

				//Get first token (port name)
				if ( item(token, line, TOKEN_SEPARATOR, PORT_NAME_COLUMN) ){

					//Compare token to service name
					if ( ACE_OS::strcmp(token.c_str(), serviceName) == 0 ){

						//Get second token (port number)
						if ( item(token, line, TOKEN_SEPARATOR, PORT_NO_COLUMN) ){

							//Convert string to integer
							portNo=atoi(token);

							// Port name found
							break;
						}
					}
				}
			}
		}//while

		//Close file
		serviceFile.close();

	}

	//Return portNo
	return portNo;
}


//-----------------------------------------------------------------------------
// Description:
//    Reads the host names from /etc/hosts and returns
//    a table with the host name as key and the host's ip
//    address as value.
//*** Exporting a STL map from a DLL does not work
//-----------------------------------------------------------------------------
/******************************************************************
int MCS::getHostTable(HostTable& hostTable)
{
   const char* DELIMITER = " \t";
   ifstream fin;

   // Clear host table
   if ( hostTable.size() > 0 ) {
      hostTable.erase( hostTable.begin(), hostTable.end() );
   }

   fin.open( MCS_Path("%SystemRoot%/System32/Drivers/etc/hosts") );
   if ( !fin ) {
      return -1;
   }

   string ip, host;
   char line[256];

   while ( fin ) {

      fin.getline(line,255);
      if ( fin.eof() && strlen(line) == 0)
         break;
      char* cp = strchr(line,'#');
      if ( cp )
 *cp = 0;

      if ( strlen(line) > 0 ) {

         if ( MCS::item(ip,line,DELIMITER,0) && MCS::match(ip,"*.*.*.*") ) {
            int n=1;
            while ( MCS::item(host,line,DELIMITER,n++) ) {
               hostTable[host] = ip;
            }
         }
      }
   }

   return hostTable.size();
}
 ******************************************************************/


//-----------------------------------------------------------------------------
// isPortReserved()
// Returns true if the given port is already defined in the /etc/services file.
//
//-----------------------------------------------------------------------------
bool ACS_APGCC::isPortReserved(int port)
{

	bool reserved = false;

	// Port numbers lesser than 1024 are always reserved
	if ( port < 1024 || port > 0xffff ) {
		return true;
	}

	struct servent* se = ::getservbyport( htons((unsigned short)port), NULL );

	if ( se ) {
		reserved = true;
	}

	return reserved;
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the given IP address part number is valid.
//    The number must be 0>= and <=255.
//-----------------------------------------------------------------------------
bool ACS_APGCC::isValidIpAddressNumber(const std::string& numstr)
{
	if (numstr.length() == 0 || !ACS_APGCC::is_number(numstr.c_str()) ) {
		return false;
	}

	int num = atoi( numstr );

	if ( num < 0 || num > 255 ){
		return false; // Out of range
	}

	return true;
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the given IP address is a valid one.
//    Performs range checks as well as host lockup.
//-----------------------------------------------------------------------------
bool ACS_APGCC::isValidIpAddress(const std::string& ipaddr, const std::string& portSep)
{
	std::string ip;
	std::string portstr;
	std::string ipp;

	if ( portSep.length() > 0 ) {

		// Port given (or allowed to follow)
		// Extract IP-adress
		ACS_APGCC::item(ip, ipaddr, portSep, 0);

		// Extract Port
		ACS_APGCC::item(portstr, ipaddr, portSep, 1);
	}
	else {
		// No port present
		ip = ipaddr;
	}

	std::string::size_type pos = ip.find_first_not_of(".0123456789");
	if ( pos == std::string::npos ) {
		std::string val;

		// Normal IP address; no symbolic name
		if ( !ACS_APGCC::match(ip,"*.*.*.*") )
			return false;

		// Check number range
		ACS_APGCC::item(val,ip,".",0);
		if ( !ACS_APGCC::isValidIpAddressNumber(val) )
			return false;

		ACS_APGCC::item(val,ip,".",1);
		if ( !ACS_APGCC::isValidIpAddressNumber(val) )
			return false;

		ACS_APGCC::item(val,ip,".",2);
		if ( !ACS_APGCC::isValidIpAddressNumber(val) )
			return false;

		ACS_APGCC::item(val,ip,".",3);
		if ( !ACS_APGCC::isValidIpAddressNumber(val) )
			return false;
	}

	// Validate the ip address by expanding
	// it into the form "a.b.c.d".

	ACE_INET_Addr inet;
	inet.set((u_short)0,ip.c_str());

	const char* hostaddr = inet.get_host_addr();

	if ( hostaddr ) {

		if ( strcmp(hostaddr,"0.0.0.0") == 0 ) {
			// Invalid ip address
			return false;
		}

	}
	else {
		// Invalid ip address
		return false;
	}




	// Check Port, if allowed to
	//--------------------------

	if ( portSep.length() > 0 ) {

		if ( portstr.length() == 0 ) {
			return false;
		}
		if ( !ACS_APGCC::is_number(portstr.c_str()) ){
			return false;
		}
		int port = atoi( portstr );

		if ( port <= 0  || port > 0xffff ) {
			return false; // Out of range
		}

		if ( ACS_APGCC::isPortReserved(port) ) {
			return false;  // Reserved port number
		}

	} // if check port


	// We have a valid ip-address (and port number)
	return true;
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the given string contains a valid IP mask,
//    on the form "*.12?.*.34" or similair.
//-----------------------------------------------------------------------------
bool ACS_APGCC::isValidIpMask(const std::string& maskStr)
{
	std::string valid_chars = "0123456789*.?";
	std::string::size_type pos;
	std::string ns;

	// Format must be "a.b.c.d"
	if( !ACS_APGCC::match(maskStr,"*.*.*.*") || countChar(maskStr,'.') != 3 ) {
		return false;
	}

	// Check for invalid chars
	ns = maskStr;

	pos = ns.find_first_not_of(valid_chars);
	if ( pos != std::string::npos ) {
		return false;
	}


	// Check each number
	ACS_APGCC::item(ns, maskStr,".",0);
	if ( (ns.length() <= 0 ) || (ns.length() > 3 ) ) {
		return false;
	}
	if ( ACS_APGCC::is_number(ns.c_str()) && atoi(ns) > 255 ) {
		return false;
	}


	ACS_APGCC::item(ns, maskStr,".",1);
	if ( (ns.length() <= 0 ) || (ns.length() > 3 ) ) {
		return false;
	}
	if ( ACS_APGCC::is_number(ns.c_str()) && atoi(ns) > 255 ) {
		return false;
	}


	ACS_APGCC::item(ns, maskStr,".",2);
	if ( (ns.length() <= 0 ) || (ns.length() > 3 ) ) {
		return false;
	}
	if ( ACS_APGCC::is_number(ns.c_str()) && atoi(ns) > 255 ) {
		return false;
	}


	ACS_APGCC::item(ns, maskStr,".",3);
	if ( (ns.length() <= 0 ) || (ns.length() > 3 ) ) {
		return false;
	}
	if ( ACS_APGCC::is_number(ns.c_str()) && atoi(ns) > 255 ) {
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Description:
//    Returns true if the given string contains valid hex numbers.
//-----------------------------------------------------------------------------
bool ACS_APGCC::isValidHexChars(const std::string& str)
{
	std::string tmp = str;
	std::string hexchars = "0123456789ABCDEF";
	std::string::size_type pos;

	if ( str.length() == 0 )
		return false;

	ACS_APGCC::toUpper(tmp);

	pos = tmp.find_first_not_of(hexchars);

	if ( pos == std::string::npos )
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------
// Description:
//    Returns number of found characters that equals the given one.
//-----------------------------------------------------------------------------
int ACS_APGCC::countChar(const std::string& str, char ch)
{
	std::string::size_type pos;
	std::string::size_type start = 0;
	int cnt = 0;

	while ( (pos=str.find(ch,start)) != std::string::npos ) {
		cnt++;
		start = pos + 1;
	}

	return cnt;
}


//----------------------------------------------------------------------
// Retrieve name of used domain
//----------------------------------------------------------------------
bool ACS_APGCC::getDomainName(char* domainName, DWORD& domainNameSize)
{
	UNUSED(domainName);
	UNUSED(domainNameSize);
#if 0
	HANDLE hProcess = NULL;
	HANDLE hAccessToken = NULL;
	UCHAR infoBuffer[1000];
	char szAccountName[20];
	PTOKEN_USER pTokenUser = (PTOKEN_USER)infoBuffer;
	DWORD dwInfoBufferSize = 0;
	DWORD dwAccountSize = 20;
	SID_NAME_USE snu;
	bool ok = false;

	hProcess = ::GetCurrentProcess();

	::OpenProcessToken(hProcess,TOKEN_READ,&hAccessToken);

	::GetTokenInformation(hAccessToken,TokenUser,infoBuffer,1000, &dwInfoBufferSize);

	if ( ::LookupAccountSid(NULL,pTokenUser->User.Sid,szAccountName,&dwAccountSize,domainName,&domainNameSize,&snu) ) {
		ok = true;
	}

	::CloseHandle(hAccessToken);

	return ok;
#endif
return true;
}


//-----------------------------------------------------------------------------
//
// Returns the name of the login user.
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getUserLoginName()
{

#ifdef WIN32

	CHAR username[100];
	DWORD bufSize = sizeof(username)-1;

	g_pthr_str = GetTLSData();
	g_pthr_str[0] = 0;

	if ( ::GetUserName((LPTSTR)&username, &bufSize) ) {
		::strcpy(g_pthr_str,username);
	}

	return g_pthr_str;

#else

	char* username;
	char *unknown=NULL;
	unknown = strdup("?unknown?");
	//char* unknown = "?unknown?";
	struct passwd* pw;

	pw = getpwuid(geteuid());

	if (pw == NULL) {
		username = unknown;
	}
	else {
		username = pw->pw_name;
	}

	/****
   username = getlogin();
   pw = getpwuid(getuid());

   if (username != NULL) {

      // It is possible that getlogin() truncates user
      // names to only 8 characters. getpwuid() returns
      // the whole user name.

      if (pw != NULL && strlen(username) < strlen(pw->pw_name)) {
         username = pw->pw_name;
      }

   }
   else if (pw == NULL) {
      username = unknown;
   }
   else {
      username = pw->pw_name;
   }
	 ****/
	/*PR_01 UserName dove definito ?*/
	/* strcpy(UserName, username);
   return (const char*)UserName;*/
	return (const char*)username;

#endif
}


#define MAX_PASS_LEN	32	// Max number of chars in password

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char* ACS_APGCC::readUserPassword(const char* prompt)
{
#ifdef WIN32

	DWORD oldMode, newMode;

	g_pthr_str = GetTLSData();
	g_pthr_str[0] = '\0';

	// Show prompt
	cout << prompt << flush;

	// Disable echo mode
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &oldMode);
	newMode = oldMode & ~ENABLE_ECHO_INPUT;
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), newMode);

	// Read password
	gets(g_pthr_str);
	cout << endl;

	// Restore old mode
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), oldMode);

#else

	struct termios term, termsave;
	char*	  ptr;
	FILE*	  fp;
	int	          c;
	/* PR_01 iobuf? */
	char iobuf[MAX_PASS_LEN+1];
	iobuf[0] = '\0';

	if ( (fp=fopen(ctermid(NULL), "r+")) == NULL)
		return NULL;

	setbuf(fp, NULL);

	// set echo off

	tcgetattr(fileno(fp), &termsave);
	term = termsave;
	term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	tcsetattr(fileno(fp), TCSAFLUSH, &term);

	fputs(prompt, fp);

	ptr = iobuf;

	while ( (c=getc(fp)) != EOF && c != '\n') {
		if (ptr < &iobuf[MAX_PASS_LEN])
			*ptr++ = c;
	}
	*ptr = 0;

	putc('\n', fp);

	// set echo on

	tcsetattr(fileno(fp), TCSAFLUSH, &termsave);

	fclose(fp);

#endif

	return g_pthr_str;
}



//-----------------------------------------------------------------------------
//
// Get name of remote host
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getRemoteHost()
{
	string remote_hostname("");
	string remote_hostname_path = "/etc/cluster/nodes/peer/hostname";
	ifstream ifs;
	ifs.open(remote_hostname_path.c_str());
	if (ifs.good())
		getline(ifs, remote_hostname);
	ifs.close();
	return remote_hostname.c_str();
}


//-----------------------------------------------------------------------------
//
// Get name of remote host
//
//-----------------------------------------------------------------------------
const char* ACS_APGCC::getRemoteHost(ACE_HANDLE fd)
{
	UNUSED(fd);
	string remote_host("");

	return remote_host.c_str();


//	struct sockaddr_in  rem_addr;
//	struct hostent* hp;
//	struct hostent  hostent;
//	socklen_t len;
//
//	g_pthr_str = GetTLSData();
//
//
//
//
//	len = sizeof(rem_addr);
//
//#ifdef WIN32
//	if (getpeername((SOCKET)fd, (sockaddr*)&rem_addr, &len) >= 0) {
//#else
//		if (getpeername(fd, (sockaddr*)&rem_addr, &len) >= 0) {
//#endif
//
//			rem_addr.sin_port = ntohs((u_short)rem_addr.sin_port);
//			//		hp = gethostbyaddr((const char*)&rem_addr.sin_addr,sizeof(struct in_addr),rem_addr.sin_family);
//#ifdef _TANDEM
//			hp = gethostbyaddr((char*)&rem_addr.sin_addr,sizeof(struct in_addr),rem_addr.sin_family);
//#else
//			hp = gethostbyaddr((const char*)&rem_addr.sin_addr,sizeof(struct in_addr),rem_addr.sin_family);
//#endif
//
//			if (hp == NULL) {
//
//				// Couldn't find the client's name
//				// Use its dotted-decimal address as its name
//
//				hp = &hostent;
//
//				hp->h_name = inet_ntoa(rem_addr.sin_addr);
//			}
//
//			strncpy( g_pthr_str, hp->h_name, TLS_SIZE-1 );
//			g_pthr_str[TLS_SIZE-1] = 0;
//		}
//
//#ifdef WIN32
//
//		return g_pthr_str;
//
//#else
//
//		struct utmpx* id;
//		int ttyindex;
//		/*PR_01 remoteHost?   */
//		char *  remoteHost;
//		if(strlen(remoteHost) == 0)
//		{
//
//			// Try to fetch it from the environment
//
//			ACS_APGCC_Path env("$REMOTEHOST");
//			if (strstr(env.path(),"REMOTEHOST") == NULL) {
//
//				strcpy( remoteHost, env.path() );
//
//			}
//			else {
//
//				// We have no remote host name
//				// Try to get entry in '/etc/inittab'
//				// for this TTY
//
//				ttyindex = ttyslot();
//				setutxent();
//				for(int i=0;i<ttyindex;i++)
//				{
//					getutxent();
//				}
//				id = getutxent();
//				if(id == 0)
//				{
//					remoteHost[0] = '\0';
//				}
//				else
//				{
//					strcpy(remoteHost, id->ut_host);
//				}
//
//			}
//
//
//		}
//
//		return (const char*) remoteHost;
//
//#endif
	}


	//-----------------------------------------------------------------------------
	//
	// Get name of local host
	//
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::getLocalHost()
	{
		ACE_INET_Addr addr;
		std::string hostName("");
		/* PR_01 g_pthr_str = GetTLSData();*/
		hostName.append(addr.get_host_name());

		//::strcpy(g_pthr_str, addr.get_host_name());
		//return g_pthr_str;
		return  hostName.c_str();
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::delay(int delayInSec, int delayInUsec)
	{
#ifdef WIN32

		::Sleep(delayInSec*1000 + delayInUsec/1000);

#else

		ACE_Time_Value tval;

		tval.set(delayInSec,delayInUsec);

		ACE_OS::select(0, (fd_set*)0, (fd_set*)0, (fd_set*)0, &tval);

#endif
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::RdItem(istream& is, std::string& out, const CHARSET& delimiters)
	{
		char ch;

		out = "";

		// skip delimiters
		while ( is.good() && is.get(ch) && delimiters.count(ch) > 0)
			;

		// found start of identifier
		if ( is.good() )
			out += ch;

		// keep reading until delimiter
		while ( is.good() && is.get(ch) && delimiters.count(ch) == 0) {
			out += ch;
		}

		return (out.length() > 0 ? true:false);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::toUpper(std::string& str)
	{
		std::string::iterator it;

		for (it = str.begin(); it!=str.end(); it++) {
			(*it) = (char)::toupper( *it );
		}
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::toLower(std::string& str)
	{
		std::string::iterator it;

		for (it = str.begin(); it!=str.end(); it++)
			(*it) = static_cast<char>( ::tolower( *it ) );
	}


	std::wstring wide_tmp_str;

	//-----------------------------------------------------------------------------
	// Description:
	//    Converts an ASCII string to Wide format.
	//-----------------------------------------------------------------------------
	const wchar_t* ACS_APGCC::toWide(const std::string& Source)
	{


		AS2WS(Source,wide_tmp_str);

		return wide_tmp_str.c_str();
	}

	std::string tmp_str;

	//-----------------------------------------------------------------------------
	// Description:
	//    Converts an Wide string to ASCII format.
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::toAscii(const std::wstring& wSource)
	{
		WS2AS(wSource,tmp_str);
		return tmp_str.c_str();
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::trim(std::string& str)
	{
		std::string::size_type pos = 0;

		// Strip from the beginning first
		while ( isspace(str[0]) )
			str.erase(pos,1);

		// Strip from the end now
		trimEnd(str);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::trimEnd(std::string& str)
	{
		std::string::size_type pos;

		// Strip from the end
		pos = str.length();

		while (pos > 0 && isspace(str[pos-1]) ) {
			str.erase(--pos,1);
		}
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::trimMmlCommand(std::string& str)
	{
		std::string::size_type i;
		//bool insideStr;
		bool firstValidChar = false;
		bool haveCmdCode    = false;


		//cout << "--> trimMmlCommand('" << str.c_str() << "')" << endl;

		// 1st, Remove spaces from start and end of the command
		trim(str);

		//cout << "\t1st pass: '" << str.c_str() << "'" << endl;

		i = 0;
		while ( i < str.length() ) {

			//cout << "[" << i << "]='"<<str[i]<<"', len=" << str.length() << endl;

			switch (str[i])
			{
			case '"':
				// Skip over the string
				i++;
				while ( i < str.length() && str[i] != '"' )
					i++;
				if ( i < str.length() )
					i++;
				//cout << "Skipping string" << endl;
				break;

			case '!':
				//cout << "inside comment!" << endl;
				str.erase(i,1);
				while ( i < str.length() && str[i] != '!' && str[i] != ';' )
					str.erase(i,1);
				if ( i < str.length() && (str[i] == '!' || str[i] == ';') )
					str.erase(i,1);

				//cout << "outside comment!" << endl;
				continue;

			case ':':
			case ';':
			case ' ':
				if ( firstValidChar && !haveCmdCode ) {

					// The command code starts when we have found a valid
					// character other than space. The command code ends
					// when semicolon, colon or a space is found.

					haveCmdCode = true;

					// For space only, check if a colon is present afterwards,
					// and if so delete the space
					// Might be "TEST SYSTEM;"
					if ( str[i] == ' ' ) {
						if ( str.find(':',i+1) == std::string::npos )
							break;
					}
				}
				/*FALLTHROUGH*/

			default:
				// Remove spaces
				if ( isspace(str[i]) ) {

					// Note that we cannot remove spaces unless we have a
					// valid command code. That is to ensure that commands
					// like "TEST SYSTEM;" doesn't end up as "TESTSYSTEM;".
					// However, spaces that are found before a valid
					// non-space character has been found, is deleted
					// that is, "  CMD;" becomes "CMD;".

					if ( haveCmdCode || !firstValidChar ) {
						//cout << "space!" << endl;
						str.erase(i,1);
						continue;
					}
				}
				else {
					// This indicates that we have found a valid
					// non-space character which indicates the start
					// of the command code.
					// All characters upto this first character will
					// be deleted as being redundant information.
					firstValidChar = true;
				}
				break;
			}

			i++;

		} // while

		// Check for the at-sign special case
		if ( !haveCmdCode && str[0] == '@' ) {
			haveCmdCode = true;
		}

		// If we don't have any command code, clear command string
		if ( !haveCmdCode && str.length() > 0 ) {
			str = "";
		}

		//cout << "\tReturns: '" << str.c_str() << "'" << endl;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::replace(std::string& str, const std::string& pattern, const std::string& target, bool caseSensitive)
	{
		bool found = false;
		std::string::size_type pos;

		if ( caseSensitive ) {

			pos = 0;

			while ( (pos=str.find(pattern,pos)) != std::string::npos ) {
				found = true;
				str.erase(pos, pattern.length());
				str.insert(pos, target);
				pos += target.length();
			}

		}
		else {
			std::string s = str;
			std::string p = pattern;

			toUpper(s);
			toUpper(p);

			pos = 0;

			while ( (pos=s.find(p,pos)) != std::string::npos ) {
				found = true;
				// Replace the string in the copy...
				s.erase(pos, pattern.length());
				s.insert(pos, target);
				// as well as in the Orginal string
				str.erase(pos, pattern.length());
				str.insert(pos, target);
				pos += target.length();
			}
		}

		return found;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::before(const std::string& str, const std::string& pattern)
	{

	std::string out = str;
	std::string::size_type pos;

	pos = str.find(pattern);
	if (pos != std::string::npos) {
		out = str.substr(0,pos);
	}

	return out.c_str();

		//return before(str.c_str(), pattern.c_str());
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::beforeLast(const std::string& str, const std::string& pattern)
	{

		std::string out = str;
		std::string::size_type pos=0,pos1=0;

		while(pos!=string::npos)
		{
			pos=str.find(pattern,pos+1);
			if(pos!=string::npos)
				pos1=pos;
		}
		if((pos1!=string::npos)|| (pos1!=0))
			out = str.substr(0,pos1);
		else
			out.clear();

		//pos = str.find_last_of(pattern);
		//if (pos != std::string::npos) {
		//	out = str.substr(0,pos);
		//}

		return out.c_str();

		//return beforeLast(str.c_str(), pattern.c_str());
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::after(const std::string& str, const std::string& pattern)
	{
		std::string out;
		std::string::size_type pos;

		pos = str.find(pattern);
		if (pos != std::string::npos) {
			out = str.substr(pos+pattern.length(),str.length()-pos-pattern.length());
		}

		return out.c_str();

		//		return after(str.c_str(),pattern.c_str());
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::afterLast(const std::string& str, const std::string& pattern)
	{

		std::string out;
		std::string::size_type pos=0,pos1=0;

		while(pos!=string::npos)
		{
			pos=str.find(pattern,pos+1);
			if(pos!=string::npos)
				pos1=pos;
		}
		if((pos1!=string::npos)|| (pos1!=0))
			out = str.substr(pos1+pattern.length(),str.length()- pos1);
		else
			out.clear();

		//		pos = str.find_last_of(pattern);
		//		if (pos != std::string::npos) {
		//			out = str.substr(pos+1,str.length()- pos);
		//		}

		return out.c_str();

		//		return afterLast(str.c_str(), pattern.c_str());
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::item(std::string& r, const std::string& s, const CHARSET& cs, int n)
	{
		int i;
		int j;
		int len;
		r = "";
		i = 0;
		len = s.length();

		for (;;) {

			// skip separators
			while (i < len && cs.count(s[i]) > 0)
				i++;

			// should this item be returned ?
			if (n == 0 || i == len)
				break;
			n--;

			// skip item
			while (i < len && cs.count(s[i]) == 0)
				i++;
		}

		// Store item
		j = 0;
		while (i < len && cs.count(s[i]) == 0) {
			r += s[i];
			i++;
			j++;
		}
		if ( r.empty() || len == 0 )
			return false;
		else
			return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::item(char* out, const char* source, const CHARSET& cs, int n)
	{
		std::string tmp;

		out[0] = 0;

		bool res = item(tmp,source,cs,n);

		if ( res )
			::strcpy(out,tmp.c_str());

		return res;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::item(std::string& r, const std::string& s, const std::string& t, int n)
	{
		int i = t.length();
		CHARSET cs;

		while (i > 0) {
			i--;
			cs.insert( t[i] );
		}
		return item(r,s,cs,n);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::item(char* out, const char* source, const char* delimiters, int n)
	{
		std::string tmp;

		out[0] = 0;

		bool res = item(tmp,source,delimiters,n);

		if ( res )
			::strcpy(out,tmp.c_str());

		return res;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::match(const std::string& Source, const std::string& Pattern)
	{
		return Rmatch(Source,0,Pattern,0);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::Rmatch(const std::string& s, int i, const std::string& p, int j)
	{
		// s = to be tested ,    i = position in s
		// p = pattern to match ,j = position in p
		bool matched;
		unsigned int indexI = i,indexJ = j;

		unsigned int k;

		if (p.length() == 0)
			return true;

		for (;;) {

			if ( indexI >= s.length() && indexJ >= p.length() )
				return true;

			else if (indexJ >= p.length())
				return false;

			else if ( p[indexJ] == '*' ) {
				k = indexI;
				if ( indexJ >= p.length()-1 )
					return true;

				for (;;) {
					matched = Rmatch(s,k,p,indexJ+1);
					if ( matched || (k >= s.length()) )
						return matched;
					k++;
				}
			}
			else if ( (p[indexJ]=='?' && indexI<s.length()) || (toupper(p[indexJ]) == toupper(s[indexI])) ) {
				indexI++;
				indexJ++;
			}
			else
				return false;
		} // for
	}



	//-----------------------------------------------------------------------------
	// Description:
	//    Return the value of the "name=value" pair from
	//    the given source string.
	//    Multiple name=value-pairs must be separated with the
	//    characters given in the delimiter string.
	//    If no delimiters are given, semicolon(;) is used.
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::getNameValuePair(std::string& value, const std::string& source, const std::string& name, const std::string& delimiters)
	{
		std::string pair;
		std::string sep = delimiters;
		std::string namestr = name;

		namestr += "=";
		if ( sep.length() == 0 )
			sep = ";";

		value = "";

		int n = 0;
		while ( ACS_APGCC::item(pair,source,sep,n++) ) {

			if ( pair.find(namestr) != std::string::npos ) {
				value = ACS_APGCC::after(pair,"=");
				return true;
			}
		}

		return false;
	}


	//-----------------------------------------------------------------------------
	// Description:
	//    Return the value of the "name=value" pair from
	//    the given source string.
	//    Multiple name=value-pairs must be separated with the
	//    characters given in the delimiter string.
	//    If no delimiters are given, semicolon(;) is used.
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::getNameValuePair(char* value, const std::string& source, const std::string& name, const std::string& delimiters)
	{
		std::string valstr;

		value[0] = 0;

		if ( ACS_APGCC::getNameValuePair(valstr,source,name,delimiters) ) {
			::strcpy(value,valstr.c_str());
			return true;
		}

		return false;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::before(const char* str, const char* pattern)
	{

		g_pthr_str = GetTLSData();

		//  g_pthr_str[0] = 0;

		if (str != NULL && pattern != NULL) {

			char* cp = strstr(const_cast<char*>(str), pattern);

			if (cp != NULL) {

				const char* sp = str;
				int i=0;

				while (sp != cp) {
					g_pthr_str[i++] = *sp;
					sp++;
				}
				g_pthr_str[i] = 0;

			}
			else {
				return str;
			}

		}
		else {
			return str;
		}

		return g_pthr_str;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::beforeLast(const char* str, const char* pattern)
	{
		g_pthr_str = GetTLSData();
		if (str != NULL && pattern != NULL) {
			char* cp = 0;
			char* lastpos = 0;
			const char* currpos = str;
			while ( (cp = strstr(const_cast<char*>(currpos), pattern)) != NULL ) {
				lastpos = cp;
				currpos = cp+1;
			}
			if ( lastpos )
				cp = lastpos;
			if (cp != NULL) {
				const char* sp = str;
				int i=0;
				while (sp != cp) {
					g_pthr_str[i++] = *sp;
					sp++;
				}
				g_pthr_str[i] = 0;

			}
			else {
				return str;
			}

		}
		else {
			return str;
		}
		return g_pthr_str;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::after(const char* str, const char* pattern)
	{
		char* cp = NULL;

		if (str != NULL && pattern != NULL) {

			cp = strstr(const_cast<char*>(str), pattern);

			if (cp != NULL) {

				int len = strlen(pattern);

				while (cp && *cp != '\0' && len > 0) {
					cp++;
					len--;
				}
			}
		}

		return (cp ? cp : NullString);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::afterLast(const char* str, const char* pattern)
	{
		char* cp = NULL;

		if (str != NULL && pattern != NULL) {

			char* lastpos = 0;
			const char* currpos = str;

			while ( (cp = strstr(const_cast<char*>(currpos), pattern)) != NULL ) {
				lastpos = cp;
				currpos = cp + 1;
			}

			if ( lastpos )
				cp = lastpos;

			if (cp != NULL) {

				int len = strlen(pattern);

				while (cp && *cp != '\0' && len > 0) {
					cp++;
					len--;
				}
			}
		}

		return (cp ? cp : NullString);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	const char* ACS_APGCC::getMmlCmdCode(const char* mml)
	{
		unsigned int i;
		g_pthr_str = GetTLSData();
		char* cp      = g_pthr_str;
		bool finished = false;

		std::string cmd = mml;

		// Trim of spaces and comments
		trimMmlCommand(cmd);

		i   = 0;
		*cp = 0;

		while ( i < cmd.length() && !finished && i < 30 ) {
			switch (cmd[i])
			{
			case ' ':
			case ':':
			case ';':
				if (g_pthr_str[0] != 0)
					finished = true;
				break;
			default:
				// must be a valid command char
				*cp++ = cmd[i];
				*cp   = 0;
			}
			i++;
		}

		if (g_pthr_str[0] != 0)
			return g_pthr_str;
		else
			return NullString;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strcasecmp(const std::string& s1, const std::string& s2)
	{
		return strcasecmp(s1.c_str(),s2.c_str());
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strcasecmp(const char *s1, const char *s2)
	{
		return strcasecmp(s1,s2);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strncasecmp(const std::string& s1, const std::string& s2, int n)
	{

		return strncasecmp(s1.c_str(),s2.c_str(),n);

	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strncasecmp(const char *s1, const char *s2, int n)
	{

		return strncasecmp(s1,s2,n);

	}



	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::strip_space(char* str)
	{
		int len = strlen(str);
		unsigned char tmp[len + 1];
		int j = 0;

		for (int i = 0; i < len; ++i) {
			if (!isspace(str[i])) {
				tmp[j] = str[i];
				++j;
			}
		}

		::memcpy(str, tmp, j);
		str[j] = 0;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::strip_pre_ctrl(char* str)
	{
		int len = strlen(str);

		while (len > 0) {
			if ( iscntrl(str[0]) ) {
				// EALOCAE: Use 'memmove' instead of 'memcpy'
				// when dealing with overlapping memory areas!!!
				memmove(&str[0], &str[1], len);
				len--;
			}
			else
				break;
		}
		/*int i = 0;

   while (i<len) {
      if ( iscntrl(str[i]) ) {
         memcpy( &str[i], &str[i+1], len-i+1 );
         len--;
      }
      else
         break;
   }*/

	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	string ACS_APGCC::capstr(string p_str)
	{
		const int length = p_str.length();
		for(int i=0; i!=length ; ++i)
		{
			p_str[i] = std::toupper(p_str[i]);
		}
		return p_str;

	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strip_ch(char *str, char ch, int len)
	{
		return strip_ch((unsigned char*)str, ch, len);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strip_ch(unsigned char *str, int ch, int len)
	{
		unsigned char *tmp = (unsigned char *)new char[len + 1];  //TR HV73427
		memset(tmp,0,len+1);
		int j = 0;

		for (int i = 0; i < len; ++i) {
			if (str[i] != ch) {
				tmp[j] = str[i];
				++j;
			}
		}

		::memcpy(str, tmp, j);
		delete[] tmp;	//TR HV73427
		str[j] = 0;

		return j;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::strip_pre_ch(char *buf, char ch, int len)
	{
		int	i;

		for (i=0; i < len; i++) {
			if ((unsigned char)buf[i] == (unsigned char)ch){
				memcpy( &buf[i], &buf[i+1], len-i+1);
				i--;
				len--;
			}
			else
				break;
		}

		return len;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::is_only_space(const char* s)
	{
		int len = strlen(s);

		for (int i=0; i<len; i++) {
			if ( !isspace(s[i]) )
				return false;
		}

		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::is_number(const char* s)
	{
		bool is_number = false;

		while (s && *s != '\0') {
			if (isdigit(*s))
				is_number = true;
			else
				return false;
			s++;
		}

		return is_number;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::writeTimeMark(ofstream& os)
	{
		// Construct date and time string
		char timestr[128];
		time_t ltime;
		struct tm* today;

		::time( &ltime );
		today = ::localtime( &ltime );

		::strftime(timestr, 127, "%Y%m%d %H%M%S", today);

		os << timestr << " ";

		return true;
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::getProcessName(std::string *processName)
	{

		FILE * proc_stat_handle = fopen("/proc/self/stat", "r");
		if (!proc_stat_handle) return ;
		int pid = 0;
		char pname[1024]={0};
		processName->clear();
		int ii=fscanf(proc_stat_handle, "%d (%1023s", &pid, pname);
		if (ii >= 2) {
			processName->append(pname);
			/*tr HR40435 -  the following line commented */
			//*processName = processName->substr(0, processName->size()-1);
			string tmp = *processName;
			*processName = tmp.substr(0, tmp.size()-1);
			/*tr HR40435 -  End */


		} else {
			processName->clear();
		}
		fclose(proc_stat_handle);
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::getProcessName(char* processName)
	{

		FILE * proc_stat_handle = fopen("/proc/self/stat", "r");
		if (!proc_stat_handle) return ;
		int pid = 0;
		char pname[1024];
		errno = 0;
		int ii=fscanf(proc_stat_handle, "%d (%1023s", &pid, pname);
		if (ii >= 2) {
			char * p = reinterpret_cast<char *>(memccpy(processName, pname, ')', 16));
			if (p)
				*--p = 0;
			// process_name[size - 1] = 0;
		} else {
			strcpy(processName,"");
		}
		fclose(proc_stat_handle);
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::writeProcessInfo(ofstream& os)
	{
		//char processName[32];
		std::string processName("");
		char procId[64];

		ACS_APGCC::getProcessName(&processName );

		sprintf(procId,"%s:%d", processName.c_str(), ::getpid());

		os.width(18);
		os.flags(ios::left);
		os << procId;

		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::switchFileOnSize(const char* logfile, long maxSize, int numOfBackupFiles)
	{
		UNUSED(logfile);
		UNUSED(maxSize);
		UNUSED(numOfBackupFiles);
#if 0
		struct stat info;

		if ( numOfBackupFiles <= 0 )
			return true;

		if ( ::stat(logfile, &info) < 0 )
			return false;

		//cout << "Current log file size=" << info.st_size << endl;

		// Check if time to switch
		if ( info.st_size < maxSize )
			return true;

		// It's time to switch; Rotate all backup
		// files by deleting the oldest one and rotating
		// the other ones.
		std::string logFile = logfile;
		std::string backupFile;
		std::string::size_type pos;

		pos = logFile.find_last_of(".");
		if ( pos != std::string::npos )
			backupFile = logFile.substr(0,pos+1);
		else {
			backupFile = logFile;
			backupFile+= '.';
		}

		//cout << "Backup file = " << backupFile.c_str() << endl;

		// Find out size of extension by the number of backup files
		int extcnt = 0;
		int tmp = numOfBackupFiles;

		if ( tmp > 1 )
			tmp--;

		while ( tmp > 0 ) {
			extcnt++;
			tmp /= 10;
		}

		// Delete oldest files
		char format[128];
		char extstr[128];
		std::string fname;

		::sprintf(format,"%%0%ii",extcnt);
		::sprintf(extstr,format,numOfBackupFiles-1);

		fname = backupFile;
		fname += extstr;

		/*BOOL deleted =*/ ::DeleteFile(fname.c_str());
		//cout << "Deleting oldest file = " << fname.c_str() << (deleted?" Ok":" Failed") << endl;

		// Rotate the remaining backup files backwards
		//int dispCnt=0;
		std::string curr;
		std::string next;
		bool moved;

		for (int cnt=numOfBackupFiles-2; cnt>0; cnt--) {

			::sprintf(extstr,format,cnt);
			curr = backupFile;
			curr += extstr;

			::sprintf(extstr,format,cnt+1);
			next = backupFile;
			next += extstr;

			moved = ::MoveFileEx(curr.c_str(), next.c_str(), MOVEFILE_REPLACE_EXISTING);

			//if ( dispCnt++ < 15 )
			//   cout << "Rotating " << curr.c_str() << " to " << next.c_str() << (moved?" Ok":" Failed") << endl;

		}

		// Rename the current log file to backup 1
		::sprintf(extstr,format,1);
		next = backupFile;
		next += extstr;

		moved = ::MoveFileEx(logFile.c_str(), next.c_str(), MOVEFILE_REPLACE_EXISTING);
		//cout << "Renaming " << logFile.c_str() << " to " << next.c_str() << (moved?" Ok":" Failed") << endl;
#endif
		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::logInfo(const char* s, ...)
	{
UNUSED(s);
#if 0
		ACE_Guard<ACE_Thread_Mutex> guard( log_lock );

		std::string file;
		ACS_APGCC_Path path;
		APGCC_Registry reg;

		file = APGCC_LOGFILE;
		path.path("$APGCC_LOGS/trace.log");

		if ( path.found() ) {
			file = path.path();
		}
		else {
			char tmp[512];
			if ( ::ExpandEnvironmentStrings("%temp%/trace.log",tmp,512) == 0 )
				file = "C:\trace.log";
			else
				file = tmp;
		}


		// Switch subfile if necessary
		switchFileOnSize(file.c_str(),MAXLOGSIZE,MAXBACKUPFILES);

		ofstream fout (file.c_str(), ios::app);

		if ( !fout ) {

			// Try to create the needed directories
			APGCC::create_directories("$MCS_LOGS");

			// Try one more time
			fout.open(file.c_str(), ios::app);

			if ( !fout ) {

				// Make one last try in current directory
				fout.open(MCS_LOGFILE, ios::app);

				if ( !fout )
					return false;

				// Write time and date stamp
				writeTimeMark(fout);

				// Write name of caller (process name)
				writeProcessInfo(fout);

				// Write severity
				fout << " ERROR: logInfo; Failed to open/create log file=" << file.c_str() << std::endl;
			}
		}

		// Write time and date stamp
		writeTimeMark(fout);

		// Write name of caller (process name)
		writeProcessInfo(fout);

		// Write severity
		fout << " Info: ";

		// Assemble output string
		int i = 0;
		int len = ::strlen(s);
		std::strstream str;
		const char* cp;
		char c;
		int n;
		unsigned int un;
		char ns[10];
		va_list args;

		va_start(args, s);

		try {

			while ( i < len ) {

				switch ( s[i] )
				{
				case '%':
				i++;
				switch (s[i])
				{
				case 'c':
					c = va_arg(args, char);
					str << c;
					break;

				case 's':
					cp = va_arg(args, const char*);
					if (cp)
						str << cp;
					break;

				case 'i':
					n = va_arg(args, int);
					::sprintf(ns,"%i",n);
					str << ns;
					break;

				case 'u':
					un = va_arg(args, unsigned int);
					::sprintf(ns,"%u",un);
					str << ns;
					break;

				case 'x':
					n = va_arg(args, int);
					::sprintf(ns,"%x",n);
					str << ns;
					break;

				case 'X':
					n = va_arg(args, int);
					::sprintf(ns,"%X",n);
					str << ns;
					break;

				case 'n':
					str << std::endl;
					break;

				default:
					str << s[i];
				}
				break;

				default:
					str << s[i];
				}

				i++;
			}//while

		}
		catch(...) {
			str << "Exception in APGCC::logInfo()";
		}

		va_end(args);

		// Freeze stream
		str << ends;
		char* sp = str.str();

		// Write data string
		fout << sp << endl;

		delete [] sp;

		fout.close();
#endif
		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::logWarning(const char* s, ...)
	{
		UNUSED(s);
#if 0
		ACE_Guard<ACE_Thread_Mutex> guard( log_lock );

		std::string file;
		ACS_APGCC_Path path;
		APGCC_Registry reg;


		file = APGCC_LOGFILE;
		path.path("$MCS_LOGS/trace.log");

		if ( path.found() ) {
			file = path.path();
		}
		else {
			char tmp[512];
			if ( ::ExpandEnvironmentStrings("%temp%/trace.log",tmp,512) == 0 )
				file = "C:\trace.log";
			else
				file = tmp;
		}


		// Switch subfile if necessary
		switchFileOnSize(file.c_str(),MAXLOGSIZE,MAXBACKUPFILES);

		ofstream fout (file.c_str(), ios::app);

		if ( !fout ) {

			// Try to create the needed directories
			APGCC::create_directories("$MCS_LOGS");

			// Try one more time
			fout.open(file.c_str(), ios::app);

			if ( !fout ) {

				// Make one last try in current directory
				fout.open(MCS_LOGFILE, ios::app);

				if ( !fout )
					return false;

				// Write time and date stamp
				writeTimeMark(fout);

				// Write name of caller (process name)
				writeProcessInfo(fout);

				// Write severity
				fout << " ERROR: logWarning; Failed to open/create log file=" << file.c_str() << endl;;
			}
		}

		// Write time and date stamp
		writeTimeMark(fout);

		// Write name of caller (process name)
		writeProcessInfo(fout);

		// Write severity
		fout << " Warning: ";

		// Assemble output string
		int i = 0;
		int len = ::strlen(s);
		std::strstream str;
		const char* cp;
		char c;
		int n;
		unsigned int un;
		char ns[10];
		va_list args;

		va_start(args, s);

		try {

			while ( i < len ) {

				switch ( s[i] )
				{
				case '%':
				i++;
				switch (s[i])
				{
				case 'c':
					c = va_arg(args, char);
					str << c;
					break;

				case 's':
					cp = va_arg(args, const char*);
					if (cp)
						str << cp;
					break;

				case 'i':
					n = va_arg(args, int);
					::sprintf(ns,"%i",n);
					str << ns;
					break;

				case 'u':
					un = va_arg(args, unsigned int);
					::sprintf(ns,"%u",un);
					str << ns;
					break;

				case 'x':
					n = va_arg(args, int);
					::sprintf(ns,"%x",n);
					str << ns;
					break;

				case 'X':
					n = va_arg(args, int);
					::sprintf(ns,"%X",n);
					str << ns;
					break;

				case 'n':
					str << endl;
					break;

				default:
					str << s[i];
				}
				break;

				default:
					str << s[i];
				}

				i++;
			}//while

		}
		catch(...) {
			str << "Exception in APGCC::logWarning()";
		}

		va_end(args);

		// Freeze stream
		str << ends;
		char* sp = str.str();

		// Write data string
		fout << sp << endl;

		delete [] sp;

		fout.close();
#endif
		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	bool ACS_APGCC::logError(const char* s, ...)
	{
		UNUSED(s);
#if 0
		ACE_Guard<ACE_Thread_Mutex> guard( log_lock );

		std::string file;
		ACS_APGCC_Path path;
		APGCC_Registry reg;


		file = APGCC_LOGFILE;
		path.path("$MCS_LOGS/trace.log");

		if ( path.found() ) {
			file = path.path();
		}
		else {
			char tmp[512];
			if ( ::ExpandEnvironmentStrings("%temp%/trace.log",tmp,512) == 0 )
				file = "C:\trace.log";
			else
				file = tmp;
		}


		// Switch subfile if necessary
		switchFileOnSize(file.c_str(),MAXLOGSIZE,MAXBACKUPFILES);

		ofstream fout (file.c_str(), ios::app);

		if ( !fout ) {

			// Try to create the needed directories
			APGCC::create_directories("$MCS_LOGS");

			// Try one more time
			fout.open(file.c_str(), ios::app);

			if ( !fout ) {

				// Make one last try in current directory
				fout.open(MCS_LOGFILE, ios::app);

				if ( !fout )
					return false;

				// Write time and date stamp
				writeTimeMark(fout);

				// Write name of caller (process name)
				writeProcessInfo(fout);

				// Write severity
				fout << " ERROR: logError; Failed to open/create log file=" << file.c_str() << endl;;
			}
		}

		// Write time and date stamp
		writeTimeMark(fout);

		// Write name of caller (process name)
		writeProcessInfo(fout);

		// Write severity
		fout << " ERROR: ";

		// Assemble output string
		int i = 0;
		int len = ::strlen(s);
		std::strstream str;
		const char* cp;
		char c;
		int n;
		unsigned int un;
		char ns[10];
		va_list args;

		va_start(args, s);

		try {

			while ( i < len ) {

				switch ( s[i] )
				{
				case '%':
				i++;
				switch (s[i])
				{
				case 'c':
					c = va_arg(args, char);
					str << c;
					break;

				case 's':
					cp = va_arg(args, const char*);
					if (cp)
						str << cp;
					break;

				case 'i':
					n = va_arg(args, int);
					::sprintf(ns,"%i",n);
					str << ns;
					break;

				case 'u':
					un = va_arg(args, unsigned int);
					::sprintf(ns,"%u",un);
					str << ns;
					break;

				case 'x':
					n = va_arg(args, int);
					::sprintf(ns,"%x",n);
					str << ns;
					break;

				case 'X':
					n = va_arg(args, int);
					::sprintf(ns,"%X",n);
					str << ns;
					break;

				case 'n':
					str << endl;
					break;

				default:
					str << s[i];
				}
				break;

				default:
					str << s[i];
				}

				i++;
			}//while

		}
		catch(...) {
			str << "Exception in APGCC::logError()";
		}

		va_end(args);

		// Freeze stream
		str << ends;
		char* sp = str.str();

		// Write data string
		fout << sp << endl;

		delete [] sp;

		fout.close();
#endif
		return true;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::format_hexdump (const void* databuf, int size, char* obuf, int obuf_sz)
	{
		const char* buffer = (const char*) databuf;
		unsigned char c;
		char textver[16 + 1];

		int maxlen = (obuf_sz / 68) * 16;

		if (size > maxlen)
			size = maxlen;

		int i;

		for (i = 0; i < (size >> 4); i++) {
			int j;

			for (j = 0 ; j < 16; j++) {
				c = (unsigned char) buffer[(i << 4) + j];
				::sprintf (obuf, "%02x ", c);
				obuf += 3;
				if (j == 7) {
					::sprintf (obuf, " ");
					obuf++;
				}
				textver[j] = (c < 0x20 || c > 0x7e) ? '.' : c;
			}

			textver[j] = 0;

			::sprintf (obuf, "  %s\n", textver);

			while (*obuf != '\0')
				obuf++;
		}

		if (size % 16) {
			for (i = 0 ; i < size % 16; i++) {
				c = (unsigned char) buffer[size - size % 16 + i];
				::sprintf (obuf, "%02x ",c);
				obuf += 3;
				if (i == 7) {
					::sprintf (obuf, " ");
					obuf++;
				}
				textver[i] = (c < 0x20 || c > 0x7e) ? '.' : c;
			}

			for (i = size % 16; i < 16; i++) {
				::sprintf (obuf, "   ");
				obuf += 3;
				textver[i] = ' ';
			}

			textver[i] = 0;
			::sprintf (obuf, "  %s\n", textver);
		}

		return size;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int ACS_APGCC::format_hexdump (const void* databuf, int size, std::string& ostr)
	{
		const char* buffer = (const char*) databuf;
		unsigned char c;
		char textver[16 + 1];
		char obuf[20];

		ostr = "";

		int i;
		//int oi = 0;

		for (i = 0; i < (size >> 4); i++) {
			int j;

			for (j = 0 ; j < 16; j++) {
				c = (unsigned char) buffer[(i << 4) + j];
				::sprintf (obuf, "%02x ", c);
				ostr += obuf;
				if (j == 7) {
					ostr += " ";
				}
				textver[j] = (c < 0x20 || c > 0x7e) ? '.' : c;
			}

			textver[j] = 0;

			::sprintf (obuf, "  %s\n", textver);
			ostr += obuf;
		}

		if (size % 16) {
			for (i = 0 ; i < size % 16; i++) {
				c = (unsigned char) buffer[size - size % 16 + i];
				::sprintf (obuf, "%02x ",c);
				ostr += obuf;
				if (i == 7) {
					ostr += " ";
				}
				textver[i] = (c < 0x20 || c > 0x7e) ? '.' : c;
			}

			for (i = size % 16; i < 16; i++) {
				ostr += "   ";
				textver[i] = ' ';
			}

			textver[i] = 0;
			::sprintf (obuf, "  %s\n", textver);
			ostr += obuf;
		}

		return size;
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::dump_hex(const char* buf, int len, const char* text)
	{
		dump_hex( (const unsigned char*)buf, len, text );
	}


	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void ACS_APGCC::dump_hex(const unsigned char* buf, int len, const char* text)
	{
		{
			ACE_Guard<ACE_Thread_Mutex> guard( dump_lock );

			int i;
			int j=0;
			int prefix=0;
			char line[20];

			if (text)
				printf("%s - ",text);

			printf("HEXDUMP %i bytes",len);

			for (i=0; i<len; i++) {

				if (i % 16==0) {
					if (i > 0)
						printf("  %s", line);
					j = 0;
					strcpy(line, "................");
					printf("\n%4x: ",prefix);
					prefix += 16;
				}

				if (i % 8 == 0) {
					printf(" ");
				}

				if ( isprint(buf[i]) ) {
					line[j] = buf[i];
				}
				j++;

				printf("%02x ",buf[i]);

			}

			if (j > 0) {
				for (i=0; i<48-3*j; i++)
					printf(" ");
				printf("  %s", line);
			}

			printf("\n");

		} // Guard

	}
