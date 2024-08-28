

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_VLAN_GetVLANAddress_h
#define ACS_CS_VLAN_GetVLANAddress_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_VLAN_GetVLANAddress : public ACS_CS_Primitive  //## Inherits: <unnamed>%461B3FFC0044
{

  public:
      ACS_CS_VLAN_GetVLANAddress();

      ACS_CS_VLAN_GetVLANAddress(const ACS_CS_VLAN_GetVLANAddress &right);

      virtual ~ACS_CS_VLAN_GetVLANAddress();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setBoardId (unsigned short identifier);

      void setVLANName (std::string name);

      unsigned short getBoardId () const;

      std::string getVLANName () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_VLAN_GetVLANAddress & operator=(const ACS_CS_VLAN_GetVLANAddress &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned boardId;

       std::string vlanName;

    // Additional Implementation Declarations

};


// Class ACS_CS_VLAN_GetVLANAddress 



#endif
