/*=================================================================== */
/**
@file		acs_logm_housekeeping.h

@brief		Header file of all class which represents the house keeping functionalities


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

#ifndef ACS_LOGM_HOUSEKEEPING_H_
#define ACS_LOGM_HOUSEKEEPING_H_
/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */

#include<acs_logm_base.h>
#include<acs_logm_configurationdata.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_housekeeping

@brief		This class represents all the functionalities related house keeping functionality of LOGM.
 **/
/*=================================================================== */


class acs_logm_housekeeping : public acs_logm_base
{
protected:
	/**
	 * @brief	The configuration data is stored in this attribute.

	 */
	acs_logm_configurationdata 			theConfigurationData;
	/**
	 * @brief	The list of files/folders in the given search path.

	 */
	std::list<std::string> 				theFileObjectsList;
	/**
	 * @brief	The Map of files/folders with last accessed time as the key element.
	 * 			it is a multi-map to ensure that files with same last modified time is stored.

	 */
	std::multimap<double,std::string> 	theTimeSortedFileObjectMap;
	/**
	 * @brief	The next clean up time for this object

	 */
	acs_logm_time 						theNextCleanUpTime;
	/**
	 * @brief	The next cleanup time is calculated for this object

	 */

	void			calculateNextCleanUpTime();


public:
	/**
	 * @brief	Default Constructor.

	 */
	acs_logm_housekeeping();
	/**
	 * @brief	Argument Constructor.

	 */
	acs_logm_housekeeping(acs_logm_configurationdata& aConfigData);
	/**
	 * @brief	Destructor.

	 */
	virtual 		~acs_logm_housekeeping();
	/**
	 * @brief	This method used to reset all the containers in the object.
	 */
	void 			resetAll();
	/**
	 * @brief	This method used to extract the list of files/folders in the given search path.

	 */
	bool 			extractFolderContent();
	/**
	 * @brief	This method used to extract the list of subfolders that has to be deleted.

	 */
	bool extractSubDirectories(std::list<std::string>& aSubDirectoryList);
	/**
	 * @brief	This method used to extract the list of files/folders that has to be deleted.

	 */
	virtual bool	extractToBeDeletedFileObjects();
	/**
	 * @brief	This method used to determine if the given search pattern is available in the path
	 * @param   aPath - Absolute path of the folder
	 * @return	true - If the file with the pattern exists. false - If the file with the pattern does not exists.
	 */
	virtual bool	isSearchPatternAvailable(std::string& aPath);
	/**
	 * @brief	This method used to clean all old files/folders.
	 * @return	true - If no problem found in cleaning. false - If  problem found in cleaning.
	 */
	bool 			clearOldFileObjects();
	/**
	 * @brief	This method used to start house keeping action.
	 * @return	true - If operation is successful. false - If operation is not successful.
	 */
	virtual bool 			houseKeepingAction();
	/**
	 * @brief	This method used to extract the ConfigurationData.
	 * @param   aConfigData - OUT data
	 */
	void 			getConfigurationData(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	This method used to calculate if the time for cleaning is elapsed .
	 * @return	true - If time is elapsed. false - If time is not elapsed.
	 */
	bool 			isTimeForCleanUpElapsed();
	/**
	 * @brief	This method used to ge the log name.
	 * @return	log name
	 */
	std::string 	getLogName();

};
#endif /* ACS_LOGM_HOUSEKEEPING_H_ */
