/*=================================================================== */
   /**
   @file  acs_emf_tra.h

   Class method implementation for EMF module.


   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       28/02/2011   XRAMMAT  Initial Release
	*/
/*=================================================================== */
#ifndef ACS_EMF_TRA_H_
#define ACS_EMF_TRA_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Mutex.h>
#include <string>
using namespace std;
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
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



#ifdef EMFDEBUG
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

#else  // EMFDEBUG

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

/*=================================================================== */
/**
   @brief           INFO
*/
/*=================================================================== */

#define INFO(IS_LOG,FMT, ...) \
	ACS_EMF_Tra::helperTraceInformation(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           DEBUG
*/
/*=================================================================== */

#define DEBUG(IS_LOG,FMT, ...) \
	   ACS_EMF_Tra::helperTraceDebug(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           WARNING
*/
/*=================================================================== */

#define WARNING(IS_LOG,FMT, ...) \
	    ACS_EMF_Tra::helperTraceWarning(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           ERROR
*/
/*=================================================================== */

#define ERROR(IS_LOG,FMT, ...) \
	     ACS_EMF_Tra::helperTraceError(IS_LOG,FMT,__VA_ARGS__);

/*=================================================================== */
/**
   @brief           FATAL
*/
/*=================================================================== */

#define FATAL(IS_LOG,FMT, ...) \
         ACS_EMF_Tra::helperTraceFatal(IS_LOG,FMT,__VA_ARGS__);

#endif

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
class ACS_EMF_Tra
{
public:
/*=================================================================== */
/*=================================================================== */
     /**
        @brief		 helperTraceInformation

        @par         Deprecated
                     never

        @pre         none

        @post        none

        @param       isLog

        @param       messageFormat

		@return      none

        @exception   none
      */
  /*=================================================================== */
static void helperTraceInformation(int isLog ,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceDebug

        @par         Deprecated
                     never

        @pre         none

        @post        none

        @param       isLog

        @param       messageFormat

		@return      none

        @exception   none
      */
  /*=================================================================== */
static void helperTraceDebug(int isLog ,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceWarning

        @par         Deprecated
                     never

        @pre         none

        @post        none

        @param       isLog

        @param       messageFormat

		@return      none

        @exception   none
      */
  /*=================================================================== */
static void helperTraceWarning(int isLog,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief       helperTraceError

        @par         Deprecated
                     never

        @pre         none

        @post        none

        @param       isLog

        @param       messageFormat

		@return      none

        @exception   none
      */
  /*=================================================================== */
static void helperTraceError(int isLog,const char* messageFormat, ...);

/*=================================================================== */
     /**
        @brief      helperTraceFatal

        @par         Deprecated
                     never

        @pre         none

        @post        none

        @param       isLog

        @param       messageFormat

		@return      none

        @exception   none
      */
  /*=================================================================== */
static void helperTraceFatal(int isLog,const char* messageFormat, ...);


/*=================================================================== */
     /**
        @brief      ACS_EMF_InformationTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_EMF_InformationTrace;

/*=================================================================== */
     /**
        @brief      ACS_EMF_WarningTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_EMF_WarningTrace;

/*=================================================================== */
     /**
        @brief      ACS_EMF_ErrorTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_EMF_ErrorTrace;

/*=================================================================== */
     /**
        @brief      ACS_EMF_DebugTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_EMF_DebugTrace;

/*=================================================================== */
     /**
        @brief      ACS_EMF_FatalTrace

      */
  /*=================================================================== */
static ACS_TRA_trace ACS_EMF_FatalTrace;

/*=================================================================== */
     /**
        @brief      ACS_EMF_Logging

      */
  /*=================================================================== */
static ACS_TRA_Logging ACS_EMF_Logging;
static ACE_Mutex log_mutex;
};

#endif /* ACS_CHB_TRA_H_ */
