//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4586C46302AD.cm preserve=no
//   %X% %Q% %Z% %W%
//## end module%4586C46302AD.cm

//## begin module%4586C46302AD.cp preserve=no
//## end module%4586C46302AD.cp

//## Module: ACS_CS_API_IdList_Implementation%4586C46302AD; Package body
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: C:\ccviews\ewubrya_hl12021ss\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_IdList_Implementation.cpp

//## begin module%4586C46302AD.additionalIncludes preserve=no
//## end module%4586C46302AD.additionalIncludes

//## begin module%4586C46302AD.includes preserve=yes
//## end module%4586C46302AD.includes

// ACS_CS_API_IdList_Implementation
#include "ACS_CS_API_IdList_Implementation.h"
//## begin module%4586C46302AD.declarations preserve=no
//## end module%4586C46302AD.declarations

//## begin module%4586C46302AD.additionalDeclarations preserve=yes
//## end module%4586C46302AD.additionalDeclarations


// Class ACS_CS_API_IdList_Implementation 

ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation()
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.hasinit preserve=no
      : array(0),
        arraySize(0)
  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.hasinit
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.initialization preserve=yes
  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.initialization
{
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.body preserve=yes
  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_const.body
}

ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation(const ACS_CS_API_IdList_Implementation &right)
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.hasinit preserve=no
  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.hasinit
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.initialization preserve=yes
  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.initialization
{
  //## begin ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.body preserve=yes
   array = 0;
   arraySize = 0;

   unsigned int numberOfElements = right.size();

   // This allocates array
   this->setSize(numberOfElements);

   for(unsigned int i = 0; i < numberOfElements; i++)
   {
      this->setValue(right[i], i);
   }

  //## end ACS_CS_API_IdList_Implementation::ACS_CS_API_IdList_Implementation%45869A460251_copy.body
}


ACS_CS_API_IdList_Implementation::~ACS_CS_API_IdList_Implementation()
{
  //## begin ACS_CS_API_IdList_Implementation::~ACS_CS_API_IdList_Implementation%45869A460251_dest.body preserve=yes

   if (this->array)
   {
      delete [] this->array;
   }

  //## end ACS_CS_API_IdList_Implementation::~ACS_CS_API_IdList_Implementation%45869A460251_dest.body
}

ACS_CS_API_IdList_Implementation & ACS_CS_API_IdList_Implementation::operator=(const ACS_CS_API_IdList_Implementation &right)
{
  //## begin ACS_CS_API_IdList_Implementation::operator=%45869A460251_assign.body preserve=yes
   if( this != &right )
   {
      unsigned int numberOfElements = right.size();

      // It deletes and reallocates the array
      this->setSize(numberOfElements);

      for(unsigned int i = 0; i < numberOfElements; i++)
      {
         this->setValue(right[i], i);
      }
      
   }
    return *this;
  //## end ACS_CS_API_IdList_Implementation::operator=%45869A460251_assign.body
}

uint16_t ACS_CS_API_IdList_Implementation::operator[](size_t index) const
{
  //## begin ACS_CS_API_IdList_Implementation::operator[]%45869A460251_ind.body preserve=yes

	uint16_t value = 0;

   if (this->array)
   {
      if (index < this->arraySize)
      {
         value = this->array[index];
      }
   }

   return value;

  //## end ACS_CS_API_IdList_Implementation::operator[]%45869A460251_ind.body
}

//## Other Operations (implementation)
size_t ACS_CS_API_IdList_Implementation::size () const
{
  //## begin ACS_CS_API_IdList_Implementation::size%458A30B40246.body preserve=yes

   return this->arraySize;

  //## end ACS_CS_API_IdList_Implementation::size%458A30B40246.body
}

 void ACS_CS_API_IdList_Implementation::setValue (uint16_t value, size_t index)
{
  //## begin ACS_CS_API_IdList_Implementation::setValue%458A30B40256.body preserve=yes

   if (this->array)
   {
      if (index < arraySize)
      {
         this->array[index] = value;
      }
   }

  //## end ACS_CS_API_IdList_Implementation::setValue%458A30B40256.body
}

 void ACS_CS_API_IdList_Implementation::setSize (size_t newSize)
{
  //## begin ACS_CS_API_IdList_Implementation::setSize%458A30B40265.body preserve=yes

   if (this->array)
   {
      delete [] this->array;
      this->array = 0;
      this->arraySize = 0;
   }

   this->arraySize = newSize;

   this->array = new unsigned short[this->arraySize];

  //## end ACS_CS_API_IdList_Implementation::setSize%458A30B40265.body
}

// Additional Declarations
  //## begin ACS_CS_API_IdList_Implementation%45869A460251.declarations preserve=yes
  //## end ACS_CS_API_IdList_Implementation%45869A460251.declarations

//## begin module%4586C46302AD.epilog preserve=yes
//## end module%4586C46302AD.epilog
