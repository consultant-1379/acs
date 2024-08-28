/*
 * ACS_CS_ComputeResourceHandler.h
 *
 *  Created on: Mar 31, 2015
 *      Author: eanform,estevol
 */

#ifndef ACS_CS_COMPUTERESOURCEHANDLER_H_
#define ACS_CS_COMPUTERESOURCEHANDLER_H_

#include "ACS_CS_TableHandler.h"
#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImmIncomingHandler.h"
#include "ACS_CC_Types.h"

#include <set>
#include <map>

class ACS_CS_PDU;
class ACS_CS_ImBase;

typedef std::pair<const ACS_CS_ImBase *, std::set<const ACS_CS_ImBase *> > ComputeResourceSubtree_t;

class ACS_CS_ComputeResourceNetwork
{
public:

	bool operator==(const ACS_CS_ComputeResourceNetwork& other) const
	{
		return (dn == other.dn);
	}

	bool operator!=(const ACS_CS_ComputeResourceNetwork& other) const
	{
		return !(*this == other);
	}

	bool operator<(const ACS_CS_ComputeResourceNetwork& other) const
	{
		return (dn < other.dn);
	}

	std::string dn;
	std::string nicName;
	std::string mac;
	std::string name;
};

class ACS_CS_ComputeResourceRole
{
public:
	ACS_CS_ComputeResourceRole(): type(UNDEF_TYPE),roleAssignment(ROLE_FREE), roleAssigmentState(ROLE_UNASSIGNED) {};
	ACS_CS_ComputeResourceRole(const ACS_CS_ComputeResourceRole &other): id(other.id),
			uuid(other.uuid),
			type(other.type),
			roleLabel(other.roleLabel),
			roleAssignment(other.roleAssignment),
			macAddressEthA(other.macAddressEthA),
			macAddressEthB(other.macAddressEthB),
			ipAddressEthA(other.ipAddressEthA),
			ipAddressEthB(other.ipAddressEthB),
			networks(other.networks),
			roleAssigmentState(other.roleAssigmentState)
	{};

	ACS_CS_ComputeResourceRole& operator=(const ACS_CS_ComputeResourceRole& other){copy(other); return *this;};

	~ACS_CS_ComputeResourceRole(){};

	inline void copy(const ACS_CS_ComputeResourceRole &other)
	{
		id = other.id;
		uuid = other.uuid;
		type = other.type;
		roleLabel = other.roleLabel;
		roleAssignment = other.roleAssignment;
		macAddressEthA = other.macAddressEthA;
		macAddressEthB =other.macAddressEthB;
		ipAddressEthA = other.ipAddressEthA;
		ipAddressEthB =other.ipAddressEthB;
		networks =other.networks;
		roleAssigmentState = other.roleAssigmentState;
	}

	inline bool operator==(const ACS_CS_ComputeResourceRole& rhs) const
			{ return ((id == rhs.id) &&
					(uuid == rhs.uuid) &&
					(type == rhs.type) &&
					(roleLabel.compare(rhs.roleLabel) == 0) &&
					(roleAssignment == rhs.roleAssignment) &&
					(macAddressEthA == rhs.macAddressEthA) && (macAddressEthB == rhs.macAddressEthB) &&
					(ipAddressEthA == rhs.ipAddressEthA) && (ipAddressEthB == rhs.ipAddressEthB)) && 
					(networks == rhs.networks); }

	inline bool operator!=(const ACS_CS_ComputeResourceRole& rhs) const {return !(*this == rhs);}

	inline bool hasValidRole() const {return roleAssignment > ROLE_FREE;}

	inline bool hasRoleFree() const {return (ROLE_FREE == roleAssignment);}

	inline bool hasRoleRevoked() const {return (ROLE_REVOKED == roleAssignment);}

	inline bool hasRoleLabel() const {return !roleLabel.empty();}

	inline bool isAssigned() const { return ROLE_ASSIGNED == roleAssigmentState; }

