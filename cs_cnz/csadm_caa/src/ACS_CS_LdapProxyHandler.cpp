/*
 * ACS_CS_LdapProxyHandler.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: xlalkak
 */
#include <sec/crypto_status.h>
#include <sec/crypto_api.h>
#include <string>
#include <fstream>
#include <vector>
#include<iterator>


#include "ACS_CS_LdapProxyHandler.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_Ldap_TRACE);

/*============================================================================
	ROUTINE: ACS_CS_LdapProxyHandler
 ============================================================================ */
ACS_CS_LdapProxyHandler::ACS_CS_LdapProxyHandler() :  acs_apgcc_objectimplementerinterface_V3(ACS_CS_ImmMapper::IMPLEMENTER_LDAP),
m_ImmClassName(ACS_CS_ImmMapper::classLdap),
immLdapWorker(0),
ldapSecApi(0)
{

	ldapSecApi = ACS_CS_SecApi::getInstance();

}

/*============================================================================
	ROUTINE: ~ACS_CS_LdapProxyHandler
 ============================================================================ */
ACS_CS_LdapProxyHandler::~ACS_CS_LdapProxyHandler()
{

	if (immLdapWorker){
		delete immLdapWorker;
		immLdapWorker = 0;
	}

	ACS_CS_SecApi::finalize();

}

int ACS_CS_LdapProxyHandler::start()
{
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_DEBUG,"Entering ACS_CS_LdapProxyHandler::start()!"));
	if(updateLdap() != 0)
	{
		ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "UPDATING LDAP AT STARTUP FAILED"));
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"updateLdap() failed at startup!"));
		return -1;
	}
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_DEBUG,"before addImplementer() in start()!"));
	//set Ldap implementer
	if (addImplementer() != 0)
	{
		ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "SETTING LDAP IMPLEMENTER FAILED"));
		return -1;
	}
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_DEBUG,"before IMM callback thread spawn in start()!"));
	immLdapWorker = new ACS_CS_IMMCallbackThread(getSelObj(), this);
	immLdapWorker->activate();

	return 0;
}

int ACS_CS_LdapProxyHandler::stop()
{
	if (immLdapWorker)
		immLdapWorker->deActivate();

	//Remove IMM Class Implementers and stop callback thread
	removeImplementer();

	if(ldapSecApi)
	{
		if(ldapSecApi->isSubscribed())
		{
			ldapSecApi->unsubscribe();
		}
		else {
                        if(ldapSecApi->nodeCredentialisSubscribed())
                                ldapSecApi->nodeCredentialUnsubscribe();

                        if(ldapSecApi->trustCategoryisSubscribed())
                                ldapSecApi->trustCategoryUnsubscribe();
                }
	}

	return 0;
}

ACS_CC_ReturnType ACS_CS_LdapProxyHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **/*attr*/)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectCreateCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " oiHandle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << className << endl;
	cout << " parent name: " << parentname << endl;

	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	if(updateLdap() == 0)
		result = ACS_CC_SUCCESS;

	return result;
}

ACS_CC_ReturnType ACS_CS_LdapProxyHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectDeleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;


	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	if(updateLdap() == 0)
		result = ACS_CC_SUCCESS;

	return result;
}

ACS_CC_ReturnType ACS_CS_LdapProxyHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          ObjectModifyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;
	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;
	cout << " object Name: " << objName << endl;

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	return result;
}

ACS_CC_ReturnType ACS_CS_LdapProxyHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbCompleteCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "COMPLETECallback " << endl;
	cout << endl;

	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	if(updateLdap() == 0)
		result = ACS_CC_SUCCESS;

	return result;
}

void ACS_CS_LdapProxyHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbAbortCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "ABORTCallback " << endl;
	cout << endl;
}

void ACS_CS_LdapProxyHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	cout << "---------------------------------------------------" << endl;
	cout << "          CcbApplyCallback invocated           " << endl;
	cout << "---------------------------------------------------" << endl;
	cout << " ObjectImplementer: " << getImpName() << endl;

	cout << " Imm Handle: " << oiHandle << endl;
	cout << " ccbId: " << ccbId << endl;

	cout << "APPLYCallback " << endl;
	cout << endl;

}

