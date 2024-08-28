



#ifndef ACS_CS_NE_GetTrafficLeader_h
#define ACS_CS_NE_GetTrafficLeader_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetTrafficLeader : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%4A8D888000E3
{

  public:
      ACS_CS_NE_GetTrafficLeader();

      virtual ~ACS_CS_NE_GetTrafficLeader();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTrafficLeader(const ACS_CS_NE_GetTrafficLeader &right);

      ACS_CS_NE_GetTrafficLeader & operator=(const ACS_CS_NE_GetTrafficLeader &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTrafficLeader 



#endif
