/*
 * ACS_CS_RoleAssignment.h
 *
 *  Created on: Mar 31, 2015
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_ACS_CS_ROLEASSIGNMENT_H_
#define CSADM_CAA_INC_ACS_CS_ROLEASSIGNMENT_H_

#ifndef CUTE_TEST
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ComputeResourceHandler.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_NetworkDefinitions.h"

#else
#include "cute_header_stub.h"
#endif



#define LOWER_AP_ROLE 20011 //AP1A
#define UPPER_AP_ROLE 20012 //AP1B

#define LOWER_CP_ROLE 10011 //CP1A
#define UPPER_CP_ROLE 10022 //CP2B

#define LOWER_BC_ROLE 0 	//BC0
#define UPPER_BC_ROLE 630 	//BC63

#define LOWER_IPLB_ROLE 30011 	//IPLB1A
#define UPPER_IPLB_ROLE 30012 	//IPLB1B

#define STATIC_ROLES_IP_OFFSET 56
#define STATIC_ROLES_SIZE 2
#define ROLE_CP1_A 10011
#define ROLE_CP1_B 10012


struct RoleTypeProperty
{
    RoleTypeProperty(): lowerThreshold(ROLE_FREE), upperThreshold(ROLE_FREE){};
    RoleTypeProperty(std::string label, int min, int max): typeLabel(label), lowerThreshold(min), upperThreshold(max){};

    std::string typeLabel;
    int lowerThreshold;
    int upperThreshold;
};

static const RoleTypeProperty BcProperties("BC",LOWER_BC_ROLE,UPPER_BC_ROLE);
static const RoleTypeProperty CpProperties("CP",LOWER_CP_ROLE,UPPER_CP_ROLE);
static const RoleTypeProperty ApProperties("AP",LOWER_AP_ROLE,UPPER_AP_ROLE);
static const RoleTypeProperty IplbProperties("IPLB",LOWER_IPLB_ROLE,UPPER_IPLB_ROLE);

static const RoleTypeProperty PropertyArray[4] = {BcProperties, CpProperties, ApProperties, IplbProperties};

static const int StaticRoles[STATIC_ROLES_SIZE] = {ROLE_CP1_A, ROLE_CP1_B};



class ACS_CS_RoleAssignment: public ACS_CS_ImmIncomingHandler
{

public:

	static ACS_CS_RoleAssignment* createRoleAssignment(RoleType type);
	static ACS_CS_RoleAssignment* createRoleAssignment(RoleType type, int roleAssignmentValue);

	virtual ~ACS_CS_RoleAssignment(){};

	int getAssignment(){return assignment;}
	void setAssignment(int newAssignment) {if (isValid(newAssignment)) assignment = newAssignment;};
	std::string getAssignmentLabel();

	int increase(); //increase

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject) = 0;

	virtual ACS_CS_ImBase* getShelf();

	virtual ACS_CS_ImBase* getCrmEquipment();

	virtual ACS_CS_ImBase* getComputeResource(const ACS_CS_ComputeResourceRole&);

	virtual ACS_CS_ImBase* getCrmComputeResource(const ACS_CS_ComputeResourceRole&);

	virtual Network::Domain getNetworkDomain() = 0;

	bool assignRole(const ACS_CS_ComputeResourceRole& computeResource);

	bool releaseRole(const ACS_CS_ComputeResourceRole& computeResource);
	bool releaseRoleAugment(const ACS_CS_ComputeResourceRole& computeResource);

	int create() { return 0;};
	int modify() { return 0;};
	int remove() { return 0;};
	bool setRevokedRole(const ACS_CS_ComputeResourceRole& computeResource);

	bool setMaintenanceRole(const ACS_CS_ComputeResourceRole& computeResource);

	void setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	ACS_CS_RoleAssignment& operator++()
	{
		increase();
		return *this;
	};

	bool operator==(const ACS_CS_RoleAssignment& other) {return this->assignment == other.assignment;}

protected:

	ACS_CS_RoleAssignment(RoleType inputType);

	virtual void nextRole() = 0;

	int getSide(){ return assignment - (int(assignment/10) * 10) - 1; };

	int getMagicNo(){ return (assignment/10)-roleType; };

	int getSlot() {return ((getMagicNo()) % 24) + getSide() + 2;}

	std::string getShelfAddress();

	bool isValid (int roleAssignment);

	RoleType roleType;

	int assignment;

private:

    RoleTypeProperty properties;

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

	bool augmentHandlesAvailable;

};


class ACS_CS_RoleTableUpdate
{
public:
	ACS_CS_RoleTableUpdate(ComputeResourceMap* validComputeResourceMap,
			ComputeResourceMap* toRemoveComputeResourceMap,
			ComputeResourceMap* toModifyComputeResourceMap):
				computeResourceMap(validComputeResourceMap),
				removeComputeResourceMap(toRemoveComputeResourceMap),
				modifyComputeResourceMap(toModifyComputeResourceMap)
				{

				};
	~ACS_CS_RoleTableUpdate() {};

	void setAugmentCcbHandles(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId,
			ACS_APGCC_AttrValues **attr,ACS_CS_ImBase *base,
			ACS_APGCC_CcbHandle &ccbHandleVal,
			ACS_APGCC_AdminOwnerHandle &adminOwnwrHandlerVal);

	void updateRoles();

	bool removeRolesAugment();

private:

	ACS_CS_RoleAssignment* getRoleAssignment(const ACS_CS_ComputeResourceRole& computeResource);

	ACS_CS_RoleAssignment* getStaticRoleAssignment(const ACS_CS_ComputeResourceRole& computeResource);

	ACS_CS_RoleAssignment* getAvailableRoleAssignment(RoleType type);

	ComputeResourceMap getComputeResourcePerType(RoleType roleTypeFilter);

	ComputeResourceMap getOtherComputeResourceOfSameType(const ACS_CS_ComputeResourceRole& computeResource);

	int getMaxRoleId(RoleType type);

	bool isRoleAvailable(ACS_CS_RoleAssignment* roleAssignment, ComputeResourceMap computeResourcePerType);

	ComputeResourceMap* computeResourceMap;

	ComputeResourceMap* removeComputeResourceMap;

	ComputeResourceMap* modifyComputeResourceMap;

	ACS_APGCC_OiHandle oiHandle;
	ACS_APGCC_CcbId ccbId;
	ACS_APGCC_AttrValues **attr;
	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CS_ImBase *base;

	bool augmentHandlesAvailable;

};

class ACS_CS_CpRoleAssignment: public ACS_CS_RoleAssignment
{
public:
	ACS_CS_CpRoleAssignment(): ACS_CS_RoleAssignment(CP_TYPE){};
	virtual ~ACS_CS_CpRoleAssignment(){};

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject);

	virtual Network::Domain getNetworkDomain() { return Network::CP_DOMAIN; };

private:

	virtual void nextRole();

};

class ACS_CS_BcRoleAssignment: public ACS_CS_RoleAssignment
{
public:
	ACS_CS_BcRoleAssignment(): ACS_CS_RoleAssignment(BC_TYPE){};
	virtual ~ACS_CS_BcRoleAssignment(){};

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject);

	virtual Network::Domain getNetworkDomain() { return Network::BC_DOMAIN; };

private:

	virtual void nextRole();

};

class ACS_CS_ApRoleAssignment: public ACS_CS_RoleAssignment
{
public:
	ACS_CS_ApRoleAssignment(): ACS_CS_RoleAssignment(AP_TYPE){};
	virtual ~ACS_CS_ApRoleAssignment(){};

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject);

	virtual Network::Domain getNetworkDomain() { return Network::AP_DOMAIN; };

private:

	virtual void nextRole();

};

class ACS_CS_IplbRoleAssignment: public ACS_CS_RoleAssignment
{
public:
	ACS_CS_IplbRoleAssignment(): ACS_CS_RoleAssignment(IPLB_TYPE){};
	virtual ~ACS_CS_IplbRoleAssignment(){};

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject);

	virtual Network::Domain getNetworkDomain() { return Network::IPLB_DOMAIN; };

private:

	virtual void nextRole();

};

class ACS_CS_NullRoleAssignment: public ACS_CS_RoleAssignment
{
public:
	ACS_CS_NullRoleAssignment(): ACS_CS_RoleAssignment(UNDEF_TYPE){};
	virtual ~ACS_CS_NullRoleAssignment(){};

	virtual ACS_CS_ImBase* getBlade(const ACS_CS_ComputeResourceRole computeResourceRole = ACS_CS_ComputeResourceRole::nullObject);

	virtual Network::Domain getNetworkDomain() { return Network::DOMAIN_NR; };

private:

	virtual void nextRole();
};

#endif /* CSADM_CAA_INC_ACS_CS_ROLEASSIGNMENT_H_ */
