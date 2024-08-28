

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


#ifndef ACS_CS_SM_SubscribeTable_h
#define ACS_CS_SM_SubscribeTable_h 1



// ACS_CS_SM_SubscriptionObject
#include "ACS_CS_SM_SubscriptionObject.h"







class ACS_CS_SM_SubscribeTable : public ACS_CS_SM_SubscriptionObject  //## Inherits: <unnamed>%4AB15F480180
{

  public:
      ACS_CS_SM_SubscribeTable();

      virtual ~ACS_CS_SM_SubscribeTable();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      ACS_CS_Protocol::CS_Scope_Identifier getTableScope () const;

      void setTableScope (ACS_CS_Protocol::CS_Scope_Identifier tableScope);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_SM_SubscribeTable(const ACS_CS_SM_SubscribeTable &right);

      ACS_CS_SM_SubscribeTable & operator=(const ACS_CS_SM_SubscribeTable &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_Protocol::CS_Scope_Identifier m_tableScope;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_SubscribeTable 



#endif
