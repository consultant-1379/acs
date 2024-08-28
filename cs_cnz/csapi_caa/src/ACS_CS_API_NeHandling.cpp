/*
 * @file ACS_CS_API_NeHandling.cpp
 * @author xmikhal
 * @date Dec 21, 2010
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#include "ACS_CS_API_NeHandling.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"

#include "acs_apgcc_paramhandling.h"

#include "ACS_CS_API_Tracer.h"
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_NeHandling_TRACE);

using namespace ACS_CS_Protocol;

bool ACS_CS_API_NeHandling::startupProfileSupervisionComplete()
{
	bool complete = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(info && info->startupProfileSupervision != 0) {
		complete = true;
	}

	if (!info){
		ACS_CS_API_TRACER_MESSAGE("Failed to get property startupProfileSupervisionComplete, assuming false");
	}

	delete base;
	return complete;
}


bool ACS_CS_API_NeHandling::cancelClusterOpModeSupervisionTimer()
{
    bool cancelled = true;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for cancelClusterOpModeSupervisionTimer (1)");

		delete base;
		return false;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	info->cancelClusterOpModeSupervision = true;
	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);

	ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
	saved = saver->save(__FUNCTION__);
	delete saver;

	if(saved == ACS_CC_FAILURE){
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for cancelClusterOpModeSupervisionTimer (2)");
		cancelled = false;
	}

	//delete info;
	delete model;

    return cancelled;
}


bool ACS_CS_API_NeHandling::cancelOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::PhaseValue phase)
{
	bool cancelled = true;
	int phaseValue = static_cast<int>(phase);
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for cancelOmProfileSupervisionTimer (1)");
        if (base)
        	delete base;

		return false;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	switch(phaseValue){
		case 0: info->cancelProfileSupervision  = VALIDATE; break;
		case 1: info->cancelProfileSupervision  = AP_NOTIFY; break;
		case 2: info->cancelProfileSupervision  = CP_NOTIFY; break;
		case 3: info->cancelProfileSupervision  = COMMIT; break;
		default: info->cancelProfileSupervision = IDLE; break;
	}

	info->action = ACS_CS_ImBase::MODIFY;
	model->addObject(info);

   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
//   saved = saver->save(); TODO
   delete saver;

	if(saved == ACS_CC_FAILURE){
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for cancelOmProfileSupervisionTimer (2)");
		cancelled = false;
	}

	delete model;

    return cancelled;
}


bool ACS_CS_API_NeHandling::setOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase, int timeout)
{
	bool result = false;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for setOmProfileSupervisionTimer (1)");
		if (base)
			delete base;
		return result;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	info->action = ACS_CS_ImBase::MODIFY;
	info->omProfileSupervisionTimeout = timeout;

	switch(phase)
	{
		case ACS_CS_API_OmProfilePhase::ApNotify:
			info->omProfileSupervisionPhase = AP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::CpNotify:
			info->omProfileSupervisionPhase = CP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::Commit:
			info->omProfileSupervisionPhase = COMMIT;
			break;
		case ACS_CS_API_OmProfilePhase::Validate:
			info->omProfileSupervisionPhase = VALIDATE;
			break;
		case ACS_CS_API_OmProfilePhase::Idle:
			info->omProfileSupervisionPhase = IDLE;
			break;
		default: info->omProfileSupervisionPhase = UNDEF_OMPROFILEPHASE;
	}

	model->addObject(info);

	ACS_CS_ImModelSaver saver(model);
	saved = saver.save(__FUNCTION__);

	if(saved == ACS_CC_SUCCESS){
		result = true;
	}
	else{
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for setOmProfileSupervisionTimer (2)");
	}

	delete model;

	return result;

}


int ACS_CS_API_NeHandling::getNeSubscriberCount()
{
    int subscriberCount = 0;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE("Failed to get property getNeSubscriberCount");
	}
	else {
		subscriberCount = info->neSubscriberCount;
	}

	delete base;
    return subscriberCount;
}


void ACS_CS_API_NeHandling::setNeSubscriberCount(int /*subscribers*/)
{
//	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
//	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
//	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);
//
//	delete immReader;
//
//	if(!info) {
//		ACS_CS_TRACE((ACS_CS_API_NeHandling_TRACE,
//				"ACS_CS_API_NeHandling::setNeSubscriberCount\n"
//				"Failed to set property setNeSubscriberCount (1)"));
//		return;
//	}
//
//	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
//	ACS_CS_ImModel *model = new ACS_CS_ImModel();
//
//	info->neSubscriberCount = subscribers;
//	info->action = ACS_CS_ImBase::MODIFY;
//
//	model->addObject(info);
//   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
//   saved = saver->save();
//   delete saver;
//
//	if(saved == ACS_CC_FAILURE){
//		ACS_CS_TRACE((ACS_CS_API_NeHandling_TRACE,
//				"ACS_CS_API_NeHandling::setNeSubscriberCount\n"
//				"Failed to set property setNeSubscriberCount (2)"));
//	}
//
//	//delete info;
//	delete model;
}


