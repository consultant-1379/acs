/*
 * ACS_CS_ApHandler.h
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_APHANDLER_H_
#define ACS_CS_APHANDLER_H_
#include "ACS_CS_ImmIncomingHandler.h"
#include <iostream>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"


class ACS_CS_ApHandler : public ACS_CS_ImmIncomingHandler {
public:

	ACS_CS_ApHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
				ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
				ACS_APGCC_CcbHandle &ccbHandleVal,
				ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);


	virtual ~ACS_CS_ApHandler();

	int modify();
	int create();
	int remove();

private:

	std::string getComputeResourceDN(const ACS_CS_ImApBlade* blade) const;

	std::string getRoleLabel(const ACS_CS_ImApBlade* blade) const;

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;


};


#endif /* ACS_CS_APHANDLER_H_ */
