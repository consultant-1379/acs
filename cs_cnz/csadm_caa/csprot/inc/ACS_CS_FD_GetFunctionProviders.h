

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_FD_GetFunctionProviders_h
#define ACS_CS_FD_GetFunctionProviders_h 1



#include <string>


// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_FD_GetFunctionProviders : public ACS_CS_Primitive  //## Inherits: <unnamed>%45DDA6A3025C
{

  public:
      ACS_CS_FD_GetFunctionProviders();

      ACS_CS_FD_GetFunctionProviders(const ACS_CS_FD_GetFunctionProviders &right);

      virtual ~ACS_CS_FD_GetFunctionProviders();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setDomainName (std::string name);

      std::string getDomainName () const;

      void setServiceName (std::string name);

      std::string getServiceName () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_FD_GetFunctionProviders & operator=(const ACS_CS_FD_GetFunctionProviders &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::string domainName;

       std::string serviceName;

    // Additional Implementation Declarations

};


// Class ACS_CS_FD_GetFunctionProviders 



#endif
