//============================================================================
// Name        : csapi_test.cpp
// Author      : pikr
// Version     :
// Copyright   : Your copyright notice
// Description : csapi_test
//============================================================================

#include "ACS_CS_API.h"
//#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_Set.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string.h>
#include <cstdlib>
#include <limits.h>
#include <arpa/inet.h>

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using std::istringstream;
using std::fstream;
using std::ios;
using std::ios_base;
using std::ostringstream;
using std::stringstream;
using std::setw;
using std::map;

vector<string *> tableLines;
int failed = 0, success = 0;		// Variables for the no of successful and failed tests
map<SysID, string> cpIdMap;
map<APID, string> apIdMap;
map<SysID, string>::iterator it;

void getNodeArchitecture();
void testHWC();
void testHWC_mag(char *argv[]);
void testNetworkElement();
void testCP();
void testFD();
void testCPG();
void testAP(char *argv[]);
void getApNum();

bool compAddress(string tableValue, unsigned long apiValue);
bool compValue(string tableValue, unsigned short apiValue);

// HWC System types
enum CS_SystemType_Identifier {
   SysType_BC =					0,
   SysType_CP =					1000,
   SysType_AP =					2000,
   SysType_NotSpecified =			0xFFFE
};
// HWC Functional Board Name Identifiers
enum CS_FBN_Identifier {
   FBN_NotSpecified =		0,
   FBN_SCBRP =                          100,
   FBN_RPBIS =                          110,
   FBN_GESB =                                 120, //CR GESB
   FBN_CPUB =                           200,
   FBN_MAUB =                           210,
   FBN_APUB =                           300,
   FBN_Disk =                           310,
   FBN_DVD	=                          320,
   FBN_GEA	=                          330
};
// HWC Side Identifiers
enum CS_Side_Identifier {
   Side_A = 0,
   Side_B = 1,
   Side_NotSpecified = 99
};
// HWC DHCP Methods
enum CS_DHCP_Method_Identifier {
   DHCP_None =                          0,
   DHCP_Normal =                1,
   DHCP_Client =                2,
   DHCP_NotSpecified =	99
};

int main(int argc, char* argv[]) {
	getNodeArchitecture();
/*
	if ( argc == 1 || argc == 2)
	{
		//if (argc > 2 || (strcmp(argv[1],"hwc")!=0 && strcmp(argv[1],"ne") && strcmp(argv[1],"cp") && strcmp(argv[1],"fd")))
		if (argc > 2 || (strcmp(argv[1],"hwc")!=0 && strcmp(argv[1],"ne") && strcmp(argv[1],"cp") && strcmp(argv[1],"fd")))
		{
			cout << endl << "csapi_test usage: " << endl << "cs_api [OPTION]" << endl << endl;
			cout << endl << "OPTION: hwc, ne, cp, fd" << endl;
			return 0;
		}
		*/
//		cout << endl << endl << "Testing boardsearch..." << endl << endl;
//		testHWC_mag(argv);
		//testHWC();

//		cout << endl << endl << "Testing Search AP board with system Number equals to 2" << endl << endl;
//		testAP(argv);

		/*cout << endl << endl << "Testing get AP Number" << endl << endl;
		getApNum();*/
		/*
		if (argc == 1 || strcmp(argv[1],"hwc") == 0)
		{
			cout << endl << endl << "Testing HWC API..." << endl << endl;
			testHWC();                     //Test HWC API
		}

		if (argc == 1 || strcmp(argv[1],"mag") == 0)
		{
			                     //Test HWC API
		}
		*/
		/*if (argc == 1 || strcmp(argv[1],"ne") == 0)
		{
			cout << endl << endl << "Testing NE API..." << endl << endl;
			testNetworkElement();	//Test NE API
		}*/
		/*
		if (argc == 1 || strcmp(argv[1],"cp") == 0)
		{
			cout << endl << endl << "Testing CP API..." << endl << endl;
			testCP();	//Test CP API
		}

		if (argc == 1 || strcmp(argv[1],"fd") == 0)
		{
			cout << endl << endl << "Testing FD API..." << endl << endl;
			testFD();	//Test FD API
		}

		if (argc == 1 || strcmp(argv[1],"cpg") == 0)
		{
			cout << endl << endl << "Testing CPG API..." << endl << endl;
			testCPG();	//Test FD API
		}
		*/
	//}
	return 0;
}

