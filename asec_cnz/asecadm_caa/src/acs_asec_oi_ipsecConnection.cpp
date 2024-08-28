
 /* * @file acs_asec_oi_ipsecConnection.cpp
 *	@brief
 *	Class method implementation for ACS_ASEC_OI_SimpleFile.
 *
 *  This module contains the implementation of class declared in
 *  the acs_asec_oi_ipsecConnection.h module
 *
 *	@author enungai (Nunziante Gaito)
 *	@date 2019-03-22
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2010
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2019-03-22 | enungai      | File created.                       |
 *	+========+============+==============+=====================================+
 *      | 2.0.0  | 2020-04-23 | xsowpol      |leftsubnet and rightsubnet parameters|
 *      |	 |	      |		     |are included in the conf file.	   |
 *      +========+============+==============+=====================================+
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_asec_oi_ipsecConnection.h"

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "ACS_APGCC_Util.H"

#include "acs_apgcc_paramhandling.h"

#include <fstream>
#include <boost/filesystem.hpp>

#include <boost/format.hpp>
#include <syslog.h>
#include <sstream>
#include <list>

using namespace boost::filesystem;

namespace ipsecConnectionClass{
    const char ImmImplementerName[] = "ASEC_OI_IpsecConnection";
}

ACS_ASEC_OI_IpsecConnection* ACS_ASEC_OI_IpsecConnection::Ipsecinstance = NULL;

/*============================================================================
	ROUTINE: ACS_ASEC_OI_IpsecConnection
 ============================================================================ */
ACS_ASEC_OI_IpsecConnection::ACS_ASEC_OI_IpsecConnection() :
	acs_apgcc_objectimplementerinterface_V3(ipsecConnectionClass::ImmImplementerName)
{
	acs_asec_oi_ipsecConnectionTrace = new (std::nothrow) ACS_TRA_trace("ACS_ASEC_OI_IpsecConnection");
	ASEC_Log.Open("ASECBIN");
}

/*============================================================================
        ROUTINE: ~ACS_ASEC_OI_IpsecConnection
 ============================================================================ */

ACS_ASEC_OI_IpsecConnection::~ACS_ASEC_OI_IpsecConnection()
{
	ASEC_Log.Close();

	if(NULL != acs_asec_oi_ipsecConnectionTrace)
		delete acs_asec_oi_ipsecConnectionTrace;
}

ACS_ASEC_OI_IpsecConnection* ACS_ASEC_OI_IpsecConnection::getInstance()
{
	if(!Ipsecinstance)
		Ipsecinstance = new ACS_ASEC_OI_IpsecConnection ();

	return Ipsecinstance;
}

/* ikev2_vector:
 * 0-connectionName
 * 1-type
 * 2-remoteIP;		//right or rightsourceip
 * 3-lifetime
 * 4-ikeEncryptAlg
 * 5-ikelifetime
 * 6-protocol
 * 7-ipsecParam
 * 8-remoteVirtualIP
 * 9-key		//:RSA
 * 10-certlinkname          //leftcert
 * 11-leftId                //leftid
 * 12-remoteCertDn          //rightid
 * 13-certprivate
 * 14-leftsubnet
 * 15-rightsubnet
 */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::makeIkev2Connection(vector<string> ipsec_vector,int v_nodeCredentialId,int v_trustCategoryId)
{
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering makeIkev2Connection()",LOG_LEVEL_INFO);
	string v_authentication,v_ipsecConnectionId,v_type,v_remoteIP,v_lifetime,v_ikelifetime,v_protocol,v_remoteCertDn,v_key,v_remoteVirtualIP,v_ikeEncryptAlg,v_ipsecParam,v_leftSubnet,v_rightSubnet;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	string connectionName,nodeDnName,leftId;
	bool resultId = true;
	bool res = true;
	int subscribeRes = 0;
	stringstream nodeCredentialSubId,trustCategorySubId;
	p_toCacert = "/etc/ipsec.d/cacerts/";
	p_toCerts = "/etc/ipsec.d/certs/";
	p_toPrivate = "/etc/ipsec.d/private/";
	vector<string> ikev2_vector;
	
	//Get AP IP address
	p_nodeIp.str("");
	nodeCredentialSubId.str("");
	trustCategorySubId.str("");
	std::ifstream clusterIpAddress("/etc/cluster/nodes/this/mip/nbi/address", ios::in);
	while(clusterIpAddress >> p_nodeIp.rdbuf());

	v_authentication = ipsec_vector[0];
	v_ipsecConnectionId = ipsec_vector[1];
	v_type = ipsec_vector[2];
	v_remoteIP = ipsec_vector[3];
	v_lifetime = ipsec_vector[4];
	v_ikelifetime = ipsec_vector[5];
	v_protocol = ipsec_vector[6];
	v_remoteCertDn = ipsec_vector[7];
	v_key = ipsec_vector[8];
	v_remoteVirtualIP = ipsec_vector[9];
	v_ikeEncryptAlg = ipsec_vector[10];
	v_ipsecParam = ipsec_vector[11];
	
	connectionName = v_ipsecConnectionId.substr(v_ipsecConnectionId.find("=")+1);
	
	ikev2_vector.push_back(connectionName);
	ikev2_vector.push_back(v_type);
	ikev2_vector.push_back(v_remoteIP);
	ikev2_vector.push_back(v_lifetime);
	ikev2_vector.push_back(v_ikeEncryptAlg);
	ikev2_vector.push_back(v_ikelifetime);
	ikev2_vector.push_back(v_protocol);
	ikev2_vector.push_back(v_ipsecParam);
	ikev2_vector.push_back(v_remoteVirtualIP);
	ikev2_vector.push_back(v_key);

	if(!strcmp(v_authentication.c_str(),"PKI")){
		nodeCredentialSubId << v_nodeCredentialId;
		trustCategorySubId << v_trustCategoryId;
		nodeDnName = "id=1,nodeCredentialId="+nodeCredentialSubId.str()+",certMId=1";
		resultId = getImmAttribute(nodeDnName,"subject",leftId);
		if(resultId != true){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "getImmAttribute failed!");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... getImmAttribute failed",LOG_LEVEL_INFO);
			return ACS_CC_FAILURE;
		}
		//subscribe to secapi
		if(unsubscribeFlag!=1){
			subscribeRes = subscribe(getIdValueFromRdn(nodeCredentialSubId.str()),getIdValueFromRdn(trustCategorySubId.str()));
			if(subscribeRes != -1){
				secDetails details;
				details._secHandle = _secHandle;
				details._tCategory = _tCategory;
				details._trustcategorySubscription = _trustcategorySubscription;
				details._nodecredentialSubscription = _nodecredentialSubscription;
				details.m_isSubscribed = m_isSubscribed;
				secConnDetails.insert({connectionName,details});
				//softlinks create
				res = createSoftLinks(connectionName);
				if(res == true){
					TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Soft links create success!!");
				}
				else{
					TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Soft links create failed!");
					ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Softlinks create failed",LOG_LEVEL_INFO);
					return ACS_CC_FAILURE;
				}
			}
			else{
				TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "subscription to secapi failed!");
				ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... subscription to sec api failed",LOG_LEVEL_INFO);
				return ACS_CC_FAILURE;
			}
		}

		ikev2_vector.push_back(connectionName+"cert");
		ikev2_vector.push_back(leftId);
		ikev2_vector.push_back(v_remoteCertDn);
		ikev2_vector.push_back(connectionName+"private");
		}

	//Calculating leftsubnet and rightsubnet values
	if(v_remoteIP.find(".") != std::string::npos && v_ipsecParam.find(".") != std::string::npos)
	{
		v_leftSubnet = calculateIpv4Subnet(v_ipsecParam);
		v_rightSubnet = calculateIpv4Subnet(v_remoteIP);
	}
	else
	{
		v_leftSubnet = calculateIpv6Subnet(v_ipsecParam);
		v_rightSubnet = calculateIpv6Subnet(v_remoteIP);
	}

	ikev2_vector.push_back(v_leftSubnet);
	ikev2_vector.push_back(v_rightSubnet);

	writeToIpsecConfFiles(v_authentication, ikev2_vector);
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... makeIkev2Connection()",LOG_LEVEL_INFO);
	return result;
}

