

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_AttributeException_h
#define ACS_CS_AttributeException_h 1



// ACS_CS_ProtocolException
#include "ACS_CS_ProtocolException.h"







class ACS_CS_AttributeException : public ACS_CS_ProtocolException
{

  public:
      ACS_CS_AttributeException();

      ACS_CS_AttributeException(const ACS_CS_AttributeException &right);

      virtual ~ACS_CS_AttributeException() throw();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_AttributeException & operator=(const ACS_CS_AttributeException &right);

    // Additional Private Declarations

  private:
    // Additional Implementation Declarations

};


// Class ACS_CS_AttributeException 



#endif
