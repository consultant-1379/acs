/*=================================================================== */
/**
   @file acs_emf_commandhandler.cpp

   Class  method implementation for EMF module.

   This module contains the implementation of class declared in
   the EMF Module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY   XSHYCHI      Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
#include<iostream>
#include <string>
#include <vector>
#include <ACS_CC_Types.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include "acs_emf_param.h"
#include "acs_emf_common.h"
#include "acs_emf_nanousbhandler.h"
#include "acs_emf_execute.h"
#include "acs_emf_tra.h"
#include "acs_emf_dsdserver.h"
#include "acs_emf_commandhandler.h"
#include "acs_emf_dvdhandler.h"
#include "acs_emf_aeh.h"
#include "acs_emf_cmdclient.h"
#include "acs_emf_cmdserver.h"
#include "acs_emf_param.h"
#include <acs_apgcc_adminoperation.h>
using namespace std;


ACS_EMF_MEDIAHandler* acs_emf_commandhandler::theMediaHandlePtr = NULL;
acs_emf_commandhandler* acs_emf_commandhandler::theInstance = NULL;
bool acs_emf_commandhandler::operationProgress = false;
bool acs_emf_commandhandler::theMediaStatusFlag = false;
bool acs_emf_commandhandler::isStopSignaled = false;
ACE_HANDLE acs_emf_commandhandler::m_stopHandle=ACE_INVALID_HANDLE;
ACE_INT32 acs_emf_commandhandler::theOperationalState = 0;
ACE_INT32 acs_emf_commandhandler::theMediaState = 0;
ACE_UINT16 acs_emf_commandhandler::theMediaOwner = NO_MEDIA;
extern const char* mediaOwnerFile;


/*===================================================================                                                                                                                                                    
   ROUTINE: acs_emf_commandhandler                                                                                                                                                                                       
=====================================================================*/
acs_emf_commandhandler::acs_emf_commandhandler(ACS_EMF_MEDIAHandler *pMediaHandler,ACE_HANDLE endEvent,
		string szImpName ):acs_apgcc_objectimplementereventhandler_V3( szImpName){
	DEBUG(1,"%s","acs_emf_commandhandler::acs_emf_commandhandler - Entering");
	// Clear DVD state
	//if( ACS_EMF_Common::GetHWVariant() != 3)
	//	clearDVDStatus();
	m_bIsDvdLocal = false; /* Modified due to Coverity Defects : Uninitialized scalar filed */
	m_iSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
	m_iRemoteDvdState = 0;
	m_enmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
	m_enmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED;
	theMediaHandlePtr = pMediaHandler;
	theOiHandlerPtr = new acs_apgcc_oihandler_V3();
	if( theOiHandlerPtr == 0 )
		ERROR(1,"%s", "acs_emf_commandhandler::acs_emf_commandhandler - Memory allocation failed for  acs_apgcc_oihandler_V2");
	errorString = "";
	errorCode = 0;
	theInstance = this;
	isMediaEmpty = false;
	m_stopHandle = endEvent;
	theFreeSpaceOnMedia = 0;
	theTotalSizeOfMedia = 0;
	theUsedSpaceOnMedia = 0;
	theMediaType = -1;
	theLastUpdatedTime = "-";
	theResultOfOperation = "-";
	theOperationalState = MEDIA_FEATURE_DISABLE;
	theMediaState = MEDIA_FEATURE_DISABLE;
	isStopSignaled = false;
	thr_grp_id = -1;

	// TR HR44823 - BEGIN
	theMediumType = "UNKNOWN";


	/** Create Reactor to handle the events**/
	m_poTp_reactor = new(std::nothrow) ACE_TP_Reactor;
	/** Create Reactor**/
	m_poReactor = new(std::nothrow) ACE_Reactor(m_poTp_reactor);
	// TR HR44823 - END

	if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1)
		INFO(1,"%s","acs_emf_commandhandler::acs_emf_commandhandler - unmounting media folder of FileM is failed!");
	if( ACS_EMF_Common::GetHWVariant() >= 3){
		DEBUG(1,"%s", "acs_emf_commandhandler::acs_emf_commandhandler - Calling ACS_EMF_Common::unMountUSBMedia().");
		ACS_EMF_Common::umountUSBData();
	}
	DEBUG(1,"%s","acs_emf_commandhandler::acs_emf_commandhandler - Exiting acs_emf_commandhandler::acs_emf_commandhandler");
}//End of acs_emf_commandhandler
/*===================================================================
ROUTINE: ~acs_emf_commandhandler
=================================================================== */
acs_emf_commandhandler::~acs_emf_commandhandler(){
	DEBUG(1,"%s","acs_emf_commandhandler::~acs_emf_commandhandler - Entering");
	// please do not delete theDVDHandlePtr; this is not our object. it is done in EMF server.
	if( theOiHandlerPtr != 0 ){
		DEBUG(1,"%s", "acs_emf_commandhandler::~acs_emf_commandhandler - Calling removeClassImpl.");
		//theOiHandlerPtr->removeClassImpl(this,ACS_APZ_IMM_APZ_CLASS_NAME);
		for (int i= 0; i < 10; ++i){
			if( theOiHandlerPtr->removeClassImpl(this,ACS_EMF_ROOT_CLASS_NAME) == ACS_CC_FAILURE ){
				int intErr = getInternalLastError();
				if ( intErr == -6){
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10)
						break;
					else
						continue;
				}
				else
					break;
			}
			else
				break;
		}
		delete theOiHandlerPtr;
		theOiHandlerPtr = 0;
	}
	if (theMediaOwner == MEDIA_ON_CURRENT_NODE && !isMediaEmpty) {
		if( ACS_EMF_Common::GetHWVariant() < 3)
			unmountDVDOnActiveNode(DSDSERVER_LOCK_MEDIA);
		else
		{
			if(ACS_EMF_Common::getMediaOperationState() == EMF_MEDIA_OPERATION_IS_IDLE){
				DEBUG(1,"%s", "acs_emf_commandhandler::~acs_emf_commandhandler - Calling ACS_EMF_Common::unMountUSBMedia().");
				ACS_EMF_Common::umountUSBData();
			}
		}
	}
	else if (theMediaOwner == MEDIA_ON_PARTNER_NODE) {// partner node
		//Performing umount only on active node.
		//During shutdown phase, service on passive node stops before service on active node.
		// This makes dsd server on passive node closes by this time. So, communication with passive will not possible.
		//unmountDVDOnPassiveNode(DSDSERVER_LOCK_MEDIA);  // Before of MERGE to CM105/106
		//if( ACS_EMF_Common::GetHWVariant() != 3){
		if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1)
			ERROR(1,"%s","acs_emf_commandhandler::~acs_emf_commandhandler - unmount nfs shared media folder from /data/opt/ap/internal_root/media is failed");
		//}
	}
	if(m_poReactor != 0){
		delete m_poReactor;
		m_poReactor=0;
	}
	if(m_poTp_reactor !=0 ){
		delete m_poTp_reactor;
		m_poTp_reactor = 0;
	}
	DEBUG(1,"%s","acs_emf_commandhandler::~acs_emf_commandhandler - Leaving");
}//End of ~acs_emf_commandhandler

/*===================================================================
ROUTINE: register_object
=================================================================== */
ACE_INT32 acs_emf_commandhandler::register_object(){
	DEBUG(1,"%s","acs_emf_commandhandler::register_object - Entering");
	ACS_CC_ReturnType errorCode = theOiHandlerPtr->addClassImpl(this,ACS_EMF_ROOT_CLASS_NAME);
	if ( errorCode == ACS_CC_FAILURE ){
		ERROR(1, "acs_emf_commandhandler::register_object - ERROR: Setting implementer %s for the class AxeExternalMediaExternalMediaM is failed",getImpName().c_str());
		DEBUG(1,"%s","acs_emf_commandhandler::register_object - Leaving");
		return EMF_RC_ERROR;
	}
	else
		INFO(1,"acs_emf_commandhandler::register_object - Setting implementer %s for the object AxeExternalMediaExternalMediaM is success",getImpName().c_str());
	//dispatch(poReactor, ACS_APGCC_DISPATCH_ALL);
	DEBUG(1,"%s","acs_emf_commandhandler::register_object() - Leaving");
	return EMF_RC_OK;
}//End of register_object
/*===================================================================
ROUTINE: svc
=====================================================================*/
int acs_emf_commandhandler::svc(){
	DEBUG(1,"%s","acs_emf_commandhandler::svc() - Entering");
	if (getMediaState(theInstance->theMediaState) == ACS_CC_FAILURE)
		ERROR(1,"%s","acs_emf_commandhandler::svc() - Unable to fetch mediaState attribute");
	if ((theInstance->theMediaState == 1) || (ACE_OS::access(mediaOwnerFile, F_OK) == 0)) {
		// EMF Feature was in ENABLED state before Node Failover.
		// Thus performs UnlockMedia operation so that media state will be in ENABLED state even after Failover.
		DEBUG(1,"%s","acs_emf_commandhandler::getMediaState - Unlocking media");
		ACS_CC_ReturnType result = acs_emf_commandhandler::unlockMedia();
		if( result == ACS_CC_FAILURE )
			ERROR(1,"%s","acs_emf_commandhandler::svc() - FAILED to UNLOCK Media after Node Failover");
        }
        // start of TR HZ70953
	if(m_poReactor != 0)
        {
            if(m_poReactor->open(1) == 0)
            {
                ERROR(1,"%s","acs_emf_commandhandler::svc() - Reactor open failed for CommandHandler");
                return EMF_RC_ERROR;
            }
                 
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
        // Start of TR IA58647
	try
	{
		m_poReactor->run_reactor_event_loop();
	}
	catch(const std::exception &e)
        {
		DEBUG(1,"%s","Exception: <%s>", e.what());
		DEBUG(1,"%s","Exception caught when calling run_reactor_event_loop()");
        }
	catch(...)
	{
		DEBUG(1,"%s","Unknown exception caught when calling run_reactor_event_loop()");
	}
	// End of TR IA58647
        } // End of TR HZ70953
	DEBUG(1,"%s","acs_emf_commandhandler::svc() - Leaving acs_emf_commandhandler::svc");
	return EMF_RC_OK;
}//end of svc
/*===================================================================
ROUTINE: shutdown
=====================================================================*/
void acs_emf_commandhandler::shutdown(){
	DEBUG(1,"%s","acs_emf_commandhandler::shutdown() - Entering");
	isStopSignaled=true;
	m_poReactor->end_reactor_event_loop();
	if (ACS_EMF_Common::killAllOutstandingPIDs() == -1)
		ERROR(1,"%s","acs_emf_commandhandler::shutdown - Killing outstanding PIDs failed\n");
	if(ACE_Thread_Manager::instance()->wait_grp(thr_grp_id) == 0)
		DEBUG(1,"%s","acs_emf_commandhandler::shutdown() - All admin operation threads closed successfully");
	if(this->wait()== -1)
		ERROR(1,"%s","acs_emf_commandhandler::shutdown() - join failed ");

	DEBUG(1,"%s","acs_emf_commandhandler::shutdown() - Leaving");
}//end of shutdown

/*===================================================================                                                                                                                                                    
ROUTINE: create                                                                                                                                                                                                       
=====================================================================*/
ACS_CC_ReturnType acs_emf_commandhandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){
	(void) oiHandle;
	(void) ccbId;
	(void) className;
	(void) parentName;
	(void) attr;

	DEBUG (1,"%s","acs_emf_commandhandler::create() - ObjectCreateCallback invocated!");

	return ACS_CC_FAILURE;
}//End of create
/*===================================================================
ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType acs_emf_commandhandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){
	(void)oiHandle;
	(void)ccbId;
	(void)objName;

	DEBUG (1,"%s","acs_emf_commandhandler::deleted() - ObjectDeletedCallback invocated!");

	return ACS_CC_FAILURE;
}//End of deleted
/*===================================================================
ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType acs_emf_commandhandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){
	(void)oiHandle;
	(void)objName;
	(void)ccbId;
	(void)attrMods;

	DEBUG (1,"%s","acs_emf_commandhandler::modify() - ObjectModifyCallback invocated!");
	for (int i = 0; attrMods[i] != NULL; i++ )
        {
                ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

                if (strcmp(modAttr->attrName, "mediaState") == 0)
                        return ACS_CC_SUCCESS;
	}

	return ACS_CC_FAILURE;
}//End of modify
/*===================================================================
ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType acs_emf_commandhandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
	(void)oiHandle;
	(void)ccbId;

	DEBUG (1,"%s","acs_emf_commandhandler::complete() - CcbCompleteCallback invocated!");

	return ACS_CC_SUCCESS;

}//End of complete
/*===================================================================
ROUTINE: abort
=================================================================== */
void acs_emf_commandhandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
	(void)oiHandle;
	(void)ccbId;

	DEBUG (1,"%s","acs_emf_commandhandler::abort() - CcbAbortCallback invocated!");

}//End of abort
/*===================================================================
ROUTINE: apply
=================================================================== */
void acs_emf_commandhandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId){
	(void)oiHandle;
	(void)ccbId;

	DEBUG (1,"%s","acs_emf_commandhandler::apply() - CcbApplyCallback invocated!");

}//End of apply
/*===================================================================
ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType acs_emf_commandhandler::updateRuntime(const char *objectName, const char **attrName){

	(void) attrName;
	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrValues = new void*[1];
	paramToChange.attrName = (char *)theEMFInfoMediaType;
	paramToChange.attrType = ATTR_INT32T;
	DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Entering");
	// LOAD mediaType
	if((theMediaType != -1) && (theEMFOperationalState != MEDIA_FEATURE_DISABLE)){
		paramToChange.attrValuesNum = 1;
		paramToChange.attrValues[0] = reinterpret_cast<void*>(&theMediaType);
	}
	else
		paramToChange.attrValuesNum = 0;

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - mediaType updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - mediaType updation is Successful.");

	// LOAD freeSpace
	paramToChange.attrName = (char *)theEMFInfoFreeSpace;
	paramToChange.attrType = ATTR_UINT32T;
	if((theMediaType != -1) && (theEMFOperationalState != MEDIA_FEATURE_DISABLE)){
		paramToChange.attrValuesNum = 1;
		paramToChange.attrValues[0] = reinterpret_cast<void*>(&theFreeSpaceOnMedia);
	}
	else
		paramToChange.attrValuesNum = 0;

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - freeSpace updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - freeSpace updation is Successful.");

	// LOAD used space
	paramToChange.attrName = (char *)theEMFInfoUsedSpace;
	paramToChange.attrType = ATTR_UINT32T;
	if((theMediaType != EMF_RC_ERROR) && (theEMFOperationalState != MEDIA_FEATURE_DISABLE)){
		paramToChange.attrValuesNum = 1;
		paramToChange.attrValues[0] = reinterpret_cast<void*>(&theUsedSpaceOnMedia);
	}
	else
		paramToChange.attrValuesNum = 0;

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - usedSpace updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - usedSpace updation is Successful.");

	// LOAD Total size
	paramToChange.attrName = (char *)theEMFInfoTotalSize;
	paramToChange.attrType = ATTR_UINT32T;
	if((theMediaType != -1) && (theEMFOperationalState != MEDIA_FEATURE_DISABLE)){
		paramToChange.attrValuesNum = 1;
		paramToChange.attrValues[0] = reinterpret_cast<void*>(&theTotalSizeOfMedia);
	}
	else
		paramToChange.attrValuesNum = 0;

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - totalSize updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - totalSize updation is Successful.");

	// LOAD LastUpdatedTime Attribute
	paramToChange.attrName = (char *)theEMFLastUpdatedTime;
	paramToChange.attrType = ATTR_STRINGT;
	if( (theMediaType != -1) && (theEMFOperationalState != MEDIA_FEATURE_DISABLE)){
		paramToChange.attrValuesNum = 1;
		paramToChange.attrValues[0] = reinterpret_cast<void*>((char *) theLastUpdatedTime.c_str());
	}
	else
		paramToChange.attrValuesNum = 0;


	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - lastUpdatedTime updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - lastUpdatedTime updation is Successful.");

	// LOAD operationalState
	paramToChange.attrName = (char *)theEMFOperationalState;
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&theOperationalState);

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - operationalState updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - operationalState updation is Successful.");

	// LOAD resultOfOperation
	paramToChange.attrName = (char *)theEMFResultOfOperation;
	paramToChange.attrType = ATTR_NAMET;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues[0] = reinterpret_cast<void*>((char *) ACS_EMF_IMM_ROOT_DN);

	if(this->modifyRuntimeObj(objectName, &paramToChange) == ACS_CC_FAILURE){
		ERROR (1,"%s","acs_emf_commandhandler::updateRuntime - resultOfOperation updation is failed.");
		DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime - Leaving");
		if(paramToChange.attrValues[0] != NULL)
			delete[] paramToChange.attrValues;
		return ACS_CC_FAILURE;
	}
	else
		INFO(0,"%s","acs_emf_commandhandler::updateRuntime() - resultOfOperation updation is Successful.");
	if(paramToChange.attrValues[0] != NULL){
		delete[] paramToChange.attrValues;
		paramToChange.attrValues = 0;
	}
	DEBUG (0,"%s","acs_emf_commandhandler::updateRuntime() - Leaving");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
ROUTINE: searchDVD
===============================================*/
bool acs_emf_commandhandler::searchDVD(int32_t* piSystemId, acs_dsd::NodeStateConstants* penmNodeState, acs_dsd::NodeSideConstants *penmNodeSide, ACE_INT16 *piState, bool *pbIsLocal){

	DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - Entering");
	bool bOk;
	bool bDvdLocked;
	int32_t iApgOwner;
	ACE_INT16 iRet;
	ACE_INT16 iTmp;
	ACS_EMF_CmdClient objCmdClient(m_stopHandle);
	std::vector<ACS_DSD_Node> vctSortReachable;
	std::vector<ACS_DSD_Node>::iterator it;
	ACS_EMF_DVDHandler *pDVDHandlePtr = NULL;
	// Initialization
	bOk = false;
	bDvdLocked = true;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	iRet = 0;
	iTmp = 0;
	vctSortReachable.clear();
	it = vctSortReachable.end();
	// Check pointers
	if((piSystemId != NULL) && (penmNodeState != NULL) && (penmNodeSide != NULL) && (piState != NULL) && (pbIsLocal != NULL)){
		// Init output args
		*pbIsLocal = false;
		*piState = 0;
		*piSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
		*penmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
		*penmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED;
		// Get local node info
		ACS_EMF_Common::getLocalNode(piSystemId, penmNodeState);
		// Get local dvd statusF
		std::string deviceFileName;
		iTmp = ACS_EMF_Common::checkForDVDStatus(deviceFileName);
		// Check if dvd is local
		if ((iTmp & CHECKDVD_MASK_PRESENT) != 0){
			// Set flag (true = dvd is local)
			*pbIsLocal = true;
			pDVDHandlePtr = reinterpret_cast<ACS_EMF_DVDHandler *>(theMediaHandlePtr);
			// Get DVD info
			pDVDHandlePtr->getDvdState(&iApgOwner, &bDvdLocked);
			// Check if local dvd is locked
			if(bDvdLocked == true){
				// DVD is locked. Check if DVD is not locked from this AP
				if(iApgOwner != *piSystemId){
					// Force mask to busy
					iTmp |= CHECKDVD_MASK_BUSY;
				}
			}
			// Set status
			*piState = iTmp;
			// Debug msg
			DEBUG(1, "acs_emf_commandhandler::searchDVD - DVD is local. Mask [%i]", iTmp);
			// Set exit flag
			bOk = true;
			// Set device name found.
			ACS_EMF_Common::setDeviceName(deviceFileName);
		}else{
			DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - Search the DVD on remote node.");
			// Set flag (false = dvd is not local)
			*pbIsLocal = false;
			// Search DVD on remote node. Query from dsd all reachable node
			bOk = objCmdClient.queryApFromDSD(&vctSortReachable);
			// Test for bOk
			if(bOk == false){
				// Error from dsd
				DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - DSD return a query error.");
			}
			// Test previews error
			if(bOk == true){
				// Set error flag
				bOk = false;
				// Set iterator to first element (this active node)
				it = vctSortReachable.begin();
				// Skip first element (the local node dvd chek has been checked; see first line of this method).
				if(it != vctSortReachable.end()){
					// Skip
					++it;
				}
				// find dvd for all node
				bool dvd_found = false;
				while((it != vctSortReachable.end()) /*&& ((iRet & CHECKDVD_MASK_PRESENT) == 0)*/ && (dvd_found == false)){
					// Debug message
					DEBUG(1,"acs_emf_commandhandler::searchDVD - Invoke for SYSID [%i], node_sate [%i]",(*it).system_id, (*it).node_state);
					// Invoke command 10: Check DVD
					iRet = objCmdClient.invokeEmfMethod(acs_emf_cmd_ns::EMF_CMD_CheckForDVDPresence, (*it).system_id, (*it).node_state, penmNodeSide);
					DEBUG(1,"acs_emf_commandhandler::searchDVD - Retcode[%i], iRet & CHECKDVD_MASK_PRESENT [%i]",iRet, iRet & CHECKDVD_MASK_PRESENT);
					// Check if DVD has been found
					if((iRet & CHECKDVD_MASK_UNKNOWN) != 0){
						DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - invokeEmfMethod() returns with 'DVD_PRESENCE_UNKNOWN' ");
						bool isconfigured=ACS_EMF_Common::isDVDConfigured((*it).system_id);
						if(isconfigured) {
							// DVD is configured in system_id: stop searching on other nodes
							dvd_found =true;
							DEBUG(1,"acs_emf_commandhandler::searchDVD - DVD is configured in AP SYSID [%i] but DVD status is unknown",(*it).system_id );
						}
					}
					else if((iRet & CHECKDVD_MASK_PRESENT) != 0){
						dvd_found = true;
					}
					else ++it;// DVD not found... Next node
				} // while
				// Check if dvd has been found
				if(/*(iRet & CHECKDVD_MASK_PRESENT) != 0 ||*/ dvd_found == true){
					// Set output params
					*piSystemId = (*it).system_id;
					*penmNodeState = (*it).node_state;
					// Set error flag
					bOk = true;
					// Set remote DVD state
					*piState = iRet;
					DEBUG(1,"acs_emf_commandhandler::searchDVD - DVD Drive is present on the AP[%i] - STATE[%i].",*piSystemId, *penmNodeState);
				}else{
					DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - No DVD found!");
				}
			}// if bOK == true
		}
	}
	DEBUG(1,"%s","acs_emf_commandhandler::searchDVD - Leaving");
	// Exit from method
	return(bOk);
}// End of searchDVD

