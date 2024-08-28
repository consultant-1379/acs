//	 Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_API_Name_Implementation_h
#define ACS_CS_API_Name_Implementation_h 1


#include "ACS_CS_API_Common_R1.h"


class ACS_CS_API_Name_Implementation 
{

  public:
      ACS_CS_API_Name_Implementation();

      ACS_CS_API_Name_Implementation(const ACS_CS_API_Name_Implementation &right);

      ACS_CS_API_Name_Implementation (const char *newName);

      virtual ~ACS_CS_API_Name_Implementation();

      ACS_CS_API_Name_Implementation & operator=(const ACS_CS_API_Name_Implementation &right);

      ACS_CS_API_NS::CS_API_Result getName (char *name, size_t &nameLength) const;

      void setName (const char *newName);

      size_t length () const;


  private: //## implementation

      char *storedName;

       size_t storedNameLength;

};

//## begin ACS_CS_API_Name_Implementation%45869A480280.postscript preserve=yes
//## end ACS_CS_API_Name_Implementation%45869A480280.postscript

// Class ACS_CS_API_Name_Implementation 

//## begin module%4586C581029D.epilog preserve=yes
//## end module%4586C581029D.epilog


#endif
