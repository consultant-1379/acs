/*
 * ACS_CS_NativeVLANHandler.h
 *
 *  Created on: Jan 31, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_NATIVEVLANHANDLER_H_
#define CSADM_CAA_INC_ACS_CS_NATIVEVLANHANDLER_H_

#include "ACS_CS_VLANHandler.h"
#include "ACS_CS_Internal_Table.h"

class ACS_CS_NativeVLANHandler: public ACS_CS_VLANHandler
{
public:
	ACS_CS_NativeVLANHandler();

	virtual ~ACS_CS_NativeVLANHandler();

private:
	virtual bool populateVLANTable(vlanVector& vlanTable,int version, Network::Domain = Network::DOMAIN_NR);


	bool getVLANList(std::vector<ACS_CS_INTERNAL_API::VLAN> &vlanList);

	bool convertTostruct(ACS_CS_VLAN_DATA & vlanStruct, const ACS_CS_INTERNAL_API::VLAN & vlanParameter);
};

#endif /* CSADM_CAA_INC_ACS_CS_NATIVEVLANHANDLER_H_ */
