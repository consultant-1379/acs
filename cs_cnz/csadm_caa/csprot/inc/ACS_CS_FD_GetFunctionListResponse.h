

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_FD_GetFunctionListResponse_h
#define ACS_CS_FD_GetFunctionListResponse_h 1



#include <string>
#include <vector>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_FD_GetFunctionListResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45DDA68B00B6
{

  public:
      ACS_CS_FD_GetFunctionListResponse();

      ACS_CS_FD_GetFunctionListResponse(const ACS_CS_FD_GetFunctionListResponse &right);

      virtual ~ACS_CS_FD_GetFunctionListResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      void setServiceVector (const std::vector<std::string> &nameVector);

      void getServiceVector (std::vector<std::string> &nameVector) const;

      void setDomainVector (const std::vector<std::string> &nameVector);

      void getDomainVector (std::vector<std::string> &nameVector) const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_FD_GetFunctionListResponse & operator=(const ACS_CS_FD_GetFunctionListResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       std::vector<std::string> domainVector;

       std::vector<std::string> serviceVector;

       short unsigned totalDomainLength;

       short unsigned totalServiceLength;

    // Additional Implementation Declarations

};


// Class ACS_CS_FD_GetFunctionListResponse 



#endif
