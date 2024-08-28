/*=================================================================== */
/**
   @file   acs_rtr__cmd_util.h

   @brief Header file for rtr cmd  module.

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
   N/A       27/02/2013   XLANSRI   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_CMD_UTIL_H
#define ACS_RTR_CMD_UTIL_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */

#include <ace/ACE.h>

/*=================================================================== */
/**
   @brief        strupr

   @param        string

   @return       ACE_TCHAR* 
*/
/*=================================================================== */

ACE_TCHAR* strupr(ACE_TCHAR* string);



ACE_TCHAR* strlwr(ACE_TCHAR* string);

ACE_INT32 stricmp( const ACE_TCHAR *s1, const ACE_TCHAR *s2 );


#endif
