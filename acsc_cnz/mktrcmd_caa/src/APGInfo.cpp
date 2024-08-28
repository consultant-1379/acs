#include "APGInfo.h"
#include "ace/ACE.h"
#include <acs_prc_api.h>
#include <ACS_DSD_Server.h>
#include "ACS_APGCC_CommonLib.h"
#include "acs_apgcc_paramhandling.h"

#define HWVER_LENGTH 64

MktrCommonDefs::ApgShelfArchitectureType APGInfo::_apg_shelf_arch_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_UNKNOWN;

enum IMM_ApzType
{
	IMM_DEF_APZ_212_3X_SHDC = 0,
	IMM_DEF_APZ_212_33 = 1,
	IMM_DEF_APZ_212_40 = 2,
	IMM_DEF_APZ_212_50 = 3,
	IMM_DEF_APZ_212_55_OR_LATER = 4
};

enum IMM_ApgShelfArchitecture
{
	IMM_DEF_SCB = 0,
	IMM_DEF_SCX = 1,
	IMM_DEF_DMX = 2,
	IMM_DEF_VIRTUALIZED = 3,
	IMM_DEF_SMX = 4
};


const char * apgShelfArchitectureType_descr(MktrCommonDefs::ApgShelfArchitectureType apgShelfArchType)
{
	switch(apgShelfArchType)
	{
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCB:
			return "SCB";
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCX:
			return "SCX";
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_DMX:
			return "DMX";
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED:
			return "VIRTUALIZED";
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX:
			return "SMX";
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_UNKNOWN:
		case MktrCommonDefs::APG_SHELF_ARCHITECTURE_NONE:
		default:
		    return "UNKNOWN";
	}
}

const char * apzType_descr(MktrCommonDefs::ApzType apzType)
{
	switch(apzType)
	{
		case MktrCommonDefs::APZ_212_3X_SHDC:
			return "APZ_212_3X_SHDC";
		case MktrCommonDefs::APZ_212_33:
			return "APZ_212_33";
		case MktrCommonDefs::APZ_212_40:
			return "APZ_212_40";
		case MktrCommonDefs::APZ_212_50:
			return "APZ_212_50";
		case MktrCommonDefs::APZ_212_55_OR_LATER:
			return "APZ_212_55_OR_LATER";
		case MktrCommonDefs::APZ_NONE:
		case MktrCommonDefs::APZ_UNKNOWN:
		default:
			return "UNKNOWN";
	}
}




APGInfo::APGInfo()
{
	char hostName[1024];
	size_t len = 1024;

	if(ACE_OS::hostname(hostName, len) < 0)
		hostName[0] = '\0';
	else
		_retrieveAPType = hostName;

	_nodeStatus = MktrCommonDefs::NODE_NONE;
	_apType = MktrCommonDefs::AP_NONE;
	_apgType = MktrCommonDefs::APG_NONE;
	_apgShelfArchitectureType = MktrCommonDefs::APG_SHELF_ARCHITECTURE_NONE;
	_apzType = MktrCommonDefs::APZ_NONE;
}

APGInfo::~APGInfo()
{
	// TODO Auto-generated destructor stub
}


int APGInfo::load()
{
	int i = 0;
	i |= _getAPGType();
	i |= _getAPType();
	i |= _getApgShelfArchitectureType();
	i |= _getApzType();
	i |= _getNodeStatus();
	i |= _getApgHwGepType();

	return (i == 0) ? 0 : -1;

}


int APGInfo::get_shelf_architecture_type(MktrCommonDefs::ApgShelfArchitectureType & apg_shelf_architecture_type)
{
	// check if we yet already successfully calculated the APG Shelf Architecture Type
	if(_apg_shelf_arch_type != MktrCommonDefs::APG_SHELF_ARCHITECTURE_UNKNOWN) {
		apg_shelf_architecture_type = _apg_shelf_arch_type;
		return 0;
	}

	acs_apgcc_paramhandling PHA;
	int nodeArchitecture;
	OmHandler omManager;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS){
		return -1;
	}/*IMM init failure: generic error*/


	/* get the root object instance of HardwareInventory class
	 * if no obj found or more than one obj found return error: just one instance hardwareInventoryId=1 must be present
	 */
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances("AxeFunctions", p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
		return -2;
	}

	string class_instance_name = p_dnList[0];
	omManager.Finalize();

	result = PHA.getParameter(class_instance_name.c_str(), "apgShelfArchitecture", &nodeArchitecture);
	if (result != ACS_CC_SUCCESS) {
		return -3;
	}

	switch(nodeArchitecture)
	{
		case IMM_DEF_SCB :
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCB;
			break;
		case IMM_DEF_SCX :
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_SCX;
			break;
		case IMM_DEF_DMX :
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_DMX;
			break;
		case IMM_DEF_VIRTUALIZED :
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_VIRTUALIZED;
			break;
		case IMM_DEF_SMX :
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_SMX;
			break;
		default:
			apg_shelf_architecture_type = MktrCommonDefs::APG_SHELF_ARCHITECTURE_UNKNOWN;
	}

	// cache the value just calculated, in order not to repeat the calculus in next function invocations
	_apg_shelf_arch_type = apg_shelf_architecture_type;

	// success
	return 0;
}