ACS_CC_ReturnType ACS_CS_LdapProxyHandler::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/)
{
	printf("------------------------------------------------------------\n");
	printf("                   updateRuntime called                     \n");
	printf("------------------------------------------------------------\n");

	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	if(updateLdap() == 0)
		result = ACS_CC_SUCCESS;

	return result;
}


void ACS_CS_LdapProxyHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**/*paramList*/) {

	printf("------------------------------------------------------------\n");
	printf("                   adminOperationCallback called             \n");
	printf("------------------------------------------------------------\n");

	cout<<"oiHandle   : "<< oiHandle <<endl;
	cout<<"invocation : "<< invocation <<endl;
	cout<<"p_objName  : "<< p_objName <<endl;
	cout<<"operationId: "<< operationId <<endl;
	cout << " Implementing Admin Operation with ID = " << operationId << endl;

}



int ACS_CS_LdapProxyHandler::updateLdap()
{
	int errorCode = 0;
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_INFO,"Entering updateLdap()"));

	//get values from IMM again
	errorCode = ldapSecApi->getLdapInfo();

	if(errorCode != 0)
	{
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Failed to retrieve Ldap(MO)information from IMM")); 
		return errorCode;
	}


	if(ldapSecApi->IsChange())
	{
		errorCode = 0;
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"No change! Don't update slapd.conf!!!"));
		return errorCode;
	}


	if((ldapSecApi->getNodeCredId() == "") && (ldapSecApi->getTrustCatId() == ""))
	{
		if(ldapSecApi->isSubscribed())
		{
			ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId & NodeCredentialId. So UnSubscribing..."));
			ldapSecApi->unsubscribe();
		}
		else {
                        if(ldapSecApi->nodeCredentialisSubscribed())
                        {
                                ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to NodeCredentialId. So UnSubscribing..."));
                                ldapSecApi->nodeCredentialUnsubscribe();
                        }
                        if(ldapSecApi->trustCategoryisSubscribed())
                        {
                                ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId. So UnSubscribing..."));
                                ldapSecApi->trustCategoryUnsubscribe();
                        }
                }
	}
	else
	{
		if((ldapSecApi->getNodeCredId()) == "" && (ldapSecApi->getTrustCatId() != ""))
                {
			bool unsubscribe = false;
                        if((ldapSecApi->getOldNodeCredId() != "") || (ldapSecApi->getOldTrustCatId() != ldapSecApi->getTrustCatId())) {
                                unsubscribe = true;
                        }
			if(unsubscribe) {
                                if(ldapSecApi->isSubscribed()) {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId & NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->unsubscribe();
                                }
                                if(ldapSecApi->trustCategoryisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId. So UnSubscribing..."));
                                        ldapSecApi->trustCategoryUnsubscribe();
                                }
                                if(ldapSecApi->nodeCredentialisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->nodeCredentialUnsubscribe();
                                }
                        }
			if(unsubscribe) {
                                int fd = ldapSecApi->trustCategorySubscribeMoRef(ldapSecApi->getTrustCatId());
                                if(fd != -1)
                                {
                                        ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "subscribe to Sec Api for trustCategory Success"));
                                }
                        }
                        
			errorCode = ldapSecApi->writeToSlapd();
                        if(errorCode != 0)
                        {
                                ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"writing data to slapd.conf failed"));
                                return errorCode;
                        }
                }
		
		else if((ldapSecApi->getNodeCredId()) != "" && (ldapSecApi->getTrustCatId() == ""))
                {
                        bool unsubscribe = false;
                        if((ldapSecApi->getOldTrustCatId() != "") || (ldapSecApi->getOldNodeCredId() != ldapSecApi->getNodeCredId())) {
                                unsubscribe = true;
                        }
                        if(unsubscribe) {
                                if(ldapSecApi->isSubscribed()) {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId & NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->unsubscribe();
                                }
                                if(ldapSecApi->trustCategoryisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId. So UnSubscribing..."));
                                        ldapSecApi->trustCategoryUnsubscribe();
                                }
                                if(ldapSecApi->nodeCredentialisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->nodeCredentialUnsubscribe();
                                }
                        }
			if(unsubscribe) {
                                int fd = ldapSecApi->nodeCredentialSubscribeMoRef(ldapSecApi->getNodeCredId());
                                if(fd != -1)
                                {
                                        ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "subscribe to Sec Api for nodeCredential Success"));
                                }
                        }

			errorCode = ldapSecApi->writeToSlapd();
                        if(errorCode != 0)
                        {
                                ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"writing data to slapd.conf failed"));
                                return errorCode;
                        }
                }

		else
		{
                        bool unsubscribe = false;
                        if((ldapSecApi->getOldNodeCredId() != ldapSecApi->getNodeCredId()) || (ldapSecApi->getOldTrustCatId() != ldapSecApi->getTrustCatId())) {
                                unsubscribe = true;
                        }
                        if(unsubscribe) {
                                if(ldapSecApi->isSubscribed()) {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId & NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->unsubscribe();
                                }
                                if(ldapSecApi->trustCategoryisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to TrustCategoryId. So UnSubscribing..."));
                                        ldapSecApi->trustCategoryUnsubscribe();
                                }
                                if(ldapSecApi->nodeCredentialisSubscribed())
                                {
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Subscribed to NodeCredentialId. So UnSubscribing..."));
                                        ldapSecApi->nodeCredentialUnsubscribe();
                                }
                        }
			if(unsubscribe) {
                                int fd = ldapSecApi->subscribeMoRef(ldapSecApi->getNodeCredId(),ldapSecApi->getTrustCatId());
                                if(fd != -1)
                                {
                                        ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "subscribe to Sec Api for nodeCredential and trustCategory Success"));

                                }
                                else
                                {
                                        ACS_CS_TRACE((ACS_CS_Ldap_TRACE, "subscribed to Sec Api for nodeCredential and trustCategory Failed"));
                                        ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Failed to subscribe to SEC API for nodeCredential and trustCategory changes"));
                                        return fd;              //Fix for TR HY61698
                                }
                        }
	
			errorCode = ldapSecApi->writeToSlapd();
			if(errorCode != 0)
			{
				ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"writing data to slapd.conf failed"));
				return errorCode;
			}
		}
	}

	return errorCode;
}