std::string ACS_ASEC_OI_IpsecConnection::calculateIpv4Subnet(string ipAddress)
{
	string subnetValue = "";
	vector <string> ipv4AddressValues;
	stringstream ipv4Address(ipAddress);
	string ipv4AddressDigits;

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering calculateIpv4Subnet");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Entering calculateIpv4Subnet()",LOG_LEVEL_INFO);

	while(getline(ipv4Address, ipv4AddressDigits, '.')) 
	{
		ipv4AddressValues.push_back(ipv4AddressDigits);
	}
	
	for(int i = 0; i < (ipv4AddressValues.size()-1); i++) 
	{
        	subnetValue = subnetValue + ipv4AddressValues[i] + ".";
	}
	
	subnetValue = subnetValue + "0/24";

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Exiting calculateIpv4Subnet");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Exiting calculateIpv4Subnet()",LOG_LEVEL_INFO);

	return subnetValue;
}

std::string ACS_ASEC_OI_IpsecConnection::calculateIpv6Subnet(string ipAddress)
{
	string subnetValue = "";
	vector <string> ipv6AddressValues;
	vector <string> completeIpv6Address;
	stringstream ipv6Address(ipAddress);
	string ipv6AddressHex;

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering calculateIpv6Subnet");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Entering calculateIpv6Subnet()",LOG_LEVEL_INFO);

	while(getline(ipv6Address, ipv6AddressHex, ':')) 
	{
    		ipv6AddressValues.push_back(ipv6AddressHex);
	}

        for(int i = 0; i < ipv6AddressValues.size(); i++) 
	{
        	if(!ipv6AddressValues[i].empty())
        	{
			completeIpv6Address.push_back(ipv6AddressValues[i]);
        	}
		else
        	{
                	for(int j = 0; j <= (8-ipv6AddressValues.size()); j++)
            		{
            		    	completeIpv6Address.push_back("0000");
	                }
        	}
    	}
   
	for(int s = 0; s < 4; s++)
	{
        	subnetValue = subnetValue + completeIpv6Address[s] + ":";
	}
     
	subnetValue = subnetValue + ":/24";

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Exiting calculateIpv6Subnet");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Exiting calculateIpv6Subnet()",LOG_LEVEL_INFO);

	return subnetValue;
}

bool ACS_ASEC_OI_IpsecConnection::executeCommand(const std::string& command, std::string& output)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering in executeCommand");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering execute Command()",LOG_LEVEL_INFO);

	bool result = false;
	result = true;

	FILE* pipe = popen(command.c_str(), "r");
	if(NULL != pipe)
	{
		char rowOutput[1024]={'\0'};
		while(!feof(pipe))
        {
			// get the cmd output
            if(fgets(rowOutput, 1023, pipe) != NULL)
            {
            	std::size_t len = strlen(rowOutput);
            	// remove the newline
            	if( rowOutput[len-1] == '\n' ) rowOutput[len-1] = 0;

            	output.append(rowOutput);
            }
        }
		// wait cmd termination
		int exitCode = pclose(pipe);
		// get the exit code from the exit status
		result = (WEXITSTATUS(exitCode) == 0);
	}

	TRACE(acs_asec_oi_ipsecConnectionTrace,"result:%s\n, command : %s\n, command output:%s\n", (result ? "TRUE" : "FALSE"), command.c_str(), output.c_str());
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving executeCommand");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Leaving execute command()",LOG_LEVEL_INFO);

	return result;
}

bool ACS_ASEC_OI_IpsecConnection::createSoftLinks(string connectionName)
{
	int result = -1;
	string createCacertsCmd,createCertsCmd,createPrivateCmd;
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering createSoftLinks()",LOG_LEVEL_INFO);

	createCacertsCmd = "/usr/bin/ln -s " + p_fromCacert + " " + p_toCacert + connectionName + "cacert";
	createCertsCmd = "/usr/bin/ln -s " + p_fromCerts + " " + p_toCerts + connectionName + "cert";
	createPrivateCmd = "/usr/bin/ln -s " + p_fromKey + " " + p_toPrivate + connectionName + "private";

	result = system(createCacertsCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Cacerts passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Cacerts failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... create softlinks in Cacerts failed",LOG_LEVEL_INFO);
		return false;
	}
	result = system(createCertsCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Certs passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Certs failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... create softlinks in Certs failed",LOG_LEVEL_INFO);
		return false;
	}
	result = system(createPrivateCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Private passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "create softlinks command in Private failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... create softlinks in private failed",LOG_LEVEL_INFO);
		return false;
	}
	int ret = chdir("/");
	if(ret != 0){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Change directory to root failed");
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving createSoftLinks()",LOG_LEVEL_INFO);
	return true;
}

