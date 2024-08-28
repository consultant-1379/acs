

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANTableResponse_h
#define ACS_CS_VLAN_GetVLANTableResponse_h 1



#include "ACS_CS_Util.h"
#include <vector>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"
#include "ACS_CS_VLAN_GetVLANListResponse.h"

struct ACS_CS_VLAN_DATA;


class ACS_CS_VLAN_GetVLANTableResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%461B3FF10332
{

public:
	ACS_CS_VLAN_GetVLANTableResponse();

	ACS_CS_VLAN_GetVLANTableResponse(const ACS_CS_VLAN_GetVLANTableResponse &right);

	virtual ~ACS_CS_VLAN_GetVLANTableResponse();


	ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

	int setBuffer (const char *buffer, int size);

	int getBuffer (char *buffer, int size) const;

	unsigned short getLength () const;

	ACS_CS_Primitive * clone () const;

	void getVLANVector (std::vector<ACS_CS_VLAN_DATA> &vlanVector) const;

	void setVLANVector (std::vector<ACS_CS_VLAN_DATA> vlanVector);

	unsigned short getLogicalClock () const;

	void setLogicalClock (unsigned short clock);

	// Additional Public Declarations

	unsigned short getTableVersion();

	void setTableVersion(unsigned short ver);

	// GEP2_GEP5 to GEP7 HWSWAP
    unsigned short getCPVersion();
      
    void setCPVersion(unsigned short ver);

protected:
	// Additional Protected Declarations

private:
	ACS_CS_VLAN_GetVLANTableResponse & operator=(const ACS_CS_VLAN_GetVLANTableResponse &right);

	// Additional Private Declarations

public: //## implementation
	// Data Members for Class Attributes

	std::vector<ACS_CS_VLAN_DATA> vlanData;

	short unsigned length;

	short unsigned logicalClock;

	unsigned short tableVersion;

	// GEP2_GEP5 to GEP7 HWSWAP

    unsigned short cpVersion;
	// Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANTableResponse 



#endif
