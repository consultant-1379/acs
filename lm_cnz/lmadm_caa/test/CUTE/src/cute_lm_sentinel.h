#include "cute_suite.h"
#include "acs_lm_common.h"
#include "acs_lm_sentinel.h"
#include <ace/ACE.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class ACS_LM_Sentinel;

class CUTE_LM_Sentinel
{
public:
	static cute::suite make_suite_cute_lm_sentinel();
	static bool ExecuteCommand(string cmd);
	static bool initBasicTest();
	static bool fetchSidBasicTest();
	static bool generateFingerPrintBasicTest();
	static bool installLkfAndfinishInstallationBasicTest();
	static bool isTestModeAllowedBasicTest();
	static bool isEmergencyKeyValidBasicTest();
	static bool getKeysFromLservrcBasicTest();
	static bool getAllLksBasicTest();

	static ACS_LM_Sentinel* theLMSentinel;
};

