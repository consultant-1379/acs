#ifndef HEADER_GUARD_CLASS__ACS_ACA_ConfigurationHelper
#define HEADER_GUARD_CLASS__ACS_ACA_ConfigurationHelper ACS_ACA_ConfigurationHelper

/** @file acs_aca_configuration_helper.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-04-30
 *
 *	COPYRIGHT Ericsson AB, 2014
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2014-04-30 | xcasale      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <string>
#include <map>

#include "ace/Recursive_Thread_Mutex.h"

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_ACA_ConfigurationHelper

/** @class ACS_ACA_ConfigurationHelper acs_aca_configuration_helper.h
 *	@brief ACS_ACA_ConfigurationHelper class
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-04-30
 *
 *	ACS_ACA_ConfigurationHelper <PUT DESCRIPTION>
 */
class __CLASS_NAME__ {

	//==============//
	// Constructors //
	//==============//
private:
	__CLASS_NAME__ ();
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__ () {}

	//===========//
	// Functions //
	//===========//
public:
	static int get_data_disk_path (char * path, int & path_len);
	static int get_data_disk_path (unsigned int cp_id, char * path, int & path_len);
	static void force_configuration_reload ();

	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static const char * const ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME;

	static ACE_Recursive_Thread_Mutex _configuration_sync;

	static volatile bool _scp_info_already_loaded;
	static std::string _scp_data_disk_path;
	static std::map<unsigned int, std::string> _mcp_data_disk_paths_map;
};

#endif // HEADER_GUARD_CLASS__ACS_ACA_ConfigurationHelper
