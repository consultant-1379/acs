#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_NetworkElement_Implementation.h"

using namespace std;

TEST(ACS_CS_API_NetworkElement, getneid)
{
	ACS_CS_API_Name neid;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getNEID(neid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		char neId[256];
		size_t size = sizeof(neId);
		neid.getName(neId, size);
		cout << "NEID = " << neId << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getAPGCount)
{
	uint32_t apgCount;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAPGCount(apgCount);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getAPGCount = " << apgCount << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getSingleSidedCPCount)
{
	unsigned int count = -1;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getSingleSidedCPCount(count);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getSingleSidedCPCount = " << count << endl;
	}

	EXPECT_TRUE(count >= 0);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}


TEST(ACS_CS_API_NetworkElement, getDoubleSidedCPCount)
{
	unsigned int count = -1;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getDoubleSidedCPCount(count);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getDoubleSidedCPCount = " << count << endl;
	}

	EXPECT_TRUE(count >= 0);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getFrontAPG)
{
	APID apid = 999;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getFrontAPG(apid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getFrontAPG = " << apid << endl;
	}

	EXPECT_TRUE(apid != 999);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getAlarmMaster)
{
	CPID cpid = 999;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAlarmMaster(cpid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getAlarmMaster = " << cpid << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getClockMaster)
{
	CPID cpid = 999;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getClockMaster(cpid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getClockMaster = " << cpid << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getDefaultCPName)
{
	ACS_CS_API_Name name;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getDefaultCPName(1, name);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		char defName[256];
		size_t size = sizeof(defName);
		name.getName(defName, size);
		cout << "getDefaultCPName = " << defName << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getOmProfile)
{
	ACS_CS_API_OmProfileChange omProfileChange;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getOmProfile(omProfileChange);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "aptCurrent = " << omProfileChange.aptCurrent << endl;
		cout << "aptQueued = " << omProfileChange.aptQueued << endl;
		cout << "aptRequested = " << omProfileChange.aptRequested << endl;

		cout << "apzCurrent = " << omProfileChange.apzCurrent << endl;
		cout << "apzQueued = " << omProfileChange.apzQueued << endl;
		cout << "apzRequested = " << omProfileChange.apzRequested << endl;

		cout << "omProfileCurrent = " << omProfileChange.omProfileCurrent << endl;
		cout << "omProfileRequested = " << omProfileChange.omProfileRequested << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getClusterOpMode)
{
	ACS_CS_API_ClusterOpMode::Value clusterOpMode;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getClusterOpMode(clusterOpMode);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		   switch(clusterOpMode)
		   {
		   case ACS_CS_API_ClusterOpMode::Normal:
		      cout << "getClusterOpMode = " <<  "NORMAL MODE" << endl;
		      break;

		   case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
			   cout << "getClusterOpMode = " << "SWITCHING TO NORMAL MODE" << endl;
		      break;

		   case ACS_CS_API_ClusterOpMode::Expert:
			   cout << "getClusterOpMode = " << "EXPERT MODE" << endl;
		      break;

		   case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
			   cout << "getClusterOpMode = " << "SWITCHING TO EXPERT MODE" << endl;
		      break;
		   default:
		      FAIL() << "getClusterOpMode = " << "ERROR";
		   }
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, trafficIsolatedCpId)
{
	CPID cpid = 999;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getTrafficIsolated(cpid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "trafficIsolatedCpId = " << cpid << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getTrafficLeader)
{
	CPID cpid = 999;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getTrafficLeader(cpid);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getTrafficLeader = " << cpid << endl;
	}

	EXPECT_TRUE(cpid != 999);
	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
}

TEST(ACS_CS_API_NetworkElement, getAPTType)
{
	string aptType;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getAPTType(aptType);

	if(returnValue == ACS_CS_API_NS::Result_Success){

		cout << "getAPTType = " << aptType << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
	EXPECT_TRUE(aptType.compare("HLR") == 0 || aptType.compare("BSC") == 0 || aptType.compare("WLN") == 0 || aptType.compare("MSC") == 0);
}

TEST(ACS_CS_API_NetworkElement, getNodeArchitecture)
{
	ACS_CS_API_CommonBasedArchitecture::ArchitectureValue arch;

	ACS_CS_API_NS::CS_API_Result returnValue = ACS_CS_API_NetworkElement::getNodeArchitecture(arch);

	if(returnValue == ACS_CS_API_NS::Result_Success){
		std::string architecture;
		switch(arch) {
		case ACS_CS_API_CommonBasedArchitecture::SCB:
			architecture = "SCB";
			break;
		case ACS_CS_API_CommonBasedArchitecture::SCX:
			architecture = "SCX";
			break;
		case ACS_CS_API_CommonBasedArchitecture::DMX:
			architecture = "DMX";
			break;
		default:
			break;
		}

		cout << "getNodeArchitecture = " << architecture << endl;
	}

	EXPECT_TRUE(returnValue == ACS_CS_API_NS::Result_Success);
	EXPECT_TRUE(arch == ACS_CS_API_CommonBasedArchitecture::SCB || arch == ACS_CS_API_CommonBasedArchitecture::SCX || arch == ACS_CS_API_CommonBasedArchitecture::DMX);
}

