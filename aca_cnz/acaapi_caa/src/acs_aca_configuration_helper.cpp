#include "ace/Guard_T.h"

#include "ACS_APGCC_CommonLib.h"

#include "acs_aca_api_tracer.h"
#include "acs_aca_configuration_helper.h"


#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__ACS_ACA_ConfigurationHelper

ACS_ACA_TRACE_DEFINE(__CLASS_NAME__)

//////////////////////////////////////////////////////////////
// Initialization of static class fields
//////////////////////////////////////////////////////////////
const char * const __CLASS_NAME__::ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME = "ACS_ACA_DATA";

ACE_Recursive_Thread_Mutex __CLASS_NAME__::_configuration_sync;

volatile bool __CLASS_NAME__::_scp_info_already_loaded = false;
std::string __CLASS_NAME__::_scp_data_disk_path;
std::map<unsigned int, std::string> __CLASS_NAME__::_mcp_data_disk_paths_map;
//////////////////////////////////////////////////////////////


int __CLASS_NAME__::get_data_disk_path (char * path, int & path_len) {
	ACS_ACA_TRACE_FUNCTION;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_configuration_sync);

	// If the path hasn't been loaded yet, ask APGCC library for it
	if (!_scp_info_already_loaded) {
		ACS_APGCC_CommonLib common_lib;
		char tmp_path[path_len];

		ACS_ACA_TRACE_MESSAGE("The info is not available, calling 'GetDataDiskPath' to get the %s path",
				ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME);

		ACS_APGCC_DNFPath_ReturnTypeT return_code = common_lib.GetDataDiskPath(ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME, tmp_path, path_len);
		if (return_code != ACS_APGCC_DNFPATH_SUCCESS) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'GetDataDiskPath' failed, return_code = %d, returning -1!", return_code);
			return -1;
		}

		_scp_data_disk_path = tmp_path;
		_scp_info_already_loaded = true;
	}

	// Copy the path to the output string
	::strncpy(path, _scp_data_disk_path.c_str(), path_len);
	path_len = _scp_data_disk_path.size();

	ACS_ACA_TRACE_MESSAGE("Returned path for %s is: '%s' (length = %d)", ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME, path, path_len);
	return 0;
}

int __CLASS_NAME__::get_data_disk_path (unsigned int cp_id, char * path, int & path_len) {
	ACS_ACA_TRACE_FUNCTION;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_configuration_sync);

	// Check if the path for the given CP has been already loaded
	std::map<unsigned int, std::string>::iterator it = _mcp_data_disk_paths_map.find(cp_id);

	// If the path hasn't been loaded yet, ask APGCC library for it
	if (it == _mcp_data_disk_paths_map.end()) {
		ACS_APGCC_CommonLib common_lib;
		char tmp_path[path_len];

		ACS_ACA_TRACE_MESSAGE("The info is not available, calling 'GetDataDiskPathForCP' to get the %s path for cp_id == %u",
				ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME, cp_id);

		ACS_APGCC_DNFPath_ReturnTypeT return_code = common_lib.GetDataDiskPathForCp(ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME, cp_id, tmp_path, path_len);
		if (return_code != ACS_APGCC_DNFPATH_SUCCESS) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'GetDataDiskPathForCp' (cp_id == %u) failed, "
					"return_code = %d, returning -1!", cp_id, return_code);
			return -1;
		}

		_mcp_data_disk_paths_map[cp_id] = tmp_path;
	}

	::strncpy(path, _mcp_data_disk_paths_map[cp_id].c_str(), path_len);
	path_len = _mcp_data_disk_paths_map[cp_id].size();

	ACS_ACA_TRACE_MESSAGE("Returned path for %s (cp_id == %u) is: '%s' (length = %d)",
			ACS_ACA_DATA_DISK_PATH_LOGICAL_NAME, cp_id, path, path_len);
	return 0;
}

void __CLASS_NAME__::force_configuration_reload () {
	ACS_ACA_TRACE_FUNCTION;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_configuration_sync);
	_scp_info_already_loaded = false;
	_scp_data_disk_path.clear();
	_mcp_data_disk_paths_map.clear();
}
