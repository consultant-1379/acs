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
 * @file ACS_CS_ImUtils.h
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
 * 2011-09-01  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#ifndef ACS_CS_IMUTILS_H_
#define ACS_CS_IMUTILS_H_

#ifndef CUTE_TEST
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"


#include "ACS_CS_ImVlan.h"
#include "ACS_CS_ImVlanCategory.h"
#include "ACS_CS_ImShelf.h"
#include "ACS_CS_ImOtherBlade.h"
#include "ACS_CS_ImHardwareMgmt.h"

#include "ACS_CS_ImCpBlade.h"
#include "ACS_CS_ImClusterCp.h"
#include "ACS_CS_ImDualSidedCp.h"
#include "ACS_CS_ImEquipment.h"
#include "ACS_CS_ImCpCluster.h"
#include "ACS_CS_ImCpClusterStruct.h"
#include "ACS_CS_ImApServiceCategory.h"
#include "ACS_CS_ImApService.h"

#include "ACS_CS_ImApBlade.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImAp.h"
#include "ACS_CS_ImAdvancedConfiguration.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImValuesDefinitionType.h"

#include "ACS_CS_ImOmProfileManager.h"
#include "ACS_CS_ImOmProfile.h"
#include "ACS_CS_ImCcFile.h"
#include "ACS_CS_ImCcFileManager.h"
#include "ACS_CS_ImCandidateCcFile.h"
#include "ACS_CS_ImOmProfileStruct.h"
#include "ACS_CS_ImCandidateCcFileStruct.h"

#include "ACS_CS_ImCpProductInfo.h"
#include "ACS_CS_ImCrMgmt.h"
#include "ACS_CS_ImComputeResource.h"
#include "ACS_CS_ImIplbCluster.h"

#include "ACS_CS_ImCrmEquipment.h"
#include "ACS_CS_ImCrmComputeResource.h"
#endif

