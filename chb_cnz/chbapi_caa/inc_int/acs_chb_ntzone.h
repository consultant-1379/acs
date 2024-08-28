/*=================================================================== */
/**
	@file		acs_chb_ntzone.h

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
N/A       25/11/2010     XTANAGG       Initial Release
==================================================================== */

/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_ACS_NtZone_H
#define ACS_CHB_ACS_NtZone_H

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <map>
#include <iostream>
#include "acs_chb_mtz_common.h"
using namespace std;


/*=====================================================================
					CONST DECLARATION SECTION
==================================================================== */

/**
 *	@brief		Maximum size of a time zone name.
 */
static const int   NTZoneSize = 200;

/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/**
 *	@brief		array[NTZoneSize]
 */
typedef  char array[NTZoneSize];

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
*	@brief		This class is used to store the time zone information.
*/
/*=================================================================== */

class ACS_CHB_NtZone {

/*=====================================================================
						PUBLIC DECLARATION SECTION
==================================================================== */
public:

/*==================================================================== */
	/**
	*	@brief		Default Constructor for ACS_CHB_NtZone class.
	*/
/*=================================================================== */
	ACS_CHB_NtZone();

	/*==================================================================== */
	/**
	*	@brief		Constructor for ACS_CHB_NtZone class.
	*
	*	@param		s
	*				Time Zone String
	*
	*	@param		TZ
	*				Pointer to TIME_ZONE_INFORMATION structure.
	*/
	/*=================================================================== */
	ACS_CHB_NtZone(const ACE_TCHAR* s, TIME_ZONE_INFORMATION* TZ) ;
	
	/**
	 *	@brief		This method is used to retrieve the
	 *				TIME_ZONE_INFORMATION.
	 *
	 * 	@return		Pointer to TIME_ZONE_INFORMATION.
	 */
	TIME_ZONE_INFORMATION *getTZI( );
	
	/**
	 * 	@brief		This method overloads the assignement operator
	 *				to copy one ACS_CHB_NtZone object to another.
	 *
	 *	@param		obj
	 *				ACS_CHB_NtZone
	 *
	 *	@return		ACS_CHB_NtZone
	 */
	ACS_CHB_NtZone operator=(const ACS_CHB_NtZone& obj) ;
    
	/**
	 * 	@brief		This method returns the name of time zone.
	 *
	 * 	@return		char *
	 *				Name of time zone.
	 */
	char *get();

	/*===================================================================
						PRIVATE DECLARATION SECTION
	=================================================================== */
private:

	/**
	 *	@brief		Name of Time Zone.
	 *
	 */
	array	str_time_zone;

	/**
	 * 	@brief		TIME_ZONE_INFORMATION
	 */
	TIME_ZONE_INFORMATION	Tzi;

};

#endif /* end ACS_CHB_ACS_NtZone_H */

