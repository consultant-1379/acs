/*=================================================================== */
/**
   @file   acs_logm_srvparam_oihandler.h

   @brief Header file for LM module.

          This module contains all the declarations useful to
          specify the class acs_lm_root_objectImplementer.

   @version 1.0.0
 */

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09-Nov-2011   XCSSATA  Initial Release
==================================================================== */
#ifndef acs_lm_root_objectImplementer_H
#define acs_lm_root_objectImplementer_H
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
/*Include Section */
#include <string>
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_lm_defs.h"
#include "acs_lm_clienthandler.h"
#include "acs_lm_electronickey_runtimeowner.h"

using namespace std;
#define NORTH_BOUND_FOLDERS_CLASS_NAME "NorthBoundFolders"
#define LICENSE_FILE_ATTR_NAME  "licenseFile"
#define NBI_PATH_ATTR_NAME "internalRoot"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */


/**
 * @brief acs_lm_root_objectImplementer
 * acs_lm_root_objectImplementer class is extended from acs_apgcc_objectImplementereventhandler_V2.
 * It is having functions to handle IMM callbacks
 *
 */

class acs_lm_root_objectImplementer:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{

public:
	/*===================================================================
	                               PUBLIC ATTRIBUTE
	  =================================================================== */

	/*===================================================================                                     PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
	            @brief       Default constructor for acs_lm_root_objectImplementer

	            @par         None

	            @pre         None

	            @post        None

	            @exception   None
	 */
	/*=================================================================== */

   acs_lm_root_objectImplementer(string szimpName );
   /*=================================================================== */
   /**
                @brief       Default destructor for acs_lm_root_objectImplementer

                @par         None

                @pre         None

                @post        None

                @exception   None
    */
   /*=================================================================== */

   ~acs_lm_root_objectImplementer();
   /*=================================================================== */
     /**
      *  @brief create                     :This method is inherited from
      *                                     base class and overridden by
      *                                     our class.
      *                                     This method is get invoked when
      *                                     IMM object created.
      *
      *  @param  oiHandle                   :ACS_APGCC_OiHandle
      *
      *  @param  ccbId                       	: ACS_APGCC_CcbId
      *
      *  @param  className                 : const char pointer
      *
      *  @param  parentname                : const char pointer
      *
      *  @param  attr                      : ACS_APGCC_AttrValues
      *
      *  @return ACS_CC_ReturnType         : Sucess/failure
      */
     /*=================================================================== */

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
    /*=================================================================== */
    /**
     *  @brief deleted                                            : This method is inherited
     *                                                             from base class and overridden by our class.         *                                                                                      This method is get invoked when IMM object deleted.
     *
     *  @param  oiHandle                                          :ACS_APGCC_OiHandle
     *
     *  @param  ccbId                                             : ACS_APGCC_CcbId
     *
     *  @param  objName                                           :const char
     *
     *  @return ACS_CC_ReturnType                                 : Sucess/failure
     */
    /*=================================================================== */

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/*=================================================================== */
	/**
	 * @brief modify                                    :This method is inherited from
	 *                                                   base class and overridden by our class.
	 *                                                   This method is get invoked when
	 *                                                   IMM object's attribute modify.
	 *
	 *  @param  oiHandle                                : ACS_APGCC_OiHandle
	 *
	 *  @param  ccbId                                   : ACS_APGCC_CcbId
	 *
	 *  @param  attrMods                            	: ACS_APGCC_AttrModification
	 *
	 *  @param  objName                                 :const char
	 *
	 *  @return ACS_CC_ReturnType               		: Sucess/failure
	 */
	/*=================================================================== */

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	/*=================================================================== */
	/**
	 *  @brief complete
	 *
	 *  @param  oiHandle                     :ACS_APGCC_OiHandle
	 *
	 *  @param  ccbId                        :ACS_APGCC_CcbId
	 *
	 *  @return ACS_CC_ReturnType            :Sucess/failure
	 */
	/*=================================================================== */

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/*=================================================================== */
	/**
	 *  @brief abort
	 *
	 *  @param  oiHandle                            :ACS_APGCC_OiHandle
	 *
	 *  @param  ccbId                               :ACS_APGCC_CcbId
	 *
	 *  @return void
	 */
	/*=================================================================== */

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/*=================================================================== */
	/**
	 *  @brief apply
	 *
	 *  @param  oiHandle                            :ACS_APGCC_OiHandle
	 *
	 *  @param  ccbId                               :ACS_APGCC_CcbId
	 *
	 *  @return void
	 */
	/*=================================================================== */

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/*=================================================================== */
	/**
	 *  @brief  updateRuntime
	 *
	 *  @param  objName                     : const char pointer
	 *
	 *  @param  attrName                    : const char pointer
	 *
	 *  @return ACS_CC_ReturnType
	 */
	/*=================================================================== */

	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);
	/*=================================================================== */
	/**
	 *  @brief isObjectExiststheLMInfo				: This method is inherited from
	 *  											 base class and overridden by our class.
	 *
	 *  @param  aRdnName          					: string
	 *
	 *  @param  dn         	      					: string
	 *
	 *  @param isObjectAvailable					:bool
	 *
	 *  @isObjectAvailable
	 */
	/*=================================================================== */
	bool isObjectExiststheLMInfo(std::string aRdnName, std::string dn, bool& isObjectAvailable);

	/*=================================================================== */
	/**
	 *  @brief getLicenseKeyFilePath				: This method is inherited from
	 *  											 base class and overridden by our class.
	 *
	 *  @param  &flePath        					 : string
	 *
	 *  @return bool								:true,false
	 */
	/*=================================================================== */
	//bool getLicenseKeyFilePath( string &flePath);
	/*=================================================================== */
        /**
         * @brief getObsoluteFilePath 				: This method gets the absolute path
         *  										from the logical path and also validates
         *  										the received RDN from method adminOperationCallback().
         *
         * @return 									:bool

         */
	/*=================================================================== */
