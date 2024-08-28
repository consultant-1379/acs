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
N/A       17/01/2013    XCSRPAD       Deletion of unwanted CMX log files
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_LOGM_CMXLOGFILE_H_
#define ACS_LOGM_CMXLOGFILE_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_logfile.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_cmxlogfile

@brief		This class represents all the functionalities related
 *			to cleaning of CMX LOG Files.
 **/
/*=================================================================== */


class acs_logm_cmxlogfile : public acs_logm_logfile
{
public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_cmxlogfile(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	Destructor.

	 */
	virtual ~acs_logm_cmxlogfile();
	/**
	 * @brief	This method used to start house keeping action.
	 * @return	true - If operation is successful. false - If operation is not successful.
	 */
	virtual bool houseKeepingAction();
};

#endif /* ACS_LOGM_CMXLOGFILE_H_ */
