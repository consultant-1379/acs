
#include "ACS_CS_Util.h"
//#include "ACS_CS_Trace.h"

#include <sstream>
extern "C"{
#include <string.h>
}
#include <sstream>
#include <iostream>

#include "ace/INET_Addr.h"

#include "ACS_CS_DHCP_Info.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_DHCP_Info_TRACE);

using std::ostringstream;
using std::string;

ACS_CS_DHCP_Info::ACS_CS_DHCP_Info()
      : ipAddress(0),
        macAddress(0),
        macLength(0),
        clientId(0),
        clientIdLength(0),
        dhcpMethod(ACS_CS_Protocol::DHCP_None),
        magazineAddress(0),
        slotPosition(0),
        networkIdentifier(ACS_CS_Protocol::Network_NotSpecified),
        isSCBRPBoard(false),
        fbn(ACS_CS_Protocol::FBN_NotSpecified),
        boardPersistent(false),
        scope(0),
        side(ACS_CS_Protocol::Side_NotSpecified),
        systemType(ACS_CS_Protocol::SysType_NotSpecified),
        sysId(ACS_CS_NS::ILLEGAL_SYSID),
        bootServer(),
		bootPath(),
		tries(0),
		acIp(0),
		trapReceiver(0)
{}

ACS_CS_DHCP_Info::ACS_CS_DHCP_Info(const ACS_CS_DHCP_Info &right)
{
	this->hostComment = right.hostComment;
	this->ipAddress = right.ipAddress;
	this->dhcpMethod = right.dhcpMethod;
	this->magazineAddress = right.magazineAddress;
	this->networkIdentifier = right.networkIdentifier;
	this->slotPosition = right.slotPosition;
	this->isSCBRPBoard = right.isSCBRPBoard;
	this->fbn = right.fbn;
	this->boardPersistent = right.boardPersistent;
	this->scope = right.scope;
	this->side = right.side;
	this->sysId = right.sysId;
	this->systemType = right.systemType;
	this->bootPath = right.bootPath;
	this->bootServer = right.bootServer;
	this->hostName = right.hostName;
	this->currentLoadModuleVersion = right.currentLoadModuleVersion;
	this->loadModuleFileName = right.loadModuleFileName;
	this->bladeProductNumber = right.bladeProductNumber;
	this->tries = right.tries;
	this->acIp = right.acIp;
	this->trapReceiver = right.trapReceiver;
	this->productInfo = right.productInfo;


	this->macLength = right.macLength;

	// Copy new MAC
	if (right.macAddress)
	{
		this->macAddress = new char[this->macLength];

		if (this->macAddress)
			memcpy(this->macAddress, right.macAddress, this->macLength);
	}
	else
	{
		macAddress=0;
		macLength=0;

	}


	this->clientIdLength = right.clientIdLength;

	// Copy new clientId
	if (right.clientId)
	{
		this->clientId = new char[this->clientIdLength];

		if (this->clientId)
			memcpy(this->clientId, right.clientId, this->clientIdLength);
	}
	else
	{
		clientId=0;
		clientIdLength=0;
	}


}

ACS_CS_DHCP_Info::ACS_CS_DHCP_Info (uint32_t ip, char *mac, int length, std::string comment)
      : ipAddress(0),
        macAddress(0),
        macLength(0),
        clientId(0),
        clientIdLength(0),
        dhcpMethod(ACS_CS_Protocol::DHCP_None),
        magazineAddress(0),
        slotPosition(0),
        networkIdentifier(ACS_CS_Protocol::Network_NotSpecified),
        isSCBRPBoard(false),
        fbn(ACS_CS_Protocol::FBN_NotSpecified),
        boardPersistent(false),
        scope(0),
        side(ACS_CS_Protocol::Side_NotSpecified),
        systemType(ACS_CS_Protocol::SysType_NotSpecified),
        sysId(ACS_CS_NS::ILLEGAL_SYSID),
        bootServer(),
        bootPath(),
        tries(0),
        acIp(0),
        trapReceiver(0)
{
   this->ipAddress = ip;
	this->hostComment = comment;
	this->macLength = length;
	this->macAddress = new char[length];

	if (this->macAddress)
		memcpy(this->macAddress, mac, length);
}


