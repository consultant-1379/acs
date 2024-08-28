

// -
//	© Copyright Ericsson AB 2007. All rights reserved.
// -


#ifndef ACS_CS_CPId_GetCPAliasNameResponse_h
#define ACS_CS_CPId_GetCPAliasNameResponse_h 1



#include <string>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetCPAliasNameResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%473853940221
{

  public:
      ACS_CS_CPId_GetCPAliasNameResponse();

      ACS_CS_CPId_GetCPAliasNameResponse(const ACS_CS_CPId_GetCPAliasNameResponse &right);

      virtual ~ACS_CS_CPId_GetCPAliasNameResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      std::string getName () const;

      void setName (std::string name);

      bool isAlias () const;

      void setAlias (bool flag = true);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetCPAliasNameResponse & operator=(const ACS_CS_CPId_GetCPAliasNameResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       std::string nameStr;

       bool aliasFlag;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPAliasNameResponse 



#endif
