/*=================================================================== */
/**
   @file   acs_aca_ms_parameters.h

   @brief Header file for aca module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMS_Parameters_H 
#define ACAMS_Parameters_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Synch.h>
/*=====================================================================
                          STRUCTURE DECLARATION SECTION
==================================================================== */
struct ACAMS_Parameters {
	char site[16];
	char name[16];
	bool nameChanged; 
	unsigned short noOfRecords;
	unsigned short recordSize;
	unsigned short noOfFiles;
	unsigned short noOfConn;
	unsigned short winSize;
	unsigned short echoValue;
	unsigned short alarmSeverity;
	unsigned short alarmLevel;
	unsigned short noOfResends;
	unsigned short resendTime;
	unsigned short reconnTime;
	unsigned short FAV;
	char echo[4];
	char ACFilledLevel[3];
	char MS_home[64];
};

#endif
