/*
 * @file ACS_CS_API_SetClusterOpModeChanged.h
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

#ifndef ACS_CS_API_SetClusterOpModeChanged_h
#define ACS_CS_API_SetClusterOpModeChanged_h 1

#include "ACS_CS_Table.h"
#include "ACS_CS_API_Set_R1.h"
#include "ACS_CS_API_NeHandling.h"


class ACS_CS_API_SetClusterOpModeChanged
{
    public:

        ACS_CS_API_SET_NS::CS_API_Set_Result setClusterOpMode (ACS_CS_API_ClusterOpMode::Value newClusterOpMode,
        		ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType);

    private:

        bool readClusterOpMode (ACS_CS_Table *table, ACS_CS_API_ClusterOpMode::Value &clusterOpMode);
        bool readClusterOpMode (ACS_CS_API_ClusterOpMode::Value &clusterOpMode);

};


#endif // ACS_CS_API_SetClusterOpModeChanged_h
