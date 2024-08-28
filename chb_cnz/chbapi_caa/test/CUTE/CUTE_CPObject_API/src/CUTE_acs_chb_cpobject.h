#ifndef _CUTE_ACS_CHB_MTZCONV_H_
#define _CUTE_ACS_CHB_MTZCONV_H_
#include "cute_suite.h"
#include "acs_chb_cp_object.h"

class CUTE_ACS_CHB_CP_object{

public:

	static void init();

	static void destroy();

	static void testConnect();

	static void testget_Status();

	static void testget_fileDescriptor();

	static void testget_name();

	static void testget_value();

	static void testdisconnect();

	static cute::suite make_suite_CUTE_acs_chb_cp_object();

	static ACS_CHB_CP_object *CP_object;


};


#endif
