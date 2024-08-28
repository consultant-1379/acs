

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANList_h
#define ACS_CS_VLAN_GetVLANList_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_VLAN_GetVLANList : public ACS_CS_Primitive  //## Inherits: <unnamed>%461B3FF8015D
{

  public:
      ACS_CS_VLAN_GetVLANList();

      ACS_CS_VLAN_GetVLANList(const ACS_CS_VLAN_GetVLANList &right);

      virtual ~ACS_CS_VLAN_GetVLANList();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VLAN_GetVLANList & operator=(const ACS_CS_VLAN_GetVLANList &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANList 



#endif
