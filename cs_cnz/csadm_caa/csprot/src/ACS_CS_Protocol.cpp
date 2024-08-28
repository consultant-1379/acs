// ACS_CS_Protocol.cpp
//      Copyright (C) Ericsson AB 2007. All rights reserved.

#include "ACS_CS_Protocol.h"

#include <set>
#include <string>

#include "ACS_CS_Util.h"
#include "ACS_CS_HostFile.h"
#include "ACS_CS_Registry.h"
#include "ACS_CS_API.h"
#include "ACS_CS_TCPServer.h"
#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_Protocol_TRACE);

using std::set;
using std::string;

using namespace ACS_CS_NS;
using namespace ACS_CS_API_NS;
using namespace ACS_CS_Protocol;


CS_ProtocolChecker * CS_ProtocolChecker::_instance = 0;

ACS_CS_Protocol::CS_Side_Identifier CS_ProtocolChecker::apSide =
   ACS_CS_Protocol::Side_NotSpecified;

ACS_CS_Protocol::CS_AP_Number CS_ProtocolChecker::apNumber =
   ACS_CS_Protocol::AP_NotSpecified;

ACS_CS_Protocol::CS_Service_Type CS_ProtocolChecker::csServiceType =
   ACS_CS_Protocol::ServiceType_NotSpecified;


