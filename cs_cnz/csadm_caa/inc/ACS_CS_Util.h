// ACS_CS_Util.h
// Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Util_h
#define ACS_CS_Util_h 1

// Needed for uint16_t etc
#include <string>
#include <stdint.h>
#include <iostream>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

// Various constants

namespace ACS_CS_NS
{
   // General constants
   static const uint16_t entryIdLowerBound = 1;    // Start for entry identifiers
   static const uint16_t entryIdUpperBound = 65535;   // Unsigned short max
   
   // Value used when no TrafficLeader or TrafficIsolated CPID is defined.
   static const int UNDEFINED_CPID = 255;

   // Timers
   static const int MILLISECONDS_PER_SECOND = 1000;
   static const int ONE_HUNDRED_NANOSECONDS_PER_MILLISECOND = 10000;
   static const int ONE_HUNDRED_NANOSECONDS_PER_SECOND = 10000000;
   static const int INITIALIZATION_DELAY = 60 * MILLISECONDS_PER_SECOND;

   static const int PHASE_TIMEOUT_IN_SECONDS = 60;
   static const int PHASE_COMMIT_TIMEOUT_IN_SECONDS = 30;
   static const int CLUSTEROPMODE_TIMEOUT_IN_SECONDS = 900;
   static const int PHASE_NO_TIMEOUT = 2;

   static const unsigned int CS_Max_Threads = 512;      // Maximum no of threads
   
   // Name of service
   static const char ACS_CS_PROCESS_NAME[] = "ACS_CS_Service";
   
   // Name of daemon
   static const char ACS_CS_DAEMON[] = "acs_csd";

   // Name of user
   static const char ACS_CS_USER[] = "root";

   static const char PATH_DHCP_FILE[] = "/var/lib/dhcp/db/dhcpd.leases";


   static const std::string ACS_DATA_PATH = "/data/acs/data/cs";

   static const std::string ACS_CONF_PATH = "/opt/ap/acs/conf";

   static const std::string TFTP_PATH = "/data/apz/data";

   static const std::string COMPUTE_TFTP_PATH = TFTP_PATH + "/compute";

   static const char TURBO_BOOST_FILE[] = "/storage/system/config/apos/turbo_boost_cp";

   //QuorumData file
   static const std::string QUORUM_DATA_FILE = "QuorumData";
   static const std::string PATH_QUORUM_DATA_FILE = ACS_DATA_PATH + "/" + QUORUM_DATA_FILE;

   //Static IP file
   static const std::string STATIC_IP_FILE_NAME = "StaticIp";
   static const std::string STATIC_IP_FILE = ACS_DATA_PATH + "/" + STATIC_IP_FILE_NAME;

   //ComputeResource file
   static const std::string COMPUTE_RESOURCE_FILE_NAME = "ComputeResource";
   static const std::string COMPUTE_RESOURCE_FILE = ACS_DATA_PATH + "/" + COMPUTE_RESOURCE_FILE_NAME;

   //APA alarm file /* HW99445 */
   static const std::string APA_ALARM_FILE_NAME = "APAalarm";
   static const std::string APA_ALARM_FILE = ACS_DATA_PATH + "/" + APA_ALARM_FILE_NAME;

