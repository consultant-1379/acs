#include "ACS_CS_API_MacList_Implementation.h"

ACS_CS_API_MacList_Implementation::ACS_CS_API_MacList_Implementation()
      : array(0),
        arraySize(0)
{
}

ACS_CS_API_MacList_Implementation::ACS_CS_API_MacList_Implementation(const ACS_CS_API_MacList_Implementation &right)
{
   array = 0;
   arraySize = 0;

   unsigned int numberOfElements = right.size();

   // This allocates array
   this->setSize(numberOfElements);

   for(unsigned int i = 0; i < numberOfElements; i++)
   {
      this->setValue(right[i], i);
   }
}


ACS_CS_API_MacList_Implementation::~ACS_CS_API_MacList_Implementation()
{
   if (this->array)
   {
      delete [] this->array;
   }
}

ACS_CS_API_MacList_Implementation & ACS_CS_API_MacList_Implementation::operator=(const ACS_CS_API_MacList_Implementation &right)
{
   if( this != &right )
   {
      unsigned int numberOfElements = right.size();

      this->setSize(numberOfElements);

      for(unsigned int i = 0; i < numberOfElements; i++)
      {
         this->setValue(right[i], i);
      }

   }
    return *this;
}

std::string ACS_CS_API_MacList_Implementation::operator[](size_t index) const
{
   std::string value = "";
   if (this->array)
   {
      if (index < this->arraySize)
      {
         value = this->array[index];
      }
   }
   return value;
}

size_t ACS_CS_API_MacList_Implementation::size () const
{
   return this->arraySize;
}

 void ACS_CS_API_MacList_Implementation::setValue (std::string value, size_t index)
{
   if (this->array)
   {
      if (index < arraySize)
      {
         this->array[index] = value;
      }
   }
}

 void ACS_CS_API_MacList_Implementation::setSize (size_t newSize)
{
   if (this->array)
   {
      delete [] this->array;
      this->array = 0;
      this->arraySize = 0;
   }

   this->arraySize = newSize;

   this->array = new std::string[this->arraySize];
}

