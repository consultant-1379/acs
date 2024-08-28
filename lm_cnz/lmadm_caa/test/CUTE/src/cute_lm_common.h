#include "cute_suite.h"
#include "acs_lm_common.h"
#include <ace/ACE.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class cute_lm_common
{
public:
	static cute::suite make_suite_cute_lm_common();
	static bool ExecuteCommand(string cmd);
	static void createLMDirectoryBasicTest();
	static void getNodeNameBasicTest();
	static void getClusterIPAddressBasicTest();
	static void generateFingerPrintBasicTest();
	static void fetchDnOfRootObjFromIMMBasicTest();
	static void backupFileBasicTest();
	static void deleteFileBasicTest();
	static void isFileExistsBasicTest();
	static void restoreFileBasicTest();
	static void getFullPathBasicTest();
	static void nodeStatusBasicTest();
	static void getFileTypeBasicTest();
	static void getlatestFileBasicTest();
	static void isDirBasicTest();

};

