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
 ----------------------------------------------------------------------*/
/**
*
* @file ACS_XBRM_PeriodicEventHandler.cpp
*
* @brief
* ACS_XBRM_PeriodicEventHandler Class for SystemBrmPeriodicEvent IMM Handling
*
* @details
* Implementation of ACS_XBRM_PeriodicEventHandler Class for handling IMM
* callbacks for SystemBrmPeriodicEvent MO
*
* @author ZPAGSAI
*
-------------------------------------------------------------------------*/
/*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-05-01  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

// ACE libraries
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include <ace/Signal.h>

// ACS libraries
#include <ACS_APGCC_Util.H>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_APGCC_CommonLib.h>
#include "ACS_XBRM_PeriodicEventHandler.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include <ACS_TRA_trace.h> //time
#include "ACS_XBRM_ImmUtlis.h"
#include "ACS_XBRM_Utilities.h"
#include <string>
using namespace std;

/*==================================================================
               STATIC VARIALE DECLARATION SECTION
=================================================================== */
string ACS_XBRM_PeriodicEventHandler::completePeventRdn = EMPTY_STR;
bool ACS_XBRM_PeriodicEventHandler::delete_Pevent_flag = false;
string ACS_XBRM_PeriodicEventHandler::newDn = "";
vector<eventDetails> create_periodic_event = vector<eventDetails>();
vector<eventDetails> modify_periodic_event = vector<eventDetails>();
bool updateNextScheduledTime;

bool stoptime_flag = false;
bool isStopTimeMod = false;
ACS_XBRM_TRACE_DEFINE(ACS_XBRM_PeriodicEventHandler);
ACS_XBRM_PeriodicEventHandler::ACS_XBRM_PeriodicEventHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope)
    : acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope),
      theClassName(ClassName)
{
    // TRACE
    ACS_XBRM_TRACE_FUNCTION;
    m_isObjImplAdded = false;
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();

    if (theReportReactorImplPtr != 0)
    {
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_PeriodicEventHandler::ACS_XBRM_PeriodicEventHandler(std::string ClassName, std::string p_impName, ACS_APGCC_ScopeT p_scope, ACE_Thread_Manager *aThrmgr)
    : acs_apgcc_objectimplementereventhandler_V3(ClassName, p_impName, p_scope),
      theClassName(ClassName)
{
    // TRACE
    ACS_XBRM_TRACE_FUNCTION;
    this->thr_mgr(aThrmgr);
    m_isObjImplAdded = false;
    theReportReactorImplPtr = 0;
    theReportReactorImplPtr = new ACE_TP_Reactor();

    if (theReportReactorImplPtr != 0)
    {
        theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
    }
}

ACS_XBRM_PeriodicEventHandler::~ACS_XBRM_PeriodicEventHandler()
{
    ACS_XBRM_TRACE_FUNCTION;
    if (ACS_CC_SUCCESS == immHandle.Finalize())
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "ACS_XBRM_PeriodicEventHandler immHandle is finalized");
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "ACS_XBRM_PeriodicEventHandler immHandle is finalization failed");
    }

    if (theReportReactorPtr != 0)
    {
        delete theReportReactorPtr;
        theReportReactorPtr = 0;
    }

    if (theReportReactorImplPtr != 0)
    {
        delete theReportReactorImplPtr;
        theReportReactorImplPtr = 0;
    }

    if (thePeriodicEventHandler != 0)
    {
        delete thePeriodicEventHandler;
        thePeriodicEventHandler = 0;
    }
}

ACE_INT32 ACS_XBRM_PeriodicEventHandler::svc()
{
    // TRACE
    ACS_XBRM_TRACE_FUNCTION;
    thePeriodicEventHandler = 0;
    thePeriodicEventHandler = new acs_apgcc_oihandler_V3();

    try
    {
        if (ACS_CC_SUCCESS != immHandle.Init())
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "IMM Handler initialization failed");
            return -1;
        }
        if (thePeriodicEventHandler != 0)
        {
            for (int i = 0; i < 10; i++)
            {
                if (ACS_CC_FAILURE == thePeriodicEventHandler->addClassImpl(this, theClassName.c_str()))
                {
                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "PeriodicEvent Handler addClassImpl() failed");
                    sleep(1);
                }
                else
                {
                    // TRACE
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "PeriodicEvent Handler addClassImpl() successful");
                    m_isObjImplAdded = true;
                    break;
                }
            }
        }
        if (false == m_isObjImplAdded)
            return -1;
    }
    catch (...)
    { // catches any type of exception
      // exception handling
    }

    if (theReportReactorPtr != 0)
    {
        // TRACE
        if (theReportReactorPtr->open(1) == 0)
        {
            // TRACE
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "PeriodicEvent Handler Reactor pointer failed to initialize ::open()");
            return -1;
        }
        dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
        theReportReactorPtr->run_reactor_event_loop();
    }
    return 0;
}

