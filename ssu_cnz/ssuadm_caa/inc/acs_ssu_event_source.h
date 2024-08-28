//******************************************************************************
//
//  NAME
//     EventSource.h
//
//  COPYRIGHT
//     Ericsson Utvecklings AB, Sweden 2002. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0xxx
//
//  AUTHOR 
//     2002-03-26 by UAB/UKB/GM UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef ACS_SSU_EVENTSOURCE_H
#define ACS_SSU_EVENTSOURCE_H

class ACS_SSU_EventSource
{
public:
   ACS_SSU_EventSource(const TCHAR* lpszLog, const TCHAR* lpszSource);
   ~ACS_SSU_EventSource();

   typedef struct _EventSourceFile
   {
      _TCHAR SourceFile[128];
      HMODULE Module;
      struct _EventSourceFile* Next;
   } EventSourceFile;

public:
   void LoadSourceModules();
   void UnloadSourceModules();

public:
   const HMODULE get_CategoryModule();
   const HMODULE get_EventModule();
   const HMODULE get_GuidModule();
   const HMODULE get_NextEventModule();
   const HMODULE get_ParameterModule();

   const BOOL get_IsLoaded() const;

private:
   void ClearEventSourceFileList(EventSourceFile& SourceFiles);
   BOOL GetSourceFiles();
   BOOL GetSourceFiles(const HKEY hKey,
                       const _TCHAR* lpszValueName,
                       EventSourceFile& SourceFiles);
   void LoadEventSourceFiles(EventSourceFile& SourceFiles);
   void UnloadEventSourceFiles(EventSourceFile& SourceFiles);

public:
   ACS_SSU_EventSource* Next;
   TCHAR m_szLog[24];
   TCHAR m_szSource[64];

private:
   EventSourceFile  m_CategorySourceFiles;
   EventSourceFile  m_EventSourceFiles;
   EventSourceFile  m_GuidSourceFiles;
   EventSourceFile  m_ParameterSourceFiles;

   EventSourceFile* m_pCurrentEventSource;
};

#endif