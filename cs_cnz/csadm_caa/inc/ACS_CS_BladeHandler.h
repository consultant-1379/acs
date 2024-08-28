/*
 * ACS_CS_BladeHandler.h
 *
 *  Created on: 07/mag/2012
 *      Author: renato
 */

#ifndef ACS_CS_BLADEHANDLER_H_
#define ACS_CS_BLADEHANDLER_H_

#include "ACS_CS_ImmIncomingHandler.h"
#include <iostream>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImIPAssignmentHelper.h"

class ACS_CS_BladeHandler : public ACS_CS_ImmIncomingHandler {

public:

	ACS_CS_BladeHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

    
	virtual ~ACS_CS_BladeHandler();

	int create();

	int modify();

	int remove();

	int otherBladeCreate();

	int cpBladeCreate();

	int apBladeCreate();


private:

//	ACS_CC_ReturnType figureOutEthaEthbAddress(std::string &etha,std::string &ethb,ACS_APGCC_CcbId ccbId);
//	int  getMaxAddressFromPreviousBoard(std::string &ethaM,std::string &ethbM,ACS_APGCC_CcbId ccbId );
	int  fetchDefaulInfo(std::string &outProductId,std::string &outPath,int boardtype);
	int  fetchCurrentLoadModuleInfo(std::string cxp_,std::string &outPackageId,std::string &oPath,std::string boardtype);
	int  fetchDefaultSwPackageDn(std::string &dn,int boardtype);
	bool checkIpaddress(ACS_CS_ImModel *model,uint32_t iptobeChecked);
	void assignDHCPOption(ACS_CS_ImBlade*, int = 4);
	ACS_CC_ReturnType storeStaticIPToImm(ACS_CS_ImBlade* blade, ACS_CS_ImModel *model);
	bool checkComputeResourceBlade(const std::string &uuid);
	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

};





#endif /* ACS_CS_BLADEHANDLER_H_ */
