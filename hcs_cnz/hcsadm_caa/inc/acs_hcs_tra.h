/*
 * ACS_HC_tra.h
 *
 *  Created on: May 16, 2011
 *      Author: tcsnarn1
 */

#ifndef ACS_HC_TRA_H_
#define ACS_HC_TRA_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ACS_TRA_trace.h>
//#include <ACS_TRA_Logging.h>
#include "acs_hcs_logger.h"

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



#ifdef LMDEBUG
#define DEBUG(FMT, ...) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX FMT ,\
__VA_ARGS__))

#define INFO(FMT, ...) \
ACE_DEBUG(( LM_INFO, \
INFO_PREFIX FMT ,\
__VA_ARGS__))

#define NOTICE(FMT, ...) \
ACE_DEBUG(( LM_NOTICE, \
NOTICE_PREFIX FMT ,\
__VA_ARGS__))

#define ERROR(FMT, ...) \
ACE_DEBUG(( LM_ERROR, \
ERROR_PREFIX FMT ,\
__VA_ARGS__))

#define CRITICAL(FMT, ...) \
ACE_DEBUG(( LM_CRITICAL, \
CRITICAL_PREFIX FMT ,\
__VA_ARGS__))

#define ALERT(FMT, ...) \
ACE_DEBUG(( LM_ALERT, \
ALERT_PREFIX FMT ,\
__VA_ARGS__))

#define EMERGENCY(FMT, ...) \
ACE_DEBUG(( LM_EMERGENCY, \
EMERGENCY_PREFIX FMT ,\
__VA_ARGS__))

#else  // LMDEBUG

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

/*=================================================================== */
/**
   @brief           INFO
*/
/*=================================================================== */

#define INFO(FMT, ...) \
		ACS_HC_Tra::helperTraceInformation(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           DEBUG
*/
/*=================================================================== */

#define DEBUG(FMT, ...) \
	   ACS_HC_Tra::helperTraceDebug(FMT,__VA_ARGS__);


/*=================================================================== */
/**
   @brief           WARNING
*/
/*=================================================================== */

#define WARNING(FMT, ...) \
	    ACS_HC_Tra::helperTraceWarning(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           ERROR
*/
/*=================================================================== */

#define ERROR(FMT, ...) \
	     ACS_HC_Tra::helperTraceError(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           FATAL
*/
/*=================================================================== */

#define FATAL(FMT, ...) \
         ACS_HC_Tra::helperTraceFatal(FMT,__VA_ARGS__);

#endif

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */

/*=================================================================== */
class ACS_HC_Tra
{
public:
/*=================================================================== */
/*=================================================================== */
     /**
        @brief		 helperTraceInformation


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceInformation(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceDebug


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceDebug(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceWarning


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceWarning(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceError


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceError(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief      helperTraceFatal

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
static void helperTraceFatal(const char* messageFormat, ...);


/*=================================================================== */
     /**
        @brief      ACS_HC_InformationTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_HC_InformationTrace;

/*=================================================================== */
     /**
        @brief      ACS_HC_WarningTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_HC_WarningTrace;

/*=================================================================== */
     /**
        @brief      ACS_HC_ErrorTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_HC_ErrorTrace;

/*=================================================================== */
     /**
        @brief      ACS_HC_DebugTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_HC_DebugTrace;

/*=================================================================== */
     /**
        @brief      ACS_HC_FatalTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_HC_FatalTrace;

/*=================================================================== */
     /**
        @brief      ACS_HC_Logging

      */
  /*=================================================================== */
//extern ACS_TRA_Logging ACS_HC_Logging;
};

#endif /* ACS_HC_TRA_H_ */
