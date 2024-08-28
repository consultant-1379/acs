/*=================================================================== */
/**
   @file   acs_lm_managed_objectImplementer.h

   @brief Header file for LM module.

          This module contains all the declarations useful to
          specify the class acs_lm_managed_objectImplementer.

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
#ifndef acs_lm_managed_objectImplementer_H
#define acs_lm_managed_objectImplementer_H
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
 * @brief acs_lm_managed_objectImplementer
 * acs_lm_managed_objectImplementer class is extended from acs_apgcc_objectImplementereventhandler_V2.
 * It is having functions to handle IMM callbacks
 *
 */

class acs_lm_managed_objectImplementer:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{

public:
	/*===================================================================
	                               PUBLIC ATTRIBUTE
	  =================================================================== */

	/*===================================================================                                     PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
	            @brief       Default constructor for acs_lm_managed_objectImplementer

	            @par         None

	            @pre         None

	            @post        None

	            @exception   None
	 */
	/*=================================================================== */

   acs_lm_managed_objectImplementer(string szimpName );
   /*=================================================================== */
   /**
                @brief       Default destructor for acs_lm_managed_objectImplementer

                @par         None

                @pre         None

                @post        None

                @exception   None
    */
   /*=================================================================== */

   ~acs_lm_managed_objectImplementer();
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
     *  @brief  lmManagedOI_svc
     *
     *
     *  @return                                :ACE_INT32
     */
    /*=================================================================== */
        ACE_INT32 lmManagedOI_svc();

        ACS_LM_Persistent* thePersistent;
        ACS_LM_ClientHandler* theClienthandlerPtr;
        ACS_LM_EventHandler* theEventHandler;
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
	void setPersistant(ACS_LM_Persistent*);
	void setEventHandler(ACS_LM_EventHandler*);

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

            @param     almmanagedObjectImplementer

            @return ACS_CC_ReturnType
         */
        /*=====================================================================*/


	ACS_CC_ReturnType setupLMManagedOIThread(acs_lm_managed_objectImplementer *almManagedObjectImplementer);


        /*=====================================================================*/
        /**
            @brief    LMManagedOIFunc

            @param    void 

            @return  ACE_THR_FUNC_RETURN
         */
        /*=====================================================================*/
	 static ACE_THR_FUNC_RETURN LMManagedOIFunc(void*);

	
            /*=================================================================== */
            /**
                        @brief  OIFuncThreadId 
            */
            /*=================================================================== */

            ACE_thread_t OIFuncThreadId;

            ACS_CC_ReturnType setLMManagedObjectImplementer();

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

	std::string managedElementvalue ;
	
	std::string errorString;

	int errorCode;

	bool setErrorText(unsigned int aErrorId, std::string ErrorText);

	void setExitCode(int error, std::string text);

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
};



#endif