	inline ACS_CS_ImBase* getImObject() const
	{
		ACS_CS_ImComputeResource* computeResourceObject = new ACS_CS_ImComputeResource();

		computeResourceObject->computeResourceId = id;
		computeResourceObject->uuid = uuid;
		computeResourceObject->crRoleId = roleAssignment;
		computeResourceObject->crRoleLabel = roleLabel;
		computeResourceObject->crType = type;
		computeResourceObject->macAddressEthA = macAddressEthA;
		computeResourceObject->macAddressEthB = macAddressEthB;
		computeResourceObject->ipAddressEthA = ipAddressEthA;
		computeResourceObject->ipAddressEthB = ipAddressEthB;

		for (std::set<ACS_CS_ComputeResourceNetwork>::iterator it = networks.begin(); networks.end() != it; ++it)
		{
			computeResourceObject->networks.insert(it->dn);
		}

		computeResourceObject->rdn = ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_ID + "=" + id + "," + ACS_CS_ImmMapper::RDN_CRMGMT;

		return computeResourceObject;
	}

	std::string id;
	std::string uuid;
	RoleType type;
	std::string roleLabel;
	int roleAssignment;
	std::string macAddressEthA;
	std::string macAddressEthB;
	std::string ipAddressEthA;
	std::string ipAddressEthB;
	set<ACS_CS_ComputeResourceNetwork> networks;

	RoleAssignmentState roleAssigmentState;



	static ACS_CS_ComputeResourceRole nullObject;
};

typedef std::map<std::string, ACS_CS_ComputeResourceRole> ComputeResourceMap;
typedef std::pair<std::string, ACS_CS_ComputeResourceRole> ComputeResourcePair;


class ACS_CS_ComputeResourceHandler: public ACS_CS_TableHandler
{
public:
	ACS_CS_ComputeResourceHandler();

	virtual ~ACS_CS_ComputeResourceHandler();

	virtual int handleRequest (ACS_CS_PDU *pdu);

	virtual int newTableOperationRequest(ACS_CS_ImModelSubset *subset);

	bool handleComputeResourceDeletion(ACS_CS_ImModelSubset *subset);

	void applyNetworkHandlerDeletionTransaction(ACS_APGCC_CcbId ccbId);
	void abortNetworkHandlerDeletionTransaction(ACS_APGCC_CcbId ccbId);

	virtual bool loadTable ();

	virtual void storeComputeResourcesOnDisk ();

	virtual void restoreComputeResourcesFromDisk ();
	void setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr, ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	void clearAugmentCcbHandles();

private:

	std::set<const ACS_CS_ImBase *> getComputeResourceObjects(const ACS_CS_ImModel* model);

	std::set<ComputeResourceSubtree_t> getComputeResourceSubtrees(const ACS_CS_ImModel* subset, const ACS_CS_ImModel* wholeModel = 0);

	ACS_CS_ComputeResourceRole getComputeResourceRole(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects = std::set<const ACS_CS_ImBase*>());

	std::string getComputeResourceKey(const ACS_CS_ImBase* computeResourceBaseObject);

	void insertComputeResource(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects);

	void removeComputeResource(const ACS_CS_ImBase* computeResourceBaseObject);

	void modifyComputeResource(const ACS_CS_ImBase* computeResourceBaseObject, std::set <const ACS_CS_ImBase *> networkObjects);

	bool updateTable(std::set<ComputeResourceSubtree_t> computeResourceSubtrees);

	void handleComputeResourceTableSubscription (std::set<ComputeResourceSubtree_t> computeResourceObjects);

	void updateRoleAssignment();

	bool releaseRoleAssignmentAugment();

	void reloadTableAtStartup();

	ACS_CS_ImModel * getComputeResourcesModel(const ACS_CS_ImModel* model);

	// List of currently defined Compute Resource objects
	ComputeResourceMap computeResourceMap;

	// List of Compute Resource objects whose role is going to be released.
	// Temporary list, only used during update operations.
	ComputeResourceMap removeComputeResourceMap;

	// List of Compute Resource objects whose role is going to be released.
	// Temporary list, only used during update operations.
	ComputeResourceMap modifyComputeResourceMap;

	mutable ACE_Recursive_Thread_Mutex computeResourceMapMutex;

	// Attributes to use in MethodHandler triggered operations, mainly to augment IMM operations
	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

	bool augmentHandlesAvailable;

};


class ACS_CS_CrMgmtHandler: public ACS_CS_ImmIncomingHandler
{
public:

	ACS_CS_CrMgmtHandler(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr, ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal,
			ACS_CS_ComputeResourceHandler* p_computeResourceHandler);

	virtual ~ACS_CS_CrMgmtHandler();

	virtual int create();
	virtual int remove();
	virtual int modify();

private:

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;
	ACS_CS_ComputeResourceHandler* computeResourceHandler;
};


#endif /* ACS_CS_COMPUTERESOURCEHANDLER_H_ */
