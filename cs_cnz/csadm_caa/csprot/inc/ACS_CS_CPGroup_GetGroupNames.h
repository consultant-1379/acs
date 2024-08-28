

//	� Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPGroup_GetGroupNames_h
#define ACS_CS_CPGroup_GetGroupNames_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"



class ACS_CS_HostNetworkConverter;



class ACS_CS_CPGroup_GetGroupNames : public ACS_CS_Primitive  //## Inherits: <unnamed>%45DD3CB40058
{

  public:
      ACS_CS_CPGroup_GetGroupNames();

      ACS_CS_CPGroup_GetGroupNames(const ACS_CS_CPGroup_GetGroupNames &right);

      virtual ~ACS_CS_CPGroup_GetGroupNames();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPGroup_GetGroupNames & operator=(const ACS_CS_CPGroup_GetGroupNames &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_CPGroup_GetGroupNames 



#endif
