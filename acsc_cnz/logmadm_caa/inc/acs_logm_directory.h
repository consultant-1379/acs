/*=================================================================== */
/**
@file		acs_logm_directory.h

@brief		Header file of LOGM Directory cleaning Functionality

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

#ifndef ACS_LOGM_DIRECTORY_H_
#define ACS_LOGM_DIRECTORY_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */

#include<acs_logm_housekeeping.h>
#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_directory

@brief		This class represents all the functionalities related
 *			to cleaning of Directory.
 **/
/*=================================================================== */

class acs_logm_directory : public acs_logm_housekeeping
{
public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_directory(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	Virtual Destructor

	 */
	virtual ~acs_logm_directory();
	/**
	 * @brief	This method used to search if the file with given pattern is present in the folder.
	 * @param   aPath - Path of the folder to be searched
	 * @return	true - If pattern is present. false - If the pattern is absent.
	 */
	bool isSearchPatternAvailable(std::string& aPath);
	/**
	 * @brief	This method used to extract all the files and sub folders in the search folder.
	 * @return	true  - If no problem found in extracting the folder content.
	 * 			false -If  problem found in extracting the folder content.
	 */
	bool extractToBeDeletedFileObjects();
};


#endif /* ACS_LOGM_DIRECTORY_H_ */
