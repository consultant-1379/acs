/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_PeriodicEventHandler.h
 *
 * @brief
 * ACS_XBRM_PeriodicEventHandler Class for SystemBrmPeriodicEvent IMM Handling
 *
 * @details
 * ACS_XBRM_PeriodicEventHandler is used as an OI interface handling IMM
 * callbacks for SystemBrmPeriodicEvent MO
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/


#ifndef ACS_XBRM_PERIODICEVENTHANDLER_H
#define ACS_XBRM_PERIODICEVENTHANDLER_H

#include <iostream>

//ACE libraries
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>

//ACS libraries
#include <acs_apgcc_oihandler_V3.h>
#include "acs_apgcc_objectimplementereventhandler_V3.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_CS_API.h"
#include "ACS_XBRM_Utilities.h"

#define stopTimeAttr			"stopTime"
#define startTimeAttr			"startTime"
#define weeksAttr			"weeks"
#define daysAttr			"days"
#define monthsAttr			"months"
#define	hoursAttr			"hours"
#define minutesAttr			"minutes"
#define periodicEventIdAttr		"systemBrmPeriodicEventId"
// #define ACS_XBRM_STARTTIME_EXPIRED	14	/*Start/scheduled Time has expired*/
 #define ACS_XBRM_STOPTIME_EXPIRED		15	/*Stop Time has expired*/
#define ACS_XBRM_INVALID_TIMEZONE 6 /*Invalid time zone*/
#define ACS_XBRM_STARTTIME_EXPIRED 7
#define ACS_XBRM_INVALID_STOPTIME		16	/*Stop time not valid */
#define ACS_XBRM_INVALID_TIME		5	/*Invalid scheduled time*/
#define EMPTY_STR			""
#define COMMA_STR			","
#define STARTTIME_EXPIRED		"Start time has expired"
#define INVALID_TIME			"Invalid scheduled time"
#define INVALID_TIMEZONE		"Invalid Time zone"
#define STOPTIME_EXPIRED		"Stop Time has expired"
#define INVALID_STOPTIME		"Stop time not valid"
#define timeAttr			"time"
#define dayOfWeekAttr			"dayOfWeek"
#define dayOfMonthAttr			"dayOfMonth"
#define	dayOfWeekOccurenceAttr		"dayOfWeekOccurrence"
#define monthAttr			"months"
#define calendarPeriodicEventIdAttr	"calendarPeriodicEventId"
#define	adminStateAttr			"adminState"
#define schedulerStateAttr		"schedulerState"
#define nextScheduledTimeAttr		"nextScheduledEventTime"
#define LASTSCHEDULEDATE		"lastScheduledDate"

#define statrTime		"startTime"
#define executionStatusAttr		"execStatus"
#define statusAttr			"status"
#define timeZoneIdAttr			"timeZoneId"
#define timeZoneStringAttr		"timeZoneString"
#define ACS_XBRM_INVALID_PERIODICITY "Please provide atleast one non-zero value for the attributes months,weeks,days,hours and minutes."


class ACS_XBRM_PeriodicEventHandler: 
public acs_apgcc_objectimplementereventhandler_V3, 
public ACE_Task_Base {
    public:
          
        /**
         * @brief
         * constructor with three parameters
         */
        ACS_XBRM_PeriodicEventHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope);
        ACS_XBRM_PeriodicEventHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr);
        char errorText[100];
        /**
         * @brief
         * destructor
         */
        ~ACS_XBRM_PeriodicEventHandler();

        /**
         *  create method: This method is inherited from base class and overridden by our class.
         *  This method is get invoked when IMM object created.
         *  @param  oiHandle      : ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
         *  @param  className     : const char pointer
         *  @param  parentname    : const char pointer
         *  @param  attr          : ACS_APGCC_AttrValues
         *  @return ACS_CC_ReturnType : Sucess/failure
         */
        ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr);

        /**
         *  deleted method: This method is inherited from base class and overridden by our class.
         *  This method is get invoked when IMM object deleted.
         *  @param  oiHandle      : ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
         *  @return ACS_CC_ReturnType : Sucess/failure
         */
        ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
        
        /**
         *  modify method: This method is inherited from base class and overridden by our class.
         *  This method is get invoked when IMM object's attribute modify.
         *  @param  oiHandle      : ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
         *  @param  attr          : ACS_APGCC_AttrValues
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
         *  @param  oiHandle      : ACS_APGCC_OiHandle
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
         *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
         *      and registers the callback function.
         * @return  :   ACE_INT32
         */
        ACE_INT32 svc();

        /**
         * adminOperationCallback method: This method is inherited from base class and overridden by our class
         * This method will be called as a callback to manage an administrative operation invoked, on the
         *      implemented object, using the adminOperationInvoke method of acs_apgcc_adminoperation class.
         * @param   oiHandler   :   ACS_APGCC_OiHandle
         * @param   invocation  :   ACS_APGCC_InvocationType
         * @param   p_objName   :   const char pointer
         * @param   operationId :   ACS_APGCC_AdminOperationIdType
         * @param   paramList   :   ACS_APGCC_AdminOperationParamType
         * @return  void
        */
        void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
                                    const char *p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType **paramList);
        
        
        int addImplementer(std::string szImpName);
        int removeImplementer();
        ACE_INT32 createValidations(string &startTime, string stopTime);
        
        std::string             theClassName;
        acs_apgcc_oihandler_V3  *thePeriodicEventHandler;
        ACE_Reactor             *theReportReactorPtr;
        ACE_TP_Reactor          *theReportReactorImplPtr;
        struct timespec         req, rem;
        ACS_XBRM_Utilities obj;
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

    public:
        ACE_INT32 shutdown();

    private:
        // IMMErrorRuntimeHandler* theErrorRuntimeHandlerPtr; COVERITY: 12553
        
        /**
         * @brief Creates the error handler object
         * @param  ACS_APGCC_CcbId      :       Error Object ID
         * @param  aErrorId             :       Error ID
         * @param  aErrorText           :       Error Text
         * @return TRUE after successful deletion
         * @return FALSE in case there is error
         */
        bool createErrorHandlerObject(ACS_APGCC_CcbId &aCCBId, unsigned int &aErrorId, std::string &aErrorText);

        /**
         * @brief Deletes the error handler object
         * @return TRUE after successful deletion
         * @return FALSE in case there is error
         */
        bool deleteErrorHandlerObject();

        /**
         * @brief Creates the Sets the error text
         * @param  ACS_APGCC_CcbId      :       Error Object ID
         * @param  aErrorId             :       Error ID
         * @param  aErrorText           :       Error Text
         * @return TRUE after successful deletion
         * @return FALSE in case there is error
         */
        bool setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText);
        bool augmentcall(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, string time, string rdn, int callback, bool isStartTimeProvided, string stopTimeProvided, string lastScheduleDateValue);
        bool m_isObjImplAdded;
        OmHandler immHandle;
};
#endif