   // Constants used by events and mutexes
   static const char EVENT_NAME_SHUTDOWN[]            = "ACS_CS_SHUTDOWN_EVENT";
   static const char EVENT_NAME_SHUTDOWN_SERVICE_HANDLER[]    = "ACS_CS_SHUTDOWN_SERVICE_HANDLER_EVENT";
   static const char EVENT_NAME_REMOVE_CLOSED_CONNECTIONS[]   = "ACS_CS_REMOVE_CLOSED_CONNECTIONS_EVENT";
   static const char EVENT_NAME_IMM_UPDATED[]   	  = "EVENT_NAME_IMM_UPDATED";
   static const char EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SERVICE[]	= "EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SERVICE";
   static const char EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SEC[]	= "EVENT_NAME_IMM_DISPATCH_FAIL_NOTIFY_SEC";
   static const char EVENT_NAME_CONNECTION[]          = "ACS_CS_TCP_CONNECTION_EVENT";
   static const char EVENT_NAME_UPDATE_DHCP[]         = "ACS_CS_UPDATE_DHCP_EVENT";
   static const char EVENT_NAME_SNMP_SYNC[]           = "ACS_CS_SNMP_SYNC_EVENT";
   static const char EVENT_NAME_REPLICATION_SYNC[]    = "ACS_CS_REPLICATION_SYNC_EVENT";
   static const char EVENT_NAME_REPLICATION_MASTER[]  = "ACS_CS_REPLICATION_EVENT_MASTER";
   static const char EVENT_NAME_REPLICATION_HWC[]     = "ACS_CS_REPLICATION_EVENT_HWC";
   static const char EVENT_NAME_REPLICATION_CPID[]    = "ACS_CS_REPLICATION_EVENT_CPID";
   static const char EVENT_NAME_REPLICATION_CPGROUP[] = "ACS_CS_REPLICATION_EVENT_CPGROUP";
   static const char EVENT_NAME_REPLICATION_FD[]      = "ACS_CS_REPLICATION_EVENT_FD";
   static const char EVENT_NAME_REPLICATION_NE[]      = "ACS_CS_REPLICATION_EVENT_NE";
   static const char EVENT_NAME_REPLICATION_VLAN[]    = "ACS_CS_REPLICATION_EVENT_VLAN";
   static const char EVENT_NAME_SIGNATURE_UPDATE[]    = "ACS_CS_SIGNATURE_UPDATE_EVENT";
   static const char EVENT_NAME_CLUSTER_ALIGNED[]   	  = "EVENT_NAME_CLUSTER_ALIGNED";
   static const char EVENT_NAME_STATICIP_ALIGNED[]   	  = "EVENT_NAME_STATICIP_ALIGNED";
   static const char MUTEX_NAME_DHCP_PROTECTION[]     = "ACS_CS_DHCP_MUTEX";
   static const char MUTEX_NAME_ENTRYCOUNTER_PROTECTION[]     = "ACS_CS_ENTRYCOUNTER_MUTEX";
   
   // Name requirements
   static const uint16_t MAX_CP_NAME_LENGTH           = 7;
   static const uint16_t MAX_CP_GROUP_NAME_LENGTH     = 7;
   static const uint16_t MAX_NEID_LENGTH              = 23;
   static const uint16_t MAX_FD_DOMAIN_NAME_LENGTH    = 7;
   static const uint16_t MAX_FD_SERVICE_NAME_LENGTH   = 15;
   static const uint16_t MAX_VLAN_NAME_LENGTH         = 31;
   static const uint16_t MAX_VLAN_ID_RANGE            = 4095;
   static const uint16_t MAX_VNIC_NAME_LENGTH         = 15;

   // CP group table predefined groups
   static const char CP_GROUP_ALL[]       = "ALL";
   static const char CP_GROUP_ALLBC[]     = "ALLBC";
   static const char CP_GROUP_OPGROUP[]   = "OPGROUP";
   
   static const uint16_t SEQNO_LOW  = 0;     // Lower range for seqNo
   static const uint16_t SEQNO_HIGH = 999;   // Upper range for seqNo
   
   static const uint16_t SYSNO_LOW  = 0;     // Lower range for sysNo
   static const uint16_t SYSNO_HIGH = 999;   // Upper range for sysNo
   
   // SYS id values
   static const uint16_t ILLEGAL_SYSID = 0xFFFE;   // 65534
   static const uint16_t FD_ALL_CP     = 0xFFFF;   // 65535

   // Constants used by SNMP
   static const char COMMUNITY_READ_ONLY[]   = "public";
   static const char COMMUNITY_READ_WRITE[]  = "private";
   static const int SNMP_Online_Timeout      = 2500;     // Timeout 2,5 s (SNMP resend timeout online)
   static const int SNMP_Offline_Timeout     = 30000;    // Timeout 30 s  (SNMP resend timeout offline)
   static const int SNMP_Agent_Timeout       = 15000;    // Timeout 15 s  (SNMP agent down timeout)
   static const unsigned short UDP_Port_SNMP = 161;
   
