

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_HWC_GetBoardList
#include "ACS_CS_HWC_GetBoardList.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_GetBoardList_TRACE);



// Class ACS_CS_HWC_GetBoardList 

ACS_CS_HWC_GetBoardList::ACS_CS_HWC_GetBoardList()
{
}

ACS_CS_HWC_GetBoardList::ACS_CS_HWC_GetBoardList(const ACS_CS_HWC_GetBoardList &right)
  : ACS_CS_HWC_MultiEntry(right)
{
}


ACS_CS_HWC_GetBoardList::~ACS_CS_HWC_GetBoardList()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_GetBoardList::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetBoardList;

}

 ACS_CS_Primitive * ACS_CS_HWC_GetBoardList::clone () const
{

   return new ACS_CS_HWC_GetBoardList(*this);

}

// Additional Declarations