unsigned int ACS_CS_API_NeHandling::getPhaseSubscriberCount (ACS_CS_API_OmProfilePhase::PhaseValue phase)
{

	unsigned int count = 0;
    ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
    ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
    ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

    if(!info) {
    	ACS_CS_API_TRACER_MESSAGE("Failed to get property getPhaseSubscriberCount");

    	delete base;
    	return count;
    }

    switch (phase)
    {
		case ACS_CS_API_OmProfilePhase::Validate:
			count = info->phaseSubscriberCountValidate;
			break;
		case ACS_CS_API_OmProfilePhase::ApNotify:
			count = info->phaseSubscriberCountApNotify;
			break;
		case ACS_CS_API_OmProfilePhase::CpNotify:
			count = info->phaseSubscriberCountCpNotify;
			break;
		case ACS_CS_API_OmProfilePhase::Commit:
			count = info->phaseSubscriberCountCommit;
			break;
		default:
			ACS_CS_API_TRACER_MESSAGE("Error! Requesting invalid phase value");
			break;
    }


	delete info;
    return count;
}


bool ACS_CS_API_NeHandling::setPhaseSubscriberCount (ACS_CS_API_NE_NS::SetPhaseSubscriberData &/*phaseData*/)
{
//	bool result = false;
//
//	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
//	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
//	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);
//
//	delete immReader;
//
//	if(!info) {
//		ACS_CS_TRACE((ACS_CS_API_NeHandling_TRACE,
//				"ACS_CS_API_NeHandling::setPhaseSubscriberCount()\n"
//				"Failed to set property for setting the phase subscriber count (1)"));
//		return result;
//	}
//
//	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
//	ACS_CS_ImModel *model = new ACS_CS_ImModel();
//
//	info->phaseSubscriberCountApNotify = phaseData.apNotify;
//	info->phaseSubscriberCountCpNotify = phaseData.cpNotify;
//	info->phaseSubscriberCountCommit   = phaseData.commit;
//	info->phaseSubscriberCountValidate = phaseData.validate;
//
//	info->action = ACS_CS_ImBase::MODIFY;
//
//	model->addObject(info);
//   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
//   saved = saver->save();
//   delete saver;
//
//	if(saved == ACS_CC_SUCCESS){
//		result = true;
//	}
//	else{
//		ACS_CS_TRACE((ACS_CS_API_NeHandling_TRACE,
//				"ACS_CS_API_NeHandling::setPhaseSubscriberCount()\n"
//				"Failed to set property for setting the phase subscriber count (2)"));
//	}
//
//	//delete info;
//	delete model;
//
//	return result;

	//dapl: temporarily, it causes many IMM callbacks problem
	return true;
}