/*
  Default constructor
  This constructor sets the valid ranges for different protocol values. When new constants
  are added to the protocol (i.e. when constants are added to ACS_CS_Protocol.h) this
  class needs to be updated too.
*/
CS_ProtocolChecker::CS_ProtocolChecker()
{
   // STL sets for the different values
   // Version, Side and DHCP Method doesn't have a set. These values are checked directly
   // in the function.

	// Insert all Global clock primitive identifiers
	globalClockPrimitiveSet.insert(Primitive_GetGlobalLogicalClock);
	globalClockPrimitiveSet.insert(Primitive_GetGlobalLogicalClockResponse);

   // Insert all HWC primitive identifiers
   hwcPrimitiveSet.insert(Primitive_GetValue);
   hwcPrimitiveSet.insert(Primitive_GetValueResponse);
   hwcPrimitiveSet.insert(Primitive_GetBoardList);
   hwcPrimitiveSet.insert(Primitive_GetBoardListResponse);
//   hwcPrimitiveSet.insert(Primitive_SetValue);
//   hwcPrimitiveSet.insert(Primitive_SetValueResponse);
//   hwcPrimitiveSet.insert(Primitive_AddEntry);
//   hwcPrimitiveSet.insert(Primitive_AddEntryResponse);
//   hwcPrimitiveSet.insert(Primitive_RemoveEntry);
//   hwcPrimitiveSet.insert(Primitive_RemoveEntryResponse);
   hwcPrimitiveSet.insert(Primitive_SynchTable);
   hwcPrimitiveSet.insert(Primitive_SynchTableResponse);

   // Insert all CP Identification primitive identifiers
   cpIdPrimitiveSet.insert(Primitive_GetCPId);
   cpIdPrimitiveSet.insert(Primitive_GetCPIdResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPName);
   cpIdPrimitiveSet.insert(Primitive_GetCPNameResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPAliasName);
   cpIdPrimitiveSet.insert(Primitive_GetCPAliasNameResponse);
   cpIdPrimitiveSet.insert(Primitive_GetAPZSystem);
   cpIdPrimitiveSet.insert(Primitive_GetAPZSystemResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPType);
   cpIdPrimitiveSet.insert(Primitive_GetCPTypeResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPList);
   cpIdPrimitiveSet.insert(Primitive_GetCPListResponse);
//   cpIdPrimitiveSet.insert(Primitive_SetCPAlias);
//   cpIdPrimitiveSet.insert(Primitive_SetCPAliasResponse);
//   cpIdPrimitiveSet.insert(Primitive_SetAPZSystem);
//   cpIdPrimitiveSet.insert(Primitive_SetAPZSystemResponse);
//   cpIdPrimitiveSet.insert(Primitive_SetCPType);
//   cpIdPrimitiveSet.insert(Primitive_SetCPTypeResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPState);
   cpIdPrimitiveSet.insert(Primitive_GetCPStateResponse);
   cpIdPrimitiveSet.insert(Primitive_GetApplicationId);
   cpIdPrimitiveSet.insert(Primitive_GetApplicationIdResponse);
   cpIdPrimitiveSet.insert(Primitive_GetApzSubstate);
   cpIdPrimitiveSet.insert(Primitive_GetApzSubstateResponse);
   cpIdPrimitiveSet.insert(Primitive_GetStateTransCode);
   cpIdPrimitiveSet.insert(Primitive_GetStateTransCodeResponse);
   cpIdPrimitiveSet.insert(Primitive_GetAptSubstate);
   cpIdPrimitiveSet.insert(Primitive_GetAptSubstateResponse);
//   cpIdPrimitiveSet.insert(Primitive_SetQuorumDataEntry);
//   cpIdPrimitiveSet.insert(Primitive_SetQuorumDataEntryResponse);
   cpIdPrimitiveSet.insert(Primitive_GetBlockingInfo);
   cpIdPrimitiveSet.insert(Primitive_GetBlockingInfoResponse);
   cpIdPrimitiveSet.insert(Primitive_GetCPCapacity);
   cpIdPrimitiveSet.insert(Primitive_GetCPCapacityResponse);
   cpIdPrimitiveSet.insert(Primitive_GetMAUType);
   cpIdPrimitiveSet.insert(Primitive_GetMAUTypeResponse);

   // Insert all CP group primitive identifiers
   cpGroupPrimitiveSet.insert(Primitive_GetGroupNames);
   cpGroupPrimitiveSet.insert(Primitive_GetGroupNamesResponse);
   cpGroupPrimitiveSet.insert(Primitive_GetGroupMembers);
   cpGroupPrimitiveSet.insert(Primitive_GetGroupMembersResponse);
//   cpGroupPrimitiveSet.insert(Primitive_DefineGroup);
//   cpGroupPrimitiveSet.insert(Primitive_DefineGroupResponse);
//   cpGroupPrimitiveSet.insert(Primitive_RemoveGroup);
//   cpGroupPrimitiveSet.insert(Primitive_RemoveGroupResponse);
//   cpGroupPrimitiveSet.insert(Primitive_ClearOG);
//   cpGroupPrimitiveSet.insert(Primitive_ClearOGResponse);

   // Insert all Functional distribution primitive identifiers
   funcDistPrimitiveSet.insert(Primitive_GetFunctionList);
   funcDistPrimitiveSet.insert(Primitive_GetFunctionListResponse);
   funcDistPrimitiveSet.insert(Primitive_GetFunctionProviders);
   funcDistPrimitiveSet.insert(Primitive_GetFunctionProvidersResponse);
   funcDistPrimitiveSet.insert(Primitive_GetFunctionUsers);
   funcDistPrimitiveSet.insert(Primitive_GetFunctionUsersResponse);
//   funcDistPrimitiveSet.insert(Primitive_DefineFunctionalDistribution);
//   funcDistPrimitiveSet.insert(Primitive_DefineFunctionalDistributionResponse);
//   funcDistPrimitiveSet.insert(Primitive_RemoveFunctionalDistribution);
//   funcDistPrimitiveSet.insert(Primitive_RemoveFunctionalDistributionResponse);

   // Insert all Network element primitive identifiers
   neIdPrimitiveSet.insert(Primitive_GetCPSystem);
   neIdPrimitiveSet.insert(Primitive_GetCPSystemResponse);
   neIdPrimitiveSet.insert(Primitive_GetNEId);
   neIdPrimitiveSet.insert(Primitive_GetNEIdResponse);
   neIdPrimitiveSet.insert(Primitive_GetAlarmMaster);
   neIdPrimitiveSet.insert(Primitive_GetAlarmMasterResponse);
   neIdPrimitiveSet.insert(Primitive_GetClockMaster);
   neIdPrimitiveSet.insert(Primitive_GetClockMasterResponse);
   neIdPrimitiveSet.insert(Primitive_GetBSOMIPAddress);
   neIdPrimitiveSet.insert(Primitive_GetBSOMIPAddressResponse);
   neIdPrimitiveSet.insert(Primitive_GetTestEnvironment);
   neIdPrimitiveSet.insert(Primitive_GetTestEnvironmentResponse);
//   neIdPrimitiveSet.insert(Primitive_SetNEId);
//   neIdPrimitiveSet.insert(Primitive_SetNEIdResponse);
//   neIdPrimitiveSet.insert(Primitive_SetAlarmMaster);
//   neIdPrimitiveSet.insert(Primitive_SetAlarmMasterResponse);
//   neIdPrimitiveSet.insert(Primitive_SetClockMaster);
//   neIdPrimitiveSet.insert(Primitive_SetClockMasterResponse);

   // Added these here because they are in NE scope
//   neIdPrimitiveSet.insert(Primitive_SetClusterOpMode);
//   neIdPrimitiveSet.insert(Primitive_SetClusterOpModeResponse);
   neIdPrimitiveSet.insert(Primitive_GetClusterOpMode);
   neIdPrimitiveSet.insert(Primitive_GetClusterOpModeResponse);

   // Added these here because they are in NE scope
   neIdPrimitiveSet.insert(Primitive_GetOmProfile);
   neIdPrimitiveSet.insert(Primitive_GetOmProfileResponse);
   // Traffic Leader/Isolated Primitives
//   neIdPrimitiveSet.insert(Primitive_SetTrafficLeader);
//   neIdPrimitiveSet.insert(Primitive_SetTrafficLeaderResponse);
   neIdPrimitiveSet.insert(Primitive_GetTrafficLeader);
   neIdPrimitiveSet.insert(Primitive_GetTrafficLeaderResponse);
//   neIdPrimitiveSet.insert(Primitive_SetTrafficIsolated);
//   neIdPrimitiveSet.insert(Primitive_SetTrafficIsolatedResponse);
   neIdPrimitiveSet.insert(Primitive_GetTrafficIsolated);
   neIdPrimitiveSet.insert(Primitive_GetTrafficIsolatedResponse);

//   neIdPrimitiveSet.insert(Primitive_SetProfile);
//   neIdPrimitiveSet.insert(Primitive_SetProfileResponse);
//   neIdPrimitiveSet.insert(Primitive_SetOmProfileNotification);
//   neIdPrimitiveSet.insert(Primitive_SetOmProfileNotificationResponse);

//   neIdPrimitiveSet.insert(Primitive_SetInternalOmProfile);

   // Insert all Network element primitive identifiers
   vlanIdPrimitiveSet.insert(Primitive_GetVLANList);
   vlanIdPrimitiveSet.insert(Primitive_GetVLANListResponse);
   vlanIdPrimitiveSet.insert(Primitive_GetVLANAddress);
   vlanIdPrimitiveSet.insert(Primitive_GetVLANAddressResponse);
   vlanIdPrimitiveSet.insert(Primitive_GetVLANTable);
   vlanIdPrimitiveSet.insert(Primitive_GetVLANTableResponse);
   vlanIdPrimitiveSet.insert(Primitive_GetNetworkTable);
   vlanIdPrimitiveSet.insert(Primitive_GetNetworkTableResponse);


   // Insert all Subscription primitive identifiers

   subscriptionPrimitiveSet.insert(Primitive_SubscribeTable);
   subscriptionPrimitiveSet.insert(Primitive_SubscribeTableResponse);
   subscriptionPrimitiveSet.insert(Primitive_SubscribeProfilePhaseChange);
   subscriptionPrimitiveSet.insert(Primitive_SubscribeProfilePhaseChangeResponse);

   subscriptionPrimitiveSet.insert(Primitive_NotifyCpidTableChange);
   subscriptionPrimitiveSet.insert(Primitive_NotifyNeTableChange);
   subscriptionPrimitiveSet.insert(Primitive_NotifyCpGroupTableChange);
   subscriptionPrimitiveSet.insert(Primitive_NotifyProfilePhaseChange);

   // Insert replication primitive identifiers
//   replicationPrimitiveSet.insert(Primitive_ACKTable);

   // MODD protocol
   moddPrimitiveSet.insert(Primitive_MODD_Change);
   moddPrimitiveSet.insert(Primitive_MODD_ChangeResponse);
   moddPrimitiveSet.insert(Primitive_MODD_Revert);
   moddPrimitiveSet.insert(Primitive_MODD_RevertResponse);

   // Insert all attribute identifiers
   hwcAttributeSet.insert(Attribute_Magazine);
   hwcAttributeSet.insert(Attribute_Slot);
   hwcAttributeSet.insert(Attribute_SysType);
   hwcAttributeSet.insert(Attribute_SysNo);
   hwcAttributeSet.insert(Attribute_FBN);
   hwcAttributeSet.insert(Attribute_Side);
   hwcAttributeSet.insert(Attribute_SeqNo);
   hwcAttributeSet.insert(Attribute_IP_EthA);
   hwcAttributeSet.insert(Attribute_IP_EthB);
   hwcAttributeSet.insert(Attribute_Alias_EthA);
   hwcAttributeSet.insert(Attribute_Alias_EthB);
   hwcAttributeSet.insert(Attribute_Netmask_Alias_EthA);
   hwcAttributeSet.insert(Attribute_Netmask_Alias_EthB);
   hwcAttributeSet.insert(Attribute_DHCP);
   hwcAttributeSet.insert(Attribute_SysId);
   hwcAttributeSet.insert(Attribute_Sync_Flag);
   hwcAttributeSet.insert(Attribute_MAC_EthA);
   hwcAttributeSet.insert(Attribute_MAC_EthB);
   hwcAttributeSet.insert(Attribute_Current_Load_Module_Version);
   hwcAttributeSet.insert(Attribute_Blade_Product_Number);
   hwcAttributeSet.insert(Attribute_SwVer_Type);
   hwcAttributeSet.insert(Attribute_Blade_Uuid);
   

   // Attributes for CP identification table
   cpIdAttributeSet.insert(Attribute_CPId_Name);
   cpIdAttributeSet.insert(Attribute_CPId_Alias);
   cpIdAttributeSet.insert(Attribute_CPId_APZ_System);
   cpIdAttributeSet.insert(Attribute_CPId_CP_Type);
   cpIdAttributeSet.insert(Attribute_CPId_CPState);
   cpIdAttributeSet.insert(Attribute_CPId_ApplicationId);
   cpIdAttributeSet.insert(Attribute_CPId_ApzSubstate);
   cpIdAttributeSet.insert(Attribute_CPId_StateTransCode);
   cpIdAttributeSet.insert(Attribute_CPId_AptSubstate);
   cpIdAttributeSet.insert(Attribute_CPId_BlockingInfo);
   cpIdAttributeSet.insert(Attribute_CPId_CPCapacity);

   // Attributes for CP group table
   cpGroupAttributeSet.insert(Attribute_CPGroup_GroupIdentifier);
   cpGroupAttributeSet.insert(Attribute_CPGroup_GroupName);
   cpGroupAttributeSet.insert(Attribute_CPGroup_CPIdentity);
   cpGroupAttributeSet.insert(Attribute_CPGroup_ClearOgCode);

   // Attributes for functional distribution table
   funcDistAttributeSet.insert(Attribute_FD_APIdentifier);
   funcDistAttributeSet.insert(Attribute_FD_DomainName);
   funcDistAttributeSet.insert(Attribute_FD_ServiceName);
   funcDistAttributeSet.insert(Attribute_FD_GroupIdentifier);
   funcDistAttributeSet.insert(Attribute_FD_CPIdentity);

   // Attributes for network element table
   neAttributeSet.insert(Attribute_NE_AlarmMaster);
   neAttributeSet.insert(Attribute_NE_ClockMaster);
   neAttributeSet.insert(Attribute_NE_NetworkIdentifier);
   neAttributeSet.insert(Attribute_NE_ClusterOpMode);
   neAttributeSet.insert(Attribute_NE_TrafficLeader);
   neAttributeSet.insert(Attribute_NE_TrafficIsolated);
   neAttributeSet.insert(Attribute_NE_OmProfileCurrent);
   neAttributeSet.insert(Attribute_NE_OmProfileRequested);
   neAttributeSet.insert(Attribute_NE_AptProfileCurrent);
   neAttributeSet.insert(Attribute_NE_AptProfileRequested);
   neAttributeSet.insert(Attribute_NE_ApzProfileCurrent);
   neAttributeSet.insert(Attribute_NE_ApzProfileRequested);
   neAttributeSet.insert(Attribute_NE_Phase);
   neAttributeSet.insert(Attribute_NE_ProfileChangeReason);
   neAttributeSet.insert(Attribute_NE_ApzProfileQueued);
   neAttributeSet.insert(Attribute_NE_AptProfileQueued);
   neAttributeSet.insert(Attribute_NE_ClusterOpModeType);

   // All system properties
   neAttributeSet.insert(SystemProperty_StartupProfileSupervisionComplete);
   neAttributeSet.insert(SystemProperty_CancelClusterOpModeSupervisionTimer);
   neAttributeSet.insert(SystemProperty_CancelOmProfileSupervisionTimer);
   neAttributeSet.insert(SystemProperty_SetOmProfileSupervisionTimer);
   neAttributeSet.insert(SystemProperty_SetPhaseSubscriberCount);
   neAttributeSet.insert(SystemProperty_NeSubscriberCount);
   neAttributeSet.insert(SystemProperty_InitiateUpdatePhaseChange);

   // Attributes for VLAN table
   vlanAttributeSet.insert(Attribute_VLAN_Identifier);
   vlanAttributeSet.insert(Attribute_VLAN_Name);
   vlanAttributeSet.insert(Attribute_VLAN_NetworkAddress);
   vlanAttributeSet.insert(Attribute_VLAN_Netmask);
   vlanAttributeSet.insert(Attribute_VLAN_Type);
   vlanAttributeSet.insert(Attribute_VLAN_AllocatedAddress);
   vlanAttributeSet.insert(Attribute_VLAN_pcp);
   vlanAttributeSet.insert(Attribute_VLAN_vlanType);
   vlanAttributeSet.insert(Attribute_VLAN_Tag);
   vlanAttributeSet.insert(Attribute_VLAN_VNICName);


   // Attributes for all scopes
   allScopesAttributeSet.insert(hwcAttributeSet.begin(), hwcAttributeSet.end());
   allScopesAttributeSet.insert(cpIdAttributeSet.begin(), cpIdAttributeSet.end());
   allScopesAttributeSet.insert(cpGroupAttributeSet.begin(), cpGroupAttributeSet.end());
   allScopesAttributeSet.insert(funcDistAttributeSet.begin(), funcDistAttributeSet.end());
   allScopesAttributeSet.insert(neAttributeSet.begin(), neAttributeSet.end());
   allScopesAttributeSet.insert(vlanAttributeSet.begin(), vlanAttributeSet.end());
	allScopesAttributeSet.insert(globalClockAttributeSet.begin(), globalClockAttributeSet.end());

   // Insert all FBN identifiers
   fbnSet.insert(FBN_SCBRP);
   fbnSet.insert(FBN_RPBIS);
   fbnSet.insert(FBN_CPUB);
   fbnSet.insert(FBN_MAUB);
   fbnSet.insert(FBN_APUB);
   fbnSet.insert(FBN_Disk);
   fbnSet.insert(FBN_DVD);
   fbnSet.insert(FBN_GEA);
   fbnSet.insert(FBN_GESB);
   fbnSet.insert(FBN_SCXB);
   fbnSet.insert(FBN_EPB1);
   fbnSet.insert(FBN_EvoET);
   fbnSet.insert(FBN_CMXB);
   fbnSet.insert(FBN_IPLB);
   fbnSet.insert(FBN_SMXB);

   scopeSet.insert(Scope_HWCTable);
   scopeSet.insert(Scope_CPIdTable);
   scopeSet.insert(Scope_CPGroupTable);
   scopeSet.insert(Scope_FDTable);
   scopeSet.insert(Scope_NE);
   scopeSet.insert(Scope_VLAN);
   scopeSet.insert(Scope_Subscription);
	scopeSet.insert(Scope_GlobalLogicalClock);

   // Insert all result codes
   resultCodeSet.insert(Result_Success);
   resultCodeSet.insert(Result_Version_Not_Supported);
   resultCodeSet.insert(Result_Attribute_Not_Suported);
   resultCodeSet.insert(Result_No_Such_Entry);
   resultCodeSet.insert(Result_No_Value);
   resultCodeSet.insert(Result_Bad_Format);
   resultCodeSet.insert(Result_Busy);
   resultCodeSet.insert(Result_Other_Failure);
   resultCodeSet.insert(Result_Incorrect_String);
   resultCodeSet.insert(Result_Incorrect_CP);
   resultCodeSet.insert(Result_Incorrect_AP);
   resultCodeSet.insert(Result_Duplicate_Definition);
   resultCodeSet.insert(Result_Reserved_Name);
   resultCodeSet.insert(Result_Not_Removable);
   resultCodeSet.insert(Result_No_Master_Access);
   resultCodeSet.insert(Result_Incorrect_Service_Name);
   resultCodeSet.insert(Result_Incorrect_Domain_Name);
   resultCodeSet.insert(Result_Incorrect_Mode);
   resultCodeSet.insert(Result_ClusterOpMode_Switch_In_Progress);
   resultCodeSet.insert(Result_Profile_Switch_In_Progress);
   resultCodeSet.insert(Result_Missing_File);
   resultCodeSet.insert(Result_Awaiting_Dependency);

   // Result codes for MODD protocol
   moddResultCodeSet.insert(MODD_Result_Success);
   moddResultCodeSet.insert(MODD_Result_Version_Not_Supported);
   moddResultCodeSet.insert(MODD_Result_No_Such_Entry);
   moddResultCodeSet.insert(MODD_Result_Other_Failure);
   moddResultCodeSet.insert(MODD_Result_Busy);   
}


