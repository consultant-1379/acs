

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


#ifndef ACS_CS_SM_SubscriptionObject_h
#define ACS_CS_SM_SubscriptionObject_h 1



// ACS_CS_Primitive
#include "ACS_CS_Primitive.h"







class ACS_CS_SM_SubscriptionObject : public ACS_CS_Primitive  //## Inherits: <unnamed>%4AAFD7B402D2
{

  public:
      ACS_CS_SM_SubscriptionObject();

      virtual ~ACS_CS_SM_SubscriptionObject();

    // Data Members for Class Attributes

       bool isSubscription;

    // Additional Public Declarations

  protected:
      ACS_CS_SM_SubscriptionObject(const ACS_CS_SM_SubscriptionObject &right);

    // Additional Protected Declarations

  private:
      ACS_CS_SM_SubscriptionObject & operator=(const ACS_CS_SM_SubscriptionObject &right);

    // Additional Private Declarations

  private: //## implementation
    // Additional Implementation Declarations

};


// Class ACS_CS_SM_SubscriptionObject 



#endif