   // Contants used by the CS protocol
   static const unsigned short CS_Server_Port       = 14015;   // Port for CS clients
   static const unsigned short CS_Replication_Port  = 14014;   // Port for CS replication
   static const unsigned short CS_Subscription_Port = 14017;   // Port for CS subscription
   static const unsigned short MODD_Protocol_Port 	= 6798;	   // Port used by MODD protocol in APZ 212 3X
   static const int Mutex_Timeout            = 1000;     // Timeout 1 s   (Mutex deadlock prevention)
   static const int Daemon_Timeout			 = 5000;	 // Timeout 5 s, used by daemon
   static const int Connection_Timeout       = 2500;     // Timeout 2.5 s (TCP connection timeout)
   static const int MODD_Connection_Timeout  = 5000;     // Timeout 5 s (TCP connection timeout used by MODD protocol)
   static const int Send_Timeout             = 2500;     // Timeout 2.5 s (TCP send timeout)
   static const int Recv_Get_Timeout         = 10000;    // Timeout 10 s  (TCP receive timeout for get functions)
   static const int Recv_Set_Timeout         = 10000;    // Timeout 10 s  (TCP receive timeout for set functions)
   static const int HWCSYNC_Timeout          = 10000;    // Timeout 10 s  (timeout for hwcsync command)
   static const int Master_Poll_Interval     = 5;        // Interval between polling for mastership (in seconds)
   
   // DHCP constants
   static const char DHCP_Server_IP[]      	  = "127.0.0.1";
   static const uint16_t DHCP_Server_Port     = 7911;
   static const uint32_t DHCP_Scope_EthA      = 0xC0A8A900;  // 192.168.169.0
   static const uint32_t DHCP_Scope_EthB      = 0xC0A8AA00;  // 192.168.170.0
   static const uint32_t DEFAULT_BOOT_IP_ETHA = 0xC0A8A901;
   static const uint32_t DEFAULT_BOOT_IP_ETHB = 0xC0A8AA01;
   
   //Cluster IP
   static const uint32_t CLUSTER_IP_ETHA  =  0xC0A8A921;//192.168.169.33
   static const uint32_t CLUSTER_IP_ETHB  =  0xC0A8AA21;//192.168.170.33

   // Server IP constants
   static const uint32_t IP_Not_Specified    = 0x0;         // 0.0.0.0
   static const uint32_t Server_Local_IP     = 0x7F000001;  // 127.0.0.1
   
   // Host names for AP1
   static const char AP_1_NODE_A_ETH_A_HOSTNAME[] = "ap1a-l1";
   static const char AP_1_NODE_A_ETH_B_HOSTNAME[] = "ap1a-l2";
   static const char AP_1_NODE_B_ETH_A_HOSTNAME[] = "ap1b-l1";
   static const char AP_1_NODE_B_ETH_B_HOSTNAME[] = "ap1b-l2";
   
   static const char SCBRP_FOR_TEST[] = "scb_rp_for_test";
   static const char AP1_FOR_TEST[] = "ap1_for_test";

   // Boot information for CPU boards in APZ 212 55/blade
   static const char TFTP_CPUBA_BOOT_FOLDER[]    = "/boot/image/a/"; // Boot folder for A-side CPUB (and default)
   static const char TFTP_CPUBB_BOOT_FOLDER[]    = "/boot/image/b/"; // Boot folder for B-side CPUB
   static const char TFTP_BCA_BOOT_FOLDER[]    = "/CPA/"; // Boot folder for A-side CPUB (and default)
   static const char TFTP_BCB_BOOT_FOLDER[]    = "/CPB/"; // Boot folder for B-side CPUB
   static const char TFTP_CPUB_NATIVE_BOOT_FILENAME[] = "pxelinux.0";          // Boot file name
   static const char TFTP_CPUB_VIRT_BOOT_FILENAME[] = "vpxelinux.0";          // Boot file name
   static const char TFTP_CPUB_BOOT_CONFIG_FILENAME[] = "pxelinux.cfg/default";          // Boot config file name