void CS_ProtocolChecker::createInstance()
{

   if (CS_ProtocolChecker::_instance == 0)
      CS_ProtocolChecker::_instance = new CS_ProtocolChecker();
}


bool CS_ProtocolChecker::checkVersion(uint16_t version)
{

   return ( version <= ACS_CS_Protocol::Latest_Version) ;
}

bool CS_ProtocolChecker::checkVlanTableVersion(uint16_t version)
{
	return ( (version == ACS_CS_Protocol::VLANVersion_0) ||
			(version == ACS_CS_Protocol::VLANVersion_1) ||
			(version == ACS_CS_Protocol::VLANVersion_2) );
}

bool CS_ProtocolChecker::checkScope(uint16_t scope)
{

   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   set<uint16_t>::iterator it = _instance->scopeSet.find(scope);

   return (it != _instance->scopeSet.end());
}

bool CS_ProtocolChecker::checkPrimitive(uint16_t primitive, uint16_t scope)
{

   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   if ( (scope != Scope_NotSpecified) )//&& (primitive == Primitive_ACKTable) )
      return true;

   // Check legal primitives for each scope
   // The check returns true if iterator contains value,
   if (scope == Scope_HWCTable)
   {
      set<uint16_t>::iterator it = _instance->hwcPrimitiveSet.find(primitive);

      return (it != _instance->hwcPrimitiveSet.end());
   }
   else if (scope == Scope_CPIdTable)
   {
      set<uint16_t>::iterator it = _instance->cpIdPrimitiveSet.find(primitive);

      return (it != _instance->cpIdPrimitiveSet.end());
   }
   else if (scope == Scope_CPGroupTable)
   {
      set<uint16_t>::iterator it = _instance->cpGroupPrimitiveSet.find(primitive);

      return (it != _instance->cpGroupPrimitiveSet.end());
   }
   else if (scope == Scope_FDTable)
   {
      set<uint16_t>::iterator it = _instance->funcDistPrimitiveSet.find(primitive);

      return (it != _instance->funcDistPrimitiveSet.end());
   }
   else if (scope == Scope_NE)
   {
      set<uint16_t>::iterator it = _instance->neIdPrimitiveSet.find(primitive);

      return (it != _instance->neIdPrimitiveSet.end());
   }
   else if (scope == Scope_VLAN)
   {
      set<uint16_t>::iterator it = _instance->vlanIdPrimitiveSet.find(primitive);

      return (it != _instance->vlanIdPrimitiveSet.end());
   }
   else if (scope == Scope_Subscription)
   {
      set<uint16_t>::iterator it = _instance->subscriptionPrimitiveSet.find(primitive);

      return (it != _instance->subscriptionPrimitiveSet.end());
   }
	else if (scope == Scope_GlobalLogicalClock)
	{
		set<uint16_t>::iterator it = _instance->globalClockPrimitiveSet.find(primitive);

		return(it != _instance->globalClockPrimitiveSet.end());
	}
   else if (scope == Scope_MODD)
      {
	   	 // The offset 900 is used since these primitives have the values 900-903 in this file
	   	 // but 0-3 in the MODD protocol
         set<uint16_t>::iterator it = _instance->moddPrimitiveSet.find(primitive + MODD_Primitive_Offset);

         return (it != _instance->moddPrimitiveSet.end());
      }
   else
      return false;
}


