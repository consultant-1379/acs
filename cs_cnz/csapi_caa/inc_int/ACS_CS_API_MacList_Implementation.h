
#ifndef ACS_CS_API_MacList_Implementation_h
#define ACS_CS_API_MacList_Implementation_h 1

#include <string>
#include <stdint.h>

class ACS_CS_API_MacList_Implementation
{

  public:
      ACS_CS_API_MacList_Implementation();

      ACS_CS_API_MacList_Implementation(const ACS_CS_API_MacList_Implementation &right);

      virtual ~ACS_CS_API_MacList_Implementation();

      ACS_CS_API_MacList_Implementation & operator=(const ACS_CS_API_MacList_Implementation &right);

      std::string operator[](const size_t index) const;

      size_t size () const;

      void setValue (std::string value, size_t index);

      void setSize (size_t newSize);

  private: //## implementation
       std::string *array;

       size_t arraySize;

};


#endif
