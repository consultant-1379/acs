#include "cute_suite.h"
#include "acs_lm_common.h"
#include "acs_lm_persistent.h"
#include <ace/ACE.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class ACS_LM_Persistent;
class ACS_LM_TestLkf;

class cute_lm_testmode
{
public:
	static cute::suite make_suite_cute_lm_testmode();
	static void testmodeBasicTest();
	static bool getReturnStatus(bool retn);
	static bool ExecuteCommand(string cmd);
private:
	ACS_LM_Persistent* persistent;

};

