/*
 * ACS_CS_NetworkList.h
 *
 *  Created on: Jan 5, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_NETWORKLIST_H_
#define CSADM_CAA_INC_ACS_CS_NETWORKLIST_H_

#include <map>
#include <set>
#include <vector>
#include <string>
#include <stddef.h>



#include "ACS_CS_NetworkDefinitions.h"

class ACS_CS_NetworkName
{
public:
	ACS_CS_NetworkName(const std::string defaultName):
        name(defaultName)
    {

    }

    std::string get() const
    {
        return name;
    }

    void set(const std::string& input_name)
    {
        name = input_name;
    }

    bool operator==(const std::string& value) const
    {
        return ( value == name );
    }

    bool operator!=(const std::string& value) const
	{
    	return !(*this == value);
	}

    ACS_CS_NetworkName& operator=(const std::string& input_name)
    {
        set(input_name);
        return *this;
    }

    operator std::string() const {return get();};

private:
    std::string name;
};

class ACS_CS_Interface
{
public:
	ACS_CS_Interface(const std::string & hostName, const std::string & mac, const std::string & networkName, Network::Domain domain, const std::string & nicName):
		hostName(hostName),
		mac(mac),
		networkName(networkName),
		nicName(nicName),
		domain(domain)
	{

	}

	bool operator==(const ACS_CS_Interface& other) const
	{
		return (mac == other.mac);
	}

	bool operator!=(const ACS_CS_Interface& other) const
	{
		return !(*this == other);
	}

	bool operator<(const ACS_CS_Interface& other) const
	{
		return (mac < other.mac);
	}

	std::string getMac() const
	{
		return mac;
	}

	std::string getHostName() const
	{
		return hostName;
	}

	std::string getNetworkName() const
	{
		return networkName;
	}

	std::string getNicName() const
	{
		return nicName;
	}

	void setNicName(const std::string & name)
	{
		nicName = name;
	}

	Network::Domain getDomain() const
	{
		return domain;
	}

private:
	std::string hostName;
	std::string mac;
	std::string networkName;
	std::string nicName;
	Network::Domain domain;

};

class ACS_CS_Network
{
public:

	ACS_CS_Network(const std::string & name, const std::string (& input_nicName)[Network::DOMAIN_NR]):
		name(name),
		interfaces(),
		nicNames()
	{
		std::copy(input_nicName, input_nicName + Network::DOMAIN_NR, nicNames);
	}

	ACS_CS_Network(const std::string & name, const std::string (& input_nicName)[Network::DOMAIN_NR], const std::set<ACS_CS_Interface> & interfaces):
		name(name),
		interfaces(interfaces),
		nicNames()
	{
		std::copy(input_nicName, input_nicName + Network::DOMAIN_NR, nicNames);
	}

	std::string getName() const { return name; }

	void setName(const std::string & new_name) { name = new_name; }

	std::string getNicName(Network::Domain domain) const { return nicNames[domain]; }

	const std::string * getNicNames() const { return nicNames; }

	const std::set<ACS_CS_Interface>& getInterfaces() const { return interfaces; }

	//Network::Domain getDomain() const { return domain; }

	void addInterfaces(std::set<ACS_CS_Interface> new_interfaces)
	{
		interfaces.insert(new_interfaces.begin(), new_interfaces.end());
	}

	void removeInterface(const ACS_CS_Interface & interface)
	{
		interfaces.erase(interface);
	}

	void addNicNames(const std::string * newNicNames)
	{
		std::copy(newNicNames, newNicNames + Network::DOMAIN_NR, nicNames);
	}


private:
	ACS_CS_NetworkName name;
	std::set<ACS_CS_Interface> interfaces;
	std::string nicNames[Network::DOMAIN_NR];

//	Network::Domain domain;
};


typedef std::map<int, ACS_CS_Network> networkMap_t;



class ACS_CS_NetworkList
{
public:
	ACS_CS_NetworkList();

	int add(int index, const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR]);
	int add(int index, const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR], std::set<ACS_CS_Interface> interfaces);

	int add(const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR]);
	int add(const std::string & name, const std::string (& nicName)[Network::DOMAIN_NR], std::set<ACS_CS_Interface> interfaces);

	bool remove(int index);


	ACS_CS_NetworkName getName(int index) const;

	std::string getNicName(int index, Network::Domain ) const;

	const std::string * getNicNames(int index) const;

	//Network::Domain getDomain(int index) const;

	const std::set<ACS_CS_Interface>& getInterfaces(int index) const;



	bool changeName(int index, const std::string & name);

	void removeInterfacesByHostName(const std::string& hostName);

	void removeInterfacesByHostName(const std::string& hostName, std::set<int>& modifiedNetworkIndexSet);

	bool hasIndex(int index);

	bool hasNicName(const std::string & nicName, Network::Domain);


	size_t size() const;

	int findIndexByName(const std::string & name);

	int findIndexByNicName(const std::string & name, Network::Domain domain);


	typedef networkMap_t::iterator iterator;

	iterator begin() { return items.begin(); }

	iterator end() { return items.end(); }


private:
	networkMap_t items;

	int add(int index, ACS_CS_Network);

	int getFirstAvailableIndex() const;

	//Search element by index or by name if index is not found
	networkMap_t::iterator getElement(int index, const std::string & name);

	static ACS_CS_Network nullNetwork;

};




#endif /* CSADM_CAA_INC_ACS_CS_NETWORKLIST_H_ */
