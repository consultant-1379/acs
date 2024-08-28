

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetBSOMIPAddressResponse_h
#define ACS_CS_NE_GetBSOMIPAddressResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetBSOMIPAddressResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45CC3AB9025E
{

  public:
      ACS_CS_NE_GetBSOMIPAddressResponse();

      ACS_CS_NE_GetBSOMIPAddressResponse(const ACS_CS_NE_GetBSOMIPAddressResponse &right);

      virtual ~ACS_CS_NE_GetBSOMIPAddressResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      int getBSOMIPAddress (uint32_t &addressEthA, uint32_t &addressEthB) const;

      void setBSOMIPAddress (uint32_t addressEthA, uint32_t addressEthB);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetBSOMIPAddressResponse & operator=(const ACS_CS_NE_GetBSOMIPAddressResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       uint32_t ipEthA;

       uint32_t ipEthB;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetBSOMIPAddressResponse 



#endif
