/*
 * ACS_APGCC_AdminOperationTypes.h
 *
 *  Created on: Jul 7, 2011
 *      Author: xpaomaz
 */

#ifndef ACS_APGCC_ADMINOPERATIONTYPES_H_
#define ACS_APGCC_ADMINOPERATIONTYPES_H_
#include "saImm.h"
#include "ACS_CC_Types.h"
#include <string.h>

/**Define the wrapper for SaImmContinuationIdT the base type is SaUint64T*/
typedef SaImmContinuationIdT ACS_APGCC_ContinuationIdType;

/*Define the wrapper for SaImmAdminOperationIdTthe base type is SaUint64T;*/
typedef SaImmAdminOperationIdT ACS_APGCC_AdminOperationIdType;

/*Define the wrapper for SaInvocationT the base type is SaUint64T;*/
typedef SaInvocationT ACS_APGCC_InvocationType;

/*Define the wrapper for SaImmAdminOperationParamsT*/
typedef struct {
	char* attrName;
	ACS_CC_AttrValueType attrType;
	void* attrValues;
}ACS_APGCC_AdminOperationParamType;
#endif /* ACS_APGCC_ADMINOPERATIONTYPES_H_ */
