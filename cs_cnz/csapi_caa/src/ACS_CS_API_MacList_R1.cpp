#include "ACS_CS_API_MacList_Implementation.h"
#include "ACS_CS_API_Common_R1.h"

ACS_CS_API_MacList_R1::ACS_CS_API_MacList_R1()
      : implementation(0)
{
	implementation = new ACS_CS_API_MacList_Implementation();
}

ACS_CS_API_MacList_R1::ACS_CS_API_MacList_R1(const ACS_CS_API_MacList_R1 &right)
{
   implementation = new ACS_CS_API_MacList_Implementation(*right.implementation);
}


ACS_CS_API_MacList_R1::~ACS_CS_API_MacList_R1()
{
	if (implementation)
		delete implementation;
}


ACS_CS_API_MacList_R1 & ACS_CS_API_MacList_R1::operator=(const ACS_CS_API_MacList_R1 &right)
{
   if (this != &right)
   {
      if (this->implementation)
      {
         delete this->implementation;
         this->implementation = 0;
      }

	   if (right.implementation)
	   {
		   this->implementation = new ACS_CS_API_MacList_Implementation(*right.implementation);
	   }
   }
	return *this;
}


std::string ACS_CS_API_MacList_R1::operator[](size_t index) const
{
	if (implementation)
		return implementation->operator [](index);
	else
		return 0;
}



size_t ACS_CS_API_MacList_R1::size () const
{
	if (implementation)
		return implementation->size();
	else
		return 0;
}

 void ACS_CS_API_MacList_R1::setValue (std::string value, size_t index)
{
	if (implementation)
		implementation->setValue(value, index);
}

 void ACS_CS_API_MacList_R1::setSize (size_t newSize)
{
	if (implementation)
		implementation->setSize(newSize);
}