ACS_CC_ReturnType ACS_XBRM_PeriodicEventHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "ACS_XBRM_PeriodicEventHandler create method entered ");
    //  TRACE
    (void)oiHandle;
    (void)ccbId;
    (void)className;
    (void)parentName;
    (void)attr;

    char startTime[32] = {0};
    char stopTime[32] = {0};
    char periodicEventId[128] = {0};
    int i = 0;
    bool isStartTimeProvided = false;
    int months = 0, weeks = 0, days = 0, hours = 0, mins = 0;

    while (attr[i])
    {
        switch (attr[i]->attrType)
        {
        case ATTR_UINT32T:

            if (ACE_OS::strcmp(attr[i]->attrName, monthAttr) == 0 && attr[i]->attrValuesNum)

            {

                months = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
            }

            else if (ACE_OS::strcmp(attr[i]->attrName, weeksAttr) == 0 && attr[i]->attrValuesNum)

            {

                weeks = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
            }

            else if (ACE_OS::strcmp(attr[i]->attrName, daysAttr) == 0 && attr[i]->attrValuesNum)

            {

                days = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
            }

            else if (ACE_OS::strcmp(attr[i]->attrName, hoursAttr) == 0 && attr[i]->attrValuesNum)

            {

                hours = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
            }

            else if (ACE_OS::strcmp(attr[i]->attrName, minutesAttr) == 0 && attr[i]->attrValuesNum)

            {

                mins = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
            }

            break;
        case ATTR_STRINGT:
            if (ACE_OS::strcmp(attr[i]->attrName, stopTimeAttr) == 0 && attr[i]->attrValuesNum)
            {
                ACE_OS::strcpy(stopTime, reinterpret_cast<char *>(attr[i]->attrValues[0]));
            }
            else if (ACE_OS::strcmp(attr[i]->attrName, startTimeAttr) == 0 && attr[i]->attrValuesNum)
            {
                ACE_OS::strcpy(startTime, reinterpret_cast<char *>(attr[i]->attrValues[0]));
                isStartTimeProvided = true;
            }
            else if (ACE_OS::strcmp(attr[i]->attrName, periodicEventIdAttr) == 0 && attr[i]->attrValuesNum)
            {
                ACE_OS::strcpy(periodicEventId, reinterpret_cast<char *>(attr[i]->attrValues[0]));
            }
            break;

        default:
            break;
        }
        i++;
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "months:%d, weeks:%d, days:%d, hours:%d, mins:%d", months, weeks, days, hours, mins);

    if (months + weeks + days + hours + mins == 0)
    {
        strcpy(errorText, ACS_XBRM_INVALID_PERIODICITY);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;
    }

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "stop time::: %s ", stopTime);
    string StartTime(startTime);
    string StopTime(stopTime);
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "start Time::%s ", StartTime.c_str());

    int retValue = createValidations(StartTime, StopTime);
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "retValue::%d ", retValue);

    switch (retValue)
    {
    case ACS_XBRM_STARTTIME_EXPIRED:
        strcpy(errorText, STARTTIME_EXPIRED);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_TIME:
        strcpy(errorText, INVALID_TIME);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_TIMEZONE:
        strcpy(errorText, INVALID_TIMEZONE);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_STOPTIME_EXPIRED:
        strcpy(errorText, STOPTIME_EXPIRED);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_STOPTIME:
        strcpy(errorText, INVALID_STOPTIME);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    default:
        break;
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "ACS_XBRM_PeriodicEventHandler create method entered--2 ");
    string Id = periodicEventId;
    string rdn = Id + COMMA_STR + parentName;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "rdn::::%s", rdn.c_str());
    ACS_XBRM_PeriodicEventHandler::newDn = rdn;

    string time(startTime);
    ACS_XBRM_Utilities obj;
    if ((time.compare(EMPTY_STR) == 0))
        time = obj.getCurrentTime();

    if (augmentcall(oiHandle, ccbId, time, rdn, CREATE_T, isStartTimeProvided, "", ""))
    {
        strcpy(errorText, "Internal program fault");
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;
    }

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving ACS_XBRM_PeriodicEventHandler::create");
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_PeriodicEventHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    if (!(obj.isAdminStateLOCKED((string)objName)))
    {
        strcpy(errorText, "System Backups Scheduler is not LOCKED");
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;
    }

    (void)oiHandle;
    (void)ccbId;
    (void)objName;

    ACS_XBRM_PeriodicEventHandler::completePeventRdn = objName;
    ACS_XBRM_PeriodicEventHandler::delete_Pevent_flag = true;

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving acs_hcs_periodicEvent_ObjectImpl::deleted");
    return result;
}

