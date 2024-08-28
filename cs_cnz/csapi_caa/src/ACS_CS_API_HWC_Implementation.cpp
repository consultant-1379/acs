#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_API_TableLoader.h"
#include <arpa/inet.h>

#include "ACS_CS_API_HWC_Implementation.h"

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImRepository.h"


#include <set>

#define BGCI_IP_OFFSET 18

ACS_CS_API_HWC_Implementation::ACS_CS_API_HWC_Implementation()
{
	model = new ACS_CS_ImModel();
	immReader = new ACS_CS_ImIMMReader();
	int hwload = immReader->loadBladeShelfObjects(model);

	if(hwload < 0){
		if(model)
			delete model;
		model = NULL;
	}
}

ACS_CS_API_HWC_Implementation::~ACS_CS_API_HWC_Implementation()
{
	if(immReader)
		delete immReader;
	if(model)
		delete model;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getMagazine (uint32_t &magazine, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 ACS_CS_ImBase *parent = model->getParentOf(board->rdn);

		 ACS_CS_ImShelf *shelf = dynamic_cast<ACS_CS_ImShelf *>(parent);
		 if (shelf) {

			 in_addr address;
			 if (inet_aton(shelf->address.c_str(), &address) != 0)
				 magazine = address.s_addr;

			 return ACS_CS_API_NS::Result_Success;
		 }
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSlot (uint16_t &slot, BoardID boardId)
 {
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 slot = board->slotNumber;
		 return ACS_CS_API_NS::Result_Success;
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSysType (uint16_t &sysType, BoardID boardId)
 {
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 if (UNDEF_SYSTEMTYPE == board->systemType)
		 {
			 return ACS_CS_API_NS::Result_NoValue;
		 }
		 else
		 {
			 sysType = static_cast<uint16_t>(board->systemType);
			 return ACS_CS_API_NS::Result_Success;
		 }
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSysNo (uint16_t &sysNo, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 sysNo = board->systemNumber;
		 return ACS_CS_API_NS::Result_Success;
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getFBN (uint16_t &fbn, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
    if (board)
    {
    	if (UNDEF_FUNCTIONALBOARDNAME == board->functionalBoardName)
    	{
    		return ACS_CS_API_NS::Result_NoValue;
    	}
    	else
    	{
    		fbn = static_cast<uint16_t>(board->functionalBoardName);
    		return ACS_CS_API_NS::Result_Success;
    	}
    }

    return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSide (uint16_t &side, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 if (UNDEF_SIDE == board->side)
		 {
			 return ACS_CS_API_NS::Result_NoValue;
		 }
		 else
		 {
			 side = static_cast<uint16_t>(board->side);
			 return ACS_CS_API_NS::Result_Success;
		 }
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSeqNo (uint16_t &seqNo, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 if (board->sequenceNumber < 0)
		 {
			 return ACS_CS_API_NS::Result_NoValue;
		 }
		 else
		 {
			 seqNo = static_cast<uint16_t>(board->sequenceNumber);
			 return ACS_CS_API_NS::Result_Success;
		 }
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getIPEthA (uint32_t &ip, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		in_addr address;
		string ipAddressEthA = (string) board->ipAddressEthA;
		if (inet_aton(ipAddressEthA.c_str(), &address) != 0)
			ip = htonl(address.s_addr);
		else
			ip = 0;
		return ACS_CS_API_NS::Result_Success;
	}

	return ACS_CS_API_NS::Result_Failure;

}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getIPEthB (uint32_t &ip, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		in_addr address;
		string ipAddressEthB = (string) board->ipAddressEthB;

		if (inet_aton(ipAddressEthB.c_str(), &address) != 0)
			ip = htonl(address.s_addr);
		else
			ip = 0;

		return ACS_CS_API_NS::Result_Success;
	}

	return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getAliasEthA (uint32_t &ip, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		in_addr address;
		string ipAliasEthA = (string) board->ipAliasEthA;

		if (inet_aton(ipAliasEthA.c_str(), &address) != 0)
			ip = htonl(address.s_addr);
		else
			ip = 0;

		return ACS_CS_API_NS::Result_Success;
	}

	return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getAliasEthB (uint32_t &ip, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		in_addr address;
		string ipAliasEthB = (string) board->ipAliasEthB;

		if (inet_aton(ipAliasEthB.c_str(), &address) != 0)
			ip = htonl(address.s_addr);
		else
			ip = 0;

		return ACS_CS_API_NS::Result_Success;
	}

	return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getAliasNetmaskEthA (uint32_t &mask, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		in_addr address;

		string aliasNetmaskEthA = (string) board->aliasNetmaskEthA;
		if (inet_aton(aliasNetmaskEthA.c_str(), &address) != 0)
			mask = htonl(address.s_addr);
		else
			mask = 0;

		return ACS_CS_API_NS::Result_Success;
	}

	return ACS_CS_API_NS::Result_Failure;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getAliasNetmaskEthB (uint32_t &mask, BoardID boardId)
{
	 if (!model)
		 return ACS_CS_API_NS::Result_Failure;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 in_addr address;
		 string aliasNetmaskEthB = (string) board->aliasNetmaskEthB;

		 if (inet_aton(aliasNetmaskEthB.c_str(), &address) != 0)
			 mask = htonl(address.s_addr);
		 else
			 mask = 0;
		 return ACS_CS_API_NS::Result_Success;
	 }

	 return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getDhcpMethod (uint16_t &dhcp, BoardID boardId)
{
	if (!model)
		return ACS_CS_API_NS::Result_Failure;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		if (UNDEF_DHCP == board->dhcpOption)
		{
			return ACS_CS_API_NS::Result_NoValue;
		}
		else
		{
			dhcp = static_cast<uint16_t>(board->dhcpOption);
			return ACS_CS_API_NS::Result_Success;
		}
	}

	return ACS_CS_API_NS::Result_Failure;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSysId (uint16_t &sysId, BoardID boardId)
{

	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	   if (!model)
	      return resultValue;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);

	if (board)
	{
		if (UNDEF_SYSTEMTYPE == board->systemType)
		{
			return ACS_CS_API_NS::Result_NoValue;
		}

		SystemTypeEnum type = board->systemType;
		if (type == SINGLE_SIDED_CP)
		{
			sysId = static_cast<uint16_t>(type + board->sequenceNumber);
		}
		else if (type == DOUBLE_SIDED_CP || type == AP)
		{
			sysId = static_cast<uint16_t>(type + board->systemNumber);
		}
		else
		{
			sysId = 0;
		}
		resultValue = ACS_CS_API_NS::Result_Success;
	}
	else
	{
		resultValue = ACS_CS_API_NS::Result_NoValue;
	}

	return resultValue;

}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getBoardIds (ACS_CS_API_IdList_R1 &listObj, ACS_CS_API_BoardSearch_R1 *searchObj)
{

    ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;  //default return value

   if (searchObj == 0)	//check for null pointer
		return resultValue;
   
   ACS_CS_API_BoardSearch_Implementation* boardSearchImp =					      //creates a BoardSearchImplementation pointer
	   reinterpret_cast<ACS_CS_API_BoardSearch_Implementation *> (searchObj);  //by casting current BoardSearch pointer

   if (!model)
      return resultValue;

   std::set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects, APBLADE_T);
   model->getObjects(objects, CPBLADE_T);
   model->getObjects(objects, OTHERBLADE_T);

   std::set<const ACS_CS_ImBase *>::const_iterator it;
   std::set<const ACS_CS_ImBlade *> foundItems;
   std::set<const ACS_CS_ImBlade *>::const_iterator it2;

   for (it = objects.begin(); it != objects.end(); it++)
      if (boardSearchImp->match(*it))
         foundItems.insert(reinterpret_cast<const ACS_CS_ImBlade *>(*it));

   listObj.setSize(foundItems.size());

   int i = 0;
   for (it2 = foundItems.begin(); it2 != foundItems.end(); it2++) {
      const ACS_CS_ImBlade *blade = *it2;

      uint16_t entryId = blade->entryId;
      listObj.setValue(entryId, i++);
   }

   return ACS_CS_API_NS::Result_Success;

}
ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSoftwareVersionType (uint16_t &version, BoardID boardId) {

        ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;
        if (!model)
                 return resultValue;
        const ACS_CS_ImBlade *board = model->getBlade(boardId);

         if (board)
         {
                const ACS_CS_ImOtherBlade *blade = dynamic_cast<const ACS_CS_ImOtherBlade*>(board);
                 if (blade && (blade->functionalBoardName == CMXB))
                 {
                	 version = static_cast<uint16_t>(blade->swVerType);
                	 resultValue = ACS_CS_API_NS::Result_Success;
                 } else
                	 resultValue = ACS_CS_API_NS::Result_NoValue;
         } else
                 resultValue = ACS_CS_API_NS::Result_NoValue;
         return resultValue;
}

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getContainerPackage (std::string &container, BoardID boardId) {
	 ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	 if (!model)
		 return resultValue;

	 const ACS_CS_ImBlade *board = model->getBlade(boardId);
	 if (board)
	 {
		 const ACS_CS_ImOtherBlade *blade = dynamic_cast<const ACS_CS_ImOtherBlade*>(board);
		 if (blade) {
			 if (blade->functionalBoardName == EPB1 || blade->functionalBoardName == SCXB
					 || blade->functionalBoardName == EVOET || blade->functionalBoardName == CMXB
					 || blade->functionalBoardName == IPTB || blade->functionalBoardName == IPLB 
					 || blade->functionalBoardName == SMXB) {
				 std::string rdn = blade->currentLoadModuleVersion;
				 container = ACS_CS_ImUtils::getIdValueFromRdn(rdn);
				 resultValue = ACS_CS_API_NS::Result_Success;
			 } else
				 resultValue = ACS_CS_API_NS::Result_NoValue;
		 } else
			 resultValue = ACS_CS_API_NS::Result_NoValue;

	 } else
		 resultValue = ACS_CS_API_NS::Result_NoValue;

	 return resultValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getBgciIPEthA (uint32_t &ip, BoardID boardId)
 {
	 uint32_t ip_ethA = 0;
	 ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	 resultValue  = getIPEthA(ip_ethA, boardId);

	 if (resultValue == ACS_CS_API_NS::Result_Success)
	 {
		 uint32_t subnet[2] = {0};
		 uint32_t netmask[2] = {0};

		 uint32_t lastByte = ip_ethA & 0x000000ff;

		 resultValue = ACS_CS_API_NetworkElement::getBGCIVlan(subnet, netmask);

		 if (resultValue == ACS_CS_API_NS::Result_Success)
		 {
			 //Add a fixed offset to addresses from 1 to 4 to have corresponding BGCI address.
			 //No offset for all other addresses
			 lastByte += (lastByte < 5)? BGCI_IP_OFFSET: 0;

			 //BGCI-A is the first subnet of the array
			 ip = (subnet[0] & netmask[0]) | lastByte;
		 }
	 }

	 return resultValue;
 }

 ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getBgciIPEthB (uint32_t &ip, BoardID boardId)
  {
 	 uint32_t ip_ethB = 0;
 	 ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

 	 resultValue  = getIPEthB(ip_ethB, boardId);

 	 if (resultValue == ACS_CS_API_NS::Result_Success)
 	 {
 		 uint32_t subnet[2] = {0};
 		 uint32_t netmask[2] = {0};

 		 uint32_t lastByte = ip_ethB & 0x000000ff;

 		 resultValue = ACS_CS_API_NetworkElement::getBGCIVlan(subnet, netmask);

 		 if (resultValue == ACS_CS_API_NS::Result_Success)
 		 {
 			 //Add a fixed offset to addresses from 1 to 4 to have corresponding BGCI address.
 			 //No offset for all other addresses
 			 lastByte += (lastByte < 5)? BGCI_IP_OFFSET: 0;

 			//BGCI-A is the last subnet of the array
 			 ip = (subnet[1] & netmask[1]) | lastByte;
 		 }
 	 }

 	 return resultValue;
  }
ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSolIPEthA (uint32_t &ip, BoardID boardId)
{
	uint32_t ip_ethA = 0;
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	resultValue  = getIPEthA(ip_ethA, boardId);

	if (resultValue == ACS_CS_API_NS::Result_Success)
	{
		uint32_t subnet[2] = {0};
		uint32_t netmask[2] = {0};
		uint32_t lastByte = ip_ethA & 0x000000ff;

		resultValue = ACS_CS_API_NetworkElement::getSOLVlan(subnet, netmask);

		if (resultValue == ACS_CS_API_NS::Result_Success)
		{
			ip = (subnet[0] & netmask[0]) | lastByte;
		}
	}
return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getSolIPEthB (uint32_t &ip, BoardID boardId)
{
	uint32_t ip_ethB = 0;
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	resultValue  = getIPEthB(ip_ethB, boardId);

	if (resultValue == ACS_CS_API_NS::Result_Success)
	{
		uint32_t subnet[2] = {0};
		uint32_t netmask[2] = {0};
		uint32_t lastByte = ip_ethB & 0x000000ff;

		resultValue = ACS_CS_API_NetworkElement::getSOLVlan(subnet, netmask);

		if (resultValue == ACS_CS_API_NS::Result_Success)
		{
			ip = (subnet[1] & netmask[1]) | lastByte;
		}
	}
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_HWC_Implementation::getUuid (std::string &uuid, BoardID boardId) {
	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	if (!model)
		return resultValue;

	const ACS_CS_ImBlade *board = model->getBlade(boardId);
	if (board)
	{
		uuid = board->uuid;
		resultValue = ACS_CS_API_NS::Result_Success;
	} else
		resultValue = ACS_CS_API_NS::Result_NoValue;

	return resultValue;
 }

