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
 * @file ACS_CS_ImUtils.cpp
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

#include <cstdarg>
#include <sstream>
#include <set>
#include <string.h>
#include <stdlib.h>

#include "acs_apgcc_paramhandling.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_HWC_R2.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_ImUtils_TRACE);

map<string, string> ACS_CS_ImUtils::ClassMap;
string ACS_CS_ImUtils::faultyValue;
ACS_CS_ImUtils::ACS_CS_ImUtils() {
}

ACS_CS_ImUtils::~ACS_CS_ImUtils() {
}

string ACS_CS_ImUtils::getErrorText(int errNo)
{
	string text = "";


	switch(errNo){
	case TC_NOERROR:                     text = "Validation success";                                                               break;
	case TC_INTERNALERROR:               text = "Error when executing (general fault)";                                             break;
	case TC_ENTRYIDNOTSET:               text = "Error when executing: Missing entryId for one or more blades";                     break;
	case TC_COMPLETIONFAULT:             text = "Error when executing (general fault)";                                             break;
	case TC_APZLESSTHAN2:                text = "APZ type is less than 2";															break;
	case TC_SYSTEMNOTOBIG:               text = "Incorrect system number " + faultyValue;                                           break;
	case TC_IPALIASETHAFAULTY:           text = "Incorrect IP alias " + faultyValue ;												break;
	case TC_IPALIASETHBFAULTY:           text = "Incorrect IP alias " + faultyValue;												break;
	case TC_ALIASNETMASKETHAFAULTY:      text = "Incorrect netmask " + faultyValue;													break;
	case TC_ALIASNETMASKETHBFAULTY:      text = "Incorrect netmask " + faultyValue;													break;
	case TC_APREFFAULTY:                 text = "AP reference faulty";  															break;
	case TC_APREFFAULTYINTERNAL:         text = "AP reference faulty (internal error)";   											break;
	case TC_CPREFFAULTY:                 text = "CP reference faulty";																break;
	case TC_CPREFFAULTYINTERNAL:         text = "CP reference faulty (internal error)"; 											break;
	case TC_CPNAMEFAULTY:                text = "Error when executing: Invalid CP name";                                            break;
	case TC_CPCPREFFAULTY:               text = "CP reference is faulty";															break;
	case TC_CPSEQNOFAULTY:               text = "Incorrect sequence number " + faultyValue;											break;
	case TC_CPSYSNOFAULTY:               text = "Incorrect system number " + faultyValue;                                           break;
	case TC_CPSYSTYPEFAULTY:             text = "Incorrect CP architecture " + faultyValue;                                         break;
	case TC_CPBCREFFAULTY:               text = "CP reference is faulty";															break;
	case TC_CPNOCPCAT:                   text = "No CP category defined";															break;
	case TC_CPTOMANYINSCP:               text = "Too many CPs defined in SCP";														break;
	case TC_CPSYSIDFAULTY:               text = "CP system id is faulty";															break;
	case TC_INCORRECTSHELF:              text = "Incorrect shelf address " + faultyValue;											break;
	case TC_SCXBSLOTFAULTY:              text = "Incorrect slot number " + faultyValue + " for SCXB";                               break;
	case TC_SMXBSLOTFAULTY:              text = "Incorrect slot number " + faultyValue + " for SMXB";                               break;
	case TC_SCXBDHCPFAULTY:              text = "No DHCP option specified for SCXB";												break;
	case TC_SMXBDHCPFAULTY:              text = "No DHCP option specified for SMXB";												break;
	case TC_SCXBNOPRODNO:                text = "Unset mandatory attribute for SCXB: <bladeProductNumber>";                         break;
	case TC_SMXBNOPRODNO:                text = "Unset mandatory attribute for SMXB: <bladeProductNumber>";                         break;
	case TC_EPB1SLOTFAULTY:              text = "Incorrect slot number " + faultyValue + " for EPB1";                               break;
	case TC_EVOETSLOTFAULTY:             text = "Incorrect slot number " + faultyValue + " for EvoET";                              break;
	case TC_SLOTOCCUPIED:                text = "Slot " + faultyValue + " already occupied";                                        break;
	case TC_NOCONFIGURATIONOBJECTFOUND:  text = "Could not find configuration info object";											break;
	case TC_CPTYPENOTZERO:               text = "Illegal CP type " + faultyValue + " in this system configuration";                 break;
	case TC_INCORRECTCPALIAS:            text = "Illegal CP alias " + faultyValue + " in this system configuration";				break;
	case TC_APZSYSTEMNOTZERO:            text = "Illegal APZ system " + faultyValue + " in this system configuration";				break;
	case TC_SENDTOMASTERFAILED:          text = "AP1 returns validation failed"; 													break;
	case TC_APNAMEFAULTY:                text = "Error when executing: Invalid AP name";                                            break;
	case TC_SLOTOUTOFRANGE:              text = "Slot number is greater than 25";           										break;
	case TC_SIDENOTSPECIFIED:            text = "Unset mandatory attribute: <side>";                                                break;
	case TC_FAULTYSYSTEMTYPE:            text = "Error when executing: Incorrect system type for one or more blades";           	break;
	case TC_FORBIDDENFBN:                text = "Illegal FBN " + faultyValue + " in this system configuration";                     break;
	case TC_INCORRECTMAG:                text = "Illegal FBN " + faultyValue + " in this shelf";           							break;
	case TC_INVALID_FBN:                 text = "Incorrect FBN for this blade type";           										break;
	case TC_SHELFNOTEMPTY:               text = "Cannot delete not empty shelf";           											break;
	case TC_NODEFAULTSOFTWARE:           text = "Missing default software version for specified FBN " + faultyValue;				break;
	case TC_APUBALREADYPRESENT:	     	 text = "APUB with the same system number and side already defined";           				break;
	case TC_CMXBSLOTFAULTY:		         text = "Incorrect slot number " + faultyValue + " for CMXB";                               break;
	case TC_MISSINGSCXINAPZMAGAZINE:     text = "Both SCXBs must be already defined in the APZ shelf";                              break;
	case TC_DISKALREADYPRESENT:			 text = "DISK with the same system number and side already defined";						break;
	case TC_IPLBSLOTFAULTY:		     	 text = "Incorrect slot number " + faultyValue + " for IPLB";                               break;
	case TC_IPLBALREADYPRESENT:	     	 text = "IPLB with the same system number and side already defined";           				break;
	case TC_IPTBSLOTFAULTY:		     	 text = "Incorrect slot number " + faultyValue + " for IPTB";                               break;
	case TC_CPALIASFAULTY:		     	 text = "Incorrect CP alias " + faultyValue ;                               				break;
	case TC_CPALIASINUSE:		     	 text = "CP alias " + faultyValue + " reserved or occupied";                               	break;
	case TC_INVALIDAPZSYSTEM:		     text = "Incorrect APZ system " + faultyValue ;                               				break;
	case TC_INVALIDCPTYPE:		     	 text = "Incorrect CP type " + faultyValue ;                               					break;
	case TC_NOTALLOWED_AP2:			 	 text = "Operation Not Allowed on AP2" ;													break;
	case TC_CPUBALREADYPRESENT:	     	 text = "Dual Sided CPUB with the same system number and side already defined";  			break;
	case TC_MAUBALREADYPRESENT:	     	 text = "MAUB with the same system number and side already defined";  						break;
	case TC_INVALIDMAUTYPE:	 			 text = "Incorrect MAU type " + faultyValue;												break;
	case TC_MAUSFAULTY:	       			 text = "Cannot have MAUB or RPBI_S when MAU type is MAUS";									break;
	case TC_NOTALLOWED:	       			 text = "Operation Not Allowed in this system configuration";								break;
	case TC_INVALIDIP:	       			 text = "Invalid IP address " + faultyValue;												break;
	case TC_INVALIDMAC:	       			 text = "Invalid MAC address " + faultyValue;												break;
	case TC_ATTRALREADYUSED:   			 text = "Attribute(s) already used: " + faultyValue;										break;
	case TC_INVALIDUUID:   			 	 text = "Invalid UUID: " + faultyValue + ", it doesn't match ComputeResourceId";			break;
	case TC_RESTRICTEDOP:   			 text = "Operation Not Allowed";
															break;
        case TC_INVALIDCRROLEID:                         text = "Invalid crRoleID : " + faultyValue;
                                                    break;
	default:                             text = "Error when executing (general fault)";												break;

	}

	//Clean up the faulty value
	faultyValue = "";

	return text;
}


string ACS_CS_ImUtils::typeToString(ACS_CC_AttrValueType type)
{
	string retStr;

	switch (type)
	{
	case ATTR_INT32T:
		retStr = "ATTR_INT32T";
		break;

	case ATTR_UINT32T:
		retStr = "ATTR_UINT32T";
		break;
	case ATTR_INT64T:
		retStr = "ATTR_INT64T";
		break;

	case ATTR_UINT64T:
		retStr = "ATTR_UINT64T";
		break;

	case ATTR_TIMET:
		retStr = "ATTR_TIMET";
		break;

	case ATTR_NAMET:
		retStr = "ATTR_NAMET";
		break;

	case ATTR_FLOATT:
		retStr = "ATTR_FLOATT";
		break;

	case ATTR_DOUBLET:
		retStr = "ATTR_DOUBLET";
		break;

	case ATTR_STRINGT:
		retStr = "ATTR_STRINGT";
		break;

	case ATTR_ANYT:
		retStr = "ATTR_ANYT";
		break;

	default:
		retStr = "UNKNOWN";
		break;
	}

	return retStr;
}