ACS_CS_DHCP_Info::~ACS_CS_DHCP_Info()
{
   if (this->macAddress)
		delete [] this->macAddress;

   if (this->clientId)
   		delete [] this->clientId;

}


ACS_CS_DHCP_Info & ACS_CS_DHCP_Info::operator=(const ACS_CS_DHCP_Info &right)
{
   if (this == &right)		// Check assignment to self
		return *this;

	// Assign values
	this->hostComment = right.hostComment;
	this->ipAddress = right.ipAddress;
	this->dhcpMethod = right.dhcpMethod;
	this->magazineAddress = right.magazineAddress;
	this->networkIdentifier = right.networkIdentifier;
	this->slotPosition = right.slotPosition;
	this->isSCBRPBoard = right.isSCBRPBoard;
	this->fbn = right.fbn;
	this->boardPersistent = right.boardPersistent;
	this->scope = right.scope;
	this->side = right.side;
	this->sysId = right.sysId;
	this->systemType = right.systemType;
	this->bootPath = right.bootPath;
	this->bootServer = right.bootServer;
	this->hostName = right.hostName;
	this->currentLoadModuleVersion = right.currentLoadModuleVersion;
	this->loadModuleFileName = right.loadModuleFileName;
	this->bladeProductNumber = right.bladeProductNumber;
	this->tries = right.tries;
	this->acIp = right.acIp;
	this->trapReceiver = right.trapReceiver;
	this->productInfo = right.productInfo;




	// Delete old MAC
	if (this->macAddress)
	{
		delete [] this->macAddress;
		this->macAddress = 0;
		this->macLength = 0;
	}

	// Delete old clientId
	if (this->clientId)
	{
		delete [] this->clientId;
		this->clientId = 0;
		this->clientIdLength = 0;
	}

	this->macLength = right.macLength;

	// Copy new MAC
	if (right.macAddress)
	{
		this->macAddress = new char[this->macLength];

		if (this->macAddress)
			memcpy(this->macAddress, right.macAddress, this->macLength);
	}


	this->clientIdLength = right.clientIdLength;

	// Copy new clientId
	if (right.clientId)
	{
		this->clientId = new char[this->clientIdLength];

		if (this->clientId)
			memcpy(this->clientId, right.clientId, this->clientIdLength);
	}

	return *this;

}


int ACS_CS_DHCP_Info::operator==(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress == right.ipAddress ? 1 : 0 );	// Compare ip address
}

int ACS_CS_DHCP_Info::operator!=(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress != right.ipAddress ? 1 : 0 );	// Compare ip address
}


int ACS_CS_DHCP_Info::operator<(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress < right.ipAddress ? 1 : 0 );	// Compare ip address
}

int ACS_CS_DHCP_Info::operator>(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress > right.ipAddress ? 1 : 0 );	// Compare ip address
}

int ACS_CS_DHCP_Info::operator<=(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress <= right.ipAddress ? 1 : 0 );	// Compare ip address
}

int ACS_CS_DHCP_Info::operator>=(const ACS_CS_DHCP_Info &right) const
{
   return ( this->ipAddress >= right.ipAddress ? 1 : 0 );	// Compare ip address
}



uint32_t ACS_CS_DHCP_Info::getIP () const
{
   return this->ipAddress;
}

 void ACS_CS_DHCP_Info::getMAC (char *mac, int size) const
{
   if (mac == 0)						// Check for valid memory
		return;
	else if (size != this->macLength)	// Check for valid size
		return;
	else if (this->macAddress == 0)		// Check for valid source
		return;

	memcpy(mac, this->macAddress, this->macLength);	// Copy MAC
}

 int ACS_CS_DHCP_Info::getMacLength () const
{
   return this->macLength;
}

 std::string ACS_CS_DHCP_Info::getComment () const
{
   return this->hostComment;
}

 std::string ACS_CS_DHCP_Info::getHostName () const
{
	 return this->hostName;
}

 void ACS_CS_DHCP_Info::setHostName (std::string hostName)
{
	 this->hostName = hostName;
}

