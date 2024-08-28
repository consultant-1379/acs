#include "MktrCmdCsFiller.h"
#include <ACS_CS_API.h>
#include <ace/ACE.h>
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_paramhandling.h"

#include <iostream>
#include <string>
#include <string.h>

#define ACS_PHA_PARAM_RC_OK 1

// constants for Message_Store

const char * const 	IMM_ACA_RECORD_TRANSFER_ROOT_DN 			= "AxeDataRecorddataRecordMId=1";
const char * const 	IMM_ACA_MESSAGE_STORE_NAME_ATTR_NAME 		= "messageStoreName";
const int 			IMM_ACA_MESSAGE_STORE_NAME_ATTR_SIZE		= 1024;
const int 			CONFIG_IMM_DN_PATH_SIZE_MAX					= 512;


//typedef string ACS_CS_API_Name;
static  int ACS_PHA_PARAM_RC = 1;
static int s_counter;
class generic {
public:
	std::string name;
	generic(){};
};

class ACS_PHA_NamedTable
{
	public:

	ACS_PHA_NamedTable(const char*  uno, const char*  due){ACE_UNUSED_ARG (uno); ACE_UNUSED_ARG (due);};

	};

//typedef vector<ACAMS_Parameters> ACS_PHA_Parameter;
template< class T > class ACS_PHA_Parameter
  {
  public :
	ACS_PHA_Parameter(){} ;

	int get(ACS_PHA_NamedTable param){
		ACE_UNUSED_ARG(param);
		if (s_counter--)
		return ACS_PHA_PARAM_RC;
	    else
	    	return 0;
	};
    generic data(){generic aa; return aa;};
  } ;

using namespace std;

MktrCmdCsFiller::MktrCmdCsFiller(const std::string& cmd) : MktrCmdFiller(cmd) {
	msgStoreList = getMessageStoreList();
	ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultiCPSystem);

	if (isMultiCPSystem) {
		cpNameList = getCPNameList();
		dualCPList = getDoubleCpList();
	}
}

std::string MktrCmdCsFiller::getDefaultCPName(short id)
{
    std::string dname = "";
	if(isMultiCPSystem)
    {
		char *m_CPName = (char*)"";
    	CPID cpid(id);
		ACS_CS_API_Name CPName(m_CPName);
		ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getDefaultCPName(cpid, CPName);
		if (result == ACS_CS_API_NS::Result_Success)
        {
			char name[100] = {0};
			size_t length = CPName.length();
			CPName.getName(name, length);
			dname = name;
			return dname;
		}
	}
	return dname;
}

//CR66-Begin
std::list<std::string> MktrCmdCsFiller::getDoubleCpList()
{
	std::list<std::string> cpDbList;
    std::list<short> cpDbIdList = getCPIDList();
    for(std::list<short>::iterator it = cpDbIdList.begin(); it != cpDbIdList.end(); it++)
    {
    	short id = *it;
		if (id >= ACS_CS_API_HWC_NS::SysType_CP && id < ACS_CS_API_HWC_NS::SysType_AP)
		{
			std::string default_name = getDefaultCPName(id);
			cpDbList.push_back(default_name);
		}
	}

	return cpDbList;
}
//CR66-End

std::list<short> MktrCmdCsFiller::getCPIDList()
{
    std::list<short> cplist;
    if(isMultiCPSystem)
    {
    	ACS_CS_API_IdList idlist;
		ACS_CS_API_CP * CPNameVer = ACS_CS_API::createCPInstance();
		ACS_CS_API_NS::CS_API_Result result = CPNameVer->getCPList(idlist);
		ACS_CS_API::deleteCPInstance(CPNameVer);
		if (result == ACS_CS_API_NS::Result_Success)
        {
			for(unsigned int i = 0; i < idlist.size(); i++)
            {
                cplist.push_back(idlist[i]);
            }
		}
	}
	return cplist;
}

bool MktrCmdCsFiller::isCP(short id)
{
	return ((id >= 1000) && (id < 2000)) ? true : false;
}

bool MktrCmdCsFiller::isCP(const char * name)
{
	char tmp[10] = {0};
	if(!name)
    {
        return false;
    }
	if(strlen(name) < 3)
    {
        return false;
    }
	snprintf(tmp, 2, "%s", name);

	if (strcmp(tmp, "CP") == 0)
    {
        return true;
    }
	else
    {
        return false;
    }
}

std::list<std::string> MktrCmdCsFiller::getCPNameList()
{
    std::list<std::string> cpList;
    std::list<short> cpIdList = getCPIDList();
    for(std::list<short>::iterator it = cpIdList.begin(); it != cpIdList.end(); it++)
    {
		short id = *it;
        std::string default_name = getDefaultCPName(id);
		cpList.push_back(default_name);
	}

	return cpList;
}

