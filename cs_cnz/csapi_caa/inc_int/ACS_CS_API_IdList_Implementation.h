
#ifndef ACS_CS_API_IdList_Implementation_h
#define ACS_CS_API_IdList_Implementation_h 1

#include <string>
#include <stdint.h>

class ACS_CS_API_IdList_Implementation 
{

  public:
      ACS_CS_API_IdList_Implementation();

      ACS_CS_API_IdList_Implementation(const ACS_CS_API_IdList_Implementation &right);

      virtual ~ACS_CS_API_IdList_Implementation();

      ACS_CS_API_IdList_Implementation & operator=(const ACS_CS_API_IdList_Implementation &right);

      uint16_t operator[](const size_t index) const;

      size_t size () const;

      void setValue (uint16_t value, size_t index);

      void setSize (size_t newSize);

  private: //## implementation
       uint16_t *array;

       size_t arraySize;

};


#endif
