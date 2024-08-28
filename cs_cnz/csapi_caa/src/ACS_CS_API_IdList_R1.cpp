//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%45869BA00186.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%45869BA00186.cm

//## begin module%45869BA00186.cp preserve=no
//## end module%45869BA00186.cp

//## Module: ACS_CS_API_IdList_R1%45869BA00186; Package body
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: C:\ccviews\ewubrya_hl12021ss\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_IdList_R1.cpp

//## begin module%45869BA00186.additionalIncludes preserve=no
//## end module%45869BA00186.additionalIncludes

//## begin module%45869BA00186.includes preserve=yes
#include "ACS_CS_API_IdList_Implementation.h"
//## end module%45869BA00186.includes

// ACS_CS_API_Common_R1
#include "ACS_CS_API_Common_R1.h"
//## begin module%45869BA00186.declarations preserve=no
//## end module%45869BA00186.declarations

//## begin module%45869BA00186.additionalDeclarations preserve=yes
//## end module%45869BA00186.additionalDeclarations


// Class ACS_CS_API_IdList_R1 

ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1()
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.hasinit preserve=no
      : implementation(0)
  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.hasinit
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.initialization preserve=yes
  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.initialization
{
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.body preserve=yes

	implementation = new ACS_CS_API_IdList_Implementation();

  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_const.body
}

ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1(const ACS_CS_API_IdList_R1 &right)
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.hasinit preserve=no
  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.hasinit
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.initialization preserve=yes
  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.initialization
{
  //## begin ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.body preserve=yes

   implementation = new ACS_CS_API_IdList_Implementation(*right.implementation);
  
  //## end ACS_CS_API_IdList_R1::ACS_CS_API_IdList_R1%458177790283_copy.body
}


ACS_CS_API_IdList_R1::~ACS_CS_API_IdList_R1()
{
  //## begin ACS_CS_API_IdList_R1::~ACS_CS_API_IdList_R1%458177790283_dest.body preserve=yes

	if (implementation)
		delete implementation;

  //## end ACS_CS_API_IdList_R1::~ACS_CS_API_IdList_R1%458177790283_dest.body
}


ACS_CS_API_IdList_R1 & ACS_CS_API_IdList_R1::operator=(const ACS_CS_API_IdList_R1 &right)
{
  //## begin ACS_CS_API_IdList_R1::operator=%458177790283_assign.body preserve=yes

   if (this != &right)
   {
      if (this->implementation)
      {
         delete this->implementation;
         this->implementation = 0; 
      }

	   if (right.implementation)
	   {
		   this->implementation = new ACS_CS_API_IdList_Implementation(*right.implementation);
	   }
   }

	return *this;

  //## end ACS_CS_API_IdList_R1::operator=%458177790283_assign.body
}


uint16_t ACS_CS_API_IdList_R1::operator[](size_t index) const
{
  //## begin ACS_CS_API_IdList_R1::operator[]%458177790283_ind.body preserve=yes

	if (implementation)
		return implementation->operator [](index);
	else
		return 0;

  //## end ACS_CS_API_IdList_R1::operator[]%458177790283_ind.body
}



//## Other Operations (implementation)
size_t ACS_CS_API_IdList_R1::size () const
{
  //## begin ACS_CS_API_IdList_R1::size%45818937006F.body preserve=yes

	if (implementation)
		return implementation->size();
	else
		return 0;

  //## end ACS_CS_API_IdList_R1::size%45818937006F.body
}

 void ACS_CS_API_IdList_R1::setValue (uint16_t value, size_t index)
{
  //## begin ACS_CS_API_IdList_R1::setValue%458929F1015C.body preserve=yes

	if (implementation)
		implementation->setValue(value, index);

  //## end ACS_CS_API_IdList_R1::setValue%458929F1015C.body
}

 void ACS_CS_API_IdList_R1::setSize (size_t newSize)
{
  //## begin ACS_CS_API_IdList_R1::setSize%458A305A01AA.body preserve=yes

	if (implementation)
		implementation->setSize(newSize);

  //## end ACS_CS_API_IdList_R1::setSize%458A305A01AA.body
}

// Additional Declarations
  //## begin ACS_CS_API_IdList_R1%458177790283.declarations preserve=yes
  //## end ACS_CS_API_IdList_R1%458177790283.declarations

//## begin module%45869BA00186.epilog preserve=yes
//## end module%45869BA00186.epilog