typedef enum {
	// Corresponding text string must be inserted in ACS_CS_ImUtils::getErrorText().
	TC_NOERROR = 0,
	TC_INTERNALERROR = 1,
	TC_VALIDATED = 100, // Validation ok
	TC_COMPLETIONFAULT = 101,
	TC_ENTRYIDNOTSET = 121,
	TC_APZLESSTHAN2 = 122,
	TC_SYSTEMNOTOBIG = 123,
	TC_IPALIASETHAFAULTY = 124,
	TC_IPALIASETHBFAULTY = 125,
	TC_ALIASNETMASKETHAFAULTY = 126,
	TC_ALIASNETMASKETHBFAULTY = 127,
	TC_APREFFAULTY = 128,
	TC_APREFFAULTYINTERNAL = 129,
	TC_CPREFFAULTY = 130,
	TC_CPREFFAULTYINTERNAL = 131,
	TC_CPNAMEFAULTY = 132,
	TC_CPBCREFFAULTY = 133,
	TC_CPCPREFFAULTY = 134,
	TC_CPSEQNOFAULTY = 135,
	TC_CPSYSNOFAULTY = 136,
	TC_CPSYSTYPEFAULTY = 137,
	TC_CPNOCPCAT = 138,
	TC_CPTOMANYINSCP = 139,
	TC_CPSYSIDFAULTY = 140,
	TC_INCORRECTSHELF = 141,
	TC_SCXBSLOTFAULTY = 142,
	TC_SCXBDHCPFAULTY = 143,
	TC_SCXBNOPRODNO = 144,
	TC_EPB1SLOTFAULTY = 145,
	TC_SLOTOCCUPIED = 146,
	TC_NOCONFIGURATIONOBJECTFOUND = 147,
	TC_CPTYPENOTZERO = 148,
	TC_INCORRECTCPALIAS = 149,
	TC_APZSYSTEMNOTZERO = 150,
	TC_SENDTOMASTERFAILED = 151,
	TC_APNAMEFAULTY = 152,
	TC_SLOTOUTOFRANGE = 153,
	TC_SIDENOTSPECIFIED = 154,
	TC_FAULTYSYSTEMTYPE = 155,
	TC_FORBIDDENFBN = 156,
	TC_INCORRECTMAG = 157,
	TC_INVALID_FBN = 158,
	TC_SHELFNOTEMPTY = 159,
	TC_NODEFAULTSOFTWARE = 160,
	TC_APUBALREADYPRESENT = 161,
	TC_CMXBSLOTFAULTY = 162,
	TC_EVOETSLOTFAULTY = 163,
	TC_MISSINGSCXINAPZMAGAZINE = 164,
	TC_DISKALREADYPRESENT = 165,
	TC_IPLBSLOTFAULTY = 166,
	TC_IPLBALREADYPRESENT = 167,
	TC_IPTBSLOTFAULTY = 168,
	TC_CPALIASFAULTY = 169,
	TC_CPALIASINUSE = 170,
	TC_INVALIDAPZSYSTEM = 171,
	TC_INVALIDCPTYPE = 172,
	TC_NOTALLOWED_AP2 = 173,
	TC_CPUBALREADYPRESENT = 174,
	TC_MAUBALREADYPRESENT = 175,
	TC_INVALIDMAUTYPE     = 176,
	TC_MAUSFAULTY    = 177,
	TC_NOTALLOWED    = 178,
	TC_INVALIDIP	= 179,
	TC_INVALIDMAC	= 180,
	TC_ATTRALREADYUSED = 181,
	TC_INVALIDUUID = 182,
	TC_SMXBSLOTFAULTY = 183,
	TC_SMXBDHCPFAULTY = 184,
	TC_SMXBNOPRODNO = 185,
	TC_RESTRICTEDOP = 186,
        TC_INVALIDCRROLEID =187

} ACS_CS_TextCode;
//
//
//OP_CHANGE_CLUSTER_OP_MODE = 1,
//	OP_CHANGE_OM_PROFILE = 3,
//	OP_REMOVE_OM_PROFILE = 4,
//	OP_ACTIVATE_CC_FILE = 5,
//	OP_IMPORT_CC_FILE = 6,
//	OP_EXPORT_CC_FILE = 7,
//	OP_ROLLBACK_CC_FILE = 8,
//	OP_HIDDEN_PROFILE_NOTIFICATION_SUCCESS = 9

namespace imm_error_code
{
	const int ERR_IMM_TRY_AGAIN = -6;
	const int ERR_IMM_BAD_HANDLE = -9;
	const int ERR_IMM_NOT_EXIST = -12;
	const int ERR_IMM_ALREADY_EXIST = -14;
	const int ERR_IMM_NOT_FOUND = -41;
};

namespace ACS_CS_ImmMapper {

	static const int MODEL_VERSION = 0x00000600;	//AxeEquipmentM model version

	typedef enum CpClusterOperationTypeEnum
	{
		CP_CLUSTER_OP_NO_OPERATION = 0,
		CP_CLUSTER_OP_CHANGE_CLUSTER_OP_MODE_ID = 1,
		CP_CLUSTER_OP_CHANGE_OM_PROFILE_ID = 3,
		CP_CLUSTER_OP_REMOVE_OM_PROFILE_ID = 4,
		CP_CLUSTER_OP_ACTIVE_OM_PROFILE_ID = 5,
		CP_CLUSTER_OP_IMPORT_CCF_ID = 6,
		CP_CLUSTER_OP_EXPORT_CCF_ID = 7,
		CP_CLUSTER_OP_ROLLBACK_FILE_ID = 8,

	} Cp_Cluster_Operation_id;

	typedef enum AdvancedConfigurationOperationTypeEnum
	{
		ADVANCED_CONFIGURATION_OP_NO_OPERATION = CP_CLUSTER_OP_NO_OPERATION,
		ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_SUCCESS = 1,
		ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_FAILURE = 2,
		ADVANCED_CONFIGURATION_OP_OM_PROFILE_APA_NOTIFICATION	  = 3

	} Advanded_Configuration_Operation_Id;

