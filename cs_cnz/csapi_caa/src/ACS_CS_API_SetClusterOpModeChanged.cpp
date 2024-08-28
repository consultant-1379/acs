/*
 * @file ACS_CS_API_SetClusterOpModeChanged.cpp
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


#include "ACS_CS_API_Util_Internal.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Util.h"

#include "ACS_CS_API_SetClusterOpModeChanged.h"

#include "ACS_CS_API_Tracer.h"
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_SetClusterOpModeChanged_TRACE);

using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_INTERNAL_API;


 ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode (
         ACS_CS_API_ClusterOpMode::Value newClusterOpMode, ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType)
{

     ACS_CS_API_SET_NS::CS_API_Set_Result resultValue = ACS_CS_API_SET_NS::Result_Failure;

     AsyncActionStateType state = UNDEFINED_STATE;
     AsyncActionType idAction = UNDEFINED_TYPE;
     ActionResultType result = NOT_AVAILABLE;
     string reason("");

 /*    // Old way with ACS_CS_Table but ACS_CS_API_NeHandling::loadNeTable() should fetch from new data model
     ACS_CS_Table *table = ACS_CS_API_NeHandling::loadNeTable();

     if (table == NULL)
         return resultValue;    // Loading of NE table failed

     ACS_CS_API_OmProfileChange profileChangeData;
     ACS_CS_TableEntry entry = table->getEntry(ENTRY_ID_OM_PROFILE);
     ACS_CS_API_Util::readOmProfile(entry, profileChangeData);

     delete table;

     if (profileChangeData.phase != ACS_CS_API_OmProfilePhase::Idle)
     {
         // A profile change is already in progress.
         ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                 "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                 "Profile switch already in progress"));

         return resultValue;
     }

     if (opType != ACS_CS_API_NE_NS::CLUSTER_OP_MODE_REPLICATION)
     {
         // for all operation types except replication, we fail if omProfile is blade

         ACS_CS_API_OmProfileChange omProfile;
         ACS_CS_TableEntry entry = table->getEntry(ENTRY_ID_OM_PROFILE);
         ACS_CS_API_Util::readOmProfile(entry, omProfile);

         // Profile 0 is considered blade profile, all others are considered cluster profiles.
         if (omProfile.omProfileCurrent == 0)
         {
             ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                     "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                     "Incorrect mode, profile 0 considered blade profile"));

             return resultValue;
         }
     }

     // Check that the suggested identifier is valid
     if ( !ACS_CS_API_Util::isValidClusterOpMode(newClusterOpMode) )
     {
         ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                 "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                 "Error: invalid cluster operation mode.\n"));

         return resultValue;
     }

     if(opType == ACS_CS_API_NE_NS::CLUSTER_OP_MODE_REQUEST)
     {
         if (!CS_ProtocolChecker::checkIfAP1())
         {
             ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                     "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                     "We're not the master, but ok"));

             // If not master, Just say thanks and leave it
             return ACS_CS_API_SET_NS::Result_Success;
         }

         // Request to initiate switch (currently only NECH sends Requests)
         if(!ACS_CS_API_NeHandling::startupProfileSupervisionComplete())
         {
             ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                     "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                     "Mode switch prohibited for %d seconds after server start.\n",
                     INITIALIZATION_DELAY / MILLISECONDS_PER_SECOND));

             return resultValue;
         }

         if(newClusterOpMode == ACS_CS_API_ClusterOpMode::Normal)
         {
             if(oldClusterOpMode == ACS_CS_API_ClusterOpMode::Normal)
             {
                 // Already in Normal mode. Just return success.
                 ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                         "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                         "Already in Normal mode"));

                 return ACS_CS_API_SET_NS::Result_Success;
             }
             else if(oldClusterOpMode != ACS_CS_API_ClusterOpMode::Expert)
             {
                 // We're already switching modes
                 ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                         "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                         "We're already switching modes"));

                 return resultValue;
             }
             else
             {
                 // Request is for a mode switch to Normal, convert to SwitchingToNormal
                 // The service will take care of the rest with the propagation to the normal mode
                 newClusterOpMode = ACS_CS_API_ClusterOpMode::SwitchingToNormal;
             }
         }
         else if(newClusterOpMode == ACS_CS_API_ClusterOpMode::Expert)
         {
             // Request is for a switch to Expert mode
             if(oldClusterOpMode == ACS_CS_API_ClusterOpMode::Expert)
             {
                 // Already in Expert mode.  Just return success.
                 ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                         "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                         "Already in Expert mode"));

                 return ACS_CS_API_SET_NS::Result_Success;
             }
             else if(oldClusterOpMode != ACS_CS_API_ClusterOpMode::Normal)
             {
                 // Already switching modes
                 ACS_CS_TRACE((ACS_CS_API_SetClusterOpModeChanged_TRACE,
                         "ACS_CS_API_SetClusterOpModeChanged::setClusterOpMode()\n"
                         "Already switching modes"));

                 return resultValue;
             }
             else
             {
                 // Request is for Expert mode, convert to SwitchingToExpert
                 // The service will take care of the rest with the propagation to the expert mode
                 newClusterOpMode = ACS_CS_API_ClusterOpMode::SwitchingToExpert;
             }
         }
     } // End of Request logic
     else*/

     ACS_CS_API_ClusterOpMode::Value oldClusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
     readClusterOpMode(oldClusterOpMode);