std::string ACS_CS_ImUtils::getClassName(ClassType type)
{
	string typeName;

   switch(type)
   {
   case VLAN_T:
	   typeName = ACS_CS_ImmMapper::CLASS_VLAN;
	   break;
   case VLANCATEGORY_T:
	   typeName = ACS_CS_ImmMapper::CLASS_VLAN_CATEGORY;
   	   break;
   case SHELF_T:
	   typeName = ACS_CS_ImmMapper::CLASS_SHELF;
   	   break;
   case OTHERBLADE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_OTHER_BLADE;
   	   break;
   case HARDWAREMGMT_T:
	   typeName = ACS_CS_ImmMapper::CLASS_HARDWARE;
   	   break;
   case CPBLADE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CP_BLADE;
   	   break;
   case CLUSTERCP_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CLUSTER_CP;
   	   break;
   case DUALSIDEDCP_T:
	   typeName = ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP;
	   break;
   case EQUIPMENT_T:
	   typeName = ACS_CS_ImmMapper::CLASS_EQUIPMENT;
   	   break;
   case CPCLUSTER_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CP_CLUSTER;
	   break;
   case CPCLUSTERSTRUCT_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT;
	   break;
   case APSERVICECATEGORY_T:
	   typeName = ACS_CS_ImmMapper::CLASS_AP_SERVICE_CATEGORY;
	   break;
   case APSERVICE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_AP_SERVICE;
	   break;
   case APBLADE_T:
   	   typeName = ACS_CS_ImmMapper::CLASS_AP_BLADE;
	   break;
   case AP_T:
	   typeName = ACS_CS_ImmMapper::CLASS_APG;
   	   break;
   case ADVANCEDCONFIGURATION_T:
	   typeName = ACS_CS_ImmMapper::CLASS_ADVANCED_CONF;
	   break;
   case OMPROFILE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_OM_PROFILE;
	   break;
   case CCFILE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CCFILE;
	   break;
   case OMPROFILEMANAGER_T:
   	   typeName = ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER;
   	   break;
   case CCFILEMANAGER_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER;
	   break;
   case CANDIDATECCFILE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE;
	   break;
   case OMPROFILEMANAGERSTRUCT_T:
	   typeName = ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT;
	   break;
   case CANDIDATECCFILESTRUCT_T:
   	   typeName = ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT;
   	   break;
   case CPPRODUCTINFO_T:
	   typeName = ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO;
	   break;
   case CRMGMT_T:
   	   typeName = ACS_CS_ImmMapper::CLASS_CRMGMT;
   	   break;
   case COMPUTERESOURCE_T:
	   typeName = ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE;
	   break;
   case CR_NETWORKSTRUCT_T:
	   typeName = ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK;
	   break;
   case IPLBCLUSTER_T:
	   typeName = ACS_CS_ImmMapper::CLASS_IPLB_CLUSTER;
	   break;
   default:
	   break;
   }
   
   return typeName;
}

ACS_CS_ImBase * ACS_CS_ImUtils::getClassObject(ClassType type)
{
	ACS_CS_ImBase *object = NULL;

   switch(type)
   {
   case VLAN_T:
	   object = new ACS_CS_ImVlan();
	   break;
   case VLANCATEGORY_T:
	   object = new ACS_CS_ImVlanCategory();
   	   break;
   case SHELF_T:
	   object = new ACS_CS_ImShelf();
   	   break;
   case OTHERBLADE_T:
	   object = new ACS_CS_ImOtherBlade();
   	   break;
   case HARDWAREMGMT_T:
	   object = new ACS_CS_ImHardwareMgmt();
   	   break;
   case CPBLADE_T:
	   object = new ACS_CS_ImCpBlade();
   	   break;
   case CLUSTERCP_T:
	   object = new ACS_CS_ImClusterCp();
   	   break;
   case DUALSIDEDCP_T:
	   object = new ACS_CS_ImDualSidedCp();
	   break;
   case EQUIPMENT_T:
	   object = new ACS_CS_ImEquipment();
   	   break;
   case CPCLUSTER_T:
	   object = new ACS_CS_ImCpCluster();
	   break;
   case CPCLUSTERSTRUCT_T:
	   object = new ACS_CS_ImCpClusterStruct();
	   break;
   case APSERVICECATEGORY_T:
	   object = new ACS_CS_ImApServiceCategory();
	   break;
   case APSERVICE_T:
	   object = new ACS_CS_ImApService();
	   break;
   case APBLADE_T:
	   object = new ACS_CS_ImApBlade();
	   break;
   case AP_T:
	   object = new ACS_CS_ImAp();
   	   break;
   case ADVANCEDCONFIGURATION_T:
	   object = new ACS_CS_ImAdvancedConfiguration();
   	   break;
   case OMPROFILE_T:
	   object = new ACS_CS_ImOmProfile();
	   break;
   case CCFILE_T:
   	   object = new ACS_CS_ImCcFile();
   	   break;
   case OMPROFILEMANAGER_T:
   	   object = new ACS_CS_ImOmProfileManager();
   	   break;
   case CCFILEMANAGER_T:
	   object = new ACS_CS_ImCcFileManager();
	   break;
   case CANDIDATECCFILE_T:
	   object = new ACS_CS_ImCandidateCcFile();
	   break;
   case OMPROFILEMANAGERSTRUCT_T:
	   object = new ACS_CS_ImOmProfileStruct();
	   break;
   case CANDIDATECCFILESTRUCT_T:
	   object = new ACS_CS_ImCandidateCcFileStruct();
	   break;
   case CRMGMT_T:
   	   object = new ACS_CS_ImCrMgmt();
   	   break;
   case COMPUTERESOURCE_T:
	   object = new ACS_CS_ImComputeResource();
	   break;
   case IPLBCLUSTER_T:
   	   object = new ACS_CS_ImIplbCluster();
   	   break;

   default:
	   break;
   }

   return object;
}

string ACS_CS_ImUtils::getClassName(const ACS_APGCC_ImmObject &object)
{
	string retStr;

	for (unsigned int i = 0; i <  object.attributes.size(); i++)
	{
		if (object.attributes[i].attrName == "SaImmAttrClassName")
		{
			retStr = (char *) object.attributes[i].attrValues[0];
			break;
		}
	}

	return retStr;
}


