//******************************************************************************
//
// NAME
//      ACS_USA_EventSource - Loads event source DLL and EXE files
//
// COPYRIGHT
//      Ericsson AB 2006 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
// 	    ACS_USA_EventSource handles loading of event source DLL and EXE files
//      to be used when fetching event data.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    2006-04-13 by EAB/AZA/DH UABPEK
//
// REV  DATE    NAME     DESCRIPTION
// A    060413  UABPEK   Adapted for USA. Based on ACS_ALOG_EventSource.h.
//
//******************************************************************************

#ifndef ACS_USA_EVENTSOURCE_H
#define ACS_USA_EVENTSOURCE_H

#include <tchar.h>

class ACS_USA_EventSource
{
public:
   ACS_USA_EventSource(const TCHAR* lpszLog, const TCHAR* lpszSource);
   ~ACS_USA_EventSource();

   typedef struct _EventSourceFile
   {
      _TCHAR  FileName[128];
      HMODULE Module;
      struct  _EventSourceFile* Next;
   } EventSourceFile;

public:
   BOOL LoadSourceModules();
   void UnloadSourceModules();

public:
   const HMODULE get_CategoryModule();
   const HMODULE get_EventModule();
   const HMODULE get_NextEventModule();
   const HMODULE get_ParameterModule();

   const BOOL get_IsLoaded() const;

private:
   void ClearEventSourceFileList(EventSourceFile& srctSourceFiles);
   BOOL GetSourceFiles();
   BOOL GetSourceFiles(const HKEY hKey,
                       const _TCHAR* lpszValueName,
                       EventSourceFile& srctSourceFiles);
   BOOL LoadEventSourceFiles(EventSourceFile& srctSourceFiles);
   void UnloadEventSourceFiles(EventSourceFile& srctSourceFiles);

public:
   ACS_USA_EventSource* Next;
   TCHAR m_szLog[24];
   TCHAR m_szSource[64];

private:
   EventSourceFile  m_srctCategorySourceFiles;
   EventSourceFile  m_srctEventSourceFiles;
   EventSourceFile  m_srctParameterSourceFiles;

   EventSourceFile* m_pCurrentEventSource;
};

#endif