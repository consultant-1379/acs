/*=================================================================== */
/**
   @file   ACS_JTP.h

   @brief  Header file for APJTP type module.

          This file is included by external applications which want to use JTP.

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
#if !defined(_ACS_JTP_H_)
#define _ACS_JTP_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "ace/ACE.h"

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#if !defined(__ACS_JTP_TYPES_INCLUDED__)
#define __ACS_JTP_TYPES_INCLUDED__

/*=================================================================== */
/**
   @brief 	JTP_HANDLE.

          JTP_HANDLE will be used as a socket descriptor to wait
          for incoming messages from remote destination.
 */
/*=================================================================== */
typedef ACE_HANDLE JTP_HANDLE;

#endif

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class ACS_JTP_Conversation_R3A;
class ACS_JTP_Job_R3A;
class ACS_JTP_Service_R3A;

#include "ACS_JTP_Conversation_R3A.h"
#include "ACS_JTP_Job_R3A.h"
#include "ACS_JTP_Service_R3A.h"

/*=================================================================== */
/**
   @brief 	ACS_JTP_Service.

          This referes the ACS_JTP_Service_R3A class.
 */
/*=================================================================== */
typedef ACS_JTP_Service_R3A			ACS_JTP_Service;

/*=================================================================== */
/**
   @brief 	ACS_JTP_Conversation.

          This referes the ACS_JTP_Conversation_R3A class.
 */
/*=================================================================== */
typedef ACS_JTP_Conversation_R3A    ACS_JTP_Conversation;

/*=================================================================== */
/**
   @brief 	ACS_JTP_Service.

          This referes the ACS_JTP_Job_R3A class.
 */
/*=================================================================== */
typedef ACS_JTP_Job_R3A				ACS_JTP_Job;

#endif
