/*=================================================================== */
/**
@file		acs_logm_filetime.h

@brief		Header file of Wrapper class which represents all the file/folder time related functionalities.

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
#ifndef ACS_LOGM_FILETIME_H_
#define ACS_LOGM_FILETIME_H_
/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */

#include<acs_logm_time.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_filetime

@brief		This is an Wrapper class which represents all the file/folder time related functionalities.
 **/
/*=================================================================== */

class acs_logm_filetime:public  acs_logm_time
{
protected:
	/**
	 * @brief	This represents the absolute path of the file/folder.
	 */
	std::string theFilePath;
	/**
	 * @brief	This represents whether the absolute path is a  file or a folder.
	 */
	bool theIsDirectory;
public:
	/**
	 * @brief	Constructor
	 */
	acs_logm_filetime(std::string aFilePath);
	/**
	 * @brief	Destructor
	 */
	~acs_logm_filetime();
	/**
	 * @brief	This method used to calculate if the folder/file is older than given days.
	 * @param   aNumberDays - Number of days that is to be used for calculation
	 * @return	true - If file/folder is old. false - If file/folder is not old.
	 */
	bool isFileOlderThanGivenDays(int aNumberDays);
	/**
	 * @brief	This method used to calculate if the folder/file is older than given Hours.
	 * @param   aNumberHours - Number of Hours that is to be used for calculation
	 * @return	true - If file/folder is old. false - If file/folder is not old.
	 */
	bool isFileOlderThanGivenHours(int aNumberHours);
	/**
	 * @brief	This method used to calculate if the folder/file is older than given Minutes.
	 * @param   aNumberMinutes - Number of Minutes that is to be used for calculation
	 * @return	true - If file/folder is old. false - If file/folder is not old.
	 */
	bool isFileOlderThanGivenMinutes(int aNumberMinutes);
	/**
	 * @brief	This method used to calculate if the folder/file is directory.
	 * @return	true - If it is folder. false - If it is file.
	 */
	bool isDirectory();
	/**
	 * @brief	This method used to calculate if the folder/file exists.
	 * @param   aFilePath - Absolute path of the file/folder
	 * @return	true - If file/folder exists. false - If file/folder does not exists.
	 */
	static bool isFileObjectExists(std::string aFilePath);
};


#endif /* ACS_LOGM_FILETIME_H_ */
