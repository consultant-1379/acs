
//	Copyright Ericsson AB 2007, 2010. All rights reserved.

#ifndef ACS_CS_SNMPHandler_h
#define ACS_CS_SNMPHandler_h 1

#include <queue>
#include <vector>
#include <ace/Semaphore.h>
#include <ace/RW_Mutex.h>

#include "ACS_CS_Event.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"



class ACS_CS_ServiceHandler;
class ACS_CS_DHCP_Info;
class ACS_CS_SNMPManager;

namespace ACS_CS_SNMP
{
	static const char OID_MAC_AGG[] = "1.3.6.1.4.1.193.154.2.1.2.1.1.1.2";
	static const char OID_SCX_MAC_AGG[] = "1.3.6.1.4.1.193.177.2.2.1.3.1.1.1.20";
	static const char OID_BLADE_PRODUCT_NUMBER[] = "1.3.6.1.4.1.193.177.2.2.1.3.1.1.1.2";
}

class ACS_CS_SNMPHandler : public ACS_CS_Thread
{

  public:

      ACS_CS_SNMPHandler (uint32_t ipAddressEthA, uint32_t ipAddressEthB,
    		  	  	  	  uint16_t udpPort, std::string roCommunity, std::string rwCommunity,
    		  	  	  	  ACS_CS_ServiceHandler* ParentService/*, std::vector<ACS_CS_DHCP_Info *> *clientOutQueue*/);

      virtual ~ACS_CS_SNMPHandler();

      virtual int exec ();

      void sendGetMAC (const ACS_CS_DHCP_Info &client);

      bool syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector);

      void cancelGetMac(const ACS_CS_DHCP_Info &client);

      void insertIntoClientOutQueue (const ACS_CS_DHCP_Info &client);

      void wakeUpGetMac ();

      bool isMaster(int slot);

  private:

      ACS_CS_SNMPHandler(const ACS_CS_SNMPHandler &right);

      ACS_CS_SNMPHandler & operator=(const ACS_CS_SNMPHandler &right);

      bool sendGetRequest (ACS_CS_DHCP_Info *info);

      void checkOutQueue ();

      //IpAddress getScbRpAddress();
      bool getScbRpAddress(std::string *strIp);

      void addToMac(char * mac, int size, int add);

      void removeCanceledClients();

      ACE_Semaphore *m_pSem;

      uint32_t scbRpEthA;

      uint32_t scbRpEthB;

      uint32_t scbRpActiveAddress;

      short unsigned agentPort;

      std::string readOnlyCommunity;

      std::string readWriteCommunity;

      ACE_RW_Mutex * mutexHandle;

      ACE_RW_Mutex * queueMutex;

      //std::vector<ACS_CS_DHCP_Info *> *clientOutQueue;

      std::vector<ACS_CS_DHCP_Info *> clientOutQueue;

      std::queue<ACS_CS_DHCP_Info *> canceledClients;

      ACS_CS_EventHandle updateSNMPEvent;

      ACS_CS_EventHandle shutdownEvent;

      bool changeScbRpAddress;

      ACS_CS_ServiceHandler* serviceCs; /* pointer to service handler, will be used for acting on successful
       MAC retrieval*/

      bool outQueueEmpty;

      bool isCBA;

      ACS_CS_SNMPManager *snmpMan;

      //uint32_t getIsCbaAddress(ACS_CS_DHCP_Info * info);
      std::string getIPString (uint32_t ipAddress);
      bool ulongToStringMagazine (uint32_t uvalue, std::string &value);

      void addToMac(unsigned char * mac, int size, int add);
      int normalize(char r);
      int hexCharToBinChar(unsigned char *input,char *out,int add);

};


#endif
