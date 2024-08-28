// ACS_CS_Protocol.h
//	 Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Protocol_h
#define ACS_CS_Protocol_h

/*
        IMPORTANT!

        This header file contains constants for the CS protocol. The accompanying cpp file
        ACS_CS_Protocol.cpp contains static methods that can be used for range checking.
        When new constants are added both this header file and the cpp file must be updated.

*/

#include <set>
#include <string>
#include <stdint.h>


namespace ACS_CS_Protocol {

   // Numeric constants for use in the CS protocol
   static const uint16_t DEFAULT_REQUEST_ID = 14567;
   static const uint16_t REQUEST_ID_SYNC = 0xFFFC;
   static const uint16_t REQUEST_ID_ACK = 0xFFFD;
   static const uint16_t REQUEST_ID_START = 0xFFFE;
   static const uint16_t REQUEST_ID_END = 0xFFFF;

   // Primtive offset used for the MODD protocol
   static const uint16_t MODD_Primitive_Offset = 900;

   // Protocol in use CS or MODD
   enum CS_Protocol_Type {
	   Protocol_NotSpecified = 0,
	   Protocol_CS = 1,
	   Protocol_MODD = 2
   };

   // Common constants
   enum CS_Protocol_Constants {
	   MODD_Header_Size = 8,
	   CS_Header_Size = 12

   };

   // GEP2/GEP5 to GEP7 HWSWAP. Added Version 7
   // HWC Version
   enum CS_Version_Identifier {
      Version_NotSpecified = 0,
      Version_1 = 1,
      Version_2 = 2,
      Version_3 = 3,
	  Version_4 = 4,
	  Version_5 = 5,
	  Version_6 = 6,
      Version_7 = 7,
	  Latest_Version = 7
   };

   enum CS_VLANTable_Version_Identifier {
	   VLANVersion_0 = 0,
	   VLANVersion_1 = 1,
	   VLANVersion_2 = 2
   };

   // HWC Scopes
   enum CS_Scope_Identifier {
      Scope_NotSpecified 			= 0,
      Scope_HWCTable 				= 1,
      Scope_CPIdTable 				= 2,
      Scope_CPGroupTable 			= 3,
      Scope_FDTable 				= 4,
      Scope_NE 						= 5,
      Scope_VLAN         			= 6,
      Scope_Subscription 			= 7,
      Scope_ComputeResourceTable 	= 8,
      Scope_MODD 					= 99,
     Scope_GlobalLogicalClock = 11,
   };

   // HWC Primitives
   enum CS_Primitive_Identifier {
      // Common
      Primitive_NotSpecified = 0,

	//Global logical Clock
	Primitive_GetGlobalLogicalClock = 1,
	Primitive_GetGlobalLogicalClockResponse = 2,

      // HWC
      Primitive_GetValue = 10,
      Primitive_GetValueResponse = 11,
      Primitive_GetBoardList = 12,
      Primitive_GetBoardListResponse = 13,
//      Primitive_SetValue = 20,
//      Primitive_SetValueResponse = 21,
//      Primitive_AddEntry = 22,
//      Primitive_AddEntryResponse = 23,
//      Primitive_RemoveEntry = 24,
//      Primitive_RemoveEntryResponse =	25,
      Primitive_SynchTable = 26,
      Primitive_SynchTableResponse = 27,

