

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANListResponse_h
#define ACS_CS_VLAN_GetVLANListResponse_h 1



#include "ACS_CS_Util.h"
#include <vector>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"


// Struct to keep track of a VLAN
struct ACS_CS_VLAN_DATA
{
   char vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH + 1];
   uint32_t vlanAddress;
   uint32_t vlanNetmask;
   uint16_t vlanStack;
   //qos start
   uint16_t pcp;
   uint16_t vlanType;
   //qos end
   uint16_t vlanTag;
   char vNICName[ACS_CS_NS::MAX_VNIC_NAME_LENGTH + 1];
};







class ACS_CS_VLAN_GetVLANListResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%461B3FF10332
{

  public:
      ACS_CS_VLAN_GetVLANListResponse();

      ACS_CS_VLAN_GetVLANListResponse(const ACS_CS_VLAN_GetVLANListResponse &right);

      virtual ~ACS_CS_VLAN_GetVLANListResponse();


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

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VLAN_GetVLANListResponse & operator=(const ACS_CS_VLAN_GetVLANListResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::vector<ACS_CS_VLAN_DATA> vlanData;

       short unsigned length;

       short unsigned logicalClock;

    // Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANListResponse 



#endif
