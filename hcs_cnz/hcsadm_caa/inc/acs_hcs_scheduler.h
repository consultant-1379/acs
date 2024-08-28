//======================================================================
//
// NAME
//      Scheduler.h
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
//      Implementation of Scheduler functionality.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Scheduler_h
#define AcsHcs_Scheduler_h

#include <map>
#include <string>

#include <ace/Singleton.h>
#include <ace/Synch.h>

#include "acs_hcs_environment.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_synchron.h"
#include "ace/OS.h"
#include "ace/Task.h"

namespace AcsHcs
{
	/**
	 * Scheduler - Schedules Jobs.
	 * This class is a Singleton.
	 */
	class Scheduler : public ACE_Task_Base  
	{
		friend class ACE_Singleton<Scheduler, ACE_Thread_Mutex>;			

		public:
			typedef std::map<time_t, JobExecutor::Job> JobMap;

			/**
			 * Use type Singleton to obtain the singleton instance.
			 * Example: Scheduler::Singleton::get();
			 */
			typedef Synchron::Singleton<Scheduler> Singleton;

			/**
			 * Scheduler::ExceptionJobNotFound - Exception indicating that a Job has not been found.
			 * Scheduler::ExceptionJobNotFound is thrown in the case of a Job has not been found.
			 */
			class ExceptionJobNotFound : public Exception
			{
			public:
				ExceptionJobNotFound( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
				~ExceptionJobNotFound() throw(){}
			};

			/**
			 * Scheduler::ExceptionJobAlreadyExists - Exception indicating that the Job to be scheduled already exists.
			 * Scheduler::ExceptionJobAlreadyExists is thrown in the case that the Job to be scheduled already exists.
			 */
			class ExceptionJobAlreadyExists : public Exception
			{
			public:
				ExceptionJobAlreadyExists( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
				~ExceptionJobAlreadyExists() throw(){}
			};

			/**
			 * Scheduler::ExceptionJobStartTimeExpired - Exception indicating that the start-time of the Job to be scheduled has expired.
			 * Scheduler::ExceptionJobStartTimeExpired is thrown in the case that the start-time of the Job to be scheduled has expired.
			 */
			class ExceptionJobStartTimeExpired : public Exception
			{
			public:
				ExceptionJobStartTimeExpired( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
				~ExceptionJobStartTimeExpired() throw() {}
			};

			/**
			 * Scheduler::ExceptionStart - Exception indicating problems starting the Scheduler.
			 * Scheduler::ExceptionStart is thrown in the case of problems starting the Scheduler.
			 */
			class ExceptionStart : public Exception
			{
			public:
				ExceptionStart( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
				~ExceptionStart() throw(){}
			};

			/**
			 * Start a thread to execute the scheduled jobs.
			 * @throws Scheduler::ExceptionStart
			 */
			static void Start();

			/**
			 * Destructor.
			 */
			~Scheduler();

			/**
			 * Checks if a job with the same name already exists in the queue of scheduled Jobs.
			 * @param[in] jobName the Job name to be checked
			 * @return true if the Job name is unique in the queue of scheduled Jobs, false otherwise.
			 */
			bool isJobNameUnique( const std::string& jobName );

			/**
			 * Adds a Job to the queue of scheduled Jobs.
			 * @param[in]  job  The Job to be enqueued.
			 */
			void jobAdd( const JobExecutor::Job& job );

			/**
			 * Executes the next scheduled Job.
			 */
			void jobExecute();

			/**
			 * Removes a Job from the queue of scheduled Jobs.
			 * @param[in]  job  The Job to be removed.
			 * @throws Scheduler::ExceptionJobNotFound
			 */
			void jobRemove( const std::string jobName );

			/**
			 * Returns the sorted map of scheduled Jobs.
		 	 * @param[inout] jobs The sorted map of scheduled Jobs.
		 	 */
			void jobsGet( JobMap& jobs ) const;

			/**
			* Checks if the Scheduler has been stopped.
			* @return true if the Scheduler has been stopped, else false.
			*/
			bool hasBeenStopped() const;

			/**
			* Stops the Scheduler's main loop and thread.
			* However, the Scheduler will wait for an ongoing rule check to finish.
			*/
			void Stop();

		private:

			/**
			 * JobQueue - Encapsulates the queue of scheduled Jobs.
			 * 
			 */
			class JobQueue
			{
				public:
					JobQueue();

					~JobQueue();

					/**
					 * Checks if no scheduled job exists with the given name. 
					 * @param[in] jobName Name of the job.
					 */
					bool isJobNameUnique( const std::string& jobName );

					/**
					 * Adds a Job to the queue of scheduled Jobs.
					 * @param[in]  job         The Job to be enqueued.
					 * @param[in]  startTime   Time when to schedule the Job.
					 * @param[in]  writeToFile If true, write queue to file.
					 */
					void jobAdd( const JobExecutor::Job& job, time_t startTime, bool writeToFile = true );

					/**
					 * After successful execution of the job, the job is removed
					 * from the queue of scheduled jobs.  If there are more jobs
					 * in the series, the next job is added to the queue.
					 * @param[in] job  The job that has been executed.
					 */
					void jobExecuted( JobExecutor::Job& job );

					/**
					 * Returns the next job from the queue that has to be executed.
					 * @param[out] job  The job to be executed next.
					 * @return The planned start time of the running job is returned. If no job is currently running, '0' is returned.
					 */
					time_t jobGet( JobExecutor::Job& job );

					/**
					 * After unsuccessful execution of the job, the job remains
					 * in the queue of scheduled jobs.
					 * @param[in] job  The job that has not been executed.
					 */
					void jobNotExecuted( JobExecutor::Job& job );

					/**
					 * Removes a Job from the queue of scheduled Jobs.
					 * @param[in]  job  The Job to be removed.
					 * @throws Scheduler::ExceptionJobNotFound
					 */
					void jobRemove( const std::string jobName );

					/**
					 * Returns the sorted map of scheduled Jobs.
					 * @param[inout] jobs The sorted map of scheduled Jobs.
					 */
					void jobsGet( JobMap& jobs ) const;

					/**
					 * Environment object
					 */
					Environment env;

					/**
					 * Running job.
					 */
					time_t runningJob;

			    private:

					/**
					 * file variables
					 */
					mutable ACE_RW_Thread_Mutex mutexFileName;

					/**
					 * Queues
					 */
					JobMap expertQueue;
					JobMap normalQueue;

					/**
					 * Constructors
					 */					
					JobQueue( const JobQueue& );
					JobQueue& operator=( const JobQueue& );

					/**
					 * Formats the date and time.
					 */
					string convertDateTime( const time_t );

					/**
					 * Adds a Job to the queue of scheduled Jobs.
					 * @param[in]  job         The Job to be enqueued.
					 * @param[in]  writeToFile If true, write queue to file.
					 */
					void jobAdd( const JobExecutor::Job& job, bool writeToFile);

					/**
					 * Read the job queues from a file.
					 */
					void readQueueFromFile();

					/**
					 * Write the job queues to a file.
					 */
					void writeQueueToFile();
			};
			
			/**
			 * Constructors
			 */
			Scheduler();
			Scheduler( const Scheduler& );
			Scheduler& operator=( const Scheduler& );
			
			/**
			 * Execute the scheduled jobs.
			 */
			//static void Execute( void* );
			int svc(void);
			void execute();

			/**
			  * Job queue object.
			  */
			JobQueue queue;

			typedef ACE_Atomic_Op<ACE_Thread_Mutex, bool> stoppedDef;
			stoppedDef stopped;
	};
}

#endif // AcsHcs_Scheduler_h
