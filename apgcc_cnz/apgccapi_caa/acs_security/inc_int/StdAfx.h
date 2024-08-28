// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if !defined(AFX_STDAFX_H__913A708C_5494_11D3_A528_00C04FF680D8__INCLUDED_)
#define AFX_STDAFX_H__913A708C_5494_11D3_A528_00C04FF680D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0500
#define VC_EXTRALEAN

//#include <windows.h>
//#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
//#include <lm.h>
//#include <lmaccess.h>
//#include <lmcons.h>
//#include <lmerr.h>
//#include <lmapibuf.h>
//#include <lmwksta.h>
//#include <aclapi.h>
//#include <io.h>
//#include <direct.h>
//#include <process.h>
#include <errno.h>
#include <time.h>
#include <string.h>
//#include <winperf.h>

#define TITLE_SIZE          64
#define PROCESS_SIZE        8*1024 //ext3 does not have a limit so we set to 8K to cover all APG cases.
#define REGKEY_PERF         "software\\microsoft\\windows nt\\currentversion\\perflib"
#define REGSUBKEY_COUNTERS  "Counters"
#define INITIAL_SIZE        51200
#define PROCESS_COUNTER     "process"
#define PROCESSID_COUNTER   "id process"
#define UNKNOWN_TASK        "unknown"
#define EXTEND_SIZE         25600
#define MAX_TASKS           256


typedef struct _TASK_LIST {
    unsigned long   dwProcessId;
    unsigned long   dwInheritedFromProcessId;
    bool       flags;
    void     *hwnd;
    char        ProcessName[PROCESS_SIZE];
    char        WindowTitle[TITLE_SIZE];
} TASK_LIST, *PTASK_LIST;

typedef struct _TASK_LIST_ENUM {
    PTASK_LIST  tlist;
    unsigned long  numtasks;
} TASK_LIST_ENUM, *PTASK_LIST_ENUM;

//void WriteLog(LPTSTR lpLine);

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__913A708C_5494_11D3_A528_00C04FF680D8__INCLUDED_)
