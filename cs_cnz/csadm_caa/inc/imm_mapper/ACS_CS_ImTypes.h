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
 ----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImTypes.h
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

#ifndef ACS_CS_IMTYPES_H_
#define ACS_CS_IMTYPES_H_

#include <string>
#ifndef CUTE_TEST
#include <ACS_CC_Types.h>
#include <vector>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <algorithm>
#endif

typedef std::string IpDNSAddress;

enum AptTypeEnum
{
   MCS = 0,
   HLR = 1,
   BCS = 2,
   WLN = 3,
   UNDEF_APTTYPE = -1
};

enum ApNameEnum
{
   AP1 = 1,
   AP2 = 2,
   UNDEF_APNAME = -1
};

enum ProfileChangeTriggerEnum
{
   MANUAL = 1,
   NO_CHANGE = 0,
   AUTOMATIC = 2,
   MANUAL_FOR_APG_ONLY = 3,
   UNDEF_PROFILECHANGETRIGGER = -1
};

enum OmProfilePhaseEnum
{
   IDLE = -1,
   VALIDATE = 0,
   AP_NOTIFY = 1,
   CP_NOTIFY = 2,
   COMMIT = 3,
   UNDEF_OMPROFILEPHASE = 4
};

enum ClusterOpModeEnum
{
   NORMAL = 0,
   SWITCHING_TO_NORMAL = 1,
   EXPERT = 2,
   SWITCHING_TO_EXPERT = 3,
   UNDEF_CLUSTEROPMODE = -1
};

enum ClusterOpModeTypeEnum
{
	CLUSTER_OP_MODE_REQUEST = 0,
	CLUSTER_OP_MODE_CHANGED = 1,
	CLUSTER_OP_MODE_REPLICATION = 2,
	UNDEF_CLUSTEROPMODETYPE = -1
};

enum VlanStackEnum
{
   KIP = 0,
   TIP = 1,
   UNDEF_VLANSTACK = -1
};

//qos start
enum VlanTypeEnum
{
   APZ = 0,
   APT = 1,
   UNDEF_VLANTYPE = -1
};
//qos end


enum SystemTypeEnum
{
   SINGLE_SIDED_CP = 0,
   DOUBLE_SIDED_CP = 1000,
   AP              = 2000,
   UNDEF_SYSTEMTYPE = -1
};


enum SideEnum {
   A = 0,
   B = 1,
   UNDEF_SIDE = -1
};


//typedef IpDNSAddress string;

enum FunctionalBoardNameEnum {
   SCB_RP = 100,
   RPBI_S = 110,
   GESB = 120,
   CPUB = 200,
   MAUB = 210,
   APUB = 300,
   DISK = 310,
   DVD = 320,
   GEA = 330,
   SCXB = 340,
   IPTB = 350,
   EPB1 = 360,
   EVOET = 370,
   CMXB = 380,
   IPLB =390,
   SMXB =400,
   UNDEF_FUNCTIONALBOARDNAME = -1
};

enum DhcpEnum {
   DHCP_NONE = 0,
   DHCP_NORMAL = 1,
   DHCP_CLIENT = 2,
   UNDEF_DHCP = -1
};

enum ClassType {
   UNDEFINED_T,
   ADVANCEDCONFIGURATION_T,
   AP_T,
   APBLADE_T,
   APCATEGORY_T,
   APSERVICE_T,
   APSERVICECATEGORY_T,
   BLADE_T,
   CPCLUSTER_T,
   EQUIPMENT_T,
   CLUSTERCP_T,
   DUALSIDEDCP_T,
   CPBLADE_T,
   CPCATEGORY_T,
   CPGROUP_T,
   CPGROUPCATEGORY_T,
   HARDWAREMGMT_T,
   OTHERBLADE_T,
   SHELF_T,
   VLAN_T,
   VLANCATEGORY_T,
   BLADEINFO_T,
   LOGICALMGMT_T,
   CPCLUSTERSTRUCT_T,
   OMPROFILE_T,
   CCFILE_T,
   OMPROFILEMANAGER_T,
   OMPROFILEMANAGERSTRUCT_T,
   CCFILEMANAGER_T,
   CANDIDATECCFILE_T,
   CANDIDATECCFILESTRUCT_T,
   FUNCTIONDISTRIBUTION_T,
   APGNODE_T,
   APFUNCTION_T,
   CPPRODUCTINFO_T,
   CRMGMT_T,
   COMPUTERESOURCE_T,
   IPLBCLUSTER_T,
   CRM_EQUIPMENT_T,
   CRM_COMPUTERESOURCE_T,
   CR_NETWORKSTRUCT_T,
   TRM_TRANSPORT_T,
   TRM_HOST_T,
   TRM_INTERFACE_T,
   TRM_EXTERNALNETWORK_T,
   TRM_INTERNALNETWORK_T
};


