

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANTable_h
#define ACS_CS_VLAN_GetVLANTable_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_VLAN_GetVLANTable : public ACS_CS_Primitive  //## Inherits: <unnamed>%461B3FFC0044
{

  public:
      ACS_CS_VLAN_GetVLANTable();

      ACS_CS_VLAN_GetVLANTable(const ACS_CS_VLAN_GetVLANTable &right);

      virtual ~ACS_CS_VLAN_GetVLANTable();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setVlanTableVersion (unsigned short version);

      unsigned short getVlanTableVersion() const;


    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VLAN_GetVLANTable & operator=(const ACS_CS_VLAN_GetVLANTable &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned vlanTableVersion;

  

    // Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANTable 



#endif
