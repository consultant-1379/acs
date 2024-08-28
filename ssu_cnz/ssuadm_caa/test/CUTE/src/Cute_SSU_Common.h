#include "cute_suite.h"
#include "acs_ssu_common.h"
#include <ace/ACE.h>
#include <string>
#include <iostream>
using namespace std;
class Cute_SSU_Common
{
public:
        Cute_SSU_Common();
	static cute::suite make_suite_Cute_SSU_Common();
        static bool ExecuteCommand(string cmd);
        static void CheckFileExistsBasicTest();

};



