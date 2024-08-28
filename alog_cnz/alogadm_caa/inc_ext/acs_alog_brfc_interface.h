//******************************************************************************
//
//  NAME
//     acs_alog_brfc_interface.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-07-12 by XCSSUHY PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************

#ifndef MYIMPLEMENTER_4_H_
#define MYIMPLEMENTER_4_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Task.h>
//#include "acs_lm_common.h"
//#include <acs_lm_clienthandler.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_apgcc_paramhandling.h>

class acs_alog_BrfImplementer: public acs_apgcc_objectimplementerinterface_V3
{
public :
	acs_alog_BrfImplementer();

	acs_alog_BrfImplementer(string p_impName );

	acs_alog_BrfImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );

	~acs_alog_BrfImplementer(){};

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char** p_nameAttr);

	/*the callback*/
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,

	ACS_APGCC_InvocationType invocation,

	const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,

	ACS_APGCC_AdminOperationParamType**paramList);

	//void setImpl(MyImplementer_4 *pImpl);

	int ResponseToBrfc(unsigned long long, int, int);

	static bool getBrfStatus();

private:
	 //acs_alog_BrfImplementer* theLMBrfcRuntimeOwnerPtr;

	static bool isBrfInProgress;
};

class acs_alog_BrfcThread: public ACE_Task_Base
{
public :
        acs_alog_BrfcThread();

        acs_alog_BrfcThread(acs_alog_BrfImplementer *pImpl);

        ~acs_alog_BrfcThread();

        int setImpl(acs_alog_BrfImplementer *pImpl);
        int deleteImpl(acs_alog_BrfImplementer *pImpl);

        void stop();
        void start();
        bool isRunning();
        inline bool isImplSet() { return isBrfcALOGImplementerSet; }

        int svc(void);
private:
        acs_apgcc_oihandler_V3 oiHandler;

        acs_alog_BrfImplementer *theBrfcALOGImplementer;

        bool isBrfcALOGImplementerSet;

        bool theIsStop;
};
// acs_alog_BrfcThread END

#endif
