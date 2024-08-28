#include "ACS_CS_SecApi.h" 


#include "ACS_CS_Trace.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_Util.h"
#include <iostream>

ACS_CS_Trace_TDEF(ACS_CS_SecApi_TRACE);

ACS_CS_SecApi* ACS_CS_SecApi::Secinstance = NULL;

void Certificate::clear()
{
	_pem.clear();
	_filename.clear();
}


ACS_CS_SecApi::ACS_CS_SecApi():
    		 _secHandle(0),
    		 _nodecredentialSubscription(0),
    		 _trustcategorySubscription(0),
    		 _tCategory(NULL),
    		 _nodeCredentialId(""),
    		 _trustCategoryId(""),
    		 _fd(-1),_trustCatDirName(""),
    		 _updated(false),m_baseDn(""),m_ipAddress(""),m_fallBackIpAddress(""),m_tlsClientKey(""),m_tlsClientCert(""),m_tlsCaCert(""),m_bindPassword(""),m_bindDn(""),m_useTls(0),m_useReferrals(0),m_serverPort(0),m_nodeCredential(""),m_tlsCategory(""),
    		 m_baseDn_old(""),m_ipAddress_old(""),m_fallBackIpAddress_old(""),m_bindPassword_old(""),m_bindDn_old(""),m_useTls_old(0),m_useReferrals_old(0),m_serverPort_old(0),m_nodeCredential_old(""),
    		 m_tlsCategory_old(""),m_isLdapConfigurationUpdated(true),m_isSubscribed(false),m_nodeCredentialisSubscribed(false),m_trustCategoryisSubscribed(false)
{
}

ACS_CS_SecApi::~ACS_CS_SecApi()
{
	unsubscribe();
	nodeCredentialUnsubscribe();
	trustCategoryUnsubscribe();
}


const std::vector<Certificate>& ACS_CS_SecApi::getTrustCategory() const
{
	return _trustCategory;
}

const Certificate& ACS_CS_SecApi::getNodeCredentialCert() const
{
	return _nodeCredentialCert;
}

const Certificate& ACS_CS_SecApi::getNodeCredentialKey() const
{
	return _nodeCredentialKey;
}

int ACS_CS_SecApi::getFd() const
{
	//ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "handle from getFd = %d",_fd));
	return _fd;
}

bool ACS_CS_SecApi::dispatch()
{
	if((_secHandle == 0) || (m_isSubscribed == false)){
		return false;
	}
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Inside dispatch"));

	// Dispatch the asynchronous response.
	SecCreduStatus retval = sec_credu_dispatch(_secHandle, _fd, SEC_CREDU_DISPATCH_ONE);
	if (SEC_CREDU_OK == retval) {
		bool updated = true;
		_updated = false;
		return updated;
	} else {

		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_dispatch failed, retval=%s",sec_credu_status_string(retval)));
	}
	return false;
}


bool ACS_CS_SecApi::isSubscribed() const
{
//	return (_secHandle != 0)? true : false;
	if((_secHandle == 0) || (m_isSubscribed==false))
		return false;
	return true;
}

bool ACS_CS_SecApi::nodeCredentialisSubscribed() const
{
        if((_secHandle == 0) || (m_nodeCredentialisSubscribed==false))
                return false;
        return true;
}

bool ACS_CS_SecApi::trustCategoryisSubscribed() const
{
        if((_secHandle == 0) || (m_trustCategoryisSubscribed==false))
                return false;
        return true;
}

int ACS_CS_SecApi::subscribeMoRef(const std::string& nodeCredentialMoRef, const std::string& trustCategoryMoRef)
{
	return subscribe(ACS_CS_ImUtils::getIdValueFromRdn(nodeCredentialMoRef),ACS_CS_ImUtils::getIdValueFromRdn(trustCategoryMoRef));
}

int ACS_CS_SecApi::nodeCredentialSubscribeMoRef(const std::string& nodeCredentialMoRef)
{
	return nodeCredentialSubscribe(ACS_CS_ImUtils::getIdValueFromRdn(nodeCredentialMoRef));
}

int ACS_CS_SecApi::trustCategorySubscribeMoRef(const std::string& trustCategoryMoRef)
{
        return trustCategorySubscribe(ACS_CS_ImUtils::getIdValueFromRdn(trustCategoryMoRef));
}

