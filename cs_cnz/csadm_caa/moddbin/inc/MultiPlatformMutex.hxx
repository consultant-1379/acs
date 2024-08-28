//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
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
//  Responsible ............ (start by filling in this field)
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//  This file declares the class MultiPlatformMutex which may be used as a
//  generic interface to a multi platform functional mutex.
//  To help (and to try to ensure the obliteration of deadlocks), the
//  SafeMutex class is provided which may be used in conjunction with the
//  MultiPlatformMutex to try to avoid deadlocks that otherwise could be the
//  result of programming errors.
//  The SafeMutex class utilizes an instance of the MultiPlatformMutex
//  (created by the user) and will attempt to lock the mutex once the
//  SafeMutex is constructed. If the user should fail to place a call to the
//  SafeMutex's unlock() method, then the MultiPlatformMutex will be unlocked
//  once the SafeMutex runs out of scope. Hence, the SafeMutex should not be
//  used as an aggregated member, but as a local method/function variable
//  only.
//>

#ifndef MULTIPLATFORMMUTEX_HXX
#define MULTIPLATFORMMUTEX_HXX

#pragma ident "@(#)filerevision "

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "PlatformTypes.hxx"

#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define UNDEF_WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#ifdef UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef UNDEF_WIN32_LEAN_AND_MEAN
#endif // UNDEF_WIN32_LEAN_AND_MEAN
#elif defined _ALPHA_TRU64 || defined _SPARC_SOLARIS || defined _IPF_LINUX || defined X86_LINUX_
// These platforms all have pthreads implementations, use it
#define use_pthreads__ 1
#include <pthread.h>
#endif
#include <cassert>

class SafeMutex;

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup gen
//
//  MultiPlatformMutex can be used as a mutex on various platforms without
//  the use of commercial libraries. Today the class supports Windows on any
//  platform, and any other platform that supports pthreads.
//
//  Class relationships.
//
//  \verbatim
//  +----------------------+
//  |                      |
//  |  MultiPlatformMutex  |
//  |                      |
//  +----------------------+
//  \endverbatim
//>
class MultiPlatformMutex
{
   friend class SafeMutex;

public:
   // The constructor.
   MultiPlatformMutex();

   // The destructor.
   ~MultiPlatformMutex();

   // Tells whether the mutex is locked by anyone or not.
   bool isLocked() const;

private:
   // Lock the mutex (must be accessed via SafeMutex).
   void lock();

   // Attempt to lock the mutex (this may fail).
   bool tryLock();

   // Unlock the mutex (must be accessed via SafeMutex).
   void unlock();

   // The copy constructor is declared but not implemented to prohibit the
   // copying of instantiated MultiPlatformMutexes
   MultiPlatformMutex(const MultiPlatformMutex&);

   // The copy assignment operator is declared but not defined in order to
   // prevent copying information between MultiPlatformMutexess.
   MultiPlatformMutex& operator=(const MultiPlatformMutex&);

   // Mutex member handle.
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   HANDLE m_mutex;
#elif use_pthreads__
   pthread_mutex_t m_mutex;
#endif

   // Tells whether the MultiPlatformMutex is locked or not.
   volatile bool m_isLockedByAnyone;
};

//< \ingroup gen
//
//  The SafeMutex class may be used to guarantee that no matter how the source
//  code exits its execution path, the provided MultiPlatformMutex will get
//  unlocked if still locked.
//
//  Class relationships.
//
//  \verbatim
//  +-------------+      +----------------------+
//  |             |      |                      |
//  |  SafeMutex  |-Ref->|  MultiPlatformMutex  |
//  |             |      |                      |
//  +-------------+      +----------------------+
//
//  Ref - Reference
//  \endverbatim
//>
class SafeMutex
{
public:
   enum Operation
   {
      Lock,   ///< Wait until the mutex can be locked.
      TryLock ///< Only attempt to lock the mutex if not already locked.
   };

   // The constructor locks the provided mutex upon construction.
   SafeMutex(MultiPlatformMutex&, Operation = Lock);

   // The destructor.
   ~SafeMutex();

   // Lock the user allocated mutex.
   void lock();

   // Unlock the user allocated mutex prior to the destruction of SafeMutex.
   void unlock();

   // Tells whether or not the mutex is locked by the current instance or not.
   bool isLocked() const;

private:
   // Reference to a mutex.
   MultiPlatformMutex& m_mutex;

