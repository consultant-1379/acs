

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Attribute.h"

#include "ACS_CS_Trace.h"


// ACS_CS_HWC_MultiEntry
#include "ACS_CS_HWC_MultiEntry.h"


ACS_CS_Trace_TDEF(ACS_CS_HWC_MultiEntry_TRACE);

using namespace ACS_CS_Protocol;



// Class ACS_CS_HWC_MultiEntry 

ACS_CS_HWC_MultiEntry::ACS_CS_HWC_MultiEntry()
      : attributeCount(0),
        attributeVector(0),
        length(2),
        attributeArray(0)
{
}

ACS_CS_HWC_MultiEntry::ACS_CS_HWC_MultiEntry(const ACS_CS_HWC_MultiEntry &right)
  : ACS_CS_Primitive(right)
{

   // Copy members
	this->attributeCount = right.attributeCount;
	this->length = right.length;
   this->attributeVector = 0;
	
	// Check if there is a vector
	if (right.attributeVector)
	{
		// Create a vector in this object
		this->attributeVector = new vector<ACS_CS_Attribute *>(attributeCount, 0);

		vector<ACS_CS_Attribute *>::iterator it;

		// Loop through the vector and make a deep copy
		for (unsigned int i = 0; i < right.attributeVector->size(); i++)
		{
			ACS_CS_Attribute * ptr = right.attributeVector->operator [](i);

			if (ptr)
			{
				ACS_CS_Attribute * newAttribute = new ACS_CS_Attribute(*ptr);
				this->attributeVector->operator [](i) = newAttribute;
			}
		}
	}

}


ACS_CS_HWC_MultiEntry::~ACS_CS_HWC_MultiEntry()
{

   // Check if there is a vector
	if (attributeVector)
	{
		vector<ACS_CS_Attribute *>::iterator it;

		// Loop through vector
		for (it = attributeVector->begin(); it != attributeVector->end(); ++it)
		{
			ACS_CS_Attribute * ptr = *it;

			if (ptr)	// Delete all attributes in the vector
			{
				delete ptr;
			}
		}

		delete attributeVector;
	}

}



 int ACS_CS_HWC_MultiEntry::setBuffer (const char *buffer, int size)
{

   if (buffer == 0)							// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( (size < 2) || (size > USHRT_MAX) )// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
	else if (attributeVector)					// Check if attributes haven't already been set
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setBuffer()\n"
			"Error: No vector\n" ));

		return -1;
	}

	// Get attributeCount from buffer
	unsigned short tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
	attributeCount = ntohs(tempShort);
	unsigned short bufferOffset = 2;	// Current offset into buffer
	bool error = false;					// If there has been an error in the buffer
	bool attributeError = false;
	bool parsingError = false;
	

	if (attributeCount == 0)			// If there are no attributes to get from the buffer
		return 0;						// then we should return since there are no attributes
										// to copy

	if (attributeCount > CHAR_MAX)		// Check for reasonable number of attributes
	{									// (protect against memory consumption)
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setBuffer()\n"
			"Error: Unreasonable number of attributes, attributeCount = %d\n",
			attributeCount ));

		ACS_CS_ParsingException ex;
		ex.setDescription("Unreasonable number of attributes");
		throw ex;
	}
	else
		attributeVector = new vector<ACS_CS_Attribute *>(attributeCount, 0);	// Create vector

	// Loop through buffer
	for (int i = 0; i < attributeCount; i++)
	{
		// These should be set for each attribute
		CS_Attribute_Identifier attId = Attribute_NotSpecified;
		unsigned short valueLength = 0;
		char * value = 0;

		if (bufferOffset > (size - 4) )	// We must always be able to get two 16-bit values
		{								// from the buffer so that we at least can get the
										// attribute id and the value length (each 2 bytes)
			error = true;
			parsingError = true;
			break;
		}

		// Get attribute identifier
		tempShort = ntohs( *( reinterpret_cast<const unsigned short *>(buffer + bufferOffset) ) );
		bufferOffset += 2;	// Increase offset for next value

		// Check and cast attribute identifier
		if (CS_ProtocolChecker::checkAttribute(tempShort, Scope_HWCTable))
			attId = static_cast<CS_Attribute_Identifier>(tempShort);
		else
		{
			error = true;
			attributeError = true;
			break;
		}

		// Get value length
		valueLength = ntohs( *( reinterpret_cast<const unsigned short *>(buffer + bufferOffset) ) );
		bufferOffset += 2;	// Increase offset for next value

		if ( (valueLength + bufferOffset) > size) // Serious length error
		{
			error = true;
			parsingError = true;
			break;
		}
		else		// Correct length => go ahead and copy the value
		{
			value = new char[valueLength];

			if (value)					// Check for successful new
			{
				if (valueLength == 2)	// Convert 2 byte value to host byte order
				{
					tempShort = ntohs( *(reinterpret_cast<const unsigned short *>(buffer + bufferOffset) ) );
					memcpy(value, &tempShort, valueLength);
				}
				else if (valueLength == 4) // Convert 4 byte value to host byte order
				{
					uint32_t tempLong =
						ntohl( *(reinterpret_cast<const uint32_t *>(buffer + bufferOffset) ) );
					memcpy(value, &tempLong, valueLength);
				}
				else	// Copy value without conversion
				{
					memcpy(value, buffer + bufferOffset, valueLength);
				}
			}
			else					// Unsuccessful new
			{
				return -1;
			}
		}

		// Create new attribute object
		ACS_CS_Attribute * newAttribute = new ACS_CS_Attribute(attId);

		if (newAttribute)
		{
			(void) newAttribute->setValue(value, valueLength);
			delete [] value;										// It will have been copied now
			
			// Add new attribute to vector
			this->attributeVector->operator [](i) = newAttribute;

			bufferOffset += valueLength;	// Increase offset
		}
		else
		{
			delete [] value;
			return -1;
		}
	}
	
	if (error)	// If there was an error. We must now clean up the mess
	{
		if (attributeVector)	// If a vector was created
		{
			vector<ACS_CS_Attribute *>::iterator it;

			// Loop through the vector
			for (it = attributeVector->begin(); it != attributeVector->end(); ++it)
			{
				ACS_CS_Attribute * ptr = *it;

				if (ptr)		// Delete all attributes
					delete ptr;
			}

			delete attributeVector;
			attributeVector = 0;
		}
	}
	else	// No error. We set the size and are done
	{
		this->length = size;
	}

	if (parsingError)
	{
		ACS_CS_ParsingException ex;
		ex.setDescription("Error parsing data");
		throw ex;
	}
	else if (attributeError)
	{
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting attribute");
		throw ex;
	}

	return 0;

}

 int ACS_CS_HWC_MultiEntry::getBuffer (char *buffer, int size) const
{

   // Suppress warning for accessing array out of range
	// lint incorrectly believes that
	//lint --e{669}
	//lint --e{662}

	if (buffer == 0)			// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::getBuffer()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size != length )	// Check for valid buffer size
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::getBuffer()\n"
			"Error: Invalid size, size = %d\n",
			size ));

		return -1;
	}