int APGInfo::_getAPType()
{
/*
	FILE * 	file_read;
	char  	buffer[10];

	// Reset the On_Off element
	memset(buffer, 0, sizeof(buffer));

	// Open the file parameter and read the side value
	file_read = fopen ("/etc/cluster/nodes/this/id", "r");

	if (file_read == NULL)
	{
		printf("Unable to read the Side\n");
		return 1;
	}

	// Read the value
	fgets (buffer , sizeof(buffer) - 1, file_read);

	//close the file
	fclose (file_read);


	if(strncmp(buffer, "1", 1) == 0)
		_apType = MktrCommonDefs::AP_1;
	else if(strncmp(buffer, "2", 1) == 0)
		_apType = MktrCommonDefs::AP_2;
	else
		_apType = MktrCommonDefs::AP_NONE;
*/
	ACS_DSD_Server svr;
	ACS_DSD_Node node_info;
	int op_res = svr.get_local_node(node_info);
	if(op_res < 0)
	{
	//	printf("Unable to get AP info using DSD API. Call 'ACS_DSD_Server::get_local_node()' failed !. Error Text: <%s>\n", svr.last_error_text());
		_lastErrorDescr = "Call 'APGInfo::_getAPType() returned '-1'. Details: Call 'ACS_DSD_Server::get_local_node()' failed ! Error Text: ";
		_lastErrorDescr += svr.last_error_text();
		return -1;
	}

	int32_t ap_num = node_info.system_id - acs_dsd::CONFIG_AP_ID_SET_START;
	switch(ap_num)
	{
	case 1:
		_apType = MktrCommonDefs::AP_1;
		break;
	case 2:
		_apType = MktrCommonDefs::AP_2;
		break;
	default:
		printf("WARNING: unexpected AP type: AP System name == %s\n", node_info.system_name);
		_apType = MktrCommonDefs::AP_NONE;
	}

	return 0;
}


int APGInfo::_getApzType()
{
	acs_apgcc_paramhandling PHA;
	int apzProtocolType;
	OmHandler omManager;
	ACS_CC_ReturnType result;

	/*try initialize imm connection to IMM*/
	result = omManager.Init();

	if (result != ACS_CC_SUCCESS){
		_lastErrorDescr = "_getApzType() returned -1.";
		return -1;
	}/*IMM init failure: generic error*/


	/* get the root object instance of AxeFunctions class
	 * if no obj found or more than one obj found return error: just one instance axeFunctionsId=1 must be present
	 */
	std::vector<std::string> p_dnList;
	result = omManager.getClassInstances("AxeFunctions", p_dnList);
	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) {
		_lastErrorDescr = "_getApzType() returned -2.";
		return -2;
	}

	string class_instance_name = p_dnList[0];
	omManager.Finalize();

	result = PHA.getParameter(class_instance_name.c_str(), "apzProtocolType", & apzProtocolType);
	if (result != ACS_CC_SUCCESS) {
		_lastErrorDescr = "_getApzType() returned -3.";
		return -3;
	}

	switch(apzProtocolType)
	{
		case IMM_DEF_APZ_212_3X_SHDC :
			_apzType = MktrCommonDefs::APZ_212_3X_SHDC;
			break;
		case IMM_DEF_APZ_212_33 :
			_apzType = MktrCommonDefs::APZ_212_33;
			break;
		case IMM_DEF_APZ_212_40 :
			_apzType = MktrCommonDefs::APZ_212_40;
			break;
		case IMM_DEF_APZ_212_50 :
			_apzType = MktrCommonDefs::APZ_212_50;
			break;
		case IMM_DEF_APZ_212_55_OR_LATER :
			_apzType = MktrCommonDefs::APZ_212_55_OR_LATER;
			break;
		default:
			_apzType = MktrCommonDefs::APZ_UNKNOWN;
	}

	// success
	return 0;
}


int APGInfo::_getNodeStatus()
{
	ACS_PRC_API objNodeStatus;

	int node_state = objNodeStatus.askForNodeState();

	if(node_state == 1)
		_nodeStatus = MktrCommonDefs::NODE_ACTIVE;
	else if(node_state == 2)
		_nodeStatus = MktrCommonDefs::NODE_PASSIVE;
	else
		_nodeStatus = MktrCommonDefs::NODE_NONE;

	return 0;
}



int APGInfo::_getAPGType()
{
	this->_apgType = MktrCommonDefs::APG_APG43L;
	return 0;
}

int APGInfo::_getApgHwGepType(){

	ACS_APGCC_CommonLib getHWInfo_obj;

	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoResult;

	_apgHwGepType = MktrCommonDefs::APG_HW_GEP_UNKNOWN;

	getHWInfo_obj.GetHwInfo( &hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION );

	if (hwInfoResult.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){

		switch ( hwInfo.hwVersion ){
			case ACS_APGCC_HWVER_GEP1 :
			case ACS_APGCC_HWVER_GEP2 :
				_apgHwGepType = MktrCommonDefs::APG_HW_GEP_1_2;
				break;
			case ACS_APGCC_HWVER_GEP5 :
				_apgHwGepType = MktrCommonDefs::APG_HW_GEP_4_5;
				break;
			case ACS_APGCC_HWVER_GEP7 :
				_apgHwGepType = MktrCommonDefs::APG_HW_GEP_7_8;
				break;
			default :
				_apgHwGepType = MktrCommonDefs::APG_HW_GEP_UNKNOWN;
		}
	}

	return 0;
}
