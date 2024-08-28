
#ifndef ACS_CS_CPCLUSTERHANDLER_H1_
#define ACS_CS_CPCLUSTERHANDLER_H1_

#include "ACS_CS_ImmIncomingHandler.h"

//#include "ACS_CS_ImBase.h"
//#include "ACS_APGCC_OiHandler.h"
#include <iostream>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImCpClusterStruct.h"


class ACS_CS_CpClusterHandler : public ACS_CS_ImmIncomingHandler
{
public:

	ACS_CS_CpClusterHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	virtual ~ACS_CS_CpClusterHandler();


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

#endif /* ACS_CS_CPCLUSTERHANDLER_H1_ */
