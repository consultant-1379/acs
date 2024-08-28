
#include "ACS_CS_Attribute.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>


#include "ACS_CS_API_BoardSearch_Implementation.h"

using namespace std;


ACS_CS_API_BoardSearch_Implementation::ACS_CS_API_BoardSearch_Implementation()
{
   searchableBlade = new ACS_CS_ImSearchableBlade();
}


ACS_CS_API_BoardSearch_Implementation::~ACS_CS_API_BoardSearch_Implementation()
{
   if (searchableBlade)
      delete searchableBlade;
}


bool ACS_CS_API_BoardSearch_Implementation::match(const ACS_CS_ImBase *object)
{
   const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(object);

   if (blade != 0)
      return searchableBlade->match(*blade);
   return false;
}

/*bool ACS_CS_API_BoardSearch_Implementation::isDefault()
{
   return !searchableBlade->attributesChanged();
}*/


void ACS_CS_API_BoardSearch_Implementation::setMagazine (uint32_t magazine)
{
   // Magazine apparently does not come in host byte order and hence we dont have to convert it... ???
    char tempValueStr[INET_ADDRSTRLEN];
    //magazine = htonl(magazine);
    searchableBlade->magazine = inet_ntop(AF_INET, &magazine, tempValueStr, INET_ADDRSTRLEN);
    searchableBlade->magazineChanged = true;

}

void ACS_CS_API_BoardSearch_Implementation::setSlot (uint16_t slot)
{
    searchableBlade->slotNumber = slot;
    searchableBlade->slotNumberChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setSysType (uint16_t sysType)
{
    searchableBlade->systemType = (SystemTypeEnum) sysType;
    searchableBlade->systemTypeChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setSysNo (uint16_t sysNo)
{
    searchableBlade->systemNumber = sysNo;
    searchableBlade->systemNumberChanged = true;
}

 void ACS_CS_API_BoardSearch_Implementation::setFBN (uint16_t fbn)
{
    searchableBlade->functionalBoardName = (FunctionalBoardNameEnum) fbn;
    searchableBlade->functionalBoardNameChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setSide (uint16_t side)
{
    searchableBlade->side = (SideEnum) side;
    searchableBlade->sideChanged = true;
}

 void ACS_CS_API_BoardSearch_Implementation::setSeqNo (uint16_t seqNo)
{
    searchableBlade->sequenceNumber = (int32_t) seqNo;
    searchableBlade->sequenceNumberChanged = true;
}

 void ACS_CS_API_BoardSearch_Implementation::setIPEthA (uint32_t address)
{
    // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
    char tempValueStr[INET_ADDRSTRLEN];
    address = htonl(address);
    searchableBlade->ipAddressEthA = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
    searchableBlade->ipAddressEthAChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setIPEthB (uint32_t address)
{
   // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
    char tempValueStr[INET_ADDRSTRLEN];
    address = htonl(address);
    searchableBlade->ipAddressEthB = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
    searchableBlade->ipAddressEthBChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setAliasEthA (uint32_t address)
{
   // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
    char tempValueStr[INET_ADDRSTRLEN];
    address = htonl(address);
    searchableBlade->ipAliasEthA = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
    searchableBlade->ipAliasEthAChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setAliasEthB (uint32_t address)
{
   // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
   char tempValueStr[INET_ADDRSTRLEN];
   address = htonl(address);
   searchableBlade->ipAliasEthB = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);
   searchableBlade->ipAliasEthBChanged = true;

}

void ACS_CS_API_BoardSearch_Implementation::setAliasNetmaskEthA (uint32_t mask)
{
   // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
   char tempValueStr[INET_ADDRSTRLEN];
   mask = htonl(mask);
   searchableBlade->aliasNetmaskEthA = inet_ntop(AF_INET, &mask, tempValueStr, INET_ADDRSTRLEN);
   searchableBlade->aliasNetmaskEthAChanged = true;

}

void ACS_CS_API_BoardSearch_Implementation::setAliasNetmaskEthB (uint32_t mask)
{
   // Input already is on the Host Byte Order, so we need to convert it back to network in order for inet_ntop
   char tempValueStr[INET_ADDRSTRLEN];
   mask = htonl(mask);
   searchableBlade->aliasNetmaskEthB = inet_ntop(AF_INET, &mask, tempValueStr, INET_ADDRSTRLEN);
   searchableBlade->aliasNetmaskEthBChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setDhcpMethod (uint16_t method)
{
    searchableBlade->dhcpOption = (DhcpEnum) method;
    searchableBlade->dhcpOptionChanged = true;
}

void ACS_CS_API_BoardSearch_Implementation::setSysId (uint16_t sysId)
{
   // @TODO Check that this logic is correct.

    unsigned short sysType = 0;
    unsigned short sysNo = 0;

    if (sysId < 1000)
    {
       searchableBlade->systemType = (SystemTypeEnum) sysType;
       searchableBlade->systemTypeChanged = true;
       searchableBlade->sequenceNumber = (int32_t) sysId;
       searchableBlade->sequenceNumberChanged = true;
    }
    else
    {
       sysNo = (sysId % 1000);
       sysType = sysId - sysNo;

       searchableBlade->systemType = (SystemTypeEnum) sysType;
       searchableBlade->systemTypeChanged = true;
       searchableBlade->systemNumber = sysNo;
       searchableBlade->systemNumberChanged = true;
    }
}

void ACS_CS_API_BoardSearch_Implementation::reset ()
{
    searchableBlade->reset();
}

void ACS_CS_API_BoardSearch_Implementation::setSwVerType (uint16_t swVerType)
{
	searchableBlade->swVerType = (uint16_t) swVerType;
	searchableBlade->swVerTypeChanged = true;
}
void ACS_CS_API_BoardSearch_Implementation::setUuid (const string &uuid)
{
    searchableBlade->uuid = uuid;
    searchableBlade->uuidChanged = true;
}
