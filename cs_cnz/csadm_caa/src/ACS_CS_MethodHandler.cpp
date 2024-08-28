
#include "ACS_CS_MethodHandler.h"


#include "vector"
#include "ACS_APGCC_Util.H"




void ACS_CS_MethodHandler::setComputeResourceHandlerObj(ACS_CS_ComputeResourceHandler* p_computeResourceHandler) {
	computeResourceHandler = p_computeResourceHandler;
}

ACS_CS_MethodHandler::ACS_CS_MethodHandler(): className(0),parentName(0),attr(0),base(0)
{
	bladehandler=0;
	logicalMgmtH=0;
	cpclusterHan=0;
	apHandler=0;
	cpHandler=0;
	omProfileHandler=0;
	ccFileHandler=0;
	omProfileMgrHandler=0;
	candidateCcFileHandler=0;
	transportHandler=0;
	crMgmtHandler=0;
	computeResourceHandler=0;
	borrowedCcbHandles=false;
}

ACS_CS_MethodHandler::ACS_CS_MethodHandler(ACS_CS_ComputeResourceHandler* p_computeResourceHandler):
		className(0),parentName(0),attr(0),base(0)
{
	bladehandler=0;
	logicalMgmtH=0;
	cpclusterHan=0;
	apHandler=0;
	cpHandler=0;
	omProfileHandler=0;
	ccFileHandler=0;
	omProfileMgrHandler=0;
	candidateCcFileHandler=0;
	transportHandler=0;
	crMgmtHandler = 0;
	computeResourceHandler=p_computeResourceHandler;
	borrowedCcbHandles=false;
}


ACS_CS_MethodHandler::~ACS_CS_MethodHandler() {
	deleteObj();
}


int ACS_CS_MethodHandler::create()
{
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	std::vector<ACS_CS_ImmIncomingHandler*>::iterator iter = ichs.begin();

	for( ; iter != ichs.end(); iter++) {

		int err = (*iter)->create();

		if (err != 0)
			return err;
	}

	if(!borrowedCcbHandles)
		applyAugmentCcb(ccbHandleVal);

	return 0;
}


int ACS_CS_MethodHandler::modify()
{
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	std::vector<ACS_CS_ImmIncomingHandler*>::iterator iter = ichs.begin();

	for( ; iter != ichs.end(); iter++) {
		(*iter)->modify();
	}

	if(!borrowedCcbHandles)
		applyAugmentCcb(ccbHandleVal);

	return 0;

}

int ACS_CS_MethodHandler::remove()
{
	ACS_CS_DEBUG(("CcbId[%d] - base-rdn == %s", ccbId, base->rdn.c_str()));
	std::vector<ACS_CS_ImmIncomingHandler*>::iterator iter = ichs.begin();

	for( ; iter != ichs.end(); iter++) {
		int err = (*iter)->remove();
		if (err != 0)
			return err;
	}
	if(!borrowedCcbHandles)
		applyAugmentCcb(ccbHandleVal);

	return 0;
}

void ACS_CS_MethodHandler::setObject(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		ACS_APGCC_CcbHandle &ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal,
		const char *className,
		const char *parentName,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
		bool borrowedCcbHandles)
{

	deleteObj();


	//getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);


	this->className=className;
	this->parentName=parentName;
	this->base=base;
	this->ccbHandleVal=ccbHandleVal;
	this->adminOwnwrHandlerVal=adminOwnwrHandlerVal;
	this->borrowedCcbHandles=borrowedCcbHandles;



	bladehandler=				new ACS_CS_BladeHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	logicalMgmtH=				new ACS_CS_LogicalMgmtHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	cpclusterHan= 				new ACS_CS_CpClusterHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	apHandler= 					new ACS_CS_ApHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	cpHandler= 					new ACS_CS_CpHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	omProfileHandler= 			new ACS_CS_OmProfileHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	ccFileHandler= 				new ACS_CS_CcFileHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	omProfileMgrHandler= 		new ACS_CS_OmProfileManager(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	candidateCcFileHandler= 	new ACS_CS_CandidateCcFile(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	transportHandler=			new ACS_CS_TransportHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	crMgmtHandler=				new ACS_CS_CrMgmtHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal, computeResourceHandler);


	if(base->type==APBLADE_T)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)logicalMgmtH);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)apHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
	}
	else if(base->type==CPBLADE_T && (base->action == ACS_CS_ImBase::CREATE || base->action == ACS_CS_ImBase::MODIFY) )
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)logicalMgmtH);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
		//ichs.push_back((ACS_CS_ImmIncomingHandler*)omProfileMgrHandler);
	}
	else if (base->type==CPBLADE_T && base->action == ACS_CS_ImBase::DELETE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
	}
	else if(base->type==OTHERBLADE_T)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);

	}
	else if (base->type==CPCLUSTER_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ACS_CS_DEBUG(("CcbId[%d] - CpClusterHandler & OmProfileManagerHandler for DN == %s", ccbId,base->rdn.c_str()));
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)omProfileMgrHandler);

	}
	else if(base->type==OMPROFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)omProfileHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)ccFileHandler);

	}
	else if(base->type==CCFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)ccFileHandler);
	}
	else if(base->type==CANDIDATECCFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)candidateCcFileHandler);
	}
	else if (base->type == CR_NETWORKSTRUCT_T || base->type == TRM_EXTERNALNETWORK_T || base->type == TRM_INTERNALNETWORK_T || base->type == TRM_INTERFACE_T)
	{
		ichs.push_back(transportHandler);
	}
	else if (base->type == COMPUTERESOURCE_T && base->action == ACS_CS_ImBase::DELETE)
	{
		ichs.push_back(crMgmtHandler);
	}

}


