/**
@file		acs_logm_base.h

@brief		Header file of acs_logm_base class.

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
#ifndef ACS_LOGM_BASE_H_
#define ACS_LOGM_BASE_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>
#include<acs_logm_filetime.h>
#include <unistd.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_base
@brief		This is an base class that contains all the common methods used in LOGM.
**/
/*=================================================================== */

class acs_logm_base
{
	/*======================================================
	            PUBLIC METHODS
	========================================================*/
public:
	/**
	* @brief	Constructor
	*/
	acs_logm_base();
	/**
	* @brief	Destructor
	*/
	~acs_logm_base();
	/**
	* @brief	This method specifies if the given path is a directory
	* @param	aPath - Absolute path.
	* @return	true in case if the given path is a directory.
	*/
	bool isFileObjectDirectory(string& aPath);
	/**
	 * @brief	This method specifies if the given file path exists or not
	 * @param	aPath - Absolute path.
	 * @return	true in case if the given file exists.
	 */
	bool isFileExists(string& aPath);
	/**
	* @brief	This method gives the elapsed time since the file/folder is last modified
	* @param	aPath - Absolute path.
	* @param	aElapsedTime - Out Variable - The elapsed time in seconds
	* @return	false in case if the given path is not accessible.
	* 			true in case if the given path is  accessible.
	*/
	bool getElapsedTimeSincelastModified(string& aPath,double & aElapsedTime );
	/**
	* @brief	This  method can be used to delete a full or empty directory.
	* @param	aPath - Absolute path.
	* @return	false in case the deletion is not successful.
	* 			true in case the deletion is successful.
	*/
	bool deletedirectory(string& aPath);
	/**
	* @brief	This  method can be used to delete a empty directory.
	* @param	aPath - Absolute path.
	* @return	false in case the deletion is not successful.
	* 			true in case the deletion is successful.
	*/
	bool deleteEmptydirectory(string& aPath);
	/**
	* @brief	This  method can be used to delete a file.
	* @param	aPath - Absolute path.
	* @return	false in case the deletion is not successful.
	* 			true in case the deletion is successful.
	*/
	bool deleteFile(string& aPath);
	/**
	* @brief	This is a method can be used to check if a directory is empty or not
	* @param	aPath - Absolute path.
	* @return	false in case the directory is not empty.
	* 			true in case the directory is empty.
	*/
	bool isDirectoryEmpty(std::string& aPath);

};
#endif