int ACS_CS_SecApi::subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId)
{
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering subscribe"));
	if(isSubscribed()) {
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "subscribe called, but sec_credu api is not available"));
		return -1;
	}

	if(_fd != -1 && _nodeCredentialId==nodeCredentialId && _trustCategoryId==trustCategoryId) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Already subscribed"));
		return _fd;  // already subscribed
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "unsubscribing inside subscribing"));
		unsubscribe();
		if(nodeCredentialId.empty() || trustCategoryId.empty()) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "nodeCredentialId or trustCategoryId is empty, so not calling secapi subscribe again"));
			_fd = -1;
			return _fd;
		}
	}

	char trace[256] = {0};
	snprintf(trace, sizeof(trace) - 1, "TrustCategory = %s  NodeCredential = %s",
			trustCategoryId.c_str() ,nodeCredentialId.c_str());
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "%s", trace));

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
	if (SEC_CREDU_OK == (retval=sec_credu_initialize(&_secHandle, &parameters, &version))) {
		// Subscribe
		if (SEC_CREDU_OK != (retval=sec_credu_nodecredential_subscribe(_secHandle, nodeCredentialId.c_str(), &_nodecredentialSubscription))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_subscribe failed, retval=%s, nodeCredentialId %s ",sec_credu_status_string(retval), nodeCredentialId.c_str()));
		} else if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_subscribe(_secHandle, trustCategoryId.c_str(), &_trustcategorySubscription))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_subscribe failed, retval=%s, trustCategoryId %s",sec_credu_status_string(retval), trustCategoryId.c_str() ));
		} else if (SEC_CREDU_OK != (retval=sec_credu_selectionobject_get(_secHandle, &_fd))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_selectionobject_get failed, retval=%s",sec_credu_status_string(retval)));
		} else { // success
			m_isSubscribed=true;
			fetchTrustCategory();
			fetchNodeCert();

			_nodeCredentialId=nodeCredentialId;
			_trustCategoryId=trustCategoryId;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "issubscribedi= %d", m_isSubscribed));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Subscribe=%u handle = %d",_secHandle,_fd));
		}
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_initialize failed: retval %s",sec_credu_status_string(retval)));
		_secHandle = 0;
		_fd = -1;
		return _fd;
	}

	if(SEC_CREDU_OK != retval) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Failed to subscribe sec_credu api: retval %s",sec_credu_status_string(retval)));
		unsubscribe();
		_fd = -1;
	}

        //useTls changes for new sec changes
        if(!nodeCredentialId.empty() && !trustCategoryId.empty()) {
          if (m_useTls == 0) {
              m_useTls=1;
              std::string dn = ACS_CS_ImmMapper::ATT_LDAP_RDN;
              updateuseTlsAttribute("useTls", m_useTls, dn);
          }
        }


	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting subscribe"));

	return _fd;
}

int ACS_CS_SecApi::nodeCredentialSubscribe(const std::string& nodeCredentialId)
{
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering nodeCredential subscribe"));

	if(isSubscribed() == false) {
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "nodeCredential subscribe called, but sec_credu api is not available"));
		return -1;
	}

	if(_fd != -1 && _nodeCredentialId==nodeCredentialId) {
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Already nodeCredential is subscribed"));
		return _fd;  // already subscribed
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "nodeCredential unsubscribing inside subscribing"));
		nodeCredentialUnsubscribe();
		if(nodeCredentialId.empty()) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "nodeCredentialId is empty, so not calling secapi subscribe again"));
			_fd = -1;
			return _fd;
		}
	}

	char trace[256] = {0};
	snprintf(trace, sizeof(trace) - 1, "NodeCredential = %s",nodeCredentialId.c_str());
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "%s", trace));

	SecCreduVersion version;
	version.release_code  = 'A';
	version.major_version = 0x01;
	version.minor_version = 0x01;

	// Parameters to be used.
	SecCreduParameters parameters;
	parameters.nodecredential_change_callback = nodeCredentialChangeCallback;

	// Initialize the API.
	SecCreduStatus retval;
	if (SEC_CREDU_OK == (retval=sec_credu_initialize(&_secHandle, &parameters, &version))) {
		// Subscribe
		if (SEC_CREDU_OK != (retval=sec_credu_nodecredential_subscribe(_secHandle, nodeCredentialId.c_str(), &_nodecredentialSubscription))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_subscribe failed, retval=%s, nodeCredentialId %s ",sec_credu_status_string(retval), nodeCredentialId.c_str()));
		} else if (SEC_CREDU_OK != (retval=sec_credu_selectionobject_get(_secHandle, &_fd))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_selectionobject_get failed, retval=%s",sec_credu_status_string(retval)));
		} else { // success
			m_nodeCredentialisSubscribed=true;
			fetchNodeCert();

			_nodeCredentialId=nodeCredentialId;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "issubscribedi= %d", m_nodeCredentialisSubscribed));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Subscribe=%u handle = %d",_secHandle,_fd));
		}
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_initialize failed: retval %s",sec_credu_status_string(retval)));
		_secHandle = 0;
		_fd = -1;
		return _fd;
	}

	if(SEC_CREDU_OK != retval) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Failed to subscribe sec_credu api: retval %s",sec_credu_status_string(retval)));
		unsubscribe();
		_fd = -1;
	}

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting nodecredential subscribe"));

	return _fd;
}

int ACS_CS_SecApi::trustCategorySubscribe(const std::string& trustCategoryId)
{
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering trustCategory subscribe"));
	if(isSubscribed()) {
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "trustCategory subscribe called, but sec_credu api is not available"));
		return -1;
	}

	if(_fd != -1 &&  _trustCategoryId==trustCategoryId) {
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Already trustCategory is subscribed"));
		return _fd;  // already subscribed
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "trustCategory unsubscribing inside subscribing"));
		trustCategoryUnsubscribe();
		if(trustCategoryId.empty()) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "trustCategoryId is empty, so not calling secapi subscribe again"));
			_fd = -1;
			return _fd;
		}
	}

	char trace[256] = {0};
	snprintf(trace, sizeof(trace) - 1, "TrustCategory = %s",trustCategoryId.c_str());
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "%s", trace));

	SecCreduVersion version;
	version.release_code  = 'A';
	version.major_version = 0x01;
	version.minor_version = 0x01;

	// Parameters to be used.
	SecCreduParameters parameters;
	parameters.trustcategory_change_callback  = trustCategoryChangeCallback;

	// Initialize the API.
	SecCreduStatus retval;
	if (SEC_CREDU_OK == (retval=sec_credu_initialize(&_secHandle, &parameters, &version))) {
		// Subscribe
		if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_subscribe(_secHandle, trustCategoryId.c_str(), &_trustcategorySubscription))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_subscribe failed, retval=%s, trustCategoryId %s",sec_credu_status_string(retval), trustCategoryId.c_str() ));
		} else if (SEC_CREDU_OK != (retval=sec_credu_selectionobject_get(_secHandle, &_fd))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_selectionobject_get failed, retval=%s",sec_credu_status_string(retval)));
		} else { // success
			m_trustCategoryisSubscribed=true;
			fetchTrustCategory();

			_trustCategoryId=trustCategoryId;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "issubscribedi= %d", m_trustCategoryisSubscribed));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Subscribe=%u handle = %d",_secHandle,_fd));
		}
	} else {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_initialize failed: retval %s",sec_credu_status_string(retval)));
		_secHandle = 0;
		_fd = -1;
		return _fd;
	}

	if(SEC_CREDU_OK != retval) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Failed to subscribe sec_credu api: retval %s",sec_credu_status_string(retval)));
		unsubscribe();
		_fd = -1;
	}

	//useTls changes for new sec changes
        if(!trustCategoryId.empty()) {
          if (m_useTls == 0) {
              m_useTls=1;
              std::string dn = ACS_CS_ImmMapper::ATT_LDAP_RDN;
              updateuseTlsAttribute("useTls", m_useTls, dn);
          }
        }

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting trustCategory subscribe"));

	return _fd;

}

