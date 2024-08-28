//=============================================================================
/**
   @file    acs_hcs_jobbased_objectImpl.h

   @brief Header file for HC module.
          It acts as OI for handling IMM Callbacks of HC jobbased object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/12/2012   XMALRAO   Initial Release
 */
//=============================================================================
#ifndef ACS_HC_JOBBASED_OBJECTIMPL_H
#define ACS_HC_JOBBASED_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "aes_ohi_extfilehandler2.h"
#include "acs_hcs_tra.h"
#include "ACS_APGCC_RuntimeOwner_V2.h"

/*============================================================================
				STRUCT FOR PARAMETERS
==============================================================================*/
struct JOB_Parameters
{
	char Categories[128];
	char JobToTrigger[128];
	char Target[32];
	char TransferQueue[128];
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief acs_hcs_jobbased_ObjectImpl
 * acs_hcs_jobbased_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */
class acs_hcs_jobbased_ObjectImpl:public acs_apgcc_objectimplementereventhandler_V3 ,public ACE_Task_Base  
{
	/*=====================================================================
		                       PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
		/*=====================================================================
					       CLASS CONSTRUCTOR
		 ==================================================================== */
		/**
		 * @brief
		 * constructor with two parameters
		 */
		acs_hcs_jobbased_ObjectImpl( string ClassName, string p_impName, ACS_APGCC_ScopeT p_scope );

		/*=====================================================================
					   CLASS DESTRUCTOR
		  ==================================================================== */
		/**
		* @brief
		* destructor
		*/
		~acs_hcs_jobbased_ObjectImpl();

		/**
		 *  create method: This method is inherited from base class and overridden by our class.
		 *  This method is get invoked when IMM object created.
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
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
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @param  objName
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

		/**
		 *  modify method: This method is inherited from base class and overridden by our class.
		 *  This method is get invoked when IMM object's attribute modify.
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @param  objName
		 *  @param  attrMods          : ACS_APGCC_AttrValues
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

		/**
		 *  complete method: This method is inherited from base class and overridden by our class.
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**
		 *  abort method: This method is inherited from base class and overridden by our class.
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 */
		void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**
		 *  apply method: This method is inherited from base class and overridden by our class.
		 *  @param  oiHandle      :	ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 */
		void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**
		 *  updateRuntime method: This method is inherited from base class and overridden by our class.
		 *  @param  objName          : const char pointer
		 *  @param  attrName         : const char pointer
		 */
		ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

		/**
                 *  adminOperationCallback method: This method is invoked when operator executes an action
                 *  @param  oiHandle      : ACS_APGCC_OiHandle
                 *  @param  invocation    : ACS_APGCC_InvocationType
                 *  @param  p_objName
                 *  @param  operationId
                 *  @param paramList
                 */
		void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
		
		/**
                 * method name : setErrorCode
                 * param     code
                 * return    void
                 */
		void setErrorCode(unsigned int code);

		/**
                 *  svc method: This method is used to spawn a thread.
                 *  return ACE_INT32
                 */
		ACE_INT32 svc();

		/**
                 *  setImpl method: This method is used to set implementer.
                 *  return ACS_CC_ReturnType
                 */
		ACS_CC_ReturnType setImpl();	

		/**
                 *  shutdown method: This method is used to bring down the implementers.
                 *  return void
                 */
		void shutdown();

		/**
		  * getCPIDs method : This method will give the list of cp names in vector
		  * return void	
		  */	
		void getCPIDs(std::vector<std::string>& cpNames);

		/**
                  * getTqDN method : This method will give the list of cp names in vector
		  * @param 	   : name of the transfer queue provided
                  * return void
                  */
		void getTqDN(string tq);

		 /**
		 *  createJobValidations method: This method will call different functions to validate all the attributes one by one
		 *  @param  request          : structure of type JOB_Parameters
		 *  @param  category_flag    : this value will be passed only when the function is invoked from modify call back
		 *  @param   dn		     : this value will be passed only when the function is invoked from modify call b
ack	 
		 */
		ACE_INT32 createJobValidations(JOB_Parameters *request,bool category_flag=true, const char* dn = "");

		 /**
		 *  deleteJobValidations method: This method will check whether a job can be deleted or not
		 *  @param  request          : const char pointer
		 */
		ACE_INT32 deleteJobValidations(const char*);

		/**
                 * @brief  classname to set the implementer
                 */
                string theClassName;

                /**
                 * @brief  char array to hold the error message that need to display
                 */
                char errorText[100];

                /*=====================================================================
                                        STATIC VARIABLES DECLARATION
                ==================================================================== */
		/**
		 * brief  This vector can have list of rdn's and this vector is filled when create call back is invoked to create the job scheduler, progress report for job and its corresponding job scheduler.Job scheduler will be created from acs_hcs_healthcheckservice thread
		 */	
		static vector<string> jobList_create;


		/**
                  * getCPGrps method : This method will give the list of cp groups in vector
                  * return void
                  */
		void getCPGrps(std::vector<std::string>& cpGrp);

		

	/*=====================================================================
		                       PRIVATE DECLARATION SECTION
	==================================================================== */
	private:
		/**
		* @brief	theOiHandlerPtr
		*/
		acs_apgcc_oihandler_V3 *theOiHandlerPtr;
		/**
		* @brief   m_poTPReactorImpl
		*/
		ACE_TP_Reactor *m_poTPReactorImpl;

		/**
		* @brief   m_poReactor
		*/
		ACE_Reactor* m_poReactor;
	
		/**
                 * @brief  m_isClassImplAdded
                 */	
		//bool m_isClassImplAdded;	
};

#endif