void ACS_CS_ImUtils::printImBaseObject(ACS_CS_ImBase *object)
{
	if (object == 0)
		return;

	ACS_CS_ImVlan *vlan = dynamic_cast<ACS_CS_ImVlan *>(object);
	ACS_CS_ImVlanCategory *vlanCategory = dynamic_cast<ACS_CS_ImVlanCategory *>(object);
	ACS_CS_ImShelf *shelf = dynamic_cast<ACS_CS_ImShelf *>(object);
	ACS_CS_ImOtherBlade *otherBlade = dynamic_cast<ACS_CS_ImOtherBlade *>(object);
	//ACS_CS_ImHardwareConfigurationCategory *hardwareConfigurationCategory = dynamic_cast<ACS_CS_ImHardwareConfigurationCategory *>(object);
	//ACS_CS_ImCpGroupCategory *cpGroupCategory = dynamic_cast<ACS_CS_ImCpGroupCategory *>(object);
	//ACS_CS_ImCpGroup *cpGroup = dynamic_cast<ACS_CS_ImCpGroup *>(object);
	//ACS_CS_ImCpCategory *cpCategory = dynamic_cast<ACS_CS_ImCpCategory *>(object);
	ACS_CS_ImCpBlade *cpBlade = dynamic_cast<ACS_CS_ImCpBlade *>(object);
	ACS_CS_ImClusterCp *clustercp = dynamic_cast<ACS_CS_ImClusterCp *>(object);
	ACS_CS_ImDualSidedCp *dualcp = dynamic_cast<ACS_CS_ImDualSidedCp *>(object);
	ACS_CS_ImEquipment *equipment = dynamic_cast<ACS_CS_ImEquipment *>(object);
	ACS_CS_ImCpCluster *bladeClusterInfo = dynamic_cast<ACS_CS_ImCpCluster *>(object);
	ACS_CS_ImApServiceCategory *apServiceCategory = dynamic_cast<ACS_CS_ImApServiceCategory *>(object);
	ACS_CS_ImApService *apService = dynamic_cast<ACS_CS_ImApService *>(object);
	//ACS_CS_ImApCategory *apCategory = dynamic_cast<ACS_CS_ImApCategory *>(object);
	ACS_CS_ImApBlade *apBlade = dynamic_cast<ACS_CS_ImApBlade *>(object);
	ACS_CS_ImAp *ap = dynamic_cast<ACS_CS_ImAp *>(object);
	ACS_CS_ImAdvancedConfiguration *advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(object);
	ACS_CS_ImCpClusterStruct *bladeClusterStructInfo = dynamic_cast<ACS_CS_ImCpClusterStruct *>(object);
	ACS_CS_ImOmProfile *omProfileInfo = dynamic_cast<ACS_CS_ImOmProfile *>(object);
	ACS_CS_ImCcFile *ccFileInfo = dynamic_cast<ACS_CS_ImCcFile *>(object);
	ACS_CS_ImOmProfileManager *omProfileMgrInfo = dynamic_cast<ACS_CS_ImOmProfileManager *>(object);
	ACS_CS_ImCcFileManager *ccFileManagerInfo = dynamic_cast<ACS_CS_ImCcFileManager *>(object);
	ACS_CS_ImCandidateCcFile *candidateCcFileInfo = dynamic_cast<ACS_CS_ImCandidateCcFile *>(object);
	ACS_CS_ImCpProductInfo *cpInfo = dynamic_cast<ACS_CS_ImCpProductInfo*>(object);

	if (vlan != NULL)
	{
		printVlanObject(vlan);
	}
	else if (vlanCategory != NULL)
	{
		printVlanCategoryObject(vlanCategory);
	}
	else if (shelf != NULL)
	{
		printShelfObject(shelf);
	}
	else if (otherBlade != NULL)
	{
		printOtherBladeObject(otherBlade);
	}
	//   else if (hardwareConfigurationCategory != NULL)
		//   {
	//      printHardwareConfigurationCategoryObject(hardwareConfigurationCategory);
	//   }
	else if (cpBlade != NULL)
	{
		printCpBladeObject(cpBlade);
	}
	else if (clustercp != NULL)
	{
		printClusterCpObject(clustercp);
	}
	else if (dualcp != NULL)
	{
		printDualSidedCpObject(dualcp);
	}
	else if (equipment != NULL)
	{
		printEquipmentObject(equipment);
	}
	else if (bladeClusterInfo != NULL)
	{
		printCpClusterObject(bladeClusterInfo);
	}
	else if (bladeClusterStructInfo != NULL)
	{
		printCpClusterStructObject(bladeClusterStructInfo);
	}

	else if (apServiceCategory != NULL)
	{
		printApServiceCategoryObject(apServiceCategory);
	}

	else if (apService != NULL)
	{
		printApServiceObject(apService);
	}

	else if (apBlade != NULL)
	{
		printApBladeObject(apBlade);
	}

	else if (ap != NULL)
	{
		printApObject(ap);
	}

	else if (advancedConfiguration != NULL)
	{
		printAdvancedConfigurationObject(advancedConfiguration);
	}

	else if (omProfileInfo != NULL)
	{
		printOmProfileObject(omProfileInfo);
	}

	else if (ccFileInfo != NULL)
	{
		printCcFileObject(ccFileInfo);
	}

	else if (omProfileMgrInfo != NULL)
	{
		printOmProfileManagerObject(omProfileMgrInfo);
	}

	else if (ccFileManagerInfo != NULL)
	{
		printCcFileManagerObject(ccFileManagerInfo);
	}

	else if (candidateCcFileInfo != NULL)
	{
		printCandidateCcFileObject(candidateCcFileInfo);
	}
	else if (cpInfo != NULL)
	{
		printCpProductInfoObject(cpInfo);
	}


}


