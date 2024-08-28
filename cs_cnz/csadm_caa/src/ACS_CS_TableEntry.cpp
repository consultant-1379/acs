

//	 Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Attribute.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"
#include "limits.h"

#include <iostream>
#include <fstream>
#include <set>


// ACS_CS_TableEntry
#include "ACS_CS_TableEntry.h"


ACS_CS_Trace_TDEF(ACS_CS_TableEntry_TRACE);

using ACS_CS_NS::ILLEGAL_TABLE_ENTRY;
using std::set;
using std::vector;

ACS_CS_TableEntry::ACS_CS_TableEntry()
    : entryId(ACS_CS_NS::ILLEGAL_TABLE_ENTRY),
      good(true)
{
}

// Class ACS_CS_TableEntry 

ACS_CS_TableEntry::ACS_CS_TableEntry(const ACS_CS_TableEntry &right)
{

	// Suppress warning for not freeing pointer ptr,
	// which we shouldn't since it is stored in an STL set.
	// Set and content freed in destructor.
	//lint --e{429}

	this->entryId = right.entryId;		// Assign value
	this->good = right.good;

	set<ACS_CS_Attribute *>::const_iterator it;	// Create iterator to set

	// Deep copy of the set
	for (it = right.attributeSet.begin(); it != right.attributeSet.end(); ++it)
	{
		ACS_CS_Attribute * ptr = new ACS_CS_Attribute(**it);
		this->attributeSet.insert(ptr);
	}

	this->fileMutexName = right.fileMutexName;
}

ACS_CS_TableEntry::ACS_CS_TableEntry (unsigned short identifier)
      : entryId(identifier),
        good(true)
{
}


ACS_CS_TableEntry::~ACS_CS_TableEntry()
{

	set<ACS_CS_Attribute *>::iterator it;

	// Loop through set
	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)
	{
		ACS_CS_Attribute * ptr = *it;
		delete ptr;							// Delete all objects
	}

}


ACS_CS_TableEntry & ACS_CS_TableEntry::operator=(const ACS_CS_TableEntry &right)
{

	if (this == &right)
		return *this;

	// Suppress warning for not freeing pointer ptr,
	// which we shouldn't since it is stored in an STL set.
	// Set and content freed in destructor.
	//lint --e{429}

	set<ACS_CS_Attribute *>::const_iterator it;

	// Loop through set
	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)
	{
		ACS_CS_Attribute * ptr = *it;
		delete ptr;							// Delete all objects
	}

	attributeSet.clear();


	this->entryId = right.entryId;		// Assign value
	this->good = right.good;
	this->fileMutexName = right.fileMutexName;

	// Deep copy of the set
	for (it = right.attributeSet.begin(); it != right.attributeSet.end(); ++it)
	{
		ACS_CS_Attribute * ptr = new ACS_CS_Attribute(**it);
		this->attributeSet.insert(ptr);
	}

	return *this;

}


int ACS_CS_TableEntry::operator==(const ACS_CS_TableEntry &right) const
{
	int result = 0;

	if (this->entryId == right.entryId)
	{
		if (this->attributeSet.size() == right.attributeSet.size())
		{
			std::set<ACS_CS_Attribute *>::const_iterator it;

			for (it = this->attributeSet.begin(); it != this->attributeSet.end(); ++it)
			{
				ACS_CS_Attribute * leftAttribute = *it;

				if (!leftAttribute)
				{
					result = 0;
					break;
				}

				ACS_CS_Attribute rightAttribute = right.getValue(leftAttribute->getIdentifier());

				if ( leftAttribute->operator ==(rightAttribute))
				{
					result = 1;
				}
				else
				{
					result = 0;
					break;
				}
			}
		}
	}

   return result;

}

int ACS_CS_TableEntry::operator!=(const ACS_CS_TableEntry &right) const
{

	return (this->entryId != right.entryId);

}


int ACS_CS_TableEntry::operator<(const ACS_CS_TableEntry &right) const
{

	return ( this->entryId < right.entryId ? 1 : 0 );	// Compare entry

}

int ACS_CS_TableEntry::operator>(const ACS_CS_TableEntry &right) const
{

	return ( this->entryId > right.entryId ? 1 : 0 );	// Compare entry

}

int ACS_CS_TableEntry::operator<=(const ACS_CS_TableEntry &right) const
{

	return ( this->entryId <= right.entryId ? 1 : 0 );	// Compare entry

}

int ACS_CS_TableEntry::operator>=(const ACS_CS_TableEntry &right) const
{

	return ( this->entryId <= right.entryId ? 1 : 0 );	// Compare entry

}


ostream & operator<<(ostream &stream,const ACS_CS_TableEntry &right)
{

	// Save entryid to stream
	unsigned short entryId = right.entryId;
	(void) stream.write( reinterpret_cast<const char *> (&entryId), sizeof(entryId) );

	// Save entry size to stream
	size_t size = right.attributeSet.size();
	(void) stream.write( reinterpret_cast<const char *> (&size), sizeof(size) );


	set<ACS_CS_Attribute *>::const_iterator it;	// Create iterator to set

	// Loop through set and save every Attribute to stream
	for (it = right.attributeSet.begin(); it != right.attributeSet.end(); ++it)
	{
		ACS_CS_Attribute * ptr = *it;

		if (ptr)
			stream << (*ptr);
	}

	return stream;

}

