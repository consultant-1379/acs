
#ifndef ACS_CS_DHCP_Info_h
#define ACS_CS_DHCP_Info_h

#include "ACS_CS_Protocol.h"

struct ACS_CS_ProductInfo
{
	std::string productRevision;
	std::string productName;
	std::string manufacturingDate;
	std::string productVendor;
	std::string productNumber;
	std::string serialNumber;

	ACS_CS_ProductInfo& operator=(const ACS_CS_ProductInfo &rhs)
	{
		productRevision = rhs.productRevision;
		productName = rhs.productName;
		manufacturingDate = rhs.manufacturingDate;
		productVendor = rhs.productVendor;
		productNumber = rhs.productNumber;
		serialNumber = rhs.serialNumber;
		return *this;
	}
};

class ACS_CS_DHCP_Info 
{

	 public:
		// Constructors
		ACS_CS_DHCP_Info();

		ACS_CS_DHCP_Info(const ACS_CS_DHCP_Info &right);
		ACS_CS_DHCP_Info (uint32_t ip, char *mac, int length, std::string comment);

		// Destructor
		virtual ~ACS_CS_DHCP_Info();

		ACS_CS_DHCP_Info & operator=(const ACS_CS_DHCP_Info &right);
		int operator==(const ACS_CS_DHCP_Info &right) const;
		int operator!=(const ACS_CS_DHCP_Info &right) const;
		int operator<(const ACS_CS_DHCP_Info &right) const;
		int operator>(const ACS_CS_DHCP_Info &right) const;
		int operator<=(const ACS_CS_DHCP_Info &right) const;
		int operator>=(const ACS_CS_DHCP_Info &right) const;

		uint32_t getIP () const;
		void getMAC (char *mac, int size) const;
		int getMacLength () const;
		std::string getComment () const;
		std::string getHostName () const;
		void getClientId (char *clientId, int size) const;
		int getClientIdLength() const;
		std::string getIPString () const;
		ACS_CS_Protocol::CS_Network_Identifier getNetwork () const;
		ACS_CS_Protocol::CS_DHCP_Method_Identifier getDHCPMethod () const;
		uint32_t getMagAddress () const;
		void calcOption61Id (int fbn = ACS_CS_Protocol::FBN_NotSpecified, ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE) ;
		void setIP (uint32_t ip);
		void setMAC (const char *mac, int size);
		void setComment (std::string comment);
		void setHostName (std::string hostName);
		void setDHCPMethod (ACS_CS_Protocol::CS_DHCP_Method_Identifier method);
		void setMagAddress (uint32_t address);
		void setSlot (uint16_t slot);
		void setNetwork (ACS_CS_Protocol::CS_Network_Identifier network);
		void setSCBRP (bool isBoardSCBRP);
		bool isSCBRP () const;
		uint16_t getSlot () const;
		bool isMacInitialized () const;
		void setFBN (ACS_CS_Protocol::CS_FBN_Identifier newFbn);
		ACS_CS_Protocol::CS_FBN_Identifier getFBN () const;
		bool isPersistent () const;
		void setPersistent (bool persistent);
		unsigned long getScope () const;
		void setScope (unsigned long newScope);
		ACS_CS_Protocol::CS_Side_Identifier getSide () const;
		void setSide (ACS_CS_Protocol::CS_Side_Identifier newSide);
		uint16_t getSystemType () const;
		void setSystemType (uint16_t type);
		uint16_t getSysId () const;
		void setSysId (uint16_t id);
		std::string getBootServer() const;
		void setBootServer(std::string server);
		std::string getBootPath() const;
		void setBootPath(std::string path);
		std::string generateHostname(ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::NOT_APPLICABLE);
		void setClientId (const char *uid, int size);
		int figureOutClientId (std::string productId, int a0, int a1, int a3, int slotid, int eth01, std::string &client, ACS_CS_Protocol::SwitchType switchType = ACS_CS_Protocol::CONTROL_SWITCH);		
	    std::string getContainerPackage () const;
        void setContainerPackage (const std::string &package);

        std::string getBladeProductNumber() const;
        void setBladeProductNumber (const std::string &product);
		
        std::string getCurrentLoadModuleVersion() const;
        void setCurrentLoadModuleVersion (const std::string &product);

        std::string getLoadModuleFileName() const;
        void setLoadModuleFileName (const std::string &product);

        ACS_CS_ProductInfo getProductInfo() const;
        void setProductInfo (const ACS_CS_ProductInfo &hwVer);

//        void setProductRevision (const std::string &revision);
//        std::string getProductRevision () const;
        std::string getDomainName () const;
        //void setDomainName (const std::string &domain);
        void calcOption61IdEpb ();
        int checkNumeric (char *str);

        void setAcAddress(uint32_t ip) ;
        uint32_t getAcAddress() const;
        void setTrapReceiverAddress(uint32_t ip);
        uint32_t getTrapReceiverAddress() const;
        void nextTries();
        int getTries() const;
        void resetTries();




	private:
		std::string convertAddress(uint32_t address) const;
		uint32_t ipAddress;
		char *macAddress;
		int macLength;
		char * clientId;
		int clientIdLength;
		std::string hostName;
		std::string hostComment;
		ACS_CS_Protocol::CS_DHCP_Method_Identifier dhcpMethod;
		uint32_t magazineAddress;
		uint16_t slotPosition;
		ACS_CS_Protocol::CS_Network_Identifier networkIdentifier;
		bool isSCBRPBoard;
		ACS_CS_Protocol::CS_FBN_Identifier fbn;
		bool boardPersistent;
		long unsigned scope;
		ACS_CS_Protocol::CS_Side_Identifier side;
		uint16_t systemType;
		uint16_t sysId;
		std::string bootServer;
		std::string bootPath;
		std::string containerPackage;
		std::string bladeProductNumber;
		std::string currentLoadModuleVersion;
		std::string loadModuleFileName;
		std::string domainName;
		bool fromTrap;
		unsigned long counterRetry;
		int tries;
		uint32_t acIp;
		uint32_t trapReceiver;
		ACS_CS_ProductInfo productInfo;

		std::string zeropad(std::string str,int length);


};

#endif
