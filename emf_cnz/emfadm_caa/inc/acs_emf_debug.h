/*=================================================================== */
/**
   @file   acs_emf_debug.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A         DD/MM/YYYY  XRAMMAT   Initial Release
 */
/*==================================================================== */

#ifndef ACS_EMF_DEBUG_H
#define ACS_EMF_DEBUG_H

#include <windows.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif
/*=================================================================== */
 	/**
 			 @brief    	Format debug information in "printf"-style and send it to a debugger or standard output

 			 @par         None

 			 @pre         None

 			 @post        None

 			 @exception   None
 	*/
 	/*=================================================================== */
void DebugPrint(const _TCHAR* lpszFormat, ...);
/*=================================================================== */
 	/**
 			 @brief    End debug session. Release character buffer and critical section object

 			 @par      None

 			 @pre      None

 			 @post     None

 			 @exception   None
 	*/
 	/*=================================================================== */
void EndDebug();
/*=================================================================== */
 	/**
 			 @brief    	Initialize a new debug session. Allocate a character buffer to store debug information and create an critical section object to gain exclusive access

 			 @par       None

 			 @pre       None

 			 @post      None

 			 @param		bDebugInteractive : BOOL

 			 @return    BOOL

 			 @exception None
 	*/
 	/*=================================================================== */
BOOL InitDebug(const BOOL bDebugInteractive = FALSE);

#ifdef __cplusplus
}
#endif

#endif