bool ACS_ASEC_OI_IpsecConnection::deleteSoftLinks(string connectionName)
{
	int result = -1;
	string removeCacertsCmd,removeCertsCmd,removePrivateCmd;

	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering deleteSoftlinks()",LOG_LEVEL_INFO);

	removeCacertsCmd = "/usr/bin/rm -r " + p_toCacert+ connectionName +"cacert";
	removeCertsCmd = "/usr/bin/rm -r " + p_toCerts + connectionName + "cert";
	removePrivateCmd = "/usr/bin/rm -r " + p_toPrivate +connectionName + "private";

	result = system(removeCacertsCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in caCerts passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in caCerts failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... remove links in Cacerts failed",LOG_LEVEL_INFO);
		return false;
	}
	result = system(removeCertsCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in certs passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in certs failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... remove links in certs failed",LOG_LEVEL_INFO);
		return false;
	}
	result = system(removePrivateCmd.c_str());
	if(0 == result){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in private passed with exit status=%d",WEXITSTATUS(result));
	}
	else{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in private failed with exit status=%d",WEXITSTATUS(result));
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... remove links in private failed",LOG_LEVEL_INFO);
		return false;
	}
	int ret = chdir("/");
	if(ret != 0){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Change directory to root failed");
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving deleteSoftLinks()",LOG_LEVEL_INFO);
	return true;
}

/* ikev2_vector:
 * 0-connectionName
 * 1-type
 * 2-remoteIP;          //right or rightsourceip
 * 3-lifetime
 * 4-ikeEncryptAlg
 * 5-ikelifetime
 * 6-protocol
 * 7-ipsecParam
 * 8-remoteVirtualIP
 * 9-key                //:RSA
 * 10-certlinkname          //leftcert
 * 11-leftId                //leftid
 * 12-remoteCertDn          //rightid
 * 13-certprivate
 * 14-leftsubnet
 * 15-rightsubnet
 */

void ACS_ASEC_OI_IpsecConnection::writeToIpsecConfFiles(string authentication, vector<string> ikev2_vector)
{

        //for(int i=0; i<ikev2_vector.size();i++)
        //      syslog(LOG_INFO, "write..........ikev2_vector[%d] = %s",i,ikev2_vector[i].c_str());


	string deletePsk = "sed -i '/PSK/d' /etc/ipsec.secrets";
	string commandOutput;
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering writeToIpsecConfFiles()",LOG_LEVEL_INFO);

	if(strcmp(authentication.c_str(),"PSK") == 0)
		executeCommand(deletePsk, commandOutput);
	ofstream ipsecConfFile("/etc/ipsec.conf",ios::out|ios::app);
	ofstream ipsecSecretsFile("/etc/ipsec.secrets", ios::out|ios::app);
	if(ipsecConfFile.is_open())
	{
		ipsecConfFile << "\nconn " << ikev2_vector[0] << "\n";
		ipsecConfFile << "\ttype=" <<ikev2_vector[1] << "\n";
		ipsecConfFile << "\tright=" << ikev2_vector[2] << "\n";
		ipsecConfFile << "\tlifetime=" << ikev2_vector[3] << "\n";
		ipsecConfFile << "\tike=" << ikev2_vector[4] <<"\n";
		ipsecConfFile << "\tikelifetime=" << ikev2_vector[5] <<"\n";
		ipsecConfFile << "\t" << ikev2_vector[6] <<"\n";
		ipsecConfFile << "\tleft=" << ikev2_vector[7] <<"\n";
		ipsecConfFile << "\tkeyexchange=" << "ikev2" <<"\n";

		if(!strcmp(authentication.c_str(),"PKI")){
			ipsecConfFile << "\tleftcert=" << ikev2_vector[10] << "\n";
			ipsecConfFile << "\tleftid=\"" << ikev2_vector[11] << "\"\n";
			ipsecConfFile << "\trightid=\"" << ikev2_vector[12] << "\"\n";
			ipsecConfFile << "\tleftsubnet=" << ikev2_vector[14] <<"\n";
			ipsecConfFile << "\trightsubnet=" << ikev2_vector[15] << "\n";
		}
		else {
			ipsecConfFile << "\tleftsubnet=" << ikev2_vector[10] <<"\n";
			ipsecConfFile << "\trightsubnet=" << ikev2_vector[11] << "\n";
			ipsecConfFile << "\tleftauth=" << "psk" <<"\n";
			ipsecConfFile << "\trightauth=" << "psk" <<"\n";
		}
		ipsecConfFile << "\tauto=add\n\n";
	}
	if(ipsecSecretsFile.is_open())
	{
		if(!strcmp(authentication.c_str(),"PKI")){		
			ipsecSecretsFile << ": RSA " << ikev2_vector[13]  << "\n";
		}
		else{
			ipsecSecretsFile << ": PSK " << ikev2_vector[9] << "\n";
		}
	}
	ipsecConfFile.close();
	ipsecSecretsFile.close();
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving writeToIpsecConfFiles()",LOG_LEVEL_INFO);
}


void ACS_ASEC_OI_IpsecConnection::clearIkev2Configuration()
{
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering clearIkev2configuration",LOG_LEVEL_INFO);

	ofstream ipsecConfFile,ipsecSecretsFile;
	string removeCacertsCmd,removeCertsCmd,removePrivateCmd;
	//clean conf files

	ipsecConfFile.open("/etc/ipsec.conf",ios::out|ios::trunc);
	ipsecSecretsFile.open("/etc/ipsec.secrets", ios::out|ios::trunc);
	ipsecSecretsFile.close();
	ipsecConfFile.close();

	//clean soft links
	clearAllSoftLinks();

	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving clearIkev2configuration",LOG_LEVEL_INFO);
}

int ACS_ASEC_OI_IpsecConnection::updateIkev2Connections(string connectionName)
{
	OmHandler immHandle;
	int numberOfConnections;
	const size_t buffLength = 255;
	char buff[buffLength+1]={'\0'};
	vector<string> ipsec_vector;
	string v_authentication,v_ipsecConnectionId,v_type,v_remoteIP,v_lifetime,v_ikelifetime,v_protocol,v_remoteCertDn,v_key,v_remoteVirtualIP,v_ikeEncryptAlg,v_ipsecParam;      
	string connStr;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	vector<string> vecObj;
	int v_nodeCredentialId = 0, v_trustCategoryId = 0;
	
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering updateIkev2Connections()",LOG_LEVEL_INFO);

	vector<ACS_APGCC_ImmAttribute *> vecAttr;
	ACS_APGCC_ImmAttribute authentication;
	ACS_APGCC_ImmAttribute ipsecConnectionId;
	ACS_APGCC_ImmAttribute type;
	ACS_APGCC_ImmAttribute remoteIP;
	ACS_APGCC_ImmAttribute lifetime;
	ACS_APGCC_ImmAttribute ikelifetime;
	ACS_APGCC_ImmAttribute protocol;
	ACS_APGCC_ImmAttribute remoteCertDn;
	ACS_APGCC_ImmAttribute key;
	ACS_APGCC_ImmAttribute nodeCredentialId;
	ACS_APGCC_ImmAttribute trustCategoryId;
	ACS_APGCC_ImmAttribute remoteVirtualIP;
	ACS_APGCC_ImmAttribute ikeEncryptAlg;
	ACS_APGCC_ImmAttribute ipsecParam;


	authentication.attrName = const_cast<char*>("authentication");
	ipsecConnectionId.attrName = const_cast<char*>("ipsecConnectionId");
	type.attrName = const_cast<char*>("type");
	remoteIP.attrName = const_cast<char*>("remoteIP");
	lifetime.attrName = const_cast<char*>("lifetime");
	ikelifetime.attrName = const_cast<char*>("ikelifetime");
	protocol.attrName = const_cast<char*>("protocol");
	remoteCertDn.attrName = const_cast<char*>("remoteCertDn");
	key.attrName = const_cast<char*>("key");
	remoteVirtualIP.attrName = const_cast<char*>("remoteVirtualIP");
	ikeEncryptAlg.attrName = const_cast<char*>("ikeEncryptAlg");
	ipsecParam.attrName = const_cast<char*>("ipsecParam");
	nodeCredentialId.attrName = const_cast<char*>("nodeCredentialId");
	trustCategoryId.attrName = const_cast<char*>("trustCategoryId");

	vecAttr.push_back(&authentication);
	vecAttr.push_back(&ipsecConnectionId);
	vecAttr.push_back(&type);
	vecAttr.push_back(&remoteIP);
	vecAttr.push_back(&lifetime);
	vecAttr.push_back(&ikelifetime);
	vecAttr.push_back(&protocol);
	vecAttr.push_back(&remoteCertDn);
	vecAttr.push_back(&key);
	vecAttr.push_back(&remoteVirtualIP);
	vecAttr.push_back(&ikeEncryptAlg);
	vecAttr.push_back(&ipsecParam);
	vecAttr.push_back(&nodeCredentialId);
	vecAttr.push_back(&trustCategoryId);

	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection::updateIkev2Connections(), immHandle.Init failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... immHanlde.Init failed",LOG_LEVEL_INFO);
		return -1;
	}
	result = immHandle.getClassInstances("IpsecConnection",vecObj);
	if ( result != ACS_CC_SUCCESS ){
		snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection::updateIkev2Connections(), immHandle.getClassInstances failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... immHanlde.getClassInstances failed",LOG_LEVEL_INFO);
		return -1;
	}
	numberOfConnections = vecObj.size();
	
	//loop for each object instance
	for ( unsigned int i = 0; i < vecObj.size(); i++){
		ipsec_vector.clear();
		result = immHandle.getAttribute(vecObj[i].c_str(), vecAttr);
		char *tmp = '\0';
		if( result != ACS_CC_SUCCESS ){
			snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection::updateIkev2Connections(), immHandle.getAttribute failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... immHandle.getAttribute failed",LOG_LEVEL_INFO);
			return -1;
		}
		if(authentication.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(authentication.attrValues)));
			v_authentication = tmp;
		}
		if(ipsecConnectionId.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ipsecConnectionId.attrValues)));
			v_ipsecConnectionId = tmp;
		}
		if(type.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(type.attrValues)));
			v_type = tmp;
		}
		if(remoteIP.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteIP.attrValues)));
			v_remoteIP = tmp;
		}
		if(lifetime.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(lifetime.attrValues)));
			v_lifetime = tmp;
		}
		if(ikelifetime.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ikelifetime.attrValues)));
			v_ikelifetime = tmp;
		}
		if(protocol.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(protocol.attrValues)));
			v_protocol = tmp;
		}
		if(remoteCertDn.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteCertDn.attrValues)));
			v_remoteCertDn = tmp;
		}
		if(key.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(key.attrValues)));
			v_key = tmp;
		}
		if(nodeCredentialId.attrValuesNum !=0){
			v_nodeCredentialId = *(reinterpret_cast<int*>(*(nodeCredentialId.attrValues)));
		}
		if(trustCategoryId.attrValuesNum !=0){
			v_trustCategoryId = *(reinterpret_cast<int*>(*(trustCategoryId.attrValues)));
		}
		if(remoteVirtualIP.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteVirtualIP.attrValues)));
			v_remoteVirtualIP = tmp;
		}
		if(ikeEncryptAlg.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ikeEncryptAlg.attrValues)));
			v_ikeEncryptAlg = tmp;
		}
		if(ipsecParam.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ipsecParam.attrValues)));
			v_ipsecParam = tmp;
		}

		ipsec_vector.push_back(v_authentication);
		ipsec_vector.push_back(v_ipsecConnectionId);
		ipsec_vector.push_back(v_type);
		ipsec_vector.push_back(v_remoteIP);
		ipsec_vector.push_back(v_lifetime);
		ipsec_vector.push_back(v_ikelifetime);
		ipsec_vector.push_back(v_protocol);
		ipsec_vector.push_back(v_remoteCertDn);
		ipsec_vector.push_back(v_key);
		ipsec_vector.push_back(v_remoteVirtualIP);
		ipsec_vector.push_back(v_ikeEncryptAlg);
		ipsec_vector.push_back(v_ipsecParam);

		connStr = v_ipsecConnectionId.substr(v_ipsecConnectionId.find("=")+1);
		if(strcmp(connStr.c_str(),connectionName.c_str()) != 0){
			result = makeIkev2Connection(ipsec_vector,v_nodeCredentialId,v_trustCategoryId);
			if(result != ACS_CC_SUCCESS){
				numberOfConnections = -1;
			}
		}
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving updateIkev2Connections()",LOG_LEVEL_INFO);
	return numberOfConnections;
}

