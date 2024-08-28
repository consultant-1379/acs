/*
 * ACS_CS_IronsideHandler.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: estevol
 */

#include "ACS_CS_IronsideHandler.h"
#include "ACS_CS_DHCP_Info.h"
#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_IronsideHandler_TRACE);
#include "ACS_CS_ServiceHandler.h"
#include "ACS_CS_NetConfBuilderCommand.h"
#include "ACS_CS_NetConfBladeFilter.h"
#include "ACS_CS_NetConfCommandInt.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "acs_nclib_library.h"
#include "ACS_CS_Protocol.h"
#include <boost/regex.hpp>
#include <algorithm>
#include <cmath>
#include <climits>
#include <fstream>
#define MAX_NO_RETRY_TIMEOUT 6 //max timeout 404 sec
#define BUFLEN 2048
#define IRONSIDE_PORT 831

#define ERR_CODE_NO_ERROR			0
#define ERR_CODE_GENERAL_ERROR		-1
#define ERR_CODE_HW_NOT_PRESENT 	-2
#define ERR_CODE_IRONSIDE_FAILURE 	-3

static const std::string ERRCODE_ABSENT = "ABSENT";
static const std::string ERRCODE_UNSPEC = "UNSPEC";

static const boost::regex MAC_REG_EX("^([[:xdigit:]]{2}(:|-)?){5}[[:xdigit:]]{2}$");

ACS_CS_IronsideHandler::ACS_CS_IronsideHandler(ACS_CS_ServiceHandler* ParentService):
		ACS_CS_ShelfManager(ParentService),
		m_retryNo(0)
{
	acs_nclib_library::init();

}

ACS_CS_IronsideHandler::~ACS_CS_IronsideHandler()
{
	acs_nclib_library::exit();
}


void ACS_CS_IronsideHandler::addShelfHandler(ACS_CS_DHCP_Info &, ACS_CS_DHCP_Info &) {

}

void ACS_CS_IronsideHandler::removeShelfHandler(uint32_t , uint16_t ) {

}

bool ACS_CS_IronsideHandler::syncClients (std::vector<ACS_CS_DHCP_Info*> &) {

	//Unused method in DMX configuration
	return true;
}


void ACS_CS_IronsideHandler::cleanUp() {
	//TODO: Check if any cleanUp action has to be performed before calling destructor
}

