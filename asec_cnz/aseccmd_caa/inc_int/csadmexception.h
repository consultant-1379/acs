#ifndef CSADMEXCEPTION_H
#define CSADMEXCEPTION_H
#include <string>

#define ERROR_INT_EXECOMMAND "Internal error: Could not execute command."
#define ERROR_INT_SVCMGR "Internal error: Could not interact with service manager."
#define ERROR_INT_CREATE_PROCESS "Internal error: Could not create process."
#define ERROR_INT_CREATE_FILE "Internal error: Could not create file."
#define ERROR_INT_APHOME_MISSING "Internal error: Environment variable AP_HOME is missing."
#define ERROR_INT_TEMP_MISSING "Internal error: Environment variable TEMP is missing."
#define ERROR_INT_UNKNOWN "Internal error: unknown error."
#define ERROR_INT_FILEOPEN "Internal error: Could not open file."
#define ERROR_INT_FILEPARSE "Internal error: Could not parse file."
#define ERROR_INT_FILECREATE "Internal error: Could not create file."
#define ERROR_INT_FILEWRITE "Internal error: Could not write to file."
#define ERROR_INT_ASSIGNPOLICY "Internal error: Cannot assign IPsec policy. Please try again later."
#define ERROR_INT_CLUSTERCOMMAND "Internal error: Cannot execute cluster command."
#define ERROR_INT_CLUSTERCOMMAND_PUBLIC2 "Internal error: Cannot execute cluster command on Public_2 resource."
#define ERROR_INT_PARSECLUSTER0 "Internal error: Cannot fetch cluster IP address."
#define ERROR_INT_PARSECLUSTER0_PUBLIC2 "Internal error: Cannot fetch cluster IP address on Public_2 resource."
#define ERROR_INT_PARSECLUSTER1 "Internal error: Could not find cluster IP address."
#define ERROR_INT_PARSECLUSTER1_PUBLIC2 "Internal error: Could not find cluster IP address on Public_2 resource."
#define ERROR_INT_PARSECLUSTER2 "Internal error: Cannot fetch node status."
#define ERROR_INT_PARSECLUSTER2_PUBLIC2 "Internal error: Cannot fetch node status on Public_2 resource."
#define ERROR_INT_PARSECLUSTER3 "Internal error: Cannot fetch public IP addresses."
#define ERROR_INT_PARSECLUSTER3_PUBLIC2 "Internal error: Cannot fetch public IP addresses on Public_2 resource."
#define ERROR_INT_VERIFYRULE0 "Internal error: Cannot verify status of ipsec rule."
#define ERROR_INT_VERIFYRULE1 "Internal error: Cannot verify filterlist name in ipsec rule."
#define ERROR_INT_VERIFYRULE2 "Internal error: Cannot verify filteraction name in ipsec rule."
#define ERROR_INT_VERIFYFILTERACTION "Internal error: Cannot verify filteraction."
#define ERROR_INT_VERIFYFILTERLIST "Internal error: Cannot verify number of filters in filterlist."

#define ERROR_CRIT_IPSECNOTRUNNING "IPSec Service is not running on node "
#define ERROR_CRIT_NETSH "Cannot execute netsh."
#define ERROR_CRIT_NOPUBLICINTERFACE "Cannot verify IP addresses of network Public."
#define ERROR_CRIT_NOPUBLICINTERFACE2 "Cannot verify IP addresses of network Public_2."
#define ERROR_CRIT_OTHERPOLICY "Conflicting policy is assigned."
#define ERROR_CRIT_IPSEC_CONTEXT "IPsec context is not available."
#define ERROR_CRIT_CLUSTERCONTROL "Cannot access cluster control."
#define ERROR_CRIT_CLUSTERCONTROL_PUBLIC2 "Cannot access cluster control on Public_2 resource."
#define ERROR_REC_SECCONFINVALID "Security configuration invalid."

using namespace std;


class CsadmException
{
public:
	enum errorCode
	{
		eInternal = 1,
		eUseage = 2,
		eRecOther = 6,
		eCritIpsecService = 10,
		eCritNetsh = 11,
		eCritPublInterface = 12,
		eCritPubl2Interface = 13,
		eCritCreatePolicyContent = 14,
		eCritOtherPolicyAssign = 15,
		eCritDsQuery = 16,
		eCritIpsecContext = 18,
		eCritClusterControl = 19,
		eCritClusterControl2 = 20
	} ec;


	CsadmException(errorCode code, string message );
	errorCode getCode();
	const string getMessage();


private:
	errorCode m_code;
	string m_message;

};

#endif
