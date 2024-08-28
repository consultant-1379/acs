//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ACS_CS_Trace.h"
#include "acs_apgcc_paramhandling.h"

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImEquipment.h"
#include "ACS_CS_Util.h"
#include "ACS_APGCC_CommonLib.h"

// ACS_CS_Registry
#include "ACS_CS_Registry.h"


ACS_CS_Trace_TDEF(ACS_CS_Registry_TRACE);

#include "acs_apgcc_omhandler.h"
using std::string;
using namespace ACS_CS_Registry_NS;

#define AP1_A_BGCI_ADDR 0x00000013
#define AP1_B_BGCI_ADDR 0x00000014
#define AP2_A_BGCI_ADDR 0x00000015
#define AP2_B_BGCI_ADDR 0x00000016

int ACS_CS_Registry::s_isBladeCluster = -1;
uint32_t ACS_CS_Registry::s_dmxcAddress[2] = {0};
uint32_t ACS_CS_Registry::s_apgBgciAddress[2][2][2] = {{{0}}};
bool ACS_CS_Registry::isDebugEnabled = false;
int ACS_CS_Registry::s_hwVersion = UNDEF_HWV;
int ACS_CS_Registry::s_nodeArchitecture = NODE_UNDEFINED;
int ACS_CS_Registry::s_apgNumber = -1;
int ACS_CS_Registry::s_aptType = -1;
int ACS_CS_Registry::s_apzType = -1;
int ACS_CS_Registry::s_enableDHCP = -1;
int ACS_CS_Registry::mau_type = 0;//ACS_CS_ImmMapper::UNDEFINED;


short unsigned ACS_CS_Registry::getFrontAPG ()
{

	unsigned short frontAp = 2001;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);

	ACS_CS_ImCpCluster* bladeInfo = dynamic_cast<ACS_CS_ImCpCluster*>(base);

	if(bladeInfo)
	   // Change 2000 to a correctly calculated value
		frontAp = 2000 + bladeInfo->frontAp;

	delete immReader;
	delete base;
	return frontAp;
}

bool ACS_CS_Registry::isMultipleCPSystem ()
{
	if (s_isBladeCluster != -1) return (s_isBladeCluster ? true : false);

	bool multipleCPSystem = ACS_CS_DEFAULT_MULTIPLE_CP_SYSTEM;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_EQUIPMENT);

	ACS_CS_ImEquipment* info = dynamic_cast<ACS_CS_ImEquipment*>(base);

	if(info)
	{
		multipleCPSystem = info->isBladeClusterSystem;
		s_isBladeCluster = info->isBladeClusterSystem ? 1 : 0;
	}

	delete immReader;
	delete base;

	return multipleCPSystem;
}

bool ACS_CS_Registry::isTestEnvironment ()
{
	bool isTestEnvironment = false;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);

	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	if(info)
		isTestEnvironment = info->isTestEnvironment;

	delete immReader;
	delete base;

	return isTestEnvironment;
}


std::string ACS_CS_Registry::getAPTType ()
{
	if (s_aptType == -1)
	{
		acs_apgcc_paramhandling acsParamHandling;
		acsParamHandling.getParameter(ACS_CS_ImmMapper::RDN_APZFUNCTIONS, ACS_CS_ImmMapper::ATTR_APT_TYPE, &s_aptType);
	}

	std::string aptType;

	switch(s_aptType){
	case 1: aptType = "HLR";
	break;
	case 2: aptType = "BSC";
	break;
	case 3: aptType = "WLN";
	break;
	case 4: aptType = "TSC";
	break;
	case 5: aptType = "IPSTP";
	break;
	default: aptType = "MSC";
	}

	return aptType;
}

bool ACS_CS_Registry::isDebug()
{
     return isDebugEnabled;
	//return true;
}


void ACS_CS_Registry::setDebug(bool debug)
{
	isDebugEnabled = debug;
}