      // CP Identification
      Primitive_GetCPId =					200,
      Primitive_GetCPIdResponse =			201,
      Primitive_GetCPName =				202,
      Primitive_GetCPNameResponse =		203,
      Primitive_GetAPZSystem =			204,
      Primitive_GetAPZSystemResponse =	205,
      Primitive_GetCPList =				206,
      Primitive_GetCPListResponse =		207,
      Primitive_GetCPType =				208,
      Primitive_GetCPTypeResponse =		209,
//      Primitive_SetCPAlias =				210,
//      Primitive_SetCPAliasResponse =		211,
//      Primitive_SetAPZSystem =			212,
//      Primitive_SetAPZSystemResponse =	213,
//      Primitive_SetCPType =				214,
//      Primitive_SetCPTypeResponse =		215,
      Primitive_GetCPAliasName =			216,
      Primitive_GetCPAliasNameResponse =	217,
      Primitive_GetCPState                 = 218,
      Primitive_GetCPStateResponse         = 219,
      Primitive_GetApplicationId           = 220,
      Primitive_GetApplicationIdResponse   = 221,
      Primitive_GetApzSubstate             = 222,
      Primitive_GetApzSubstateResponse     = 223,
      Primitive_GetStateTransCode          = 224,
      Primitive_GetStateTransCodeResponse  = 225,
      Primitive_GetAptSubstate             = 226,
      Primitive_GetAptSubstateResponse     = 227,
//      Primitive_SetQuorumDataEntry         = 228,
//      Primitive_SetQuorumDataEntryResponse = 229,
      Primitive_GetBlockingInfo            = 230,
      Primitive_GetBlockingInfoResponse    = 231,
      Primitive_GetCPCapacity              = 232,
      Primitive_GetCPCapacityResponse      = 233,
      Primitive_GetMAUType		   = 234,
      Primitive_GetMAUTypeResponse         = 235,

      // CP Group
      Primitive_GetGroupNames =           300,
      Primitive_GetGroupNamesResponse =   301,
      Primitive_GetGroupMembers =         302,
      Primitive_GetGroupMembersResponse = 303,
//      Primitive_DefineGroup =             304,
//      Primitive_DefineGroupResponse =     305,
//      Primitive_RemoveGroup =             306,
//      Primitive_RemoveGroupResponse =     307,
//      Primitive_ClearOG =                 308,
//      Primitive_ClearOGResponse =         309,

      // Functional Distribution
      Primitive_GetFunctionList =                        400,
      Primitive_GetFunctionListResponse =                401,
      Primitive_GetFunctionProviders =                   402,
      Primitive_GetFunctionProvidersResponse =           403,
      Primitive_GetFunctionUsers =                       404,
      Primitive_GetFunctionUsersResponse =               405,
//      Primitive_DefineFunctionalDistribution =           406,
//      Primitive_DefineFunctionalDistributionResponse =   407,
//      Primitive_RemoveFunctionalDistribution =           408,
//      Primitive_RemoveFunctionalDistributionResponse =   409,

      // Network Element
      Primitive_GetCPSystem                      = 500,
      Primitive_GetCPSystemResponse              = 501,
      Primitive_GetNEId                          = 502,
      Primitive_GetNEIdResponse                  = 503,
      Primitive_GetAlarmMaster                   = 504,
      Primitive_GetAlarmMasterResponse           = 505,
      Primitive_GetClockMaster                   = 506,
      Primitive_GetClockMasterResponse           = 507,
      Primitive_GetBSOMIPAddress                 = 508,
      Primitive_GetBSOMIPAddressResponse         = 509,
      Primitive_GetTestEnvironment               = 510,
      Primitive_GetTestEnvironmentResponse       = 511,
//      Primitive_SetNEId                          = 512,
//      Primitive_SetNEIdResponse                  = 513,
//      Primitive_SetAlarmMaster                   = 514,
//      Primitive_SetAlarmMasterResponse           = 515,
//      Primitive_SetClockMaster                   = 516,
//      Primitive_SetClockMasterResponse           = 517,
//      Primitive_SetClusterOpMode                 = 518,
//      Primitive_SetClusterOpModeResponse         = 519,
      Primitive_GetClusterOpMode                 = 520,
      Primitive_GetClusterOpModeResponse         = 521,
      Primitive_GetOmProfile                     = 524,
      Primitive_GetOmProfileResponse             = 525,

