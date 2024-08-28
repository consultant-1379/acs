
//	Copyright Ericsson AB 2007. All rights reserved.



#include "ACS_CS_API_Name_Implementation.h"

// ACS_CS_API_Common_R1
#include "ACS_CS_API_Common_R1.h"



// Class ACS_CS_API_Name_R1 

ACS_CS_API_Name_R1::ACS_CS_API_Name_R1()
      : implementation(0)
{

   implementation = new ACS_CS_API_Name_Implementation();

}

ACS_CS_API_Name_R1::ACS_CS_API_Name_R1(const ACS_CS_API_Name_R1 &right)
{
	if (right.implementation)
		implementation = new ACS_CS_API_Name_Implementation(*right.implementation);
	else
		implementation = new ACS_CS_API_Name_Implementation("NONAME");
}

ACS_CS_API_Name_R1::ACS_CS_API_Name_R1 (const char *name)
      : implementation(0)
{

	implementation = new ACS_CS_API_Name_Implementation(name);

}


ACS_CS_API_Name_R1::~ACS_CS_API_Name_R1()
{
	if (implementation)
		delete implementation;
}


ACS_CS_API_Name_R1 & ACS_CS_API_Name_R1::operator=(const ACS_CS_API_Name_R1 &right)
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
		   this->implementation = new ACS_CS_API_Name_Implementation(*right.implementation);
	   }
   }

   return  *this;

}



 ACS_CS_API_NS::CS_API_Result ACS_CS_API_Name_R1::getName (char *name, size_t &nameLength) const
{
	if (this->implementation)
		return this->implementation->getName(name, nameLength);
	else
		return ACS_CS_API_NS::Result_Failure;

}

 void ACS_CS_API_Name_R1::setName (const char *newName)
{
   if (this->implementation)
      this->implementation->setName(newName);
}

size_t ACS_CS_API_Name_R1::length () const
{
	if (this->implementation)
		return this->implementation->length();
	else
		return 0;

}

// Additional Declarations

