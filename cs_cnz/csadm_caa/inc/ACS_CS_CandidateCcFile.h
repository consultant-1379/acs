/*
 * ACS_CS_CandidateCcFile.h
 *
 *  Created on: Apr 15, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_CANDIDATECCFILE_H_
#define ACS_CS_CANDIDATECCFILE_H_


#include "ACS_CS_ImmIncomingHandler.h"
#include <iostream>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"


class ACS_CS_CandidateCcFile : public ACS_CS_ImmIncomingHandler
{
public:

	ACS_CS_CandidateCcFile(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	virtual ~ACS_CS_CandidateCcFile();


	int modify();
	int create();
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



#endif /* ACS_CS_CANDIDATECCFILE_H_ */
