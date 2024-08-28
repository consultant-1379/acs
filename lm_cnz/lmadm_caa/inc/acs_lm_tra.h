/*
 * acs_lm_tra.h
 *
 *  Created on: May 16, 2011
 *      Author: tcsnarn1
 */

#ifndef ACS_LM_TRA_H_
#define ACS_LM_TRA_H_






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

#define EMERGENCYFMT, ...) \
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
		helperTraceInformationLM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           DEBUG
*/
/*=================================================================== */

#define DEBUG(FMT, ...) \
	   helperTraceDebugLM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           WARNING
*/
/*=================================================================== */

#define WARNING(FMT, ...) \
	    helperTraceWarningLM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           ERROR
*/
/*=================================================================== */

#define ERROR(FMT, ...) \
	     helperTraceErrorLM(FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           FATAL
*/
/*=================================================================== */

#define FATAL(FMT, ...) \
         helperTraceFatalLM(FMT,__VA_ARGS__);

#endif

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */

/*=================================================================== */
/*=================================================================== */
     /**
        @brief		 helperTraceInformationLM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceInformationLM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceDebugLM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceDebugLM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceWarningLM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceWarningLM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceErrorLM


        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceErrorLM(const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief      helperTraceFatalLM

        @param       messageFormat

	@return      void

      */
  /*=================================================================== */
void helperTraceFatalLM(const char* messageFormat, ...);


/*=================================================================== */
     /**
        @brief      ACS_LM_InformationTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LM_InformationTrace;

/*=================================================================== */
     /**
        @brief      ACS_LM_WarningTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LM_WarningTrace;

/*=================================================================== */
     /**
        @brief      ACS_LM_ErrorTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LM_ErrorTrace;

/*=================================================================== */
     /**
        @brief      ACS_LM_DebugTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LM_DebugTrace;

/*=================================================================== */
     /**
        @brief      ACS_LM_FatalTrace

      */
  /*=================================================================== */
extern ACS_TRA_trace ACS_LM_FatalTrace;

/*=================================================================== */
     /**
        @brief      ACS_LM_Logging

      */
  /*=================================================================== */
extern ACS_TRA_Logging ACS_LM_Logging;

#endif /* ACS_LM_TRA_H_ */
