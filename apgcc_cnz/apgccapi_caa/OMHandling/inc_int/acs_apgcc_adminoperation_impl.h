/*
 * acs_apgcc_adminoperation_impl.h
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATION_IMPL_H_
#define ACS_APGCC_ADMINOPERATION_IMPL_H_

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

class acs_apgcc_adminoperation_impl {
public:
	acs_apgcc_adminoperation_impl();

	acs_apgcc_adminoperation_impl(acs_apgcc_adminoperation_impl* obj);


	virtual ~acs_apgcc_adminoperation_impl();

	ACS_CC_ReturnType init ( );

	ACS_CC_ReturnType finalize ();

	ACS_CC_ReturnType adminOperationInvoke ( const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector, int* returnVal, long long int timeoutVal );

	int getInternalLastError();

	char* getInternalLastErrorText();

	ACS_CC_ReturnType adminOperationInvoke ( const char* p_objName, ACS_APGCC_ContinuationIdType continuationId, ACS_APGCC_AdminOperationIdType operationId, vector<ACS_APGCC_AdminOperationParamType> paramVector, int* returnVal, long long int timeoutVal, vector<ACS_APGCC_AdminOperationParamType> &outparamVector);

	ACS_CC_ReturnType freeadminOperationReturnValue ( vector<ACS_APGCC_AdminOperationParamType> &outparamVector );

private:

	void setInternalError(int p_errorCode);

	SaImmHandleT immHandle;

	SaVersionT version;

	SaImmAdminOwnerHandleT ownerHandle;

	ACS_TRA_Logging log;

	int errorCode;

	std::string processName;

};

#endif /* ACS_APGCC_ADMINOPERATION_IMPL_H_ */
