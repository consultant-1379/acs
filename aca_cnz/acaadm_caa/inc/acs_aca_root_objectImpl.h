//=============================================================================
/**
   @file    acs_aca_root_objectImpl.h

   @brief Header file for ACA module.
          It acts as OI for handling IMM Callbacks of ACA root object.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/10/2011   XRAMMAT   Initial Release
 */
//=============================================================================
#ifndef ACS_ACA_ROOTOBJECTIMPL_H
#define ACS_ACA_ROOTOBJECTIMPL_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>

#include "ace/ace_wchar.h"
#include "ace/Synch.h"
#include "ace/Event_Handler.h"
#include "ace/Reactor.h"
#include "ace/TP_Reactor.h"
#include "ace/Task.h"

#include "acs_apgcc_objectimplementereventhandler_V2.h"
#include "acs_apgcc_oihandler_V2.h"

#include "acs_aca_msd_service.h"

class ACS_MSD_Service;

class ACS_ACA_ObjectImpl : public acs_apgcc_objectimplementereventhandler_V2, public ACE_Task_Base {
public:
	ACS_ACA_ObjectImpl (std::vector<std::string> aca_classes_name, std::string impl_name, ACS_APGCC_ScopeT p_scope);
	inline virtual ~ACS_ACA_ObjectImpl () {}

	ACS_CC_ReturnType create (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,	const char * className,	const char * parentname, ACS_APGCC_AttrValues ** attr);
	ACS_CC_ReturnType deleted (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char * objName);
	ACS_CC_ReturnType modify (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,	const char * objName, ACS_APGCC_AttrModification ** attrMods);
	ACS_CC_ReturnType complete (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	void abort (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	void apply (ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	ACS_CC_ReturnType updateRuntime (const char * objName, const char * attrName);
	void adminOperationCallback (ACS_APGCC_OiHandle oiHandle,	ACS_APGCC_InvocationType invocation, const char * p_objName, ACS_APGCC_AdminOperationIdType operationId, ACS_APGCC_AdminOperationParamType ** paramList);

	ACE_INT32 svc ();
	void shutdown ();

private:
	const char * get_error_message (ACS_MSD_Service::ParameterChangeErrorCode error_code, const char * param_to_change);

private:
	std::vector<std::string> _aca_classes_name;
	acs_apgcc_oihandler_V2 _oi_handler;
	ACE_TP_Reactor * _TP_reactor_impl;
	ACE_Reactor * _reactor;
};

#endif
