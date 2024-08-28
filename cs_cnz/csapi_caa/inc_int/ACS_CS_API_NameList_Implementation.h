//	 Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_API_NameList_Implementation_h
#define ACS_CS_API_NameList_Implementation_h 1


#include "ACS_CS_API_Common_R1.h"

class ACS_CS_API_NameList_Implementation 
{

  public:
      ACS_CS_API_NameList_Implementation();

      virtual ~ACS_CS_API_NameList_Implementation();

      size_t size ();

      void setSize (size_t newSize);

      ACS_CS_API_Name_R1 getNameR1 (size_t index);

      void setNameR1 (const ACS_CS_API_Name_R1 &value, size_t index);

  private:
      ACS_CS_API_NameList_Implementation(const ACS_CS_API_NameList_Implementation &right);

      ACS_CS_API_NameList_Implementation & operator=(const ACS_CS_API_NameList_Implementation &right);

  private:

       ACS_CS_API_Name_R1 *array;

       size_t  arraySize;
};

#endif
