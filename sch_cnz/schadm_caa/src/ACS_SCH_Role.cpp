//******************************************************************************//
//  NAME
//     ACS_SCH_Role.cpp
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
//     2017-05-04  XBHABHE
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
#include <ACS_SCH_Role.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Task.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_ImUtils.h"
#include "ACS_SCH_Trace.h"
#include <sstream>
#include <time.h>

ACS_SCH_Trace_TDEF(ACS_SCH_ROLE_TRACE);
using namespace std;
//! Constructor
ACS_SCH_Role::ACS_SCH_Role( std::string ClassName,
		std::string szImpName,
		ACS_APGCC_ScopeT enScope )
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)

{
	m_isObjImplAdded=false; //COVERITY: 11281
	theReportReactorPtr = 0;
	theReportReactorImplPtr = 0;
	theReportReactorImplPtr = new ACE_TP_Reactor();
	if(theReportReactorImplPtr != 0)
	{
		theReportReactorPtr = new ACE_Reactor(theReportReactorImplPtr);
	}
	std::string ImpName=szImpName;
	req.tv_sec = 0;
	req.tv_nsec = 10000000; /*1 millisecond */
}
//-----------------------------------------------------------------------------
ACS_SCH_Role::ACS_SCH_Role( std::string ClassName,
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
	req.tv_sec = 0;
	req.tv_nsec = 10000000; /*1 millisecond */
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: ACS_SCH_Role Destructor
=================================================================== */
ACS_SCH_Role::~ACS_SCH_Role()
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

  if(theRoleHandler != 0)
  {
    delete theRoleHandler;
    theRoleHandler=0;
  }

	if(immHandle.Finalize()== ACS_CC_SUCCESS)
	{
		syslog(LOG_INFO, "[ACS_SCH_Role] immHandle is finalized");
	}
	else
	{
		syslog(LOG_INFO, "[ACS_SCH_Role] immHandle finalization failed");
	}
}

bool ACS_SCH_Role::deleteErrorHandlerObject()
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
ACS_SCH_Role::createErrorHandlerObject(ACS_APGCC_CcbId& aCCBId,unsigned int & aErrorId,std::string & aErrorText)
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
ACE_INT32 ACS_SCH_Role::svc()
{
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In svc function\n"));
	theRoleHandler = 0;
	theRoleHandler = new acs_apgcc_oihandler_V3();
	try
	{
			if(ACS_CC_SUCCESS != immHandle.Init())
			{
				ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_ERROR,"[ACS_SCH_Role]immHandle initialization of OmHandler failed"));
				return -1;
			}
			if ( theRoleHandler != 0)
			{
				for(int i=0;i<10;i++)
				{
					if(ACS_CC_FAILURE == theRoleHandler->addClassImpl(this, theClassName.c_str()) )
					{
						std::string errorText=immHandle.getInternalLastErrorText();
						ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_ERROR,"addition of implementer failed for Role retrying....%s",errorText.c_str()));
						sleep(0.5);
					}
					else
					{
						ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_INFO,"adding of implementer for Role successful"));
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
		if (0 == theReportReactorPtr->open(1) )
		{
			ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_ERROR,"Reactor open failed for ImmReportHandler"));
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
ACS_SCH_Role::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In create function\n"));
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
ACS_SCH_Role::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In deleted function\n"));
	(void) oiHandle;
	(void) ccbId;
	(void) objName;
	return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_SCH_Role::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In modify function\n"));
	(void) oiHandle;
	(void) ccbId;
	(void) objName;
	(void) attrMods;

	/*int i=0;
	while( attrMods[i])
  {
		if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,"isProvidedBy") == 0)
		{
			switch(attrMods[i]->modType)
        {
          case ACS_APGCC_ATTR_VALUES_REPLACE:
							cout<<"In replace"<<endl;
							break;	
          case ACS_APGCC_ATTR_VALUES_ADD:
							cout<<"In add"<<endl;
              break;
          case ACS_APGCC_ATTR_VALUES_DELETE:
							cout<<"In delete"<<endl;
              break;
					default:
						cout<<"In default"<<endl;
        }
		}
	i++;
	}*/

	return result;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_SCH_Role::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In complete function\n"));
	(void) oiHandle;
	(void) ccbId;
	return result;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: abort
=================================================================== */

void  ACS_SCH_Role::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In abort function\n"));
	(void) oiHandle;
	(void) ccbId;
}


//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_SCH_Role::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In apply function\n"));
	(void) oiHandle;
	(void) ccbId;
}

//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_SCH_Role::updateRuntime(const char *objName, const char **attrName)
{
	(void) attrName;
  (void) objName;
	return ACS_CC_SUCCESS;
}
//-----------------------------------------------------------------------------
/*===================================================================
   ROUTINE: setErrorText
=================================================================== */
bool ACS_SCH_Role::setErrorText(ACS_APGCC_CcbId ccbId, unsigned int aErrorId, std::string ErrorText)
{
	(void) ccbId;
	(void) aErrorId;
	(void) ErrorText;
	return true;
}

/*============================================================================
        ROUTINE: ACS_SCH_Role::adminOperationCallback
 ============================================================================ */
void ACS_SCH_Role::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	ACS_SCH_TRACE((ACS_SCH_ROLE_TRACE,"(%t) [ACS_SCH_Role] In adminOperationCallback function\n"));
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;
}
/*=====================================================================
   ROUTINE: shutdown
  =====================================================================*/

ACE_INT32 ACS_SCH_Role::shutdown()
{
	if( true == m_isObjImplAdded)
	{
		try
		{
				if ( theRoleHandler != 0)
				{
					if( ACS_CC_FAILURE == theRoleHandler->removeClassImpl(this, theClassName.c_str()))
					{
						std::string errorText=immHandle.getInternalLastErrorText();
						ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_ERROR,"Removal of implementer failed for Role",errorText.c_str()));
					}
					else
					{
						ACS_SCH_FTRACE((ACS_SCH_ROLE_TRACE,LOG_LEVEL_INFO,"Removal of implementer successful for Role"));
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
//-----------------------------------------------------------------------------
