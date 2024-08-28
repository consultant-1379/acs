
//	Copyright Ericsson AB 2007. All rights reserved.

#include <string>

extern "C" {
#include <string.h>
}


// ACS_CS_API_Name_Implementation
#include "ACS_CS_API_Name_Implementation.h"



// Class ACS_CS_API_Name_Implementation 

ACS_CS_API_Name_Implementation::ACS_CS_API_Name_Implementation()
      : storedName(0),
        storedNameLength(0)
{
}

ACS_CS_API_Name_Implementation::ACS_CS_API_Name_Implementation(const ACS_CS_API_Name_Implementation &right)
{
   this->storedName = new char[right.storedNameLength];
   this->storedNameLength = right.storedNameLength;
   strncpy(this->storedName, right.storedName, right.storedNameLength);
}

ACS_CS_API_Name_Implementation::ACS_CS_API_Name_Implementation (const char *newName)
      : storedName(0),
        storedNameLength(0)
{
	if (newName)
   {
      this->storedNameLength = static_cast<int>(strlen(newName)) + 1;
      this->storedName = new char[this->storedNameLength];
      if (this->storedName)
         strncpy(this->storedName, newName, this->storedNameLength);
   }
}


ACS_CS_API_Name_Implementation::~ACS_CS_API_Name_Implementation()
{
	if (this->storedName)
		delete [] this->storedName;
}


ACS_CS_API_Name_Implementation & ACS_CS_API_Name_Implementation::operator=(const ACS_CS_API_Name_Implementation &right)
{
	if (this == &right)
		return *this;

	if (this->storedName)
	{
		delete [] this->storedName;
		this->storedName = 0;
      this->storedNameLength = 0;
	}

	if (right.storedName)
	{
      this->storedNameLength = right.storedNameLength;
      this->storedName = new char[this->storedNameLength];
      if (this->storedName)
         strncpy(this->storedName, right.storedName, this->storedNameLength);
	}

	return *this;

}



 ACS_CS_API_NS::CS_API_Result ACS_CS_API_Name_Implementation::getName (char *name, size_t &nameLength) const
{
	if (this->storedName)
	{
      if (nameLength >= this->storedNameLength)
		{
			if (name)
			{
            strncpy(name, this->storedName, this->storedNameLength);

				return ACS_CS_API_NS::Result_Success;
			}
		}
      else
         nameLength = this->storedNameLength;
	}
	return ACS_CS_API_NS::Result_Failure;

}

 void ACS_CS_API_Name_Implementation::setName (const char *newName)
{
	if (this->storedName)
	{
		delete [] this->storedName;
		this->storedName = 0;
      this->storedNameLength = 0;
	}

	if (newName)
	{
      this->storedNameLength = static_cast<int>(strlen(newName)) + 1;
      this->storedName = new char[this->storedNameLength];
      if (this->storedName)
         strncpy(this->storedName, newName, this->storedNameLength);
	}
}

size_t ACS_CS_API_Name_Implementation::length () const
{

   return this->storedNameLength;

}

// Additional Declarations

