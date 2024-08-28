/*=================================================================== */
/**
   @file   acs_chb_param_oihandler.h

   @brief Header file for CHB  module.

          This module contains all the declarations useful to
          specify the class acs_chb_param_oihandler.

   @version 1.0.0
 */

/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       09/07/2011   XTANAGG   Initial Release
==================================================================== */
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#if 0
#ifndef ACS_CHB_PARAM_OIHANDLER_H
#define ACS_CHB_PARAM_OIHANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_apgcc_objectimplementereventhandler_V2.h>
#include <acs_apgcc_oihandler_V2.h>
#include <acs_chb_common.h>
#include <ACS_CC_Types.h>
#include <ace/TP_Reactor.h>
#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/ACE.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

/*=====================================================================
			CLASS DECLARATION SECTION
=======================================================================*/

/*=====================================================================*/
/**
 *	@brief		acs_chb_param_oihandler
 *			This class implements the object implementer functionality
 *			for handling IMM parameters for CHB.
 *
 */
/*======================================================================= */

class acs_chb_param_oihandler : public acs_apgcc_objectimplementereventhandler_V2
{
	/*=====================================================================
			PUBLIC DECLARATION SECTION
	==================================================================== */

public :
	/*=====================================================================*/
	/**
	   @brief       This function is the inline constructor for 
			the acs_chb_param_oihandler class.

	   @param       p_objName
			Name of the object.

	   @param       p_impName
			Name of implementer.

	   @param       p_scope
			Set of objects for which implementer is defined.

	 */
	/*==================================================================== */
	 acs_chb_param_oihandler(	string p_objName, 
					string p_impName, 
					ACS_APGCC_ScopeT p_scope );
	/*=====================================================================*/
	/**
		@brief  This function is the destructor for the
				 acs_chb_param_oihandler class.

	 */
   /*=====================================================================*/
	~acs_chb_param_oihandler();
	/*=====================================================================*/
	/**
	   @brief       This function will be called as a callback when 
			an object is created as an instance of a class 
			for which the application has registered as OI.

	   @param       oiHandle
			The object implementer handle.

	   @param       ccbId
			Configuration Change Bundle ID.

	   @param       className
			Name of the class.

	   @param       parentname
			Name of the parent object.

	   @param       attr
			Array of ACS_APGCC_AttrValues.

	   @return      ACS_CC_ReturnType
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType create(	ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *className, 
					const char* parentname, 
					ACS_APGCC_AttrValues **attr);
	/*=====================================================================*/
	/**
	   @brief       This function will be called as a callback when 
					an object of the class is deleted.

	   @param       oiHandle
			The Object Implementer Handle.

	   @param       ccbId
			Configuration Change Bundle ID.

	   @param       objName
			Name of the object.

	   @return      ACS_CC_ReturnType
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType deleted(	ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *objName);
	/*=====================================================================*/
	/**
	   @brief       This function will called as a callback when 
			an object is modified.

	   @param       oiHandle
			The Object Implementer Handle.

	   @param       ccbId
			Configuration Change Bundle ID.

	   @param       objName
			Name of the object.

	   @param       attrMods
			Array of ACS_APGCC_AttrModification.

	   @return      ACS_CC_ReturnType.
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType modify( 	ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId, 
					const char *objName, 
					ACS_APGCC_AttrModification **attrMods);
	/*=====================================================================*/
	/**
	   @brief       This function will be called when a Configuration 
			Change bundle is complete.

	   @param       oiHandle
			The Object Implementer handle.

	   @param       ccbId
			Configuration Change Bundle ID.

	   @return      ACS_CC_ReturnType.
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType complete( 	ACS_APGCC_OiHandle oiHandle, 
					ACS_APGCC_CcbId ccbId);
	/*=====================================================================*/
	/**
	   @brief       This function will be called when an registered 
			application's object has aborted.

	   @param       oiHandle
			The Object Implementer Handle.

	   @param       ccbId
			Configuration Change Bundle ID.

	   @return      void
	 */
	/*=====================================================================*/
	void abort( ACS_APGCC_OiHandle oiHandle, 
		ACS_APGCC_CcbId ccbId);

	/*=====================================================================*/
	/*=====================================================================*/
	/**
	   @brief       This function will be called when the change is 
			complete and can be applied.

	   @param       oiHandle
			The Object Implementer Handle.

	   @param       ccbId
			Configuration Change Bundle Id.

	   @return      void
	 */
	/*=====================================================================*/
	void apply( ACS_APGCC_OiHandle oiHandle, 
			ACS_APGCC_CcbId ccbId);
	/*=====================================================================*/
	/**
	   @brief       updateRunTime callback.

	   @param		param1

	   @param		param2

	   @return      ACS_CC_ReturnType
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType updateRuntime( const char* param1, 
					 const char* param2);

	/*=====================================================================*/

	/*=====================================================================*/
	/**
	   @brief       		 admin Operation Callback.

	   @param                oiHandle       
                                 ACS_APGCC_OiHandle 

	   @param                invocation     
                                 invocation id

	   @param                p_objName       
                                 Name of the Object

	   @param                operationId     
                                 Administrative Operation Identifier

	   @param                paramList      
                                 Operation Params Elements
	 */
	/*=====================================================================*/
        void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	/*=====================================================================*/

	/*=====================================================================*/
	/**
		 @brief     shutdown

		 @return    void
	 */
	/*=====================================================================*/
	void shutdown();

	/*=====================================================================*/
	/**
		 @brief     getReactor

		 @return    ACE_Reactor
	 */
	/*=====================================================================*/
	ACE_Reactor* getReactor();

	/*=====================================================================*/
	/**
		 @brief     setObjectImplementer

		 @return    ACS_CC_ReturnType
	 */
	/*=====================================================================*/
	ACS_CC_ReturnType setObjectImplementer();

	/*=====================================================================*/
	/**
		@brief		setupIMMParamOIThread

		@param		aCHBImplementer

		@return		ACS_CC_ReturnType
	*/
	/*======================================================================*/
	ACS_CC_ReturnType setupIMMParamOIThread(acs_chb_param_oihandler* aCHBImplementer);
	/*=======================================================================*/
	/**
		@brief		ParamFunctionalMethod

		@param		aCHBPtr	

		@return		ACE_THR_FUNC_RETURN
	*/
	/*======================================================================*/
	static ACE_THR_FUNC_RETURN IMMParamFunctionalMethod(void* aCHBPtr);
	/*======================================================================*/
	
	/**
		@brief		startIMMParamFunctionality

		@return		ACS_CC_ReturnType
	*/
	/*======================================================================*/
	ACS_CC_ReturnType startIMMParamFunctionality();
	/*======================================================================*/
    private:
	/*===================================================================
                                PRIVATE ATTRIBUTE
	=================================================================== */
	/*=================================================================== */
	/**
		  @brief   thetp_reactor_impl
	 */
	/*=================================================================== */
	ACE_TP_Reactor *thetp_reactor_impl;
	/*=================================================================== */
	/**
		  @brief   theReactor
	 */
	/*=================================================================== */
	ACE_Reactor *theReactor;
	/*=================================================================== */
	/**
		  @brief   theoiHandler
	 */
	/*=================================================================== */
	acs_apgcc_oihandler_V2* theoiHandler;
	/*=================================================================== */
	/**
	  @brief   theCHBObjName
	 */
	/*=================================================================== */
	std::string theCHBObjName;
	/*==================================================================== */
};

#endif
#endif
