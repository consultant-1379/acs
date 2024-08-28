#ifndef HEADER_GUARD_CLASS__CuteTester
#define HEADER_GUARD_CLASS__CuteTester CuteTester

#include <cstdio>
#include <cstring>
#include <alloca.h>

//#include "cute.h"

#define CHECK_TEST_STR(expr) ((expr) ? "OK" : "KO")

#ifndef TEST_PRINT_ERROR
#define TEST_PRINT_ERROR(class_obj, method, ...) { \
		printf("\nTESTING [" ACS_DSD_STRINGIZE(class_obj) "." ACS_DSD_STRINGIZE(method) "(...)] --> "); \
		fflush(stdout); \
		if ((call_result = class_obj.method(__VA_ARGS__))) \
			printf("%s\n  %s at line %d\n" \
				"    last_error == %d\n    last_error_text == '%s'\n", (call_result < 0 ? "ERR" : "WAR"), \
				(call_result < 0 ? "ERROR" : "WARNING"), __LINE__, class_obj.last_error(), class_obj.last_error_text()); \
		else \
			printf("OK!\n"); \
	}
#endif

#ifndef CHECK_POINT_PAUSE
#define CHECK_POINT_PAUSE printf("\nCHECK-POINT --> Press RETURN when ready..."); getchar()
#endif

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__CuteTester

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__() {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual ~__CLASS_NAME__() {}

	//===========//
	// Operators //
	//===========//
public:
	inline virtual void operator()() {}

	//===========//
	// Functions //
	//===========//
protected:
	inline int print_errno(int line_indent, int last_sys_error, int return_code) {
		while (line_indent-- > 0) std::cout << ' ';
		char buf[1024];
		std::cout << "SYSTEM ERROR: " << strerror_r(last_sys_error, buf, 1024) << std::endl;

		return return_code;
	}
};

#endif // HEADER_GUARD_CLASS__CuteTester
