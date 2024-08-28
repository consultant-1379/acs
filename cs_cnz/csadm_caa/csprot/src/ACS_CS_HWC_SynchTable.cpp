

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Trace.h"


// ACS_CS_HWC_SynchTable
#include "ACS_CS_HWC_SynchTable.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_SynchTable_TRACE);



// Class ACS_CS_HWC_SynchTable 

ACS_CS_HWC_SynchTable::ACS_CS_HWC_SynchTable()
{
}

ACS_CS_HWC_SynchTable::ACS_CS_HWC_SynchTable(const ACS_CS_HWC_SynchTable &right)
  : ACS_CS_Primitive(right)
{
}


ACS_CS_HWC_SynchTable::~ACS_CS_HWC_SynchTable()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_SynchTable::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_SynchTable;

}

 int ACS_CS_HWC_SynchTable::setBuffer (const char */*buffer*/, int /*size*/)
{

	return 0;

}

 int ACS_CS_HWC_SynchTable::getBuffer (char */*buffer*/, int /*size*/) const
{

	return 0;

}

 short unsigned ACS_CS_HWC_SynchTable::getLength () const
{

	return 0;

}

 ACS_CS_Primitive * ACS_CS_HWC_SynchTable::clone () const
{

   return new ACS_CS_HWC_SynchTable(*this);

}

// Additional Declarations

