/*
 * ACS_CS_API_CpTableChange_R2.h
 *
 *  Created on: Aug 6, 2014
 *      Author: estevol
 */

#ifndef ACS_CS_API_CPTABLECHANGE_R2_H_
#define ACS_CS_API_CPTABLECHANGE_R2_H_

#include "ACS_CS_API_CpTableChange_R1.h"

struct ACS_CS_API_CpTableData_R2: ACS_CS_API_CpTableData_R1
{
	ACS_CS_API_NS::MauType mauType;
};

struct ACS_CS_API_CpTableChange_R2
{
      size_t dataSize;
      ACS_CS_API_CpTableData_R2* cpData;
};

#endif /* ACS_CS_API_CPTABLECHANGE_R2_H_ */