	typedef enum OperationDefaultValueEnum
	{
		DEFAULT_OP_CHANGE_CLUSTER_OP_MODE_REQUESTED = -1,
		DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED = -1,
		DEFAULT_OP_OM_PROFILE_NOTIFICATION_PHASE = -1,
		DEFAULT_OP_OM_PROFILE_NOTIFICATION_REASON = -1

	} Operation_default_value;

	typedef enum SystemServiceOperation
	{
		SERVICE_IPTABLE_RESTART = 100

	} system_service_operation;

	typedef enum MauType
	{
	UNDEFINED = 0,
	MAUB = 1,
	MAUS = 2
	}mautype_values;


	//CS Model NameSpace Prefix
	static const string NAMESPACE_CS			= "AxeEquipment";

	//CCH Model NameSpace Prefix
	static const string NAMESPACE_CCH			= "BladeSwManagement";

	//FD Model NameSpace Prefix
	static const std::string NAMESPACE_FD		= "AxeFunctionDistribution";

	//CRM Model NameSpace Prefix
	static const std::string NAMESPACE_CRM		= "AxeComputeResource";

	//TransportM Model NameSpace Prefix
	static const std::string NAMESPACE_TRM		= "AxeTransportM";

	//CS Model Class Names
	static const string CLASS_EQUIPMENT				= NAMESPACE_CS + "EquipmentM";
	static const string CLASS_HARDWARE				= NAMESPACE_CS + "HardwareMgmt";
	static const string CLASS_SHELF					= NAMESPACE_CS + "Shelf";
	static const string CLASS_ADVANCED_CONF			= NAMESPACE_CS + "AdvancedConfiguration";
	static const string CLASS_LOGICAL				= NAMESPACE_CS + "LogicalMgmt";
	static const string CLASS_APG					= NAMESPACE_CS + "Apg";
	static const string CLASS_CLUSTER_CP			= NAMESPACE_CS + "ClusterCp";
	static const string CLASS_CP_CLUSTER			= NAMESPACE_CS + "CpCluster";
	static const string CLASS_DUAL_SIDED_CP			= NAMESPACE_CS + "DualSidedCp";
	static const string CLASS_AP_SERVICE_CATEGORY	= NAMESPACE_CS + "ApServiceCategory";
	static const string CLASS_VLAN_CATEGORY			= NAMESPACE_CS + "VlanCategory";
	static const string CLASS_CP_BLADE 				= NAMESPACE_CS + "CpBlade";
	static const string CLASS_OTHER_BLADE 			= NAMESPACE_CS + "OtherBlade";
	static const string CLASS_AP_BLADE 				= NAMESPACE_CS + "ApBlade";
	static const string CLASS_AP_SERVICE			= NAMESPACE_CS + "ApService";
	static const string CLASS_VLAN					= NAMESPACE_CS + "Vlan";
	static const string CLASS_AP_BLADE_INFO 		= NAMESPACE_CS + "ApBladeInfo";
	static const string CLASS_CP_CLUSTER_STRUCT		= NAMESPACE_CS + "AsyncActionProgress";
	static const string CLASS_OM_PROFILE			= NAMESPACE_CS + "OmProfile";
	static const string CLASS_CCFILE				= NAMESPACE_CS + "CcFile";
	static const string CLASS_OM_PROFILE_MANAGER	= NAMESPACE_CS + "OmProfileManager";
	static const string CLASS_OM_PROFILE_STRUCT		= NAMESPACE_CS + "AsyncProfileActionProgress";
	static const string CLASS_CCFILE_MANAGER		= NAMESPACE_CS + "CcFileManager";
	static const string CLASS_CANDIDATE_CCFILE		= NAMESPACE_CS + "CandidateCcFile";
	static const string CLASS_CANDIDATE_STRUCT		= NAMESPACE_CS + "AsyncImportActionProgress";
	static const string CLASS_CP_PRODUCT_INFO		= NAMESPACE_CS + "CpProductInfo";
	static const string CLASS_LDAP_CONFIGURATION	= NAMESPACE_CS + "LdapConfiguration";
	static const string CLASS_CRMGMT				= NAMESPACE_CS + "CrMgmt";
	static const string CLASS_COMPUTE_RESOURCE		= NAMESPACE_CS + "ComputeResource";
	static const string CLASS_COMPUTE_RESOURCE_NETWORK	= NAMESPACE_CS + "Network";
	static const string CLASS_IPLB_CLUSTER			= NAMESPACE_CS + "IplbCluster";

