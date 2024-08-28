#ifndef _CUTE_ACS_CHB_MTZCONV_H_
#define _CUTE_ACS_CHB_MTZCONV_H_
#include "cute_suite.h"
#include "ACS_CHB_mtzconv.h"

class CUTE_ACS_CHB_mtzconv{

public:

	static void init();

	static void destroy();

	static void testGetTMZTime();

	static void testUTCtoLocalTime();

	static void testLocalTimeToUTC();

	static void testCalculateTheOffsetTime();

	static void testCPTime();

	static cute::suite make_suite_CUTE_ACS_CHB_mtzconv();

	static ACS_CHB_mtzconv *pMTZConv;

	static ACS_CHB_filehandler *pfilehandler;

};


#endif
