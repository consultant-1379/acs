//******************************************************************************
//
//  NAME
//     ACS_SSU_PHA.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2003. All rights reserved.
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
//	    190 89-CAA 109 0529
//
//  AUTHOR 
//     2003-04-07 by EAB/UKY/GM UABCHSN
//     2004-11-18 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************

#ifndef _ACS_SSU_PHA_H
#define _ACS_SSU_PHA_H

// The SSU CXC product number parameter
//#define SSU_PHA_PRODUCT             "ACS/CXC1371104"   //old for APG43
//#define SSU_PHA_PRODUCT             "ACS/CXC1371261"  // old cxc
#define SSU_PHA_PRODUCT             "ACS/CXC1371274"  

// Common performance counter parameter (Memory, Handles, ...)
#define SSU_PHA_PERFMONDATAITEM     "ACS_SSUBIN_PerfMonDataItem"

// Default basic disk monitor performance counter parameter
#define SSU_PHA_BASICDISKMONDATA    "ACS_SSUBIN_BasicDiskMonData"

// Default disk monitor alarm/cease settings parameter ("low" and "high" alarm)
#define SSU_PHA_DEFDISKMONLOW       "ACS_SSUBIN_DefDiskMonLow"
#define SSU_PHA_DEFDISKMONHIGH      "ACS_SSUBIN_DefDiskMonHigh"

// Specific user defined alarm/cease settings parameter
#define SSU_PHA_DISKMONITOR         "ACS_SSUBIN_DiskMonitor"

// Folder quota monitor items
#define SSU_PHA_FOLDERQUOTAMONITOR  "ACS_SSUBIN_FolderQuotaItem"

#endif
