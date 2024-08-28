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
#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_R1.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Common_R1.h"
#include "ACS_CS_API_IdList_Implementation.h"
#include "ACS_CS_ImRepository.h"

using namespace std;

class ACS_CS_API_FunctionDistribution_ImplementationTest : public ::testing::Test
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

//###################################################################//
//####   ACS_CS_API_FunctionDistribution_ImplementationTest     #####//
//###################################################################//

TEST_F(ACS_CS_API_FunctionDistribution_ImplementationTest, getFunctionNames)
{

	ACS_CS_API_NameList functionList;

	//ACS_CS_API_TestUtils::printTestHeader("FunctionDistribution:: getFunctionNames");

	CS_API_Result result = functionDistribution->getFunctionNames(functionList);

	if(result == Result_Success){

		int names = functionList.size();

		cout << "SUCCESS: getFunctionNames() returned Result_Success. Names returned = " << names << endl;

		if(names > 0)
			cout << endl << "Now fetching FunctionProviders for each name..." << endl << endl;

		for(int i = 0; i < names; i++){

			char functionName[256];
			ACS_CS_API_Name fd_name = (ACS_CS_API_Name)functionList[i];

//			size_t size = sizeof((ACS_CS_API_Name)functionList[i]);
//			functionList[i].getName(functionName, size);

			size_t size = 256;

			fd_name.getName(functionName, size);

			cout << "->FunctionName names[" << i << "] = "<< functionName << endl;

			ACS_CS_API_IdList apIdList;
			result = functionDistribution->getFunctionProviders(fd_name, apIdList);
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
