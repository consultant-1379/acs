/*=================================================================== */
/**
   @file   acs_alog_fileDef.h

   @brief Header file for acs_alog_cmdHandler type module.

          This module contains all the declarations useful to
          specify the class.

   @version 2.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/02/2011     xgencol/xgaeerr       Initial Release
   N/A       07/08/2012     xbhakat               Final Release
==================================================================== */

#ifndef ACS_ALOG_FILEDEF_H
#define ACS_ALOG_FILEDEF_H


#include <ace/ACE.h>
#include <string.h>
#include <iostream>
enum Status
{
   passivee,
   activee
};

class ACS_ALOG_AttrFileRecord
{
public:
   Status status;               // ALOG status
   int    FileSize;             // Max file size expressed in Mb
   ACE_TCHAR  FileDest[128+1];      // File destination for GOH
   bool   IdTag;				// new 0512 Command-id and session-id from MTS/MSS logging
};

class ACS_ALOG_Exclude
{
public:
   // Default constructor
   ACS_ALOG_Exclude(const ACE_TCHAR* lpszData1 = (""), const ACE_TCHAR* lpszData2 = (""))
   {
	   strcpy(Data1, lpszData1);
	   strcpy(Data2, lpszData2);
   }

public:
   // Overloaded operators, used when STL-list object sorting the list
   friend bool operator < (const ACS_ALOG_Exclude &Exclude1, const ACS_ALOG_Exclude &Exclude2)
   {
      return (strcmp(Exclude1.Data1, Exclude2.Data1) < 0);
   }
   friend bool operator > (const ACS_ALOG_Exclude &Exclude1, const ACS_ALOG_Exclude &Exclude2)
   {
      return (strcmp(Exclude1.Data1, Exclude2.Data1) > 0);
   }
   friend bool operator == (const ACS_ALOG_Exclude &Exclude1, const ACS_ALOG_Exclude &Exclude2)
   {
      return (strcmp(Exclude1.Data1, Exclude2.Data1) == 0);
   }
   friend bool operator != (const ACS_ALOG_Exclude &Exclude1, const ACS_ALOG_Exclude &Exclude2)
   {
      return (strcmp(Exclude1.Data1, Exclude2.Data1) != 0);
   }

public:
   ACE_TCHAR Data1[500];
   ACE_TCHAR Data2[500];
};

class ACS_ALOG_FileRecord
{
public:
   // Default constructor
   ACS_ALOG_FileRecord(const ACE_TCHAR* lpszFilename = (""),
                       const long nSize = 0,
                       const unsigned int nStatus = 255)
   {
      strcpy(Filename, lpszFilename);
      Size = nSize;
      Status = nStatus;
   }

public:
   // Overloaded operators, used when STL-list object sorting the list
   friend bool operator < (const ACS_ALOG_FileRecord &Rec1, const ACS_ALOG_FileRecord &Rec2)
   {
      return (strcmp(Rec1.Filename, Rec2.Filename) < 0);
   }
   friend bool operator > (const ACS_ALOG_FileRecord &Rec1, const ACS_ALOG_FileRecord &Rec2)
   {
      return (strcmp(Rec1.Filename, Rec2.Filename) > 0);
   }
   friend bool operator == (const ACS_ALOG_FileRecord &Rec1, const ACS_ALOG_FileRecord &Rec2)
   {
      return (strcmp(Rec1.Filename, Rec2.Filename) == 0);
   }
   friend bool operator != (const ACS_ALOG_FileRecord &Rec1, const ACS_ALOG_FileRecord &Rec2)
   {
      return (strcmp(Rec1.Filename, Rec2.Filename) != 0);
   }

public:
   // Public members
   ACE_TCHAR Filename[32+1];
   long Size;
   unsigned int Status;
};

#endif
