#include "cute_suite.h"
#include "acs_lm_common.h"
#include "acs_lm_persistent.h"
#include <ace/ACE.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <ace/ACE.h>
using namespace std;
class ACS_LM_Persistent;

class CUTE_LM_Persistent
{
public:
	static cute::suite make_suite_cute_lm_persistent();
	static bool ExecuteCommand(string cmd);
	static bool setLmModeBasicTest();
	static bool loadPersistentFileBasicTest();
	static bool commitFileBasicTest();
	static bool backupFileBasicTest();
	static bool restoreFileBasicTest();
	static bool deletePersistentFileBasicTest();

	static ACS_LM_Persistent* getLMPersistent(){
			return theLMPersistent;
		}
private:
	static ACS_LM_Persistent* theLMPersistent;

	//static bool fetchParameterIMMTest();

};

