/*
 * @file ACS_CS_API_NeHandling.h
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

#ifndef ACS_CS_API_NeHandling_h
#define ACS_CS_API_NeHandling_h 1

#include "ACS_CS_Table.h"

namespace ACS_CS_API_NE_NS
{
    typedef struct SetOmProfileTimerData {
        ACS_CS_API_OmProfilePhase::PhaseValue phase;
        int timeout;
    } SetOmProfileTimerData;

    typedef struct SetPhaseSubscriberData {
        unsigned int validate;
        unsigned int apNotify;
        unsigned int cpNotify;
        unsigned int commit;
    } SetPhaseSubscriberData;

    typedef enum {
		CLUSTER_OP_MODE_REQUEST = 0,
		CLUSTER_OP_MODE_CHANGED = 1,
		CLUSTER_OP_MODE_REPLICATION = 2
	} ACS_CS_ClusterOpModeType;
};

class ACS_CS_API_NeHandling
{
    public:

        // Timers section

        static bool startupProfileSupervisionComplete();

        static bool cancelClusterOpModeSupervisionTimer();

        static bool cancelOmProfileSupervisionTimer(ACS_CS_API_OmProfilePhase::PhaseValue phase);

        static bool setOmProfileSupervisionTimer (ACS_CS_API_OmProfilePhase::PhaseValue phase, int timeout);


        // Subscription section

        static int getNeSubscriberCount();

        static void setNeSubscriberCount(int subscribers);

        static unsigned int getPhaseSubscriberCount (ACS_CS_API_OmProfilePhase::PhaseValue phase);

        static bool setPhaseSubscriberCount (ACS_CS_API_NE_NS::SetPhaseSubscriberData &phaseData);

        static bool updatePhaseChange (ACS_CS_API_OmProfileChange &newProfile);


        // Table handling section

        static ACS_CS_Table* loadNeTable ();

    protected:

        ACS_CS_API_NeHandling() {}

};

#endif // ACS_CS_API_NeHandling_h
