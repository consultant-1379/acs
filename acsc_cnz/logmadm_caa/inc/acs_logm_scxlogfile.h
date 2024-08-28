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
#ifndef ACS_LOGM_SCXLOGFILE_H_
#define ACS_LOGM_SCXLOGFILE_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_logfile.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_scxlogfile

@brief		This class represents all the functionalities related
 *			to cleaning of SCX LOG Files.
 **/
/*=================================================================== */


class acs_logm_scxlogfile : public acs_logm_logfile
{
public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_scxlogfile(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	Destructor.

	 */
	virtual ~acs_logm_scxlogfile();
	/**
	 * @brief	This method used to start house keeping action.
	 * @return	true - If operation is successful. false - If operation is not successful.
	 */
	virtual bool houseKeepingAction();
};

#endif /* ACS_LOGM_SCXLOGFILE_H_ */
