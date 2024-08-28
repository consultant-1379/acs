

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPName_h
#define ACS_CS_CPId_GetCPName_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetCPName : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C86C2102C0
{

  public:
      ACS_CS_CPId_GetCPName();

      ACS_CS_CPId_GetCPName(const ACS_CS_CPId_GetCPName &right);

      virtual ~ACS_CS_CPId_GetCPName();


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
      ACS_CS_CPId_GetCPName & operator=(const ACS_CS_CPId_GetCPName &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned cpId;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPName 



#endif