ACS_CC_ReturnType ACS_XBRM_PeriodicEventHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
    ACS_XBRM_TRACE_FUNCTION;
    //  TRACE
    (void)oiHandle;
    (void)ccbId;
    (void)objName;
    (void)attrMods;

    string rdn(objName);
    ACS_XBRM_PeriodicEventHandler::newDn = rdn;
    char startTime[32] = {0};
    char stopTime[32] = {0};

    bool starttime_flag = false;
    int months = 0, weeks = 0, days = 0, hours = 0, mins = 0;
    int i = 0;
    OmHandler immHandler;

    ACS_CC_ReturnType returnCode;

    vector<ACS_APGCC_ImmAttribute *> attributes;

    ACS_APGCC_ImmAttribute Months;

    ACS_APGCC_ImmAttribute Weeks;

    ACS_APGCC_ImmAttribute Hours;

    ACS_APGCC_ImmAttribute Days;

    ACS_APGCC_ImmAttribute Minutes;

    ACS_APGCC_ImmAttribute lastScheduleDate;

    Months.attrName = monthsAttr;

    Days.attrName = daysAttr;

    Hours.attrName = hoursAttr;

    Weeks.attrName = weeksAttr;

    Minutes.attrName = minutesAttr;

    lastScheduleDate.attrName = (char *)"lastScheduledDate";

    attributes.push_back(&Months);

    attributes.push_back(&Weeks);

    attributes.push_back(&Days);

    attributes.push_back(&Hours);

    attributes.push_back(&Minutes);
    
    attributes.push_back(&lastScheduleDate);

    returnCode = immHandle.getAttribute(objName, attributes);

    string lastScheduleDateValue;

    if (returnCode == ACS_CC_FAILURE)

    {
        cout << "getInternalLastError :" << immHandler.getInternalLastError() << endl;

        cout << "getInternalLastErrorText :" << immHandler.getInternalLastErrorText() << endl;
    }
    else
    {
        months = *(reinterpret_cast<int *>(attributes[0]->attrValues[0]));

        weeks = *(reinterpret_cast<int *>(attributes[1]->attrValues[0]));

        days = *(reinterpret_cast<int *>(attributes[2]->attrValues[0]));

        hours = *(reinterpret_cast<int *>(attributes[3]->attrValues[0]));

        mins = *(reinterpret_cast<int *>(attributes[4]->attrValues[0]));

        lastScheduleDateValue = reinterpret_cast<char *>(attributes[5]->attrValues[0]);

    }
    while (attrMods[i])
    {
        isStopTimeMod = false;
        switch (attrMods[i]->modAttr.attrType)
        {
        case ATTR_UINT32T:
            if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, monthAttr) == 0 && attrMods[i]->modAttr.attrValuesNum)
            {
                months = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, weeksAttr) == 0 && attrMods[i]->modAttr.attrValuesNum)
            {
                weeks = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, daysAttr) == 0 && attrMods[i]->modAttr.attrValuesNum)
            {
                days = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, hoursAttr) == 0 && attrMods[i]->modAttr.attrValuesNum)
            {
                hours = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, minutesAttr) == 0 && attrMods[i]->modAttr.attrValuesNum)
            {
                mins = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
            }

            break;
        case ATTR_STRINGT:
            if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, stopTimeAttr) == 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Coming Hereeeeeeeee");
                if ((int)attrMods[i]->modAttr.attrValuesNum == 0)
                {
                    ACE_OS::strcpy(stopTime, EMPTY_STR);

                    stoptime_flag = true;
                    isStopTimeMod = false;
                }
                else
                {
                    ACE_OS::strcpy(stopTime, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
                    isStopTimeMod = true;
                }
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, startTimeAttr) == 0)
            {
                if ((int)attrMods[i]->modAttr.attrValuesNum == 0)
                {
                    ACE_OS::strcpy(startTime, EMPTY_STR);
                    starttime_flag = true;
                }
                else
                {
                    ACE_OS::strcpy(startTime, reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
                    starttime_flag = true;
                }
            }
            else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, LASTSCHEDULEDATE) == 0)
            {
                return ACS_CC_SUCCESS;
                // no need to validation in case of lastscheduled date updation
            }
            break;

        default:
            break;
        }
        i++;
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "months:%d, weeks:%d, days:%d, hours:%d, mins:%d", months, weeks, days, hours, mins);

    if (months + weeks + days + hours + mins == 0)
    {
        strcpy(errorText, ACS_XBRM_INVALID_PERIODICITY);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;
    }
    string StartTime(startTime);
    string StopTime(stopTime);

    if (stoptime_flag == false && StopTime.size() == 0) // To get the previously provided stopTime
    {
        StopTime = obj.getImmAttribute(objName, stopTimeAttr);
    }
    if (starttime_flag == false)
    {
        StartTime = obj.getImmAttribute(objName, startTimeAttr);
        if (obj.compareDates(StartTime, obj.getCurrentTime()))
            StartTime = EMPTY_STR;
    }

    int retValue = createValidations(StartTime, StopTime);
    switch (retValue)
    {
    case ACS_XBRM_STARTTIME_EXPIRED:
        strcpy(errorText, STARTTIME_EXPIRED);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_TIME:
        strcpy(errorText, INVALID_TIME);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_TIMEZONE:
        strcpy(errorText, INVALID_TIMEZONE);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_STOPTIME_EXPIRED:
        strcpy(errorText, STOPTIME_EXPIRED);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    case ACS_XBRM_INVALID_STOPTIME:
        strcpy(errorText, INVALID_STOPTIME);
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;

    default:
        break;
    }

    string time(startTime);

    bool isStartTimeProvided = true;
    if ((time.compare(EMPTY_STR) == 0))
    {
        time = obj.getCurrentTime();
        isStartTimeProvided = false;
    }
    string stopTimeProvided(stopTime);
    if (augmentcall(oiHandle, ccbId, time, rdn, MODIFY, isStartTimeProvided, stopTimeProvided, lastScheduleDateValue))
    {
        strcpy(errorText, "Internal program fault");
        this->setExitCode(17, errorText);
        return ACS_CC_FAILURE;
    }
    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_PeriodicEventHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    // TRACE
    (void)oiHandle;
    (void)ccbId;

    return result;
}

