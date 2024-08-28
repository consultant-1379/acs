//======================================================================
//
// NAME
//      Environment.h
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
//      To check Environment conditions on APG before performing any Health Check related operation
//      Based on 190 89-CAA xxxxx.
//
// DOCUMENT NO
//      190 89-CAA xxxxx
//
// AUTHOR
//      2011-07-21 by XINDOBE
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Environment_h
#define AcsHcs_Environment_h

#include <string>
#include <queue>
//#include <dos.h>

#include "acs_hcs_configuration.h"
#include "acs_hcs_exception.h"

#define ACS_AXEFUNCTIONS_CLASS_NAME             "AxeFunctions"

namespace AcsHcs
{
	/**
	*check the environment conditions that must be met before a health check job is allowed to be executed,
	*e.g. memory and cpu limits, APT mode, and active node and all resources ujp and running
	*/
	class Environment 
	{
	public:
		/**
		* Environment::ExceptionRetrieve - Exception indicating IO problems.
		* Environment::ExceptionRetrieve is thrown in the case of IO problems.
		*/
		class ExceptionIo : public Exception
		{
		public:
			ExceptionIo(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionIo() throw(){}
		};

		/**
		* Environment::ExceptionLimitCpu - Exception indicating that the CPU load is too high.
		* Environment::ExceptionLimitCpu is thrown in the case the predefined CPU load limit has been crossed.
		*/
		class ExceptionLimitCpu : public Exception
		{
		public:
			ExceptionLimitCpu(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionLimitCpu() throw(){}
		};

		/**
		* Environment::ExceptionLimitMemory - Exception indicating that there is not enough memory.
		* Environment::ExceptionLimitMemory is thrown in the case the predefined lower memory limit has been crossed.
		*/
		class ExceptionLimitMemory : public Exception
		{
		public:
			ExceptionLimitMemory(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionLimitMemory() throw(){}
		};

		/**
		* Environment::ExceptionNodePassive - Exception indicating that the node is passive.
		* Environment::ExceptionNodePassive is thrown in the case that the node is passive.
		*/
		class ExceptionNodePassive : public Exception
		{
		public:
			ExceptionNodePassive(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionNodePassive() throw(){}
		};

		/**
		* Environment::ExceptionNodeReboot - Exception indicating that the node might go for a reboot.
		* Environment::ExceptionNodeReboot is thrown in the case that the node might go for a reboot.
		*/
		class ExceptionNodeReboot : public Exception
		{
		public:
			ExceptionNodeReboot(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionNodeReboot() throw(){}
		};

		/**
		* Environment::ExceptionRetrieve - Exception indicating problems retrieving information about the environment.
		* Environment::ExceptionRetrieve is thrown in the case of problems retrieving information about the environment.
		*/
		class ExceptionRetrieve : public Exception
		{
		public:
			ExceptionRetrieve(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionRetrieve() throw(){}
		};

		 Environment();

		~Environment();

		typedef enum APT{MSCS, MSCBC, HLR, HLRBC, WLN, IPSTPS, UnknownType} APType;

		typedef enum swUpdateState
		{
			SW_UP_UNKNOWN  = 0,
			SW_UP_INITIALIZED  = 1,
			SW_UP_PREPARE_IN_PROGRESS = 2,
			SW_UP_PREPARE_COMPLETED = 3,
			SW_UP_ACTIVATION_IN_PROGRESS = 4,
			SW_UP_ACTIVATION_STEP_COMPLETED = 5,
			SW_UP_WAITING_FOR_COMMIT = 6,
			SW_UP_COMMIT_COMPLETED = 7,
			SW_UP_DEACTIVATION_IN_PROGRESS = 8
		};

		
		/**
		* Checks if environment conditions to execute HCS commands other than hcjdef are as expected.
		* @throws Environment::ExceptionNodePassive
		* @throws Environment::ExceptionNodeReboot
		*/
		void assertEnvSetHCSCmd();

		/**
		* Checks if environment conditions to execute 'hcjdef' command are as expected.
		* @throws Environment::ExceptionLimitCpu
		* @throws Environment::ExceptionLimitMemory
		*/
		
		void assertEnvSetHcjdef(std::string jobName = "");

		
		/**
		* Inserts a new CPU usage sample in the cpu usage queue.
		* @return '0' if successul else '-1'.
		*/
		int pushCpuUsageSample();

		/**
		* Removes the first CPU usage sample from the cpu usage queue.
		* @return .
		*/
		void popCpuUsageSample();

		/**
		* Inserts a new memory usage sample in the memory usage queue.
		* @return '0' if successul else '-1'.
		*/
		int pushMemUsageSample();

		/**
		* Removes the first memory usage sample from the memory usage queue.
		* @return .
		*/
		void popMemUsageSample();

		/**
		* Gets the count of cpu usage samples in cpuUsageSamples queue.
		* @return the count of cpu usage samples.
		*/
		int getCpuUsageCount() const;

		/**
		* Gets the count of memory usage samples in memUsageSamples queue.
		* @return the count of memory usage samples.
		*/
		int getMemUsageCount() const;

		/**
		* Checks if the node is in EXPERT Mode and the APT is MSC.
		* @param[out] expertMode If the node is in EXPERT mode, this parameter is set to true.
		* @param[out] apt It is set to a value from enum APType depending on the APT of the node.
		* @throws Environment::ExceptionIo
		* @throws Environment::ExceptionRetrieve
		*/
		bool getMode(bool& expertMode) const;

		bool getApt(APType& apt) const;

		/**
		* Checks if the node is ACTIVE and all resources are up and running.
		* @return 'true' if ACTIVE and all resources are up and running else 'false'.
		*/
		bool isActiveNode();

	private:		

		//Queue to store samples of CPU usage
		static queue<long> cpuUsageSamples;

		//Queue to store samples of memory usage
		static queue<long> memUsageSamples;

		/**
		* Get a CPU usage sample.
		* @return The CPU usage.
		*/
		long getCPUUsage();

		/**
		* Get a Memory load sample.
		* @return The Memory load.
		*/
		long getMemoryLoad();

		/**
		* Get average CPU usage.
		* @return The average CPU usage over 30 seconds.
		*/
		long getAvrgCPUUsage();

		/**
		* Get average Memory Load.
		* @return The average Memory load over 30 seconds.
		*/
		long getAvrgMemoryLoad();

		/**
		* Checks if FCH|Backup|Restore|FCR|SFC is ongoing.
		* @return 'true' if the FCH|Backup|Restore|FCR|SFC is ongoing else false.
		*/
	
		bool isBackUpOngoing();
	
		bool isRestoreOngoing();
		
		bool isSwUpdateOngoing();
	};
}

#endif // AcsHcs_Environment_h