void ACS_CS_IronsideHandler::checkOutQueue ()
{
	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_IronsideHandler::checkOutQueue()\n"
			"Entering method"));

	//removeCanceledClients();

	//ACS_CS_DHCP_Info * info = 0;

	size_t size = normalDhcpVector.size();
	ACS_CS_DEBUG(("Inside checkoutqueue NormalVector size %d", normalDhcpVector.size()));

	queueUpdated = false;

	dhcpQueueLock.start_reading();
	std::vector<ACS_CS_DHCP_Info> copyQueue = normalDhcpVector;
	dhcpQueueLock.stop_reading();

	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
			"Queue size %d\n", size));

	bool onlyCachedShelves = false;

	for (size_t i = 0; i < copyQueue.size() && !queueUpdated && !exit_; i++)
	{
		std::string mac_string;
		ACS_CS_DHCP_Info info = copyQueue[i];

		int slot = info.getSlot();

		char shelfAddr[INET_ADDRSTRLEN];
		uint32_t naddr = info.getMagAddress();
		inet_ntop(AF_INET, &naddr, shelfAddr, sizeof(shelfAddr));

		int mag = -1;

		mag = magAddrToPhysicalAddr(shelfAddr);

		if(mag != -1)
		{
			char cmag[32]={0};
			sprintf(cmag,"%d",mag);

			//Fetch the shelfId from physical Address
			std::string shelf_id;
			int ret = getShelfidFromPhysicalAddress(cmag, shelf_id, onlyCachedShelves);

			ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
					"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
					"shelf id%s\n", shelf_id.c_str()));

			if(shelf_id.empty())
			{

				//Shelf not found
				if(ERR_CODE_NO_ERROR == ret)
				{
					//Query was successful but shelf was not found ==> Move the item at the bottom of the queue
					putAtBottomIntoDhcpQueue(info);

					//From this moment on we don't reload full shelf list from BSP until next queue iteration.
					onlyCachedShelves = true;

					ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
							"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
							"Unable to fetch shelf id as shelf is not present in BSP\n"));
				}
				else if (ERR_CODE_IRONSIDE_FAILURE == ret)
				{
					putAtBottomIntoDhcpQueue(info);
					incrementRetryNo();

					ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
							"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
							"Unable to fetch shelf id due to ironside failure\n"));
					return; // in case ironside failed
				}
			}
			else
			{
				ACS_CS_DEBUG(("checkOutQueue bladeId = %s-%d", shelf_id.c_str(), slot));

				int result = getMacaddress(slot,shelf_id,mac_string);

				ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
						"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
						"Mac address %s\n", mac_string.c_str()));

				if(ERR_CODE_NO_ERROR == result)
				{
					int macAddition = 0;
					// MAC for EthA == Aggregated MAC + 1
					if (info.getNetwork() == ACS_CS_Protocol::Eth_A)
						macAddition = 1;
					// MAC for EthB == Aggregated MAC + 2
					else if (info.getNetwork() == ACS_CS_Protocol::Eth_B)
						macAddition = 2;
					// Add 1 or 2 to the MAC. Since a MAC is 6 byte and doesn't conform
					// to a standard data type, the addition is carried out byte by byte

					char mac[30] = {0};

					int length = hexCharToBinChar(mac_string.c_str(),mac,macAddition);

					ACS_CS_DEBUG(("firstMacAddr + addition = %s, shelf_id = %s, slot = %d", mac, shelf_id.c_str(), slot));

					info.setMAC(mac, length);
					serviceCs->MacUpdate(&info);

					ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
							"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
							"Mac Update()"));

					if((info.getFBN() == ACS_CS_Protocol::FBN_CPUB ) && (info.getNetwork() == ACS_CS_Protocol::Eth_A))
					{
						result = getProductInfo(slot,shelf_id,info);

						if (ERR_CODE_IRONSIDE_FAILURE == result)
						{
							//Ironside failure on prod Info collection. Item is moved at the bottom but we remain in the loop
							//in order to continue fetching the mac addresses
							ACS_CS_DEBUG(("ERROR: Unable to fetch the product info for blade %s-%d", shelf_id.c_str(), slot));
							putAtBottomIntoDhcpQueue(info);
							incrementRetryNo();
						}
						else
						{
							ACS_CS_DEBUG(("Calling Update along with prod info..for slot %d", slot));
							serviceCs->ProdInfoUpdate(&info);
						}
					}

					if (ERR_CODE_NO_ERROR == result)
					{
						//MAC addresses (and Prod Info in case of CP) have been collected. Remove item from the queue
						canceledClients.push(info);
					}

				}
				else if(ERR_CODE_HW_NOT_PRESENT == result)
				{
					// In case of HW not present remove the item from the queue
					canceledClients.push(info);
				}
				else if (ERR_CODE_IRONSIDE_FAILURE == result)
				{
					// In case of ironside failure move the item at the bottom of the queue and exit
					ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
							"(%t) ACS_CS_IronsideHandler::checkOutQueue()\n"
							"Unable to fetch mac address\n"));
					ACS_CS_DEBUG(("ERROR: Unable to fetch the MAC address for blade %s-%d", shelf_id.c_str(), slot));
					putAtBottomIntoDhcpQueue(info);
					incrementRetryNo();
					return;
				}
			}
		}	
	}

	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_IronsideHandler::checkOutQueue()leaving method\n"
			"Queue size %d\n",normalDhcpVector.size() ));

}

void ACS_CS_IronsideHandler::updateRetries(const char* hostName)
{
	dhcpQueueLock.start_writing();

	for (size_t i = normalDhcpVector.size(); i > 0; i--)
	{
		if (strcmp(normalDhcpVector[i - 1].getHostName().c_str(),hostName) == 0)
		{
			normalDhcpVector[i - 1].nextTries();
			break;
		}
	}
	dhcpQueueLock.stop_writing();
}

