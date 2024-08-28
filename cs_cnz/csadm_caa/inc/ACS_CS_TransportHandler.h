/*
 * ACS_CS_TransportHandler.h
 *
 *  Created on: Feb 2, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_TRANSPORTHANDLER_H_
#define CSADM_CAA_INC_ACS_CS_TRANSPORTHANDLER_H_

#include "ACS_CS_ImmIncomingHandler.h"
class ACS_CS_ImBase;

class ACS_CS_TransportHandler: public ACS_CS_ImmIncomingHandler
{
public:

	ACS_CS_TransportHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	virtual ~ACS_CS_TransportHandler();

	virtual int create();
	virtual int remove();
	virtual int modify();

private:

	template<class ImNetwork_t> void augmentCrmNetwork(const ACS_CS_ImBase *);

	void augmentTrmNetwork();

	void augmentCrmNetworkFromInterface();

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	const char *className;
	const char *parentName;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;
};

#endif /* CSADM_CAA_INC_ACS_CS_TRANSPORTHANDLER_H_ */
