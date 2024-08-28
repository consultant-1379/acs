



#ifndef ACS_CS_NE_GetTrafficLeaderResponse_h
#define ACS_CS_NE_GetTrafficLeaderResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetTrafficLeaderResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A8D87450097
{

  public:
      ACS_CS_NE_GetTrafficLeaderResponse();

      virtual ~ACS_CS_NE_GetTrafficLeaderResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      unsigned getTrafficLeader () const;

      void setTrafficLeader (unsigned cpId);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTrafficLeaderResponse(const ACS_CS_NE_GetTrafficLeaderResponse &right);

      ACS_CS_NE_GetTrafficLeaderResponse & operator=(const ACS_CS_NE_GetTrafficLeaderResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned trafficLeaderCpId;

    // Data Members for Associations

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTrafficLeaderResponse 



#endif