/*===================================================================
ROUTINE: adminOperationCallback
=================================================================== */
void acs_emf_commandhandler::adminOperationCallback(ACS_APGCC_OiHandle immOiHandle,ACS_APGCC_InvocationType invocation, const char* objectName, ACS_APGCC_AdminOperationIdType opId,  ACS_APGCC_AdminOperationParamType** attrMods){
	DEBUG(1,"%s","Entering acs_emf_commandhandler::adminOperationCallback");
	DEBUG(1,"Entering acs_emf_commandhandler::adminOperationCallback :%s",objectName);
	//std::vector<std::string> myArgumentList;
	SaAisErrorT resultOfOperation = SA_AIS_OK;
	string errText = "@ComNbi@";
	int i = 0;
	DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Entering");
	DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - Operation ID received by adminOperationCallback is = %d  '%s'",opId, adminoperationString(opId));
	switch (opId)
	{
	case EMF_CMD_TYPE_COPYTOMEDIA:
	case EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA:
	{
		acs_emf_operation_status_t state = ACS_EMF_Common::getMediaOperationState();
		if (theOperationalState != MEDIA_FEATURE_ENABLE){
			resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media Feature in not enabled");
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Export operation is not Allowed");
			setExitCode(EMF_RC_MEDIAACCESSNOTENABLED);
		}else if(state != EMF_MEDIA_OPERATION_IS_IDLE)
		{
			if ((state == EMF_TOMEDIA_OPERATION_IS_IN_PROGRESS) && (opId == EMF_CMD_TYPE_COPYTOMEDIA)){
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - COPYTOMEDIA: DVD Operation is already in progress");
				setExitCode(EMF_RC_OPERATIONINPROGRESS);
			}else if((state == EMF_ERASETOMEDIA_OPERATION_IS_IN_PROGRESS) && (opId == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA))
			{
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERASEANDCOPYTOMEDIA: DVD Operation is already in progress");
				setExitCode(EMF_RC_OPERATIONINPROGRESS);
			}
			else{
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media drive is busy");
				setExitCode(EMF_RC_DVDDRIVEBUSY);
			}
		}
		else{
			bool formatMedia = (opId == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA)? true: false;
			//	myArgumentList.clear();
			std::string label;                              // for label
			std::string verify("true");
			std::string overwrite("false");
			std::string flist;                              // for filelist

			while(attrMods[i] != 0){
				std::string myAttrName(attrMods[i]->attrName);  			// name
				std::string myAttrValue((char*)attrMods[i]->attrValues);  	// value
				DEBUG(1,"parameter name = %s",myAttrName.c_str());
				DEBUG(1,"parameter value = %s",myAttrValue.c_str());

				if (ACE_OS::strcmp(myAttrName.c_str(),"label") == 0)
					label = myAttrValue;
				else if (ACE_OS::strcmp(myAttrName.c_str(),"verify") == 0)
					verify = myAttrValue;
				else if (ACE_OS::strcmp(myAttrName.c_str(),"overwriteEnable") == 0)
					overwrite = myAttrValue;
				else if (ACE_OS::strcmp(myAttrName.c_str(),"fileList") == 0)
					flist = myAttrValue;
				i++;
			} // END of while loop

			int call_result = handle_exportToMedia(label.c_str(),verify.c_str(),overwrite.c_str(),flist.c_str(),formatMedia);
			if (call_result != 0) {
				ERROR(1,"acs_emf_commandhandler::adminOperationCallback - handle_exportToMedia() returns with error < call_result == %d > <error_str == %s> ", call_result, get_rc_errorText(call_result));
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else {
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - handle_copyToDVD() returns with success <call_result == %d>", call_result);
			}
			setExitCode(call_result);
		}
	}
	break;
	case EMF_CMD_TYPE_COPYFROMMEDIA:
	{
		if (theOperationalState != MEDIA_FEATURE_ENABLE){
			resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media Feature in not enabled");
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Import from Media operation is not Allowed");
			setExitCode(EMF_RC_MEDIAACCESSNOTENABLED);
		}else if(ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE)
		{
			acs_emf_operation_status_t state = ACS_EMF_Common::getMediaOperationState();
			if (state == EMF_FROMMEDIA_OPERATION_IS_IN_PROGRESS){
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR: DVD Operation is already in progress");
				setExitCode(EMF_RC_OPERATIONINPROGRESS);
			}
			else{
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR: Media drive is busy");
				setExitCode(EMF_RC_DVDDRIVEBUSY);
			}
		}
		else{
			std::string overwrite("false"); 	// for overwrite flag
			std::string flist; 					// for source FILE List
			std::string destfoldername; 		// for destination FOLDER

			while(attrMods[i] !=0){
				std::string myAttrName(attrMods[i]->attrName);  	// name
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[%i]=[%s]", i, myAttrName.c_str());

				if (ACE_OS::strcmp(myAttrName.c_str(),"overwriteEnable") == 0){
					int myAttrValue;
					myAttrValue = *(reinterpret_cast <int*>(attrMods[i]->attrValues));  // value
					overwrite = myAttrValue ? "yes" : "no";
					DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[overwriteEnable]->myAttrValue=[%d] (overwrite flag = %s)", myAttrValue, (myAttrValue?"true" : "false"));
				}
				else if (ACE_OS::strcmp(myAttrName.c_str(),"fileList") == 0){
					std::string myAttrValue((char*)attrMods[i]->attrValues);  // value
					DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[fileList]->myAttrValue=[%s]", myAttrValue.c_str());
					flist = myAttrValue;
				}
				else if (ACE_OS::strcmp(myAttrName.c_str(),"destinationFolder") == 0){
					std::string myAttrValue((char*)attrMods[i]->attrValues);  // value
					DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[destinationFolder]->myAttrValue=[%s]", myAttrValue.c_str());
					destfoldername = myAttrValue;
				}
				i++;
			} // End of while loop

			int call_result = handle_importFromMedia(overwrite.c_str(),flist.c_str(),destfoldername.c_str());
			if (call_result != 0) {
				ERROR(1,"acs_emf_commandhandler::adminOperationCallback - ERROR: handle_importFromMedia() returns with error < call_result == %d > <error_str == %s> ", call_result, get_rc_errorText(call_result));
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else{
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - handle_importFromMedia() returns with success <call_result == %d>", call_result);
			}
			setExitCode(call_result);
		}
	}
	break;
	case EMF_CMD_TYPE_GETMEDIAINFO:
	{
		int iRet;
		// Initialization
		iRet = EMF_RC_ERROR;
		// Check state
		if (theOperationalState != MEDIA_FEATURE_ENABLE){
			// ERROR: Media is DISABLED
			resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media Feature in not enabled");
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Refresh operation is not Allowed");
			// Set exit code
			iRet = EMF_RC_MEDIAACCESSNOTENABLED;
		}// FOR MEDIA INFO
		else{
			// Set "actionName" attribute to refresh media (NO in the model)
			// set_imm_action_startingState(REFRESH_MEDIA);
			// Operational state is ENABLED. Call getMediaInfo.
			// Call getMediaInfo
			iRet = handle_getMediaInfo(/*srctData*/);
			// Check return status
			if (iRet != EMF_RC_OK){
				// Error
				ERROR(1,"acs_emf_commandhandler::adminOperationCallback - ERROR: handle_getMediaInfo() returns with error < call_result == %d > <error_str == %s> ", iRet, get_rc_errorText(iRet));
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}else{
				// Success
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - handle_getMediaInfo() returns with success <call_result == %d>", iRet);

			}
		}
		// Set exit code
		setExitCode(iRet);
	}
	break;
	case EMF_CMD_UNLOCK_MEDIA:
	{
		if (theOperationalState != MEDIA_FEATURE_DISABLE){ //runtime parameter
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - WARNING: handle_unlockMedia() returns with success: theOperationalState != MEDIA_FEATURE_DISABLE");
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - UnLock operation is not Allowed");
			resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			setExitCode(EMF_RC_MEDIAACCESSALREADYENABLED);
		}else if(ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE){
			acs_emf_operation_status_t operation_state = ACS_EMF_Common::getMediaOperationState();
			if (operation_state == EMF_MOUNT_OPERATION_IS_IN_PROGRESS){
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR:Operation is already in progress");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				setExitCode(EMF_RC_OPERATIONINPROGRESS);
			}
			if (operation_state != EMF_MEDIA_OPERATION_IS_IDLE){
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR:Media drive is busy");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				setExitCode(EMF_RC_DVDDRIVEBUSY);
			}
		}
		else{
			ACS_EMF_ParamList paramList;
			ACS_EMF_DATA srctData;
			ACE_INT32 nPlatform = ACS_EMF_Common::GetHWVersion();
			if(nPlatform >= 3){
				std::string executeFormat("no") ; 	// for formatEnable flag
				while(attrMods[i] != 0){
					std::string myAttrName(attrMods[i]->attrName);  	// name
					DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[%i]=[%s]", i, myAttrName.c_str());

					if (ACE_OS::strcmp(myAttrName.c_str(),"formatEnable") == 0){
						int myAttrValue;
						myAttrValue = *(reinterpret_cast <int*>(attrMods[i]->attrValues));  // value
						executeFormat = myAttrValue ? "yes" : "no" ;
						DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - myAttrName[formatEnable]->myAttrValue=[%d] (format flag = %s)", myAttrValue, (myAttrValue?"true" : "false"));
					}
					i++;

				}
				paramList.CmdCode = EMF_CMD_UNLOCK_MEDIA;
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback -executeFormat : %s", executeFormat.c_str() );
				paramList.Data[0] = executeFormat.c_str();
				srctData.Code = paramList.CmdCode;
				paramList.Encode();
				(void)paramList.getEncodedBuffer(srctData.Data);
				paramList.Clear();
				int call_result = handle_unlockMedia(srctData);
				if (call_result != EMF_RC_OK) {
					ERROR(1,"acs_emf_commandhandler::adminOperationCallback - ERROR: handle_unlockMedia() returns with error < call_result == %d > <error_str == %s> ", call_result, get_rc_errorText(call_result));
					resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				}
				else{
					
					DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - handle_unlockMedia() returns with success <call_result == %d>", call_result);
				}
				setExitCode(call_result);
			}
			else {
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR: The option is not valid for this platform");
				//setErrorText(0, 31, "The option is not valid for this platform");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				setExitCode(EMF_RC_OPTIONNOTVALIDONTHISPLATFORM);
			}
		}
	}
	break;

	case EMF_CMD_LOCK_MEDIA:
	{
		if (theOperationalState != MEDIA_FEATURE_ENABLE){
			resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media Feature in not enabled");
			DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Lock operation is not Allowed");
			setExitCode(EMF_RC_MEDIAACCESSNOTENABLED);
		}else if(ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE){
			acs_emf_operation_status_t operation_state = ACS_EMF_Common::getMediaOperationState();
			if (operation_state == EMF_UNMOUNT_OPERATION_IS_IN_PROGRESS){
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR: Unmount Operation is already in progress");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				setExitCode(EMF_RC_OPERATIONINPROGRESS);;
			}
			if (operation_state != EMF_MEDIA_OPERATION_IS_IDLE){
				ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - ERROR: Media drive is busy");
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
				setExitCode(EMF_RC_DVDDRIVEBUSY);
			}
		}
		else{
			ACS_EMF_ParamList paramList;
			ACS_EMF_DATA srctData;
			// For making async action
			paramList.CmdCode = EMF_CMD_LOCK_MEDIA;
			//paramList.Data[0] = MediaName.c_str();
			srctData.Code = paramList.CmdCode;
			paramList.Encode();
			(void)paramList.getEncodedBuffer(srctData.Data);
			paramList.Clear();
			// end
			int call_result = handle_lockMedia(srctData);

			if (call_result != EMF_RC_OK) {
				ERROR(1,"acs_emf_commandhandler::adminOperationCallback - ERROR: handle_lockMedia() returns with error < call_result == %d > <error_str == %s> ", call_result, get_rc_errorText(call_result));
				resultOfOperation = SA_AIS_ERR_FAILED_OPERATION;
			}
			else{
				DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - handle_lockMedia() returns with success <call_result == %d>", call_result);
			}
			setExitCode(call_result);
		}
	}
	break;

	case EMF_CMD_TYPE_CLEANUPANDLOCK:
	{
                // ONLY GEP5 require this action
                if(ACS_EMF_Common::GetHWVariant() >= 3){
                        if (theOperationalState == MEDIA_FEATURE_ENABLE)
                        {
                                if(theMediaOwner == MEDIA_ON_PARTNER_NODE){
                                        if( ACS_EMF_Common::unmountPassiveDVDOnActive() == EMF_RC_OK ){
                                                DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - unmounted the Media which is mounted to FileM=media");
                                                theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
                                        }
                                        else{
                                                sleep(10);
                                                if( ACS_EMF_Common::unmountPassiveDVDOnActive() == EMF_RC_OK){
                                                        DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - unmounted the media which is mounted to FileM=media");
                                                }
                                                else{
                                                        DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Failed to unmount the Media which is mounted to FileM=media");
                                                }
                                                theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
                                        }
                                        // Marks DVD Operations to IS IN IDLE
                                        if (ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE){
                                                ACS_EMF_Common::setMediaOperationStateToIDLE();
                                                theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"FAILED DUE TO EMF SERVICE ON OTHER NODE WENT DOWN");
                                                theMediaHandlePtr->modifyStructObjState();
                                        }
					DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - No media is connected to this node and media attached node went down");
                                }
                        }
                        else
                                DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Media Feature is not enabled");
                }
	}
	break;
	default:
		DEBUG(1,"%s","acs_emf_commandhandler::adminOperationCallback - Operation ID is received by is not valid");
		resultOfOperation = SA_AIS_ERR_INVALID_PARAM;
		break;
	};

	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;

	ACS_APGCC_AdminOperationParamType firstElem;
	//	ACS_APGCC_AdminOperationParamType secondElem;

	/*create first Elemenet of param list*/
	char attName1[]= "errorCode";

	firstElem.attrName = attName1;
	firstElem.attrType =  ATTR_STRINGT;
	string exitCodeString  = getExitCodeString();
	DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - exitCodeString %s",exitCodeString.c_str());
	errText.append(exitCodeString);
	DEBUG(1,"acs_emf_commandhandler::adminOperationCallback - errText %s",errText.c_str());
	char* errValue = const_cast<char*>(errText.c_str());
	firstElem.attrValues=reinterpret_cast<void*>(errValue);

	/*insert parameter into the vector*/
	if(getExitCode()!=0) {
		vectorOut.push_back(firstElem);
	}
	ACS_CC_ReturnType rc = adminOperationResult(immOiHandle, invocation, resultOfOperation,vectorOut);
	if(rc != ACS_CC_SUCCESS){
		ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Failure occurred in sending AdminOperation Result");
		ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Leaving");
		return;
	}
	ERROR(1,"%s","acs_emf_commandhandler::adminOperationCallback - Leaving");
}//End of adminOperationCallback
/*===================================================================
ROUTINE: clearDVDStatus
=================================================================== */
void acs_emf_commandhandler::clearDVDStatus(){
	DEBUG(1,"%s","acs_emf_commandhandler::clearDVDStatus - Entering");
	m_bIsDvdLocal = false;
	m_iSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
	m_iRemoteDvdState = 0;
	m_enmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
	m_enmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED;
	DEBUG(1,"%s","acs_emf_commandhandler::clearDVDStatus - Leaving");
}//End of clearDVDStatus
/*===================================================================
ROUTINE: initDVD
=================================================================== */
void acs_emf_commandhandler::initDVD()
{
	DEBUG(1,"%s","acs_emf_commandhandler::initDVD - Entering");
	bool bOk;
	bool bIsLocal;
	int32_t iSystemId;
	ACE_INT16 iRemoteDvdState;
	acs_dsd::NodeStateConstants enmNodeState;
	acs_dsd::NodeSideConstants enmNodeSide;
	acs_emf_operation_status_t  operation_state;
	// Initialize
	bOk = false;
	bIsLocal = false;
	iSystemId = 0;
	iRemoteDvdState = 0;
	enmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
	enmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED;
	// Clear DVD global status
	clearDVDStatus();
	// Get operation state
	//operation_state = EMF_GETDRIVE_OWNERSHIP_IS_IN_PROGESS;
	operation_state = ACS_EMF_Common::getMediaOperationState();
	DEBUG(1,"acs_emf_commandhandler::initDVD - DVDOperationState [%d]", operation_state);
	// Get dvd status
	//if(operation_state == EMF_MEDIA_OPERATION_IS_IDLE){
	// Search DVD
	bOk = searchDVD(&iSystemId, &enmNodeState, &enmNodeSide, &iRemoteDvdState, &bIsLocal);
	// Check error
	if(bOk == true){
		// Set params
		m_bIsDvdLocal = bIsLocal;
		m_iSystemId = iSystemId;
		m_enmNodeState = enmNodeState;
		m_enmNodeSide = enmNodeSide;
		m_iRemoteDvdState = iRemoteDvdState;
	}
	//}
	DEBUG(1,"%s","acs_emf_commandhandler::initDVD - Leaving");

}//end of initDVD
/*===================================================================
ROUTINE: handle_getMediaInfo
=================================================================== */
int acs_emf_commandhandler::handle_getMediaInfo()
{
	int iRet;
	ACE_UINT16 iFreeSpace;
	ACE_UINT32 iTotalSpace;
	ACE_UINT16 iUsedSpace;
	ACE_INT32 iMediaType;
	acs_emf_operation_status_t enmState;
	// Initialization
	iRet = EMF_RC_NOK;
	iFreeSpace = 0;
	iTotalSpace = 0;
	iUsedSpace = 0;
	iMediaType = 0;
	enmState = EMF_MEDIA_OPERATION_IS_IDLE;
	// Debug info
	DEBUG(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - Entering");
	// FOR MEDIA INFO
	enmState = ACS_EMF_Common::getMediaOperationState();
	// Check DVD state
	if (enmState == EMF_MEDIA_OPERATION_IS_IDLE){
		// Marks DVD Operations to IS IN PROGRESS
		ACS_EMF_Common::setMediaOperationStateToProgress(EMF_GETMEDIA_INFO_IS_IN_PROGRESS);
		// Get the DVD owner
		if (theMediaOwner == MEDIA_ON_CURRENT_NODE){
			// Owner is the local node
			if(ACS_EMF_Common::GetHWVariant() < 3 ){
				if(isMediaEmpty)
					iRet = EMF_RC_OK;
				else
					iRet = theMediaHandlePtr->GetMediaInformation(iMediaType,iFreeSpace,iUsedSpace,iTotalSpace);
			}else
				iRet = theMediaHandlePtr->GetMediaInformation(iMediaType,iFreeSpace,iUsedSpace,iTotalSpace);
		}
		else if(theMediaOwner == MEDIA_ON_PARTNER_NODE)
		{
			// Owner is the partner node
			if(ACS_EMF_Common::GetHWVariant() < 3 ){
				if(isMediaEmpty)
					iRet = EMF_RC_OK;
				else
					iRet = getMediaInfo(iMediaType,iFreeSpace,iUsedSpace,iTotalSpace);
			}else
				iRet = theMediaHandlePtr->GetMediaInformation(iMediaType,iFreeSpace,iUsedSpace,iTotalSpace);
		}
		// Check result of operation
		if (iRet != EMF_RC_OK){
			DEBUG(1,"acs_emf_commandhandler::handle_getMediaInfo - getMediaInfo return an error code [%i].", iRet);
			ERROR(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - No Media information available. Set default value.");
			// Error: When DVD is not present, the corresponding attributes in model should be set to "-"
			theFreeSpaceOnMedia = 0;
			theTotalSizeOfMedia = 0;
			theUsedSpaceOnMedia = 0;
			theMediaType = -1;
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			theInstance->clearDVDStatus();
			// Check the correct error
			if(iRet == EMF_RC_UNABLETOCONNECTSERV){
				// Unable connect to server
				ACS_EMF_Common::setMediaOperationStateToIDLE();
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unable to connect to server");
			}else if(iRet == EMF_RC_DVDDEVICENOTFOUND){
				// DVD not found.
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unable to access to Media");
			}else{
				// Unknow
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Connection lost");
			}

			if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1)
				ERROR(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - unmount nfs shared media folder from /data/opt/ap/internal_root/media is failed");
		}else{
			// Copy media information to class variables
			if (isMediaEmpty)
			{
				DEBUG(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - Fill Media information.");
				theFreeSpaceOnMedia = 100;
				theTotalSizeOfMedia = 4650000;
				theUsedSpaceOnMedia = 0;

				if(ACE_OS::strcmp(theMediumType.c_str(),"DVD+R") == 0)
					theMediaType = 0;
				else if (ACE_OS::strcmp(theMediumType.c_str(),"DVD-R") == 0)
					theMediaType = 1;
				else if (ACE_OS::strcmp(theMediumType.c_str(),"DVD+RW") == 0)
					theMediaType = 2;
				else if (ACE_OS::strcmp(theMediumType.c_str(),"DVD-RW") == 0)
					theMediaType = 3;
				else if (ACE_OS::strcmp(theMediumType.c_str(),"THUMBDRIVE") == 0)
					theMediaType = 4;
				else
				{
					theMediaType = -1;
					theFreeSpaceOnMedia = 0;
					theUsedSpaceOnMedia = 0;
					theTotalSizeOfMedia = 0;
				}
			}else{
				// Success: Copy media information to class variables
				theFreeSpaceOnMedia = iFreeSpace;
				theTotalSizeOfMedia = iTotalSpace;
				theUsedSpaceOnMedia = iUsedSpace;
				theMediaType = iMediaType;
				ERROR(1,"acs_emf_commandhandler::handle_getMediaInfo - Set theMediaType to [%d]",theMediaType);
				theLastUpdatedTime = ACS_EMF_Common::GetDateTimeWithoutOffset();
			}
		}
		// Marks DVD Operations to IS IN IDLE
		ACS_EMF_Common::setMediaOperationStateToIDLE();
	}else{
		// DVD is busy... Check cause
		if (enmState == EMF_GETMEDIA_INFO_IS_IN_PROGRESS){
			// Operation is already in progress
			ERROR(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - ERROR:Operation is already in progress");
			// Set return code
			setExitCode(14, "Operation is already in progress");
			iRet = EMF_RC_OPERATIONINPROGRESS;
		}else{
			// DVD driver is in generic busy.
			ERROR(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - ERROR: Media drive is busy");
			// Set return code
			setExitCode( 23, "Media drive is busy");
			iRet = EMF_RC_DVDDRIVEBUSY;
		}
	}
	// Debug info
	DEBUG(1,"%s","acs_emf_commandhandler::handle_getMediaInfo - Leaving");
	// Exit from method
	return(iRet);
}//end of handle_getMediaInfo



/*===================================================================
ROUTINE: mediaOperationOnOtherNode
=================================================================== */
ACE_THR_FUNC_RETURN acs_emf_commandhandler::mediaOperationOnOtherNode(void* lpvThis)
{
	DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - Entering");
	//copy to DVD requires
	if((ACS_EMF_Common::GetHWVariant() <3 && !theInstance->isMediaEmpty))
	{
		if(const int oper_result = theInstance->performUnmountOperation(MEDIA_ON_PARTNER_NODE)){
			if (oper_result == EMF_RC_UNABLETOCONNECTSERV ||
					oper_result == EMF_RC_CANNOTRECEIVEROMSERVER||
					oper_result == EMF_RC_CANNOTSENDCMDTOSERVER){
				ERROR(1,"acs_emf_commandhandler::mediaOperationOnOtherNode() - unmounting media on partner node is failed <oper_result == %d>", oper_result);

				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unable to connect to server");
			}else {
				ERROR(1,"%s: %s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - unmounting media on partner node and from FileM folder is failed",
						"The reason could be another session is accessing FileGroup=media");
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Copy failed while unmount media. The reason could be another session is accessing FileGroup=media");
			}
			DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Leaving");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			theInstance->clearDVDStatus();
			return 0;
		}
	}

	if (ACS_EMF_Common::setupNFSOnActive() != 0){
		ERROR(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - NFS setup failed on active node; cannot perform copy operation");
		DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - Leaving");
		return false;
	}

	//int resultCode = EMF_RC_OK;
	std::list<std::string> myList;
	ACS_EMF_ParamList Params;
	Params.Decode(((ACS_EMF_DATA*)lpvThis)->Data);
	// prepare filelist parameter to send to the DSDServer
	const ACE_TCHAR* lpszFile;
	int nNum = Params.NumOfData();
	for (int nIdx = 0; nIdx < nNum; nIdx++){
		lpszFile = (const ACE_TCHAR*)Params.Data[nIdx];
		myList.push_back(lpszFile);
	}
	// Start - For debug purpose
	std::list<std::string>::iterator itr = myList.begin();
	DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - Sending the following values from active to DVD-owner node");
	while(itr != myList.end()) {
		std::string arg = (*itr);
		DEBUG(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - %s",arg.c_str());
		++itr;
	}// END - For debug purpose

	// prepare filelist  for updating EMFHistory
	std::string filestrlist;
	ACE_INT32 nooffiles = nNum - 3;
	size_t found = 0;
	for (int nIdx = 0; nIdx < nooffiles ; nIdx++){
		lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
		std::string str(lpszFile);
		found=str.find_last_of("/\\");
		filestrlist.append(str.substr(found+1));
		if (nooffiles > 1 ) filestrlist.append(" ");
	}
	if(theMediaStatusFlag == false){
		theMediaHandlePtr->initEMFHistRuntimeHandler();
		//TODO:  VERIFY
		ACE_INT32 cntValue= theMediaHandlePtr->get_cntValue();
		if (cntValue == 1)
		{
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			theMediaHandlePtr->createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			theMediaHandlePtr->createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			theMediaHandlePtr->modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}
		else
		{
			theMediaHandlePtr->modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			/*cntValue > 1 */
			if(!theMediaHandlePtr->moveObjectsOneLevelDown(cntValue)){
				DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - moveObjectsOneLevelDown failed !!!");
				// modify result and endtime
				theInstance->update_imm_HistoryAttrs(FAILED);
				theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Failed due to service is stopped during operation");
				//resultCode = EMF_RC_SERVERNOTRESPONDING;
			}
		}
		theMediaHandlePtr->incr_cntValue();
		theMediaHandlePtr->copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	}
	int media_status = CH_MEDIA_STATE_ENABLED;
	int cmdCode = Params.CmdCode;
	DEBUG(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - cmdCode == %d filestrlist=[%s]", cmdCode, filestrlist.c_str());
	ACS_EMF_Cmd cmdSend(cmdCode);
	cmdSend.addArguments(myList);
	ACS_EMF_Cmd cmdRecv;
	ACE_INT32 result_code = theInstance->sendreceive_to_mediaOwner(cmdSend, cmdRecv);
	switch (result_code ){
	case EMF_RC_UNABLETOCONNECTSERV:
	case EMF_RC_CANNOTSENDCMDTOSERVER:
	case EMF_RC_CANNOTRECEIVEROMSERVER:
		DEBUG(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - Error Received from EMF server! < result_code == %d >", result_code);
		theInstance->update_imm_HistoryAttrs(FAILED);
		theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Unable to connect to server");
		media_status = CH_MEDIA_STATE_UNAVAILABLE;
		break;
	case EMF_RC_ERRORRECEIVINGFROMSERVER:
	{
		std::list<std::string> argList = cmdRecv.commandArguments();
		DEBUG(1,"%s", "acs_emf_commandhandler::mediaOperationOnOtherNode - Error Received from EMF server!");
		//argList = cmdRecv.commandArguments();
		if (argList.size() >= 2){
			std::list<std::string>::iterator itr = argList.begin();
			DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - following arguments received from  DVD-owner node");

			std::string historyResult_text = (*itr);
			DEBUG(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - historyResult_text == %s", historyResult_text.c_str());
			++itr;
			std::string lastOperation_error_text = (*itr);
			DEBUG(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - lastOperation_error_text == %s",lastOperation_error_text.c_str());
			theInstance->update_imm_HistoryAttrs(historyResult_text.c_str());
			theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,lastOperation_error_text.c_str());
		}else{
			theInstance->update_imm_HistoryAttrs(FAILED);
			theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Exporting on media is failed.");
		}
	}
	break;
	case EMF_RC_DVDDEVICENOTFOUND:
		DEBUG(1,"%s", "acs_emf_commandhandler::mediaOperationOnOtherNode - Media might be no longer present or configured ");
		theInstance->update_imm_HistoryAttrs(FAILED);
		theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Unable to access to Media");
		media_status = CH_MEDIA_STATE_DISABLED;
		break;
	case EMF_RC_MEDIANOTMOUNTED:
	case EMF_RC_MOUNTINGFAILED:
		DEBUG(1,"%s", "acs_emf_commandhandler::mediaOperationOnOtherNode - Media might be no longer enabled");
		theInstance->update_imm_HistoryAttrs(FAILED);
		theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Media not enabled");
		media_status = CH_MEDIA_STATE_DISABLED;
		break;
	case EMF_RC_OK:
		// modify result and endtime
		if ( ACS_EMF_Common::GetHWVariant() < 3){
			theInstance->update_imm_HistoryAttrs(SUCCESSFUL);
			theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"-");
		}
		else if (( ACS_EMF_Common::GetHWVariant() >= 3)&&(ACS_EMF_Common::checkForUSBStatus() != 0)) {//On active node
			if(ACS_EMF_Common::getNode() == NODE_A)
				theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node A is not Available");
			else
				theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node B is not Available");
		}
		DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - Command execution on other node is completed");
		theInstance->isMediaEmpty = false;
		break;
	default:
		ERROR(1,"acs_emf_commandhandler::mediaOperationOnOtherNode - Command execution on other node Failed  <resultCode == %d>", result_code);
		theInstance->update_imm_HistoryAttrs(FAILED);
		theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Exporting on media is failed.");
		break;
	}
	//Remove all NFS share between both node
	if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
	{
		ERROR(1, "%s", "acs_emf_commandhandler::mediaOperationOnOtherNode - Removing NFS mount on Active node is failed");
	}
	if(ACS_EMF_Common::GetHWVariant() < 3 && media_status == CH_MEDIA_STATE_ENABLED){
		if (theInstance->performMountOperation(MEDIA_ON_PARTNER_NODE) != EMF_RC_OK){
			DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - call 'performMountOperation ()' failed ");
		}
	}
	if(media_status != CH_MEDIA_STATE_ENABLED){
		if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1){
			ERROR(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - unmount nfs shared media folder from /data/opt/ap/internal_root/media is failed");
		}
		theInstance->unmountMediaOnPassiveNode(LOGICALMOUNT_MODE);
		theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
		theInstance->clearDVDStatus();
	}
	// Marks DVD Operations to IS IN IDLE
	theMediaHandlePtr->modifyStructObjState();
	ACS_EMF_Common::setMediaOperationStateToIDLE();
	DEBUG(1,"%s","acs_emf_commandhandler::mediaOperationOnOtherNode - Leaving");
	return 0;
}//end of mediaOperationOnOtherNode

/*===================================================================
ROUTINE: EmfDVDOperationProc
=================================================================== */
ACE_THR_FUNC_RETURN acs_emf_commandhandler::EmfMediaOperationProc(void *lpvThis)
{
	// TODO: please remove magic number and use define\enum
	DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering");
	ACS_EMF_ParamList Params;
	int resultOfActive = EMF_RC_ERROR;
	int resultOfPassive = EMF_RC_ERROR;
	Params.Decode(((ACS_EMF_DATA*)lpvThis)->Data);
	const ACE_TCHAR* lpszArgs = Params.Data[0];
	bool formatEnabled = false;
	formatEnabled = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);
	switch (Params.CmdCode){
	case EMF_CMD_UNLOCK_MEDIA:
		if (theInstance->theMediaOwner == MEDIA_ON_CURRENT_NODE){// Current node
			if( ACS_EMF_Common::GetHWVariant() >= 3){
				DEBUG(1,"acs_emf_commandhandler::EmfMediaOperationProc -formatEnabled : %d", formatEnabled );
				resultOfActive = theInstance->mountnanoUSB(formatEnabled);
				if(resultOfActive == EMF_RC_OK || resultOfActive == EMF_RC_MOUNTINGFAILED){
					resultOfPassive = theInstance->mountMediaOnPassiveNode(NULL,lpszArgs);
					if((resultOfPassive != EMF_RC_OK) && (resultOfPassive != EMF_RC_NOMEDIAINDRIVE)){// Mount fails when thumb drive is available on passive node
						if(resultOfPassive != EMF_RC_MOUNTINGFAILED)
						resultOfPassive = 2 ;
					}
				}
				else
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - UnLock operation on the passive node is failed.");
			}else{
				int mountStatus = theInstance->mountDVDOnActiveNode();
				if( mountStatus == EMF_RC_OK){
					// Filter
					resultOfActive = EMF_RC_OK;
				}else if (mountStatus == EMF_MEDIA_MOUNTEDBAD){
					// Mount bad (force to print "UNLOCKING FAILED DUE TO MEDIA..."
					// TODO: remove magic number
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - mountStatus == EMF_MEDIA_MOUNTEDBAD")
					resultOfActive = 2;
				}
			}
		}else if (theInstance->theMediaOwner == MEDIA_ON_PARTNER_NODE){// partner node
//			const ACE_TCHAR* lpszArgs = Params.Data[0];
//			formatEnabled = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);
			DEBUG(1,"acs_emf_commandhandler::EmfMediaOperationProc -when media is in partner node formatEnabled : %d", formatEnabled );
			int mountStatus = theInstance->mountMediaOnPassiveNode(NULL,lpszArgs);
			// Check for error
			if(mountStatus == EMF_RC_MEDIAMOUNTEDONOTHERDIR){
				resultOfPassive = 2;
			}else if(mountStatus == EMF_RC_OK){
				resultOfPassive = EMF_RC_OK;
			}else{
				resultOfPassive = mountStatus;
			}
		}
		if ( (resultOfActive == EMF_RC_OK) && (resultOfPassive == EMF_RC_OK)){ // SUCCESS in BOTH THE NODES
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive == 0) && (resultOfPassive == 0)");
			ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_ENABLE);
			//theInstance->isMediaEmpty = false;
			/*This flag is used to remember the presence of Thumbdrive on both the Node*/
			theMediaStatusFlag = true;
			if(theMediaHandlePtr->syncMediaOnActive() == EMF_RC_OK){
				ACS_EMF_Common::setMediaFlag(Sync_Pos,Sync);
				ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode());
			}
				ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
			theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive == 0) && (resultOfPassive == 0)");
		}
		else if((resultOfActive == EMF_RC_OK) && (resultOfPassive != EMF_RC_OK)){//SUCCESS IN ACTIVE FAILED IN PASSIVE
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive == 0) && (resultOfPassive != 0)");
			if(ACS_EMF_Common::GetHWVariant() >= 3){
				if(resultOfPassive == 2){
					theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
					theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS, "-");
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving check resultOfActive and !resultOfPassive");
				}
//				else if(resultOfPassive == EMF_RC_MOUNTINGFAILED){
//					theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
//					theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"Unlocking Failed due to mount failure");
//					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
//				}
				else{
					theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
					theMediaStatusFlag = false;
					ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_DISABLE);
					if(ACS_EMF_Common::getNode() == NODE_A)
						theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS, "Unlock Successful but thumb drive on AP Node B is not Available");
					else
						theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS, "Unlock Successful but thumb drive on AP Node A is not Available");

					ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
					ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode());
					ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
				}
			}
			else{
				theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS, "-");
			}
			DEBUG(1,"%s","Exiting (resultOfActive == 0) && (resultOfPassive != 0)");
		}
		else if((resultOfActive != EMF_RC_OK) && (resultOfPassive == EMF_RC_OK)){ //FAILED IN ACTIVE SUCCESS IN PASSIVE
			DEBUG(1,"%s","Entering (resultOfActive != 0) && (resultOfPassive == 0)");

			if(ACS_EMF_Common::GetHWVariant() >= 3){
				//if(resultOfActive== EMF_RC_MOUNTINGFAILED){
				//	theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
					//theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"Unlocking Failed due to mount failure");
				//	DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
				//}else{
				theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
				theMediaStatusFlag = false;
				ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_ENABLE);
				int ret = 0;
				if(ACS_EMF_Common::getNode() == NODE_A) ret = 2;
				else if(ACS_EMF_Common::getNode() == NODE_B) ret =1;
				if(ret == 2)
					theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"Unlock Successful but thumb drive on AP Node A is not Available");
				else
					theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"Unlock Successful but thumb drive on AP Node B is not Available");

				ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ret);
				ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
				ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
			//}
			}else{
				theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
			}
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive != 0) && (resultOfPassive == 0)");
		}
		else if((resultOfActive == 2) || (resultOfPassive == 2)){
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive == 2) && (resultOfPassive == 2)");
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"UNLOCKING FAILED DUE TO MEDIA IS ALREADY MOUNTED TO OTHER THAN FileGroup=media");
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive != 0) && (resultOfPassive == 0)");
		}else if (((resultOfActive == EMF_RC_MOUNTINGFAILED) && (resultOfPassive == EMF_RC_MOUNTINGFAILED))){
			if(ACS_EMF_Common::GetHWVariant() >= 3){
				theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"Unlocking Failed due to mount failure");
				DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
			}
		}else if(((resultOfActive != EMF_RC_OK) && (resultOfPassive == EMF_RC_MOUNTINGFAILED))){
			if(ACS_EMF_Common::GetHWVariant() >= 3){
				theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"Unlocking Failed due to mount failure");
				DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
			}
		}else if(((resultOfPassive != EMF_RC_OK) && (resultOfActive == EMF_RC_MOUNTINGFAILED))){
			if(ACS_EMF_Common::GetHWVariant() >= 3){
				theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"Unlocking Failed due to mount failure");
				DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
			}
		}
		else{ // FAILURE
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in failure on check resultOfActive and resultOfPassive");
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"UNLOCKING FAILED");
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
			//theInstance->isMediaEmpty = true;
		}
		if(theInstance->theMediaState != theInstance->theOperationalState){
			theInstance->modifyMediaState(theInstance->theOperationalState);
			theInstance->theMediaState = theInstance->theOperationalState;
		}
		break;
	case EMF_CMD_LOCK_MEDIA:

		if (theInstance->theMediaOwner == MEDIA_ON_CURRENT_NODE) {// Current node
			if( ACS_EMF_Common::GetHWVariant() >= 3){
				resultOfActive = theInstance->umountnanoUSB();
				if ((resultOfActive == 0) && (theMediaStatusFlag == true)){
					resultOfPassive = theInstance->unmountMediaOnPassiveNode(DSDSERVER_LOCK_MEDIA);
					if((resultOfPassive != EMF_RC_OK) && (resultOfPassive != EMF_RC_NOMEDIAINDRIVE))// Umount fails when thumb drive is available on passive node
						resultOfPassive = 2;
				}
				else
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Lock operation on the passive node is failed.");
			}
			else{
				// if(!theInstance->isMediaEmpty)
				resultOfActive = theInstance->unmountDVDOnActiveNode(DSDSERVER_LOCK_MEDIA);
				// else
				// resultOfActive = EMF_RC_OK;
			}

		}
		else if (theInstance->theMediaOwner == MEDIA_ON_PARTNER_NODE) {// partner node
			if( ACS_EMF_Common::GetHWVariant() >= 3){
				resultOfPassive = theInstance->unmountMediaOnPassiveNode(DSDSERVER_LOCK_MEDIA);
			}else
			{
				// if(!theInstance->isMediaEmpty)
				resultOfPassive = theInstance->unmountMediaOnPassiveNode(DSDSERVER_LOCK_MEDIA, DSDSERVER_REMOVE_MEDIA_OWN_FILE);
				// else
				//	resultOfPassive = EMF_RC_OK;
			}

		}
		theInstance->theMediaType = EMF_RC_ERROR;
		if ( (resultOfActive == EMF_RC_OK) && (resultOfPassive == EMF_RC_OK)){ // SUCCESS in BOTH THE NODES
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive == 0) && (resultOfPassive == 0)");
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE; 
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
			if( ACS_EMF_Common::GetHWVariant() < 3)
				// Clear dvd attributes
				theInstance->clearDVDStatus();
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive == 0) && (resultOfPassive == 0)");
		}
		else if((resultOfActive == EMF_RC_OK) && (resultOfPassive != EMF_RC_OK)){//SUCCESS IN ACTIVE FAILED IN PASSIVE
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive == 0) && (resultOfPassive != 0)");
			if(ACS_EMF_Common::GetHWVariant() >= 3){
				if(resultOfPassive == 2){
					theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
					theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - resultOfPassive(2)");
				}else{
					theInstance->theOperationalState = MEDIA_FEATURE_DISABLE; 
					ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_DISABLE);
					theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
				}
			}
			else{
				theInstance->clearDVDStatus();
				theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
			}
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive == 0) && (resultOfPassive != 0)");
		}
		else if((resultOfActive != EMF_RC_OK) && (resultOfPassive == EMF_RC_OK)){//FAILED IN ACTIVE SUCCESS IN PASSIVE
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in if(resultOfActive != 0) && (resultOfPassive == 0)");
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_DISABLE);
			if(ACS_EMF_Common::GetHWVariant() < 3) theInstance->clearDVDStatus();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS,"-");
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving if(resultOfActive != 0) && (resultOfPassive == 0)");
		}
		else{ //  FAILURE
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Entering in failure on check resultOfActive and resultOfPassive");
			theInstance->theOperationalState = MEDIA_FEATURE_ENABLE; 
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"LOCKING FAILED DUE TO ANOTHER SESSION IS ACCESSING FileGroup=media");
			DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving in failure on check resultOfActive and resultOfPassive");
			//theInstance->isMediaEmpty = true;
		}
		if(theInstance->theMediaState != theInstance->theOperationalState){
			theInstance->modifyMediaState(theInstance->theOperationalState);
			theInstance->theMediaState = theInstance->theOperationalState;
		}
		break;
	case EMF_CMD_TYPE_COPYTOMEDIA:
		if((ACS_EMF_Common::GetHWVariant() >= 3) && (theMediaStatusFlag == true)){
			if(theMediaHandlePtr->syncMediaOnActive() != EMF_RC_OK)
				DEBUG(1,"%s","Sync failed before COPYTOMEDIA operation");
		}
	case EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA:
	case EMF_CMD_TYPE_COPYFROMMEDIA:
		theMediaHandlePtr->initEMFHistRuntimeHandler();
		int resultforOperation = theMediaHandlePtr->mediaOperation((ACS_EMF_DATA*)lpvThis);
		DEBUG(1,"acs_emf_commandhandler::EmfMediaOperationProc - mediaOperation() return [%d]",resultforOperation);
		if (resultforOperation == EMF_RC_OK){
			if( ACS_EMF_Common::GetHWVariant() >= 3){
				if((Params.CmdCode == EMF_CMD_TYPE_COPYTOMEDIA) || (Params.CmdCode == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA)){
					ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
					ACE_THR_FUNC_RETURN resultforExport;
					if(ACS_EMF_DSDServer::getOperationalStateinPassive() == MEDIA_FEATURE_ENABLE){
						ACS_EMF_CmdClient cmdClient(m_stopHandle);
						if(cmdClient.queryApFromDSD() == true){
							resultforExport = theInstance->mediaOperationOnOtherNode((ACS_EMF_DATA*)lpvThis);
						}else{
							DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - mediaOperationPassive Node is not reachable");
							//ACS_EMF_Common::umountUSBData();
							theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
							//theMediaHandlePtr->modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
							theInstance->update_imm_HistoryAttrs(SUCCESSFUL);
							if(ACS_EMF_Common::getNode() == NODE_A)
								theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node B is not Available");
							else
								theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node A is not Available");
							theMediaHandlePtr->modifyStructObjState();
							ACS_EMF_Common::setMediaOperationStateToIDLE();
							DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving");
							return EMF_RC_OK;
						}
					}
					if(resultforExport == EMF_RC_OK){
						ACS_EMF_Common::setMediaFlag(Sync_Pos,Sync);
						theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
						theInstance->mountnanoUSB(false);
						theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"-");
						//theMediaHandlePtr->modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
						theInstance->update_imm_HistoryAttrs(SUCCESSFUL);
					}else{
						if ((theMediaStatusFlag == true) && (Params.CmdCode == EMF_CMD_TYPE_COPYTOMEDIA)){
							if(isStopSignaled == false){
								ACE_INT16 iReturn = theInstance->removeNewCopiedFile((ACS_EMF_DATA*)lpvThis);
								if (iReturn == EMF_RC_OK)
									ACS_EMF_Common::setMediaFlag(Sync_Pos,Sync);
							}
							theMediaHandlePtr->modifyStructObjFinalAttr(FAILURE,"Media is not writable");
						}else{
							//theMediaHandlePtr->modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
							if((ACS_EMF_DSDServer::getOperationalStateinPassive()) != MEDIA_FEATURE_ENABLE) {
								ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode()); 
							}
							theInstance->theOperationalState = MEDIA_FEATURE_ENABLE;
							theInstance->mountnanoUSB(false);
							theInstance->update_imm_HistoryAttrs(SUCCESSFUL);
							if(ACS_EMF_Common::getNode() == NODE_A)
								theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node B is not Available");
							else
								theMediaHandlePtr->modifyStructObjFinalAttr(SUCCESS,"Writing Successful but thumb drive on AP Node A is not Available");
						}
					}
					DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving");
				}
			}
			// COPY TO media was successful, hence media is no longer empty !
			theInstance->isMediaEmpty = false;
		}
		else
			ERROR(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - mediaOperation in Current node is Failed");

		if( ACS_EMF_Common::GetHWVariant() < 3){
			if (Params.CmdCode != EMF_CMD_TYPE_COPYFROMMEDIA){
				//	theInstance->performMountOperation(MEDIA_ON_CURRENT_NODE);
				int mount_result = theInstance->performMountOperation(MEDIA_ON_CURRENT_NODE);
				if ( mount_result != EMF_RC_OK ){
					for ( int i = 1 ; i < 4 ; i++ ){
						if(isStopSignaled == false){
							sleep (10);
							mount_result = theInstance->performMountOperation(MEDIA_ON_CURRENT_NODE);
							if ( mount_result == EMF_RC_OK )
								break;
						}
					}
				}
				if ( mount_result != EMF_RC_OK ){
					DEBUG(1,"%s","Mounting is failed after 4 retries");
				}
			}
		}
		break;
	}
	// Marks DVD Operations to IS IN IDLE
	theMediaHandlePtr->modifyStructObjState();
	ACS_EMF_Common::setMediaOperationStateToIDLE();
	DEBUG(1,"%s","acs_emf_commandhandler::EmfMediaOperationProc - Leaving");
	return EMF_RC_OK;
}//End of EmfMediaOperationProc
/*===================================================================
ROUTINE: importFromOtherNode
====================================================================*/
ACE_THR_FUNC_RETURN acs_emf_commandhandler::importFromOtherNode(void *lpvThis)
{
	DEBUG(1,"%s","acs_emf_commandhandler::importFromOtherNode - Entering");
	int result = theMediaHandlePtr->ImportFiles((ACS_EMF_DATA*)lpvThis);
	if (result != EMF_RC_OK)
		ERROR(1,"%s","acs_emf_commandhandler::importFromOtherNode - ImportFiles operation is Failed. RetCode[%i]", result);
	// Marks DVD Operations to IS IN IDLE
	theMediaHandlePtr->modifyStructObjState();
	ACS_EMF_Common::setMediaOperationStateToIDLE();
	DEBUG(1,"%s","acs_emf_commandhandler::importFromOtherNode - Leaving");
	return EMF_RC_OK;
}//End of  importFromOtherNode
/*===================================================================
ROUTINE: performErrChkForTOMediaOper
====================================================================*/
ACE_INT32  acs_emf_commandhandler::performErrChkForTOMediaOper(std::string label,
		/*std::string verify,*/
		/*std::string overwrite,*/
		int noOfFiles,
		std::vector<std::string> fileList,
		bool formatOption)
{
	DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - Entering");
	ACE_INT32 resultCode = EMF_RC_OK;
	// Verify first if the required operation is valid!
	if(ACS_EMF_Common::GetHWVariant() < 3){
		// TR HR44823 - BEGIN
		if (formatOption){
			// Check should performed only for FormatAndEraseOperation only.
			// Check for Overwrite for CD-R,CD+R,DVD+R and DVD-R
			//Overwrite option is not valid for CD-R,CD+R,DVD+R and DVD-R
			if(ACE_OS::strcmp(theMediumType.c_str(),"CD-R") == 0 \
					|| ACE_OS::strcmp(theMediumType.c_str(),"CD+R") == 0 \
					|| ACE_OS::strcmp(theMediumType.c_str(),"DVD-R") == 0 \
					|| ACE_OS::strcmp(theMediumType.c_str(),"DVD+R") == 0 \
					|| ACE_OS::strcmp(theMediumType.c_str(),"UNKNOWN") == 0)
			{
				DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - ERROR: Overwriting data to this media is not supported");
				//setErrorText(0, 34, "Overwriting data to this media is not supported");
				DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - Leaving");
				return EMF_RC_MEDIANOTOVERWRITABLE;
			}
		}
		// TR HR44823 - END
	}

	if(!label.empty() && (resultCode = ACS_EMF_Common::ValidateLabel(label.c_str()))!= EMF_RC_OK){
		DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - Failure occurred in Label validation !!");
		ACE_INT32 code = EMF_RC_OK;
		const ACE_TCHAR *errMsg = ACS_EMF_Execute::GetResultCodeText(resultCode, &code);
		DEBUG(1,"acs_emf_commandhandler::performErrChkForTOMediaOper - ERROR:%s EXIT CODE = %d",errMsg, code);
		DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - Leaving");
		return resultCode;
	}
	if (noOfFiles == 0){ // Check for file arguments
		DEBUG(1,"%s", "acs_emf_commandhandler::performErrChkForTOMediaOper - ERROR: Missing File Name argument for ExportTOMedia");
		DEBUG(1,"%s","Exiting acs_emf_commandhandler::performErrChkForTOMediaOper");
		return EMF_RC_INVALIDARGUMENT;
	}

	if ((resultCode = validateFileNamesForTOMedia(fileList)) != EMF_RC_OK){
		DEBUG(1,"validation of file arguments failed  < resultCode == %d > ", resultCode);
		DEBUG(1,"%s","Exiting acs_emf_commandhandler::performErrChkForTOMediaOper");
		return resultCode;
	}
	DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForTOMediaOper - Leaving");
	return resultCode;
}//End of performErrChkForTOMediaOper
/*===================================================================
ROUTINE: handle_unlockMedia
====================================================================*/
int acs_emf_commandhandler::handle_unlockMedia(ACS_EMF_DATA & paramList)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia - Entering");
	theOperationalState = MEDIA_FEATURE_DISABLE;
	ACS_EMF_Common::setMediaOperationStateToProgress(EMF_MOUNT_OPERATION_IS_IN_PROGRESS);
	set_imm_action_startingState(UNLOCK_MEDIA);
	ACS_EMF_ParamList Params;
	Params.Decode(((ACS_EMF_DATA*)&paramList)->Data);
	if( ACS_EMF_Common::GetHWVariant() < 3){
		//Check the format option for unlockMedia
		const ACE_TCHAR* lpszArgs = Params.Data[0];
		bool EnableformatInDVD = false;
		EnableformatInDVD = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);
		DEBUG(1," acs_emf_commandhandler::handle_unlockMedia value of EnableformatInDVD :%d",EnableformatInDVD);
		if(EnableformatInDVD == true ){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia - ERROR:Unlocking Failed due to invalid format option on this platform");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking Failed due to invalid format option on this platform");
			return  EMF_RC_FORMATOPTIONNOTVALIDONTHISPLATFORM;
		}

		// Initialize dvd
		initDVD();
		// Check dvd presence
		if (m_iSystemId == acs_dsd::SYSTEM_ID_UNKNOWN){
			// DVD Drive is not present on the board.
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia - ERROR:No external media board available");
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking Failed due to no external media board available");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			return  EMF_RC_DVDDEVICENOTFOUND;
		}
		// Check DVD mount state
		if((m_iRemoteDvdState & CHECKDVD_MASK_UNKNOWN) != 0){
			// DVD is busy
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia - DVD status unknown due o Unable to connect to the server ");

			// Set last action
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlock Failed due to unable to connect the server");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			// Set com failure
			return  EMF_RC_UNABLETOCONNECTSERV;
		}
		// Check DVD mount state
		if((m_iRemoteDvdState & CHECKDVD_MASK_BUSY) != 0){
			// DVD is busy
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia - ERROR:DVD is already mounted on another user");

			// Set last action
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "UNLOCKING FAILED DUE TO MEDIA ALREADY MOUNTED ON THE OTHER AP");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			// Set com failure
			return  EMF_RC_DVDDRIVEBUSY;
		}

		DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia - the DVD board is in the %s AP magazine", m_bIsDvdLocal ? "local": "remote");


		return (m_bIsDvdLocal == true)? handle_unlockMedia_on_local_node(paramList)
				: handle_unlockMedia_on_remote_node(paramList);
	}
	else{
		//ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
		if (ACS_EMF_Common::checkForUSBStatus() == 0){
			return handle_unlockMedia_on_local_node(paramList);
		}else{
			//set no sync flag
			ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
			return handle_unlockMedia_on_remote_node(paramList);
		}
	}
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia - Leaving");
	return EMF_RC_OK;
}//end of handle_unlockMedia
/*===================================================================
ROUTINE: handle_unlockMedia_on_local_node
====================================================================*/
int acs_emf_commandhandler::handle_unlockMedia_on_local_node(ACS_EMF_DATA & paramList){
	int returnCode = EMF_RC_OK;
	isMediaEmpty = false;
	//int call_result;
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Entering");
	// Marks DVD Operations to IS IN PROGRESS
	ACS_EMF_Common::setMediaOperationStateToProgress(EMF_MOUNT_OPERATION_IS_IN_PROGRESS);
	if( ACS_EMF_Common::GetHWVariant() < 3){
		if(isStopSignaled){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking failed due to Unable to connect the server");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Leaving");
			return EMF_RC_UNABLETOCONNECTSERV;
		}
		// DVD Drive is present on the current node.
		ACE_INT32 mediumPresence = ACS_EMF_Common::checkForMediumPresence();
		if(mediumPresence == acs_emf_common::DVDMEDIA_NOT_PRESENT){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - ERROR:No Media in DVD Drive");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "No Media in DVD Drive");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Leaving");
			return EMF_RC_NOMEDIAINDRIVE;
		}
		//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
		// TR HR44823
		ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
		if (ACS_EMF_Common::getMediumType(mediumType) == -1){
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - ERROR:Unknown Medium");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - EXIT CODE = 56");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking failed due to Unknown Medium type");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Leaving");
			return EMF_RC_DISCMEDIAERROR;
		}

		theMediumType = mediumType;
		DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia_on_local_node - Medium Type Detected is %s",mediumType);
		// TR HR44823 - END
		if(isStopSignaled == true){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media operation is interrupted because of service is stopped");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Leaving");
			return EMF_RC_UNABLETOCONNECTSERV;
		}
		int call_result = ACS_EMF_Common::isDVDEmpty();
		if (call_result == acs_emf_common::DVDMEDIA_STATE_ERROR){
			ERROR(1,"acs_emf_commandhandler::handle_unlockMedia_on_local_node - isDVDEmpty() returns with error <call_result == %d>",call_result);
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking failed");
			return EMF_RC_OTHERERROR;
			// Nothing to DO ???
		}
		theMediaOwner = MEDIA_ON_CURRENT_NODE;
		if (call_result == acs_emf_common::DVDMEDIA_STATE_EMPTY){ // Empty
			DEBUG(1, "%s", "acs_emf_commandhandler::handle_unlockMedia_on_local_node - DVD media is empty - No mount operation is done but OperationalState is set to ENABLE");
			ACS_EMF_DVDHandler *pDVDHandlePtr = reinterpret_cast <ACS_EMF_DVDHandler *> (theMediaHandlePtr);
			// Lock DVD
			if(isStopSignaled == true){
				ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
				ACS_EMF_Common::setMediaOperationStateToIDLE();
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media operation is interrupted because of service is stopped");
				return EMF_RC_UNABLETOCONNECTSERV;
			}
			pDVDHandlePtr->setDvdState(m_iSystemId,true);
			isMediaEmpty = true;
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->theOperationalState = MEDIA_FEATURE_ENABLE; 
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_SUCCESS, "-");

			thr_grp_id  = ACS_EMF_Common::StartThread(EmfUnlockMediaIfDVDIsEmpty,(void *) &paramList, ACE_TEXT("ModifyMediaState"));
	                if (thr_grp_id == -1){
                	        ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Unable to create thread for modifying mediaState attribute.");
			}
		}
		else if (call_result == acs_emf_common::DVDMEDIA_STATE_NOT_EMPTY)
		{
			// For making async action
			if(isStopSignaled == true){
				ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
				ACS_EMF_Common::setMediaOperationStateToIDLE();
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media operation is interrupted because of service is stopped");
				return EMF_RC_UNABLETOCONNECTSERV;
			}
			thr_grp_id  =  ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &paramList, ACE_TEXT("MediaOperation"));
			if (thr_grp_id == -1){
				ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Unable to create thread for doing unlock operation on Current Node");
				ACS_EMF_Common::setMediaOperationStateToIDLE();
				theMediaOwner = NO_MEDIA;
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking failed");
				returnCode = EMF_RC_UNABLETOCONNECTSERV;
			} // End
			else
				DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia_on_local_node - Separate Thread spawned for Unlock Operation successfully and Status = %d",thr_grp_id);
			// end
		}
	}
	else{
		theMediaOwner = MEDIA_ON_CURRENT_NODE;
		DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia_on_local_node - The value for 'theMediaOwner' attribute  <theMediaOwner == %d>", theMediaOwner);
		//set a falg here for usb check in EmfMediaOperationProc
		// For making async action
		thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &paramList, ACE_TEXT("MediaOperation"));
		if (thr_grp_id == -1)
		{
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Unable to create thread for doing unlock operation on Current Node");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			returnCode = EMF_RC_UNABLETOCONNECTSERV;
		}
		else{
			DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia_on_local_node - Separate Thread spawned for Unlock Operation successfully and Status = %d",thr_grp_id);
		}
	}
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_local_node - Leaving");
	return returnCode;
}// end of handle_unlockMedia_on_local_node
/*===================================================================
ROUTINE: handle_unlockMedia_on_remote_node
====================================================================*/
int acs_emf_commandhandler::handle_unlockMedia_on_remote_node(ACS_EMF_DATA & paramList)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Entering");
	int returnCode = EMF_RC_OK;
	// Marks DVD Operations to IS IN PROGRESS
	ACS_EMF_Common::setMediaOperationStateToProgress(EMF_MOUNT_OPERATION_IS_IN_PROGRESS);
	// DVD drive is present on other node
	// Check for Medium
	int call_result;
	if(isStopSignaled == true){
		ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media operation is interrupted because of service is stopped");
		return EMF_RC_UNABLETOCONNECTSERV;
	}
	call_result = checkForMedia();
	if (call_result == EMF_RC_OK){
		theMediaOwner = MEDIA_ON_PARTNER_NODE;
		if(isStopSignaled == true){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media operation is interrupted because of service is stopped");
			return EMF_RC_UNABLETOCONNECTSERV;
		}
		// For making async action
		thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &paramList, ACE_TEXT("MediaOperation"));
		if (thr_grp_id == EMF_RC_ERROR){
			ERROR(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Unable to create thread for doing unlock operation on partner Node");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Exit Code :117, Error Text: Unable to connect to server");
			//	setExitCode( 117, "Unable to connect to server");
			returnCode = EMF_RC_UNABLETOCONNECTSERV;
		} // End
		else
			DEBUG(1,"acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Separate Thread spawned for Unlock Operation successfully on partner node Status = %d",thr_grp_id);
	}
	else{
		ERROR(1,"acs_emf_commandhandler::handle_unlockMedia_on_remote_node - ERROR: checkForMedia() return an error code [%i].", call_result);
		returnCode = EMF_RC_NOMEDIAINDRIVE;
	}
	// Marks DVD Operations to IS IN IDLE
	if (returnCode != EMF_RC_OK){
		theMediaOwner = NO_MEDIA;
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
		theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE,"No Media in DVD Drive");
		DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Exit Code :117, Error Text: Unable to connect to server");
		setExitCode( 22, "Media Drive is not available");
	}
	DEBUG(1,"%s","acs_emf_commandhandler::handle_unlockMedia_on_remote_node - Leaving");
	return returnCode;
}// end of handle_unlockMedia_on_remote_node

