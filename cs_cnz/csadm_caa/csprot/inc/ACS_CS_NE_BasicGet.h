

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_BasicGet_h
#define ACS_CS_NE_BasicGet_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_NE_BasicGet : public ACS_CS_Primitive  //## Inherits: <unnamed>%45CC37CB00E7
{

  public:
      ACS_CS_NE_BasicGet();

      virtual ~ACS_CS_NE_BasicGet();


      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual unsigned short getLength () const;

    // Additional Public Declarations

  protected:
      ACS_CS_NE_BasicGet(const ACS_CS_NE_BasicGet &right);

    // Additional Protected Declarations

  private:
      ACS_CS_NE_BasicGet & operator=(const ACS_CS_NE_BasicGet &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_NE_BasicGet 



#endif
