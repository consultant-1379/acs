#ifndef _CUTE_ACS_CHB_MTZFILEHANDLER_H_
#define _CUTE_ACS_CHB_MTZFILEHANDLER_H_
#include "cute_suite.h"
#include "ACS_CHB_filehandler.h"

class CUTE_ACS_CHB_filehandler{

public:

	static void init();

	static void destroy();

	static void testValidateTimeZone();

	static void testWriteZoneFile();

	static void testWriteZoneFileCPtime();

	static void testReadZoneFile();

	static void testReadZoneFileCPtime();

	static void testGetNTZone();

	static void testCheckTimeZone();

	static void testValidateTMZNumber();

	static cute::suite make_suite_CUTE_ACS_CHB_filehandler();

	static ACS_CHB_filehandler *pMTZfilehandler;

	static time_t CPtimeSec;

	static time_t written;

};


#endif
