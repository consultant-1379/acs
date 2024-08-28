/*
 * ACS_CS_LogicalMgmtHandler.h
 *
 *  Created on: 08/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_LOGICALMGMTHANDLER_H_
#define ACS_CS_LOGICALMGMTHANDLER_H_
#include "ACS_CS_ImmIncomingHandler.h"
#include <iostream>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImLogicalMgmt.h"

class ACS_CS_LogicalMgmtHandler : public ACS_CS_ImmIncomingHandler {

public:

	ACS_CS_LogicalMgmtHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	virtual ~ACS_CS_LogicalMgmtHandler();

	int create();

	int modify();

	int remove();

private:

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

};



#endif /* ACS_CS_LOGICALMGMTHANDLER_H_ */
