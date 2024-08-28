//	© Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_Trace.h"
#include "ACS_CS_Attribute.h"

// ACS_CS_TableSearch
#include "ACS_CS_TableSearch.h"

ACS_CS_Trace_TDEF(ACS_CS_TableSearch_TRACE);

using std::vector;

// Class ACS_CS_TableSearch 

ACS_CS_TableSearch::ACS_CS_TableSearch()
      : attributeVector(0),
        entryList(0),
        entryCount(0),
        attributeCount(0)
{
}

ACS_CS_TableSearch::~ACS_CS_TableSearch()
{
	if (this->attributeVector)		// If there is a vector with attributes
	{
		vector<ACS_CS_Attribute *>::iterator it;

		// Loop through vector
		for (it = attributeVector->begin(); it != attributeVector->end(); ++it)
		{
			ACS_CS_Attribute * ptr = *it;		// Delete all objects in it
			delete ptr;
		}

		delete attributeVector;
	}

	if (entryList)
		delete [] entryList;
}

int ACS_CS_TableSearch::getAttributeCount () const
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::getAttributeCount()\n"
		"Entering method\n"));

	return attributeCount;
}

int ACS_CS_TableSearch::getEntryCount () const
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::getEntryCount()\n"
		"Entering method\n"));

	return entryCount;
}

const ACS_CS_Attribute  * ACS_CS_TableSearch::getAttribute (int index) const
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::getAttribute()\n"
		"Entering method\n"));

	ACS_CS_Attribute * ptr = 0;

	if (attributeVector)								// If there is a vector
	{
		if ( (index < attributeCount) && (index >= 0) )	// If valid index
		{
			ptr = attributeVector->operator [](index);	// Get attribute from vector
		}
	}

	return ptr;
}

int ACS_CS_TableSearch::getEntryList (unsigned short *array, int size) const
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::getEntryList()\n"
		"Entering method\n"));

	if (array == 0)								// Check for valid memory
		return -1;
	else if (size != entryCount)				// Check for valid size
		return -1;
	else if (entryList == 0)					// Check if there is a list to copy
		return -1;

	memcpy(array, entryList, size * sizeof(unsigned short));			// Copy list

	return 0;
}

int ACS_CS_TableSearch::setAttributeArray (const ACS_CS_Attribute **array, int size)
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::setAttributeArray()\n"
		"Entering method\n"));

	if (array == 0)								// Check for valid memory
		return -1;
	else if (size > CHAR_MAX)					// Sanity check
		return -1;
	else if (attributeVector)					// If there is a vector
		return -1;

	attributeVector = new vector<ACS_CS_Attribute *>(size);	// Create new vector

	// Loop through vector
	for (int i = 0; i < size; i++)
	{
		ACS_CS_Attribute * ptr = new ACS_CS_Attribute(*(array[i]));		// Create attribute object
		attributeVector->operator [](i) = ptr;							// Assign to vector
	}

	attributeCount = size;

	return 0;
}

int ACS_CS_TableSearch::setAttribute (const ACS_CS_Attribute &attribute)
{
   ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::setAttribute()\n"
		"Entering method\n"));

	if (attributeVector) // If there is a vector
        return -1;

    attributeVector = new vector<ACS_CS_Attribute *> (1); // Create new vector

    ACS_CS_Attribute * ptr = new ACS_CS_Attribute(attribute); // Create attribute object
    attributeVector->operator [](0) = ptr; // Assign to vector

    attributeCount = 1;

    return 0;
}

int ACS_CS_TableSearch::setEntryList (const unsigned short *array, int size)
{
	ACS_CS_TRACE((ACS_CS_TableSearch_TRACE,
		"(%t) ACS_CS_TableSearch::setEntryList()\n"
		"Entering method\n"));

	if (array == 0)						// Check for valid memory
		return -1;
	else if (size > USHRT_MAX)			// Check for valid size
		return -1;
	else if (entryList)					// Check if there already is a list
		return -1;

	entryList = new unsigned short[size];	// Create new list
	entryCount = size;

	if (entryList)							// Check for successful new
		memcpy(entryList, array, size * 2);	// Copy list

	return 0;
}

