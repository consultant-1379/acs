/*
 * acs_apgcc_adminoperationasync_V2_impl.h
 *
 *  Created on: Nov 15, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONASYNC_V2_IMPL_H_
#define ACS_APGCC_ADMINOPERATIONASYNC_V2_IMPL_H_

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
class acs_apgcc_adminoperationasync_V2_R1;

class acs_apgcc_adminoperationasync_V2_R1_impl {
public:
	acs_apgcc_adminoperationasync_V2_R1_impl();

	acs_apgcc_adminoperationasync_V2_R1_impl(acs_apgcc_adminoperationasync_V2_R1_impl* obj);

	ACS_CC_ReturnType init ( acs_apgcc_adminoperationasync_V2_R1* p_callBackImpl );

	ACS_CC_ReturnType finalize ();

	ACS_CC_ReturnType adminOperationInvokeAsync ( ACS_APGCC_InvocationType invocation ,const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector);

	int getInternalLastError();

	char* getInternalLastErrorText();

	ACS_CC_ReturnType dispatch(ACS_APGCC_DispatchFlags p_flag);

	int getSelObj() const ;

	virtual ~acs_apgcc_adminoperationasync_V2_R1_impl();

private:

	static map<SaUint64T, acs_apgcc_adminoperationasync_V2_R1 *> adminOpList;

	acs_apgcc_adminoperationasync_V2_R1 *omAdminOp;

	static void objectManagerAdminOperationCallback( SaInvocationT invocation,
			SaAisErrorT operationReturnValue,
			SaAisErrorT error,
			const SaImmAdminOperationParamsT_2 **outParamVector );

	void setInternalError(int p_errorCode);

	SaImmHandleT immHandle;

	SaVersionT version;

	SaImmAdminOwnerHandleT ownerHandle;

	ACS_TRA_Logging log;

	int errorCode;

	SaSelectionObjectT selObj;

	std::string processName;
};

#endif /* acs_apgcc_adminoperationasync_V2_R1_IMPL_H_ */