   // Tells whether the MultiPlatformMutex is locked by the current instance
   // or not.
   bool m_isLocked;
};

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  The constructor.
//>
//----------------------------------------------------------------------------
inline MultiPlatformMutex::MultiPlatformMutex() :
   m_isLockedByAnyone(false)
{
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   m_mutex = CreateMutex(NULL, FALSE, NULL);
   assert(m_mutex != NULL && "m_mutex == NULL");
#elif use_pthreads__
#ifndef NDEBUG
   // avoid unreferenced variable warning when assert is a NOP (NDEBUG defined)
   int status = 
#endif
   pthread_mutex_init(&m_mutex, NULL);
   assert(status == 0 && "failed to initialise the mutex");
#endif
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  The destructor.
//>
//----------------------------------------------------------------------------
inline MultiPlatformMutex::~MultiPlatformMutex()
{
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   BOOL status = CloseHandle(m_mutex);
   assert(status && "failed to close handle for mutex");
#elif use_pthreads__
#ifndef NDEBUG
   // avoid unrefrenced variable warning when assert is NOP (NDEBUG defined)
   int status =
#endif
      pthread_mutex_destroy(&m_mutex);
   assert(status == 0 && "failed to destroy the mutex");
#endif
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Tells whether the mutex is locked or not.
//
//  \return  True if the mutex is already locked, otherwise false.
//>
//----------------------------------------------------------------------------
inline bool MultiPlatformMutex::isLocked() const
{
   return m_isLockedByAnyone;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Lock the mutex.
//>
//----------------------------------------------------------------------------
inline void MultiPlatformMutex::lock()
{
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   if(WAIT_OBJECT_0 != WaitForSingleObject(m_mutex, INFINITE))
#elif use_pthreads__
   if(0 != pthread_mutex_lock(&m_mutex))
#endif
   {
      assert(0 && "The mutex could not be locked");
   }

   m_isLockedByAnyone = true;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Attempt to lock the mutex (this may fail).
//
//  \return  True if the attempt to lock the mutex was successful.
//>
//----------------------------------------------------------------------------
inline bool MultiPlatformMutex::tryLock()
{
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   m_isLockedByAnyone = (WAIT_OBJECT_0 == WaitForSingleObject(m_mutex, 0)) ? true : false;
#elif use_pthreads__
   m_isLockedByAnyone = (0 == pthread_mutex_trylock(&m_mutex)) ? true : false;
#endif

   return m_isLockedByAnyone;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Release the mutex.
//>
//----------------------------------------------------------------------------
inline void MultiPlatformMutex::unlock()
{
#if defined _ALPHA_WIN32 || defined _INTEL_WIN32
   if(!ReleaseMutex(m_mutex))
#elif use_pthreads__
   if(0 != pthread_mutex_unlock(&m_mutex))
#endif
   {
      assert(0 && "The mutex could not be released");
   }

   m_isLockedByAnyone = false;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  The constructor locks the provided mutex upon unless told to only try to
//  lock the mutex.
//>
//----------------------------------------------------------------------------
inline SafeMutex::SafeMutex(MultiPlatformMutex& mutex, Operation operation) :
   m_mutex(mutex),
   m_isLocked(false)
{
   if(operation == Lock)
   {
      // Lock the mutex unconditionally (wait until ceased).
      lock();
   }
   else
   {
      assert(operation == TryLock && "Programming error, invalid operation value");

      m_isLocked = mutex.tryLock();
   }
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  The destructor unlocks the mutex unless already unlocked.
//>
//----------------------------------------------------------------------------
inline SafeMutex::~SafeMutex()
{
   // Unlock the mutex if still locked by our instance.
   if(m_isLocked == true)
   {
      m_mutex.unlock();
   }
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Lock the user allocated mutex.
//>
//----------------------------------------------------------------------------
inline void SafeMutex::lock()
{
   assert(m_isLocked == false && "Trying to lock an already locked mutex causes a deadlock");

   m_mutex.lock();

   m_isLocked = true;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Unlock the user allocated mutex prior to the destruction of SafeMutex.
//>
//----------------------------------------------------------------------------
inline void SafeMutex::unlock()
{
   assert(m_isLocked == true && "Trying to unlock a mutex that is not locked");

   m_mutex.unlock();

   m_isLocked = false;
}

//----------------------------------------------------------------------------
//< \ingroup genAPI
//
//  Tells whether or not the mutex is locked by the current instance or not.
//
//  \return  True if the mutex is locked by the calling SafeMutex instance.
//           Note that this method may return false even though the mutex is
//           locked by another SafeMutex instance.
//>
//----------------------------------------------------------------------------
inline bool SafeMutex::isLocked() const
{
   return m_isLocked;
}

#endif // MULTIPLATFORMMUTEX_HXX
