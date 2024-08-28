
/*=================================================================== */
   /**
   @file APGCC_Amf_CC.cpp

   Class method implementation for APG common class API to integrate APG applications with AMF.

   This module contains the implementation of class declared in
   the apg_amf_cc_api.h 

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "APGCC_Amf_CC.h"

SaAmfHandleT saf::ApplicationManager::amf_handle=-1 ;
SaVersionT saf::ApplicationManager::amf_version={'B',0x01,0x01};

#define m_GET_AMF_VERSION(amf_ver) \
		amf_ver.releaseCode='B'; \
		amf_ver.majorVersion=0x01; \
		amf_ver.minorVersion=0x01; 

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: amfVersionGet
=================================================================== */

SaVersionT saf::ApplicationManager::amfVersionGet(){
	m_GET_AMF_VERSION(amf_version);
	return amf_version;
}

/*===================================================================
   ROUTINE: initialize
=================================================================== */

SaAisErrorT saf::ApplicationManager::initialize(SaAmfCallbacksT* callbacks) {

	m_GET_AMF_VERSION(amf_version);
	return  saAmfInitialize(&amf_handle,
				callbacks,
				&amf_version);
}


/*===================================================================
   ROUTINE: selectionObjectGet
=================================================================== */
SaAisErrorT saf::ApplicationManager::selectionObjectGet(SaSelectionObjectT* out_selection_object) {

	return saAmfSelectionObjectGet( amf_handle,
					out_selection_object);

}

/*===================================================================
   ROUTINE: dispatch
=================================================================== */
SaAisErrorT saf::ApplicationManager::dispatch(SaDispatchFlagsT dispatch_flags) {

	return saAmfDispatch(amf_handle,
			     dispatch_flags );
}

/*===================================================================
   ROUTINE: finalize
=================================================================== */
SaAisErrorT saf::ApplicationManager::finalize() {

	return saAmfFinalize(amf_handle);
}

/*===================================================================
   ROUTINE: componentRegister
=================================================================== */
SaAisErrorT saf::ApplicationManager::componentRegister(const SaNameT* comp_name,
						       const SaNameT* proxy_comp_name) {

	return saAmfComponentRegister(amf_handle,
				      comp_name,
				      proxy_comp_name);
}

/*===================================================================
   ROUTINE: componentUnRegister
=================================================================== */
SaAisErrorT saf::ApplicationManager::componentUnRegister(const SaNameT* comp_name,
					  	         const SaNameT* proxy_comp_name){

	 return saAmfComponentUnregister(amf_handle,
					 comp_name,
					 proxy_comp_name);
}

/*===================================================================
   ROUTINE: componentNameGet
=================================================================== */
SaAisErrorT saf::ApplicationManager::componentNameGet(SaNameT* out_comp_name) {

	return saAmfComponentNameGet(amf_handle,
				     out_comp_name);
}

/*===================================================================
   ROUTINE: pmStart
=================================================================== */
SaAisErrorT saf::ApplicationManager::pmStart(const SaNameT* comp_name,
					     SaInt64T process_id,
					     SaInt32T descendents_tree_depth,
					     SaAmfPmErrorsT pm_errors,
					     SaAmfRecommendedRecoveryT recommended_recovery ) {

	return saAmfPmStart(amf_handle,
			    comp_name,
			    process_id,
			    descendents_tree_depth,
			    pm_errors,
		            recommended_recovery);

}

/*===================================================================
   ROUTINE: pmStop
=================================================================== */
SaAisErrorT saf::ApplicationManager::pmStop(const SaNameT* comp_name,
					    SaAmfPmStopQualifierT stop_qualifier,
					    SaInt64T process_id,SaAmfPmErrorsT pm_errors ) {

	return saAmfPmStop(amf_handle,
			   comp_name,
			   stop_qualifier,
			   process_id,
			   pm_errors );

}

/*===================================================================
   ROUTINE: healthcheckStart
=================================================================== */
SaAisErrorT saf::ApplicationManager::healthcheckStart(const SaNameT* comp_name,
						      const SaAmfHealthcheckKeyT* healthcheck_key,
						      SaAmfHealthcheckInvocationT invocation_type,
						      SaAmfRecommendedRecoveryT recommended_recovery) {

	return saAmfHealthcheckStart( amf_handle,
				      comp_name,
				      healthcheck_key,
				      invocation_type,
				      recommended_recovery);

}

/*===================================================================
   ROUTINE: healthcheckConfirm
=================================================================== */
SaAisErrorT saf::ApplicationManager::healthcheckConfirm(const SaNameT* comp_name,
							const SaAmfHealthcheckKeyT* healthcheck_key,
							SaAisErrorT healthcheck_result) {

	return saAmfHealthcheckConfirm(amf_handle,
				       comp_name,
				       healthcheck_key,
				       healthcheck_result);

}	

