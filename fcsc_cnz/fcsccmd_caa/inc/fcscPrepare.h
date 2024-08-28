/*=================================================================== */
/**
   @file   FCSC_PREPARE_H

   @brief Header file for SoftwareUpdate Module.

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
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef FCSC_PREPARE_H
#define FCSC_PREPARE_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
#endif
#ifndef FCSC_APG_REP_H
#include "fcsc_RepositoryMgr.h"
#endif
#ifndef FCSC_COMP_CHK_H
#include "fcsc_CompatibilityChecker.h"
#endif
#ifndef FCSC_DOWNLOADMGR_H_
#include "fcsc_DownloadMgr.h"
#endif


 /*===================================================================
                                   METHODS
 =================================================================== */

/*=================================================================== */
   /**
          @brief       Returns a pointer to the directory

          @par         None

          @pre         None

          @post        None

          @return      a pointer to the directory.

          @exception   None
        */
/*=================================================================== */
ACE_INT32 fcscPrepare();

/*=================================================================== */
   /**
          @brief       used for setting the environment variables

          @par         None

          @pre         None

          @post        None

          @return      a pointer to the directory.

          @exception   None
        */
/*=================================================================== */
ACE_INT32 setEnvVariables();

ACE_INT32 checkIntegrityCompatibility();
ACE_INT32 fcscPrepareWoOption();
#endif

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2001-2006
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

