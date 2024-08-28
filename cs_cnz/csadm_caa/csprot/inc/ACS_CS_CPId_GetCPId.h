

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPId_h
#define ACS_CS_CPId_GetCPId_h 1



#include <string>


// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetCPId : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C86C1B033D
{

  public:
      ACS_CS_CPId_GetCPId();

      ACS_CS_CPId_GetCPId(const ACS_CS_CPId_GetCPId &right);

      virtual ~ACS_CS_CPId_GetCPId();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setName (std::string name);

      std::string getName () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPId & operator=(const ACS_CS_CPId_GetCPId &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::string nameStr;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPId 



#endif
