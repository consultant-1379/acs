

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetAPZSystem_h
#define ACS_CS_CPId_GetAPZSystem_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetAPZSystem : public ACS_CS_Primitive  //## Inherits: <unnamed>%46694F9103E6
{

  public:
      ACS_CS_CPId_GetAPZSystem();

      ACS_CS_CPId_GetAPZSystem(const ACS_CS_CPId_GetAPZSystem &right);

      virtual ~ACS_CS_CPId_GetAPZSystem();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getCPId () const;

      void setCPId (unsigned short identifier);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetAPZSystem & operator=(const ACS_CS_CPId_GetAPZSystem &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned cpId;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetAPZSystem 



#endif