void ACS_CS_DHCP_Info::getClientId (char *clientId, int size) const
{
   //return this->clientId;
	if (clientId == 0)						// Check for valid memory
		return;
	else if (size != this->clientIdLength)	// Check for valid size
		return;
	else if (this->clientId == 0)		// Check for valid source
		return;

	memcpy(clientId, this->clientId, this->clientIdLength);	// Copy clientId
}

int ACS_CS_DHCP_Info::getClientIdLength () const
{
  return this->clientIdLength;
}


 std::string ACS_CS_DHCP_Info::getIPString () const
 {
    string ip = "No IP";

 	if (ipAddress)
 	{
 		char buffer[17] = { 0 };
 		sprintf(buffer, "%u.%u.%u.%u",	( ( ipAddress  >> 24 ) & 0xFF ),
 										( ( ipAddress  >> 16 ) & 0xFF ),
 										( ( ipAddress  >> 8 ) & 0xFF ),
 										( ipAddress & 0xFF ) );
 		ip = buffer;
 	}

 	return ip;
 }



 ACS_CS_Protocol::CS_Network_Identifier ACS_CS_DHCP_Info::getNetwork () const
{
   return networkIdentifier;
}

 ACS_CS_Protocol::CS_DHCP_Method_Identifier ACS_CS_DHCP_Info::getDHCPMethod () const
{
   return this->dhcpMethod;
}

