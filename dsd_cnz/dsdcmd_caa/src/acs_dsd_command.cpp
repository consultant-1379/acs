/*
 *
 * NAME: acs_dsd_command.cpp
 *
 * COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 * All rights reserved.
 *
 *  The Copyright to the computer program(s) herein
 *  is the property of Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with
 *  the written permission from Ericsson Telecom AB or in
 *  accordance with the terms and conditions stipulated in the
 *  agreement/contract under which the program(s) have been
 *  supplied.
 *
 * .DESCRIPTION
 *  This file contains all methods and variables of the class Command.
 *
 * DOCUMENT NO
 *	190 89-CAA 109 0122 Ux
 *
 * AUTHOR
 * 	2010-12-13 TEI/XSD  XGIOPAP
 *
 * REVISION
 *
 * CHANGES
 *
 * RELEASE REVISION HISTORY
 *
 * REV NO		DATE		NAME 		DESCRIPTION
 * PA1 	       	20101213	XGIOPAP		Impact due to IP:
 *	  									8/159 41-8/FCP 121 1642
 *	  									"AXE IO, DSD and APJTP in APG50"
 *
 */

// Module Include Files
#include "acs_dsd_command.h"

#include <algorithm>
#include <sstream>

Attributesp myAttr;
vector<Attributesp>AttributesResult;

/*
 * Class Name: DSDlsCommand
 * Method Name: DSDlsCommand
 * Description: Class constructor
 * Used: -
 */
DSDlsCommand::DSDlsCommand(void)
{
	//init parameters
	Node = 0;
	Side = 0;
	State = 0;
	CPname = const_cast<char*>(EMPTY_STRING);
	APidentity = const_cast<char*>(EMPTY_STRING);
	ServiceName = const_cast<char*>(EMPTY_STRING);
	Domain = const_cast<char*>(EMPTY_STRING);
	memset(AP_CP, 0, 5);
	CPSingle = false;
	Format1 = false; //dsdls
	Format2 = false; //dsdls -p
	Format3 = false; //dsdls -a
	Format4 = false; //dsdls -s servicename
	Format5 = false; //dsdls -s servicename -p
	Format6 = false;//dsdls -s servicename -a
	Format7 = false; //dsdls -d domain
	Format8 = false; //dsdls -d domain -p
	Format9 = false; //dsdls -d domain -a
	Format10 = false; //dsdls -s servicename -d domain
	Format11 = false; //dsdls -s servicename -d domain -p
	Format12 = false; //dsdls -s servicename -d domain -a
	Format13 = false; //dsdls -ap apidentity
	Format14 = false; //dsdls -ap apidentity -p
	Format15 = false; //dsdls -cp cpname
	Format16 = false; //dsdls -n
}

/*
 * Class Name: DSDlsCommand
 * Method Name: ~DSDlsCommand
 * Description: Class destructor
 * Used: -
 */
DSDlsCommand::~DSDlsCommand(void)
{
	free(Node);
	free(Side);
	free(State);
}

/*
 * Class Name: DSDlsCommand
 * Method Name: MultipleCpCheck
 * Description: Check the type of the CP
 * Used: This method is used in the following member:
 *       Main program in ACS_DSD_dsdls.cpp file
 */
