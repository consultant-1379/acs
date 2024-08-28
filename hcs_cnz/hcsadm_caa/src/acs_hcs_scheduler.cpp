//======================================================================
//
// NAME
//      Scheduler.cpp
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
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-07-25 by EEDTROB
// CHANGES
//     
//======================================================================

#include <iostream>
#include <fstream>

#include <map>
#include <string>
#include <sstream>

#include <ace/ACE.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_tracer.h"

#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_environment.h"
#include "acs_hcs_scheduler.h"

namespace AcsHcs
{
//	ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class Scheduler
	//================================================================================

	Scheduler::Scheduler() : stopped(true)
	{
	}

	//void Scheduler::Execute(void*)
	int Scheduler::svc(void)
	{
		Scheduler::Singleton::get()->execute();
		return 0;
	}

	void Scheduler::Start()
	{
		Scheduler scheduler;		
		scheduler.activate();
		/*
		if ( ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) Execute,
			                                       0,
												   THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
												   0,
												   0,
												   ACE_DEFAULT_THREAD_PRIORITY,
												   -1,
												   0,
												   ACE_DEFAULT_THREAD_STACKSIZE,
												   0) == -1)
		{
			ACS_HCS_THROW( Scheduler::ExceptionStart, "start()", "Could not start the scheduler.", "" );
		}
		*/
	}

	Scheduler::~Scheduler()
	{
	}

	void Scheduler::execute()
	{
		if ( !Synchron::ThrExitHandler::init() )
		{
			cout << "Error occured while registering for Thread Exit Handler."<<endl;
			//ACS_HCS_TRACE("Error occured while registering for Thread Exit Handler.");
		}

	//	ACS_HCS_TRACE("Scheduler Thread Started.");

		this->stopped = false;
		queue.runningJob = 0;

		while (!this->hasBeenStopped()) // && !HealthCheckCmd::stopEventFlag )
		{
			try
			{
				this->jobExecute();
			}
			catch (...)
			{
				// Ignore.
			}
		}

		Scheduler::Stop();

		//ACS_HCS_TRACE("Scheduler Thread Exited.");
	}

	bool Scheduler::hasBeenStopped() const
	{
		return this->stopped.value();
	}
	
	void Scheduler::Stop()
	{
		this->stopped = true;
		JobExecutor::Singleton::get()->cancel();
	}

	bool Scheduler::isJobNameUnique( const std::string& jobName )
	{
		return this->queue.isJobNameUnique( jobName );
	}

	void Scheduler::jobAdd( const JobExecutor::Job& job )
	{
		time_t startTime = job.getNextStartTime(30); // Rounding up/down to next minute.

		if ( startTime == 0 )
		{
		//	ACS_HCS_TRACE( "jobAdd(): Could not schedule job '" << job.getName() << "'. Cause: Start time has expired." );
			ACS_HCS_THROW( Scheduler::ExceptionJobStartTimeExpired, "jobAdd()", "Could not schedule job '" << job.getName() << "'.", "Start time has expired." );
		}

		if ( ( !isJobNameUnique( job.getName() )) || ( job.getName() == JobExecutor::Singleton::get()->getCurrentJob().getName() ))
		{
		//	ACS_HCS_TRACE( "jobAdd(): Could not schedule job '" << job.getName() << "'. Cause: Job name already exists." );
			ACS_HCS_THROW( Scheduler::ExceptionJobAlreadyExists, "jobAdd()", "Could not schedule job '" << job.getName() << "'.", "Job name already exists." );
		}
	
		this->queue.jobAdd( job, startTime );
	}

	void Scheduler::jobExecute()
	{
		JobExecutor::Job job;

		this->queue.runningJob = this->queue.jobGet( job );

		if ( this->queue.runningJob )
		{
			try
			{
				 JobExecutor::Singleton::get()->start( job );
				 this->queue.jobExecuted( job );
			}
			catch (const JobExecutor::ExceptionJobInProgress&)
			{
				this->queue.jobNotExecuted( job );
			}
			catch (const Environment::ExceptionLimitMemory&)
			{
				this->queue.jobNotExecuted( job );
			}
			catch (const Environment::ExceptionLimitCpu&)
			{
				this->queue.jobNotExecuted( job );
			}
			catch (...)
			{
				// Consider erroneous execution as executed.
				 this->queue.jobExecuted( job );
			}

			this->queue.runningJob = 0;
		}
		else
		{	
			ACE_OS::sleep(10); // 10 s
		}
	}

