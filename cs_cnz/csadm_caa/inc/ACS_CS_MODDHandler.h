

//	-
//	Copyright Ericsson AB 2011. All rights reserved.
//	-


#ifndef ACS_CS_MODDHandler_h
#define ACS_CS_MODDHandler_h 1

#include "ACS_CS_TableHandler.h"


#include "ace/RW_Mutex.h"
#include <vector>
#include <map>

#include "ACS_CS_Event.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_PTBPeerHandler.h"

class ACS_CS_PDU;
class ACS_CS_DHCP_Info;
class ACS_CS_MODDHandler;

struct Default_HWC_Entry {
	ACS_CS_Protocol::CS_FBN_Identifier fbn;
	uint16_t slot;
	uint32_t magazine;
	ACS_CS_Protocol::CS_SystemType_Identifier sysType;
	uint16_t sysNum;
	ACS_CS_Protocol::CS_Side_Identifier side;
};

struct Default_DHCP_Entry {
	const std::string hostName;
	const std::string description;
	const std::string bootImage;
	const uint32_t ipAddress;
    char mac[6];
    const ACS_CS_Protocol::CS_Network_Identifier network;
	const std::string uid;
    
    void setMAC(const char new_mac[6])
    {
        memset(mac,0,6);
        memcpy(mac, new_mac, 6 * sizeof(char));
    }
};


//APZ2123X entries

static const Default_HWC_Entry DEFAULT_HWC_CPUBA = {
		ACS_CS_Protocol::FBN_CPUB,
		97,
		0x63636363,
		ACS_CS_Protocol::SysType_CP,
		1,
		ACS_CS_Protocol::Side_A
};

static const Default_HWC_Entry DEFAULT_HWC_CPUBB = {
		ACS_CS_Protocol::FBN_CPUB,
		98,
		0x63636363,
		ACS_CS_Protocol::SysType_CP,
		1,
		ACS_CS_Protocol::Side_B
};

static const Default_HWC_Entry DEFAULT_HWC_MAUBA = {
		ACS_CS_Protocol::FBN_MAUB,
		99,
		0x63636363,
		ACS_CS_Protocol::SysType_CP,
		1,
		ACS_CS_Protocol::Side_B
};

static const Default_DHCP_Entry DEFAULT_DHCP_IPN0 = {
		"ipn0",
		"IPN0-AP Lan1",
		"boot.ipn0",
		0xc0a8a980,	// 192.168.169.128
		//{0x49, 0x50, 0x4e, 0x41, 0x30, 0x30 },
		{},
		ACS_CS_Protocol::Eth_A,
		"\01""IPNA00" // requirement: prefix the uid with 0x01
};

static const Default_DHCP_Entry DEFAULT_DHCP_IPN1 = {
		"ipn1",
		"IPN1-AP Lan2",
		"boot.ipn1",
		0xc0a8aa80,	// 192.168.170.128
		//{0x49, 0x50, 0x4e, 0x41, 0x30, 0x31 },
		{},
		ACS_CS_Protocol::Eth_B,
		"\01""IPNA01" // requirement: prefix the uid with 0x01
};

static const Default_DHCP_Entry DEFAULT_DHCP_IPN2 = {
		"ipn2",
		"IPN2-AP Lan1",
		"boot.ipn2",
		0xc0a8a982,	// 192.168.169.130
		//{0x49, 0x50, 0x4e, 0x41, 0x30, 0x32 },
		{},
		ACS_CS_Protocol::Eth_A,
		"\01""IPNA02" // requirement: prefix the uid with 0x01
};

static const Default_DHCP_Entry DEFAULT_DHCP_IPN3 = {
		"ipn3",
		"IPN3-AP Lan2",
		"boot.ipn3",
		0xc0a8aa82,	// 192.168.170.130
		//{0x49, 0x50, 0x4e, 0x41, 0x30, 0x33 },
		{},
		ACS_CS_Protocol::Eth_B,
		"\01""IPNA03" // requirement: prefix the uid with 0x01
};

//APZ21250 entries.