//	bool getObsoluteFilePath(string, string &);
	/*=================================================================== */
	/**
	 *  @brief adminOperationCallback
	 *
	 *  @param  oiHandle                    : ACS_APGCC_OiHandle
	 *
	 *  @param  invocation                  : ACS_APGCC_InvocationType
	 *
	 *  @param  p_objName                   : const char pointer
	 *
	 *  @param  operationId                 : ACS_APGCC_AdminOperationIdType
	 *
	 *  @param  paramList                   : ACS_APGCC_AdminOperationParamType pointer
	 *
	 *  @return void
	 */
	/*=================================================================== */


	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	//int handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext);
    /*=================================================================== */
    /**
     *  @brief  svc                              : This method is used to
     *                                            initializes the object Implementer
     *                                            functions of the IMM service
     *                                            for the invoking process
     *                                                                                      and registers the callback function.
     *  @param  poReactor                        :ACE_Reactor
     *
     *  @return                                  :ACE_INT32
     */
    /*=================================================================== */

	ACE_INT32 svc(ACE_Reactor *poReactor);

    /*=================================================================== */
    /**
     *  @brief  lmRootOI_svc
     *
     *
     *  @return                                :ACE_INT32
     */
    /*=================================================================== */
        ACE_INT32 lmRootOI_svc();

    /*=================================================================== */
    /**
     *  @brief  getMonthString
     *
     *
     *  @return                                  :string
     */
    /*=================================================================== */
	std::string getMonthString(int);

        ACS_LM_ElectronicKey_RuntimeOwner * theElectronicKeyLMRuntimeOwnerPtr;
        ACS_LM_ClientHandler* theClienthandlerPtr;
    	OmHandler* theOmHandlerPtr;
    	/*=================================================================== */
    	/**
    	 *  @brief  setClienthandler
    	 *
    	 *
    	 *  @return                                  :void
    	 */
    	/*=================================================================== */
	void setClienthandler(ACS_LM_ClientHandler*);
	/*=================================================================== */
	/**
	 *  @brief  setElectronicKeyhandler
	 *
	 *
	 *  @return                                  :void
	 */
	/*=================================================================== */
	void setElectronicKeyhandler(ACS_LM_ElectronicKey_RuntimeOwner*);
	/*=================================================================== */
	/**
	 *  @brief  setOmHandler
	 *
	 *
	 *  @return                                  :void
	 */
	/*=================================================================== */
	void setOmHandler(OmHandler*);

	 /*=====================================================================*/
        /**
             @brief     shutdown
	
             @return    void
         */
        /*=====================================================================*/
        void shutdown();

        /*=====================================================================*/
        /**
            @brief     setupLMOIThread 

            @param     almRootObjectImplementer 

            @return ACS_CC_ReturnType
         */
        /*=====================================================================*/


	ACS_CC_ReturnType setupLMOIThread(acs_lm_root_objectImplementer *almRootObjectImplementer);


        /*=====================================================================*/
        /**
            @brief    LMRootOIFunc 

            @param    void 

            @return  ACE_THR_FUNC_RETURN
         */
        /*=====================================================================*/
	 static ACE_THR_FUNC_RETURN LMRootOIFunc(void*);

	
            /*=================================================================== */
            /**
                        @brief  OIFuncThreadId 
            */
            /*=================================================================== */

            ACE_thread_t OIFuncThreadId;

	    bool getLmActionStatus();
	   /* LM Maintenance Mode */
	   /**
            @brief     setErrorCode

            @param     code

            @return    void
           */
           /*============================================================*/
            void setErrorCode(unsigned int code);
           /*============================================================*/

            ACS_CC_ReturnType setLMRootObjectImplementer();
private:
	/*===================================================================
	                              PRIVATE ATTRIBUTE
	  =================================================================== */

	 /*===================================================================
	                            PRIVATE METHOD
	 =================================================================== */

	/*=================================================================== */

    /**
  	* @brief	theOiHandlerPtr
  	*/
  	acs_apgcc_oihandler_V3 *theOiHandlerPtr;
	
	std::string errorString;
	
	char errorText[100]; /* LM Maintenance Mode */ 

	int errorCode;

	bool setErrorText(unsigned int aErrorId, std::string ErrorText);

	/* LM Maintenance Mode */
	//Changing the method name from "setErrorText" to "setThisExitCode".
	void setThisExitCode(int error, std::string text); 
	int getExitCode();


        /*=================================================================== */
        /**
              @brief   thetp_reactor_impl
         */
        /*=================================================================== */
        ACE_TP_Reactor *thetp_reactor_impl;
        /**
              @brief   theReactor
         */
        /*=================================================================== */
        ACE_Reactor *theReactor;

        /*=================================================================== */

	static bool isLmActionInProgress;
	std::string getExportErrorText(const ACS_LM_AppExitCode appExitCode);
};



#endif