	//CCH Model Class Names
	static const string CLASS_AP_SW_PACKAGE			= NAMESPACE_CCH + "ApManagedSwPackage";
	static const string CLASS_CP_SW_PACKAGE			= NAMESPACE_CCH + "CpManagedSwPackage";

	// FD Model Class Names
	static const std::string CLASS_FUNCTION_DIST	= NAMESPACE_FD + "FunctionDistributionM";
	static const std::string CLASS_APGNODE		    = NAMESPACE_FD + "Apg";
	static const std::string CLASS_FUNCTION 		= NAMESPACE_FD + "Function";

	// CRM Model Class Names
	static const std::string CLASS_CRM_EQUIPMENT			= NAMESPACE_CRM + "Equipment";
	static const std::string CLASS_CRM_COMPUTE_RESOURCE		= NAMESPACE_CRM + "ComputeResource";

	// TransportM Model Class Names
	static const std::string CLASS_TRM_TRANSPORT			= NAMESPACE_TRM + "TransportM";
	static const std::string CLASS_TRM_HOST					= NAMESPACE_TRM + "Host";
	static const std::string CLASS_TRM_EXTERNALNETWORK		= NAMESPACE_TRM + "ExternalNetwork";
	static const std::string CLASS_TRM_INTERFACE			= NAMESPACE_TRM + "Interface";
	static const std::string CLASS_TRM_INTERNALNETWORK		= NAMESPACE_TRM + "InternalNetwork";

	//AXE Top Model Attributes
	static const string ATTR_NODE_ARCHITECTURE 		= "apgShelfArchitecture";
	static const string ATTR_NODE_NUMBER 			= "apNodeNumber";
	static const string ATTR_SYSTEM_TYPE	 		= "systemType";
	static const string ATTR_APT_TYPE	 			= "axeApplication";
	static const string ATTR_APZ_PROTOCOL_TYPE		= "apzProtocolType";

	//CS Model ID Attributes
	static const string ATTR_EQUIPMENT_ID					= NAMESPACE_CS + "equipmentMId";
	static const string ATTR_HARDWARE_ID					= "hardwareMgmtId";
	static const string ATTR_SHELF_ID 						= "shelfId";
	static const string ATTR_ADVANCED_CONF_ID				= "advancedConfigurationId";
	static const string ATTR_LOGICAL_ID						= "logicalMgmtId";
	static const string ATTR_APG_ID							= "apgId";
	static const string ATTR_CLUSTER_CP_ID					= "clusterCpId";
	static const string ATTR_CP_CLUSTER_ID					= "cpClusterId";
	static const string ATTR_DUAL_SIDED_CP_ID				= "dualSidedCpId";
	static const string ATTR_AP_SERVICE_CATEGORY_ID			= "apServiceCategoryId";
	static const string ATTR_VLAN_CATEGORY_ID 				= "vlanCategoryId";
	static const string ATTR_CP_BLADE_ID 					= "cpBladeId";
	static const string ATTR_OTHER_BLADE_ID					= "otherBladeId";
	static const string ATTR_AP_BLADE_ID					= "apBladeId";
	static const string ATTR_AP_SERVICE_ID					= "apServiceId";
	static const string ATTR_VLAN_ID						= "vlanId";
	static const string ATTR_AP_BLADE_INFO_ID				= "apBladeInfoId";
	static const string ATTR_OM_PROFILE_ID					= "omProfileId";
	static const string ATTR_CCFILE_ID						= "ccFileId";
	static const string ATTR_CP_CLUSTER_STRUCT_ID			= "id";
	static const string ATTR_CP_PRODUCT_INFO_ID				= "id";
	static const string ATTR_OM_PROFILE_MANAGER_ID			= "omProfileManagerId";
	static const string ATTR_CCFILE_MANAGER_ID				= "ccFileManagerId";
	static const string ATTR_CANDIDATE_CCFILE_ID			= "candidateCcFileId";
	static const string ATTR_LDAP_CONFIGURATION_ID			= "ldapConfigurationId";
	static const string ATTR_CRM_ID							= "crMgmtId";
	static const string ATTR_COMPUTE_RESOURCE_ID			= "computeResourceId";
	static const string ATTR_COMPUTE_RESOURCE_NETWORK_ID	= "id";
	static const string ATTR_IPLB_CLUSTER_ID				= "iplbClusterId";
	static const string ATTR_CR_TYPE                                        = "crType";
	static const string ATTR_SW_MAU_ID						= "swMauId";

