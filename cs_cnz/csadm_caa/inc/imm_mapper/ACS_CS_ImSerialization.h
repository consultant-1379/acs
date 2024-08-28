/*
 * ACS_CS_ImSerialization.h
 *
 *  Created on: Apr 24, 2015
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMSERIALIZATION_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMSERIALIZATION_H_

#include "ACS_CS_ImModel.h"

#include "ACS_CS_ImEquipment.h"

#include "ACS_CS_ImHardwareMgmt.h"
#include "ACS_CS_ImLogicalMgmt.h"

#include "ACS_CS_ImCpCluster.h"
#include "ACS_CS_ImCpClusterStruct.h"
#include "ACS_CS_ImAdvancedConfiguration.h"
#include "ACS_CS_ImClusterCp.h"
#include "ACS_CS_ImDualSidedCp.h"
#include "ACS_CS_ImCpBlade.h"

#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImAp.h"

#include "ACS_CS_ImOmProfile.h"
#include "ACS_CS_ImOmProfileManager.h"
#include "ACS_CS_ImCcFile.h"
#include "ACS_CS_ImCcFileManager.h"
#include "ACS_CS_ImCandidateCcFile.h"
#include "ACS_CS_ImOmProfileStruct.h"
#include "ACS_CS_ImCandidateCcFileStruct.h"
#include "ACS_CS_ImCpProductInfo.h"
#include "ACS_CS_ImCrMgmt.h"
#include "ACS_CS_ImComputeResource.h"
#include "ACS_CS_ImComputeResourceNetwork.h"
#include "ACS_CS_ImIplbCluster.h"

#include "ACS_CS_ImApService.h"
#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImFunctionDistribution.h"
#include "ACS_CS_ImApg.h"
#include "ACS_CS_ImFunction.h"

#include "ACS_CS_ImCrmEquipment.h"
#include "ACS_CS_ImCrmComputeResource.h"
#include "ACS_CS_ImExternalNetwork.h"

#include "ACS_CS_ImTransport.h"
#include "ACS_CS_ImHost.h"
#include "ACS_CS_ImInternalNetwork.h"
#include "ACS_CS_ImInterface.h"

//IMPORTANT NOTE: This class shall be included ONLY from cpp files using serialization on model objects.

// versions of boost library
// 1  - initial version
// 7  - Boost 1.42 2 Feb 2010
// 8  - Boost 1.44
// 10 - fixed base64 output/input.
static const int BOOST_ARCHIVE_VERSION = 10; //Boost library archive version

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMSERIALIZATION_H_ */
