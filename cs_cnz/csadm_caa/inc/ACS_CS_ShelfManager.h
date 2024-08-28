/*
 * ACS_CS_ShelfManager.h
 *
 *  Created on: Oct 4, 2012
 *      Author: estevol
 */

#ifndef ACS_CS_SHELFMANAGER_H_
#define ACS_CS_SHELFMANAGER_H_

#include <queue>
#include <vector>
#include <ace/Semaphore.h>
#include <ace/RW_Mutex.h>

#include "ACS_CS_Event.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_ReaderWriterLock.h"

class ACS_CS_DHCP_Info;
class ACS_CS_ServiceHandler;

class ACS_CS_ShelfManager: public ACS_CS_Thread {
public:
	ACS_CS_ShelfManager(ACS_CS_ServiceHandler*);

	virtual ~ACS_CS_ShelfManager();

	void insertIntoDhcpQueue (const ACS_CS_DHCP_Info & client);

	void wakeUpQueue();

	void cancelClient(const ACS_CS_DHCP_Info & infoA,const ACS_CS_DHCP_Info & infoB);

	int exec();

	void putAtBottomIntoDhcpQueue (const ACS_CS_DHCP_Info & client);

	virtual void addShelfHandler(ACS_CS_DHCP_Info &infoA, ACS_CS_DHCP_Info &infoB) = 0;

	virtual void removeShelfHandler(uint32_t magazine, uint16_t slot) = 0;

	virtual bool syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector) = 0;

protected:

      virtual void cleanUp() = 0;

      virtual void checkOutQueue () = 0;

      virtual int updateTimeout() = 0;

private:
      void removeCanceledClients();

protected:
	std::vector<ACS_CS_DHCP_Info> normalDhcpVector;

	std::queue<ACS_CS_DHCP_Info> canceledClients;

	ACS_CS_ReaderWriterLock dhcpQueueLock;

	ACS_CS_EventHandle handleSNMPRequestEvent;

	ACS_CS_EventHandle shutdownEvent;

	ACS_CS_ServiceHandler* serviceCs;

	bool queueUpdated;

	bool outQueueEmpty;

};

#endif /* ACS_CS_SHELFMANAGER_H_ */
