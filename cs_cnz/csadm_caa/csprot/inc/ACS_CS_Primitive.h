

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_Primitive_h
#define ACS_CS_Primitive_h 1

#include  <arpa/inet.h>

#include "ACS_CS_Protocol.h"









class ACS_CS_Primitive 
{

  public:
      ACS_CS_Primitive();

      virtual ~ACS_CS_Primitive();


      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const = 0;

      virtual int setBuffer (const char *buffer, int size) = 0;

      virtual int getBuffer (char *buffer, int size) const = 0;

      virtual unsigned short getLength () const = 0;

      virtual ACS_CS_Primitive * clone () const = 0;

    // Additional Public Declarations

  protected:
      ACS_CS_Primitive(const ACS_CS_Primitive &right);

    // Additional Protected Declarations

  private:
      ACS_CS_Primitive & operator=(const ACS_CS_Primitive &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Associations

    // Additional Implementation Declarations

};


// Class ACS_CS_Primitive 



#endif
