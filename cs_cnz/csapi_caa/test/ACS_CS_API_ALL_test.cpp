#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "ACS_CS_API.h"

using namespace std;
using namespace ACS_CS_API_NS;

#include <iostream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ACS_CS_Protocol.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImCpBlade.h"
#include "ACS_CC_Types.h"
//#include "ACS_CS_ImDefaultObjectCreator.h"
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_HWC_R2.h"
#include "ACS_CS_API_R1.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_IdList_Implementation.h"
#include "ACS_CS_ImRepository.h"

using namespace std;

class ACS_CS_API_TestUtils{

public:
	static void printTestHeader(string testName);
	static void printBladeInfo(ACS_CS_API_HWC * hwc, ACS_CS_API_IdList & board_list);
	static void printBladeAttributes(ACS_CS_API_HWC * hwc, ACS_CS_API_IdList & board_list);
	static string getFbnString(uint16_t param);
	static string getDhcpString(uint16_t param);
	static string getSideString(uint16_t param);
	static string getSysTypeString(uint16_t param);
};

class ACS_CS_API_CP_ImplementationTest : public ::testing::Test
{
protected:

	ACS_CS_API_CP *apiCp;
	ACS_CS_API_IdList cpIds;

	virtual void SetUp()
	{
		apiCp = ACS_CS_API::createCPInstance();

		CS_API_Result res = apiCp->getCPList(cpIds);

		if(res != ACS_CS_API_NS::Result_Success){
			FAIL() << "-ERROR: Call to getCPList() failed with error code " << res;
		}

		int numberOfCpIds = cpIds.size();

		cout << endl << "-Successfully called getCPList() and got " << numberOfCpIds << " CP id's back" << endl;

		for (unsigned int i = 0; i < numberOfCpIds; i++)
		{
			cout << "CpId[" << i << "] = " << cpIds[i] << endl;
		}

		//cout << endl;

		if(numberOfCpIds == 0){
			FAIL() << "-No CP id's where returned. Aborting...";
		}
	}

	virtual void TearDown()
	{
		delete apiCp;
	}
};

/*class ACS_CS_API_FunctionDistribution_ImplementationTest : public ::testing::Test
{
protected:

	ACS_CS_API_FunctionDistribution * functionDistribution;
	ACS_CS_API_IdList cpIds;


	virtual void SetUp()
	{
		functionDistribution = ACS_CS_API::createFunctionDistributionInstance();
	}

	virtual void TearDown()
	{
		delete functionDistribution;
	}
};
*/
class ACS_CS_API_HWC_ImplementationTest : public ::testing::Test
{
protected:

	ACS_CS_API_BoardSearch *boardSearch;
	ACS_CC_ReturnType result;
	ACS_CS_API_HWC * hwc;

	virtual void SetUp()
	{
		hwc =  ACS_CS_API::createHWCInstance();
		boardSearch = hwc->createBoardSearchInstance();
		boardSearch->reset();
	}

	virtual void TearDown()
	{
		if(boardSearch)
			delete boardSearch;
		if(hwc)
			delete hwc;
	}
};

//###################################################################//
//####               ACS_CS_API_TestUtils                       #####//
//###################################################################//

