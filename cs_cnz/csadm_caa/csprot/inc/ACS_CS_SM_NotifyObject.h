

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************


#ifndef ACS_CS_SM_NotifyObject_h
#define ACS_CS_SM_NotifyObject_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_SM_NotifyObject : public ACS_CS_Primitive  //## Inherits: <unnamed>%4AAE77AD00F4
{

  public:
      ACS_CS_SM_NotifyObject();

      virtual ~ACS_CS_SM_NotifyObject();


      virtual unsigned short getLength () const;

      void setLength (unsigned short length);

    // Additional Public Declarations

  protected:
      explicit ACS_CS_SM_NotifyObject(const ACS_CS_SM_NotifyObject &right);

    // Data Members for Class Attributes

      mutable  short unsigned m_length;

    // Additional Protected Declarations

  private:
    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_SM_NotifyObject 



#endif
