//******************************************************************************//
//  NAME
//     ACS_SCH_CrmHandler.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//
//
//  AUTHOR
//     2017-05-08  XBHABHE
//
//  SEE ALSO
//     -
//
//******************************************************************************

#include <iostream>
#include <ACS_APGCC_Util.H>
#include <ace/Signal.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_SCH_CrmHandler.h>
#include <sstream>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_ImUtils.h"
#include "ACS_SCH_Trace.h"
#include <time.h>
ACS_SCH_Trace_TDEF(ACS_SCH_CRMHANDLER_TRACE);   
//! Constructor
ACS_SCH_CrmHandler::ACS_SCH_CrmHandler( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)

{
	ACS_SCH_TRACE((ACS_SCH_CRMHANDLER_TRACE,"(%t) [ACS_SCH_CrmHandler] In constructor\n")); 
	m_isObjImplAdded=false; 
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}
	std::string ImpName=szImpName;
  if(ACS_CC_SUCCESS == immHandle.Init())
  {
    syslog(LOG_INFO, "[ACS_SCH_Server] immHandle is initialized");
  }
  else
  {
    syslog(LOG_INFO, "[ACS_SCH_Server] Initialization of immHandle is failed");
  }

}
//-----------------------------------------------------------------------------
ACS_SCH_CrmHandler::ACS_SCH_CrmHandler( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope,  ACE_Thread_Manager *aThrmgr )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)

{
	this->thr_mgr(aThrmgr);
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}

	m_isObjImplAdded=false;
  if(ACS_CC_SUCCESS == immHandle.Init())
  {
    syslog(LOG_INFO, "[ACS_SCH_Server] immHandle is initialized");
  }
  else
  {
    syslog(LOG_INFO, "[ACS_SCH_Server] Initialization of immHandle is failed");
  }

}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: ACS_SCH_CrmHandler Destructor
=================================================================== */
ACS_SCH_CrmHandler::~ACS_SCH_CrmHandler()
{
  if(ACS_CC_SUCCESS == immHandle.Finalize())
  {
    cout<< "[ACS_SCH_Server] immHandle is finalized"<<endl;
  }
  else
  {
    cout<<"[ACS_SCH_Server] immHandle finalization failed"<<endl;
  }

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
  if(theCrmHandler != 0)
  {
    delete theCrmHandler;
    theCrmHandler=0;
  }
}

bool ACS_SCH_CrmHandler::deleteErrorHandlerObject()
{
#if 0
	char myRDN[150];
	ACE_OS::sprintf(myRDN,"%s,%s",ACE_TEXT(ALDA_ERRORHANDLER_RDNOBJECTNAME),ACS_SSU_Common::dnOfSSURoot.c_str());
	if((theErrorRuntimeHandlerPtr->deleteRuntimeObj(myRDN)) == ACS_CC_FAILURE)
	{
		DEBUG ("%s \n","Failure in deleting Error runtime object");
		return false;
	}
#endif
	return true;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: createErrorHandlerObject
=================================================================== */
bool
ACS_SCH_CrmHandler::createErrorHandlerObject(ACS_APGCC_CcbId& aCCBId,unsigned int & aErrorId,std::string & aErrorText)
{
	(void) aCCBId;
	(void) aErrorId;
	(void) aErrorText;
	return true;
}
//----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 ACS_SCH_CrmHandler::svc()
{
	ACS_SCH_TRACE((ACS_SCH_CRMHANDLER_TRACE,"(%t) [ACS_SCH_CrmHandler] inside svc\n")); 
	theCrmHandler = 0;
	theCrmHandler = new acs_apgcc_oihandler_V3();
	try
	{
			if(ACS_CC_SUCCESS != immHandle.Init())
			{
				ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_CrmHandler] Imm handle initialization failed for CrM Handler"));
				return -1;
			}
			if ( theCrmHandler != 0)
			{
				for(int i=0;i<10;i++)
				{
					if(ACS_CC_FAILURE == theCrmHandler->addClassImpl(this, theClassName.c_str()) )
					{
									std::string errorText=immHandle.getInternalLastErrorText();
									ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_ERROR,"Adding implementer failed for CrM...%s",errorText.c_str()));
									sleep(0.5);
					}
					else
					{
									ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"Adding implementer for crm successful"));
									m_isObjImplAdded = true;
									break;
					}
				}
				if(m_isObjImplAdded == false)
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
		if (theReportReactorPtr->open(1) == 0 )
		{
			ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_CrmHandler] Reactor open failed for ImmReportHandler"));
			return -1;

		}
		dispatch(theReportReactorPtr, ACS_APGCC_DISPATCH_ALL);
		theReportReactorPtr->run_reactor_event_loop();
	}

	return 0;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType
ACS_SCH_CrmHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler]create callback"));
	(void) oiHandle;
	(void) ccbId;
	(void) className;
	(void) parentName;
	(void) attr;
	return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType
ACS_SCH_CrmHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler]deleted callback"));
	(void) oiHandle;
	(void) ccbId;
	(void) objName;
	return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_SCH_CrmHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler]modify callback "));
	(void) oiHandle;
	(void) ccbId;
	(void) objName;
	(void) attrMods;
	return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_SCH_CrmHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler]complete callback "));
	(void) oiHandle;
	(void) ccbId;
	return result;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: abort
=================================================================== */

void  ACS_SCH_CrmHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler] abort callback "));
	(void) oiHandle;
	(void) ccbId;
}


//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_SCH_CrmHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler]apply callback"));
	(void) oiHandle;
	(void) ccbId;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_SCH_CrmHandler::updateRuntime(const char *objName, const char **attrName)
{
	ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"[ACS_SCH_CrmHandler] updateRunTime callback"));
	(void) objName;
	(void) attrName;
	return ACS_CC_SUCCESS;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: setErrorText
=================================================================== */
bool ACS_SCH_CrmHandler::setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText)
{
	(void) ccbId;
	(void) aErrorId;
	(void) ErrorText;
	return true;
}

//-----------------------------------------------------------------------------


void ACS_SCH_CrmHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}
/*=====================================================================
   ROUTINE: shutdown
  =====================================================================*/

ACE_INT32 ACS_SCH_CrmHandler::shutdown()
{
	if( m_isObjImplAdded == true )
	{
		try
		{
				if ( theCrmHandler != 0)
				{
					if( theCrmHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
					{
						std::string errorText=immHandle.getInternalLastErrorText();
						ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_ERROR,"Adding of implementer for CrM failed %s",errorText.c_str()));
					}
					else
					{
						ACS_SCH_FTRACE((ACS_SCH_CRMHANDLER_TRACE,LOG_LEVEL_INFO,"Removal of implementer successful for CrmHandler."));
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