//std::list<std::string> MktrCmdCsFiller::getMessageStoreList()
//{
//	s_counter = 4;
//	ACS_PHA_NamedTable msdTables(ACABIN_ID, "ACS_ACABIN_MessageStores");
//	ACS_PHA_Parameter<ACAMS_Parameters> tableValue;
//	std::list<std::string> msList;
//	std::string ms;

//	while((tableValue.get(msdTables)) == ACS_PHA_PARAM_RC_OK)
//	{
//		ms = tableValue.data().name;
//		msList.push_back(ms);
//	}


//	return msList;

	/*ACS_PHA_Parameter<ACAMS_Parameters> tableValue;
	std::list<std::string> msList;
	std::string ms;

    tableValue.initialize(p_name);

    ms = tableValue.data();
    msList.push_back(ms);

    return msList;*/
    /*if(retCode == ACS_PHA_PARAM_RC_ERROR)
    	cout << "error in search library" << endl;
    else if(retCode == ACS_PHA_PARAM_RC_NOT_FOUND)
    	cout << "parameter not found" << endl;
	else
    	cout << tableValue.keyIs() << ": " << tableValue.data() << endl;*/
//}

std::list<std::string> MktrCmdCsFiller::getMessageStoreList()
{
	std::list<std::string> msList;
	OmHandler om_handler;

	msList.clear();

	if ( om_handler.Init() )
	{
		// Error to init OmHandler
		//om_handler.getInternalLastErrorText();
		return msList;
	}

	// Set up the dn to reach the MessageStore instances
	char imm_dnName[CONFIG_IMM_DN_PATH_SIZE_MAX] = {0};
	snprintf(imm_dnName, CONFIG_IMM_DN_PATH_SIZE_MAX, IMM_ACA_RECORD_TRANSFER_ROOT_DN);

	// Fetch from IMM the message store objects
	std::vector<std::string> message_stores;
	if ( om_handler.getChildren(imm_dnName, ACS_APGCC_SUBLEVEL, &message_stores) )
	{
		// Error to retrive messageStore in IMM
		om_handler.Finalize();
		return msList;
	}

	acs_apgcc_paramhandling par_hdl;

	// Check if the given name is a message store name
	for ( std::vector<std::string>::iterator it = message_stores.begin(); it != message_stores.end(); it++ )
	{
		// First, get the Name attribute from IMM and check it with the given name
	//	char message_store_alias[IMM_ACA_MESSAGE_STORE_NAME_ATTR_SIZE] = {0};
	//	int call_result = par_hdl.getParameter(*it, IMM_ACA_MESSAGE_STORE_NAME_ATTR_NAME, (char(&)[])message_store_alias);
		char message_store_alias[SA_MAX_NAME_LENGTH + 1] = {0};
    int call_result = par_hdl.getParameter<SA_MAX_NAME_LENGTH + 1>(*it, IMM_ACA_MESSAGE_STORE_NAME_ATTR_NAME, message_store_alias);
		if ( call_result )
		{
			// Error to retrive messageStore alias
			om_handler.Finalize();
			msList.clear();
			return msList;
		}else{
			msList.push_back(string(message_store_alias));
		}

	}
	om_handler.Finalize();
	return msList;
}

std::list<std::string> MktrCmdCsFiller::combinations() {
	std::list<std::string> cmd_list;
	std::list<std::string> combinations_list;

	// FIRST: Preprocess the placeholder <msgstore>, that can be present in all configurations
	if (cmdLine.find("<msgstore>") != std::string::npos) {
		for (std::list<std::string>::iterator it = msgStoreList.begin(); it != msgStoreList.end(); it++) {
			std::string cmd = replace(cmdLine, "<msgstore>", *it);
			cmd_list.push_back(cmd);
		}
	}
	else {
		cmd_list.push_back(cmdLine);
	}

	// SECOND: Only in MCP configuration, handle the <dualcp> and <cpname>
	//         placeholders (that are mutually exclusive)
	if (isMultiCPSystem) {
		for (std::list<std::string>::iterator it = cmd_list.begin(); it != cmd_list.end(); it++) {
	  	std::string cmd = *it;

	  	if(cmd.find("<dualcp>") != std::string::npos) {
	  		for (std::list<std::string>::iterator it = dualCPList.begin(); it != dualCPList.end(); it++) {
	  			std::string new_cmd = replace(cmd, "<dualcp>", *it);
	  			combinations_list.push_back(new_cmd);
	  		}
	  	}
	  	else if (cmd.find("<cpname>") != std::string::npos) {
	  		for (std::list<std::string>::iterator it = cpNameList.begin(); it != cpNameList.end(); it++) {
	  			std::string new_cmd = replace(cmd, "<cpname>", *it);
	  			combinations_list.push_back(new_cmd);
	  		}
	  	}
		}
	}

	if (combinations_list.size() == 0) {
		combinations_list = cmd_list;
	}

	return combinations_list;
}