/*string ACS_CS_API_TestUtils::getDhcpString(uint16_t dhcp){

	switch(dhcp){
	case ACS_CS_Protocol::DHCP_Client: return "Client";
	case ACS_CS_Protocol::DHCP_None: return "None";
	case ACS_CS_Protocol::DHCP_Normal: return "Normal";
	case ACS_CS_Protocol::DHCP_NotSpecified: return "DHCP_NotSpecified";
	default: return "UNKNOWN DHCP OPTION VALUE";
	}
}

string ACS_CS_API_TestUtils::getSideString(uint16_t side){

	switch(side){
	case ACS_CS_Protocol::Side_A: return "A";
	case ACS_CS_Protocol::Side_B: return "B";
	default: return "UNKNOWN SIDE VALUE";
	}
}

string ACS_CS_API_TestUtils::getSysTypeString(uint16_t type){

	switch(type){
	case ACS_CS_Protocol::SysType_AP: return "AP";
	case ACS_CS_Protocol::SysType_BC: return "BC";
	case ACS_CS_Protocol::SysType_CP: return "CP";
	case ACS_CS_Protocol::SysType_NotSpecified: return "SysType_NotSpecified";
	default: return "UNKNOWN SYSTEM TYPE VALUE";
	}
}

string ACS_CS_API_TestUtils::getFbnString(uint16_t fbn){

	switch(fbn){
	case ACS_CS_Protocol::FBN_APUB: return "APUB";
	//case ACS_CS_Protocol::FBN_CETB: return "CETB";
	case ACS_CS_Protocol::FBN_CPUB: return "CPUB";
	case ACS_CS_Protocol::FBN_DVD: return "DVD";
	case ACS_CS_Protocol::FBN_Disk: return "Disk";
	case ACS_CS_Protocol::FBN_EPB1: return "EPB1";
	case ACS_CS_Protocol::FBN_GEA: return "GEA";
	case ACS_CS_Protocol::FBN_GESB: return "GESB";
	case ACS_CS_Protocol::FBN_IPTB: return "IPTB";
	case ACS_CS_Protocol::FBN_MAUB: return "MAUB";
	case ACS_CS_Protocol::FBN_RPBIS: return "RPBIS";
	case ACS_CS_Protocol::FBN_SCBRP: return "SCBRP";
	case ACS_CS_Protocol::FBN_SCXB: return "SCXB";
	case ACS_CS_Protocol::FBN_NotSpecified: return "FBN_NotSpecified";
	default: return "UNKNOWN FBN VALUE";
	}
}

void ACS_CS_API_TestUtils::printTestHeader(string testName){

	cout << endl;
	cout << "********************************************" << endl;
	cout << testName << endl;
	cout << "********************************************" << endl;
	cout << endl;
}


void ACS_CS_API_TestUtils::printBladeAttributes(ACS_CS_API_HWC * hwc, ACS_CS_API_IdList & board_list){

	int board_count = board_list.size();

	for(int i = 0; i < board_count; i++)
	{
		uint16_t dhcp_option = 0;
		uint16_t fbn = 0;
		uint16_t slot_number = 0;
		uint16_t system_id = 0;
		uint16_t system_num = 0;
		uint16_t system_type = 0;
		uint16_t node_side = 0xBAAD;
		uint32_t eth_ip = 0;
		in_addr addr = {0};
		char addr_str [16] = {0};
		bool match = false;

		cout << i+1<< ". FETCHED ATTRIBUTE VALUES FOR BOARDID " << board_list[i] << ":" << endl;

		char node_side_char = '\0';

		if (hwc->getSlot(slot_number, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getSlot() did not return Result_Success";
		else
			cout << "SLOT NO = " << slot_number << endl;

		if (hwc->getMagazine(eth_ip, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getMagazine() did not return Result_Success";
		else{
			char *mag_addr;
			struct sockaddr_in addr;
			addr.sin_addr.s_addr = eth_ip;
			mag_addr = inet_ntoa(addr.sin_addr);

			cout << "MAGAZINE = " << mag_addr << " (" << eth_ip << ")" << endl;
		}

		if (hwc->getSysType(system_type, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getSysType() did not return Result_Success";
		else
			cout << "SYSTEM TYPE = " << ACS_CS_API_TestUtils::getSysTypeString(system_type) << " (" << system_type << ")" << endl;

		if (system_type == ACS_CS_API_HWC_NS::SysType_BC)
		{
			if (hwc->getSeqNo(system_num, board_list[i]) != ACS_CS_API_NS::Result_Success)
				FAIL() << "getSeqNo() did not return Result_Success";
			else
				cout << "SYSTEM NO = " << system_num << endl;
		}
		else
		{
			if (hwc->getSide(node_side, board_list[i]) != ACS_CS_API_NS::Result_Success)
				FAIL() << "getSide() did not return Result_Success";
			else
				cout << "NODE SIDE = " << ACS_CS_API_TestUtils::getSideString(node_side) << " (" << node_side << ")" << endl;

			if (hwc->getSysNo(system_num, board_list[i]) != ACS_CS_API_NS::Result_Success)
				FAIL() << "getSysNo() did not return Result_Success";
			else
				cout << "SYSTEM NO = " << system_num << endl;
		}

		if (hwc->getSysId(system_id, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getSysId() did not return Result_Success";
		else
			cout << "SYSTEM ID = " << system_id << endl;

		if (hwc->getFBN(fbn, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getFBN() did not return Result_Success";
		else{
			cout << "FBN = " << ACS_CS_API_TestUtils::getFbnString(fbn) << " (" << fbn << ")" << endl;
		}

		if (hwc->getDhcpMethod(dhcp_option, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getDhcpMethod() did not return Result_Success";
		else
			cout << "DHCP OPTION = " << ACS_CS_API_TestUtils::getDhcpString(dhcp_option) << " (" << dhcp_option << ")" << endl;

		ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

		if ((cs_call_result = hwc->getAliasEthA(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasEthA returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getAliasEthA() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS ETH-A = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if ((cs_call_result = hwc->getAliasEthB(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasEthB returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getAliasEthB() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS ETH-B = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if ((cs_call_result = hwc->getAliasNetmaskEthA(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasNetmaskEthA returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() << "getAliasNetmaskEthA() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS NETMASK ETH-A = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if ((cs_call_result = hwc->getAliasNetmaskEthB(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasNetmaskEthB returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() << "getAliasNetmaskEthB() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS NETMASK ETH-B = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if (((cs_call_result = hwc->getIPEthA(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
			FAIL() <<  "getIPEthA() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getIPEthA() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ETH-A = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if (((cs_call_result = hwc->getIPEthB(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
			FAIL() <<  "getIPEthB() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getIPEthB() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ETH-B = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		cout << endl;
	}

}

void ACS_CS_API_TestUtils::printBladeInfo(ACS_CS_API_HWC * hwc, ACS_CS_API_IdList & board_list){

	int board_count = board_list.size();

	for(int i = 0; i < board_count; i++)
	{
		char *mag_addr;
		uint16_t fbn = 0;
		uint16_t slot_number = 0;
		uint32_t eth_ip_Mag = 0;
		uint32_t eth_ip_A = 0;
		uint32_t eth_ip_B = 0;
		in_addr addr = {0};
		char addr_str_A [16] = {0};
		char addr_str_B [16] = {0};
		bool match = false;
		ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

		char node_side_char = '\0';

		if (hwc->getSlot(slot_number, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getSlot() did not return Result_Success";

		if (hwc->getMagazine(eth_ip_Mag, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getMagazine() did not return Result_Success";
		else{
			struct sockaddr_in addr;
			addr.sin_addr.s_addr = eth_ip_Mag;
			mag_addr = inet_ntoa(addr.sin_addr);
		}

		if (hwc->getFBN(fbn, board_list[i]) != ACS_CS_API_NS::Result_Success)
			FAIL() << "getFBN() did not return Result_Success";

		if (((cs_call_result = hwc->getIPEthA(eth_ip_A, board_list[i])) == ACS_CS_API_NS::Result_NoValue))
			FAIL() <<  "getIPEthA() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getIPEthA() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip_A);
			::inet_ntop(AF_INET, &addr, addr_str_A, sizeof(addr_str_A));
		}

		if (((cs_call_result = hwc->getIPEthB(eth_ip_B, board_list[i])) == ACS_CS_API_NS::Result_NoValue))
			FAIL() <<  "getIPEthB() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result != ACS_CS_API_NS::Result_Success)
			FAIL() <<  "getIPEthB() did not return Result_Success";
		else{
			addr.s_addr = htonl(eth_ip_B);
			::inet_ntop(AF_INET, &addr, addr_str_B, sizeof(addr_str_B));
		}

		cout << i+1 << ". BOARDID " << board_list[i] << ": FBN=" << ACS_CS_API_TestUtils::getFbnString(fbn) << " MAG=" << mag_addr << " SLOT=" << slot_number << endl;
		cout << " IP ETH-A=" << addr_str_A << " IP ETH-B=" << addr_str_B << endl;
		cout << endl;
	}

}

//###################################################################//
//####        ACS_CS_API_CP_ImplementationTest                  #####//
//###################################################################//

TEST_F(ACS_CS_API_CP_ImplementationTest, getCPName)
{
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getCPName");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		ACS_CS_API_Name name1;
		res = apiCp->getCPName((CPID)cpIds[i], name1);

		char cpName[256];
		size_t size = sizeof(name1);
		name1.getName(cpName, size);

		if(res == Result_Success){
			cout << "-SUCCESS: getCPName("  <<  cpIds[i]<< ") = " << cpName << endl;
		}
		else if (res == Result_NoValue){
			FAIL() << "-ERROR: getCPName(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			FAIL() << "-ERROR: Call to getCPName(" << cpIds[i] << ") Failed";
		}

		cout << endl << "-Verifying CP name by calling getCPId(" << cpName << ")..." << endl;

		CPID cpId = 0;
		ACS_CS_API_Name name2;
		name2.setName(cpName);
		res = apiCp->getCPId(name2, cpId);

		if(res == Result_Success){
			if(cpId == cpIds[i]){
				cout << "-SUCCESS: getCPId("  <<  cpName<< ") returned " << cpId <<  " which is equal to original CP id " << cpIds[i] << endl;
			}
			else{
				FAIL() << "-ERROR: getCPId("  <<  cpName<< ") returned " << cpId <<  " which is NOT equal to original CP id " << cpIds[i];
			}

		}
		else if (res == Result_NoValue){
			FAIL() << "-ERROR: getCPId(" << cpName << ") returned code Result_NoValue";
		}
		else{
			FAIL() << "-ERROR: Call to getCPId(" << cpName << ") Failed";
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getAPZSystem)
{

	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getAPZSystem");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		uint16_t apzSystem = 0;

		res = apiCp->getAPZSystem((CPID)cpIds[i], apzSystem);

		if(res == Result_Success){
			cout << "SUCCESS: getAPZSystem("  <<  cpIds[i]<< ") = " << apzSystem << endl;
		}
		else if (res == Result_NoEntry){
			FAIL() << "ERROR: getAPZSystem(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else{
			FAIL() << "ERROR: Call to getAPZSystem(" << cpIds[i] << ") Failed";
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}


TEST_F(ACS_CS_API_CP_ImplementationTest, getAPZType)
{
	string apzType = "";
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getAPZType");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{

		ACS_CS_API_NS::CS_API_APZ_Type type;

		res = apiCp->getAPZType((CPID)cpIds[i], type);

		if(res == Result_Success){

			switch(type){
			case APZ21401:
				apzType = "APZ21401";
				break;
			case APZ21255:
				apzType = "APZ21255";
				break;
			default:
				apzType = "UNDEFINED";
			}

			cout << "SUCCESS: getAPZType("  <<  cpIds[i]<< ") = " << apzType << endl;
		}
		else if (res == Result_NoEntry){
			FAIL() << "ERROR: getAPZType(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else{
			FAIL() << "ERROR: Call to getAPZType(" << cpIds[i] << ") Failed!\n(Check that apzSystem attribute is set to 21255 or 21401)";
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getCPType)
{
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getCPType");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		uint16_t cpType = 0;

		res = apiCp->getCPType((CPID)cpIds[i], cpType);

		if(res == Result_Success){
			cout << "SUCCESS: getCPType("  <<  cpIds[i]<< ") = " << cpType << endl;
		}
		else if (res == Result_NoEntry){
			FAIL() << "ERROR: getCPType(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else{
			FAIL() << "ERROR: Call to getCPType(" << cpIds[i] << ") Failed";
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getCPAliasName)
{
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getCPAliasName");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		bool isAlias = false;
		ACS_CS_API_Name alias;

		res = apiCp->getCPAliasName ((CPID)cpIds[i], alias, isAlias);

		if(res == Result_Success){

			char cpName[256];
			size_t size = sizeof(alias);
			alias.getName(cpName, size);

			cout << "SUCCESS: getCPAliasName("  <<  cpIds[i]<< ") = " << cpName << " isAlias = "  << (isAlias?"YES":"NO") << endl;
		}
		else if (res == Result_NoEntry){
			FAIL() << "ERROR: getCPAliasName(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			FAIL() << "ERROR: getCPAliasName(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			FAIL() << "ERROR: Call to getCPAliasName(" << cpIds[i] << ") Failed";
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getState)
{
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getState");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		int state = 0;

		res = apiCp->getState((CPID)cpIds[i], state);

		if(res == Result_Success){

			cout << "SUCCESS: getState("  <<  cpIds[i]<< ") = " << state << endl;
			break;
		}
		else if (res == Result_NoEntry){
			cout << "ERROR: getState(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			cout << "ERROR: getState(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			cout << "ERROR: Call to getState(" << cpIds[i] << ") Failed! \n(Wrong value set? Should be between 21200 and 21299)" << endl;
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getApzSubstate)
{

	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getApzSubstate");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		ApzSubstate state = 0;

		res = apiCp->getApzSubstate((CPID)cpIds[i], state);

		if(res == Result_Success){

			cout << "SUCCESS: getApzSubstate("  <<  cpIds[i]<< ") = " << state << endl;
			break;
		}
		else if (res == Result_NoEntry){
			cout << "ERROR: getApzSubstate(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			cout << "ERROR: getApzSubstate(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			cout << "ERROR: Call to getApzSubstate(" << cpIds[i] << ") Failed! \n(Wrong value set? Should be between 0 and 255)" << endl;
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getAptSubstate)
{

	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getAptSubstate");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		AptSubstate state = 0;

		res = apiCp->getAptSubstate((CPID)cpIds[i], state);

		if(res == Result_Success){

			cout << "SUCCESS: getAptSubstate("  <<  cpIds[i]<< ") = " << state << endl;
			break;
		}
		else if (res == Result_NoEntry){
			cout << "ERROR: getAptSubstate(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			cout << "ERROR: getAptSubstate(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			cout << "ERROR: Call to getAptSubstate(" << cpIds[i] << ") Failed! \n(Wrong value set? Should be between 0 and 255)" << endl;
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getStateTransition)
{

	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getStateTransition");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		StateTransition state = 0;

		res = apiCp->getStateTransition((CPID)cpIds[i], state);

		if(res == Result_Success){

			cout << "SUCCESS: getStateTransition("  <<  cpIds[i]<< ") = " << state << endl;
			break;
		}
		else if (res == Result_NoEntry){
			cout << "ERROR: getStateTransition(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			cout << "ERROR: getStateTransition(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			cout << "ERROR: Call to getStateTransition(" << cpIds[i] << ") Failed! \n(Wrong value set? Should be between 0 and 255)" << endl;
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, getApplicationId)
{

	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("CP:: getApplicationId");

	for (unsigned int i = 0; i < cpIds.size(); i++)
	{
		ApplicationId appId = 0;

		res = apiCp->getApplicationId((CPID)cpIds[i], appId);

		if(res == Result_Success){

			cout << "SUCCESS: getApplicationId("  <<  cpIds[i]<< ") = " << appId << endl;
			break;
		}
		else if (res == Result_NoEntry){
			cout << "ERROR: getApplicationId(" << cpIds[i] << ") returned code Result_NoEntry";
		}
		else if (res == Result_NoValue){
			cout << "ERROR: getApplicationId(" << cpIds[i] << ") returned code Result_NoValue";
		}
		else{
			cout << "ERROR: Call to getApplicationId(" << cpIds[i] << ") Failed! \n(Wrong value set? Should be between 0 and 255)" << endl;
		}
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

//###################################################################//
//####      ACS_CS_API_NetworkElement_ImplementationTest        #####//
//###################################################################//

TEST(ACS_CS_API_NetworkElementTest, getneid)
{
	ACS_CS_API_Name neid;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getNEID");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getNEID(neid);

	if(returnValue == Result_Success){

		char neId[256];
		size_t size = sizeof(neId);
		neid.getName(neId, size);
		cout << "SUCCESS: getneid() = " << neId << endl << endl;

		cout << "...as retrieved from ManagedElement attribute: networkManagedElementId" << endl << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getAPGCount)
{
	uint32_t apgCount;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getAPGCount");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAPGCount(apgCount);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getAPGCount() = " << apgCount << endl << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getSingleSidedCPCount)
{
	unsigned int count = -1;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getSingleSidedCPCount");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getSingleSidedCPCount(count);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getSingleSidedCPCount() = " << count << endl << endl;
	}

	EXPECT_TRUE(count >= 0);
	EXPECT_TRUE(returnValue == Result_Success);
}


TEST(ACS_CS_API_NetworkElementTest, getDoubleSidedCPCount)
{
	unsigned int count = -1;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getDoubleSidedCPCount");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getDoubleSidedCPCount(count);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getDoubleSidedCPCount() = " << count << endl << endl;
	}

	EXPECT_TRUE(count >= 0);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getFrontAPG)
{
	APID apid = 999;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getFrontAPG");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getFrontAPG(apid);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getFrontAPG() = " << apid << endl << endl;
	}

	EXPECT_TRUE(apid == 2001);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getAlarmMaster)
{
	CPID cpid = 999;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getAlarmMaster");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAlarmMaster(cpid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "SUCCESS: getAlarmMaster() = " << cpid << endl << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getClockMaster)
{
	CPID cpid = 999;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getClockMaster");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getClockMaster(cpid);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getClockMaster() = " << cpid << endl << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getDefaultCPName)
{
	ACS_CS_API_Name name;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getDefaultCPName");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getDefaultCPName(1, name);

	if(returnValue == Result_Success){

		char defName[256];
		size_t size = sizeof(defName);
		name.getName(defName, size);
		cout << "SUCCESS: getDefaultCPName() = " << defName << endl << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getOmProfile)
{
	ACS_CS_API_OmProfileChange omProfileChange;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getOmProfile");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getOmProfile(omProfileChange);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getOmProfile() :" <<  endl << endl;
		cout << "aptCurrent = " << omProfileChange.aptCurrent << endl;
		cout << "aptQueued = " << omProfileChange.aptQueued << endl;
		cout << "aptRequested = " << omProfileChange.aptRequested << endl;

		cout << "apzCurrent = " << omProfileChange.apzCurrent << endl;
		cout << "apzQueued = " << omProfileChange.apzQueued << endl;
		cout << "apzRequested = " << omProfileChange.apzRequested << endl;

		cout << "omProfileCurrent = " << omProfileChange.omProfileCurrent << endl;
		cout << "omProfileRequested = " << omProfileChange.omProfileRequested << endl;
		cout << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getClusterOpMode)
{
	ACS_CS_API_ClusterOpMode::Value clusterOpMode;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getClusterOpMode");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getClusterOpMode(clusterOpMode);

	if(returnValue == Result_Success){

		switch(clusterOpMode)
		{
		case ACS_CS_API_ClusterOpMode::Normal:
			cout << "SUCCESS: getClusterOpMode() = " <<  "NORMAL MODE" << endl << endl;
			break;

		case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
			cout << "SUCCESS: getClusterOpMode() = " << "SWITCHING TO NORMAL MODE" << endl << endl;
			break;

		case ACS_CS_API_ClusterOpMode::Expert:
			cout << "SUCCESS: getClusterOpMode() = " << "EXPERT MODE" << endl << endl;
			break;

		case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
			cout << "SUCCESS: getClusterOpMode() = " << "SWITCHING TO EXPERT MODE" << endl << endl;
			break;
		default:
			FAIL() << "ERROR: Call to getClusterOpMode() failed";
		}
	}

	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, trafficIsolatedCpId)
{
	CPID cpid = 999;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getTrafficIsolated");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getTrafficIsolated(cpid);

	if(returnValue == Result_Success){

		cout << "SUCCESS: trafficIsolatedCpId() = " << cpid << endl << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getTrafficLeader)
{
	CPID cpid = 999;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getTrefficLeader");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getTrafficLeader(cpid);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getTrafficLeader() = " << cpid << endl << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == Result_Success);
}

TEST(ACS_CS_API_NetworkElementTest, getAPTType)
{
	string aptType;

	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getAPTType");

	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAPTType(aptType);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getAPTType() = " << aptType << endl << endl;
		cout << "...as retrieved from ApzFunction attribute aptType" << endl << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
	EXPECT_TRUE(aptType.compare("HLR") == 0 || aptType.compare("BSC") == 0 || aptType.compare("WLN") == 0 || aptType.compare("MSC") == 0);
}


TEST(ACS_CS_API_NetworkElementTest, getApgNumber)
{
	ACS_CS_API_TestUtils::printTestHeader("NetworkElement:: getApgNumber");
	ACS_CS_API_CommonBasedArchitecture::ApgNumber number;


	CS_API_Result returnValue = ACS_CS_API_NetworkElement::getApgNumber(number);

	if(returnValue == Result_Success){

		cout << "SUCCESS: getApgNumber() = " << number << endl << endl;
		cout << "...as retrieved from ApzFunction attribute apNodeNumber" << endl << endl;
	}

	EXPECT_TRUE(returnValue == Result_Success);
	EXPECT_TRUE(number == ACS_CS_API_CommonBasedArchitecture::AP1 || number == ACS_CS_API_CommonBasedArchitecture::AP2);
}

//###################################################################//
//####   ACS_CS_API_FunctionDistribution_ImplementationTest     #####//
//###################################################################//

TEST_F(ACS_CS_API_FunctionDistribution_ImplementationTest, getFunctionNames)
{

	ACS_CS_API_NameList functionList;

	ACS_CS_API_TestUtils::printTestHeader("FunctionDistribution:: getFunctionNames");

	CS_API_Result result = functionDistribution->getFunctionNames(functionList);

	if(result == Result_Success){

		int names = functionList.size();

		cout << "SUCCESS: getFunctionNames() returned Result_Success. Names returned = " << names << endl;

		if(names > 0)
			cout << endl << "Now fetching FunctionProviders for each name..." << endl << endl;

		for(int i = 0; i < names; i++){

			char functionName[256];
			size_t size = sizeof((ACS_CS_API_Name)functionList[i]);
			functionList[i].getName(functionName, size);

			cout << "->FunctionName names[" << i << "] = "<< functionName << endl;

			ACS_CS_API_IdList apIdList;
			result = functionDistribution->getFunctionProviders((ACS_CS_API_Name)functionList[i], apIdList);
			int aps = apIdList.size();

			if(result == Result_Success){

				cout << endl << "-->SUCCESS: Now fetching FunctionUsers for each provider..." << endl << endl;

				for(int ii = 0; ii < aps; ii++){

					cout << "-->FunctionProvider apId[" << ii << "] = "<< apIdList[ii] << endl;

					ACS_CS_API_IdList cpIdList;
					result = functionDistribution->getFunctionUsers((APID)apIdList[ii], (ACS_CS_API_Name)functionList[i], cpIdList);
					int cps = cpIdList.size();

					if(result == Result_Success){

						cout << endl << "--->SUCCESS: Listing FunctionUsers..." << endl << endl;

						for(int iii = 0; iii < cps; iii++){

							cout << "--->FunctionUser cpId[" << iii << "] = "<< cpIdList[iii] << endl;
						}
					}
					else{
						FAIL () << "--->ERROR: getFunctionUsers(" << apIdList[ii] << ", " << functionName << ", &cpIdList" << ") returned failure";
					}
				}
			}
			else{
				FAIL () << "-->ERROR: getFunctionProviders(" << functionName << ", &apIdList" << ") returned failure";
			}
		}
	}
	else if (result == Result_NoValue){
		FAIL() << "ERROR: getFunctionNames() returned code Result_NoValue";
	}
	else if (result == Result_NoEntry){
		FAIL() << "ERROR: getFunctionNames() returned code Result_NoEntry";
	}
	else{
		FAIL() << "ERROR: getFunctionNames() returned failure!";
	}

	cout << endl;

	EXPECT_TRUE(result == Result_Success);
}

//###################################################################//
//####           ACS_CS_API_HWC_ImplementationTest              #####//
//###################################################################//

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchNoFilter)
{

	ACS_CS_API_IdList boardList;
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: No filter (all)");

	res = hwc->getBoardIds(boardList, boardSearch);

	if(res != ACS_CS_API_NS::Result_Success){
		FAIL() << "ERROR: getBoardIds() returned failure!";
	}

	int boardcount = boardList.size();

	if(boardcount > 0){
		cout << "getBoardIds() returned " << boardcount << " boards!" << endl << endl;

		ACS_CS_API_TestUtils::printBladeAttributes(hwc, boardList);
	}
	else{
		FAIL() <<  "ERROR: No boards returned from getBoardIds()" << endl;
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetMagazine)
{
	boardSearch->setMagazine(0x04000201);  // 1.2.0.4

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetMagazine");

	ACS_CS_API_IdList boardList;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setMagazine(0x04000201);  // 1.2.0.4" << endl << endl;

	res = hwc->getBoardIds(boardList, boardSearch);
	int boardcount = boardList.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(boardcount> 0){

			cout << "getBoardIds() returned " << boardcount << " boards!" << endl << endl;
			ACS_CS_API_TestUtils::printBladeInfo(hwc, boardList);
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given magazine!";
		}
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(boardcount > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetSlot)
{
	uint16_t slot_in = 14;

	boardSearch->setSlot(slot_in);

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetSlot");

	ACS_CS_API_IdList boardList;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setSlot(" << slot_in << ");" << endl << endl;

	res = hwc->getBoardIds(boardList, boardSearch);
	int boardcount = boardList.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(boardcount> 0){

			cout << "getBoardIds() returned " << boardcount << " boards!" << endl << endl;

			for (unsigned int i = 0; i < boardcount; i++)
			{
				cout << "boardList [" << i << "] = " << boardList[i] << ". Fetching slot for comparison..." << endl;

				uint16_t slot_out = 0;
				hwc->getSlot(slot_out, boardList[i]);

				if(res == ACS_CS_API_NS::Result_Success){
					cout << "-->hwc->getSlot() returned slotNumber " << slot_out << " for boardId " << boardList[i] << endl << endl;

					if(slot_in != slot_out){
						FAIL() << "SlotNumbers in and out does not match!";
					}
					else{
						cout << "SlotNumbers match! IN: " << slot_in << " OUT: " << slot_out << endl;
					}
				}
				else{
					FAIL() << "-->ERROR: hwc->getSlot() for boardId " << boardList[i] << " returned failure!";
				}
			}
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given slot number!";
		}
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(boardcount > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetSlotAndMagazine)
{
	uint16_t slot_in = 14;
	uint32_t mag_in = 0x04000201;

	boardSearch->setSlot(slot_in);
	boardSearch->setMagazine(mag_in);  // 1.2.0.4

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: Set Slot and Magazine");

	ACS_CS_API_IdList boardList;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setSlot(" << slot_in << ");" << endl;
	cout << "boardSearch->setMagazine(0x04000201);  // 1.2.0.4" << endl << endl;

	res = hwc->getBoardIds(boardList, boardSearch);
	int boardcount = boardList.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(boardcount> 0){

			cout << "getBoardIds() returned " << boardcount << " boards!" << endl << endl;

			for (unsigned int i = 0; i < boardcount; i++)
			{
				cout << "boardList [" << i << "] = " << boardList[i] << ". Fetching slot for comparison..." << endl;

				uint16_t slot_out = 0;
				uint32_t mag_out = 0;
				res = hwc->getSlot(slot_out, boardList[i]);
				res = hwc->getMagazine(mag_out, boardList[i]);

				if(res == ACS_CS_API_NS::Result_Success){
					cout << "-->hwc->getMagazine() returned address " << mag_out << " for boardId " << boardList[i] << endl;
					cout << "-->hwc->getSlot() returned slotNumber " << slot_out << " for boardId " << boardList[i] << endl << endl;

					bool match = true;

					if(slot_in != slot_out){
						cout << "SlotNumbers in and out does not match!" <<  slot_in << " != " << slot_out << endl;
						match = false;
					}
					if(mag_in != mag_out){
						cout << "Magazine address in and out does not match!" <<  mag_in << " != " << mag_out << endl;
						match = false;
					}

					if(match){
						char *mag_addr;
						struct sockaddr_in addr;
						addr.sin_addr.s_addr = mag_out;
						mag_addr = inet_ntoa(addr.sin_addr);

						cout << "SlotNumbers match! IN: " << slot_in << " OUT: " << slot_out << endl;
						cout << "MagAddress match? IN: " << mag_in << " OUT: " << mag_out << " (" << mag_addr << ")" << endl;
					}
					else{
						FAIL() << "Comparison failed!";
					}
				}
				else{
					FAIL() << "-->ERROR: hwc->getSlot() for boardId " << boardList[i] << " returned failure!";
				}
			}
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given magazine!";
		}
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(boardcount > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetSysType)
{
	uint16_t p_out = DOUBLE_SIDED_CP;
	boardSearch->setSysType(p_out);

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetSysType");

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setSysType(DOUBLE_SIDED_CP);" << endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count> 0){

			cout << "getBoardIds() returned " << board_count << " boards!" << endl << endl;
			ACS_CS_API_TestUtils::printBladeInfo(hwc, board_list);
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given SysType!";
		}
	}

	cout << "Comparing results by fetching search params for each result..." << endl;

	for(int i = 0; i < board_count; i++)
	{
		uint16_t p_in = 0;
		hwc->getSysType(p_in, board_list[i]);

		if(p_out == p_in)
			cout << i+1 << ". SYSTYPE IN match fetched SYSTYPE: " << ACS_CS_API_TestUtils::getSysTypeString(p_out) << "(" << p_out  << ")" << endl;
		else
			FAIL() << i+1 << ". SYSTYPE IN does not match fetched SYSTYPE! ("  << p_out << "!=" << p_in << endl;
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(board_count > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetSysNo)
{
	uint16_t p_out = 1;
	boardSearch->setSysNo(p_out);

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetSysNo");

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setSysNo(1);" << endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count> 0){

			cout << "getBoardIds() returned " << board_count << " boards!" << endl << endl;
			ACS_CS_API_TestUtils::printBladeInfo(hwc, board_list);
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given SysNo!";
		}
	}

	cout << "Comparing results by fetching search params for each result..." << endl;

	for(int i = 0; i < board_count; i++)
	{
		uint16_t p_in = 0;
		hwc->getSysNo(p_in, board_list[i]);

		if(p_out == p_in)
			cout << i+1 << ". SYSNO IN match fetched SYSNO: " << p_out << endl;
		else
			FAIL() << i+1 << ". SYSNO IN does not match fetched SYSNO! ("  << p_out << "!=" << p_in << endl;
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(board_count > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetSide)
{
	uint16_t p_out = A;
	boardSearch->setSide(p_out);

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetSide");

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setSide(A);" << endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count> 0){

			cout << "getBoardIds() returned " << board_count << " boards!" << endl << endl;
			ACS_CS_API_TestUtils::printBladeInfo(hwc, board_list);
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given side!";
		}
	}

	cout << "Comparing results by fetching search params for each result..." << endl;

	for(int i = 0; i < board_count; i++)
	{
		uint16_t p_in = 0;
		hwc->getSide(p_in, board_list[i]);

		if(p_out == p_in)
			cout << i+1 << ". SIDE IN match fetched SIDE: " << ACS_CS_API_TestUtils::getSideString(p_out) << "(" << p_out  << ")" << endl;
		else
			FAIL() << i+1 << ". SIDE IN does not match fetched SIDE! ("  << p_out << "!=" << p_in << endl;
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(board_count > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetDhcpMethod)
{
	uint16_t p_out = DHCP_CLIENT;
	boardSearch->setDhcpMethod(p_out);

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetDhcpMethod");

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setDhcpMethod(DHCP_CLIENT);" << endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count> 0){

			cout << "getBoardIds() returned " << board_count << " boards!" << endl << endl;
			ACS_CS_API_TestUtils::printBladeInfo(hwc, board_list);
		}
		else{
			FAIL() << "ERROR: getBoardIds() did not return any boards for given DHCP method!";
		}
	}

	cout << "Comparing results by fetching search params for each result..." << endl;

	for(int i = 0; i < board_count; i++)
	{
		uint16_t p_in = 0;
		hwc->getDhcpMethod(p_in, board_list[i]);

		if(p_out == p_in)
			cout << i+1 << ". DHCP METHOD IN match fetched DHCP METHOD: " << ACS_CS_API_TestUtils::getDhcpString(p_out) << "(" << p_out  << ")" << endl;
		else
			FAIL() << i+1 << ". DHCP METHOD IN does not match fetched DHCP METHOD! ("  << p_out << "!=" << p_in << endl;
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_TRUE(board_count > 0);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetFBN)
{
	uint16_t p_out = CPUB;
	ACS_CS_API_IdList board_list;
	CS_API_Result res = Result_Failure;

	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetFBN");

	boardSearch->setFBN(p_out);

	cout << "boardSearch->setFBN(CPUB);" << endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);

	if(res != ACS_CS_API_NS::Result_Success){
		FAIL() << "ERROR: getBoardIds() returned failure!";
	}

	int board_count = board_list.size();

	if(board_count > 0){
		cout << "getBoardIds() returned " << board_count << " boards! Fetching some attributes for each board..." << endl << endl;
	}
	else{
		FAIL() <<  "ERROR: No boards returned from getBoardIds()" << endl;
	}

	ACS_CS_API_TestUtils::printBladeAttributes(hwc, board_list);

	cout << "Comparing results by fetching search params for each result..." << endl;

	for(int i = 0; i < board_count; i++)
	{
		uint16_t p_in = 0;
		hwc->getFBN(p_in, board_list[i]);

		if(p_out == p_in)
			cout << i+1 << ". FBN IN match fetched FBN: " << ACS_CS_API_TestUtils::getFbnString(p_out) << "(" << p_out  << ")" << endl;
		else
			FAIL() << i+1 << ". FBN IN does not match fetched FBN! ("  << p_out << "!=" << p_in << endl;
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetIPEthA)
{
	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetIPEthA");

	uint32_t eth_ip_IN = 0xC0A8A901;
	boardSearch->setIPEthA(eth_ip_IN);  // 192.168.169.1

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setIPEthA(0xC0A8A901); // 192.168.169.1 (" << eth_ip_IN << ")"<< endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count > 0)
			cout << "getBoardIds() returned " << board_count << " boards! Fetching some attributes for each board..." << endl << endl;
		else
			FAIL() <<  "ERROR: No boards returned from getBoardIds()" << endl;
	}
	else{
		FAIL() << "ERROR: getBoardIds() returned failure!";
	}

	ACS_CS_API_TestUtils::printBladeAttributes(hwc,board_list);

	for(int i = 0; i < board_count; i++)
	{
		uint32_t eth_ip = 0;
		hwc->getIPEthA(eth_ip, board_list[i]);

		if(eth_ip == eth_ip_IN)
			cout << i+1 << ". IP ETH-A IN match fetched IP ETH-A: " << eth_ip  << endl;
		else
			FAIL() << i+1 << ". IP ETH-A IN does not match fetched IP ETH-A!";
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_EQ(1, board_count);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, boardSearchSetIPEthB)
{
	ACS_CS_API_TestUtils::printTestHeader("BoardSearch:: SetIPEthB");

	uint32_t eth_ip_IN = 0xC0A8AA02;
	boardSearch->setIPEthB(eth_ip_IN);  // 192.168.169.2

	ACS_CS_API_IdList board_list;
	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_NoValue;

	cout << "boardSearch->setIPEthB(0xC0A8AA02); // 192.168.170.2 (" << eth_ip_IN << ")"<< endl << endl;

	res = hwc->getBoardIds(board_list, boardSearch);
	int board_count = board_list.size();

	if(res == ACS_CS_API_NS::Result_Success){

		if(board_count > 0)
			cout << "getBoardIds() returned " << board_count << " boards! Fetching some attributes for each board..." << endl << endl;
		else
			FAIL() <<  "ERROR: No boards returned from getBoardIds()" << endl;
	}
	else{
		FAIL() << "ERROR: getBoardIds() returned failure!";
	}

	ACS_CS_API_TestUtils::printBladeAttributes(hwc,board_list);

	for(int i = 0; i < board_count; i++)
	{
		uint32_t eth_ip = 0;
		hwc->getIPEthB(eth_ip, board_list[i]);

		if(eth_ip == eth_ip_IN)
			cout << i+1 << ". IP ETH-B IN match fetched IP ETH-B: " << eth_ip  << endl;
		else
			FAIL() << i+1 << ". IP ETH-B IN does not match fetched IP ETH-B!";
	}

	cout << endl;

	// Assert
	EXPECT_EQ(ACS_CS_API_NS::Result_Success, res);
	EXPECT_EQ(1, board_count);
}

TEST_F(ACS_CS_API_HWC_ImplementationTest, dsd_load_ap_nodes_from_cs)
{

	ACS_CS_API_TestUtils::printTestHeader("DSD:: load_ap_nodes_from_cs");

	ACS_CS_API_NS::CS_API_Result res = ACS_CS_API_NS::Result_Success;

	ACS_CS_API_IdList board_list;
	unsigned board_count = 0;
	unsigned total_board_count = 0;
	unsigned system_type = ACS_CS_API_HWC_NS::SysType_AP;


	// set Board search criteria
	boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);
	boardSearch->setSysType(ACS_CS_API_HWC_NS::SysType_AP);

	cout << "boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_APUB);" << endl;
	cout << "boardSearch->setSysType(ACS_CS_API_HWC_NS::SysType_AP);" << endl << endl;

	// get BoardList for selected criteria
	if ((res = hwc->getBoardIds(board_list, boardSearch)) == ACS_CS_API_NS::Result_Success)
	{
		board_count = board_list.size();

		if(board_count == 0)
			FAIL() << "ERROR: getBoardIds() did not return any boards";
	}
	else{
		FAIL() << "ERROR: getBoardIds() returned failure";
	}

	for(int i = 0; i < board_count; i++)
	{

		uint16_t system_id = 0;
		uint16_t system_num = 0;
		uint16_t node_side = 0xBAAD;
		uint32_t eth_ip = 0;
		in_addr addr = {0};
		char addr_str [16] = {0};

		cout << "FETCHED VALUES FOR BOARDID " << board_list[i] << ":" << endl;

		char node_side_char = '\0';

		if (hwc->getSysId(system_id, board_list[i]) ^ ACS_CS_API_NS::Result_Success){
			FAIL() << "acs_dsd::ERR_CS_GET_SYSTEM_ID";
		}
		else{
			cout << "system_id = " << system_id << endl;
		}

		if (system_type ^ ACS_CS_API_HWC_NS::SysType_BC)
		{
			if (hwc->getSysNo(system_num, board_list[i]) ^ ACS_CS_API_NS::Result_Success){
				FAIL() << "acs_dsd::ERR_CS_GET_SYSTEM_NUMBER";
			}
			else{
				cout << "system_num = " << system_num << endl;
			}
			if (hwc->getSide(node_side, board_list[i]) ^ ACS_CS_API_NS::Result_Success){
				FAIL() <<  "acs_dsd::ERR_CS_GET_SIDE";
			}
			else{
				cout << "node_side = " << node_side << endl;
			}
		} else {
			if (hwc->getSeqNo(system_num, board_list[i]) ^ ACS_CS_API_NS::Result_Success){
				FAIL() << "acs_dsd::ERR_CS_GET_SYSTEM_SEQUENCE";
			}
			else{
				cout << "system_num = " << system_num << endl;
			}
		}

		ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_Success;

		if (((cs_call_result = hwc->getIPEthA(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
			FAIL() <<  "getIPEthA() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result ^ ACS_CS_API_NS::Result_Success)
			FAIL() <<  "acs_dsd::ERR_CS_GET_IP_ETH_A";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ETH-A = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if (((cs_call_result = hwc->getIPEthB(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue) && (system_type == ACS_CS_API_HWC_NS::SysType_AP))
			FAIL() <<  "getIPEthB() returned ACS_CS_API_NS::Result_NoValue";
		else if (cs_call_result ^ ACS_CS_API_NS::Result_Success)
			FAIL() << "acs_dsd::ERR_CS_GET_IP_ETH_B";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ETH-B = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if ((cs_call_result = hwc->getAliasEthA(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasEthA returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result ^ ACS_CS_API_NS::Result_Success)
			FAIL() << "acs_dsd::ERR_CS_GET_ALIAS_ETH_A";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS ETH-A = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		if ((cs_call_result = hwc->getAliasEthB(eth_ip, board_list[i])) == ACS_CS_API_NS::Result_NoValue)
			cout << "hwc->getAliasEthB returned ACS_CS_API_NS::Result_NoValue" << endl;
		else if (cs_call_result ^ ACS_CS_API_NS::Result_Success)
			FAIL() <<  "acs_dsd::ERR_CS_GET_ALIAS_ETH_B";
		else{
			addr.s_addr = htonl(eth_ip);
			::inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
			cout << "IP ALIAS ETH-B = " <<  addr_str << " (" << eth_ip << ")" << endl;
		}

		cout << endl;
	}

	cout << endl;

	EXPECT_TRUE(res == Result_Success);
}

TEST_F(ACS_CS_API_CP_ImplementationTest, mml_load_cp_names_from_cs) {

	const char* cp_name = "CP1";
	bool cp_in_group = false;

	ACS_CS_API_CPGroup* cpGroup = ACS_CS_API::createCPGroupInstance();


	if (cpGroup && apiCp) {

		ACS_CS_API_NameList nameList;
		ACS_CS_API_NS::CS_API_Result result;

		// Get list of all CP groups
		result = cpGroup->getGroupNames(nameList);

		if (result == ACS_CS_API_NS::Result_Success) { // The list was successfully retrieved


			// Loop through list
			for (unsigned int i = 0; i < nameList.size(); i++) {

				// Get each name in the list
				ACS_CS_API_Name groupName = nameList[i];

				// Create temporary buffer
				size_t nameLength = groupName.length();
				char* cpGroupStr = new char(nameLength + 1);

				ASSERT_FALSE (0 == cpGroupStr) << "checkCPgroup() - new char* failed...";

				result = groupName.getName(cpGroupStr, nameLength);
				cpGroupStr[nameLength] = 0; // Null terminate

				// Check if cp_name is a defined group name
				if (::strcasecmp(cpGroupStr, cp_name) == 0) {
					cp_in_group = true;
					delete[] cpGroupStr;
					break;
				}

				delete[] cpGroupStr;
				if (cp_in_group)
					break;
			}

			if (!cp_in_group)
			{
				ACS_CS_API_IdList cpList;
				// Get list of all CP
				result = apiCp->getCPList(cpList);
				// The list was successfully retrieved
				if (result == ACS_CS_API_NS::Result_Success)
				{
					ACS_CS_API_Name CPName;
					ACS_CS_API_Name CPAliasName;
					bool isAlias = false;
					ACS_CS_API_NS::CS_API_Result returnValue;

					for (unsigned int u = 0; u < cpList.size(); u++)
					{
						CPID cpId = cpList[u];

						returnValue = apiCp->getCPName(cpId, CPName);
						if (returnValue == ACS_CS_API_NS::Result_Success)
						{
							char cpnamebuffer[256]={};
							size_t cpnamelength;
							result = CPName.getName(cpnamebuffer, cpnamelength);
							if (result == ACS_CS_API_NS::Result_Success)
							{
								if (::strcasecmp(cpnamebuffer, cp_name) == 0)
								{
									cp_in_group = true;
									break;
								}
							}
						}

						//Alias handling
						isAlias = false;
						returnValue = apiCp->getCPAliasName(cpId,CPAliasName,isAlias);
						if ((returnValue == ACS_CS_API_NS::Result_Success) && (isAlias))
						{
							char cpAliasnamebuffer[256]={};
							size_t cpAliasnamelength;
							result = CPAliasName.getName(cpAliasnamebuffer, cpAliasnamelength);
							if (result == ACS_CS_API_NS::Result_Success)
							{
								if (::strcasecmp(cpAliasnamebuffer, cp_name) == 0)
								{
									cp_in_group = true;
									break;
								}
							}
						}
					}//for
				}
			}
		}

		EXPECT_TRUE(cp_in_group) << "CP1 not found in CP groups";

		ACS_CS_API::deleteCPGroupInstance(cpGroup);

	}
}*/
TEST_F(ACS_CS_API_HWC_ImplementationTest, getSoftwareVersionType)
{
std::string container;
uint16_t version =0;
BoardID boardId;
ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
        if (hwc)
        {
                ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
                if (boardSearch)
                {
                        ACS_CS_API_IdList boardList;
                        boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_CMXB);
                        ACS_CS_API_NS::CS_API_Result returnValue = hwc->getBoardIds(boardList, boardSearch);
                        if (returnValue == ACS_CS_API_NS::Result_Success)
                        {
                                for (unsigned int i = 0; i < boardList.size(); i++)
                                {
                                        boardId = boardList[i];
					std::cout<<"boardid==="<<boardId<<endl;
	       				int res= hwc->getSoftwareVersionType(version,boardId);
					std::cout<<"version==="<<version<<endl;
				}
                        }
        	}
	}
	
        EXPECT_EQ(4, version);

}