void ACS_CS_SecApi::unsubscribe()
{
	bool success = true;
	SecCreduStatus retval;

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering unsubscribe"));

	if(_secHandle != 0){	//Fix for TR HY61698
		m_isSubscribed=false;
		_nodeCredentialId.clear();
		_trustCategoryId.clear();
		_nodeCredentialCert.clear();
		_nodeCredentialKey.clear();
		_trustCategory.clear();
		_trustCatDirName.clear();
		m_tlsCaCertDir.clear();

		// Free the allocated memory.
		if (_tCategory) {
			if ( (SEC_CREDU_OK != (retval = sec_credu_trustcategory_free(_secHandle, &_tCategory)))) {
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_free failed, retval=%s",sec_credu_status_string(retval)));
			}
		}

		if(_nodecredentialSubscription != 0) {
			if (SEC_CREDU_OK != sec_credu_nodecredential_unsubscribe(_secHandle, _nodecredentialSubscription))
				success = false;
			_nodecredentialSubscription = 0;
		}

		if(_trustcategorySubscription != 0) {
			if (SEC_CREDU_OK != sec_credu_trustcategory_unsubscribe(_secHandle, _trustcategorySubscription))
				success = false;
			_trustcategorySubscription = 0;
		}

		if(SEC_CREDU_OK != sec_credu_finalize(_secHandle)) {
			success = false;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to finalize SEC API handle"));
		}
		_secHandle = 0;
		_fd = -1;

		if(!success) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE,"Failed to unsubscribe SEC subscription"));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to unsubscribe from SEC API"));
		}
	}
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting unsubscribe"));
}

void ACS_CS_SecApi::nodeCredentialUnsubscribe()
{
	bool success = true;
	SecCreduStatus retval;

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering nodecredential unsubscribe"));

	if(_secHandle != 0){    //Fix for TR HY61698
		m_nodeCredentialisSubscribed=false;
		_nodeCredentialId.clear();
		_nodeCredentialCert.clear();
		_nodeCredentialKey.clear();
		_trustCatDirName.clear();
		m_tlsCaCertDir.clear();

		if(_nodecredentialSubscription != 0) {
			if (SEC_CREDU_OK != sec_credu_nodecredential_unsubscribe(_secHandle, _nodecredentialSubscription))
				success = false;
			_nodecredentialSubscription = 0;
		}

		if(SEC_CREDU_OK != sec_credu_finalize(_secHandle)) {
			success = false;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to finalize SEC API handle"));
		}
		_secHandle = 0;
		_fd = -1;

		if(!success) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE,"Failed to nodecredential unsubscribe SEC subscription"));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to nodecredential unsubscribe from SEC API"));
		}
	}
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting nodecredential unsubscribe"));
}

void ACS_CS_SecApi::trustCategoryUnsubscribe()
{
	bool success = true;
	SecCreduStatus retval;

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Entering trustcategory unsubscribe"));

	if(_secHandle != 0){    //Fix for TR HY61698
		m_trustCategoryisSubscribed=false;
		_trustCategoryId.clear();
		_trustCategory.clear();
		_trustCatDirName.clear();
		m_tlsCaCertDir.clear();

		// Free the allocated memory.
		if (_tCategory) {
			if ( (SEC_CREDU_OK != (retval = sec_credu_trustcategory_free(_secHandle, &_tCategory)))) {
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_free failed, retval=%s",sec_credu_status_string(retval)));
			}
		}

		if(_trustcategorySubscription != 0) {
			if (SEC_CREDU_OK != sec_credu_trustcategory_unsubscribe(_secHandle, _trustcategorySubscription))
				success = false;
			_trustcategorySubscription = 0;
		}

		if(SEC_CREDU_OK != sec_credu_finalize(_secHandle)) {
			success = false;
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to finalize SEC API handle"));
		}
		_secHandle = 0;
		_fd = -1;

		if(!success) {
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE,"Failed to trustcategory unsubscribe SEC subscription"));
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to trustcategory unsubscribe from SEC API"));
		}
	}
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO,"Exiting trustcategory unsubscribe"));
}

