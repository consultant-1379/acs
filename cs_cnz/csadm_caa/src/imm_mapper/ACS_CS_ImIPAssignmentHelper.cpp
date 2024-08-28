/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImIPAssignmentHelper.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/
#include "ACS_CS_ImIPAssignmentHelper.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_Trace.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImSerialization.h"

ACS_CS_Trace_TDEF(ACS_CS_ImIPAssignmentHelper_TRACE);

ACS_CS_ImIPAssignmentHelper::ACS_CS_ImIPAssignmentHelper()
{
}

ACS_CS_ImIPAssignmentHelper::~ACS_CS_ImIPAssignmentHelper()
{
}

bool ACS_CS_ImIPAssignmentHelper::assignIp(ACS_CS_ImModel *model,
		std::string bladeRdn)
{
	using namespace ACS_CS_Protocol;

	ACS_CS_ImBlade *blade = dynamic_cast<ACS_CS_ImBlade *> (model->getObject(
			bladeRdn.c_str()));

	if (0 == blade)
		return false;

	string ipA,ipAliasA="0.0.0.0";
	string ipB,ipAliasB="0.0.0.0";

	// TEST STATIC IPs ////////////////////////////////////////////////////////////////////////////
	if ((blade->type == CPBLADE_T) || (blade->type == OTHERBLADE_T))
	{

		string shelfDn = ACS_CS_ImUtils::getParentName(blade->rdn);

		ACS_CS_ImShelf
				* shelf = dynamic_cast<ACS_CS_ImShelf*> (model->getObject(
						shelfDn.c_str()));

		ACS_CS_ImHardwareMgmt* hw =
				dynamic_cast<ACS_CS_ImHardwareMgmt*> (model->getObject(
						ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

		if (hw && shelf)
		{
			set < string > table = hw->staticIpMap;

			if(blade->functionalBoardName==SMXB)
			{
				if (getIPfromTable(table, shelf->address, blade->slotNumber,
					blade->ipAddressEthA, blade->ipAddressEthB))
				{
					if(getIPfromTable(table, shelf->address, (blade->slotNumber + ACS_CS_NS::SLOT_OFFSET),
					blade->ipAliasEthA, blade->ipAliasEthB))
					{
						return (!blade->ipAddressEthA.empty()
						&& !blade->ipAddressEthB.empty() && !blade->ipAliasEthA.empty()
						&& !blade->ipAliasEthB.empty());
					}
				}				
			}
			else  if (getIPfromTable(table, shelf->address, blade->slotNumber,
					blade->ipAddressEthA, blade->ipAddressEthB))
			{				
				return (!blade->ipAddressEthA.empty()
						&& !blade->ipAddressEthB.empty());
			}
		}
		else
			return false;
	}
	//END TEST STATIC IPs ////////////////////////////////////////////////////////////////////////////
	if (APBLADE_T == blade->type)
	{
		uint32_t ipaddr = CS_ProtocolChecker::getAPaddress(
				static_cast<ACS_CS_Protocol::CS_Side_Identifier> (blade->side),
				blade->systemNumber, ACS_CS_Protocol::Eth_A);
		char str[INET_ADDRSTRLEN];
		

		ipaddr = ntohl(ipaddr);
		inet_ntop(AF_INET, &ipaddr, str, INET_ADDRSTRLEN);
		blade->ipAddressEthA = str;
		ipaddr = CS_ProtocolChecker::getAPaddress(
				static_cast<ACS_CS_Protocol::CS_Side_Identifier> (blade->side),
				blade->systemNumber, ACS_CS_Protocol::Eth_B);
		ipaddr = ntohl(ipaddr);
		inet_ntop(AF_INET, &ipaddr, str, INET_ADDRSTRLEN);
		blade->ipAddressEthB = str;
	}
	else
	{		
		assignIp(model,  blade->ipAddressEthA, blade->ipAddressEthB);
		
		if(blade->functionalBoardName == SMXB)
		{
			assignIp(model, blade->ipAliasEthA, blade->ipAliasEthB);	
			return (!blade->ipAddressEthA.empty() && !blade->ipAddressEthB.empty()
				&& !blade->ipAliasEthA.empty() && !blade->ipAliasEthB.empty());
		}		
	}	
	return (!blade->ipAddressEthA.empty() && !blade->ipAddressEthB.empty());
}

bool ACS_CS_ImIPAssignmentHelper::assignIp(const ACS_CS_ImModel *model,
		ACS_CS_ImBlade *blade)
{
	bool result = false;

	if (0 == blade)
		return result;

	ACS_CS_ImModel modelCopy(*model);
	ACS_CS_ImBlade * tmp = dynamic_cast<ACS_CS_ImBlade*>(modelCopy.getObject(blade->rdn.c_str()));
	if (tmp)
	{		
		tmp->action=blade->action;
		result = assignIp(&modelCopy, tmp->rdn);
		*blade = *tmp;
	}
	else
	{
		ACS_CS_ImBlade * clone = dynamic_cast<ACS_CS_ImBlade*> (blade->clone());
		modelCopy.addObject(clone);
		result = assignIp(&modelCopy, clone->rdn);
		*blade = *clone;
	}
	return result;
}

bool ACS_CS_ImIPAssignmentHelper::assignIp(ACS_CS_ImModel *model,
		std::string &ethA, std::string &ethB)
{
	using namespace ACS_CS_NS;
	std::set<uint32_t> addressSetEthA;
	std::set<uint32_t> addressSetEthB;

	std::set<const ACS_CS_ImBase *> blades;

	// fetch all blades
	model->getObjects(blades, BLADE_T);
	
	// Check if any were returned<<endl;	
	
	if (blades.size() > 0)
	{
		// Loop through returned blades
		std::set<const ACS_CS_ImBase *>::const_iterator it;
		for (it = blades.begin(); it != blades.end(); it++)
		{
			const ACS_CS_ImBlade *b =
					dynamic_cast<const ACS_CS_ImBlade *> (*it);

			if (NULL == b)
				return false;

			// convert it to an uint32_t
			uint32_t ip = 0;

			// store IP address in ip:
			inet_pton(AF_INET, b->ipAddressEthA.c_str(), &ip);
			if (ip)
			{
				ip = ntohl(ip);
				addressSetEthA.insert(ip);
			}

			ip = 0;
			inet_pton(AF_INET, b->ipAddressEthB.c_str(), &ip);

			if (ip)
			{
				ip = ntohl(ip);
				addressSetEthB.insert(ip);
			}
			if(b->functionalBoardName==SMXB)
			{
				ip = 0;
				inet_pton(AF_INET, b->ipAliasEthA.c_str(), &ip);
				if (ip)
				{
					ip = ntohl(ip);
					addressSetEthA.insert(ip);
				}
				ip=0;
				inet_pton(AF_INET, b->ipAliasEthB.c_str(), &ip);
				if (ip)
				{
					ip = ntohl(ip);
					addressSetEthB.insert(ip);
				}
			}
		}
	}

	//TEST STATIC IPs ///////////////////////////////////////////////////////////////////////
	std::set<std::string>::iterator it;

	ACS_CS_ImHardwareMgmt* hw =
			dynamic_cast<ACS_CS_ImHardwareMgmt*> (model->getObject(
					ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));

	if (!hw)
		return false;

	std::set < std::string > table = hw->staticIpMap;

	for (it = table.begin(); it != table.end(); it++)
	{
		std::string magazine, ipA, ipB, entry;
		
		uint16_t slot = 0;

		entry = *it;
		//split(*it,delim,splitted);

		//if (splitted.size() == 4) {
		if (ACS_CS_ImUtils::fetchDataFromStaticIpEntry(magazine, slot, ipA,
				ipB, entry))
		{
			// convert it to an uint32_t
			uint32_t ip = 0;
			
			inet_pton(AF_INET, ipA.c_str(), &ip);

			if (ip)
			{
				ip = ntohl(ip);
				addressSetEthA.insert(ip);
			}

			ip = 0;

			inet_pton(AF_INET, ipB.c_str(), &ip);

			if (ip)
			{
				ip = ntohl(ip);
				addressSetEthB.insert(ip);
			}			
		}
		else
		{
			cout << "ERROR!!! Wrong Entry in Static IP table" << endl;
		}
	}
	//END TEST STATIC IPs ///////////////////////////////////////////////////////////////////////


	// Try to allocate on EthA
	uint32_t ipEthA = ethALowerBound;

	// Search upwards for free address
	while (addressSetEthA.find(ipEthA) != addressSetEthA.end())
	{
		ipEthA++;
	}

	// Check if is within bounds
	if (ipEthA < ethAUpperBound)
	{
		char str[INET_ADDRSTRLEN];
		ipEthA = htonl(ipEthA);
		inet_ntop(AF_INET, &(ipEthA), str, INET_ADDRSTRLEN);
		ethA = str; // Use this IP
	}

	// Try to allocate on EthB
	uint32_t ipEthB = ethBLowerBound;

	// Search upwards for free address
	while (addressSetEthB.find(ipEthB) != addressSetEthB.end())
	{
		ipEthB++;
	}

	// Check if is within bounds
	if (ipEthB < ethBUpperBound)
	{
		char str[INET_ADDRSTRLEN];
		ipEthB = htonl(ipEthB);
		inet_ntop(AF_INET, &(ipEthB), str, INET_ADDRSTRLEN);
		ethB = str; // Use this IP

	}

	return (!ethA.empty() && !ethB.empty());

}
bool ACS_CS_ImIPAssignmentHelper::getIPfromTable(std::set<std::string> &table,
		std::string shelf, uint16_t slot, std::string &ethA, std::string &ethB)
{
	bool found = false;

	for (std::set<std::string>::iterator it = table.begin(); it != table.end()
			&& !found; it++)
	{
		std::string ipA, ipB, magazine,ipAliasA,ipAliasB;
		uint16_t tmpSlot = 0;
		std::string entry = *it;

		if (ACS_CS_ImUtils::fetchDataFromStaticIpEntry(magazine, tmpSlot, ipA,
				ipB, entry))
		{
			if (shelf == magazine && slot == tmpSlot)
			{
				ethA = ipA;
				ethB = ipB;
				found = true;
			}
		}
	}

	return found;
}
bool ACS_CS_ImIPAssignmentHelper::storeStaticIpToDisk(
		const ACS_CS_ImModel *model)
{
	ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	std::cout << __FUNCTION__ << "@" << __LINE__ << " Save StaticIP File on disk..." << endl;

	ACS_CS_ImModel *tempModel = new ACS_CS_ImModel();
	if (!tempModel)
		return false;

	ACS_CS_ImBase *advBase = model->getObject(
			ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str());
	ACS_CS_ImHardwareMgmt* hw = dynamic_cast<ACS_CS_ImHardwareMgmt*> (advBase);

	if (!advBase || !hw)
	{
		delete tempModel;
		return false;
	}

	ACS_CS_ImHardwareMgmt *advancedConfigurationClonedObj =
			dynamic_cast<ACS_CS_ImHardwareMgmt *> (hw->clone());
	tempModel->addObject(advancedConfigurationClonedObj);

	//create cs directory if doestn't exist
	try
	{
		boost::filesystem::path csFolder(ACS_CS_NS::ACS_DATA_PATH);
		if (!boost::filesystem::exists(csFolder))
		{
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Create directory: " << ACS_CS_NS::ACS_DATA_PATH << endl;
			ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "Create directory: %s", ACS_CS_NS::ACS_DATA_PATH.c_str()));
			// create the folder
			boost::filesystem::create_directory(csFolder);
		}
	} catch (const boost::filesystem::filesystem_error& ex)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Failed to create folder: " << ACS_CS_NS::ACS_DATA_PATH << " Error: " << ex.what() << endl;
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "Failed to create folder: <%s>, error: <%s>", ACS_CS_NS::ACS_DATA_PATH.c_str(), ex.what()));
	}

	//store on data disk
	bool retVal = true;

	try
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Save StaticIP file on data disk: " << ACS_CS_NS::STATIC_IP_FILE << endl;
		std::ofstream ofs(ACS_CS_NS::STATIC_IP_FILE.c_str());
		boost::archive::binary_oarchive ar(ofs);
		ar << *tempModel;
		std::cout << __FUNCTION__ << "@" << __LINE__ << "StaticIP File saved on data disk" << endl;
	}
	catch (boost::archive::archive_exception& e)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << "Serialization error in saving Static IP file: " << e.what() << endl;
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "storeStaticIpToDisk() Serialization exception occurred: %s.", e.what() ));
		retVal = false;
	}
	catch (boost::exception&)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << "Exception occurred in saving Static IP file" << endl;
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "storeStaticIpToDisk() exception occurred." ));
		retVal = false;
	}

	delete tempModel;
	return retVal;
}