TEST_F(ACS_CS_API_HWC_ImplementationTest, getSoftwareVersionType)
{
std::string container;
uint16_t version =0;
BoardID boardId;
ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
        if (hwc)
        {
                ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
                if (boardSearch)
                {
                        ACS_CS_API_IdList boardList;
                        boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_CMXB);
                        ACS_CS_API_NS::CS_API_Result returnValue = hwc->getBoardIds(boardList, boardSearch);
                        if (returnValue == ACS_CS_API_NS::Result_Success)
                        {
                                for (unsigned int i = 0; i < boardList.size(); i++)
                                {
                                        boardId = boardList[i];
					std::cout<<"boardid==="<<boardId<<endl;
	       				int res= hwc->getSoftwareVersionType(version,boardId);
					std::cout<<"version==="<<version<<endl;
				}
                        }
        	}
	}
	
        EXPECT_EQ(4, version);

}*/
TEST_F(ACS_CS_API_HWC_ImplementationTest, getSolIPEthA)
{
std::string container;
uint32_t ip=0;
int a =0;
char bgci_ip_str [2][16];
BoardID boardId;
ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance();
        if (hwc)
        {
                ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();
                if (boardSearch)
                {
                        ACS_CS_API_IdList boardList;
                        boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_CPUB);
                        ACS_CS_API_NS::CS_API_Result returnValue = hwc->getBoardIds(boardList, boardSearch);
                        if (returnValue == ACS_CS_API_NS::Result_Success)
                        {
				 std::cout <<setiosflags(ios::left)<<std::setw(15)<< "BOARD ID";
                                 std::cout <<setiosflags(ios::left)<<std::setw(19)<<"ADDRESS";
				 std::cout<< std::endl;
                                for (unsigned int i = 0; i < boardList.size(); i++)
                                {
                                        boardId = boardList[i];
					hwc->getSolIPEthA(ip,boardId);

					uint32_t address = htonl(ip);
					in_addr ip_addr;
					ip_addr.s_addr = address;
					std::cout <<setiosflags(ios::left)<<std::setw(15)<< boardId;
                                        std::cout <<setiosflags(ios::left)<<std::setw(19)<<inet_ntoa(ip_addr);
					std::cout<< std::endl;
                                        //std::cout<<"ip==="<<inet_ntoa(ip_addr)<<endl;
                                }
                        }
                }
        }
EXPECT_EQ(0,a);

}