void ACS_CS_ImUtils::printImmObject(const ACS_APGCC_ImmObject &object)
{
	cout << "------------------------ ImmObject ------------------------" << endl;
	cout << "ObjName:" << object.objName << endl;
	cout << "Number of attributes: " << object.attributes.size() << endl;

	for (unsigned int i = 0; i <  object.attributes.size(); i++)
	{
		cout << "Attribute: " << i + 1 << endl;
		cout << "\tattrName: " << object.attributes[i].attrName << endl;
		cout << "\tattrType: " << object.attributes[i].attrType << " (" << typeToString((ACS_CC_AttrValueType) object.attributes[i].attrType) << ")" << endl;
		cout << "\tattrValuesNum: " << object.attributes[i].attrValuesNum << endl;

		for (unsigned int j = 0; j < object.attributes[i].attrValuesNum; j++)
		{
			cout << "\t\tattrValues[" << j << "]: ";

			switch (object.attributes[i].attrType)
			{
			case ATTR_INT32T:
				cout << (*(int *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_UINT32T:
				cout << (*(unsigned int*) object.attributes[i].attrValues[j]);
				break;

			case ATTR_INT64T:
				cout << (*(long *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_UINT64T:
				cout << (*(unsigned long *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_TIMET:
				cout << ((char *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_NAMET:
				cout << ((char *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_FLOATT:
				cout << *((float *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_DOUBLET:
				cout << *((double *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_STRINGT:
				cout << ((char *) object.attributes[i].attrValues[j]);
				break;

			case ATTR_ANYT:
				cout << object.attributes[i].attrValues[j];
				break;

			default:
				cout << object.attributes[i].attrValues[j] << endl;
				break;

			}

			cout << endl;
			cout << "--------------------- End ImmObject ------------------------" << endl;

		}
	}
}


void ACS_CS_ImUtils::printVlanObject(ACS_CS_ImVlan *vlan)
{
	if (vlan == 0)
		return;

	cout << endl;
	cout << "---------------- VLAN OBJECT -------------------" << endl;
	cout << "rdn = " << vlan->rdn << endl;
	cout << "vlanId = " << vlan->vlanId << endl;
	cout << "networkAddress = " << vlan->networkAddress << endl;
	cout << "netmask = " << vlan->netmask << endl;
	cout << "name = " << vlan->name << endl;
	cout << "stack = " << vlan->stack << endl;
	cout << "pcp = " << vlan->pcp <<endl;
	cout << "vlanType = " << vlan->vlanType <<endl;
	cout << "vlanTag = " << vlan->vlanTag <<endl;
	cout << "vNICName = " << vlan->vNICName <<endl;
	cout << "reserved = " << vlan->reserved <<endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printVlanCategoryObject(ACS_CS_ImVlanCategory *vlanCategory)
{
	if (vlanCategory == 0)
		return;

	string vlanCategoryId;
	set<ACS_CS_ImVlan *> vlans;

	cout << endl;
	cout << "---------------- VLANCATEGORY OBJECT -------------------" << endl;
	cout << "rdn = " << vlanCategory->rdn << endl;
	cout << "vlanCategoryId = " << vlanCategory->vlanCategoryId << endl;

	cout << "number of vlans: " << vlanCategory->vlans.size() << endl;

	set<ACS_CS_ImVlan *>::iterator it;
	for (it = vlanCategory->vlans.begin(); it != vlanCategory->vlans.end(); it++)
		printVlanObject(*it);

	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printShelfObject(ACS_CS_ImShelf *shelf)
{

	if (shelf == 0)
		return;


	cout << endl;
	cout << "---------------- SHELF OBJECT -------------------" << endl;
	cout << "rdn = " << shelf->rdn << endl;
	cout << "axeShelfId = " << shelf->axeShelfId << endl;
	cout << "address = " << shelf->address << endl;

	set<ACS_CS_ImCpBlade *>::iterator cpBladesIt;
	set<ACS_CS_ImOtherBlade *>::iterator otherBladesIt;
	set<ACS_CS_ImApBlade *>::iterator apBladesIt;

	//   cout << "number of cpblades: " << shelf->cpBlades.size() << endl << endl;
	//   for (cpBladesIt = shelf->cpBlades.begin(); cpBladesIt != shelf->cpBlades.end(); cpBladesIt++)
	//      printCpBladeObject(*cpBladesIt);
	//
	//   cout << "number of apblades: " << shelf->apBlades.size() << endl << endl;
	//   for (apBladesIt = shelf->apBlades.begin(); apBladesIt != shelf->apBlades.end(); apBladesIt++)
	//      printApBladeObject(*apBladesIt);
	//
	//   cout << "number of otherblades: " << shelf->otherBlades.size() << endl << endl;
	//   for (otherBladesIt = shelf->otherBlades.begin(); otherBladesIt != shelf->otherBlades.end(); otherBladesIt++)
	//      printOtherBladeObject(*otherBladesIt);

	cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printOtherBladeObject(ACS_CS_ImOtherBlade *otherBlade)
{
	if (otherBlade == 0)
		return;

	printBladeParameters(otherBlade);

	cout << "---------------- OTHERBLADE OBJECT -------------------" << endl;
	cout << "otherBladeId = " << otherBlade->axeOtherBladeId << endl;
	cout << "currentLoadModuleVersion = " << otherBlade->currentLoadModuleVersion << endl;
	cout << "bladeProductNumber = " << otherBlade->bladeProductNumber << endl;
	cout << "------------------------------------------------" << endl << endl;
}


//void ACS_CS_ImUtils::printHardwareConfigurationCategoryObject(ACS_CS_ImHardwareConfigurationCategory *hardwareConfigurationCategory)
//{
//   if (hardwareConfigurationCategory == 0)
//      return;
//
//   cout << endl;
//   cout << "---------------- HARDWARECONFIGURATIONCATEGORY OBJECT -------------------" << endl;
//   cout << "rdn = " << hardwareConfigurationCategory->rdn << endl;
//   cout << "hardwareConfigurationCategoryId = " << hardwareConfigurationCategory->hardwareConfigurationCategoryId << endl;
//
//   set<ACS_CS_ImShelf *>::iterator it;
//   for (it = hardwareConfigurationCategory->shelves.begin(); it != hardwareConfigurationCategory->shelves.end(); it++)
//      printShelfObject(*it);
//
//   cout << "------------------------------------------------" << endl << endl;
//}

//void ACS_CS_ImUtils::printCpGroupCategoryObject(ACS_CS_ImCpGroupCategory *cpGroupCategory)
//{
//   if (cpGroupCategory == 0)
//      return;
//
//   cout << endl;
//   cout << "----------------  CPGROUPCATEGORY OBJECT -------------------" << endl;
//   cout << "rdn = " << cpGroupCategory->rdn << endl;
//   cout << "cpGroupCategoryId = " << cpGroupCategory->cpGroupCategoryId << endl;
//   set<ACS_CS_ImCpGroup *>::iterator it;
//   for (it = cpGroupCategory->cpGroups.begin(); it != cpGroupCategory->cpGroups.end(); it++)
//      printCpGroupObject(*it);
//   cout << "------------------------------------------------" << endl << endl;
//}
//
//
//void ACS_CS_ImUtils::printCpGroupObject(ACS_CS_ImCpGroup *cpGroup)
//{
//   cout << endl;
//   cout << "----------------  CPGROUP OBJECT -------------------" << endl;
//   cout << "rdn = " << cpGroup->rdn << endl;
//   cout << "cpGroupId = " << cpGroup->cpGroupId << endl;
//   cout << "groupName = " << cpGroup->groupName << endl;
//
//   set<string>::iterator strIt;
//   set<ACS_CS_ImCp *>::iterator cpIt;
//
//   for (strIt = cpGroup->cpRdns.begin(); strIt != cpGroup->cpRdns.end(); strIt++)
//      cout << *strIt << endl;
//
//   for (cpIt = cpGroup->cps.begin(); cpIt != cpGroup->cps.end(); cpIt++)
//      printCpObject(*cpIt);
//
//   cout << "------------------------------------------------" << endl << endl;
//
//}
//
//void ACS_CS_ImUtils::printCpCategoryObject(ACS_CS_ImCpCategory *cpCategory)
//{
//   if (cpCategory == 0)
//      return;
//
//   cout << endl;
//   cout << "----------------  CPGROUP OBJECT -------------------" << endl;
//   cout << "rdn = " << cpCategory->rdn << endl;
//   cout << "cpCategoryId = " << cpCategory->cpCategoryId << endl;
//
//   set<ACS_CS_ImCp *>::iterator it;
//   for (it = cpCategory->cp.begin(); it != cpCategory->cp.end(); it++)
//      printCpObject(*it);
//
//   cout << "------------------------------------------------" << endl << endl;
//}



void ACS_CS_ImUtils::printCpBladeObject(ACS_CS_ImCpBlade *cpBlade)
{
	if (cpBlade == 0)
		return;

	printBladeParameters(cpBlade);

	cout << "---------------- CPBLADE OBJECT -------------------" << endl;
	cout << "axeCpBladeId = " << cpBlade->axeCpBladeId << endl;

	cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printClusterCpObject(ACS_CS_ImClusterCp *cp)
{
	if (cp == 0)
		return;

	cout << endl;
	cout << "---------------- CP OBJECT ---------------------" << endl;
	cout << "rdn = " << cp->rdn << endl;
	cout << "cpId = " << cp->axeClusterCpId << endl;
	cout << "systemIdentifier = " << cp->systemIdentifier << endl;
	cout << "alias = " << cp->alias << endl;
	cout << "apzSystem = " << cp->apzSystem << endl;
	cout << "cpType = " << cp->cpType << endl;
	cout << "cpState = " << cp->cpState << endl;
	cout << "applicationId = " << cp->applicationId << endl;
	cout << "apzSubstate = " << cp->apzSubstate << endl;
	cout << "aptSubstate = " << cp->aptSubstate << endl;
	cout << "stateTransition = " << cp->stateTransition << endl;
	cout << "blockingInfo = " << cp->blockingInfo << endl;
	cout << "cpCapacity = " << cp->cpCapacity << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printDualSidedCpObject(ACS_CS_ImDualSidedCp *cp)
{
	if (cp == 0)
		return;

   cout << endl;
   cout << "---------------- CP OBJECT ---------------------" << endl;
   cout << "rdn = " << cp->rdn << endl;
   cout << "cpId = " << cp->axeDualSidedCpId << endl;
   cout << "alias = " << cp->alias << endl;
   cout << "apzSystem = " << cp->apzSystem << endl;
   cout << "cpType = " << cp->cpType << endl;
   cout << "mauType = " << cp->mauType <<endl;
   cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printEquipmentObject(ACS_CS_ImEquipment *configurationInfo)
{
	if (configurationInfo == 0)
		return;

	cout << endl;
	cout << "---------------- CONFIGURATIONINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << configurationInfo->rdn << endl;
	cout << ACS_CS_ImmMapper::ATTR_EQUIPMENT_ID << " = " << configurationInfo->axeEquipmentId << endl;
	cout << "isBladeClusterSystem = " << configurationInfo->isBladeClusterSystem << endl;
	//   printAdvancedConfigurationObject(configurationInfo->advancedConfiguration);
	//   printBladeClusterInfoObject(configurationInfo->bladeClusterInfo);
	//   printApCategoryObject(configurationInfo->apCategory);
	//   printHardwareConfigurationCategoryObject(configurationInfo->hardwareConfigurationCategory);
	//   printCpCategoryObject(configurationInfo->cpCategory);
	//   printVlanCategoryObject(configurationInfo->vlanCategory);
	//   printApServiceCategoryObject(configurationInfo->apServiceCategory);
	cout << "------------------------------------------------" << endl << endl;

}



void ACS_CS_ImUtils::printCpClusterObject(ACS_CS_ImCpCluster *bladeClusterInfo)
{
	if (bladeClusterInfo == 0)
		return;

	cout << endl;
	cout << "---------------- BLADECLUSTERINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << bladeClusterInfo->rdn << endl;
	cout << "bladeClusterInfoId = " << bladeClusterInfo->axeCpClusterId << endl;
	cout << "alarmMaster = " << bladeClusterInfo->alarmMaster << endl;
	cout << "clockMaster = " << bladeClusterInfo->clockMaster << endl;
	//   cout << "omProfile = " << bladeClusterInfo->omProfile << endl;
	//   cout << "phase = " << bladeClusterInfo->phase << endl;
	cout << "clusterOpMode = " << bladeClusterInfo->clusterOpMode << endl;
	cout << "clusterOpModeType = " << bladeClusterInfo->clusterOpModeType << endl;
	cout << "reportProgress = " << bladeClusterInfo->reportProgress << endl;
	//   cout << "activeCcFile = " << bladeClusterInfo->activeCcFile << endl;
	cout << "frontAp = " << bladeClusterInfo->frontAp << endl;
	cout << "ogClearCode = " << bladeClusterInfo->ogClearCode << endl;
	cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printApServiceCategoryObject(ACS_CS_ImApServiceCategory *apServiceCategory)
{
	if (apServiceCategory == 0)
		return;

	cout << endl;
	cout << "---------------- APSERVICECATEGORY OBJECT -------------------" << endl;
	cout << "rdn = " << apServiceCategory->rdn << endl;
	cout << "apServiceCategoryId = " << apServiceCategory->axeApServiceCategoryId << endl;

	set<ACS_CS_ImApService *>::iterator it;
	for (it = apServiceCategory->apServices.begin(); it != apServiceCategory->apServices.end(); it++)
		printApServiceObject(*it);

	cout << "------------------------------------------------" << endl << endl;

}


void ACS_CS_ImUtils::printApServiceObject(ACS_CS_ImApService *apService)
{
	if (apService == 0)
		return;

	cout << endl;
	cout << "---------------- APSERVICE OBJECT -------------------" << endl;
	cout << "rdn = " << apService->rdn << endl;
	cout << "serviceName = " << apService->serviceName << endl;
	cout << "domainName = " << apService->domainName << endl;
	cout << "apServiceId = " << apService->apServiceId << endl;

	set<string>::iterator strIt;
	set<ACS_CS_ImAp *>::iterator apIt;

	for (strIt = apService->apRdns.begin(); strIt != apService->apRdns.end(); strIt++)
		cout << *strIt << endl;

	for (apIt = apService->aps.begin(); apIt != apService->aps.end(); apIt++)
		printApObject(*apIt);

	cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printApBladeObject(ACS_CS_ImApBlade *apBlade)
{

	if (apBlade == 0)
		return;

	printBladeParameters(apBlade);

	cout << "---------------- APBLADE OBJECT -------------------" << endl;
	cout << "axeApBladeId = " << apBlade->axeApBladeId << endl;


	cout << "------------------------------------------------" << endl << endl;
}


void ACS_CS_ImUtils::printApObject(ACS_CS_ImAp *ap)
{
	if (ap == 0)
		return;


	cout << endl;
	cout << "---------------- AP OBJECT -------------------" << endl;
	cout << "rdn = " << ap->rdn << endl;
	cout << "apId = " << ap->axeApgId << endl;
	//cout << "name = " << ap->name << endl;
	cout << "entryId = " << ap->entryId << endl;
	cout << "------------------------------------------------" << endl << endl;
}



void ACS_CS_ImUtils::printAdvancedConfigurationObject(ACS_CS_ImAdvancedConfiguration *advancedConfiguration)
{
	if (advancedConfiguration == 0)
		return;

	cout << endl;
	cout << "---------------- ADVANCEDCONFIGURATION OBJECT -------------------" << endl;
	cout << "rdn = " << advancedConfiguration->rdn << endl;
	cout << "omProfileSupervisionPhase = " << advancedConfiguration->omProfileSupervisionPhase << endl;
	cout << "omProfileSupervisionTimeout = " << advancedConfiguration->omProfileSupervisionTimeout << endl;
	cout << "omProfileChangeTrigger = " << advancedConfiguration->omProfileChangeTrigger << endl;
	cout << "phaseSubscriberCountCpNotify = " << advancedConfiguration->phaseSubscriberCountCpNotify << endl;
	cout << "phaseSubscriberCountApNotify = " << advancedConfiguration->phaseSubscriberCountApNotify << endl;
	cout << "phaseSubscriberCountCommit = " << advancedConfiguration->phaseSubscriberCountCommit << endl;
	cout << "phaseSubscriberCountValidate = " << advancedConfiguration->phaseSubscriberCountValidate << endl;
	cout << "startupProfileSupervision = " << advancedConfiguration->startupProfileSupervision << endl;
	cout << "trafficLeaderCp = " << advancedConfiguration->trafficLeaderCp << endl;
	cout << "trafficIsolatedCp = " << advancedConfiguration->trafficIsolatedCp << endl;
	cout << "omProfileRequested = " << advancedConfiguration->omProfileRequested << endl;
	cout << "omProfilePhase = " << advancedConfiguration->omProfilePhase << endl;
	cout << "omProfileCurrent = " << advancedConfiguration->omProfileCurrent << endl;
	cout << "neSubscriberCount = " << advancedConfiguration->neSubscriberCount << endl;
	cout << "initiateUpdatePhaseChange = " << advancedConfiguration->initiateUpdatePhaseChange << endl;
	cout << "cancelProfileSupervision = " << advancedConfiguration->cancelProfileSupervision << endl;
	cout << "cancelClusterOpModeSupervision = " << advancedConfiguration->cancelClusterOpModeSupervision << endl;
	cout << "apzProfileRequested = " << advancedConfiguration->apzProfileRequested << endl;
	cout << "apzProfileQueued = " << advancedConfiguration->apzProfileQueued << endl;
	cout << "apzProfileCurrent = " << advancedConfiguration->apzProfileCurrent << endl;
	cout << "aptProfileRequested = " << advancedConfiguration->aptProfileRequested << endl;
	cout << "aptProfileQueued = " << advancedConfiguration->aptProfileQueued << endl;
	cout << "aptProfileCurrent = " << advancedConfiguration->aptProfileCurrent << endl;
	cout << "isTestEnvironment = " << advancedConfiguration->isTestEnvironment << endl;
	cout << "advancedConfigurationId = " << advancedConfiguration->axeAdvancedConfigurationId << endl;
	cout << "clusterAligned = " << advancedConfiguration->clusterAligned << endl;
	cout << "------------------------------------------------" << endl << endl;

}


void ACS_CS_ImUtils::printBladeParameters(ACS_CS_ImBlade *blade)
{
	if (blade == 0)
		return;

	cout << endl;
	cout << "---------------- BLADE OBJECT -------------------" << endl;
	cout << "rdn = " << blade->rdn << endl;
	cout << "systemType = " << blade->systemType << endl;
	cout << "systemNumber = " << blade->systemNumber << endl;
	cout << "slotNumber = " << blade->slotNumber << endl;
	cout << "side = " << blade->side << endl;
	cout << "sequenceNumber = " << blade->sequenceNumber << endl;
	cout << "macAddressEthB = " << blade->macAddressEthB << endl;
	cout << "macAddressEthA = " << blade->macAddressEthA << endl;
	cout << "ipAliasEthB = " << blade->ipAliasEthB << endl;
	cout << "ipAliasEthA = " << blade->ipAliasEthA << endl;
	cout << "ipAddressEthB = " << blade->ipAddressEthB << endl;
	cout << "ipAddressEthA = " << blade->ipAddressEthA << endl;
	cout << "functionalBoardName = " << (int) blade->functionalBoardName << endl;
	cout << "dhcpOption = " << blade->dhcpOption << endl;
	cout << "aliasNetmaskEthB = " << blade->aliasNetmaskEthB << endl;
	cout << "aliasNetmaskEthA = " << blade->aliasNetmaskEthA << endl;
	cout << "entryId = " << blade->entryId << endl;
	cout << "------------------------------------------------" << endl << endl;

}


void ACS_CS_ImUtils::printValuesDefinitionType(ACS_CC_ValuesDefinitionType &valuesDefinitionType)
{
	cout << "----------------------------------------------" << endl;
	cout << "attrName = " << valuesDefinitionType.attrName << endl;
	cout << "attrType = " << valuesDefinitionType.attrType << endl;
	cout << "attrValuesNum = " << valuesDefinitionType.attrValuesNum << endl;
	for (unsigned int i = 0; i < valuesDefinitionType.attrValuesNum; i++)
	{
		stringstream ss;

		if (valuesDefinitionType.attrType == ATTR_INT32T)
		{
			int value = *(reinterpret_cast<int *>(valuesDefinitionType.attrValues[i]));
			//         int high = value << 16;
			//         int low = value >> 16;
			ss << "value " << value << endl;
			//ss << "value " << value << " high (" << high << ") low (" << low << ")";
		}

		else if (valuesDefinitionType.attrType == ATTR_UINT32T)
		{
			unsigned int value = *(reinterpret_cast<unsigned int *>(valuesDefinitionType.attrValues[i]));
			//         unsigned int high = value << 16;
			//         unsigned int low = value >> 16;
			ss << "value " << value;
			//ss << "value " << value << " high (" << high << ") low (" << low << ")";
		}

		else if (valuesDefinitionType.attrType == ATTR_STRINGT || valuesDefinitionType.attrType == ATTR_NAMET)
			ss << reinterpret_cast<const char *>(valuesDefinitionType.attrValues[i]);

		cout << "attrValues " << ss.str() << endl;
	}
}

string ACS_CS_ImUtils::getIdValueFromRdn(const string & rdn)
{
	string rdnId = rdn;

	size_t index = rdnId.find_first_of(",");
	if (index != string::npos)
		rdnId = rdnId.substr(0, index);

	index = rdnId.find_first_of("=");
	if (index != string::npos)
		rdnId = rdnId.substr(index+1);

	return rdnId;
}

string ACS_CS_ImUtils::getIdAttributeFromClassName (const string & className) {

	map <string,string>* myMap = getClassMap();

	map<string, string>::iterator it;
	it = myMap->find(className);

	if (it != myMap->end()) {
		return it->second;
	} else
		return "";

}

map <string,string>* ACS_CS_ImUtils::getClassMap() {

	if (ClassMap.size() == 0) {
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_EQUIPMENT, ACS_CS_ImmMapper::ATTR_EQUIPMENT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_ADVANCED_CONF, ACS_CS_ImmMapper::ATTR_ADVANCED_CONF_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_APG, ACS_CS_ImmMapper::ATTR_APG_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_AP_BLADE, ACS_CS_ImmMapper::ATTR_AP_BLADE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_AP_SERVICE, ACS_CS_ImmMapper::ATTR_AP_SERVICE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_AP_SERVICE_CATEGORY, ACS_CS_ImmMapper::ATTR_AP_SERVICE_CATEGORY_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CLUSTER_CP, ACS_CS_ImmMapper::ATTR_CLUSTER_CP_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CP_BLADE, ACS_CS_ImmMapper::ATTR_CP_BLADE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CP_CLUSTER, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP, ACS_CS_ImmMapper::ATTR_DUAL_SIDED_CP_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_HARDWARE, ACS_CS_ImmMapper::ATTR_HARDWARE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_LOGICAL, ACS_CS_ImmMapper::ATTR_LOGICAL_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_OTHER_BLADE, ACS_CS_ImmMapper::ATTR_OTHER_BLADE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_SHELF, ACS_CS_ImmMapper::ATTR_SHELF_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_VLAN, ACS_CS_ImmMapper::ATTR_VLAN_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_VLAN_CATEGORY, ACS_CS_ImmMapper::ATTR_VLAN_CATEGORY_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_OM_PROFILE, ACS_CS_ImmMapper::ATTR_OM_PROFILE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CCFILE, ACS_CS_ImmMapper::ATTR_CCFILE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER, ACS_CS_ImmMapper::ATTR_OM_PROFILE_MANAGER_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER, ACS_CS_ImmMapper::ATTR_CCFILE_MANAGER_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE, ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO, ACS_CS_ImmMapper::ATTR_CP_PRODUCT_INFO_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CRMGMT, ACS_CS_ImmMapper::ATTR_CRM_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE, ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK, ACS_CS_ImmMapper::ATTR_COMPUTE_RESOURCE_NETWORK_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_IPLB_CLUSTER, ACS_CS_ImmMapper::ATTR_IPLB_CLUSTER_ID));

		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_FUNCTION_DIST, ACS_CS_ImmMapper::ATTR_FUNCTION_DIST_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_APGNODE, ACS_CS_ImmMapper::ATTR_APGNODE_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_FUNCTION, ACS_CS_ImmMapper::ATTR_FUNCTION_ID));

		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CRM_EQUIPMENT, ACS_CS_ImmMapper::ATTR_CRM_EQUIPMENT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_CRM_COMPUTE_RESOURCE, ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID));

		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_TRM_TRANSPORT, ACS_CS_ImmMapper::ATTR_TRM_TRANSPORT_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_TRM_EXTERNALNETWORK, ACS_CS_ImmMapper::ATTR_TRM_EXTERNALNETWORK_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_TRM_INTERNALNETWORK, ACS_CS_ImmMapper::ATTR_TRM_INTERNALNETWORK_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_TRM_HOST, ACS_CS_ImmMapper::ATTR_TRM_HOST_ID));
		ClassMap.insert(pair<string,string> (ACS_CS_ImmMapper::CLASS_TRM_INTERFACE, ACS_CS_ImmMapper::ATTR_TRM_INTERFACE_ID));


	}

	return &ClassMap;
}


string ACS_CS_ImUtils::getParentName(const string & rdn)
{
	string parentRdn;
	size_t index = rdn.find_first_of(",");
	if (index != string::npos)
		parentRdn = rdn.substr(index + 1);
	return parentRdn;

}

string ACS_CS_ImUtils::getParentName(const ACS_CS_ImBase * child)
{
	size_t index = child->rdn.find_first_of(",");
	string parentRdn;
	if (index != string::npos)
		parentRdn = child->rdn.substr(index + 1);
	return parentRdn;
}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createStringType(const char* attrName, const string &value)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 1;
	void* values[SIZE];

	const char *cStrVal = value.c_str();
	//   char *valueArr = new char[strlen(cStrVal) + 1];
	//   strcpy(valueArr, cStrVal);

	void *voidVal = reinterpret_cast<void *>(const_cast<char *>(cStrVal));
	values[0] = voidVal;
	//std::cout<<"__FUNCTION__9: " << __FUNCTION__ << "OK!"<<std::endl;
	attr.setAttrValues(values, SIZE, ATTR_STRINGT);
	return attr;

}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createEmptyStringType(const char* attrName)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 0;

	attr.setAttrValues(0, SIZE, ATTR_STRINGT);
	return attr;

}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createEmptyNameType(const char* attrName)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 0;

	attr.setAttrValues(0, SIZE, ATTR_NAMET);
	return attr;

}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createNameType(const char * attrName, const string &value)
{

	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 1;
	void* values[SIZE];

	const char *cStrVal = value.c_str();
	//   char *valueArr = new char[strlen(cStrVal) + 1];
	//   strcpy(valueArr, cStrVal);

	void *voidVal = reinterpret_cast<void *>(const_cast<char *>(cStrVal));
	values[0] = voidVal;
	//std::cout<<"__FUNCTION__8: " << __FUNCTION__ << "OK!"<<std::endl;
	attr.setAttrValues(values, SIZE, ATTR_NAMET);
	return attr;

}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createNameType(const char* attrName, const std::set<string> &strings)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	int count = strings.size();

	void* values[count];

	std::set<string>::const_iterator it;
	uint32_t i = 0;

	//   for (it = strings.begin(); it != strings.end(); it++)
	//   {
	//      string str = *it;
	//      const char *value = (*it).c_str();
	//      char *valueArr = new char[strlen(value) + 1];
	//      strcpy(valueArr, value);
	//
	//      void *voidVal = reinterpret_cast<void *>(const_cast<char *>(value));
	//      values[i++] = voidVal;
	//   }

	for (it = strings.begin(); it != strings.end(); it++)
		values[i++] = reinterpret_cast<void *>(const_cast<char *>(it->c_str()));


	//std::cout<<"__FUNCTION__6: " << __FUNCTION__ << "OK!"<<std::endl;
	attr.setAttrValues(values, count, ATTR_NAMET);
	return attr;
}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createMultipleIntType(const char* attrName, std::set<int> integers)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	int count = integers.size();

	void* values[count];

	std::set<int>::iterator it;
	uint32_t i = 0;

	//   for (it = integers.begin(); it != integers.end(); it++)
	//   {
	//      int instr = *it;
	//
	////      int *valueArr = new int(1);
	////      *valueArr=instr;
	//
	//      void *voidVal = reinterpret_cast<void *>(&instr);
	//      values[i++] = voidVal;
	//   }

	for (it = integers.begin(); it != integers.end(); it++)
		values[i++] = reinterpret_cast<void *>(*it);


	//std::cout<<"__FUNCTION__5: " << __FUNCTION__ << "OK!"<<std::endl;
	attr.setAttrValues(values, count, ATTR_INT32T);

	return attr;
}



ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createMultipleStringType(const char* attrName, const std::set<string> & strings)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	int count = strings.size();

	void* values[count];

	std::set<string>::iterator it;
	uint32_t i = 0;

	//   for (it = strings.begin(); it != strings.end(); it++)
	//   {
	//      string str = *it;
	//      const char *value = str.c_str();
	//   //      char *valueArr = new char[strlen(value) + 1];
	//   //      strcpy(valueArr, value);
	//
	//      values[i++] = reinterpret_cast<void *>(const_cast<char *>(value));
	//   }

	for (it = strings.begin(); it != strings.end(); it++)
		values[i++] = reinterpret_cast<void *>(const_cast<char *>(it->c_str()));


	//std::cout<<"__FUNCTION__4: " << __FUNCTION__ << "OK!"<<std::endl;
	attr.setAttrValues(values, count, ATTR_STRINGT);

	return attr;
}



ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createUIntType(const char * attrName, unsigned int &value)
{

	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	uint16_t SIZE = 1;
	void * values[1]= {0};

	//   unsigned int *pUI = new unsigned int;
	//   memset(pUI, 0, sizeof(unsigned int));
	//   *pUI = value;
	values[0] = reinterpret_cast<void *>(&value);

	attr.setAttrValues(values, SIZE, ATTR_UINT32T);

	//std::cout<<"__FUNCTION__: " << __FUNCTION__ << "OK!"<<std::endl;

	return attr;
}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createUIntType(const char * attrName, uint16_t &value)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	uint16_t SIZE = 1;
	void* values[1]= {0};

	//   unsigned int *pUI = new unsigned int;
	//   memset(pUI, 0, sizeof(unsigned int));
	//   *pUI = value;
	uint32_t temp_Value = value;
	values[0] = reinterpret_cast<void *>(&temp_Value);

	attr.setAttrValues(values, SIZE, ATTR_UINT32T);

	//std::cout<<"__FUNCTION__2: " << __FUNCTION__ << "OK!"<<std::endl;

	return attr;

	//   return ACS_CS_ImUtils::createUIntType(attrName, (unsigned int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createUIntType(const char * attrName, OgClearCode &value)
{
	return ACS_CS_ImUtils::createUIntType(attrName, (unsigned int &) value);
}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, int &value)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 1;
	void* values[1]= {0};

	//   int *pI = new int;
	//   memset(pI, 0, sizeof(int));
	//   *pI = value;
	values[0] = reinterpret_cast<void *>(&value);

	attr.setAttrValues(values, SIZE, ATTR_INT32T);

	//std::cout<<"__FUNCTION__3: " << __FUNCTION__ << "OK!"<<std::endl;
	return attr;
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createEmptyIntType(const char * attrName)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 0;

	attr.setAttrValues(0, SIZE, ATTR_INT32T);

	return attr;
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createEmptyUIntType(const char * attrName)
{
	ACS_CS_ImValuesDefinitionType attr;

	attr.setAttrName(const_cast<char *>(attrName));

	unsigned int SIZE = 0;

	attr.setAttrValues(0, SIZE, ATTR_UINT32T);

	return attr;
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ApNameEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, uint16_t &value)
{
	int val = value;
	return ACS_CS_ImUtils::createIntType(attrName, val);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ProfileChangeTriggerEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, AdminState &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, bool &value)
{
	int valueInt = 0;

	if (value == true)
		valueInt=1;

	return ACS_CS_ImUtils::createIntType(attrName, (int &) valueInt);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, int16_t &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

//ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createMultiIntType(const char * attrName, int16_t **value)
//{
//   return ACS_CS_ImUtils::createMultiIntType(attrName, (int &) value);
//}
//
//ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createMultiStringType(const char * attrName, string **value)
//{
//   return ACS_CS_ImUtils::createMultiIntType(attrName, (int &) value);
//}


ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, OmProfilePhaseEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ClusterOpModeEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ClusterOpModeTypeEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, AptTypeEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, VlanStackEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, SystemTypeEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, SideEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, FunctionalBoardNameEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, DhcpEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, OgClearCode &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, AsyncActionStateType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, AsyncActionType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ActionResultType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, OmProfileStateType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, CcFileStateType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, ProfileScopeType &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, VlanTypeEnum &value)
{
	return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

ACS_CS_ImValuesDefinitionType ACS_CS_ImUtils::createIntType(const char * attrName, RoleType &value)
{
   return ACS_CS_ImUtils::createIntType(attrName, (int &) value);
}

bool ACS_CS_ImUtils::getImmAttributeString (std::string object, std::string attribute, std::string &value)
{
	bool res = true;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
	}
	else
	{
		ACS_CC_ImmParameter Param;
		//Param.ACS_APGCC_IMMFreeMemory(1);
		char *name_attrPath = const_cast<char*>(attribute.c_str());
		Param.attrName = name_attrPath;
		result = omHandler.getAttribute(object.c_str(), &Param );
		if ( result != ACS_CC_SUCCESS )
		{
			res = false;
		}
		else if (Param.attrValuesNum > 0)
		{
			value = (char*)Param.attrValues[0];
		}
		else
		{
			res = false;
		}


		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}
	}

	return res;
}


bool ACS_CS_ImUtils::getImmAttributeInt(std::string object, std::string attribute, int &value)
{
	bool res = true;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
	}
	else
	{
		ACS_CC_ImmParameter Param;
		//Param.ACS_APGCC_IMMFreeMemory(1);
		char *name_attrPath = const_cast<char*>(attribute.c_str());
		Param.attrName = name_attrPath;
		result = omHandler.getAttribute(object.c_str(), &Param );
		if ( result != ACS_CC_SUCCESS )
		{
			res = false;
		}
		else if (Param.attrValuesNum > 0)
			value = (*(int*)Param.attrValues[0]);
		else
			res = false;


		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}
	}

	return res;
}

int ACS_CS_ImUtils::readAPZType()
{
	int returnValue = 4; //Set default to APZ21255

	acs_apgcc_paramhandling acsParamHandling;
	acsParamHandling.getParameter(ACS_CS_ImmMapper::RDN_APZFUNCTIONS, ACS_CS_ImmMapper::ATTR_APZ_PROTOCOL_TYPE, &returnValue);

	return returnValue;
}

string ACS_CS_ImUtils::readDHCPToggle()
{
 	return ACS_CS_ImUtils::getToggle(ACS_CS_ImmMapper::AXEINFO_DHCP_OBJECT); 
}

string ACS_CS_ImUtils::getToggle(std::string dn)
{
	std::string toggleValue;
	if(!ACS_CS_ImUtils::getImmAttributeString(dn,"value",toggleValue))
	{
		ACS_CS_TRACE((ACS_CS_ImUtils_TRACE, "error in fetching value of dn: <%s>", dn.c_str()));
	}

	return toggleValue;
	
}

void ACS_CS_ImUtils::createStaticIpEntry(std::string &magazine, uint16_t & slot, std::string &ipA, std::string &ipB, std::string & entry)
{
	//format: magazine_slot_ipEthA_ipEthB
	char tmp[512] = {0};
	snprintf(tmp, sizeof(tmp) - 1, "%s_%u_%s_%s", magazine.c_str(), slot, ipA.c_str(), ipB.c_str());
	entry = tmp;
}

bool ACS_CS_ImUtils::fetchDataFromStaticIpEntry(std::string &magazine, uint16_t & slot, std::string &ipA, std::string &ipB, std::string & entry/*, std::string &ipAliasA, std::string &ipAliasB*/)
{
	//format: magazine_slot_ipEthA_ipEthB

	std::stringstream ss(entry);
	std::string item;
	char delim = '_';
	std::vector<std::string> elems;
	while(std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	if (elems.size() == ACS_CS_ImmMapper::STATICIP_ENTRY_SIZE)
	{
		magazine = elems[0];
		slot = atoi(elems[1].c_str());
		ipA= elems[2];
		ipB= elems[3];
		return true;
	}
	else
	{
		return false;
	}

}

void ACS_CS_ImUtils::printCpClusterStructObject(ACS_CS_ImCpClusterStruct *bladeClusterStructInfo)
{
	if (bladeClusterStructInfo == 0)
		return;

	cout << endl;
	cout << "---------------- BLADECLUSTERSTRUCTINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << bladeClusterStructInfo->rdn << endl;
	cout << "bladeClusterInfoStructId = " << bladeClusterStructInfo->axeCpClusterStructId << endl;
	cout << "state = " << bladeClusterStructInfo->state << endl;
	cout << "reason = " << bladeClusterStructInfo->reason << endl;
	cout << "result = " << bladeClusterStructInfo->result << endl;
	cout << "actionId = " << bladeClusterStructInfo->actionId << endl;
	cout << "timeOfLastAction = " << bladeClusterStructInfo->timeOfLastAction << endl;
	cout << "------------------------------------------------" << endl << endl;
}

bool ACS_CS_ImUtils::getNbiCcFile(std::string & completeFolderPath)
{
	bool result = false;
	completeFolderPath.clear();
	char * p_finaloutputdir = new char[100];
	int len=100;
	ACS_APGCC_CommonLib apgccCommon;
	ACS_APGCC_DNFPath_ReturnTypeT retcode;
	const char * p_FileMFuncName =  ACS_CS_ImmMapper::ATTR_CCF_FOLDER.c_str();


	retcode = apgccCommon.GetFileMPath(p_FileMFuncName,p_finaloutputdir,len);
	if( retcode == ACS_APGCC_DNFPATH_SUCCESS)
	{
		completeFolderPath = p_finaloutputdir;
		result = true;
	}

	delete[] p_finaloutputdir;
	return result;
}


void ACS_CS_ImUtils::printOmProfileObject(ACS_CS_ImOmProfile *omProfileInfo)
{
	if (omProfileInfo == 0)
		return;

	cout << endl;
	cout << "---------------- OMPROFILEINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << omProfileInfo->rdn << endl;
	cout << "omProfileId = " << omProfileInfo->omProfileId << endl;
	cout << "state = " << omProfileInfo->state << endl;
	cout << "apzProfile = " << omProfileInfo->apzProfile << endl;
	cout << "aptProfile = " << omProfileInfo->aptProfile << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printCcFileObject(ACS_CS_ImCcFile *ccFileInfo)
{
	if (ccFileInfo == 0)
		return;

	cout << endl;
	cout << "---------------- CCFILEINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << ccFileInfo->rdn << endl;
	cout << "ccFileId = " << ccFileInfo->ccFileId << endl;
	cout << "state = " << ccFileInfo->state << endl;
	cout << "rulesVersion = " << ccFileInfo->rulesVersion << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printOmProfileManagerObject(ACS_CS_ImOmProfileManager *omProfileMgrInfo)
{
	if (omProfileMgrInfo == 0)
		return;

	cout << endl;
	cout << "---------------- OMPROFILEMANAGERINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << omProfileMgrInfo->rdn << endl;
	cout << "omProfileManagerId = " << omProfileMgrInfo->omProfileManagerId << endl;
	cout << "activeCcFile = " << omProfileMgrInfo->activeCcFile << endl;
	cout << "omProfile = " << omProfileMgrInfo->omProfile << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printCcFileManagerObject(ACS_CS_ImCcFileManager *ccFileMagerInfo)
{
	if (ccFileMagerInfo == 0)
		return;

	cout << endl;
	cout << "---------------- CCFILEINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << ccFileMagerInfo->rdn << endl;
	cout << "ccFileManagerId = " << ccFileMagerInfo->ccFileManagerId << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printCandidateCcFileObject(ACS_CS_ImCandidateCcFile *candidateCcFileInfo)
{
	if (candidateCcFileInfo == 0)
		return;

	cout << endl;
	cout << "---------------- CCFILEINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << candidateCcFileInfo->rdn << endl;
	cout << "candidateCcFileId = " << candidateCcFileInfo->candidateCcFileId << endl;
	cout << "------------------------------------------------" << endl << endl;
}

void ACS_CS_ImUtils::printCpProductInfoObject(ACS_CS_ImCpProductInfo* info)
{
	if (info == 0)
		return;

	cout << endl;
	cout << "---------------- CPPRODUCTINFO OBJECT ---------------------" << endl;
	cout << "rdn = " << info->rdn << endl;
	cout << "------------------------------------------------" << endl << endl;
}

bool ACS_CS_ImUtils::isAllowedAliasName(std::string aliasName)
{
	if (aliasName.length() > 7)
		return false;

	std::string validChars = "";    //string to hold valid characters

	for (int i = 'A'; i <= 'Z'; i++)      //add chars "A-Z"
	{
		validChars = validChars + char(i);
	}

	for (int i = 'a'; i <= 'z'; i++)         //add chars "a-z"
	{
		validChars = validChars + char(i);
	}

	size_t num = strspn((aliasName.substr(0,1)).c_str(), validChars.c_str());   //check if first char in str 1 match chars from str 2
	if (num != 1)     //check if string 1 doesn't start with a letter
		return false;

	return true;
}

bool ACS_CS_ImUtils::isReservedCPName (std::string cpName)
{
	std::stringstream addStringStream;
	std::set<string> invalidNameSet;
	ACS_APGCC::toUpper(cpName);

	//Add default CP names to the SET (BC0-BC999 and CP0-CP999)
	for (int i = 0; i < 1000; i++)
	{
		addStringStream.str("");   //empty the stringstream
		addStringStream<<"BC"<<i;
		invalidNameSet.insert(addStringStream.str());   //add "BCn" to SET (n = 0 - 999)
		addStringStream.str("");   //empty the stringstream
		addStringStream<<"CP"<<i;
		invalidNameSet.insert(addStringStream.str());   //add "CPn" to SET (n = 0 - 999)
	}

	size_t stringCount = invalidNameSet.count(cpName);   //check if given name is a default CP name

	if (stringCount == 1)      //check if given name was found
		return true;
	else
		return false;
}

bool ACS_CS_ImUtils::isValidAPZSystem(std::string apzSys)
{
	std::stringstream trim;
	trim << apzSys;
	trim >> apzSys;
	apzSys = trim.str();
	if(apzSys.length() == 8)
		apzSys = apzSys.substr(3);

	int apzSystem = atoi(apzSys.c_str());

	if (  ( (apzSystem >= 21200) && (apzSystem <= 21299) )
			|| ( (apzSystem >= 21400) && (apzSystem <= 21499) )
			|| ( apzSystem == 0) )
		return true;
	else
		return false;
}

bool ACS_CS_ImUtils::isValidCPType(uint16_t type)
{
	if (  ( (type >= 21200) && (type <= 21299) )
			|| ( type == 0 ) )
		return true;
	else
		return false;
}


std::string ACS_CS_ImUtils::getRoleLabel(const int& type, const int& side, const int& seqNo)
{
	std::string delim = (seqNo >= 0)? ACS_APGCC::itoa(seqNo): "";
	return getCrTypeLabel(type) + delim + getSideLabel(side);
}

std::string ACS_CS_ImUtils::getSideLabel (const int& side)
{
	switch (side)
	{
	case A:
		return ACS_CS_ImmMapper::SIDE_A;
	case B:
		return ACS_CS_ImmMapper::SIDE_B;
	default:
		return "";
	}
}

std::string ACS_CS_ImUtils::getCrTypeLabel(const int& type)
{
	switch (type)
	{
	case SINGLE_SIDED_CP:
		return ACS_CS_ImmMapper::BC;
	case DOUBLE_SIDED_CP:
		return ACS_CS_ImmMapper::CP;
	case AP:
		return ACS_CS_ImmMapper::AP;
	default:
		return ACS_CS_ImmMapper::IPLB;
	}
}

void ACS_CS_ImUtils::setFaultyAttribute(const int value)
{
	char val[32] = {0};
	sprintf(val,"%d",value);

	std::string strVal(val);

	setFaultyAttribute(strVal);
}

void ACS_CS_ImUtils::setFaultyAttribute(const std::string& value)
{
	faultyValue = value;
}
void ACS_CS_ImUtils::setFaultyFbn(const int fbn)
{
	std::string strVal("");
	switch(fbn)
	{
	case SCB_RP:
		strVal = "SCB_RP";
		break;
	case GESB:
		strVal = "GESB";
		break;
	case SCXB:
		strVal = "SCXB";
		break;
	case IPTB:
		strVal = "IPTB";
		break;
	case EPB1:
		strVal = "EPB1";
		break;
	case EVOET:
		strVal = "EVOET";
		break;
	case CMXB:
		strVal = "CMXB";
		break;
	case CPUB:
		strVal = "CPUB";
		break;
	case MAUB:
		strVal = "MAUB";
		break;
	case RPBI_S:
		strVal = "RPBI_S";
		break;
	case APUB:
		strVal = "APUB";
		break;
	case DISK:
		strVal = "DISK";
		break;
	case DVD:
		strVal = "DVD";
		break;
	case GEA:
		strVal = "GEA";
		break;
	case IPLB:
		strVal = "IPLB";
		break;
	case SMXB:
		strVal = "SMXB";
		break;
	}

	setFaultyAttribute(strVal);
}
void ACS_CS_ImUtils::setFaultyCpArchitecture(const int value)
{
	std::string strVal("");
	switch(value)
	{
	case SINGLE_SIDED_CP:
		strVal = "SINGLE_SIDED_CP";
		break;
	case DOUBLE_SIDED_CP:
		strVal = "DOUBLE_SIDED_CP";
		break;
	case AP:
		strVal = "AP";
		break;
	}

	setFaultyAttribute(strVal);
}

ACS_APGCC_AdminOperationParamType ACS_CS_ImUtils::createAdminOperationErrorParam(std::string errorText)
{
	ACS_APGCC_AdminOperationParamType errorTextElem;
	errorTextElem.attrType = ATTR_STRINGT;
	char *attrNameText = new char[10];
	memset(attrNameText,0,10);
	memcpy(attrNameText,"errorText",10);
	errorTextElem.attrName=attrNameText;

	string errText ="@ComNbi@"+ errorText;
	int len = strlen(errText.c_str());
	char* errValue = new char[len+1];
	memset(errValue,0,len+1);
	memcpy(errValue,errText.c_str(),len+1);
	errorTextElem.attrValues = reinterpret_cast<void*>(errValue);

	return errorTextElem;
}

bool ACS_CS_ImUtils::getObject(std::string dn, ACS_APGCC_ImmObject *object)
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;
	char *nameObject =  const_cast<char*>(dn.c_str());
	object->objName = nameObject;


	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		result = omHandler.getObject(object);
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error getObject()" << std::endl;
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool ACS_CS_ImUtils::createImmObject(const char *p_className, const char *parent, vector<ACS_CC_ValuesDefinitionType> AttrList)
{
	bool res = false;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	char* nameClasse = const_cast<char*>(p_className);
	char* parentName = const_cast<char*>(parent);

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		int id = -1;
		char* text;
		result = omHandler.createObject(nameClasse, parentName, AttrList);
		id = omHandler.getInternalLastError();
		text = omHandler.getInternalLastErrorText();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error createObject()" << std::endl;
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Into Class : "<<  p_className << std::endl;
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "From Parent Object: "<<  parent << std::endl;
			cout<<"   	ErrorCode: "<< id <<" TEXT: "<< text  << endl;
			res = false;
		}
		else res = true;

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

bool ACS_CS_ImUtils::deleteImmObject(const char *dn)
{
	bool res = false;
	ACS_CC_ReturnType result;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Init()" << std::endl;
		res = false;
	}
	else
	{
		int id = -1;
		char* text;
		result = omHandler.deleteObject(dn);
		id = omHandler.getInternalLastError();
		text = omHandler.getInternalLastErrorText();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error while deleting the object" << std::endl;
			cout<<"     ErrorCode: "<< id <<" TEXT: "<< text  << endl;
			res = false;
		}
		else
		{
			res = true;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			std::cout << "DBG: " << __FUNCTION__ << "@" << __LINE__ << "Error Finalize()" << std::endl;
			res = false;
		}
	}

	return res;
}

ACS_CC_ValuesDefinitionType ACS_CS_ImUtils::defineAttributeString(const char* attributeName, ACS_CC_AttrValueType type, const char *value, unsigned int numValue)
{
	ACS_CC_ValuesDefinitionType attribute;
	char* stringValue = const_cast<char *>(value);
	attribute.attrName = const_cast<char*>(attributeName);
	attribute.attrType = type;
	attribute.attrValuesNum = numValue;

	if (attribute.attrValuesNum == 0)	attribute.attrValues = 0;
	else
	{
		attribute.attrValues=new void*[attribute.attrValuesNum];
		attribute.attrValues[0] =reinterpret_cast<void*>(stringValue);
	}

	return attribute;
}

ACS_CC_ValuesDefinitionType ACS_CS_ImUtils::defineAttributeInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
{
	ACS_CC_ValuesDefinitionType attribute;

	attribute.attrName = const_cast<char*>(attributeName);
	attribute.attrType = type;
	attribute.attrValuesNum = numValue;

	if (attribute.attrValuesNum == 0)	attribute.attrValues = 0;
	else
	{
		attribute.attrValues=new void*[attribute.attrValuesNum];
		attribute.attrValues[0] =reinterpret_cast<void*>(value);
	}


	return attribute;
}

bool ACS_CS_ImUtils::modifyImmAttribute(const char *object, ACS_CC_ImmParameter parameter)
{
	ACS_CC_ReturnType result;
	bool res = true;
	OmHandler omHandler;

	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
	}
	else
	{
		result = omHandler.modifyAttribute(object,&parameter);
		if (result != ACS_CC_SUCCESS)
		{
			int id = 0;
			char * text;
			id = omHandler.getInternalLastError();
			text = omHandler.getInternalLastErrorText();
			res = false;
		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
		}
	}

	return res;
}

ACS_CC_ImmParameter ACS_CS_ImUtils::defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
{
	ACS_CC_ImmParameter parToModify;

	char *name_attrUnsInt32 = const_cast<char*>(attributeName);
	parToModify.attrName = name_attrUnsInt32;
	parToModify.attrType = type;
	parToModify.attrValuesNum = numValue;

	if (parToModify.attrValuesNum == 0)	parToModify.attrValues = 0;
	else
	{
		parToModify.attrValues=new void*[parToModify.attrValuesNum];
		parToModify.attrValues[0] =reinterpret_cast<void*>(value);
	}

return parToModify;

}

ACS_CC_ImmParameter ACS_CS_ImUtils::defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue)
{
	ACS_CC_ImmParameter parToModify;

	char* stringValue = const_cast<char *>(value);
	char *name_attrString = const_cast<char*>(attributeName);
	parToModify.attrName = name_attrString;
	parToModify.attrType = type;
	parToModify.attrValuesNum = numValue;

	if (parToModify.attrValuesNum == 0)	parToModify.attrValues = 0;
	else
	{
		parToModify.attrValues=new void*[parToModify.attrValuesNum];
		parToModify.attrValues[0] =reinterpret_cast<void*>(stringValue);
	}


return parToModify;

}