uint32_t ACS_CS_DHCP_Info::getMagAddress () const
{
   return this->magazineAddress;
}

 void ACS_CS_DHCP_Info::calcOption61Id (int fbn, ACS_CS_Protocol::SwitchType switchType)
{
   if (this->dhcpMethod != ACS_CS_Protocol::DHCP_Client)
		return;

   if (fbn == ACS_CS_Protocol::FBN_NotSpecified)
	   fbn = this->fbn;

   if (clientId)
   {
	   delete [] clientId;
	   clientId = 0;
	   clientIdLength = 0;
   }

	ostringstream os;					// Stream to build up the hardware identifier

	// Build up the hardware identifier
	// It has the format
	// <MagAddressPlug3>.<MagAddressPlug1>.<MagAddressPlug0>.<Slot>:<Interface name>

	os << ( ( magazineAddress >> 24 ) & 0xFF ) << ".";	// Plug 3
	os << ( ( magazineAddress >> 8 ) & 0xFF ) << ".";	// Plug 1
	os << ( magazineAddress & 0xFF ) << ".";			// Plug 0
	os << slotPosition << ":";							// Slot



	if (this->networkIdentifier == ACS_CS_Protocol::Eth_A)
	{
		if (fbn == ACS_CS_Protocol::FBN_GESB)
			os << "eth0";
		else if(fbn == ACS_CS_Protocol::FBN_SMXB    &&  (switchType == ACS_CS_Protocol::CONTROL_SWITCH))
			os <<  "__local";
		else if (fbn == ACS_CS_Protocol::FBN_SMXB    &&  (switchType == ACS_CS_Protocol::TRANSPORT_SWITCH))
			os <<  "__tlocal";
		else
			os << "ethA";
		//clientId.append(fbn == ACS_CS_Protocol::FBN_GESB ? "eth0" : "ethA"); //CR GESB
	}
	else if (this->networkIdentifier == ACS_CS_Protocol::Eth_B)
	{
		if (fbn == ACS_CS_Protocol::FBN_GESB)
			os << "eth1";
		else if(fbn == ACS_CS_Protocol::FBN_SMXB    &&  (switchType == ACS_CS_Protocol::CONTROL_SWITCH))
			os <<  "__remote";
		else if (fbn == ACS_CS_Protocol::FBN_SMXB    &&  (switchType == ACS_CS_Protocol::TRANSPORT_SWITCH))
			os <<  "__tremote";
		else
			os << "ethB";
		//clientId.append(fbn == ACS_CS_Protocol::FBN_GESB ? "eth1" : "ethB"); //CR GESB
	}
	else
	{

		ACS_CS_TRACE((ACS_CS_DHCP_Info_TRACE,
			"(%t) ACS_CS_DHCP_Info::calcOption61Id()\n"
			"Error: Network not set.\n"));

		return;
	}

	std::string hardwareIdStr;

	if ((this->getFBN() == ACS_CS_Protocol::FBN_SCXB)||(this->getFBN() == ACS_CS_Protocol::FBN_EPB1) || (this->getFBN() == ACS_CS_Protocol::FBN_EvoET) || (this->getFBN() == ACS_CS_Protocol::FBN_SMXB))
	{
		std::string prodPrefix;
		std::string prodIDStr;

		if(getFBN() == ACS_CS_Protocol::FBN_EPB1 || getFBN() == ACS_CS_Protocol::FBN_EvoET)
		{
			prodIDStr = macAddress;
		}
		else
		{
			prodIDStr = this->getBladeProductNumber();
		}

		size_t slashIndex = prodIDStr.find_first_of("/");
		if (slashIndex == std::string::npos) slashIndex = prodIDStr.length();
		size_t startIndex = (slashIndex >= 5) ? (slashIndex - 5) : 0;
		size_t endIndex = slashIndex;
		if (startIndex <= endIndex)
		{
			endIndex = slashIndex - startIndex;
			prodPrefix = prodIDStr.substr(startIndex, endIndex);
		}
		else
		{
			char reason[256] = {0};
			snprintf(reason, sizeof(reason) - 1, "Unable to create reservation id with productId %s", prodIDStr.c_str());

			ACS_CS_TRACE((ACS_CS_DHCP_Info_TRACE,
						"(%t) ACS_CS_DHCP_Info::calcOption61Id()\n"
						"Error: Network not set. Reason: %s\n", reason));
			return;
		}

		//prodsuffix
		std::string prodSuffix = "00";
		if (slashIndex < prodIDStr.length())
		{
			prodSuffix = prodIDStr.substr(slashIndex + 1, 2);
		}

		unsigned plug3 = ( magazineAddress >> 24 ) & 0xFF;
		unsigned plug1 = ( magazineAddress >> 8 ) & 0xFF;
		unsigned plug0 = magazineAddress & 0xFF;

		//revision
		std::string revSuffix = "000";

		char hexPlugs[33] = {0};
		snprintf(hexPlugs, sizeof(hexPlugs) - 1, "%x%x%x", plug3, plug1, plug0); //hex magazine

		std::string subrackIdentifier = hexPlugs;
		std::string netId;

		int netId3;
		if((ACS_CS_Protocol::FBN_SMXB == fbn )   &&  (ACS_CS_Protocol::CONTROL_SWITCH == switchType))
		{
			if (((networkIdentifier == ACS_CS_Protocol::Eth_A) && (slotPosition == 0))
				||
				((networkIdentifier == ACS_CS_Protocol::Eth_B) && (slotPosition == 25))
				)			
			{
				netId = "__local";
				netId3=0;
			}
			else{
				netId = "__remote";
				netId3=1;
			}
		}
		else if((ACS_CS_Protocol::FBN_SMXB == fbn)    &&  (ACS_CS_Protocol::TRANSPORT_SWITCH == switchType))
		{
			if (((networkIdentifier == ACS_CS_Protocol::Eth_A) && (slotPosition == 0))
				||
				((networkIdentifier == ACS_CS_Protocol::Eth_B) && (slotPosition == 25))
				)
			{
				netId = "__tlocal";	
				netId3=0;
			}
			else
			{
				netId = "__tremote";	
				netId3=1;
			}
		}
		else if (((networkIdentifier == ACS_CS_Protocol::Eth_A) && (slotPosition == 0))
				||
				((networkIdentifier == ACS_CS_Protocol::Eth_B) && (slotPosition == 25))
				)
		{
			netId3=0;
			netId = "__eth0";			
		}
		else
		{
			netId3=1;				
			netId = "__eth1";
		}
		

		if(getFBN() == ACS_CS_Protocol::FBN_EPB1 || getFBN() == ACS_CS_Protocol::FBN_EvoET)
		{
			if(this->getNetwork() == ACS_CS_Protocol::Eth_A)
			{
				netId3 = 0;
				netId = "__eth0";
			}
			else
			{
				netId3 = 1;
				netId = "__eth1";
			}
		}

		char clientIdChar[256] = {0};

		//client ID, ACS_CS_DHCPHandler::createAddClientString is responsible for translating this string into an ASCII string

		snprintf(clientIdChar, sizeof(clientIdChar) - 1, "%s%s%s%s%2d%s",
				zeropad(prodPrefix.c_str(),5).c_str(),
				zeropad(prodSuffix.c_str(),2).c_str(),
				zeropad(revSuffix.c_str(),3).c_str(),subrackIdentifier.c_str(),slotPosition,netId.c_str());

		hardwareIdStr = clientIdChar;
		figureOutClientId(prodIDStr,(int)plug0,(int)plug1,(int)plug3,slotPosition,netId3,hardwareIdStr,switchType);
	}
	else if (getFBN() == ACS_CS_Protocol::FBN_CMXB)
	{
		std::string prodIDStr = macAddress;

		unsigned plug3 = ( magazineAddress >> 24 ) & 0xFF;
		unsigned plug1 = ( magazineAddress >> 8 ) & 0xFF;
		unsigned plug0 = magazineAddress & 0xFF;

		int netId;

		switch (networkIdentifier)
		{
		case ACS_CS_Protocol::Eth_A:
			if (slotPosition == 26) netId = 1;
			if (slotPosition == 28) netId = 0;
			break;
		case ACS_CS_Protocol::Eth_B:
			if (slotPosition == 26) netId = 0;
			if (slotPosition == 28) netId = 1;
			break;
		default:
			break;
		}

		figureOutClientId(prodIDStr,(int)plug0,(int)plug1,(int)plug3,slotPosition,netId,hardwareIdStr,switchType);
	}

	if (hardwareIdStr.length() > 0)
	{

		clientIdLength = hardwareIdStr.length();

		clientId = new char[clientIdLength];
		memcpy(clientId, hardwareIdStr.c_str(), clientIdLength);	// Copy string to byte array

		if(macAddress)
		{
			delete [] macAddress;

		}
		macAddress = new char[clientIdLength];
		memcpy(macAddress, hardwareIdStr.c_str(), clientIdLength);	// Copy string to byte array
		macLength=clientIdLength;
	}
	else
	{

		//Use quoted-string format for BC boards client identifier ('0' char at the beginning)
		if (fbn == ACS_CS_Protocol::FBN_CPUB && systemType == ACS_CS_Protocol::SysType_BC)
		{
			clientIdLength = os.str().size()  + 1;
			clientId = new char[clientIdLength];
			clientId[0] = '0';
			memcpy(clientId +1, os.str().c_str(), os.str().size());
		}
		else //Use colon-separated hexadecimal list format client identifier (NULL char at the beginning)
		{
			// clientId is NOT a null terminated C-string
			// it is a binary sequence that begins with null
			clientIdLength = os.str().size() + 1;
			clientId = new char[clientIdLength];
			clientId[0] = 0;
			memcpy(clientId + 1, os.str().c_str(), os.str().size());
		}
	}

}
void ACS_CS_DHCP_Info::setIP (uint32_t ip)
{
   this->ipAddress = ip;
}

 void ACS_CS_DHCP_Info::setMAC (const char *mac, int size)
{
   if (mac == 0)			// Check for valid memory
		return;
	else if (size < 1)		// Check for valid size
		return;
	
	if (this->macAddress)	// Delete old MAC
	{
		delete [] this->macAddress;
		this->macAddress = 0;
		this->macLength = 0;
	}

	// Copy new MAC
	this->macLength = size;
	this->macAddress = new char[size+1];
	if (this->macAddress) {
		memset(this->macAddress,0,size+1);
		memcpy(this->macAddress, mac, size);
	}
}

 void ACS_CS_DHCP_Info::setComment (std::string comment)
{
   this->hostComment = comment;
}


 void ACS_CS_DHCP_Info::setDHCPMethod (ACS_CS_Protocol::CS_DHCP_Method_Identifier method)
{
   dhcpMethod = method;
}

 void ACS_CS_DHCP_Info::setMagAddress (uint32_t address)
{
   magazineAddress = address;
}

 void ACS_CS_DHCP_Info::setSlot (uint16_t slot)
{
   slotPosition = slot;
}

 void ACS_CS_DHCP_Info::setNetwork (ACS_CS_Protocol::CS_Network_Identifier network)
{
   networkIdentifier = network;
}

 void ACS_CS_DHCP_Info::setSCBRP (bool isBoardSCBRP)
{
   this->isSCBRPBoard = isBoardSCBRP;
}

 bool ACS_CS_DHCP_Info::isSCBRP () const
{
   return this->isSCBRPBoard;
}

