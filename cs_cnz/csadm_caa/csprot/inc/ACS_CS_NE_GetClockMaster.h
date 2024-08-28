

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetClockMaster_h
#define ACS_CS_NE_GetClockMaster_h 1



// ACS_CS_NE_BasicGet
#include "ACS_CS_NE_BasicGet.h"







class ACS_CS_NE_GetClockMaster : public ACS_CS_NE_BasicGet  //## Inherits: <unnamed>%45CC38F9025E
{

  public:
      ACS_CS_NE_GetClockMaster();

      ACS_CS_NE_GetClockMaster(const ACS_CS_NE_GetClockMaster &right);

      virtual ~ACS_CS_NE_GetClockMaster();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetClockMaster & operator=(const ACS_CS_NE_GetClockMaster &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetClockMaster 



#endif
