#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include "ACS_CS_API.h"

using namespace std;
using namespace ACS_CS_API_NS;


TEST(ACS_CS_API, getCPId)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 0;
	CS_API_Result res;
	ACS_CS_API_Name name;

	name.setName("CP1");

	res = apiCp->getCPId(name, id);

	if(res == Result_Success){
		cout << "getCPId = " << id << endl;
	}
	else if (res == Result_NoValue){
		cout << "getCPId : NoValue" << endl;
	}
	else{
		cout << "getCPId Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}


TEST(ACS_CS_API, getCPName)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 1001;
	CS_API_Result res;
	ACS_CS_API_Name name;

	res = apiCp->getCPName(id, name);

	char cpName[256];
	size_t size = sizeof(name);
	name.getName(cpName, size);

	if(res == Result_Success){
		cout << "getCPName = " << cpName << endl;
	}
	else if (res == Result_NoValue){
		cout << "getCPName : NoValue" << endl;
	}
	else{
		cout << "getCPName Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, getAPZSystem)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 1001;
	CS_API_Result res;
	uint16_t apzType = 0;

	res = apiCp->getAPZSystem(id, apzType);

	if(res == Result_Success){
		cout << "getAPZSystem = " << apzType << endl;
	}
	else if (res == Result_NoEntry){
		cout << "getAPZSystem : NoEntry" << endl;
	}
	else{
		cout << "getAPZSystem Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}


TEST(ACS_CS_API, getAPZType)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 1001;
	CS_API_Result res;

	ACS_CS_API_NS::CS_API_APZ_Type type;

	res = apiCp->getAPZType(id, type);

	switch(type){
	case APZ21401:
		cout << "getAPZType = APZ21401" << endl;
		break;
	case APZ21255:
		cout << "getAPZType = APZ21255" << endl;
		break;
	default:
		cout << "getAPZType = UNDEFINED (" << type << ")" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, getCPList)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CS_API_Result res;

	ACS_CS_API_IdList cpIds;

	res = apiCp->getCPList(cpIds);

	if(res == Result_Success){

		if(cpIds.size() > 0){

			for (unsigned int i = 0; i < cpIds.size(); i++)
			{
				cout << "getCPList (" << i << ") = " << cpIds[i] << endl;
			}
		}
		else {
			cout << "getCPList Successful but EMPTY" << endl;
		}

	}
	else{
		cout << "getCPList Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, getCPType)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 1001;
	CS_API_Result res;
	uint16_t cpType = 0;

	res = apiCp->getCPType(id, cpType);

	if(res == Result_Success){
		cout << "getCPType = " << cpType << endl;
	}
	else if (res == Result_NoEntry){
		cout << "getCPType : NoEntry" << endl;
	}
	else{
		cout << "getCPType Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}


TEST(ACS_CS_API, getCPAliasName)
{

	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
	CPID id = 1001;
	CS_API_Result res;
	bool isAlias = false;
	ACS_CS_API_Name alias;

	res = apiCp->getCPAliasName (id, alias, isAlias);

	if(res == Result_Success){

		char cpName[256];
		size_t size = sizeof(alias);
		alias.getName(cpName, size);

		cout << "getCPAliasName = " << cpName << " isAlias = "  << (isAlias?"YES":"NO") << endl;
	}
	else if (res == Result_NoEntry){
		cout << "getCPAliasName : NoEntry" << endl;
	}
	else if (res == Result_NoValue){
			cout << "getCPAliasName : NoValue" << endl;
		}
	else{
		cout << "getCPAliasName Failed" << endl;
	}

	delete apiCp;

	EXPECT_TRUE(res == Result_Success);
}
//
//TEST(ACS_CS_API, getState)
//{
//
//	ACS_CS_API_CP *apiCp = ACS_CS_API::createCPInstance();
//	CPID id = 1001;
//	CS_API_Result res;
//	int state = 0;
//
//	res = apiCp->getState(id, state);
//
//	if(res == Result_Success){
//
//		cout << "getState = " << state << endl;
//	}
//	else if (res == Result_NoEntry){
//		cout << "getCPAliasName : NoEntry" << endl;
//	}
//	else if (res == Result_NoValue){
//			cout << "getState : NoValue" << endl;
//		}
//	else{
//		cout << "getState Failed (wrong range set? Should be between 21200 and 21299)" << endl;
//	}
//
//	delete apiCp;
//
//	EXPECT_TRUE(res == Result_Success);
//}
