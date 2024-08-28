#include <iostream>
#include <iostream>
#include <fstream>
#include <grp.h>
#include <pwd.h>
#include "acs_apgcc_paramhandling.h"
#include "sec/crypto_status.h"
#include "sec/crypto_api.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Guard_T.h"
#include "ace/Condition_Attributes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_uai_service_provider.h"
#include "ACS_APGCC_CommonUtil.h"

ACE_Recursive_Thread_Mutex apgccMutex;

const char PSAClearFilePath[] = "/usr/share/pso/storage-paths/clear";
const char ApgccFolder[] = "acs-apgccbin";
const char DirDelim ='/';

std::string ACS_APGCC_CommonUtil::_password;
struct pam_response * ACS_APGCC_CommonUtil::_custom_resp = 0;
const char * const ACS_APGCC_CommonUtil::ACS_APGCC_APOS_CACHE_CREDS_DURATION_FILE_PATH = "/storage/system/config/apos/cached_creds_duration";
const char * const ACS_APGCC_CommonUtil::ACS_APGCC_AUTHZ_CACHE_VALIDITY_TIME_FILE_PATH = "/opt/ap/acs/conf/acs_apgcc_authorization_cache_validity_time";
ACE_Recursive_Thread_Mutex ACS_APGCC_CommonUtil::_authorization_cache_sync;
int ACS_APGCC_CommonUtil::_authorization_cache_duration = -1;
ACS_APGCC_CommonUtil::authorization_cache_map_t ACS_APGCC_CommonUtil::_authorization_cache_map;

ACS_APGCC_CommonUtil::ACS_APGCC_CommonUtil()
{
    //Initialize the log object
    log.Open("APGCC");

}

ACS_APGCC_CommonUtil::~ACS_APGCC_CommonUtil()
{
	log.Close();
}

std::string ACS_APGCC_CommonUtil::decryptString(const std::string &strPwdCrypt)
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	char * plaintext = NULL;
	std::string ciphertext = strPwdCrypt;
	std::string strPwdDecrypt("");
	SecCryptoStatus decryptStatus;

	if(!strPwdCrypt.empty())
	{
		decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
		if(decryptStatus != SEC_CRYPTO_OK )
		{
			::snprintf(log_buffer, log_buffer_size, "Decryption failed");
			log.Write(log_buffer, LOG_LEVEL_ERROR);
		}else{
			strPwdDecrypt = plaintext;
		}
		if(plaintext !=0 )
			free(plaintext);
	}

	return strPwdDecrypt;
}


void ACS_APGCC_CommonUtil::loadCachedUser ()
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	initRoleUserMap();

	try
	{
		std::ifstream file(_roles_file_map.c_str());
		boost::archive::binary_iarchive ia(file);
		ia >> m_rolesMap;
	}
	catch (boost::archive::archive_exception& e)
	{
		::snprintf(log_buffer, log_buffer_size, "Serialization exception occurred: %s.", e.what());
		log.Write(log_buffer, LOG_LEVEL_ERROR);
	}
	catch (boost::exception& )
	{
		::snprintf(log_buffer, log_buffer_size, "Serialization exception occurred.");
		log.Write(log_buffer, LOG_LEVEL_ERROR);
	}
}

void ACS_APGCC_CommonUtil::saveCachedUser (const userRoleMapData_t &map)
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	try
	{
		std::ofstream ofs (_roles_file_map.c_str(), ios::binary);
		boost::archive::binary_oarchive oa(ofs);
		oa << map;
	}
	catch (boost::archive::archive_exception& e)
	{
		::snprintf(log_buffer, log_buffer_size, "Serialization exception occurred: %s.", e.what());
		log.Write(log_buffer, LOG_LEVEL_ERROR);
	}
	catch (boost::exception& )
	{
		::snprintf(log_buffer, log_buffer_size, "Serialization exception occurred.");
		log.Write(log_buffer, LOG_LEVEL_ERROR);
	}

}

