/* .PAGENAME
 * .HEADER
 * .LEFT_FOOTER Ericsson Telecom AB
 * .INCLUDE
 *
 * .COPYRIGHT
 *  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2010.
 *  All rights reserved.
 *
 *  The Copyright to the computer program(s) herein is the property of
 *  Ericsson Telecom AB, Sweden.
 *  The program(s) may be used and/or copied only with the written
 *  permission from Ericsson Telecom AB or in accordance with
 *  the terms and conditions stipulated in the agreement/contract under
 *  which the program(s) have been supplied.
 *
 * .DESCRIPTION
 *	This is header file for the dsdls command.
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
 */
#ifndef ACS_DSD_COMMAND_H_
#define ACS_DSD_COMMAND_H_

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include "saImm.h"
#include "saImmOi.h"
#include "saImmOm.h"
#include <ace/ACE.h>
#include <ACS_CS_API.h>
#include "ACS_DSD_Server.h"
#include "ACS_DSD_ConfigurationHelper.h"

using namespace std;

//define
#define ERR1               	"Error when executing"
#define ERR2               	"Incorrect usage"
#define ERR55               "Unable to connect to configuration server"
#define ERR56               "Configuration Server Error"
#define ERR113             	"AP is not defined"
#define ERR116             	"Illegal option in this system configuration"
#define ERR117              "Unable to connect to server"
#define ERR118             	"CP is not defined"
#define EMPTY_STRING       	""
#define DEFAULT_SIDE       	"-"
#define EMPTY_VALUE			1000
// Conn-type constants
#define OCP					1
#define TCPIP				2
#define UNIX				4
#define TCPIP_UNIX			6

#define STR_DSDLS_AP								"AP"
#define STR_DSDLS_UNDEFINED					"UNDEFINED"
#define STR_DSDLS_OCP								"OCP"
#define STR_DSDLS_TCPIP							"TCP/IP"
#define STR_DSDLS_UNIX							"UNIX"
#define STR_DSDLS_TCPIPUNIX					"TCP/IP - UNIX"
// IMM Class string
#define STR_DSDLS_SRTNODE						"SRTNode"
// Header strings
#define STR_DSDLS_MULTICPSTART			"DIRECTORY SERVICE LISTING\n"
#define STR_DSDLS_NODE							"NODE"
#define STR_DSDLS_DOMAIN						"DOMAIN"
#define STR_DSDLS_APPLICATION				"APPLICATION"
#define STR_DSDLS_CONNTYPE					"CONN-TYPE"
#define STR_DSDLS_PID								"PID"
#define STR_DSDLS_IPADDRESS					"IP ADDRESS"
#define STR_DSDLS_PORT							"PORT"
#define STR_DSDLS_SIDE							"SIDE"
#define STR_DSDLS_STATE							"STATE"
#define STR_DSDLS_SERVICENAME				"SERVICENAME"
#define STR_DSDLS_PIPENAME					"PIPENAME"
#define STR_DSDLS_CR								"\n"
// Tag strings
#define STR_DSDLS_NONE							"-"
#define STR_DSDLS_ZERO							"0"
#define STR_DSDLS_NULL							""
// Token chars
#define CHR_DSDLS_IPPORT_SEPARATOR	':'
#define CHR_DSDLS_DN_SEPARATOR			','
// Format strings
#define STR_DSDLS_SINGLECPFMT				"%-7s %-8s %-16s %-10s %-6s %-15s %-5s\n"
#define STR_DSDLS_SINGLECPUNIXFMT		"%-7s %-8s %-16s %-10s %-6s %-50s\n"
#define STR_DSDLS_MULTICPFMT				"%-7s %-4s %-12s %-8s %-16s %-10s %-6s %-15s %-5s\n"
#define STR_DSDLS_MULTICPUNIXFMT		"%-7s %-4s %-12s %-8s %-16s %-10s %-6s %-50s\n"
#define STR_DSDLS_MULTICPFMT16			"%-7s %-4s %-12s\n"

