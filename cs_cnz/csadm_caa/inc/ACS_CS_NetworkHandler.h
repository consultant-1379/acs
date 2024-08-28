/*
 * ACS_CS_NetworkHandler.h
 *
 *  Created on: Jan 5, 2017
 *      Author: estevol
 */

#ifndef CSADM_CAA_INC_ACS_CS_NETWORKHANDLER_H_
#define CSADM_CAA_INC_ACS_CS_NETWORKHANDLER_H_

#include <set>
#include <boost/thread/recursive_mutex.hpp>

#include "ACS_CS_NetworkDefinitions.h"
#include "ACS_CS_NetworkList.h"
#include "ACS_CS_ImmIncomingHandler.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ComputeResourceHandler.h"

class ACS_CS_Interface;
class ACS_CS_ImModel;

typedef struct oiCcbAugmentHandlesStruct{
	ACS_APGCC_OiHandle oiHandle;
		ACS_APGCC_CcbId ccbId;
		ACS_APGCC_AttrValues **attr;
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
		ACS_CS_ImBase *base;


	oiCcbAugmentHandlesStruct();
	void setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
				ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
				ACS_APGCC_CcbHandle &ccbHandleVal,
				ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);
} oiCcbAugmentHandlesStruct;


class ACS_CS_NetworkTransaction: public ACS_CS_ImmIncomingHandler
{
public:
	ACS_CS_NetworkTransaction() {};
	ACS_CS_NetworkTransaction(const ACS_CS_NetworkTransaction& other) {
		this->model = other.model;
		this->ccbAugmentHandles = other.ccbAugmentHandles;
		this->augmentHandlesAvailable = other.augmentHandlesAvailable;
		for (uint32_t i = 0; i < Network::TYPE_NR; ++i)
			this->networks[i] = other.networks[i];
	};

	virtual ~ACS_CS_NetworkTransaction(){};
	void  storeNetworkAndHost(int index, Network::Type);
	void  storeNetwork(int index, Network::Type);
	void storeAllNetworks(Network::Type);
	void deleteHost(const std::string & hostname);
	int create() { return 0;};
	int modify() { return 0;};
	int remove() { return 0;};
	template<class ImNetwork_t> bool detachHostInterfaceFromNetwork(const std::string hostname, int index, Network::Type type);
	ACS_CS_ImModel* model;
	ACS_CS_NetworkList networks[Network::TYPE_NR];
	oiCcbAugmentHandlesStruct ccbAugmentHandles;
	bool augmentHandlesAvailable;
private:

	template<class ImNetwork_t> void saveNetwork(int index, Network::Type, bool createHost = false);

	std::string getNetworkDescription(const std::string& name);

	bool isRestrictedName(const std::string & name);
};



class ACS_CS_NetworkHandler: public ACS_CS_ImmIncomingHandler
{
public:

	static ACS_CS_NetworkHandler* getInstance ();
	virtual ~ACS_CS_NetworkHandler() {};
	static void finalize();

	void startTransaction(std::string, ACS_CS_ImModel*);
	void startDeletionTransaction(oiCcbAugmentHandlesStruct ccbAugmentHandles, ACS_CS_ImModel*);
	void commitTransaction(std::string);
	void commitDeletionTransaction(ACS_APGCC_CcbId ccbId);
	void abortTransaction(std::string);
	void abortDeletionTransaction(ACS_APGCC_CcbId ccbId);

	bool handleComputeResourceDeletion(const ACS_CS_ComputeResourceRole& computeResource, ACS_APGCC_CcbId ccbId);

	bool addHost(const std::set<ACS_CS_Interface> &, const std::string& transactionId);

	bool deleteHost(const std::string & hostName, const std::string& transactionId);

	bool addNetwork(const std::string& name, Network::Domain domain, const std::string& transactionId);

	bool addNetworkAndInterface(const std::string & networkName, const ACS_CS_Interface &, const std::string& transactionId);


	bool changeNetworkName(int index, Network::Type type, const std::string& name);
	int create() { return 0;};
	int modify() { return 0;};
	int remove() { return 0;};

private:
	ACS_CS_NetworkHandler();

	Network::Type getNetworkType(const std::string& name);

	std::string generateNicName(const std::string& name, Network::Domain domain);

	template<class ImNetwork_t> void rebuildNetworkList(Network::Type type);

	static ACS_CS_NetworkHandler* m_instance;

	ACS_CS_NetworkList m_networks[Network::TYPE_NR];

	boost::recursive_mutex m_networkMutex[Network::TYPE_NR];

	int m_nicNameCounter[Network::DOMAIN_NR];

	std::map<std::string, ACS_CS_NetworkTransaction> m_transactions;
	std::map<ACS_APGCC_CcbId, ACS_CS_NetworkTransaction> m_deletion_transactions;

};


#endif /* CSADM_CAA_INC_ACS_CS_NETWORKHANDLER_H_ */