uint16_t ACS_CS_DHCP_Info::getSlot () const
{
   return this->slotPosition;
}

 bool ACS_CS_DHCP_Info::isMacInitialized () const
{
	 // Check if there is a mac address
	 if ( this->macAddress && this->macLength )
	 {
		 // See that it isn't set to all zeros
		 char * tmpMac = new char[macLength];
		 memset(tmpMac, 0, macLength);
		 int compare = memcmp(this->macAddress, tmpMac, macLength);
		 delete [] tmpMac;

		 // Return true if macAddress isn't all zeros
		 return compare != 0;
	 }
	 else
	 {
		 return false;
	 }
}

 void ACS_CS_DHCP_Info::setFBN (ACS_CS_Protocol::CS_FBN_Identifier newFbn)
{
   this->fbn = newFbn;
}

 ACS_CS_Protocol::CS_FBN_Identifier ACS_CS_DHCP_Info::getFBN () const
{
   return this->fbn;
}

 bool ACS_CS_DHCP_Info::isPersistent () const
{
   return boardPersistent;
}

 void ACS_CS_DHCP_Info::setPersistent (bool persistent)
{
   boardPersistent = persistent;
}

 long unsigned ACS_CS_DHCP_Info::getScope () const
{
   return scope;
}

 void ACS_CS_DHCP_Info::setScope (unsigned long newScope)
{
   scope = newScope;
}

 ACS_CS_Protocol::CS_Side_Identifier ACS_CS_DHCP_Info::getSide () const
{
   return side;
}

 void ACS_CS_DHCP_Info::setSide (ACS_CS_Protocol::CS_Side_Identifier newSide)
{
   side = newSide;
}

 short unsigned ACS_CS_DHCP_Info::getSystemType () const
{
   return systemType;
}

 void ACS_CS_DHCP_Info::setSystemType (uint16_t type)
{
   systemType = type;
}

 short unsigned ACS_CS_DHCP_Info::getSysId () const
{
   return sysId;
}

 void ACS_CS_DHCP_Info::setSysId (uint16_t id)
{
    sysId = id;
}