/*============================================================================
	ROUTINE: create
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering in create(...) callback");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering create()",LOG_LEVEL_INFO);
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	vector<string> ipsec_vector;
	string v_authentication,v_ipsecConnectionId,v_type,v_remoteIP,v_lifetime,v_ikelifetime,v_protocol,v_remoteCertDn,v_key,v_remoteVirtualIP,v_ikeEncryptAlg,v_ipsecParam;

	int v_nodeCredentialId=0, v_trustCategoryId=0;
	unsubscribeFlag=0;

	UNUSED(oiHandle);
	UNUSED(ccbId);

	TRACE(acs_asec_oi_ipsecConnectionTrace, "create(...), Imm Handle: %d\n  ccbId: %d\n  class Name: %s\n  parent Name: %s\n", (int)oiHandle, (int)ccbId, className, parentname);
	for(size_t idx = 0; attr[idx] != NULL ; idx++){
		if(!ACE_OS::strcmp(attr[idx]->attrName,"authentication")){
			v_authentication = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"ipsecConnectionId")){
			v_ipsecConnectionId = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"type")){
			v_type = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"remoteIP")){
			v_remoteIP = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"lifetime")){
			v_lifetime = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"ikelifetime")){
			v_ikelifetime = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"protocol")){
			v_protocol = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"remoteCertDn")){
			v_remoteCertDn = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"key")){
			v_key = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"nodeCredentialId")){
			v_nodeCredentialId = *reinterpret_cast<int *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"trustCategoryId")){
                        v_trustCategoryId = *reinterpret_cast<int *>(attr[idx]->attrValues[0]);
                }
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"remoteVirtualIP")){
			v_remoteVirtualIP = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"ikeEncryptAlg")){
			v_ikeEncryptAlg = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
		else if(!ACE_OS::strcmp(attr[idx]->attrName,"ipsecParam")){
			v_ipsecParam = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
		}
	}
	ipsec_vector.push_back(v_authentication);
	ipsec_vector.push_back(v_ipsecConnectionId);
	ipsec_vector.push_back(v_type);
	ipsec_vector.push_back(v_remoteIP);
	ipsec_vector.push_back(v_lifetime);
	ipsec_vector.push_back(v_ikelifetime);
	ipsec_vector.push_back(v_protocol);
	ipsec_vector.push_back(v_remoteCertDn);
	ipsec_vector.push_back(v_key);
	ipsec_vector.push_back(v_remoteVirtualIP);
	ipsec_vector.push_back(v_ikeEncryptAlg);
	ipsec_vector.push_back(v_ipsecParam);

	//for(int i=0; i<ipsec_vector.size();i++)
	//	syslog(LOG_INFO, "sowjanya ..create..........ipsec_vector[%d] = %s",i,ipsec_vector[i].c_str());

	result = makeIkev2Connection(ipsec_vector,v_nodeCredentialId,v_trustCategoryId);
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving create(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving create()",LOG_LEVEL_INFO);	
	return result;
}

/*============================================================================
	ROUTINE: deleted
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering deleted(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering deleted()",LOG_LEVEL_INFO);
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);

	string rdn,connectionName,v_authentication;
	OmHandler immHandle;
	int unsubscribeRes = 0;
	bool res = true;
	string objNameStr = objName;
	int pos = objNameStr.find(",");
	rdn = objNameStr.substr(0, pos);
	connectionName = rdn.substr(rdn.find("=")+1);

	res = getImmAttribute (objName, "authentication", v_authentication);
	if(res != true){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s","getImmAttribute() failed!!!");
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... getImmAttribute failed",LOG_LEVEL_INFO);
		return ACS_CC_FAILURE;
	}

	//clear ikve2 configuration
	clearIkev2Configuration();

	if(!strcmp(v_authentication.c_str(),"PKI")){
		//unsubscribe
		secDetails detailsUnSub = secConnDetails.find(connectionName)->second;
		if(isSubscribed(detailsUnSub)){
			unsubscribeFlag=1;
			unsubscribeRes = unsubscribe(detailsUnSub);
			if(unsubscribeRes != -1){
				secConnDetails.erase(connectionName);
			}
			else{
				TRACE(acs_asec_oi_ipsecConnectionTrace, "%s","Unsubscription failed!");
				ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... unsubscription failed",LOG_LEVEL_INFO);
				return ACS_CC_FAILURE;
			}
		}
	}
	//write other objects to configuration files
	int numberOfConnections = updateIkev2Connections(connectionName);
	if(numberOfConnections == -1){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Error while making Ikev2 connections!");
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... Error while making Ikev2 connections",LOG_LEVEL_INFO);
		return ACS_CC_FAILURE;
	}
	TRACE(acs_asec_oi_ipsecConnectionTrace, "delete(...), Imm Handle: %d\n  ccbId: %d\n  object Name:%s\n", (int)oiHandle, (int)ccbId,objName);
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving deleted(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving deleted",LOG_LEVEL_INFO);
	return result;
}

/*============================================================================
	ROUTINE: modify
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering modify(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering modify",LOG_LEVEL_INFO);
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);

 	string rdn,connectionName;
        string objNameStr = objName;
        int pos = objNameStr.find(",");
        rdn = objNameStr.substr(0, pos);
        connectionName = rdn.substr(rdn.find("=")+1);

 	OmHandler immHandle;
        const size_t buffLength = 255;
        char buff[buffLength+1]={'\0'};
        vector<string> ipsec_vector;
        string v_authentication,v_ipsecConnectionId,v_type,v_remoteIP,v_lifetime,v_ikelifetime,v_protocol,v_remoteCertDn,v_key,v_remoteVirtualIP,v_ikeEncryptAlg,v_ipsecParam;
        string connStr;
        vector<string> vecObj;
        int v_nodeCredentialId = 0, v_trustCategoryId = 0;

        if(attrMods[1]){
                ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... modify....multiple attributes cannot be modified at at time", LOG_LEVEL_ERROR);
                syslog(LOG_INFO,"Multiple attributes cannot be modified at a time ");
                return ACS_CC_FAILURE;
        }

        clearIkev2Configuration();

        vector<ACS_APGCC_ImmAttribute *> vecAttr;
        ACS_APGCC_ImmAttribute authentication;
        ACS_APGCC_ImmAttribute ipsecConnectionId;
        ACS_APGCC_ImmAttribute type;
        ACS_APGCC_ImmAttribute remoteIP;
        ACS_APGCC_ImmAttribute lifetime;
        ACS_APGCC_ImmAttribute ikelifetime;
        ACS_APGCC_ImmAttribute protocol;
        ACS_APGCC_ImmAttribute remoteCertDn;
        ACS_APGCC_ImmAttribute key;
        ACS_APGCC_ImmAttribute nodeCredentialId;
        ACS_APGCC_ImmAttribute trustCategoryId;
        ACS_APGCC_ImmAttribute remoteVirtualIP;
        ACS_APGCC_ImmAttribute ikeEncryptAlg;
        ACS_APGCC_ImmAttribute ipsecParam;


        authentication.attrName = const_cast<char*>("authentication");
        ipsecConnectionId.attrName = const_cast<char*>("ipsecConnectionId");
        type.attrName = const_cast<char*>("type");
        remoteIP.attrName = const_cast<char*>("remoteIP");
        lifetime.attrName = const_cast<char*>("lifetime");
        ikelifetime.attrName = const_cast<char*>("ikelifetime");
        protocol.attrName = const_cast<char*>("protocol");
        remoteCertDn.attrName = const_cast<char*>("remoteCertDn");
        key.attrName = const_cast<char*>("key");
        remoteVirtualIP.attrName = const_cast<char*>("remoteVirtualIP");
        ikeEncryptAlg.attrName = const_cast<char*>("ikeEncryptAlg");
        ipsecParam.attrName = const_cast<char*>("ipsecParam");
        nodeCredentialId.attrName = const_cast<char*>("nodeCredentialId");
        trustCategoryId.attrName = const_cast<char*>("trustCategoryId");

        vecAttr.push_back(&authentication);
        vecAttr.push_back(&ipsecConnectionId);
        vecAttr.push_back(&type);
        vecAttr.push_back(&remoteIP);
        vecAttr.push_back(&lifetime);
        vecAttr.push_back(&ikelifetime);
        vecAttr.push_back(&protocol);
        vecAttr.push_back(&remoteCertDn);
        vecAttr.push_back(&key);
        vecAttr.push_back(&remoteVirtualIP);
        vecAttr.push_back(&ikeEncryptAlg);
        vecAttr.push_back(&ipsecParam);
        vecAttr.push_back(&nodeCredentialId);
        vecAttr.push_back(&trustCategoryId);
	result = immHandle.Init();
        if ( result != ACS_CC_SUCCESS ){
                snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection class ...modify, immHandle.Init failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
                TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
                ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...modify,  immHanlde.Init failed",LOG_LEVEL_INFO);
                return ACS_CC_FAILURE;
        }
        result = immHandle.getClassInstances("IpsecConnection",vecObj);
        if ( result != ACS_CC_SUCCESS ){
                snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection class ...modify, immHandle.getClassInstances failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
                TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
                ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...modify, immHanlde.getClassInstances failed",LOG_LEVEL_INFO);
                return ACS_CC_FAILURE;
        }

        //loop for each object instance
        for ( unsigned int i = 0; i < vecObj.size(); i++){
                ipsec_vector.clear();
                result = immHandle.getAttribute(vecObj[i].c_str(), vecAttr);
                char *tmp = '\0';
                if( result != ACS_CC_SUCCESS ){
                        snprintf(buff, buffLength, "ACS_ASEC_OI_IpsecConnection class ...modify, immHandle.getAttribute failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
                        TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", buff);
                        ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...modify .. immHandle.getAttribute failed",LOG_LEVEL_INFO);
                        return ACS_CC_FAILURE;
                }
               if(authentication.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(authentication.attrValues)));
                        v_authentication = tmp;
                }
                if(ipsecConnectionId.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(ipsecConnectionId.attrValues)));
                        v_ipsecConnectionId = tmp;
                }
                if(type.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(type.attrValues)));
                        v_type = tmp;
                }
                if(remoteIP.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(remoteIP.attrValues)));
                        v_remoteIP = tmp;
                }
                if(lifetime.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(lifetime.attrValues)));
                        v_lifetime = tmp;
                }
                if(ikelifetime.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(ikelifetime.attrValues)));
                        v_ikelifetime = tmp;
                }
                if(protocol.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(protocol.attrValues)));
                        v_protocol = tmp;
                }
               if(remoteCertDn.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(remoteCertDn.attrValues)));
                        v_remoteCertDn = tmp;
                }
                if(key.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(key.attrValues)));
                        v_key = tmp;
                }
                if(nodeCredentialId.attrValuesNum !=0){
                        v_nodeCredentialId = *(reinterpret_cast<int*>(*(nodeCredentialId.attrValues)));
                }
                if(trustCategoryId.attrValuesNum !=0){
                        v_trustCategoryId = *(reinterpret_cast<int*>(*(trustCategoryId.attrValues)));
                }
                if(remoteVirtualIP.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(remoteVirtualIP.attrValues)));
                        v_remoteVirtualIP = tmp;
                }
                if(ikeEncryptAlg.attrValuesNum !=0){
                        tmp = (reinterpret_cast<char *>(*(ikeEncryptAlg.attrValues)));
                        v_ikeEncryptAlg = tmp;
                }
               	ipsec_vector.push_back(v_authentication);
                ipsec_vector.push_back(v_ipsecConnectionId);
                ipsec_vector.push_back(v_type);
                ipsec_vector.push_back(v_remoteIP);
                ipsec_vector.push_back(v_lifetime);
                ipsec_vector.push_back(v_ikelifetime);
                ipsec_vector.push_back(v_protocol);
                ipsec_vector.push_back(v_remoteCertDn);
                ipsec_vector.push_back(v_key);
                ipsec_vector.push_back(v_remoteVirtualIP);
                ipsec_vector.push_back(v_ikeEncryptAlg);

                connStr = v_ipsecConnectionId.substr(v_ipsecConnectionId.find("=")+1);
                if(strcmp(connStr.c_str(),connectionName.c_str()) == 0){
       			if(ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"ipsecParam") == 0 )
        		{	
                		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...modify ipsecParam match found", LOG_LEVEL_INFO);
                        	v_ipsecParam = reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]);
				syslog(LOG_INFO,"ACS_ASEC_OI_IpsecConnection class ...modify...conn name is %s, ipsecParam new is %s",connStr.c_str(), v_ipsecParam.c_str());
			}
		}
		else {
			v_ipsecParam =(reinterpret_cast<char *>(*(ipsecParam.attrValues)));
		}	
		ipsec_vector.push_back(v_ipsecParam);
                result = makeIkev2Connection(ipsec_vector,v_nodeCredentialId,v_trustCategoryId);
                }

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving modify(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving modify",LOG_LEVEL_INFO);

	return result;
}

/*============================================================================
	ROUTINE: complete
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering complete(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering complete",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving complete(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving complete",LOG_LEVEL_INFO);

	return result;
}

/*============================================================================
	ROUTINE: abort
 ============================================================================ */