   static const char TFTP_IPLB_BOOT_FOLDER[]    = "/IPLB/default/"; // Boot folder for IPLB A and B
   static const char TFTP_IPLB_BOOT_IPXE_PATH[]    = "/IPLB/default/boot.ipxe";

   static const char DHCP_CPUBA_BOOT_PATH[]    = "/boot/image/a/pxelinux.0"; // Boot path for A-side CPUB (and default)
   static const char DHCP_CPUBB_BOOT_PATH[]    = "/boot/image/b/pxelinux.0"; // Boot path for B-side CPUB
   static const char DHCP_BCA_BOOT_PATH[]    = "/CPA/pxelinux.0"; // Boot path for A-side CPUB (and default)
   static const char DHCP_BCB_BOOT_PATH[]    = "/CPB/pxelinux.0"; // Boot path for B-side CPUB

   static const char DHCP_ETHA_NODEA_BOOT_IP[] = "192.168.169.1";       // Boot IP backplane A Node A
   static const char DHCP_ETHB_NODEA_BOOT_IP[] = "192.168.170.1";       // Boot IP backplane B Node A
   static const char DHCP_ETHA_NODEB_BOOT_IP[] = "192.168.169.2";       // Boot IP backplane A Node B
   static const char DHCP_ETHB_NODEB_BOOT_IP[] = "192.168.170.2";       // Boot IP backplane B Node B
   static const char DHCP_ETHA_BOOT_IP[] = "192.168.169.3";       // Boot IP backplane A Node B
   static const char DHCP_ETHB_BOOT_IP[] = "192.168.170.3";       // Boot IP backplane B Node B
   
   // Used by Table class
   static const unsigned short ILLEGAL_TABLE_ENTRY = 0xFFFE;
   
   // ACS_CS_HWCHandler constants
   static const uint32_t ethALowerBound   = 0xC0A8A938;  // 192.168.169.56
   static const uint32_t ethBLowerBound   = 0xC0A8AA38;  // 192.168.170.56
   
   static const uint32_t ethAUpperBound   = 0xC0A8A9FD;  // 192.168.169.253
   static const uint32_t ethBUpperBound   = 0xC0A8AAFD;  // 192.168.170.253

   static const uint32_t ethAStaticLowerBound = 0xC0A8A97F;  // 192.168.169.127
   static const uint32_t ethBStaticLowerBound = 0xC0A8AA7F;  // 192.168.170.127

   static const uint32_t ethAStaticUpperBound = 0xC0A8A987;  // 192.168.169.135
   static const uint32_t ethBStaticUpperBound = 0xC0A8AA87;  // 192.168.170.135
   
   static const uint32_t ethANetmask      = 0xFFFFFF00;  // 255.255.255.0
   static const uint32_t ethBNetmask      = 0xFFFFFF00;  // 255.255.255.0
   
   // NE data constants
   static const uint16_t DEFAULT_ALARM_MASTER = 1000;
   static const uint16_t DEFAULT_CLOCK_MASTER = 1000;
   
   static const uint32_t BSOMIPAddressEthA = 0xC0A8A9FE;  // 192.168.169.254
   static const uint32_t BSOMIPAddressEthB = 0xC0A8AAFE;  // 192.168.170.254
   
   static const unsigned short ENTRY_ID_ALARM_MASTER       = 1;
   static const unsigned short ENTRY_ID_CLOCK_MASTER       = 2;
   static const unsigned short ENTRY_ID_NETWORK_IDENTIFIER = 3;
   static const unsigned short ENTRY_ID_CLUSTER_OP_MODE    = 4;
   static const unsigned short ENTRY_ID_OM_PROFILE         = 5;
   static const unsigned short ENTRY_ID_TRAFFIC_ISOLATED   = 6;
   static const unsigned short ENTRY_ID_TRAFFIC_LEADER     = 7;
   static const unsigned short ENTRY_ID_SYSTEM_PROPERTIES  = 8;

