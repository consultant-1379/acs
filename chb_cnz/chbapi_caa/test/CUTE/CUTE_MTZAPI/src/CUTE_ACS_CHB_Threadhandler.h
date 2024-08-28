#ifndef _CUTE_ACS_CHB_MTZTHREADHANDLER_H_
#define _CUTE_ACS_CHB_MTZTHREADHANDLER_H_
#include "cute_suite.h"
#include "ACS_CHB_Threadhandler.h"

class CUTE_ACS_CHB_Threadhandler{

public:

	static void init();

	static void destroy();

	static ACS_CHB_Threadhandler *pMTZThreadhandler;

	static ACS_CHB_filehandler *pMTZfilehandler;
};

#endif
