

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetBSOMIPAddress_h
#define ACS_CS_NE_GetBSOMIPAddress_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetBSOMIPAddress : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%45CC38FE02EA
{

  public:
      ACS_CS_NE_GetBSOMIPAddress();

      ACS_CS_NE_GetBSOMIPAddress(const ACS_CS_NE_GetBSOMIPAddress &right);

      virtual ~ACS_CS_NE_GetBSOMIPAddress();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetBSOMIPAddress & operator=(const ACS_CS_NE_GetBSOMIPAddress &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetBSOMIPAddress 



#endif
