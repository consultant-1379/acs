/*
 * ACS_CS_MethodHandler.h
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_METHODHANDLER_H_
#define ACS_CS_METHODHANDLER_H_
#include "ACS_CS_ImmIncomingHandler.h"
#include "string.h"
#include "vector"
#include "ACS_CS_BladeHandler.h"
#include "ACS_CS_ImBase.h"

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_LogicalMgmtHandler.h"
#include "ACS_CS_CpClusterHandler.h"
#include "ACS_CS_ApHandler.h"
#include "ACS_CS_CpHandler.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_OmProfileHandler.h"
#include "ACS_CS_CcFileHandler.h"
#include "ACS_CS_OmProfileManager.h"
#include "ACS_CS_CandidateCcFile.h"
#include "ACS_CS_ComputeResourceHandler.h"
#include "ACS_CS_TransportHandler.h"

class ACS_CS_MethodHandler : public ACS_CS_ImmIncomingHandler {

public:
	ACS_CS_MethodHandler();

	ACS_CS_MethodHandler(ACS_CS_ComputeResourceHandler* p_computeResourceHandler);

	virtual ~ACS_CS_MethodHandler();

	int create();
	int modify();
	int remove();


	void setObject(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			const char *className,
			const char *parentName,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base);

	void setObject(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal,
			const char *className,
			const char *parentName,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			bool borrowedCcbHandles);


	int handleApBlade();
	int handleCpBlade();
	void setComputeResourceHandlerObj(ACS_CS_ComputeResourceHandler* p_computeResourceHandler);
private:

    void deleteObj();
	ACS_CS_BladeHandler 	  	*bladehandler;
	ACS_CS_LogicalMgmtHandler 	*logicalMgmtH;
	ACS_CS_CpClusterHandler		*cpclusterHan;
	ACS_CS_ApHandler			*apHandler;
	ACS_CS_CpHandler			*cpHandler;
	ACS_CS_OmProfileHandler		*omProfileHandler;
	ACS_CS_CcFileHandler		*ccFileHandler;
	ACS_CS_OmProfileManager		*omProfileMgrHandler;
	ACS_CS_CandidateCcFile		*candidateCcFileHandler;
	ACS_CS_TransportHandler		*transportHandler;
	ACS_CS_CrMgmtHandler		*crMgmtHandler;
	ACS_CS_ComputeResourceHandler *computeResourceHandler;


	std::vector<ACS_CS_ImmIncomingHandler*> ichs;

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_CS_ImBase *base;

	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

	bool borrowedCcbHandles;
        
};

#endif /* ACS_CS_METHODHANDLER_H_ */