      // Network Element Traffic PDUs
//      Primitive_SetTrafficLeader =           526,
//      Primitive_SetTrafficLeaderResponse =   527,
      Primitive_GetTrafficLeader =           528,
      Primitive_GetTrafficLeaderResponse =   529,
//      Primitive_SetTrafficIsolated =           530,
//      Primitive_SetTrafficIsolatedResponse =   531,
      Primitive_GetTrafficIsolated =           532,
      Primitive_GetTrafficIsolatedResponse =   533,

//      Primitive_SetProfile                       = 534,
//      Primitive_SetProfileResponse               = 535,
//      Primitive_SetOmProfileNotification         = 536,
//      Primitive_SetOmProfileNotificationResponse = 537,

      // Note a distinct lack of a response, as this is internal only.
//      Primitive_SetInternalOmProfile = 538,

	  // VLAN
	  Primitive_GetVLANList =                600,
	  Primitive_GetVLANListResponse =        601,
	  Primitive_GetVLANAddress =             602,
	  Primitive_GetVLANAddressResponse =     603,
	  Primitive_GetVLANTable =		         604,
	  Primitive_GetVLANTableResponse =	     605,
	  Primitive_GetNetworkTable =		     606,
	  Primitive_GetNetworkTableResponse =	 607,
      
      // Subscription / Notify
      Primitive_SubscribeTable                      = 700,
      Primitive_SubscribeTableResponse              = 701, // not used
      Primitive_SubscribeProfilePhaseChange         = 702,
      Primitive_SubscribeProfilePhaseChangeResponse = 703, // not used

      Primitive_NotifyCpidTableChange    = 750,
      Primitive_NotifyNeTableChange      = 751,
      Primitive_NotifyProfilePhaseChange = 752,
      Primitive_NotifyCpGroupTableChange = 753,
      Primitive_NotifyHWCTableChange	 = 754,

      // MODD protocol
	  Primitive_MODD_Change				= 900,		// These constants are in the MODD protocol
	  Primitive_MODD_ChangeResponse		= 901,		// 0-3, not 900-903
	  Primitive_MODD_Revert				= 902,
	  Primitive_MODD_RevertResponse		= 903,

      // Replication
//      Primitive_ACKTable =                   1000
   };

   // HWC Attributes
   enum CS_Attribute_Identifier {
      // Common attribute
      Attribute_NotSpecified =             0,

      // HWC Table
      Attribute_Magazine           = 1,
      Attribute_Slot               = 2,
      Attribute_SysType            = 3,
      Attribute_SysNo              = 4,
      Attribute_FBN                = 5,
      Attribute_Side               = 6,
      Attribute_SeqNo              = 7,
      Attribute_IP_EthA            = 8,
      Attribute_IP_EthB            = 9,
      Attribute_Alias_EthA         = 10,
      Attribute_Alias_EthB         = 11,
      Attribute_Netmask_Alias_EthA = 12,
      Attribute_Netmask_Alias_EthB = 13,
      Attribute_DHCP               = 14,
      Attribute_SysId              = 15,
      Attribute_Current_Load_Module_Version =16,
      Attribute_Blade_Product_Number =17,
      Attribute_SwVer_Type 	   = 18,
      Attribute_Blade_Uuid         =19,
      Attribute_Sync_Flag          = 97,
      Attribute_MAC_EthA           = 98, // These are internal attributes not to be used
      Attribute_MAC_EthB           = 99, // by external clients. They are used for MAC-
      // addresses for the boards when the HWC table
      // is saved to disk.


      // CP Identification Table
      Attribute_CPId_Name =         201,
      Attribute_CPId_Alias =        202,
      Attribute_CPId_APZ_System =   203,
      Attribute_CPId_CP_Type =      204,
      Attribute_CPId_CPState = 205,
      Attribute_CPId_ApplicationId = 206,
      Attribute_CPId_ApzSubstate = 207,
      Attribute_CPId_StateTransCode = 208,
      Attribute_CPId_AptSubstate = 209,
      Attribute_CPId_BlockingInfo = 210,
      Attribute_CPId_CPCapacity = 211,
	
      Attribute_CPId_MAU_Type = 212,


