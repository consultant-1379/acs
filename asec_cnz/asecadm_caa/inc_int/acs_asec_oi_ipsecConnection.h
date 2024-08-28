/*
 * * @file fms_cpf_oi_compositefile.h
 *	@brief
 *	Header file for FMS_CPF_OI_IpsecConnection class.
 *  This module contains the declaration of the class FMS_CPF_OI_IpsecConnection.
 *
 *	@author enungai
 *	@date 2019-03-22
 *	@version 1.0.0
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
 *	| 1.0.0  | 2019-03-22 | enungai      | File created.                       |
 *      +========+============+==============+=====================================+
 *      | 2.0.0  | 2020-04-23 | xsowpol      |leftsubnet and rightsubnet parameters|
 *      |        |            |              |are included in the conf file.       |
 *      +========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ASEC_OI_IPSECCONN_H_
#define ACS_ASEC_OI_IPSECCONN_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */


#include "acs_apgcc_objectimplementerinterface_V3.h"

#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "acs_asec_trace.h"

#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"

#include <sec_credu_api.h>
#include <sec/crypto_status.h>
#include <sec/crypto_api.h>

#include <vector>
#include <map>

#include <ace/Task.h>


namespace
{
	const int SUCCESS = 0;
	const int FAILURE = -1;
	const int INVALID = -1;
};
using namespace std;


/*===================================================================
                        CLASS FORWARD SECTION
=================================================================== */
class ACS_TRA_trace;


/*===================================================================
                        CLASS DECLARATION SECTION
=================================================================== */
class Certificate
{
	std::string _filename;
	std::string _pem; // base64 encoded

	public:
	Certificate() {}
	Certificate(const std::string& filename, const std::string& pem):_filename(filename),_pem(pem) {}
	const std::string& getFilename() const
	{
		return _filename;
	}
	void clear();
};

class ACS_ASEC_OI_IpsecConnection :  public acs_apgcc_objectimplementerinterface_V3
{
	public:
	/**	@brief	constructor of ACS_ASEC_OI_IpsecConnection class
	 */
	ACS_ASEC_OI_IpsecConnection();
	/**	@brief	destructor of ACS_ASEC_OI_IpsecConnection class
	 */
	virtual ~ACS_ASEC_OI_IpsecConnection();
	/** @brief create method
	 *
	 *	This method will be called as a callback when an Object is created as instance of a Class IpsecConnection
	 *	All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 *	proper actions.
	 *
	 *	@param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param ccbId: the ID for the Configuration Change Bundle in which the creation of the Object is contained.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param className: the name of the class. When an object is created as instance of this class this method is
	 *	called if the application has registered as class implementer. This is an Input Parameter provided by IMMSV Application.
	 *	@param parentname: the name of the parent object for the object now creating.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues element pointers each one containing
	 *	the info about the attributes belonging to the now creating class.
	 *	This is an Input Parameter provided by IMMSV Application.
	 *
	 *	@return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 */
	virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	/** @brief deleted method
	 *
	 * This method will be called as a callback when deleting a IpsecConnection Object
	 * Object Implementer. All input parameters are input provided by IMMSV Application and have to be used by
	 * the implementer to perform proper actions.
	 *
	 * @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param ccbId: the ID for the Configuration Change Bundle in which the deletion of the Object is contained.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param objName: the Distinguished name of the object that has to be deleted.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 * @remarks Remarks
	 */
	virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	/** @brief modify method
	 *
	 * This method will be called as a callback when modifying a IpsecConnection Object
	 * All input parameters are input provided by IMMSV Application and have to be used by the implementer to perform
	 * proper actions.
	 *
	 * @param oiHandle: the object implementer handle.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param objName: the Distinguished name of the object that has to be modified.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements containing
	 * 					the information about the modify to perform. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 * @remarks Remarks
	 */

	virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	/** @brief complete method
	 * This method will be called as a callback when a Configuration Change Bundle is complete and can be applied
	 * regarding a IpsecConnection Object
	 *
	 * @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param ccbId: the ID for the Configuration Change Bundle in which the modify of the Object is contained.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 * @remarks Remarks
	 */

	virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief abort method
	 *
	 * This method will be called as a callback when a Configuration Change Bundle, regarding a IpsecConnection Object,
	 * has aborted. This method is called only if at least one complete method failed.
	 *
	 * @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 * registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 * @remarks Remarks
	 */
	virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief apply method
	 *
	 * This method will be called as a callback when a Configuration Change Bundle, regarding a IpsecConnection Object, is complete and can be applied.
	 * This method is called only if all the complete method have been successfully executed.
	 *
	 * @param oiHandle: the object implementer handle. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param ccbId: the ID for the Configuration Change Bundle containing actions on Objects for which the Application
	 * registered as Object Implementer. This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 * @remarks Remarks
	 */
	virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	/** @brief updateRuntime method
	 * This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
	 * for which the Application has registered as Object Implementer.
	 *
	 * All input parameters are input provided by IMMSV Application and have to be used by the implementer
	 * to perform proper actions.
	 *
	 * @param p_objName: the Distinguished name of the object that has to be modified.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @param p_attrName: the name of attribute that has to be modified.
	 * This is an Input Parameter provided by IMMSV Application.
	 *
	 * @return ACS_CC_ReturnType On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE.
	 *
	 * @remarks Remarks
	 */
	virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);
	/*
	 * @brief adminOperationCallback method
	 * adminOperationCallback method: This method will be called as a callback to manage an administrative operation invoked, on the
	 * Simple CP File object.
	 *
	 * @param  p_objName:	the distinguished name of the object for which the administrative operation
	 * has to be managed.
	 *
	 * @param  oiHandle : input parameter,ACS_APGCC_OiHandle this value has to be used for returning the callback
	 * result to IMM.
	 *
	 * @param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation
	 * of result function
	 *
	 * @param  p_objName: input parameter,the name of the object
	 *
	 * @param  operationId: input parameter, the administrative operation identifier
	 *
	 * @param paramList: a null terminated array of pointers to operation params elements. each element of the list
	 * is a pointer toACS_APGCC_AdminOperationParamType element holding the params provided to the Administretive operation.
	 *
	 * @return ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 *
	 */
	virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);

	struct secDetails{
		SecCreduTrustCategory *_tCategory;
		SecCreduHandle _secHandle;
		SecCreduSubscription _trustcategorySubscription;
		SecCreduSubscription _nodecredentialSubscription;
		bool m_isSubscribed;
	};

	static bool getImmAttribute (std::string object, std::string attribute, std::string &value);
	bool executeCommand(const std::string& command, std::string& output);
	void fetchNodeCert();
	void fetchTrustCategory();
	static void nodeCredentialChangeCallback(SecCreduHandle handle, SecCreduSubscription nodecredential_subscription, const char *nodecredential_id);
	static void trustCategoryChangeCallback(SecCreduHandle handle, SecCreduSubscription trustcategory_subscription, const char *trustcategory_id);
	bool isSubscribed(secDetails detailsUnSub);
	int subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId);
	string getIdValueFromRdn(const string & rdn);
	int unsubscribe(secDetails detailsUnSub);
	static ACS_ASEC_OI_IpsecConnection* getInstance();
	std::string getNodeCertFileName() const;
	std::string getNodeKeyFileName() const;
	std::string getTrustCatDirName() const;
	std::string getTrustCatId() const;
	std::string getNodeCredId() const;
	ACS_CC_ReturnType makeIkev2Connection(vector<string> ipsec_vector,int v_nodeCredentialId,int v_trustCategoryId);
	ACS_CC_ReturnType removeIkev2Connection(vector<string> ipsec_vector,int v_nodeCredentialId,int v_trustCategoryId);
	bool createSoftLinks(string conectionName);
	bool deleteSoftLinks(string connectionName);
	void clearIkev2Configuration();
	int updateIkev2Connections(string connectionName);
	void writeToIpsecConfFiles(string authentication, vector<string> ikev2_vector);
	void clearAllSoftLinks();
	std::string calculateIpv4Subnet(string ipAddress);
	std::string calculateIpv6Subnet(string ipAddress);
	string p_fromCacert,p_fromCerts, p_fromKey;

	private:

	ACS_TRA_Logging ASEC_Log;
	ACS_TRA_trace* acs_asec_oi_ipsecConnectionTrace;
	static ACS_ASEC_OI_IpsecConnection *Ipsecinstance;

	stringstream p_nodeIp;
	string p_toCacert, p_toCerts, p_toPrivate;

	bool _updated;
	bool m_isSubscribed;

	std::string _nodeCredentialId;
	std::string _trustCategoryId;
	std::string _trustCatDirName;
	std::string m_tlsCaCertDir;
	std::string m_nodeCredential;
	std::string m_tlsCategory; 

	SecCreduTrustCategory *_tCategory;
	SecCreduHandle _secHandle;
	SecCreduSubscription _trustcategorySubscription;
	SecCreduSubscription _nodecredentialSubscription;

	int _fd;
	int unsubscribeFlag;

	std::vector<Certificate> _trustCategory;
	Certificate _nodeCredentialCert;
	Certificate _nodeCredentialKey;
	std::map<string,secDetails> secConnDetails;
};

#endif /* ACS_ASEC_OI_IPSECCONN_H_ */
