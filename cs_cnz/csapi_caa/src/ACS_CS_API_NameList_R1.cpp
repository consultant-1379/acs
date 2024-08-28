
//	Copyright Ericsson AB 2007. All rights reserved.



#include "ACS_CS_API_NameList_Implementation.h"

// ACS_CS_API_Common_R1
#include "ACS_CS_API_Common_R1.h"



// Class ACS_CS_API_NameList_R1 

ACS_CS_API_NameList_R1::ACS_CS_API_NameList_R1()
      : implementation(0)
{

	implementation = new ACS_CS_API_NameList_Implementation();

}


ACS_CS_API_NameList_R1::~ACS_CS_API_NameList_R1()
{
	if (implementation)
		delete implementation;
}


ACS_CS_API_Name_R1 ACS_CS_API_NameList_R1::operator[](size_t index) const
{
	ACS_CS_API_Name_R1 name("");

	if (this->implementation)
		return this->implementation->getNameR1(index);
	else
		return name;

}



size_t ACS_CS_API_NameList_R1::size () const
{
	if (this->implementation)
		return this->implementation->size();
	else
		return 0;

}

 void ACS_CS_API_NameList_R1::setValue (const ACS_CS_API_Name_R1 &value, size_t index)
{
	if (this->implementation)
		this->implementation->setNameR1(value, index);
}

 void ACS_CS_API_NameList_R1::setSize (size_t newSize)
{
	if (this->implementation)
		this->implementation->setSize(newSize);
}

// Additional Declarations