void ACS_APGCC_CommonUtil::addRoleEntry (UserRoleMapData data)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	 //load cached user map
	 loadCachedUser();

	//check if the user is already present in the map
	removeRoleEntry(data.m_userName);

	//insert into the map
	m_rolesMap.insert(userRoleMapData_t::value_type(data.m_userName, data));

	//remove the older one in case the limit is reached
	cleanRoleUserMap();

	//store on disk
	saveCachedUser(m_rolesMap);

	//List of cached users stored
	//printCachedUser();
}

void ACS_APGCC_CommonUtil::removeRoleEntry (string userName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	userRoleMapData_t::iterator it = m_rolesMap.find(userName);
	if (it != m_rolesMap.end())
	{
		m_rolesMap.erase(it);
	}
}

void ACS_APGCC_CommonUtil::printCachedUser ()
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	for (userRoleMapData_t::iterator itr = m_rolesMap.begin(); itr != m_rolesMap.end(); itr++)
	{
		ACS_APGCC_CommonUtil::UserRoleMapData data = itr->second;
		::snprintf(log_buffer, log_buffer_size, "User[%s] - Roles[%s] - Time[%ld]", data.m_userName.c_str(), decryptString(data.m_roles).c_str(),data.m_date);
		log.Write(log_buffer, LOG_LEVEL_INFO);
	}
}

ACS_APGCC_Roles_ReturnTypeT ACS_APGCC_CommonUtil::getRolesfromMapUser(const string userName, string& roles)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	//load cached user map
	loadCachedUser();

	userRoleMapData_t::iterator it = m_rolesMap.find(userName);
	if (it != m_rolesMap.end())
	{
		ACS_APGCC_CommonUtil::UserRoleMapData data = it->second;
		roles = decryptString(data.m_roles);
		return ACS_APGCC_ROLES_SUCCESS;
	}

	return ACS_APGCC_ERR_ERROR;
}

void ACS_APGCC_CommonUtil::cleanRoleUserMap ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> apgccGuard(apgccMutex);

	if (m_rolesMap.size() > 2000)
	{
		userRoleMapData_t::iterator it_r = m_rolesMap.begin();
		ACS_APGCC_CommonUtil::UserRoleMapData data_r = it_r->second;
		string user_r = data_r.m_userName;

		for (userRoleMapData_t::iterator it = m_rolesMap.begin(); it != m_rolesMap.end(); it++)
		{
			ACS_APGCC_CommonUtil::UserRoleMapData data = it->second;
			if (data.m_date < data_r.m_date)
			{
				user_r = data.m_userName;
			}
		}

		removeRoleEntry(user_r);
	}
}

string ACS_APGCC_CommonUtil::getClearPath()
{
	string clearDataPath("");

	int bufferLength;
	ifstream clearFileStream;
	// open the clear file
	clearFileStream.open(PSAClearFilePath, ios::binary );

	// check for open error
	if(clearFileStream.good())
	{
		// get length of stored path:
		clearFileStream.seekg(0, ios::end);
		bufferLength = clearFileStream.tellg();
		clearFileStream.seekg(0, ios::beg);

		// allocate the buffer
		char buffer[bufferLength+1];
		ACE_OS::memset(buffer, 0, bufferLength+1);

		// read data
		clearFileStream.read(buffer, bufferLength);

		if(buffer[bufferLength-1] == '\n') buffer[bufferLength-1] = 0;

		clearDataPath = buffer;
		clearDataPath += DirDelim;
	}

	clearFileStream.close();

	return clearDataPath;
}

void ACS_APGCC_CommonUtil::initRoleUserMap ()
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	string apgccFolderPath = getClearPath() + DirDelim + ApgccFolder;

	if(! boost::filesystem::exists(apgccFolderPath) )
	{
		::snprintf(log_buffer, log_buffer_size, "Folder does not exist [%s]", apgccFolderPath.c_str());
		log.Write(log_buffer, LOG_LEVEL_INFO);

		// create the folder
		boost::filesystem::create_directory(apgccFolderPath);
	}

	_roles_file_map = apgccFolderPath + DirDelim + "rolesMap";
}

