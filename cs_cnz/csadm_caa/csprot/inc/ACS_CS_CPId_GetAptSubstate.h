

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_CPId_GetAptSubstate_h
#define ACS_CS_CPId_GetAptSubstate_h 1


#include "ACS_CS_API.h"

// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_CPId_GetAptSubstate : public ACS_CS_Primitive  //## Inherits: <unnamed>%4B86C3EB01E6
{

  public:
      ACS_CS_CPId_GetAptSubstate();

      virtual ~ACS_CS_CPId_GetAptSubstate();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      CPID getCpId () const;

      void setCpId (CPID newCpId);

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_CPId_GetAptSubstate(const ACS_CS_CPId_GetAptSubstate &right);

      ACS_CS_CPId_GetAptSubstate & operator=(const ACS_CS_CPId_GetAptSubstate &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       CPID cpId;

    // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetAptSubstate 



#endif