void ACS_CS_MethodHandler::setObject(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
		const char *className,
		const char *parentName,
		ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base)
{

	deleteObj();


	getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);


	this->className=className;
	this->parentName=parentName;
	this->base=base;



	bladehandler=				new ACS_CS_BladeHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	logicalMgmtH=				new ACS_CS_LogicalMgmtHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	cpclusterHan= 				new ACS_CS_CpClusterHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	apHandler= 					new ACS_CS_ApHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	cpHandler= 					new ACS_CS_CpHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	omProfileHandler= 			new ACS_CS_OmProfileHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	ccFileHandler= 				new ACS_CS_CcFileHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	omProfileMgrHandler= 		new ACS_CS_OmProfileManager(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	candidateCcFileHandler= 	new ACS_CS_CandidateCcFile(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	transportHandler=			new ACS_CS_TransportHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal);
	crMgmtHandler=				new ACS_CS_CrMgmtHandler(oiHandle,ccbId,attr,base,ccbHandleVal,adminOwnwrHandlerVal, computeResourceHandler);


	if(base->type==APBLADE_T)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)logicalMgmtH);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)apHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
	}
	else if(base->type==CPBLADE_T && (base->action == ACS_CS_ImBase::CREATE || base->action == ACS_CS_ImBase::MODIFY) )
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)logicalMgmtH);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
	}
	else if (base->type==CPBLADE_T && base->action == ACS_CS_ImBase::DELETE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);
	}
	else if(base->type==OTHERBLADE_T)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)bladehandler);

	}
	else if (base->type==CPCLUSTER_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ACS_CS_DEBUG(("CcbId[%d] - CpClusterHandler & OmProfileManagerHandler for DN == %s", ccbId,base->rdn.c_str()));
		ichs.push_back((ACS_CS_ImmIncomingHandler*)cpclusterHan);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)omProfileMgrHandler);

	}
	else if(base->type==OMPROFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)omProfileHandler);
		ichs.push_back((ACS_CS_ImmIncomingHandler*)ccFileHandler);

	}
	else if(base->type==CCFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)ccFileHandler);
	}
	else if(base->type==CANDIDATECCFILE_T && base->action == ACS_CS_ImBase::CREATE)
	{
		ichs.push_back((ACS_CS_ImmIncomingHandler*)candidateCcFileHandler);
	}
	else if (base->type == CR_NETWORKSTRUCT_T || base->type == TRM_EXTERNALNETWORK_T || base->type == TRM_INTERNALNETWORK_T || base->type == TRM_INTERFACE_T)
	{
		ichs.push_back(transportHandler);
	}
	else if (base->type == COMPUTERESOURCE_T && base->action == ACS_CS_ImBase::DELETE)
	{
		ichs.push_back(crMgmtHandler);
	}

}

void ACS_CS_MethodHandler::deleteObj()
{


	if(ichs.size()>0)
	{
			ichs.clear();
	}


	if(bladehandler)
	{
		delete bladehandler;
		bladehandler=0;
	}

	if(logicalMgmtH)
	{
		delete logicalMgmtH;
		logicalMgmtH=0;
	}

	if(cpclusterHan)
	{
		delete cpclusterHan;
		cpclusterHan=0;
	}


	if(apHandler)
	{
		delete apHandler;
		apHandler=0;
	}


	if(cpHandler)
	{
		delete cpHandler;
		cpHandler=0;

	}

	if(omProfileHandler)
	{
		delete omProfileHandler;
		omProfileHandler=0;

	}

	if(ccFileHandler)
	{
		delete ccFileHandler;
		ccFileHandler=0;

	}

	if(omProfileMgrHandler)
	{
		delete omProfileMgrHandler;
		omProfileMgrHandler=0;

	}

	if(candidateCcFileHandler)
	{
		delete candidateCcFileHandler;
		candidateCcFileHandler=0;

	}

	if(crMgmtHandler)
	{
		delete crMgmtHandler;
		crMgmtHandler = 0;
	}

	//DONT delete compu

}


