#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

#include <vector>
#include <map>
#include <sstream>
#include <string.h>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "csapi_set.h"
#include "csapi_subscription.h"

#include "ACS_CS_API_CP_R1.h"

#include "ACS_CS_EventReporter.h"

using namespace std;
using namespace ACS_CS_API_SET_NS;

//
// Definitions
//
typedef bool (*__testfunction)();
typedef Result (*__apiFunction)(const std::vector<std::string> &);
#define make_test_entry(x)      make_pair(#x, x)

extern char *optarg;

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------

bool testSetAlarmMaster();
bool testSetClockMaster();
bool testOgDissolve();
bool testSetClusterRecovery();
bool testSetProfiles();
bool testSetQuorumData();
bool testSetClustOpModeChanged();
bool testSubscription();

void printHelp(const vector<pair<string, __testfunction> > &tests);
void setup(map<string, __apiFunction> &apiFunctionsMap);
void split(const std::string &str, std::vector<std::string> &elems);

bool testEvents(const std::string &arg);

CPID cpId = 1001;

int main(int argc, char *argv[])
{
    int c;
    bool all = false;
    bool api = false;
    bool event = false;
    int testCase = -1;
    string arg;
    vector<pair<string, __testfunction> > tests;
    map<string, __apiFunction> apiFunctionsMap;

    // All test cases
    tests.push_back(make_test_entry(testSetAlarmMaster));
    tests.push_back(make_test_entry(testSetClockMaster));
    tests.push_back(make_test_entry(testOgDissolve));
    tests.push_back(make_test_entry(testSetClusterRecovery));
    tests.push_back(make_test_entry(testSetProfiles));
    tests.push_back(make_test_entry(testSetQuorumData));
    tests.push_back(make_test_entry(testSetClustOpModeChanged));
    tests.push_back(make_test_entry(testSubscription));

    setup(apiFunctionsMap);

    while ((c = getopt(argc, argv, "a:c:m:i:e:")) != -1)
    {
        switch (c)
        {
            case 'a':
                all = true;
                break;

            case 'i':
                cpId = atoi(optarg);
                break;

            case 'c':
                testCase = atoi(optarg);
                break;

            case 'm':
            	api = true;
            	arg = optarg;
            	break;

            case 'e':
            	event = true;
            	arg = optarg;
				break;
        }
    }

    if (all == false && testCase == -1 && api == false && event == false)
    {
    	printHelp(tests);

        return 0;
    }

    if(api)
    {
    	vector<string> elems;
    	split(arg, elems);

    	map<string, __apiFunction>::const_iterator it;
		it = apiFunctionsMap.find(elems.at(0));

		if(it != apiFunctionsMap.end())
		{
			__apiFunction f = it->second;
			Result res = f(elems);
			cout << "Result: " << res.result << endl;
			cout << res.value << endl;
		} else {
			printHelp(tests);
		}
    }
    else if(event)
    {
    	if(testEvents(arg) == false)
    	{
    		cout << "Wrong parameters. The event has not been sent." << endl;
    		return 1;
    	}
    }
    else if(all == true)
    {
        for (size_t i = 0;i < tests.size(); i++)
        {
            __testfunction f = tests[i].second;
            cout << "Calling SET API "
                    " " << (i+1) << ", " << tests[i].first << " returned "
                 << (f() == true ? "success" : "failure!") << endl;
        }
    }
    else
    {
    	cout << "testCase=" << testCase << endl;
        __testfunction f = tests[testCase].second;
        cout << "Calling SET API " << (testCase) << ", " << tests[testCase].first << " returned "
             << (f() == true ? "success" : "failure!") << endl;
    }

    return 0;
}

void printHelp(const vector<pair<string, __testfunction> > &tests)
{
	cout << endl;
	cout << "csapi_set: -a [-i] | -c test_case_num [-i cpid] | -m \"ClassName::methodName args\" | -e \"event [severity probableCause data text]\"" << endl;
	cout << " -a = run all tests" << endl;
	cout << " -i = use cp id for tests" << endl;
	cout << " -c = run a specific test case" << endl;
	cout << " -m = run a specific method" << endl << endl;
	cout << " -e = report an event" << endl << endl;

	cout << "where test_case_num can be any of the following: " << endl;

	for (size_t i = 0;i < tests.size(); i++) {
		cout << "  " << i << " - " << tests[i].first << endl;
	}
}