/*===================================================================
ROUTINE: handle_lockMedia
====================================================================*/
int acs_emf_commandhandler::handle_lockMedia(ACS_EMF_DATA & paramList)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_lockMedia - Entering");
	int returnCode = EMF_RC_OK;
	if( ACS_EMF_Common::GetHWVariant() < 3){
		if (m_iSystemId == acs_dsd::SYSTEM_ID_UNKNOWN){
			// DVD Drive is not present on the board.
			ERROR(1,"%s","acs_emf_commandhandler::handle_lockMedia - ERROR:No external media board available");
			return  EMF_RC_DVDDEVICENOTFOUND;
		}
		DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - the DVD board is in the %s AP magazine", m_bIsDvdLocal ? "local": "remote");
		if ( (m_bIsDvdLocal && (theMediaOwner == MEDIA_ON_CURRENT_NODE)) || (!m_bIsDvdLocal && (theMediaOwner == MEDIA_ON_PARTNER_NODE))){
			// Marks DVD Operations to IS IN PROGRESS
			ACS_EMF_Common::setMediaOperationStateToProgress(EMF_UNMOUNT_OPERATION_IS_IN_PROGRESS);
			set_imm_action_startingState(LOCK_MEDIA);
			// For making async action
			thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &paramList, ACE_TEXT("DVDOperation"));
			if (thr_grp_id == -1){
				ACS_EMF_Common::setMediaOperationStateToIDLE();
				ERROR(1,"%s","acs_emf_commandhandler::handle_lockMedia - Unable to create thread for doing lock operation on Current Node");
				theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unlocking Failed due to Unable to connect to server");
				returnCode = EMF_RC_UNABLETOCONNECTSERV;
			} // End
			else
				DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - Separate Thread spawned for lock Operation successfully and Status = %d",thr_grp_id);
		}
		else {

			DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - WARNING: Inconsistent value for 'theDVDOwner' attribute  <theDVDOwner == %d>", theMediaOwner);
			returnCode = EMF_RC_OTHERERROR;
			// It has NOT occur !!
		}
	}
	else{
		DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - The value for 'theMediaOwner' attribute  <theMediaOwner == %d>", theMediaOwner);
		DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - No Mediaowner %s","After");
		ACS_EMF_Common::setMediaOperationStateToProgress(EMF_UNMOUNT_OPERATION_IS_IN_PROGRESS);
		set_imm_action_startingState(LOCK_MEDIA);
		thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &paramList, ACE_TEXT("DVDOperation"));
		if (thr_grp_id == EMF_RC_ERROR){
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			ERROR(1,"%s","acs_emf_commandhandler::handle_lockMedia - Unable to create thread for doing lock operation on Current Node");
			ERROR(1,"%s","acs_emf_commandhandler::handle_lockMedia - Exit Code :117, Error Text: Unable to connect to server");
			//setExitCode( 117, "Unable to connect to server");
			returnCode = EMF_RC_UNABLETOCONNECTSERV;
		} // End
		else
			DEBUG(1,"acs_emf_commandhandler::handle_lockMedia - Separate Thread spawned for lock Operation successfully and Status = %d",thr_grp_id);
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::handle_lockMedia - Leaving");
	return returnCode;
}//end of handle_lockMedia
/*===================================================================
ROUTINE: handle_exportToMedia
====================================================================*/
int acs_emf_commandhandler::handle_exportToMedia ( const char *label, const char * verify, const char *overwrite, const char *fileslist, bool formatMedia)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::handle_exportToMedia - Entering");
	// Marks Media Operations to IS IN PROGRESS
	if(formatMedia)
		ACS_EMF_Common::setMediaOperationStateToProgress(EMF_ERASETOMEDIA_OPERATION_IS_IN_PROGRESS);
	else
		ACS_EMF_Common::setMediaOperationStateToProgress(EMF_TOMEDIA_OPERATION_IS_IN_PROGRESS);

	ACS_EMF_ParamList  paramList;
	ACS_EMF_DATA srctData;
	std::string flist;
	std::vector<std::string> myArgumentList;

	paramList.CmdCode = (formatMedia)? EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA: EMF_CMD_TYPE_COPYTOMEDIA;

	flist = fileslist;
	paramList.Data[0] = label;
	paramList.Data[1] = verify;
	paramList.Data[2] = overwrite;
	int j = 3;

	size_t found;
	found=flist.find(";");
	DEBUG(1,"acs_emf_commandhandler::handle_exportToMedia - FILE LIST RECEIVED = %s",flist.c_str());
	while (found != string::npos){
		found=flist.find_first_of(";");
		std::string fullPath(flist.substr(0,found));
		ACS_EMF_Common::ExpandPath(fullPath);
		myArgumentList.push_back(fullPath);
		paramList.Data[j] = fullPath.c_str();
		flist = flist.substr(found+1);
		found=flist.find(";");
		j++;
	}
	if (!flist.empty()){
		std::string fullPath(flist.c_str());
		ACS_EMF_Common::ExpandPath(fullPath);
		paramList.Data[j] = fullPath.c_str();
		myArgumentList.push_back(fullPath);
		j++;
	}

	DEBUG(1,"acs_emf_commandhandler::handle_exportToMedia - Total no of values received = %d",j);
	srctData.Code = paramList.CmdCode;
	int noOfFiles = j - 3;
	paramList.Encode();
	(void)paramList.getEncodedBuffer(srctData.Data);
	int check_result = performErrChkForTOMediaOper(label,noOfFiles,myArgumentList,formatMedia);
	if (check_result != EMF_RC_OK){
		ERROR(1,"%s","acs_emf_commandhandler::handle_exportToMedia - 'performErrChkForTODVDOper' failed ! EMF service cannot execute copytoDVD operation");
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		return check_result;
	}
	DEBUG(1,"acs_emf_commandhandler::handle_exportToMedia - The Media board is in the %s AP magazine",(theMediaOwner == MEDIA_ON_CURRENT_NODE) ? "local": "remote");
	set_imm_action_startingState((paramList.CmdCode == EMF_CMD_TYPE_COPYTOMEDIA)? EXPORT_TO_MEDIA: FORMAT_AND_EXPORT_TO_MEDIA);
	int call_result = (theMediaOwner == MEDIA_ON_CURRENT_NODE) ? handle_exportToMedia_on_local_node(srctData)
			: handle_exportToMedia_on_remote_node(srctData);
	if (call_result != EMF_RC_OK)
		ACS_EMF_Common::setMediaOperationStateToIDLE();
	myArgumentList.clear();
	DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia - Leaving");
	return call_result;

}//end of handle_exportToMedia
/*===================================================================
ROUTINE: handle_exportToMedia_on_local_node
====================================================================*/
int acs_emf_commandhandler::handle_exportToMedia_on_local_node(ACS_EMF_DATA & emf_data)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_local_node - Entering");
	// DVD Drive is present on the current node.
	// Question: Why here it has to execute unmount operation ???
	// As per GEP5 implementation it is not required.But it is an extra check
	//provided before start of export operation in case of DVD.If not required in GEP2 machine.It can be removed.
	if( ACS_EMF_Common::GetHWVariant() < 3){
		if (!isMediaEmpty && performUnmountOperation(MEDIA_ON_CURRENT_NODE) != EMF_RC_OK){
			ERROR(1,"acs_emf_commandhandler::handle_exportToMedia_on_local_node - %s %s","unmounting media from FileM folder is failed",
					"The reason could be another session is accessing FileGroup=media");
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Failed while unmount media. The reason could be another session is accessing FileGroup=media");
			return  EMF_RC_UNMOUNTINGFAILED;
		}
	}
	//theMediaStatusFlag = true;
	thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &emf_data, ACE_TEXT("MediaOperation"));
	if (thr_grp_id == EMF_RC_ERROR){
		if( ACS_EMF_Common::GetHWVariant() < 3)
			performMountOperation(MEDIA_ON_CURRENT_NODE);
		ERROR(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_local_node - Unable to spawn a separate thread for doing Export Operation (TODVD) on current Node");
		theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Unable to connect the server");
		return EMF_RC_UNABLETOCONNECTSERV;

	}
	DEBUG(1,"acs_emf_commandhandler::handle_exportToMedia_on_local_node - Separate Thread is spawned for CopytoDVD Operation successfully and Status = %d",thr_grp_id);
	DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_local_node - Leaving");
	return  EMF_RC_OK;
}//end of handle_exportToMedia_on_local_node
/*===================================================================
ROUTINE: handle_exportToMedia_on_remote_node
====================================================================*/
int acs_emf_commandhandler::handle_exportToMedia_on_remote_node(ACS_EMF_DATA & emf_data)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Entering");
	ACS_EMF_ParamList Params;
	Params.Decode(((ACS_EMF_DATA*)&emf_data)->Data);
	//int cmdCode = Params.CmdCode;