int ACS_CS_LdapProxyHandler::addImplementer()
{
	int result = 0;

	setImpName(ACS_CS_ImmMapper::IMPLEMENTER_LDAP);


	setScope(ACS_APGCC_ONE);

	result = oiHandler.addClassImpl(this, this->getIMMClassName());

	if(ACS_CC_SUCCESS != result)
	{
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Error occurred while setting LDAP PROXY implementer %d - %s ",
				getInternalLastError(), getInternalLastErrorText()));

		if (imm_error_code::ERR_IMM_ALREADY_EXIST == getInternalLastError())
		{
			removeImplementer();
		}

		result = -1;
	}
	else
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Successfully set the ACS_CS_LdapProxyHandler implementer"));

	return result;
}

int ACS_CS_LdapProxyHandler::removeImplementer()
{
	if(ACS_CC_SUCCESS != oiHandler.removeClassImpl(this, this->getIMMClassName()))
	{
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Error occurred while removing LDAP PROXY implementer %d - %s ",
				getInternalLastError(), getInternalLastErrorText()));

		return -1;
	}

	return 0;
}

int ACS_CS_LdapProxyHandler::implementerRegister()		//TR HW99171
{
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Entering implementerRegister()"));
	removeImplementer();
	if (immLdapWorker)
	{
		delete immLdapWorker;
		immLdapWorker = 0;
	}
	//set Ldap implementer
	if (addImplementer() != 0)
	{
		ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"implementerRegister() - Failed to set the ACS_CS_LdapProxyHandler implementer"));
		return -1;
	}
	immLdapWorker = new ACS_CS_IMMCallbackThread(getSelObj(), this);
	immLdapWorker->activate();
	ACS_CS_FTRACE((ACS_CS_Ldap_TRACE,LOG_LEVEL_ERROR,"Leaving implementerRegister()"));
	return 0;
}
