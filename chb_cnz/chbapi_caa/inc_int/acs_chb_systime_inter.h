/*=================================================================== */
/**
	@file		acs_chb_systime_inter.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations useful to
				specify the class.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/11/2010    XTANAGG    Initial Release
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef __ACS_CHB_SysTimeInter__
#define __ACS_CHB_SysTimeInter__

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/*===================================================================*/
/**
	@brief		ACS_CHB_SysTimeInter

				This class is a base class for utility functions.
*/
/*=================================================================== */
class ACS_CHB_SysTimeInter
{
/*=====================================================================
						PROTECTED DECLARATION SECTION
==================================================================== */
protected:

/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
/**
 *	@brief		Class Constructor
 *
 *	@return		None.
 */
ACS_CHB_SysTimeInter(){};

/**
 *	@brief		Virtual method.
 *				Get the GMT time from the system time.
 *
 * 	@param		GMTtime
 * 				pointer to GMTtime
 *
 *	@param		TMZ
 *				Time Zone Number
 *
 *	@return		struct tm*
 */
virtual struct tm* get_TMZTime(struct tm * GMTtime,int TMZ,int daylightStatus)=0;

/**
 *	@brief		This method is a virtual method that converts UTC time to local time for a time zone.
 *
 *	@param		UTCtime
 * 				Time in UTC.
 *
 *	@param 		LocalTime
 * 				Local time.
 *
 *	@param		TMZ
 * 				Time Zone Number.
 *
 * 	@return		int
 */
virtual int UTCtoLocal(const time_t UTCtime,struct tm & LocalTime,int TMZ)=0;

/**
 *	@brief		This method is a virtual method that converts local time to UTC time.
 *
 *	@param		LocalTime
 * 				local time.
 *
 * 	@param 		TMZ
 * 				Time Zone Number
 *
 * @return		time_t
 */
virtual time_t LocalToUTC(struct tm * LocalTime,int TMZ,int daylightBias)=0;

/**
 *	@brief		This method is a virtual method that calculates the
				offset time between the system time	and a time zone.
 *
 * 	@param		TMZvalue
 * 				The time zone (value from 0 to 23) for which the
 * 				offset is calculated.
 *
 * 	@param		DSS
 *				DST status
 *
 *	@param		adjust
 *				1 means forward, 0 means backward
 *
 *	@return		time_t
 */
virtual time_t CalculateTheOffset(int TMZvalue, int &DSS,int &adjust)=0;

/**
 *	@brief		This method is used to obtain an approximation of the CP time.
 *
 *	@param		TimeOfCP
 *				A reference struct which handle the CP time information.
 *
 *	@return		int
 */
virtual int TimeFromCP(struct tm & TimeOfCP)=0;

/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
/**
 *	@brief		Virtual destructor for the class.
 *
 * 	@return		None.
 */
virtual ~ACS_CHB_SysTimeInter() {};

};

#endif
