

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_VLAN_GetNetworkTable
#include "ACS_CS_VLAN_GetNetworkTable.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetNetworkTable_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_VLAN_GetNetworkTable

ACS_CS_VLAN_GetNetworkTable::ACS_CS_VLAN_GetNetworkTable()
      : tableVersion(0),
		boardId(0)
{
}

ACS_CS_VLAN_GetNetworkTable::ACS_CS_VLAN_GetNetworkTable(const ACS_CS_VLAN_GetNetworkTable &right)
  : ACS_CS_Primitive(right)
{

   tableVersion = right.tableVersion;
   boardId = right.boardId;
 }


ACS_CS_VLAN_GetNetworkTable::~ACS_CS_VLAN_GetNetworkTable()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetNetworkTable::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetNetworkTable;

}

 int ACS_CS_VLAN_GetNetworkTable::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTable::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 2 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTable::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert Ap identifier
   unsigned short offset = 0;

   unsigned short tempShort = 0;
   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
   tableVersion = ntohs(tempShort);
   offset += sizeof(tempShort);

   ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
		   "(%t) ACS_CS_VLAN_GetNetworkTable::setBuffer()\n"
		   "vlanTableVersion=%d\n", tableVersion));

   if (! CS_ProtocolChecker::checkVlanTableVersion(tableVersion))
   {
	   ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
			   "(%t) ACS_CS_VLAN_GetNetworkTable::setBuffer()\n"
			   "Error: Illegal VLAN table version , version = %d\n",
			   tableVersion ));

	   // Some error code
	   ACS_CS_AttributeException ex;
	   ex.setDescription("Error casting VLAN table version");
	   throw ex;
   }

   tempShort = *( reinterpret_cast<const unsigned short *>(buffer + offset) );
   boardId = ntohs(tempShort);

   ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
		   "(%t) ACS_CS_VLAN_GetNetworkTable::setBuffer()\n"
		   "boardId=%d\n", boardId));

   return 0;

}

 int ACS_CS_VLAN_GetNetworkTable::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTable::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTable_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTable::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Convert AP identifier to network byte order and copy it
   unsigned short offset = 0;

   unsigned short tempShort = htons(tableVersion);
   memcpy(buffer + offset, &tempShort, sizeof(tempShort));
   offset += sizeof(tempShort);


   tempShort = htons(boardId);
   memcpy(buffer + offset, &tempShort, sizeof(tempShort));

   return 0;

}

 short unsigned ACS_CS_VLAN_GetNetworkTable::getLength () const
{
    return (static_cast<unsigned short>(sizeof(tableVersion) + sizeof(boardId)) );
}

 ACS_CS_Primitive * ACS_CS_VLAN_GetNetworkTable::clone () const
{

   return new ACS_CS_VLAN_GetNetworkTable(*this);

}

 void ACS_CS_VLAN_GetNetworkTable::setTableVersion (unsigned short version)
{

   tableVersion = version;

}

 short unsigned ACS_CS_VLAN_GetNetworkTable::getTableVersion () const
{
   return tableVersion;
}

 void ACS_CS_VLAN_GetNetworkTable::setBoardId (unsigned short identifier)
 {
	 boardId = identifier;

 }

 unsigned short ACS_CS_VLAN_GetNetworkTable::getBoardId () const
 {
	 return boardId;
 }


// Additional Declarations

