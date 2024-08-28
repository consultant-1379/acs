//******************************************************************************
//
//  NAME
//     ACS_SSU_Path.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2006. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0529
//
//  AUTHOR 
//     2005-02-28 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef ACS_SSU_PATH
#define ACS_SSU_PATH

/*! Data Disk path.Used in case not found in APGCC
 * @todo To be later replaced to /data once partitions are ready on LOTC
 */
#define ACS_SSU_DATA_DISK_PATH "/data/ACS"

#include <ace/ACE.h>
#include <map>
#include <string>
#include <iostream>

#include "acs_ssu_types.h"
 
class ACS_SSU_Path
{
public:
   ACS_SSU_Path();
   ACS_SSU_Path(const ACE_TCHAR* lpszpPath);
   ~ACS_SSU_Path();

public:
   void SetPath(const ACE_TCHAR* lpszNewPath);
   // Description:
   //    ?
   // Parameters:
   //    -                        -
   // Return value: 
   //    -
   // Additional information:
   //    -

   void SetPath(const ACS_SSU_Path& NewPath);
   // Description:
   //    ?
   // Parameters:
   //    -                        -
   // Return value: 
   //    -
   // Additional information:
   //    -

   const ACE_TCHAR* Path() const;
   // Description:
   //    ?
   // Parameters:
   //    -                        -
   // Return value: 
   //    -
   // Additional information:
   //    -

   const ACE_TCHAR* operator*() const;
   operator const ACE_TCHAR*() const;
   operator ACE_TCHAR*() const;
   bool Expand(const ACE_TCHAR* lpszPath); // expands internal path
   //FileM change
   bool ExpandFileMPath(const ACE_TCHAR* lpszPath); // expands internal path
   bool createMap();
   bool createFileMMap();
   ACE_TCHAR m_szPath[FILE_MAX_PATH];
protected:
   void Clear();
//   ACE_TCHAR m_szPath[FILE_MAX_PATH];

private:
   static std::map<std::string,std::string> m_mapInternalPaths;
   static std::map<std::string,std::string> m_mapInternalFileMPaths;
//   std::map<std::string,std::string> m_mapInternalPaths;
   void vGetInternalPath( std::string &szPathSrc, std::string &szPathDest );
};

#endif