void ACS_XBRM_PeriodicEventHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    ACS_XBRM_TRACE_FUNCTION;
    // TRACE
    (void)oiHandle;
    (void)ccbId;
}

void ACS_XBRM_PeriodicEventHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
    ACS_XBRM_TRACE_FUNCTION;
    // TRACE
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Entering ::apply");
    (void)oiHandle;
    (void)ccbId;

    if (ACS_XBRM_PeriodicEventHandler::delete_Pevent_flag == true)
    {

        obj.updateNextScheduledEventTime();
        ACS_XBRM_PeriodicEventHandler::completePeventRdn = EMPTY_STR;
        ACS_XBRM_PeriodicEventHandler::delete_Pevent_flag = false;
    }

    int create_size = create_periodic_event.size();
    int modify_size = modify_periodic_event.size();

    if (create_size != 0)
    {

        obj.updateNextScheduledEventTime();
        create_periodic_event.clear();
    }

    if (modify_size != 0)
    {

        obj.updateNextScheduledEventTime();
        modify_periodic_event.clear();
    }

    updateNextScheduledTime = true;

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving ACS_XBRM_PeriodicEventHandler::apply");
}

ACS_CC_ReturnType ACS_XBRM_PeriodicEventHandler::updateRuntime(const char *objName, const char **attrName)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    // TRACE
    (void)objName;
    (void)attrName;

    return result;
}

void ACS_XBRM_PeriodicEventHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
                                                           const char *p_objName, ACS_APGCC_AdminOperationIdType operationId,
                                                           ACS_APGCC_AdminOperationParamType **paramList)
{
    ACS_XBRM_TRACE_FUNCTION;
    (void)oiHandle;
    (void)invocation;
    (void)p_objName;
    (void)operationId;
    (void)paramList;
}