std::string ACS_CS_DHCP_Info::getBootServer() const {
	return bootServer;
}

void ACS_CS_DHCP_Info::setBootServer(std::string server) {
	bootServer = server;
}

std::string ACS_CS_DHCP_Info::getBootPath() const {
	return bootPath;
}

void ACS_CS_DHCP_Info::setBootPath(std::string path) {
	bootPath = path;
}

std::string ACS_CS_DHCP_Info::convertAddress(uint32_t address) const
{
	string converted;
	char tempValueStr[INET_ADDRSTRLEN];

	const char * dst = inet_ntop(AF_INET, &address, tempValueStr, INET_ADDRSTRLEN);

	if (dst != 0)
		converted = dst;

	return converted;
}


std::string ACS_CS_DHCP_Info::generateHostname(ACS_CS_Protocol::SwitchType switchType)
{
	std::stringstream name;

	name << convertAddress(magazineAddress) << "_" << slotPosition;

	if (networkIdentifier == ACS_CS_Protocol::Eth_A){
		if(switchType == ACS_CS_Protocol::NOT_APPLICABLE)
			name << "_A";
		else if(switchType == ACS_CS_Protocol::CONTROL_SWITCH) {
				if (slotPosition == 0)
					name << "_local";
				else
					name << "_remote";
		}
		else if(switchType == ACS_CS_Protocol::TRANSPORT_SWITCH) {
				if (slotPosition == 0)
					name << "_tlocal";
				else
					name << "_tremote";
		}
	}	
	else if (networkIdentifier == ACS_CS_Protocol::Eth_B){
		if(switchType == ACS_CS_Protocol::NOT_APPLICABLE)
			name << "_B";
		else if(switchType == ACS_CS_Protocol::CONTROL_SWITCH) {
				if (slotPosition == 0)
					name << "_remote";
				else
					name << "_local";
		}
		else if(switchType == ACS_CS_Protocol::TRANSPORT_SWITCH) {
				if (slotPosition == 0)
					name << "_tremote";
				else
					name << "_tlocal";
		}
	}
	return name.str();
}
void ACS_CS_DHCP_Info::setClientId(const char *uid, int size)
{
  if (uid == 0)			// Check for valid memory
		return;
	else if (size < 1)		// Check for valid size
		return;

	if (this->clientId)	// Delete old UID
	{
		delete [] this->clientId;
		this->clientId = 0;
		this->clientIdLength = 0;
	}

	// Copy new UID
	this->clientIdLength = size;
	this->clientId = new char[size];

	if (this->clientId)
		memcpy(this->clientId, uid, size);
}


