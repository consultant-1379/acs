

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_MultiEntry_h
#define ACS_CS_HWC_MultiEntry_h 1



#include <vector>


// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"


class ACS_CS_Attribute;
using std::vector;







class ACS_CS_HWC_MultiEntry : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C8523F00FB
{

  public:
      ACS_CS_HWC_MultiEntry();

      ACS_CS_HWC_MultiEntry(const ACS_CS_HWC_MultiEntry &right);

      virtual ~ACS_CS_HWC_MultiEntry();


      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      unsigned short getAttributeCount () const;

      const ACS_CS_Attribute * getAttribute (unsigned short index) const;

      int setAttributeArray (const ACS_CS_Attribute **array, unsigned short size);

    // Additional Public Declarations

  protected:
    // Data Members for Class Attributes

       short unsigned attributeCount;

      vector<ACS_CS_Attribute * > *attributeVector;

       short unsigned length;

      const ACS_CS_Attribute  **attributeArray;

    // Additional Protected Declarations

  private:
      ACS_CS_HWC_MultiEntry & operator=(const ACS_CS_HWC_MultiEntry &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_HWC_MultiEntry 



#endif
