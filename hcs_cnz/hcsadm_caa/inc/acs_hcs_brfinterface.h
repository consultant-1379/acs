//******************************************************************************
//  NAME
//     acs_hcs_brfinterface.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2013. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//******************************************************************************

#ifndef BRFIMPLEMENTER_H_ 
#define BRFIMPLEMENTER_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <poll.h>
#include <ace/ACE.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Task.h>
#include <ACS_CC_Types.h>
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_oihandler_V3.h"
#include "acs_apgcc_paramhandling.h"

namespace AcsHcs
{
	class acs_hcs_brfthread;
	
	class acs_hcs_brfimplementer: public acs_apgcc_objectimplementerinterface_V3, public ACE_Task_Base
	{
		public :
			acs_hcs_brfimplementer();

			acs_hcs_brfimplementer(string p_impName );

			acs_hcs_brfimplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );

			~acs_hcs_brfimplementer(){};

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

			int ResponseToBrfc(unsigned long long, int, int);
			
			static bool getBrfStatus();

			void createBrfcThread();

			int createParticipant();

			int deleteParticipant();

			void createBrfcRunTimeOwner();

			bool initializeBrfcRunTimeOwner();

			int svc(void);

			acs_hcs_brfimplementer* theHcBrfcImpl;

			acs_hcs_brfthread* theHcBrfcRuntimeHandlerThreadPtr;

			static bool stopRequested;

			static bool brfThrCreated;

		private:
			static bool isBrfInProgress;
	};

	class acs_hcs_brfthread: public ACE_Task_Base
	{
		public :
			acs_hcs_brfthread();

			acs_hcs_brfthread(acs_hcs_brfimplementer *pImpl);

			~acs_hcs_brfthread();

			void setImpl(acs_hcs_brfimplementer *pImpl);

			void deleteImpl(acs_hcs_brfimplementer *pImpl);

			bool stop();

			int svc(void);

			static bool theIsStop;

			int m_StopEvent;

		private:
			acs_apgcc_oihandler_V3 oiHandler;

			acs_hcs_brfimplementer *theBrfHcImplementer;
	};
}
#endif
