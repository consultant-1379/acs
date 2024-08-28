/*
 * * @file fms_cpf_oi_compositefile.h
 *	@brief
 *	Header file for FMS_CPF_OI_GsnhConnection class.
 *  This module contains the declaration of the class FMS_CPF_OI_GsnhConnection.
 *
 *
 *	COPYRIGHT Ericsson AB, 2019
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ASEC_OI_GSNHCONN_H_
#define ACS_ASEC_OI_GSNHCONN_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */


#include "acs_apgcc_objectimplementerinterface_V3.h"

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "acs_asec_trace.h"
#include <sec_credu_api.h>
#include <sec/crypto_status.h>
#include <sec/crypto_api.h>
#include <stdlib.h>
#include <syslog.h>

namespace gsnhConnectionClass{
    const char ImmImplementerName[] = "AESC_OI_GsnhConnection";
    const char dnName[]="asecGsnhConfigDataId=GSNH,acsSecurityMId=1";
    const char ATTR_ISRESTARTED[] = "isRestartTriggered";
    const char ATTR_CONFTLSVERSION[] = "confTLSversion";
    const char ATTR_CONFTLSCIPHERS[] = "confCiphersList";
}


// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

//ERROR CODES
#define EXIT_SSL_NOT_VALID 10 
#define EXIT_TRUSTED_CERTIFICATE_NOT_VALID 13
#define EXIT_GENERAL_FAULT 2




/*===================================================================
                        CLASS FORWARD SECTION
=================================================================== */
class ACS_TRA_trace;

class GsnhCertificate
{
        std::string _filename;
        std::string _pem; // base64 encoded
public:
        GsnhCertificate() {}
        GsnhCertificate(const std::string& filename, const std::string& pem):_filename(filename),_pem(pem) {}



        const std::string& getFilename() const {
                return _filename;
        }


