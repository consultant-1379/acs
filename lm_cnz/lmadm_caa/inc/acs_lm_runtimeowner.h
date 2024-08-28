#ifndef _ACS_LM_RUNTIMEOWNER_H_
#define _ACS_LM_RUNTIMEOWNER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Event.h>
#include "acs_lm_common.h"
#include "acs_lm_cmdserver.h"
#include "acs_lm_scheduler.h"
#include "acs_lm_cprestartmonitor.h"
#include "acs_lm_eventhandler.h"
#include "acs_lm_clienthandler.h"
#include <saImmOi.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <acs_apgcc_objectimplementerinterface_V2.h>
#include <acs_apgcc_oihandler_V2.h>
#include "acs_lm_electronickey_runtimeowner.h"
/*===================================================================*/
/**
      @brief  The ACS_LM_RuntimeOwner class handle runtime
                          object creation in IMM.
*/
/*=================================================================== */

class ACS_LM_RuntimeOwner : public ACS_APGCC_RuntimeOwner_V2
{
	/*=====================================================================
                                PUBLIC DECLARATION SECTION
     ==================================================================== */
public:
	/*===================================================================
		   						  PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
		   							   PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_RuntimeOwner

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_RuntimeOwner();
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_RuntimeOwner

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_RuntimeOwner();
	/*=================================================================== */
	/**
	 * @brief updateCallback               : This is a base class method.
	 * 										 This method will be called
	 * 										 as a callback when an non-cached attribute
	 * 										 of a runtime Object is updated.
	 *
	 * @param  objName					   :  const char pointer
	 *
	 * @param  attrName					   : const char**
	 *
	 * @return ACS_CC_ReturnType           : On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	 */
	/*=================================================================== */
	ACS_CC_ReturnType updateCallback(const char* objName, const char** attrName);
	/*=================================================================== */
	/**
	 *  @brief adminOperationCallback
	 *
	 *  @param  oiHandle      				:	ACS_APGCC_OiHandl
	 *  e
	 *  @param  invocation    				: ACS_APGCC_InvocationType
	 *
	 *  @param  p_objName     				: const char pointer
	 *
	 *  @param  operationId   				: ACS_APGCC_AdminOperationIdType
	 *
	 *  @param  paramList     				: ACS_APGCC_AdminOperationParamType
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	/*=================================================================== */
	/**
	 *  @brief setClienthandler
	 *
	 *  @param  aClienthandler      		:	ACS_LM_ClientHandler
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void setClienthandler(ACS_LM_ClientHandler* aClienthandler);
	/*=================================================================== */
	/**
	 *  @brief setElectronicKeyRunTimeOwnerPointer
	 *
	 *  @param  aElectronicKeyLMRuntimeOwnerPtr      :	ACS_M_ElectronicKey_RuntimeOwner
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void setElectronicKeyRunTimeOwnerPointer(ACS_LM_ElectronicKey_RuntimeOwner* aElectronicKeyLMRuntimeOwnerPtr);
	/*=================================================================== */
	/**
	 *  @brief initLMRunTimeOwner
	 *
	 *  @return bool 								: true/false
	 */
	/*=================================================================== */
	bool initLMRunTimeOwner();
	/*=================================================================== */
	/**
	 *  @brief finalizeLMRunTimeOwner
	 *
	 *  @return bool 								: true/false
	 */
	/*=================================================================== */
	bool finalizeLMRunTimeOwner();
	/*=================================================================== */
	/**
	 *  @brief setInternalOMhandler
	 *
	 *  @param aOmHandlerPtr 						: OmHandler
	 *
	 *  @return void
	 */
	/*=================================================================== */
	void setInternalOMhandler(OmHandler*  aOmHandlerPtr);
	/*=================================================================== */
private:
	/*===================================================================
									 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
									 PRIVATE METHOD
	 =================================================================== */
	ACS_LM_ClientHandler* theClienthandlerPtr;
	OmHandler*  theOmHandlerPtr;
	ACS_LM_ElectronicKey_RuntimeOwner * theElectronicKeyLMRuntimeOwnerPtr;
	/*=================================================================== */
	/**
	 *   @brief getMonthString
	 *
	 *  @param aMonth						 : int
	 *
	 *  @return string
	 */
	/*=================================================================== */
	std::string getMonthString(int aMonth);
	/*=================================================================== */
	/**
	 *   @brief setErrorText
	 *
	 *  @param aErrorId						 : unsigned int
	 *
	 *	@param ErrorText					  :string
	 *
	 *  @return bool
	 */
	/*=================================================================== */
	bool setErrorText(unsigned int aErrorId, std::string ErrorText);
};

class ACS_LM_RuntimeHandlerThread : public ACE_Task_Base
{
public :
	/*===================================================================
		   						  PUBLIC ATTRIBUTE
	=================================================================== */
	int m_StopEvent;

	/*===================================================================
		   							   PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_LM_RuntimeHandlerThread

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_RuntimeHandlerThread();

	/*=================================================================== */
	/**
				@brief       Parameterised constructor for ACS_LM_RuntimeHandlerThread

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_RuntimeHandlerThread(ACS_LM_RuntimeOwner *pImpl);
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_RuntimeHandlerThread

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_RuntimeHandlerThread();
	/*=================================================================== */
	/** 	@brief setImpl  					:setImpl for setting implementer
	 *
	 * 		@param 	pImpl						:ACS_LM_RuntimeOwner
	 *
	 *      @return 							:void
	 */
	/*=================================================================== */
	void setImpl(ACS_LM_RuntimeOwner *pImpl);
	/*=================================================================== */

	/** 	@brief stop							:stop for stopping the thread
	 *
	 *      @return 							:void
	 */
	/*=================================================================== */
	bool stop();
	/*=================================================================== */
	/**
	 *  @brief svc
	 *
	 *  @return int
	 */
	/*=================================================================== */
	int svc(void);
	/*=================================================================== */


private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	ACS_LM_RuntimeOwner *implementer;
	bool isStop;


};
// ACS_LM_RuntimeHandlerThread END

#endif
