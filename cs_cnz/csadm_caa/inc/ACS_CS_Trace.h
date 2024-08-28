
//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_Trace_h
#define ACS_CS_Trace_h 1


#include <string>
#include <stdio.h>
#include <iostream>

#include "ace/Mutex.h"
#include <ace/Singleton.h>

using std::string;


#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "ACS_CS_Registry.h"


class ACS_CS_Trace 
{

  public:
      void trace (ACS_TRA_trace &tp, const char *format, ... );
      void ftrace (ACS_TRA_trace &tp, ACS_TRA_LogLevel logLevel, const char *format, ... );
      std::string dtrace (const char *format, ... );

      static  ACS_CS_Trace * instance ();

//      void setBlock (const char *str);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_Trace();

      virtual ~ACS_CS_Trace();

      ACS_CS_Trace(const ACS_CS_Trace &right);

      ACS_CS_Trace & operator=(const ACS_CS_Trace &right);


      std::string getString (const char *format, va_list argp);

      ACE_Mutex traceMutex_;

      ACS_TRA_Logging * logFile;

      typedef ACE_Singleton<ACS_CS_Trace, ACE_Recursive_Thread_Mutex> instance_;
      friend class ACE_Singleton<ACS_CS_Trace, ACE_Recursive_Thread_Mutex>;
};


#define ACS_CS_Trace_TDEF(X) ACS_TRA_trace X = ACS_TRA_trace(const_cast<char*>(#X), const_cast<char*>("C"))

#define ACS_CS_TRACE(X) \
    do \
    { \
    ACS_CS_Trace *log__ = ACS_CS_Trace::instance(); \
    log__->trace X; \
    } while(0);

#define ACS_CS_FTRACE(X) \
    do \
    { \
    ACS_CS_Trace *log__ = ACS_CS_Trace::instance(); \
    log__->ftrace X; \
    } while(0);

#define ACS_CS_DEBUG(X) \
    do \
    { \
    	if(ACS_CS_Registry::isDebug()) \
    	{\
    		ACS_CS_Trace *log__ = ACS_CS_Trace::instance(); \
    		std::cout << "DBG: "<< __FILE__ << "::" << __FUNCTION__ << "()@" << __LINE__<< " - " << log__->dtrace X << std::endl; \
    	}\
    } while(0);


// Class ACS_CS_Trace 



#endif
