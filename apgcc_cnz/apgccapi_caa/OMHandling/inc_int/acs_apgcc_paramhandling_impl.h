/*
 * acs_apgcc_paramhandling_impl.h
 *
 *  Created on: Aug 22, 2011
 *      Author: xlucpet
 */

#ifndef ACS_APGCC_PARAMHANDLING_IMPL_H_
#define ACS_APGCC_PARAMHANDLING_IMPL_H_

#include "ACS_CC_Types.h"
#include <string.h>

using namespace std;

class acs_apgcc_paramhandling_impl {
public:
	acs_apgcc_paramhandling_impl();
	virtual ~acs_apgcc_paramhandling_impl();

	void setInternalError(int p_errorCode);

private:
	int errorCode;

};

#endif /* ACS_APGCC_PARAMHANDLING_IMPL_H_ */