void ACS_CS_SecApi::fetchNodeCert()
{
	SecCreduStatus retval;

	ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "fetchNodeCert"));

	if((_secHandle != 0) && ((m_isSubscribed==true) || (m_nodeCredentialisSubscribed==true) || (m_trustCategoryisSubscribed==true))){
		_nodeCredentialCert.clear();
		_nodeCredentialKey.clear();

		char *certContent = NULL;
		char *certFilename = NULL;
		char *keyContent = NULL;
		char *keyFilename = NULL;
		if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle, _nodecredentialSubscription,
				SEC_CREDU_FILENAME, &certFilename))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_cert_get FILENAME failed, retval=%s",sec_credu_status_string(retval)));
		} else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_PEM, &certContent))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_cert_get PEM failed, retval=%s",sec_credu_status_string(retval)));
		} else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_FILENAME, &keyFilename))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_key_get FILENAME failed, retval=%s",sec_credu_status_string(retval)));
		} else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
				_nodecredentialSubscription, SEC_CREDU_PEM, &keyContent))) {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_nodecredential_key_get PEM failed, retval=%s",sec_credu_status_string(retval)));
		}

		if(retval == SEC_CREDU_OK) {
			if((certFilename != NULL)&&(certContent!=NULL))/*HW89015*/
			{
			_nodeCredentialCert = Certificate(certFilename, certContent);
			m_tlsClientCert=certFilename;
			}
			if((keyFilename != NULL)&&(keyContent!=NULL))  /*HW89015*/
			{
			_nodeCredentialKey = Certificate(keyFilename, keyContent);
			m_tlsClientKey=keyFilename;
			}

			//Update IMM with LdapCert Info
			if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCLIENTCERTIFICATE, m_tlsClientCert, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
				m_isLdapConfigurationUpdated = false;

			if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCLIENTKEY, m_tlsClientKey, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
				m_isLdapConfigurationUpdated = false;

			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "certFilename=%s keyFilename=%s",m_tlsClientCert.c_str(), m_tlsClientKey.c_str()));



		} else {
			ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "fetchNodeCert failed"));

		}

		// Free the allocated memory.
		free(certContent);
		free(certFilename);
		free(keyFilename);
		free(keyContent);
	}
}

void ACS_CS_SecApi::fetchTrustCategory()
{
	bool success = true;

	SecCreduTrustCategory *category = NULL;
	SecCreduStatus retval;
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, " Inside fetchTrustCategory"));

	if((_secHandle == 0)  || ((m_isSubscribed==false) && (m_nodeCredentialisSubscribed==false) && (m_trustCategoryisSubscribed==false))){
		return;
	}
	_trustCategory.clear();
	_trustCatDirName.clear();
	char *trustCatDirName = NULL;

	// Fetch information on the TrustCategory instance.
	if (SEC_CREDU_OK != (retval= sec_credu_trustcategory_get(_secHandle, _trustcategorySubscription, &category))) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_get failed, retval=%s",sec_credu_status_string(retval)));
		success = false;
	} else if(SEC_CREDU_OK != (retval= sec_credu_trustcategory_dirname_get(category, &trustCatDirName))) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_dirname_get failed, retval=%s",sec_credu_status_string(retval)));
		trustCatDirName = NULL;
		success = false;
	} else {
		char* trustedCertFilename = NULL;
		char* trustedCertContent = NULL;
		char* trustedCertId = NULL;
		char* trustedCertId2 = NULL;

		_trustCatDirName = (trustCatDirName)?std::string(trustCatDirName):std::string();
		m_tlsCaCertDir=_trustCatDirName;
		m_tlsCaCert = "";

		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, " fetching directory"));
		size_t numTrustCategoryCerts=0;
		sec_credu_trustcategory_cert_count_get(category, &numTrustCategoryCerts);

		for (size_t i = 0; i < numTrustCategoryCerts; ++i) {
			bool ok = true;
			if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_FILENAME, i,
					&trustedCertFilename,
					&trustedCertId))) {
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_cert_get FILENAME failed, retval=%s",sec_credu_status_string(retval)));
				trustedCertFilename=NULL;
				trustedCertId=NULL;
				ok=false;
			}

			if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_PEM, i,
					&trustedCertContent,
					&trustedCertId2))) {
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "sec_credu_trustcategory_cert_get PEM failed, retval=%s",sec_credu_status_string(retval)));
				trustedCertContent=NULL;
				trustedCertId2=NULL;
				ok=false;
			}

			if(ok) {
				_tCategory = category;
				_trustCategory.push_back(Certificate(trustedCertFilename, trustedCertContent));

				//Update IMM with LdapCert Info
				if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCACERTIFICATE, m_tlsCaCertDir, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
					m_isLdapConfigurationUpdated = false;

				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "TrustedCertFolder=%s",m_tlsCaCertDir.c_str()));

			} else {
				success = false;
			}

			free(trustedCertFilename);
			free(trustedCertContent);
			free(trustedCertId);
			free(trustedCertId2);
		}

		free(trustCatDirName);
	}
	if(success) {
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, " fetching Success"));
		return;
	}

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "fetchTrustCategory failed"));
	_trustCategory.clear();
}

ACS_CS_SecApi* ACS_CS_SecApi::getInstance()
{

	if(!Secinstance)
		Secinstance = new ACS_CS_SecApi();

	return Secinstance;
}

std::string ACS_CS_SecApi::getNodeKeyFileName() const
{
	return _nodeCredentialKey.getFilename();
}

std::string ACS_CS_SecApi::getNodeCertFileName() const
{
	return _nodeCredentialCert.getFilename();
}

std::string ACS_CS_SecApi::getTrustCatDirName() const
{
	return _trustCatDirName;
}

std::string ACS_CS_SecApi::getTrustCatId() const
{
	return m_tlsCategory;
}

std::string ACS_CS_SecApi::getNodeCredId() const
{
	return m_nodeCredential;
}

std::string ACS_CS_SecApi::getClientCert() const
{
	return m_tlsClientCert;
}

std::string ACS_CS_SecApi::getClientKey() const
{
	return m_tlsClientKey;

}