      // CP Group Table
      Attribute_CPGroup_GroupIdentifier = 301,
      Attribute_CPGroup_GroupName =       302,
      Attribute_CPGroup_CPIdentity =      303,
      Attribute_CPGroupMembers_CPIdentities = 304,
      Attribute_CPGroup_CPIds = 305, //internal use only

      // Internal attributes used by CS
      // Used for notifying the service the dissolve has been
      // called
      Attribute_CPGroup_ClearOgCode    = 399,

      // Functional Distribution Table
      Attribute_FD_APIdentifier =      401,
      // 402 intentionally left out. It was used for function name in temporary release
      Attribute_FD_DomainName =        403,
      Attribute_FD_ServiceName =       404,
      Attribute_FD_GroupIdentifier =   405,
      Attribute_FD_CPIdentity =        406,

      // Network Element Table
      Attribute_NE_AlarmMaster =       501,
      Attribute_NE_ClockMaster =       502,
      Attribute_NE_NetworkIdentifier = 503,
      Attribute_NE_ClusterOpMode =     504,
      Attribute_NE_TrafficIsolated =   505,
      Attribute_NE_TrafficLeader =     506,
      Attribute_NE_OmProfileCurrent    = 507,
      Attribute_NE_OmProfileRequested  = 508,
      Attribute_NE_AptProfileCurrent   = 509,
      Attribute_NE_AptProfileRequested = 510,
      Attribute_NE_ApzProfileCurrent   = 511,
      Attribute_NE_ApzProfileRequested = 512,
      Attribute_NE_Phase               = 513,
      Attribute_NE_ProfileChangeReason = 514,
      Attribute_NE_ApzProfileQueued    = 515,
      Attribute_NE_AptProfileQueued    = 516,

      // Used by CS internally to handle
      // timeout for changing Cluster Op Mode
      Attribute_NE_ClusterOpModeType   = 599,

      // VLAN Table
      Attribute_VLAN_Identifier =         601,
      Attribute_VLAN_Name =               602,
      Attribute_VLAN_NetworkAddress =     603,
      Attribute_VLAN_Netmask =            604,
      Attribute_VLAN_Type =               605,
      Attribute_VLAN_AllocatedAddress =   606,
	Attribute_VLAN_pcp = 		  607,
	Attribute_VLAN_vlanType=            608,
      Attribute_VLAN_Tag =                609,
      Attribute_VLAN_VNICName =           610,


      SystemProperty_StartupProfileSupervisionComplete      = 901,
      SystemProperty_CancelClusterOpModeSupervisionTimer    = 902,
      SystemProperty_CancelOmProfileSupervisionTimer        = 903,
      SystemProperty_SetOmProfileSupervisionTimer           = 904,
      SystemProperty_SetPhaseSubscriberCount                = 905,
      SystemProperty_NeSubscriberCount                      = 906,
      SystemProperty_InitiateUpdatePhaseChange              = 907

   };

   // HWC System types
   enum CS_SystemType_Identifier {
      SysType_BC =					0,
      SysType_CP =					1000,
      SysType_AP =					2000,
	  SysType_IPLB =				3000,
      SysType_NotSpecified =		0xFFFE
   };

   // HWC Functional Board Name Identifiers
   enum CS_FBN_Identifier {
      FBN_NotSpecified =	0,
      FBN_SCBRP =			100,
      FBN_RPBIS =			110,
      FBN_GESB =			120, //CR GESB
      FBN_CPUB =			200,
      FBN_MAUB =			210,
      FBN_APUB =			300,
      FBN_Disk =			310,
      FBN_DVD	=			320,
      FBN_GEA	=			330,
      FBN_SCXB	=			340,
      FBN_IPTB	=			350,
      FBN_EPB1	=			360,
      FBN_EvoET =			370,
      FBN_CMXB =			380,
      FBN_IPLB =			390,
	  FBN_SMXB =			400
   };




   // HWC Side Identifiers
   enum CS_Side_Identifier {
      Side_A = 0,
      Side_B = 1,
      Side_NotSpecified = 99
   };