bool CS_ProtocolChecker::checkAttribute(uint16_t attribute)
{

   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   set<uint16_t>::iterator it = _instance->allScopesAttributeSet.find(attribute);

   return (it != _instance->allScopesAttributeSet.end());
}


bool CS_ProtocolChecker::checkAttribute(uint16_t attribute, uint16_t scope)
{

   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   // Check legal attributes for each scope
   // The check returns true if iterator contains value,
   if (scope == Scope_HWCTable)
   {
      set<uint16_t>::iterator it = _instance->hwcAttributeSet.find(attribute);

      return (it != _instance->hwcAttributeSet.end());
   }
   else if (scope == Scope_CPIdTable)
   {
      set<uint16_t>::iterator it = _instance->cpIdAttributeSet.find(attribute);

      return (it != _instance->cpIdAttributeSet.end());
   }
   else if (scope == Scope_CPGroupTable)
   {
      set<uint16_t>::iterator it = _instance->cpGroupAttributeSet.find(attribute);

      return (it != _instance->cpGroupAttributeSet.end());
   }
   else if (scope == Scope_FDTable)
   {
      set<uint16_t>::iterator it = _instance->funcDistAttributeSet.find(attribute);

      return (it != _instance->funcDistAttributeSet.end());
   }
   else if (scope == Scope_NE)
   {
      set<uint16_t>::iterator it = _instance->neAttributeSet.find(attribute);

      return (it != _instance->neAttributeSet.end());
   }
   else if (scope == Scope_VLAN)
   {
      set<uint16_t>::iterator it = _instance->vlanAttributeSet.find(attribute);

      return (it != _instance->vlanAttributeSet.end());
   }
	else if (scope == Scope_GlobalLogicalClock)
	{
		set<uint16_t>::iterator it = _instance->globalClockAttributeSet.find(attribute);

		return (it != _instance->globalClockAttributeSet.end());
	}
   else
      return false;
}


