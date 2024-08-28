#ifndef COMMONUTIL_H_
#define COMMONUTIL_H_

#include "ACS_APGCC_CLibTypes.h"
#include "ACS_TRA_Logging.h"
#include "ACS_CC_Types.h"
#include <list>
#include <map>
#include <unordered_map>

#include <security/pam_appl.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>


class ACS_APGCC_CommonUtil
{
private:
	typedef std::pair<time_t, std::string> authorization_cache_info_t;
	typedef std::unordered_map<std::string, authorization_cache_info_t> authorization_cache_map_t;

private:
    static std::string _password;
	static struct pam_response * _custom_resp;
	static const char * const ACS_APGCC_APOS_CACHE_CREDS_DURATION_FILE_PATH;
	static const char * const ACS_APGCC_AUTHZ_CACHE_VALIDITY_TIME_FILE_PATH;
	static ACE_Recursive_Thread_Mutex _authorization_cache_sync;
	static int _authorization_cache_duration;
    static authorization_cache_map_t _authorization_cache_map;

    std::string encryptString(const std::string& inputString);
    std::string decryptString(const std::string &strPwdCrypt);

	static int custom_conv (int num_msg, const struct pam_message ** msg, struct pam_response ** resp, void * appdata_ptr);

    ACS_TRA_Logging log;

public:

    class UserRoleMapData
    {
    public:
    	// Data Members for Class Attributes
    	std::string m_userName;

    	std::string m_roles;

    	time_t m_date;

    	// Additional Public Declarations

    protected:
    	// Additional Protected Declarations

    private:
    	// Additional Private Declarations
    	friend class boost::serialization::access;
    	template <typename Archive> void serialize(Archive &ar, const unsigned int /*version*/) {
    		ar & m_userName;
    		ar & m_roles;
    		ar & m_date;
    	}

    };

    typedef std::map<std::string, ACS_APGCC_CommonUtil::UserRoleMapData> userRoleMapData_t;

    ACS_APGCC_CommonUtil();

    virtual ~ACS_APGCC_CommonUtil();

    ACS_APGCC_Roles_ReturnTypeT getRolesfromMapUser(const std::string userName, std::string& roles);

    bool getUserRoles(const std::string userName, std::string& roles);

    ACS_APGCC_Roles_ReturnTypeT authenticate(const char * service, const char * user, const char * pass);

    static void parseIPv4andIPv6AddrFromString(const std::string inDualStackIPs, std::string& outIPv4Address, std::string& outIPv6Address, ACS_APGCC_IpAddress_StackT& outStackType);

private:

    void loadCachedUser ();

    void saveCachedUser (const userRoleMapData_t &map);

    void addRoleEntry (UserRoleMapData data);

    void removeRoleEntry (std::string userName);

    void printCachedUser ();

    void cleanRoleUserMap ();

    std::string getClearPath();

    void initRoleUserMap ();

    bool retrieveRolesFromCacheMap (const std::string & username, std::string & roles);

    void storeRolesIntoCacheMap (const std::string & username, std::string & roles);

    userRoleMapData_t m_rolesMap;

    std::string _roles_file_map;
}; // class


#endif /* COMMONUTIL_H_ */
