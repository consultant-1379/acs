

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPAliasName_h
#define ACS_CS_CPId_GetCPAliasName_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetCPAliasName : public ACS_CS_Primitive  //## Inherits: <unnamed>%473576FC02F4
{

  public:
      ACS_CS_CPId_GetCPAliasName();

      ACS_CS_CPId_GetCPAliasName(const ACS_CS_CPId_GetCPAliasName &right);

      virtual ~ACS_CS_CPId_GetCPAliasName();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setCPId (unsigned short identifier);

      unsigned short getCPId () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPAliasName & operator=(const ACS_CS_CPId_GetCPAliasName &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned cpId;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPAliasName 



#endif
