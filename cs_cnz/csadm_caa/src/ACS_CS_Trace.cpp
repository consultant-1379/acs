

//	Copyright Ericsson AB 2007. All rights reserved.



#include <syslog.h>
#include <sstream>
#include <stdarg.h>
#include <stdlib.h>

#include "ace/Thread.h"
#include "ace/Process.h"

// ACS_CS_Trace
#include "ACS_CS_Trace.h"

#include "ACS_CS_Registry.h"

using std::cerr;
using std::stringstream;

typedef int BOOL;



ACS_CS_Trace::ACS_CS_Trace()
{
	ACS_TRA_LogResult result = TRA_LOG_ERROR;
	logFile = new ACS_TRA_Logging();

	if(logFile)
		result = logFile->Open("CS");

	if(result != TRA_LOG_OK){
		if(logFile){
			delete logFile;
		}
		logFile = 0;
		syslog(LOG_ERR, "Failed to open CS log file. Will not log to file...");
	}
}


ACS_CS_Trace::~ACS_CS_Trace()
{
	traceMutex_.remove();

	if(logFile){
		logFile->Close();
		delete logFile;
	}
}

 void ACS_CS_Trace::trace (ACS_TRA_trace &tp, const char *format, ... )
{

	 	 if (!tp.ACS_TRA_ON() && !ACS_CS_Registry::isDebug())
	 		 return;

		 traceMutex_.acquire();

		 va_list argp;
		 va_start(argp, format);

		 string out = getString(format, argp);
		 string::iterator it = out.end();
		 --it;
		 if (*it == '\n')
			 out.erase(it);

		 if (tp.ACS_TRA_ON())
		 {
			 tp.ACS_TRA_event(1, out.c_str());
		 }

		 va_end(argp);

		 if(ACS_CS_Registry::isDebug())
	     {
			 // Get the block name, substring it from the ACS_CS_TRACE string
//			 string block_name = block;
//			 block_name = block_name.substr(1, block_name.find_first_of(",")-1);
	         //std::cout << "(" << block_name << ") - " << out << std::endl;
	         std::cout << "(" << tp.getRecordName() << ") - " << out << std::endl;
	     }

		 traceMutex_.release();

}

void ACS_CS_Trace::ftrace (ACS_TRA_trace &tp, ACS_TRA_LogLevel logLevel, const char *format, ... )
{
	traceMutex_.acquire();

	va_list argp;
	va_start(argp, format);

	string out = getString(format, argp);
	string::iterator it = out.end();
	--it;

	if (*it == '\n')
		out.erase(it);

	if(logFile)
		logFile->Write(out.c_str(), logLevel);

	va_end(argp);

	traceMutex_.release();

	ACS_CS_TRACE((tp, out.c_str()));

	//DO NOT WRITE ON SYSTEM LOG
//	if(logLevel == LOG_LEVEL_WARN || logLevel ==  LOG_LEVEL_ERROR || logLevel ==  LOG_LEVEL_FATAL){
//
//		syslog(LOG_ERR, out.c_str());
//	}
}

std::string ACS_CS_Trace::dtrace (const char *format, ... )
{
	va_list argp;
	va_start(argp, format);

	string out = getString(format, argp);
	string::iterator it = out.end();
	--it;

	if (*it == '\n')
		out.erase(it);

	va_end(argp);

	return out;
}

ACS_CS_Trace * ACS_CS_Trace::instance ()
{
	return instance_::instance();
}

 std::string ACS_CS_Trace::getString (const char *format, va_list argp)
{
   stringstream out;

    // Walk through the format string and create a output string
    while (*format != 0)
    {
        // If it is normal text, just add it
        if (*format != '%')
            out << *format++;

        // Check for a escaped precent-sign
        else if (format[1] == '%')
        {
            out << *format++;
            format++;
        }

        // It is some kind of control character to expand
        else
        {
            format++;
            switch (*format++)
            {
            case 'B':       // Bool as strings true or false
                out << (va_arg(argp, BOOL) == 1 ? "true" : "false");
                break;

            case 'b':       // Bool as strings true or false
                out << (va_arg(argp, int) == 1 ? "true" : "false");
                break;

            case 'i':
            case 'd':       // Decimal
                out << va_arg(argp, int);
                break;

            case 'p':       // Process ID (PID)
                out << getpid();
                break;

            case 's':       // char * string
                out << va_arg(argp, char *);
                break;

            case 't':       // Thread ID (TID)
                out << ACE_OS::thr_self();
                break;

            case 'u':       // Unsigned decimal
                out << va_arg(argp, unsigned int);
                break;

            default:
                // ?????
                break;
            }
        }
    }
    return out.str();
}

//void ACS_CS_Trace::setBlock (const char *str)
//{
//	block = const_cast<char*>(str);
//}