std::string ACS_CS_SecApi::getCaCert() const
{
	return m_tlsCaCert;
}

std::string ACS_CS_SecApi::getOldNodeCredId() const
{
        return m_nodeCredential_old;
}

std::string ACS_CS_SecApi::getOldTrustCatId() const
{
        return m_tlsCategory_old;
}

void ACS_CS_SecApi::nodeCredentialChangeCallback(
		SecCreduHandle /*handle*/, SecCreduSubscription /*nodecredential_subscription*/, const char */*nodecredential_id*/)
{

//	cout<<"oiHandle   : "<< handle <<endl;
//	cout<<"nodecredential_subscription : "<< nodecredential_subscription <<endl;
//	cout<<"nodecredential_id : "<< nodecredential_id <<endl;
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "*************entering into nodeCredentialChangeCallback*******************"));
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Modify nodeCredentialChangeCallback"));
	ACS_CS_SecApi *pinstance = ACS_CS_SecApi::getInstance();
	if (pinstance) {
		pinstance->fetchNodeCert();
		pinstance->_updated=true;

	}
}

void ACS_CS_SecApi::trustCategoryChangeCallback(
		SecCreduHandle /*handle*/, SecCreduSubscription /*trustcategory_subscription*/, const char */*trustcategory_id*/)
{
//	cout<<"oiHandle   : "<< handle <<endl;
//	cout<<"trustcategory_subscription : "<< trustcategory_subscription <<endl;
//	cout<<"trustcategory_id : "<< trustcategory_id <<endl;
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "*************entering into trustCategoryChangeCallback*******************"));
	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_INFO, "Modify trustCategoryChangeCallback"));
	ACS_CS_SecApi *pinstance = ACS_CS_SecApi::getInstance();
	if (pinstance) {
		pinstance->fetchTrustCategory();
		pinstance->_updated=true;

	}
}