	void Scheduler::jobRemove( const std::string jobName )
	{
		this->queue.jobRemove( jobName );
	}

	void Scheduler::jobsGet( JobMap& jobs ) const
	{
		this->queue.jobsGet(jobs);
	}

	//================================================================================
	// Class Job Queue
	//================================================================================

	Scheduler::JobQueue::JobQueue()
	{	
		readQueueFromFile();
	}

	Scheduler::JobQueue::~JobQueue()
	{
	}

	bool Scheduler::JobQueue::isJobNameUnique( const std::string& jobName ) 
	{
		if ( !expertQueue.empty())
		{
			for ( JobMap::iterator it = expertQueue.begin(); it != expertQueue.end(); it++ )
			{
				const JobExecutor::Job& job = it->second;

				if ( job.getName() == jobName )			
					return false;
			}
		}

		if ( !normalQueue.empty())
		{
			for ( JobMap::iterator it = normalQueue.begin(); it != normalQueue.end(); it++ )
			{
				const JobExecutor::Job& job = it->second;

				if ( job.getName() == jobName )			
					return false;
			}
		}

		return true;
	}

	string Scheduler::JobQueue::convertDateTime( const time_t startTime )
	{
		struct tm  localTime;
		char       timeBuf[32];

		/*ACE_OS::*/localtime_r( &startTime, &localTime );
		/*ACE_OS::*/strftime( timeBuf, sizeof timeBuf, "%Y-%m-%d %H:%M:%S", &localTime );

		return timeBuf;
	}

	void Scheduler::JobQueue::jobAdd( const JobExecutor::Job& job, time_t startTime, bool writeToFile )
	{
		if ( startTime > 0 )
		{
			Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard( this->mutexFileName );

			while ( expertQueue.count( startTime ) || normalQueue.count( startTime ))
			{
				startTime++;
			}

			if ( job.isExpertUser())
				expertQueue[startTime] = job;
			else
				normalQueue[startTime] = job;

			if (writeToFile)
				this->writeQueueToFile();
		}
	
	}

	void Scheduler::JobQueue::jobAdd( const JobExecutor::Job& job, bool writeToFile )
	{
		if (job.getPeriod() == JobExecutor::Job::/*Period::*/PD_INVALID)
			this->jobAdd(job, job.getNextStartTime(JobExecutor::Job::/*Period::*/PD_HOURLY / 2), writeToFile);
		else
			this->jobAdd(job, job.getNextStartTime(job.getPeriod() / 2), writeToFile);
	}

	void Scheduler::JobQueue::jobExecuted( JobExecutor::Job& job )
	{
		// Command handler might have removed the job from scheduler during execution
		// in that case job is not rescheduled

		try
		{
			job.confirm();
			this->jobRemove( job.getName() );

			if ( job.getPeriod() != JobExecutor::Job::/*Period::*/PD_INVALID )
				this->jobAdd( job, true );
		}
		catch ( const Scheduler::ExceptionJobNotFound& )
		{
			// Ignore
		}
	}

	time_t Scheduler::JobQueue::jobGet( JobExecutor::Job& job )
	{
		try
		{
			if ( !expertQueue.empty() || !normalQueue.empty() )
			{
				JobExecutor::Job currentJob;

				if(!currentJob.isValid())
				{
					time_t currentTime = /*ACE_OS::*/time(0);

					if (!expertQueue.empty() && !normalQueue.empty())
					{
						if (expertQueue.begin()->first < normalQueue.begin()->first)
						{
							JobMap::const_iterator it = expertQueue.begin();

							if (it->first <= currentTime)
							{
								Environment env;
								env.assertEnvSetHcjdef();

								job = it->second;
								return it->first;
							}
						}
						else
						{
							JobMap::const_iterator it = normalQueue.begin();

							if ( it->first <= currentTime )
							{
								Environment env;
								env.assertEnvSetHcjdef();

								bool expertMode;
								Environment::APType apt; // Not used.
								env.getMode(expertMode);
								env.getApt(apt);

								if (!expertMode)
								{
									job = it->second;
									return it->first;
								}
								else // Handle expert queue instead.
								{
									it = expertQueue.begin();

									if (it->first <= currentTime)
									{
										job = it->second;
										return it->first;
									}
								}
							}
						}

					}
					else if (!expertQueue.empty())
					{
						JobMap::const_iterator it = expertQueue.begin();

						if ( it->first <= currentTime )
						{
							Environment env;
							env.assertEnvSetHcjdef();

							job = it->second;
							return it->first;
						}
					}
					else
					{
						JobMap::const_iterator it = normalQueue.begin();

						if ( it->first <= currentTime )
						{
							Environment env;
							env.assertEnvSetHcjdef();

							bool expertMode;
							Environment::APType apt; // Not used.
							env.getMode(expertMode);
							env.getApt(apt);

							if (!expertMode)
							{
								job = it->second;
								return it->first;
							}
						}
					}
				}
			}
		}
		catch (const Environment::ExceptionLimitCpu&)
		{
			return 0;
		}
		catch (const Environment::ExceptionLimitMemory&)
		{
			return 0;
		}

		return 0;
	}

