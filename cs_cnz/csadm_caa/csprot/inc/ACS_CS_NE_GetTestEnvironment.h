

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetTestEnvironment_h
#define ACS_CS_NE_GetTestEnvironment_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetTestEnvironment : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%45DC5AA00204
{

  public:
      ACS_CS_NE_GetTestEnvironment();

      ACS_CS_NE_GetTestEnvironment(const ACS_CS_NE_GetTestEnvironment &right);

      virtual ~ACS_CS_NE_GetTestEnvironment();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetTestEnvironment & operator=(const ACS_CS_NE_GetTestEnvironment &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetTestEnvironment 



#endif