int ACS_CS_IronsideHandler::updateTimeout() {

	//TODO: Update Timeout
	int aMonth = 108000000;
	int fiveSeconds = 5000;
	int oneSecond = 1000;
	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
                        "ACS_CS_IronsideHandler::updateTimeout() m_retryNo=%d\n",m_retryNo));

	int timeout = 0;

	if (outQueueEmpty)
	{
		timeout = aMonth;
		m_retryNo = 0;
	}
	else
	{
		if(m_retryNo == 0)
		{
			timeout = fiveSeconds;
		}
		else
		{
			if(m_retryNo <= MAX_NO_RETRY_TIMEOUT)
			{
				int temp = max(m_retryNo,4);
				double value = exp(temp);
				timeout = (oneSecond * static_cast<int>(value+0.5));
			}
			else
			{
				double value = exp(MAX_NO_RETRY_TIMEOUT);
				timeout = (oneSecond * static_cast<int>(value+0.5));
			}
		}
	}
	
	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_IronsideHandler::updateTimeout() timeout=%d\n",timeout));
	
	return timeout;
}


void ACS_CS_IronsideHandler::addToMac(unsigned char * mac, int size, int add)
{
	if (size == 6 && add > 0)
	{
	 unsigned char b0 = mac[0];
	 unsigned char b1 = mac[1];
	 unsigned char b2 = mac[2];
	 unsigned char b3 = mac[3];
	 unsigned char b4 = mac[4];
	 unsigned char b5 = mac[5];


	 unsigned char before = b5;
	 b5 += add;

	 if (before > b5)
	 {
		before = b4;
		b4++;

		if (before > b4)
		{
		   before = b3;
		   b3++;

		   if (before > b3)
		   {
			  before = b2;
			  b2++;

			  if (before > b2)
			  {
				 before = b1;
				 b1++;

				 if (before > b1)
					b0++;
			  }
		   }
		}
	 }

	 memcpy(mac, &b0, 1);
	 memcpy(mac + 1, &b1, 1);
	 memcpy(mac + 2, &b2, 1);
	 memcpy(mac + 3, &b3, 1);
	 memcpy(mac + 4, &b4, 1);
	 memcpy(mac + 5, &b5, 1);
	}
}


int ACS_CS_IronsideHandler::hexCharToBinChar(const char *input,char *out,int add)
{
    std::stringstream ss(input);

    std::string item;

    char delim = ':';

    std::vector<std::string> elems;

    while(std::getline(ss, item, delim))
    {
    	ACS_CS_DEBUG(("DEBUG: Item: %s", item.c_str()));
    	elems.push_back(item);
    }

    uint32_t length = elems.size();

    for (unsigned int i = 0; i < length; ++i)
    {
    	char* endPtr;
    	unsigned char tmp = (unsigned char) strtoul(elems.at(i).c_str(), &endPtr, 16);
    	out[i] = (char) tmp;
    	ACS_CS_DEBUG(("DEBUG: out[%d] = %d", i, out[i]));
    }

    addToMac((unsigned char*) out, length, add);

    return length;
}


int ACS_CS_IronsideHandler::magAddrToPhysicalAddr(std::string shelfaddr)
{

	std::stringstream ss(shelfaddr);

	std::string item;

	char delim = '.';

	std::vector<std::string> elems;

	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	char tmp[32] = {0};

	if (elems.size() == 4)
	{
		int plug0 = atoi(elems.at(0).c_str());
		int plug1 = atoi(elems.at(1).c_str());
		int plug3 = atoi(elems.at(3).c_str());

		sprintf(tmp,"%x%x%x",plug3,plug1,plug0);
	}
	else
		return -1;

	char* endPtr;

	int mag = strtol(tmp, &endPtr,16);

	return mag;

}