        void clear();
};
/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class ACS_ASEC_OI_GsnhConnection :  public acs_apgcc_objectimplementerinterface_V3
{
	public:


		/**		@brief	constructor of ACS_ASEC_OI_GsnhConnection class
		 */
		//ACS_ASEC_OI_GsnhConnection(string szObjName,string szImpName,ACS_APGCC_ScopeT enScope);
		ACS_ASEC_OI_GsnhConnection();

		/**		@brief	destructor of ACS_ASEC_OI_GsnhConnection class
		 */
		virtual ~ACS_ASEC_OI_GsnhConnection();

		/** @brief create method
		 *
		 *	This method will be called as a callback when an Object is created as instance of a Class GsnhConnection
		 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
		 *	proper actions.
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle in which the creation of the Object is contained.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param className: the name of the class. When an object is created as instance of this class this method is
		 *	called if the application has registered as class implementer. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param parentname: the name of the parent object for the object now creating.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
		 *	the info about the attributes belonging to the now creating class.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 */
		virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

		/** @brief deleted method
		 *
		 *	This method will be called as a callback when deleting a GsnhConnection Object
		 *	Object Implementer. All input parameters are input provided by IMMSV Application and have to be used by
		 *	the implementer to perform proper actions.
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle in which the deletion of the Object is contained.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param objName: the Distinguished name of the object that has to be deleted.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

		/** @brief modify method
		 *
		 *	This method will be called as a callback when modifying a GsnhConnection Object
		 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
		 *	proper actions.
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param objName: the Distinguished name of the object that has to be modified.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements containing
		 *	the information about the modify to perform. This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

		/** @brief complete method
		 *
		 *	This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
		 *	regarding a GsnhConnection Object
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
		 *	This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/** @brief abort method
		 *
		 *	This method will be called as a callback when a Configuration Change Bundle, regarding a GsnhConnection Object,
		 *	has aborted. This method is called only if at least one complete method failed.
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
		 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/** @brief apply method
		 *
		 *	This method will be called as a callback when a Configuration Change Bundle, regarding a GsnhConnection Object, is complete and can be applied.
		 *	This method is called only if all the complete method have been successfully executed.
		 *
		 *  @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
		 *	registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/** @brief updateRuntime method
		 *
		 * 	This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
		 * 	for which the Application has registered as Object Implementer.
		 * 	All input parameters are input provided by IMMSV Application and have to be used by the implementer
		 * 	to perform proper actions.
		 *
		 *  @param p_objName: the Distinguished name of the object that has to be modified.
		 *  				  This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @param p_attrName: the name of attribute that has to be modified.
		 *  				   This is an Input Parameter provided by IMMSV Application.
		 *
		 *  @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);



		/**
		 * @brief adminOperationCallback method
		 * adminOperationCallback method: This method will be called as a callback to manage an administrative operation invoked, on the
		 * Simple CP File object.
		 *
		 * @param  p_objName:	the distinguished name of the object for which the administrative operation
		 * has to be managed.
		 *
		 * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
		 *  result to IMM.
		 *
		 * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
		 * of result function
		 *
		 * @param  p_objName: input parameter,the name of the object
		 *
		 * @param  operationId: input parameter, the administrative operation identifier
		 *
		 * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
		 * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation..
		 *
		 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
		 */
		virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);
		/**
		 * @brief writeToSslConf method
		 * writeToSslConf method: This method will be called to update ssl.conf file with needed information for Apache Web server configuration
		 *
		 * @NodeCertFileName: A string type value fetched from SecApi that holds NodeCertFileName with path
		 *
		 * @NodeKeyFileName: A string type value fetched from SecApi that holds NodeKeyFileName with path
		 *
		 * @TrustCatFileName: A string type value fetched from SecApi that holds TrustCatFileName with path
		 *
		 */
		void writeToSslConf(string NodeCertFileName ,string NodeKeyFileName,string TrustCatFileName,string confTLSVersion, string confTLSCiphers);	

		/**
		 * @brief executeCmd method
		 * executeCmd method: This method will be called to execute OS command
		 *
		 * @command: A string type value holding the command to be executed
		 *
		 * @return boolen:  On success true,on Failure false
		 */
		bool executeCmd(string command);

		/**
		 * @brief restartApacheServer method
		 * restartApacheServer method: This method will be called to Restart Apache web server
		 *
		 * @return boolen:  On success true,on Failure false
		 */
		void restartApacheServer();

		/**
		 * @brief fetchGsnhImmattribs method
		 * fetchGsnhImmattribs method: This method will be called to fetch GSNH related Attribute values froom IMM
		 *
		 * @dnName: A string type value holding GSNH DN Name used to fetch IMM attributes 
		 *
		 * @return boolen:  On success true,on Failure false
		 */
		bool fetchGsnhImmattribs(string dnName);

		/**
		 * @brief performGsnhOperaions method
		 * performGsnhOperaions method: This method will be called to perform Gsnh security related operations 
		 *
		 */
		void performGsnhOperaions();

		/**
		 * @brief validateNodeCertFile method
		 * validateNodeCertFile method: This method will be called to validate nodeCredential Certificate file
		 *
		 * @FileName: A string type value holding the Certificate that need to validated
		 *
		 */
		void validateNodeCertFile(string FileName);
		/**
		 * @brief validateTrustCatFile method
		 * validateTrustCatFile method: This method will be called to validate Trustcategory Certificate file
		 *
		 * @FileName: A string type value holding the Certificate that need to validated
		 *
		 */
		void validateTrustCatFile(string FileName);

		/**
		 * @brief validateKeyFile method
		 * validateKeyFile method: This method will be called to validate Node credential Key file
		 *
		 * @FileName: A string type value holding the Node credential Key file that need to validated
		 *
		 */
		void validateKeyFile(string FileName);

		/**
		 * @brief fetchNodeCert method
		 *
		 * fetchNodeCert method: This method will be called to fetch node Cerdential filename
		 *
		 */
		void fetchNodeCert();

		/**
		 * @brief fetchTrustCategory method
		 * fetchTrustCategory method: This method will be called to execute OS command
		 */
		void fetchTrustCategory();

		static void nodeCredentialChangeCallback(
				SecCreduHandle handle, SecCreduSubscription nodecredential_subscription, const char *nodecredential_id);

		static void trustCategoryChangeCallback(
				SecCreduHandle handle, SecCreduSubscription trustcategory_subscription, const char *trustcategory_id);

		/**
		 * @brief getInstance method
		 * getInstance method: This method will be called to get the instance of  ACS_ASEC_OI_GsnhConnection class
		 *
		 * @return ACS_ASEC_OI_GsnhConnection:  returns ACS_ASEC_OI_GsnhConnection class instance
		 */
		static ACS_ASEC_OI_GsnhConnection* getInstance();

		/**
		 * @brief finalize method
		 * finalize method: This method will be called to delete  ACS_ASEC_OI_GsnhConnection instance
		 */
		static void finalize();
		/**
		 * @brief isSubscribed method
		 * @returns: true if the already subscribed to Sec API ,false otherwise
		 */
		bool isSubscribed() const;

		/**
		 * @brief subscribe method
		 * subscribe method: Subscribe to Sec Api
		 * @param nodeCredentialId the InstanceId you are interested in
		 * @param trustCategoryId the InstanceId you are interested in
		 * @return The file descriptor used for asynchronous callbacks, or -1
		 */
		int subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId);

		/**
		 * @brief unsubscribe
		 * unsubscribe method: ends the subscription
		 */
		void unsubscribe();
		/**
		 * @return A vector containing all the public keys of the trust category
		 */
		const std::vector<GsnhCertificate>& getTrustCategory() const;

		/**
		 * @return The public key of the node credential
		 */
		const GsnhCertificate& getNodeCredentialCert() const;

		/**
		 * @return The private key of the node credential
		 */
		const GsnhCertificate& getNodeCredentialKey() const;

		/**
		 * @return The cert filename of the node credential
		 */
		std::string getNodeCertFileName() const;
		/**
		 * @return The cert key filename of the node credential
		 */
		std::string getNodeKeyFileName() const;
		/**
		 * @return The trustcert Dir name 
		 */
		std::string getTrustCatDirName() const;
		/**
		 * @return The trustcert File name 
		 */
		std::string getTrustCatFileName() const;

		//static variable to hold instance of ACS_ASEC_OI_GsnhConnection class
		static ACS_ASEC_OI_GsnhConnection *gsnhinstance;
		bool isSecurityEnabled();
		
                // GSNH security enhancements feature
                bool setIsRestartTriggered(int isRestartTriggered);
                bool clearConfTLSversion();
                bool clearConfTLSciphers();

	private:

		bool is_security_toggled;	
		std::string m_NodeCredentialId;
		std::string m_TrustCategoryId;
		char m_security[8];
		ACS_ASEC_OI_GsnhConnection *pinstance;
		ACS_TRA_Logging ASEC_Log;
		ACS_TRA_trace* acs_asec_oi_gsnhConnectionTrace;
		SecCreduHandle _secHandle;
		SecCreduSubscription _nodecredentialSubscription;
		SecCreduSubscription _trustcategorySubscription;
		SecCreduTrustCategory *_tCategory;
		std::string _nodeCredentialId;
		std::string _trustCategoryId;

		int _fd;
		std::vector<GsnhCertificate> _trustCategory;
		GsnhCertificate _nodeCredentialCert;
		GsnhCertificate _nodeCredentialKey;
		std::string _trustCatDirName;
		std::string _trustCatFileName;
                //To update PrivateTmp value in apache.service file
		void updateApacheServiceFile();



		static ACS_ASEC_OI_GsnhConnection *gsnhInstance;
		bool _updated;

		// GSNH enhancements feature
		std::string m_EnabledTLSVersion;
		std::string m_EnabledTLSCiphers;
		bool m_IsRestartTriggered;
		bool modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter);
		ACS_CC_ImmParameter defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);
		ACS_CC_ImmParameter defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue);
                std::string get_TLS_string(std::string tls_str);
};


#endif /* ACS_ASEC_OI_GSNHCONN_H_ */
