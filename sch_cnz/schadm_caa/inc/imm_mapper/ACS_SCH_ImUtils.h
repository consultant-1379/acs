#ifndef ACS_SCH_IMUTILS_H_
#define ACS_SCH_IMUTILS_H_

#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"

//CRR and Role class enumerations
#define LOCKED 0
#define UNLOCKED 1
#define SHUTTINGDOWN 2
#define OPER_ENABLED 1
#define OPER_DISABLED 0
#define UNINSTANTIATED 1
#define INSTANTIATING 2
#define INSTANTIATION_FAILED 3
#define UNINSTANTIATION_FAILED 4
#define INSTANTIATED 5
#define UNINSTANTIATING 6
#define SCALABLE 1
#define NON_SCALABLE 2
#define ACTIVATE_FAILED -1

//CP state enumerations
#define NONOPSTATE    1
#define PASSIVESTATE  2
#define ACTIVESTATE   3

namespace ACS_SCH_ImmMapper 
{
static const string classModel ="AxeComputeResourceComputeResource";
static const string clustercpModel ="AxeEquipmentClusterCp";
//CRR CLASS IMPLEMENTER
static const string CRRClassName ="AxeScalingMComputeResourceRole";
static const string CRRImplName("SCH_CRR_Impl");
//Role CLASS IMPLEMENTER
static const string RoleClassName = "AxeScalingMRole";
static const string RoleImplName("SCH_Role_Impl");
//CrM CLASS IMPLEMENTER
static const string CrmClassName = "AxeScalingMCrM";
static const string CrmImplName("SCH_Crm_Impl");
static const string crmDn="CrM=1,SysM=1,SystemFunctions=1,ManagedElement=";
static const string bcroleDn="Role=BC_Role,CrM=1,SysM=1,SystemFunctions=1,ManagedElement=";
static const string equipmentDn="Equipment=1,ManagedElement=";
static const string IMPLEMENTER_Model = "@ACS_SCH_CrmObserver";
static const string CLUSTERCP_OBSERVER = "@ACS_SCH_ClusterCPObserver";
static const string IMPLEMENTER_CRR="ACS_SCH_CRRHandler";
static const string CRRParentDn="AxeScalingMcrMId=1";
static const string clusterDn="cpClusterId=1,logicalMgmtId=1,AxeEquipmentequipmentMId=1";
static const string bcRoleDn="roleId=BC_Role,AxeScalingMcrMId=1";

//CrM class attributes
static const string crmIdAttr = "AxeScalingMcrMId";
static const string roleAssignmentAttr = "autoRoleAssignment";

//ComputeResource attribute list
static const string computeResourceIdAttr = "computeResourceId";

static const string networkManagedElementIdAttr = "networkManagedElementId";
static const string ME = "ManagedElement";

//CRR arrtibute list
static const string crrIdAttr = "computeResourceRoleId";
static const string adminStateAttr = "adminState";
static const string instantiateStateAttr = "instantiationState";
static const string operStateAttr = "operationalState";
static const string usesAttr = "uses";
static const string providesAttr = "provides";

//Role class attribute list
static const string roleIdAttr = "roleId";
static const string scalabilityAttr = "scalability";
static const string isProvidedByAttr = "isProvidedBy";

//CrM class attributes
static const string autoRoleAssignmentAttr = "autoRoleAssignment";

//cpCluster class attributes
static const string clusterCpIdAttr = "clusterCpId";
static const string cpStateAttr = "cpState";
static const string cpTypeAttr = "cpType";
static const string apzSystemAttr = "apzSystem";

//values required for model creation
static const string CRREqual = "ComputeResourceRole=";
static const string commaCrMRoot = ",CrM=1,SysM=1,SystemFunctions=1,ManagedElement=";
static const string RoleEqual = "Role=";
static const string CREqual = "ComputeResource=";
static const string commaEquipment = ",Equipment=1,ManagedElement=";
static const string ROLE ="_Role";

//Roles
static const string APROLE = "AP_Role";
static const string CPROLE = "CP_Role";
static const string BCROLE = "BC_Role";
static const string IPLBROLE = "IPLB_Role";
}

namespace imm_error_code
{
const int ERR_IMM_TRY_AGAIN = -6;
const int ERR_IMM_BAD_HANDLE = -9;
const int ERR_IMM_ALREADY_EXIST = -14;
const int ERR_IMM_NOT_FOUND = -41;
};


class ACS_SCH_ImUtils {
public:
	ACS_SCH_ImUtils();
	virtual ~ACS_SCH_ImUtils();
	static string getClassName(const ACS_APGCC_ImmObject &object);
	static string getIdAttributeFromClassName (const string & className);
	static map <string,string>* getClassMap();
	static map<string, string> ClassMap;
	static string faultyValue;

};
#endif /* ACS_SCH_IMUTILS_H_ */
