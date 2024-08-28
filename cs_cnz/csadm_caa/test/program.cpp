
#include <stdio.h>
#include <iostream>
#include "ACS_CS_Api.h"

using namespace std;

int getCPNameTest(int argc, char* argv[]);
int getCPAliasNameTest(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	static char PROMPT[] = "test>";
	
	bool first = true;
	int result = 0;
	unsigned choice = 0;


	cout << "what do you want to test?" << endl;

	do {
		if (first) {
			cout << PROMPT << "0. CP list" << endl;
			cout << PROMPT << "1. CP name" << endl;
			cout << PROMPT << "2. CP alias" << endl;
			cout << PROMPT << "3. QUIT" << endl;
			first = false;
		}

		cout << PROMPT;
		cin >> choice;

		switch(choice) {
			case 0: system("cpls"); cout << endl; break;
			case 1: result = getCPNameTest(argc,argv); if (result!= 0) cerr << PROMPT << "error " << result << endl; break;
			case 2: result = getCPAliasNameTest(argc,argv); if (result!= 0) cerr << PROMPT << "error " << result << endl; break;
			case 3: cout << "good bye..." << endl; break;
			default: cout << PROMPT << "bad choice" << endl; first = true;
		}
	} while (choice != 3);
	
	return 0;
}

int getCPNameTest(int argc, char* argv[]) {
	static char PROMPT[] = "test>";
	ACS_CS_API_CP * CPTable = 0;
	CPTable = ACS_CS_API::createCPInstance();
	if (!CPTable) {
		cerr << "Cannot create CPTable" <<  endl;
		return -1;
	}
	
	short s_id = 0;
	cout << PROMPT << "insert cpId:";
	cin >> s_id;

	CPID cpId(s_id);
	ACS_CS_API_Name name;
	char cpName[512] = {0};
	unsigned nameLen = sizeof(cpName);
	ACS_CS_API_NS::CS_API_Result result = CPTable->getCPName(cpId, name);
	switch(result) {
		case ACS_CS_API_NS::Result_Success:
			name.getName(cpName, nameLen);
			cout << PROMPT << "CP NAME:" << cpName << endl;
			break;
		case ACS_CS_API_NS::Result_Failure: cerr << "No Failure" << endl; break;
		case ACS_CS_API_NS::Result_NoAccess: cerr << "No Access" << endl; break;
		case ACS_CS_API_NS::Result_NoEntry: cerr << "No Entry" << endl; break;
		case ACS_CS_API_NS::Result_NoValue: cerr << "No Value" << endl; break;
		default: cerr << "unknown error" << endl;
	}

	ACS_CS_API::deleteCPInstance(CPTable);

	return result;
}

int getCPAliasNameTest(int argc, char* argv[]) {
	static char PROMPT[] = "test>";
	ACS_CS_API_CP * CPTable = 0;
	CPTable = ACS_CS_API::createCPInstance();
	if (!CPTable) {
		cerr << "Cannot create CPTable" <<  endl;
		return -1;
	}
	
	short s_id = 0;
	cout << PROMPT << "insert cpId:";
	cin >> s_id;

	CPID cpId(s_id);
	ACS_CS_API_Name name;
	bool isAlias = false;
	char cpName[512] = {0};
	unsigned nameLen = sizeof(cpName);
	ACS_CS_API_NS::CS_API_Result result = CPTable->getCPAliasName(cpId, name, isAlias);
	switch(result) {
		case ACS_CS_API_NS::Result_Success:
			name.getName(cpName, nameLen);
			if (isAlias) cout << PROMPT << "ALIAS NAME:" << cpName << endl;
			else cout << PROMPT << "CP NAME:" << cpName << endl;
			break;
		case ACS_CS_API_NS::Result_Failure: cerr << "No Failure" << endl; break;
		case ACS_CS_API_NS::Result_NoAccess: cerr << "No Access" << endl; break;
		case ACS_CS_API_NS::Result_NoEntry: cerr << "No Entry" << endl; break;
		case ACS_CS_API_NS::Result_NoValue: cerr << "No Value" << endl; break;
		default: cerr << "unknown error" << endl;
	}

	ACS_CS_API::deleteCPInstance(CPTable);

	return result;
}