void ACS_ASEC_OI_IpsecConnection::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering abort(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering abort",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving abort(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving abort",LOG_LEVEL_INFO);
}

/*============================================================================
	ROUTINE: apply
 ============================================================================ */
void ACS_ASEC_OI_IpsecConnection::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering apply(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering apply()",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	const string restartStrongswan = "systemctl restart strongswan";

	//restart strongswan
	string commandOutput;
	executeCommand(restartStrongswan, commandOutput);

	TRACE(acs_asec_oi_ipsecConnectionTrace, "command output: %s\n", commandOutput.c_str());
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving apply(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving apply()",LOG_LEVEL_INFO);
}

/*============================================================================
	ROUTINE: updateRuntime
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_IpsecConnection::updateRuntime(const char* p_objName, const char** p_attrName)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace,"%s","Entering in updateRuntime()");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering updateRuntime ",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(p_attrName);
	UNUSED(p_objName);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving updateRuntime");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving updateRuntime",LOG_LEVEL_INFO);

	return result;
}

/*============================================================================
ROUTINE: adminOperationCallback
============================================================================ */
void ACS_ASEC_OI_IpsecConnection::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering adminOperationCallback(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering adminOperationCallback",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(p_objName);
	UNUSED(oiHandle);
	UNUSED(operationId);
	UNUSED(paramList);
	UNUSED(invocation);

	// No actions are defined in CompositeFile class
	//adminOperationResult(oiHandle, invocation, actionResult::NOOPERATION);

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Leaving adminOperationCallback(...)");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving adminOperationCallback",LOG_LEVEL_INFO);
}