enum OgClearCode {
   UNDEF_CLEARCODE 	= -1,
   NOT_EMPTY 		= 0,
   CLUSTER_RECOVERY = 1,
   OTHER_REASON 	= 2
};

enum hwVersionEnum {
	APG43   	= 1,
	APG43_2 	= 2,
	APG43_3 	= 3,
	APG43_VM 	= 4,
	APG43_4  	= 5,
	UNDEF_HWV 	= -1
};

enum SystemType {
	SINGLE_CP_SYSTEM = 0,
	MULTI_CP_SYSTEM  = 1
};

enum ApgShelfArchitecture {
	NODE_SCB 	= 0,
	NODE_SCX  	= 1,
	NODE_DMX 	= 2,
	NODE_VIRTUALIZED = 3,
	NODE_SMX = 4,
	NODE_UNDEFINED = -1
};

enum AsyncActionType {
	UNDEF_ACTIONTYPE	= -1,
	UNDEFINED_TYPE		= 0,
	CHANGE_OP_MODE 		= 1,
	CHANGE_OM_PROFILE	= 2,
	REMOVE_OM_PROFILES 	= 3,
	ACTIVATE_CCF		= 4,
	IMPORT_CCF			= 5,
	EXPORT_CCF			= 6,
	ROLLBACK_CCF		= 7
};

enum AsyncActionStateType {
	UNDEF_ACTIONSTATETYPE		= -1,
	UNDEFINED_STATE	= 0,
	RUNNING			= 1,
	FINISHED		= 2
};

enum ActionResultType {
	UNDEF_ACTIONRESULTTYPE		= -1,
	SUCCESS			= 1,
	FAILURE			= 2,
	NOT_AVAILABLE	= 3
};

enum OmProfileStateType {
	UNDEF_OMPROFILESTATETYPE 	= -1,
	UNDEFINED_OMP				= 0,
	ACTIVE_OMP					= 1,
	SUPPORTED_OMP				= 2,
	ACTIVE_WITHOUT_CCF_OMP		= 3
};

enum CcFileStateType {
	UNDEF_CCFSTATETYPE 	= -1,
	NEW_CCF				= 0,
	CURRENT_CCF			= 1,
	BACKUP_CCF			= 2
};

enum ProfileScopeType {
	UNDEF_PROFILESCOPETYPE 	= -1,
	CP_AP				= 0,
	ONLY_APG			= 1
};

enum RoleType {
	UNDEF_TYPE 	= -1,
	BC_TYPE 	= 0,
	CP_TYPE 	= 1000,
	AP_TYPE 	= 2000,
	IPLB_TYPE 	= 3000,
};

enum RoleAssignment
{
    ROLE_FREE        = -1,
    ROLE_EXCEEDED    = -2,
	ROLE_REVOKED	 = -3,
	ROLE_MAINTENANCE = -4
};

enum RoleAssignmentState
{
    ROLE_UNASSIGNED  = 0,
    ROLE_ASSIGNED    = 1
};

enum AdminState
{
	LOCKED = 0,
	UNLOCKED,
	EMPTY_ADMIN_STATE
};

#endif /* ACS_CS_IMTYPES_H_ */
