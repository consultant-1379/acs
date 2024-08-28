/*=================================================================== */
/**
   @file acs_emf_root_ObjectImpl.cpp

   Class method implementation for EMF module.

   This module contains the implementation of class declared in
   the EMF Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     XRAMMAT       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/Signal.h>
#include "acs_emf_root_ObjectImpl.h"
#include "acs_emf_tra.h"
#include <acs_emf_defs.h>

using namespace std;

/*===================================================================
   ROUTINE: ACS_EMF_ObjectImpl
=================================================================== */
ACS_EMF_ObjectImpl::ACS_EMF_ObjectImpl(string szimpName )
:acs_apgcc_objectimplementereventhandler_V2( szimpName )
{
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::ACS_EMF_ObjectImpl - Entering");
	theOiHandlerPtr = new acs_apgcc_oihandler_V2();
	if( theOiHandlerPtr == 0 )
	{
		ERROR(1,"%s", "ACS_EMF_ObjectImpl::ACS_EMF_ObjectImpl - Memory allocation failed for  acs_apgcc_oihandler_V2");
	}
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::ACS_EMF_ObjectImpl - Leaving");
}//End of ACS_EMF_ObjectImpl

/*===================================================================
   ROUTINE: ~ACS_EMF_ObjectImpl
=================================================================== */
ACS_EMF_ObjectImpl::~ACS_EMF_ObjectImpl()
{
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::~ACS_EMF_ObjectImpl - Entering");
	if( theOiHandlerPtr != 0 )
	{
		theOiHandlerPtr->removeClassImpl(this,ACS_EMF_ROOT_CLASS_NAME);
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::~ACS_EMF_ObjectImpl - Leaving");
}//End of ~ACS_EMF_ObjectImpl

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 ACS_EMF_ObjectImpl::svc( ACE_Reactor* &poReactor )
{
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::svc Dispatch - Entering");

	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_EMF_ROOT_CLASS_NAME);

	if ( errorCode == ACS_CC_FAILURE )
	{
		ERROR(1, "ACS_EMF_ObjectImpl::svc Dispatch - ERROR: Set implementer %s for the object failure ",getImpName().c_str(), getObjName().c_str() );
		DEBUG(1,"%s","ACS_EMF_ObjectImpl::svc Dispatch - Leaving");
		return -1;
	}
	else
	{
		DEBUG(1,"ACS_EMF_ObjectImpl::svc Dispatch - Success: Set implementer %s for the object ",getImpName().c_str(), getObjName().c_str() );
	}

	dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::svc Dispatch - Leaving");
	return 0;
}//End of svc

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType ACS_EMF_ObjectImpl::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
    (void) oiHandle;
    (void) ccbId;
    (void) className;
    (void) parentName;
    (void) attr;

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::create Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::create Dispatch - Leaving");

	return ACS_CC_FAILURE;
}//End of create
/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType ACS_EMF_ObjectImpl::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::deleted Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::deleted Dispatch - Leaving");
	
	return ACS_CC_FAILURE;
}//End of deleted
/*===================================================================
   ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType ACS_EMF_ObjectImpl::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void) attrMods;

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::modify Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::modify Dispatch - Leaving");

	return ACS_CC_FAILURE;
}//End of modify
/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType ACS_EMF_ObjectImpl::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::complete Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::complete Dispatch - Leaving");
	
	return ACS_CC_SUCCESS;

}//End of complete
/*===================================================================
   ROUTINE: abort
=================================================================== */
void ACS_EMF_ObjectImpl::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::abort Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::abort Dispatch - Leaving");
	
}//End of abort
/*===================================================================
   ROUTINE: apply
=================================================================== */
void ACS_EMF_ObjectImpl::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::apply Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::apply Dispatch - Leaving");
	
}//End of apply
/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType ACS_EMF_ObjectImpl::updateRuntime(const char *objName, const char *attrName)
{
	(void) objName;
	(void) attrName;
	
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::updateRuntime Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::updateRuntime Dispatch - Leaving");

	return ACS_CC_FAILURE;
}//End of updateRuntime
/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void ACS_EMF_ObjectImpl::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) operationId;
	(void) paramList;
	(void) p_objName;

	DEBUG(1,"%s","ACS_EMF_ObjectImpl::adminOperationCallback Dispatch - Entering");
	DEBUG(1,"%s","ACS_EMF_ObjectImpl::adminOperationCallback Dispatch - Leaving");

}//End of adminOperationCallback


