#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include "ACS_CS_API.h"

using namespace std;
using namespace ACS_CS_API_NS;


TEST(ACS_CS_API, getGroupNames)
{
	CS_API_Result res;
	ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();


	ACS_CS_API_NameList groupNameList;

	res = cpGroupTable->getGroupNames(groupNameList);

	if(res == Result_Success){
		if(groupNameList.size() > 0){

			for (unsigned int i = 0; i < groupNameList.size(); i++)
			{
				ACS_CS_API_Name name = groupNameList[i];
				size_t nameLength = name.length();
				char* groupNameChar = new char[nameLength];
				name.getName(groupNameChar, nameLength);
				cout << "groupNameList (" << i << ") = " << groupNameChar << endl;
				delete groupNameChar;
			}
		}
		else {
			cout << "groupNameList Successful but EMPTY" << endl;
		}
	}
	else if (res == Result_NoValue){
		cout << "groupNameList : NoValue" << endl;
	}
	else{
		cout << "groupNameList Failed" << endl;
	}

	delete cpGroupTable;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, getGroupMembersAllBc)
{
	CS_API_Result res;
	ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();


	const char _tmpname[6] = {'A','L','L','B','C',0x0};
	ACS_CS_API_Name cpGroupName(_tmpname);
	ACS_CS_API_IdList cpList;

	res = cpGroupTable->getGroupMembers(cpGroupName, cpList);

	if(res == Result_Success){
		if(cpList.size() > 0){

			for (unsigned int i = 0; i < cpList.size(); i++)
			{
				CPID cpId = cpList[i];

				cout << "getGroupMembers ALLBC (" << i << ") = " << cpId << endl;
			}
		}
		else {
			cout << "getGroupMembers ALLBC Successful but EMPTY" << endl;
		}
	}
	else if (res == Result_NoValue){
		cout << "getGroupMembers ALLBC: NoValue" << endl;
	}
	else{
		cout << "getGroupMembers ALLBC Failed" << endl;
	}

	delete cpGroupTable;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, getGroupMembersOpGroup)
{
	CS_API_Result res;
	ACS_CS_API_CPGroup* cpGroupTable = ACS_CS_API::createCPGroupInstance();


	const char _tmpname[8] = {'O','P','G','R','O','U','P',0x0};
	ACS_CS_API_Name cpGroupName(_tmpname);
	ACS_CS_API_IdList cpList;

	res = cpGroupTable->getGroupMembers(cpGroupName, cpList);

	if(res == Result_Success){
		if(cpList.size() > 0){

			for (unsigned int i = 0; i < cpList.size(); i++)
			{
				CPID cpId = cpList[i];

				cout << "getGroupMembers OPGROUP (" << i << ") = " << cpId << endl;
			}
		}
		else {
			cout << "getGroupMembers OPGROUP Successful but EMPTY" << endl;
		}
	}
	else if (res == Result_NoValue){
		cout << "getGroupMembers OPGROUP: NoValue" << endl;
	}
	else{
		cout << "getGroupMembers OPGROUP Failed" << endl;
	}

	delete cpGroupTable;

	EXPECT_TRUE(res == Result_Success);
}