//     cout << __FUNCTION__ << "Old Cluster Op Mode: "<< oldClusterOpMode << endl;
//     cout << __FUNCTION__ << "New Cluster Op Mode: "<< newClusterOpMode << endl;

     if(opType == ACS_CS_API_NE_NS::CLUSTER_OP_MODE_CHANGED)
     {
    	 state = FINISHED;
    	 idAction = CHANGE_OP_MODE;
    	 result = SUCCESS;

    	 if (CS_ProtocolChecker::getServiceType() != ACS_CS_Protocol::ServiceType_MASTER)
    	 {
    		 ACS_CS_API_TRACER_MESSAGE("We're not the master, but ok");

    		 // If not master, Just say thanks and leave it
    		 return ACS_CS_API_SET_NS::Result_Success;
    	 }

    	 //HS21311 - begin
    	 if (ACS_CS_API_ClusterOpMode::Expert == oldClusterOpMode && ACS_CS_API_ClusterOpMode::Expert == newClusterOpMode )
    	 {
    		 ACS_CS_API_TRACER_MESSAGE("ClusterOpMode Expert for old and new request, the ALARM has to be raised !!!");

    		 ACS_CS_EVENT(Event_ClusterOpModeExpert,
    				 ACS_CS_EventReporter::Severity_O1,
    				 "AP, CLUSTER OPERATION MODE, EXPERT",
    				 " ", //HS33984 - space needed to avoid the dash character as problem text
    				 "",
    				 "acs_csd");
    	 }
    	 //HS21311 - end
    	 else
    	 {
    		 // Notification from ADH that mode switch is complete
    		 if((newClusterOpMode == ACS_CS_API_ClusterOpMode::Normal &&
    				 oldClusterOpMode != ACS_CS_API_ClusterOpMode::SwitchingToNormal) ||
    				 (newClusterOpMode == ACS_CS_API_ClusterOpMode::Expert &&
    						 oldClusterOpMode != ACS_CS_API_ClusterOpMode::SwitchingToExpert))
    		 {
    			 // Unexpected transition, but not considered an error.
    			 // Log a trace statement, but honor the request.
    			 ACS_CS_API_TRACER_MESSAGE(
    					 "Unexpected clusterOpMode transition from %d to %d",
    					 oldClusterOpMode, newClusterOpMode);

    			 reason = "Unexpected clusterOpMode transition";
    		 }

    		 // Actually set the new value
    		 if(ACS_CS_API_Util::writeClusterOpMode(newClusterOpMode, opType, state,idAction,result,reason))
    		 {
    			 resultValue = ACS_CS_API_SET_NS::Result_Success;

    			 ACS_CS_API_TRACER_MESSAGE("NEWCLUSTEROPMODE start");


    			 if (CS_ProtocolChecker::getServiceType() == ACS_CS_Protocol::ServiceType_MASTER)
    			 {
    				 if(newClusterOpMode==ACS_CS_API_ClusterOpMode::Expert)
    				 {
    					 ACS_CS_API_TRACER_MESSAGE("NEWCLUSTEROPMODE Expert");

    					 ACS_CS_EVENT(Event_ClusterOpModeExpert,
    							 ACS_CS_EventReporter::Severity_O1,
    							 "AP, CLUSTER OPERATION MODE, EXPERT",
    							 " ",	//HS33984 - space needed to avoid the dash character as problem text
    							 "",
    							 "acs_csd");
    				 }
    				 else if(newClusterOpMode==ACS_CS_API_ClusterOpMode::Normal)
    				 {
    					 ACS_CS_API_TRACER_MESSAGE("NEWCLUSTEROPMODE Normal");
    					 ACS_CS_CEASE(Event_ClusterOpModeExpert, "acs_csd");
    				 }
    			 }

    		 }

    		 ACS_CS_API_NeHandling::cancelClusterOpModeSupervisionTimer();
    	 }

    	 ACS_CS_EventReporter::instance()->resetAllEvents();

     }

     return resultValue;
}


 bool ACS_CS_API_SetClusterOpModeChanged::readClusterOpMode (ACS_CS_Table *table,
         ACS_CS_API_ClusterOpMode::Value &clusterOpMode)
{
     ACS_CS_TableEntry entry(ILLEGAL_TABLE_ENTRY);

     entry = table->getEntry(ENTRY_ID_CLUSTER_OP_MODE);

     if (entry.getId() == ENTRY_ID_CLUSTER_OP_MODE)
     {
         ACS_CS_Attribute attribute = entry.getValue(Attribute_NE_ClusterOpMode);

         if(attribute.getIdentifier() == Attribute_NE_ClusterOpMode &&
                 attribute.getValueLength() == sizeof(clusterOpMode))
         {
             // Found a good attriute...get it's value.
             attribute.getValue(reinterpret_cast<char *>(&clusterOpMode), sizeof(clusterOpMode));
         }
         else
         {
             // Use default value of Normal
             clusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
         }
     }
     else // If no entry was found, use defaults.
     {
    	 ACS_CS_API_TRACER_MESSAGE("Old ClusterOpMode attribute not valid...using default Normal");

         // Use default value of Normal
         clusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
     }

     return true;
}

 bool ACS_CS_API_SetClusterOpModeChanged::readClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode)
 {
	 return ACS_CS_API_Util_Internal::readClusterOpMode(clusterOpMode);

 }
