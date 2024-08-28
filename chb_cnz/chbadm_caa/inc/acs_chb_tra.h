/*=================================================================== */
/**
   @file   acs_chb_tra.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the ACS_CHB_ClockSync class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011   XTANAGG      APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_TRA_H_
#define ACS_CHB_TRA_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
using namespace std;
#include <ace/ACE.h>
#include <ace/Mutex.h>
#include <ace/Log_Msg.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  DEBUG_PREFIX

 */
/*=================================================================== */
#define DEBUG_PREFIX "\n DEBUG%I"
/*=================================================================== */
/**
   @brief  INFO_PREFIX

 */
/*=================================================================== */
#define INFO_PREFIX "\n INFO%I"
/*=================================================================== */
/**
   @brief  NOTICE_PREFIX

 */
/*=================================================================== */
#define NOTICE_PREFIX "\n NOTICE%I"
/*=================================================================== */
/**
   @brief  WARNING_PREFIX

 */
/*=================================================================== */
#define WARNING_PREFIX "\n WARNING%I"
/*=================================================================== */
/**
   @brief  ERROR_PREFIX

 */
/*=================================================================== */
#define ERROR_PREFIX "\n ERROR%I"
/*=================================================================== */
/**
   @brief  CRITICAL_PREFIX

 */
/*=================================================================== */
#define CRITICAL_PREFIX "\n CRITICAL%I"
/*=================================================================== */
/**
   @brief  ALERT_PREFIX

 */
/*=================================================================== */
#define ALERT_PREFIX "\n ALERT%I"
/*=================================================================== */
/**
   @brief  EMERGENCY_PREFIX

 */
/*=================================================================== */
#define EMERGENCY_PREFIX "\n EMERGENCY%I"



#ifdef CHBDEBUG
#define DEBUG(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX FMT ,\
__VA_ARGS__))

#define INFO(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_INFO, \
INFO_PREFIX FMT ,\
__VA_ARGS__))

#define NOTICE(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_NOTICE, \
NOTICE_PREFIX FMT ,\
__VA_ARGS__))

#define ERROR(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_ERROR, \
ERROR_PREFIX FMT ,\
__VA_ARGS__))

#define CRITICAL(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_CRITICAL, \
CRITICAL_PREFIX FMT ,\
__VA_ARGS__))

#define ALERT(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_ALERT, \
ALERT_PREFIX FMT ,\
__VA_ARGS__))

#define EMERGENCY(IS_LOG,FMT, ...) \
ACE_DEBUG(( LM_EMERGENCY, \
EMERGENCY_PREFIX FMT ,\
__VA_ARGS__))

#else  // CHBDEBUG

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

/*=================================================================== */
/**
   @brief           INFO
*/
/*=================================================================== */

#define INFO(IS_LOG,FMT, ...) \
	ACS_CHB_Tra::helperTraceInformationCHB(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           DEBUG
*/
/*=================================================================== */

#define DEBUG(IS_LOG,FMT, ...) \
	   ACS_CHB_Tra::helperTraceDebugCHB(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           WARNING
*/
/*=================================================================== */

#define WARNING(IS_LOG,FMT, ...) \
	    ACS_CHB_Tra::helperTraceWarningCHB(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           ERROR
*/
/*=================================================================== */

#define ERROR(IS_LOG,FMT, ...) \
	     ACS_CHB_Tra::helperTraceErrorCHB(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           FATAL
*/
/*=================================================================== */

#define FATAL(IS_LOG,FMT, ...) \
         ACS_CHB_Tra::helperTraceFatalCHB(IS_LOG,FMT,__VA_ARGS__);

#endif

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */

/*=================================================================== */
class ACS_CHB_Tra
{
public:
/*=================================================================== */
     /**
        @brief		 helperTraceInformationCHB

        @param       isLog

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceInformationCHB(int isLog ,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceDebugCHB

        @param       isLog

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceDebugCHB(int isLog ,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceWarningCHB

        @param       isLog

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceWarningCHB(int isLog,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceErrorCHB

        @param       isLog

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceErrorCHB(int isLog,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief      helperTraceFatalCHB

        @param       isLog

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceFatalCHB(int isLog,const char* messageFormat, ...);


/*=================================================================== */
     /**
        @brief      ACS_CHB_InformationTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_CHB_InformationTrace;

/*=================================================================== */
     /**
        @brief      ACS_CHB_WarningTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_CHB_WarningTrace;

/*=================================================================== */
     /**
        @brief      ACS_CHB_ErrorTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_CHB_ErrorTrace;

/*=================================================================== */
     /**
        @brief      ACS_CHB_DebugTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_CHB_DebugTrace;

/*=================================================================== */
     /**
        @brief      ACS_CHB_FatalTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_CHB_FatalTrace;

/*=================================================================== */
     /**
        @brief      ACS_CHB_Logging

      */
  /*=================================================================== */
static ACS_TRA_Logging ACS_CHB_Logging;
static ACE_Mutex logMutex_;
};

#endif /* ACS_CHB_TRA_H_ */
