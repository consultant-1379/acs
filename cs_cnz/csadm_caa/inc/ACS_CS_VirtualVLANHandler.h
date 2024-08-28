/*
 * ACS_CS_VirtualVLANHandler.h
 *
 *  Created on: Jan 26, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_VIRTUALVLANHANDLER_H_
#define CSADM_CAA_INC_ACS_CS_VIRTUALVLANHANDLER_H_


#include "ACS_CS_NetworkDefinitions.h"
#include "ACS_CS_VLANHandler.h"


class ACS_CS_Table;
class ACS_CS_TableEntry;

//	This is a class that handles the VLAN table. All
//	requests about this table are sent to an instance of
//	this class. The class manages the table when entries are
//	added and removed and saves the table to disk when it
//	has changed.



class ACS_CS_VirtualVLANHandler : public ACS_CS_VLANHandler
{

public:

	ACS_CS_VirtualVLANHandler();

	virtual ~ACS_CS_VirtualVLANHandler();

//	virtual int handleRequest (ACS_CS_PDU *pdu);

	virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

	virtual bool loadTable ();

private:

	virtual bool populateVLANTable(vlanVector& vlanTable, int tableVersion, Network::Domain = Network::DOMAIN_NR);


	template<class ImNetwork_t> void changeName(const ACS_CS_ImBase* baseObj, Network::Type type);

	template<class ImNetwork_t> void insert(vlanVector & , const ACS_CS_ImBase *, Network::Type type, Network::Domain domain, ACS_CS_ImModel * = 0);

	const ACS_CS_ImVlan* getLegacyVlanObj(const std::string & name, ACS_CS_ImModel * model);

	template<class ImNetwork_t> bool isValidEntry(const ImNetwork_t *, const ACS_CS_ImVlan * = 0);

};


#endif /* CSADM_CAA_INC_ACS_CS_VIRTUALVLANHANDLER_H_ */