/*
 * This is the structure to store all attribute retrieved
 * from IMM, for child and father node
 */
struct Attributes {
	// father's attributes
	unsigned int    state;
	string     		srtNodeId;
	int	      		side;
	string     		node;

	// child's attributes
	int			visibility;
	vector<string>	unix_address;
	string			srvInfoId;
	string			proc_name;
	int				pid;
	string			name;
	vector<string>	inet_address;
	string			domain;
	unsigned int	conn_type;
};
typedef struct Attributes Attributesp;

/*
 * Name: DSDlsCommand
 * Description: This class create, initialize and manage the shared
 *              memory and the semaphore
 */
class DSDlsCommand
{
	//methods
	int searchChild(SaImmHandleT immHandle, SaNameT tempName, vector<SaNameT> &FobjectName);
	int getFatherAttribute(SaImmHandleT immHandle, SaNameT objectName);
	int checkFatherAttribute(SaImmHandleT immHandle, SaNameT objectName);
	int getChildAttribute(SaImmHandleT immHandle, SaNameT objectName);
	int getObject();
	int getMyApNodeSide();
	void setMyState();
	char* getNodeNamefromsrtNodeId(char* srtNodeid);
	char* getSidefromsrtNodeId(char* srtSide);
	char* getNodeNamefromObjectName(char* ObjectName);
	char* getSidefromObjectName(char* srtSide);
	char* getSide(char* side, char* newNode);
	char* getState(char* state, char* newNode);
	void getConnType(int iConnType, string *pStrOut);
	void getPid(unsigned int uiPid, const string &strNode, string *pStrOut);
	void getIp(const string &strIp, const string &strNode, string *pStrOut);
	void getPort(const string &strAddress, string *pStrOut);
	bool unixConnTypeExist();
	char* getVisibility(char* visibility);
	void fillChildStructure(SaImmAttrValuesT_2 **attributes);
	void fillFatherStructure(SaImmAttrValuesT_2 **attributes);
	void cleanStructure();
	void printStructure();
	void printMultiCP(bool bOnlyUnixOrTcpIp);
	void printSingleCP(bool bOnlyUnixOrTcpIp);
	void printEthAndChild(const Attributesp &attr, bool bOcpFrmt);
	void printMultiCPEthChild(const Attributesp &attr, bool bOcpFrmt);
	int GetCpIdentity(char* name, CPID& id);
	int GetAPId(char* name);
	int CheckProcessRunning(const char * process_name, pid_t pid);
	void getNodeIdFromDN(const SaNameT &objectName, string *pstrOut);

	//parameters
	char* Node;
	char* Side;
	char* State;
	char AP_CP[5];
	ACS_CS_API_CP* _cpTable;

public:
	DSDlsCommand(void);
	~DSDlsCommand(void);

	//methods
	void execute_cmd();
	void usage(bool CPSys);
	bool MultipleCpCheck();
	bool CheckData(const char* data);

private:
	char * to_upper(char * str);

public:
	//parameters
	char* CPname;
	char* APidentity;
	char* ServiceName;
	char* Domain;
	bool CPSingle;
	bool Format1; //dsdls
	bool Format2; //dsdls -p
	bool Format3; //dsdls -a
	bool Format4; //dsdls -s servicename
	bool Format5; //dsdls -s servicename -p
	bool Format6; //dsdls -s servicename -a
	bool Format7; //dsdls -d domain
	bool Format8; //dsdls -d domain -p
	bool Format9; //dsdls -d domain -a
	bool Format10; //dsdls -s servicename -d domain
	bool Format11; //dsdls -s servicename -d domain -p
	bool Format12; //dsdls -s servicename -d domain -a
	bool Format13; //dsdls -ap apidentity
	bool Format14; //dsdls -ap apidentity -p
	bool Format15; //dsdls -cp cpname
	bool Format16; //dsdls -n
};
#endif /* ACS_DSD_COMMAND_H_ */