int ACS_CS_SecApi::writeToSlapd()
{
	int res = 0 , count = 0;

	if (!ACS_CS_Protocol::CS_ProtocolChecker::checkIfAP1())
		return res;

	std::string protocol;
	std::string refValue;
	ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Entering WriteTo slapd"));

	if (m_useReferrals==0)
	{
		refValue="no";
	}
	else
	{
		refValue="yes";
	}
        //HX91370->introduced check for port-1636!
	if(m_serverPort == 636 || m_serverPort == 1636)
	{
		protocol = "ldaps://";
	}
	else
	{
		protocol = "ldap://";
	}

	string pszReadLine,findString,updateString,findStrelem;
	std::stringstream Str;
	string fileName = "/etc/openldap/slapd.conf";

	getInstance()->lock_.acquire_write();

	ifstream myfile(fileName.c_str());
	if(myfile.is_open())
	{
		while(getline(myfile,pszReadLine))
		{
			contentsBlock.push_back(pszReadLine);
		}
		//find first line to be inserted
		findString ="include";
		updateString = "include         /etc/openldap/schema/euac-extended.schema";
		searchString( findString, updateString );

		//find second line to be inserted
		findString = "moduleload";
		//updateString = "moduleload    back_meta.la \nallow bind_v2";
		//updateString = "moduleload    back_ldap.la \nallow bind_v2";
		updateString = "moduleload      rwm \nmoduleload    back_ldap.la \nallow bind_v2";
		searchString( findString, updateString );

		//find third line to be inserted
		findString = "EVERYTHING!";
		Str << "# Security SSL" << "\n";

		Str << "#######################################################################" << "\n";
		Str << "#META backend 1 (For IPT - base DN overwrite)" << "\n";
		Str << "#######################################################################" << "\n\n";
		Str << "database 	ldap" << "\n";
		if(m_useTls)
		{
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Adding TLS entries in IPT database"));
			Str << "tls start" << "\n";
			Str << "    tls_cacertdir=" << m_tlsCaCertDir << "\n";

			Str << "    tls_cert=" << m_tlsClientCert << "\n";
			Str << "    tls_key=" << m_tlsClientKey << "\n";
			Str << "    tls_reqcert=" << "demand" << "\n";
		}
		Str << "suffix            \"dc=aplocaldomain,dc=com\"\n";
                //HX91370-start
		if (m_serverPort == 1636)
		{
		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Port is 1636"));
					Str << "uri               \"" << protocol << m_ipAddress<<":"<<m_serverPort;

					if (!m_fallBackIpAddress.empty())
					{
						Str << " "<< protocol << m_fallBackIpAddress<<":"<<m_serverPort;
					}

					Str << "\"\n";
		}
		else
		{
			Str << "uri               \"" << protocol << m_ipAddress;

					if (!m_fallBackIpAddress.empty())
					{
						Str << " "<< protocol << m_fallBackIpAddress;
					}

					Str << "\"\n";
		}
                //HX91370-end
		Str << "overlay            rwm" << "\n";
		Str << "rwm-rewriteEngine  on" << "\n";
		Str << "rwm-suffixmassage " << "\"dc=aplocaldomain,dc=com\" " <<"\"" << m_baseDn <<"\"\n";
		Str << "lastmod            off" << "\n";
		Str << "idassert-bind bindmethod=simple" << "\n";
		Str << "         binddn=\"" << m_bindDn <<"\"" << "\n";
		Str << "         credentials=\"" << m_bindPassword <<"\"" << "\n";
		Str << "	 mode=none" << "\n";
		if(m_useTls)
		{
			Str << "         starttls=no" << "\n";
			Str << "    tls_cacertdir=" << m_tlsCaCertDir << "\n";

			Str << "         tls_cert=" << m_tlsClientCert << "\n";
			Str << "         tls_key=" << m_tlsClientKey << "\n";
			Str << "         tls_reqcert=" << "demand" << "\n";
		}

		Str << "idassert-authzFrom \"*\""<<"\n";
		Str << "chase-referrals		"<< refValue <<"\n" <<"\n";
		Str << "norefs 		yes"<<"\n";
		Str << "network-timeout 4"<<"\n";
		Str << "timeout 4"<<"\n";
		Str << "conn-ttl 4"<<"\n";
		Str << "rebind-as-user yes"<<"\n";

		Str << "#######################################################################" << "\n";
		Str << "#META backend 2 (For other clients - NO base DN overwrite)" << "\n";
		Str << "#######################################################################" << "\n\n";
		Str << "database      ldap" << "\n";

		if(m_useTls)
		{
			ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Adding TLS entries in AP2 database"));
			Str << "tls start" << "\n";
			Str << "    tls_cacertdir=" << m_tlsCaCertDir << "\n";

			Str << "    tls_cert=" << m_tlsClientCert << "\n";
			Str << "    tls_key=" << m_tlsClientKey << "\n";
			Str << "    tls_reqcert=" << "demand" << "\n";
		}
		//if(m_useReferrals)
		//{
		Str << "suffix          " << "\"\"" << "\n";
		//}
		//else
		//{
		//	Str << "suffix          \"" << m_baseDn << "\"" << "\n";
		//}
                //HX91370-start
		if (m_serverPort == 1636)
		{

		ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Port is 1636"));
					Str << "uri                \"" << protocol << m_ipAddress<<":"<<m_serverPort;

							if (!m_fallBackIpAddress.empty())
							{
								Str << " "<< protocol << m_fallBackIpAddress<<":"<<m_serverPort;
							}
							Str <<"\"\n";
		}
		else
		{
			Str << "uri                \"" << protocol << m_ipAddress;

					if (!m_fallBackIpAddress.empty())
					{
						Str << " "<< protocol << m_fallBackIpAddress;
					}
					Str <<"\"\n";
		}
                //HX91370-end
		Str << "lastmod         off" << "\n";

		//HU15820 - begin
//		Str << "acl-bind bindmethod=simple" << "\n";
//		Str << "         binddn=\"" << m_bindDn <<"\"" << "\n";
//		Str << "         credentials=\"" << m_bindPassword <<"\"" << "\n";

		Str << "idassert-bind bindmethod=simple" << "\n";
		Str << "         binddn=\"" << m_bindDn <<"\"" << "\n";
		Str << "         credentials=\"" << m_bindPassword <<"\"" << "\n";
		Str << "         mode=none"<< "\n";
		if(m_useTls)
		{
			Str << "         starttls=no" << "\n";
			Str << "         tls_cacertdir=" << m_tlsCaCertDir << "\n";

			Str << "         tls_cert=" << m_tlsClientCert << "\n";
			Str << "         tls_key=" << m_tlsClientKey << "\n";
			Str << "         tls_reqcert=" << "demand" << "\n";
		}
		Str << "idassert-authzFrom \"*\""<<"\n";

		//HU15820 - end

		Str << "chase-referrals		"<< refValue <<"\n";
		Str << "norefs 		yes"<<"\n";
		Str << "network-timeout 4"<<"\n";
		Str << "timeout 4"<<"\n";
		Str << "conn-ttl 4"<<"\n";
		Str << "rebind-as-user yes"<<"\n";


		Str << "#######################################################################" << "\n";

		count = 0;
		while(getline(Str,pszReadLine))
		{
			count++;
		}
		updateString = Str.str();
		findStrelem = "# Security SSL";
		updatestring(findString, findStrelem, count, updateString);

		//delete the content related to BDB database
		findString  = "# BDB database definitions";
		for(unsigned int i=0;i<contentsBlock.size();i++)
		{

			if(contentsBlock[i].find(findString) != std::string::npos)
			{
				contentsBlock.erase(contentsBlock.begin()+i, contentsBlock.begin()+contentsBlock.size());
			}
		}
		//copy the buffer(vector content) to the slapd.conf file
		ofstream output_file("/etc/openldap/slapd.conf");
		ostream_iterator<std::string> output_iterator(output_file, "\n");
		copy(contentsBlock.begin(), contentsBlock.end(), output_iterator);
		output_file.close();
		contentsBlock.clear();
		myfile.close();

		getInstance()->lock_.release();

	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Error:Unable to open file!!"));
		getInstance()->lock_.release();
		return -1;
	}

	//assign right permissions
	if (!ACS_CS_NS::ACS_CS_Util::setRecursivePermission(ACS_APGCC::beforeLast(m_tlsCaCertDir,(string)"/"), ACS_CS_NS::PERM_700, ACS_CS_NS::PERM_640, ACS_CS_NS::cert_owner, ACS_CS_NS::cert_group))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_ERROR, "Failed to set permission on folder %s...", ACS_APGCC::beforeLast(m_tlsCaCertDir,(string)"/")));
	}

	//int errorCode = system("rcldap restart");
	std::vector<std::string> options;

	std::string command("/opt/ap/apos/bin/servicemgmt/servicemgmt");
	std::string restart_option("restart");
	std::string service_name_option("apg-ldap.service");
	options.push_back(restart_option);
	options.push_back(service_name_option);

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_WARN, "Restarting OpenLdap Server..."));
	int errorCode = executeCmd(command, options);

	if(errorCode != 0)
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN, "OpenLdap restart failed, error code = %d",errorCode));
		return errorCode;
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN, "OpenLdap successfully restarted"));
	}

	return res;
}
bool ACS_CS_SecApi::searchString(std::string astr, std::string updatestr)
{
	int count = 0, b = 0;
	vector<string>::iterator anotherHelper;
	//counting the no.of of instances of the string
	for(unsigned int i=0;i<contentsBlock.size();i++)
	{
		if(strstr (contentsBlock[i].c_str(),astr.c_str())!=NULL)
		{
			count ++;
		}
	}
	// traversing to the last instance of the string
	for (anotherHelper = contentsBlock.begin(),b = 0;  anotherHelper <  contentsBlock.end(); anotherHelper++,b++)
	{
		if(strstr (contentsBlock[b].c_str(),astr.c_str())!=NULL)
		{
			count --;
		}
		if (count == 0)
		{
			// inserting the string
			if(!(strstr (updatestr.c_str(),contentsBlock[b].c_str())!=NULL))
			{
				contentsBlock.insert(contentsBlock.begin()+b+1,updatestr);
			}
			break;
		}
	}
	return true;
}
bool ACS_CS_SecApi::updatestring(std::string astr,std::string str1,unsigned int count, std::string updatestr)
{
	for(unsigned int i=0;i<contentsBlock.size();i++)
	{

		if(contentsBlock[i].find(astr) != std::string::npos)
		{
			if(contentsBlock[i+1].find(str1) != std::string::npos)
			{
				contentsBlock.erase(contentsBlock.begin()+i+1, contentsBlock.begin()+contentsBlock.size());
				contentsBlock.insert(contentsBlock.begin()+i+1,updatestr);
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_SecApi_TRACE, "Default slap.conf is existing"));
				contentsBlock.insert(contentsBlock.begin()+i+1,updatestr);
			}
			break;
		}
	}
	return true;
}