	//CS Model Reference Attributes
	static const string ATTR_CP_BLADE_REF_TO		= "cpBladesDn";
	static const string ATTR_AP_BLADE_REF_TO		= "apBladesDn";

	//CCH Model ID Attributes
	static const string ATTR_BLADE_SW_MGMT_ID 		= NAMESPACE_CCH + "bladeSwMId";
	static const string ATTR_AP_SW_PACKAGE_ID 		= "apManagedSwPackageId";
	static const string ATTR_CP_SW_PACKAGE_ID 		= "cpManagedSwPackageId";
	static const string ATTR_BLADE_SW_PERS_ID 		= "bladeSwPersistanceValueId";

	//FD Model ID Attributes
	static const std::string ATTR_FUNCTION_DIST_ID	= NAMESPACE_FD + "functionDistributionMId";
	static const std::string ATTR_APGNODE_ID		= "apgId";
	static const std::string ATTR_FUNCTION_ID		= "functionId";

	// CRM Model Class Names
	static const std::string ATTR_CRM_EQUIPMENT_ID			= NAMESPACE_CRM + "equipmentId";
	static const std::string ATTR_CRM_COMPUTE_RESOURCE_ID	= "computeResourceId";

	// TRM Model Class Names
	static const std::string ATTR_TRM_TRANSPORT_ID			= NAMESPACE_TRM + "transportMId";
	static const std::string ATTR_TRM_HOST_ID				= "hostId";
	static const std::string ATTR_TRM_INTERFACE_ID			= "interfaceId";
	static const std::string ATTR_TRM_EXTERNALNETWORK_ID	= "externalNetworkId";
	static const std::string ATTR_TRM_INTERNALNETWORK_ID	= "internalNetworkId";

	// TRM Model Attribute Names
	static const std::string ATTR_TRM_ATTACHEDINTERFACES	= "attachedInterfaces";

	//AXE Top Model static DNs
	static const string RDN_MANAGEDELEMENT			= "managedElementId=1";
	static const string RDN_APZFUNCTIONS			= "axeFunctionsId=1";

	//CpCluster related attributes and paramenters
	static const std::string PARAM_OP_MODE_REQUESTED = "clusterOpModeRequested";
	static const std::string PARAM_OM_PROFILE_REQUESTED = "omProfileRequested";
	static const std::string PARAM_IMPORT_CC_FILENAME = "filename";