bool DSDlsCommand::MultipleCpCheck()
{
	/*
	 * invoke the ACS_CS_API to know if we're executing
	 * on a SingleCP System or on a MultipleCP System
	 */
	bool multipleCpSystem;

	//calling to isMultipleCpSystem method
	ACS_CS_API_NS::CS_API_Result returnValue =	ACS_CS_API_NetworkElement::isMultipleCPSystem(multipleCpSystem);

	//check return value
	if(returnValue != ACS_CS_API_NS::Result_Success)
	{//error
		switch(returnValue)
		{
		case ACS_CS_API_NS::Result_NoAccess:
			printf("%s\n", ERR56);
			exit(56);
		case ACS_CS_API_NS::Result_NoEntry:
			printf("%s\n", ERR55);
			exit(55);
		case ACS_CS_API_NS::Result_NoValue:
			printf("%s\n", ERR56);
			exit(56);
		case ACS_CS_API_NS::Result_Failure:
			printf("%s\n", ERR56);
			exit(56);
		default:
			printf("%s\n", ERR56);
			exit(56);
		}//switch
	}

	//check return value OK
	return multipleCpSystem;
	//return true;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: GetCpIdentity
 * Description: Get the CP identity
 * Used: This method is used in the following member:
 *       DSDlsCommand::execute_cmd program in ACS_DSD_command.cpp file
 */
int DSDlsCommand::GetCpIdentity(char* name, CPID& cpID)
{
	_cpTable = ACS_CS_API::createCPInstance();//create a CP instance
	ACS_CS_API_Name cpName(name);
	ACS_CS_API_NS::CS_API_Result returnValue = _cpTable->getCPId(cpName, cpID);

	if(returnValue != ACS_CS_API_NS::Result_Success)
	{ //error
		switch(returnValue)
		{
		case ACS_CS_API_NS::Result_NoAccess:
			printf("%s\n", ERR55);
			return(55);
		case ACS_CS_API_NS::Result_NoEntry:
			printf("%s\n", ERR118);
			return(118);
		case ACS_CS_API_NS::Result_NoValue:
			printf("%s\n", ERR118);
			return(118);
		case ACS_CS_API_NS::Result_Failure:
			printf("%s\n", ERR56);
			return(56);
		default:
			printf("%s\n", ERR56);
			return(56);
		}//switch
	}

	ACS_CS_API::deleteCPInstance(_cpTable);

	return (0);
}

/*
 * Class Name: DSDlsCommand
 * Method Name: GetAPId
 * Description: Get the AP identity
 * Used: This method is used in the following member:
 *       DSDlsCommand::execute_cmd program in ACS_DSD_command.cpp file
 */
int DSDlsCommand::GetAPId(char* name)
{
	if(name)
	{
		string ap(name);
		string str;

		//check length of the name
		if((ap.length() > 5) || (ap.length() < 3))
		{
			printf("%s\n", ERR113);
			return(113);
		}

		//check first 2 chars
		str = ap.substr(0, 2);
		if((str.compare("AP") != 0) && (str.compare("ap") != 0) && (str.compare("Ap") != 0)	&& (str.compare("aP") != 0))
		{
			printf("%s\n", ERR113);
			return(113);
		}

		//check last chars
		str = ap.substr(2,(ap.length() - 2));

		// check if the string contains not only numbers
		if (str.find_first_not_of("0123456789") != std::string::npos) {
			printf("%s\n", ERR113);
			return(113);
		}

		// check if the string has the format AP0XX
		if (!str.substr(0, 1).compare("0")) {
			printf("%s\n", ERR113);
			return(113);
		}

		int ap_system_id = ::atoi(str.c_str()) + 2000;
		if (!ACS_DSD_ConfigurationHelper::system_exist(ap_system_id)) {
			printf("%s\n", ERR113);
			return(113);
		}

		name = to_upper(name);
		return 0;
	}
	else
	{
		printf("%s\n", ERR113);
		return(113);
	}
}
/*
 * Class Name: DSDlsCommand
 * Method Name: usage
 * Description: Print the correct usage of the command
 * Used: This method is used in the following member:
 *       Main program in ACS_DSD_dsdls.cpp file
 */
void DSDlsCommand::usage(bool CPSys)
{
	printf("%s\n", ERR2);
	if(CPSys == false)
	{
		printf("%s\n","Usage: dsdls [-a ]");
	}
	else
	{
		printf("%s\n","Usage: dsdls [-s servicename] [-d domain] [-a | -p ]");
		printf("%s\n","");
		printf("%s\n","Usage: dsdls -cp cpname");
		printf("%s\n","");
		printf("%s\n","Usage: dsdls -ap apidentity [-p ]");
		printf("%s\n","");
		printf("%s\n","Usage: dsdls -n");
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: CheckData
 * Description: Check the data format
 * Used: This method is used in the following member:
 *       Main program in ACS_DSD_dsdls.cpp file
 */
bool DSDlsCommand::CheckData(const char* data)
{
	if(data)
	{
		string str(data);
		int ind = 0;
		if(str.compare("") == 0)
			return false;
		else
		{
			//find char -
			ind = (int)str.find_first_of("-",0);
			if(ind == 0)
				return false;
			else
				return true;
		}
	}
	else
		return false;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: execute_cmd
 * Description: Execute the dsdls command
 * Used: This method is used in the following member:
 *       Main program in ACS_DSD_dsdls.cpp file
 */
void DSDlsCommand::execute_cmd()
{
	int codeResult;

	if (Format13 || Format14)
	{
		codeResult = GetAPId(APidentity); //get id for that AP

		if (codeResult == 0)
		{
			codeResult = getObject();

			if (codeResult != 0)
				exit (codeResult);
		}
		else
			exit (codeResult);

	}
	else if (Format15)
	{
		//unsigned short cpID;
		CPID cpID = 0;
		codeResult = GetCpIdentity(CPname, cpID);

		if (codeResult == 0)
		{
			codeResult = getObject();

			if (codeResult != 0)
				exit (codeResult);
		}
		else
			exit (codeResult);
	}
	else
	{
		codeResult = getObject();

		if (codeResult != 0)
			exit (codeResult);
	}

	// Print the structure
	printStructure();
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getObject
 * Description: retrieve the object name and class from IMM
 * Used: This method is used in the following member:
 *       DSDlsCommand::execute_cmd in acs_dsd_dsdls.cpp file
 */
int DSDlsCommand::getObject()
{
	 //vector<string>SuList;
	 SaImmHandleT immHandle;
	 SaAisErrorT error;
	 SaImmSearchHandleT searchHandle;
	 SaImmSearchParametersT_2 searchParam;
	 SaNameT objectName;
	 SaImmAttrValuesT_2 **attributes;
	 SaNameT rootName = {0, ""};
	 SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */
	 SaVersionT version;

	const char * imm_dsd_root_dn = 0;

	/*Searching the parent name of the class to be defined*/
	if (ACS_DSD_ConfigurationHelper::get_dsd_imm_root_dn(imm_dsd_root_dn)) {
		ACE_OS::printf("%s\n", ERR55);
		return(55);
	}

	//string strRoot = "prvDsdId=1,safApp=safImmService";
	const string strRoot(imm_dsd_root_dn);
	rootName.length = strlen(strRoot.c_str());
	memcpy(rootName.value, strRoot.c_str(), rootName.length);

	version.releaseCode  = 'A';
	 version.majorVersion =  2;
	 version.minorVersion =  1;

	 error = saImmOmInitialize(&immHandle, 0, &version);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return(1);
	 }

	 // retrieve and set the Node, Side and AP_CP variable
	 if (getMyApNodeSide() != 0)
	 {
		 return (0);
		 //ACE_OS::printf("%s\n", ERR56);
		 //return(56);
	 }

	 // retrieve the node status and set the related variable
	 setMyState();

	 if (Format1 || Format2 || Format4 || Format5 || Format7 || Format8 || Format10 || Format11)	// dsdls, dsdls -p, dsdls -s, dsdls -s -p, dsdls -d, dsdls -d -p, dsdls -s -d, dsdls -s -d -p (current node)
	 {
		searchParam.searchOneAttr.attrName = const_cast<char*>("node");
		searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
		searchParam.searchOneAttr.attrValue = &Node;
	 }
	 else if (Format3 || Format6 || Format9 || Format12 || Format16)	//dsdls -a, dsdls -s -a, dsdls -d -a, dsdls -s -d -a, dsdls -n
	 {
		 searchParam.searchOneAttr.attrName = const_cast<char*>("node");
		 searchParam.searchOneAttr.attrValue = NULL;
	 }
	 else if (Format13 || Format14)	// dsdls -ap, dsdls -ap -p
	 {
		searchParam.searchOneAttr.attrName = const_cast<char*>("node");
		searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
		searchParam.searchOneAttr.attrValue = &APidentity;
	 }
	 else if (Format15)	// dsdls -cp
	 {
		searchParam.searchOneAttr.attrName = const_cast<char*>("node");
		searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SASTRINGT;
		searchParam.searchOneAttr.attrValue = &CPname;
	 }

	 error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, 0, &searchHandle);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return(1);
	 }

	 do
	 {
		 error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);

		 int codeResult;

		 // Check if the Object implementer is sunning
		 codeResult = checkFatherAttribute(immHandle, objectName);

		 if(codeResult != 0) {
			 if (Format15) // In dsdls -cp format, it's possible that the BC/CP is defined but not connected to DSD
				 return 0;

			 return(codeResult);
		 }

		 if (error != SA_AIS_ERR_NOT_EXIST)
		 {
			 if (Format16)
			 {
				// retrieve the father's attributes
				codeResult = getFatherAttribute(immHandle, objectName);

				if(codeResult == 0)
				{
					// Populate the vector
					AttributesResult.push_back(myAttr);

					// Clean the structure
					cleanStructure();
				}
				else
					return codeResult;
			 }
			 else
			 {
				 // retrieve the father name
				 vector<SaNameT>FobjectName;
				 int myresult = searchChild(immHandle, objectName, FobjectName);
				 if (myresult == 0)
				 {
					 unsigned i = 1;
					 while (i < FobjectName.size())
					 {
						 // retrieve the child's attributes
						 codeResult = getChildAttribute(immHandle, FobjectName[i]);

						 if(codeResult == 0)
						 {
							 // Populate the vector
							 AttributesResult.push_back(myAttr);

							 // Clean the structure
							 cleanStructure();
							 i++;
						 }
						 else
							return codeResult;
					 }
				 }
			 }
		 }
	 } while (error != SA_AIS_ERR_NOT_EXIST);

	 error = saImmOmSearchFinalize(searchHandle);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return(1);
	 }

	 // At this point the list of all SU with the Attribute SaAmfSIAssignment has

	 error = saImmOmFinalize(immHandle);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return(1);
	 }

	 return 0;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getChildAttribute
 * Description: retrieve the attribute and class from IMM
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
int DSDlsCommand::getChildAttribute(SaImmHandleT immHandle, SaNameT objectName)
{
	SaImmAttrValuesT_2 **attributes;
	SaImmAttrNameT * attributeNames = NULL;
	SaAisErrorT error;
	SaImmAccessorHandleT accessorHandle;
	int ChildOK = 0;
	string srtNode;
	string srtSide;

	attributeNames = new SaImmAttrNameT[10];
	attributeNames[0] = const_cast<char*>("srvInfoId");
	attributeNames[1] = const_cast<char*>("name");
	attributeNames[2] = const_cast<char*>("domain");
	attributeNames[3] = const_cast<char*>("conn_type");
	attributeNames[4] = const_cast<char*>("proc_name");
	attributeNames[5] = const_cast<char*>("pid");
	attributeNames[6] = const_cast<char*>("inet_addresses");
	attributeNames[7] = const_cast<char*>("unix_addresses");
	attributeNames[8] = const_cast<char*>("visibility");
	attributeNames[9] = 0;

    error = saImmOmAccessorInitialize(immHandle, &accessorHandle);

    if (error != SA_AIS_OK)
    {
    	delete attributeNames;
    	ACE_OS::printf("%s\n", ERR1);
    	return(1);
    }

	error = saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	srtNode = (char *)objectName.value;
	srtSide = (char *)objectName.value;

	if (Format1 || Format2 || Format3 || Format13 || Format14 || Format15)	// dsdls, dsdls -p, dsdls -a, dsdls -ap, dsdls -ap -p, dsdls -cp
	{
		if (attributes[1]->attrValuesNumber == 0 && attributes[3]->attrValuesNumber == 0 && attributes[4]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				if(CheckProcessRunning(*((SaStringT *)attributes[3]->attrValues[0]), *((SaInt32T *)attributes[4]->attrValues[0])) == 1)
				{
					// Set the child's attributes
					fillChildStructure(attributes);
					ChildOK = 1;
				}
			}else{
				// Set the child's attributes
				fillChildStructure(attributes);
				ChildOK = 1;
			}
		}
	}
	else if (Format4 || Format5 || Format6)	// dsdls -s, dsdls -s -p, dsdls -s -a
	{
		if (attributes[1]->attrValuesNumber == 0 && attributes[3]->attrValuesNumber == 0 && attributes[4]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				if (strcmp(*((SaStringT *)attributes[5]->attrValues[0]), ServiceName) == 0)
				{
					if(CheckProcessRunning(*((SaStringT *)attributes[3]->attrValues[0]), *((SaInt32T *)attributes[4]->attrValues[0])) == 1)
					{
						// Set the child's attributes
						fillChildStructure(attributes);
						ChildOK = 1;
					}
				}
			}else{
				if (strcmp(*((SaStringT *)attributes[5]->attrValues[0]), ServiceName) == 0)
				{
					// Set the child's attributes
					fillChildStructure(attributes);
					ChildOK = 1;
				}
			}
		}
	}
	else if (Format7 || Format8 || Format9)	// dsdls -d, dsdls -d -p, dsdls -d -a
	{
		if (attributes[1]->attrValuesNumber == 0 && attributes[3]->attrValuesNumber == 0 && attributes[4]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				if (strcmp(*((SaStringT *)attributes[7]->attrValues[0]), Domain) == 0)
				{
					if(CheckProcessRunning(*((SaStringT *)attributes[3]->attrValues[0]), *((SaInt32T *)attributes[4]->attrValues[0])) == 1)
					{
						// Set the child's attributes
						fillChildStructure(attributes);
						ChildOK = 1;
					}
				}
			}else{
				if (strcmp(*((SaStringT *)attributes[7]->attrValues[0]), Domain) == 0)
				{
					// Set the child's attributes
					fillChildStructure(attributes);
					ChildOK = 1;
				}
			}
		}
	}
	else if (Format10 || Format11 || Format12)
	{
		if (attributes[1]->attrValuesNumber == 0 && attributes[3]->attrValuesNumber == 0 && attributes[4]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			if(strcmp(getNodeNamefromObjectName(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromObjectName(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				if (strcmp(*((SaStringT *)attributes[5]->attrValues[0]), ServiceName) == 0 && strcmp(*((SaStringT *)attributes[7]->attrValues[0]), Domain) == 0)
				{
					if(CheckProcessRunning(*((SaStringT *)attributes[3]->attrValues[0]), *((SaInt32T *)attributes[4]->attrValues[0])) == 1)
					{
						// Set the child's attributes
						fillChildStructure(attributes);
						ChildOK = 1;
					}
				}
			}else{
				if (strcmp(*((SaStringT *)attributes[5]->attrValues[0]), ServiceName) == 0 && strcmp(*((SaStringT *)attributes[7]->attrValues[0]), Domain) == 0)
				{
					// Set the child's attributes
					fillChildStructure(attributes);
					ChildOK = 1;
				}
			}
		}
	}

	if (ChildOK == 1)
	{
		char* temp = (char*)(objectName.value);
		temp = strchr(temp, ',');
		temp++;

		strcpy((char*)objectName.value, temp);

		// retrieve the father's attributes
		int codeResult = getFatherAttribute(immHandle, objectName);

		if (codeResult != 0)
		{
			delete attributeNames;
			return codeResult;
		}

	}

	error = saImmOmAccessorFinalize(accessorHandle);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	delete attributeNames;
	return 0;
}

void DSDlsCommand::getNodeIdFromDN(const SaNameT &objectName, string *pstrOut)
{
	size_t idxStrNode;
	size_t idxSeparator;
	size_t idxLength;
	const char *szString;
	string strLocal;
	string strNode;
	// initialization
	idxStrNode = string::npos;
	idxSeparator = string::npos;
	szString = NULL;
	strLocal.clear();
	strNode.clear();
	// Cast object to char
	szString = reinterpret_cast<const char*>(objectName.value);
	// Check cast
	if((szString != NULL) && (pstrOut != NULL)){
		// Clear out string
		pstrOut->clear();
		// Copy buffer in std::string
		strLocal.assign(szString);
		// Search STRNode
		idxStrNode = strLocal.find(STR_DSDLS_SRTNODE);
		// Check if "StrNode" exist in the input string
		if(idxStrNode != string::npos){
			idxSeparator = strLocal.find(CHR_DSDLS_DN_SEPARATOR, idxStrNode);
			// Check exists
			if(idxSeparator != string::npos){
				// Compute length of the substring
				idxLength = idxSeparator - idxStrNode;
				// check length
				if(idxLength > 0){
					// Extract substring ex. SRTNode=AP1A
					strNode = strLocal.substr(idxStrNode,idxLength);
				}
			}else{
				// Separator not exist. Copy all remaining char
				strNode = strLocal.substr(idxStrNode);
			}
		}
		// Verify in node string exist (ex. "SRTNode=AP1A")
/*		if(strNode.empty() == false){
			// Search '='
			idxSeparator = strNode.find('=');
			// Check if '=' exist
			if(idxSeparator != string::npos){
				// Next char
				++idxSeparator;
				// Check if iterator is out of range
				if(idxSeparator <= strNode.length()){
					// Assign output string
					pstrOut->assign(strNode.substr(idxSeparator));
				}
			}
		}*/
		// Assign out string
		pstrOut->assign(strNode);
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getFatherAttribute
 * Description: retrieve the attribute and class from IMM
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
int DSDlsCommand::getFatherAttribute(SaImmHandleT immHandle, SaNameT objectName)
{
	SaImmAttrValuesT_2 **attributes;
	SaImmAttrNameT * attributeNames = NULL;
	SaAisErrorT error;
	SaImmAccessorHandleT accessorHandle;
	string srtNode;
	string srtSide;

	attributeNames = new SaImmAttrNameT[5];
	attributeNames[0] = const_cast<char*>("srtNodeId");
	attributeNames[1] = const_cast<char*>("side");
	attributeNames[2] = const_cast<char*>("node");
	attributeNames[3] = const_cast<char*>("state");
	attributeNames[4] = 0;

	error = saImmOmAccessorInitialize(immHandle, &accessorHandle);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	error = saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	getNodeIdFromDN(objectName, &srtNode);
	srtSide = srtNode;

//	srtNode = *((SaStringT *)attributes[1]->attrValues[0]);
//	srtSide = *((SaStringT *)attributes[1]->attrValues[0]);

	if (Format1 || Format4 || Format7 || Format10)	// dsdls, dsdls -s, dsdls -d, dsdls -s -d
	{
		if (attributes[0]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromsrtNodeId(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromsrtNodeId(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			char t1[127];
			char t2[127];
			char tNode1[127];

			sprintf(t1, "%i", *((SaInt32T *)attributes[2]->attrValues[0]));
			sprintf(t2, "%d", *((SaUint32T *)attributes[0]->attrValues[0]));
			sprintf(tNode1, "%s", *((SaStringT *)attributes[3]->attrValues[0]));

			// call the function to retrieve the side from the imm value
			if((strcmp(getSide(t1, tNode1), Side) == 0) && (strcmp(getState(t2, tNode1), State) == 0))
			{
				// Set the fhater's's attributes
				fillFatherStructure(attributes);
			}
		}
	}
	else if (Format2 || Format3 || Format5 || Format6 || Format8 || Format9 || Format11 || Format12 || Format14 || Format15 || Format16)	// dsdls -p, dsdls -a, dsdls -s -p, dsdls -s -a, dsdls -d -p, dsdls -d -a, dsdls -s -d -p, dsdls -s -d -a, dsdls -ap -p, dsdls -cp, dsdls -n
	{
		if (attributes[0]->attrValuesNumber == 0)
		{
			if(strcmp(getNodeNamefromsrtNodeId(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromsrtNodeId(const_cast<char*>(srtSide.c_str())), Side) == 0)
			{
				delete attributeNames;
				ACE_OS::printf("%s\n", ERR117);
				return(117);
			}
		}
		else
		{
			// Set the fhater's attributes
			fillFatherStructure(attributes);
		}
	}
	else if (Format13)	// dsdls -ap
	{
		char t3[127];
		char tNode2[127];

		sprintf(t3, "%d", *((SaUint32T *)attributes[0]->attrValues[0]));
		sprintf(tNode2, "%s", *((SaStringT *)attributes[3]->attrValues[0]));

		// call the function to retrieve the state from the imm value
		if(strcmp(getState(t3, tNode2), "ACTIVE") == 0)
		{
			// Set the fhater's's attributes
			fillFatherStructure(attributes);
		}
	}

	error = saImmOmAccessorFinalize(accessorHandle);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	delete attributeNames;
	return 0;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: checkFatherAttribute
 * Description: check if the father's attributes are empty
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
int DSDlsCommand::checkFatherAttribute(SaImmHandleT immHandle, SaNameT objectName)
{
	SaImmAttrValuesT_2 **attributes;
	SaImmAttrNameT * attributeNames = NULL;
	SaAisErrorT error;
	SaImmAccessorHandleT accessorHandle;

	attributeNames = new SaImmAttrNameT[5];
	attributeNames[0] = const_cast<char*>("srtNodeId");
	attributeNames[1] = const_cast<char*>("side");
	attributeNames[2] = const_cast<char*>("node");
	attributeNames[3] = const_cast<char*>("state");
	attributeNames[4] = 0;

	error = saImmOmAccessorInitialize(immHandle, &accessorHandle);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	error = saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;

		// In dsdls -cp, dsdls -ap, dsdls -ap -p formats is possible that
		// the given BC/CP or remote AP node are defined but not connected to DSD on the local node.
		if (Format13 || Format14 || Format15)	return 0;

		// In all other formats, print an error message
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	if (attributes[0]->attrValuesNumber == 0)
	{
		string srtNode;
		string srtSide;

		getNodeIdFromDN(objectName, &srtNode);
		srtSide = srtNode;
		//srtNode = *((SaStringT *)attributes[1]->attrValues[0]);
		//srtSide = *((SaStringT *)attributes[1]->attrValues[0]);

		if(strcmp(getNodeNamefromsrtNodeId(const_cast<char*>(srtNode.c_str())), Node) == 0 && strcmp(getSidefromsrtNodeId(const_cast<char*>(srtSide.c_str())), Side) == 0)
		{
			delete attributeNames;
			ACE_OS::printf("%s\n", ERR117);
			return(117);
		}
	}

	error = saImmOmAccessorFinalize(accessorHandle);

	if (error != SA_AIS_OK)
	{
		delete attributeNames;
		ACE_OS::printf("%s\n", ERR1);
		return(1);
	}

	delete attributeNames;
	return 0;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: searchChild
 * Description: Search the father class from the child class
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
int DSDlsCommand::searchChild(SaImmHandleT immHandle, SaNameT tempName, vector<SaNameT> &FobjectName)
{
	string strRoot = (char*)tempName.value;
	SaAisErrorT error;
	SaImmSearchHandleT searchHandle;
	SaNameT objectName;
	SaImmAttrValuesT_2 **attributes;
	SaNameT rootName = {0, ""};
	rootName.length = strlen(strRoot.c_str());
	memcpy(rootName.value, strRoot.c_str(), rootName.length);
	SaImmScopeT scope = SA_IMM_SUBTREE;     /* default search scope */

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, NULL, 0, &searchHandle);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return (1);
	 }

	 do
	 {
		 error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);

		 // Find the father's name
		 if (error != SA_AIS_ERR_NOT_EXIST)
			 FobjectName.push_back(objectName);

	 } while (error != SA_AIS_ERR_NOT_EXIST);

	 error = saImmOmSearchFinalize(searchHandle);

	 if (error != SA_AIS_OK)
	 {
		 ACE_OS::printf("%s\n", ERR1);
		 return (1);
	 }

	 return 0;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getMyAP
 * Description: retrieve the name of my node and set the AP_CP variable
 * Used: This method is used in the following member:
 *       DSDlsCommand::execute_cmd in acs_dsd_command.cpp file
 */
int DSDlsCommand::getMyApNodeSide()
{
	int call_result = 0;

	ACS_DSD_ConfigurationHelper::HostInfo_const_pointer_t my_host_info_ptr = 0;
	call_result = ACS_DSD_ConfigurationHelper::get_my_ap_node(my_host_info_ptr);

	if (call_result == 0)
	{
		Node =  strdup(my_host_info_ptr->system_name);

		if (my_host_info_ptr->side == 0)
			Side = strdup("A");
		else if (my_host_info_ptr->side == 1)
			Side = strdup("B");
		else if (my_host_info_ptr->side == 2)
			Side = strdup("C");
		else
			Side = strdup("UNDEFINED");

		//set the AP/CP flag
		strncpy(AP_CP, Node, 2);
	}

	return call_result;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: CheckProcessRunning
 * Description: retrieve if the process is still running
 * Used: This method is used in the following member:
 *       DSDlsCommand::GetChildAttribute in acs_dsd_command.cpp file
 */
int DSDlsCommand::CheckProcessRunning(const char * process_name, pid_t pid)
{
	int call_result = 0;

	call_result = ACS_DSD_ConfigurationHelper::process_running(process_name, pid);

	return call_result;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: setMyState
 * Description: Set the state of the current node
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_command.cpp file
 */
void DSDlsCommand::setMyState()
{
	ACS_DSD_Node local_node;
	ACS_DSD_Server svr;

	int op_res = svr.get_local_node(local_node);

	if(op_res < 0)
		State = strdup("UNDEFINED");
	else
	{
		if (local_node.node_state == acs_dsd::NODE_STATE_ACTIVE)
		{
			if (strcmp(AP_CP, "AP") == 0)
				State = strdup("ACTIVE");
			else
				State = strdup("EX");
		}
		else if (local_node.node_state == acs_dsd::NODE_STATE_PASSIVE)
		{
			if (strcmp(AP_CP, "AP") == 0)
				State = strdup("PASSIVE");
			else
				State = strdup("SB");
		}
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getNodeNamefromsrtNodeId
 * Description: retrieve the Node name from the srtNodeId attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
char* DSDlsCommand::getNodeNamefromsrtNodeId(char* srtNodeName)
{
	string mysrtNodeName;

	mysrtNodeName = srtNodeName;

	if(mysrtNodeName.length() == 12)
		srtNodeName = const_cast<char*>(mysrtNodeName.substr(8, 3).c_str());
	else
		srtNodeName = const_cast<char*>(mysrtNodeName.substr(8, 4).c_str());

	return srtNodeName;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getSidefromsrtNodeId
 * Description: retrieve the Side from the srtNodeId attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
char* DSDlsCommand::getSidefromsrtNodeId(char* srtSide)
{
	string mysrtNodeName;

	mysrtNodeName = srtSide;

	if(mysrtNodeName.length() == 12)
		srtSide = const_cast<char*>(mysrtNodeName.substr(11, 1).c_str());
	else
		srtSide = const_cast<char*>(mysrtNodeName.substr(12, 1).c_str());

	return srtSide;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getNodeNamefromsrtNodeId
 * Description: retrieve the Node name from the objectName
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
char* DSDlsCommand::getNodeNamefromObjectName(char* ObjectName)
{
	string mysrtNodeName;
	char * pch;

	pch = strtok(ObjectName, ",");
	pch = strtok(NULL, ",");
	mysrtNodeName = pch;

	if(mysrtNodeName.length() == 12)
		ObjectName = const_cast<char*>(mysrtNodeName.substr(8, 3).c_str());
	else
		ObjectName = const_cast<char*>(mysrtNodeName.substr(8, 4).c_str());

	return ObjectName;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getSidefromObjectName
 * Description: retrieve the Side from the objectName
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
char* DSDlsCommand::getSidefromObjectName(char* srtSide)
{
	string mysrtNodeName;
	char * pch;

	pch = strtok(srtSide, ",");
	pch = strtok(NULL, ",");
	mysrtNodeName = pch;

	if(mysrtNodeName.length() == 12)
		srtSide = const_cast<char*>(mysrtNodeName.substr(11, 1).c_str());
	else
		srtSide = const_cast<char*>(mysrtNodeName.substr(12, 1).c_str());

	return srtSide;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getSide
 * Description: retrieve the side attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_dsdls.cpp file
 */
char* DSDlsCommand::getSide(char* side, char* newNode)
{
	char mySide[10] = {0};
	char myNode[10] = {0};

	strncpy(myNode, newNode, 2);
	strcpy(mySide, side);

	if (strcmp(myNode, "AP") == 0)
	{
		if (strcmp(mySide, "0") == 0)
			strcpy(side, "A");
		else if (strcmp(mySide, "1") == 0)
			strcpy(side, "B");
		else
			strcpy(side, "UNDEFINED");
	}
	else if (strcmp(myNode, "CP") == 0)
	{
		if (strcmp(mySide, "0") == 0)
			strcpy(side, "A");
		else if (strcmp(mySide, "1") == 0)
			strcpy(side, "B");
		else
			strcpy(side, "UNDEFINED");
	}
	else
		strcpy(side, "-");

	return side;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getState
 * Description: retrieve the state attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
char* DSDlsCommand::getState(char* state, char* newNode)
{
	char myState[20] = {0};
	char myNode[10] = {0};

	strncpy(myNode, newNode, 2);
	strcpy(myState, state);

	if (strcmp(myNode, "AP") == 0)
	{
		if (strcmp(myState, "2") == 0)
			strcpy(state, "UNDEFINED");
		else if (strcmp(myState, "0") == 0)
			strcpy(state, "ACTIVE");
		else if (strcmp(myState, "1") == 0)
			strcpy(state, "PASSIVE");
		else
			strcpy(state, "UNREACHABLE");
	}
	else //if (strcmp(myNode, "CP") == 0)
	{
		if (strcmp(myState, "2") == 0)
			strcpy(state, "UNDEFINED");
		else if (strcmp(myState, "0") == 0)
			strcpy(state, "EX");
		else if (strcmp(myState, "1") == 0)
			strcpy(state, "SB");
		else
			strcpy(state, "UNREACHABLE");
	}

	return state;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getConnType
 * Description: retrieve the connection type from the attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
void DSDlsCommand::getConnType(int iConnType, string *pStrOut)
{
	// Check pStrOut pointer
	if(pStrOut != NULL){
		// Initialize output string
		pStrOut->clear();
		// switch on connType
		switch(iConnType){
			// OCP type
			case OCP:
				pStrOut->assign(STR_DSDLS_OCP);
			break;
			// TCPIP type
			case TCPIP:
				pStrOut->assign(STR_DSDLS_TCPIP);
			break;
			// UNIX type
			case UNIX:
				pStrOut->assign(STR_DSDLS_UNIX);
			break;
			// TCPIP_UNIX
			case TCPIP_UNIX:
				pStrOut->assign(STR_DSDLS_TCPIPUNIX);
			break;
			// Default value
			default:
				// Set to undefined type
				pStrOut->assign(STR_DSDLS_UNDEFINED);
		}
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getPid
 * Description: retrieve the pid attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
void DSDlsCommand::getPid(unsigned int uiPid, const string &strNode, string *pStrOut)
{
	string strTmp;
	stringstream strSS;
	// Check pointer
	if(pStrOut != NULL){
		// Initialize output string
		pStrOut->assign(STR_DSDLS_NULL);
		// Get first two chars
		strTmp = strNode.substr(0,2);
		// Upper case
		std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);
		// Check is an AP
		if((strTmp.compare(STR_DSDLS_AP) == 0) && (uiPid != 0)){
			// Convert pid in string
			strSS << uiPid;
			// Set output string same as input "pid"
			pStrOut->assign(strSS.str());
		}else{
			// Not an "AP" OR pid is 0. Check if is a single cp
			if(CPSingle == true){
				// Single CP, return pid "0"
				pStrOut->assign(STR_DSDLS_ZERO);
			}else{
				// Multi CP, return pid "-"
				pStrOut->assign(STR_DSDLS_NONE);
			}
		}
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getIp
 * Description: retrieve the ip number attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
void DSDlsCommand::getIp(const string &strAddress, const string &strNode, string *pStrOut)
{
	size_t idx;
	string strTmp;
	// Initialization
	idx = string::npos;
	strTmp.clear();
	// Check pStrOut pointer
	if((pStrOut != NULL) && (strAddress.length() > 0) && (strNode.length() > 1)){
		// Extract a first two char from strNode
		strTmp = strNode.substr(0,2);
		// Upper case
		std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);
		// Check if is an "AP"
		if(strTmp.compare(STR_DSDLS_AP) == 0){
			// Copy strIp in a local string
			strTmp = strAddress;
			// Initialize output string
			pStrOut->clear();
			// Find a port-ip separator
			idx = strTmp.find(':');
			// Check if port-ip separator exist in input string string
			if (idx != string::npos){
				// Copy a right part of the string
				pStrOut->assign(strTmp.substr(0, idx));
			}else{
				// Not contain a ":"? Copy all value
				pStrOut->assign(strTmp);
			}
		} else {
			// Node is not a "AP"
			pStrOut->assign(STR_DSDLS_ZERO);
		}
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getPort
 * Description: retrieve the port number attribute
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
void DSDlsCommand::getPort(const string &strAddress, string *pStrOut)
{
	size_t idx;
	// Initialization
	idx = string::npos;
	// Check input params
	if((pStrOut != NULL) && (strAddress.length() > 0)){
		// Clear out string
		pStrOut->clear();
		// Find port-ip separator (address format is "ip:port")
		idx = strAddress.find(CHR_DSDLS_IPPORT_SEPARATOR);
		// Check if port-ip separator exist in input string string
		if (idx != string::npos){
			// set to prev char
			++idx;
			// Check pointer
			if(idx < strAddress.size()){
				// Left part of the input string
				pStrOut->assign(strAddress.substr(idx));
			}
		}
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: getVisibility
 * Description: retrieve the global or local scope
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
char* DSDlsCommand::getVisibility(char* visibility)
{
	char myVisibility[10] = {0};

	strcpy(myVisibility, visibility);

	if (strcmp(myVisibility, "0") == 0)
		strcpy(visibility, "GLOBAL");
	else if (strcmp(myVisibility, "1") == 0)
		strcpy(visibility, "LOCAL");
	else
		strcpy(visibility, "UNDEFINED");

	return visibility;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: fillChildStructure
 * Description: Fill the child's attributes in the structure with the IMM data
 * Used: This method is used in the following member:
 *       DSDlsCommand::getFatherAttribute and DSDlsCommand::getChildAttribute
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::fillChildStructure(SaImmAttrValuesT_2 **attributes)
{
	myAttr.visibility = *((SaInt32T *)attributes[0]->attrValues[0]);

	if (attributes[1]->attrValuesNumber > 1)
	{
		unsigned int tmpUnix = 0;

		while (tmpUnix != attributes[1]->attrValuesNumber)
		{
			if (strcmp(*((SaStringT *)attributes[1]->attrValues[tmpUnix]), "") == 0)
				tmpUnix++;
			else
			{
				myAttr.unix_address.push_back(*((SaStringT *)attributes[1]->attrValues[tmpUnix]));
				tmpUnix++;
			}
		}
	}
	else
		myAttr.unix_address.push_back(*((SaStringT *)attributes[1]->attrValues[0]));

	myAttr.srvInfoId = *((SaStringT *)attributes[2]->attrValues[0]);
	myAttr.proc_name = *((SaStringT *)attributes[3]->attrValues[0]);
	myAttr.pid = *((SaInt32T *)attributes[4]->attrValues[0]);
	myAttr.name = *((SaStringT *)attributes[5]->attrValues[0]);

	if (attributes[6]->attrValuesNumber > 1)
	{
		unsigned int tmpInet = 0;

		while (tmpInet != attributes[6]->attrValuesNumber)
		{
			if (strcmp(*((SaStringT *)attributes[6]->attrValues[tmpInet]), "") == 0)
				tmpInet++;
			else
			{
				myAttr.inet_address.push_back(*((SaStringT *)attributes[6]->attrValues[tmpInet]));
				tmpInet++;
			}
		}
	}
	else
		myAttr.inet_address.push_back(*((SaStringT *)attributes[6]->attrValues[0]));

	myAttr.domain = *((SaStringT *)attributes[7]->attrValues[0]);
	myAttr.conn_type = *((SaUint32T *)attributes[8]->attrValues[0]);
}

/*
 * Class Name: DSDlsCommand
 * Method Name: fillFatherStructure
 * Description: Fill the father's attributes in the structure with the IMM data
 * Used: This method is used in the following member:
 *       DSDlsCommand::getFatherAttribute and DSDlsCommand::getChildAttribute
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::fillFatherStructure(SaImmAttrValuesT_2 **attributes)
{
	myAttr.state = *((SaUint32T *)attributes[0]->attrValues[0]);
	myAttr.srtNodeId = *((SaStringT *)attributes[1]->attrValues[0]);
	myAttr.side = *((SaInt32T *)attributes[2]->attrValues[0]);
	myAttr.node = *((SaStringT *)attributes[3]->attrValues[0]);
}

/*
 * Class Name: DSDlsCommand
 * Method Name: cleanStructure
 * Description: clean the attributes in the structure
 * Used: This method is used in the following member:
 *       DSDlsCommand::getFatherAttribute and DSDlsCommand::getChildAttribute
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::cleanStructure()
{
	myAttr.visibility = 0;
	myAttr.unix_address.clear();
	myAttr.srvInfoId = EMPTY_STRING;
	myAttr.proc_name = EMPTY_STRING;
	myAttr.pid = 0;
	myAttr.name = EMPTY_STRING;
	myAttr.inet_address.clear();
	myAttr.domain = EMPTY_STRING;
	myAttr.conn_type = 0;
	myAttr.state = EMPTY_VALUE;
	myAttr.srtNodeId = EMPTY_STRING;
	myAttr.side = EMPTY_VALUE;
	myAttr.node = EMPTY_STRING;
}

/*
 * Class Name: DSDlsCommand
 * Method Name: printEthAndChild
 * Description: print structure child
 * Used: This method is used in the following member:
 *       DSDlsCommand::printSingleCP
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::printEthAndChild(const Attributesp &attr, bool bOcpFrmt)
{
	string strNodeSide;
	string strConnTyp;
	string strPid;
	string strIp;
	string strPort;
	const vector<string> *vctDummy;
	vector<string>::const_iterator itString;
	// Initialization
	vctDummy = NULL;

	//------------------ Questo blocco e' tenuto per compatibilita' con i metodi della classe ------------------
	char szMySide[127];
	char sznode[127];
	szMySide[0] = '\0';
	sznode[0] = '\0';
	sprintf(szMySide, "%i", attr.side);
	sprintf(sznode, "%s", attr.node.c_str());
	getSide(szMySide, sznode);
	// ---------------------------------------------------------------------------------------------------------

	// Set reference of string vector (unix or net)
	if(attr.conn_type != UNIX){
		// Position on begin of the net list
		vctDummy = &(attr.inet_address);
	}else{
		// Position on begin of the list
		vctDummy = &(attr.unix_address);
	}
	// Position on begin of the list
	itString = vctDummy->begin();
	// Check iterator
	if((itString != vctDummy->end()) && (itString->size() > 0)){
		// Create NodeSide
		strNodeSide = attr.node;
		strNodeSide.append(szMySide);
		// Create conn-type string
		getConnType(attr.conn_type, &strConnTyp);
		// Create pid string
		getPid(attr.pid, attr.node.c_str(), &strPid);
		// Test if a format is OCP
		if(bOcpFrmt == true){
			// Create IP string
			getIp(*itString, attr.node.c_str(), &strIp);
			// Create PORT string
			getPort(*itString, &strPort);
		}else{
			// Another format (no ip and port)
			strIp = STR_DSDLS_NONE;
			strPort = STR_DSDLS_NONE;
		}
		// Check if conn-type is unix
		if(attr.conn_type == UNIX){
			// Print unix line
			printf(STR_DSDLS_SINGLECPUNIXFMT, strNodeSide.c_str(), attr.domain.c_str(), attr.name.c_str(),
					strConnTyp.c_str(), strPid.c_str(), strIp.c_str());
		}else{
			// Print no unix line
			printf(STR_DSDLS_SINGLECPFMT, strNodeSide.c_str(), attr.domain.c_str(), attr.name.c_str(),
					strConnTyp.c_str(),strPid.c_str(), strIp.c_str(), strPort.c_str());
		}
		// Next net element
		++itString;
		// For only element
		while (itString != vctDummy->end()){
			// Test if a format is OCP
			if(bOcpFrmt == true){
				// Create IP string
				getIp(*itString, attr.node.c_str(), &strIp);
				// Create PORT string
				getPort(*itString, &strPort);
			}else{
				// Another format (no ip and port)
				strIp = STR_DSDLS_NONE;
				strPort = STR_DSDLS_NONE;
			}
			// Check if conn-type is unix
			if(attr.conn_type == UNIX){
				// Print unix line
				printf(STR_DSDLS_SINGLECPUNIXFMT, STR_DSDLS_NULL, STR_DSDLS_NULL, STR_DSDLS_NULL,
						strConnTyp.c_str(), STR_DSDLS_NULL, strIp.c_str());
			}else{
				// Print no unix line
				printf(STR_DSDLS_SINGLECPFMT, STR_DSDLS_NULL, STR_DSDLS_NULL, STR_DSDLS_NULL, strConnTyp.c_str(),
						STR_DSDLS_NULL, strIp.c_str(), strPort.c_str());
			}
			// Next element
			++itString;
		}
	} // if((itString != vctDummy->end()) && (itString->size() > 0))
}

/*
 * Class Name: DSDlsCommand
 * Method Name: printSingleCP
 * Description: print main structure
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::printSingleCP(bool bOnlyUnixOrTcpIp)
{
	std::vector<Attributesp>::iterator it;
	// Set iterator to point to first element
	it = AttributesResult.begin();
	// Cycle on all attribute
	while(it != AttributesResult.end()){
		// Verify imm entry
		if (it->node != STR_DSDLS_NULL && it->side != EMPTY_VALUE && it->state != EMPTY_VALUE){
			// Check if conn-type is unix and flag is false
			if(((it->conn_type != UNIX) && (bOnlyUnixOrTcpIp == true)) || ((it->conn_type == UNIX) && (bOnlyUnixOrTcpIp == false))){
				// Check connection type
				if (it->conn_type == OCP || it->conn_type == TCPIP || it->conn_type == UNIX){
					// Connection is a OCP or TCPIP OR UNIX.
					printEthAndChild(*it, true);
				}else{
					// Print in another format: without ip and port
					printEthAndChild(*it, false);
				}
			}
		}
		// Next element
		++it;
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: printSingleCP
 * Description: print structure child for multiCP
 * Used: This method is used in the following member:
 *       DSDlsCommand::printMultiCP
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::printMultiCPEthChild(const Attributesp &attr, bool bOcpFrmt)
{
	string strNode;
	string strConnTyp;
	string strPid;
	string strIp;
	string strPort;
	const vector<string> *vctDummy;
	vector<string>::const_iterator itString;
	// Initialization
	vctDummy = NULL;

	//------------------ Questo blocco e' tenuto per compatibilita' con i metodi della classe ------------------
	char szMySide[127];
	char sznode[127];
	char szMyState[127];
	szMySide[0] = '\0';
	sznode[0] = '\0';
	szMyState[0] = '\0';
	sprintf(szMySide, "%i", attr.side);
	sprintf(sznode, "%s", attr.node.c_str());
	sprintf(szMyState, "%d", attr.state);
	getSide(szMySide, sznode);
	getState(szMyState, sznode);
	// ---------------------------------------------------------------------------------------------------------

	// Check command format
	if(Format16 == false){
		// Set reference of string vector (unix or net)
		if(attr.conn_type != UNIX){
			// Position on begin of the net list
			vctDummy = &(attr.inet_address);
		}else{
			// Position on begin of the list
			vctDummy = &(attr.unix_address);
		}
		// Position on begin of the list
		itString = vctDummy->begin();
		// Check iterator
		if((itString != vctDummy->end()) && (itString->size() > 0)){
			// Create Node
			strNode = attr.node;
			// Create conn-type string
			getConnType(attr.conn_type, &strConnTyp);
			// Create pid string
			getPid(attr.pid, attr.node.c_str(), &strPid);
			// Test if a format is OCP
			if(bOcpFrmt == true){
				// Create IP string
				getIp(*itString, attr.node.c_str(), &strIp);
				// Create PORT string
				getPort(*itString, &strPort);
			}else{
				// Another format (no ip and port)
				strIp = STR_DSDLS_NONE;
				strPort = STR_DSDLS_NONE;
			}
			// Check if conn-type is unix
			if(attr.conn_type == UNIX){
				// Print unix line
				printf(STR_DSDLS_MULTICPUNIXFMT, strNode.c_str(), szMySide, szMyState,
						attr.domain.c_str(), attr.name.c_str(), strConnTyp.c_str(),strPid.c_str(), strIp.c_str());
			}else{
				// Print no unix line
				printf(STR_DSDLS_MULTICPFMT, strNode.c_str(), szMySide, szMyState,
						attr.domain.c_str(), attr.name.c_str(), strConnTyp.c_str(), strPid.c_str(), strIp.c_str(), strPort.c_str());
			}
			// Next net element
			++itString;
			// For only element
			while (itString != vctDummy->end()){
				// Test if a format is OCP
				if(bOcpFrmt == true){
					// Create IP string
					getIp(*itString, attr.node.c_str(), &strIp);
					// Create PORT string
					getPort(*itString, &strPort);
				}else{
					// Another format (no ip and port)
					strIp = STR_DSDLS_NONE;
					strPort = STR_DSDLS_NONE;
				}
				// Check if conn-type is unix
				if(attr.conn_type == UNIX){
					// Print unix line
					printf(STR_DSDLS_MULTICPUNIXFMT, STR_DSDLS_NULL, STR_DSDLS_NULL, STR_DSDLS_NULL,
							STR_DSDLS_NULL, STR_DSDLS_NULL, strConnTyp.c_str(), STR_DSDLS_NULL, strIp.c_str());
				}else{
					// Print no unix line
					printf(STR_DSDLS_MULTICPFMT, STR_DSDLS_NULL, STR_DSDLS_NULL, STR_DSDLS_NULL,
							STR_DSDLS_NULL, STR_DSDLS_NULL, strConnTyp.c_str(), STR_DSDLS_NULL, strIp.c_str(), strPort.c_str());
				}
				// Next element
				++itString;
			}
		} // if((itString != vctDummy->end()) && (itString->size() > 0))
	}else{
		// Is format16 : "dsdls -n". Print line
		printf(STR_DSDLS_MULTICPFMT16, sznode, szMySide, szMyState);
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: printSingleCP
 * Description: print main structure for multiCP
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure
 *       in acs_dsd_command.cpp file
 */
void DSDlsCommand::printMultiCP(bool bOnlyUnixOrTcpIp)
{
	std::vector<Attributesp>::iterator it;
	// Set iterator to point to first element
	it = AttributesResult.begin();
	// Cycle on all attribute
	while(it != AttributesResult.end()){
		// Verify imm entry
		if (it->node != STR_DSDLS_NULL && it->side != EMPTY_VALUE && it->state != EMPTY_VALUE){
			// Check if conn-type is unix and flag is false
			if(((it->conn_type != UNIX) && (bOnlyUnixOrTcpIp == true)) || ((it->conn_type == UNIX) && (bOnlyUnixOrTcpIp == false))){
				// Check connection type
				if (it->conn_type == OCP || it->conn_type == TCPIP || it->conn_type == UNIX){
					// Connection is a OCP or TCPIP OR UNIX.
					printMultiCPEthChild(*it, true);
				}else{
					// Print in another format: without ip and port
					printMultiCPEthChild(*it, false);
				}
			}
		}
		// Next element
		++it;
	}
}

/*
 * Class Name: DSDlsCommand
 * Method Name: unixConnTypeExist
 * Description: check if unix connection type exist in main structure
 * Used: This method is used in the following member:
 *       DSDlsCommand::printStructure in acs_dsd_command.cpp file
 */
bool DSDlsCommand::unixConnTypeExist()
{
	bool bRet;
	std::vector<Attributesp>::iterator it;
	// Initialization
	bRet = false;
	// Set iterator to point to first element
	it = AttributesResult.begin();
	// Cycle on all attribute
	while((it != AttributesResult.end()) && (bRet == false)){
		// Verify imm entry
		if (it->node != STR_DSDLS_NULL && it->side != EMPTY_VALUE && it->state != EMPTY_VALUE){
			// Check if conn-type is unix
			if(it->conn_type == UNIX){
				// Set exit flag
				bRet = true;
			}
		}
		// Next element
		++it;
	}
	// Exit of method
	return(bRet);
}

/*
 * Class Name: DSDlsCommand
 * Method Name: printStructure
 * Description: Print the attributes in the structure
 * Used: This method is used in the following member:
 *       DSDlsCommand::getObject in acs_dsd_command.cpp file
 */
void DSDlsCommand::printStructure()
{
	// Check on single CPSystem
	if(CPSingle == true){
		// Print singleCP Header
		printf(STR_DSDLS_SINGLECPFMT, STR_DSDLS_NODE, STR_DSDLS_DOMAIN, STR_DSDLS_APPLICATION, STR_DSDLS_CONNTYPE,
				STR_DSDLS_PID, STR_DSDLS_IPADDRESS, STR_DSDLS_PORT);
		// Print singleCP elements exclude UNIX conn-type
		printSingleCP(true);
		// Print only singleCP UNIX conn-type
		printSingleCP(false);
	}else{
		// Print MultiCP header
		printf(STR_DSDLS_MULTICPSTART);
		// Print a CR
		printf(STR_DSDLS_CR);
		// Check format
		if(Format16 == true){
			// Print Format16 header
			printf(STR_DSDLS_MULTICPFMT16, STR_DSDLS_NODE, STR_DSDLS_SIDE, STR_DSDLS_STATE);
		}else{
			// Print complete header
			printf(STR_DSDLS_MULTICPFMT, STR_DSDLS_NODE, STR_DSDLS_SIDE, STR_DSDLS_STATE,
					STR_DSDLS_DOMAIN, STR_DSDLS_SERVICENAME, STR_DSDLS_CONNTYPE, STR_DSDLS_PID,
					STR_DSDLS_IPADDRESS, STR_DSDLS_PORT);
		}
		// Print multiCP elements exlude UNIX conn-type
		printMultiCP(true);
		// Print header if unix service exists
		if(unixConnTypeExist() == true){
			printf(STR_DSDLS_CR);
			printf(STR_DSDLS_MULTICPUNIXFMT, STR_DSDLS_NODE, STR_DSDLS_SIDE,
					STR_DSDLS_STATE, STR_DSDLS_DOMAIN, STR_DSDLS_SERVICENAME, STR_DSDLS_CONNTYPE, STR_DSDLS_PID,
					STR_DSDLS_PIPENAME);
			// Print only multiCP UNIX conn-type
			printMultiCP(false);
		}
	}
}

char * DSDlsCommand::to_upper(char * str) {
	int str_len = ::strlen(str);
	char tmp_str[str_len + 1];

	for (int i = 0; i < str_len; i++)
		tmp_str[i] = ::toupper(str[i]);
	tmp_str[str_len] = 0;

	::strncpy(str, tmp_str, str_len);

	return str;
}
