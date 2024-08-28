//******************************************************************************
//
//  NAME
//     ACS_ALOG_Lister.h
//
//  COPYRIGHT
//     Ericsson Utvecklings AB, Sweden 2001. All rights reserved.
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
//	    190 89-CAA 109 0260
//
//  AUTHOR
//     2001-08-17 by UAB/S/GM UABCHSN
//
//  SEE ALSO
//     -
//
//******************************************************************************

#ifndef ACS_ALOG_LISTER_H
#define ACS_ALOG_LISTER_H



#include <acs_alog_fileDef.h>

class ACS_ALOG_Ls;

class ACS_ALOG_Lister
{
public:
   ACS_ALOG_Lister();
   ~ACS_ALOG_Lister();

public:
   // Methods to fetch attributes
   Status GetStatus();
   const ACE_TCHAR* GetFileDest();
   long GetMaximumFileSize();
   long GetCurrentFileSize();
   ACE_TCHAR* GetOldestLogfile();
   ACE_TCHAR* GetNewestLogfile();
   int GetRetCode();
   bool GetIdTag();  // new 0512
	 int GetLogRecordTimes(std::string&,std::string&,std::string&,std::string&);
   void makeFileListZIP();
private:
   ACS_ALOG_Ls* m_pLs;
};

#endif