std::string ACS_APGCC_CommonUtil::encryptString(const std::string& inputString)
{
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	char * plaintext = NULL;
	std::string ciphertext = inputString;
	std::string strEncrypt("");
	SecCryptoStatus encryptStatus;

	if(!inputString.empty())
	{
		encryptStatus = sec_crypto_encrypt_ecimpassword(&plaintext, ciphertext.c_str());
		if(encryptStatus != SEC_CRYPTO_OK )
		{
			::snprintf(log_buffer, log_buffer_size, "Encryption failed");
			log.Write(log_buffer, LOG_LEVEL_ERROR);
		}else{
			strEncrypt = plaintext;
		}
		if(plaintext !=0 )
			free(plaintext);
	}

	return strEncrypt;
}


bool ACS_APGCC_CommonUtil::getUserRoles(const std::string userName, std::string & roles) {
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};

	/*
	 * Check if the roles for the given user have been already retrieved few seconds ago.
	 * This is needed since SEC-ACS API doesn't provide good performances when too many
	 * authZ calls are performed in few seconds.
	 */
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_authorization_cache_sync);
	if (retrieveRolesFromCacheMap(userName, roles)) {
		::snprintf(log_buffer, log_buffer_size, "Retrieved roles from the internal cache for user '%s': %s", userName.c_str(), roles.c_str());
		log.Write(log_buffer, LOG_LEVEL_DEBUG);
		return true;
	}

	// No roles have been found into internal map (new user or too much time elapsed), let's make an authZ call
	AcsStatus call_result = ACS_OK;

	// Create an API session
	AcsHandle handle = 0;
	AcsVersion version = {'A', 1, 1};
	if ((call_result = acs_uai_initialize(&handle, &version)) != ACS_OK) {
		::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_initialize' failed with error_code == %d.", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);
		return false;
	}

	// Start an authorization session
	AcsUserSession trusted_user_session = 0;
	if ((call_result = acs_uai_user_session_start(handle, userName.c_str(), &trusted_user_session)) != ACS_OK) {
		::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_user_session_start' failed with error_code == %d.", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);

		// Release all allocated data in case of errors
		acs_uai_finalize(&handle);
		return false;
	}

	// Retrieve the number of roles associated to the given user
	size_t role_count = 0;
	if ((call_result = acs_uai_user_data_role_count_get(trusted_user_session, &role_count)) != ACS_OK) {
		::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_user_data_role_count_get' failed with error_code == %d.", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);

		// Release all allocated data in case of errors
		acs_uai_user_session_stop(&trusted_user_session);
		acs_uai_finalize(&handle);
		return false;
	}
	::snprintf(log_buffer, log_buffer_size, "The user '%s' has %lu roles!", userName.c_str(), role_count);
	log.Write(log_buffer, LOG_LEVEL_DEBUG);

	// Retrieve the complete list of roles associated to the given user
	char * one_role = 0;
	for (size_t i = 0; i < role_count; ++i) {
		if ((call_result = acs_uai_user_data_role_get(trusted_user_session, i, &one_role)) != ACS_OK) {
			::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_user_data_role_get' failed with error_code == %d.", call_result);
			log.Write(log_buffer, LOG_LEVEL_ERROR);

			// Release all allocated data in case of errors
			acs_uai_user_session_stop(&trusted_user_session);
			acs_uai_finalize(&handle);
			return false;
		}
		::snprintf(log_buffer, log_buffer_size, "Retrieved role for user '%s': %s", userName.c_str(), one_role);
		log.Write(log_buffer, LOG_LEVEL_DEBUG);

		// Build the string of roles as a comma-separated list of strings
		if (roles.size() > 0)	roles += ",";
		roles += one_role;

		// Role returned by SEC ACS API must be deallocated with free()
		::free(one_role);
		one_role = 0;
	}

	// Open the Cache Credentials configuration file in order to understand if the feature is enabled or not
	bool cache_credentials_enabled = false;
	std::ifstream input_stream;
	input_stream.open(ACS_APGCC_APOS_CACHE_CREDS_DURATION_FILE_PATH, std::ios::in);
	if (input_stream.is_open()) {
		// The file has been correctly opened: check the value stored in it (the duration value)
		int cache_duration;
		input_stream >> cache_duration;

		// The cache duration value is greater than zero: Cache Credentials feature is enabled!
		if (cache_duration > 0)
			cache_credentials_enabled = true;
	}
	input_stream.close();

	// If the Cached Credentials feature is enabled, also authorization information must be cached on file system.
	// Otherwise, only a cleanup of internal roles map must be performed.
	if (cache_credentials_enabled) {
	    ACS_APGCC_CommonUtil::UserRoleMapData data;
	    data.m_userName = userName;
	    data.m_roles = encryptString(roles);
	    time_t currTime;
	    time (&currTime);
	    data.m_date = currTime;
	    addRoleEntry(data);
	}
	else
		cleanRoleUserMap();

    bool return_code = true;

    // Finalize the authorization session
	if ((call_result = acs_uai_user_session_stop(&trusted_user_session)) != ACS_OK) {
		::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_user_session_stop' failed with error_code == %d.", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);
		// DO NOT RETURN: finalization must be completed!!!
		return_code = false;
	}

	// Finalize the API session
	if ((call_result = acs_uai_finalize(&handle)) != ACS_OK) {
		::snprintf(log_buffer, log_buffer_size, "Call 'acs_uai_finalize' failed with error_code == %d.", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);
		// DO NOT RETURN: finalization must be completed!!!
		return_code = false;
	}

	/*
	 *  Store user roles into the internal map, in order to make faster
	 *  following calls to authZ process.
	 *  This is needed because SEC-ACS API is not providing good
	 *  performances when many authZ calls are performed in few seconds.
	 */
	storeRolesIntoCacheMap(userName, roles);
	return return_code;
}


