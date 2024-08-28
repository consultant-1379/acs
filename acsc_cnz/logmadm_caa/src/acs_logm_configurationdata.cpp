#include<acs_logm_configurationdata.h>

		acs_logm_configurationdata::acs_logm_configurationdata()
		:theLogName(""),
		 theFolderPath(""),
		 theNumberofOldDays(0),
		 theSearchPattern(""),
		 theNumberofFilesToBeMaintained(0),
		 theInitiationHour(0),
		 theInitiationMinute(0),
		 theCleanUpFrequency(acs_logm_configurationdata::DAILY),
		 thePeriodicInterval(0),
		 theCleaningEnabled(false),
		 theCleanupInitiation(acs_logm_configurationdata::TIMEBASED),
		 theCleanFileObject(acs_logm_configurationdata::FILE)
		{
			DEBUG("%s","Entering acs_logm_configurationdata::acs_logm_configurationdata");
			DEBUG("%s","Leaving acs_logm_configurationdata::acs_logm_configurationdata");

		}
		acs_logm_configurationdata  acs_logm_configurationdata::operator=(const acs_logm_configurationdata & aConfigData)
		{
			DEBUG("%s","Entering acs_logm_configurationdata::operator=");
			theLogName = aConfigData.theLogName;
			theFolderPath   = aConfigData.theFolderPath;
			theNumberofOldDays = aConfigData.theNumberofOldDays;
			theSearchPattern  = aConfigData.theSearchPattern;
			theNumberofFilesToBeMaintained  = aConfigData.theNumberofFilesToBeMaintained;
			theInitiationHour  = aConfigData.theInitiationHour;
			theInitiationMinute  = aConfigData.theInitiationMinute;
			theCleanUpFrequency  = aConfigData.theCleanUpFrequency;
			thePeriodicInterval  = aConfigData.thePeriodicInterval;
			theCleaningEnabled   = aConfigData.theCleaningEnabled;
			theCleanupInitiation  = aConfigData.theCleanupInitiation;
			theCleanFileObject  = aConfigData.theCleanFileObject;
			DEBUG("%s","Leaving acs_logm_configurationdata::operator=");
			return *this;
		}

		acs_logm_configurationdata::acs_logm_configurationdata(const acs_logm_configurationdata & aConfigData)
		{
			DEBUG("%s","Entering acs_logm_configurationdata::acs_logm_configurationdata-copy constructor");
			theLogName = aConfigData.theLogName;
			theFolderPath   = aConfigData.theFolderPath;
			theNumberofOldDays = aConfigData.theNumberofOldDays;
			theSearchPattern  = aConfigData.theSearchPattern;
			theNumberofFilesToBeMaintained  = aConfigData.theNumberofFilesToBeMaintained;
			theInitiationHour  = aConfigData.theInitiationHour;
			theInitiationMinute  = aConfigData.theInitiationMinute;
			theCleanUpFrequency  = aConfigData.theCleanUpFrequency;
			thePeriodicInterval  = aConfigData.thePeriodicInterval;
			theCleaningEnabled   = aConfigData.theCleaningEnabled;
			theCleanupInitiation  = aConfigData.theCleanupInitiation;
			theCleanFileObject  = aConfigData.theCleanFileObject;
			DEBUG("%s","Leaving acs_logm_configurationdata::acs_logm_configurationdata-copy constructor");
		}
		void acs_logm_configurationdata::logData()
		{
			DEBUG("Entering acs_logm_configurationdata::logData - %s","Logging configuration Data");
			DEBUG("%s","----------------------------------------------------");
			DEBUG("acs_logm_configurationdata::logData - Log Name - %s",theLogName.c_str());
			DEBUG("acs_logm_configurationdata::logData - Folder Path - %s",theFolderPath.c_str());
			DEBUG("acs_logm_configurationdata::logData - Search Pattern - %s",theSearchPattern.c_str());
			DEBUG("acs_logm_configurationdata::logData - NumberofFilesToBeMaintained - %d",theNumberofFilesToBeMaintained);
			DEBUG("acs_logm_configurationdata::logData - NumberofOldDays - %d",theNumberofOldDays);
			if(theCleaningEnabled)
			{
				DEBUG("acs_logm_configurationdata::logData - %s","Cleaning is Enabled");
			}
			else
			{
				DEBUG("acs_logm_configurationdata::logData - %s","Cleaning is Disabled");
			}
			if(theCleanupInitiation == acs_logm_configurationdata::TIMEBASED)
			{
				DEBUG("acs_logm_configurationdata::logData - InitiationHour - %d",theInitiationHour);
				DEBUG("acs_logm_configurationdata::logData - InitiationMinute - %d",theInitiationMinute);
				if(theCleanUpFrequency == acs_logm_configurationdata ::DAILY )
				{
					DEBUG("acs_logm_configurationdata::logData - %s","theCleanUpFrequency is DAILY");
				}
				else
				{
					DEBUG("acs_logm_configurationdata::logData - %s","theCleanUpFrequency is WEEKLY");
				}

			}
			else
			{
				DEBUG("acs_logm_configurationdata::logData - Periodic Interval - %d",thePeriodicInterval);
			}
			DEBUG("%s","----------------------------------------------------");
			DEBUG("Leaving acs_logm_configurationdata::logData - %s","Logging configuration Data");
		}

