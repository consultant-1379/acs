/*=================================================================== */
/**
@file		acs_logm_logfile.h

@brief		Header file of LOGM File cleaning Functionality

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
#ifndef ACS_LOGM_LOGFILE_H_
#define ACS_LOGM_LOGFILE_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_housekeeping.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_logfile

@brief		This class represents all the functionalities related
 *			to cleaning of Files.
 **/
/*=================================================================== */


class acs_logm_logfile : public acs_logm_housekeeping
{
public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_logfile(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	Destructor.

	 */
	virtual ~acs_logm_logfile();
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

#endif /* ACS_LOGM_LOGFILE_H_ */
