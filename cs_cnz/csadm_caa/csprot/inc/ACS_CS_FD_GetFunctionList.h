

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_FD_GetFunctionList_h
#define ACS_CS_FD_GetFunctionList_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_FD_GetFunctionList : public ACS_CS_Primitive  //## Inherits: <unnamed>%45DDA6A00077
{

  public:
      ACS_CS_FD_GetFunctionList();

      ACS_CS_FD_GetFunctionList(const ACS_CS_FD_GetFunctionList &right);

      virtual ~ACS_CS_FD_GetFunctionList();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_FD_GetFunctionList & operator=(const ACS_CS_FD_GetFunctionList &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_FD_GetFunctionList 



#endif