ACS_APGCC_Roles_ReturnTypeT ACS_APGCC_CommonUtil::authenticate(const char * service, const char * user, const char * pass) {
	const size_t log_buffer_size = 1024;
	char log_buffer[log_buffer_size] = {0};
	int call_result = PAM_SUCCESS;
	pam_handle_t * pam_handle = 0;
	struct pam_conv pam_conversation;
	pam_conversation.conv = custom_conv;
	pam_conversation.appdata_ptr = 0;

	// Start a PAM conversation to authenticate the user with the provided credentials
	if ((call_result = pam_start(service, user, &pam_conversation, &pam_handle)) != PAM_SUCCESS) {
		::snprintf(log_buffer, log_buffer_size, "Call 'pam_start' failed with error_code == %d", call_result);
		log.Write(log_buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_ERR_ERROR;
	}

	// Set the global password variable in order to make it available to the callback function
	_password = pass;

	// Authenticate the user
	if ((call_result = pam_authenticate(pam_handle, 0)) != PAM_SUCCESS) {
		::snprintf(log_buffer, log_buffer_size, "Authentication failed for user '%s' (error_message == '%s')", user, pam_strerror(pam_handle, call_result));
		log.Write(log_buffer, LOG_LEVEL_ERROR);
	}
	else {
		::snprintf(log_buffer, log_buffer_size, "Authentication successful for user '%s'", user);
		log.Write(log_buffer, LOG_LEVEL_DEBUG);
	}

	// Stop the PAM conversation and clear the stored password
	pam_end(pam_handle, PAM_SUCCESS);
	_password.clear();
    return (call_result == PAM_SUCCESS) ? ACS_APGCC_ROLES_SUCCESS : ACS_APGCC_ERR_NOUSER;
}


int ACS_APGCC_CommonUtil::custom_conv (int num_msg, const struct pam_message ** msg, struct pam_response ** resp, void * /*appdata_ptr*/) {
	// Loop over all the received messages (also if the value of num_msg should be always 1
	for (int i = 0; i < num_msg; ++i) {
		// Consider only ECHO_OFF messages (authentication messages) skipping all the remaining ones
		if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF) {
			// NOTE: Don't take care of free()ing the memory associated to the next two variables: PAM will take care of it!
			_custom_resp = (struct pam_response *)calloc(1, sizeof(struct pam_response));
			_custom_resp->resp = ::strdup(_password.c_str());
			_custom_resp->resp_retcode = 0;
		}
		else
			_custom_resp = 0;
	}

	*resp = _custom_resp;
	return 0;
}

