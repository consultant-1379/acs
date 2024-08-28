//======================================================================
//
// NAME
//      Configuration.cpp
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
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <sstream>

#include <ace/ACE.h>
#include <ace/Synch.h>

#include "acs_hcs_properties.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_tra.h"
#include "ACS_APGCC_CLibTypes.h"
#include "ACS_APGCC_CommonLib.h" 

#define ACS_HC_NBI_FOLDER_ATTR_NM "healthCheck"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class Configuration
	//================================================================================

	Configuration::Configuration() : Properties("/data/opt/ap/internal_root/health_check/conf;../health_check/conf;../conf;./conf;.", "server.conf")
	{
		DEBUG("%s","Entering Configuration::Configuration()");
		this->read();
		DEBUG("%s","Leaving Configuration::Configuration()");
	}

	Configuration::~Configuration()
	{
	}

	string Configuration::getNbiHcPath() const
	{
		DEBUG("%s","Entering Configuration::getNbiHcPath()");
	        ACS_APGCC_DNFPath_ReturnTypeT retCode = ACS_APGCC_DNFPATH_FAILURE;
	        ACS_APGCC_CommonLib oComLib;
	        string fileMFuncName = ACS_HC_NBI_FOLDER_ATTR_NM;
		ACE_INT32 dwlen = 530;
		char szPath[530] = {0};
		retCode = oComLib.GetFileMPath(fileMFuncName.c_str(),szPath,dwlen);
		if (retCode != ACS_APGCC_DNFPATH_SUCCESS)
		{	
			DEBUG("%s","return "" in Configuration::getNbiHcPath() ");
			return "";
		}
		else
		{
			DEBUG("%s","return szPath in Configuration::getNbiHcPath() ");
			return szPath;
		}
	}

	string Configuration::getRulesDir() const
	{
		DEBUG("%s"," return this->getNbiHcPath() + /rules ");
		return this->getNbiHcPath() + "/rules";
	}	
	string Configuration::getRulesInitialDir() const
	{
		DEBUG("%s","return /opt/ap/acs/conf");
		return "/opt/ap/acs/conf";
	}	
	string Configuration::getReportsDir() const
	{
		DEBUG("%s","return this->getNbiHcPath() + /reports ");
		return this->getNbiHcPath() + "/reports";	
	}

	string Configuration::getCacheDir() const
	{
		DEBUG("%s","return this->getReportsDir() + /cache ");
		return this->getReportsDir() + "/cache";
	}

	string Configuration::getBackupPath() const
	{
		DEBUG("%s"," return /cluster/storage/system/config/acs_hc");
		return "/cluster/storage/system/config/acs_hc";
	}
	
	string Configuration::getHCFolderforTQ() const
	{
		DEBUG("%s"," return /data/opt/ap/internal_root/data_transfer/source/health_check");
		return  "/data/opt/ap/internal_root/data_transfer/source/health_check";
			
	} 

	string Configuration::getTQDirName(string tqName) const
	{
		DEBUG("%s", "return this->getHCFolderforTQ() +/tqName");
		return this->getHCFolderforTQ() + "/" +tqName; 
				
	}

	string Configuration::getCacheDirName() const
	{
		DEBUG("%s","Leaving Configuration::getCacheDirName()");
		return this->getCacheDir();
	}

	time_t Configuration::getCacheTtl() const
	{
		DEBUG("%s","Entering Configuration::getCacheTtl()");
		const unsigned int  MS = 60 * 1000;
		const unsigned long TTL_DEFAULT = 0;

		try
		{
			string property = Properties::get("acs.hcs.server.cache.ttl.mins");
			time_t ttlMins = ACE_OS::strtoul(property.c_str(), 0, 10);

			if (ttlMins == 0 && property != "0")
			{
				//ACS_HCS_TRACE("getCacheTtl(): Invalid value '" << property << "'. Returning default '" << TTL_DEFAULT << "'.");
				DEBUG("%s","Leaving Configuration::getCacheTtl()");
				return TTL_DEFAULT;
			}

			DEBUG("%s","Leaving Configuration::getCacheTtl()");
			return (ttlMins * MS);
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			//ACS_HCS_TRACE("getCacheTtl(): " << ex.info() << ". Returning default '" << TTL_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getCacheTtl()");
			return TTL_DEFAULT;
		}
	}

	string Configuration::getDataDir() const
	{
		/*DEBUG("%s","Entering Configuration::getDataDir()");
		try
		{
			DEBUG("%s","Leaving Configuration::getDataDir()");
			return Properties::get("acs.hcs.server.dir.data");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const char* DIR_DEFAULT = "K:/ACS/data/HCS";
			//ACS_HCS_TRACE("getDataDir(): " << ex.info() << ". Returning default '" << DIR_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getDataDir()");
			return DIR_DEFAULT;
		}*/
		return getNbiHcPath();
	}

	unsigned long Configuration::getDataDirSizeMax() const
	{
		DEBUG("%s","Entering Configuration::getDataDirSizeMax()");
		const unsigned int  MB = 1000 * 1024;
		const unsigned long SIZE_MAX_DEFAULT = 10 * MB;
		return SIZE_MAX_DEFAULT;
		/*try
		{
			string property = Properties::get("acs.hcs.server.dir.data.size.max");
			unsigned long sizeMax = ACE_OS::strtoul(property.c_str(), 0, 10);

			if (sizeMax == 0L)
			{
				//ACS_HCS_TRACE("getDataDirSizeMax(): Invalid value '" << property << "'. Returning default '" << SIZE_MAX_DEFAULT << "'.");
				DEBUG("%s","Leaving Configuration::getDataDirSizeMax()");
				return SIZE_MAX_DEFAULT;
			}

			DEBUG("%s","Leaving Configuration::getDataDirSizeMax()");
			return (sizeMax * MB);
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			//ACS_HCS_TRACE("getDataDirSizeMax(): " << ex.info() << ". Returning default '" << SIZE_MAX_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getDataDirSizeMax()");
			return SIZE_MAX_DEFAULT;
		}*/
	}

	string Configuration::getReportsDirName() const
	{
		DEBUG("%s","Leaving Configuration::getReportsDirName()");
		return this->getReportsDir();
	}

