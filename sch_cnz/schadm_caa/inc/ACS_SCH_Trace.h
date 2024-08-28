#ifndef ACS_SCH_Trace_h
#define ACS_SCH_Trace_h 1


#include <string>
#include <stdio.h>
#include <iostream>

#include "ace/Mutex.h"
#include <ace/Singleton.h>

using std::string;


#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "ACS_SCH_Registry.h"


class ACS_SCH_Trace
{

  public:
      void trace (ACS_TRA_trace &tp, const char *format, ... );
      void ftrace (ACS_TRA_trace &tp, ACS_TRA_LogLevel logLevel, const char *format, ... );
      std::string dtrace (const char *format, ... );

      static  ACS_SCH_Trace * instance ();

//      void setBlock (const char *str);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_SCH_Trace();

      virtual ~ACS_SCH_Trace();
ACS_SCH_Trace(const ACS_SCH_Trace &right);

      ACS_SCH_Trace & operator=(const ACS_SCH_Trace &right);


      std::string getString (const char *format, va_list argp);

      ACE_Mutex traceMutex_;

      ACS_TRA_Logging * logFile;

      typedef ACE_Singleton<ACS_SCH_Trace, ACE_Recursive_Thread_Mutex> instance_;
      friend class ACE_Singleton<ACS_SCH_Trace, ACE_Recursive_Thread_Mutex>;
};


#define ACS_SCH_Trace_TDEF(X) ACS_TRA_trace X = ACS_TRA_trace(const_cast<char*>(#X), const_cast<char*>("C"))

#define ACS_SCH_TRACE(X) \
    do \
    { \
    ACS_SCH_Trace *log__ = ACS_SCH_Trace::instance(); \
    log__->trace X; \
    } while(0);

#define ACS_SCH_FTRACE(X) \
    do \
    { \
    ACS_SCH_Trace *log__ = ACS_SCH_Trace::instance(); \
    log__->ftrace X; \
    } while(0);
#define ACS_SCH_DEBUG(X) \
    do \
    { \
      if(ACS_SCH_Registry::isDebug()) \
      {\
        ACS_SCH_Trace *log__ = ACS_SCH_Trace::instance(); \
        std::cout << "DBG: "<< __FILE__ << "::" << __FUNCTION__ << "()@" << __LINE__<< " - " << log__->dtrace X << std::endl; \
      }\
    } while(0);


// Class ACS_SCH_Trace



#endif