   // HWC DHCP Methods
   enum CS_DHCP_Method_Identifier {
      DHCP_None =                          0,
      DHCP_Normal =                1,
      DHCP_Client =                2,
      DHCP_NotSpecified =	99
   };

   // HWC Result Codes
   enum CS_Result_Code {
      Result_Success                = 0,
      Result_Version_Not_Supported  = 1,
      Result_Attribute_Not_Suported = 2,
      Result_No_Such_Entry          = 3,
      Result_No_Value               = 4,
      Result_Busy                   = 10,
      Result_Bad_Format             = 13,
      Result_Other_Failure          = 15,
      Result_Incorrect_String       = 16,
      Result_Incorrect_CP           = 17,
      Result_Incorrect_AP           = 18,
      Result_Duplicate_Definition   = 19,
      Result_Reserved_Name          = 20,
      Result_Not_Removable          = 21,
      Result_No_Master_Access       = 22,
      Result_Incorrect_Service_Name = 23,
      Result_Incorrect_Domain_Name  = 24,
      Result_Incorrect_Mode         = 25,
      Result_ClusterOpMode_Switch_In_Progress = 26,
      Result_Missing_File           = 27,
      Result_Awaiting_Dependency    = 28,
      Result_Profile_Switch_In_Progress = 29,
      Result_NotSpecified           = 99
   };

   enum MODD_Result_Code {
	   MODD_Result_Success 				 = 0,
	   MODD_Result_Version_Not_Supported = 1,
	   MODD_Result_No_Such_Entry		 = 2,
	   MODD_Result_Other_Failure		 = 3,
	   MODD_Result_Busy					 = 4,
	   MODD_Result_NotSpecified			 = 99
   };

   // HWC Side Identifiers
   enum CS_Network_Identifier {
      Network_NotSpecified = 0,
      Eth_A = 1,
      Eth_B =	2
   };

   // APZ Type Identifiers
   enum CS_APZ_Type_Identifier {
      APZ_Undef = 0,
      APZ_21255 = 1,
      APZ_21401 =	2,
      APZ_NotSpecified =	99
   };

   // CP System Identifiers
   enum CS_CP_System_Identifier {
      System_Single_CP = 0,
      System_Multiple_CP = 1,
      System_NotSpecified = 99
   };

   // CP System Identifiers
   enum CS_TestEnvironment_Identifier {
      TestEnvironment_FALSE = 0,
      TestEnvironment_TRUE = 1,
      TestEnvironment_NotSpecified = 99
   };

   enum CS_VLAN_Stack_Identifier {
      Stack_KIP = 0,
      Stack_TIP = 1,
      Stack_NotSpecified = 99,
      Stack_Undefined = 0xFFFF
   };

enum CS_VLAN_Type_Identifier {
	Type_APZ = 0,
	Type_APT = 1,
	Type_Undefined = -1
};

enum CS_VLAN_PCP {
	MIN_PCP = 0,
	DEFAULT_PCP = 6,
	MAX_PCP = 7
};

enum CS_VLAN_TAG {
	DEFAULT_VLAN_TAG = 0,
	MAX_VLAN_TAG = 4095
};

   enum CS_Service_Type {
      ServiceType_MASTER = 0,
      ServiceType_SLAVE = 1,
      ServiceType_NotSpecified = 2
   };

   // AP Identifiers
   enum CS_AP_Number {
      AP_NotSpecified = 0,
      AP_1 = 1,
      AP_2_16 = 2
   };

   // Sync flags
   enum CS_Sync_Flag {
	 No_sync = 0,
	 Do_sync = 1
  };

   // Sync flags
  enum CS_StartupProfileSupervisionComplete {
	 SupervisionInProgress = 0,
	 SupervisionCompleted = 1
  };

   // MAU  Type Identifiers
   enum CS_MAU_Type_Identifier {
      	UNDEFINED = 0,
	MAUB = 1,
	MAUS = 2
   };

