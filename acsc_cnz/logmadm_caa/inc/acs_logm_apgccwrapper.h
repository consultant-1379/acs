/*=================================================================== */
/**
@file		acs_logm_apgccwrapper.h

@brief		Header file of APGCC WRAPPER for all APGCC functions used in LOGM.

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

#ifndef ACS_LOGM_APGCCWRAPPER_H_
#define ACS_LOGM_APGCCWRAPPER_H_


/*====================================================================
			INCLUDE DECLARATION SECTION
==================================================================== */
#include<acs_logm_definitions.h>
#include<acs_logm_tra.h>
#include<acs_logm_configurationdata.h>

/*=====================================================================
			CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
@class		acs_logm_apgccwrapper

@brief		This is an wrapper class that wraps all the APGCC APIs to
*			be used in LOGM.
**/
/*=================================================================== */
class acs_logm_apgccwrapper
{
	/*======================================================
		            PRIVATE METHODS
	========================================================*/
private:
	/**
	* @brief	This method used to extract all the attributes from a instance of a class in IMM.
	* @param	aOmHandler - This Parameter is the APGCC OM Handler object which is used for all APGCC operations.
	* @param	aRDNName   - This Parameter is the RDN of the instance from the attributes has to be extracted.
	* @param	aConfigurationData - This Parameter is the Configuration data instance into which the extracted attributes are stored.
	* @return	If there is any error in the APGCC operation the method will return false.
	*/
	bool getAttributesFromInstance(OmHandler & aOmHandler,std::string& aRDNName,acs_logm_configurationdata& aConfigurationData);
public:
	/**
	* @brief	Constructor method.
	*/
	acs_logm_apgccwrapper();
	/**
	* @brief	destructor method.
	*/
	~acs_logm_apgccwrapper();
	/**
	* @brief	This method is to extract all the instances of the given class from IMM .
	* @param	aClass - This parameter represents the class name
	* @param	aInstanceList - This Parameter is the list into which the extracted instances are stored.
	* @return	If there is any error in the APGCC operation the method will return false.
	*/
	bool getInstances(std::string aClass , std::list<acs_logm_configurationdata>& aInstanceList);
	/**
	* @brief	This method is to extract the absolute path for a given symbolic name .
	* @param	aSymbolicPathName - This parameter represents the symbolic path name
	* @return	The absolute path name is returned.
	*/
	std::string getAbsoluteLogPath(std::string& aSymbolicPathName);

};



#endif /* ACS_LOGM_APGCCWRAPPER_H_ */
