/*
 * @file ACS_CS_API_SetQuorumData.h
 * @author xmikhal
 * @date Dec 20, 2010
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

#ifndef ACS_CS_API_SetQuorumData_h
#define ACS_CS_API_SetQuorumData_h 1

#include "ACS_CS_Internal_Table.h"
#include "ACS_CS_API_Set_R1.h"

class ACS_CS_ImModel;
class ACS_CS_ImClusterCp;
class ACS_CS_ImCpCluster;

using namespace ACS_CS_INTERNAL_API;


//namespace ACS_CS_API_SET_NS
//{
//
//typedef struct {
//    IMMHandle_t handle;
//    ACS_CS_IMM_Table *immTable;
//} ACS_CS_API_Set_ImmTable ;
//
//};

class ACS_CS_API_SetQuorumData
{
    public:

        static ACS_CS_API_SET_NS::CS_API_Set_Result setCpData (ACS_CS_ImModel* tmpModel, std::vector<ACS_CS_API_CpData_R1>& cpDataVector);

        static ACS_CS_API_SET_NS::CS_API_Set_Result setTrafficLeader (ACS_CS_ImModel* tmpModel, CPID trafficLeader);

        static ACS_CS_API_SET_NS::CS_API_Set_Result setTrafficIsolated (ACS_CS_ImModel* tmpModel, CPID trafficIsolated);

        static bool getCpIdList (ACS_CS_API_IdList &cpList);

        static ACS_CS_API_CpData_R1* getCpDataByCpId (std::vector<ACS_CS_API_CpData_R1> &cpDataVector, CPID cpId);

    private:

        static int updateCPGroupTable (ACS_CS_ImClusterCp *cp, ACS_CS_ImModel* model, ACS_CS_ImCpCluster* intialCpCluster);

};

#endif // ACS_CS_API_SetQuorumData_h
