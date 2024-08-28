//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 2003                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
//  **************************************************************************
//
//  File Name ........@(#)fn 
//  Document Number ..@(#)dn 
//  Revision Date ....@(#)rd 
//  Current Version ..@(#)cv 
//  Responsible ............ UAB/UKY/SD Peter Johansson (Contactor Data AB)
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//>

#ifndef PERFORMANCEMONITOR_HXX
#define PERFORMANCEMONITOR_HXX

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/timeb.h>
#include <winbase.h>
#include <winsock2.h>
#include <windows.h>

class Watch
{
public:
   // Constructor.
   Watch();
   
   // Start the Watch to set the starting point for the measurement.
   void start();

   // Stop the Watch to calculate the time from the starting point. Stop
   // may be called repeatedly to measure the time from the starting point.
   void stop();

   // Returns the elapsed time in microsecond resolution. Returns a valid
   // value only if a start call has been preceeded by a stop call.
   __int64 timeInMicroSec() const;

private:
   void elapsedTime();

   __int64 m_time;
   struct _timeb m_timerStart;
   struct _timeb m_timerStop;
};

// Constructor
inline Watch::Watch() :
   m_time(0)
{
   m_timerStart.time    = 0;
   m_timerStart.millitm = 0;
   m_timerStop.time     = 0;
   m_timerStop.millitm  = 0;
}

// Returns elapsed time in units of us.
inline __int64 Watch::timeInMicroSec() const
{
   return m_time;
}

// Start the timer.
inline void Watch::start()
{
   _ftime(&m_timerStart);
}

// Stop the timer and calculate the elapsed time.
inline void Watch::stop()
{
   _ftime(&m_timerStop);

   elapsedTime();
}

// Calculates the elapsed time between a start and stop operation.
inline void Watch::elapsedTime()
{
   m_time = ((m_timerStop.time - m_timerStart.time) * 1000000L) +
            ((m_timerStop.millitm - m_timerStart.millitm) * 1000L);
}

class PerformanceMonitor
{
public:
   // Constructor.
   PerformanceMonitor();

   // Starts monitoring.
   void start();

   // Ends monitoring.
   void stop();

   // Returns true if the monitored thread was suspended from execution for
   // more than 'diff' ms.
   bool wasThreadSuspended(__int64 diff = 0) const;

   // Returns the wall clock time that elapsed when monitoring.
   __int64 wallClockTimeInMilliSec() const;

   // Returns the thread's user time in units of ms.
   __int64 userTimeInMilliSec() const;

   // Returns the thread's kernel time in units of ms.
   __int64 kernelTimeInMilliSec() const;

private:
   // Calculates the elapsed time between start and stop operations.
   void elapsedTime();

   // The timer.
   Watch m_watch;

   // The thread's start time for the monitoring process.
   LARGE_INTEGER m_userTimeStart;

   // The thread's end time for the monitoring process.
   LARGE_INTEGER m_userTimeStop;

   // The thread's user time.
   __int64 m_userTime;

   // The thread's start kernel time.
   LARGE_INTEGER m_kernelTimeStart;

   // The thread's stop kernel time.
   LARGE_INTEGER m_kernelTimeStop;

   // The thread's kernel time.
   __int64 m_kernelTime;

   // Handle to the thread.
   HANDLE m_thread;
};

// Constructor. Retrieves the current thread handle.
inline PerformanceMonitor::PerformanceMonitor()
{
   m_thread = GetCurrentThread();
}

// Starts the monitoring process.
inline void PerformanceMonitor::start()
{
   FILETIME threadCreationTime;
   FILETIME threadExitTime;
   FILETIME threadKernelTime;
   FILETIME threadUserTime;

   // Read the time information for the current thread.
   if(GetThreadTimes(m_thread, &threadCreationTime, &threadExitTime, &threadKernelTime, &threadUserTime))
   {
      m_userTimeStart = *(LARGE_INTEGER*)&threadUserTime;
      m_kernelTimeStart = *(LARGE_INTEGER*)&threadKernelTime;
   }

   // The Watch must be started after reading the thread times.
   m_watch.start();
}

// Ends the monitoring process and calculates the elapsed time.
inline void PerformanceMonitor::stop()
{
   FILETIME threadCreationTime;
   FILETIME threadExitTime;
   FILETIME threadKernelTime;
   FILETIME threadUserTime;

   // The Watch must be stopped before reading the thread times.
   m_watch.stop();

   // Read the time information for the current thread.
   if(GetThreadTimes(m_thread, &threadCreationTime, &threadExitTime, &threadKernelTime, &threadUserTime))
   {
      m_userTimeStop = *(LARGE_INTEGER*)&threadUserTime;
      m_kernelTimeStop = *(LARGE_INTEGER*)&threadKernelTime;
   }

   elapsedTime();
}

// Returns true if the thread's execution has been suspended during the
// process of monitoring it for more than 'diff' ms.
inline bool PerformanceMonitor::wasThreadSuspended(__int64 diff) const
{
   if(diff == 0)
   {
      return (m_watch.timeInMicroSec() / 1000) > ((m_userTime + m_kernelTime + 5000) / 10000);
   }
   else
   {
      return wallClockTimeInMilliSec() > (userTimeInMilliSec() + kernelTimeInMilliSec() + diff);
   }
}

// Calculates the elapsed time.
inline void PerformanceMonitor::elapsedTime()
{
   m_userTime = (m_userTimeStop.QuadPart - m_userTimeStart.QuadPart);
   m_kernelTime = (m_kernelTimeStop.QuadPart - m_kernelTimeStart.QuadPart);
}

// Returns the thread's wall clock time in units of ms.
inline __int64 PerformanceMonitor::wallClockTimeInMilliSec() const
{
   return m_watch.timeInMicroSec() / 1000;
}

// Returns the thread's user time in units of ms.
inline __int64 PerformanceMonitor::userTimeInMilliSec() const
{
   return m_userTime != 0 ? (m_userTime + 5000) / 10000 : 0;
}

// Returns the thread's kernel time in units of ms.
inline __int64 PerformanceMonitor::kernelTimeInMilliSec() const
{
   return m_kernelTime != 0 ? (m_kernelTime + 5000) / 10000 : 0;
}

#endif // PERFORMANCEMONITOR_HXX