std::string ACS_CS_IronsideHandler::physicalAddrToMagAddr(int phyAddr)
{
	std::string ret_val("");

	if (phyAddr >= 0)
	{
		char tmp[32] = {0};
		sprintf(tmp, "%03x", phyAddr);


		char plug0_str[2] = {tmp[2], 0};
		char plug1_str[2] = {tmp[1], 0};
		char plug3_str[2] = {tmp[0], 0};

		char* endPtr;
		int plug0 = strtol(plug0_str, &endPtr,16);
		int plug1 = strtol(plug1_str, &endPtr,16);
		int plug3 = strtol(plug3_str, &endPtr,16);

		char addr[16] = {0};

		sprintf(addr, "%d.%d.0.%d", plug0, plug1, plug3);

		ret_val = addr;
	}

	return ret_val;

}
int ACS_CS_IronsideHandler::getShelfidFromPhysicalAddress(std::string i_physical_Address,std::string &o_shelfId, bool onlyCached)
{
	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"ACS_CS_IronsideHandler::getShelfid()\n"
			"Entering method"));

	int result = ERR_CODE_NO_ERROR;

	int i_phAddr = atoi(i_physical_Address.c_str());
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(syncObject, true);

	o_shelfId = "";

	//Look for the shelf ID in the map...
	std::map<int, std::string>::iterator it = shelfMap.find(i_phAddr);

	if (it != shelfMap.end())
	{
		o_shelfId = it->second;
	}
	else if (!onlyCached)
	{
		//... if not found then ask BSP.
		result = ERR_CODE_IRONSIDE_FAILURE;
		char filter[16] = {0};
		std::string data;
		std::vector<std::string> tokens;
		std::string buf;

		snprintf(filter, 13, "SHLF--------");

		ACS_CS_NetConfBuilderCommand commandBuilder;
		ACS_CS_NetConfCommandInt *com = commandBuilder.make(netconfBuilder::UDP);

		if(com)
		{
			com->setFilter(filter);

			if (ERR_CODE_NO_ERROR == com->executeUdp(data))
			{
				stringstream ss(data);

				result = ERR_CODE_NO_ERROR;

				//Ironside query was successful. Populate shelf map now.
				while (ss >> buf) {
					tokens.push_back(buf);
				}

				for(unsigned int i=0;i<tokens.size();i++)
				{
					std::string shelfid,physadd;
					string::size_type pos_start = 0, pos_end;
					pos_end = tokens[i].find(":",pos_start);

					if (pos_end != string::npos)
					{
						shelfid = tokens[i].substr(0, pos_end);
						physadd = tokens[i].substr(pos_end+1);
					}

					int tmpPhysAddr = atoi(physadd.c_str());

					shelfMap.insert(std::pair<int, std::string>(tmpPhysAddr, shelfid));

					if (i_phAddr == tmpPhysAddr)
					{
						o_shelfId = shelfid;
					}
				}
			}

			delete com;
		}
	}

	return result;
}

