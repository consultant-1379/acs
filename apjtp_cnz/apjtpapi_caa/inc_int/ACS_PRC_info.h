/*=================================================================== */
/**
   @file   ACS_PRC_info.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class ACS_PRC_info.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/26/2010     XTANAGG        APG43 on Linux.
==================================================================== */
/*=====================================================================
					  DIRECTIVE DECLARATION SECTION
==================================================================== */
/*
 * Define _ACS_PRC_info_H_
 */
#if !defined(_ACS_PRC_info_H_)
#define _ACS_PRC_info_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>


/*=================================================================== */
	/**
		@class			ACS_PRC_info

		@brief			This is the class for getting value of restart counter.
	**/
/*=================================================================== */
class ACS_PRC_info {
/*=====================================================================
	                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
	/*=================================================================== */
		/**
			@brief			This is used to fetch timestamp
							of the file having the value of AP restart counter.

			@pre			none

			@post			none

			@return			unsigned char
							Value of restart counter.

			@exception		none
		*/
	/*=================================================================== */
	static unsigned char getSystemLifeId();


};	/* end ACS_PRC_info */

#endif