int ACS_CS_DHCP_Info::figureOutClientId (std::string productId, int a0, int a1, int a3, int slotid, int eth01, std::string &client, ACS_CS_Protocol::SwitchType switchType)
{
 //## begin ACS_CS_DHCP_Info::figureOutClientId%4D0107BA01C3.body preserve=yes

	 char raw[30]={0};
	 char strP[4]={0};
	 char strC[4]={0};
	 char str123[4]={0};
	 char str1234[6]={0};
	 char str12[255]={0};
	 char clientId[20]={0};

	 strcpy(raw,productId.c_str());

	 size_t ind=0;

	 int slashCount=0;

	 for(ind=0;ind<strlen(raw);ind++)
	 {

		 if(raw[ind]=='/')
		 {
			 slashCount++;
		 }
	 }

	 if((slashCount>2)|| (slashCount==0))
	 {
		 return 1;
	 }

	 int index=0;

	 strncpy(strP,raw,3);

	 if(checkNumeric(strP)==0)
	 {
		 sprintf(strP,"%s",strP+2);

		 strncpy(strC,raw+4,3);

		 index+=4;
	 }
	 else
	 {
		 sprintf(strP,"%s","-");
		 strncpy(strC,raw,3);
	 }

	 sprintf(strC,"%s",strC+2);

	 strncpy(str123,raw+index,3);

	 index+=3;


	 if(checkNumeric(str123)!=0)
	 {

		 // ROX1181/12222

		 int t=0;

		 for(t=0;t<20;t++)
		 {
			 if((raw[index+t]<='9') && raw[index+t]>='0')
			 {
				 break;
			 }
		 }

		 index+=t;

		 for(t=0;t<3;t++)
		 {
			 if(!(raw[index+t]<='9') && raw[index+t]>='0')
			 {
				 break;
			 }
		 }

		 memset(str123,0,strlen(str123));

		 strncpy(str123,raw+index,t);

		 index+=t;
	 }

	 int ris;

	 ris=strcspn(raw,"/");

	 if(ris==3)
	 {
		 char rawcpy[400]={0};
		 sprintf(rawcpy,"%s",raw);
		 rawcpy[3]='a';
		 ris=strcspn(rawcpy,"/");
	 }

	 int t;

	 for(t=0;t<20;t++)
	 {
		 if((raw[index+t]<'9') && raw[index+t]>'0')
		 {
			 break;
		 }
	 }

	 if((ris-index-t)>=0)
	 {
		 strncpy(str1234,raw+index+t,ris-index-t);

		 string tmpStr(str1234);
		 sprintf(str1234,"%-4s",tmpStr.c_str());

		 int y=0;

		 for(y=0;y<(int)strlen(str1234);y++)
		 {
			 if(str1234[y]==' ')
			 {
				 str1234[y]='-';
			 }
		 }
	 }
	 else
	 {
		 return 1;
	 }

	 if(strlen(str1234)>4)
	 {
		 return 1;
	 }

	 strncpy(str12,raw+ris+1,2);

	 sprintf(str12,"%02d",atoi(str12));

	 printf("\n%s",strC);
	 if(!(((strC[0]>='a') && (strC[0]<='z')) ||
			 ((strC[0]>='A') && (strC[0]<='Z'))))
	 {
		 return 1;
	 }

	 if(eth01==0){
		if (((slotid == 0) || (slotid ==25)) &&(switchType == ACS_CS_Protocol::CONTROL_SWITCH))			
				sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__local",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);
		else if (((slotid == 0) || (slotid ==25)) &&(switchType == ACS_CS_Protocol::TRANSPORT_SWITCH))			
				sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__tlocal",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);			
		 else
			sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__eth0",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);
	}
	 else{
		if (((slotid == 0) || (slotid ==25)) &&(switchType == ACS_CS_Protocol::CONTROL_SWITCH))			
				sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__remote",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);
		else if (((slotid == 0) || (slotid ==25)) &&(switchType == ACS_CS_Protocol::TRANSPORT_SWITCH))			
				sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__tremote",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);			
		else
			sprintf(clientId,"%s%s%s%s%s%x%x%x%02d__eth1",strP,strC,str123,str1234,str12,a3,a1,a0,slotid);
	}
	 client=clientId;

	 return 0;

 //## end ACS_CS_DHCP_Info::figureOutClientId%4D0107BA01C3.body
}

