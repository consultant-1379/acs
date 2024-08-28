//!@todo Recheck changes

//=============================================================================
/**
 *  @file    ACS_LCT_TRA.h
 *
 *  @copyright  Ericsson AB, Sweden 2010. All rights reserved.
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Utvecklings AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Utvecklings AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been supplied.
 *
 *  @brief Common class having common functionalities is present in this file
 *
 *  @author 2010-07-7 by XSIRKUM
 *
 *  @documentno
 *
 */

#ifndef ACS_LCT_TRA_H
#define ACS_LCT_TRA_H

#include <ace/ACE.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <sys/stat.h>

//! Common Class include for Getting Active Node
#include "ACS_APGCC_Util.H"

#include <stdio.h>
#include <sstream>

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "ACS_APGCC_CommonLib.h"
#include "acs_prc_api.h"

//! ACE Trace Macros
#define DEBUG_PREFIX "\n DEBUG%I"
#define INFO_PREFIX "\n INFO%I"
#define NOTICE_PREFIX "\n NOTICE%I"
#define WARNING_PREFIX "\n WARNING%I"
#define ERROR_PREFIX "\n ERROR%I"
#define CRITICAL_PREFIX "\n CRITICAL%I"
#define ALERT_PREFIX "\n ALERT%I"
#define EMERGENCY_PREFIX "\n EMERGENCY%I"

#ifdef LCTDEBUG
#define DEBUG(FMT, ...) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX FMT ,\
__VA_ARGS__))


#define WARNING(FMT, ...) \
	ACS_LCT_TRA::helperTraceWarning(FMT,__VA_ARGS__);
#define TRA_ERROR(FMT, ...) \
	ACS_LCT_TRA::helperTraceWarning(FMT,__VA_ARGS__);

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

#else  // LCTDEBUG

#define INFO(FMT, ...) \
	ACS_LCT_TRA::helperTraceInformation(FMT,__VA_ARGS__);

#define DEBUG(FMT, ...) \
	ACS_LCT_TRA::helperTraceDebug(FMT,__VA_ARGS__);
#define WARNING(FMT, ...) \
	ACS_LCT_TRA::helperTraceWarning(FMT,__VA_ARGS__);

#define ERROR(FMT, ...) \
	ACS_LCT_TRA::helperTraceError(FMT,__VA_ARGS__);

#define FATAL(FMT, ...) \
	ACS_LCT_TRA::helperTraceFatal(FMT,__VA_ARGS__);

#define NOTICE(FMT, ...) \
	ACS_LCT_TRA::helperTraceDebug(FMT,__VA_ARGS__);

#endif

/*!
 * @class ACS_LCT_TRA
 *
 * @brief It contains all the common functions
 *
 */

class ACS_LCT_TRA
{
public:
	static ACS_TRA_trace *ACS_LCT_InformationTrace;
	static ACS_TRA_trace *ACS_LCT_DebugTrace;
	static ACS_TRA_trace *ACS_LCT_WarningTrace;
	static ACS_TRA_trace *ACS_LCT_ErrorTrace;
	static ACS_TRA_trace *ACS_LCT_FatalTrace;
	static ACS_TRA_Logging *ACS_LCT_Logging;
	//static bool theIsActiveNode;
protected:
	/**
	* @brief
	* Protected default constructor
	* Not allowed to instantiate an object of this class
	* @return void
	*
	*/
   ACS_LCT_TRA() { };

public:
   /**
    * @brief
    * static function used to pass the TRA information trace.
    * No arguments
    * @return void
    *
    */

   static void helperTraceInformation(const ACE_TCHAR* messageFormat, ...)
   {
     if( ACS_LCT_InformationTrace==0 )
     return;
     const ACE_UINT32 TRACE_BUF_SIZE = 1024;
     ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
     if ( messageFormat && *messageFormat )
        {
     	   va_list params;
      	   va_start(params, messageFormat);
      	   ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
      	   traceBuffer[TRACE_BUF_SIZE-1]='\0';
      	   //! If Trace if ON .Send trace to TRA
       	   if( ACS_LCT_InformationTrace->ACS_TRA_ON() )
      	   {
              ACS_LCT_InformationTrace->ACS_TRA_event(1,traceBuffer);
      	   }

       	   ACS_LCT_Logging->Write(traceBuffer,LOG_LEVEL_INFO);
       	   va_end(params);
           }
   }

