/*
 * ACS_TRAPDS_API_R2.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: eanform
 */

#include "ACS_TRAPDS_API_R2.h"


ACS_TRAPDS::ACS_TRAPDS_API_Result ACS_TRAPDS_API_R2::unsubscribe()
{
	ACS_TRAPDS::ACS_TRAPDS_API_Result result = ACS_TRAPDS::Result_Failure;

	if (this->close()==0) return ACS_TRAPDS::Result_Success;
	else
		return result;
}