int ACS_CS_SecApi::getLdapInfo ()
{
	int res = 0;
	std::string bindPasswordDn;
        int tlsMode = 1;

	m_baseDn_old = m_baseDn;
	m_ipAddress_old = m_ipAddress;
	m_fallBackIpAddress_old = m_fallBackIpAddress;
	m_bindDn_old = m_bindDn;
	m_bindPassword_old = m_bindPassword;
	m_useReferrals_old = m_useReferrals;
	m_useTls_old = m_useTls;
	m_serverPort_old = m_serverPort;
	// BEGIN : Impacted code for SEC1.3
	m_nodeCredential_old = m_nodeCredential;
	m_tlsCategory_old = m_tlsCategory;
	// Impacted code for SEC 2.1
	std::string dn = ACS_CS_ImmMapper::ATT_LDAP_RDN;


	if (!ACS_CS_ImUtils::getImmAttributeString(dn,"baseDn",m_baseDn))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read baseDn from IMM"));
		res = 1;
		return res;
	}

	if (!ACS_CS_ImUtils::getImmAttributeString(dn,"ldapIpAddress",m_ipAddress))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read ldapIpAddress from IMM"));
		res = 1;
		return res;
	}
	if (!ACS_CS_ImUtils::getImmAttributeString(dn,"fallbackLdapIpAddress",m_fallBackIpAddress))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read fallbackLdapIpAddress from IMM"));
		m_fallBackIpAddress="";
//		res = 1;
//		return res;
	}
	if (!ACS_CS_ImUtils::getImmAttributeInt(dn,"useTls",m_useTls))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read useTls value from IMM"));
		res = 1;
		return res;
	}
	if (!ACS_CS_ImUtils::getImmAttributeInt(dn,"useReferrals",m_useReferrals))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read useReferrals from IMM"));
		//res = 1;
		//return res;
	}
        if (!ACS_CS_ImUtils::getImmAttributeInt(dn,"tlsMode",tlsMode))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read tlsMode from IMM"));
		//res = 1;
		//return res;
	}
	if (!ACS_CS_ImUtils::getImmAttributeInt(dn,"serverPort",m_serverPort))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read serverPort from IMM"));
		if((m_useTls == 1) && (tlsMode == 1))
		{
			m_serverPort = 636;
		}
		else
		{
			m_serverPort = 389;
		}
                //res = 1;
		//return res;
	}

	if(!ACS_CS_ImUtils::getImmAttributeString(dn,"bindDn",m_bindDn))
	{
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read bindDn from IMM"));
		res = 1;
		return res;
	}

	if(!ACS_CS_ImUtils::getImmAttributeString(dn,"bindPassword",bindPasswordDn))
	{
		std::cout << "DBG: Could not read m_bindPassword from SecM fragment" << std::endl;
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read bindPassword  from IMM"));
		res = 1;
		return res;
	}
	if(!ACS_CS_ImUtils::getImmAttributeString(dn,"trustCategory",m_tlsCategory))
	{
		std::cout << "DBG: Could not read tlsCategory from SecM fragment" << std::endl;
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read tlsCategory  from IMM"));
		m_tlsCategory="";
		//res = 1;
		//return res;
	}
	if(!ACS_CS_ImUtils::getImmAttributeString(dn,"nodeCredential",m_nodeCredential))
	{
		std::cout << "DBG: Could not read nodeCredential from SecM fragment" << std::endl;
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read nodeCredential from IMM"));
		m_nodeCredential="";
		//res = 1;
		//return res;
	}
	else
	{
		if (!bindPasswordDn.empty())
		{
			if(!ACS_CS_ImUtils::getImmAttributeString(bindPasswordDn,"password",m_bindPassword))
			{
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_ERROR,"Failed to read bindPassword  from IMM"));
				res = 1;
				return res;
			}
			char * plaintext = NULL;
			std::string ciphertext = m_bindPassword;
			SecCryptoStatus decryptStatus;
			decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
			if(decryptStatus != SEC_CRYPTO_OK )
			{
				ACS_CS_TRACE((ACS_CS_SecApi_TRACE,"Password decryption failed"));
				res = 1;
				return res;
			}
			else
			{
				m_bindPassword = plaintext;
			}
			if(plaintext !=0 )
				free(plaintext);
		}
	}

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE, LOG_LEVEL_INFO, "TrustCategory = %s  NodeCredential = %s",m_tlsCategory.c_str() ,m_nodeCredential.c_str()));

	return res;
}

