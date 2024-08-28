//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_VLANHandler_h
#define ACS_CS_VLANHandler_h 1

#include "ACS_CS_TableHandler.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_API.h"

#include "ACS_CS_NetworkDefinitions.h"

#include "ace/RW_Mutex.h"

class ACS_CS_Table;
class ACS_CS_TableEntry;

using std::string;

// Struct to keep track of a VLAN
struct ACS_CS_VLAN_DATA;




typedef std::vector<ACS_CS_VLAN_DATA> vlanVector;
typedef std::map<unsigned short, unsigned long> usULMap;

//	This is a class that handles the VLAN table. All
//	requests about this table are sent to an instance of
//	this class. The class manages the table when entries are
//	added and removed and saves the table to disk when it
//	has changed.



class ACS_CS_VLANHandler : public ACS_CS_TableHandler
{

public:

	ACS_CS_VLANHandler();

	virtual ~ACS_CS_VLANHandler();

	virtual int handleRequest (ACS_CS_PDU *pdu);

	virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

	virtual bool loadTable ();

protected:

	int handleGetVLANAddress (ACS_CS_PDU *pdu);

	int handleGetVLANList (ACS_CS_PDU *pdu);

	int handleGetVLANTable(ACS_CS_PDU *pdu);

	int handleGetNetworkTable(ACS_CS_PDU *pdu);

	short unsigned logicalClock;

private:

	ACS_CS_VLANHandler(const ACS_CS_VLANHandler &right);

	ACS_CS_VLANHandler & operator=(const ACS_CS_VLANHandler &right);

	bool extractIPAddress (const std::string &ipAddressString, uint32_t &ipAddress);

	bool validateIPAddress (uint32_t ipAddress, uint32_t ipNetmask);

	bool validateIPNetmask (uint32_t ipNetmask);

	int createBasicResponse (ACS_CS_PDU *pdu, ACS_CS_Protocol::CS_Primitive_Identifier type, uint16_t requestId, ACS_CS_Protocol::CS_Result_Code result);



	bool getVLANAddressM(BoardID boardId, std::string vlanName, std::string &ipAddress);


	virtual bool populateVLANTable(vlanVector& vlanTable, int version, Network::Domain = Network::DOMAIN_NR) = 0;

	Network::Domain getNetworkDomain(BoardID);

	//vlanVector *vlanTable;

	usULMap boardIdLastByteMap;

	ACE_RW_Mutex lock_;

	int shelfArchitecture;

};

#endif
