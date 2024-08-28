#ifndef ACS_CS_SecApi_h_
#define ACS_CS_SecApi_h_ 1

#include "ace/ACE.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <map>
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_Protocol.h"
#include <sec_credu_api.h>
#include <sec/crypto_status.h>
#include <sec/crypto_api.h>
using namespace ACS_CS_API_NS;


class Certificate
{
	std::string _filename;
	std::string _pem; // base64 encoded
public:
	Certificate() {}
	Certificate(const std::string& filename, const std::string& pem):_filename(filename),_pem(pem) {}



	const std::string& getFilename() const {
		return _filename;
	}


	void clear();
};



class ACS_CS_SecApi  
{
	SecCreduHandle _secHandle;
	SecCreduSubscription _nodecredentialSubscription;
	SecCreduSubscription _trustcategorySubscription;
	SecCreduTrustCategory *_tCategory;
	std::string _nodeCredentialId;
	std::string _trustCategoryId;

	int _fd;
	std::vector<Certificate> _trustCategory;
	Certificate _nodeCredentialCert;
	Certificate _nodeCredentialKey;
	std::string _trustCatDirName;


	//static std::map<SecCreduHandle,ACS_CS_SecApi*> _instances;
	static ACS_CS_SecApi *Secinstance;
	bool _updated;
	ACE_RW_Mutex lock_;

	vector <string> contentsBlock;
	std::string m_baseDn;
	std::string m_ipAddress;
	std::string m_fallBackIpAddress;
	std::string m_tlsClientKey;
	std::string m_tlsClientCert;
	std::string m_tlsCaCert;
	std::string m_bindPassword;
	std::string m_bindDn;


	int m_useTls;
	int m_useReferrals;
	int m_serverPort;

	std::string m_nodeCredential;
	std::string m_tlsCategory;
	std::string m_tlsCaCertDir;


	std::string m_baseDn_old;
	std::string m_ipAddress_old;
	std::string m_fallBackIpAddress_old;
	std::string m_bindPassword_old;
	std::string m_bindDn_old;


	int m_useTls_old;
	int m_useReferrals_old;
	int m_serverPort_old;

	std::string m_nodeCredential_old;
	std::string m_tlsCategory_old;
	std::string m_tlsCaCertDir_old;

	bool m_isLdapConfigurationUpdated;
        bool m_isSubscribed;
	bool m_nodeCredentialisSubscribed;
	bool m_trustCategoryisSubscribed;

public:


	void fetchNodeCert();
	void fetchTrustCategory();
	
	static void nodeCredentialChangeCallback(
			SecCreduHandle handle, SecCreduSubscription nodecredential_subscription, const char *nodecredential_id);

	static void trustCategoryChangeCallback(
			SecCreduHandle handle, SecCreduSubscription trustcategory_subscription, const char *trustcategory_id);

	static ACS_CS_SecApi* getInstance();

	static void finalize();

	ACS_CS_SecApi();
	~ACS_CS_SecApi();


	/**
	 * Returns true if both nodeCredential and trustCategory already subscribed
	 */
	bool isSubscribed() const;
	
	/**
         * Returns true if nodeCredential already subscribed
         */
        bool nodeCredentialisSubscribed() const;

	/**
         * Returns true if trustCategory already subscribed
         */
        bool trustCategoryisSubscribed() const;

	/**
	 * creates a subscription for both nodeCredential and trustCategory
	 * @param nodeCredentialId the InstanceId you are interested in
	 * @param trustCategoryId the InstanceId you are interested in
	 * @return The file descriptor used for asynchronous callbacks, or -1
	 */
	int subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId);
	
	/**
         * creates a subscription for nodeCredential
         * @param nodeCredentialId the InstanceId you are interested in
         * @return The file descriptor used for asynchronous callbacks, or -1
         */
	int nodeCredentialSubscribe(const std::string& nodeCredentialId);
	
	/**
         * creates a subscription for trustCategory
         * @param trustCategoryId the InstanceId you are interested in
         * @return The file descriptor used for asynchronous callbacks, or -1
         */
	int trustCategorySubscribe(const std::string& trustCategoryId);

	/**
	 * creates a subscription for both nodeCredential and trustCategory
	 * @param nodeCredentialMoRef the Mo Ref you are interested in
	 * @param trustCategoryMoRef the Mo Ref you are interested in
	 * @return The file descriptor used for asynchronous callbacks, or -1
	 */
	int subscribeMoRef(const std::string& nodeCredentialMoRef, const std::string& trustCategoryMoRef);
	
	/**
         * creates a subscription for nodeCredential
         * @param nodeCredentialMoRef the Mo Ref you are interested in
         * @return The file descriptor used for asynchronous callbacks, or -1
         */
        int nodeCredentialSubscribeMoRef(const std::string& nodeCredentialMoRef);
	
	/**
         * creates a subscription for trustCategory
         * @param trustCategoryMoRef the Mo Ref you are interested in
         * @return The file descriptor used for asynchronous callbacks, or -1
         */
        int trustCategorySubscribeMoRef(const std::string& trustCategoryMoRef);

	/**
	 * ends the subscription
	 */
	void unsubscribe();

	/**
         * ends the nodeCredential subscription
         */
	void nodeCredentialUnsubscribe();

	/**
         * ends the trustCategory subscription
         */
	void trustCategoryUnsubscribe();

	/**
	 * @return A vector containing all the public keys of the trust category
	 */
	const std::vector<Certificate>& getTrustCategory() const;

	/**
	 * @return The public key of the node credential
	 */
	const Certificate& getNodeCredentialCert() const;

	/**
	 * @return The private key of the node credential
	 */
	const Certificate& getNodeCredentialKey() const;

	/**
	 * Gets a file decriptor that is used by the sec api to signal
	 * changes on subscribed data.
	 *
	 * @return A file descriptor.
	 */
	int getFd() const;

	/**
	 * Must be called when data arrives on file descriptor
	 * @return true if any certificate was changed
	 */
	bool dispatch();

	/**
	 * @return The cert filename of the node credential
	 */
	std::string getNodeCertFileName() const;
	/**
	 * @return The cert key filename of the node credential
	 */
	std::string getNodeKeyFileName() const;
	/**
	 * @return The trustcert Dir name of the node credential
	 */
	std::string getTrustCatDirName() const;

	int writeToSlapd();
	int updateLdap();
	bool searchString(std::string astr, std::string updatestr);
	bool updatestring(std::string astr,std::string str1,unsigned int count,std::string updatestr);
	int setLdapParams(std::string baseDn,std::string ipAddress,std::string fallBackIpAddress, std::string tlsClientKey,std::string tlsClientCert,std::string tlsCaCert,std::string bindPassword,std::string nodeCredential,std::string tlsCategory,std::string tlsCaCertDir,int useTls,
			int useReferrals, int serverPort);
	int getLdapInfo ();
	bool IsChange();
	std::string getTrustCatId() const;
	std::string getNodeCredId() const;
	std::string getCaCert() const;
	std::string getClientKey() const;
	std::string getClientCert() const;
	std::string getOldNodeCredId() const;
	std::string getOldTrustCatId() const;

	bool isLdapConfigurationChanged(){ return m_isLdapConfigurationUpdated;};

	bool updateLdapConfigurationAttribute(const std::string , const std::string , const std::string );

	bool updateLdapConfigurationClass();

  bool updateuseTlsAttribute(const std::string , int , const std::string );

private:
	
	int executeCmd(const std::string  &cmd, const std::vector<std::string> &options);

};


#endif // ACS_CS_SecApi_h_

