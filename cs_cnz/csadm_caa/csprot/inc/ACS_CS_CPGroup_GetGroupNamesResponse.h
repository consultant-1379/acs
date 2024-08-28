

//	� Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPGroup_GetGroupNamesResponse_h
#define ACS_CS_CPGroup_GetGroupNamesResponse_h 1



#include <string>
#include <vector>

class ACS_CS_HostNetworkConverter;
// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPGroup_GetGroupNamesResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45DD3CA70394
{

  public:
      ACS_CS_CPGroup_GetGroupNamesResponse();

      ACS_CS_CPGroup_GetGroupNamesResponse(const ACS_CS_CPGroup_GetGroupNamesResponse &right);

      virtual ~ACS_CS_CPGroup_GetGroupNamesResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      void setNameVector (const std::vector<std::string> &nameVector);

      void getNameVector (std::vector<std::string> &nameVector) const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPGroup_GetGroupNamesResponse & operator=(const ACS_CS_CPGroup_GetGroupNamesResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       std::vector<std::string> responseVector;

       short unsigned totalStringSize;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPGroup_GetGroupNamesResponse 



#endif
