/**
@file		acs_logm_apglogmFactory.h

@brief		Header file of LOGM Factory.

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
N/A       17/01/2013     XCSRPAD       Deletion of unwanted CMX log files	
N/A       03/10/2016     XCSRPAD       Deletion of unwanted SMX log files
==================================================================== */

/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_LOGM_APGLOGMFACTORY_H_
#define ACS_LOGM_APGLOGMFACTORY_H_
/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_logm_abstractFactory.h>
#include <acs_logm_housekeeping.h>
#include <acs_logm_logfile.h>
#include <acs_logm_directory.h>
#include <acs_logm_definitions.h>
#include <acs_logm_apgccwrapper.h>
#include <acs_logm_scxlogfile.h>
#include <acs_logm_cmxlogfile.h>
#include <acs_logm_smxlogfile.h>
/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */

class acs_logm_apglogmFactory:public acs_logm_abstractFactory
{
	/*======================================================
		            PRIVATE METHODS
	========================================================*/
private:
	/**
	* @brief	This is a method used to extract all House keeping components for a given class name in IMM
	* @param	aClassname - Class name in IMM.
	* @param	aHouseKeepingComponents - This is list in which all the house keeping components are stored.
	*/
	bool getConfigurationParametersClassName(std::string aClassname,std::list<acs_logm_housekeeping*>& aHouseKeepingComponentsList);
	/*======================================================
			            PUBLIC METHODS
 	 ========================================================*/
public:
	/**
	* @brief	Constructor Method
	*/
			acs_logm_apglogmFactory();
	/**
	* @brief	Destructor Method
	*/
	virtual ~acs_logm_apglogmFactory();
	/**
	* @brief	This is a interface method used to create all House keeping components
	* @param	aHouseKeepingComponents - This is list in which all the house keeping components are stored.
	*/
	void	createComponents(std::list<acs_logm_housekeeping*>& aHouseKeepingComponents);
};

#endif /* ACS_LOGM_APGLOGMFACTORY_H_ */