bool CS_ProtocolChecker::checkSysType(uint16_t sysType)
{

   if ( (sysType == SysType_BC) || (sysType == SysType_CP) || (sysType == SysType_AP) )
      return true;
   else
      return false;
}


bool CS_ProtocolChecker::checkFBN(uint16_t fbn)
{

   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
   {
      CS_ProtocolChecker::createInstance();
   }

   set<uint16_t>::iterator it = _instance->fbnSet.find(fbn);      // Search for value

   return (it != _instance->fbnSet.end());      // Returns true if iterator contains value,
   // i.e. is not the end
}


bool CS_ProtocolChecker::checkSide(uint16_t side)
{
   return ((side == ACS_CS_Protocol::Side_A) || (side == ACS_CS_Protocol::Side_B));
}


bool CS_ProtocolChecker::checkDHCP(uint16_t dhcp)
{
   return ((dhcp == ACS_CS_Protocol::DHCP_None)
           || (dhcp == ACS_CS_Protocol::DHCP_Normal)
           || (dhcp == ACS_CS_Protocol::DHCP_Client));
}


bool CS_ProtocolChecker::checkResultCode(uint16_t resultCode)
{
   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   set<uint16_t>::iterator it = _instance->resultCodeSet.find(resultCode);        // Search for value

   return (it != _instance->resultCodeSet.end());       // Returns true if iterator contains value,
   // i.e. is not the end
}

