/*
 * ACS_USA_AdhIntUserObserver.cpp
 *
 *  Created on: Mar 29, 2018
 *      Author: xmalrao
 */
#include "acs_usa_adhIntUserObserver.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include "acs_usa_global.h"
#include "acs_usa_sysloganalyser.h"

ACS_USA_Trace traObserverAdm ("ACS_USA_AdhIntUserObserver            ");
/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver
 ============================================================================ */

//! Constructor
ACS_USA_AdhIntUserObserver::ACS_USA_AdhIntUserObserver( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)
{
	m_isObjImplAdded=false;
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}
	std::string ImpName=szImpName;
	typeOfOperation = NONE;
	state = NOSTATE;
}

/*============================================================================
        ROUTINE: ~ACS_USA_AdhIntUserObserver
 ============================================================================ */
ACS_USA_AdhIntUserObserver::~ACS_USA_AdhIntUserObserver()
{
	if(theReportReactorPtr != 0)
	{
		delete theReportReactorPtr;
		theReportReactorPtr=0;
	}
	if(theReportReactorImplPtr != 0)
	{
		delete theReportReactorImplPtr;
		theReportReactorImplPtr=0;
	}
	if(theObserver != 0)
	{
		delete theObserver;
		theObserver=0;
	}
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::svc
 ============================================================================ */
ACE_INT32  ACS_USA_AdhIntUserObserver::svc()
{
	theObserver = 0;
	theObserver = new acs_apgcc_oihandler_V3();
	try
	{
		if ( theObserver != 0)
		{
			for(int i=0;i<10;i++)
			{	
				if(ACS_CC_SUCCESS != theObserver->addClassImpl(this, theClassName.c_str()))
				{
					traObserverAdm.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s(): Adding OI is failed !", __func__);
					if (-14 == getInternalLastError())
					{
						removeImplementer();
					}
					sleep(1);
				}
				else
				{
					traObserverAdm.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s(): Adding OI is sucessful !", __func__);
					m_isObjImplAdded = true;
					if(!ACS_USA_Global::instance()->adh_implementer_set())
					{
						ACS_USA_Global::instance()->adh_implementer_set(true);
					}
					break;
				}
			}
			if(m_isObjImplAdded ==false)
			{
				return -1;
			}
		}
	}
	catch(...)
	{
	}
	if (theReportReactorPtr != 0)
	{
		if (0 == theReportReactorPtr->open(1))
		{
			traObserverAdm.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s(): Reactor Open is failed !", __func__);
			return -1;

		}
		traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): Before run reactor loop!", __func__);
		theReportReactorPtr->reset_reactor_event_loop();
		dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
		theReportReactorPtr->run_reactor_event_loop();
		traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): After run reactor loop!", __func__);
	}

	return 0;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::updateRuntime
 ============================================================================ */
ACS_CC_ReturnType ACS_USA_AdhIntUserObserver::updateRuntime(const char* p_objName, const char** p_attrName)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	(void) p_objName;
	(void) p_attrName;
	return result;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::adminOperationCallback
 ============================================================================ */
void ACS_USA_AdhIntUserObserver::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::stop
 ============================================================================ */
ACE_INT32 ACS_USA_AdhIntUserObserver::shutdown()
{
	if( true == m_isObjImplAdded )
	{
		try
		{
			if ( theObserver != 0)
			{
				if(removeImplementer() ==0)
				{
					traObserverAdm.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s(): Removal of OI is successful !", __func__);
					m_isObjImplAdded = false;
				}
			}
		}
		catch(...)
		{
		}
	}

	if (theReportReactorPtr != 0)
	{
		theReportReactorPtr->end_reactor_event_loop();
	}
	if (m_isObjImplAdded != false)
		return -1;
	else
		return 0;
}
           
/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::removeImplementer
 ============================================================================ */