bool ACS_ASEC_OI_IpsecConnection::getImmAttribute (std::string object, std::string attribute, std::string &value)
{
	bool res = true;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();

	if (result != ACS_CC_SUCCESS){
		res = false;
	}
	else{
		ACS_CC_ImmParameter Param;
		char *name_attrPath = const_cast<char*>(attribute.c_str());
		Param.attrName = name_attrPath;
		result = omHandler.getAttribute(object.c_str(), &Param );

		if ( result != ACS_CC_SUCCESS ){
			res = false;
		}
		else if (Param.attrValuesNum > 0){
			value = (char*)Param.attrValues[0];
		}
		else{
			res = false;
		}
		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS){
			res = false;
		}
	}
	return res;
}

string ACS_ASEC_OI_IpsecConnection::getIdValueFromRdn(const string & rdn)
{
	string rdnId = rdn;
	size_t index = rdnId.find_first_of(",");

	if (index != string::npos)
		rdnId = rdnId.substr(0, index);

	index = rdnId.find_first_of("=");

	if (index != string::npos)
		rdnId = rdnId.substr(index+1);

	return rdnId;
}

bool ACS_ASEC_OI_IpsecConnection::isSubscribed(secDetails detailsUnSub)
{
	if((detailsUnSub._secHandle == 0) || (detailsUnSub.m_isSubscribed==false))
		return false;

	return true;
}