#if 0
	if((ACS_EMF_Common::GetHWVariant() < 3 && !isMediaEmpty))
		//	|| (ACS_EMF_Common::GetHWVariant() == 3) && cmdCode == acs_emf_cmd_ns::EMF_CMD_EraseAndCopyToMedia))
	{
		// Question: Why here it has to execute unmount operation ???
		if(performUnmountOperation(MEDIA_ON_PARTNER_NODE) != EMF_RC_OK){
			ERROR(1,"%s: %s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - unmounting media on partner node and from FileM folder is failed",
					"The reason could be another session is accessing FileGroup=media");
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Failed while unmount media. The reason could be another session is accessing FileGroup=media");
			//setExitCode( 117, "Unable to connect to server");
			DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Leaving");
			return  EMF_RC_UNMOUNTINGFAILED;
		}
	}
#endif
	theMediaStatusFlag = false;
	thr_grp_id  = ACS_EMF_Common::StartThread(mediaOperationOnOtherNode,(void *) &emf_data, ACE_TEXT("mediaOperationOnOtherNode"));
	if (thr_grp_id == EMF_RC_ERROR){
		performMountOperation(MEDIA_ON_PARTNER_NODE);
		ERROR(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Unable to spawn a separate thread for executing mediaOperationOnOtherNode");
		ERROR(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Exit Code :117, Error Text: Unable to connect to server");
		setExitCode( 117, "Unable to connect to server");
		DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Leaving");
		return EMF_RC_UNABLETOCONNECTSERV;
	}// End
	DEBUG(1,"acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Separate Thread is spawned for executing mediaOperationOnOtherNode <Status == %d >",thr_grp_id);
	DEBUG(1,"%s","acs_emf_commandhandler::handle_exportToMedia_on_remote_node - Leaving");
	return EMF_RC_OK;
}//end of handle_exportToMedia_on_remote_node
/*===================================================================
ROUTINE: handle_importFromMedia
====================================================================*/
int acs_emf_commandhandler::handle_importFromMedia(const char *overwrite, const char *fileslist, const char * destinationfolder)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Entering");
	// Marks Media Operations to IS IN PROGRESS
	ACS_EMF_Common::setMediaOperationStateToProgress(EMF_FROMMEDIA_OPERATION_IS_IN_PROGRESS);
	ACS_EMF_ParamList  paramList;
	paramList.CmdCode = EMF_CMD_TYPE_COPYFROMMEDIA;
	paramList.Data[0] = overwrite;
	std::string flist(fileslist);
	int j = 1;
	// FIRST COPIES ALL FILE LIST param object
	size_t found;
	found=flist.find(";");
	DEBUG(1,"FILE LIST RECIVED = %s",flist.c_str());
	if (flist.empty()){
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		DEBUG(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Source Name is empty");
		ERROR(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Leaving");
		return EMF_RC_INVALIDPATHNAME;
	}
	if (destinationfolder != NULL){
		std::string destiName(destinationfolder);
		if(destiName.empty()){
			ACS_EMF_Common::setMediaOperationStateToIDLE();
			DEBUG(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Destination Name is empty");
			ERROR(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Leaving");
			return EMF_RC_INVALIDPATHNAME;
		}
	}
	else{
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		DEBUG(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Destination Name is empty");
		ERROR(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Leaving");
		return EMF_RC_INVALIDPATHNAME;
	}
	std::vector<std::string> myArgumentList;
	bool a_flag = true;
	while (found != string::npos){
		found=flist.find_first_of(";");
		paramList.Data[j] = flist.substr(0,found).c_str();
		myArgumentList.push_back(flist.substr(0,found));
		flist = flist.substr(found+1);
		found=flist.find(";");
		j++;
	}
	if (!flist.empty()){
		paramList.Data[j] = flist.c_str();
		myArgumentList.push_back(flist);
		j++;
	}
	if (destinationfolder != NULL){
		// COPIES FOLDER NAME AT END.
		std::string fullPath(destinationfolder);
		ACS_EMF_Common::ExpandPath(fullPath);
		paramList.Data[j] = fullPath.c_str();
		myArgumentList.push_back(fullPath);
		j++;

		if(j > 2)// INDICATES COPY IS SPECIFIC FILE COPY FROM DVD.
			a_flag = false;
	}
	DEBUG(1,"acs_emf_commandhandler::handle_importFromMedia - Total no of values received = %d",j-1);
	DEBUG(1,"acs_emf_commandhandler::handle_importFromMedia - Vector Size = %d",myArgumentList.size());
	ACS_EMF_DATA srctData;
	srctData.Code = paramList.CmdCode;
	paramList.Encode();
	(void)paramList.getEncodedBuffer(srctData.Data);
	paramList.Clear();

	int check_result = performErrChkForFromMediaOper(myArgumentList,a_flag);
	if (check_result != EMF_RC_OK){
		ERROR(1,"acs_emf_commandhandler::handle_importFromMedia - call 'performErrChkForFromMediaOper'failed! <check_result == %d>. EMF service cannot execute importFromMedia", check_result);
		if (check_result == EMF_RC_CANNOTACCESSTOMEDIA) {
			theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "NFS access to media failed.");
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE; 
			theInstance->clearDVDStatus();
		}
		ACS_EMF_Common::setMediaOperationStateToIDLE();
		ERROR(1,"%s", "acs_emf_commandhandler::handle_importFromMedia - Leaving");
		return check_result;
	}
	DEBUG(1,"acs_emf_commandhandler::handle_importFromMedia - The DVD board is in the %s AP magazine",(theMediaOwner == MEDIA_ON_CURRENT_NODE) ? "local": "remote");
	set_imm_action_startingState(IMPORT_FROM_MEDIA);
	int call_result = (theMediaOwner == MEDIA_ON_CURRENT_NODE) ? handle_importFromMedia_on_local_node(srctData)
			: handle_importFromMedia_on_remote_node(srctData);
	if (call_result != EMF_RC_OK)
		ACS_EMF_Common::setMediaOperationStateToIDLE();
	DEBUG(1,"%s","acs_emf_commandhandler::handle_importFromMedia - Leaving");
	return call_result;
}//end of handle_importFromMedia
/*===================================================================
ROUTINE: handle_importFromMedia_on_local_node
====================================================================*/
int acs_emf_commandhandler::handle_importFromMedia_on_local_node(ACS_EMF_DATA & emf_data)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_local_node - Entering");
	thr_grp_id  = ACS_EMF_Common::StartThread(EmfMediaOperationProc,(void *) &emf_data, ACE_TEXT("MediaOperation"));
	if (thr_grp_id == EMF_RC_ERROR){
		ERROR(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_local_node - Unable to create thread for doing Import Operation(FROMDVD) on Current Node");
		ERROR(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_local_node - Leaving");
		return EMF_RC_UNABLETOCONNECTSERV;
	}
	DEBUG(1,"acs_emf_commandhandler::handle_importFromMedia_on_local_node - Separate Thread spawned for Import Operation successfully and Status = %d", thr_grp_id);
	ERROR(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_local_node - Leaving");
	return  EMF_RC_OK;
}//end of handle_importFromMedia_on_local_node
/*===================================================================
ROUTINE: handle_importFromMedia_on_remote_node
====================================================================*/
int acs_emf_commandhandler::handle_importFromMedia_on_remote_node(ACS_EMF_DATA & emf_data)
{
	DEBUG(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_remote_node - Entering");
	thr_grp_id  = ACS_EMF_Common::StartThread(importFromOtherNode, (void *) &emf_data, ACE_TEXT("mediaOperationOnOther"));
	// sleep(1);  why there is a sleep here?
	// This is the root cause of TR HS27905
	if (thr_grp_id == EMF_RC_ERROR){
		ERROR(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_remote_node - Unable to create thread for doing Import From Media Operation on DVD-owner Node");
		DEBUG(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_remote_node - Leaving");
		return EMF_RC_UNABLETOCONNECTSERV;
	}// End
	DEBUG(1,"acs_emf_commandhandler::handle_importFromMedia_on_remote_node - Separate Thread spawned for Import From Media Operation on DVD-owner node successfully and Status = %d",thr_grp_id);
	DEBUG(1,"%s","acs_emf_commandhandler::handle_importFromMedia_on_remote_node - Leaving");
	return EMF_RC_OK;
}//end of handle_importFromMedia_on_remote_node

/*===================================================================
ROUTINE: validateFileNamesForTODVD
=====================================================================*/
ACE_INT32 acs_emf_commandhandler::validateFileNamesForTOMedia(std::vector<std::string> fileList)
{
	DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Entering");
	ACE_INT32 dwResult = EMF_RC_OK;
	for(int i = fileList.size() - 1 ; i >= 0 ; i--){
		string fileListLen = fileList[i];
		if( fileListLen.length() < 27){
			DEBUG(1,"%s","EXIT CODE = 9, Error Text :Invalid path name");
			//setExitCode(9, "Invalid path name");
			DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
			return EMF_RC_INVALIDPATHNAME;
		}
		// check  for duplicated filename ...
		for(int j=i-1; j>= 0; j-- ){
			DEBUG(1,"acs_emf_commandhandler::validateFileNamesForTOMedia:: comparing $s with %s",fileList[i].c_str(),fileList[j].c_str());
			if(fileList[i].compare(fileList[j])== 0){
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia:: ERROR Duplicated filename as argument!");
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
				return EMF_RC_INVALIDARGUMENT;
			}
		}
		DEBUG(1,"acs_emf_commandhandler::validateFileNamesForTOMedia - File name recieved for validation is %s\n",fileList[i].c_str());
		ACE_TCHAR szFilePath[512];
		ACE_OS::strcpy(szFilePath,fileList[i].c_str());
		DEBUG(1,"acs_emf_commandhandler::validateFileNamesForTOMedia - Source Name = %s",fileList[i].c_str());
		DEBUG(1,"acs_emf_commandhandler::validateFileNamesForTOMedia - fileList.size() inside for loop %d",i);
		string main_sub = fileList[i].substr(27);

		for (unsigned int j = 0; j < main_sub.length(); j++){
			if ( main_sub[j] != '/')
				break;
		}
		std::string str_sub = main_sub.substr(i);
		size_t pos1 = str_sub.find("media");
		int errorId = 0;
		ACE_TCHAR errorText[150];
		if ( pos1 != string::npos){
			if ( pos1 == 0 ){
				DEBUG(1,"%s", "acs_emf_commandhandler::validateFileNamesForTOMedia - ERROR:Invalid source name. EXIT CODE=[EMF_RC_INVALIDPATHNAME]");
				//setExitCode(9, "Invalid path name");
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
				return EMF_RC_INVALIDPATHNAME;
			}
		}

		size_t pos2 = str_sub.find_last_of("/\\");
		if ( pos2 == (str_sub.size()-1) )
		{
			// File/Folder path ended with '/'
			DEBUG(1,"%s", "acs_emf_commandhandler::validateFileNamesForTOMedia - ERROR:Invalid source name. EXIT CODE=[EMF_RC_INVALIDPATHNAME]");
			DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
			return EMF_RC_INVALIDPATHNAME;
		}

		dwResult = ACS_EMF_Common::ValidateFileArgumentToMedia(szFilePath);
		if (dwResult != EMF_RC_OK){
			ACE_OS::strcpy(errorText,ACS_EMF_Execute::GetResultCodeText(dwResult, &errorId));
			DEBUG(1,"acs_emf_commandhandler::validateFileNamesForTOMedia - ERROR:%s EXIT CODE = %d",errorText, errorId);
			DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
			return dwResult;
		}
	}
	DEBUG(1,"%s","acs_emf_commandhandler::validateFileNamesForTOMedia - Leaving");
	return dwResult;
}//End of validateFileNamesForTOMedia
/*===================================================================
ROUTINE: validateFileNameForFROMMedia
=====================================================================*/
ACE_INT32 acs_emf_commandhandler::validateFileNameForFROMMedia(std::vector<std::string>& fileList, bool copyAllDataFlag)
{
	DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Entering");
	if (copyAllDataFlag){// for entire DVD/CD copy to destination Folder
		for(int i = fileList.size() - 1 ; i >= 0 ; i--){
			string fileListLen = fileList[i];
			if( fileListLen.length() < 27)
			{
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR: Invalid path name '%s' ", fileListLen.c_str());
				DEBUG(1,"%s","Exiting acs_emf_commandhandler::validateFileNameForTODVD");
				return EMF_RC_INVALIDPATHNAME;
			}
			ACE_TCHAR szFilePath[ACS_EMF_ARRAYSIZE];
			DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - Folder Name Received for Validation is %s",fileList[i].c_str());
			ACE_OS::strcpy(szFilePath,fileList[i].c_str());
			ACE_INT32 dwResult = ACS_EMF_Common::ValidateFileArgumentFromMedia(szFilePath);
			if (dwResult != EMF_RC_OK){
				int errorId = 0;
				ACE_TCHAR errorText[150];
				ACE_OS::strcpy(errorText,ACS_EMF_Execute::GetResultCodeText(dwResult, &errorId));
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR: %s '%s'",errorText, szFilePath);
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
				return dwResult;
			}
			if(ACS_EMF_Common::isEmptyDir(szFilePath) == EMF_RC_ERROR){
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:Directory is not empty");
				return EMF_RC_INVALIDPATHNAME;
			}
		}
	}
	else {  // For specific file copy to given destinations
		if (isMediaEmpty){ // DVD is Empty
			DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:DVD empty, copy is not possible");
			DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
			return EMF_RC_FILEEMPTY;
		}
		for(unsigned int i = 0; i < fileList.size(); i++){
			ACE_TCHAR szFilePath[ACS_EMF_ARRAYSIZE];
			DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - File Name Received for Validation is %s",fileList[i].c_str());
			ACE_OS::strcpy(szFilePath,fileList[i].c_str());
			if (i < fileList.size() - 1){
				// FOR FILE LIST
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - FILE NAME = %s",fileList[i].c_str());
				if (fileList[i].find("/media") != EMF_RC_OK || fileList[i].compare("/media") == EMF_RC_OK){
					int errorId = 0;
					ACE_TCHAR errorText[150];
					ACE_OS::strcpy(errorText,ACS_EMF_Execute::GetResultCodeText(EMF_RC_INVALIDFILENAME,&errorId));
					DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:%s",errorText);
					//DEBUG(1,"EXIT CODE = %d",errorId);
					//setExitCode(errorId, errorText);
					DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
					return EMF_RC_INVALIDFILENAME;
				}
				else{
					std::string str_sub (fileList[i]);
					size_t pos = str_sub.find_last_of("/\\");
					if ( pos == (str_sub.size()-1) ){
						// File/Folder path ended with '/'
						DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:Invalid source name");
						DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
						return EMF_RC_INVALIDFILENAME;
					}
					std::string fullPath(fileList[i].c_str());
					ACS_EMF_Common::ExpandPath(fullPath);
					fileList[i] = fullPath;
				}
			}
			ACE_OS::strcpy(szFilePath,fileList[i].c_str());
			ACE_INT32 dwResult;
			if (i == fileList.size() - 1){
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - Destination Name received for verification = %s",fileList[i].c_str());
				string fileListLen = fileList[i];
				if(fileListLen.length() == 27){
					DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
					return EMF_RC_INVALIDPATHNAME;
				}
				size_t myfound,myprefound;
				myfound= fileListLen.find_last_of("/\\");
				if(myfound == (fileListLen.size() - 1))
				{
					fileListLen = fileListLen.substr(0,myfound);
				}
				myprefound = fileListLen.find_last_of("/\\");
				DEBUG(1,"Pos found = %d, prefound = %d \n",myfound,myprefound);
				if((myprefound == (myfound -1))){
					DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
					return EMF_RC_INVALIDPATHNAME;
				}

				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - Destination Name = %s",fileList[i].c_str());
				string main_sub = fileList[i].substr(27);
				unsigned int i = 0;
				for (i=0; i < main_sub.length();i++){
					if ( main_sub[i] != '/')
						break;
				}
				std::string str_sub = main_sub.substr(i);
				size_t pos1 = str_sub.find("media");
				if ( pos1 != string::npos){
					if ( pos1 == 0 ){
						DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:Invalid destination name");
						DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
						return EMF_RC_INVALIDDESTINATIONNAME;
					}
				}
				dwResult = ACS_EMF_Common::ValidateFileArgumentFromMedia(szFilePath);
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - call 'ValidateFileArgumentFromMedia (%s)' returns %d ",szFilePath, dwResult);
			}
			else{
				dwResult = ACS_EMF_Common::ValidateForSpecificFileCopy(szFilePath);
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - call 'ValidateForSpecificFileCopy (%s)' returns %d ",szFilePath, dwResult);
			}

			if (dwResult != EMF_RC_OK){
				int errorId = EMF_RC_OK;
				ACE_TCHAR errorText[150];
				ACE_OS::strcpy(errorText,ACS_EMF_Execute::GetResultCodeText(dwResult, &errorId));
				DEBUG(1,"acs_emf_commandhandler::validateFileNameForFROMMedia - ERROR:%s",errorText);
				DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
				return dwResult;
			}
		}
	}
	DEBUG(1,"%s","acs_emf_commandhandler::validateFileNameForFROMMedia - Leaving");
	return EMF_RC_OK;
}//End of validateFileNameForFROMMedia
/*===================================================================
ROUTINE: performErrChkForFROMMediaOper
======================================================================*/
ACE_INT32 acs_emf_commandhandler::performErrChkForFromMediaOper(std::vector<std::string>& fileList,bool copyAllDataFlag)
{
	DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - Entering");
	ACE_INT32 resultOfOperation = EMF_RC_OK;
	if (fileList.size() == 0){// Check for file arguments
		ERROR(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - ERROR:INVALID ARGUMENT");
		DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - Leaving");
		return EMF_RC_INVALIDARGUMENT;
	}
	if (copyAllDataFlag && fileList.size() > 1){
		ERROR(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - ERROR:Too many Destinations specified");
		DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - Leaving");
		return EMF_RC_TOOMANYDESTINATIONS;
	}
	if (const int retcode = validateFileNameForFROMMedia(fileList,copyAllDataFlag)){
		ERROR(1, "acs_emf_commandhandler::performErrChkForFromMediaOper - call 'validateFileNameForFROMMedia()'failed! <retcode == %d>", retcode);
		DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - Leaving");
		return retcode;
	}
	DEBUG(1,"%s","acs_emf_commandhandler::performErrChkForFromMediaOper - Leaving");
	return resultOfOperation;
}//End of performErrChkForFromMediaOper

/*===================================================================
ROUTINE: setExitCode
======================================================================*/
void acs_emf_commandhandler::setExitCode(int error, std::string text)
{
	errorCode = error;
	errorString = text;
}//end of setExitCode
/*===================================================================
ROUTINE: setExitCode
======================================================================*/
void acs_emf_commandhandler::setExitCode(const int emf_rc_error)
{
	switch (emf_rc_error)
	{
	case EMF_RC_MEDIAACCESSNOTENABLED:
		errorCode = acs_emf_constants::EMF_EXITCODE_MEDIAACCESSNOTENABLED;
		errorString = "External media access is not enabled";
		break;

	case EMF_RC_MEDIAACCESSALREADYENABLED:
		errorCode = acs_emf_constants::EMF_EXITCODE_MEDIAACCESSALREADYENABLED;
		errorString = "External media access is already enabled";
		break;

	case EMF_RC_DVDDEVICENOTFOUND:
		errorCode = acs_emf_constants::EMF_EXITCODE_DVDDEVICENOTFOUND;
		errorString = "No external media board available";
		break;

	case EMF_RC_OPERATIONINPROGRESS:
		errorCode = acs_emf_constants::EMF_EXITCODE_OPERATIONINPROGRESS;
		errorString = "Operation is already in progress";
		break;

	case EMF_RC_DVDDRIVEBUSY:
		errorCode = acs_emf_constants::EMF_EXITCODE_DVDDRIVEBUSY;
		errorString ="Media drive is busy";
		break;

	case EMF_RC_NOMEDIAINDRIVE:
		errorCode = acs_emf_constants::EMF_EXITCODE_NOMEDIAINDRIVE;
		errorString = "No Media in Media Drive";
		break;

	case EMF_RC_MEDIANOTOVERWRITABLE:
		errorCode = acs_emf_constants::EMF_EXITCODE_MEDIA_NOTOVERWRITABLE;
		errorString = "Overwriting data to this media is not supported";
		break;

	case EMF_RC_UNABLETOCONNECTSERV:
		errorCode = acs_emf_constants::EMF_EXITCODE_UNABLETOCONNECTSERV;
		errorString = "Unable to connect to server";
		break;

	case EMF_RC_OPTIONNOTVALIDONTHISPLATFORM:
		errorCode = acs_emf_constants::EMF_EXITCODE_OPTIONNOTVALIDONTHISPLATFORM;
		errorString = "The option is not valid for this platform";
		break;

	case EMF_RC_FORMATOPTIONNOTVALIDONTHISPLATFORM:
		errorCode = acs_emf_constants::EMF_EXITCODE_FORMATOPTIONNOTVALIDONTHISPLATFORM;
		errorString = "The format option in unlockMedia is not valid for this platform";
		break;

	case EMF_RC_UNMOUNTINGFAILED:
		errorCode = acs_emf_constants::EMF_EXITCODE_OTHERERROR;
		errorString = "Failed while unmount media";
		break;

	case EMF_RC_FILENOTFOUND:
	case EMF_RC_CANNOTACCESSTOMEDIA:
		errorCode = acs_emf_constants::EMF_EXITCODE_FILENOTFOUND;
		errorString = "File Not Found";
		break;

	case EMF_RC_PATHNOTFOUND:
		errorCode = acs_emf_constants::EMF_EXITCODE_PATHNOTFOUND;
		errorString = "Folder does not exist";
		break;

	case EMF_RC_SOURCETOOLONG:
		errorCode = acs_emf_constants::EMF_EXITCODE_SOURCETOOLONG;
		errorString = "Operand source too long";
		break;

	case EMF_RC_INVALIDFILENAME:
		errorCode = acs_emf_constants::EMF_EXITCODE_INVALIDPATHNAME;
		errorString = "Invalid File Name";
		break;

	case EMF_RC_INVALIDPATHNAME:
		errorCode = acs_emf_constants::EMF_EXITCODE_INVALIDPATHNAME;
		errorString = "Invalid path name";
		break;

	case EMF_RC_FILEEMPTY:
		errorCode = acs_emf_constants::EMF_EXITCODE_EMPTYFILE;
		errorString = "File empty, copy not possible";
		break;


	case EMF_RC_INVALIDARGUMENT:
		errorCode = acs_emf_constants::EMF_EXITCODE_INVALIDARGUMENT;
		errorString = "Invalid Arguments";
		break;

	case EMF_RC_INVALIDDESTINATIONNAME:
		errorCode = acs_emf_constants::EMF_EXITCODE_INVALIDDESTINATIONNAME;
		errorString = "Invalid destination name";
		break;

	case EMF_RC_PERMISSIONSDENIED:
		errorCode = acs_emf_constants::EMF_EXITCODE_PERMISSIONDENIED;
		errorString = "Permission denied";
		break;

	case EMF_RC_TOOMANYDESTINATIONS:
		errorCode = acs_emf_constants::EMF_EXITCODE_TOOMANYDESTINATIONS;
		errorString = "Too many Destinations specified";
		break;

	case EMF_RC_INVALIDLABELNAME:
		errorCode = acs_emf_constants::EMF_EXITCODE_INVALIDLABELNAME;
		errorString = "Invalid label name";
		break;

	case EMF_RC_LABELNAMETOOLONG:
		errorCode =acs_emf_constants::EMF_EXITCODE_LABELNAMETOOLONG;
		errorString = "Argument label too long";
		break;

	case EMF_RC_OK:
		errorCode = acs_emf_constants::EMF_EXITCODE_SUCCESSFUL;
		errorString = "Successful";
		break;

	default:
		errorCode = acs_emf_constants::EMF_EXITCODE_OTHERERROR;
		errorString = "Other Error";
		break;
	}
}//end of setExitCode
/*===================================================================
ROUTINE: setExitCode
======================================================================*/
void acs_emf_commandhandler::setOperationalState(int operationState)
{
	theOperationalState=operationState;
}
/*===================================================================
ROUTINE: getExitCode
======================================================================*/
int acs_emf_commandhandler::getExitCode()
{
	return errorCode;
}//end of getExitCode
/*===================================================================
ROUTINE: getMediaOwner
======================================================================*/
ACE_UINT16 acs_emf_commandhandler::getMediaOwner()
{
	return theMediaOwner;
}//end of getMediaOwner
/*===================================================================
ROUTINE: getExitCodeString
======================================================================*/
string acs_emf_commandhandler::getExitCodeString()
{
	return errorString;
}//end of getExitCodeString
/*===================================================================
ROUTINE: get_rc_errorText
======================================================================*/
const char * acs_emf_commandhandler::get_rc_errorText(const int emf_rc_error)
{

	switch (emf_rc_error)
	{
	case EMF_RC_DVDDEVICENOTFOUND:  return "No external media board available";

	case EMF_RC_OPERATIONINPROGRESS:  return "Operation is already in progress";

	case EMF_RC_DVDDRIVEBUSY: return "Media drive is busy";

	case EMF_RC_NOMEDIAINDRIVE: return "No Media in Media Drive";

	case EMF_RC_UNABLETOCONNECTSERV: return "Unable to connect to server";

	case EMF_RC_OPTIONNOTVALIDONTHISPLATFORM: return "The option is not valid for this platform";

	case EMF_RC_DISCMEDIAERROR: return "Disc Media error";

	case EMF_RC_MEDIAACCESSNOTENABLED: return "Media Access is not enabled";

	case EMF_RC_FILENOTFOUND: return "File Not Found";

	case EMF_RC_PATHNOTFOUND: return "Folder does not exist";

	case EMF_RC_INVALIDFILENAME: return "Invalid File Name";

	case EMF_RC_INVALIDPATHNAME: return  "Invalid path name";

	case EMF_RC_TOOMANYDESTINATIONS: return "Too many Destinations specified";

	case EMF_RC_LABELNAMETOOLONG: return "Argument label too long";

	case EMF_RC_INVALIDLABELNAME: return "Invalid label name";

	case EMF_RC_SOURCETOOLONG: return "Operand source too long";

	case EMF_RC_FILEEMPTY: return  "File empty, copy not possible";

	case EMF_RC_OK: return "Successful";

	default:
		return "Other error";
	}
}//end of get_rc_errorText
/*===================================================================
ROUTINE: mountDVDOnActiveNode
======================================================================*/
int acs_emf_commandhandler::mountDVDOnActiveNode()
{
	int iRet;
	bool bDvdLocked;
	int32_t iSystemId;
	int32_t iApgOwner;
	acs_dsd::NodeStateConstants enmNodeState;
	ACS_EMF_CmdClient objCmdClient(m_stopHandle);
	ACS_EMF_DVDHandler *pDVDHandlePtr = reinterpret_cast<ACS_EMF_DVDHandler *>(theMediaHandlePtr);
	// Initialization
	bDvdLocked = true;
	iRet = 0;
	iSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
	iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	enmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
	DEBUG(1,"%s", "acs_emf_commandhandler::mountDVDOnActiveNode - Entering");
	// Get local node info
	ACS_EMF_Common::getLocalNode(&iSystemId, &enmNodeState);
	// Get DVD state and owner
	pDVDHandlePtr->getDvdState(&iApgOwner, &bDvdLocked);
	// Check state
	if((bDvdLocked == false) || ((bDvdLocked == true) && (iSystemId == iApgOwner))){
		if(isStopSignaled == true){
			ERROR(1,"%s","acs_emf_commandhandler::mountDVDOnActiveNode - EMF Service has received Stop Signal.");
			DEBUG(1,"%s", "acs_emf_commandhandler::mountDVDOnActiveNode - Leaving");
			return EMF_RC_NOK;
		}

		// This control is make first.
		//		if(ACS_EMF_Common::isMediaAlreadyMountedOnActive() == 2){
		//			DEBUG(1,"%s", "Media might be already mounted to other than FileGroup=media");
		//			DEBUG(1,"%s", "Leaving acs_emf_commandhandler::mountDVDOnActiveNode");
		//			return 2;
		//		}

		if(isMediaEmpty == false){
			// Mount DVD
			iRet = ACS_EMF_Common::mountDVDData();
			for(int i=3; i>0 ; i--)
			{
				if (iRet == EMF_MEDIA_NOK)
				{
					DEBUG(1,"%s", "acs_emf_commandhandler::mountDVDOnActiveNode - Failed to mount DVD data. Retrying...");
					sleep(10);
					iRet = ACS_EMF_Common::mountDVDData();
				}
			}
		}
		//		if (iRet == EMF_MEDIA_NOK)
		//			ERROR(1,"%s","acs_emf_commandhandler::mountDVDOnActiveNode - Failed to mount DVD data even after retries. DVD might be corrupted.");

		// Check if fail of mount is DVD empty
		//if((iRet != 0) && (ACS_EMF_Common::isDVDEmpty() == acs_emf_common::DVDMEDIA_STATE_EMPTY)){
		//		if((iRet != 0) && (isMediaEmpty == true)){
		//			// This is no error. Mount fails because media is empty. Set flag to "no error"
		//			iRet = 0;
		//			// Force media empty to false
		//			isMediaEmpty = false;
		//			// debug message
		//			DEBUG(1,"%s","acs_emf_commandhandler::mountDVDOnActiveNode - mount fail because DVD is empty but DVD is unlocked correctly.");
		//		}
		// Check for error
		if(iRet == EMF_MEDIA_SUCCESS){
			// Lock DVD
			pDVDHandlePtr->setDvdState(iSystemId, true);
			DEBUG(1,"acs_emf_commandhandler::mountDVDOnActiveNode - Set DVD owner [AP=%i]", iSystemId);
			if (ACE_OS::access(mediaOwnerFile, F_OK) == 0){
				ACE_OS::unlink(mediaOwnerFile);
			}
		}else{
			// Mount fail for other cause
			DEBUG(1,"acs_emf_commandhandler::mountDVDOnActiveNode - Mount fail. ACS_EMF_Common::mountDVDData return [%i]", iRet);
		}
	}else{
		// DVD is locked.
		DEBUG(1,"acs_emf_commandhandler::mountDVDOnActiveNode - DVD is locked from another APG (owner [%i])",iApgOwner);
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::mountDVDOnActiveNode - Leaving");
	return iRet;
}//end of mountDVDOnActiveNode

/*===================================================================
ROUTINE: checkAndMountMediaOnAccessibleNode
======================================================================*/
int acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode(const ACS_DSD_Node &remoteNodeInfo)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - Entering");
	int iRet = EMF_RC_NOK;

	if ((theOperationalState == MEDIA_FEATURE_ENABLE) && (theInstance->theMediaState == MEDIA_FEATURE_ENABLE)){
		if ((theInstance->theMediaOwner == MEDIA_ON_CURRENT_NODE) && (ACS_EMF_Common::GetHWVariant() >= 3)) {
			if((ACS_EMF_DSDServer::getOperationalStateinPassive()) != MEDIA_FEATURE_ENABLE) {
				ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_ENABLE);
				theMediaStatusFlag = true;
			}
		}

		if(ACS_EMF_Common::GetHWVariant() < 3) {
			m_enmNodeState = remoteNodeInfo.node_state;
			m_enmNodeSide = remoteNodeInfo.node_side;
		}
		DEBUG(1,"%s","acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode OperationalState is enabled");
		iRet = EMF_RC_MEDIAACCESSALREADYENABLED;
	}
	else if ((theOperationalState != MEDIA_FEATURE_ENABLE) && (theInstance->theMediaState == MEDIA_FEATURE_ENABLE)){
		if (ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE) {
			DEBUG(1,"%s","acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode Waiting for OperationalState to become IDLE");
			while (ACS_EMF_Common::getMediaOperationState() != EMF_MEDIA_OPERATION_IS_IDLE){
				sleep(5);
			}
		}

		if(ACS_EMF_Common::GetHWVariant() >= 3) {
			if (theOperationalState == MEDIA_FEATURE_ENABLE) {
				if (theInstance->theMediaOwner == MEDIA_ON_CURRENT_NODE) {
					if(theMediaStatusFlag == false) {
						ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_ENABLE);
						theMediaStatusFlag = true;
						DEBUG(1,"%s", "acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - Leaving");
						return EMF_RC_MEDIAACCESSALREADYENABLED;
					}
					else {
						DEBUG(1,"%s", "acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - Leaving");
						return EMF_RC_MEDIAACCESSALREADYENABLED;
					}
				}
			}
		}

		ACS_CC_ReturnType adminOpRes = ACS_CC_SUCCESS;
		acs_apgcc_adminoperation admOp;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
		int returnValue;
		long long int timeOutVal_20sec = 20*(1000000000LL);

		std::string dnOfEMFMediaObj(ACS_EMF_Common::parentDNofEMF);
		adminOpRes = admOp.init();

		if (adminOpRes != ACS_CC_SUCCESS){
			DEBUG(1,"%s","acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - EMF admin instance initialization is failed!!");
		}
		else{
			adminOpRes = admOp.adminOperationInvoke(dnOfEMFMediaObj.c_str() , 0, EMF_CMD_UNLOCK_MEDIA, vectorIN, &returnValue, timeOutVal_20sec, vectorOut);
			if (adminOpRes != ACS_CC_SUCCESS){
				DEBUG(1,"acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - FAILED to invoke AdminOperation on %s",dnOfEMFMediaObj.c_str());
				DEBUG(1,"acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - ERROR CODE = %d",admOp.getInternalLastError());
				DEBUG(1,"acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - ERROR MESSAGE = %s",admOp.getInternalLastErrorText());
				admOp.finalize();
				iRet = EMF_RC_MOUNTINGFAILED;
			}
			else{
				iRet = EMF_RC_OK;
			}
		}
	}
	else if ((theOperationalState != MEDIA_FEATURE_ENABLE) && (theInstance->theMediaState != MEDIA_FEATURE_ENABLE)){
		iRet = EMF_RC_MEDIANOTMOUNTED;
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::checkAndMountMediaOnAccessibleNode - Leaving");
	return iRet;
}//end of checkAndMountMediaOnAccessibleNode

int acs_emf_commandhandler::mountnanoUSB(bool format)
{
	DEBUG(1,"%s","acs_emf_commandhandler::mountnanoUSB - Entering");
	if(format){
		DEBUG(1,"%s","acs_emf_commandhandler::mountnanoUSB - Formation option is enabled in unlonckMedia");
		if (ACS_EMF_Common::isMountd() == TRUE){
			if(ACS_EMF_Common::umountUSBData() != EMF_RC_OK){
				DEBUG(1,"%s","ACS_EMF_Common::mountnanoUSB - Failed to unmount media");
			return EMF_RC_ERROR;
			}
		}
		ACS_EMF_Common::setMediaFlag(formatPosition,NO_Sync);
		if(ACS_EMF_Common::formatMedium() == EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::mountnanoUSB - formatMedium is successful !! ");
		}
		else{
			DEBUG(1,"%s","ACS_EMF_Common::mountnanoUSB - Formating thumb drive failed");
			return EMF_RC_ERROR;
		}
		ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
	}
	int ret ;
	ret = ACS_EMF_Common::mountThumbDrive();
	DEBUG(1,"%s", "acs_emf_commandhandler::mountnanoUSB - Leaving");
	return ret;
}
/*===================================================================
ROUTINE: mountMediaOnPassiveNode
======================================================================*/
int acs_emf_commandhandler::mountMediaOnPassiveNode(const char * mountMode,const ACE_TCHAR* formatOption)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Entering");
	ACS_EMF_CmdClient cmdClient(m_stopHandle);
	bool formatCheck = false;
	if(!cmdClient.connect(m_iSystemId, m_enmNodeState)){
		ERROR(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Sending message to DSD server from client is failed");
		DEBUG(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Leaving");
		return EMF_RC_NOK;
	}
	DEBUG(0, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Connection to service on passive node from active node is successful");

	ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_MountMedia);  // 13 - For mounting media on passive node
	if(ACS_EMF_Common::GetHWVariant() < 3){
		if (mountMode != NULL ){
			std::list<string> lstArgs;
			// clear
			lstArgs.clear();
			lstArgs.push_back(mountMode);
			cmdSend.addArguments(lstArgs);
		}
	}else{
		if (formatOption != NULL ){
			std::list<string> lstArgs;
			// clear
			lstArgs.clear();
			 formatCheck = (ACE_OS::strcmp(formatOption, ACE_TEXT("yes")) == 0);
			if(formatCheck)
				ACS_EMF_Common::setMediaFlag(formatPosition,NO_Sync);
			lstArgs.push_back(formatOption);
			cmdSend.addArguments(lstArgs);
		}
	}
	if(!cmdClient.send(cmdSend)){
		ERROR(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Sending message to DSD server from client is failed");
		DEBUG(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Leaving");
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		return EMF_RC_NOK;
	}

	ACS_EMF_Cmd cmdRecv;
	if(!cmdClient.receive(cmdRecv)){
		ERROR(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Client not able to receive reply from DSD server");
		DEBUG(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Leaving");
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		return EMF_RC_NOK;
	}
	int retCode = EMF_RC_OK;
	DEBUG(0, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Received reply from DSD server to client successfully");

	std::list<std::string> argList = cmdRecv.commandArguments();
	int exitCode=(int)cmdRecv.commandCode();
	INFO(1,"acs_emf_commandhandler::mountMediaOnPassiveNode - Code received from Server = %d",exitCode);
	switch (exitCode ){
	case ACS_EMF_DSDServer::EMFSERVER_ERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Error Received from EMF server: Mounting Media on DVD owner fails !");
		retCode = EMF_RC_NOK;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_MEDIAMOUNTEDONOTHERDIR:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Media might be already mounted to other than FileGroup=media");
		//DEBUG(1,"%s", "Leaving acs_emf_commandhandler::mountMediaOnPassiveNode");
		//return 2;
		retCode = EMF_RC_MEDIAMOUNTEDONOTHERDIR;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTPRESENT:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Media is not present on passive node");
		retCode = EMF_RC_NOMEDIAINDRIVE;
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		break;

	case ACS_EMF_DSDServer::EMFSERVER_MEDIAMOUNTERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Mounting Media on other Node is failed");
		retCode = EMF_RC_MOUNTINGFAILED;
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		break;

	case ACS_EMF_DSDServer::EMFSERVER_MEDIAUNMOUNTERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - UnMounting Media on other Node before format is failed");
		retCode = EMF_RC_UNMOUNTINGFAILED;
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		break;

	case ACS_EMF_DSDServer::EMFSERVER_MEDIAFORMATERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Format Media on other Node is failed");
		retCode = EMF_RC_DVDFORMATNOTSUPPORTED;
		if(formatCheck)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		break;

	case ACS_EMF_DSDServer::EMFSERVER_OK:
		if (mountMode != NULL) {  // Logical Mount do not mount media on active node
			DEBUG(1,"acs_emf_commandhandler::mountMediaOnPassiveNode - Mounting Media <mountMode == %s> on Media owner [%d] was successful", mountMode, theMediaOwner);
		}
		else {
			DEBUG(1,"acs_emf_commandhandler::mountMediaOnPassiveNode - Mounting Media on Media owner [%d] was successful", theMediaOwner);
			if(isStopSignaled == true){
				ERROR(1,"%s","acs_emf_commandhandler::mountMediaOnPassiveNode - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
				DEBUG(1, "%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Leaving");
				return EMF_RC_NOK;
			}
			if(formatCheck)
				ACS_EMF_Common::setMediaFlag(formatPosition,Sync);

			/* Check for MediaOwner is Required in GEP5 when the thumb drive is connect to both the node
			 * In GEP5,when both the thumb drive is available , mountPassiveDVDOnActive() must not be called.
			 * */
			if(theMediaOwner ==  MEDIA_ON_PARTNER_NODE ){
				if (!isMediaEmpty && ACS_EMF_Common::mountPassiveDVDOnActive(m_iSystemId, m_enmNodeSide) != 0){
					DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Failed to mount passive media on active.");
					retCode = EMF_RC_NOK;
				}else{
					DEBUG(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Mount passive media on active Successful");
					INFO(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Mounting DVD media on the Media-owner Node is SUCCESSFUL (i.e passiveshare.sh is completed)");
				}
			}
		}
		break;
	default:
		ERROR(1,"acs_emf_commandhandler::mountMediaOnPassiveNode - Unexpected Code received from Server = %d",exitCode);
		ERROR(1,"%s", "acs_emf_commandhandler::mountMediaOnPassiveNode - Mounting DVD media on the DVD-owner Node is FAILED (i.e passiveshare.sh is failed)");
		retCode = EMF_RC_NOK;
		break;
	}
	DEBUG(1,"%s", "Exiting acs_emf_commandhandler::mountMediaOnPassiveNode");
	return retCode;
}//end of mountMediaOnPassiveNode
/*===================================================================
ROUTINE: unmountDVDOnActiveNode
======================================================================*/
int acs_emf_commandhandler::unmountDVDOnActiveNode(const std::string &strMountMode)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNode - Entering");
	int ret = 0;
	bool bUnlockMedia = false;
	ACS_EMF_DVDHandler *pDVDHandlePtr = reinterpret_cast<ACS_EMF_DVDHandler *>(theMediaHandlePtr);
	// Check umount flag
	bUnlockMedia = strMountMode.compare(DSDSERVER_LOCK_MEDIA) == 0;
	if(isStopSignaled == true)
	{
		ERROR(1,"%s","acs_emf_commandhandler::unmountDVDOnActiveNode - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNode - Leaving");
		return EMF_RC_NOK;
	}
	// umount
	if( ACS_EMF_Common::isDVDEmpty() == acs_emf_common::DVDMEDIA_STATE_EMPTY) ret = EMF_RC_OK;
	else ret = ACS_EMF_Common::unmountDVDData();
	// Check error
	// Check error (if error occour, check if mount fail because DVD is empty)
	if((ret == EMF_RC_OK)){
		// unLock DVD delete session if required
		if(bUnlockMedia == true){
			// Release DVD
			pDVDHandlePtr->setDvdState(acs_dsd::SYSTEM_ID_UNKNOWN, false);
			DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNode - DVD has been released!");
			// Set exit flag
			ret = 0;
		}
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNode - Leaving");
	return ret;

}//end of unmountDVDOnActiveNode

/*===================================================================
ROUTINE: unmountDVDOnActiveNode
======================================================================*/
int acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable(const std::string &strMountMode)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - Entering");
	int ret = EMF_RC_OK;
	bool bUnlockMedia = false;
	ACS_EMF_DVDHandler *pDVDHandlePtr = reinterpret_cast<ACS_EMF_DVDHandler *>(theMediaHandlePtr);
	// Check umount flag
	bUnlockMedia = strMountMode.compare(DSDSERVER_LOCK_MEDIA) == 0;
	if(isStopSignaled == true)
	{
		ERROR(1,"%s","acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - Leaving");
		return EMF_RC_NOK;
	}
	// umount
	ACS_EMF_Common::setMediaOperationStateToProgress(EMF_UNMOUNT_OPERATION_IS_IN_PROGRESS);

	if((theMediaOwner == MEDIA_ON_PARTNER_NODE) && (bUnlockMedia == true))
	{
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - theMediaOwner = MEDIA_ON_PARTNER_NODE");
		if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1){
			ERROR(1,"%s","acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - unmount nfs shared media folder from /data/opt/ap/internal_root/media is failed");
			DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - Leaving");
			return  EMF_RC_UNMOUNTMEDIAONACTIVENODE;
		}
		// Release DVD
		pDVDHandlePtr->setDvdState(acs_dsd::SYSTEM_ID_UNKNOWN, false);
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - DVD has been released!");

	}

	ACS_EMF_Common::setMediaOperationStateToIDLE();
	theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
	ACS_EMF_DSDServer::setOperationalStateinPassive(MEDIA_FEATURE_DISABLE);
	theInstance->clearDVDStatus();
	theInstance->update_imm_actionResult(EMF_IMM_RESULT_FAILURE, "Media disabled due to external media available node is down");

	DEBUG(1,"%s", "acs_emf_commandhandler::unmountDVDOnActiveNodeAndDisable - Leaving");
	return ret;
}//end of unmountDVDOnActiveNodeAndDisable


/*===================================================================
ROUTINE: unmountMediaOnPassiveNode
======================================================================*/
int acs_emf_commandhandler::unmountMediaOnPassiveNode(const std::string &strMountMode, const char *rmFile)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Entering");
	ACS_EMF_CmdClient cmdClient(m_stopHandle);
	std::list<string> lstArgs;
	std::string strRmDvdInfo;
	// clear
	lstArgs.clear();
	lstArgs.push_back(strMountMode);
	if(NULL != rmFile){
		strRmDvdInfo=rmFile;
		lstArgs.push_back(strRmDvdInfo);
	}
	if(isStopSignaled == true){
		ERROR(1,"%s","acs_emf_commandhandler::unmountMediaOnPassiveNode - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Leaving");
		return EMF_RC_NOK;
	}
	if(theMediaOwner == MEDIA_ON_PARTNER_NODE && strMountMode.compare(LOGICALMOUNT_MODE)!= 0)
	{
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - theMediaOwner = MEDIA_ON_PARTNER_NODE");
		if (ACS_EMF_Common::unmountPassiveDVDOnActive() == -1){
			ERROR(1,"%s","acs_emf_commandhandler::unmountMediaOnPassiveNode - unmount nfs shared media folder from /data/opt/ap/internal_root/media is failed");
			DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Leaving");
			return  EMF_RC_UNMOUNTMEDIAONACTIVENODE;
		}
	}else if(theMediaOwner == MEDIA_ON_CURRENT_NODE){
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - theMediaOwner = MEDIA_ON_CURRENT_NODE");
	}else{
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - theMediaOwner == UNKNOWN or Logical umount has been ordered!");
	}
	if(!cmdClient.connect(m_iSystemId, m_enmNodeState)){
		ERROR(1, "%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Connection to service on Media-owner node from active node is failed!");
		return EMF_RC_UNABLETOCONNECTSERV;
	}
	ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_UnmountMedia);// 14 - For unmounting media on passive node
	cmdSend.addArguments(lstArgs);
	std::list<std::string> lst = cmdSend.commandArguments();
	std::list<std::string>::const_iterator it;
	DEBUG(1,"acs_emf_commandhandler::unmountMediaOnPassiveNode - Send [%i] elements to Media-owner node.", lst.size());
	for(it = lst.begin(); it !=  lst.end(); ++it)
		DEBUG(1,"acs_emf_commandhandler::unmountMediaOnPassiveNode - Send to Media-owner node element [%s]", (*it).c_str());
	if(!cmdClient.send(cmdSend)){
		ERROR(1, "%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Sending message to DVD-owner node is failed!");
		return EMF_RC_CANNOTSENDCMDTOSERVER;
	}
	ACS_EMF_Cmd cmdRecv;
	if(!cmdClient.receive(cmdRecv)){
		ERROR(1, "%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Client not able to receive reply from DSD server!");
		return EMF_RC_CANNOTRECEIVEROMSERVER;
	}
	int retCode = EMF_RC_OK;
	DEBUG(0, "%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Successfully Received reply from Media-owner!");
	std::list<std::string> argList = cmdRecv.commandArguments();
	int exitCode=(int)cmdRecv.commandCode();
	DEBUG(1,"acs_emf_commandhandler::unmountMediaOnPassiveNode - Code received from Media-owner = %d",exitCode);

	switch(exitCode){
	case ACS_EMF_DSDServer::EMFSERVER_ERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Error Received from EMF server: Mounting Media on DVD owner fails !");
		retCode = EMF_RC_ERRORRECEIVINGFROMSERVER;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTPRESENT:
		DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Media is not present on passive node");
		retCode = EMF_RC_NOMEDIAINDRIVE;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_OK:
		INFO(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - UnMounting Media media on the Media-owner Node is SUCCESSFUL (i.e remove_passivedvd.sh is completed)");
		break;
	default:
		ERROR(1,"acs_emf_commandhandler::unmountMediaOnPassiveNode - Unexpected Code received from Server = %d",exitCode);
		ERROR(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - UnMounting DVD media on the Media-owner Node is FAILED (i.e remove_passivedvd.sh is failed)");
		retCode = EMF_RC_NOK;
		break;
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::unmountMediaOnPassiveNode - Leaving");
	return retCode;
}//end of unmountMediaOnPassiveNode

/*===================================================================
ROUTINE: checkForMedia
======================================================================*/
int acs_emf_commandhandler::checkForMedia()
{
	ACS_EMF_CmdClient cmdClient(m_stopHandle);
	int retCode = EMF_RC_OK;
	DEBUG(1,"%s", "acs_emf_commandhandler::checkForMedia - Entering");
	if(cmdClient.connect(m_iSystemId, m_enmNodeState)){
		DEBUG(0, "%s", "acs_emf_commandhandler::checkForMedia - Connection to service on passive node from active node is successful");
		ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_CheckForDVDMediaPresence);  // 11 - For DVD media checking
		if(cmdClient.send(cmdSend)){
			DEBUG(1, "%s", "acs_emf_commandhandler::checkForMedia - command has been successfully sent to DVD-owner node!");
			ACS_EMF_Cmd cmdRecv;
			if(cmdClient.receive(cmdRecv,EMF_TIMEOUT_ON_RECEIVE)){
				DEBUG(0, "%s", "acs_emf_commandhandler::checkForMedia - Received reply from DSD server to client successfully");

				std::list<std::string> argList = cmdRecv.commandArguments();
				int exitCode=(int)cmdRecv.commandCode();
				DEBUG(1,"acs_emf_commandhandler::checkForMedia - Code received from DVD-owner = %d",exitCode);
				if (exitCode == 1)
					retCode = -1;
				// TR HR44823 - BEGIN
				if( ACS_EMF_Common::GetHWVariant() < 3){
					// TODO: check why it is done twice!!
					argList = cmdRecv.commandArguments();
					std::list<std::string>::iterator itr = argList.begin();
					int i = 0;
					while(itr != argList.end()){
						std::string arg = (*itr);
						DEBUG(1,"acs_emf_commandhandler::checkForMedia - arguments received from passive : %s",arg.c_str());
						switch(i)
						{
						case 0:
							theMediumType = arg;
							break;
						case 1:
							isMediaEmpty = ((arg.compare("EMPTY") == 0) ? true : false);
							break;
						}
						i++;
						++itr;
					}
					DEBUG(1,"acs_emf_commandhandler::checkForMedia - Medium Type received = %s",theMediumType.c_str());
				}
				// TR HR44823 - END
			}
			else{
				ERROR(1, "%s", "acs_emf_commandhandler::checkForMedia - Client not able to receive reply from server");
				retCode =EMF_RC_CANNOTRECEIVEROMSERVER;
			}
		}
		else{
			ERROR(1, "%s", "acs_emf_commandhandler::checkForMedia - Sending message to DSD server from client is failed");
			retCode = EMF_RC_CANNOTSENDCMDTOSERVER;
		}

	}
	else{
		ERROR(1,"%s", "acs_emf_commandhandler::checkForMedia - Connection to service on passive node from active node is failed");
		//flag = false;
		retCode = EMF_RC_UNABLETOCONNECTSERV;
	}
	if(isStopSignaled == true){
		ERROR(1,"%s","acs_emf_commandhandler::checkForMedia - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","acs_emf_commandhandler::checkForMedia - Leaving");
		return 1;
	}
	if (retCode == 0){
		INFO(1,"%s", "acs_emf_commandhandler::checkForMedia - return OK");
	}else{
		ERROR(1,"%s", "acs_emf_commandhandler::checkForMedia - return error");
	}

	DEBUG(1,"%s", "acs_emf_commandhandler::checkForMedia - Leaving");
	return retCode;

}//end of checkForMedia
/*===================================================================
ROUTINE: getMediaInfo
======================================================================*/
int acs_emf_commandhandler::getMediaInfo(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace)
{
	ACS_EMF_CmdClient cmdClient(m_stopHandle);
	int retCode = EMF_RC_OK;
	// Initialize out arguments
	mediaType = 0;
	freeSpace = 0;
	usedSpace = 0;
	totalSpace = 0;

	DEBUG(1, "%s", "acs_emf_commandhandler::getMediaInfo - Entering");
	if(cmdClient.connect(m_iSystemId, m_enmNodeState)){
		DEBUG(1, "%s", "acs_emf_commandhandler::getMediaInfo - Connection to DVD-owner is successful");
		ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_FetchMediaInfo);  // 12 - For Fetching DVD media info
		if(cmdClient.send(cmdSend)){
			DEBUG(1, "%s", "acs_emf_commandhandler::getMediaInfo - Command [acs_emf_cmd_ns::EMF_CMD_FetchMediaInfo] has been successfully sent to DVD-owner node!");
			ACS_EMF_Cmd cmdRecv;
			if(cmdClient.receive(cmdRecv)){
				DEBUG(0, "%s", "acs_emf_commandhandler::getMediaInfo - Received reply from DSD server to client successfully");
				std::list<std::string> argList = cmdRecv.commandArguments();
				int exitCode=(int)cmdRecv.commandCode();
				DEBUG(1,"acs_emf_commandhandler::getMediaInfo - Code received from DVD-owner is [%d]",exitCode);
				if(exitCode == 0){
					argList = cmdRecv.commandArguments();
					// FOR DISPLAY
					std::list<std::string>::iterator itr = argList.begin();
					int i = 0;
					while(itr != argList.end()){
						std::string arg = (*itr);
						DEBUG(1,"acs_emf_commandhandler::getMediaInfo - arguments received from DVD-owner : %s",arg.c_str());
						switch(i)
						{
						case 0:
							mediaType = atoi(arg.c_str());
							break;
						case 1:
							freeSpace = atoi(arg.c_str());
							break;
						case 2:
							usedSpace = atoi(arg.c_str());
							break;
						case 3:
							totalSpace = atol(arg.c_str());
							break;
						}
						i++;
						++itr;
						//DEBUG(" cmdSize = %d",cmdSize);
					}// while
				}else{
					// exitCode != 1 : error
					DEBUG(1, "%s", "acs_emf_commandhandler::getMediaInfo - cmdRecv.commandCode fail.");
					retCode = EMF_RC_DVDDEVICENOTFOUND;
				}
			}else{
				ERROR(1, "%s", "acs_emf_commandhandler::getMediaInfo - Client not able to receive reply from server");
				retCode = EMF_RC_UNABLETOCONNECTSERV;
			}
		}else{
			ERROR(1, "%s", "acs_emf_commandhandler::getMediaInfo - Sending message to DSD server from client is failed");
			retCode = EMF_RC_UNABLETOCONNECTSERV;
		}

	}else{
		ERROR(1, "%s", "acs_emf_commandhandler::getMediaInfo - Connection to service on DVD-owner node from active node is failed");
		retCode = EMF_RC_UNABLETOCONNECTSERV;
	}
	DEBUG(1,"%s", "acs_emf_commandhandler::getMediaInfo - Leaving");
	return retCode;
}//end of getMediaInfo
/*===================================================================
ROUTINE: performUnmountOperation
======================================================================*/
int acs_emf_commandhandler::performUnmountOperation(int node)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::performUnmountOperation - Entering");
	if(isStopSignaled == true){
		ERROR(1,"%s","acs_emf_commandhandler::performUnmountOperation - EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s", "acs_emf_commandhandler::performUnmountOperation - Leaving");
		return EMF_RC_NOK;
	}
	int result = EMF_RC_NOK;
	if(node == MEDIA_ON_CURRENT_NODE){
		// DVD Drive is present on the current node.
		result = unmountDVDOnActiveNode(DSDSERVER_UMOUNTONLY_MEDIA);
		DEBUG(1,"%s", "acs_emf_commandhandler::performUnmountOperation - node=[MEDIA_ON_CURRENT_NODE]");

	}
	else if (node == MEDIA_ON_PARTNER_NODE){
		// DVD drive is present on other node
		result = unmountMediaOnPassiveNode(DSDSERVER_UMOUNTONLY_MEDIA);
		DEBUG(1,"%s", "acs_emf_commandhandler::performUnmountOperation - node=[MEDIA_ON_PARTNER_NODE]");

	}else{
		DEBUG(1,"%s", "acs_emf_commandhandler::performUnmountOperation - Invalid node.");
	}
	DEBUG(1,"%s","acs_emf_commandhandler::performUnmountOperation - Leaving");
	return result;
}//end of performUnmountOperation
/*===================================================================
ROUTINE: performMountOperation
======================================================================*/
int acs_emf_commandhandler::performMountOperation(int node){
	DEBUG(1,"%s", "acs_emf_commandhandler::performMountOperation - Entering");
	if(isStopSignaled == true){
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s", "acs_emf_commandhandler::performMountOperation - Leaving");
		return EMF_RC_NOK;
	}
	int result = EMF_RC_NOK;
	if(node == MEDIA_ON_CURRENT_NODE){
		// DVD Drive is present on the current node.
		result = mountDVDOnActiveNode();
		DEBUG(1,"%s", "acs_emf_commandhandler::performMountOperation - node=[MEDIA_ON_CURRENT_NODE]");
	}
	else if (node == MEDIA_ON_PARTNER_NODE){
		// DVD drive is present on other node
		result = mountMediaOnPassiveNode();
		DEBUG(1,"%s", "acs_emf_commandhandler::performMountOperation - node=[MEDIA_ON_PARTNER_NODE]");
	}else{
		DEBUG(1,"%s", "acs_emf_commandhandler::performMountOperation - Invalid node.");
	}
	DEBUG(1, "%s", "acs_emf_commandhandler::performMountOperation - Leaving");
	return result;
}//end of performMountOperation
/*===================================================================
ROUTINE: adminoperationString
======================================================================*/
const char * acs_emf_commandhandler::adminoperationString (ACS_APGCC_AdminOperationIdType  opId){
	switch (opId)
	{
	case EMF_CMD_TYPE_COPYTOMEDIA:  return ("COPYTOMEDIA");
	case EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA: return ("ERASEANDCOPYTOMEDIA");
	case EMF_CMD_TYPE_COPYFROMMEDIA: return ("CPYFROMMEDIA");
	case EMF_CMD_TYPE_GETMEDIAINFO: return ("GETMEDIAINFO");
	case EMF_CMD_UNLOCK_MEDIA:  return("UNLOCK_MEDIA");
	case EMF_CMD_LOCK_MEDIA: return ("LOCK_MEDIA");
	case EMF_CMD_TYPE_CLEANUPANDLOCK: return ("CLEANUP_AND_LOCK");
	default:  return ("UNKNOWN ACTION");
	}
}//end of adminoperationString
/*===================================================================
ROUTINE: umountnanoUSB
======================================================================*/
int acs_emf_commandhandler::umountnanoUSB(){
	DEBUG(1,"%s", "acs_emf_commandhandler::umountnanoUSB - Entering");
	int ret = EMF_RC_OK;
	ret = ACS_EMF_Common::umountUSBData();
	DEBUG(1,"%s", "acs_emf_commandhandler::umountnanoUSB - Leaving");
	return ret;
}//end of umountnanoUSB

ACE_INT16 acs_emf_commandhandler::removeNewCopiedFile(ACS_EMF_DATA* pData)
{
	DEBUG(1,"%s", "acs_emf_commandhandler::removeNewCopiedFile - Entering");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);
	int nNum = (Params.NumOfData() - 3); // No of files
	DEBUG(1,"acs_emf_commandhandler::removeNewCopiedFile - Total No. of File arguments received is %d",nNum);
	const ACE_TCHAR* lpszFile;
	const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES] = {'\0'}; // contains file list
	ACE_INT32 nooffiles = 0;     // No of files

	for (int nIdx = 0; nIdx < nNum; nIdx++){
		lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
		// Check all file/s are exist or not
		DEBUG(1,"acs_emf_commandhandler::removeNewCopiedFile - %s",lpszFile);
		fileList[nooffiles] = lpszFile;
		nooffiles++;
	}
	int ret = ACS_EMF_Common::removeFilesOrFolders(fileList,nooffiles);
	DEBUG(1,"%s", "acs_emf_commandhandler::removeNewCopiedFile - Leaving");
	return ret;
}
void acs_emf_commandhandler::update_imm_actionResult(emf_imm_result_t result, const char *resultInfo){

	std::string resultInfo_str(resultInfo);
	theMediaHandlePtr->modifyStructObjFinalAttr(result, resultInfo_str);
	theMediaHandlePtr->modifyStructObjState();
}

void acs_emf_commandhandler::set_imm_action_startingState(int opId)
{
	theMediaHandlePtr->modifyStructObjInitialAttr(opId);

}

void acs_emf_commandhandler::update_imm_HistoryAttrs(const char *resultInfo)
{
	theMediaHandlePtr->modifyEndHistoryAttrs((ACE_TCHAR*) resultInfo);
	theMediaHandlePtr->copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
}


ACE_INT32 acs_emf_commandhandler::sendreceive_to_mediaOwner(const ACS_EMF_Cmd & cmdSend, ACS_EMF_Cmd & cmdRecv)
{
	ACE_INT32 systemId = theInstance->m_iSystemId;
	acs_dsd::NodeStateConstants current_nodeState = theInstance->m_enmNodeState;
	DEBUG(1, "acs_emf_commandhandler::sendreceive_to_mediaOwner - EMF is trying to send  a command to AP == %d state == %d ",systemId, current_nodeState);

	ACS_EMF_CmdClient cmdClient(m_stopHandle);
	ACE_INT32 resultCode = EMF_RC_OK;
	if ((resultCode = cmdClient.send_receive(cmdSend, systemId, current_nodeState, cmdRecv)) != 0){
		ERROR(1, "acs_emf_commandhandler::sendreceive_to_mediaOwner - Sending command Failed ! <resultCode == %d>", resultCode);
		return resultCode;
	}
	//get the result of command execution on Media Owner
	int command_result=(int)cmdRecv.commandCode();
	if(command_result == ACS_EMF_DSDServer::EMFSERVER_MEDIANOTPRESENT ||
			command_result == ACS_EMF_DSDServer::EMFSERVER_MEDIANOTCONFIGURED){
		DEBUG(1, "acs_emf_commandhandler::sendCommandToMediaOwner - Received 'MEDIANOTPRESENT' or 'MEDIANOTCONFIGURED' from DSD server <resultCode == %d>",resultCode);
		acs_dsd::NodeStateConstants nodeState;
		//try to connect changing node state, a failover could be occurred after unlockmedia operation!!
		nodeState = (current_nodeState == acs_dsd::NODE_STATE_ACTIVE)? acs_dsd::NODE_STATE_PASSIVE:
		acs_dsd::NODE_STATE_ACTIVE;

		DEBUG(1, "acs_emf_commandhandler::sendreceive_to_mediaOwner - EMF is trying to send command <%d> to AP == %d state == %d ",cmdSend.commandCode(), systemId, nodeState);
		theInstance->m_enmNodeState = nodeState;
		int mount_result = theInstance->mountMediaOnPassiveNode(LOGICALMOUNT_MODE);
		if (mount_result != EMF_RC_OK){
			ERROR(1, "acs_emf_commandhandler::sendreceive_to_mediaOwner - mount DVD media failed! <mount_result == %d> ",mount_result);
			theInstance->m_enmNodeState = current_nodeState;
			return EMF_RC_MOUNTINGFAILED;
		}

		if ((resultCode = cmdClient.send_receive(cmdSend, systemId, nodeState, cmdRecv)) != 0){
			ERROR(1, "acs_emf_commandhandler::sendreceive_to_mediaOwner - Sending command Failed ! <resultCode == %d> ",resultCode);
			theInstance->m_enmNodeState = current_nodeState;
			return resultCode;
		}
		//get newly the result of command execution on Media Owner
		command_result=(int)cmdRecv.commandCode();
	}
	DEBUG(1, "%s", "acs_emf_commandhandler::sendreceive_to_mediaOwner - Received reply from DSD server to client successfully");
	DEBUG(1,"acs_emf_commandhandler::sendreceive_to_mediaOwner - Code received from Server=[%d]",command_result);
	//std::list<std::string> argList = cmdRecv.commandArguments();
	switch (command_result ){
	case ACS_EMF_DSDServer::EMFSERVER_OK:
		DEBUG(1,"%s","acs_emf_commandhandler::sendreceive_to_mediaOwner - Command execution on other node is completed");
		break;
	case ACS_EMF_DSDServer::EMFSERVER_ERROR:
		DEBUG(1,"%s", "acs_emf_commandhandler::sendreceive_to_mediaOwner - Error Received from EMF server!");
		resultCode = EMF_RC_ERRORRECEIVINGFROMSERVER;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTPRESENT:
	case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTCONFIGURED:
		DEBUG(1,"%s", "acs_emf_commandhandler::sendreceive_to_mediaOwner - Media might be no longer present or configured on APG");
		resultCode = EMF_RC_DVDDEVICENOTFOUND;
		break;
	case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTMOUNTED:
	case ACS_EMF_DSDServer::EMFSERVER_MEDIABUSY:
		DEBUG(1,"%s", "acs_emf_commandhandler::sendreceive_to_mediaOwner - Media is no longer mounted");
		resultCode = EMF_RC_MEDIANOTMOUNTED;
		break;
	default:
		if(cmdSend.commandCode() == acs_emf_cmd_ns::EMF_CMD_EraseAndCopyToMedia)
		{
			if((ACS_EMF_Common::GetHWVariant() >= 3) && (theMediaStatusFlag == true)){
				//if(resultCode != EMF_RC_OK){
				if(theMediaHandlePtr->syncMediaOnActive() == 0){
					ACS_EMF_Common::setMediaFlag(Sync_Pos,Sync);
					ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode());
				}
				//	}
			}
			if(!isStopSignaled){
				if (theInstance->performMountOperation(MEDIA_ON_PARTNER_NODE) != EMF_RC_OK)
					ERROR(1, "%s", "acs_emf_commandhandler::mediaOperationOnOtherNode - performMountOperation failed during Format Operation");
			}
		}
		resultCode = EMF_RC_ERRORRECEIVINGFROMSERVER;
		break;
	}
	return resultCode;
}

ACS_CC_ReturnType acs_emf_commandhandler::unlockMedia()
{
	DEBUG(1,"%s","acs_emf_commandhandler::unlockMedia - Entering");

	thr_grp_id  =  ACS_EMF_Common::StartThread(EmfUnlockMediaOperationProc,(void *)this, ACE_TEXT("UnlockMediaOperation"));
	if (thr_grp_id == -1){
		ERROR(1,"%s","acs_emf_commandhandler::unlockMedia - ERROR:Unable to create thread for performing Unlock Media operation.");
		return ACS_CC_FAILURE;
	}
	DEBUG(1,"%s","acs_emf_commandhandler::unlockMedia - creation of thread for performing Unlock Media operation is succesful.");
	return ACS_CC_SUCCESS;
}

ACE_THR_FUNC_RETURN acs_emf_commandhandler::EmfUnlockMediaOperationProc(void* lpvThis)
{
	DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaOperationProc - Entering");
	((acs_emf_commandhandler*)lpvThis)->EmfUnlockMediaInvoke();
	DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaOperationProc - Leaving");
	return 0;
}

int acs_emf_commandhandler::EmfUnlockMediaInvoke()
{
	DEBUG(1,"%s", "acs_emf_commandhandler::EmfUnlockMediaInvoke - Entering");
	bool isMediaPresent = false;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
        int retCode = EMF_RC_OK;
	std::string dev_info;

	if(ACS_EMF_Common::GetHWVariant() < 3){
		ACE_INT16 iTmp = 0;
		iTmp = ACS_EMF_Common::checkForDVDStatus(dev_info);
		if ((iTmp & CHECKDVD_MASK_PRESENT ) != 0)
			isMediaPresent = true;
	}

	if (theInstance->theMediaState == 1){
		int nodeState = 0;
		if((ACS_EMF_Common::GetHWVariant() < 3)) {
			if (isMediaPresent != true) {
				ACS_EMF_Common::getPeerNodeState(nodeState);
				if (nodeState == 1){
					DEBUG(1,"%s", "acs_emf_commandhandler::EmfUnlockMediaInvoke - Peer node connected with Media is up and running.");
					isMediaPresent = true;
				}
			}
		}
		else { //GEP5
			if (ACS_EMF_Common::checkForUSBStatus() == 0)
				isMediaPresent = true;
			else {
				ACS_EMF_Common::getPeerNodeState(nodeState);
				if (nodeState == 1){
					DEBUG(1,"%s", "acs_emf_commandhandler::EmfUnlockMediaInvoke - Peer node connected with Media is up and running.");
					isMediaPresent = true;
				}
			}
		}

		if(isMediaPresent){
			acs_apgcc_adminoperation admOp;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
			int returnValue;
			long long int timeOutVal_20sec = 20*(1000000000LL);

			std::string dnOfEMFMediaObj(ACS_EMF_Common::parentDNofEMF);
			result = admOp.init();

			if (result != ACS_CC_SUCCESS){
				DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaInvoke - EMF admin instance initialization is failed!!");
				retCode = EMF_RC_NOK;
			}
			else{
				result = admOp.adminOperationInvoke(dnOfEMFMediaObj.c_str() , 0, EMF_CMD_UNLOCK_MEDIA, vectorIN, &returnValue, timeOutVal_20sec, vectorOut);
				if (result != ACS_CC_SUCCESS){
					retCode = EMF_RC_NOK;
					DEBUG(1,"acs_emf_commandhandler::EmfUnlockMediaInvoke - FAILED to invoke AdminOperation on %s",dnOfEMFMediaObj.c_str());
					DEBUG(1,"acs_emf_commandhandler::EmfUnlockMediaInvoke - ERROR CODE = %d",admOp.getInternalLastError());
					DEBUG(1,"acs_emf_commandhandler::EmfUnlockMediaInvoke - ERROR MESSAGE = %s",admOp.getInternalLastErrorText());
					admOp.finalize();
				}
			}
		}
		else{
			theInstance->theOperationalState = MEDIA_FEATURE_DISABLE;
			DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaInvoke - No media is connected to this node and peer node is down");
		}
	}
	else {
		if(ACS_EMF_Common::GetHWVariant() < 3){
			if ( (ACE_OS::access(mediaOwnerFile, F_OK) == 0) && (isMediaPresent == true) ) {
				retCode =  theMediaHandlePtr->enableMediaOnAccessibleNode(theInstance->theMediaState);
				if( EMF_RC_OK != retCode ) {
					DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaInvoke - Unlock Media Failed");
				}
			}


		}
	}
	return result;
}

ACE_THR_FUNC_RETURN acs_emf_commandhandler::EmfUnlockMediaIfDVDIsEmpty(void* lpvThis)
{
        DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaIfDVDIsEmpty - Entering");
        ((acs_emf_commandhandler*)lpvThis)->modifyMediaStateInvoke();
        DEBUG(1,"%s","acs_emf_commandhandler::EmfUnlockMediaIfDVDIsEmpty - Leaving");
        return 0;
}

ACS_CC_ReturnType acs_emf_commandhandler::modifyMediaStateInvoke()
{
	DEBUG(1,"%s","acs_emf_commandhandler::modifyMediaStateInvoke - Entering");
        ACS_CC_ReturnType res = ACS_CC_FAILURE;
	if(theInstance->theMediaState != theInstance->theOperationalState){
        	res = theInstance->modifyMediaState(theInstance->theOperationalState);
                theInstance->theMediaState = theInstance->theOperationalState;
        }
	return res;
}

ACS_CC_ReturnType acs_emf_commandhandler::getMediaState(int &mediaState)
{
	// To find whether EMF Feature is enabled or not
	DEBUG(1,"%s","acs_emf_commandhandler::getMediaState - Entering");
	ACS_CC_ReturnType ret = ACS_CC_FAILURE;
	std::string dnNameOfNodeObj(ACS_EMF_Common::parentDNofEMF);

	DEBUG(1,"acs_emf_commandhandler::getMediaState - DN = %s",dnNameOfNodeObj.c_str());
	OmHandler* omHandlerPtr = new OmHandler();
	if( omHandlerPtr->Init() == ACS_CC_FAILURE ){
		DEBUG(1,"%s","acs_emf_commandhandler::getMediaState - FAILED to init OmHandler for reading mediaState");
		return ACS_CC_FAILURE;
	}
	ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
	paramToFindPtr->attrName = new ACE_TCHAR [30];
	ACS_CC_ReturnType enResult;
	ACE_OS::strcpy( paramToFindPtr->attrName,theEMFMediaState);
	if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNodeObj.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS ){
		if (paramToFindPtr->attrValuesNum != 0){
			mediaState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
			DEBUG(1,"acs_emf_commandhandler::getMediaState - mediaState = %d",mediaState);
			ret = ACS_CC_SUCCESS;
		}
	}
	else{
		DEBUG(1,"%s","acs_emf_commandhandler::getMediaState - failed to fetch mediaState from EMF MO!!");
		ret = ACS_CC_FAILURE;
	}
	delete [] paramToFindPtr->attrName;
        if(NULL != paramToFindPtr) {
                delete(paramToFindPtr);
                paramToFindPtr = NULL;
        }
	omHandlerPtr->Finalize();
	return ret;
}

ACS_CC_ReturnType acs_emf_commandhandler::modifyMediaState(int mediaState)
{
	DEBUG(1,"%s","acs_emf_commandhandler::modifyMediaState - Entering");
	OmHandler immHandle;
	ACS_CC_ReturnType res;
	res = immHandle.Init();
	if ( res != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","acs_emf_commandhandler::modifyMediaState - ERROR: OmHandler Initialization FAILURE!!! in modifyMediaState");
		DEBUG(1,"%s","acs_emf_commandhandler::modifyMediaObjState - Leaving");
		return ACS_CC_FAILURE;
	}

	ACS_CC_ImmParameter paramToChange;
	paramToChange.attrName = new ACE_TCHAR [30];
	int state = mediaState;

	// mediaState
	ACE_OS::strcpy( paramToChange.attrName,theEMFMediaState);
	paramToChange.attrType = ATTR_INT32T;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(&state);

	std::string dnOfEMFMediaObj(ACS_EMF_Common::parentDNofEMF);
	DEBUG(1,"acs_emf_commandhandler::modifyMediaState - dnOfEMFMediaObj = %s",dnOfEMFMediaObj.c_str());
	DEBUG(1,"acs_emf_commandhandler::modifyMediaState - theEMFMediaState = %s",theEMFMediaState);

	res = immHandle.modifyAttribute(dnOfEMFMediaObj.c_str(),&paramToChange);
	if(res != ACS_CC_SUCCESS)
	{
		ERROR(1,"acs_emf_commandhandler::modifyMediaState - ERROR: Unable to modify media state attribute - %d",immHandle.getInternalLastError());
		ERROR(1,"acs_emf_commandhandler::modifyMediaState - ERROR MESSAGE: %s",immHandle.getInternalLastErrorText());
	}
	immHandle.Finalize();
	delete [] paramToChange.attrName;
	delete []  paramToChange.attrValues;
	DEBUG(1,"%s","acs_emf_commandhandler::modifyMediaState - Leaving");
	return res;
}
