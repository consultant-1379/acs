/*=================================================================== */
/**
   @file   APGCC_Amf_CC.h

   @brief Header file for APG Common Class API to Integrate with AMF.

          This module contains all the declarations useful to
          ApplicationManager class.
   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef APG_COREMW_AMF_H_
#define APG_COREMW_AMF_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "saAmf.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/* ==================================================================== */

/**
      @brief     The ApplicationManager class is responsible for providing common APIs for 
	APG aplications to integrate with Availability Management Framework of CoreMW.
*/


namespace saf {

	class ApplicationManager {

	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
  	private:
		
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */

  	public:
		/*
		@brief 	member data containing the AMF Handle designating
			this particular initialization of the Availability Management Framework and 
			current AMF version being used.
		*/
		static SaAmfHandleT amf_handle;
		static SaVersionT amf_version;


		/****************************************************************************
  		Name          : amfVersionGet

  		Description   : This function initializes the AMF version for the invoking process

  		Arguments     : None

  		Return Values : AMF version

  		Notes         : None.
		******************************************************************************/

		static SaVersionT amfVersionGet();


		/****************************************************************************
  		Name          : initialize

  		Description   : This function initializes the AMF for the invoking process
                 		 and registers the various callback functions.

  		Arguments     : callbacks - ptr to a SaAmfCallbacksT structure

  		Return Values : Refer to SAI-AIS specification for various return values.

  		Notes         : None.
		******************************************************************************/
		static SaAisErrorT initialize(SaAmfCallbacksT* callbacks);

