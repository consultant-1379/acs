/*=================================================================== */
/**
@file		acs_logm_configurationdata.h

@brief		Header file of LOGM Configuration Data

This module contains all the declarations useful to specify the class.

@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       20/08/2012     XCSSATA       Initial Release
==================================================================== */

/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LOGM_CONFIGURATIONDATA_H_
#define ACS_LOGM_CONFIGURATIONDATA_H_
/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_configurationdata

@brief		This class represents the configuration data used by LOGM
 **/
/*=================================================================== */

class acs_logm_configurationdata
{
public:
	/**
	 * @brief	Unique Name of the LOG File to be cleaned.
	 */
	std::string  theLogName;
	/**
	 * @brief	Absolute path where the log files have to be searched.
	 */
	std::string  theFolderPath;
	/**
	 * @brief	This parameter represents the age of the file/folder in days
	 * 		    File/Folder lesser than this number will be cleaned
	 */
	unsigned int theNumberofOldDays;
	/**
	 * @brief	This parameter represents the file pattern to be searched.
	 */
	std::string  theSearchPattern;
	/**
	 * @brief	This parameter represents the minimum number of files to be
	 * 			maintained irrespective of their age.
	 */
	unsigned int theNumberofFilesToBeMaintained;
	/**
	 * @brief	The Hour in the day during which cleaning has to be initiated.
	 */
	unsigned int theInitiationHour;
	/**
	 * @brief	The Minute in the Hour during which cleaning has to be initiated.
	 */
	unsigned int theInitiationMinute;
	/**
	 * @brief	The parameter represents whether the cleaning must be daily/weekly.
	 */
	enum cleanupFrequency{ DAILY,WEEKLY}   theCleanUpFrequency;
	/**
	 * @brief	The parameter represents periodic cleaning interval in minutes.
	 */
	unsigned int thePeriodicInterval;
	/**
	 * @brief	The parameter represents whether cleaning for this object is enabled/disabled.
	 */
	bool 		 theCleaningEnabled;
	/**
	 * @brief	The parameter represents whether cleaning for this object
	 * 			periodic based (represented by thePeriodicInterval)
	 * 			or time base ( represented by theInitiationHour & theInitiationMinute)
	 */
	enum cleanupInitiation{ TIMEBASED,PERIODICBASED}  theCleanupInitiation;
	/**
	 * @brief	The parameter represents whether cleaning object is file/folder.
	 */
	enum cleanFileObject{ FILE,FOLDER,BOTH}   theCleanFileObject;

	/**
	 * @brief	Default constructor.
	 */
	acs_logm_configurationdata();
	/**
	 * @brief	Copy constructor.
	 */
	acs_logm_configurationdata(const acs_logm_configurationdata & aConfig);
	/**
	 * @brief	Overloaded Assignment operator.
	 */
	acs_logm_configurationdata operator=(const acs_logm_configurationdata & aConfig);
	/**
	 * @brief	This method can be used to Log the Configuration data to LOG file.
	 */
	void logData();
};

#endif /* ACS_LOGM_CONFIGURATIONDATA_H_ */
