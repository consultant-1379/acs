//	Copyright Ericsson AB 2007. All rights reserved.
#ifndef ACS_CS_Attribute_h
#define ACS_CS_Attribute_h 1

#include "ACS_CS_Protocol.h"


using std::istream;
using std::ostream;



class ACS_CS_Attribute 
{

  public:
      ACS_CS_Attribute();

      ACS_CS_Attribute(const ACS_CS_Attribute &right);

      ACS_CS_Attribute (ACS_CS_Protocol::CS_Attribute_Identifier identifier);

      virtual ~ACS_CS_Attribute();

      ACS_CS_Attribute & operator=(const ACS_CS_Attribute &right);

      int operator==(const ACS_CS_Attribute &right) const;

      int operator!=(const ACS_CS_Attribute &right) const;

      int operator<(const ACS_CS_Attribute &right) const;

      int operator>(const ACS_CS_Attribute &right) const;

      int operator<=(const ACS_CS_Attribute &right) const;

      int operator>=(const ACS_CS_Attribute &right) const;

      friend ostream & operator<<(ostream &stream,const ACS_CS_Attribute &right);

      friend istream & operator>>(istream &stream,ACS_CS_Attribute &object);


      int getValue (char *buffer, int size) const;

      int setValue (const char *buffer, int size);

      ACS_CS_Protocol::CS_Attribute_Identifier getIdentifier () const;

      int getValueLength () const;

      bool isGood () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
    // Additional Private Declarations

  private:
    // Data Members for Class Attributes

       ACS_CS_Protocol::CS_Attribute_Identifier attributeId;

       char *value;

       int length;

       bool good;

    // Data Members for Associations

    // Additional Implementation Declarations

};


// Class ACS_CS_Attribute 



#endif
