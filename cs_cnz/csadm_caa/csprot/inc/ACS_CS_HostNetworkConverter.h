

//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the	agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_HostNetworkConverter_h
#define ACS_CS_HostNetworkConverter_h 1


#include "ACS_CS_API.h"


//struct ACS_CS_API_OmProfileChange_R1;
class ACS_CS_SM_NETableNotify;
class ACS_CS_SM_CPTableNotify;
class ACS_CS_SM_CPGroupTableNotify;
class ACS_CS_SM_ProfilePhaseNotify;
class ACS_CS_SM_SubscribeTable;
class ACS_CS_SM_SubscribeProfilePhaseChange;
class ACS_CS_CPId_GetBlockingInfo;
class ACS_CS_CPId_GetCPCapacity;
class ACS_CS_CPId_GetBlockingInfoResponse;
class ACS_CS_CPId_GetCPCapacityResponse;
class ACS_CS_SM_HWCTableNotify;






class ACS_CS_HostNetworkConverter 
{

  public:
      virtual ~ACS_CS_HostNetworkConverter() = 0;


      static int checkSize (size_t actualSize, size_t maxSize);

      static int serialize (char * &buffer, int &size, const ACS_CS_API_OmProfileChange &omProfileChange);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_ProfilePhaseNotify &notify);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_CPGroupTableNotify &cpGroupTableNotify);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_CPTableNotify &cpTableNotify);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_NETableNotify &neTableNotify);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_HWCTableNotify &hwcTableNotify);

      static int deserialize (const char * &buffer, ACS_CS_SM_HWCTableNotify &hwcTableNotify);

      static int deserialize (const char * &buffer, ACS_CS_API_OmProfileChange &omProfileChange);

      static int deserialize (const char * &buffer, ACS_CS_SM_ProfilePhaseNotify &notify);

      static int deserialize (const char * &buffer, ACS_CS_SM_CPGroupTableNotify &cpGroupTableNotify);

      static int deserialize (const char * &buffer, ACS_CS_SM_CPTableNotify &cpTableNotify);

      static int deserialize (const char * &buffer, ACS_CS_SM_NETableNotify &neTableNotify);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_SubscribeProfilePhaseChange &ppc);

      static int serialize (char * &buffer, int &size, const ACS_CS_SM_SubscribeTable &subscriptionTable);

      static int deserialize (const char * &buffer, ACS_CS_SM_SubscribeTable &subscriptionTable);

      static int deserialize (const char * &buffer, ACS_CS_SM_SubscribeProfilePhaseChange &ppc);

      static int serialize (char * &buffer, int &size, const ACS_CS_CPId_GetCPCapacity &getCpCapacity);

      static int serialize (char * &buffer, int &size, const ACS_CS_CPId_GetCPCapacityResponse &getCpCapacityResponse);

      static int serialize (char * &buffer, int &size, const ACS_CS_CPId_GetBlockingInfo &getBlockingInfo);

      static int serialize (char * &buffer, int &size, const ACS_CS_CPId_GetBlockingInfoResponse &getBlockingInfoResponse);

      static int deserialize (const char * &buffer, ACS_CS_CPId_GetCPCapacity &getCpCapacity);

      static int deserialize (const char * &buffer, ACS_CS_CPId_GetCPCapacityResponse &getCpCapacityResponse);

      static int deserialize (const char * &buffer, ACS_CS_CPId_GetBlockingInfo &getBlockingInfo);

      static int deserialize (const char * &buffer, ACS_CS_CPId_GetBlockingInfoResponse &getBlockingInfoResponse);

  private:
      ACS_CS_HostNetworkConverter();

      ACS_CS_HostNetworkConverter(const ACS_CS_HostNetworkConverter &right);

      ACS_CS_HostNetworkConverter & operator=(const ACS_CS_HostNetworkConverter &right);

};


// Class ACS_CS_HostNetworkConverter 



#endif
