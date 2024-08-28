



#ifndef ACS_CS_NE_GetTrafficIsolated_h
#define ACS_CS_NE_GetTrafficIsolated_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetTrafficIsolated : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%4A8D887200E3
{

  public:
      ACS_CS_NE_GetTrafficIsolated();

      virtual ~ACS_CS_NE_GetTrafficIsolated();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTrafficIsolated(const ACS_CS_NE_GetTrafficIsolated &right);

      ACS_CS_NE_GetTrafficIsolated & operator=(const ACS_CS_NE_GetTrafficIsolated &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTrafficIsolated 



#endif
