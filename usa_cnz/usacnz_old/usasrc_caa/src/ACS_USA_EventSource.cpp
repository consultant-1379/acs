//******************************************************************************
//
// NAME
//      ACS_USA_EventSource.cpp
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
// A    060418  UABPEK   Adapted for USA. Based on ACS_ALOG_EventSource.cpp.
//
//******************************************************************************

#include <stdio.h>
#include <windows.h>

#include "ACS_USA_EventSource.h"

#define EVENT_REGKEY_SOURCE _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\%s\\%s")

//============================================================================
// Constructor
//============================================================================
ACS_USA_EventSource::ACS_USA_EventSource(const TCHAR* lpszLog, const TCHAR* lpszSource)
{
   Next = NULL;
   _tcscpy(m_szLog, lpszLog);
   _tcscpy(m_szSource, lpszSource);

   *m_srctCategorySourceFiles.FileName = _T('\0');
   m_srctCategorySourceFiles.Module = NULL;
   m_srctCategorySourceFiles.Next = NULL;

   *m_srctEventSourceFiles.FileName = _T('\0');
   m_srctEventSourceFiles.Module = NULL;
   m_srctEventSourceFiles.Next = NULL;

   *m_srctParameterSourceFiles.FileName = _T('\0');
   m_srctParameterSourceFiles.Module = NULL;
   m_srctParameterSourceFiles.Next = NULL;

   m_pCurrentEventSource = &m_srctEventSourceFiles;

   GetSourceFiles();
}

//============================================================================
// Destructor
//============================================================================
ACS_USA_EventSource::~ACS_USA_EventSource()
{
   // Unload the loaded source dll's
   UnloadSourceModules();

   ClearEventSourceFileList(m_srctCategorySourceFiles);
   ClearEventSourceFileList(m_srctEventSourceFiles);
   ClearEventSourceFileList(m_srctParameterSourceFiles);

   *m_srctCategorySourceFiles.FileName = _T('\0');
   *m_srctEventSourceFiles.FileName = _T('\0');
   *m_srctParameterSourceFiles.FileName = _T('\0');
}

//============================================================================
// Free all EventSourceFile items pointed by the "Next" member
//============================================================================
void ACS_USA_EventSource::ClearEventSourceFileList(EventSourceFile& srctSourceFiles)
{
   EventSourceFile* pPtr = srctSourceFiles.Next;
   srctSourceFiles.Next = NULL;

   while (pPtr)
   {
      EventSourceFile* pNext = pPtr->Next;
      ::HeapFree(::GetProcessHeap(), 0, pPtr);
      pPtr = pNext;
   }
}

//============================================================================
// Returns a reference to the loaded category source dll
//============================================================================
const HMODULE ACS_USA_EventSource::get_CategoryModule()
{
   return m_srctCategorySourceFiles.Module;
}

//============================================================================
// Returns a reference to the loaded event text source dll
//============================================================================
const HMODULE ACS_USA_EventSource::get_EventModule()
{
   // Reset pointer to first event text file.
   m_pCurrentEventSource = &m_srctEventSourceFiles;

   return m_srctEventSourceFiles.Module;
}

//============================================================================
// Returns a reference to the next loaded event text source dll in the list
//============================================================================
const HMODULE ACS_USA_EventSource::get_NextEventModule()
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
const HMODULE ACS_USA_EventSource::get_ParameterModule()
{
    if (m_srctParameterSourceFiles.Module)
      return m_srctParameterSourceFiles.Module;
   else
      // If no parameter source dll exists, use the Event source dll instead
      return get_EventModule();
}

//============================================================================
// Checks wether some of the source moduls dll:s are loaded or not
//============================================================================
const BOOL ACS_USA_EventSource::get_IsLoaded() const
{
   if (*m_srctCategorySourceFiles.FileName && !m_srctCategorySourceFiles.Module)
      return FALSE;

   if (*m_srctEventSourceFiles.FileName && !m_srctEventSourceFiles.Module)
      return FALSE;

   if (*m_srctParameterSourceFiles.FileName && !m_srctParameterSourceFiles.Module)
      return FALSE;

   // All source module dll:s are loaded
   return TRUE;
}

