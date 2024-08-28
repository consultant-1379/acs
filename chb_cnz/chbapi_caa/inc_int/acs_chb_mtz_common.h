/*=================================================================== */
/**
	@file		acs_chb_mtz_common.h

	@brief		Header file for MTZ  module.

				This module contains all the declarations for
				common utilities.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/11/2010     XTANAGG   Initial Release
==================================================================== */
//******************************************************************************
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_MTZ_Common_H
#define ACS_CHB_MTZ_Common_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Event.h>
#include <map>
#include <fstream>
#include <iostream>
#include <ACS_TRA_trace.h>
using namespace std;

/**
 *	@struct		_SYSTEMTIME
 *
 *	@brief		This structure stores the SYSTEMTIME.
 *
 *	@par		wYear
 *
 *	@par		wMonth
 *
 *	@par		wDayOfWeek
 *
 *	@par		wDay
 *
 *	@par		wHour
 *
 *	@par		wMinute
 *
 *	@par		wSecond
 *
 *	@par		wMilliseconds
 */
  struct _SYSTEMTIME
  {
	  /**
	   * @brief		wYear
	   */
	  ACE_UINT16	wYear;

	  /**
	   * @brief		wMonth
	   */
	  ACE_UINT16	wMonth;

	  /**
	   * @brief		wDayOfWeek
	   */
	  ACE_UINT16	wDayOfWeek;

	  /**
	   * @brief		wDay
	   */
	  ACE_UINT16	wDay;

	  /**
	   * @brief		wHour
	   */
	  ACE_UINT16	wHour;

	  /**
	   * @brief		wMinute
	   */
	  ACE_UINT16	wMinute;

	  /**
	   * @brief		wSecond
	   */
	  ACE_UINT16	wSecond;

	  /**
	   * @brief		wMilliseconds
	   */
	  ACE_UINT16	wMilliseconds;
  };

 /**
  *	@brief	SYSTEMTIME
  *
  */
typedef struct _SYSTEMTIME SYSTEMTIME;

/**
 * @brief	PSYSTEMTIME
 */
typedef struct _SYSTEMTIME *PSYSTEMTIME;
/**
 *	@struct		_TIME_ZONE_INFORMATION
 *
 *	@brief		This structure stores the time zone information.
 *
 *	@par		Bias
 *
 *	@par		StandardName
 *
 *	@par		StandardDate
 *
 *	@par		StandardBias
 *
 *	@par		DaylightName
 *
 *	@par		DaylightDate
 *
 *	@par		DaylightBias
 */
  struct _TIME_ZONE_INFORMATION
  {
	  /**
	   * @brief		Bias
	   */
	  ACE_INT32		Bias;

	  /**
	   * @brief		StandardName
	   */
	  ACE_TCHAR		StandardName[32];

	  /**
	   * @brief		StandardDate
	   */
	  SYSTEMTIME	StandardDate;

	  /**
	   * @brief		StandardBias
	   */
	  ACE_INT32		StandardBias;

	  /**
	   * @brief		DaylightName
	   */
	  ACE_TCHAR		DaylightName[32];

	  /**
	   * @brief		DaylightDate
	   */
	  SYSTEMTIME	DaylightDate;

	  /**
	   * @brief		DaylightBias
	   */
	  ACE_INT32		DaylightBias;
  };

 /**
  *	@brief		TIME_ZONE_INFORMATION
  */
typedef struct _TIME_ZONE_INFORMATION TIME_ZONE_INFORMATION;

/**
 * @brief		PTIME_ZONE_INFORMATION
 */
typedef struct _TIME_ZONE_INFORMATION *PTIME_ZONE_INFORMATION;
/**
 *	@brief		SECSPERMIN
 */
#ifndef SECSPERMIN
#define SECSPERMIN	60
#endif /* !defined SECSPERMIN */

/**
 *	@brief		MINSPERHOUR
 */
#ifndef MINSPERHOUR
#define MINSPERHOUR	60
#endif /* !defined MINSPERHOUR */

/**
 *	@brief		SECSPERHOUR
 */
#ifndef SECSPERHOUR
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#endif /* !defined SECSPERHOUR */

/**
 *	@brief		HOURSPERDAY
 */
#ifndef HOURSPERDAY
#define HOURSPERDAY	24
#endif /* !defined HOURSPERDAY */

/**
 *	@brief		EPOCH_YEAR
 */
#ifndef EPOCH_YEAR
#define EPOCH_YEAR	1970
#endif /* !defined EPOCH_YEAR */

/**
 *	@brief		TM_YEAR_BASE
 */
#ifndef TM_YEAR_BASE
#define TM_YEAR_BASE	1900
#endif /* !defined TM_YEAR_BASE */

/**
 *	@brief		DAYSPERNYEAR
 */
#ifndef DAYSPERNYEAR
#define DAYSPERNYEAR	365
#endif /* !defined DAYSPERNYEAR */

/**
 * 	@brief		isleap
 * 				This methods checks whether the particular year is a leap year or not.
 */
#ifndef isleap
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)
#endif /* !defined isleap */

/**
 *	@brief		MAX_NO_TIME_ZONES
 */
#define MAX_NO_TIME_ZONES 24

/**
 *	@brief		TZ_DESCRIPTION
 */
#define TZ_DESCRIPTION  "ObjectAcsCHB_TZParam"

/**
 *	@brief		PARENT_OBJECT_DN
 */
//#define MTZ_PARENT_OBJECT_DN "clockSyncHeartBeatMId=1,safApp=safImmService"

/**
 *	@brief		TZ_ID
 */
#define TZ_ID					"timeZoneId"

/**
 *	@brief		TZ_STRING
 */
#define TZ_STRING				"timeZoneString"

/**
 *	@brief		TZ_DN_BASE
 */
#define TZ_DN_BASE				"timeZoneLinkId="

/**
 *	@brief		TZ_CL_NAME	
 */
#define TZ_CL_NAME "TimeZoneLink"

/**
 *	@brief		TZ_CL_ATTRNAME
 */
#define TZ_CL_ATTRNAME	"timeZoneLinkId"

/**
 *	@brief		MAX_DN_SIZE
 */
#define MAX_DN_SIZE				300

/**
 *	@brief		MAX_TZ_VALUE_SIZE
 */
#define MAX_TZ_VALUE_SIZE		3

/**
 *	@brief		MAX_TZ_VALUE_SIZE
 */
#define TZ_LOCATION_LINUX		"/usr/share/zoneinfo/"

/**
 *	@brief		MAX_TZ_VALUE_SIZE
 */
#define MTZDIR "mtz/"

/**
 *	@brief		FILENAME
 */
#define FILENAME  "ACS_TZ_links.cfg"

/**
 *	@brief		PATHLENGTH
 */
static const ACE_INT32  PATHLENGTH = 200;

/**
 * 	@brief		TIMEZONESECTNAME
 */
#define TIMEZONESECTNAME	"ACS_TZ_links"

/**
 *	@brief		CPTIMESECTNAME
 */
#define CPTIMESECTNAME  "ACS_CPTime"

/**
 *	@brief		CPTIMEKEYNAME
 */
#define CPTIMEKEYNAME "CPTimeSec"

/**
 *	@brief		UPDATEDKEYNAME
 */
#define UPDATEDKEYNAME  "UpDated"

/**
 *	@brief		CHB_MTZ_DIRPATH
 */
#define CHB_MTZ_DIRPATH		"/data/acs/data/mtz"

/**
 *	@brief		ACS_CHB_TIMEZONE
 */
#define ACS_CHB_TIMEZONE	"acs_chbtimezone"


#endif


