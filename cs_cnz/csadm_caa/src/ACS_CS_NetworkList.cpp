/*
 * ACS_CS_NetworkList.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: estevol
 */

#include "ACS_CS_NetworkList.h"
#include "ACS_CS_Trace.h"
#include <set>

static const std::string EMPTY_NIC_NAMES[Network::DOMAIN_NR];
ACS_CS_Network ACS_CS_NetworkList::nullNetwork("",EMPTY_NIC_NAMES);

ACS_CS_NetworkList::ACS_CS_NetworkList()
{

}

int ACS_CS_NetworkList::add(int index, const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR])
{
	return add(index, ACS_CS_Network(name, nicName));
}

int ACS_CS_NetworkList::add(int index, const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR], std::set<ACS_CS_Interface> interfaces)
{
	return add(index, ACS_CS_Network(name, nicName, interfaces));
}

int ACS_CS_NetworkList::add(const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR])
{
	return add(getFirstAvailableIndex(), name, nicName);
}

int ACS_CS_NetworkList::add(const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR], std::set<ACS_CS_Interface> interfaces)
{
	return add(getFirstAvailableIndex(), name, nicName, interfaces);
}

int ACS_CS_NetworkList::add(int index, ACS_CS_Network network)
{
	if (network.getName().empty())
	{
		return Network::NO_INDEX;
	}

	if (Network::NO_INDEX == index)
	{
		index = getFirstAvailableIndex();
	}

	networkMap_t::iterator it = getElement(index, network.getName());

	if (items.end() != it)
	{
		it->second.addInterfaces(network.getInterfaces());
		it->second.addNicNames(network.getNicNames());
		return it->first;
	}

	items.insert(std::pair<int, ACS_CS_Network>(index, network));
	return index;
}

bool ACS_CS_NetworkList::remove(int index)
{
	networkMap_t::iterator it = items.find(index);
	if (it != items.end())
	{
		items.erase(it);
		return true;
	}

	return false;
}

ACS_CS_NetworkName ACS_CS_NetworkList::getName(int index) const
{
	networkMap_t::const_iterator it = items.find(index);
	if (it != items.end())
	{
		return it->second.getName();
	}
	return nullNetwork.getName();
}

std::string ACS_CS_NetworkList::getNicName(int index, Network::Domain domain) const
{
	networkMap_t::const_iterator it = items.find(index);
	if (it != items.end())
	{
		return it->second.getNicName(domain);
	}
	return nullNetwork.getNicName(domain);
}

const std::string * ACS_CS_NetworkList::getNicNames(int index) const
{
	networkMap_t::const_iterator it = items.find(index);
	if (it != items.end())
	{
		return it->second.getNicNames();
	}
	return nullNetwork.getNicNames();
}

//Network::Domain ACS_CS_NetworkList::getDomain(int index) const
//{
//	networkMap_t::const_iterator it = items.find(index);
//	if (it != items.end())
//	{
//		return it->second.getDomain();
//	}
//	return nullNetwork.getDomain();
//}


const std::set<ACS_CS_Interface>& ACS_CS_NetworkList::getInterfaces(int index) const
{
	networkMap_t::const_iterator it = items.find(index);
	if (it != items.end())
	{
		return it->second.getInterfaces();
	}

	return nullNetwork.getInterfaces();
}

bool ACS_CS_NetworkList::changeName(int index, const std::string & name)
{
	if (!name.empty())
	{
		networkMap_t::iterator it = items.find(index);
		if (it != items.end())
		{
			it->second.setName(name);
			return true;
		}
	}
	return false;
}

bool ACS_CS_NetworkList::hasIndex(int index)
{
	return items.end() != items.find(index);
}

bool ACS_CS_NetworkList::hasNicName(const std::string & nicName, Network::Domain domain)
{
	bool found = false;
	for (networkMap_t::iterator it = items.begin(); items.end() != it && !found; ++it)
	{
		found = nicName.compare(it->second.getNicName(domain)) == 0;
	}

	return found;
}

size_t ACS_CS_NetworkList::size() const
{
	return items.size();
}

int ACS_CS_NetworkList::findIndexByName(const std::string & name)
{
	if (name.empty())
	{
		return Network::NO_INDEX;
	}

	for (networkMap_t::iterator it = items.begin(); it != items.end(); ++it)
	{
		if (it->second.getName() == name)
		{
			return it->first;
		}
	}

	return Network::NO_INDEX;
}

int ACS_CS_NetworkList::findIndexByNicName(const std::string & nicName, Network::Domain domain)
{
	if (nicName.empty())
	{
		return Network::NO_INDEX;
	}

	for (networkMap_t::iterator it = items.begin(); it != items.end(); ++it)
	{
		if (it->second.getNicName(domain) == nicName)
		{
			return it->first;
		}
	}

	return Network::NO_INDEX;
}

void ACS_CS_NetworkList::removeInterfacesByHostName(const std::string& hostName)
{
	for (networkMap_t::iterator net_it = items.begin(); net_it != items.end(); ++net_it)
	{
		const std::set<ACS_CS_Interface> & interfaces = net_it->second.getInterfaces();

		std::set<ACS_CS_Interface>::iterator if_it = interfaces.begin();

		while (interfaces.end() != if_it)
		{
			std::set<ACS_CS_Interface>::iterator current = if_it++;

			if (hostName.compare(current->getHostName()) == 0)
			{
				net_it->second.removeInterface(*current);
			}
		}

	}
}

void ACS_CS_NetworkList::removeInterfacesByHostName(const std::string& hostName, std::set<int>& modifiedNetworkIndexSet)
{
	for (networkMap_t::iterator net_it = items.begin(); net_it != items.end(); ++net_it)
	{
		const std::set<ACS_CS_Interface> & interfaces = net_it->second.getInterfaces();

		std::set<ACS_CS_Interface>::iterator if_it = interfaces.begin();

		while (interfaces.end() != if_it)
		{
			std::set<ACS_CS_Interface>::iterator current = if_it++;

			if (hostName.compare(current->getHostName()) == 0)
			{
				net_it->second.removeInterface(*current);
				modifiedNetworkIndexSet.insert(net_it->first);
			}
		}

	}
}

networkMap_t::iterator ACS_CS_NetworkList::getElement(int index, const std::string & name)
{
	networkMap_t::iterator it = items.find(index);

	return (items.end() != it)? it: items.find(findIndexByName(name));

}

int ACS_CS_NetworkList::getFirstAvailableIndex() const
{
	int index = Network::NO_INDEX;
	for ( ++index ; items.find(index) != items.end(); ++index)
	{
		//Do nothing
	}

	return index;
}