ACE_INT32 ACS_XBRM_PeriodicEventHandler::shutdown()
{
    ACS_XBRM_TRACE_FUNCTION;
    if (m_isObjImplAdded == true)
    {
        try
        {
            if (thePeriodicEventHandler != 0)
            {
                for (int i = 0; i < 10; i++)
                {
                    if (thePeriodicEventHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE)
                    {
                        // Trace
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "PeriodicEvent Handler removeClassImpl() failed");
                        sleep(1);
                    }
                    else
                    {
                        ACS_XBRM_LOG(LOG_LEVEL_INFO, "PeriodicEvent Handler removeClassImpl() successful");
                        m_isObjImplAdded = false;
                        break;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }

    if (theReportReactorPtr != 0)
    {
        theReportReactorPtr->end_reactor_event_loop();
    }

    return m_isObjImplAdded ? -1 : 0;
}

/*===================================================================
        ROUTINE: createValidations
=================================================================== */
ACE_INT32 ACS_XBRM_PeriodicEventHandler::createValidations(string &startTime, string stopTime)
{
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Entering::createValidations ");
    int returnCode = -1;

    // Validations for start time
    if (startTime.compare(EMPTY_STR) == 0)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "Taking default starttime ::createValidations ");

        startTime = obj.getCurrentTime();
        // cout << startTime << endl;
    }
    else
    {
        returnCode = obj.timeValidation(startTime, startTimeAttr);
        if (returnCode != 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "Leaving ::createValidations in starttime");
            return returnCode;
        }
    }

    // validations for stopTime
    if (stopTime.compare(EMPTY_STR) != 0)
    {
        returnCode = obj.timeValidation(stopTime, stopTimeAttr);
        if (returnCode != 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "leaving ::createValidationsin stoptime");
            return returnCode;
        }
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Strat Time ::: %s", startTime.c_str());
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Stop TImE ::: %s", stopTime.c_str());
        if (obj.compareDates(startTime, stopTime) == false) // check whether stop time is greater than start time
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "Leaving ::createValidations as stoptime invalid");
            return ACS_XBRM_INVALID_STOPTIME;
        }
    }

    return 0;
}
/*===================================================================
        ROUTINE: augmentcall
=================================================================== */
bool ACS_XBRM_PeriodicEventHandler::augmentcall(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, string time, string rdn, int callback, bool isStartTimeProvided, string stopTimeProvided, string lastScheduleDateValue)
{

    // Augmentation call to update last schedule date in single event
    ACS_APGCC_CcbHandle ccbHandleVal;
    ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
    bool isAugmentFail = false;

    ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);
    if (retVal != ACS_CC_SUCCESS)
    {
        cout << "Augmentation initialization is failed " << endl;

        isAugmentFail = true;
    }
    else
    {
        time = obj.localZoneTime(time);
        eventDetails e(time, rdn);
        if (callback == CREATE_T)
            create_periodic_event.push_back(e);
        else
            modify_periodic_event.push_back(e);

        ACS_CC_ImmParameter attributeRDN;
        attributeRDN.attrName = (char *)LASTSCHEDULEDATE;
        attributeRDN.attrType = ATTR_STRINGT;
        attributeRDN.attrValuesNum = 1;
        char *rdnValue = const_cast<char *>(time.c_str());
        void *value2[1] = {reinterpret_cast<void *>(rdnValue)};
        attributeRDN.attrValues = value2;

        if (!isStartTimeProvided && ((callback == CREATE_T) || (callback == MODIFY && !isStopTimeMod)))
        {

            ACS_CC_ImmParameter attributeRDN1;
            attributeRDN1.attrName = (char *)startTimeAttr;
            attributeRDN1.attrType = ATTR_STRINGT;
            attributeRDN1.attrValuesNum = 1;
            char *rdnValue1 = const_cast<char *>(time.c_str());
            void *value3[1] = {reinterpret_cast<int *>(rdnValue1)};
            attributeRDN1.attrValues = value3;

            retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, rdn.c_str(), &attributeRDN1);
            if (retVal != ACS_CC_SUCCESS)
            {
                isAugmentFail = true;
            }
        }
        if(isStopTimeMod){
            /*
            if stopTime is updated need to check if the last schedule date is less than stop or not
            */
            if (0 != ACE_OS::strcmp(lastScheduleDateValue.c_str(), "") && obj.compareDates(stopTimeProvided, lastScheduleDateValue)){
                void *values[1] = {reinterpret_cast<void *>(const_cast<char *>(""))};
                attributeRDN.attrValues = values;
                retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, rdn.c_str(), &attributeRDN);
            }
            /* skip updating lastScheduleDate when only stopTime is modified*/
        }else{
            retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, rdn.c_str(), &attributeRDN);
        }
        
        if (retVal != ACS_CC_SUCCESS)
        {
            cout << "Augmentation create is failed " << endl;
            isAugmentFail = true;
        }
        else
        {
            retVal = applyAugmentCcb(ccbHandleVal);
            if (retVal != ACS_CC_SUCCESS)
            {
                cout << "Augmentation apply is failed " << endl;
                isAugmentFail = true;
            }
        }
    }
    isStopTimeMod = false;
    isStartTimeProvided = false;
    return isAugmentFail;
}