bool ACS_APGCC_CommonUtil::retrieveRolesFromCacheMap (const std::string & username, std::string & roles) {
	// Only the first time, read from the file system the duration of the authorization cache
	if (_authorization_cache_duration < 0) {
		std::ifstream input_stream;
		input_stream.open(ACS_APGCC_AUTHZ_CACHE_VALIDITY_TIME_FILE_PATH, std::ios::in);
		if (input_stream.is_open()) {
			// The file has been correctly opened: check the value stored in it (the duration value)
			int authorization_cache_duration;
			input_stream >> authorization_cache_duration;

			// Assume valid only positive values (or zero)
			if (authorization_cache_duration >= 0)	_authorization_cache_duration = authorization_cache_duration;
		}
		input_stream.close();

		const size_t log_buffer_size = 1024;
		char log_buffer[log_buffer_size] = {0};
		::snprintf(log_buffer, log_buffer_size, "The authorization cache will be valid for '%d' seconds!", _authorization_cache_duration);
		log.Write(log_buffer, LOG_LEVEL_DEBUG);
	}

	// Look for the entry for the given username in the cache map: if not found, authZ must be performed
	authorization_cache_map_t::const_iterator cache_item;
	if ((cache_item = _authorization_cache_map.find(username)) == _authorization_cache_map.end())	return false;

	// Valid entry found: check if it's still valid or must be refreshed
	const authorization_cache_info_t & cache_info = cache_item->second;
	const time_t now = ::time(0);
	if ((now - cache_info.first) > _authorization_cache_duration)	return false;

	// Roles are valid, using them!
	roles = cache_info.second;
	return true;
}

void ACS_APGCC_CommonUtil::storeRolesIntoCacheMap (const std::string & username, std::string & roles) {
	// Build the information to be cached and store them in the internal cache map
	authorization_cache_info_t cache_info;
	cache_info.first = ::time(0); cache_info.second = roles;
	_authorization_cache_map[username] = cache_info;
}

/*ACS_APGCC_CommonUtil::parseIPv4andIPv6AddrFromString(...)
 * Method to parse and separate IPv4 and/or IPv6 addresses from single string variable like Cluster IP in NorthBound=1 MO
 * and apos_hwinfo.sh printout
 * inDualStackIPs - Input string containing IPv4 and/or IPv6 addresses comma separated
 * outIPv4Address - Output string containing IPv4 address
 * outIPv6Address - Output string containing IPv6 address
 * stackType      - Output variable containing stack type of the node
 */
void ACS_APGCC_CommonUtil::parseIPv4andIPv6AddrFromString(const std::string inDualStackIPs, std::string& outIPv4Address, std::string& outIPv6Address, ACS_APGCC_IpAddress_StackT& stackType)
{
	size_t posDot = inDualStackIPs.find('.');
	size_t posColon = inDualStackIPs.find(':');
	size_t posDelim = inDualStackIPs.find(',');
	stackType = ACS_APGCC_DEFAULT_STACK;
	if(posDelim == string::npos)
	{
		if((posDot != string::npos) && (posColon == string::npos))		// IPv4 address only
		{
			stackType = ACS_APGCC_IPv4_STACK;
			outIPv4Address.assign(inDualStackIPs);
		}
		else if((posDot == string::npos) && (posColon != string::npos))		// IPv6 address only
		{
			stackType = ACS_APGCC_IPv6_STACK;
			outIPv6Address.assign(inDualStackIPs);
		}
	}
	else if((posDot != string::npos) && (posColon != string::npos))	// Dual Stack IPs
	{
		stackType = ACS_APGCC_DUAL_STACK;
		if(posDot < posDelim)		//Format is "<IPv4>,<IPv6>"
		{
			outIPv4Address = inDualStackIPs.substr(0,posDelim);
			outIPv6Address = inDualStackIPs.substr(posDelim + 1, inDualStackIPs.length() - posDelim +1);
		}
		else						//Fail-safe logic to handle format "<IPv6>,<IPv4>"
		{
			outIPv6Address = inDualStackIPs.substr(0,posDelim);
			outIPv4Address = inDualStackIPs.substr(posDelim + 1, inDualStackIPs.length() - posDelim +1);
		}
	}
}