istream & operator>>(istream &stream,ACS_CS_TableEntry &object)
{

	// Suppress warning for not freeing pointer newAttribute,
	// which we shouldn't since it is stored in an STL set.
	// Set and content freed in destructor
	//lint --e{429}

	// Read entry id from stream
	unsigned short entryId = ILLEGAL_TABLE_ENTRY;
	(void) stream.read( reinterpret_cast<char *> (&entryId), sizeof(entryId) );

	if (entryId != ILLEGAL_TABLE_ENTRY)
		object.entryId = entryId;
	else
	{
		/*ACS_CS_TRACE((ACS_CS_TableEntry_TRACE,
			"(%t) ACS_CS_TableEntry::operator>>\n"
			"Error: entryId is ILLEGAL_TABLE_ENTRY (%u)\n", entryId));*/
		object.good = false;
		return stream;
	}
/*
	ACS_CS_TRACE((ACS_CS_TableEntry_TRACE,
		"(%t) ACS_CS_TableEntry::operator>>\n"
		"Entry id read %d\n", object.entryId));
*/
	// Read entry size from stream
	size_t size = 0;
	(void) stream.read( reinterpret_cast<char *> (&size), sizeof(size) );
/*
	ACS_CS_TRACE((ACS_CS_TableEntry_TRACE,
		"(%t) ACS_CS_TableEntry::operator>>\n"
		"Size read %d\n", size));
*/
	// Check for valid size
	if ( (size > 0) && (size < USHRT_MAX) )
	{
		// Loop over size
		for (size_t i = 0; i < size; i++)
		{
			// Read attribute from stream
			ACS_CS_Attribute tempAttribute(ACS_CS_Protocol::Attribute_NotSpecified);
			stream >> tempAttribute;

			// If read was unsuccessful
			if ( ! tempAttribute.isGood() )
			{
				/*ACS_CS_TRACE((ACS_CS_TableEntry_TRACE,
					"(%t) ACS_CS_TableEntry::operator>>\n"
					"Error: tempAttribute is not good\n"));*/
				object.good = false;
				return stream;
			}
			else	// If read was successful
			{
				// Create new attribute
				ACS_CS_Attribute * newAttribute = new ACS_CS_Attribute(tempAttribute);	

				// and add it to set
				if (newAttribute)
					object.attributeSet.insert(newAttribute);
			}
		}
	}
	else
	{
	/*	ACS_CS_TRACE((ACS_CS_TableEntry_TRACE,
					"(%t) ACS_CS_TableEntry::operator>>\n"
					"Error: not valid size %u\n", size));*/
		object.good = false;
		return stream;
	}

	object.good = true;

	return stream;

}



 short unsigned ACS_CS_TableEntry::getId () const
{

	return this->entryId;

}


void ACS_CS_TableEntry::setId(unsigned short identifier)
{
    this->entryId = identifier;
}

 ACS_CS_Attribute ACS_CS_TableEntry::getValue (ACS_CS_Protocol::CS_Attribute_Identifier identifier) const
{

	ACS_CS_Attribute attribute;
	
	set<ACS_CS_Attribute *>::const_iterator it;				// Create iterator for set
	ACS_CS_Attribute * foundAttribute = 0;

	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)	// Loop through set
	{
		if ((*it)->getIdentifier() == identifier)			// If attribute exists
		{
			foundAttribute = *it;
			break;
		}
	}

	if (foundAttribute)
		attribute = *foundAttribute;

	return attribute;									// return attribute

}

 bool ACS_CS_TableEntry::hasValue(ACS_CS_Protocol::CS_Attribute_Identifier identifier) const
 {
	bool found = false;

	ACS_CS_Attribute attribute;

	set<ACS_CS_Attribute *>::const_iterator it;				// Create iterator for set

	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)	// Loop through set
	{
		if ((*it)->getIdentifier() == identifier)			// If attribute exists
		{
			found = true;
			break;
		}
	}

	return found;
 }

 void ACS_CS_TableEntry::setValue (const ACS_CS_Attribute &attribute)
{

	// Suppress warning for not freeing newAttribute,
	// (which we shouldn't since it is stored in an STL set.
	// Set and content freed in destructor.
	//lint --e{429}

	if (attribute.getValueLength() == 0)
		return;

	set<ACS_CS_Attribute *>::iterator it;
	ACS_CS_Attribute * newAttribute = 0;

	// Loop through set
	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)
	{
		if ( (*it)->getIdentifier() == attribute.getIdentifier() )	// If attribute exists
		{
			newAttribute = *it;								// save pointer
			break;
		}
	}
	
	if (newAttribute)										// If attribute exists
	{
		newAttribute->operator =(attribute);				// Assign the new attribute
	}
	else													// Attribute does not exist
	{
		newAttribute = new ACS_CS_Attribute(attribute);	// Create new attribute

		if (newAttribute)
			attributeSet.insert(newAttribute);				// and add it to set
	}

}

 void ACS_CS_TableEntry::removeValue (ACS_CS_Protocol::CS_Attribute_Identifier identifier)
{

	set<ACS_CS_Attribute *>::iterator it;

	// Loop through set
	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)
	{
		if ( (*it)->getIdentifier() == identifier )	// If attribute exists
		{

			ACS_CS_Attribute * ptr = *it;
			delete ptr;							// Delete all objects
			(void) attributeSet.erase(it);
			break;
		}
	}

}

 bool ACS_CS_TableEntry::isGood () const
{

	return good;

}

 size_t ACS_CS_TableEntry::getValueLength()
{
     return attributeSet.size();
}

 void ACS_CS_TableEntry::getValueVector (std::vector<ACS_CS_Attribute> &valueVector) const
{

	set<ACS_CS_Attribute *>::const_iterator it;				// Create iterator for set

	for (it = attributeSet.begin(); it != attributeSet.end(); ++it)	// Loop through set
	{
      const ACS_CS_Attribute * attribute = *it;

      if (attribute)
         valueVector.push_back(*attribute);
	}

}

// Additional Declarations

