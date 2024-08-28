

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPList_h
#define ACS_CS_CPId_GetCPList_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetCPList : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C86C2E030E
{

  public:
      ACS_CS_CPId_GetCPList();

      ACS_CS_CPId_GetCPList(const ACS_CS_CPId_GetCPList &right);

      virtual ~ACS_CS_CPId_GetCPList();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPList & operator=(const ACS_CS_CPId_GetCPList &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPList 



#endif
