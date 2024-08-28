

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"


// ACS_CS_VLAN_GetVLANTable
#include "ACS_CS_VLAN_GetVLANTable.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetVLANTable_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_VLAN_GetVLANTable 

ACS_CS_VLAN_GetVLANTable::ACS_CS_VLAN_GetVLANTable()
      : vlanTableVersion(0)
{
}

ACS_CS_VLAN_GetVLANTable::ACS_CS_VLAN_GetVLANTable(const ACS_CS_VLAN_GetVLANTable &right)
  : ACS_CS_Primitive(right)
{

   this->vlanTableVersion = right.vlanTableVersion;
 }


ACS_CS_VLAN_GetVLANTable::~ACS_CS_VLAN_GetVLANTable()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetVLANTable::getPrimitiveType () const
{

   return ACS_CS_Protocol::Primitive_GetVLANTable;

}

 int ACS_CS_VLAN_GetVLANTable::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)								// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANTable::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size < 2 )	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANTable::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Get and convert Ap identifier
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
        this->vlanTableVersion = ntohs(tempShort);
   
        ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANTable::setBuffer()\n"
			"vlanTableVersion=%d\n", vlanTableVersion));
       
           if (! CS_ProtocolChecker::checkVlanTableVersion(vlanTableVersion))
           {
                ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
                        "(%t) ACS_CS_VLAN_GetVLANTable::setBuffer()\n"
                        "Error: Illegal VLAN table version , version = %d\n",
                        vlanTableVersion ));

                // Some error code
                ACS_CS_AttributeException ex;
                ex.setDescription("Error casting VLAN table version");
                throw ex;
           }

    return 0;

}

 int ACS_CS_VLAN_GetVLANTable::getBuffer (char *buffer, int size) const
{

   if (buffer == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANTable::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
   else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetVLANTable_TRACE,
			"(%t) ACS_CS_VLAN_GetVLANTable::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

   // Convert AP identifier to network byte order and copy it
   unsigned short tempShort = htons(this->vlanTableVersion);
   memcpy(buffer, &tempShort, 2);
   
   return 0;

}

 short unsigned ACS_CS_VLAN_GetVLANTable::getLength () const
{
    return (static_cast<unsigned short>(sizeof(this->vlanTableVersion)) );
}

 ACS_CS_Primitive * ACS_CS_VLAN_GetVLANTable::clone () const
{

   return new ACS_CS_VLAN_GetVLANTable(*this);

}

 void ACS_CS_VLAN_GetVLANTable::setVlanTableVersion (unsigned short identifier)
{

   this->vlanTableVersion = identifier;

}



 short unsigned ACS_CS_VLAN_GetVLANTable::getVlanTableVersion () const
{

   return this->vlanTableVersion;

}



// Additional Declarations

