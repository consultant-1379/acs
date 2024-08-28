//======================================================================
//
// NAME
//      Configuration.h
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
//      Implementation of the configurable properties of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Configuration_h
#define AcsHcs_Configuration_h

#include <string>

#include <ace/Singleton.h>
#include <ace/Synch.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_properties.h"
#include "acs_apgcc_omhandler.h"

using namespace std;

namespace AcsHcs
{
	/**
	* Configuration - Provides a common interface for all configuration issues.
	*/
	class Configuration : private Properties
	{
		friend class ACE_Singleton<Configuration, ACE_Thread_Mutex>;

	public:
		/**
		* Use type Singleton to obtain the singleton instance.
		* Example: Configuration::Singleton::get();
		*/
		typedef Synchron::Singleton<Configuration> Singleton;

		/**
		* Destructor.
		*/
		~Configuration();

		/**
		* Gets the maximum size in bytes of the data-directory.
		* Default is 10 MB.
		* @return Maximum size in bytes of the data-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		unsigned long getDataDirSizeMax() const;

		/**
		* Gets the maximum allowed memory usage in percentage.
		* Default is 85%.
		* @return maximum allowed memory usage in percentage.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		unsigned long getMemUsageMax() const;

		/**
		* Gets the maximum allowed CPU usage in percentage.
		* Default is 85%.
		* @return maximum allowed CPU usage in percentage.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		unsigned long getCpuUsageMax() const;

		/**
		* Gets the name of the bin-directory of all ACS-applications.
		* Default is 'C:/Program Files/AP/ACS/bin'.
		* @return The name of the ACS-bin-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getBinDir() const;

		/**
		* Gets the name of the Cache-directory.
		* Default is 'K:/ACS/data/HCS/cache'.
		* @return The name of the Cache-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getCacheDirName() const;

		/**
		* Gets the time-to-live in millis of a file in the Cache-directory.
		* Default is 0.
		* @return The time-to-live in millis of a file in the Cache-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		time_t getCacheTtl() const;

		/**
		* Gets the name of the HCS-configuration-directory.
		* Default is 'C:/Program Files/AP/ACS/HCS'.
		* @return The name of the HCS-configuration-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getConfDir() const;

		/**
		* Gets the name of the directory containing the configuration files.
		* Default is 'C:/Program Files/AP/ACS/HCS/conf'.
		* @return The name of the directory containing the configuration files.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getConfDirName() const;

		/**
		* Gets the name of the HCS-data-directory.
		* Default is 'K:/ACS/data/HCS'.
		* @return The name of the HCS-data-directory.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getDataDir() const;

		/**
		* Gets the name of the directory containing the reports.
		* Default is 'K:/ACS/data/HCS/reports'.
		* @return The name of the directory containing the reports.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getReportsDirName() const;

		/**
		* Gets the name of the directory containing the reports.
		* Default is 'K:/ACS/data/HCS/reports/.tmp'.
		* @return The name of the directory containing the reports.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		
		//std::string getReportsTmpDirName()const;
		std::string getReportsTmpDirName(string)const;

		/**
		* Gets the name of the directory containing the rule-sets.
		* Default is 'C:/Program Files/AP/ACS/HCS/rules'.
		* @return The name of the directory containing the rule-sets.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getRulesDirName() const;

		std::string getRulesInitialDirName() const;

		/**
		* Gets the name of the directory containing the scheduled jobs.
		* Default is 'K:/ACS/data/HCS/scheduled'.
		* @return The name of the directory containing the scheduled jobs.
		* @throws AcsHcs::Configuration::ExceptionParameterUndefined
		*/
		std::string getScheduledDirName() const;

		/**
		* Returns whether or not debugging is enabled.
		* Default is 'false'.
		* @return true if debugging is enabled, false otherwise.
		*/
		bool isDebugEnabled() const;

		/**
		* Reads the configuration file.
		* The operation is performed only in the case the file
		* has been updated since it was read last.
		* All properties stored previously are cleared before reading.
		*/
		void read();

		string getNbiHcPath()const;	
		string getRulesDir()const;
		string getReportsDir()const;
		string getCacheDir()const;
		string getBackupPath()const;
		string getRulesInitialDir() const;
		string getHCFolderforTQ() const;
		string getTQDirName(string tqName) const;
		string getReportJobName(string jobName) const;


#ifdef ACS_HCS_SIMULATION_USE
		std::string getApt() const;
		bool isExpertMode() const;
		bool isExpertUser() const;
		std::string getPrintoutDirs() const;
#endif

	private:
		Configuration();
		Configuration(const Configuration&);
		Configuration& operator=(const Configuration&);
	};
}

#endif // AcsHcs_Configuration_h
