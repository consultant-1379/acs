



#ifndef ACS_CS_NE_GetTrafficIsolatedResponse_h
#define ACS_CS_NE_GetTrafficIsolatedResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetTrafficIsolatedResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A9729860095
{

  public:
      ACS_CS_NE_GetTrafficIsolatedResponse();

      virtual ~ACS_CS_NE_GetTrafficIsolatedResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      unsigned getTrafficIsolated () const;

      void setTrafficIsolated (unsigned cpId);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTrafficIsolatedResponse(const ACS_CS_NE_GetTrafficIsolatedResponse &right);

      ACS_CS_NE_GetTrafficIsolatedResponse & operator=(const ACS_CS_NE_GetTrafficIsolatedResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned trafficIsolatedCpId;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTrafficIsolatedResponse 



#endif
