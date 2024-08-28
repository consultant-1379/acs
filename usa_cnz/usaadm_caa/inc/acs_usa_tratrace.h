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
 * @file acs_usa_trace.h
 *
 * @brief
 * Encapsulates the ACS TRA Class used for tracing and logging.
 * The main feature is the mcc_trace() function that will reduce the amount
 * of code needed to do add tracing.
 *
 * An extra feature when using this trace class is that the trace can be directed
 * to a file using environment variable DEBUG_USA=1
 * If so the file /tmp/usa_trace.log is created
 *
 *
 * @details
 * The tracing is based on an sprintf style which is not C++ and not the
 * best solution but it outranks the interface provided by TRA.
 * TRA does not provide a streaming interface and the design base heavily
 * used this usa_trace() function.
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 *
 ****************************************************************************/

#ifndef ACS_USA_TRACE_H_
#define ACS_USA_TRACE_H_

#include <string> 
#include <stdio.h>
#include <iostream>
#include <ace/Mutex.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>

using std::string;

enum USA_LOGLEVEL {
   TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
};


//----------------------------------------------------------------------------------------
// Set g_trace_active true or false
//----------------------------------------------------------------------------------------
void usa_do_trace(bool onoff);

//----------------------------------------------------------------------------------------
class ACS_USA_Trace {
public:
   ACS_USA_Trace(const string className);
   ~ACS_USA_Trace();

   bool isOn();
   bool isDebug() const;

   void write(const string & s);  // TODO To be replaced by traceStr()
   void writeToFile(string str); // TODO To be replaced by traceAndLog()

   // Just tracing
   void traceFmt(const char* format, ...);
   void traceStr(const string &traceStr);

   // Tracing and logging
   //----------------------------------------------------------------------------------------
   // Use these to write to log4cplus file at the same time
   //----------------------------------------------------------------------------------------
   void traceAndLogStr(USA_LOGLEVEL logLevel, const string &traceStr);
   void traceAndLogFmt(USA_LOGLEVEL logLevel, const char* format, ...);
   static void logFmt(USA_LOGLEVEL logLevel, const char* format, ...);

private:
   void loadDebug();
   string m_className;
   ACS_TRA_trace *m_traObject;
   bool m_debug;
};

//----------------------------------------------------------------------------------------
// Use these to write to trautil traces
//----------------------------------------------------------------------------------------
void usa_trace(ACS_USA_Trace& tp, const char* format, ...);


#endif /* ACS_USA_TRACE_H_ */

