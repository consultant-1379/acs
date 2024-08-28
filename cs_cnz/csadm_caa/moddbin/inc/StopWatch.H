//******************************************************************************
// 
// .NAME
//    StopWatch - Provides a stopwatch implementation.
// .LIBRARY 3C++
// .PAGENAME <CLASSNAME>
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2005.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  The stop watch accurately measures time from start to stop operations.
//  Countless stop operation may follow on a single start operation. Each new
//  stop operation will present the elapsed time from the initial start order
//  in units of microseconds.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-18 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef STOPWATCH_H 
#define STOPWATCH_H

#include <sys/timeb.h>
#include <time.h>

//Member functions, constructors, destructors, operators
class StopWatch
{
public:
   StopWatch();
   
   // Start the StopWatch to set the starting point for the measurement.
   void start();

   // Stop the StopWatch to calculate the time from the starting point. Stop
   // may be called repeatedly to measure the time from the starting point.
   void stop();

   // Returns the elapsed time in microsecond resolution. Returns a valid
   // value only if a start call has been preceeded by a stop call.
   __int64 timeInMicroSec() const;

   enum
   {
      MICROS_PER_SEC = 1000000L,
      MILLIS_PER_SEC = 1000L
   };

private:
   void elapsedTime();

   __int64 m_time;

   // Records when the timer started.
   struct _timeb m_timerStart;

   // Records when the timer stopped.
   struct _timeb m_timerStop;
};

//******************************************************************************
// Desription:
//    This method returns the elapsed time in microseconds.
//
// Return value:
//    The elapsed time in microseconds between a call to start and stop. If
//    the timer has not been started or the timer has been started but not
//    stopped, then 0 is returned.
//******************************************************************************
inline __int64 StopWatch::timeInMicroSec() const
{
   return m_time;
}

//******************************************************************************
// Desription:
//    This method starts the timer.
//******************************************************************************
inline void StopWatch::start()
{
   _ftime(&m_timerStart);
}

//******************************************************************************
// Desription:
//    This method stops the timer. The stop method may be called numerous times
//    after the start method has been called. The timeInMicroSec method will
//    always return the difference from the last point in time stop has been
//    called compared to the last point in time that start has been called.
//******************************************************************************
inline void StopWatch::stop()
{
   _ftime(&m_timerStop);

   elapsedTime();
}

//******************************************************************************
// Desription:
//    This method calculates the time between calls to the start and stop
//    methods.
//******************************************************************************
inline void StopWatch::elapsedTime()
{
   __int64 elapsed_us = ((m_timerStop.time - m_timerStart.time) * MICROS_PER_SEC) +
                         ((m_timerStop.millitm - m_timerStart.millitm) * MILLIS_PER_SEC);

   m_time  = elapsed_us;
}

//******************************************************************************
// Desription:
//    The constructor initializes the member data.
//******************************************************************************
inline StopWatch::StopWatch() :
   m_time(0)
{
   m_timerStart.time    = 0;
   m_timerStart.millitm = 0;
   m_timerStop.time     = 0;
   m_timerStop.millitm  = 0;
}

#endif
