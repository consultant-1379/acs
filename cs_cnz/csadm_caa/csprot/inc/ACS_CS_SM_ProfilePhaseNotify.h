

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


#ifndef ACS_CS_SM_ProfilePhaseNotify_h
#define ACS_CS_SM_ProfilePhaseNotify_h 1



// ACS_CS_SM_NotifyObject
#include "ACS_CS_SM_NotifyObject.h"







class ACS_CS_SM_ProfilePhaseNotify : public ACS_CS_SM_NotifyObject  //## Inherits: <unnamed>%4AAE77BD0099
{

  public:
      explicit ACS_CS_SM_ProfilePhaseNotify();

      virtual ~ACS_CS_SM_ProfilePhaseNotify();


      void getOmProfileChange (ACS_CS_API_OmProfileChange &omChange) const;

      void setOmProfileChange (const ACS_CS_API_OmProfileChange &omProfileChange);

      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual ACS_CS_Primitive * clone () const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      explicit ACS_CS_SM_ProfilePhaseNotify(const ACS_CS_SM_ProfilePhaseNotify &right);

      ACS_CS_SM_ProfilePhaseNotify & operator=(const ACS_CS_SM_ProfilePhaseNotify &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_API_OmProfileChange m_omProfileChange;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_ProfilePhaseNotify 



#endif