/*	string Configuration::getReportsTmpDirName() const
	{
		DEBUG("%s","Leaving Configuration::getReportsTmpDirName()");
		return this->getReportsDir() + "/.tmp";
	}
*/


	string Configuration::getReportsTmpDirName(string jobName) const
        {
                //DEBUG("%s","Leaving Configuration::getReportsTmpDirName()");
                return this->getReportJobName(jobName) + "/.tmp";
        }

	string Configuration::getRulesDirName() const
	{
		//DEBUG("%s","Leaving Configuration::getRulesDirName() ");
		return this->getRulesDir();
	}
	string Configuration::getRulesInitialDirName() const
	{
		 //DEBUG("%s","Leaving Configuration::getRulesInitialDir() ");
		 return this->getRulesInitialDir();
	}	
	string Configuration::getScheduledDirName() const
	{
		//DEBUG("%s","Leaving Configuration::getScheduledDirName()");
		return this->getNbiHcPath() + "/scheduled";
	}

        string Configuration::getReportJobName(string jobName) const
        {
                //DEBUG("%s","Leaving Configuration::getScheduledDirName()");
                return this->getReportsDirName()+"/"+jobName;
        }




	unsigned long Configuration::getCpuUsageMax() const
	{
		DEBUG("%s","Entering Configuration::getCpuUsageMax()");
		const unsigned long SIZE_MAX_DEFAULT = 85;

		try
		{
			string property = Properties::get("acs.hcs.server.cpu.usage.max");
			unsigned long usageMax = ACE_OS::strtoul(property.c_str(), 0, 10);

			if (usageMax == 0L)
			{
				//ACS_HCS_TRACE("getCpuUsageMax(): Invalid value '" << property << "'. Returning default '" << SIZE_MAX_DEFAULT << "'.");
				DEBUG("%s","Leaving Configuration::getCpuUsageMax()");
				return SIZE_MAX_DEFAULT;
			}

			DEBUG("%s","Leaving Configuration::getCpuUsageMax()");
			return usageMax;
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			//ACS_HCS_TRACE("getCpuUsageMax(): " << ex.info() << ". Returning default '" << SIZE_MAX_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getCpuUsageMax()");
			return SIZE_MAX_DEFAULT;
		}
	}

	unsigned long Configuration::getMemUsageMax() const
	{
		DEBUG("%s","Entering  Configuration::getMemUsageMax()");
		const unsigned long SIZE_MAX_DEFAULT = 85;

		try
		{
			string property = Properties::get("acs.hcs.server.mem.usage.max");
			unsigned long usageMax = ACE_OS::strtoul(property.c_str(), 0, 10);

			if (usageMax == 0L)
			{
				//ACS_HCS_TRACE("getMemUsageMax(): Invalid value '" << property << "'. Returning default '" << SIZE_MAX_DEFAULT << "'.");
				DEBUG("%s","Leaving  Configuration::getMemUsageMax()");
				return SIZE_MAX_DEFAULT;
			}

			DEBUG("%s","Leaving  Configuration::getMemUsageMax()");
			return usageMax;
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			//ACS_HCS_TRACE("getMemUsageMax(): " << ex.info() << ". Returning default '" << SIZE_MAX_DEFAULT << "'.");
			DEBUG("%s","Leaving  Configuration::getMemUsageMax()");
			return SIZE_MAX_DEFAULT;
		}
	}

	bool Configuration::isDebugEnabled() const
	{
		DEBUG("%s","Entering  Configuration::isDebugEnabled()");
		try
		{
			DEBUG("%s","Leaving  Configuration::isDebugEnabled()");
			return (Properties::get("acs.hcs.server.debug.enabled") == "true");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const bool DEBUG_DEFAULT = false;
			//ACS_HCS_TRACE("isDebugEnabled(): " << ex.info() << ". Returning default '" << DEBUG_DEFAULT << "'.");
			DEBUG("%s","Leaving  Configuration::isDebugEnabled()");
			return DEBUG_DEFAULT;
		}
	}

	void Configuration::read() 
	{
		DEBUG("%s","Entering Configuration::read()");
		try
		{
			Properties::read();
		}
		catch (const Properties::ExceptionFileNotFound& ex)
		{
			// Ignore, i.e. allow a non-existing config-file.
			//ACS_HCS_TRACE(ex.info());
		}
		DEBUG("%s","Leaving Configuration::read()");
	}