bool ACS_CS_API_NeHandling::updatePhaseChange (ACS_CS_API_OmProfileChange &newProfile)
{
	bool result = false;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	delete immReader;

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for updating phase change (1)");

		if (base)
			delete base;
		return result;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	info->aptProfileCurrent		= newProfile.aptCurrent;
	info->aptProfileQueued		= newProfile.aptQueued;
	info->aptProfileRequested	= newProfile.aptRequested;

	info->apzProfileCurrent		= newProfile.apzCurrent;
	info->apzProfileQueued		= newProfile.apzQueued;
	info->apzProfileRequested	= newProfile.apzRequested;

	info->omProfileCurrent = newProfile.omProfileCurrent;
	info->omProfileRequested = newProfile.omProfileRequested;

	switch(newProfile.changeReason)
	{
		case ACS_CS_API_OmProfileChange::NoChange:
			info->omProfileChangeTrigger = NO_CHANGE;
			break;
		case ACS_CS_API_OmProfileChange::NechCommand:
			info->omProfileChangeTrigger = MANUAL;
			break;
		case ACS_CS_API_OmProfileChange::NechCommandForApgOnly:
			info->omProfileChangeTrigger = MANUAL_FOR_APG_ONLY;
			break;
		case ACS_CS_API_OmProfileChange::AutomaticProfileAlignment:
			info->omProfileChangeTrigger = AUTOMATIC;
			break;
		default:
			info->omProfileChangeTrigger = UNDEF_PROFILECHANGETRIGGER;
	}

	switch(newProfile.phase)
	{
		case ACS_CS_API_OmProfilePhase::ApNotify:
			info->omProfilePhase = AP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::CpNotify:
			info->omProfilePhase = CP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::Commit:
			info->omProfilePhase = COMMIT;
			break;
		case ACS_CS_API_OmProfilePhase::Validate:
			info->omProfilePhase = VALIDATE;
			break;
		case ACS_CS_API_OmProfilePhase::Idle:
			info->omProfilePhase = IDLE;
			break;
		default: info->omProfilePhase = UNDEF_OMPROFILEPHASE;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);
   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
//   saved = saver->save();
   delete saver;

	if(saved == ACS_CC_SUCCESS){
		result = true;
	}
	else{
		ACS_CS_API_TRACER_MESSAGE("Failed to set property for updating phase change (2)");
	}

	delete model;

	return result;
}


