/*
 * @file ACS_CS_API_SetOmProfileNotification.h
 * @author xmikhal
 * @date Jan 4, 2011
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


#ifndef ACS_CS_API_SetOmProfileNotification_h
#define ACS_CS_API_SetOmProfileNotification_h 1

#include "ACS_CS_API_NeHandling.h"
#include "ACS_CS_API_ProfileHandling.h"

class ACS_CS_API_SetOmProfileNotification
{
    public:

        ACS_CS_API_SetOmProfileNotification();

        ~ACS_CS_API_SetOmProfileNotification();

        ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileNotificationStatus(ACS_CS_API_OmProfilePhase::PhaseValue phase, int reasonCode);

        ACS_CS_API_SET_NS::CS_API_Set_Result setOmProfileApaNotification(ACS_CS_API_OmProfileChange::Profile aptProfile, ACS_CS_API_OmProfileChange::Profile apzProfile);


    private:

        ACS_CS_API_ProfileHandling *profileHandling;

};

#endif // ACS_CS_API_SetOmProfileNotification_h