#ifdef ACS_HCS_SIMULATION_USE

	string Configuration::getApt() const
	{
		DEBUG("%s","Entering Configuration::getApt()");
		try
		{
			DEBUG("%s","Leaving Configuration::getApt()");
			return Properties::get("acs.hcs.server.debug.apt");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const char* APT_DEFAULT = "MSC-S";
			//ACS_HCS_TRACE("getApt(): " << ex.info() << ". Returning default '" << APT_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getApt()");
			return APT_DEFAULT;
		}
	}

	bool Configuration::isExpertMode() const
	{
		DEBUG("%s","Entering Configuration::isExpertMode()");
		try
		{
			DEBUG("%s","Leaving Configuration::isExpertMode()");
			return (Properties::get("acs.hcs.server.debug.expert.mode") == "true");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const bool MODE_DEFAULT = false;
			//ACS_HCS_TRACE("isExpertMode(): " << ex.info() << ". Returning default '" << MODE_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::isExpertMode()");
			return MODE_DEFAULT;
		}
	}

	bool Configuration::isExpertUser() const
	{
		DEBUG("%s","Entering Configuration::isExpertUser()");
		try
		{
			DEBUG("%s","Leaving Configuration::isExpertUser()");
			return (Properties::get("acs.hcs.server.debug.expert.user") == "true");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const bool MODE_DEFAULT = false;
			//ACS_HCS_TRACE("isExpertUser(): " << ex.info() << ". Returning default '" << MODE_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::isExpertUser()");
			return MODE_DEFAULT;
		}
	}

	std::string Configuration::getPrintoutDirs() const
	{
		DEBUG("%s","Entering Configuration::getPrintoutDirs()");
		try
		{
			DEBUG("%s","Leaving Configuration::getPrintoutDirs()");
			return Properties::get("acs.hcs.server.debug.printout.dirs");
		}
		catch (const Properties::ExceptionPropertyUndefined& ex)
		{
			const char* PRINTOUT_DIRS_DEFAULT = "/data/opt/ap/internal_root/health_check/printouts";
			//ACS_HCS_TRACE("getExpertMode(): " << ex.info() << ". Returning default '" << PRINTOUT_DIRS_DEFAULT << "'.");
			DEBUG("%s","Leaving Configuration::getPrintoutDirs()");
			return PRINTOUT_DIRS_DEFAULT;
		}
	}
	
#endif // ACS_HCS_SIMULATION_USE
}
