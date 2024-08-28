/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2011
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file ACS_USA_Trace.cpp
 *
 * @brief
 * See header file
 *
 * @details
 * See header file
 *  *
 * @author XTBAKLU
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 *
 ****************************************************************************/

#include <sstream>
#include <iostream>

#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include "acs_usa_tratrace.h"
#include <ace/Synch.h>
#include <sys/stat.h>

using namespace std;


// Name of the log file produced if environment variable DEBUG_USA is defined to 1, "export DEBUG_USA=1"
const char *LOG_FILE_TRACE = "/tmp/usa_trace.log";
const char *TRACE_CLASS ="USA";

//----------------------------------------------------------------------------------------
string formatString(const char *format, va_list argp) {

   stringstream out;

   // Walk through the format string and create an output string
   while (*format != 0) {
      // If it is normal text, just add it
      if (*format != '%') {
         out << *format++;
         continue;
      }

      // Check for a escaped precent-sign
      else if (format[1] == '%') {
         out << *format++;
         format++;
      }

      // It is some kind of control chparameteraracter to expand
      else {
         format++;
         switch (*format++) {
            case 'B': // Bool as strings true or false
               out << (va_arg(argp, int) == 1 ? "true" : "false");
               break;

            case 'b': // Bool as strings true or false
               out << (va_arg(argp, int) == 1 ? "true" : "false");
               break;

            case 'i':
            case 'd': // Decimal
               out << va_arg(argp, int);
               break;

            case 'p': // Process ID (PID)
               out << getpid();
               break;

            case 's': // char * string
               out << va_arg(argp, char *);
               break;

            case 't': // Thread ID (TID)
               out << ACE_OS::thr_self();
               break;

            case 'u': // Unsigned decimal
               out << va_arg(argp, unsigned int);
               break;

            case 'x':
            case 'X':
               out << std::hex << va_arg(argp, unsigned int);
               out << std::dec;
               break;

            default:
               // ?????
               break;
         }
      }
   }
   return out.str();
}



//----------------------------------------------------------------------------------------
void usa_trace(ACS_USA_Trace& tp, const char* format, ...) {
   bool traceCalled = false;
   string out2      = "";

   if (tp.isOn()) {
      va_list argp;
      va_start(argp, format);

      try {
         string out = formatString(format, argp);
         out2 = out;
         // Write to TRA
         tp.write(out);
         traceCalled = true;
      }
      catch(...) {
         // No action
      }

      va_end(argp);
   }

   if (tp.isDebug()) {
      //ACE_GUARD(ACE_Thread_Mutex, guard, g_mutex); // Guard this file write
      if (traceCalled) {
         tp.writeToFile(out2);  // Reuse the previous formatting
      } else {
         // We have to do the formatting first
         va_list argp;
         va_start(argp, format);
         try {
            string out = formatString(format, argp);
            tp.writeToFile(out);
         }
         catch (...) {
            // No action
         }
         va_end(argp);
      }
   }
}


//----------------------------------------------------------------------------------------
// This method is used by adh to enable/disable using the cpdtest command
// This feature has nmot been implemented yet since the cpdtest command will probably not be implemented
//----------------------------------------------------------------------------------------
void adh_do_trace(bool /* onoff */) {
//   __g_trace_active = onoff;
}

//----------------------------------------------------------------------------------------
ACS_USA_Trace::ACS_USA_Trace(const string className):
   m_className(className),
   m_traObject(0) {
   ACE_NEW_NORETURN(m_traObject, ACS_TRA_trace(m_className, "C"));  // Will set m_traObject to 0 if it fails
   this->loadDebug();
}

//----------------------------------------------------------------------------------------
ACS_USA_Trace::~ACS_USA_Trace() {
   if (m_traObject != 0) {
      delete m_traObject;
      m_traObject = 0;
   }
}