//	if (attributeVector == 0)	// Check if there are attributes set for this object
//		return -1;

	// Copy attributeCount to buffer
	unsigned short tempShort = htons(attributeCount);
	memcpy(buffer, &tempShort, 2);
	unsigned short bufferOffset = 2;	// Current offset into buffer

	// Loop through all attributes
	for (int i = 0; i < attributeCount; i++)
	{
		if (attributeVector == 0)	// Check for valid memory
		{
			ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::getBuffer()\n"
			"Error: No vector\n" ));

			return -1;
		}

		ACS_CS_Attribute * ptr = attributeVector->operator [](i);	// Get current attribute

		tempShort = htons(ptr->getIdentifier());					// Get attribute id
		memcpy(buffer + bufferOffset, &tempShort, 2);				// Copy id to buffer
		bufferOffset += 2;											// Increase buffer offset

		tempShort = htons(ptr->getValueLength());					// Get value length
		memcpy(buffer + bufferOffset, &tempShort, sizeof(tempShort)); // Copy to buffer
		bufferOffset += 2;											// Increase buffer offset

		unsigned short valueLength = ptr->getValueLength();	// Get length of value
		char * tempBuffer = new char[valueLength];		// Allocate memory for value

		if (tempBuffer == 0)	// Check for successful new
			return -1;

		if (valueLength == 2)	// Convert 2 byte value to network byte order and copy it
		{
			(void) ptr->getValue(tempBuffer, 2);
			tempShort = htons( *(reinterpret_cast<const unsigned short *>(tempBuffer) ) );
			memcpy(buffer + bufferOffset, &tempShort, valueLength);
			bufferOffset += valueLength;								// Increase buffer offset
		}
		else if (valueLength == 4)	// Convert 4 byte value to network byte order and copy it
		{
			(void) ptr->getValue(tempBuffer, 4);
			uint32_t tempLong = htonl( *(reinterpret_cast<const uint32_t *>(tempBuffer) ) );
			memcpy(buffer + bufferOffset, &tempLong, valueLength);
			bufferOffset += valueLength;								// Increase buffer offset
		}
		else // Copy value without conversion to network byte order
		{
			(void) ptr->getValue(tempBuffer, valueLength);
			memcpy(buffer + bufferOffset, tempBuffer, valueLength);
			bufferOffset += valueLength;								// Increase buffer offset
		}

		delete [] tempBuffer;
	}

	return 0;

}

 short unsigned ACS_CS_HWC_MultiEntry::getLength () const
{

   return length;

}

 short unsigned ACS_CS_HWC_MultiEntry::getAttributeCount () const
{

   return attributeCount;

}

const ACS_CS_Attribute  * ACS_CS_HWC_MultiEntry::getAttribute (unsigned short index) const
{

   ACS_CS_Attribute * ptr = 0;
	
	if (attributeVector)								// If there is a vector
	{
		if (index < attributeCount)						// Check index
			ptr = attributeVector->operator [](index);	// Get pointer
	}
	
	return ptr;

}

 int ACS_CS_HWC_MultiEntry::setAttributeArray (const ACS_CS_Attribute **array, unsigned short size)
{

   // Suppress warning for not freeing pointer array,
	// lint --e{429}

	if (array == 0)				// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setAttributeArray()\n"
			"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size > CHAR_MAX)	// Check for reasonable size
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setAttributeArray()\n"
			"Error: Unreasonable size, size = %d\n",
			size ));

		return -1;
	}
	else if (attributeVector)	// Check if vector already has been created
	{
		ACS_CS_TRACE((ACS_CS_HWC_MultiEntry_TRACE,
			"(%t) ACS_CS_MultiEntry::setAttributeArray()\n"
			"Error: No vector\n" ));

		return -1;
	}

	attributeVector = new vector<ACS_CS_Attribute *>(size, 0);	// Create vector

	if (attributeVector == 0)	// Check for successful new
		return -1;
	
	for (int i = 0; i < size; i++)		// Loop through array, create new objects and 
	{									// store them in the vector
		attributeVector->operator [](i) = new ACS_CS_Attribute(*array[i]);
		length += (array[i]->getValueLength() + 4);		// Each attribute will increase the size of
	}													// the primitive with 4 bytes + the length of
														// the value
	attributeCount = size;				// Save the size

	return 0;

}

// Additional Declarations