		/****************************************************************************
		  Name          : selectionObjectGet

 		  Description   : This function creates & returns the operating system handle
                  		  associated with the AMF Handle.

		  Arguments     : out_selection_object - ptr to the selection object

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/

		static SaAisErrorT selectionObjectGet(SaSelectionObjectT* out_selection_object);

		/****************************************************************************
		  Name          : dispatch

		  Description   : This function invokes, in the context of the calling thread,
               			   the next pending callback for the AMF handle.

		  Arguments     : dispatch_flags - flags that specify the callback execution behavior
		                  of the dispatch() function,

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/

		static SaAisErrorT dispatch(SaDispatchFlagsT dispatch_flags);

		/****************************************************************************
		  Name          : finalize

		  Description   : This function closes the association, represented by the
               			   AMF handle, between the invoking process and the AMF.

		  Arguments     : None

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT finalize();

		/****************************************************************************
		  Name          : componentRegister

		  Description   : This function registers the component with the AMF.

		  Arguments     : comp_name       - ptr to the comp name
		                  proxy_comp_name - ptr to the proxy comp name

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT componentRegister(const SaNameT* comp_name
						    ,const SaNameT* proxy_comp_name);


		/****************************************************************************
		  Name          : componentUnRegister

		  Description   : This function unregisters the component with the AMF.

		  Arguments     : comp_name       - ptr to the comp name
		                  proxy_comp_name - ptr to the proxy comp name

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT componentUnRegister(const SaNameT* comp_name
						      ,const SaNameT* proxy_comp_name);


		/****************************************************************************
		  Name          : componentNameGet

		  Description   : This function returns the name of the component to which the
               			   process belongs.

		  Arguments     : out_comp_name - ptr to the comp name

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT componentNameGet(SaNameT* out_comp_name);


		/****************************************************************************
		  Name          : pmStart

		  Description   : This function allows a process (as part of a comp) to start
               			   Passive Monitoring.

		  Arguments     : comp_name      - ptr to the comp name
		                  process_id      - Identifier of a process to be monitored
               			  descendents_tree_depth - Depth of the tree of descendents of the process
		                  pm_errors          - Specifies the type of process errors to monitor
               			  recommended_recovery   - recommended recovery


		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT pmStart(const SaNameT* comp_name
					   ,SaInt64T process_id
					   ,SaInt32T descendents_tree_depth
					   ,SaAmfPmErrorsT pm_errors
					   ,SaAmfRecommendedRecoveryT recommended_recovery);

		/****************************************************************************
		  Name          : pmStop

		  Description   : This function allows a process to stop an already started
               			   passive monitor.

		  Arguments     : comp_name  - ptr to the comp name
               			  stop_qualifier   - Qualifies which processes should stop being monitored
		                  process_id  - Identifier of a process to be monitored
               			  pm_errors      - type of process errors that the Availability
                               			   Management Framework should stop monitoring

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT pmStop(const SaNameT* comp_name
					,SaAmfPmStopQualifierT stop_qualifier
					,SaInt64T process_id
					,SaAmfPmErrorsT pm_errors);


		/****************************************************************************
		  Name          : healthcheckStart

		  Description   : This function allows a process (as part of a comp) to start
               			   a specific configured healthcheck.

		  Arguments     : comp_name   - ptr to the comp name
		                  healthcheck_key      - ptr to the healthcheck type
               			  invocation_type         - indicates whether the healthcheck is
                               					 AMF initiated / comp initiated
		                  recommended_recovery    - recommended recovery


		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/		

		static SaAisErrorT healthcheckStart(const SaNameT* comp_name
						   ,const SaAmfHealthcheckKeyT* healthcheck_key
						   ,SaAmfHealthcheckInvocationT invocation_type
						   ,SaAmfRecommendedRecoveryT recommended_recovery);


		/****************************************************************************
		  Name          : healthcheckConfirm

		  Description   : This function allows a process (as part of a component) to
               			   inform the AMF that it has performed the healthcheck.

		  Arguments     : comp_name - ptr to the comp name
		                  healthcheck_key    - ptr to the healthcheck type
               			  healthcheck_result - healthcheck result

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT healthcheckConfirm(const SaNameT* comp_name
						     ,const SaAmfHealthcheckKeyT* healthcheck_key
						     ,SaAisErrorT healthcheck_result);

		
		/****************************************************************************
		  Name          : healthcheckStop

		  Description   : This function allows a process to stop an already started
               			   healthcheck.

		  Arguments     : comp_name - ptr to the comp name
		                  healthcheck_key    - ptr to the healthcheck type

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT healthcheckStop(const SaNameT* comp_name
						  ,const SaAmfHealthcheckKeyT* healthcheck_key);


		/****************************************************************************
		  Name          : haStateGet

		  Description   : This function returns the HA state of the CSI assigned to
               			   the component.

		  Arguments     : comp_name - ptr to the comp name
		                  csi_name  - ptr to the CSI name
               			  out_ha_state      - ptr to the CSI HA state

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT haStateGet(const SaNameT* comp_name
					     ,const SaNameT* csi_name
					     ,SaAmfHAStateT* out_ha_state);

		
		/****************************************************************************
		  Name          : csiQuiescingComplete

		  Description   : This functions allows a component to inform the AMF that it
               			   has successfully completed its ongoing work and is now idle.

		  Arguments     : invocation   - invocation value (used to match the corresponding
			                          callback)
		                  error - status of the operation

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT csiQuiescingComplete(SaInvocationT invocation
							,SaAisErrorT error);

		/****************************************************************************
		  Name          : haReadinessStateSet

		  Description   : This function is invoked to set the HA readiness state of the pre-instantiable component
					identified by the name to which comp_name refers for the component service
					instance identified by the name to which csi_name refers.

		  Arguments     : comp_name - pointer to the name of the pre-instantiable component
				  csi_name  - pointer to the name of the component service instance
				  ha_readiness_state - The HA Readiness state that the Availability Management
							Framework must set for the component identified by the comp_name
				  inout_correlation_ids - Pointer to correlation identifiers associated with the
							  HA readiness state change

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT haReadinessStateSet(const SaNameT* comp_name
						      ,const SaNameT* csi_name
						      ,SaAmfHAReadinessStateT ha_readiness_state
						      ,SaNtfCorrelationIdsT* inout_correlation_ids);


		/****************************************************************************
 	          Name          : protectionGroupTrack

		  Description   : This fuction requests the AMF to start tracking the changes
                 			 in the PG associated with the specified CSI.

		  Arguments     : csi_name  - ptr to the CSI name
		                  track_flags     - flag that determines when the PG callback is called
		                  inout_notification_buffer       - ptr to the linear buffer provided by the application

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT protectionGroupTrack(const SaNameT* csi_name
						      ,SaUint8T track_flags
						      ,SaAmfProtectionGroupNotificationBufferT* inout_notification_buffer);


		/****************************************************************************
		  Name          : protectionGroupTrackStop

		  Description   : This fuction requests the AMF to stop tracking the changes
               			   in the PG associated with the CSI.

		  Arguments     : csi_name  - ptr to the CSI name

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT protectionGroupTrackStop(const SaNameT* csi_name);


		/****************************************************************************
		  Name          : protectionGroupNotificationFree

		  Description   : This function frees the memory to which notification points and which was allocated
					by the Availability Management Framework library in a previous call to the
					protectionGroupTrack() function.

		  Arguments     : notification  - pointer to the memory that was allocated by the Availability
						Management Framework library in the protectionGroupTrack() function
						and is to be released

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT protectionGroupNotificationFree(SaAmfProtectionGroupNotificationT_4* notification);


		/****************************************************************************
		  Name          : componentErrorReport

		  Description   : This function reports an error and a recovery recommendation
               			   to the AMF.

		  Arguments     : erroneous_component - ptr to the erroneous comp name
		                  error_detection_time - error detection time
               			  recommended_recovery - recommended recovery
		                  inout_correlation_ids - Pointer to correlation identifiers associated with the error report

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : Notification Identifier is currently not used.
		******************************************************************************/
		static SaAisErrorT componentErrorReport(const SaNameT* erroneous_component
							,SaTimeT error_detection_time
							,SaAmfRecommendedRecoveryT recommended_recovery
							,SaNtfCorrelationIdsT* inout_correlation_ids);