	//CS Model static DNs
	static const string RDN_EQUIPMENT						= ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_ADVANCEDCONFIGURATION			= ATTR_ADVANCED_CONF_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_HARDWARECATEGORY				= ATTR_HARDWARE_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_APSERVICECATEGORY				= ATTR_AP_SERVICE_CATEGORY_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_LDAPCONFIGURATION  				= ATTR_LDAP_CONFIGURATION_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_VLANCATEGORY					= ATTR_VLAN_CATEGORY_ID +"=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_AXE_CP_CLUSTER					= ATTR_CP_CLUSTER_ID + "=1," +  ATTR_LOGICAL_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_AXE_LOGICAL_MGMT				= ATTR_LOGICAL_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";
	static const string RDN_AXE_CP_CLUSTER_STRUCT			= ATTR_CP_CLUSTER_STRUCT_ID + "=cpCluster," + RDN_AXE_CP_CLUSTER;
	static const string RDN_AXE_OM_PROFILE_MANAGER  		= ATTR_OM_PROFILE_MANAGER_ID + "=1," + RDN_AXE_CP_CLUSTER;
	static const string RDN_AXE_OM_PROFILE_MANAGER_STRUCT   = ATTR_CP_CLUSTER_STRUCT_ID + "=omProfile," + RDN_AXE_OM_PROFILE_MANAGER;
	static const string RDN_AXE_CCFILE_MANAGER				= ATTR_CCFILE_MANAGER_ID + "=1," + RDN_AXE_OM_PROFILE_MANAGER;
	static const string RDN_CP_PRODUCT_INFO					= ATTR_CP_PRODUCT_INFO_ID + "=productInfo";
	static const string RDN_CRMGMT							= ATTR_CRM_ID + "=1," + ATTR_EQUIPMENT_ID + "=1";


	//CCH Model static DNs
	static const string RDN_BLADE_SW_MGMT			= ATTR_BLADE_SW_MGMT_ID + "=1";

	//FD Model static DNs
	static const std::string RDN_FUNCTION_DIST		= ATTR_FUNCTION_DIST_ID + "=1";

	//CRM Model Static DNs
	static const string RDN_CRM_EQUIPMENT			= ATTR_CRM_EQUIPMENT_ID + "=1";

	//TRM Model Static DNs
	static const string RDN_TRM_TRANSPORT			= ATTR_TRM_TRANSPORT_ID + "=1";

	//FileM CCFile attribute
	static const string ATTR_CCF_FOLDER				= "swPackageCCF";

	//LDAP CLASSES
	static const string classSecM = "SecM";
	static const string classUserManagement = "UserManagement";
	static const string classLdapAuthenticationMethod = "SecLdapAuthenticationLdapAuthenticationMethod";
	static const string classLdap = "SecLdapAuthenticationLdap";

	//CLASS IMPLEMENTER
	static const string IMPLEMENTER_LDAP = "@ACS_CS_LdapProxyHandler";

	//CRR(compute resource role) observer name
	static const string IMPLEMENTER_CRR = "@ACS_CS_CRRProxyHandler";
	//CRR class name
	static const string classCRR = "AxeScalingMComputeResourceRole";

	//ATTRIBUTES
	static const string ATT_LDAPAUTHENTICATIONMETHOD_RDN = "SecLdapAuthenticationldapAuthenticationMethodId=1";
	static const string ATT_LDAP_RDN = "ldapId=1," + ATT_LDAPAUTHENTICATIONMETHOD_RDN;
	static const string ATT_LDAP_BASEDN = "baseDn";
	static const string ATT_LDAP_IPADDRESS = "ldapIpAddress";
	static const string ATT_LDAP_TLSCLIENTKEY = "tlsClientKey";
	static const string ATT_LDAP_TLSCLIENTCERTIFICATE = "tlsClientCertificate";
	static const string ATT_LDAP_TLSCACERTIFICATE = "tlsCaCertificate";

	//BRF Object related declaration
	static const char classBrfPersistentDataOwner[] = "BrfPersistentDataOwner";
	static const char IMPLEMENTER_BRF[] = "ACS_CS_BrfPersistentDataOwner";
	static const char ATT_BRFPERSISTENTDATAOWNER_RDN[] = "brfPersistentDataOwnerId";
	static const char RDN_BRFPERSISTENTDATAOWNER[] = "brfPersistentDataOwnerId=ERIC-APG-ACS-CS";
	static const char DN_BRFPERSISTENTDATAOWNER[] = "brfPersistentDataOwnerId=ERIC-APG-ACS-CS,brfParticipantContainerId=1";
	static const char ATT_BRFPERSISTENTDATAOWNER_VERSION[] = "version";
	static const char ATT_BRFPERSISTENTDATAOWNER_BACKUPTYPE[] = "backupType";
	static const char ATT_BRFPARTICIPANTCONTAINER_RDN[] = "brfParticipantContainerId";
	static const char RDN_BRFPARTICIPANTCONTAINER[] = "brfParticipantContainerId=1";
	
