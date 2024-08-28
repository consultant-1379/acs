/*
 * @file ACS_CS_API_ProfileHandling.cpp
 * @author xmikhal
 * @date Jan 9, 2011
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


#include "ACS_CS_API_ProfileHandling.h"

#include <string>
#include <sstream>

#include "ACS_CS_Util.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_EventReporter.h"

#include "ACS_CS_API_Tracer.h"
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_ProfileHandling_TRACE);


using namespace std;
using namespace ACS_CS_NS;
using namespace ACS_CS_Event_NS;


bool ACS_CS_API_ProfileHandling::checkQueuedApaProfiles ()
{
    ACS_CS_API_OmProfileChange omProfileData;

    ACS_CS_Table *table = ACS_CS_API_NeHandling::loadNeTable();

    if (table == NULL)
        return false;

    ACS_CS_TableEntry entry = table->getEntry(ENTRY_ID_OM_PROFILE);
    ACS_CS_API_Util::readOmProfile(entry, omProfileData);

    delete table;

    if (omProfileData.aptQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile &&
        omProfileData.apzQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
    {
        // We can initiate the queued APA profile change.
        omProfileData.changeReason = ACS_CS_API_OmProfileChange::AutomaticProfileAlignment;

        // Transfer queued values to requested values.
        omProfileData.aptRequested = omProfileData.aptQueued;
        omProfileData.apzRequested = omProfileData.apzQueued;

        // Clear previously queued values now that they have been copied to requested.
        omProfileData.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
        omProfileData.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;

        // Check if we need to initiate APA. If the current and requested profiles
        // are same then ignore APA.
        if (omProfileData.aptCurrent == omProfileData.aptRequested &&
            omProfileData.apzCurrent == omProfileData.apzRequested)
        {
            return ignoreApaProfileRequest(omProfileData);
        }

        // Move to validate phaseACS_CS_API_ProfileHandling
        omProfileData.phase = ACS_CS_API_OmProfilePhase::Validate;

        // write data to the table
        ACS_CS_API_Util::writeOmProfile(omProfileData);

        // Notify Subscribers and continue with phase change.
        ACS_CS_API_NeHandling::setNeSubscriberCount(0);

        // Notify subscribers and get actual subscriber count for the phase.
        ACS_CS_API_NeHandling::updatePhaseChange(omProfileData);

        // Start Phase 0 Supervision Timer for 60 seconds
        ACS_CS_API_NeHandling::setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Validate, PHASE_TIMEOUT_IN_SECONDS);

        ostringstream eventData;
        eventData << "APA Profile change initiated." << "\n\n"
                  << convertOmProfileToString(omProfileData);

        ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

        ACS_CS_EventReporter::instance()->resetAllEvents();
    }

    return true;
}


bool ACS_CS_API_ProfileHandling::ignoreApaProfileRequest (ACS_CS_API_OmProfileChange& omProfileChange)
{
    ostringstream eventData;

    eventData << "APA Profile change request is ignored since requested profiles are same as current.\n\n"
              << convertOmProfileToString(omProfileChange);

    ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

    ACS_CS_EventReporter::instance()->resetAllEvents();

    // We're done...reset to Idle phase.
    omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;

    // Reset requested profile values to unspecified.
    omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
    omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

    // store updated profile values back to table.
    if (!ACS_CS_API_Util::writeOmProfile(omProfileChange))
    {
        // Failed to store updated profile values.
        return false;
    }

   return true;
}


bool ACS_CS_API_ProfileHandling::rollbackOmProfile ()
{
    ACS_CS_API_OmProfileChange omProfileChange;
    ostringstream eventData;

    //ACS_CS_Table *table = ACS_CS_API_NeHandling::loadNeTable();

    //if (table == NULL)
      //  return false;

    // get current profile values from table.
    //ACS_CS_TableEntry entry = table->getEntry(ENTRY_ID_OM_PROFILE); //advancedConfigurationId=1,AxeEquipmentequipmentMId=1, attr = omProfileCurrent
    ACS_CS_API_Util::readOmProfile(NULL, omProfileChange);

    //delete table;

    switch (omProfileChange.phase)
    {
        case ACS_CS_API_OmProfilePhase::Validate:
        case ACS_CS_API_OmProfilePhase::ApNotify:
        case ACS_CS_API_OmProfilePhase::CpNotify:

            // Immediately stop the supervision timer from the current phase.




        	//ACS_CS_API_NeHandling::cancelOmProfileSupervisionTimer(omProfileChange.phase); // TODO ?????????????????????????????????
            //TODO ?????? ACS_CS_Timer::CancelTimer(omProfileSupervisionTimerHandle[phase]); ??????????????

            // Initiate rollback to current phase.
            omProfileChange.omProfileRequested = omProfileChange.omProfileCurrent;
            omProfileChange.aptRequested = omProfileChange.aptCurrent;
            omProfileChange.apzRequested = omProfileChange.apzCurrent;

            // jump to the commit phase.
            omProfileChange.phase = ACS_CS_API_OmProfilePhase::Commit;

            eventData << "Moving to Phase " << omProfileChange.phase << "."
                      << "\n\n" << convertOmProfileToString(omProfileChange);

            ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

            ACS_CS_EventReporter::instance()->resetAllEvents();

            // Mark as NoChange to avoid clearing alarm on Commit.
            omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

            // Write updates profile change info to table.
            ACS_CS_API_Util::writeOmProfile(omProfileChange);

            // Send notifications and get current subcriberCount
            ACS_CS_API_NeHandling::updatePhaseChange(omProfileChange);

            // Set supervision timer for commit phase.
            ACS_CS_API_NeHandling::setOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Commit, PHASE_COMMIT_TIMEOUT_IN_SECONDS);

            break;

        case ACS_CS_API_OmProfilePhase::Commit:

            if (ACS_CS_API_NeHandling::getNeSubscriberCount() == 0)
            {
                // Failures in commit phase are simply logged and ignored...
                // However, we do need to handle the case where the last
                // subscriber responds with a failure...go back to Idle.

                // Stop the supervision timer for the Commit phase.
                ACS_CS_API_NeHandling::cancelOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::Commit);

                omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;

                // Reset requested profile values to unspecified.
                omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
                omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
                omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
                omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;

                // Write updates profile change info to table.
                ACS_CS_API_Util::writeOmProfile(omProfileChange);

                eventData << "OaM Profile Change completed."
                          << "\n\n" << convertOmProfileToString(omProfileChange);

                ACS_CS_EVENT(Event_ProfileChangeEvent, ACS_CS_EventReporter::Severity_Event, "OaM Profile Switch", eventData.str(), "");

                ACS_CS_EventReporter::instance()->resetAllEvents();
            }

            break;

        case ACS_CS_API_OmProfilePhase::Idle:
            // Do nothing
            break;

        default:
            // Should never be here!
        	ACS_CS_API_TRACER_MESSAGE("Error: Undefined Phase (%d)\n", omProfileChange.phase);
            break;
    }

    return true;
}


std::string ACS_CS_API_ProfileHandling::convertOmProfileToString (const ACS_CS_API_OmProfileChange& omProfile)
{
   ostringstream omProfileData;
   omProfileData << "Current OaM Profile: " << omProfile.omProfileCurrent << "\n"
      << "Current APT Profile: " << omProfile.aptCurrent << "\n"
      << "Current APZ Profile: " << omProfile.apzCurrent << "\n\n"
      << "Requested OaM Profile: ";

   if (omProfile.omProfileRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.omProfileRequested;
   }
   omProfileData << "\n"
      << "Requested APT Profile: ";

   if (omProfile.aptRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.aptRequested;
   }
   omProfileData << "\n"
      << "Requested APZ Profile: ";

   if (omProfile.apzRequested != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.apzRequested;
   }


   omProfileData << "\n"
      << "Change Reason: ";

   // Note: ACS_CS_API_OmProfileChange_R1 object should get a ".getChangeReasonName()"
   // (1) The way it is now, if changeReason is expanded, this code location is easily missed...
   // (2) the following switch block would just become a one-liner where ever this is used:
   //     omProfileData << omProfile.getChangeReasonName();

   switch (omProfile.changeReason)
   {
      case ACS_CS_API_OmProfileChange::NoChange:
         omProfileData << "NoChange";
         break;
      case ACS_CS_API_OmProfileChange::NechCommand:
         omProfileData << "NechCommand";
         break;
      case ACS_CS_API_OmProfileChange::AutomaticProfileAlignment:
         omProfileData << "AutomaticProfileAlignment";
         break;
      case ACS_CS_API_OmProfileChange::NechCommandForApgOnly:
         omProfileData << "NechCommandOnlyAPG";
         break;
      default:
         omProfileData << "Unknown";
   }

   omProfileData << "(" << omProfile.changeReason << ")\n";


   omProfileData << "Queued APT Profile: ";

   if (omProfile.aptQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.aptQueued;
   }
   omProfileData << "\n"
      << "Queued APZ Profile: ";

   if (omProfile.apzQueued != ACS_CS_API_OmProfileChange::UnspecifiedProfile)
   {
      omProfileData << omProfile.apzQueued;
   }

   return omProfileData.str();
}


std::string ACS_CS_API_ProfileHandling::convertReasonToString (ACS_CS_API_Set::ReasonType reason)
{
   string reasonString;

   switch(reason)
   {
      case ACS_CS_API_Set::ClusterSessionLockOngoing:
         reasonString = "ClusterSessionLockOngoing";
         break;
      case ACS_CS_API_Set::CommandClassificationMissing:
         reasonString = "CommandClassificationFileMissing";
         break;
      case ACS_CS_API_Set::InvalidCommandClassificationFile:
         reasonString = "InvalidCommandClassificationFile";
         break;
      case ACS_CS_API_Set::GeneralFailure:
         reasonString = "GeneralFailure";
         break;
      default:
         reasonString = "UnknownReason";
         break;
   }

   return reasonString;
}