   typedef enum SwitchType
	{
	CONTROL_SWITCH = 0,
	TRANSPORT_SWITCH = 1,
	NOT_APPLICABLE = 2
	}SwitchType_values;
//   static const char * CS_Service_Type_String[] = {"Master", "Slave", "NotSpecified"};

   /*
     This is a singleton class with static functions. The class contains utility functions
     to be used with the protocol
   */
   class CS_ProtocolChecker {
   public:
      // Range checking functions
      static bool checkVersion(uint16_t version);
	static bool checkVlanTableVersion(uint16_t version);
      static bool checkScope(uint16_t scope);
      static bool checkPrimitive(uint16_t primitive, uint16_t scope);
      static bool checkAttribute(uint16_t attribute);
      static bool checkAttribute(uint16_t attribute, uint16_t scope);
      static bool checkSysType(uint16_t sysType);
      static bool checkFBN(uint16_t fbn);
      static bool checkSide(uint16_t side);
      static bool checkDHCP(uint16_t dhcp);
      static bool checkResultCode(uint16_t resultCode);
      static bool checkMODDResultCode(uint16_t resultCode);
      static bool checkAPZSystem(uint16_t system);
      static bool checkCPType(uint16_t type);
      static bool checkTestEnvironment(uint16_t identifier);
      static bool checkCPSystem(uint16_t system);
      static bool checkMauType(uint16_t type);

      // Conversion functions
      static std::string getFBN(uint16_t fbn);
      static std::string binToString(char * binaryData, int length);
      static ACS_CS_Protocol::CS_Side_Identifier getNode();
      static ACS_CS_Protocol::CS_Service_Type getServiceType();
      static void checkServiceType();
      static bool checkIfActiveNode();
      static bool checkIfAP1();
      static bool checkAddress(uint32_t address, uint16_t port);
      static uint32_t getAPaddress(CS_Side_Identifier side, uint16_t sysNo, CS_Network_Identifier eth);
	  static uint32_t getCPaddress(CS_Side_Identifier side, CS_Network_Identifier eth); //to get the static ip address of the CP (APG43 connected to APZ2123x)
	  static uint32_t getMAUaddress(CS_Network_Identifier eth); //to get the static ip address of the MAU (APG43 connected to APZ2123x)
	  static uint16_t getHeaderSize(ACS_CS_Protocol::CS_Protocol_Type protocolType);

   protected:
      CS_ProtocolChecker();
      static void createInstance();

   private:
      static CS_ProtocolChecker * _instance;
      static ACS_CS_Protocol::CS_Side_Identifier apSide;
      static ACS_CS_Protocol::CS_AP_Number apNumber;
      static ACS_CS_Protocol::CS_Service_Type csServiceType;
      std::set<uint16_t> hwcPrimitiveSet;
      std::set<uint16_t> cpIdPrimitiveSet;
      std::set<uint16_t> cpGroupPrimitiveSet;
      std::set<uint16_t> funcDistPrimitiveSet;
      std::set<uint16_t> neIdPrimitiveSet;
      std::set<uint16_t> subscriptionPrimitiveSet;
      std::set<uint16_t> vlanIdPrimitiveSet;
      std::set<uint16_t> replicationPrimitiveSet;
      std::set<uint16_t> hwcAttributeSet;
      std::set<uint16_t> cpIdAttributeSet;
      std::set<uint16_t> cpGroupAttributeSet;
      std::set<uint16_t> funcDistAttributeSet;
      std::set<uint16_t> neAttributeSet;
      std::set<uint16_t> vlanAttributeSet;
      std::set<uint16_t> allScopesAttributeSet;
      std::set<uint16_t> fbnSet;
      std::set<uint16_t> scopeSet;
	std::set<uint16_t> globalClockPrimitiveSet;
	std::set<uint16_t> globalClockAttributeSet;
      std::set<uint16_t> resultCodeSet;

      // MODD protocol
      std::set<uint16_t> moddPrimitiveSet;
      std::set<uint16_t> moddResultCodeSet;
   }; // Class

} // End of namespace


#endif
