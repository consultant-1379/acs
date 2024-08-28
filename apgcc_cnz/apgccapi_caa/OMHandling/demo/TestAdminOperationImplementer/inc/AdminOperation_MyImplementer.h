/*
 * AdminOperation_MyImplementer.h
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#ifndef ADMINOPERATION_MYIMPLEMENTER_H_
#define ADMINOPERATION_MYIMPLEMENTER_H_
#include "acs_apgcc_adminoperationimplementer.h"

#include "ACS_CC_Types.h"

#include "acs_apgcc_adminoperationtypes.h"

#include <iostream>
class AdminOperation_MyImplementer: public acs_apgcc_adminoperationimplementer {
public:

	AdminOperation_MyImplementer();

	inline ~AdminOperation_MyImplementer(){};

	/*the callback*/

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList);
};

#endif /* ADMINOPERATION_MYIMPLEMENTER_H_ */