	static const string DELIM = "/";
	static const string MAUTYPE_PATH = DELIM + "storage" + DELIM + "system" + DELIM + "config" + DELIM + "acs_csbin" + DELIM + "mau_type";
	static const int STATICIP_ENTRY_SIZE = 4;

	static const std::string AP = "AP";
	static const std::string CP = "CP";
	static const std::string BC = "BC";
	static const std::string IPLB = "IPLB";


	static const std::string SIDE_A = "A";
	static const std::string SIDE_B = "B";

	static const std::string AXEINFO_DHCP_OBJECT = "axeInfoId=apg_dhcp";
	static const std::string AXEINFO_CP_OBJECT = "axeInfoId=cphw_env";
	static const std::string TOGGLE_OFF = "OFF";

 }

#ifndef CUTE_TEST
class ACS_CS_ImUtils {
public:
   ACS_CS_ImUtils();
   virtual ~ACS_CS_ImUtils();

   static string getClassName(const ACS_APGCC_ImmObject &object);
   static string typeToString(ACS_CC_AttrValueType type);
   static void printImmObject(const ACS_APGCC_ImmObject &object);
   static void printImBaseObject(ACS_CS_ImBase *object);
   static std::string getClassName(ClassType type);
   static ACS_CS_ImBase * getClassObject(ClassType type);
   static void printValuesDefinitionType(ACS_CC_ValuesDefinitionType &valuesDefinitionType);
   static string getErrorText(int errNo);


   static string getParentName(const ACS_CS_ImBase * child);
   static string getParentName(const string & rdn);
   static string getIdValueFromRdn(const string & rdn);
   static string getIdAttributeFromClassName (const string & className);

   static map<string,string>* getClassMap();

   static ACS_CS_ImValuesDefinitionType createStringType(const char * attrName, const string &value);
   static ACS_CS_ImValuesDefinitionType createNameType(const char * attrName, const string &value);
   static ACS_CS_ImValuesDefinitionType createNameType(const char * attrName, const std::set<string>& strings);
   static ACS_CS_ImValuesDefinitionType createUIntType(const char * attrName, unsigned int &value);
   static ACS_CS_ImValuesDefinitionType createUIntType(const char * attrName, uint16_t &value);
   static ACS_CS_ImValuesDefinitionType createUIntType(const char * attrName, OgClearCode &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, int &value);

   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, bool &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, int16_t &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ApNameEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ProfileChangeTriggerEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, AdminState &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, OmProfilePhaseEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ClusterOpModeEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ClusterOpModeTypeEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, AptTypeEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, VlanStackEnum &value);
