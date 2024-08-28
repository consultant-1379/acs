



#ifndef ACS_CS_NE_GetClusterOpMode_h
#define ACS_CS_NE_GetClusterOpMode_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetClusterOpMode : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%4A806A080253
{

  public:
      ACS_CS_NE_GetClusterOpMode();

      virtual ~ACS_CS_NE_GetClusterOpMode();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetClusterOpMode(const ACS_CS_NE_GetClusterOpMode &right);

      ACS_CS_NE_GetClusterOpMode & operator=(const ACS_CS_NE_GetClusterOpMode &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetClusterOpMode 



#endif