bool ACS_CS_ImIPAssignmentHelper::readStaticIpFromDisk(
		std::set<std::string> &table)
{
	bool retVal = true;

	//read from data disk
	ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));
	std::cout << __FUNCTION__ << "@" << __LINE__ << " Read StaticIP File from disk..." << endl;

	ACS_CS_ImModel *tempModel = new ACS_CS_ImModel();
	try
	{
		std::ifstream ofs(ACS_CS_NS::STATIC_IP_FILE.c_str());
		boost::archive::binary_iarchive ar(ofs);

		//check achive version compatibility
		if (ar.get_library_version() < BOOST_ARCHIVE_VERSION)
		{
			ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "readStaticIpFromDisk() Boost archive version NOT SUPPORTED!!!."));

			//remove data file
			std::remove(ACS_CS_NS::STATIC_IP_FILE.c_str());

			retVal = false;
		}
		else
		{
			ar >> *tempModel;
		}

	}
	catch (boost::archive::archive_exception& e)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << "Serialization error in reading Static IP file: " << e.what() << endl;
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "readStaticIpFromDisk() Serialization exception occurred: %s.", e.what() ));
		retVal = false;
	}
	catch (boost::exception&)
	{
		std::cout << __FUNCTION__ << "@" << __LINE__ << "Exception occurred in reading Static IP file" << endl;
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "readStaticIpFromDisk() exception occurred." ));
		retVal = false;
	}
	catch (int e)
	{
		ACS_CS_TRACE((ACS_CS_ImIPAssignmentHelper_TRACE, "readStaticIpFromDisk() Invalid format file exception occurred." ));
		retVal = false;
	}

	if (retVal && tempModel && (tempModel->size() > 0))
	{
		ACS_CS_ImHardwareMgmt* hw =
				dynamic_cast<ACS_CS_ImHardwareMgmt*> (tempModel->getObject(
						ACS_CS_ImmMapper::RDN_HARDWARECATEGORY.c_str()));
		if (hw)
		{
			table = hw->staticIpMap;
		}
	}

	delete tempModel;
	return retVal;
}