std::string ACS_CS_DHCP_Info::getContainerPackage () const
{
	return containerPackage;

}

void ACS_CS_DHCP_Info::setContainerPackage (const std::string &package)
{
	containerPackage=package;
}

std::string ACS_CS_DHCP_Info::getBladeProductNumber () const
{
	return bladeProductNumber;
}

void ACS_CS_DHCP_Info::setBladeProductNumber (const std::string &product)
{
	bladeProductNumber=product;
}

std::string ACS_CS_DHCP_Info::getCurrentLoadModuleVersion () const{

	return currentLoadModuleVersion;

}

std::string ACS_CS_DHCP_Info::getLoadModuleFileName () const{

	return loadModuleFileName;

}

void ACS_CS_DHCP_Info::setCurrentLoadModuleVersion (const std::string &curr){
	currentLoadModuleVersion=curr;
}

void ACS_CS_DHCP_Info::setLoadModuleFileName (const std::string &curr){
	loadModuleFileName=curr;
}

std::string ACS_CS_DHCP_Info::getDomainName () const{
	return "";
}

ACS_CS_ProductInfo ACS_CS_DHCP_Info::getProductInfo() const {
	return productInfo;
}

void ACS_CS_DHCP_Info::setProductInfo(const ACS_CS_ProductInfo &hwVer) {
	productInfo = hwVer;
}

void calcOption61IdEpb (){
}


int ACS_CS_DHCP_Info::checkNumeric (char *str)
{
 //## begin ACS_CS_DHCP_Info::checkNumeric%4D0108F4011C.body preserve=yes

	size_t lun=0,i;

	lun=strlen(str);

	for(i=0;i<lun;i++)
	{
		if(!((str[i]<='9') &&(str[i]>='0')))
		{
			return 1;
		}
	}

	return 0;

 //## end ACS_CS_DHCP_Info::checkNumeric%4D0108F4011C.body
}



void ACS_CS_DHCP_Info::setAcAddress(uint32_t ip)
{
	this->acIp=ip;
}

uint32_t ACS_CS_DHCP_Info::getAcAddress() const
{
	return this->acIp;
}

void ACS_CS_DHCP_Info::setTrapReceiverAddress(uint32_t ip)
{
	this->trapReceiver=ip;
}

uint32_t ACS_CS_DHCP_Info::getTrapReceiverAddress() const
{
	return this->trapReceiver;
}


void ACS_CS_DHCP_Info::nextTries()
{
	tries++;
}

int ACS_CS_DHCP_Info::getTries() const
{
	return tries;
}

void ACS_CS_DHCP_Info::resetTries()
{
	tries=0;
}

std::string ACS_CS_DHCP_Info::zeropad(std::string str,int length)
 {
         const int len=str.length();
         std::string result("");
         char *res=(char*)malloc(length+1);
         memset(res,48,length+1);
         res[length]='\0';
         int diff=0;
         if(length>0)
         {
                 diff=length-len;

                 if(diff<0)
                         diff=0;
         }

         sprintf(res+diff,"%s",str.c_str());

         result=res;
         free(res);
         return std::string(result);
 }





