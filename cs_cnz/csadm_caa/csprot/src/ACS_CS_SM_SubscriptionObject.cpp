

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




// ACS_CS_SM_SubscriptionObject
#include "ACS_CS_SM_SubscriptionObject.h"



// Class ACS_CS_SM_SubscriptionObject 

ACS_CS_SM_SubscriptionObject::ACS_CS_SM_SubscriptionObject()
      : isSubscription(true)
{
}

ACS_CS_SM_SubscriptionObject::ACS_CS_SM_SubscriptionObject(const ACS_CS_SM_SubscriptionObject &right)
   : ACS_CS_Primitive(right),
   isSubscription(right.isSubscription)
{
}


ACS_CS_SM_SubscriptionObject::~ACS_CS_SM_SubscriptionObject()
{
}


// Additional Declarations