int ACS_CS_IronsideHandler::getMacaddress(int slot ,string Shelf_Id,string &macaddress)
{
	ACS_CS_FTRACE((ACS_CS_IronsideHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_IronsideHandler::getMacaddress()\n"
			"Mac address"));
	int result = ERR_CODE_GENERAL_ERROR;
	string macaddr;
	string data;
	char filter[16] = {0};
	ACS_CS_NetConfBuilderCommand commandBuilder;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(syncObject, true);
	ACS_CS_NetConfCommandInt *com=commandBuilder.make(netconfBuilder::UDP);

	snprintf(filter, 13, "BLD:%02d%02dMAC-",atoi(Shelf_Id.c_str()),slot);

	if(com)
	{
		com->setFilter(filter);
		int ironside_result = com->executeUdp(data);

		if (!data.empty() && data[data.length()-1] == '\n') {
			data.erase(data.length()-1);
		}

		if(ironside_result == ERR_CODE_NO_ERROR)
		{
			if(data.size() < 17)
			{
				//If error message is UNSPEC or ABSENT we treat this as an HW not present scenario
				if((strcmp(data.c_str(),ERRCODE_UNSPEC.c_str()) == 0)|| (strcmp(data.c_str(),ERRCODE_ABSENT.c_str()) == 0) )
				{
					result = ERR_CODE_HW_NOT_PRESENT;
				}
			}
			else
			{
				if(boost::regex_match(data.begin(),data.end(), MAC_REG_EX ))
				{
					macaddress = data.substr(0,17);
					result = ERR_CODE_NO_ERROR;
				}
			}
		}
		else
		{
			result = ERR_CODE_IRONSIDE_FAILURE;
		}

		delete com;
	}

	return result;
        
}
int ACS_CS_IronsideHandler::getProductInfo(int slot ,string shelf_id,ACS_CS_DHCP_Info &info)
{
	char filter[16] = {0};
	string udpdata;
	string buf;
	vector<string> tokens;
	int result = ERR_CODE_IRONSIDE_FAILURE;
	ACS_CS_ProductInfo prodInfo;        
	snprintf(filter, 13, "BLD:%02d%02dHWI-",atoi(shelf_id.c_str()),slot);
	ACS_CS_NetConfBuilderCommand commandBuilder;
	ACS_CS_NetConfCommandInt *com = commandBuilder.make(netconfBuilder::UDP);

	if(!com)
		return ERR_CODE_GENERAL_ERROR;

	com->setFilter(filter);

	if(ERR_CODE_NO_ERROR == com->executeUdp(udpdata))
	{
		stringstream ss(udpdata);

		result = ERR_CODE_NO_ERROR;

		while (getline(ss,buf)!=0){
			tokens.push_back(buf);
		}
		for(unsigned int i=0; i<tokens.size(); i++)
		{
			std::string shelfid,physadd;
			string::size_type pos_start = 0, pos_end;
			pos_end = tokens[i] .find(":",pos_start);
			if (pos_end != string::npos)
			{
				std::string str="";
				str = tokens[i].substr(0, pos_end);
				if(str.compare("productName")==0)
					prodInfo.productName = tokens[i].substr(pos_end+1);
				else if(str.compare("productNumber")==0)
					prodInfo.productNumber = tokens[i].substr(pos_end+1);
				else if(str.compare("productRevisionState")==0)
					prodInfo.productRevision = tokens[i].substr(pos_end+1);
				else if(str.compare("vendorName")==0)
					prodInfo.productVendor = tokens[i].substr(pos_end+1);
				else if(str.compare("serialNumber")==0)
					prodInfo.serialNumber = tokens[i].substr(pos_end+1);
				else if(str.compare("manufacturingDate")==0)
					prodInfo.manufacturingDate = tokens[i].substr(pos_end+1);
			}	
		}

		info.setProductInfo(prodInfo);
	}

	if (com)
		delete com;

	return result;
}
int ACS_CS_IronsideHandler::getPhysicalAddressFromShelfId(std::string i_shelfId)
{
	std::map<int, std::string>::iterator it;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(syncObject, true);
	int o_phAddr = -1;
	bool found = false;
	int result = ERR_CODE_GENERAL_ERROR;
	char filter[16] = {0};
	string data;
	vector<string> tokens;
	string buf;

	//Look for physical address in the map
	for (it = shelfMap.begin(); it != shelfMap.end() && !found; it++)
	{
		if (i_shelfId.compare(it->second) == 0)
		{
			o_phAddr = it->first;
			found = true;
			break;
		}
	}

	// If not in the map fetch it from BSP
	if (!found)
	{
		ACS_CS_NetConfBuilderCommand commandBuilder;
		ACS_CS_NetConfCommandInt *com=commandBuilder.make(netconfBuilder::UDP);

		snprintf(filter, 13, "SHLF--------");
		if(com)
		{
			com->setFilter(filter);
			result=com->executeUdp(data);
			stringstream ss(data);
			if(result)
			{
				while (ss >> buf){
					tokens.push_back(buf);
				}
				for(unsigned int i=0;i<tokens.size();i++)
				{
					std::string shelfid,physadd;
					string::size_type pos_start = 0, pos_end;
					pos_end = tokens[i].find(":",pos_start);

					if (pos_end != string::npos)
					{
						shelfid = tokens[i].substr(0, pos_end);
						physadd = tokens[i].substr(pos_end+1);
					}

					int phAddr = atoi(physadd.c_str());
					shelfMap.insert(std::pair<int, std::string>(phAddr, shelfid));

					if (i_shelfId.compare(shelfid) == 0)
					{
						o_phAddr = phAddr;
					}
				}	
			}

			delete com;
		}
	}

	return o_phAddr;
}

std::string ACS_CS_IronsideHandler::getMagAddressFromShelfId(std::string shelfId)
{
	return physicalAddrToMagAddr(getPhysicalAddressFromShelfId(shelfId));
}

std::string ACS_CS_IronsideHandler::getMagAddressFromShelfId(int shelfId)
{
     stringstream sid;
     sid << shelfId;
     std::string shelfId_str = sid.str();

     return getMagAddressFromShelfId(shelfId_str);

}

void ACS_CS_IronsideHandler::incrementRetryNo()
{
	if(m_retryNo == INT_MAX)
		return;
	else
		m_retryNo++;
}
