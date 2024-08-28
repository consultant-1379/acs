#include "ACS_CS_API.h"
//#include "ACS_CS_API_Set.h"

//#include "ACS_CS_Protocol.h"
//#include "ACS_CS_API_Internal.h"
#include "ACS_CS_IMM_Table.h"
#include "ACS_CS_Attribute.h"
//#include "ACS_CS_TableEntry.h"

#include <vector>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


using namespace std;
//using namespace ACS_CS_API_SET_NS;
using namespace ACS_CS_INTERNAL_API;

//
// Definitions
//

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------



CPID cpId = 1001;

int main(int argc, char *argv[])
{

	bool result = true;
	ACS_CS_IMM_Table * immTable = ACS_CS_IMM_Table::createTableInstance();

	vector<uint16_t> boards;

	result = immTable->getEntries(Table_CPGroupNames, NULL, boards);

	for (size_t i = 0;i < boards.size(); i++)
	{
		vector<ACS_CS_Attribute> attrs;
		result = immTable->getAttributes(Table_CPGroupNames, boards[i], attrs);
		uint16_t groupId;
		string str_name;
		for (size_t j = 0;j < attrs.size(); j++)
		{
			if (attrs[j].getIdentifier() == ACS_CS_Protocol::Attribute_CPGroup_GroupIdentifier)
			{
				attrs[j].getValue(reinterpret_cast<char*>(&groupId), sizeof(groupId));
			}
			if (attrs[j].getIdentifier() == ACS_CS_Protocol::Attribute_CPGroup_GroupName)
			{
				int sizeOfGroupName = attrs[j].getValueLength();
				char *name = new char[sizeOfGroupName + 1];
				name[sizeOfGroupName] = '\0';
				attrs[j].getValue(name, sizeOfGroupName);
				str_name = name;
				delete [] name;
			}
		}
		cout << groupId << " " << str_name << endl;
	}

    return 0;
}


