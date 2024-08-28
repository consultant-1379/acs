

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_GetBoardList_h
#define ACS_CS_HWC_GetBoardList_h 1



// ACS_CS_HWC_MultiEntry
#include "ACS_CS_HWC_MultiEntry.h"







class ACS_CS_HWC_GetBoardList : public ACS_CS_HWC_MultiEntry  //## Inherits: <unnamed>%45C85273004F
{

  public:
      ACS_CS_HWC_GetBoardList();

      ACS_CS_HWC_GetBoardList(const ACS_CS_HWC_GetBoardList &right);

      virtual ~ACS_CS_HWC_GetBoardList();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_HWC_GetBoardList & operator=(const ACS_CS_HWC_GetBoardList &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_HWC_GetBoardList 



#endif
