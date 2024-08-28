//======================================================================
//
// NAME
//      Synchron.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Implementation of Synchron for basic functionalities like Mutex, Singleton and thread.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Synchron_h
#define AcsHcs_Synchron_h

#include <sstream>
#include <string>
#include <map>

#include <ace/ACE.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_tracer.h"

using namespace std;

namespace AcsHcs
{
	/**
	* Synchron - Synchronisation.
	*/
	namespace Synchron
	{
	//	ACS_HCS_TRACE_INIT ;
		/**
		* Synchron::ExceptionAcquireLock - Exception on unsuccessful lock-acquisition.
		* Synchron::ExceptionAcquireLock is thrown in the case of an unsuccessful attempt to acquire a lock.
		*/
		class ExceptionAcquireLock : public Exception
		{
		public:
			ExceptionAcquireLock(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionAcquireLock() throw() {}
		};

		/**
		* template for a Guard used for synchronization
		*/
		template<class T> class Guard
		{
		public:
			/**
			* Constructor.
			* Implicitley acquires the lock if parameter 'doAquire' == true.
			* @param[in] lock      The lock to be guarded. 
			* @param[in] doAcquire If true, implicitly acquire the lock.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			Guard(T& lock, bool doAcquire = true) : lock(&lock), unlocked(true)
			{
				if (doAcquire)
					this->acquire();
			}

			/**
			* Destructor.
			* Releases the lock.
			*/
			virtual ~Guard()
			{
				this->release();
			}

			/**
			* Acquires the lock.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			void acquire()
			{
				this->unlocked = (this->lock->acquire() == -1);

				if (this->unlocked)
					ACS_HCS_THROW(ExceptionAcquireLock, "acquire()", "Could not acquire the lock.", ACE_OS::strerror(ACE_OS::last_error()));
			}

			/**
			* Releases the lock;
			*/
			void release()
			{
				if (!this->unlocked)
				{
					this->unlocked = true;
					this->lock->release();
				}
			}

			/**
			* Tries to acquire the lock.
			* @return true on success, false otherwise.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			bool tryAcquire()
			{
				this->unlocked = (this->lock->tryacquire() == -1);

				if (this->unlocked)
				{
					unsigned int errNo = ACE_OS::last_error();

					if (errNo == EBUSY)
						return false;

					ACS_HCS_THROW(ExceptionAcquireLock, "tryAcquire()", "Could not acquire the lock.", ACE_OS::strerror(errNo));
				}

				return true;
			}

		protected:
			T*  lock;
			int unlocked;

		private:
			Guard(const Guard&);
			Guard& operator=(const Guard&);
		};

		/**
		* template for a read guard used for file access synchronization
		*/
		template<class T> class GuardRead : private Guard<T>
		{
		public:
			/**
			* Constructor.
			* Implicitley acquires the lock if parameter 'doAquire' == true.
			* @param[in] lock      The lock to be guarded. 
			* @param[in] doAcquire If true, implicitly acquire the lock.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			GuardRead(T& lock, bool doAcquire = true) : Guard<T>(lock, false)
			{
				if (doAcquire)
					this->acquire();
			}

			/**
			* Destructor.
			* Releases the lock.
			*/
			~GuardRead()
			{
			}

			/**
			* Acquires the lock for read-access only.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			void acquire()
			{
				this->unlocked = (this->lock->acquire_read() == -1);

				if (this->unlocked)
					ACS_HCS_THROW(ExceptionAcquireLock, "acquire()", "Could not acquire the lock.", ACE_OS::strerror(ACE_OS::last_error()));
			}

			/**
			* Tries to acquire the lock for read-access only.
			* @return true on success, false otherwise.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			bool tryAcquire()
			{
				this->unlocked = (this->lock->tryacquire_read() == -1);

				if (this->unlocked)
				{
					unsigned int errNo = ACE_OS::last_error();

					if (errNo == EBUSY)
						return false;

					ACS_HCS_THROW(ExceptionAcquireLock, "tryAcquire()", "Could not acquire the lock.", ACE_OS::strerror(errNo));
				}

				return true;
			}

		private:
			GuardRead(const GuardRead&);
			GuardRead& operator=(const GuardRead&);
		};

		/**
		* template for a write guard used for file access synchronization
		*/
		template<class T> class GuardWrite : private Guard<T>
		{
		public:
			/**
			* Constructor.
			* Implicitley acquires the lock if parameter 'doAquire' == true.
			* @param[in] lock      The lock to be guarded. 
			* @param[in] doAcquire If true, implicitly acquire the lock.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			GuardWrite(T& lock, bool doAcquire = true) : Guard<T>(lock, false)
			{
				if (doAcquire)
					this->acquire();
			}

			/**
			* Destructor.
			* Releases the lock.
			*/
			~GuardWrite()
			{
			}

			/**
			* Acquires the lock for write-access only.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			void acquire()
			{
				this->unlocked = (this->lock->acquire_write() == -1);

				if (this->unlocked)
					ACS_HCS_THROW(ExceptionAcquireLock, "acquire()", "Could not acquire the lock.", ACE_OS::strerror(ACE_OS::last_error()));
			}

			/**
			* Tries to acquire the lock for write-access only.
			* @return true on success, false otherwise.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			bool tryAcquire()
			{
				this->unlocked = (this->lock->tryacquire_write() == -1);

				if (this->unlocked)
				{
					unsigned int errNo = ACE_OS::last_error();

					if (errNo == EBUSY)
						return false;

					ACS_HCS_THROW(ExceptionAcquireLock, "tryAcquire()", "Could not acquire the lock.", ACE_OS::strerror(errNo));
				}

				return true;
			}

		private:
			GuardWrite(const GuardWrite&);
			GuardWrite& operator=(const GuardWrite&);
		};

		/** template for a singleton */
		template<class T> class Singleton
		{
		public:
			/**
			* Returns the singleton instance of class T.
			* @return The singleton instance of class T.
			* @throws AcsHcs::Synchron::ExceptionAcquireLock
			*/
			static T* get()
			{
				T* instance = ACE_Singleton<T, ACE_Thread_Mutex>::instance();

				if (instance == 0)
					ACS_HCS_THROW(Synchron::ExceptionAcquireLock, "getInstance()", "Error creating singleton. Probable cause: Could not acquire the lock.", "");

				return instance;
			}
		};

		/**
		*      @brief          ThrExitHandler
		*                      Class for handling thread exit.
		*
		*/
		class ThrExitHandler : public ACE_At_Thread_Exit
		{
		public:
			static bool init()
			{
				ThrExitHandler *thrExitHandler = new ThrExitHandler();
				if( thrExitHandler == 0 )
				{
					return false;
				}
				ACE_hthread_t hthread;
				ACE_Thread_Manager::instance()->thr_self(hthread);				
				thrExitHandler->setThrStatusInMap(hthread, true);

				int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

				if( result == -1 )
				{
					return false;
				}
				else
				{	
					return true;
				}
			}

			static void cleanup()
			{
				theThrMutex.acquire();

				if( !theThrStatusMap.empty())
				{
					//Check if any thread has exited or not.
					for( map<ACE_hthread_t, bool>::iterator itr = theThrStatusMap.begin();
						itr != theThrStatusMap.end();  ++itr )
					{
						cout << "Map data"<< itr->first << " "<< itr->second <<endl;
					}
					theThrStatusMap.clear();
				}
				theThrMutex.release();
				theThrMutex.remove();
			}

			static bool CheckAllFuncThreads( void )
			{
				//Lock the mutex.
				theThrMutex.acquire();

				//Check if any thread has exited or not.
				for( map<ACE_hthread_t, bool>::iterator itr = theThrStatusMap.begin();
					itr != theThrStatusMap.end();  ++itr )
				{
					stringstream mesg;
					mesg.clear();

					mesg<<"the thread is:"<< itr->first << " "<< itr->second;

					//ACS_HCS_TRACE(mesg.str());

					//cout << mesg.str() <<endl;

					if( itr->second == true )
					{
						theThrMutex.release();
						return true;
					}
				}
				theThrMutex.release();
				return false;
			}


			virtual void apply(void)
			{
				if( !theThrStatusMap.empty())
				{
					ACE_hthread_t hthread;
					ACE_Thread_Manager::instance()->thr_self(hthread);
					cout <<"Exiting the thread.."<< hthread<<endl;
					setThrStatusInMap(hthread, false);
				}
			}

			static map<ACE_hthread_t, bool>     theThrStatusMap;
			static ACE_Recursive_Thread_Mutex  theThrMutex;

		private:

			void setThrStatusInMap( ACE_hthread_t thrId, bool isThrAlive)
			{
				//Acquire the Mutex
				theThrMutex.acquire();

				map<ACE_hthread_t, bool>::iterator itr = theThrStatusMap.find( thrId ); 
				if ( itr  != theThrStatusMap.end() )
				{
					theThrStatusMap.erase( itr );
				}
				theThrStatusMap.insert(std::pair<ACE_hthread_t, bool>(thrId, isThrAlive));

				cout << "Thread ID:"<< thrId <<endl;
				theThrMutex.release();
			}

		};

	}
}

#endif // AcsHcs_Synchron_h
