

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"

#include "ACS_CS_Trace.h"


// ACS_CS_HWC_GetValue
#include "ACS_CS_HWC_GetValue.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_GetValue_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_HWC_GetValue 

ACS_CS_HWC_GetValue::ACS_CS_HWC_GetValue()
      : boardId(0),
        attributeId(ACS_CS_Protocol::Attribute_NotSpecified)
{
}

ACS_CS_HWC_GetValue::ACS_CS_HWC_GetValue(const ACS_CS_HWC_GetValue &right)
  : ACS_CS_Primitive(right)
{
	
	// Assign values
	this->attributeId = right.attributeId;
	this->boardId = right.boardId;

}


ACS_CS_HWC_GetValue::~ACS_CS_HWC_GetValue()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_HWC_GetValue::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetValue;

}

 int ACS_CS_HWC_GetValue::getBuffer (char *buffer, int size) const
{

	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValue_TRACE,
			"(%t) ACS_CS_HWC_GetValue::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 4)	// Check for size (2 byte boardId + 2 byte attributeId)
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValue_TRACE,
			"(%t) ACS_CS_HWC_GetValue::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	// Create struct to hold values (and convert to network byte order)
	getValueBuffer temp = { htons(this->boardId),
							htons(this->attributeId) };

	// Copy struct to buffer
	memcpy(buffer, &temp, 4);

	return 0;

}

 int ACS_CS_HWC_GetValue::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValue_TRACE,
			"(%t) ACS_CS_HWC_GetValue::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != 4)	// Check for size
	{
		ACS_CS_TRACE((ACS_CS_HWC_GetValue_TRACE,
			"(%t) ACS_CS_HWC_GetValue::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}

	getValueBuffer temp;	// Create struct to hold values

	// Copy buffer to struct
	memcpy(&temp, buffer, 4);

	// Convert to host byte order
	temp.attributeId = ntohs(temp.attributeId);
	temp.boardId = ntohs(temp.boardId);

	// Check for valid attribute and cast to HWC_Attribute_Identifier

	this->boardId = temp.boardId;

	// Check for valid attribute and cast it
	if (CS_ProtocolChecker::checkAttribute(temp.attributeId, Scope_HWCTable))
		this->attributeId = static_cast<CS_Attribute_Identifier> (temp.attributeId);
	else
	{
		this->attributeId = Attribute_NotSpecified;	// Not a valid attribute

		ACS_CS_AttributeException ex;
		ex.setDescription("Not a valid attribute");
		throw ex;
	}

	return 0;

}

 short unsigned ACS_CS_HWC_GetValue::getLength () const
{

	return 4;	// The length of this primitive is always 4 bytes

}

 ACS_CS_Primitive * ACS_CS_HWC_GetValue::clone () const
{

   return new ACS_CS_HWC_GetValue(*this);

}

 short unsigned ACS_CS_HWC_GetValue::getBoardId () const
{

	return this->boardId;

}

 ACS_CS_Protocol::CS_Attribute_Identifier ACS_CS_HWC_GetValue::getAttributeId () const
{

	return ACS_CS_Protocol::CS_Attribute_Identifier(this->attributeId);

}

 void ACS_CS_HWC_GetValue::setAttributeId (ACS_CS_Protocol::CS_Attribute_Identifier identifier)
{

	this->attributeId = identifier;

}

 void ACS_CS_HWC_GetValue::setBoardId (unsigned short identifier)
{

	this->boardId = identifier;

}

// Additional Declarations