//----------------------------------------------------------------------------------------
bool ACS_USA_Trace::isOn() {
	if (m_traObject != 0) {
		return m_traObject->isOn();
	}
	return false;
}


//----------------------------------------------------------------------------------------
void ACS_USA_Trace::write(const string & s){
	if (m_traObject != 0) {
#ifndef SHOW_THREAD_ID
	   char buf[10000] = {0};
	   unsigned short threadid = pthread_self(); // Use only lower part of the thread id, it is most likely unique!

	   sprintf(buf, "(%06d) %s", threadid, s.c_str());
      m_traObject->trace(buf); // Send to TRA
#else
		m_traObject->trace(s); // Send to TRA
#endif
	}
}

//----------------------------------------------------------------------------------------
void ACS_USA_Trace::traceFmt(const char* format, ...) {
   if (m_traObject != 0) {
      try {
         // Write to TRA
         if (m_traObject->isOn()) {
            va_list argp;
            va_start(argp, format);
            string s = formatString(format, argp);
            m_traObject->trace(s); // Send to TRA
            va_end(argp);
         }
      }
      catch(...) {
         // Do nothing
      }

   }
}


//----------------------------------------------------------------------------------------
void ACS_USA_Trace::traceStr(const string &traceStr){
   if (m_traObject != 0) {
      m_traObject->trace(traceStr); // Send to TRA
   }
}

//----------------------------------------------------------------------------------------
void ACS_USA_Trace::traceAndLogFmt(USA_LOGLEVEL logLevel, const char* format, ...) {
   if (m_traObject != 0) {
      va_list argp;
      va_start(argp, format);

      try {
         ACS_TRA_Logging* traLogging = 0;

         ACE_NEW_NORETURN(traLogging, ACS_TRA_Logging());
         if (traLogging == 0) {
            // For some reason traLogging sometimes is 0 causing ugly crashes
            return;
         }

         traLogging->Open(TRACE_CLASS);

         string s = formatString(format, argp);

         ACS_TRA_LogLevel traLogLevel =  LOG_LEVEL_TRACE;

         switch (logLevel) {
            case TRACE:
               traLogLevel = LOG_LEVEL_TRACE;
               break;
            case DEBUG:
               traLogLevel = LOG_LEVEL_DEBUG;
               break;
            case INFO:
               traLogLevel = LOG_LEVEL_INFO;
               break;
            case WARNING:
               traLogLevel = LOG_LEVEL_WARN;
               s = "WARNING: " + s;
               break;
            case ERROR:
               traLogLevel = LOG_LEVEL_ERROR;
               s = "ERROR: " + s;
               break;
            case FATAL:
               s = "FATAL: " + s;
               traLogLevel = LOG_LEVEL_FATAL;
               break;
            default:
               traLogLevel = LOG_LEVEL_TRACE;
               break;
         }

         traLogging->Write(s.c_str(), traLogLevel);

         delete traLogging;

         if (m_traObject->isOn()) {
            // Write to TRA as well
            this->write(s);
//            m_traObject->trace(s); // TODO REMOVE
         }
      }
      catch(...) {
         // Do nothing
      }

      va_end(argp);
   }
}

