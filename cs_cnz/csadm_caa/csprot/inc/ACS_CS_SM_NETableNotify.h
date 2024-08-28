

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


#ifndef ACS_CS_SM_NETableNotify_h
#define ACS_CS_SM_NETableNotify_h 1


#include "ACS_CS_HostNetworkConverter.h"

// ACS_CS_SM_NotifyObject
#include "ACS_CS_SM_NotifyObject.h"

struct ACS_CS_NetworkElementChange 
{
   // Data Members for Class Attributes
   std::string neId;
   ACS_CS_API_OmProfileChange omProfile;
   ACS_CS_API_ClusterOpMode::Value clusterMode;
   CPID trafficIsolatedCpId;
   CPID trafficLeaderCpId;
   CPID alarmMasterCpId;
   CPID clockMasterCpId;
};







class ACS_CS_SM_NETableNotify : public ACS_CS_SM_NotifyObject  //## Inherits: <unnamed>%4AAE77C5003D
{

  public:
      ACS_CS_SM_NETableNotify();

      explicit ACS_CS_SM_NETableNotify(const ACS_CS_SM_NETableNotify &right);

      virtual ~ACS_CS_SM_NETableNotify();


      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual ACS_CS_Primitive * clone () const;

      void getNeChange (ACS_CS_NetworkElementChange &neChange) const;

      void setNeChange (const ACS_CS_NetworkElementChange &neChange);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_SM_NETableNotify & operator=(const ACS_CS_SM_NETableNotify &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_NetworkElementChange m_neChange;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_NETableNotify 



#endif