ACE_INT32 ACS_USA_AdhIntUserObserver::removeImplementer()
{
	if(ACS_CC_FAILURE == theObserver->removeClassImpl(this, theClassName.c_str()))
	{
		traObserverAdm.traceAndLogFmt(ERROR, "ACS_USA_AdhIntUserObserver:%s(): Removal of OI is failed !", __func__);
		return -1;
	}
	if(ACS_USA_Global::instance()->adh_implementer_set())
	{
		ACS_USA_Global::instance()->adh_implementer_set(false);
	}
	return 0;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::create
 ============================================================================ */
ACS_CC_ReturnType ACS_USA_AdhIntUserObserver::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	(void)attr;
	(void)oiHandle;
	(void)ccbId;
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): ObjectCreateCallback invocated", __func__);
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): Class Name: %s, Parent Name: %s", __func__, className, parentname);
	typeOfOperation = CREATE_OPERATION;
	return ACS_CC_SUCCESS;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::deleted
 ============================================================================ */
ACS_CC_ReturnType ACS_USA_AdhIntUserObserver::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{	
	(void)oiHandle;
	(void)ccbId;
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): ObjectDeleteCallback invocated", __func__);
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): Object Name: %s", __func__, objName);
	typeOfOperation = DELETE_OPERATION;
	return ACS_CC_FAILURE;

}
/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::modify
 ============================================================================ */
ACS_CC_ReturnType ACS_USA_AdhIntUserObserver::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void) oiHandle;
	(void) ccbId;
	traObserverAdm.traceAndLogFmt(DEBUG, "ACS_USA_AdhIntUserObserver:%s(): ObjectModifyCallback invocated", __func__);
	traObserverAdm.traceAndLogFmt(DEBUG, "ACS_USA_AdhIntUserObserver:%s(): Object Name: %s", __func__, objName);

	traObserverAdm.traceAndLogFmt(DEBUG, "ACS_USA_AdhIntUserObserver:%s(): Entering.", __func__);
	
	for(size_t idx = 0U; (NULL != attrMods[idx]); idx++)
	{
		ACS_APGCC_AttrValues modAttribute = attrMods[idx]->modAttr;
		if( (0 == imm::AxeAdditionalInfo::userName.compare(modAttribute.attrName)))
		{
			if((modAttribute.attrValuesNum != 0))
			{
			userName.assign(reinterpret_cast<char*>( modAttribute.attrValues[0]));
			}
			else
			{
				userName.assign("");
			}
			traObserverAdm.traceAndLogFmt(WARNING,"UserName to Monitored: %s", userName.c_str());
			typeOfOperation = MODIFY_OPERATION_USER_SET;
		}
		
		if( (0 == imm::AxeAdditionalInfo::status.compare(modAttribute.attrName)) && (modAttribute.attrValuesNum != 0))
		{
			state = StateType((*reinterpret_cast<int*>(modAttribute.attrValues[0])));
			traObserverAdm.traceAndLogFmt(WARNING,"Montoring Status = %d   [0 - DISABLE and 1 - ENABLE]", state);
			typeOfOperation = MODIFY_OPERATION_STATE_SET;
		}
	}
	traObserverAdm.traceAndLogFmt(DEBUG, "ACS_USA_AdhIntUserObserver:%s(): Exiting.", __func__);
	return ACS_CC_SUCCESS;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::complete
 ============================================================================ */
ACS_CC_ReturnType ACS_USA_AdhIntUserObserver::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): CcbCompleteCallback invocated", __func__);
	ACS_CC_ReturnType result = ACS_CC_FAILURE;

	return result;
}

/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::abort
 ============================================================================ */
void ACS_USA_AdhIntUserObserver::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): CcbAbortCallback invocated", __func__);
}


/*============================================================================
        ROUTINE: ACS_USA_AdhIntUserObserver::apply
 ============================================================================ */
void ACS_USA_AdhIntUserObserver::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	traObserverAdm.traceAndLogFmt(INFO, "ACS_USA_AdhIntUserObserver:%s(): CcbApplyCallback invocated", __func__);

	if (typeOfOperation == MODIFY_OPERATION_USER_SET)
	{
		ACS_USA_Global::instance()->Analyser()->userDetails()->setUserName(userName);
		userName.clear();
	}

	if (typeOfOperation == MODIFY_OPERATION_STATE_SET)
	{
		if (state == DISABLE || state == ENABLE)
		{
			ACS_USA_Global::instance()->Analyser()->userDetails()->setMonitoingStatus(state);
			state = NOSTATE;
		}
		typeOfOperation = NONE;
	}
}
