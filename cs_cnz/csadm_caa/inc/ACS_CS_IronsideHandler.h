/*
 * ACS_CS_IronsideHandler.h
 *
 *  Created on: Oct 4, 2012
 *      Author: estevol
 */

#ifndef ACS_CS_IRONSIDEHANDLER_H_
#define ACS_CS_IRONSIDEHANDLER_H_
 #include <arpa/inet.h>
 #include <netinet/in.h>
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <unistd.h>
#include "ACS_CS_ShelfManager.h"
class ACS_CS_ServiceHandler;

class ACS_CS_IronsideHandler: public ACS_CS_ShelfManager {

public:

	ACS_CS_IronsideHandler (ACS_CS_ServiceHandler* ParentService);

	virtual ~ACS_CS_IronsideHandler();

	virtual void addShelfHandler(ACS_CS_DHCP_Info &infoA, ACS_CS_DHCP_Info &infoB);

	virtual void removeShelfHandler(uint32_t magazine, uint16_t slot);

	virtual bool syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector);

	int getShelfidFromPhysicalAddress(std::string physical_Address, std::string &shelfid, bool onlyCached = false);

	int getPhysicalAddressFromShelfId(std::string shelfId);

	std::string getMagAddressFromShelfId(std::string shelfId);

	std::string getMagAddressFromShelfId(int shelfId);

protected:

	void addToMac(unsigned char * mac, int size, int add);

	int hexCharToBinChar(const char *input,char *out,int add);

	virtual void cleanUp();

	virtual void checkOutQueue ();

	virtual int updateTimeout();

	int magAddrToPhysicalAddr(std::string shelfaddr);

	std::string physicalAddrToMagAddr(int phyAddr);

	void updateRetries(const char* hostName);

	void incrementRetryNo();

	int getMacaddress(int slot ,string Shelf_Id,string &mac);

	int getProductInfo(int slot, string Shelf_Id, ACS_CS_DHCP_Info &info);

private:
	std::map<int,std::string> shelfMap;

	ACE_Recursive_Thread_Mutex syncObject;

	int m_retryNo;

};

#endif /* ACS_CS_IRONSIDEHANDLER_H_ */

