/*
 * AdminOperation_MyImplementer.h
 *
 *  Created on: Jul 8, 2011
 *      Author: xpaomaz
 */

#ifndef ADMINOPERATIONASYNC_MYIMPLEMENTER_H_
#define ADMINOPERATIONASYNC_MYIMPLEMENTER_H_

#include "acs_apgcc_adminoperationasync.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"

#include <iostream>

class AdminOperationAsync_MyImplementer: public acs_apgcc_adminoperationasync {
public:

	AdminOperationAsync_MyImplementer();

	inline ~AdminOperationAsync_MyImplementer(){};

	/*the callback*/
	void objectManagerAdminOperationCallback(ACS_APGCC_InvocationType invocation,int returnVal ,int error);


};

#endif /* ADMINOPERATIONASYNC_MYIMPLEMENTER_H_ */
