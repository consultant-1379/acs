//=============================================================================
/**
   @file    acs_hcs_PeriodicEvent_objectImpl.h

   @brief Header file for HC module.
          It acts as OI for handling IMM Callbacks of HC PeriodicEvent object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A			   XMALRAO   Initial Release
 */
//=============================================================================
#ifndef ACS_HC_PERIODICEVENT_OBJECTIMPL_H
#define ACS_HC_PERIODICEVENT_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <ACS_TRA_trace.h>
#include "acs_hcs_tra.h"
#include "acs_hcs_global_ObjectImpl.h" 
#include<iostream>

using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief acs_hcs_PeriodicEvent_ObjectImpl
 * acs_hcs_PeriodicEvent_ObjectImpl class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 */
class acs_hcs_periodicEvent_ObjectImpl:public acs_apgcc_objectimplementereventhandler_V3 ,public ACE_Task_Base
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
		acs_hcs_periodicEvent_ObjectImpl( string ClassName, string p_impName, ACS_APGCC_ScopeT p_scope );

		/*=====================================================================
					   CLASS DESTRUCTOR
		  ==================================================================== */
		 /**
		* @brief
		* destructor
		*/
		~acs_hcs_periodicEvent_ObjectImpl();

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
		 *  @param  oiHandle      : ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @param  objName
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

		/**
		 *  modify method: This method is inherited from base class and overridden by our class.
		 *  This method is get invoked when IMM object's attribute modify.
		 *  @param  oiHandle      : ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @param  objName
		 *  @param  attrMods          : ACS_APGCC_AttrValues
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

		/**
		 *  complete method: This method is inherited from base class and overridden by our class.
		 *  @param  oiHandle      : ACS_APGCC_OiHandle
		 *  @param  ccbId         : ACS_APGCC_CcbId
		 *  @return ACS_CC_ReturnType : Sucess/failure
		 */
		ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**
		 *  abort method: This method is inherited from base class and overridden by our class.
		 *  @param  oiHandle      : ACS_APGCC_OiHandle
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
                 *  createValidations method: This method will call different functions to validate the provided time
                 *  @param  startTime : time in 'yyyy-mm-ddThh:mm:ss' format
		 *  @param  stopTime  : time in 'yyyy-mm-ddThh:mm:ss' format	 
                 *  return ACE_INT32  : return the error code depents on type of error
                 */
		ACE_INT32 createValidations(string & startTime,string stopTime);

		 /**
                 *  augmentcall method: This method will call during creaion/deletion of event to update hidden attributes
                 *  @param  oiHandle  : oiHandle of ACS_APGCC_OiHandle type
                 *  @param ccbId      : ccbId of ACS_APGCC_CcbId
                 *  @param scheduledTime : value of the attribute that need to update
                 *  @param rdn        : rdn of the event
                 *  @param callback   : Either 1 or 2; 1 for create and 2 for modify callback.
		 *  @param isStartTimeProvided : 'true' if starttime provided else 'false'
                 *  return bool       : return 'true' if augmentation is successful else 'false' would be return
                 */
                bool augmentcall(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, string time, string rdn, int callback, bool isStartTimeProvided);

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
		/*
                 * @brief   This variable will be checked in apply call back to update the next schedule time
                        It will be true when the event is deleted
                 */
		static bool delete_Pevent_flag ;

		/**
		 * @brief  by default this flag wpuld be false, it would be true in acs_hcs_healthcheckservice class to update the default start time for periodic event
		 */		
		//static bool time_flag;

		/*
                 * @brief  static variable to hold the complete rdn of periodic event
                   Ex:periodicEventId=1,jobSchedulerId=1,jobId=HC,HealthCheckhealthCheckMId=1
                 */
		static std::string completePeventRdn;

		/**
                 * @brief This is the dn of periodic event that is being created or modified.
                */
		static string newDn;

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

		/**
                 * @brief g_ptr
                 */
		acs_hcs_global_ObjectImpl *g_ptr;

};

#endif