bool ACS_CS_SecApi::IsChange()
{
	if((m_baseDn_old == m_baseDn) && (m_ipAddress_old == m_ipAddress) && (m_fallBackIpAddress_old == m_fallBackIpAddress) && ( m_bindDn_old == m_bindDn) && (m_bindPassword_old == m_bindPassword) && (m_useReferrals_old == m_useReferrals) && (m_useTls_old == m_useTls) && (m_serverPort_old == m_serverPort) && (m_nodeCredential_old == m_nodeCredential) && (m_tlsCategory_old == m_tlsCategory))
	{
		//errorCode = 0;
		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN,"No change! Don't update slapd.conf!!!"));
		return true;
	}
	return false;
}

void ACS_CS_SecApi::finalize()
{
	if (Secinstance)
		delete(Secinstance);
	Secinstance = 0;
}

bool ACS_CS_SecApi::updateuseTlsAttribute(const std::string immAttribute, int immAttributeValue, const std::string dnObject)
{
  bool result = false;
  if(immAttributeValue != 0)
  {
      ACS_CC_ImmParameter parameterObj = ACS_CS_ImUtils::defineParameterInt(immAttribute.c_str(),ATTR_INT32T,&immAttributeValue,1);
      if (ACS_CS_ImUtils::modifyImmAttribute(dnObject.c_str(), parameterObj))
        result = true;
  }

  return result;
}

bool ACS_CS_SecApi::updateLdapConfigurationAttribute(const std::string immAttribute, const std::string immAttributeValue, const std::string dnObject)
{
	bool result = false;

	if (!immAttributeValue.empty())
	{
		ACS_CC_ImmParameter parameterObj = ACS_CS_ImUtils::defineParameterString(immAttribute.c_str(),ATTR_STRINGT,immAttributeValue.c_str(),1);
		if (ACS_CS_ImUtils::modifyImmAttribute(dnObject.c_str(), parameterObj))
			result = true;

		delete [] parameterObj.attrValues;
	}

	return result;
}

bool ACS_CS_SecApi::updateLdapConfigurationClass()
{
	m_isLdapConfigurationUpdated = true;

	if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCACERTIFICATE, m_tlsCaCertDir, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
		m_isLdapConfigurationUpdated = false;

	if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCLIENTCERTIFICATE, m_tlsClientCert, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
		m_isLdapConfigurationUpdated = false;

	if (!updateLdapConfigurationAttribute(ACS_CS_ImmMapper::ATT_LDAP_TLSCLIENTKEY, m_tlsClientKey, ACS_CS_ImmMapper::RDN_LDAPCONFIGURATION))
		m_isLdapConfigurationUpdated = false;

	return m_isLdapConfigurationUpdated;
}

int ACS_CS_SecApi::executeCmd(const std::string  &cmd, const std::vector<std::string> &options)
{

	// Open event used to signal service shutdown
	ACS_CS_EventHandle shutdownAllEvent = ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN, "Starting execution of %s command...", cmd.c_str()));

	pid_t child = fork();
	int return_value;

	if(child == 0)
	{

		size_t arguments_nr =  options.size() + 1;
		char ** argv = new char*[arguments_nr + 1];
		memset(argv, 0, arguments_nr + 1);

		argv[0] = new char[cmd.length() + 1];
		memset(argv[0], 0 , cmd.length() + 1);
		strcpy(argv[0], cmd.c_str());

		for (size_t i = 0; i < options.size(); ++i)
		{
			const std::string option = options[i];
			size_t length = option.length();

			argv[i +1] = new char[ length + 1];
			memset(argv[i + 1], 0 , length + 1);
			strcpy(argv[i + 1], option.c_str());
		}

		argv[ arguments_nr ] = 0;

		int result = execv(argv[0], argv);

		for (size_t i = 0; i < arguments_nr; ++i)
		{
			delete[] argv[i];
		}

		delete[] (argv);

		if(result < 0) {
			exit(1);
		}
		else
		{
			exit(0);
		}
	}
	else
	{
		int status;

		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_DEBUG, "Waiting for %s command to complete...", cmd.c_str()));

		ACS_CS_DEBUG(("Waiting for client process to finish"));
		int sent_kill = 0;

		while (waitpid(child, &status, WNOHANG) != -1)
		{
			if (!sent_kill)
			{
				int event = ACS_CS_Event::WaitForEvents(1,&shutdownAllEvent, 1000);
				ACS_CS_DEBUG(("Waiting for client process to finish - loop"));
				ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_DEBUG, "Waiting for %s command to complete...", cmd.c_str()));
				if (event == 0)
				{
					ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN, "Command %s execution aborted! Received Shutdown.", cmd.c_str()));
					ACS_CS_DEBUG(("Received shutdown"));
					kill(child, SIGTERM);
					sent_kill++;
				}
			}
			else
			{
				usleep(100000);
			}
		}

		return_value = (WIFEXITED(status))? WEXITSTATUS(status):2;

		ACS_CS_FTRACE((ACS_CS_SecApi_TRACE,LOG_LEVEL_WARN, "Command %s completed with status %d!", cmd.c_str(), return_value));
		ACS_CS_DEBUG(("Client Process finished"));
	}

	return return_value;
}