int ACS_ASEC_OI_IpsecConnection::subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId)
{

	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering subscribe()",LOG_LEVEL_INFO);
	_fd = 0;
	char trace[256] = {0};
	snprintf(trace, sizeof(trace) - 1, "TrustCategory = %s  NodeCredential = %s",trustCategoryId.c_str() ,nodeCredentialId.c_str());
	SecCreduVersion version;
	version.release_code  = 'A';
	version.major_version = 0x01;
	version.minor_version = 0x01;

	// Parameters to be used.
	SecCreduParameters parameters;
	parameters.nodecredential_change_callback = nodeCredentialChangeCallback;
	parameters.trustcategory_change_callback  = trustCategoryChangeCallback;

	// Initialize the API.
	SecCreduStatus retval;

	if (SEC_CREDU_OK == (retval=sec_credu_initialize(&_secHandle, &parameters, &version))){
		// Subscribe
		if (SEC_CREDU_OK != (retval=sec_credu_nodecredential_subscribe(_secHandle, nodeCredentialId.c_str(), &_nodecredentialSubscription))){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Failed to Subscribe to Node Credential(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... subscribe to node credential failed",LOG_LEVEL_INFO);
		}
		else if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_subscribe(_secHandle, trustCategoryId.c_str(), &_trustcategorySubscription))){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Failed to Subscribe to Trust Category(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... subscribe to trust category failed",LOG_LEVEL_INFO);
		}
		else if (SEC_CREDU_OK != (retval=sec_credu_selectionobject_get(_secHandle, &_fd))){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_selectionobject_get failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sed_credu_selectionobject_get failed",LOG_LEVEL_INFO);
		}
		else{
			// success
			m_isSubscribed=true;
			fetchTrustCategory();
			fetchNodeCert();
			_nodeCredentialId=nodeCredentialId;
			_trustCategoryId=trustCategoryId;
		}
	}
	else{
		_secHandle = 0;
		_fd = -1;
		return _fd;
	}
	if(SEC_CREDU_OK != retval){
		//unsubscribe();
		_fd = -1;
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving subscribe ",LOG_LEVEL_INFO);
	return _fd;
}

void ACS_ASEC_OI_IpsecConnection::nodeCredentialChangeCallback(
                SecCreduHandle /*handle*/, SecCreduSubscription /*nodecredential_subscription*/, const char */*nodecredential_id*/)
{
	ACS_ASEC_OI_IpsecConnection *pinstance = ACS_ASEC_OI_IpsecConnection::getInstance();
	if (pinstance){
		pinstance->fetchNodeCert();
		pinstance->_updated=true;
	}
}

void ACS_ASEC_OI_IpsecConnection::trustCategoryChangeCallback(
                SecCreduHandle /*handle*/, SecCreduSubscription /*trustcategory_subscription*/, const char */*trustcategory_id*/)
{
	ACS_ASEC_OI_IpsecConnection *pinstance = ACS_ASEC_OI_IpsecConnection::getInstance();
	if (pinstance){
		pinstance->fetchTrustCategory();
		pinstance->_updated=true;
	}
}

void ACS_ASEC_OI_IpsecConnection::fetchTrustCategory()
{
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering fetchTrustCategory",LOG_LEVEL_INFO);
	bool success = true;

	SecCreduTrustCategory *category = NULL;
	SecCreduStatus retval;

	if((_secHandle == 0)  || (m_isSubscribed==false)){
		return;
	}

	_trustCategory.clear();
	_trustCatDirName.clear();

	char *trustCatDirName = NULL;

	// Fetch information on the TrustCategory instance.
	if (SEC_CREDU_OK != (retval= sec_credu_trustcategory_get(_secHandle, _trustcategorySubscription, &category))){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_trustcategory_get failed(...)");
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_trustcategory_get failed",LOG_LEVEL_INFO);
		success = false;
	}
	else if(SEC_CREDU_OK != (retval= sec_credu_trustcategory_dirname_get(category, &trustCatDirName)))
	{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_trustcategory_dirname_get failed(...)");
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_trustcategory_dirname_get failed",LOG_LEVEL_INFO);
		trustCatDirName = NULL;
		success = false;
	}
	else{
		char* trustedCertFilename = NULL;
		char* trustedCertContent = NULL;
		char* trustedCertId = NULL;
		char* trustedCertId2 = NULL;
		size_t numTrustCategoryCerts=0;

		_trustCatDirName = (trustCatDirName)?std::string(trustCatDirName):std::string();

		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "fetching directory(...)");
		ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... fetching directory",LOG_LEVEL_INFO);

		sec_credu_trustcategory_cert_count_get(category, &numTrustCategoryCerts);

		for (size_t i = 0; i < numTrustCategoryCerts; ++i){
			bool ok = true;
			if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_FILENAME, i,&trustedCertFilename,&trustedCertId)))
			{
				TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_trustcategory_cert_get FILENAME failed(...)");
				ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_trustcategory_cert_get filename failed",LOG_LEVEL_INFO);
				trustedCertFilename=NULL;
				trustedCertId=NULL;
				ok=false;
			}
			if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_PEM, i,&trustedCertContent,&trustedCertId2)))
			{
				TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_trustcategory_cert_get PEM failed(...)");
				ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_trustcategory_cert_get PEM failed",LOG_LEVEL_INFO);

				trustedCertContent=NULL;
				trustedCertId2=NULL;
				ok=false;
			}
			if(ok)
			{
				_tCategory = category;
				_trustCategory.push_back(Certificate(trustedCertFilename, trustedCertContent));
				//for (size_t i = 0; i < numTrustCategoryCerts; ++i) {
				p_fromCacert = _trustCatDirName.c_str();

				if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_FILENAME, i,&trustedCertFilename,&trustedCertId)))
				{
					trustedCertFilename=NULL;
				}
				if(trustedCertFilename != NULL)
				{
					p_fromCacert = p_fromCacert + '/'+ trustedCertFilename;
				}
			}
			else
			{
				success = false;
			}
			
			free(trustedCertFilename);
			free(trustedCertContent);
			free(trustedCertId);
			free(trustedCertId2);
		}
		free(trustCatDirName);
	}
	if(success)
	{
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "fetching Success(...)");
		return;
	}

	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "fetchTrustCategory failed(...)");
	_trustCategory.clear();
}

