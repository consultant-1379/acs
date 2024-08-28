//	Copyright Ericsson AB 2007. All rights reserved.

// ACS_CS_API_NameList_Implementation
#include "ACS_CS_API_NameList_Implementation.h"



// Class ACS_CS_API_NameList_Implementation 
ACS_CS_API_NameList_Implementation::ACS_CS_API_NameList_Implementation()
      : array(0),
        arraySize(0)
{
}


ACS_CS_API_NameList_Implementation::~ACS_CS_API_NameList_Implementation()
{
	if (this->array)
		delete [] this->array;
}


size_t ACS_CS_API_NameList_Implementation::size ()
{
    return this->arraySize;
}

void ACS_CS_API_NameList_Implementation::setSize (size_t newSize)
{
	if (this->array)
	{
		delete [] this->array;
		this->array = 0;
		this->arraySize = 0;
	}

	this->array = new ACS_CS_API_Name_R1[newSize];
	this->arraySize = newSize;
}

ACS_CS_API_Name_R1 ACS_CS_API_NameList_Implementation::getNameR1 (size_t index)
{
	ACS_CS_API_Name_R1 defName;

	if (this->array)
	{
		if (index < this->arraySize)
			return this->array[index];
	}

	return defName;
}

void ACS_CS_API_NameList_Implementation::setNameR1 (const ACS_CS_API_Name_R1 &value, size_t index)
{
	if (this->array)
	{
		if (index < this->arraySize)
			this->array[index] = value;
	}
}