   // Constants used by VLAN PHA
   static const char CXC_BLOCK_AND_NAME[]      = "ACS/CXC1371283";
   static const char VLAN_NAMED_TABLE[]        = "ACS_CSBIN_VLANTable";
   static const char VLAN_APZ_A[]              = "APZ-A";
   static const char VLAN_APZ_B[]              = "APZ-B";
   static const char VLAN_BGCI_A[]             = "BGCI-A";
   static const char VLAN_BGCI_B[]             = "BGCI-B";
   static const char VLAN_SOL_A[]              = "APZ-SOL-A";
   static const char VLAN_SOL_B[]              = "APZ-SOL-B";
   static const uint32_t VLAN_APZ_A_NETWORK_ADDR    = 0xC0A8A900;  // 192.168.169.0
   static const uint32_t VLAN_APZ_B_NETWORK_ADDR    = 0xC0A8AA00;  // 192.168.170.0
   static const uint32_t VLAN_APZ_A_NETMASK         = 0xFFFFFF00;  // 255.255.255.0
   static const uint32_t VLAN_APZ_B_NETMASK         = 0xFFFFFF00;  // 255.255.255.0
   static const uint16_t VLAN_APZ_A_PROTOCOL_STACK = 0;           // "KIP"
   static const uint16_t VLAN_APZ_B_PROTOCOL_STACK = 0;           // "KIP"
   
   // The AP IP-addresses are fixed according to the tables below.
   const int NO_OF_AP=16;
   const int SLOT_OFFSET=100;//to store ipAliasA and ipAlaiasB for SMXB (SLOT_OFFSET+slot) in static IP
/*RoGa   static const char * nodeA_dotIP[NO_OF_AP][2] =
   {
      {"192.168.169.1","192.168.170.1"},
      {"192.168.169.3","192.168.170.3"},
      {"192.168.169.5","192.168.170.5"},
      {"192.168.169.7","192.168.170.7"},
      {"192.168.169.9","192.168.170.9"},
      {"192.168.169.11","192.168.170.11"},
      {"192.168.169.13","192.168.170.13"},
      {"192.168.169.15","192.168.170.15"},
      {"192.168.169.17","192.168.170.17"},
      {"192.168.169.19","192.168.170.19"},
      {"192.168.169.21","192.168.170.21"},
      {"192.168.169.23","192.168.170.23"},
      {"192.168.169.25","192.168.170.25"},
      {"192.168.169.27","192.168.170.27"},
      {"192.168.169.29","192.168.170.29"},
      {"192.168.169.31","192.168.170.31"},
   }; RoGa*/
	static const int Amplify_Timeout				= 2;		// Amlify the timeout to wait for Thread is terminated.
	static const int DHCP_Online_Timeout		= 5000;			// Timeout 5 s
/*RoGa    
   static const char * nodeB_dotIP[NO_OF_AP][2] = {
      {"192.168.169.2","192.168.170.2"},
      {"192.168.169.4","192.168.170.4"},
      {"192.168.169.6","192.168.170.6"},
      {"192.168.169.8","192.168.170.8"},
      {"192.168.169.10","192.168.170.10"},
      {"192.168.169.12","192.168.170.12"},
      {"192.168.169.14","192.168.170.14"},
      {"192.168.169.16","192.168.170.16"},
      {"192.168.169.18","192.168.170.18"},
      {"192.168.169.20","192.168.170.20"},
      {"192.168.169.22","192.168.170.22"},
      {"192.168.169.24","192.168.170.24"},
      {"192.168.169.26","192.168.170.26"},
      {"192.168.169.28","192.168.170.28"},
      {"192.168.169.30","192.168.170.30"},
      {"192.168.169.32","192.168.170.32"},
   }; RoGa*/