bool testEvents(const std::string &arg)
{
	stringstream ss(arg);
	vector<string> elems;
	string item;
	int eventNumber;

	ss >> eventNumber;

	while(!ss.eof())
	{
		ss >> item;
		elems.push_back(item);
	}

	if(elems.size() == 0)
	{
		ACS_CS_CEASE(eventNumber);
	} else if(elems.size() == 4)
	{
		ACS_CS_EventReporter::Severity severity;

		if(elems[0] == "A1")
		{
			severity = ACS_CS_EventReporter::Severity_A1;
		} else if(elems[0] == "A2")
		{
			severity = ACS_CS_EventReporter::Severity_A2;
		} else if(elems[0] == "A3")
		{
			severity = ACS_CS_EventReporter::Severity_A3;
		} else if(elems[0] == "O1")
		{
			severity = ACS_CS_EventReporter::Severity_O1;
		} else if(elems[0] == "O2")
		{
			severity = ACS_CS_EventReporter::Severity_O2;
		} else if(elems[0] == "EVENT")
		{
			severity = ACS_CS_EventReporter::Severity_Event;
		} else
		{
			return false;
		}

		ACS_CS_EVENT(eventNumber, severity, elems[1], elems[2], elems[3]);
	} else
	{
		return false;
	}

	return true;
}

void setup(map<string, __apiFunction> &apiFunctionsMap)
{
	apiFunctionsMap["ACS_CS_API_CP::getCPId"] = getCPId;
	apiFunctionsMap["ACS_CS_API_CP::getCPName"] = getCPName;
	apiFunctionsMap["ACS_CS_API_CP::getAPZType"] = getAPZType;
	apiFunctionsMap["ACS_CS_API_CP::getCPList"] = getCPList;
	apiFunctionsMap["ACS_CS_API_CP::getAPZSystem"] = getAPZSystem;
	apiFunctionsMap["ACS_CS_API_CP::getCPType"] = getCPType;
	apiFunctionsMap["ACS_CS_API_CP::getCPAliasName"] = getCPAliasName;
	apiFunctionsMap["ACS_CS_API_CP::getState"] = getState;
	apiFunctionsMap["ACS_CS_API_CP::getApplicationId"] = getApplicationId;
	apiFunctionsMap["ACS_CS_API_CP::getApzSubstate"] = getApzSubstate;
	apiFunctionsMap["ACS_CS_API_CP::getStateTransition"] = getStateTransition;
	apiFunctionsMap["ACS_CS_API_CP::getAptSubstate"] = getAptSubstate;
	apiFunctionsMap["ACS_CS_API_CP::getCPQuorumData"] = getCPQuorumData;
	apiFunctionsMap["ACS_CS_API_CP::getBlockingInfo"] = getBlockingInfo;
	apiFunctionsMap["ACS_CS_API_CP::getCpCapacity"] = getCpCapacity;

	apiFunctionsMap["ACS_CS_API_FunctionDistribution::getFunctionNames"] = getFunctionNames;
	apiFunctionsMap["ACS_CS_API_FunctionDistribution::getFunctionProviders"] = getFunctionProviders;
	apiFunctionsMap["ACS_CS_API_FunctionDistribution::getFunctionUsers"] = getFunctionUsers;

	apiFunctionsMap["ACS_CS_API_HWC::getMagazine"] = getMagazine;
	apiFunctionsMap["ACS_CS_API_HWC::getSlot"] = getSlot;
	apiFunctionsMap["ACS_CS_API_HWC::getSysType"] = getSysType;
	apiFunctionsMap["ACS_CS_API_HWC::getSysNo"] = getSysNo;
	apiFunctionsMap["ACS_CS_API_HWC::getFBN"] = getFBN;
	apiFunctionsMap["ACS_CS_API_HWC::getSide"] = getSide;
	apiFunctionsMap["ACS_CS_API_HWC::getSeqNo"] = getSeqNo;
	apiFunctionsMap["ACS_CS_API_HWC::getIPEthA"] = getIPEthA;
	apiFunctionsMap["ACS_CS_API_HWC::getIPEthB"] = getIPEthB;
	apiFunctionsMap["ACS_CS_API_HWC::getAliasEthA"] = getAliasEthA;
	apiFunctionsMap["ACS_CS_API_HWC::getAliasEthB"] = getAliasEthB;
	apiFunctionsMap["ACS_CS_API_HWC::getAliasNetmaskEthA"] = getAliasNetmaskEthA;
	apiFunctionsMap["ACS_CS_API_HWC::getAliasNetmaskEthB"] = getAliasNetmaskEthB;
	apiFunctionsMap["ACS_CS_API_HWC::getDhcpMethod"] = getDhcpMethod;
	apiFunctionsMap["ACS_CS_API_HWC::getSysId"] = getSysId;
	apiFunctionsMap["ACS_CS_API_HWC::getBoardIds"] = getBoardIds;

	apiFunctionsMap["ACS_CS_API_NetworkElement::isMultipleCPSystem"] = isMultipleCPSystem;
	apiFunctionsMap["ACS_CS_API_NetworkElement::isTestEnvironment"] = isTestEnvironment;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getFrontAPG"] = getFrontAPG;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getNEID"] = getNEID;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getSingleSidedCPCount"] = getSingleSidedCPCount;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getDoubleSidedCPCount"] = getDoubleSidedCPCount;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getAPGCount"] = getAPGCount;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getAlarmMaster"] = getAlarmMaster;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getClockMaster"] = getClockMaster;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getBSOMIPAddress"] = getBSOMIPAddress;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getDefaultCPName"] = getDefaultCPName;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getOmProfile"] = getOmProfile;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getClusterOpMode"] = getClusterOpMode;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getTrafficIsolated"] = getTrafficIsolated;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getTrafficLeader"] = getTrafficLeader;
	apiFunctionsMap["ACS_CS_API_NetworkElement::getAPTType"] = getAPTType;
}