int ACS_CS_Registry::getApzSystem()
{
	int apzSystem = 4;

	char * envVar = getenv("CS_APZSYS");

	if(envVar != 0)
	{
		std::stringstream is;
		is << envVar;
		is >> apzSystem;
	}

	return apzSystem;
}

bool ACS_CS_Registry::TImmAccess::getImmAttributeInt(const std::string &strDn, const std::string &strAttr, int *iVal)
{
  ACS_CC_ReturnType nResult;
  ACS_CC_ImmParameter Param;
  OmHandler omHandler;
  char *name_attrPath;
  // Initialize
  nResult = ACS_CC_FAILURE;
  name_attrPath = NULL;
  // Initialize the Handler
  nResult = omHandler.Init();
  // Check error
  if (nResult != ACS_CC_SUCCESS){
    //std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
  }
  // Continut if no error
  if(nResult == ACS_CC_SUCCESS){
    name_attrPath = const_cast<char*>(strAttr.c_str());
    Param.attrName = name_attrPath;
    nResult = omHandler.getAttribute(strDn.c_str(), &Param );
    // Check error
    if ( nResult != ACS_CC_SUCCESS ){
      //std::cout << "ERROR: Param FAILURE!!!\n" << std::endl;
    }
  }
  // Check if no error
  if(nResult == ACS_CC_SUCCESS){
    //std::cout << " --> Param value########: " << *(int*)Param.attrValues[0] << std::endl;
    // get the value
    (*iVal) = (*(int*)Param.attrValues[0]);
    // Finalize the handle
    nResult = omHandler.Finalize();
    // Check if error
    if (nResult != ACS_CC_SUCCESS){
      //std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
    }
  }
  // Exit from method
  return (nResult == ACS_CC_SUCCESS);
}

bool ACS_CS_Registry::TImmAccess::getImmAttributeString(const std::string &strDn, const std::string &strAttr, std::string *strVal)
{
  ACS_CC_ReturnType nResult;
  ACS_CC_ImmParameter Param;
  OmHandler omHandler;
  char *name_attrPath;
  // Initialize
  nResult = ACS_CC_FAILURE;
  name_attrPath = NULL;
  // Initialize the Handler
  nResult = omHandler.Init();
  // Check error
  if (nResult != ACS_CC_SUCCESS){
    //std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
  }
  // Continut if no error
  if(nResult == ACS_CC_SUCCESS){
    name_attrPath = const_cast<char*>(strAttr.c_str());
    Param.attrName = name_attrPath;
    nResult = omHandler.getAttribute(strDn.c_str(), &Param );
    // Check error
    if ( nResult != ACS_CC_SUCCESS ){
      //std::cout << "ERROR: Param FAILURE!!!\n" << std::endl;
    }
  }
  // Check if no error
  if(nResult == ACS_CC_SUCCESS){
    //std::cout << " --> Param value########: " << *(char*)Param.attrValues[0] << std::endl;
    // get the value
    (*strVal) = (char*)(Param.attrValues[0]);
    // Finalize the handle
    nResult = omHandler.Finalize();
    // Check if error
    if (nResult != ACS_CC_SUCCESS){
      //std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
    }
  }
  // Exit from method
  return (nResult == ACS_CC_SUCCESS);
}

bool ACS_CS_Registry::IsCba()
{
	int nodeArch = 0;
	getNodeArchitecture(nodeArch);

	return ( (NODE_SCX == nodeArch || NODE_SMX == nodeArch) ? true : false);

}

bool ACS_CS_Registry::getNodeArchitecture(int &architecture)
{
	bool res = true;

	if ( NODE_UNDEFINED == s_nodeArchitecture )
	{
		ACS_CS_Registry::TImmAccess immAccess;
		if (!immAccess.getImmAttributeInt(ACS_CS_ImmMapper::RDN_APZFUNCTIONS, ACS_CS_ImmMapper::ATTR_NODE_ARCHITECTURE, &s_nodeArchitecture))
			res = false;
	}

	architecture = s_nodeArchitecture;

	return res;
}