void testHWC()
{
	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	   if (hwc)
	   {

		  ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

		  if (boardSearch)
		  {

				ACS_CS_API_IdList boardList;

				(void) hwc->getBoardIds(boardList, boardSearch);
				int hits = boardList.size();
				cout << "Number of boards: " << hits << endl;
				cout << "======================================= " << endl;
				for (int i = 0; i < hits; i++)
				{
					unsigned short boardId = boardList[i];
					cout << "BoardID: " << boardId << endl;

					uint32_t mag, ipa, ipb, ala, alb, ma, mb;
					unsigned short slot, systype, sysno, FBN, side, seqno, dhcp;

					//char* tempBuffer = new char[sizeof(long)];	//creates buffer for storing the response value

					ACS_CS_API_NS::CS_API_Result result;

					result = hwc->getMagazine(mag, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char magStr[16] = {0};     //size 16, "255.255.255.255" + null character
						mag = ntohl(mag);    //changes the representation to little-endian
						sprintf(magStr, "%u.%u.%u.%u", (mag >> 24) & 0xFF,
						(mag >> 16) & 0xFF, (mag >> 8) & 0xFF, mag & 0xFF);
						cout << "  Magazine: " << magStr << endl;
					}

					result = hwc->getSlot(slot, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						cout << "  Slot: " << slot << endl;
					}

					result = hwc->getSysType(systype, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						if (systype == SysType_BC)
							cout << "  SysType: " << "BC" << endl;
						else if (systype == SysType_CP)
							cout << "  SysType: " << "CP" << endl;
						else if (systype == SysType_AP)
							cout << "  SysType: " <<  "AP" << endl;
					}

					result = hwc->getSysNo(sysno, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					   cout << "  SysNo: " << sysno << endl;
					}

					result = hwc->getFBN(FBN, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						if (FBN == FBN_SCBRP)
							cout << "  FBN: " <<  "SCB-RP" << endl;
						else if (FBN == FBN_RPBIS)
							cout << "  FBN: " << "RPBI-S" << endl;
						else if (FBN == FBN_CPUB)
							cout << "  FBN: " <<  "CPUB" << endl;
						else if (FBN == FBN_MAUB)
							cout << "  FBN: " <<  "MAUB" << endl;
						else if (FBN == FBN_APUB)
							cout << "  FBN: " <<  "APUB" << endl;
						else if (FBN == FBN_Disk)
							cout << "  FBN: " <<  "Disk" << endl;
						else if (FBN == FBN_DVD)
							cout << "  FBN: " <<  "DVD" << endl;
						else if (FBN == FBN_GEA)
							cout << "  FBN: " <<  "GEA" << endl;
						else if (FBN == FBN_GESB) //CR GESB
							cout << "  FBN: " <<  "GESB" << endl;
					}

					result = hwc->getSide(side, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					  if (side == Side_A)
						  cout << "  Side: " << "A" << endl;
					  else if (side == Side_B)
						  cout << "  Side: " << "B" << endl;
					}

					result = hwc->getSeqNo(seqno, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					   cout << "  SeqNo: " << seqno << endl;
					}

					result = hwc->getIPEthA(ipa, boardId);

					if (result == ACS_CS_API_NS::Result_Success)
					{
					    char ipaStr[INET_ADDRSTRLEN];
					    ipa  = htonl(ipa);
					    cout << "  IPEthA: " << inet_ntop(AF_INET, &ipa, ipaStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getIPEthB(ipb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char ipbStr[INET_ADDRSTRLEN];
						ipb  = htonl(ipb);
					    cout << "  IPEthB: " << inet_ntop(AF_INET, &ipb, ipbStr, INET_ADDRSTRLEN) << endl;
					}

					result = hwc->getAliasEthA(ala, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char alaStr[INET_ADDRSTRLEN];
						ala  = htonl(ala);
						cout << "  AliasEthA: " << inet_ntop(AF_INET, &ala, alaStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getAliasEthB(alb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char albStr[INET_ADDRSTRLEN];
						alb  = htonl(alb);
						cout << "  AliasEthB: " << inet_ntop(AF_INET, &alb, albStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getAliasNetmaskEthA(ma, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char maStr[INET_ADDRSTRLEN];
						ma  = htonl(ma);
						cout << "  NetmaskEthA: " << inet_ntop(AF_INET, &ma, maStr, INET_ADDRSTRLEN) << endl;
					}

					result = hwc->getAliasNetmaskEthB(mb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char mbStr[INET_ADDRSTRLEN];
						mb  = htonl(mb);
						cout << "  NetmaskEthB: " << inet_ntop(AF_INET, &mb, mbStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getDhcpMethod(dhcp, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					  if (dhcp == DHCP_None)
					  	  cout << "  DhcpMethod: " << "None" << endl;
					  else if (dhcp == DHCP_Normal)
						  cout << "  DhcpMethod: " << "Normal" << endl;
					  else if (dhcp == DHCP_Client)
						  cout << "  DhcpMethod: " << "Client" << endl;
					}

					cout << "======================================= " << endl << endl << endl;

				}
		  }
	   ACS_CS_API::deleteHWCInstance(hwc);
	   }
	   else
	   {
		   cout<<"ERROR: ACS_CS_API::createHWCInstance"<< endl;
	   }
	   return;

}

void testHWC_mag(char *argv[])
{
	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	   if (hwc)
	   {

		  ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();


		  if (boardSearch)
		  {

				ACS_CS_API_IdList boardList;
				/*uint8_t plug[4];
				uint32_t mag;
				for (int i = 0; i < 4; i++)
				   {
				      plug[i] = atoi(argv[i + 1]);
				   }
				*/
				boardSearch->setMagazine(0x01020004);
				//boardSearch->setMagazine(0x00040102);
				//boardSearch->setMagazine(0x04000201);
				//boardSearch->setMagazine(0x02010400);
				//boardSearch->setSlot(10); WORKS
				boardSearch->setSlot(0x0a);
				//boardSearch->setFBN(0xc8);
				boardSearch->setIPEthA(0x38a9a8c0); //65.169.168.192
				//boardSearch->setIPEthA(0xc0a8a938);
				/*char * value = new char[10];
				uint16_t val = 0x000a;
				//value = reinterpret_cast<char*>(&val);
				cout << "Value: " << val << endl;
				delete []value;
				*/
				//boardSearch->setSlot(atoi(argv[1]));
				(void) hwc->getBoardIds(boardList, boardSearch);
				int hits = boardList.size();
				cout << "Number of boards: " << hits << endl;
				cout << "======================================= " << endl;
				/*for (int i = 0; i < hits; i++)
				{
					unsigned short boardId = boardList[i];
					cout << "BoardID: " << boardId << endl;

					uint32_t mag, ipa, ipb, ala, alb, ma, mb;
					unsigned short slot, systype, sysno, FBN, side, seqno, dhcp;

					//char* tempBuffer = new char[sizeof(long)];	//creates buffer for storing the response value

					ACS_CS_API_NS::CS_API_Result result;

					result = hwc->getMagazine(mag, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char magStr[16];     //size 16, "255.255.255.255" + null character
						mag = ntohl(mag);    //changes the representation to little-endian
						sprintf(magStr, "%u.%u.%u.%u", (mag >> 24) & 0xFF,
						(mag >> 16) & 0xFF, (mag >> 8) & 0xFF, mag & 0xFF);
						cout << "  Magazine: " << magStr << endl;
					}

					result = hwc->getSlot(slot, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						cout << "  Slot: " << slot << endl;
					}

					result = hwc->getSysType(systype, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						if (systype == SysType_BC)
							cout << "  SysType: " << "BC" << endl;
						else if (systype == SysType_CP)
							cout << "  SysType: " << "CP" << endl;
						else if (systype == SysType_AP)
							cout << "  SysType: " <<  "AP" << endl;
					}

					result = hwc->getSysNo(sysno, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					   cout << "  SysNo: " << sysno << endl;
					}

					result = hwc->getFBN(FBN, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						if (FBN == FBN_SCBRP)
							cout << "  FBN: " <<  "SCB-RP" << endl;
						else if (FBN == FBN_RPBIS)
							cout << "  FBN: " << "RPBI-S" << endl;
						else if (FBN == FBN_CPUB)
							cout << "  FBN: " <<  "CPUB" << endl;
						else if (FBN == FBN_MAUB)
							cout << "  FBN: " <<  "MAUB" << endl;
						else if (FBN == FBN_APUB)
							cout << "  FBN: " <<  "APUB" << endl;
						else if (FBN == FBN_Disk)
							cout << "  FBN: " <<  "Disk" << endl;
						else if (FBN == FBN_DVD)
							cout << "  FBN: " <<  "DVD" << endl;
						else if (FBN == FBN_GEA)
							cout << "  FBN: " <<  "GEA" << endl;
						else if (FBN == FBN_GESB) //CR GESB
							cout << "  FBN: " <<  "GESB" << endl;
					}

					result = hwc->getSide(side, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					  if (side == Side_A)
						  cout << "  Side: " << "A" << endl;
					  else if (side == Side_B)
						  cout << "  Side: " << "B" << endl;
					}

					result = hwc->getSeqNo(seqno, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					   cout << "  SeqNo: " << seqno << endl;
					}

					result = hwc->getIPEthA(ipa, boardId);

					if (result == ACS_CS_API_NS::Result_Success)
					{
					    char ipaStr[INET_ADDRSTRLEN];
					    ipa  = htonl(ipa);
					    cout << "  IPEthA: " << inet_ntop(AF_INET, &ipa, ipaStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getIPEthB(ipb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char ipbStr[INET_ADDRSTRLEN];
						ipb  = htonl(ipb);
					    cout << "  IPEthB: " << inet_ntop(AF_INET, &ipb, ipbStr, INET_ADDRSTRLEN) << endl;
					}

					result = hwc->getAliasEthA(ala, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char alaStr[INET_ADDRSTRLEN];
						ala  = htonl(ala);
						cout << "  AliasEthA: " << inet_ntop(AF_INET, &ala, alaStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getAliasEthB(alb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char albStr[INET_ADDRSTRLEN];
						alb  = htonl(alb);
						cout << "  AliasEthB: " << inet_ntop(AF_INET, &alb, albStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getAliasNetmaskEthA(ma, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char maStr[INET_ADDRSTRLEN];
						ma  = htonl(ma);
						cout << "  NetmaskEthA: " << inet_ntop(AF_INET, &ma, maStr, INET_ADDRSTRLEN) << endl;
					}

					result = hwc->getAliasNetmaskEthB(mb, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
						char mbStr[INET_ADDRSTRLEN];
						mb  = htonl(mb);
						cout << "  NetmaskEthB: " << inet_ntop(AF_INET, &mb, mbStr, INET_ADDRSTRLEN) << endl;

					}

					result = hwc->getDhcpMethod(dhcp, boardId);
					if (result == ACS_CS_API_NS::Result_Success)
					{
					  if (dhcp == DHCP_None)
					  	  cout << "  DhcpMethod: " << "None" << endl;
					  else if (dhcp == DHCP_Normal)
						  cout << "  DhcpMethod: " << "Normal" << endl;
					  else if (dhcp == DHCP_Client)
						  cout << "  DhcpMethod: " << "Client" << endl;
					}

					cout << "======================================= " << endl << endl << endl;

				}
*/		  }
	   ACS_CS_API::deleteHWCInstance(hwc);
	   }
	   else
	   {
		   cout<<"ERROR: ACS_CS_API::createHWCInstance"<< endl;
	   }
	   return;

}

void testNetworkElement()
{
   stringstream neStringStream;
   bool multipleCPSystem = false;
   unsigned int doubleSidedCPCount = 0;
   unsigned int singleSidedCPCount = 0;
   unsigned int apCount = 0;
   string frontAP = "";
   string alarmMasterCP = "";
   string clockMasterCP = "";
   string neidNameString = "";
   string clusterModeString = "";
   string aptType = "";

   ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;

   bool testEnvironment;
   result = ACS_CS_API_NetworkElement::isTestEnvironment(testEnvironment);

   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (testEnvironment)
    	  neStringStream << "Test environment" << endl;
      else
    	  neStringStream << "Not a test environment" << endl;
   }
   else
	   neStringStream << "Failed to get test environment" << endl;

   //Get system type
   result = ACS_CS_API_NetworkElement::isMultipleCPSystem(multipleCPSystem);
   if (result == ACS_CS_API_NS::Result_Success)
   {
      if (multipleCPSystem)
    	  neStringStream << "System Type: Multiple-CP system" << endl;
      else
    	  neStringStream << "System Type: Single-CP system" << endl;
   }
   else
	   neStringStream << "Failed to get system type" << endl;


   //Get APT type
   result = ACS_CS_API_NetworkElement::getAPTType(aptType);
   if (result == ACS_CS_API_NS::Result_Success)
	   neStringStream << "APT Type: " << aptType << endl;
   else
	   neStringStream << "Failed to get APT Type" << endl;

   if (multipleCPSystem)
   {
																																														   //Get number of double sided CPs
	   result = ACS_CS_API_NetworkElement::getDoubleSidedCPCount(doubleSidedCPCount);
	   if (result == ACS_CS_API_NS::Result_Success)
		   neStringStream << "Number of Dual-Sided CPs: " << doubleSidedCPCount << endl;
	   else
		   neStringStream << "Failed to get number of double sided CPs" << endl;

	   //Get number of single sided CPs
	   result = ACS_CS_API_NetworkElement::getSingleSidedCPCount(singleSidedCPCount);
	   if (result == ACS_CS_API_NS::Result_Success)
		   neStringStream << "Number of Single-Sided CPs: " << singleSidedCPCount << endl;
	   else
		   neStringStream << "Failed to get number of single sided CPs" << endl;

	   //Get number of APs
	   result = ACS_CS_API_NetworkElement::getAPGCount(apCount);
	   if (result == ACS_CS_API_NS::Result_Success)
		   neStringStream << "Number of APs: " << apCount << endl;
	   else
		   neStringStream << "Failed to get number of APs" << endl;;


	   //Get front AP id
	   APID frontAPId;
	   result = ACS_CS_API_NetworkElement::getFrontAPG(frontAPId);
	   if (result == ACS_CS_API_NS::Result_Success)
		   neStringStream << "Front AP: AP" << frontAPId-2000 << endl;
	   else
		   neStringStream << "Failed to get front AP" << endl;


	   //Get alarm master CP
		CPID alarmMasterCPId = 0;
		result = ACS_CS_API_NetworkElement::getAlarmMaster(alarmMasterCPId);
		if (result == ACS_CS_API_NS::Result_Success)
		{
		   neStringStream << "Alarm Master CP: CP" << (alarmMasterCPId - 1000) << endl;

		}
		else
		   neStringStream << "Failed to get alarm master CP" << endl;


		//Get clock master CP
		CPID clockMasterCPId = 0;
		result = ACS_CS_API_NetworkElement::getClockMaster(clockMasterCPId);
		if (result == ACS_CS_API_NS::Result_Success)
		{
		   neStringStream << "Clock Master CP: CP" << (clockMasterCPId - 1000) << endl;

		}
		else
		   neStringStream << "Failed to get clock master CP" << endl;


	   //Get NEID
	   ACS_CS_API_Name neid;
	   result = ACS_CS_API_NetworkElement::getNEID(neid);

	   if (result == ACS_CS_API_NS::Result_Success)
	   {
		  size_t length = neid.length();
		  char * buffer = new char[length];
		  neid.getName(buffer, length);

		  neStringStream << "NEID: " << buffer << endl;

		  delete [] buffer;
	   }
	   else
		   neStringStream << "Failed to get NEID" << endl;

	   // Get omProfile
		 ACS_CS_API_OmProfileChange profileChange;
		 result = ACS_CS_API_NetworkElement::getOmProfile(profileChange);
		 if (result == ACS_CS_API_NS::Result_Success)
			 neStringStream << "Oam Profile: " << profileChange.omProfileCurrent << endl;
		 else
			 neStringStream << "Failed to get omProfile" << endl;


		 // Get ClusterOpMode only if OM Profile is not set to Blade Profile (Profile 0).
		 if (profileChange.omProfileCurrent != ACS_CS_API_OmProfileChange::BladeProfile)
		 {
			 ACS_CS_API_ClusterOpMode::Value clusterMode;
			 result = ACS_CS_API_NetworkElement::getClusterOpMode(clusterMode);
			 if (result == ACS_CS_API_NS::Result_Success)
			 {
				 switch(clusterMode)
					{
					case 0:
						clusterModeString = "NORMAL MODE";
					   break;

					case 1:
						clusterModeString = "SWITCHING TO NORMAL MODE";
					   break;

					case 2:
						clusterModeString = "EXPERT MODE";
					   break;

					case 3:
						clusterModeString = "SWITCHING TO EXPERT MODE";
					   break;
					default:
						clusterModeString = "-";
					}
			 }
			 else
				 neStringStream << "Failed to get Cluster Operation Mode" << endl;
		 }
		 else
		 {
			// OM Profile is Profile 0...just output '-'
			clusterModeString = "-";
		 }
		 neStringStream << "Cluster Operation Mode: " << clusterModeString << endl;

	   uint32_t BSOMIPethA, BSOMIPethB;
	   result = ACS_CS_API_NetworkElement::getBSOMIPAddress(BSOMIPethA, BSOMIPethB);

	   if (result == ACS_CS_API_NS::Result_Success)
	   {
		  stringstream IPethA, IPethB;
		  IPethA<<(BSOMIPethA >> 24 & 0xFF)<<"."<<(BSOMIPethA >> 16 & 0xFF)<< "."<<(BSOMIPethA >> 8 & 0xFF)<<"."<<(BSOMIPethA & 0xFF);
		  IPethB<<(BSOMIPethB >> 24 & 0xFF)<<"."<<(BSOMIPethB >> 16 & 0xFF)<< "."<<(BSOMIPethB >> 8 & 0xFF)<<"."<<(BSOMIPethB & 0xFF);
		  neStringStream<<"BSOM IP on eth A "<<IPethA.str()<<endl;
		  neStringStream<<"BSOM IP on eth B "<<IPethB.str()<<endl;
	   }
	   else
		   neStringStream<<"Failed to get BSOM IP addresses"<<endl;
   }
   neStringStream<<endl;

   //Print the NE information
   cout << neStringStream.str();
   return;
}



void testCP()
{
   stringstream neStringStream;

   ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();
   cout <<"Created CPInstance" <<endl;
      if (cpTable)
      {
         ACS_CS_API_IdList cpIds;
         ACS_CS_API_NS::CS_API_Result result;

         result = cpTable->getCPList(cpIds);

         if (result == ACS_CS_API_NS::Result_Success)
         {

        	 cout<<"Number of CPs: " << cpIds.size() << endl;
        	 cout<< "======================================= " << endl;

            for (unsigned int i = 0; i < cpIds.size(); i++)
            {
               cout<<"CPId: " << cpIds[i] << endl;

               //CPName
               ACS_CS_API_Name cpName;
               result = cpTable->getCPName(cpIds[i], cpName);
               if (result == ACS_CS_API_NS::Result_Success)
               {
            	  size_t nameLength =(size_t) cpName.length();
				  char* tempName = new char[nameLength];
				  if (tempName)
				  {
					 result = cpName.getName(tempName, nameLength);
					 if (result == ACS_CS_API_NS::Result_Success)
					 {
						 cout << "CPName: " << tempName <<endl;
					 }
					 else
					   {
						  cout<<"cpName.getName ERROR"<< endl;
					   }
					 delete [] tempName;

				  }
               }
			   else
			   {
			      cout<<"getCPName ERROR"<< endl;
			   }
               //APZ Type
               ACS_CS_API_NS::CS_API_APZ_Type apzType;
			   result = cpTable->getAPZType(cpIds[i], apzType);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"apzType: " << apzType<< endl;
			   else
			   {
			      cout<<"getAPZType ERROR"<< endl;
			   }
			   //APZ System
			   uint16_t apzSystem;
			   result = cpTable->getAPZSystem(cpIds[i], apzSystem);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"apzType: " << apzSystem<< endl;
			   else
			   {
			      cout<<"getAPZSystem ERROR"<< endl;
			   }
			   //CP Type
			   uint16_t cpType;
			   result = cpTable->getCPType(cpIds[i], cpType);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"cpType: " << cpType<< endl;
               else
               {
            	   cout<<"getCPType ERROR"<< endl;
               }
			   //CP Alias Name
			   ACS_CS_API_Name alias;
			   bool isAlias;
			   ACS_CS_API_NS::CS_API_Result result = cpTable->getCPAliasName(cpIds[i], alias, isAlias);

               if (result == ACS_CS_API_NS::Result_Success)
               {
            	  size_t nameLength =(size_t) alias.length();
            	  cout<<"alias.length: " << nameLength<< endl;
				  char* tempName = new char[nameLength];
				  if (tempName)
				  {
					 result = alias.getName(tempName, nameLength);
					 if (result == ACS_CS_API_NS::Result_Success)
					 {
						 cout << "CP alias: " << tempName <<endl;
					 }
					 else
					{
					   cout<<"getName ERROR"<< endl;
					}
					 delete [] tempName;

				  }
               }
               else
               {
            	   cout<<"getCPAliasName ERROR"<< endl;
               }


			   //CP State
			   int cpState;
			   result = cpTable->getState(cpIds[i], cpState);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"cpState: " << cpState<< endl;
			   else
				   cout<<"getState ERROR" <<  endl;

			   //Application ID
			   int appId;
			   result = cpTable->getApplicationId(cpIds[i], appId);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"appId: " << appId<< endl;
			   else
			   		cout<<"getApplicationId ERROR" <<  endl;

			   //APZ Substate
			   int apzSubstate;
			   result = cpTable->getApzSubstate(cpIds[i], apzSubstate);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"apzSubstate: " << apzSubstate<< endl;
			   else
			   		cout<<"getApzSubstate ERROR" <<  endl;

			   //APT Substate
			   int aptSubstate;
			   result = cpTable->getAptSubstate(cpIds[i], aptSubstate);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"aptSubstate: " << aptSubstate<< endl;
			   else
			   		cout<<"getAptSubstate ERROR" <<  endl;

			   //State Transition Code
			   int stateTrans;
			   result = cpTable->getStateTransition(cpIds[i], stateTrans);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"stateTrans: " << stateTrans<< endl;
			   else
			   		cout<<"getStateTransition ERROR" <<  endl;

			   //Blocking info
			   uint16_t blockingInfo;
			   result = cpTable->getBlockingInfo(cpIds[i], blockingInfo);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"blockingInfo: " << blockingInfo<< endl;
			   else
			   		cout<<"getBlockingInfo ERROR" <<  endl;

			   //CP Capacity
			   uint32_t cpCapacity;
			   result = cpTable->getCpCapacity(cpIds[i], cpCapacity);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"cpCapacity: " << cpCapacity<< endl;
			   else
			   		cout<<"getCpCapacity ERROR" <<  endl;

			   //Quorum data
			   ACS_CS_API_CpData_R1 quorumData;
			   result = cpTable->getCPQuorumData(cpIds[i], quorumData);
			   if (result == ACS_CS_API_NS::Result_Success)
				   cout<<"getCPQuorumData SUCCESS" << endl;
			   else
			   	  cout<<"getCPQuorumData ERROR" <<  endl;
               cout<< "======================================= " << endl << endl << endl;
            }
         }
         else
		   {
			  cout<<"getCPList ERROR"<< endl;
		   }

         ACS_CS_API::deleteCPInstance(cpTable);
      }
      else
	  {
	       cout<<"ERROR: ACS_CS_API::createCPInstance"<< endl;
	  }

}

void testCPG()
{

   ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();

   if (cpGroupTable)
   {
	    const char _tmpname[4] = {'A','L','L',0x0};
		ACS_CS_API_Name cpGroupName(_tmpname);
		ACS_CS_API_IdList cpList;

		//getGroupMembers
		ACS_CS_API_NS::CS_API_Result result = cpGroupTable->getGroupMembers(cpGroupName, cpList);
		if (result == ACS_CS_API_NS::Result_Success)
		{
			  cout << "ALL group members:" << endl;

			 for (unsigned int i = 0; i < cpList.size(); i++)
			 {
				CPID cpId = cpList[i];

				cout << "CPID: " << cpId << endl;
			 }
		}
		else if (result == ACS_CS_API_NS::Result_NoEntry)
		  cout << "getGroupMembers: No Entry" << endl;
		else
		  cout << "getGroupMembers: ERROR" << endl;


        //getGroupNames
      	ACS_CS_API_NameList nameList;

		result = cpGroupTable->getGroupNames(nameList);

		if (result == ACS_CS_API_NS::Result_Success)
		{
		   for (unsigned int i = 0; i < nameList.size(); i++)
		   {
			  ACS_CS_API_Name cpGroupName = nameList[i];

			  ACS_CS_API_IdList cpList;

			  result = cpGroupTable->getGroupMembers(cpGroupName, cpList);

			  if (result == ACS_CS_API_NS::Result_Success)
			  {
				 size_t groupNameLength = (size_t)cpGroupName.length();
				 char* groupNameChar = new char [groupNameLength];
				 (void)cpGroupName.getName(groupNameChar, groupNameLength);
				 cout << "CP group " << groupNameChar << " members:" << endl;

				 for (unsigned int j = 0; j < cpList.size(); j++)
				 {
					CPID cpId = cpList[j];
						cout << "  " << cpId << endl;
				 }

				 delete [] groupNameChar;
			  }
			  else
				 cout<<"getGroupMembers: ERROR"<<endl;

			  cout << endl;
		   }  //end for loop

		   cout<<endl;
		}
		else
		   cout<<"getGroupNames: ERROR"<<endl;


      ACS_CS_API::deleteCPGroupInstance(cpGroupTable);
   }
   else
	   cout << "createCPGroupInstance: ERROR" << endl;

}

void testFD()
{
   ACS_CS_API_FunctionDistribution* fdTable = ACS_CS_API::createFunctionDistributionInstance();

   if (fdTable)
   {
      ACS_CS_API_NameList funcNameList;

      if (fdTable->getFunctionNames(funcNameList) == ACS_CS_API_NS::Result_Success)
      {
         if (funcNameList.size() > 0)
         {
			 cout<<"Getting function names list SUCCESS"<< endl;
		 }
         else
         {
        	 cout<<"Function names list size ERROR"<< endl;
         }
      }
      else
	   {
		  cout<<"getFunctionNames ERROR"<< endl;
	   }


      ACS_CS_API_IdList apList;
      //const char _tmpfname[4] = {'S','T','S',0x0};
      ACS_CS_API_Name funcName("domain:service");

      if (fdTable->getFunctionProviders(funcName, apList) == ACS_CS_API_NS::Result_NoEntry)
      {
      			 cout<<"getFunctionProviders SUCCESS"<< endl;
      }
      else
	   {
		  cout<<"getFunctionProviders ERROR"<< endl;
	   }

      APID apId = 2001;
      ACS_CS_API_IdList cpList;

      if (fdTable->getFunctionUsers(apId, funcName, cpList) == ACS_CS_API_NS::Result_Success)
      {
		 cout<<"getFunctionUsers SUCCESS"<< endl;
      }
      else
      {
    	  cout<<"getFunctionUsers ERROR"<< endl;
      }

      ACS_CS_API::deleteFunctionDistributionInstance(fdTable);
   }
   else
   {
	  cout<<"ERROR: ACS_CS_API::createFunctionDistributionInstance"<< endl;
   }

}

bool compAddress(string tableValue, unsigned long apiValue)
{
	ostringstream stream;
	string apiAddress;

	stream << ( (apiValue >> 24 & 0xFF) ) << "."
		   << ( (apiValue >> 16 & 0xFF) ) << "."
		   << ( (apiValue >> 8 & 0xFF) ) << "."
		   << ( apiValue & 0xFF);

	apiAddress = stream.str();

	if (tableValue == apiAddress)
		return true;
	else
		return false;
}

bool compValue(string tableValue, unsigned short apiValue)
{
	istringstream stream(tableValue);
	unsigned short value = USHRT_MAX;

	stream >> value;

	if (value == apiValue)
		return true;
	else
		return false;
}


void testAP(char *argv[])
{
	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	   if (hwc)
	   {
		   bool found = false;
		  ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

		  if (boardSearch)
		  {
			  ACS_CS_API_IdList boardList;

			  (void) hwc->getBoardIds(boardList, boardSearch);
			  for (int i = 0; i < boardList.size(); i++)
			  {
				  unsigned short boardId = boardList[i];

				  uint32_t mag, ipa, ipb, ala, alb, ma, mb;
				  unsigned short slot, systype, sysno, FBN, side, seqno, dhcp;

				  ACS_CS_API_NS::CS_API_Result result;
				  result = hwc->getSysNo(sysno, boardId);
				  if (result == ACS_CS_API_NS::Result_Success)
				  {
					  if (sysno == 2){
						  result = hwc->getFBN(FBN, boardId);
						  if (result == ACS_CS_API_NS::Result_Success)
						  {
							  if (FBN == FBN_APUB)
							  {
								  found = true;

								  result = hwc->getMagazine(mag, boardId);
								  if (result == ACS_CS_API_NS::Result_Success)
								  {
									  char magStr[16] = {0};     //size 16, "255.255.255.255" + null character
									  mag = ntohl(mag);    //changes the representation to little-endian
									  sprintf(magStr, "%u.%u.%u.%u", (mag >> 24) & 0xFF,
											  (mag >> 16) & 0xFF, (mag >> 8) & 0xFF, mag & 0xFF);
									  cout << "  Magazine: " << magStr << endl;
								  }

								  result = hwc->getSlot(slot, boardId);
								  if (result == ACS_CS_API_NS::Result_Success)
								  {
									  cout << "  Slot: " << slot << endl;
								  }

								  result = hwc->getSide(side, boardId);
								  if (result == ACS_CS_API_NS::Result_Success)
								  {
									  if (side == Side_A)
										  cout << "  Side: " << "A" << endl;
									  else if (side == Side_B)
										  cout << "  Side: " << "B" << endl;
								  }

								  result = hwc->getIPEthA(ipa, boardId);

								  if (result == ACS_CS_API_NS::Result_Success)
								  {
									  char ipaStr[INET_ADDRSTRLEN];
									  ipa  = htonl(ipa);
									  cout << "  IPEthA: " << inet_ntop(AF_INET, &ipa, ipaStr, INET_ADDRSTRLEN) << endl;

								  }

								  result = hwc->getIPEthB(ipb, boardId);
								  if (result == ACS_CS_API_NS::Result_Success)
								  {
									  char ipbStr[INET_ADDRSTRLEN];
									  ipb  = htonl(ipb);
									  cout << "  IPEthB: " << inet_ntop(AF_INET, &ipb, ipbStr, INET_ADDRSTRLEN) << endl;
								  }

								  break;
							  }
						  }
					  }
				  }
				  if (!found) continue;
			  }
		  }

		  if (!found){
			  cout<<"AP board with sysNo 2 not found !!!"<< endl;
		  }

	   ACS_CS_API::deleteHWCInstance(hwc);
	   }
	   else
	   {
		   cout<<"ERROR: ACS_CS_API::createHWCInstance"<< endl;
	   }
	   return;

}

void getApNum()
{
	ACS_CS_API_CommonBasedArchitecture::ApgNumber number;

	ACS_CS_API_NS::CS_API_Result  returnValue = ACS_CS_API_NetworkElement::getApgNumber(number);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "SUCCESS: getApgNumber() = " << number << endl << endl;
		cout << "...as retrieved from ApzFunction attribute apNodeNumber" << endl << endl;
	}
}
void getNodeArchitecture()
{
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue node_arc;
	ACS_CS_API_NS::CS_API_Result  returnValue = ACS_CS_API_NetworkElement::getNodeArchitecture(node_arc);
	string architectureString="";
	if(returnValue == ACS_CS_API_NS::Result_Success){

	switch(node_arc)
	{
		case 0:
			architectureString = "SCB";
				break;
		case 1:
			architectureString = "SCX";
				break;
		case 2:
			architectureString = "BSP";
				break;
		case 3:
			architectureString = "VIRTUALIZED";
				break;
		case 4:
			architectureString = "SMX";
				break;	
	}
		cout << "SUCCESS: getNodeArchitecture() = " << architectureString <<"=" <<node_arc << endl << endl;
		cout << "...as retrieved from ApzFunction attribute node architecture" << endl << endl;
	}
}
