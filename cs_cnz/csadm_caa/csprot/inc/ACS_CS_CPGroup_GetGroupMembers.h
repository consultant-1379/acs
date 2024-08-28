

//	� Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPGroup_GetGroupMembers_h
#define ACS_CS_CPGroup_GetGroupMembers_h 1


class ACS_CS_HostNetworkConverter;

#include <string>


// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPGroup_GetGroupMembers : public ACS_CS_Primitive  //## Inherits: <unnamed>%45DD3CB700D5
{

  public:
      ACS_CS_CPGroup_GetGroupMembers();

      ACS_CS_CPGroup_GetGroupMembers(const ACS_CS_CPGroup_GetGroupMembers &right);

      virtual ~ACS_CS_CPGroup_GetGroupMembers();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void setGroupName (std::string name);

      std::string getGroupName () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPGroup_GetGroupMembers & operator=(const ACS_CS_CPGroup_GetGroupMembers &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::string groupName;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPGroup_GetGroupMembers 



#endif