bool ACS_CS_Registry::getApgNumber(int &apgNumber)
{
	if (s_apgNumber != -1)
	{
		apgNumber = s_apgNumber;
		return true;
	}

	ACS_CS_Registry::TImmAccess immAccess;

	bool res = immAccess.getImmAttributeInt(ACS_CS_ImmMapper::RDN_APZFUNCTIONS, ACS_CS_ImmMapper::ATTR_NODE_NUMBER, &s_apgNumber);

	if (res)
		apgNumber = s_apgNumber;

	return res;

}

bool ACS_CS_Registry::getDmxcAddress(uint32_t &ipA, uint32_t &ipB)
{
	bool result = true;

	if (s_dmxcAddress[0] == 0 || s_dmxcAddress[1] == 0)
	{
		uint32_t subnet[2] = {0};
		uint32_t netmask[2] = {0};

		//Get BGCI subnet and netmask
		result = ACS_CS_Registry::getBGCIVlan(subnet, netmask);

		//Whatever the subnet, DMXC will always have .1 address
		if (result)
		{
			s_dmxcAddress[0] = (subnet[0] & netmask[0]) | 0x00000001;
			s_dmxcAddress[1] = (subnet[1] & netmask[1]) | 0x00000001;
		}

	}

	if (result)
	{
		ipA = s_dmxcAddress[0];
		ipB = s_dmxcAddress[1];
	}

	return result;
}

bool ACS_CS_Registry::getApgBgciAddress(uint16_t apgNr, uint16_t side, uint32_t &ipA, uint32_t &ipB)
{
	bool result = true;

	if (0 < apgNr && apgNr <= 2 && (side == 0 || side == 1))
	{

		if (s_apgBgciAddress[apgNr-1][side][0] == 0 || s_apgBgciAddress[apgNr-1][side][1] == 0)
		{
			uint32_t subnet[2] = {0};
			uint32_t netmask[2] = {0};
			//int apgNr = 0;

			//Select the right IP addresses (last byte) depending on APG number
			uint32_t apAddr = 0;
			if (side == 0)
			{
				apAddr = (apgNr == 1)? AP1_A_BGCI_ADDR: AP2_A_BGCI_ADDR;
			}
			else if (side == 1)
			{
				apAddr = (apgNr == 1)? AP1_B_BGCI_ADDR: AP2_B_BGCI_ADDR;
			}

			//Get BGCI subnet and netmask
			result = ACS_CS_Registry::getBGCIVlan(subnet, netmask);
			if (result)
			{
				s_apgBgciAddress[apgNr-1][side][0] = (subnet[0] & netmask[0]) | apAddr;
				s_apgBgciAddress[apgNr-1][side][1] = (subnet[1] & netmask[1]) | apAddr;
			}
		}
	}
	else
	{
		result=false;
	}

	if (result)
	{
		ipA = s_apgBgciAddress[apgNr-1][side][0];
		ipB = s_apgBgciAddress[apgNr-1][side][1];
	}


	return result;
}

