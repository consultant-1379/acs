#if 0
//******************************************************************************
//
//  NAME
//     ACS_SSU_EventSource.cpp
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

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "acs_ssu_event_source.h"

#define EVENT_REGKEY_SOURCE _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\%s\\%s")


//============================================================================
// Constructor
//============================================================================
ACS_SSU_EventSource::ACS_SSU_EventSource(const TCHAR* lpszLog,
                                         const TCHAR* lpszSource)
{
   Next = NULL;
   (void)_tcsncpy(m_szLog, lpszLog, 23);
   (void)_tcsncpy(m_szSource, lpszSource, 63);

   *m_CategorySourceFiles.SourceFile = _T('\0');
   m_CategorySourceFiles.Module = NULL;
   m_CategorySourceFiles.Next = NULL;

   *m_EventSourceFiles.SourceFile = _T('\0');
   m_EventSourceFiles.Module = NULL;
   m_EventSourceFiles.Next = NULL;

   *m_GuidSourceFiles.SourceFile = _T('\0');
   m_GuidSourceFiles.Module = NULL;
   m_GuidSourceFiles.Next = NULL;

   *m_ParameterSourceFiles.SourceFile = _T('\0');
   m_ParameterSourceFiles.Module = NULL;
   m_ParameterSourceFiles.Next = NULL;

   m_pCurrentEventSource = &m_EventSourceFiles;

   (void)GetSourceFiles();
}

//============================================================================
// Destructor
//============================================================================
ACS_SSU_EventSource::~ACS_SSU_EventSource()
{
   try
   {
      // Unload the loaded source dll's
      UnloadSourceModules();

      (void)ClearEventSourceFileList(m_CategorySourceFiles);
      (void)ClearEventSourceFileList(m_EventSourceFiles);
      (void)ClearEventSourceFileList(m_GuidSourceFiles);
      (void)ClearEventSourceFileList(m_ParameterSourceFiles);

      *m_CategorySourceFiles.SourceFile = _T('\0');
      *m_EventSourceFiles.SourceFile = _T('\0');
      *m_GuidSourceFiles.SourceFile = _T('\0');
      *m_ParameterSourceFiles.SourceFile = _T('\0');
   }
   catch (...) { }
}

//============================================================================
// Free all EventSourceFile items pointed by the "Next" member
//============================================================================
void ACS_SSU_EventSource::ClearEventSourceFileList(EventSourceFile& SourceFiles)
{
   EventSourceFile* pPtr = SourceFiles.Next;
   SourceFiles.Next = NULL;

   while (pPtr)
   {
      EventSourceFile* pNext = pPtr->Next;
      (void)::HeapFree(::GetProcessHeap(), 0, pPtr);
      pPtr = pNext;
   }
}

//============================================================================
// Returns a reference to the loaded category source dll
//============================================================================
const HMODULE ACS_SSU_EventSource::get_CategoryModule()
{
   return m_CategorySourceFiles.Module;
}

//============================================================================
// Returns a reference to the loaded eventtext source dll
//============================================================================
const HMODULE ACS_SSU_EventSource::get_EventModule()
{
   m_pCurrentEventSource = &m_EventSourceFiles;
   return m_EventSourceFiles.Module;
}

//============================================================================
// Returns a reference to the loaded GUID source dll
//============================================================================
const HMODULE ACS_SSU_EventSource::get_GuidModule()
{
   return m_GuidSourceFiles.Module;
}

//============================================================================
// Returns a reference to the next loaded eventtext source dll in the list
//============================================================================
const HMODULE ACS_SSU_EventSource::get_NextEventModule()
{
   if (m_pCurrentEventSource)
      m_pCurrentEventSource = m_pCurrentEventSource->Next;

   if (m_pCurrentEventSource)
      return m_pCurrentEventSource->Module;
   else
      return NULL;
}

//============================================================================
// Returns a reference to the loaded parameter source dll
//============================================================================ 
const HMODULE ACS_SSU_EventSource::get_ParameterModule()
{
   // If no parameter source dll exists, use the Event source dll instead
   if (!m_ParameterSourceFiles.Module)
      return get_EventModule();
   else
      return m_ParameterSourceFiles.Module;
}

//============================================================================
// Checks wether some of the source moduls dll:s are loaded or not
//============================================================================
const BOOL ACS_SSU_EventSource::get_IsLoaded() const
{
   if (*m_CategorySourceFiles.SourceFile && !m_CategorySourceFiles.Module)
      return FALSE;

   if (*m_EventSourceFiles.SourceFile && !m_EventSourceFiles.Module)
      return FALSE;

   if (*m_GuidSourceFiles.SourceFile && !m_GuidSourceFiles.Module)
      return FALSE;

   if (*m_ParameterSourceFiles.SourceFile && !m_ParameterSourceFiles.Module)
      return FALSE;

   // All source module dll:s are loaded
   return TRUE;
}

//============================================================================
// Fetch the filenames of the source dll's of a event source from the registry
//============================================================================
BOOL ACS_SSU_EventSource::GetSourceFiles()
{
   // Get the filename + path of the module to be loaded
   TCHAR szRegKey[MAX_PATH];
   _stprintf(szRegKey, EVENT_REGKEY_SOURCE, m_szLog, m_szSource);

   HKEY hKey;
   LONG nReturn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                 szRegKey,
                                 NULL,
                                 KEY_QUERY_VALUE,
                                 &hKey);

   if (nReturn != ERROR_SUCCESS)
      return FALSE;

   // Get the CategoryMessageFile source dll names from the registry
   (void)GetSourceFiles(hKey, _T("CategoryMessageFile"), m_CategorySourceFiles);

   // Get the EventMessageFile source dll names from the registry
   (void)GetSourceFiles(hKey, _T("EventMessageFile"), m_EventSourceFiles);

   // Get the GuidMessageFile source dll names from the registry
   (void)GetSourceFiles(hKey, _T("GuidMessageFile"), m_GuidSourceFiles);

   // Get the ParameterMessageFile source dll names from the registry
   (void)GetSourceFiles(hKey, _T("ParameterMessageFile"), m_ParameterSourceFiles);

   (void)::RegCloseKey(hKey);

   return TRUE;
}