bool CS_ProtocolChecker::checkMODDResultCode(uint16_t resultCode)
{
   if ( ! CS_ProtocolChecker::_instance )       // Check if the singleton has been created
      CS_ProtocolChecker::createInstance();

   set<uint16_t>::iterator it = _instance->moddResultCodeSet.find(resultCode);        // Search for value

   return (it != _instance->moddResultCodeSet.end());       // Returns true if iterator contains value,
   // i.e. is not the end
}

bool CS_ProtocolChecker::checkAPZSystem(uint16_t system)
{
   if (  ( (system >= 21200) && (system <= 21299) )
         || ( (system >= 21400) && (system <= 21499) )
         || ( system == 0) )
      return true;
   else
      return false;
}

bool CS_ProtocolChecker::checkCPType(uint16_t type)
{
   if (  ( (type >= 21200) && (type <= 21299) )
         || ( type == 0 ) )
      return true;
   else
      return false;
}

bool CS_ProtocolChecker::checkCPSystem(uint16_t system)
{
   return ((system == ACS_CS_Protocol::System_Single_CP)
           || (system == ACS_CS_Protocol::System_Multiple_CP));
}

bool CS_ProtocolChecker::checkTestEnvironment(uint16_t identifier)
{
   return ((identifier == ACS_CS_Protocol::TestEnvironment_FALSE)
           || (identifier == ACS_CS_Protocol::TestEnvironment_TRUE));
}

string CS_ProtocolChecker::getFBN(uint16_t fbn)
{
   string name = "Board";

   if (fbn == ACS_CS_Protocol::FBN_APUB)
      name = "APUB";
   else if (fbn == ACS_CS_Protocol::FBN_CPUB)
      name = "CPUB";
   else if (fbn == ACS_CS_Protocol::FBN_Disk)
      name = "GED_DISK";
   else if (fbn == ACS_CS_Protocol::FBN_DVD)
      name = "GED_DVD";
   else if (fbn == ACS_CS_Protocol::FBN_GEA)
      name = "GEA";
   else if (fbn == ACS_CS_Protocol::FBN_MAUB)
      name = "MAUB";
   else if (fbn == ACS_CS_Protocol::FBN_RPBIS)
      name = "RPBI-S";
   else if (fbn == ACS_CS_Protocol::FBN_SCBRP)
      name = "SCB-RP";
   else if (fbn == ACS_CS_Protocol::FBN_GESB)
		name = "GESB";
   else if (fbn == ACS_CS_Protocol::FBN_SCXB)
		name = "SCXB";
   else if (fbn == ACS_CS_Protocol::FBN_EPB1)
		name = "EPB1";
   else if (fbn == ACS_CS_Protocol::FBN_EvoET)
   		name = "EvoET";
   else if (fbn == ACS_CS_Protocol::FBN_CMXB)
      	name = "CMXB";
   else if (fbn == ACS_CS_Protocol::FBN_IPLB)
      	name = "IPLB";
   else if (fbn == ACS_CS_Protocol::FBN_SMXB)
      	name = "SMXB";

   return name;
}


