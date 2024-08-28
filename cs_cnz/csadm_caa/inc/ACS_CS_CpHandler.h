/*
 * ACS_CS_CpHandler.h
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_CPHANDLER_H_
#define ACS_CS_CPHANDLER_H_
#include <iostream>
#include "ACS_CS_ImmIncomingHandler.h"

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"



class ACS_CS_CpHandler : public ACS_CS_ImmIncomingHandler
{
public:

	ACS_CS_CpHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	virtual ~ACS_CS_CpHandler();

	virtual int create();
	virtual int remove();
	virtual int modify();

	int createClusterCp();
	int createDualSidedCp();

	int removeClusterCp();
	int removeDualSidedCp();


private:

	std::string getComputeResourceDN(const ACS_CS_ImCpBlade*) const;
	std::string getRoleLabel(const ACS_CS_ImCpBlade*) const;

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

};

#endif /* ACS_CS_CPHANDLER_H_ */