//============================================================================
// Fetch the filenames of the source dll's of a event source from the registry
//============================================================================
BOOL ACS_SSU_EventSource::GetSourceFiles(const HKEY hKey,
                                 const _TCHAR* lpszValueName,
                                 EventSourceFile& SourceFiles)
{
   _TCHAR szModuleFileName[MAX_PATH];
   DWORD dwType = REG_SZ;
   DWORD dwSize = MAX_PATH;
   LONG nReturn = ::RegQueryValueEx(hKey,
                                    lpszValueName,
                                    NULL,
                                    &dwType,
                                    (LPBYTE)szModuleFileName,
                                    &dwSize);

   if (nReturn == ERROR_SUCCESS)
   {
      if (dwType == REG_EXPAND_SZ)
      {
         _TCHAR szTemp[MAX_PATH];
         if (::ExpandEnvironmentStrings(szModuleFileName, szTemp, MAX_PATH-1) == 0)
            nReturn = ERROR_BAD_ENVIRONMENT;
         else
            (void)_tcsncpy(szModuleFileName, szTemp, 259);
      }

      if (nReturn == ERROR_SUCCESS)
      {
         // There might be several messagefiles defined, just pick the first one
         _TCHAR* lpszPtr;
         if ((lpszPtr = _tcschr(szModuleFileName, ';')) != NULL)
         {
            *(lpszPtr++) = _T('\0');
            (void)_tcsncpy(SourceFiles.SourceFile, szModuleFileName, 127);

            _TCHAR* lpszStart = lpszPtr;
            EventSourceFile* pSourceFile = &SourceFiles;
            while (lpszPtr)
            {
               pSourceFile->Next = (EventSourceFile*)::HeapAlloc(::GetProcessHeap(),
                                                                 HEAP_ZERO_MEMORY,
                                                                 sizeof(EventSourceFile));

               if (pSourceFile->Next)
               {
                  pSourceFile = pSourceFile->Next;
                  pSourceFile->Next = NULL;

                  if ((lpszPtr = _tcschr(lpszStart, ';')) != NULL)
                  {
                     *(lpszPtr++) = _T('\0');
                     (void)_tcsncpy(pSourceFile->SourceFile, lpszStart, 127);
                     lpszStart = lpszPtr;
                  }
                  else
                     (void)_tcsncpy(pSourceFile->SourceFile, lpszStart, 127);
               }
               else
                  lpszPtr = NULL;
            }
         }
         else
            (void)_tcsncpy(SourceFiles.SourceFile, szModuleFileName, 127);
      }
      else
         (void)_tcsncpy(SourceFiles.SourceFile, szModuleFileName, 127);
   }

   return TRUE;
}

//============================================================================
// Load all associated source dll files
//============================================================================
void ACS_SSU_EventSource::LoadEventSourceFiles(EventSourceFile& SourceFiles)
{
   if (*SourceFiles.SourceFile && !SourceFiles.Module)
   {
      SourceFiles.Module = ::LoadLibraryEx(SourceFiles.SourceFile,
                                           NULL,
                                           LOAD_LIBRARY_AS_DATAFILE);
   }

   EventSourceFile* m_pSourceFile = SourceFiles.Next;
   while (m_pSourceFile)
   {
      if (*m_pSourceFile->SourceFile && !m_pSourceFile->Module)
      {
         m_pSourceFile->Module = ::LoadLibraryEx(m_pSourceFile->SourceFile,
                                                 NULL,
                                                 LOAD_LIBRARY_AS_DATAFILE);
      }

      m_pSourceFile = m_pSourceFile->Next;
   }
}

//============================================================================
// Load the category, eventtext and the parameter source dll files
//============================================================================
void ACS_SSU_EventSource::LoadSourceModules()
{
   LoadEventSourceFiles(m_CategorySourceFiles);
   LoadEventSourceFiles(m_EventSourceFiles);
   LoadEventSourceFiles(m_GuidSourceFiles);
   LoadEventSourceFiles(m_ParameterSourceFiles);
}

//============================================================================
// Unload all associated source dll files
//============================================================================
void ACS_SSU_EventSource::UnloadEventSourceFiles(EventSourceFile& SourceFiles)
{
   if (SourceFiles.Module)
   {
      (void)::FreeLibrary(SourceFiles.Module);
      SourceFiles.Module = NULL;
   }

   EventSourceFile* m_pSourceFile = SourceFiles.Next;
   while (m_pSourceFile)
   {
      (void)::FreeLibrary(m_pSourceFile->Module);
      m_pSourceFile->Module = NULL;
      m_pSourceFile = m_pSourceFile->Next;
   }
}

//============================================================================
// Unload source dll files
//============================================================================
void ACS_SSU_EventSource::UnloadSourceModules()
{
   UnloadEventSourceFiles(m_CategorySourceFiles);
   UnloadEventSourceFiles(m_EventSourceFiles);
   UnloadEventSourceFiles(m_GuidSourceFiles);
   UnloadEventSourceFiles(m_ParameterSourceFiles);
}

#endif