//qos start
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, VlanTypeEnum &value);
//qos end
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, SystemTypeEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, SideEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, FunctionalBoardNameEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, DhcpEnum &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, OgClearCode &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, AsyncActionStateType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, AsyncActionType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ActionResultType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, OmProfileStateType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, CcFileStateType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, uint16_t &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, ProfileScopeType &value);
   static ACS_CS_ImValuesDefinitionType createIntType(const char * attrName, RoleType &value);
   static ACS_CS_ImValuesDefinitionType createMultipleIntType(const char* attrName, std::set<int> integers);
   static ACS_CS_ImValuesDefinitionType createMultipleStringType(const char* attrName, const std::set<string>& strings);

   static ACS_CS_ImValuesDefinitionType createEmptyStringType(const char * attrName);
   static ACS_CS_ImValuesDefinitionType createEmptyNameType(const char * attrName);
   static ACS_CS_ImValuesDefinitionType createEmptyIntType(const char * attrName);
   static ACS_CS_ImValuesDefinitionType createEmptyUIntType(const char * attrName);

   static bool getImmAttributeString (std::string object, std::string attribute, std::string &value);
   static bool getImmAttributeInt(std::string object, std::string attribute, int &value);
   static int readAPZType();
   static string readDHCPToggle();
	 static string getToggle(std::string dn);

   static void createStaticIpEntry(std::string &magazine, uint16_t & slot, std::string &ipA, std::string &ipB, std::string & entry);
   static bool fetchDataFromStaticIpEntry(std::string &magazine, uint16_t & slot, std::string &ipA, std::string &ipB, std::string & entry);

   static void setFaultyAttribute(const std::string&);
   static void setFaultyAttribute(const int value);
   static void setFaultyFbn(const int value);
   static void setFaultyCpArchitecture(const int value);

   static bool getNbiCcFile(std::string & completeFolderPath);

   static bool isAllowedAliasName(std::string aliasName);
   static bool isReservedCPName (std::string cpName);
   static bool isValidAPZSystem(std::string apzSys);
   static bool isValidCPType(uint16_t type);

   static std::string getRoleLabel(const int& type, const int& side, const int& seqNo = -1);
   static std::string getSideLabel (const int& side);
   static std::string getCrTypeLabel(const int& type);

	
   static ACS_APGCC_AdminOperationParamType createAdminOperationErrorParam(std::string errorText);
   
   static bool getObject(std::string dn, ACS_APGCC_ImmObject *object);
   static bool createImmObject(const char *p_className, const char *parent, vector<ACS_CC_ValuesDefinitionType> AttrList);
   static bool deleteImmObject(const char *dn);
   
   static ACS_CC_ValuesDefinitionType defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue);
   static ACS_CC_ValuesDefinitionType defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);
   
   static bool modifyImmAttribute(const char *object, ACS_CC_ImmParameter parameter);
   static ACS_CC_ImmParameter defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue);
   static ACS_CC_ImmParameter defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue);

private:

   static void printVlanObject(ACS_CS_ImVlan *vlan);
   static void printVlanCategoryObject(ACS_CS_ImVlanCategory *vlanCategory);
   static void printShelfObject(ACS_CS_ImShelf *shelf);
   static void printOtherBladeObject(ACS_CS_ImOtherBlade *otherBlade);
   static void printCpBladeObject(ACS_CS_ImCpBlade *cpBlade);
   static void printClusterCpObject(ACS_CS_ImClusterCp *cp);
   static void printDualSidedCpObject(ACS_CS_ImDualSidedCp *cp);
   static void printEquipmentObject(ACS_CS_ImEquipment *configurationInfo);
   static void printCpClusterObject(ACS_CS_ImCpCluster *bladeClusterInfo);
   static void printApServiceCategoryObject(ACS_CS_ImApServiceCategory *apServiceCategory);
   static void printApServiceObject(ACS_CS_ImApService *apService);
   static void printApBladeObject(ACS_CS_ImApBlade *apBlade);
   static void printApObject(ACS_CS_ImAp *ap);
   static void printAdvancedConfigurationObject(ACS_CS_ImAdvancedConfiguration *advancedConfiguration);

   static void printBladeParameters(ACS_CS_ImBlade *blade);

   static void printCpClusterStructObject(ACS_CS_ImCpClusterStruct *bladeClusterStructInfo);
   static void printOmProfileObject(ACS_CS_ImOmProfile *omProfileInfo);
   static void printCcFileObject(ACS_CS_ImCcFile *ccFileInfo);
   static void printOmProfileManagerObject(ACS_CS_ImOmProfileManager *omProfileMgrInfo);
   static void printCcFileManagerObject(ACS_CS_ImCcFileManager *ccFileManagerInfo);
   static void printCandidateCcFileObject(ACS_CS_ImCandidateCcFile *candidateCcFileInfo);
   static void printCpProductInfoObject(ACS_CS_ImCpProductInfo* info);
   static map<string, string> ClassMap;
   static string faultyValue;
};
#endif

#endif /* ACS_CS_IMUTILS_H_ */
