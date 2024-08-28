/*=================================================================== */
/**
   @file   acs_chb_mtzln_implementer.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       12/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_CHB_MTZLN_Implementer_H_
#define ACS_CHB_MTZLN_Implementer_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <acs_apgcc_objectimplementereventhandler_V2.h>
#include <acs_chb_file_handler.h>
#include <ace/File_Lock.h>
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/OS_NS_ctype.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/ACE.h>
#include <acs_apgcc_oihandler_V2.h>
#include <acs_chb_common.h>
#include <ACS_CC_Types.h>
#include <ace/TP_Reactor.h>
#include <ace/Task.h>
#include <ace/Reactor.h>
#include <iostream>
#include <string>
#include <vector>
#include <ACS_APGCC_Util.H>
using namespace std;


extern FileMapType g_timeZoneMap;

#define ACS_CHB_MTZLN_IMPLEMENTER	"MTZLNImplmenter"
#define ACS_CHB_MTZLN_IMM_CLASS_NM	"TimeZoneLink"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	This class implements the object implementer functionality
 			for MTZLN command.
 */

/*===================================================================*/
class ACS_CHB_MTZLN_Implementer: public acs_apgcc_objectimplementereventhandler_V2
{
/*=====================================================================
	                        PUBLIC DECLARATION SECTION
==================================================================== */

public :
	/*=====================================================================*/
		/**
		   @brief	This function is the inline constructor for the ACS_CHB_MTZLN_Implementer class.

		   @param	p_objName
		  			Name of the object.

		   @param	p_impName
		  			Name of implementer.

		   @param	p_scope
		 			Set of objects for which implementer is defined.

		 */
	/*==================================================================== */

         ACS_CHB_MTZLN_Implementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );
        /*=====================================================================*/
        /**
         	@brief	This function is the destructor for the ACS_CHB_MTZLN_Implementer class.

         */
       /*=====================================================================*/
        ~ACS_CHB_MTZLN_Implementer();
        /*=====================================================================*/
        /**
           @brief	This function will be called as a callback when an object is created as
          			an instance of a class for which the application has registered as OI.

           @param	oiHandle
          			The object implementer handle.

           @param	ccbId
         			Configuration Change Bundle ID.

           @param	className
          			Name of the class.

           @param	parentname
          			Name of the parent object.

           @param	attr
          			Array of ACS_APGCC_AttrValues.

           @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

        /*=====================================================================*/
        /**
           @brief	This function will be called as a callback when an object of the class is deleted.

           @param	oiHandle
          			The Object Implementer Handle.

           @param	ccbId
          			Configuration Change Bundle ID.

           @param	objName
          			Name of the object.

           @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

        /*=====================================================================*/
        /**
           @brief	This function will called as a callback when an object is modified.

           @param	oiHandle
          			The Object Implementer Handle.

           @param	ccbId
          			Configuration Change Bundle ID.

           @param	objName
          			Name of the object.

           @param	attrMods
          			Array of ACS_APGCC_AttrModification.

           @return	ACS_CC_ReturnType.
         */
        /*=====================================================================*/
        ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

        /*=====================================================================*/
        /**
           @brief	This function will be called when a Configuration Change bundle
          			is complete.

           @param	oiHandle
          			The Object Implementer handle.

           @param	ccbId
          			Configuration Change Bundle ID.

           @return	ACS_CC_ReturnType.
         */
        /*=====================================================================*/
        ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /*=====================================================================*/
        /**
           @brief	This function will be called when an registered application's object has aborted.

           @param	oiHandle
         			The Object Implementer Handle.

           @param	ccbId
          			Configuration Change Bundle ID.

           @return	void
         */
        /*=====================================================================*/
        void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /*=====================================================================*/
        /**
           @brief	This function will be called when the change is complete and
          			can be applied.

           @param	oiHandle
          			The Object Implementer Handle.

           @param	ccbId
          			Configuration Change Bundle Id.

           @return	void
         */
        /*=====================================================================*/
        void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

        /*=====================================================================*/
        /**
           @brief	updateRunTime callback.

           @param	*

           @param	*

           @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType updateRuntime(const char*, const char*);

        /*=====================================================================*/




void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

        /**
           @brief	MTZFunctionalMethod

           @param	aMTZPtr

           @return	ACE_THR_FUNC_RETURN
         */
        /*=====================================================================*/
        static ACE_THR_FUNC_RETURN MTZFunctionalMethod(void*);

        /*=====================================================================*/
        /**
            @brief	setupMTZThread

            @param	aMTZLNImplementer
		
	        @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType setupMTZThread(ACS_CHB_MTZLN_Implementer * aMTZLNImplementer);

        /*=====================================================================*/
        /**
             @brief	shutdown

             @return	void
         */
        /*=====================================================================*/
        void shutdown();

        /*=====================================================================*/
        /**
             @brief	getReactor

             @return	ACE_Reactor
         */
        /*=====================================================================*/
        ACE_Reactor* getReactor();

        /*=====================================================================*/
        /**
             @brief	setObjectImplementer

             @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType setObjectImplementer();

        /*=====================================================================*/
        /**
              @brief	startMTZFunctionality

              @return	ACS_CC_ReturnType
          */
        /*=====================================================================*/
        ACS_CC_ReturnType startMTZFunctionality();

        /*=====================================================================*/
        /**
               @brief	readTimeZones

               @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType readTimeZones();

		/*=================================================================== */
	    /**
			@brief   mtzFuncThreadId
	    */
	    /*=================================================================== */

	    ACE_thread_t mtzFuncThreadId;

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
              @brief   theMTZClassName
         */
        /*=================================================================== */
        std::string theMTZClassName;
        /*==================================================================== */

        /**
                      @brief   theDependancyFlag
         */
        /*=================================================================== */
        bool theDependancyFlag;
        /*==================================================================== */
        /*!
           * @brief Creates the Sets the error text
           * @param  ccbId	:	Error Object ID
           * @param  aErrorId      		:	Error ID
           * @param  ErrorText      	:	Error Text
           * @return TRUE after successful deletion
           * @return FALSE in case there is error
           */
        bool setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText);

};

#endif