   /**
       * @brief
       * static function used to pass the TRA debug trace.
       * No arguments
       * @return void
       *
       */
   static void helperTraceDebug(const ACE_TCHAR* messageFormat, ...)
      {
       if( ACS_LCT_DebugTrace==0 )
         	   return;
       const ACE_UINT32 TRACE_BUF_SIZE = 1024;
       ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
       if ( messageFormat && *messageFormat )
          {
         	   va_list params;
          	   va_start(params, messageFormat);
          	   ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
          	   traceBuffer[TRACE_BUF_SIZE-1]='\0';
           	   //! If Trace if ON .Send trace to TRA
          	   if( ACS_LCT_DebugTrace->ACS_TRA_ON() )
          	   {
                    ACS_LCT_DebugTrace->ACS_TRA_event(1,traceBuffer);
          	   }

          	   ACS_LCT_Logging->Write(traceBuffer,LOG_LEVEL_DEBUG);
           	   va_end(params);
           }
      }

   /*===================================================================
     	   ROUTINE: helperTraceWarning
  	=================================================================== */
   /**
       * @brief
       * static function used to pass the TRA warning trace.
       * No arguments
       * @return void
       *
       */
     	static void helperTraceWarning(const ACE_TCHAR* messageFormat, ...)
     	{
          if( ACS_LCT_WarningTrace==0 )
        	   return;

          static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
          ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
          if ( messageFormat && *messageFormat )
             {

        	  va_list params;
        	  va_start(params, messageFormat);

        	  ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
        	  //! If Trace if ON .Send trace to TRA

        	  if( ACS_LCT_WarningTrace->ACS_TRA_ON() )
        	  {
        		  ACS_LCT_WarningTrace->ACS_TRA_event(1,traceBuffer);
        	  }
        	  ACS_LCT_Logging->Write(traceBuffer,LOG_LEVEL_WARN);
        	  va_end(params);
             }
     	}

    /**
    * @brief
    * static function used to pass the TRA Error trace.
    * No arguments
    * @return void
    *
    */
     	static void helperTraceError(const ACE_TCHAR* messageFormat, ...)
     	{
           if( ACS_LCT_ErrorTrace ==0 )
         	   return;

           static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
           ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
           if ( messageFormat && *messageFormat )
            {
            	va_list params;
            	va_start(params, messageFormat);

            	ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

            	//! If Trace if ON .Send trace to TRA

            	if( ACS_LCT_ErrorTrace->ACS_TRA_ON() )
            	{
            		ACS_LCT_ErrorTrace->ACS_TRA_event(1,traceBuffer);
            	}
            	 ACS_LCT_Logging->Write(traceBuffer,LOG_LEVEL_ERROR);

            	va_end(params);
            }
     	}
    /**
    * @brief
    * static function used to pass the TRA FATAL trace.
    * No arguments
    * @return void
    *
    */
     	static void helperTraceFatal(const ACE_TCHAR* messageFormat, ...)
     	   {
     		if( ACS_LCT_FatalTrace==0 )
     			return;
     		const ACE_UINT32 TRACE_BUF_SIZE = 1024;
     		ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
     		if ( messageFormat && *messageFormat )
     		{
     			va_list params;
     			va_start(params, messageFormat);
     			ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
     			traceBuffer[TRACE_BUF_SIZE-1]='\0';

     			//! If Trace if ON .Send trace to TRA
     			if( ACS_LCT_FatalTrace->ACS_TRA_ON() )
     			{
     				ACS_LCT_FatalTrace->ACS_TRA_event(1,traceBuffer);
     			}

     			ACS_LCT_Logging->Write(traceBuffer,LOG_LEVEL_FATAL);
     			va_end(params);
     		}
     	   }
};

#endif