//============================================================================
// Fetch the filenames of the source dll's of a event source from the registry
//============================================================================
BOOL ACS_USA_EventSource::GetSourceFiles()
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
   GetSourceFiles(hKey, _T("CategoryMessageFile"), m_srctCategorySourceFiles);

   // Get the EventMessageFile source dll names from the registry
   GetSourceFiles(hKey, _T("EventMessageFile"), m_srctEventSourceFiles);

   // Get the ParameterMessageFile source dll names from the registry
   GetSourceFiles(hKey, _T("ParameterMessageFile"), m_srctParameterSourceFiles);

   ::RegCloseKey(hKey);

   return TRUE;
}

//============================================================================
// Fetch the filenames of the source dll's of a event source from the registry
//============================================================================
BOOL ACS_USA_EventSource::GetSourceFiles(const HKEY hKey,
                                 const _TCHAR* lpszValueName,
                                 EventSourceFile& srctSourceFiles)
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
            _tcscpy(szModuleFileName, szTemp);
      }

      if (nReturn == ERROR_SUCCESS)
      {
         // There might be several messagefiles defined, just pick the first one
         _TCHAR* lpszPtr;
         if ((lpszPtr = _tcschr(szModuleFileName, ';')) != NULL)
         {
            *(lpszPtr++) = _T('\0');
            _tcscpy(srctSourceFiles.FileName, szModuleFileName);

            _TCHAR* lpszStart = lpszPtr;
            EventSourceFile* pSourceFile = &srctSourceFiles;
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
                     _tcscpy(pSourceFile->FileName, lpszStart);
                     lpszStart = lpszPtr;
                  }
                  else
                     _tcscpy(pSourceFile->FileName, lpszStart);
               }
               else
                  lpszPtr = NULL;
            }
         }
         else
           _tcscpy(srctSourceFiles.FileName, szModuleFileName);
      }
      else
        _tcscpy(srctSourceFiles.FileName, szModuleFileName);
   }

   return TRUE;
}

//============================================================================
// Load all associated source dll files
//============================================================================
BOOL ACS_USA_EventSource::LoadEventSourceFiles(EventSourceFile& srctSourceFiles)
{
   if (*srctSourceFiles.FileName && !srctSourceFiles.Module)
   {
      srctSourceFiles.FileName[127] = _T('\0');
      try
      {
        srctSourceFiles.Module = ::LoadLibraryEx(srctSourceFiles.FileName,
                                                 NULL,
                                                 LOAD_LIBRARY_AS_DATAFILE);
      }
      catch(...)
      {
         return FALSE;
      }
   }

   EventSourceFile* m_pSourceFile = srctSourceFiles.Next;
   while (m_pSourceFile)
   {
      if (*m_pSourceFile->FileName && !m_pSourceFile->Module)
      {
         m_pSourceFile->FileName[127] = _T('\0');
         try
         {
            m_pSourceFile->Module = ::LoadLibraryEx(m_pSourceFile->FileName,
                                                    NULL,
                                                    LOAD_LIBRARY_AS_DATAFILE);
         }
         catch(...)
         {
            return FALSE;
         }
      }

      m_pSourceFile = m_pSourceFile->Next;
   }
   return TRUE;
}

//============================================================================
// Load the category, event text, and the parameter source dll files
//============================================================================
BOOL ACS_USA_EventSource::LoadSourceModules()
{
   if(!LoadEventSourceFiles(m_srctCategorySourceFiles) ||
      !LoadEventSourceFiles(m_srctEventSourceFiles) ||
      !LoadEventSourceFiles(m_srctParameterSourceFiles))
   {
      return FALSE;
   }
   return TRUE;
}

//============================================================================
// Unload all associated source dll files
//============================================================================
void ACS_USA_EventSource::UnloadEventSourceFiles(EventSourceFile& srctSourceFiles)
{
   if (srctSourceFiles.Module)
   {
      ::FreeLibrary(srctSourceFiles.Module);
      srctSourceFiles.Module = NULL;
   }

   EventSourceFile* m_pSourceFile = srctSourceFiles.Next;
   while (m_pSourceFile)
   {
      ::FreeLibrary(m_pSourceFile->Module);
      m_pSourceFile->Module = NULL;
      m_pSourceFile = m_pSourceFile->Next;
   }
}

//============================================================================
// Unload source dll files
//============================================================================
void ACS_USA_EventSource::UnloadSourceModules()
{
   UnloadEventSourceFiles(m_srctCategorySourceFiles);
   UnloadEventSourceFiles(m_srctEventSourceFiles);
   UnloadEventSourceFiles(m_srctParameterSourceFiles);
}