    const static uint32_t nodeA_AXE10IP[NO_OF_AP][2] = { //little endian IPs
      {0xC0A8A901,0xC0A8AA01},
      {0xC0A8A903,0xC0A8AA03},
      {0xC0A8A905,0xC0A8AA05},
      {0xC0A8A907,0xC0A8AA07},
      {0xC0A8A909,0xC0A8AA09},
      {0xC0A8A90B,0xC0A8AA0B},
      {0xC0A8A90D,0xC0A8AA0D},
      {0xC0A8A90F,0xC0A8AA0F},
      {0xC0A8A911,0xC0A8AA11},
      {0xC0A8A913,0xC0A8AA13},
      {0xC0A8A915,0xC0A8AA15},
      {0xC0A8A917,0xC0A8AA17},
      {0xC0A8A919,0xC0A8AA19},
      {0xC0A8A91B,0xC0A8AA1B},
      {0xC0A8A91F,0xC0A8AA1F},
      {0xC0A8A91F,0xC0A8AA1F},
    };
   
    const static uint32_t nodeB_AXE10IP[NO_OF_AP][2] = { //little endian IPs
      {0xC0A8A902,0xC0A8AA02},
      {0xC0A8A904,0xC0A8AA04},
      {0xC0A8A906,0xC0A8AA06},
      {0xC0A8A908,0xC0A8AA08},
      {0xC0A8A90A,0xC0A8AA0A},
      {0xC0A8A90C,0xC0A8AA0C},
      {0xC0A8A90E,0xC0A8AA0E},
      {0xC0A8A910,0xC0A8AA10},
      {0xC0A8A912,0xC0A8AA12},
      {0xC0A8A914,0xC0A8AA14},
      {0xC0A8A916,0xC0A8AA16},
      {0xC0A8A918,0xC0A8AA18},
      {0xC0A8A91A,0xC0A8AA1A},
      {0xC0A8A91C,0xC0A8AA1C},
      {0xC0A8A91E,0xC0A8AA1E},
      {0xC0A8A920,0xC0A8AA20},
    };
	
	// The CP IP-addresses are fixed for APZ2123x according to the table below.
	const int NO_OF_CP=2;

	static const uint32_t APZ_2123XIP[NO_OF_CP][2] = { //little endian IPs
		{0xC0A8A980,0xC0A8AA80},
		{0xC0A8A981,0xC0A8AA81},
	};
	// The MAU IP-addresses are fixed for APZ2123x according to the table below.
	const int NO_OF_MAU=2;

	static const uint32_t MAU_2123XIP[NO_OF_MAU] = { //little endian IPs
		0xC0A8A97F,0xC0A8AA7F
	};

	// Used in replication to define payload
	typedef enum {
	    Unspecified   = 0,
	    NewTables	  = 1,		// Payload is new set of tables (startup syncronization)
	    NewOperation  = 2,		// Payload is new TableOperation
	    ENTRY_OK      = 3,		// Earlier entry ok (no payload)
	    ENTRY_NOK     = 4		// Earlier entry not ok (no payload)
	} ACS_CS_ReplicationType;

	// Used as header in replication
	typedef struct {
	    int size;						// Size of replication data excluding this struct
	    ACS_CS_ReplicationType type;	// Type of replication data
	    int version;					// Version of replication data
	} ACS_CS_ReplicationData;

	static const int UNSPECIFIED_VERSION = 0;
	
	const int perm_640 = (const int) fs::owner_read | fs::owner_write | fs::group_read;

	const int perm_700 = (const int) fs::owner_read | fs::owner_write | fs::owner_exe;

	enum perm{
		NO_PERM		=	fs::no_perms, //0
		PERM_640	=	perm_640, //0640	
		PERM_700	=	perm_700, //0700
		PERM_ALL	=	fs::all_all //0777
	};
	
	static const std::string cert_owner = "ldap";
	static const std::string cert_group = "sec-credu-users";

	class ACS_CS_Util {
	public:
		inline ACS_CS_Util(){};
		virtual ~ACS_CS_Util();

		static bool setRecursivePermission(const std::string &path, perm permissions);

		static bool setRecursivePermission(const std::string &path, perm p_dir_permissions, perm p_file_permissions, const std::string &owner, const std::string &group);

		static void splitString(std::string &first, std::string & second, const std::string & input, const std::string & separator);


	private:


	};

}

#endif