/*===================================================================
   ROUTINE: healthcheckStop
=================================================================== */
SaAisErrorT saf::ApplicationManager::healthcheckStop(const SaNameT* comp_name,
						     const SaAmfHealthcheckKeyT* healthcheck_key) {

	return saAmfHealthcheckStop(amf_handle,
				    comp_name,
			 	    healthcheck_key);

}

/*===================================================================
   ROUTINE: haStateGet
=================================================================== */
SaAisErrorT saf::ApplicationManager::haStateGet(const SaNameT* comp_name,
					  	const SaNameT* csi_name,
					  	SaAmfHAStateT* out_ha_state) {


	return saAmfHAStateGet(	amf_handle,
			    	comp_name,
			    	csi_name,
			    	out_ha_state);

}

/*===================================================================
   ROUTINE: csiQuiescingComplete
=================================================================== */
SaAisErrorT saf::ApplicationManager::csiQuiescingComplete(SaInvocationT invocation,
							  SaAisErrorT error) {

	return saAmfCSIQuiescingComplete(amf_handle,
					 invocation,
					 error);

}

/*===================================================================
   ROUTINE: haReadinessStateSet
=================================================================== */
SaAisErrorT saf::ApplicationManager::haReadinessStateSet(const SaNameT* comp_name,
							 const SaNameT* csi_name,
							 SaAmfHAReadinessStateT ha_readiness_state,
							 SaNtfCorrelationIdsT* inout_correlation_ids) {
	SaAisErrorT rc = SA_AIS_OK;

	/* AMF-B.01.01 Specification does not provide this API */
	/* AMF-B.04.01 Specification provides this API */

	return rc;
}

/*===================================================================
   ROUTINE: protectionGroupTrack
=================================================================== */
SaAisErrorT saf::ApplicationManager::protectionGroupTrack(const SaNameT* csi_name,
							  SaUint8T track_flags,
							  SaAmfProtectionGroupNotificationBufferT* inout_notification_buffer) {

	return saAmfProtectionGroupTrack(amf_handle,
					 csi_name,
					 track_flags,
					 inout_notification_buffer);
}

/*===================================================================
   ROUTINE: protectionGroupTrackStop
=================================================================== */
SaAisErrorT saf::ApplicationManager::protectionGroupTrackStop(const SaNameT* csi_name) {

	return saAmfProtectionGroupTrackStop(amf_handle,
					     csi_name);

}

/*===================================================================
   ROUTINE: protectionGroupNotificationFree
=================================================================== */
SaAisErrorT saf::ApplicationManager::protectionGroupNotificationFree(SaAmfProtectionGroupNotificationT_4* notification) {

	SaAisErrorT rc = SA_AIS_OK;

	/* AMF-B.01.01 Specification does not provide this API */
	/* AMF-B.04.01 Specification provides this API */

	return rc;
}

/*===================================================================
   ROUTINE: componentErrorReport
=================================================================== */
SaAisErrorT saf::ApplicationManager::componentErrorReport(const SaNameT* erroneous_component,
						    	  SaTimeT error_detection_time,
						    	  SaAmfRecommendedRecoveryT recommended_recovery,
						    	  SaNtfCorrelationIdsT* inout_correlation_ids) {

	return saAmfComponentErrorReport(amf_handle,
					 erroneous_component,
					 error_detection_time,
					 recommended_recovery,
					 0);

}

/*===================================================================
   ROUTINE: componentErrorClear
=================================================================== */
SaAisErrorT saf::ApplicationManager::componentErrorClear(const SaNameT* comp_name,
						         SaNtfCorrelationIdsT* inout_correlation_ids) {

	return saAmfComponentErrorClear(amf_handle,
					comp_name,
					0);

}

/*===================================================================
   ROUTINE: correlationIdsGet
=================================================================== */
SaAisErrorT saf::ApplicationManager::correlationIdsGet(SaInvocationT invocation,
						       SaNtfCorrelationIdsT* inout_correlation_ids) {
	SaAisErrorT rc = SA_AIS_OK;

	/* AMF-B.01.01 Specification does not provide this API */
	/* AMF-B.04.01 Specification provides this API */

	return rc;
}

/*===================================================================
   ROUTINE: response
=================================================================== */
SaAisErrorT saf::ApplicationManager::response(SaInvocationT invocation,
					      SaAisErrorT error) {

	return saAmfResponse(amf_handle,
			     invocation,
			     error);
}

//******************************************************************************

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

