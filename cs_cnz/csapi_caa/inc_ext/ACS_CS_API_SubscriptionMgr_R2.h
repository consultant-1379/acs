//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_API_SubscriptionMgr_R2_h
#define ACS_CS_API_SubscriptionMgr_R2_h 1


#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_CpTableObserver_R2.h"
#include "ACS_CS_API_SubscriptionMgr_R1.h"
class ACS_CS_API_SubscriptionMgr_R2;

class ACS_CS_API_SubscriptionMgr_R2: public ACS_CS_API_SubscriptionMgr_R1
{
public:

	virtual ACS_CS_API_NS::CS_API_Result subscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer);

	virtual ACS_CS_API_NS::CS_API_Result unsubscribeCpTableChanges (ACS_CS_API_CpTableObserver_R2 &observer);

	static  ACS_CS_API_SubscriptionMgr_R2 * getInstance ();

protected:
      ACS_CS_API_SubscriptionMgr_R2();

      virtual ~ACS_CS_API_SubscriptionMgr_R2();

  private:
      ACS_CS_API_SubscriptionMgr_R2(const ACS_CS_API_SubscriptionMgr_R2 &right);

      ACS_CS_API_SubscriptionMgr_R2 & operator=(const ACS_CS_API_SubscriptionMgr_R2 &right);

};



#endif