string CS_ProtocolChecker::binToString(char * binaryData, int length)
{
   string outString;

   if (length <= 0)
      return outString;

   if (binaryData)
   {
      // springf wants unsigned char
      unsigned char * binPtr = reinterpret_cast<unsigned char *> (binaryData);
      int stringLength = length * 2;

      char * buffer = new char[ stringLength + 1];

      if (buffer)
      {
         for (int i = 0; i < length; i++)                                               // Loop through binary data
         {
            (void) sprintf(buffer + i * 2, "%02x", binPtr[i]);          // Copy each byte to string
         }

         buffer[stringLength] = 0;

         outString.append(buffer);

         delete [] buffer;
      }
   }

   return outString;
}

/*
ACS_CS_Protocol::CS_Side_Identifier CS_ProtocolChecker::getNode()
{
   if (apSide == ACS_CS_Protocol::Side_NotSpecified)
   {
      // Default to side A
      apSide = ACS_CS_Protocol::Side_A;

      uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_A_HOSTNAME);
      uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_B_HOSTNAME);

      bool addressIsLocal = false;

      addressIsLocal = CS_ProtocolChecker::checkAddress(ipB1, CS_Server_Port);

      if ( ! addressIsLocal)
         addressIsLocal = CS_ProtocolChecker::checkAddress(ipB2, CS_Server_Port);

      if (addressIsLocal)
         apSide = ACS_CS_Protocol::Side_B;
   }

   return apSide;
}
*/
ACS_CS_Protocol::CS_Service_Type CS_ProtocolChecker::getServiceType()
{
   if (CS_ProtocolChecker::csServiceType == ACS_CS_Protocol::ServiceType_NotSpecified)
      CS_ProtocolChecker::checkServiceType();

   //return CS_ProtocolChecker::csServiceType;
   return ACS_CS_Protocol::ServiceType_MASTER; //MIHA ADDED FOR NOW
}


void CS_ProtocolChecker::checkServiceType()
{
   if ( checkIfAP1() )
   {
      if ( checkIfActiveNode() )
      {
         CS_ProtocolChecker::csServiceType = ACS_CS_Protocol::ServiceType_MASTER;
      }
      else
      {
            CS_ProtocolChecker::csServiceType = ACS_CS_Protocol::ServiceType_SLAVE;
      }
   }
   else
   {
         CS_ProtocolChecker::csServiceType = ACS_CS_Protocol::ServiceType_SLAVE;
   }
}



bool CS_ProtocolChecker::checkIfActiveNode()
{
   bool active = false;
/*
   HCLUSTER     clusterHandle = 0;
   HGROUP groupHandle = 0;
   const wchar_t CLUSTER_GROUP_NAME[] = L"Cluster Group";
   DWORD size = 300;
   wchar_t ownerNode[300];

   // Open handle to cluster
   clusterHandle = OpenCluster(NULL);

   if (clusterHandle)
   {
      // Open cluster group
      groupHandle = OpenClusterGroup( clusterHandle, (LPWSTR)CLUSTER_GROUP_NAME );

      if (groupHandle != 0)
      {
         // Get group state
         CLUSTER_GROUP_STATE groupState = GetClusterGroupState(groupHandle, ownerNode, &size);

         // Check if group was found
         if (groupState != ClusterGroupStateUnknown)
         {
            DWORD       namelen = 0;
            LPWSTR nodeName = 0;

            // Get name length for the computer's NETBIOS name
            (void) GetComputerNameW(nodeName, &namelen);

            if (namelen > 0)
            {
               nodeName = new WCHAR[namelen];

               if (nodeName)
               {
                  // Get NETBIOS name for this computer
                  (void) GetComputerNameW(nodeName, &namelen);

                  if (wcsicmp( nodeName, ownerNode ) == 0)
                  {
                     // Node is active
                     active = true;
                  }
                  else
                  {
                     // Node is passive
                     active = false;
                  }

                  delete [] nodeName;
               }
            }
         }

         (void) CloseClusterGroup(groupHandle);
      }

      (void) CloseCluster(clusterHandle);
   }
   // Cluster not available on AP-DE. Servicetype always master on AP1
   // for debug compilation
#ifdef _DEBUG
   active = true;
#endif
*/
   return active;
}


