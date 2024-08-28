/*
 * ACS_CS_SNMPMasterHandler.h
 *
 *  Created on: May 29, 2012
 *      Author: estevol
 */

#ifndef ACS_CS_SNMPMASTERHANDLER_H_
#define ACS_CS_SNMPMASTERHANDLER_H_


#include "ACS_CS_SNMPHandler.h"
#include "ACS_CS_ShelfManager.h"

class ACS_CS_ServiceHandler;

typedef struct
{
	ACS_CS_SNMPHandler * handlerSlot0;
	ACS_CS_SNMPHandler * handlerSlot25;

} SnmPHandlerMapEntry;


class ACS_CS_ServiceHandler;
class ACS_CS_DHCP_Info;
class ACS_CS_SNMPManager;

class ACS_CS_SNMPMasterHandler : public ACS_CS_ShelfManager
{

typedef std::map<uint32_t, SnmPHandlerMapEntry *> snmpMapType;
typedef std::map<uint32_t, std::vector<ACS_CS_DHCP_Info*>* > magazineMapType;

  public:

      ACS_CS_SNMPMasterHandler (ACS_CS_ServiceHandler* ParentService);

      virtual ~ACS_CS_SNMPMasterHandler();

      virtual void addShelfHandler(ACS_CS_DHCP_Info &infoA, ACS_CS_DHCP_Info &infoB);

      virtual void removeShelfHandler(uint32_t magazine, uint16_t slot);

      virtual bool syncClients (std::vector<ACS_CS_DHCP_Info*> &syncVector);

  protected:

      virtual void cleanUp();

      virtual void checkOutQueue ();

      virtual int updateTimeout();


  private:

      ACS_CS_SNMPMasterHandler(const ACS_CS_SNMPMasterHandler &right);

//      void removeCanceledClients();

      void snmpHandlerCleanup();


      ACS_CS_ReaderWriterLock snmpHandlerLock;

      ACS_CS_ReaderWriterLock snmpMapLock;

      snmpMapType snmpMap;

      std::vector<ACS_CS_SNMPHandler *> removedSnmpHandlers;

      bool missingMaster;

      bool isCBA;
};


#endif /* ACS_CS_SNMPMASTERHANDLER_H_ */