		/****************************************************************************
		  Name          : componentErrorClear

		  Description   : This function clears the previous errors reported about
               			   the component.

		  Arguments     : comp_name - ptr to the comp name
		                  inout_correlation_ids - Pointer to correlation identifiers associated with the error clear

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : Notification Identifier is currently not used.
		******************************************************************************/
		static SaAisErrorT componentErrorClear(const SaNameT* comp_name
						      ,SaNtfCorrelationIdsT* inout_correlation_ids);


		/****************************************************************************
		  Name          : correlationIdsGet

		  Description   : This function is typically used by a process that needs to generate a notification as a
				consequence of a particular callback invocation that the process has received from
				the Availability Management Framework, and the process wants to set the proper
				correlation identifiers in this notification to allow the reconstruction of the notification
				correlation tree.

		  Arguments     : invocation - This parameter identifies a particular invocation of a callback
						function by the Availability Management Framework.
		                  inout_correlation_ids - pointer to the correlation identifiers to which the invocation
							of the callback function identified by invocation is related.

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : Notification Identifier is currently not used.
		******************************************************************************/
		static SaAisErrorT correlationIdsGet(SaInvocationT invocation
						    ,SaNtfCorrelationIdsT* inout_correlation_ids);

		
		/****************************************************************************
		  Name          : response

		  Description   : The component responds to the AMF with the result of its
               			   execution of a particular AMF request.

		  Arguments     : invocation   - invocation value (used to match the corresponding callback)
		                  error - status of the operation

		  Return Values : Refer to SAI-AIS specification for various return values.

		  Notes         : None.
		******************************************************************************/
		static SaAisErrorT response(SaInvocationT invocation
					   ,SaAisErrorT error);
	};

}

//******************************************************************************

#endif /* APG_COREMW_AMF_H_ */

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