ACS_CS_Table* ACS_CS_API_NeHandling::loadNeTable ()
{
	ACS_CS_Table *table = NULL;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advInfo = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);
	ACS_CS_ImBase *bcBase= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	ACS_CS_ImCpCluster* bcInfo = dynamic_cast<ACS_CS_ImCpCluster*>(bcBase);

	delete immReader;

	if(!advInfo || !bcInfo) {
		ACS_CS_API_TRACER_MESSAGE("Failed to connect to IMM");
		if (advBase)
			delete advBase;
		if (bcBase)
			delete bcBase;
		return NULL;
	}

	table = new ACS_CS_Table();
	table->setTableType(ACS_CS_INTERNAL_API::Table_NE);

	ACS_CS_TableEntry omEntry(ACS_CS_NS::ENTRY_ID_OM_PROFILE);
	ACS_CS_TableEntry alEntry(ACS_CS_NS::ENTRY_ID_ALARM_MASTER);
	ACS_CS_TableEntry clEntry(ACS_CS_NS::ENTRY_ID_CLOCK_MASTER);
	ACS_CS_TableEntry opEntry(ACS_CS_NS::ENTRY_ID_CLUSTER_OP_MODE);
	ACS_CS_TableEntry tiEntry(ACS_CS_NS::ENTRY_ID_TRAFFIC_ISOLATED);
	ACS_CS_TableEntry tlEntry(ACS_CS_NS::ENTRY_ID_TRAFFIC_LEADER);
	ACS_CS_TableEntry neEntry(ACS_CS_NS::ENTRY_ID_NETWORK_IDENTIFIER);

	ACS_CS_Attribute attOmProfileCurrent(Attribute_NE_OmProfileCurrent);
	attOmProfileCurrent.setValue(reinterpret_cast<char*>(&advInfo->omProfileCurrent), sizeof(advInfo->omProfileCurrent));
	omEntry.setValue(attOmProfileCurrent);

	ACS_CS_Attribute attOmProfileRequested(Attribute_NE_OmProfileRequested);
	attOmProfileRequested.setValue(reinterpret_cast<char*>(&advInfo->omProfileRequested), sizeof(advInfo->omProfileRequested));
	omEntry.setValue(attOmProfileRequested);

	ACS_CS_Attribute attAptProfileCurrent(Attribute_NE_AptProfileCurrent);
	attAptProfileCurrent.setValue(reinterpret_cast<char*>(&advInfo->omProfileCurrent), sizeof(advInfo->omProfileCurrent));
	omEntry.setValue(attAptProfileCurrent);

	ACS_CS_Attribute attAptProfileRequested(Attribute_NE_AptProfileRequested);
	attAptProfileRequested.setValue(reinterpret_cast<char*>(&advInfo->aptProfileRequested), sizeof(advInfo->aptProfileRequested));
	omEntry.setValue(attAptProfileRequested);

	ACS_CS_Attribute attAptProfileQueued(Attribute_NE_AptProfileQueued);
	attAptProfileQueued.setValue(reinterpret_cast<char*>(&advInfo->aptProfileQueued), sizeof(advInfo->aptProfileQueued));
	omEntry.setValue(attAptProfileQueued);

	ACS_CS_Attribute attApzProfileCurrent(Attribute_NE_ApzProfileCurrent);
	attApzProfileCurrent.setValue(reinterpret_cast<char*>(&advInfo->apzProfileCurrent), sizeof(advInfo->apzProfileCurrent));
	omEntry.setValue(attApzProfileCurrent);

	ACS_CS_Attribute attApzProfileRequested(Attribute_NE_ApzProfileRequested);
	attApzProfileRequested.setValue(reinterpret_cast<char*>(&advInfo->apzProfileRequested), sizeof(advInfo->apzProfileRequested));
	omEntry.setValue(attApzProfileRequested);

	ACS_CS_Attribute attApzProfileQueued(Attribute_NE_ApzProfileQueued);
	attApzProfileQueued.setValue(reinterpret_cast<char*>(&advInfo->apzProfileQueued), sizeof(advInfo->apzProfileQueued));
	omEntry.setValue(attApzProfileQueued);

	ACS_CS_Attribute attPhase(Attribute_NE_Phase);
	attPhase.setValue(reinterpret_cast<char*>(&advInfo->omProfilePhase), sizeof(advInfo->omProfilePhase));
	omEntry.setValue(attPhase);

	ACS_CS_Attribute attChangeReason(Attribute_NE_ProfileChangeReason);
	attChangeReason.setValue(reinterpret_cast<char*>(&advInfo->omProfileChangeTrigger), sizeof(advInfo->omProfileChangeTrigger));
	omEntry.setValue(attChangeReason);

	ACS_CS_Attribute attalarmMaster(Attribute_NE_AlarmMaster);
	attalarmMaster.setValue(reinterpret_cast<char*>(&bcInfo->alarmMaster), sizeof(bcInfo->alarmMaster));
	alEntry.setValue(attalarmMaster);

	ACS_CS_Attribute attclockMaster(Attribute_NE_ClockMaster);
	attclockMaster.setValue(reinterpret_cast<char*>(&bcInfo->clockMaster), sizeof(bcInfo->clockMaster));
	clEntry.setValue(attclockMaster);

	ACS_CS_Attribute attClusterOpMode(Attribute_NE_ClusterOpMode);
	attClusterOpMode.setValue(reinterpret_cast<char*>(&bcInfo->clusterOpMode), sizeof(bcInfo->clusterOpMode));
	opEntry.setValue(attClusterOpMode);

	ACS_CS_Attribute attClusterOpModeType(Attribute_NE_ClusterOpModeType);
	attClusterOpModeType.setValue(reinterpret_cast<char*>(&bcInfo->clusterOpModeType), sizeof(bcInfo->clusterOpModeType));
	opEntry.setValue(attClusterOpModeType);

	ACS_CS_Attribute attTrafficIsolated(Attribute_NE_TrafficIsolated);
	attTrafficIsolated.setValue(reinterpret_cast<char*>(&advInfo->trafficIsolatedCp), sizeof(advInfo->trafficIsolatedCp));
	tiEntry.setValue(attTrafficIsolated);

	ACS_CS_Attribute attTrafficLeader(Attribute_NE_TrafficLeader);
	attTrafficLeader.setValue(reinterpret_cast<char*>(&advInfo->trafficLeaderCp), sizeof(advInfo->trafficLeaderCp));
	tlEntry.setValue(attTrafficLeader);

	char neId[256] = {0};
	acs_apgcc_paramhandling acsParamHandling;
	acsParamHandling.getParameter<256>(ACS_CS_ImmMapper::RDN_MANAGEDELEMENT, "networkManagedElementId", neId);

	ACS_CS_Attribute attNetworkIdentifier(Attribute_NE_NetworkIdentifier);
	attNetworkIdentifier.setValue(neId, sizeof(neId));
	neEntry.setValue(attNetworkIdentifier);

	table->addEntry(omEntry);
	table->addEntry(alEntry);
	table->addEntry(clEntry);
	table->addEntry(opEntry);
	table->addEntry(tiEntry);
	table->addEntry(tlEntry);
	table->addEntry(neEntry);

	delete advInfo;
	delete bcInfo;

    return table;
}
