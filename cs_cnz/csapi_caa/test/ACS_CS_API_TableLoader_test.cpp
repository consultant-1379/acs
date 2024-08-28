#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include "ACS_CS_API.h"
#include "ACS_CS_API_TableLoader.h"

using namespace std;
using namespace ACS_CS_API_NS;

class ACS_CS_Table;

TEST(ACS_CS_API, loadHWCTable)
{
	CS_API_Result res;
	ACS_CS_Table table;
	ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();


	res = tableLoader->loadHWCTable(table);

	if (res == Result_Success) {
		cout << "Loaded " << table.getSize() << " entries" << endl;
	}
	else{
		cout << "loadHWCTable Failed" << endl;
	}

	delete tableLoader;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, loadCPTable)
{
	CS_API_Result res;
	ACS_CS_Table table;
	ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();


	res = tableLoader->loadCPTable(table);

	if (res == Result_Success) {
		cout << "Loaded " << table.getSize() << " entries" << endl;

		ACS_CS_Attribute attr = table.getValue(1001, ACS_CS_Protocol::Attribute_CPId_Name);

		int lenght = attr.getValueLength();
		char * buffer = new char[lenght+1];

		attr.getValue(buffer, attr.getValueLength());

		cout << "Attribute_CPId_Name " << buffer << endl;

		delete buffer;
	}
	else{
		cout << "loadCPTable Failed" << endl;
	}

	delete tableLoader;

	EXPECT_TRUE(res == Result_Success);
}

TEST(ACS_CS_API, loadFunctionDistributionTable)
{
	CS_API_Result res;
	ACS_CS_Table table;
	ACS_CS_API_TableLoader *tableLoader = new ACS_CS_API_TableLoader();


	res = tableLoader->loadFunctionDistributionTable(table);

	if (res == Result_Success) {
		cout << "Loaded " << table.getSize() << " entries" << endl;
	}
	else{
		cout << "loadFunctionDistributionTable Failed" << endl;
	}

	delete tableLoader;

	EXPECT_TRUE(res == Result_Success);
}
