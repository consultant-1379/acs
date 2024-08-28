/*=================================================================== */
/**
@file		acs_logm_smxlogfile.h

@brief		Header file of SMX LOGM File cleaning Functionality

This module contains all the declarations useful to specify the class.

@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       3/10/2016     XCSRPAD       Initial Release
==================================================================== */
/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_LOGM_SMXLOGFILE_H_
#define ACS_LOGM_SMXLOGFILE_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_logfile.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_smxlogfile

@brief		This class represents all the functionalities related
 *			to cleaning of SMX LOG Files.
 **/
/*=================================================================== */


class acs_logm_smxlogfile : public acs_logm_logfile
{
public:
	/**
	 * @brief	Constructor.

	 */
	acs_logm_smxlogfile(acs_logm_configurationdata & aConfigData);
	/**
	 * @brief	Destructor.

	 */
	virtual ~acs_logm_smxlogfile();
	/**
	 * @brief	This method used to start house keeping action.
	 * @return	true - If operation is successful. false - If operation is not successful.
	 */
	virtual bool houseKeepingAction();
};

#endif /* ACS_LOGM_smxLOGFILE_H_ */
