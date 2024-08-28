/*
 * acs_apgcc_adminoperationasync_impl.h
 *
 *  Created on: Jul 10, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONASYNC_IMPL_H_
#define ACS_APGCC_ADMINOPERATIONASYNC_IMPL_H_

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include "ACS_CC_Types.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include "ace/OS.h"
#include "ACS_TRA_Logging.h"
#include "acs_apgcc_adminoperationtypes.h"


using namespace std;
class acs_apgcc_adminoperationasync;

class acs_apgcc_adminoperationasync_impl {
public:
	acs_apgcc_adminoperationasync_impl();

	acs_apgcc_adminoperationasync_impl(acs_apgcc_adminoperationasync_impl* obj);

	ACS_CC_ReturnType init ( acs_apgcc_adminoperationasync* p_callBackImpl );

	ACS_CC_ReturnType finalize ();

	ACS_CC_ReturnType adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation ,const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector);

	int getInternalLastError();

	char* getInternalLastErrorText();

	ACS_CC_ReturnType dispatch(ACS_APGCC_DispatchFlags p_flag);

	int getSelObj() const ;

	virtual ~acs_apgcc_adminoperationasync_impl();

private:

	static map<SaUint64T, acs_apgcc_adminoperationasync *> adminOpList;

	acs_apgcc_adminoperationasync *omAdminOp;

	static void objectManagerAdminOperationCallback( SaInvocationT invocation,
			SaAisErrorT operationReturnValue,
			SaAisErrorT error);

	void setInternalError(int p_errorCode);

	SaImmHandleT immHandle;

	SaVersionT version;

	SaImmAdminOwnerHandleT ownerHandle;

	ACS_TRA_Logging log;

	int errorCode;

	SaSelectionObjectT selObj;

	std::string processName;
};

#endif /* ACS_APGCC_ADMINOPERATIONASYNC_IMPL_H_ */
