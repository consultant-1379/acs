 /*=================================================================== */
/**
@file		acs_logm_tra.h

@brief		Header file of TRA related Functionality

This module contains all the declarations useful to specify the class.

@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       20/08/2012     XCSSATA       Initial Release
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_logm_tra_H_
#define acs_logm_tra_H_



/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include<sstream>
using namespace std;
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
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

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

#ifdef LOGMDEBUG
#define DEBUG(FMT, ...) \
ACE_DEBUG(( LOGM_DEBUG, \
DEBUG_PREFIX FMT ,\
__VA_ARGS__))

#define INFO(FMT, ...) \
ACE_DEBUG(( LOGM_INFO, \
INFO_PREFIX FMT ,\
__VA_ARGS__))

#define NOTICE(FMT, ...) \
ACE_DEBUG(( LOGM_NOTICE, \
NOTICE_PREFIX FMT ,\
__VA_ARGS__))

#define ERROR(FMT, ...) \
ACE_DEBUG(( LOGM_ERROR, \
ERROR_PREFIX FMT ,\
__VA_ARGS__))

#define CRITICAL(FMT, ...) \
ACE_DEBUG(( LOGM_CRITICAL, \
CRITICAL_PREFIX FMT ,\
__VA_ARGS__))

#define ALERT(FMT, ...) \
ACE_DEBUG(( LOGM_ALERT, \
ALERT_PREFIX FMT ,\
__VA_ARGS__))

#define EMERGENCYFMT, ...) \
ACE_DEBUG(( LOGM_EMERGENCY, \
EMERGENCY_PREFIX FMT ,\
__VA_ARGS__))

#else  // LOGMDEBUG

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

/*=================================================================== */
/**
   @brief           INFO
*/
/*=================================================================== */

#define INFO(FMT, ...) \
		helperTraceInformationLOGM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           DEBUG
*/
/*=================================================================== */

#define DEBUG(FMT, ...) \
	   helperTraceDebugLOGM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           WARNING
*/
/*=================================================================== */

#define WARNING(FMT, ...) \
	    helperTraceWarningLOGM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           ERROR
*/
/*=================================================================== */

#define ERROR(FMT, ...) \
	     helperTraceErrorLOGM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           FATAL
*/
/*=================================================================== */

#define FATAL(FMT, ...) \
         helperTraceFatalLOGM(FMT,__VA_ARGS__);

#endif

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */

/*=================================================================== */
/*=================================================================== */
     /**
        @brief		 helperTraceInformationLOGM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceInformationLOGM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceDebugLOGM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceDebugLOGM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceWarningLOGM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceWarningLOGM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceErrorLOGM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceErrorLOGM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief      helperTraceFatalLOGM

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceFatalLOGM(const char* messageFormat, ...);


/*=================================================================== */
     /**
        @brief      ACS_LOGM_InformationTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_ACS_LOGM_InformationTrace;

/*=================================================================== */
     /**
        @brief      ACS_LOGM_WarningTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LOGM_WarningTrace;

/*=================================================================== */
     /**
        @brief      ACS_LOGM_ErrorTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LOGM_ErrorTrace;

/*=================================================================== */
     /**
        @brief      ACS_LOGM_DebugTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LOGM_DebugTrace;

/*=================================================================== */
     /**
        @brief      ACS_LOGM_FatalTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LOGM_FatalTrace;

/*=================================================================== */
     /**
        @brief      ACS_LOGM_Logging

      */
  /*=================================================================== */
extern ACS_TRA_Logging ACS_LOGM_Logging;

#endif /* acs_logm_tra_H_ */
