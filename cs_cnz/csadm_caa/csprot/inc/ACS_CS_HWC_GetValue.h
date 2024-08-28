

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_GetValue_h
#define ACS_CS_HWC_GetValue_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_HWC_GetValue : public ACS_CS_Primitive  //## Inherits: <unnamed>%45C708090020
{

  public:
      ACS_CS_HWC_GetValue();

      ACS_CS_HWC_GetValue(const ACS_CS_HWC_GetValue &right);

      virtual ~ACS_CS_HWC_GetValue();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int getBuffer (char *buffer, int size) const;

      int setBuffer (const char *buffer, int size);

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getBoardId () const;

      ACS_CS_Protocol::CS_Attribute_Identifier getAttributeId () const;

      void setAttributeId (ACS_CS_Protocol::CS_Attribute_Identifier identifier);

      void setBoardId (unsigned short identifier);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_HWC_GetValue & operator=(const ACS_CS_HWC_GetValue &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned boardId;

       ACS_CS_Protocol::CS_Attribute_Identifier attributeId;

    // Additional Implementation Declarations
	   // Define struct to hold values
		struct getValueBuffer
		{
			unsigned short boardId;
			unsigned short attributeId;
		};
};


// Class ACS_CS_HWC_GetValue 



#endif