static Default_DHCP_Entry DEFAULT_DHCP_PCIH_A_Own = {
		"PCIH_A_Own",
		"PCIH_A-AP Lan1",
		"/boot/image/a/pxelinux.0",
		0xc0a8a982,	// 192.168.169.130
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, //temporary for testing
		ACS_CS_Protocol::Eth_A,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_PCIH_A_Twin = {
		"PCIH_A_Twin",
		"PCIH_A-AP Lan2",
		"/boot/image/a/pxelinux.0",
		0xc0a8aa82,	// 192.168.170.130
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, //temporary for testing
		ACS_CS_Protocol::Eth_B,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_PCIH_B_Own = {
		"PCIH_B_Own",
		"PCIH_B-AP Lan2",
		"/boot/image/b/pxelinux.0",
		0xc0a8aa83,	// 192.168.170.131
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x03}, //temporary for testing
		ACS_CS_Protocol::Eth_B,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_PCIH_B_Twin = {
		"PCIH_B_Twin",
		"PCIH_B-AP Lan1",
		"/boot/image/b/pxelinux.0",
		0xc0a8a983,	// 192.168.169.131
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}, //temporary for testing
		ACS_CS_Protocol::Eth_A,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_CPSB_A_Eth0 = {
		"CPSB_A_Eth0",
		"CPSB_A-AP Lan1",
		"/boot/image/a/elilo.efi",
		0xc0a8a980,	// 192.168.169.128
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x05}, //temporary for testing
		ACS_CS_Protocol::Eth_A,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_CPSB_A_Eth1 = {
		"CPSB_A_Eth1",
		"CPSB_A-AP Lan2",
		"/boot/image/a/elilo.efi",
		0xc0a8aa80,	// 192.168.170.128
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x06}, //temporary for testing
		ACS_CS_Protocol::Eth_B,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_CPSB_B_Eth0 = {
		"CPSB_B_Eth0",
		"CPSB_B-AP Lan2",
		"/boot/image/b/elilo.efi",
		0xc0a8aa81,	// 192.168.170.129
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x07}, //temporary for testing
		ACS_CS_Protocol::Eth_B,
		""
};

static Default_DHCP_Entry DEFAULT_DHCP_CPSB_B_Eth1 = {
		"CPSB_B_Eth1",
		"CPSB_B-AP Lan1",
		"/boot/image/b/elilo.efi",
		0xc0a8a981,	// 192.168.169.129
		{},
		// {0x00, 0x00, 0x00, 0x00, 0x00, 0x08}, //temporary for testing
		ACS_CS_Protocol::Eth_A,
		""
};


//APZ21240 entries

static const Default_DHCP_Entry DEFAULT_DHCP_EIB_A = {
		"eib_a",
		"CP-AP Lan1",
		"/boot/image/boot_image_cpa",
		0xc0a8a980,	// 192.168.169.128
		{0x00, 0x01, 0xec, 0xf6, 0x42, 0xba},
		ACS_CS_Protocol::Eth_A,
		""
};

static const Default_DHCP_Entry DEFAULT_DHCP_EIC_A = {
		"eic_a",
		"CP-AP Lan2",
		"/boot/image/boot_image_cpa",
		0xc0a8aa80,	// 192.168.170.128
		{0x00, 0x01, 0xec, 0xf6, 0x42, 0xbb},
		ACS_CS_Protocol::Eth_B,
		""
};

static const Default_DHCP_Entry DEFAULT_DHCP_EIB_B = {
		"eib_b",
		"CP-AP Lan4",
		"/boot/image/boot_image_cpb",
		0xc0a8aa81,	// 192.168.170.129
		{0x00, 0x01, 0xec, 0xf6, 0x42, 0xb6},
		ACS_CS_Protocol::Eth_B,
		""
};

static const Default_DHCP_Entry DEFAULT_DHCP_EIC_B = {
		"eic_b",
		"CP-AP Lan3",
		"/boot/image/boot_image_cpb",
		0xc0a8a981,	// 192.168.169.129
		{0x00, 0x01, 0xec, 0xf6, 0x42, 0xb7},
		ACS_CS_Protocol::Eth_A,
		""
};


//GESB static entries for APZ21240 and APZ21250

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_I_A_ETH0 = {
		"GESB-I-A-0",
		"Internal network plane A (eth0)",
		"",
		0xc0a8a984,	// 192.168.169.132
		{},
		ACS_CS_Protocol::Eth_A,
		//"255.255.15.1:eth0"
		"003235352e3235352e31352e313a65746830"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_E_A_ETH0 = {
		"GESB-E-A-0",
		"External network plane A (eth0)",
		"",
		0xc0a8a986,	// 192.168.169.134
		{},
		ACS_CS_Protocol::Eth_A,
		//"255.255.15.3:eth0"
		"003235352e3235352e31352e333a65746830"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_E_B_ETH0 = {
		"GESB-E-B-0",
		"External network plane B (eth0",
		"",
		0xc0a8a987,	// 1192.168.169.135
		{},
		ACS_CS_Protocol::Eth_A,
		//"255.255.15.21:eth0"
		"003235352e3235352e31352e32313a65746830"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_I_B_ETH0 = {
		"GESB-I-B-0",
		"Internal network plane B (eth0)",
		"",
		0xc0a8a985,	// 192.168.169.133
		{},
		ACS_CS_Protocol::Eth_A,
		//"255.255.15.23:eth0"
		"003235352e3235352e31352e32333a65746830"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_I_A_ETH1 = {
		"GESB-I-A-1",
		"Internal network plane A (eth1)",
		"",
		0xc0a8aa84,	// 192.168.170.132
		{},
		ACS_CS_Protocol::Eth_B,
		//"255.255.15.1:eth1"
		"003235352e3235352e31352e313a65746831"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_E_A_ETH1 = {
		"GESB-E-A-1",
		"External network plane A (eth1)",
		"",
		0xc0a8aa86,	// 192.168.170.134
		{},
		ACS_CS_Protocol::Eth_B,
		//"255.255.15.3:eth1"
		"003235352e3235352e31352e333a65746831"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_E_B_ETH1 = {
		"GESB-E-B-1",
		"External network plane B (eth1)",
		"",
		0xc0a8aa87,	// 192.168.170.135
		{},
		ACS_CS_Protocol::Eth_B,
		//"255.255.15.21:eth1"
		"003235352e3235352e31352e32313a65746831"
};

static const Default_DHCP_Entry DEFAULT_DHCP_GESB_I_B_ETH1 = {
		"GESB-I-B-1",
		"Internal network plane B (eth1)",
		"",
		0xc0a8aa85,	// 192.168.170.133
		{},
		ACS_CS_Protocol::Eth_B,
		//"255.255.15.23:eth1"
		"003235352e3235352e31352e32333a65746831"
};
//------------------------------------------------------

/** @class ACS_CS_RevertTimer
 *	@brief Internal class to run the timer thread for automatic revert of boot file changes
 *	@author xminaon
 */

class ACS_CS_RevertTimer: public ACS_CS_Thread
{
	public:
		ACS_CS_RevertTimer(ACS_CS_MODDHandler * handler, const Default_DHCP_Entry * entry, uint16_t timeout);
		virtual int exec();
		void cancelRevertTimer();
	private:
		ACS_CS_MODDHandler * handler;
		const Default_DHCP_Entry * entry;
		uint16_t timeout;
		ACS_CS_EventHandle timerHandle;
		ACS_CS_EventHandle cancelHandle;
};


class ACS_CS_MODDHandler: public ACS_CS_TableHandler
{
	typedef std::map<std::string, ACS_CS_RevertTimer *> maptype;

	public:
    
    enum APZ_Type {
        APZ_UNDEF   = 0,
        APZ2123X    = 1,
        APZ21240    = 2,
        APZ21250    = 3,
        APZ21255_60 = 4
    };

      ACS_CS_MODDHandler(uint32_t bootIpEthA, uint32_t bootIpEthB, APZ_Type = APZ_UNDEF);

      virtual ~ACS_CS_MODDHandler();

      int handleRequest(ACS_CS_PDU *pdu);

      void updateDhcpAPZ2123X();

      void addAPZ2123xEntry();

      void clearAPZ2123xEntry();

	  virtual bool saveTable () const {return true;}

	  virtual bool loadTable () {return true;}

	  friend class ACS_CS_RevertTimer;


	  void updateDhcpAPZ21250();

	  void updateDhcpAPZ21250(const MACMap &macMap, ACS_CS_PTBPeerHandler::PTBPeer side);

	  void updateDhcpAPZ21240();

	  void addStaticGESBEntries();

	  void addHwcClassicEntries();

	  void clearAPZ21240Entry();

	  void clearAPZ21250Entry();

	  void clearGESBEntries();

	  void clearImmEntries();

  private:

      ACS_CS_MODDHandler(const ACS_CS_MODDHandler &right);

      ACS_CS_MODDHandler & operator=(const ACS_CS_MODDHandler &right);

      int handleChange(ACS_CS_PDU *pdu);

      int handleRevert(ACS_CS_PDU *pdu);

      int createResponse (ACS_CS_PDU *pdu,
    		  ACS_CS_Protocol::CS_Primitive_Identifier type,
    		  ACS_CS_Protocol::MODD_Result_Code result,
    		  std::string description);

      bool addRemoveHwcEntry(const Default_HWC_Entry &entry, ACS_CS_ImModel *tempModel, bool add);

      bool getDhcpInfo(ACS_CS_DHCP_Info * info, const Default_DHCP_Entry * entry, uint32_t bootIp) const;

      const Default_DHCP_Entry * getDhcpEntry(const char * mac, int size) const;

      void performAutomaticRevert(const Default_DHCP_Entry &entry);

      void createRevertTimer(const Default_DHCP_Entry * entry, uint16_t timeout);

      void cancelRevertTimer(const Default_DHCP_Entry * entry);

      void timerCleanup();

	  int normalize(char r);
	  void hexCharToBinChar(unsigned char *input,char *out);

	  int getRealMac(std::string ss,char *out);

	  void hexCharToBinChar(unsigned char *input,char *out, int size);

	  int getRealMac(std::string ss,char *out, int size);


      // Private variables
	  ACE_RW_Mutex _lock;

	  uint32_t bootIpEthA;

	  uint32_t bootIpEthB;

	  maptype timerMap;

	  std::vector<ACS_CS_RevertTimer *> oldTimers;
    
      APZ_Type apzType;
    

};


// Class ACS_CS_MODDHandler 



#endif