void ACS_ASEC_OI_IpsecConnection::fetchNodeCert()
{
	SecCreduStatus retval;
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering fetchNodeCert()",LOG_LEVEL_INFO);

	if((_secHandle != 0) && (m_isSubscribed==true))
	{
		_nodeCredentialCert.clear();
		_nodeCredentialKey.clear();

		char *certContent = NULL;
		char *certFilename = NULL;
		char *keyContent = NULL;
		char *keyFilename = NULL;

		if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle, _nodecredentialSubscription,
				SEC_CREDU_FILENAME, &certFilename)))
		{
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_nodecredential_cert_get FILENAME failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...sec_credu_nodecredential_get FILENAME failed ",LOG_LEVEL_INFO);
		}
		else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_PEM, &certContent)))
		{
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_nodecredential_cert_get PEM failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_nodecredential_cert_get PEM failed ",LOG_LEVEL_INFO);
		}
		else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_FILENAME, &keyFilename)))
		{
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_nodecredential_key_get FILENAME failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...sec_credu_nodecredential_key_get FILENAME failed ",LOG_LEVEL_INFO);
		}
		else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_PEM, &keyContent)))
		{
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_nodecredential_key_get PEM failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... sec_credu_nodecredential_key_get PEM failed",LOG_LEVEL_INFO);
		}

		if(retval == SEC_CREDU_OK)
		{
			if((certFilename != NULL)&&(certContent!=NULL))
			{
				_nodeCredentialCert = Certificate(certFilename, certContent);
			}
			if((keyFilename != NULL)&&(keyContent!=NULL))
			{
				_nodeCredentialKey = Certificate(keyFilename, keyContent);
			}
			
			p_fromCerts = getNodeCertFileName().c_str();
			p_fromKey = getNodeKeyFileName().c_str();
		}
		else
		{
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "fetch node cert failed(...)");
			ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ...fetch node cert failed ",LOG_LEVEL_INFO);
		}
		// Free the allocated memory.
		free(certContent);
		free(certFilename);
		free(keyFilename);
		free(keyContent);
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving fetchNodeCert()",LOG_LEVEL_INFO);
}

std::string ACS_ASEC_OI_IpsecConnection::getNodeKeyFileName() const
{
	return _nodeCredentialKey.getFilename();
}

std::string ACS_ASEC_OI_IpsecConnection::getNodeCertFileName() const
{
	return _nodeCredentialCert.getFilename();
}

std::string ACS_ASEC_OI_IpsecConnection::getTrustCatDirName() const
{
	return _trustCatDirName;
}

int ACS_ASEC_OI_IpsecConnection::unsubscribe(secDetails detailsUnSub)
{
	bool success = true;
	int result = 0;
        SecCreduStatus retval;
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering unsubscribe ",LOG_LEVEL_INFO);
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Entering unsubscribe");
	if((detailsUnSub._secHandle != 0) && (detailsUnSub.m_isSubscribed==true)){
		m_isSubscribed=false;
		_nodeCredentialId.clear();
		_trustCategoryId.clear();
		_nodeCredentialCert.clear();
		_nodeCredentialKey.clear();
		_trustCategory.clear();
		_trustCatDirName.clear();

		// Free the allocated memory.
		if (detailsUnSub._tCategory){
			if ( (SEC_CREDU_OK != (retval = sec_credu_trustcategory_free(detailsUnSub._secHandle, &(detailsUnSub._tCategory))))){			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "sec_credu_trustcategory_free failed");
			return -1;
			}
		}
		if (SEC_CREDU_OK != sec_credu_nodecredential_unsubscribe(detailsUnSub._secHandle, detailsUnSub._nodecredentialSubscription)) success=false;
		_nodecredentialSubscription = 0;
		if (SEC_CREDU_OK != sec_credu_trustcategory_unsubscribe(detailsUnSub._secHandle, detailsUnSub._trustcategorySubscription)) success=false;
		_trustcategorySubscription = 0;
		if (SEC_CREDU_OK != sec_credu_finalize(detailsUnSub._secHandle)) success=false;
		_secHandle = 0;
		if(!success){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "failed to unsubscribe SEC subscription");
			return -1;
		}
	}
	TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Exiting unsubscribe");
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving unsubscribe",LOG_LEVEL_INFO);
	return result;
}

void Certificate::clear()
{
	_pem.clear();
	_filename.clear();
}
std::string ACS_ASEC_OI_IpsecConnection::getTrustCatId() const
{
	return m_tlsCategory;
}
std::string ACS_ASEC_OI_IpsecConnection::getNodeCredId() const
{
	return m_nodeCredential;
}

void ACS_ASEC_OI_IpsecConnection::clearAllSoftLinks()
{
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... entering clearAllSoftLinks",LOG_LEVEL_INFO);
	int result = -1;
	string removeCacertsCmd,removeCertsCmd,removePrivateCmd;
	string toCacertPath = "/etc/ipsec.d/cacerts/";
	string toCertsPath = "/etc/ipsec.d/certs/";
	string toPrivatePath = "/etc/ipsec.d/private/";

	removeCacertsCmd = "/usr/bin/rm -r " + toCacertPath + "*";
	removeCertsCmd = "/usr/bin/rm -r " + toCertsPath + "*";
	removePrivateCmd = "/usr/bin/rm -r " + toPrivatePath + "*";

	path p(toCacertPath);
	if(!is_empty(p)){
		result = system(removeCacertsCmd.c_str());
		if(0 != result){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in caCerts failed with exit status=%d",WEXITSTATUS(result));
		}
	}
	p = toCertsPath;
	if(!is_empty(p)){
		result = system(removeCertsCmd.c_str());
		if(0 != result){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in certs failed with exit status=%d",WEXITSTATUS(result));
		}
	}
	p = toPrivatePath;
	if(!is_empty(p)){
		result = system(removePrivateCmd.c_str());
		if(0 != result){
			TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "remove links command in private failed with exit status=%d",WEXITSTATUS(result));
		}
	}
	int ret = chdir("/");
	if(ret != 0){
		TRACE(acs_asec_oi_ipsecConnectionTrace, "%s", "Change directory to root failed");
	}
	ASEC_Log.Write("ACS_ASEC_OI_IpsecConnection class ... leaving clearAllSoftLinks",LOG_LEVEL_INFO);
}