void split(const std::string &str, std::vector<std::string> &elems)
{
	stringstream ss(str);
	string item;

	while(ss >> item)
	{
		elems.push_back(item);
	}
}

Result getCPId(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id;
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_Name_R1 name(arg.at(1).c_str());

	res = apiCp->getCPId(name, id);

	Result r;
	stringstream ss;
	r.result = res;
	ss << id;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCPName(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_Name_R1 name;

	res = apiCp->getCPName(id, name);

	Result r;
	char str[128];
	unsigned long int len = 128;

	name.getName(str, len);
	r.result = res;
	r.value = str;

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getAPZType(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_NS::CS_API_APZ_Type type;

	res = apiCp->getAPZType(id, type);

	Result r;
	stringstream ss;
	r.result = res;
	ss << type;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCPList(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_IdList_R1 list;

	res = apiCp->getCPList(list);

	Result r;
	stringstream ss;
	r.result = res;
	for(uint32_t i = 0; i < list.size(); i++)
	{
		ss << list[i] << endl;
	}

	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getAPZSystem(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t sys;

	res = apiCp->getAPZSystem(id, sys);

	Result r;
	stringstream ss;
	r.result = res;
	ss << sys;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCPType(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t type;

	res = apiCp->getAPZSystem(id, type);

	Result r;
	stringstream ss;
	r.result = res;
	ss << type;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCPAliasName(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_Name_R1 alias;
	bool isAlias = false;

	res = apiCp->getCPAliasName(id, alias, isAlias);

	Result r;
	stringstream ss;
	r.result = res;
	if(isAlias)
	{
		char str[128];
		unsigned long int len = 128;

		alias.getName(str, len);
		ss << str;
	}
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getState(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	int state;

	res = apiCp->getState(id, state);

	Result r;
	stringstream ss;
	r.result = res;
	ss << state;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getApplicationId(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	int appId;

	res = apiCp->getState(id, appId);

	Result r;
	stringstream ss;
	r.result = res;
	ss << appId;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getApzSubstate(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	int substate;

	res = apiCp->getApzSubstate(id, substate);

	Result r;
	stringstream ss;
	r.result = res;
	ss << substate;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getStateTransition(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	int state;

	res = apiCp->getStateTransition(id, state);

	Result r;
	stringstream ss;
	r.result = res;
	ss << state;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getAptSubstate(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	int substate;

	res = apiCp->getAptSubstate(id, substate);

	Result r;
	stringstream ss;
	r.result = res;
	ss << substate;
	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCPQuorumData(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_CpData_R1 data;

	res = apiCp->getCPQuorumData(id, data);

	Result r;
	stringstream ss;
	r.result = res;

	ss << "applicationId: " << data.applicationId << endl;
	ss << "aptSubstate: " << data.aptSubstate << endl;
	ss << "apzSubstate: " << data.apzSubstate << endl;
	ss << "blockingInfo: " << data.blockingInfo << endl;
	ss << "cpCapacity: " << data.cpCapacity << endl;
	ss << "id: " << data.id << endl;
	ss << "state: " << data.state << endl;
	ss << "stateTransition: " << data.stateTransition << endl;

	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getBlockingInfo(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t blockingInfo;

	res = apiCp->getBlockingInfo(id, blockingInfo);

	Result r;
	stringstream ss;
	r.result = res;

	ss << blockingInfo;

	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getCpCapacity(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_CP *apiCp = ACS_CS_API_R1::createCPInstance();
	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t capacity;

	res = apiCp->getCpCapacity(id, capacity);

	Result r;
	stringstream ss;
	r.result = res;

	ss << capacity;

	r.value = ss.str();

	ACS_CS_API_R1::deleteCPInstance(apiCp);

	return r;
}

Result getFunctionNames(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_FunctionDistribution *apiFd = ACS_CS_API_R1::createFunctionDistributionInstance();

	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_NameList_R1 list;

	res = apiFd->getFunctionNames(list);

	Result r;
	stringstream ss;
	r.result = res;

	for(uint32_t i = 0; i < list.size(); i++)
	{
		char str[128];
		unsigned long int len = 128;

		list[i].getName(str, len);
		ss << str << endl;
	}

	r.value = ss.str();

	ACS_CS_API_R1::deleteFunctionDistributionInstance(apiFd);

	return r;
}

Result getFunctionProviders(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_FunctionDistribution *apiFd = ACS_CS_API_R1::createFunctionDistributionInstance();

	ACS_CS_API_Name name = arg.at(1).c_str();
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_IdList_R1 list;

	res = apiFd->getFunctionProviders(name, list);

	Result r;
	stringstream ss;
	r.result = res;

	for(uint32_t i = 0; i < list.size(); i++)
	{
		ss << list[i] << endl;
	}

	r.value = ss.str();

	ACS_CS_API_R1::deleteFunctionDistributionInstance(apiFd);

	return r;
}

Result getFunctionUsers(const std::vector<std::string> &arg)
{
	if(arg.size() < 3)
		return Result();

	ACS_CS_API_FunctionDistribution *apiFd = ACS_CS_API_R1::createFunctionDistributionInstance();

	APID id = atoi(arg.at(1).c_str());
	ACS_CS_API_Name name = arg.at(2).c_str();
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_IdList_R1 cpList;

	res = apiFd->getFunctionUsers(id, name, cpList);

	Result r;
	stringstream ss;
	r.result = res;

	for(uint32_t i = 0; i < cpList.size(); i++)
	{
		ss << cpList[i] << endl;
	}

	r.value = ss.str();

	ACS_CS_API_R1::deleteFunctionDistributionInstance(apiFd);

	return r;
}

Result getMagazine(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t mag = 0;

	res = apiHwc->getMagazine(mag, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(ntohl(mag)).c_str();

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSlot(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t slot = 0;

	res = apiHwc->getSlot(slot, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << slot;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSysType(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t sysType = 0;

	res = apiHwc->getSysType(sysType, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << sysType;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSysNo(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t sysNo = 0;

	res = apiHwc->getSysNo(sysNo, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << sysNo;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getFBN(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t fbn = 0;

	res = apiHwc->getFBN(fbn, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getFbnValue(fbn);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSide(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t side = 0;

	res = apiHwc->getSide(side, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << side;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSeqNo(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t seqNo = 0;

	res = apiHwc->getSeqNo(seqNo, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << seqNo;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getIPEthA(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t ip = 0;

	res = apiHwc->getIPEthA(ip, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(ip);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getIPEthB(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t ip = 0;

	res = apiHwc->getIPEthB(ip, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(ip);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getAliasEthA(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t ip = 0;

	res = apiHwc->getAliasEthA(ip, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(ip);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getAliasEthB(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t ip = 0;

	res = apiHwc->getAliasEthB(ip, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(ip);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getAliasNetmaskEthA(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t mask = 0;

	res = apiHwc->getAliasNetmaskEthA(mask, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(mask);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getAliasNetmaskEthB(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint32_t mask = 0;

	res = apiHwc->getAliasNetmaskEthB(mask, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getIpAddressValue(mask);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getDhcpMethod(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t dhcp = 0;

	res = apiHwc->getDhcpMethod(dhcp, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << getDhcpValue(dhcp);

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getSysId(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	BoardID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	uint16_t sysId = 0;

	res = apiHwc->getSysId(sysId, id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << sysId;

	r.value = ss.str();

	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result getBoardIds(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_HWC_R1 *apiHwc = ACS_CS_API_R1::createHWCInstance();

	ACS_CS_API_BoardSearch *bs = ACS_CS_API_HWC_R1::createBoardSearchInstance();
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_IdList_R1 list;

	for(size_t i = 1; i < arg.size(); i++)
	{
		string val;
		const char *opt = arg.at(i).c_str();

		if(++i < arg.size())
		{
			val = arg.at(i);
		} else {
			break;
		}

		if(strcmp(opt, "setMagazine") == 0) {
			uint32_t mag = getIpAddressValue(val);
			bs->setMagazine(htonl(mag));
		} else if(strcmp(opt, "setSlot") == 0) {
			uint16_t slot = atoi(val.c_str());
			bs->setSlot(slot);
		} else if(strcmp(opt, "setSysType") == 0) {
			uint16_t sysType = atoi(val.c_str());
			bs->setSysType(sysType);
		} else if(strcmp(opt, "setSysNo") == 0) {
			uint16_t sysNo = atoi(val.c_str());
			bs->setSysNo(sysNo);
		} else if(strcmp(opt, "setFBN") == 0) {
			uint16_t fbn = 0;
			if(val.compare("SCB_RP") == 0) {
				fbn = 100;
			} else if(val.compare("RPBI_S") == 0) {
				fbn = 110;
			} else if(val.compare("GESB") == 0) {
				fbn = 120;
			} else if(val.compare("CPUB") == 0) {
				fbn = 200;
			} else if(val.compare("MAUB") == 0) {
				fbn = 210;
			} else if(val.compare("APUB") == 0) {
				fbn = 300;
			} else if(val.compare("DISK") == 0) {
				fbn = 310;
			} else if(val.compare("DVD") == 0) {
				fbn = 320;
			} else if(val.compare("GEA") == 0) {
				fbn = 330;
			}
			bs->setFBN(fbn);
		} else if(strcmp(opt, "setSide") == 0) {
			uint16_t side = atoi(val.c_str());
			bs->setSide(side);
		} else if(strcmp(opt, "setSeqNo") == 0) {
			uint16_t seqNo = atoi(val.c_str());
			bs->setSeqNo(seqNo);
		} else if(strcmp(opt, "setIPEthA") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setIPEthA(htonl(ip));
		} else if(strcmp(opt, "setIPEthB") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setIPEthB(htonl(ip));
		} else if(strcmp(opt, "setAliasEthA") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setAliasEthA(htonl(ip));
		} else if(strcmp(opt, "setAliasEthB") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setAliasEthB(htonl(ip));
		} else if(strcmp(opt, "setAliasNetmaskEthA") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setAliasNetmaskEthA(htonl(ip));
		} else if(strcmp(opt, "setAliasNetmaskEthB") == 0) {
			uint32_t ip = getIpAddressValue(val);
			bs->setAliasNetmaskEthB(htonl(ip));
		} else if(strcmp(opt, "setDhcpMethod") == 0) {
			uint16_t dhcp = 0;
			if(val.compare("NONE") == 0) {
				dhcp = 0;
			} else if(val.compare("NORMAL") == 0) {
				dhcp = 1;
			} else if(val.compare("CLIENT") == 0) {
				dhcp = 2;
			}
			bs->setDhcpMethod(dhcp);
		} else if(strcmp(opt, "setSysId") == 0) {
			uint16_t sysId = atoi(val.c_str());
			bs->setSysId(sysId);
		}
	}

	res = apiHwc->getBoardIds(list, bs);

	Result r;
	stringstream ss;
	r.result = res;

	for(size_t i = 0; i < list.size(); i++)
	{
		ss << list[i] << endl;
	}

	r.value = ss.str();

	ACS_CS_API_HWC_R1::deleteBoardSearchInstance(bs);
	ACS_CS_API_R1::deleteHWCInstance(apiHwc);

	return r;
}

Result isMultipleCPSystem(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	bool multipleCpSys = false;

	res = ACS_CS_API_NetworkElement_R1::isMultipleCPSystem(multipleCpSys);

	Result r;
	stringstream ss;
	r.result = res;

	ss << multipleCpSys;

	r.value = ss.str();

	return r;
}

Result isTestEnvironment(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	bool testEnv = false;

	res = ACS_CS_API_NetworkElement_R1::isTestEnvironment(testEnv);

	Result r;
	stringstream ss;
	r.result = res;

	ss << testEnv;

	r.value = ss.str();

	return r;
}

Result getFrontAPG(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	APID id = 0;

	res = ACS_CS_API_NetworkElement_R1::getFrontAPG(id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << id;

	r.value = ss.str();

	return r;
}

Result getNEID(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_Name_R1 name = 0;

	res = ACS_CS_API_NetworkElement_R1::getNEID(name);

	Result r;
	stringstream ss;
	r.result = res;

	char str[128];
	unsigned long int len = 128;
	name.getName(str, len);

	ss << str;

	r.value = ss.str();

	return r;
}

Result getSingleSidedCPCount(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	uint32_t count = 0;

	res = ACS_CS_API_NetworkElement_R1::getSingleSidedCPCount(count);

	Result r;
	stringstream ss;
	r.result = res;

	ss << count;

	r.value = ss.str();

	return r;
}

Result getDoubleSidedCPCount(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	uint32_t count = 0;

	res = ACS_CS_API_NetworkElement_R1::getDoubleSidedCPCount(count);

	Result r;
	stringstream ss;
	r.result = res;

	ss << count;

	r.value = ss.str();

	return r;
}

Result getAPGCount(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	uint32_t count = 0;

	res = ACS_CS_API_NetworkElement_R1::getAPGCount(count);

	Result r;
	stringstream ss;
	r.result = res;

	ss << count;

	r.value = ss.str();

	return r;
}

Result getAlarmMaster(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	CPID id = 0;

	res = ACS_CS_API_NetworkElement_R1::getAlarmMaster(id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << id;

	r.value = ss.str();

	return r;
}

Result getClockMaster(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	CPID id = 0;

	res = ACS_CS_API_NetworkElement_R1::getClockMaster(id);

	Result r;
	stringstream ss;
	r.result = res;

	ss << id;

	r.value = ss.str();

	return r;
}

Result getBSOMIPAddress(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	uint32_t addressA = 0;
	uint32_t addressB = 0;

	res = ACS_CS_API_NetworkElement_R1::getBSOMIPAddress(addressA, addressB);

	Result r;
	stringstream ss;
	r.result = res;

	ss << addressA << endl;
	ss << addressB << endl;

	r.value = ss.str();

	return r;
}

Result getDefaultCPName(const std::vector<std::string> &arg)
{
	if(arg.size() < 2)
		return Result();

	CPID id = atoi(arg.at(1).c_str());
	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_Name_R1 name;

	res = ACS_CS_API_NetworkElement_R1::getDefaultCPName(id, name);

	Result r;
	char str[128];
	unsigned long int len = 128;

	name.getName(str, len);
	r.result = res;
	r.value = str;

	return r;
}

Result getOmProfile(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_OmProfileChange_R1 profile;

	res = ACS_CS_API_NetworkElement_R1::getOmProfile(profile);

	stringstream ss;

	ss << "aptCurrent: " << profile.aptCurrent << endl;
	ss << "aptQueued: " << profile.aptQueued << endl;
	ss << "aptRequested: " << profile.aptRequested << endl;
	ss << "apzCurrent: " << profile.apzCurrent << endl;
	ss << "apzQueued: " << profile.apzQueued << endl;
	ss << "apzRequested: " << profile.apzRequested << endl;
	ss << "apzRequested: " << profile.changeReason << endl;
	ss << "omProfileCurrent: " << profile.omProfileCurrent << endl;
	ss << "omProfileRequested: " << profile.omProfileRequested << endl;
	ss << "phase: " << profile.phase << endl;

	Result r;
	r.result = res;
	r.value = ss.str();

	return r;
}

Result getClusterOpMode(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	ACS_CS_API_ClusterOpMode::Value mode;

	res = ACS_CS_API_NetworkElement_R1::getClusterOpMode(mode);

	stringstream ss;

	ss << mode;

	Result r;
	r.result = res;
	r.value = ss.str();

	return r;
}

Result getTrafficIsolated(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	CPID id;

	res = ACS_CS_API_NetworkElement_R1::getTrafficIsolated(id);

	stringstream ss;

	ss << id;

	Result r;
	r.result = res;
	r.value = ss.str();

	return r;
}

Result getTrafficLeader(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	CPID id;

	res = ACS_CS_API_NetworkElement_R1::getTrafficLeader(id);

	stringstream ss;

	ss << id;

	Result r;
	r.result = res;
	r.value = ss.str();

	return r;
}

Result getAPTType(const std::vector<std::string> &arg)
{
	if(arg.size() < 1)
		return Result();

	ACS_CS_API_NS::CS_API_Result res;
	string type;

	res = ACS_CS_API_NetworkElement_R1::getAPTType(type);

	stringstream ss;

	ss << type;

	Result r;
	r.result = res;
	r.value = ss.str();

	return r;
}




bool testSetAlarmMaster()
{
    const CPID setAlarmMasterId = cpId;

    if (ACS_CS_API_Set::setAlarmMaster(setAlarmMasterId) != Result_Success) {
        cout << "setAlarmMaster(" << setAlarmMasterId << ") failed" << endl;
        return false;
    }

    CPID getAlarmMasterId = 0;    cout << "Subscribing for NE notifications" << endl;

    ACS_CS_API_NetworkElement::getAlarmMaster(getAlarmMasterId);
    assert(getAlarmMasterId == setAlarmMasterId);

    if (ACS_CS_API_Set::setAlarmMaster(1010) != Result_Incorrect_CP_Id) {
        cout << "setAlarmMaster(" << setAlarmMasterId << ") failed" << endl;
        return false;
    }

    return true;
}

bool testSetClockMaster()
{
    const CPID setClockMasterId = cpId;

    if (ACS_CS_API_Set::setClockMaster(setClockMasterId) != Result_Success) {
        cout << "setClockMaster(" << setClockMasterId << ") failed" << endl;
        return false;
    }

    CPID getClockMasterId = 0;
    ACS_CS_API_NetworkElement::getClockMaster(getClockMasterId);
    assert(getClockMasterId == setClockMasterId);

    if (ACS_CS_API_Set::setClockMaster(1010) != Result_Incorrect_CP_Id) {
        cout << "setClockMaster(" << setClockMasterId << ") failed" << endl;
        return false;
    }

    return true;
}

bool testOgDissolve()
{
    if (ACS_CS_API_Set::setOpGroupDissolved() != Result_Success) {
        cout << "setOpGroupDissolved() failed" << endl;
        return false;
    }

    return true;
}

bool testSetClusterRecovery()
{
    if (ACS_CS_API_Set::setClusterRecovery() != Result_Success) {
        cout << "setClusterRecovery() failed" << endl;
        return false;
    }

    return true;
}

bool testSetProfiles()
{
    if (ACS_CS_API_Set::setProfiles(0, -1, -1) != Result_Success) {
        cout << "setProfiles() failed" << endl;
        return false;
    }

    return true;
}

bool testSetQuorumData()
{
    ACS_CS_API_QuorumData data;

    data.trafficIsolatedCpId = 1001;
    data.trafficLeaderCpId = 1002;
    data.automaticQuorumRecovery = false;
    data.apzProfile = 1;
    data.aptProfile = 2;

    ACS_CS_API_CpData cpData;

    cpData.id = 1;
    cpData.state = 3;
    cpData.applicationId = 2;
    cpData.apzSubstate = 3;
    cpData.stateTransition = 4;
    cpData.aptSubstate = 5;
    cpData.blockingInfo = 6;
    cpData.cpCapacity = 7;

    data.cpData.push_back(cpData);

    if (ACS_CS_API_Set::setQuorumData(data) != Result_Success) {
        cout << "setQuorumData() failed" << endl;
        return false;
    }

    return true;
}

bool testSetClustOpModeChanged()
{
//    if (ACS_CS_API_Set::setClusterOpModeChanged(ACS_CS_API_RequestedClusterOpMode::Normal)) {
//        cout << "setClusterOpModeChanged() failed" << endl;
//        return false;
//    }

    if (ACS_CS_API_Set::setClusterOpModeChanged(ACS_CS_API_RequestedClusterOpMode::Expert)) {
        cout << "setClusterOpModeChanged() failed" << endl;
        return false;
    }

    return true;
}

bool testSubscription()
{
    cs_subscription_init();

    cout << "Press any key to exit..." << endl << endl;
    getchar();

    cs_subscription_cleanup();

    return true;
}