bool CS_ProtocolChecker::checkIfAP1()
{
    bool isAP1 = true;

	char * envVar = getenv("CS_AP1");

	// Test settings used during testing/development
	if(envVar != 0)
	{
	    isAP1 = strcmp(envVar, "1") == 0;
	}
	// Normal checks
	else if (CS_ProtocolChecker::apNumber != ACS_CS_Protocol::AP_NotSpecified)
   {
      if (CS_ProtocolChecker::apNumber == ACS_CS_Protocol::AP_1)

         isAP1 = true;

      else

         isAP1 = false;
   }
   else
   {
      // To find out which AP we are running on we try to bind to the IP addresses
      // for AP1, one by one.
      uint32_t ipA1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_A_HOSTNAME);
      uint32_t ipB1 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_A_HOSTNAME);
      uint32_t ipA2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_A_ETH_B_HOSTNAME);
      uint32_t ipB2 = ACS_CS_HostFile::getIPAddress(AP_1_NODE_B_ETH_B_HOSTNAME);

      bool addressIsLocal = false;

      addressIsLocal = CS_ProtocolChecker::checkAddress(ipA1, 0);

      if ( ! addressIsLocal)
         addressIsLocal = CS_ProtocolChecker::checkAddress(ipB1, 0);

      if ( ! addressIsLocal)
         addressIsLocal = CS_ProtocolChecker::checkAddress(ipA2, 0);

      if ( ! addressIsLocal)
         addressIsLocal = CS_ProtocolChecker::checkAddress(ipB2, 0);

      // If bind succeeded for one of the addresses above then we are on AP1.
      // If the bind failed, then we are on AP2-16.
      if ( ! addressIsLocal )
      {
         // We are on AP2-16. CS should then always run as a slave.
         CS_ProtocolChecker::apNumber = ACS_CS_Protocol::AP_2_16;
         isAP1 = false;
      }
      else
      {
         // We are on AP1.
         CS_ProtocolChecker::apNumber = ACS_CS_Protocol::AP_1;
         isAP1 = true;
      }
   }

	ACS_CS_TRACE((ACS_CS_Protocol_TRACE,
		"ACS_CS_Protocol::checkIfAP1()\n"
		"isAP1 = %s", isAP1 ? "true" : "false"));

	return isAP1;
}


bool CS_ProtocolChecker::checkAddress(uint32_t address, uint16_t port)
{
   bool addressAccessible = false;

   // To find out which AP we are running on we try to bind to the IP addresses
   // for AP1, one by one. We create an address structure and set the IP address
   // to AP1's A-plane addresses and then the B-plane addresses. The test order
   // is therefore 192.168.169.1, 192.168.169.2, 192.168.170.1 and 192.168.170.2
   // The port is always the CS service port (14015)

   ACS_CS_TCPServer tcpServer;
   tcpServer.addTcpServer(address, port);
   int result = tcpServer.startListen();

   if (result)
	   addressAccessible = true;
   else
	   addressAccessible = false;

   return addressAccessible;
}


//--------------
// getAPaddress
// calculate the AP address using
//--------------
uint32_t CS_ProtocolChecker::getAPaddress(CS_Side_Identifier side, uint16_t sysNo, CS_Network_Identifier eth) {
   uint32_t ipaddress = 0; //AP address
   uint16_t x = 0, y = 0; //indexes to access to the AP address table

   if ((sysNo >= 1) && (sysNo <= NO_OF_AP)) x = sysNo - 1;
   else return 0; //error

   if (eth == Eth_A) y = 0;
   else if (eth == Eth_B) y = 1;
   else return 0;

   if (side == Side_A) ipaddress = nodeA_AXE10IP[x][y];
   else if (side == Side_B) ipaddress = nodeB_AXE10IP[x][y];
   else return 0;

   return ipaddress;
}


//--------------
// getCPaddress
// calculate the CP address for APZ2123x 
//--------------
uint32_t CS_ProtocolChecker::getCPaddress(CS_Side_Identifier side, CS_Network_Identifier eth) {
	uint32_t ipaddress = 0; //CP address
	uint16_t y = 0; //index to access to the CP address table

	if (eth == Eth_A) y = 0;
	else if (eth == Eth_B) y = 1;
	else return 0;

	if (side == Side_A) ipaddress = APZ_2123XIP[0][y];
	else if (side == Side_B) ipaddress = APZ_2123XIP[1][y];
	else return 0;

	return ipaddress;
}

//--------------
// getMAUaddress
// calculate the MAU address for APZ2123x 
//--------------
uint32_t CS_ProtocolChecker::getMAUaddress(CS_Network_Identifier eth) {
	uint32_t ipaddress = 0; //CP address
	uint16_t y = 0; //index to access to the CP address table

	if (eth == Eth_A) y = 0;
	else if (eth == Eth_B) y = 1;
	else return 0;

	ipaddress = MAU_2123XIP[y];
	
	
	return ipaddress;
}



uint16_t CS_ProtocolChecker::getHeaderSize(ACS_CS_Protocol::CS_Protocol_Type protocolType)
{
	if (protocolType == ACS_CS_Protocol::Protocol_CS)
	{
		return ACS_CS_Protocol::CS_Header_Size;
	}
	else if (protocolType == ACS_CS_Protocol::Protocol_MODD)
	{
		return ACS_CS_Protocol::MODD_Header_Size;
	}
	else
	{
		return 0;
	}
}

bool CS_ProtocolChecker::checkMauType(uint16_t identifier)
{
   return ((identifier == ACS_CS_Protocol::MAUB)
           || (identifier == ACS_CS_Protocol::MAUS));
}