	void Scheduler::JobQueue::jobNotExecuted( JobExecutor::Job& job )
	{
		ACE_OS::sleep(60); // 60 s
		return;
	}

	void Scheduler::JobQueue::jobRemove( const std::string jobName )
	{
		Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard( this->mutexFileName );

		JobMap::iterator it;

		for ( it = expertQueue.begin(); it!=expertQueue.end(); it++ )
		{
			const JobExecutor::Job job = it->second;

			if ( job.getName() == jobName )
			{
				expertQueue.erase( it );
				this->writeQueueToFile();

				return;
			}
		}

		for ( it = normalQueue.begin(); it!=normalQueue.end(); it++ )
		{
			const JobExecutor::Job job = it->second;

			if ( job.getName() == jobName )
			{
				normalQueue.erase( it );
				this->writeQueueToFile();

				return;
			}
		}

		ACS_HCS_THROW(Scheduler::ExceptionJobNotFound, "jobRemove()", "Could not remove job '" << jobName << "'.", "Job not found.");
	}

	void Scheduler::JobQueue::jobsGet(JobMap& jobs) const
	{
		if ( !expertQueue.empty())
			jobs.insert( expertQueue.begin(), expertQueue.end());

		if ( !normalQueue.empty())
			jobs.insert( normalQueue.begin(), normalQueue.end());
	}

	void Scheduler::JobQueue::readQueueFromFile()
	{
		Directory dirScheduler( Configuration::Singleton::get()->getScheduledDirName() );
		std::string fileName = dirScheduler.getName() + "/scheduled_jobs.txt";

		try
		{
			dirScheduler.create();

			std::ifstream inputFile( fileName.c_str() );

			if ( inputFile.is_open() )
			{
				for ( JobExecutor::Job job; inputFile >> job; )
					this->jobAdd( job, false );

				inputFile.close();

				this->writeQueueToFile();
			}
			else
			{
			//	ACS_HCS_TRACE( "readQueueFromFile(): Could not read scheduled jobs from file '" << fileName << "'. Cause: Could not open file." );
			}
		}
		catch ( const Directory::ExceptionIo& ex )
		{
	//		ACS_HCS_TRACE( "readQueueFromFile(): Could not read scheduled jobs from file '" << fileName << "'. Cause: " << ex.what());
		}
	}

	void Scheduler::JobQueue::writeQueueToFile()
	{
		Directory dirScheduler( Configuration::Singleton::get()->getScheduledDirName() );
		std::string fileName = dirScheduler.getName() + "/scheduled_jobs.txt";

		try
		{
			dirScheduler.create();

			std::ofstream outputFile( fileName.c_str() );

			if ( outputFile.is_open())
			{

				for(JobMap::iterator it = expertQueue.begin(); it != expertQueue.end(); it++)
				{
					outputFile << it->second;
				}

				for(JobMap::iterator it = normalQueue.begin(); it != normalQueue.end(); it++)
				{
					outputFile << it->second;
				}

				outputFile.close();
			}

		//	ACS_HCS_TRACE( "writeQueueToFile(): The Scheduled jobs are updated to the file '" << fileName << "'." );
		}
		catch ( const Directory::ExceptionIo& ex )
		{
		//	ACS_HCS_TRACE( "writeQueueToFile(): Could not write scheduled jobs to file '" << fileName << "'. Cause: " << ex.what() );
		}
	}
}