//----------------------------------------------------------------------------------------
void ACS_USA_Trace::traceAndLogStr(USA_LOGLEVEL logLevel, const string &traceStr){
   if (m_traObject != 0) {

      try {
         ACS_TRA_Logging* traLogging = 0;

         ACE_NEW_NORETURN(traLogging, ACS_TRA_Logging());
         if (traLogging == 0) {
            // For some reason traLogging sometimes is 0 causing ugly crashes
            return;
         }

         string s = traceStr;

         traLogging->Open(TRACE_CLASS);

         ACS_TRA_LogLevel traLogLevel =  LOG_LEVEL_TRACE;

         switch (logLevel) {
            case TRACE:
               traLogLevel = LOG_LEVEL_TRACE;
               break;
            case DEBUG:
               traLogLevel = LOG_LEVEL_DEBUG;
               break;
            case INFO:
               traLogLevel = LOG_LEVEL_INFO;
               break;
            case WARNING:
               traLogLevel = LOG_LEVEL_WARN;
               s = "WARNING: " + s;
               break;
            case ERROR:
               traLogLevel = LOG_LEVEL_ERROR;
               s = "ERROR: " + s;
               break;
            case FATAL:
               s = "FATAL: " + s;
               traLogLevel = LOG_LEVEL_FATAL;
               break;
            default:
               traLogLevel = LOG_LEVEL_TRACE;
               break;
         }

         traLogging->Write(s.c_str(), traLogLevel);

         delete traLogging;


         if (m_traObject->isOn()) {
            // Write to TRA as well
            m_traObject->trace(s);
         }
      }
      catch(...) {
         // Do nothing
      }

   }
}


//----------------------------------------------------------------------------------------
void ACS_USA_Trace::logFmt(USA_LOGLEVEL logLevel, const char* format, ...) {
   va_list argp;
   va_start(argp, format);

   try {
      ACS_TRA_Logging* traLogging = 0;

      ACE_NEW_NORETURN(traLogging, ACS_TRA_Logging());
      if (traLogging == 0) {
         // For some reason traLogging sometimes is 0 causing ugly crashes
         return;
      }

      traLogging->Open(TRACE_CLASS);

      string s = formatString(format, argp);

      ACS_TRA_LogLevel traLogLevel = LOG_LEVEL_TRACE;

      switch (logLevel) {
         case TRACE:
            traLogLevel = LOG_LEVEL_TRACE;
            break;
         case DEBUG:
            traLogLevel = LOG_LEVEL_DEBUG;
            break;
         case INFO:
            traLogLevel = LOG_LEVEL_INFO;
            break;
         case WARNING:
            traLogLevel = LOG_LEVEL_WARN;
            s = "WARNING: " + s;
            break;
         case ERROR:
            traLogLevel = LOG_LEVEL_ERROR;
            s = "ERROR: " + s;
            break;
         case FATAL:
            s = "FATAL: " + s;
            traLogLevel = LOG_LEVEL_FATAL;
            break;
         default:
            traLogLevel = LOG_LEVEL_TRACE;
            break;
      }

      traLogging->Write(s.c_str(), traLogLevel);
      traLogging->Close();

      delete traLogging;

   }
   catch(...) {
      // Do nothing
   }

   va_end(argp);
}





//----------------------------------------------------------------------------------------
bool ACS_USA_Trace::isDebug() const {

	return m_debug;
}


//----------------------------------------------------------------------------------------
void ACS_USA_Trace::loadDebug() {

   m_debug = false;

   char * envVar = getenv("DEBUG_MCS");

   if (envVar != 0 && !strcmp(envVar, "1")) {
	   m_debug = true;
   }
   else {
      struct stat st;
      if (stat("/tmp/DEBUG_MCS", &st) == 0) {
    	  m_debug = true;
      }
   }

}


//----------------------------------------------------------------------------------------
void ACS_USA_Trace::writeToFile(string str) {

   FILE *logfile;
   char s[50];
   logfile = fopen(LOG_FILE_TRACE, "a");

   if (!logfile) {
      return;
   }

   // Get time
   struct timeval tv;
   struct timezone tz;
   struct tm *tm;
   gettimeofday(&tv, &tz);
   tm = localtime(&tv.tv_sec);
   int usec = tv.tv_usec;

   unsigned short threadid = pthread_self(); // Use only lower part of the thread id, it is most likely unique!

   sprintf(s, "%d:%02d:%02d %d (%06d)", tm->tm_hour, tm->tm_min, tm->tm_sec, usec, threadid);

   fprintf(logfile, "%s [%s] %s\n", s, m_className.c_str(), str.c_str());
   fclose(logfile);
}

