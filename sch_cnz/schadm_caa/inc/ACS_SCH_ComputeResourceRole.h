//=============================================================================
/**
 *  @file    ACS_SCH_ComputeResourceRole.h
 *
 *  @copyright  Ericsson AB, Sweden 2017. All rights reserved.
 *
 *  @author 2017-04-11 by XBHABHE
 *
 *  @documentno
 *
 *  @brief  It acts as OI for handling IMM Callbacks.
 *
 */
//=============================================================================
/*Include Section */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include "acs_apgcc_oihandler_V3.h"
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include "acs_apgcc_objectimplementereventhandler_V3.h"
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ACS_CS_API.h"


/**
 * @brief IMMErrorRuntimeHandler
 * IMMErrorRuntimeHandler class is extended from ACS_APGCC_RuntimeOwner.
 * It is having functions to handle IMM callbacks
 *
 */
/*class IMMErrorRuntimeHandler : public ACS_APGCC_RuntimeOwner_V2
{
public:
        IMMErrorRuntimeHandler() { }
        inline ~IMMErrorRuntimeHandler(){}
        ACS_CC_ReturnType updateCallback(const char* objName, const char** ImplName)
        {
                return ACS_CC_SUCCESS;
        }

};*/
typedef std::vector<unsigned short> CpIdentityList;
class ACS_SCH_ComputeResourceRole: public acs_apgcc_objectimplementereventhandler_V3, public ACE_Task_Base
{
public:
	/**
	 * @brief
	 * constructor with three parameters
	 */
	ACS_SCH_ComputeResourceRole( std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
	ACS_SCH_ComputeResourceRole( std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr);

	/**
	 * @brief
	 * destructor
	 */
	~ACS_SCH_ComputeResourceRole();
	/**
	 *  create method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object created.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  className     : const char pointer
	 *  @param  parentname    : const char pointer
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
	/**
	 *  deleted method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object deleted.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	/**
	 *  complete method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  abort method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  apply method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

	//int handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext);
	/**
	 *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 *      and registers the callback function.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	ACE_INT32 svc();

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	std::string theClassName;

	int  addImplementer(std::string szImpName);
	int removeImplementer();
	acs_apgcc_oihandler_V3 *theCRRHandler;
	ACE_Reactor* theReportReactorPtr;
	ACE_TP_Reactor* theReportReactorImplPtr;
	struct timespec req,rem;
	static string _threadParam;
	ACE_INT32 shutdown();

private:

	//IMMErrorRuntimeHandler* theErrorRuntimeHandlerPtr; COVERITY: 12553
	/*!
	 * @brief Deletes the error handler object
	 * @return TRUE after successful deletion
	 * @return FALSE in case there is error
	 */
	bool deleteErrorHandlerObject();
	/*!
	 * @brief Creates the error handler object
	 * @param  ACS_APGCC_CcbId      :       Error Object ID
	 * @param  aErrorId                     :       Error ID
	 * @param  aErrorText           :       Error Text
	 * @return TRUE after successful deletion
	 * @return FALSE in case there is error
	 */
	bool createErrorHandlerObject(ACS_APGCC_CcbId& aCCBId,unsigned int& aErrorId,std::string & aErrorText);
	/*!
	 * @brief Creates the Sets the error text
	 * @param  ACS_APGCC_CcbId      :       Error Object ID
	 * @param  aErrorId                     :       Error ID
	 * @param  aErrorText           :       Error Text
	 * @return TRUE after successful deletion
	 * @return FALSE in case there is error
	 */
	bool setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText);
	bool getBCs(vector<string>& bcRoles,string role_type);
	bool updateRoleObject(vector<string> &bcRoles, string role_type);
	int scaleinPreconditions(string objName);
	int getBladeCount();
	bool isQuorumExist();
	int getBladeStatus(string objName);
	void setErrorCode(unsigned int code);
	ACS_CC_ReturnType triggerProcedure(string dnName);
	ACS_CC_ReturnType spawnScalingProcedureThread(string dn, int procedure);

	static void rollback(void *dn);
	static void apgCleanup(string dnName);
	static ACS_CC_ReturnType deleteCRRObject(string objName);
	static void cleanup(void *dn);
	static void scalein(void *dn);
	static void scaleinFailed(string objName);
	static void scaleoutFinalize(void *dn);

	bool m_isObjImplAdded;
	OmHandler immHandle;
	char errorText[128];

};