bool ACS_CS_Registry::getSOLVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
{
	ACS_CS_ImIMMReader reader;

	std::vector<std::string> classNames;
	classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN);

	std::vector<std::string> vlanRdn = reader.getClassInstancesRdns(classNames);

	//Look for SOL VLANs among APZ VLANs
	for (std::vector<std::string>::iterator it = vlanRdn.begin(); it != vlanRdn.end(); it++)
	{
		std::string vlanName = ACS_CS_ImUtils::getIdValueFromRdn(*it);
		bool solA, solB = false;

		if ((solA = (vlanName.compare(ACS_CS_NS::VLAN_SOL_A) == 0)) ||
				(solB = (vlanName.compare(ACS_CS_NS::VLAN_SOL_B) == 0)))
		{
			ACS_CS_ImBase* base = reader.getObject(*it);

			if (base)
			{
				ACS_CS_ImVlan* vlan = dynamic_cast<ACS_CS_ImVlan*>(base);

				uint32_t ip = 0;
				uint32_t mask = 0;

				//Fetch subnet and netmask
				if (vlan)
				{
					in_addr address;
					if (inet_aton(vlan->networkAddress.c_str(), &address) != -1){
						ip = htonl(address.s_addr);
					}

					if (inet_aton(vlan->netmask.c_str(), &address) != -1){
						mask = htonl(address.s_addr);
					}
				}

				delete base;

				if (ip == 0 || mask == 0)
				{
					return false;
				}
				else
				{
					int index = (solA)? 0: 1;
					subnet[index] = ip;
					netmask[index] = mask;
				}
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}
bool ACS_CS_Registry::getBGCIVlan(uint32_t (&subnet)[2], uint32_t (&netmask)[2])
 {
	 ACS_CS_ImIMMReader reader;

	 std::vector<std::string> classNames;
	 classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN);

	 std::vector<std::string> vlanRdn = reader.getClassInstancesRdns(classNames);

	 //Look for BGCI VLANs among APZ VLANs
	 for (std::vector<std::string>::iterator it = vlanRdn.begin(); it != vlanRdn.end(); it++)
	 {
		 std::string vlanName = ACS_CS_ImUtils::getIdValueFromRdn(*it);

		 bool bgciA, bgciB = false;

		 if ((bgciA = (vlanName.compare(ACS_CS_NS::VLAN_BGCI_A) == 0)) ||
				 (bgciB = (vlanName.compare(ACS_CS_NS::VLAN_BGCI_B) == 0)))
		 {
			 ACS_CS_ImBase* base = reader.getObject(*it);

			 if (base)
			 {
				 ACS_CS_ImVlan* vlan = dynamic_cast<ACS_CS_ImVlan*>(base);

				 uint32_t ip = 0;
				 uint32_t mask = 0;

				 //Fetch subnet and netmask
				 if (vlan)
				 {
					 in_addr address;
					 if (inet_aton(vlan->networkAddress.c_str(), &address) != -1){
						 ip = htonl(address.s_addr);
					 }

					 if (inet_aton(vlan->netmask.c_str(), &address) != -1){
						 mask = htonl(address.s_addr);
					 }
				 }

				 delete base;

				 if (ip == 0 || mask == 0)
				 {
					 return false;
				 }
				 else
				 {
					 int index = (bgciA)? 0: 1;

					 subnet[index] = ip;
					 netmask[index] = mask;
				 }
			 }
			 else
			 {
				 return false;
			 }
		 }
	 }

	 return true;
 }

bool ACS_CS_Registry::getApgHwVersion(int &hwVersion)
{
	bool result = true;

	if (s_hwVersion == UNDEF_HWV)
	{
		ACS_APGCC_CommonLib acsApgccCommonLib;
		ACS_APGCC_HWINFO hwInfo;
		ACS_APGCC_HWINFO_RESULT hwInfoResult;

		acsApgccCommonLib.GetHwInfo(&hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION);

		result = (ACS_APGCC_HWINFO_SUCCESS == hwInfoResult.hwVersionResult);

		if(result)
		{
			switch(hwInfo.hwVersion)
			{

			case ACS_APGCC_HWVER_GEP1:
				s_hwVersion = APG43;
				break;

			case ACS_APGCC_HWVER_GEP2:
				s_hwVersion = APG43_2;
				break;

			case ACS_APGCC_HWVER_GEP5:
				s_hwVersion = APG43_3;
				break;

			case ACS_APGCC_HWVER_VM:
				s_hwVersion = APG43_VM;
				break;

			case ACS_APGCC_HWVER_GEP7:
				s_hwVersion = APG43_4;
				break;
			default:
				s_hwVersion = UNDEF_HWV;
				result=false;
				break;
			}
		}
		else
		{
			result = false;
		}
	}

	hwVersion = s_hwVersion;

	return result;
}

int ACS_CS_Registry::getAPZType()
{
	if (s_apzType == -1)
	{
		s_apzType =  ACS_CS_ImUtils::readAPZType();
	}

	return s_apzType;

}

int ACS_CS_Registry::getDHCPOnOffFlag()
{
  // The flag s_enableDHCP is set to true by default, irrespective of IMM DHCP ON/OFF availability.
  // In native envrionment, IMM DHCP ON/OFF toggle is not available. so the flag s_enableDHCP holds 1(s_enableDHCP = 1)
  // In ECS based Virtual environment, IMM DHCP ON/OFF toggle is set to OFF, so the s_enableDHCP flag holds 0(s_enableDHCP = 0)
  // In VMWare based Virtual envronment, IMM DHCP ON/OFF toggle is set to ON, so the s_enableDHCP flag holds 1(s_enableDHCP = 1)
	if (s_enableDHCP == -1)
	{
		s_enableDHCP = 1;
		if (ACS_CS_ImUtils::readDHCPToggle().compare(ACS_CS_ImmMapper::TOGGLE_OFF) == 0){
			s_enableDHCP=0;
		}
	}
	
	return s_enableDHCP;
}


//Convert a MAC address in HEX format (aa:bb:cc:dd:ee:ff) in an array of 6 char each one representing an octet of the mac.
//Returns the length of char array in case of success, -1 in case of failure.
//If out is a null pointer it just validates the input MAC.
int ACS_CS_Registry::hexMACtoDecMAC(const char *input, char *out)
{
	std::stringstream ss(input);

	std::string item;

	char delim = ':';

	std::vector<std::string> elems;

	while(std::getline(ss, item, delim))
	{
		//ACS_CS_DEBUG(("DEBUG: Item: %s", item.c_str()));
		elems.push_back(item);
	}

	if (elems.size() == 6)
	{

		for (unsigned int i = 0; i < 6; ++i)
		{
			const char* octet = elems.at(i).c_str();

			if (strlen(octet) == 2 && isxdigit(octet[0]) && isxdigit(octet[1]))
			{
				if (out)
				{
					char* endPtr;
					unsigned char tmp = (unsigned char) strtoul(octet, &endPtr, 16);

					out[i] = (char) tmp;
					//ACS_CS_DEBUG(("DEBUG: out[%d] = %d", i, out[i]));
				}
			}
			else
				return -1;
		}

	}
	else
		return -1;

	return 6;
}
void ACS_CS_Registry::loadDefaultMauType ()
{
	if (  mau_type == 0 )//UNDEFINED
	{
		string line;
		int nodeArch = 0;
		getNodeArchitecture(nodeArch);

		if(nodeArch == NODE_VIRTUALIZED)
		{
			mau_type=ACS_CS_ImmMapper::MAUS;//MAUS
		}
		else
		{
			//default setting in native environment
			mau_type = ACS_CS_ImmMapper::MAUB;

			std::ifstream inFile;
			inFile.open(ACS_CS_ImmMapper::MAUTYPE_PATH.c_str());

			if(inFile.is_open())
			{
				while(getline(inFile, line))
				{
					if(line!="")
					{
						if (strcmp(line.c_str(),"MAUB") == 0)
						{
							mau_type = ACS_CS_ImmMapper::MAUB;
						}
						else if (strcmp(line.c_str(),"MAUS") == 0)
						{
							mau_type = ACS_CS_ImmMapper::MAUS;
						}			
					}
				}
				inFile.close();	
			}
		}		
	}	
}
int ACS_CS_Registry::getDefaultMauType ()
{
	if (mau_type == 0)//UNDEFINED
	{
		loadDefaultMauType();
	}

	return mau_type;
}
