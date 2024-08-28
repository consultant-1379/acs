

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_FD_GetFunctionUsers_h
#define ACS_CS_FD_GetFunctionUsers_h 1



#include <string>


// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_FD_GetFunctionUsers : public ACS_CS_Primitive  //## Inherits: <unnamed>%45DDA6A6023C
{

  public:
      ACS_CS_FD_GetFunctionUsers();

      ACS_CS_FD_GetFunctionUsers(const ACS_CS_FD_GetFunctionUsers &right);

      virtual ~ACS_CS_FD_GetFunctionUsers();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setDomainName (std::string name);

      std::string getDomainName () const;

      void setServiceName (std::string name);

      std::string getServiceName () const;

      void setAPIdentifier (unsigned short identifier);

      unsigned short getAPIdentifier () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_FD_GetFunctionUsers & operator=(const ACS_CS_FD_GetFunctionUsers &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned apIdentifier;

       std::string domainName;

       std::string serviceName;

    // Additional Implementation Declarations

};


// Class ACS_CS_FD_GetFunctionUsers 



#endif
