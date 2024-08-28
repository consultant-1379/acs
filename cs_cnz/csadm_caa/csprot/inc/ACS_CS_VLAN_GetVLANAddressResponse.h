

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANAddressResponse_h
#define ACS_CS_VLAN_GetVLANAddressResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_VLAN_GetVLANAddressResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%461B3FF400E0
{

  public:
      ACS_CS_VLAN_GetVLANAddressResponse();

      ACS_CS_VLAN_GetVLANAddressResponse(const ACS_CS_VLAN_GetVLANAddressResponse &right);

      virtual ~ACS_CS_VLAN_GetVLANAddressResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      uint32_t getAddress () const;

      void setAddress (uint32_t address);

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VLAN_GetVLANAddressResponse & operator=(const ACS_CS_VLAN_GetVLANAddressResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

      uint32_t ipAddress;

       short unsigned logicalClock;

    // Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANAddressResponse 



#endif
