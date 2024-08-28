/*=================================================================== */
/**
@file		acs_logm_abstractFactory.h

@brief		Header file of LOGM Abstract Factory.

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

#ifndef ACS_LOGM_ABSTRACTFACTORY_H_
#define ACS_LOGM_ABSTRACTFACTORY_H_

/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>
#include<acs_logm_housekeeping.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_abstractFactory

@brief		This is an abstract class that specifies the interface
*			for Factory functionality.
**/
/*=================================================================== */
class acs_logm_abstractFactory
{
	/*======================================================
	            PUBLIC METHODS
	========================================================*/
public:
	/**
	* @brief	This is a interface method used to create all House keeping components
	* @param	aHouseKeepingComponents - This is list in which all the house keeping components are stored.
	*/
	virtual void createComponents(std::list<acs_logm_housekeeping*>& aHouseKeepingComponents) = 0;

};
#endif /* ACS_LOGM_BASEFACTORY_H_ */
